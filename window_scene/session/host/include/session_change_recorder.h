/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_SESSION_CHANGE_RECORDER_H
#define OHOS_ROSEN_SESSION_CHANGE_RECORDER_H

#include <cstdint>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>

#include "window_manager_hilog.h"
#include "ws_common.h"
#include "wm_single_instance.h"

namespace OHOS::Rosen{
/**
 * @brief Record type of session change
 */
enum class RecordType : uint32_t {
    RECORD_TYPE_BEGIN = 0,
    SESSION_STATE_RECORD,
    VISIBLE_RECORD,
    PRIVACY_MODE,
    ORIENTAION_RECORD,
    RECORD_TYPE_END,
};

/**
 * @brief Scene session change info
 */
struct SceneSessionChangeInfo {
    int32_t persistentId_ = INVALID_SESSION_ID;
    std::string changeInfo_ = "";
    WmsLogTag logTag_ = WmsLogTag::DEFAULT;
    std::string time_ = "";
};

class SessionChangeRecorder {
WM_DECLARE_SINGLE_INSTANCE_BASE(SessionChangeRecorder)
public:
    void Init();
    WSError RecordSceneSessionChange(RecordType recordType, SceneSessionChangeInfo& changeInfo);
    WSError SetRecordSize(RecordType recordType, uint32_t recordSize);
    void GetSceneSessionNeedDumpInfo(std::string& dumpInfo, std::vector<std::string>& params);
    void SimplifyDumpInfo(std::string& dumpInfo, std::string preCompressInfo);
    int CompressString(const char* in_str, size_t in_len, std::string& out_str, int level);
    std::unordered_map<RecordType, std::queue<SceneSessionChangeInfo>> const GetNeedLogMap() {
        return sceneSessionChangeNeedLogMap_; }
    std::unordered_map<RecordType, std::queue<SceneSessionChangeInfo>> const GetNeedDumpMap() {
        return sceneSessionChangeNeedDumpMap_; }
    uint32_t GetLogSize() { return currentLogSize_; }
    std::atomic<bool> stopLogFlag { false };

private:
    SessionChangeRecorder() = default;
    virtual ~SessionChangeRecorder();
    void RecordLog(RecordType recordType, SceneSessionChangeInfo& changeInfo);
    void PrintLog(std::unordered_map<RecordType, std::queue<SceneSessionChangeInfo>> sceneSessionChangeNeedDumpMap);
    void RecordDump(RecordType recordType, SceneSessionChangeInfo& changeInfo);

    std::unordered_map<RecordType, std::queue<SceneSessionChangeInfo>> sceneSessionChangeNeedLogMap_;
    std::unordered_map<RecordType, std::queue<SceneSessionChangeInfo>> sceneSessionChangeNeedDumpMap_;
    std::unordered_map<RecordType, uint32_t> recordSizeMap_;
    std::mutex sessionChangeRecorderMutex_;
    uint32_t currentLogSize_ = 0;
    std::thread mThread;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_SESSION_CHANGE_RECORDER_H