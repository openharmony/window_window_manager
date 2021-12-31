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

#include "display_screen_manager.h"

namespace OHOS::Rosen {
IMPLEMENT_SINGLE_INSTANCE(DisplayScreenManager);

DisplayScreenManager::DisplayScreenManager() : rsInterface_(&(RSInterfaces::GetInstance()))
{
    parepareRSScreenManger();
}

DisplayScreenManager::~DisplayScreenManager()
{
    rsInterface_ = nullptr;
}

void DisplayScreenManager::parepareRSScreenManger()
{
}

ScreenId DisplayScreenManager::GetDefaultScreenId()
{
    if (rsInterface_ == nullptr) {
        return INVALID_SCREEN_ID;
    }
    return rsInterface_->GetDefaultScreenId();
}

RSScreenModeInfo DisplayScreenManager::GetScreenActiveMode(ScreenId id)
{
    RSScreenModeInfo screenModeInfo;
    if (rsInterface_ == nullptr) {
        return screenModeInfo;
    }
    return rsInterface_->GetScreenActiveMode(id);
}
} // namespace OHOS::Rosen