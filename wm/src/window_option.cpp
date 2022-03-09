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

#include "window_option.h"
#include "window_helper.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
WindowOption::WindowOption(): windowTag_(WindowTag::SYSTEM_WINDOW)
{
    AddWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
}

void WindowOption::SetWindowRect(const struct Rect& rect)
{
    windowRect_ = rect;
}

void WindowOption::SetWindowType(WindowType type)
{
    type_ = type;
}

void WindowOption::SetWindowMode(WindowMode mode)
{
    if (!WindowHelper::IsValidWindowMode(mode)) {
        return;
    }
    mode_ = mode;
}

void WindowOption::SetWindowBackgroundBlur(WindowBlurLevel level)
{
    if (!WindowHelper::IsValidWindowBlurLevel(level)) {
        return;
    }
    level_ = level;
}

void WindowOption::SetAlpha(float alpha)
{
    alpha_ = alpha;
}

void WindowOption::SetFocusable(bool isFocusable)
{
    focusable_ = isFocusable;
}

void WindowOption::SetTouchable(bool isTouchable)
{
    touchable_ = isTouchable;
}

void WindowOption::SetDisplayId(DisplayId displayId)
{
    displayId_ = displayId;
}

void WindowOption::SetParentName(const std::string& parentName)
{
    parentName_ = parentName;
}

void WindowOption::SetWindowName(const std::string& windowName)
{
    windowName_ = windowName;
}

void WindowOption::AddWindowFlag(WindowFlag flag)
{
    flags_ |= static_cast<uint32_t>(flag);
}

void WindowOption::RemoveWindowFlag(WindowFlag flag)
{
    flags_ &= ~(static_cast<uint32_t>(flag));
}

void WindowOption::SetWindowFlags(uint32_t flags)
{
    flags_ = flags;
}

void WindowOption::SetSystemBarProperty(WindowType type, const SystemBarProperty& property)
{
    if (type == WindowType::WINDOW_TYPE_STATUS_BAR || type == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
        sysBarPropMap_[type] = property;
    }
}

Rect WindowOption::GetWindowRect() const
{
    return windowRect_;
}

WindowType WindowOption::GetWindowType() const
{
    return type_;
}

WindowMode WindowOption::GetWindowMode() const
{
    return mode_;
}

WindowBlurLevel WindowOption::GetWindowBackgroundBlur() const
{
    return level_;
}

float WindowOption::GetAlpha() const
{
    return alpha_;
}

bool WindowOption::GetFocusable() const
{
    return focusable_;
}

bool WindowOption::GetTouchable() const
{
    return touchable_;
}

DisplayId WindowOption::GetDisplayId() const
{
    return displayId_;
}

const std::string& WindowOption::GetParentName() const
{
    return parentName_;
}

const std::string& WindowOption::GetWindowName() const
{
    return windowName_;
}

uint32_t WindowOption::GetWindowFlags() const
{
    return flags_;
}

void WindowOption::SetHitOffset(int32_t x, int32_t y)
{
    hitOffset_.x = x;
    hitOffset_.y = y;
}

void WindowOption::SetWindowTag(WindowTag windowTag)
{
    windowTag_ = windowTag;
}

WindowTag WindowOption::GetWindowTag() const
{
    return windowTag_;
}

const PointInfo& WindowOption::GetHitOffset() const
{
    return hitOffset_;
}

const std::unordered_map<WindowType, SystemBarProperty>& WindowOption::GetSystemBarProperty() const
{
    return sysBarPropMap_;
}
} // namespace Rosen
} // namespace OHOS

