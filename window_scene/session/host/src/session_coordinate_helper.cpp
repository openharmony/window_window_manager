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

#include "session_coordinate_helper.h"

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
WSRect SessionCoordinateHelper::RelativeToGlobalDisplayRect(ScreenId screenId, const WSRect& relativeRect)
{
    auto screenSession = ScreenSessionManagerClient::GetInstance().GetScreenSession(screenId);
    if (!screenSession) {
        TLOGW(WmsLogTag::WMS_LAYOUT,
            "Screen session not found, screenId: %{public}" PRIu64 ", relativeRect: %{public}s",
            screenId, relativeRect.ToString().c_str());
        return relativeRect;
    }
    const auto& screenProperty = screenSession->GetScreenProperty();
    WSRect globalRect {
        screenProperty.GetX() + relativeRect.posX_,
        screenProperty.GetY() + relativeRect.posY_,
        relativeRect.width_,
        relativeRect.height_
    };
    TLOGD(WmsLogTag::WMS_LAYOUT, "screenId: %{public}" PRIu64 ", relativeRect: %{public}s, globalRect: %{public}s",
        screenId, relativeRect.ToString().c_str(), globalRect.ToString().c_str());
    return globalRect;
}

WSRelativeDisplayRect SessionCoordinateHelper::GlobalToRelativeDisplayRect(
    ScreenId originalScreenId, const WSRect& globalRect)
{
    auto originalScreen = ScreenSessionManagerClient::GetInstance().GetScreenSession(originalScreenId);
    if (!originalScreen) {
        TLOGW(WmsLogTag::WMS_LAYOUT,
            "Original screen not found, screenId: %{public}" PRIu64 ", globalRect: %{public}s.",
            originalScreenId, globalRect.ToString().c_str());
        return { MAIN_SCREEN_ID_DEFAULT, globalRect };
    }

    // Convert globalRect to virtual pixel size
    const float originalScreenVpr = originalScreen->GetScreenProperty().GetVirtualPixelRatio();
    const float widthInVp = globalRect.width_ / originalScreenVpr;
    const float heightInVp = globalRect.height_ / originalScreenVpr;

    // Iterate all screens and find the best matched screen by intersection area
    ScreenId matchedScreenId = SCREEN_ID_INVALID;
    WSRect matchedScreenRect = WSRect::EMPTY_RECT;
    WSRect originalScreenRect = WSRect::EMPTY_RECT;
    uint64_t maxIntersectionArea = 0;
    const auto screenPropMap = ScreenSessionManagerClient::GetInstance().GetAllScreensProperties();
    for (const auto& [screenId, screenProp] : screenPropMap) {
        const float screenVpr = screenProp.GetVirtualPixelRatio();
        WSRect screenRect {
            screenProp.GetX(),
            screenProp.GetY(),
            screenProp.GetBounds().rect_.GetWidth(),
            screenProp.GetBounds().rect_.GetHeight()
        };
        if (screenId == originalScreenId) {
            originalScreenRect = screenRect;
        }
        // Scale globalRect to the current screen's virtual pixel ratio
        WSRect scaledGlobalRect {
            globalRect.posX_,
            globalRect.posY_,
            static_cast<int32_t>(widthInVp * screenVpr),
            static_cast<int32_t>(heightInVp * screenVpr)
        };
        uint64_t area = scaledGlobalRect.IntersectionArea<uint64_t>(screenRect);
        if (area > maxIntersectionArea || (area == maxIntersectionArea && screenId < matchedScreenId)) {
            matchedScreenId = screenId;
            maxIntersectionArea = area;
            matchedScreenRect = screenRect;
        }
    }

    // Determine final screenId and relativeRect
    const auto finalScreenId = (matchedScreenId != SCREEN_ID_INVALID) ? matchedScreenId : originalScreenId;
    const auto& finalScreenRect = (matchedScreenId != SCREEN_ID_INVALID) ? matchedScreenRect : originalScreenRect;
    const WSRect relativeRect {
        globalRect.posX_ - finalScreenRect.posX_,
        globalRect.posY_ - finalScreenRect.posY_,
        globalRect.width_, globalRect.height_
    };
    TLOGD(WmsLogTag::WMS_LAYOUT,
        "matchedScreenId: %{public}" PRIu64 ", intersectionArea: %{public}" PRIu64
        ", globalRect: %{public}s -> relativeRect: %{public}s",
        finalScreenId, maxIntersectionArea, globalRect.ToString().c_str(), relativeRect.ToString().c_str());
    return WSRelativeDisplayRect{ finalScreenId, relativeRect };
}
} // namespace OHOS::Rosen
