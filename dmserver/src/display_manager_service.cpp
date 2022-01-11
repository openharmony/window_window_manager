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

#include "display_manager_service.h"

#include <cinttypes>
#include <unistd.h>

#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DisplayManagerService"};
}

const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(&SingletonContainer::Get<DisplayManagerService>());

DisplayManagerService::DisplayManagerService() : SystemAbility(DISPLAY_MANAGER_SERVICE_SA_ID, true)
{
}

void DisplayManagerService::OnStart()
{
    WLOGFI("DisplayManagerService::OnStart start");
    if (!Init()) {
        return;
    }
}

bool DisplayManagerService::Init()
{
    WLOGFI("DisplayManagerService::Init start");
    bool ret = Publish(this);
    if (!ret) {
        WLOGFW("DisplayManagerService::Init failed");
        return false;
    }
    WLOGFI("DisplayManagerService::Init success");
    return true;
}

DisplayId DisplayManagerService::GetDisplayIdFromScreenId(ScreenId screenId)
{
    return (DisplayId)screenId;
}

ScreenId DisplayManagerService::GetScreenIdFromDisplayId(DisplayId displayId)
{
    return (ScreenId)displayId;
}

DisplayId DisplayManagerService::GetDefaultDisplayId()
{
    ScreenId screenId = AbstractDisplayManager::GetInstance().GetDefaultScreenId();
    WLOGFI("GetDefaultDisplayId %{public}llu", screenId);
    return GetDisplayIdFromScreenId(screenId);
}

DisplayInfo DisplayManagerService::GetDisplayInfoById(DisplayId displayId)
{
    DisplayInfo displayInfo;
    ScreenId screenId = GetScreenIdFromDisplayId(displayId);
    auto screenModeInfo = AbstractDisplayManager::GetInstance().GetScreenActiveMode(screenId);
    displayInfo.id_ = displayId;
    displayInfo.width_ = screenModeInfo.GetScreenWidth();
    displayInfo.height_ = screenModeInfo.GetScreenHeight();
    displayInfo.freshRate_ = screenModeInfo.GetScreenFreshRate();
    return displayInfo;
}

DisplayId DisplayManagerService::CreateVirtualDisplay(const VirtualDisplayInfo &virtualDisplayInfo,
    sptr<Surface> surface)
{
    WLOGFI("name %{public}s, width %{public}u, height %{public}u, mirrotId %{public}llu, flags %{public}d",
        virtualDisplayInfo.name_.c_str(), virtualDisplayInfo.width_, virtualDisplayInfo.height_,
        virtualDisplayInfo.displayIdToMirror_, virtualDisplayInfo.flags_);
    ScreenId screenId = AbstractDisplayManager::GetInstance().CreateVirtualScreen(virtualDisplayInfo, surface);
    return GetDisplayIdFromScreenId(screenId);
}

bool DisplayManagerService::DestroyVirtualDisplay(DisplayId displayId)
{
    WLOGFI("DisplayManagerService::DestroyVirtualDisplay");
    ScreenId screenId = GetScreenIdFromDisplayId(displayId);
    return AbstractDisplayManager::GetInstance().DestroyVirtualScreen(screenId);
}

sptr<Media::PixelMap> DisplayManagerService::GetDispalySnapshot(DisplayId displayId)
{
    ScreenId screenId = GetScreenIdFromDisplayId(displayId);
    sptr<Media::PixelMap> screenSnapshot = AbstractDisplayManager::GetInstance().GetScreenSnapshot(screenId);
    return screenSnapshot;
}

void DisplayManagerService::OnStop()
{
    WLOGFI("ready to stop display service.");
}

bool DisplayManagerService::SuspendBegin(PowerStateChangeReason reason)
{
    return displayPowerController_.SuspendBegin(reason);
}

bool DisplayManagerService::SetDisplayState(DisplayState state)
{
    return displayPowerController_.SetDisplayState(state);
}

DisplayState DisplayManagerService::GetDisplayState(uint64_t displayId)
{
    return displayPowerController_.GetDisplayState(displayId);
}

void DisplayManagerService::NotifyDisplayEvent(DisplayEvent event)
{
    displayPowerController_.NotifyDisplayEvent(event);
}
} // namespace OHOS::Rosen