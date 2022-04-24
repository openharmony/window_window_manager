/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "window_node.h"
#include "window_helper.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WindowNode"};
}

void WindowNode::SetDisplayId(DisplayId displayId)
{
    property_->SetDisplayId(displayId);
}

void WindowNode::SetHotZoneRect(const Rect& rect)
{
    hotZoneRect_ = rect;
}

void WindowNode::SetWindowRect(const Rect& rect)
{
    property_->SetWindowRect(rect);
}

void WindowNode::SetDecoStatus(bool status)
{
    property_->SetDecoStatus(status);
}

void WindowNode::SetRequestRect(const Rect& rect)
{
    property_->SetRequestRect(rect);
}

void WindowNode::SetWindowProperty(const sptr<WindowProperty>& property)
{
    property_ = property;
}

void WindowNode::SetSystemBarProperty(WindowType type, const SystemBarProperty& property)
{
    property_->SetSystemBarProperty(type, property);
}

void WindowNode::SetWindowMode(WindowMode mode)
{
    property_->SetWindowMode(mode);
}

void WindowNode::SetWindowBackgroundBlur(WindowBlurLevel level)
{
    float blurRadiusX;
    float blurRadiusY;
    const float OFF_BLUR_RADIUS = 0.0f;
    const float LOW_BLUR_RADIUS = 3.0f;
    const float MEDIUM_BLUR_RADIUS = 11.0f;
    const float HIGH_BLUR_RADIUS = 19.0f;

    switch (level) {
        case WindowBlurLevel::WINDOW_BLUR_LOW:
            blurRadiusX = LOW_BLUR_RADIUS;
            blurRadiusY = LOW_BLUR_RADIUS;
            break;
        case WindowBlurLevel::WINDOW_BLUR_MEDIUM:
            blurRadiusX = MEDIUM_BLUR_RADIUS;
            blurRadiusY = MEDIUM_BLUR_RADIUS;
            break;
        case WindowBlurLevel::WINDOW_BLUR_HIGH:
            blurRadiusX = HIGH_BLUR_RADIUS;
            blurRadiusY = HIGH_BLUR_RADIUS;
            break;
        default:
            blurRadiusX = OFF_BLUR_RADIUS;
            blurRadiusY = OFF_BLUR_RADIUS;
            break;
    }
    property_->SetWindowBackgroundBlur(level);
    WLOGFI("WindowEffect WindowNode Setblur X:%{public}f  Y:%{public}f!", blurRadiusX, blurRadiusY);
    surfaceNode_->SetBackgroundFilter(RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY));
}

void WindowNode::SetAlpha(float alpha)
{
    property_->SetAlpha(alpha);
    WLOGFI("WindowEffect WinodwNode SetAlpha alpha:%{public}f", alpha);
    surfaceNode_->SetAlpha(alpha);
}

void WindowNode::SetBrightness(float brightness)
{
    property_->SetBrightness(brightness);
}

void WindowNode::SetFocusable(bool focusable)
{
    property_->SetFocusable(focusable);
}

void WindowNode::SetTouchable(bool touchable)
{
    property_->SetTouchable(touchable);
}

void WindowNode::SetTurnScreenOn(bool turnScreenOn)
{
    property_->SetTurnScreenOn(turnScreenOn);
}

void WindowNode::SetKeepScreenOn(bool keepScreenOn)
{
    property_->SetKeepScreenOn(keepScreenOn);
}

void WindowNode::SetCallingWindow(uint32_t windowId)
{
    property_->SetCallingWindow(windowId);
}

uint32_t WindowNode::GetCallingWindow() const
{
    return property_->GetCallingWindow();
}

void WindowNode::SetWindowSizeChangeReason(WindowSizeChangeReason reason)
{
    windowSizeChangeReason_ = reason;
}

void WindowNode::SetRequestedOrientation(Orientation orientation)
{
    property_->SetRequestedOrientation(orientation);
}

void WindowNode::SetShowingDisplays(const std::vector<DisplayId>& displayIdVec)
{
    showingDisplays_.clear();
    showingDisplays_.assign(displayIdVec.begin(), displayIdVec.end());
}

void WindowNode::ResetWindowSizeChangeReason()
{
    windowSizeChangeReason_ = WindowSizeChangeReason::UNDEFINED;
}

const sptr<IWindow>& WindowNode::GetWindowToken() const
{
    return windowToken_;
}

void WindowNode::SetWindowToken(sptr<IWindow> window)
{
    windowToken_ = window;
}

void WindowNode::SetCallingPid()
{
    callingPid_ = IPCSkeleton::GetCallingPid();
}

void WindowNode::SetCallingUid()
{
    callingUid_ = IPCSkeleton::GetCallingUid();
}

DisplayId WindowNode::GetDisplayId() const
{
    return property_->GetDisplayId();
}

const std::string& WindowNode::GetWindowName() const
{
    return property_->GetWindowName();
}

uint32_t WindowNode::GetWindowId() const
{
    return property_->GetWindowId();
}

uint32_t WindowNode::GetParentId() const
{
    return property_->GetParentId();
}

Rect WindowNode::GetHotZoneRect() const
{
    return hotZoneRect_;
}

Rect WindowNode::GetWindowRect() const
{
    return property_->GetWindowRect();
}

bool WindowNode::GetDecoStatus() const
{
    return property_->GetDecoStatus();
}

Rect WindowNode::GetRequestRect() const
{
    return property_->GetRequestRect();
}

WindowType WindowNode::GetWindowType() const
{
    return property_->GetWindowType();
}

WindowMode WindowNode::GetWindowMode() const
{
    return property_->GetWindowMode();
}

WindowBlurLevel WindowNode::GetWindowBackgroundBlur() const
{
    return property_->GetWindowBackgroundBlur();
}

float WindowNode::GetAlpha() const
{
    return property_->GetAlpha();
}

float WindowNode::GetBrightness() const
{
    return property_->GetBrightness();
}

bool WindowNode::IsTurnScreenOn() const
{
    return property_->IsTurnScreenOn();
}

bool WindowNode::IsKeepScreenOn() const
{
    return property_->IsKeepScreenOn();
}

uint32_t WindowNode::GetWindowFlags() const
{
    return property_->GetWindowFlags();
}

const sptr<WindowProperty>& WindowNode::GetWindowProperty() const
{
    return property_;
}

int32_t WindowNode::GetCallingPid() const
{
    return callingPid_;
}

int32_t WindowNode::GetCallingUid() const
{
    return callingUid_;
}

const std::unordered_map<WindowType, SystemBarProperty>& WindowNode::GetSystemBarProperty() const
{
    return property_->GetSystemBarProperty();
}

bool WindowNode::IsSplitMode() const
{
    return (property_->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_PRIMARY ||
        property_->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
}

WindowSizeChangeReason WindowNode::GetWindowSizeChangeReason() const
{
    return windowSizeChangeReason_;
}

Orientation WindowNode::GetRequestedOrientation() const
{
    return property_->GetRequestedOrientation();
}

std::vector<DisplayId> WindowNode::GetShowingDisplays() const
{
    return showingDisplays_;
}
} // namespace Rosen
} // namespace OHOS
