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

#include "../include/window_focus_controller.h"

namespace OHOS {
namespace Rosen {

WindowFocusController::WindowFocusController() noexcept
{
    TLOGI(WmsLogTag::WMS_FOCUS, "constructor");
    AddFocusGroup(DEFAULT_DISPLAY_ID);
}

DisplayId WindowFocusController::GetDisplayGroupId(DisplayId displayId)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "displayId: %{public}" PRIu64, displayId);
    if (displayId == DEFAULT_DISPLAY_ID || virtualScreenDisplayIdSet_.size() == 0) {
        return DEFAULT_DISPLAY_ID;
    }
    if (virtualScreenDisplayIdSet_.find(displayId) != virtualScreenDisplayIdSet_.end()) {
        return displayId;
    }
    return DEFAULT_DISPLAY_ID;
}

WSError WindowFocusController::AddFocusGroup(DisplayId displayId)
{
    TLOGI(WmsLogTag::WMS_FOCUS, "displayId: %{public}" PRIu64, displayId);
    if (displayId == DISPLAY_ID_INVALID) {
        TLOGE(WmsLogTag::WMS_FOCUS, "displayId id invalid");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    std::shared_lock<std::shared_mutex> lock(focusGroupMutex_);
    sptr<FocusGroup> focusGroup = sptr<FocusGroup>::MakeSptr(displayId);
    focusGroupMap_.insert(std::make_pair(displayId, focusGroup));
    if (displayId != DEFAULT_DISPLAY_ID) {
        virtualScreenDisplayIdSet_.emplace(displayId);
    }
    return  WSError::WS_OK;
}

WSError WindowFocusController::RemoveFocusGroup(DisplayId displayId)
{
    TLOGI(WmsLogTag::WMS_FOCUS, "displayId: %{public}" PRIu64, displayId);
    if (displayId == DISPLAY_ID_INVALID) {
        TLOGE(WmsLogTag::WMS_FOCUS, "displayId id invalid");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    std::shared_lock<std::shared_mutex> lock(focusGroupMutex_);
    focusGroupMap_.erase(displayId);
    if (displayId != DEFAULT_DISPLAY_ID) {
        virtualScreenDisplayIdSet_.erase(displayId);
    }
    return  WSError::WS_OK;
}

sptr<FocusGroup> WindowFocusController::GetFocusGroupInner(DisplayId displayId)
{
    DisplayId displayGroupId = GetDisplayGroupId(displayId);
    TLOGD(WmsLogTag::WMS_FOCUS, "displayId: %{public}" PRIu64 ", displayGroupId: %{public}" PRIu64,
          displayId, displayGroupId);
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
    TLOGD(WmsLogTag::WMS_FOCUS, "displayId: %{public}" PRIu64, displayId);
    if (displayId == DISPLAY_ID_INVALID) {
        TLOGE(WmsLogTag::WMS_FOCUS, "displayId id invalid");
        return INVALID_SESSION_ID;
    }
    std::shared_lock<std::shared_mutex> lock(focusGroupMutex_);
    auto focusGroup = GetFocusGroupInner(displayId);
    if (focusGroup == nullptr) {
        TLOGE(WmsLogTag::WMS_FOCUS, "focus group is null, displayId: %{public}" PRIu64, displayId);
        return INVALID_SESSION_ID;
    }
    return focusGroup->GetFocusedSessionId();
}

sptr<FocusGroup> WindowFocusController::GetFocusGroup(DisplayId displayId)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "displayId: %{public}" PRIu64, displayId);
    if (displayId == DISPLAY_ID_INVALID) {
        TLOGE(WmsLogTag::WMS_FOCUS, "displayId id invalid");
        return nullptr;
    }
    std::shared_lock<std::shared_mutex> lock(focusGroupMutex_);
    auto focusGroup = GetFocusGroupInner(displayId);
    return focusGroup;
}

std::vector<std::pair<DisplayId, int32_t>> WindowFocusController::GetAllFocusedSessionList()
{
    TLOGI(WmsLogTag::WMS_FOCUS, "in");
    std::unordered_map<DisplayId, sptr<FocusGroup>> focusGroupMapCopy;
    {
        std::shared_lock<std::shared_mutex> lock(focusGroupMutex_);
        focusGroupMapCopy = focusGroupMap_;
    }
    std::vector<std::pair<DisplayId, int32_t>> allFocusGroup;
    for (const auto& elem : focusGroupMapCopy) {
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
        TLOGE(WmsLogTag::WMS_FOCUS, "displayId id invalid");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    std::unique_lock<std::shared_mutex> lock(focusGroupMutex_);
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
        TLOGE(WmsLogTag::WMS_FOCUS, "displayId id invalid");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    std::unique_lock<std::shared_mutex> lock(focusGroupMutex_);
    auto focusGroup = GetFocusGroupInner(displayId);
    if (focusGroup == nullptr) {
        TLOGE(WmsLogTag::WMS_FOCUS, "focus group is null, displayId: %{public}" PRIu64, displayId);
        return WSError::WS_ERROR_NULLPTR;
    }
    return focusGroup->UpdateFocusedAppSessionId(persistentId);
}
}
}
