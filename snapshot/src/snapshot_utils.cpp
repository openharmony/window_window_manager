/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "snapshot_utils.h"

#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <getopt.h>
#include <hitrace_meter.h>
#include <image_type.h>
#include <iostream>
#include <ostream>
#include <pixel_map.h>
#include <png.h>
#include <securec.h>
#include <string>
#include <sys/time.h>

using namespace OHOS::Rosen;

namespace OHOS {
constexpr int BITMAP_DEPTH = 8;
constexpr int MAX_TIME_STR_LEN = 40;
constexpr int YEAR_SINCE = 1900;

const char *VALID_SNAPSHOT_PATH = "/data";
const char *DEFAULT_SNAPSHOT_PREFIX = "/snapshot";
const char *VALID_SNAPSHOT_SUFFIX = ".png";

void SnapShotUtils::PrintUsage(const std::string &cmdLine)
{
    std::cout << "usage: " << cmdLine.c_str() <<
        " [-i displayId] [-f output_file] [-w width] [-h height] [-m]" << std::endl;
}

std::string SnapShotUtils::GenerateFileName(int offset)
{
    timeval tv;
    std::string fileName = VALID_SNAPSHOT_PATH;

    fileName += DEFAULT_SNAPSHOT_PREFIX;
    if (gettimeofday(&tv, nullptr) == 0) {
        tv.tv_sec += offset; // add offset second
        struct tm *tmVal = localtime(&tv.tv_sec);
        if (tmVal != nullptr) {
            char timeStr[MAX_TIME_STR_LEN] = { 0 };
            snprintf_s(timeStr, sizeof(timeStr), sizeof(timeStr) - 1,
                "_%04d-%02d-%02d_%02d-%02d-%02d",
                tmVal->tm_year + YEAR_SINCE, tmVal->tm_mon + 1, tmVal->tm_mday,
                tmVal->tm_hour, tmVal->tm_min, tmVal->tm_sec);
            fileName += timeStr;
        }
    }
    fileName += VALID_SNAPSHOT_SUFFIX;
    return fileName;
}

bool SnapShotUtils::CheckFileNameValid(const std::string &fileName)
{
    if (fileName.length() <= strlen(VALID_SNAPSHOT_SUFFIX)) {
        std::cout << "error: fileName " << fileName.c_str() << " invalid, file length too short!" << std::endl;
        return false;
    }
    // check file path
    std::string fileDir = fileName;
    auto pos = fileDir.find_last_of("/");
    if (pos != std::string::npos) {
        fileDir.erase(pos + 1);
    } else {
        fileDir = "."; // current work dir
    }
    char resolvedPath[PATH_MAX] = { 0 };
    char *realPath = realpath(fileDir.c_str(), resolvedPath);
    if (realPath == nullptr) {
        std::cout << "error: fileName " << fileName.c_str() << " invalid, realpath nullptr!" << std::endl;
        return false;
    }
    if (strncmp(realPath, VALID_SNAPSHOT_PATH, strlen(VALID_SNAPSHOT_PATH)) != 0) {
        std::cout << "error: fileName " << fileName.c_str() << " invalid, realpath "
            << realPath << " must dump at dir: " << VALID_SNAPSHOT_PATH << std::endl;
        return false;
    }

    // check file suffix
    const char *fileNameSuffix = fileName.c_str() + (fileName.length() - strlen(VALID_SNAPSHOT_SUFFIX));
    if (strncmp(fileNameSuffix, VALID_SNAPSHOT_SUFFIX, strlen(VALID_SNAPSHOT_SUFFIX)) == 0) {
        return true; // valid suffix
    }

    std::cout << "error: fileName " << fileName.c_str() <<
        " invalid, suffix must be " << VALID_SNAPSHOT_SUFFIX << std::endl;
    return false;
}

bool SnapShotUtils::CheckWHValid(int32_t param)
{
    return (param > 0) && (param <= DisplayManager::MAX_RESOLUTION_SIZE_SCREENSHOT);
}

bool SnapShotUtils::CheckWidthAndHeightValid(int32_t w, int32_t h)
{
    return CheckWHValid(w) && CheckWHValid(h);
}

bool SnapShotUtils::CheckParamValid(const WriteToPngParam &param)
{
    if (!CheckWidthAndHeightValid(param.width, param.height)) {
        return false;
    }
    if (param.stride < BPP * param.width) {
        return false;
    }
    if (param.data == nullptr) {
        return false;
    }
    return true;
}

bool SnapShotUtils::WriteToPng(const std::string &fileName, const WriteToPngParam &param)
{
    if (!CheckFileNameValid(fileName)) {
        return false;
    }
    if (!CheckParamValid(param)) {
        return false;
    }

    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "snapshot:WriteToPng(%s)", fileName.c_str());

    png_structp pngStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (pngStruct == nullptr) {
        std::cout << "error: png_create_write_struct nullptr!" << std::endl;
        return false;
    }
    png_infop pngInfo = png_create_info_struct(pngStruct);
    if (pngInfo == nullptr) {
        std::cout << "error: png_create_info_struct error nullptr!" << std::endl;
        png_destroy_write_struct(&pngStruct, nullptr);
        return false;
    }
    FILE *fp = fopen(fileName.c_str(), "wb");
    if (fp == nullptr) {
        std::cout << "error: open file [" << fileName.c_str() << "] error, " << errno << "!" << std::endl;
        png_destroy_write_struct(&pngStruct, &pngInfo);
        return false;
    }
    png_init_io(pngStruct, fp);

    // set png header
    png_set_IHDR(pngStruct, pngInfo,
        param.width, param.height,
        param.bitDepth,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE,
        PNG_FILTER_TYPE_BASE);
    png_set_packing(pngStruct); // set packing info
    png_write_info(pngStruct, pngInfo); // write to header

    for (uint32_t i = 0; i < param.height; i++) {
        png_write_row(pngStruct, param.data + (i * param.stride));
    }

    png_write_end(pngStruct, pngInfo);

    // free
    png_destroy_write_struct(&pngStruct, &pngInfo);
    if (fclose(fp) != 0) {
        return false;
    }
    return true;
}

bool SnapShotUtils::WriteToPng(int fd, const WriteToPngParam &param)
{
    if (!CheckParamValid(param)) {
        return false;
    }

    png_structp pngStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (pngStruct == nullptr) {
        std::cout << "error: png_create_write_struct nullptr!" << std::endl;
        return false;
    }
    png_infop pngInfo = png_create_info_struct(pngStruct);
    if (pngInfo == nullptr) {
        std::cout << "error: png_create_info_struct error nullptr!" << std::endl;
        png_destroy_write_struct(&pngStruct, nullptr);
        return false;
    }
    FILE *fp = fdopen(fd, "wb");
    if (fp == nullptr) {
        png_destroy_write_struct(&pngStruct, &pngInfo);
        return false;
    }
    png_init_io(pngStruct, fp);

    // set png header
    png_set_IHDR(pngStruct, pngInfo,
        param.width, param.height,
        param.bitDepth,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE,
        PNG_FILTER_TYPE_BASE);
    png_set_packing(pngStruct); // set packing info
    png_write_info(pngStruct, pngInfo); // write to header

    for (uint32_t i = 0; i < param.height; i++) {
        png_write_row(pngStruct, param.data + (i * param.stride));
    }

    png_write_end(pngStruct, pngInfo);

    // free
    png_destroy_write_struct(&pngStruct, &pngInfo);
    if (fclose(fp) != 0) {
        return false;
    }
    return true;
}

bool SnapShotUtils::WriteToPngWithPixelMap(const std::string &fileName, Media::PixelMap &pixelMap)
{
    WriteToPngParam param;
    param.width = static_cast<uint32_t>(pixelMap.GetWidth());
    param.height = static_cast<uint32_t>(pixelMap.GetHeight());
    param.data = pixelMap.GetPixels();
    param.stride = static_cast<uint32_t>(pixelMap.GetRowBytes());
    param.bitDepth = BITMAP_DEPTH;
    return SnapShotUtils::WriteToPng(fileName, param);
}

bool SnapShotUtils::WriteToPngWithPixelMap(int fd, Media::PixelMap &pixelMap)
{
    WriteToPngParam param;
    param.width = static_cast<uint32_t>(pixelMap.GetWidth());
    param.height = static_cast<uint32_t>(pixelMap.GetHeight());
    param.data = pixelMap.GetPixels();
    param.stride = static_cast<uint32_t>(pixelMap.GetRowBytes());
    param.bitDepth = BITMAP_DEPTH;
    return SnapShotUtils::WriteToPng(fd, param);
}

bool SnapShotUtils::ProcessDisplayId(Rosen::DisplayId &displayId, bool isDisplayIdSet)
{
    if (!isDisplayIdSet) {
        displayId = DisplayManager::GetInstance().GetDefaultDisplayId();
    } else {
        bool validFlag = false;
        auto displayIds = DisplayManager::GetInstance().GetAllDisplayIds();
        for (auto id: displayIds) {
            if (displayId == id) {
                validFlag = true;
                break;
            }
        }
        if (!validFlag) {
            std::cout << "error: displayId " << static_cast<int64_t>(displayId) << " invalid!" << std::endl;
            std::cout << "tips: supported displayIds:" << std::endl;
            for (auto dispId: displayIds) {
                std::cout << "\t" << dispId << std::endl;
            }
            return false;
        }
    }
    return true;
}

bool SnapShotUtils::ProcessArgs(int argc, char * const argv[], CmdArgments &cmdArgments)
{
    int opt = 0;
    const struct option longOption[] = {
        { "id", required_argument, nullptr, 'i' },
        { "width", required_argument, nullptr, 'w' },
        { "height", required_argument, nullptr, 'h' },
        { "file", required_argument, nullptr, 'f' },
        { "help", required_argument, nullptr, 'm' },
        { nullptr, 0, nullptr, 0 }
    };
    while ((opt = getopt_long(argc, argv, "i:w:h:f:m", longOption, nullptr)) != -1) {
        switch (opt) {
            case 'i': // display id
                cmdArgments.displayId = static_cast<DisplayId>(atoll(optarg));
                cmdArgments.isDisplayIdSet = true;
                break;
            case 'w': // output width
                cmdArgments.width = atoi(optarg);
                cmdArgments.isWidthSet = true;
                break;
            case 'h': // output height
                cmdArgments.height = atoi(optarg);
                cmdArgments.isHeightSet = true;
                break;
            case 'f': // output file name
                cmdArgments.fileName = optarg;
                break;
            case 'm': // help
            default:
                SnapShotUtils::PrintUsage(argv[0]);
                return false;
        }
    }

    if (!ProcessDisplayId(cmdArgments.displayId, cmdArgments.isDisplayIdSet)) {
        return false;
    }

    if (cmdArgments.fileName == "") {
        cmdArgments.fileName = GenerateFileName();
        std::cout << "process: set filename to " << cmdArgments.fileName.c_str() << std::endl;
    }

    // check fileName
    if (!SnapShotUtils::CheckFileNameValid(cmdArgments.fileName)) {
        std::cout << "error: filename " << cmdArgments.fileName.c_str() << " invalid!" << std::endl;
        return false;
    }
    return true;
}
}