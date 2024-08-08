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

#include <cstdio>
#include <image_type.h>
#include <iosfwd>
#include <iostream>
#include <memory>
#include <ostream>
#include <refbase.h>

#include "display_manager.h"
#include "parameters.h"
#include "snapshot_utils.h"

using namespace OHOS;
using namespace OHOS::Media;
using namespace OHOS::Rosen;
using OHOS::system::GetParameter;

// developer mode
static const std::string DEVELOPER_MODE_STATE_ON_DEFAULT = "false";
static const std::string DEVELOPER_MODE_PARAMETER = "const.security.developermode.state";
static const std::string IS_DEVELOPER_MODE = GetParameter(DEVELOPER_MODE_PARAMETER, DEVELOPER_MODE_STATE_ON_DEFAULT);

static bool GetScreenshotByCmdArguments(CmdArguments& cmdArguments, sptr<Display> display,
    std::shared_ptr<OHOS::Media::PixelMap>& pixelMap);

int main(int argc, char* argv[])
{
    CmdArguments cmdArguments;
    cmdArguments.fileName = "";

    if (!SnapShotUtils::ProcessArgs(argc, argv, cmdArguments)) {
        _exit(-1);
    }

    if (DEVELOPER_MODE_STATE_ON_DEFAULT == IS_DEVELOPER_MODE) {
        std::cout << "current mode is not developer mode, just return." << std::endl;
        _exit(-1);
    }

    auto display = DisplayManager::GetInstance().GetDisplayById(cmdArguments.displayId);
    if (display == nullptr) {
        std::cout << "error: GetDisplayById " << cmdArguments.displayId << " error!" << std::endl;
        _exit(-1);
    }
    if (cmdArguments.fileType != "png") {
        cmdArguments.fileType = "jpeg";
    }

    std::cout << "process: display " << cmdArguments.displayId << ", file type: " << cmdArguments.fileType <<
        ", width: " << display->GetWidth() << ", height: " << display->GetHeight() << std::endl;

    // get PixelMap from DisplayManager API
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap = nullptr;
    if (!GetScreenshotByCmdArguments(cmdArguments, display, pixelMap)) {
        _exit(-1);
    }

    bool ret = false;
    if (pixelMap != nullptr) {
        if (cmdArguments.fileType == "png") {
            ret = SnapShotUtils::SaveSnapShot(cmdArguments.fileName, *pixelMap, cmdArguments.fileType);
        } else {
            ret = SnapShotUtils::WriteToJpegWithPixelMap(cmdArguments.fileName, *pixelMap);
        }
    }
    if (!ret) {
        std::cout << "\nerror: snapshot display " << cmdArguments.displayId <<
            ", write to " << cmdArguments.fileName << " as jpeg failed!" << std::endl;
        _exit(-1);
    }

    std::cout << "\nsuccess: snapshot display " << cmdArguments.displayId << " , write to " <<
        cmdArguments.fileName << " as " << cmdArguments.fileType << ", width: " << pixelMap->GetWidth() <<
        ", height: " << pixelMap->GetHeight() << std::endl;
    _exit(0);
}

static bool GetScreenshotByCmdArguments(CmdArguments& cmdArguments, sptr<Display> display,
    std::shared_ptr<OHOS::Media::PixelMap>& pixelMap)
{
    if (!cmdArguments.isWidthSet && !cmdArguments.isHeightSet) {
        pixelMap = DisplayManager::GetInstance().GetScreenshot(cmdArguments.displayId);  // default width & height
    } else {
        if (!cmdArguments.isWidthSet) {
            cmdArguments.width = display->GetWidth();
            std::cout << "process: reset to display's width " << cmdArguments.width << std::endl;
        }
        if (!cmdArguments.isHeightSet) {
            cmdArguments.height = display->GetHeight();
            std::cout << "process: reset to display's height " << cmdArguments.height << std::endl;
        }
        if (!SnapShotUtils::CheckWidthAndHeightValid(cmdArguments.width, cmdArguments.height)) {
            std::cout << "error: width " << cmdArguments.width << " height " <<
            cmdArguments.height << " invalid!" << std::endl;
            return false;
        }
        const Media::Rect rect = {0, 0, display->GetWidth(), display->GetHeight()};
        const Media::Size size = {cmdArguments.width, cmdArguments.height};
        constexpr int rotation = 0;
        pixelMap = DisplayManager::GetInstance().GetScreenshot(cmdArguments.displayId, rect, size, rotation);
    }
    return true;
}