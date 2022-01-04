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
#include <inttypes.h>
#include <unistd.h>

#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "display_screen_manager.h"
#include "display_manager_service.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DisplayManagerServiceInner"};
}

IMPLEMENT_SINGLE_INSTANCE(DisplayManagerServiceInner);

DisplayId DisplayManagerServiceInner::GetDefaultDisplayId()
{
    auto dms = DisplayManagerService::GetInstance();
    if (dms == nullptr) {
        WLOGFE("GetDefaultDisplayId null!\n");
        return DISPLAY_ID_INVALD;
    }
    return dms->GetDefaultDisplayId();
}

const sptr<DisplayScreen> DisplayManagerServiceInner::GetDisplayById(DisplayId displayId)
{
    auto dms = DisplayManagerService::GetInstance();
    if (dms == nullptr) {
        WLOGFE("GetDisplayById null!\n");
        return nullptr;
    }
    DisplayInfo displayInfo = dms->GetDisplayInfoById(displayId);

    sptr<DisplayScreen> display = new DisplayScreen(displayInfo);
    if (display == nullptr) {
        WLOGFE("GetDisplayById failed!\n");
        return nullptr;
    }
    return display;
}

const sptr<DisplayScreen> DisplayManagerServiceInner::GetDefaultDisplay()
{
    return GetDisplayById(GetDefaultDisplayId());
}

std::vector<DisplayId> DisplayManagerServiceInner::GetAllDisplayIds()
{
    std::vector<DisplayId> res;
    res.push_back(GetDefaultDisplayId());
    return res;
}

std::vector<const sptr<DisplayScreen>> DisplayManagerServiceInner::GetAllDisplays()
{
    std::vector<const sptr<DisplayScreen>> res;
    auto displayIds = GetAllDisplayIds();
    for (auto displayId: displayIds) {
        const sptr<DisplayScreen> display = GetDisplayById(displayId);
        if (display != nullptr) {
            res.push_back(display);
        } else {
            WLOGFE("GetAllDisplays display %" PRIu64" nullptr!", displayId);
        }
    }
    return res;
}
} // namespace OHOS::Rosen