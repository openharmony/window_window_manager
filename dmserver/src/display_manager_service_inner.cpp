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

#include "display_manager_service_inner.h"

#include <cinttypes>
#include <unistd.h>

#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "abstract_display_controller.h"
#include "display_manager_service.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManagerServiceInner"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(DisplayManagerServiceInner)

DisplayId DisplayManagerServiceInner::GetDefaultDisplayId()
{
    return DisplayManagerService::GetInstance().GetDefaultDisplayId();
}

const sptr<AbstractDisplay> DisplayManagerServiceInner::GetDisplayById(DisplayId displayId)
{
    sptr<AbstractDisplay> display = DisplayManagerService::GetInstance().GetAbstractDisplay(displayId);
    if (display == nullptr) {
        WLOGFE("GetDisplayById can not find corresponding display!\n");
    }
    return display;
}

const sptr<AbstractDisplay> DisplayManagerServiceInner::GetDefaultDisplay()
{
    DisplayId defaultDisplayId = GetDefaultDisplayId();
    if (defaultDisplayId == DISPLAY_ID_INVALD) {
        WLOGFE("Fail to get default displayId");
        return nullptr;
    }
    return GetDisplayById(GetDefaultDisplayId());
}

std::vector<DisplayId> DisplayManagerServiceInner::GetAllDisplayIds()
{
    return DisplayManagerService::GetInstance().GetAllDisplayIds();
}

std::vector<const sptr<AbstractDisplay>> DisplayManagerServiceInner::GetAllDisplays()
{
    std::vector<const sptr<AbstractDisplay>> res;
    auto displayIds = GetAllDisplayIds();
    for (auto displayId: displayIds) {
        const sptr<AbstractDisplay> display = GetDisplayById(displayId);
        if (display != nullptr) {
            res.push_back(display);
        } else {
            WLOGFE("GetAllDisplays display %" PRIu64" nullptr!", displayId);
        }
    }
    return res;
}

void DisplayManagerServiceInner::UpdateRSTree(DisplayId displayId, std::shared_ptr<RSSurfaceNode>& surfaceNode,
    bool isAdd)
{
    DisplayManagerService::GetInstance().UpdateRSTree(displayId, surfaceNode, isAdd);
}

ScreenId DisplayManagerServiceInner::GetRSScreenId(DisplayId displayId) const
{
    return DisplayManagerService::GetInstance().GetRSScreenId(displayId);
}

const sptr<ScreenInfo> DisplayManagerServiceInner::GetScreenInfoByDisplayId(DisplayId displayId) const
{
    return DisplayManagerService::GetInstance().GetScreenInfoById(
        DisplayManagerService::GetInstance().GetScreenIdByDisplayId(displayId));
}

const sptr<SupportedScreenModes> DisplayManagerServiceInner::GetScreenModesByDisplayId(DisplayId displayId)
{
    const sptr<AbstractDisplay> display = GetDisplayById(displayId);
    if (display == nullptr) {
        WLOGFE("can not get display.");
        return nullptr;
    }
    ScreenId dmsScreenId = display->GetAbstractScreenId();
    sptr<AbstractScreen> abstractScreen =
        DisplayManagerService::GetInstance().abstractScreenController_->GetAbstractScreen(dmsScreenId);
    if (abstractScreen == nullptr) {
        WLOGFE("can not get screenMode.");
        return nullptr;
    }
    return abstractScreen->GetActiveScreenMode();
}

void DisplayManagerServiceInner::RegisterDisplayChangeListener(sptr<IDisplayChangeListener> listener)
{
    DisplayManagerService::GetInstance().RegisterDisplayChangeListener(listener);
}
} // namespace OHOS::Rosen