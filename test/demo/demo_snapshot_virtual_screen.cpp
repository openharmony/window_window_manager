/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include <ctime>
#include <iostream>
#include <string>

#include "display_manager_proxy.h"
#include "screen_manager.h"
#include "snapshot_utils.h"
#include "surface_reader.h"
#include "surface_reader_handler_impl.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace OHOS::Media;

namespace {
constexpr int SLEEP_US = 10 * 1000; // 10ms
constexpr int MAX_SNAPSHOT_COUNT = 10;
constexpr int MAX_WAIT_COUNT = 200;
constexpr float DEFAULT_DENSITY = 2.0;
const std::string FILE_NAME = "/data/local/tmp/snapshot_virtual_screen";
}

static ScreenId mainId;
static ScreenId virtualScreenId;

static std::unique_ptr<VirtualScreenOption> InitOption(ScreenId mainId, const SurfaceReader& surfaceReader)
{
    auto defaultScreen = ScreenManager::GetInstance().GetScreenById(mainId);
    std::unique_ptr<VirtualScreenOption> option = std::make_unique<VirtualScreenOption>();
    if (option == nullptr) {
        return option;
    }

    option->name_ = "virtualScreen";
    option->width_ = defaultScreen->GetWidth();
    option->height_ = defaultScreen->GetHeight();
    option->density_ = DEFAULT_DENSITY;
    option->surface_ = surfaceReader.GetSurface();
    option->flags_ = 0;
    option->isForShot_ = true;
    return option;
}

static bool InitMirror(const SurfaceReader& surfaceReader)
{
    mainId = static_cast<ScreenId>(DisplayManager::GetInstance().GetDefaultDisplayId());
    if (mainId == SCREEN_ID_INVALID) {
        std::cout<< "get default display id failed!" << std::endl;
        return false;
    }

    std::unique_ptr<VirtualScreenOption> option = InitOption(mainId, surfaceReader);
    if (option == nullptr) {
        return false;
    }

    virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(*option);
    std::vector<ScreenId> mirrorIds;
    mirrorIds.push_back(virtualScreenId);
    ScreenId screenGroupId = static_cast<ScreenId>(1);
    ScreenManager::GetInstance().MakeMirror(mainId, mirrorIds, screenGroupId);
    return true;
}

int main(int argc, char *argv[])
{
    SurfaceReader surfaceReader;
    sptr<SurfaceReaderHandlerImpl> surfaceReaderHandler = new SurfaceReaderHandlerImpl();
    if (!surfaceReader.Init()) {
        std::cout << "surfaceReader init failed!" << std::endl;
        return 0;
    }
    surfaceReader.SetHandler(surfaceReaderHandler);
    if (!InitMirror(surfaceReader)) {
        return 0;
    }
    int fileIndex = 1;
    auto startTime = time(nullptr);
    if (startTime < 0) {
        std::cout << "startTime error!" << std::endl;
        return 0;
    }
    while (time(nullptr) - startTime < MAX_SNAPSHOT_COUNT) {
        int waitCount = 0;
        while (!surfaceReaderHandler->IsImageOk()) {
            waitCount++;
            if (waitCount >= MAX_WAIT_COUNT) {
                std::cout << "wait image overtime" << std::endl;
                break;
            }
            usleep(SLEEP_US);
        }
        if (waitCount >= MAX_WAIT_COUNT) {
            continue;
        }
        auto pixelMap = surfaceReaderHandler->GetPixelMap();
        bool ret = SnapShotUtils::WriteToJpegWithPixelMap(FILE_NAME + std::to_string(fileIndex) + ".jpeg", *pixelMap);
        if (ret) {
            std::cout << "snapshot "<< mainId << " write to " <<
                (FILE_NAME + std::to_string(fileIndex)).c_str() << " as jpeg" << std::endl;
        } else {
            std::cout << "snapshot "<< mainId << " write to " <<
                (FILE_NAME + std::to_string(fileIndex)).c_str() << " failed!" << std::endl;
        }
        surfaceReaderHandler->ResetFlag();
        fileIndex++;
    }
    ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId);
    std::cout << "DestroyVirtualScreen " << virtualScreenId << std::endl;
    return 0;
}
