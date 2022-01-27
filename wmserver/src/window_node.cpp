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
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
void WindowNode::SetDisplayId(DisplayId displayId)
{
    property_->SetDisplayId(displayId);
}

void WindowNode::SetLayoutRect(const Rect& rect)
{
    layoutRect_ = rect;
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

WindowType WindowNode::GetWindowType() const
{
    return property_->GetWindowType();
}

WindowMode WindowNode::GetWindowMode() const
{
    return property_->GetWindowMode();
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
}
}
