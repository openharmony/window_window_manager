/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "session_manager/include/window_focus_controller.h"

namespace OHOS {
namespace Rosen {

WSError FocusGroup::UpdateFocusedSessionId(int32_t persistentId)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "focusedId change: %{public}d -> %{public}d", focusedSessionId_, persistentId);
    if (focusedSessionId_ == persistentId) {
        TLOGD(WmsLogTag::WMS_FOCUS, "focus scene not change, id: %{public}d", focusedSessionId_);
        return WSError::WS_DO_NOTHING;
    }
    lastFocusedSessionId_ = focusedSessionId_;
    focusedSessionId_ = persistentId;
    return WSError::WS_OK;
}

WSError FocusGroup::UpdateFocusedAppSessionId(int32_t persistentId)
{
    lastFocusedAppSessionId_ = persistentId;
    return WSError::WS_OK;
}

WindowFocusController::WindowFocusController() noexcept
{
    TLOGI(WmsLogTag::WMS_FOCUS, "constructor");
    AddFocusGroup(DEFAULT_DISPLAY_ID, DEFAULT_DISPLAY_ID);
}

DisplayId WindowFocusController::GetDisplayGroupId(DisplayId displayId)
{
    if (displayId == DEFAULT_DISPLAY_ID) {
        return DEFAULT_DISPLAY_ID;
    }
    auto iter = displayId2GroupIdMap_.find(displayId);
    if (iter != displayId2GroupIdMap_.end()) {
        TLOGD(WmsLogTag::WMS_FOCUS, "displayId: %{public}" PRIu64, "displayGroupId: %{public}" PRIu64,
              displayId, iter->second);
        return iter->second;
    }
    if (deletedDisplayId2GroupIdMap_.find(displayId) != deletedDisplayId2GroupIdMap_.end()) {
        return DISPLAY_ID_INVALID;
    } else {
        return DEFAULT_DISPLAY_ID;
    }
}

WSError WindowFocusController::AddFocusGroup(DisplayGroupId displayGroupId, DisplayId displayId)
{
    TLOGI(WmsLogTag::WMS_FOCUS,
          "displayId: %{public}" PRIu64,
          " ,displayGroupId: %{public}" PRIu64,
          displayId,
          displayGroupId);
    if (displayId == DISPLAY_ID_INVALID) {
        TLOGE(WmsLogTag::WMS_FOCUS, "displayId invalid");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    displayId2GroupIdMap_[displayId] = displayGroupId;
    auto iter = focusGroupMap_.find(displayGroupId);
    if (iter == focusGroupMap_.end()) {
        sptr<FocusGroup> focusGroup = sptr<FocusGroup>::MakeSptr(displayGroupId);
        focusGroup->displayIds_.insert(displayId);
        focusGroupMap_.insert(std::make_pair(displayGroupId, focusGroup));
        LogDisplayIds();
    } else {
        iter->second->displayIds_.insert(displayId);
        LogDisplayIds();
    }
    if (deletedDisplayId2GroupIdMap_.find(displayId) != deletedDisplayId2GroupIdMap_.end()) {
        deletedDisplayId2GroupIdMap_.erase(displayId);
    }
    return WSError::WS_OK;
}

WSError WindowFocusController::RemoveFocusGroup(DisplayGroupId displayGroupId, DisplayId displayId)
{
    TLOGI(WmsLogTag::WMS_FOCUS,
          "displayId: %{public}" PRIu64,
          " ,displayGroupId: %{public}" PRIu64,
          displayId,
          displayGroupId);
    if (displayId == DISPLAY_ID_INVALID) {
        TLOGE(WmsLogTag::WMS_FOCUS, "displayId invalid");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    auto iter = focusGroupMap_.find(displayGroupId);
    if (iter != focusGroupMap_.end()) {
        auto displayIds = iter->second->displayIds_;
        displayIds.erase(displayId);
        if (displayIds.size() == 0) {
            focusGroupMap_.erase(displayGroupId);
        }
    } else {
        TLOGE(WmsLogTag::WMS_FOCUS, "displayGroupId invalid, displayGroupId: %{public}" PRIu64, displayGroupId);
    }
    displayId2GroupIdMap_.erase(displayId);
    deletedDisplayId2GroupIdMap_[displayId] = displayGroupId;
    LogDisplayIds();
    return WSError::WS_OK;
}

sptr<FocusGroup> WindowFocusController::GetFocusGroupInner(DisplayId displayId)
{
    DisplayId displayGroupId = GetDisplayGroupId(displayId);
    if (displayGroupId == DEFAULT_DISPLAY_ID) {
        return focusGroupMap_[DEFAULT_DISPLAY_ID];
    }
    auto iter = focusGroupMap_.find(displayGroupId);
    if (iter == focusGroupMap_.end()) {
        TLOGE(WmsLogTag::WMS_FOCUS, "Not found focus group with displayId: %{public}" PRIu64, displayId);
        return nullptr;
    }
    return iter->second;
}

int32_t WindowFocusController::GetFocusedSessionId(DisplayId displayId)
{
    if (displayId == DISPLAY_ID_INVALID) {
        TLOGE(WmsLogTag::WMS_FOCUS, "displayId invalid");
        return INVALID_SESSION_ID;
    }
    auto focusGroup = GetFocusGroupInner(displayId);
    if (focusGroup == nullptr) {
        TLOGE(WmsLogTag::WMS_FOCUS, "focus group is null, displayId: %{public}" PRIu64, displayId);
        return INVALID_SESSION_ID;
    }
    return focusGroup->GetFocusedSessionId();
}

sptr<FocusGroup> WindowFocusController::GetFocusGroup(DisplayId displayId)
{
    if (displayId == DISPLAY_ID_INVALID) {
        TLOGE(WmsLogTag::WMS_FOCUS, "displayId invalid");
        return nullptr;
    }
    return GetFocusGroupInner(displayId);
}

std::vector<std::pair<DisplayId, int32_t>> WindowFocusController::GetAllFocusedSessionList() const
{
    std::vector<std::pair<DisplayId, int32_t>> allFocusGroup;
    for (const auto& elem : focusGroupMap_) {
        auto curFocusGroup = elem.second;
        if (curFocusGroup == nullptr) {
            TLOGE(WmsLogTag::WMS_FOCUS, "focus group is null");
            continue;
        }
        allFocusGroup.emplace_back(std::make_pair(elem.first, curFocusGroup->GetFocusedSessionId()));
    }
    return allFocusGroup;
}

WSError WindowFocusController::UpdateFocusedSessionId(DisplayId displayId, int32_t persistentId)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "displayId: %{public}" PRIu64 ", persistentId: %{public}d", displayId, persistentId);
    if (displayId == DISPLAY_ID_INVALID) {
        TLOGE(WmsLogTag::WMS_FOCUS, "displayId invalid");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    auto focusGroup = GetFocusGroupInner(displayId);
    if (focusGroup == nullptr) {
        TLOGE(WmsLogTag::WMS_FOCUS, "focus group is null, displayId: %{public}" PRIu64, displayId);
        return WSError::WS_ERROR_NULLPTR;
    }
    return focusGroup->UpdateFocusedSessionId(persistentId);
}

WSError WindowFocusController::UpdateFocusedAppSessionId(DisplayId displayId, int32_t persistentId)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "displayId: %{public}" PRIu64 ", persistentId: %{public}d", displayId, persistentId);
    if (displayId == DISPLAY_ID_INVALID) {
        TLOGE(WmsLogTag::WMS_FOCUS, "displayId invalid");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    auto focusGroup = GetFocusGroupInner(displayId);
    if (focusGroup == nullptr) {
        TLOGE(WmsLogTag::WMS_FOCUS, "focus group is null, displayId: %{public}" PRIu64, displayId);
        return WSError::WS_ERROR_NULLPTR;
    }
    return focusGroup->UpdateFocusedAppSessionId(persistentId);
}

void WindowFocusController::LogDisplayIds()
{
    std::ostringstream oss;
    {
        for (auto it = focusGroupMap_.begin(); it != focusGroupMap_.end(); it++) {
            oss << "focusGroupId: " << it->first << ", displayids:";
            auto displayIds = it->second->displayIds_;
            for (auto it2 = displayIds.begin(); it2 != displayIds.end(); it2++) {
                oss << *it2;
                if (std::next(it2) != displayIds.end()) {
                    oss << ",";
                } else {
                    oss << ";";
                }
            }
        }
        for (auto it = displayId2GroupIdMap_.begin(); it != displayId2GroupIdMap_.end(); it++) {
            oss << "displayId2GroupIdMap:" << it->first << "-" << it->second;
            if (std::next(it) != displayId2GroupIdMap_.end()) {
                oss << ", ";
            }
        }
    }
    TLOGI(WmsLogTag::WMS_FOCUS, "%{public}s", oss.str().c_str());
}
} // namespace Rosen
} // namespace OHOS
