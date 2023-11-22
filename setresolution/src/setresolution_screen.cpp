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

#include <iostream>

#include "dm_common.h"
#include "screen_manager.h"
#include "setresolution_utils.h"

using namespace OHOS;
using namespace OHOS::Rosen;

int main(int argc, char *argv[])
{
    CmdArgments cmdArgments;

    if (!SetResolutionUtils::ProcessArgs(argc, argv, cmdArgments)) {
        return 0;
    }
    if (!cmdArgments.isWidthSet || !cmdArgments.isHeightSet || !cmdArgments.isDpiSet) {
        std::cout << "Error! Must set width, height and dpi." << std::endl;
        return 0;
    }
    std::cout << "width: " << cmdArgments.width << " height: " << cmdArgments.height <<
        " dpi: " << cmdArgments.dpi << std::endl;
    
    std::vector<sptr<Rosen::Screen>> screens;
    Rosen::ScreenManager::GetInstance().GetAllScreens(screens);
    if (screens.size() == 0) {
        return 0;
    }
    DMError ret = screens[0]->SetResolution(cmdArgments.width, cmdArgments.height, cmdArgments.dpi);
    if (ret != DMError::DM_OK) {
        std::cout<< "Error! SetResolution failed!" << std::endl;
    }
    std::cout<< "SetResolution successful!" << std::endl;
    return 0;
}