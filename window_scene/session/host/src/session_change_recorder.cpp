/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "session/host/include/session_change_recorder.h"

#include <algorithm>
#include <chrono>

#include "window_helper.h"
#include "zlib.h"

namespace OHOS::Rosen {
namespace {
#define CHUNK 16384
#define COMPRESS_VERSION 9

constexpr uint32_t MAX_RECORD_TYPE_SIZE = 10;
constexpr uint32_t MAX_RECORD_LOG_SIZE = 102400;
constexpr uint32_t MAX_EVENT_DUMP_SIZE = 512 * 1024;
constexpr int32_t SCHEDULE_SECONDS = 5;

std::string GetFormattedTime()
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    struct tm timeBuffer;
    std::tm* tmPtr = localtime_r(&t, &timeBuffer);
    // 1000：million record
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    const int formatThreeSpace = 3;
    oss << std::put_time(tmPtr, "%m-%d %H:%M:%S") << "." << std::setw(formatThreeSpace) << ms.count();
    return oss.str();
}
} // namespace

WM_IMPLEMENT_SINGLE_INSTANCE(SessionChangeRecorder)

void SessionChangeRecorder::Init()
{
    TLOGD(WmsLogTag::DEFAULT, "In");
    if (isInitFlag_.load()) {
        return;
    }

    isInitFlag_.store(true);
    if (!HiLogIsLoggable(HILOG_DOMAIN_WINDOW, g_domainContents[static_cast<uint32_t>(WmsLogTag::DEFAULT)], LOG_DEBUG)) {
        stopLogFlag_.store(true);
        return;
    }
    
    stopLogFlag_.store(false);
    mThread = std::thread([this]() {
        std::unordered_map<RecordType, std::queue<SceneSessionChangeInfo>> sceneSessionChangeNeedLogMapCopy;
        while (!stopLogFlag_.load()) {
            {
                std::lock_guard<std::mutex> lock(sessionChangeRecorderMutex_);
                if (!sceneSessionChangeNeedLogMap_.empty()) {
                    sceneSessionChangeNeedLogMapCopy = sceneSessionChangeNeedLogMap_;
                    sceneSessionChangeNeedLogMap_.clear();
                    currentLogSize_ = 0;
                }
            }
            if (!sceneSessionChangeNeedLogMapCopy.empty()) {
                PrintLog(sceneSessionChangeNeedLogMapCopy);
                sceneSessionChangeNeedLogMapCopy.clear();
            }
            std::this_thread::sleep_for(std::chrono::seconds(SCHEDULE_SECONDS));
        }
    });
}

SessionChangeRecorder::~SessionChangeRecorder()
{
    TLOGD(WmsLogTag::DEFAULT, "In");
    stopLogFlag_.store(true);
    if (mThread.joinable()) {
        mThread.join();
    }
}

WSError SessionChangeRecorder::RecordSceneSessionChange(RecordType recordType, SceneSessionChangeInfo& changeInfo)
{
    TLOGD(WmsLogTag::DEFAULT, "In");
    if (changeInfo.logTag_ == WmsLogTag::DEFAULT || changeInfo.logTag_ >= WmsLogTag::END ||
        changeInfo.changeInfo_ == "") {
        TLOGD(WmsLogTag::DEFAULT, "Invalid log tag");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    if (changeInfo.time_ == "") {
        changeInfo.time_ = GetFormattedTime();
    }
    RecordLog(recordType, changeInfo);
    RecordDump(recordType, changeInfo);
    return WSError::WS_OK;
}

WSError SessionChangeRecorder::SetRecordSize(RecordType recordType, uint32_t recordSize)
{
    TLOGD(WmsLogTag::DEFAULT, "recordType: %{public}" PRIu32 ", size: %{public}d", recordType, recordSize);
    if (recordSize <= 0 || recordSize > MAX_RECORD_TYPE_SIZE) {
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    {
        std::lock_guard<std::mutex> lock(sessionChangeRecorderMutex_);
        recordSizeMap_[recordType] = recordSize;
    }
    return WSError::WS_OK;
}

void SessionChangeRecorder::GetSceneSessionNeedDumpInfo(
    const std::vector<std::string>& dumpParams, std::string& dumpInfo)
{
    std::ostringstream oss;
    oss << "Record session change: " << std::endl;

    bool simplifyFlag = false;
    std::vector<std::string> params = dumpParams;
    auto it = std::find(params.begin(), params.end(), "-simplify");
    if (it != params.end()) {
        simplifyFlag = true;
        params.erase(it);
    }

    int32_t specifiedWindowId = INVALID_SESSION_ID;
    if (params.size() >= 1 && params[0] == "all") {
        specifiedWindowId = INVALID_SESSION_ID;
    } else if (params.size() >= 1 && WindowHelper::IsNumber(params[0])) {
        const std::string& str = params[0];
        int32_t value;
        auto res = std::from_chars(str.data(), str.data() + str.size(), value);
        if (res.ec == std::errc()) {
            specifiedWindowId = value;
        } else {
            oss << "Available args: '-v all/[specified window id]'" << std::endl;
            dumpInfo.append(oss.str());
            return;
        }
    } else {
        oss << "Available args: '-v all/[specified window id]'" << std::endl;
        dumpInfo.append(oss.str());
        return;
    }

    uint32_t specifiedRecordType = INVALID_SESSION_ID;
    if (params.size() >= 2 && WindowHelper::IsNumber(params[1])) {
        const std::string& str = params[1];
        uint32_t value;
        auto res = std::from_chars(str.data(), str.data() + str.size(), value);
        if (res.ec == std::errc()) {
            specifiedRecordType = value;
        }
    }

    std::unordered_map<RecordType, std::queue<SceneSessionChangeInfo>> sceneSessionChangeNeedDumpMapCopy;
    {
        std::lock_guard<std::mutex> lock(sessionChangeRecorderMutex_);
        sceneSessionChangeNeedDumpMapCopy = sceneSessionChangeNeedDumpMap_;
    }

    std::string dumpInfoJsonString = FormatDumpInfoToJsonString(specifiedRecordType, specifiedWindowId,
        sceneSessionChangeNeedDumpMapCopy);
    oss << dumpInfoJsonString;
    if (simplifyFlag && oss.str().size() > MAX_EVENT_DUMP_SIZE) {
        dumpInfo.append("wmsDumpSimplify\n");
        SimplifyDumpInfo(dumpInfo, oss.str());
    } else {
        dumpInfo.append(oss.str());
    }
}

std::string SessionChangeRecorder::FormatDumpInfoToJsonString (uint32_t specifiedRecordType, int32_t specifiedWindowId,
    std::unordered_map<RecordType, std::queue<SceneSessionChangeInfo>>& dumpMap)
{
    nlohmann::json jsonArrayDump = nlohmann::json::array();
    for (const auto& elem : dumpMap) {
        if (specifiedRecordType && static_cast<uint32_t>(elem.first) != specifiedRecordType) {
            continue;
        }
        std::queue<SceneSessionChangeInfo> tempDumpQueue = elem.second;
        while (!tempDumpQueue.empty()) {
            auto tempQueue = tempDumpQueue.front();
            if (specifiedWindowId && tempQueue.persistentId_ != specifiedWindowId) {
                tempDumpQueue.pop();
                continue;
            }
            jsonArrayDump.push_back({{"winId", tempQueue.persistentId_},
                {"changeInfo", tempQueue.changeInfo_}, {"time", tempQueue.time_}});
            tempDumpQueue.pop();
        }
    }
    return jsonArrayDump.dump();
}

void SessionChangeRecorder::SimplifyDumpInfo(std::string& dumpInfo, std::string preCompressInfo)
{
    std::unique_ptr<std::ostringstream> compressOstream = std::make_unique<std::ostringstream>();
    std::string compressInfo;
    if (CompressString(preCompressInfo.c_str(), preCompressInfo.size(), compressInfo, COMPRESS_VERSION) == Z_OK) {
        compressOstream->write(compressInfo.c_str(), compressInfo.length());
    } else {
        compressOstream->write(preCompressInfo.c_str(), preCompressInfo.length());
    }
    preCompressInfo.clear();
    compressOstream->flush();

    dumpInfo.append(compressOstream->str());
}
int SessionChangeRecorder::CompressString (const char* inStr, size_t inLen, std::string& outStr, int level)
{
    if (!inStr) {
        return Z_DATA_ERROR;
    }

    int ret;
    int flush;
    unsigned have;
    z_stream strm;

    unsigned char out[CHUNK];

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, level);
    if (ret != Z_OK)
        return ret;

    std::shared_ptr<z_stream> sp_strm(&strm, [](z_stream* strm) { (void)deflateEnd(strm); });
    const char* end = inStr + inLen;

    size_t distance = 0;
    /* compress until end of file */
    do {
        distance = end - inStr;
        strm.avail_in = (distance >= CHUNK) ? CHUNK : distance;
        strm.next_in = (Bytef*)inStr;
        inStr += strm.avail_in;
        flush = (inStr == end) ? Z_FINISH : Z_NO_FLUSH;
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);
            if (ret == Z_STREAM_ERROR)
                break;
            have = CHUNK - strm.avail_out;
            outStr.append((const char*)out, have);
        } while (strm.avail_out == 0);
    } while (flush != Z_FINISH || strm.avail_in != 0);
    if (ret != Z_STREAM_END) {
        return Z_STREAM_ERROR;
    }
    return Z_OK;
}

void SessionChangeRecorder::RecordDump(RecordType recordType, SceneSessionChangeInfo& changeInfo)
{
    TLOGD(WmsLogTag::DEFAULT, "In");
    uint32_t maxRecordTypeSize = MAX_RECORD_TYPE_SIZE;
    {
        std::lock_guard<std::mutex> lock(sessionChangeRecorderMutex_);
        if (sceneSessionChangeNeedDumpMap_.find(recordType) == sceneSessionChangeNeedDumpMap_.end()) {
            std::queue<SceneSessionChangeInfo> dumpQueue;
            dumpQueue.push(changeInfo);
            sceneSessionChangeNeedDumpMap_[recordType] = dumpQueue;
        } else {
            sceneSessionChangeNeedDumpMap_[recordType].push(changeInfo);
        }
        maxRecordTypeSize = recordSizeMap_.find(recordType) != recordSizeMap_.end() ?
            recordSizeMap_[recordType] : MAX_RECORD_TYPE_SIZE;
    }

    if (sceneSessionChangeNeedDumpMap_[recordType].size() > maxRecordTypeSize) {
        std::lock_guard<std::mutex> lock(sessionChangeRecorderMutex_);
        uint32_t diff = sceneSessionChangeNeedDumpMap_[recordType].size() - maxRecordTypeSize;
        while (diff > 0) {
            sceneSessionChangeNeedDumpMap_[recordType].pop();
            diff--;
        }
    }
}

void SessionChangeRecorder::RecordLog(RecordType recordType, SceneSessionChangeInfo& changeInfo)
{
    TLOGD(WmsLogTag::DEFAULT, "In");
    uint32_t maxRecordTypeSize = MAX_RECORD_TYPE_SIZE;
    {
        std::lock_guard<std::mutex> lock(sessionChangeRecorderMutex_);
        if (sceneSessionChangeNeedLogMap_.find(recordType) == sceneSessionChangeNeedLogMap_.end()) {
            std::queue<SceneSessionChangeInfo> logQueue;
            logQueue.push(changeInfo);
            sceneSessionChangeNeedLogMap_[recordType] = logQueue;
        } else {
            sceneSessionChangeNeedLogMap_[recordType].push(changeInfo);
        }
        currentLogSize_ += changeInfo.changeInfo_.size();
        maxRecordTypeSize = recordSizeMap_.find(recordType) != recordSizeMap_.end() ?
            recordSizeMap_[recordType] : MAX_RECORD_TYPE_SIZE;
    }
    if (currentLogSize_ >= MAX_RECORD_LOG_SIZE ||
        sceneSessionChangeNeedLogMap_[recordType].size() > maxRecordTypeSize) {
        TLOGD(WmsLogTag::DEFAULT, "currentLogSize: %{public}d", currentLogSize_);
        std::unordered_map<RecordType, std::queue<SceneSessionChangeInfo>> sceneSessionChangeNeedLogMapCopy;
        {
            std::lock_guard<std::mutex> lock(sessionChangeRecorderMutex_);
            sceneSessionChangeNeedLogMapCopy = sceneSessionChangeNeedLogMap_;
            sceneSessionChangeNeedLogMap_.clear();
            currentLogSize_ = 0;
        }
        PrintLog(sceneSessionChangeNeedLogMapCopy);
    }
}

void SessionChangeRecorder::PrintLog(
    std::unordered_map<RecordType, std::queue<SceneSessionChangeInfo>> sceneSessionChangeNeedLogMap)
{
    TLOGD(WmsLogTag::DEFAULT, "In");
    for (const auto& item : sceneSessionChangeNeedLogMap) {
        std::queue<SceneSessionChangeInfo> logInfoQueue = item.second;
        while (!logInfoQueue.empty()) {
            SceneSessionChangeInfo curChange = logInfoQueue.front();
            TLOGD(curChange.logTag_, "winId: %{public}d, changeInfo: %{public}s, time: %{public}s",
                curChange.persistentId_, curChange.changeInfo_.c_str(), curChange.time_.c_str());
            logInfoQueue.pop();
        }
    }
}
} // namespace OHOS::Rosen