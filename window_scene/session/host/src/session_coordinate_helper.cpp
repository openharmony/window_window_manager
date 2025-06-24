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
WSRect SessionCoordinateHelper::RelativeToGlobalDisplayRect(DisplayId displayId, const WSRect& relativeRect)
{
    auto screenSession = ScreenSessionManagerClient::GetInstance().GetScreenSession(displayId);
    if (!screenSession) {
        TLOGW(WmsLogTag::WMS_LAYOUT,
            "Screen session not found, displayId: %{public}" PRIu64 ", relativeRect: %{public}s",
            displayId, relativeRect.ToString().c_str());
        return relativeRect;
    }
    const auto& screenProperty = screenSession->GetScreenProperty();
    WSRect globalRect {
        screenProperty.GetX() + relativeRect.posX_,
        screenProperty.GetY() + relativeRect.posY_,
        relativeRect.width_,
        relativeRect.height_
    };
    TLOGD(WmsLogTag::WMS_LAYOUT, "displayId: %{public}" PRIu64 ", relativeRect: %{public}s, globalRect: %{public}s",
        displayId, relativeRect.ToString().c_str(), globalRect.ToString().c_str());
    return globalRect;
}

WSRelativeDisplayRect SessionCoordinateHelper::GlobalToRelativeDisplayRect(
    DisplayId originalDisplayId, const WSRect& globalRect)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "originalDisplayId: %{public}" PRIu64 ", globalRect: %{public}s",
        originalDisplayId, globalRect.ToString().c_str());
    auto screenRectMap = BuildScreenRectMap();
    // Assign to display with largest intersection area, and smallest displayId.
    if (auto match = MatchBestIntersectionScreen(screenRectMap, globalRect)) {
        return *match;
    }
    // If no intersection with any display, fallback to original or primary display.
    return FallbackToOriginalOrPrimaryScreen(screenRectMap, globalRect, originalDisplayId);
}

WSRect SessionCoordinateHelper::ToRelative(const WSRect& globalRect, const WSRect& screenRect)
{
    return {
        globalRect.posX_ - screenRect.posX_,
        globalRect.posY_ - screenRect.posY_,
        globalRect.width_, globalRect.height_
    };
}

std::unordered_map<uint64_t, WSRect> SessionCoordinateHelper::BuildScreenRectMap()
{
    std::unordered_map<uint64_t, WSRect> screenRectMap;
    auto screenPropertyMap = ScreenSessionManagerClient::GetInstance().GetAllScreensProperties();
    for (const auto& [screenId, screenProperty] : screenPropertyMap) {
        WSRect screenRect {
            screenProperty.GetX(),
            screenProperty.GetY(),
            screenProperty.GetBounds().rect_.GetWidth(),
            screenProperty.GetBounds().rect_.GetHeight()
        };
        screenRectMap[screenId] = screenRect;

        TLOGD(WmsLogTag::WMS_LAYOUT, "screenId: %{public}" PRIu64 ", screenRect: %{public}s",
            screenId, screenRect.ToString().c_str());
    }
    return screenRectMap;
}

std::optional<WSRelativeDisplayRect> SessionCoordinateHelper::MatchBestIntersectionScreen(
    const std::unordered_map<uint64_t, WSRect>& screenRectMap, const WSRect& globalRect)
{
    uint64_t bestDisplayId = DISPLAY_ID_INVALID;
    uint64_t maxArea = 0;
    for (const auto& [displayId, screenRect] : screenRectMap) {
        uint64_t area = globalRect.IntersectionArea<uint64_t>(screenRect);
        if (area > maxArea || (area == maxArea && displayId < bestDisplayId)) {
            maxArea = area;
            bestDisplayId = displayId;
        }
    }
    if (maxArea > 0 && bestDisplayId != DISPLAY_ID_INVALID) {
        auto relativeRect = ToRelative(globalRect, screenRectMap.at(bestDisplayId));
        TLOGD(WmsLogTag::WMS_LAYOUT, "Best display %{public}" PRIu64 " with area %{public}" PRIu64
            ", globalRect: %{public}s, relativeRect: %{public}s",
            bestDisplayId, maxArea, globalRect.ToString().c_str(), relativeRect.ToString().c_str());
        return WSRelativeDisplayRect { bestDisplayId, relativeRect };
    }
    return std::nullopt;
}

WSRelativeDisplayRect SessionCoordinateHelper::FallbackToOriginalOrPrimaryScreen(
    const std::unordered_map<uint64_t, WSRect>& screenRectMap, const WSRect& globalRect, DisplayId originalDisplayId)
{
    constexpr uint64_t mainDisplayId = MAIN_SCREEN_ID_DEFAULT;
    const auto it = screenRectMap.find(originalDisplayId);
    if (it == screenRectMap.end()) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "Fallback to mainDisplayId: %{public}" PRIu64 ", globalRect: %{public}s",
            mainDisplayId, globalRect.ToString().c_str());
        return { mainDisplayId, globalRect };
    }
    auto relativeRect = ToRelative(globalRect, it->second);
    TLOGD(WmsLogTag::WMS_LAYOUT, "Fallback to originalDisplayId: %{public}" PRIu64
        ", globalRect: %{public}s, relativeRect: %{public}s",
        originalDisplayId, globalRect.ToString().c_str(), relativeRect.ToString().c_str());
    return { originalDisplayId, relativeRect };
}
} // namespace OHOS::Rosen
