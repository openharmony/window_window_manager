/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "window_manager_service_utils.h"
#include "display_manager_service_inner.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "WmsUtils"};
}

bool WmsUtils::IsFixedOrientation(Orientation orientation, WindowMode mode)
{
    if (mode != WindowMode::WINDOW_MODE_FULLSCREEN) {
        return false;
    }
    if (orientation > Orientation::REVERSE_HORIZONTAL) {
        return false;
    }
    if (orientation < Orientation::VERTICAL) {
        return false;
    }
    return true;
}

bool WmsUtils::IsExpectedRotateLandscapeWindow(Orientation requestOrientation,
    DisplayOrientation currentOrientation)
{
    if (requestOrientation != Orientation::HORIZONTAL && requestOrientation != Orientation::REVERSE_HORIZONTAL) {
        return false;
    }
    return IsExpectedRotatableWindow(requestOrientation, currentOrientation);
}

bool WmsUtils::IsExpectedRotatableWindow(Orientation requestOrientation,
    DisplayOrientation currentOrientation, WindowMode mode)
{
    if (mode != WindowMode::WINDOW_MODE_FULLSCREEN) {
        return false;
    }
    return IsExpectedRotatableWindow(requestOrientation, currentOrientation);
}

bool WmsUtils::IsExpectedRotatableWindow(Orientation requestOrientation,
    DisplayOrientation currentOrientation)
{
    if (WINDOW_TO_DISPLAY_ORIENTATION_MAP.count(requestOrientation) == 0) {
        return false;
    }
    DisplayOrientation disOrientation = WINDOW_TO_DISPLAY_ORIENTATION_MAP.at(requestOrientation);
    if (disOrientation != currentOrientation) {
        return true;
    }
    return false;
}

void WmsUtils::AdjustFixedOrientationRSSurfaceNode(const sptr<WindowNode>& node, const Rect& winRect,
    std::shared_ptr<RSSurfaceNode> surfaceNode)
{
    auto displayInfo = DisplayManagerServiceInner::GetInstance().GetDisplayById(node->GetDisplayId());
    WmsUtils::AdjustFixedOrientationRSSurfaceNode(node, winRect, surfaceNode, displayInfo);
}
void WmsUtils::AdjustFixedOrientationRSSurfaceNode(const sptr<WindowNode>& node, const Rect& winRect,
    std::shared_ptr<RSSurfaceNode> surfaceNode, sptr<DisplayInfo> displayInfo)
{
    if (!displayInfo) {
        WLOGFE("display invaild");
        return;
    }
    auto requestOrientation = node->GetRequestedOrientation();
    if (!IsFixedOrientation(requestOrientation, node->GetWindowMode())) {
        return;
    }

    auto displayOri = displayInfo->GetDisplayOrientation();
    auto displayW = displayInfo->GetWidth();
    auto displayH = displayInfo->GetHeight();
    if (WINDOW_TO_DISPLAY_ORIENTATION_MAP.count(requestOrientation) == 0) {
        return;
    }
    int32_t diffOrientation = static_cast<int32_t>(WINDOW_TO_DISPLAY_ORIENTATION_MAP.at(requestOrientation)) -
        static_cast<int32_t>(displayOri);
    float rotation = -90.f * (diffOrientation); // 90.f is base degree
    WLOGFD("[FixOrientation] adjust param display [%{public}u, %{public}d, %{public}d], rotation: %{public}f",
        displayOri, displayW, displayH, rotation);
    surfaceNode->SetTranslateX((displayW - static_cast<int32_t>(winRect.width_)) / 2); // 2 is half
    surfaceNode->SetTranslateY((displayH - static_cast<int32_t>(winRect.height_)) / 2); // 2 is half
    surfaceNode->SetPivotX(0.5); // 0.5 means center
    surfaceNode->SetPivotY(0.5); // 0.5 means center
    surfaceNode->SetRotation(rotation);
}
}
}