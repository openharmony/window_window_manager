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

#include "abstract_display_manager.h"

#include "window_manager_hilog.h"

#include <surface.h>

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "AbstractDisplayManager"};
}

AbstractDisplayManager::AbstractDisplayManager() : rsInterface_(&(RSInterfaces::GetInstance()))
{
    parepareRSScreenManger();
}

AbstractDisplayManager::~AbstractDisplayManager()
{
    rsInterface_ = nullptr;
}

void AbstractDisplayManager::parepareRSScreenManger()
{
}

ScreenId AbstractDisplayManager::GetDefaultScreenId()
{
    if (rsInterface_ == nullptr) {
        return INVALID_SCREEN_ID;
    }
    return rsInterface_->GetDefaultScreenId();
}

RSScreenModeInfo AbstractDisplayManager::GetScreenActiveMode(ScreenId id)
{
    RSScreenModeInfo screenModeInfo;
    if (rsInterface_ == nullptr) {
        return screenModeInfo;
    }
    return rsInterface_->GetScreenActiveMode(id);
}

ScreenId AbstractDisplayManager::CreateVirtualScreen(const VirtualDisplayInfo &virtualDisplayInfo,
    sptr<Surface> surface)
{
    if (rsInterface_ == nullptr) {
        return INVALID_SCREEN_ID;
    }
    ScreenId result = rsInterface_->CreateVirtualScreen(virtualDisplayInfo.name_, virtualDisplayInfo.width_,
        virtualDisplayInfo.height_, surface, virtualDisplayInfo.displayIdToMirror_, virtualDisplayInfo.flags_);
    WLOGFI("AbstractDisplayManager::CreateVirtualDisplay id: %{public}llu", result >> 32);
    return result;
}

bool AbstractDisplayManager::DestroyVirtualScreen(ScreenId screenId)
{
    if (rsInterface_ == nullptr) {
        return false;
    }
    WLOGFI("AbstractDisplayManager::DestroyVirtualScreen");
    rsInterface_->RemoveVirtualScreen(screenId);
    return true;
}

sptr<Media::PixelMap> AbstractDisplayManager::GetScreenSnapshot(ScreenId screenId)
{
    if (rsInterface_ == nullptr) {
        return nullptr;
    }

    sptr<Media::PixelMap> screenshot = nullptr;

    return screenshot;
}
} // namespace OHOS::Rosen