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

#include "display_manager.h"
#include "display_manager_adapter.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DisplayManager"};
}

IMPLEMENT_SINGLE_INSTANCE(DisplayManager);

DisplayManager::DisplayManager()
{
    dmsAdapter_ = SingletonContainer::Get<DisplayManagerAdapter>();
}

DisplayManager::~DisplayManager()
{
}

const sptr<Display>& DisplayManager::GetDisplay(const DisplayType type)
{
    if (dmsAdapter_ == nullptr) {
        WLOGFE("DisplayManager::GetDisplay null!");
        return nullptr;
    }
    return dmsAdapter_->GetDisplay(type);
}

DisplayId DisplayManager::GetDefaultDisplayId()
{
    if (dmsAdapter_ == nullptr) {
        WLOGFE("DisplayManager::GetDefaultDisplayId null!\n");
        return DISPLAY_ID_INVALD;
    }
    return dmsAdapter_->GetDefaultDisplayId();
}

const sptr<Display> DisplayManager::GetDisplayById(DisplayId displayId)
{
    if (dmsAdapter_ == nullptr) {
        WLOGFE("DisplayManager::GetDisplayById null!\n");
        return nullptr;
    }

    sptr<Display> display = dmsAdapter_->GetDisplayById(displayId);
    if (display == nullptr) {
        WLOGFE("DisplayManager::GetDisplayById failed!\n");
        return nullptr;
    }
    return display;
}


const sptr<Display> DisplayManager::GetDefaultDisplay()
{
    return GetDisplayById(GetDefaultDisplayId());
}

std::vector<DisplayId> DisplayManager::GetAllDisplayIds()
{
    std::vector<DisplayId> res;
    res.push_back(GetDefaultDisplayId());
    // 获取所有displayIds
    return res;
}

std::vector<const sptr<Display>> DisplayManager::GetAllDisplays()
{
    std::vector<const sptr<Display>> res;
    auto displayIds = GetAllDisplayIds();
    for (auto displayId: displayIds) {
        const sptr<Display> display = GetDisplayById(displayId);
        if (display != nullptr) {
            res.push_back(display);
        } else {
            WLOGFE("DisplayManager::GetAllDisplays display %llu nullptr!", displayId);
        }
    }
    return res;
}
} // namespace OHOS::Rosen