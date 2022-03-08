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

void WindowNode::SetLayoutRect(const Rect& rect)
{
    layoutRect_ = rect;
}

void WindowNode::SetHotZoneRect(const Rect& rect)
{
    hotZoneRect_ = rect;
}

void WindowNode::SetWindowRect(const Rect& rect)
{
    property_->SetWindowRect(rect);
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

void WindowNode::SetWindowSizeChangeReason(WindowSizeChangeReason reason)
{
    windowSizeChangeReason_ = reason;
}

const sptr<IWindow>& WindowNode::GetWindowToken() const
{
    return windowToken_;
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

const Rect& WindowNode::GetLayoutRect() const
{
    return layoutRect_;
}

Rect WindowNode::GetHotZoneRect() const
{
    return hotZoneRect_;
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
} // namespace Rosen
} // namespace OHOS
