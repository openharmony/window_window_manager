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

#ifndef SNAPSHOT_UTILS_H
#define SNAPSHOT_UTILS_H

#include <cstdint>
#include <pixel_map.h>
#include <string>

#include "display_manager.h"
#include "dm_common.h"

namespace OHOS {

struct WriteToJpegParam {
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    Media::PixelFormat format;
    const uint8_t *data;
};

struct CmdArguments {
    Rosen::DisplayId displayId = Rosen::DISPLAY_ID_INVALID;
    std::string fileName;
    std::string fileType;
    int32_t width = -1;
    int32_t height = -1;
    bool isDisplayIdSet = false;
    bool isWidthSet = false;
    bool isHeightSet = false;
};

class SnapShotUtils {
public:
    SnapShotUtils() = default;
    ~SnapShotUtils() = default;

    static void PrintUsage(const std::string& cmdLine);
    static bool CheckFileNameValid(const std::string& fileName, std::string fileType = "jpeg");
    static std::string GenerateFileName(std::string fileType, int offset = 0);
    static bool CheckWidthAndHeightValid(int32_t w, int32_t h);
    static bool RGBA8888ToRGB888(const uint8_t* rgba8888Buf, uint8_t* rgb888Buf, int32_t size);
    static bool RGB565ToRGB888(const uint8_t* rgb565Buf, uint8_t* rgb888Buf, int32_t size);
    static bool WriteRgb888ToJpeg(FILE* file, uint32_t width, uint32_t height, const uint8_t* data);
    static bool WriteToJpeg(const std::string& fileName, const WriteToJpegParam& param);
    static bool WriteToJpeg(int fd, const WriteToJpegParam& param);
    static bool WriteToJpegWithPixelMap(const std::string& fileName, Media::PixelMap& pixelMap);
    static bool WriteToJpegWithPixelMap(int fd, Media::PixelMap& pixelMap);
    static bool ProcessArgs(int argc, char * const argv[], CmdArguments& cmdArgments);
    static bool CheckWHValid(int32_t param);
    static bool CheckParamValid(const WriteToJpegParam& param);
    static bool SaveSnapShot(const std::string& filename, Media::PixelMap& pixelMap, std::string fileType = "jpeg");
private:
    static bool ProcessDisplayId(Rosen::DisplayId& displayId, bool isDisplayIdSet);
};
}

#endif // SNAPSHOT_UTILS_H
