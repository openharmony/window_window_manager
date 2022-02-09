/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <cstdio>
#include <getopt.h>
#include <png.h>
#include "wm_trace.h"

using namespace OHOS::Media;
using namespace OHOS::Rosen;

namespace OHOS {
constexpr int BITMAP_DEPTH = 8;
constexpr int BPP = 4;
constexpr int MAX_RESOLUTION_SIZE = 15360;
const char *VALID_SNAPSHOT_PATH = "/data";
const char *VALID_SNAPSHOT_SUFFIX = ".png";

void SnapShotUtils::PrintUsage(const std::string &cmdLine)
{
    printf("usage: %s [-i displayId] [-f output_file] [-w width] [-h height] [-m]\n", cmdLine.c_str());
}

bool SnapShotUtils::CheckFileNameValid(const std::string &fileName)
{
    WM_SCOPED_TRACE("snapshot:CheckFileNameValid(%s)", fileName.c_str());
    if (fileName.length() <= strlen(VALID_SNAPSHOT_SUFFIX)) {
        printf("error: fileName %s invalid, file length too short!\n", fileName.c_str());
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
        printf("error: fileName %s invalid, realpath nullptr!\n", fileName.c_str());
        return false;
    }
    if (strncmp(realPath, VALID_SNAPSHOT_PATH, strlen(VALID_SNAPSHOT_PATH)) != 0) {
        printf("error: fileName %s invalid, realpath %s must dump at dir: %s \n",
            fileName.c_str(), realPath, VALID_SNAPSHOT_PATH);
        return false;
    }

    // check file suffix
    const char *fileNameSuffix = fileName.c_str() + (fileName.length() - strlen(VALID_SNAPSHOT_SUFFIX));
    if (strncmp(fileNameSuffix, VALID_SNAPSHOT_SUFFIX, strlen(VALID_SNAPSHOT_SUFFIX)) == 0) {
        return true; // valid suffix
    }

    printf("error: fileName %s invalid, suffix must be %s\n", fileName.c_str(), VALID_SNAPSHOT_SUFFIX);
    return false;
}

static bool CheckWHValid(int32_t param)
{
    if ((param <= 0) || (param > MAX_RESOLUTION_SIZE)) {
        return false;
    }
    return true;
}

bool SnapShotUtils::CheckWidthAndHeightValid(const CmdArgments& cmdArgments)
{
    if (!CheckWHValid(cmdArgments.width) || !CheckWHValid(cmdArgments.height)) {
        return false;
    }
    return true;
}

static bool CheckParamValid(const WriteToPngParam &param)
{
    if (param.width > MAX_RESOLUTION_SIZE) {
        return false;
    }
    if (param.height > MAX_RESOLUTION_SIZE) {
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

    WM_SCOPED_TRACE("snapshot:WriteToPng(%s)", fileName.c_str());

    png_structp pngStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (pngStruct == nullptr) {
        printf("error: png_create_write_struct nullptr!\n");
        return false;
    }
    png_infop pngInfo = png_create_info_struct(pngStruct);
    if (pngInfo == nullptr) {
        printf("error: png_create_info_struct error nullptr!\n");
        png_destroy_write_struct(&pngStruct, nullptr);
        return false;
    }
    FILE *fp = fopen(fileName.c_str(), "wb");
    if (fp == nullptr) {
        printf("error: open file [%s] error, %d!\n", fileName.c_str(), errno);
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

bool SnapShotUtils::WriteToPngWithPixelMap(const std::string &fileName, PixelMap &pixelMap)
{
    WriteToPngParam param;
    param.width = static_cast<uint32_t>(pixelMap.GetWidth());
    param.height = static_cast<uint32_t>(pixelMap.GetHeight());
    param.data = pixelMap.GetPixels();
    param.stride = static_cast<uint32_t>(pixelMap.GetRowBytes());
    param.bitDepth = BITMAP_DEPTH;
    return SnapShotUtils::WriteToPng(fileName, param);
}

static bool ProcessDisplayId(DisplayId &displayId)
{
    WM_SCOPED_TRACE("snapshot:ProcessDisplayId(%" PRIu64")", displayId);
    if (displayId == DISPLAY_ID_INVALD) {
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
            printf("error: displayId %" PRIu64 " invalid!\n", displayId);
            printf("tips: supported displayIds:\n");
            for (auto dispId: displayIds) {
                printf("\t%" PRIu64 "\n", dispId);
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
                SnapShotUtils::PrintUsage(argv[0]);
                return false;
            default:
                SnapShotUtils::PrintUsage(argv[0]);
                return false;
        }
    }

    if (!ProcessDisplayId(cmdArgments.displayId)) {
        return false;
    }

    // check fileName
    if (!SnapShotUtils::CheckFileNameValid(cmdArgments.fileName)) {
        printf("error: filename %s invalid!\n", cmdArgments.fileName.c_str());
        return false;
    }
    return true;
}
}