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

#include "dm_common.h"
#include "screen_session_manager_client/include/screen_session_manager_client.h"
#include "window_manager_hilog.h"
#include "wm_math.h"

namespace OHOS::Rosen {
WSRect SessionCoordinateHelper::RelativeToGlobalDisplayRect(ScreenId screenId, const WSRect& relativeRect)
{
    auto screenSession = ScreenSessionManagerClient::GetInstance().GetScreenSession(screenId);
    if (!screenSession) {
        TLOGD(WmsLogTag::WMS_LAYOUT,
            "Screen not found, screenId: %{public}" PRIu64 ", relativeRect: %{public}s",
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

WSScreenRelativeRect SessionCoordinateHelper::GlobalToScreenRelativeRect(
    ScreenId originalScreenId, const WSRect& globalRect)
{
    auto originalScreen = ScreenSessionManagerClient::GetInstance().GetScreenSession(originalScreenId);
    if (!originalScreen) {
        return { MAIN_SCREEN_ID_DEFAULT, globalRect };
    }

    const float originalScreenVpr = originalScreen->GetScreenProperty().GetVirtualPixelRatio();
    if (MathHelper::NearZero(originalScreenVpr)) {
        return { MAIN_SCREEN_ID_DEFAULT, globalRect };
    }

    // Convert globalRect's size (width/height) to virtual pixel units using original screen's VPR.
    // Keep posX/posY unchanged, only adjust width and height to match how large it would appear on different screens.
    const float widthInVp = globalRect.width_ / originalScreenVpr;
    const float heightInVp = globalRect.height_ / originalScreenVpr;

    // Iterate all screens and find the one that has the largest intersection area with the scaled rect.
    // For comparison, convert the width and height back into actual pixels using each screen's VPR.
    ScreenId candidateScreenId = SCREEN_ID_INVALID;
    WSRect candidateScreenRect = WSRect::EMPTY_RECT;
    WSRect originalScreenRect = WSRect::EMPTY_RECT;
    uint64_t maxIntersectionArea = 0;
    const auto screenPropMap = ScreenSessionManagerClient::GetInstance().GetAllScreensProperties();
    for (const auto& [screenId, screenProp] : screenPropMap) {
        const float screenVpr = screenProp.GetVirtualPixelRatio();
        WSRect screenRect {
            screenProp.GetX(), screenProp.GetY(),
            screenProp.GetBounds().rect_.GetWidth(),
            screenProp.GetBounds().rect_.GetHeight()
        };
        if (screenId == originalScreenId) {
            originalScreenRect = screenRect;
        }

        // Scale globalRect’s width/height to match how it would look on this screen.
        WSRect scaledGlobalRect {
            globalRect.posX_, globalRect.posY_,
            static_cast<int32_t>(std::round(widthInVp * screenVpr)),
            static_cast<int32_t>(std::round(heightInVp * screenVpr))
        };

        // Compute intersection area between this screen and the scaled globalRect.
        uint64_t area = scaledGlobalRect.IntersectionArea<uint64_t>(screenRect);
        if (area > maxIntersectionArea || (area != 0 && area == maxIntersectionArea && screenId < candidateScreenId)) {
            maxIntersectionArea = area;
            candidateScreenId = screenId;
            candidateScreenRect = screenRect;
        }
    }

    // Determine the final matched screen and compute the relative rect to it.
    bool hasCandidate = candidateScreenId != SCREEN_ID_INVALID;
    const auto matchedScreenId = hasCandidate ? candidateScreenId : originalScreenId;
    const auto& matchedScreenRect = hasCandidate ? candidateScreenRect : originalScreenRect;
    const WSRect relativeRect {
        globalRect.posX_ - matchedScreenRect.posX_,
        globalRect.posY_ - matchedScreenRect.posY_,
        globalRect.width_, globalRect.height_
    };
    TLOGD(WmsLogTag::WMS_LAYOUT, "matchedScreenId: %{public}" PRIu64 ", relativeRect: %{public}s",
        matchedScreenId, relativeRect.ToString().c_str());
    return { matchedScreenId, relativeRect };
}
} // namespace OHOS::Rosen
