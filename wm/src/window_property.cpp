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

#include "window_property.h"

namespace OHOS {
namespace Rosen {
void WindowProperty::SetWindowRect(const struct Rect& rect)
{
    windowRect_ = rect;
}

void WindowProperty::SetWindowType(WindowType type)
{
    type_ = type;
}

void WindowProperty::SetWindowMode(WindowMode mode)
{
    if (mode != WindowMode::WINDOW_MODE_SPLIT_PRIMARY &&
        mode != WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        SetLastWindowMode(mode);
    }
    mode_ = mode;
}

void WindowProperty::SetLastWindowMode(WindowMode mode)
{
    lastMode_ = mode;
}

void WindowProperty::SetFullScreen(bool isFullScreen)
{
    isFullScreen_ = isFullScreen;
}

void WindowProperty::SetFocusable(bool isFocusable)
{
    focusable_ = isFocusable;
}

void WindowProperty::SetTouchable(bool isTouchable)
{
    touchable_ = isTouchable;
}

void WindowProperty::SetPrivacyMode(bool isPrivate)
{
    isPrivacyMode_ = isPrivate;
}

void WindowProperty::SetTransparent(bool isTransparent)
{
    isTransparent_ = isTransparent;
}

void WindowProperty::SetAlpha(float alpha)
{
    alpha_ = alpha;
}

void WindowProperty::SetDisplayId(int32_t displayId)
{
    displayId_ = displayId;
}

void WindowProperty::SetWindowFlags(uint32_t flags)
{
    flags_ = flags;
}

void WindowProperty::SetSystemBarProperty(WindowType type, const SystemBarProperty& property)
{
    if (type == WindowType::WINDOW_TYPE_STATUS_BAR || type == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
        sysBarPropMap_[type] = property;
    }
}

void WindowProperty::ResumeLastWindowMode()
{
    mode_ = lastMode_;
}

Rect WindowProperty::GetWindowRect() const
{
    return windowRect_;
}

WindowType WindowProperty::GetWindowType() const
{
    return type_;
}

WindowMode WindowProperty::GetWindowMode() const
{
    return mode_;
}

bool WindowProperty::GetFullScreen() const
{
    return isFullScreen_;
}

bool WindowProperty::GetFocusable() const
{
    return focusable_;
}

bool WindowProperty::GetTouchable() const
{
    return touchable_;
}

bool WindowProperty::GetPrivacyMode() const
{
    return isPrivacyMode_;
}

bool WindowProperty::GetTransparent() const
{
    return isTransparent_;
}

float WindowProperty::GetAlpha() const
{
    return alpha_;
}

int32_t WindowProperty::GetDisplayId() const
{
    return displayId_;
}

uint32_t WindowProperty::GetWindowFlags() const
{
    return flags_;
}

const std::unordered_map<WindowType, SystemBarProperty>& WindowProperty::GetSystemBarProperty() const
{
    return sysBarPropMap_;
}

// TODO
void WindowProperty::SetWindowId(uint32_t windowId)
{
    windowId_ = windowId;
}
void WindowProperty::SetParentId(uint32_t parentId)
{
    parentId_ = parentId;
}
uint32_t WindowProperty::GetWindowId() const
{
    return windowId_;
}
uint32_t WindowProperty::GetParentId() const
{
    return parentId_;
}

bool WindowProperty::MapMarshalling(Parcel& parcel) const
{
    auto size = sysBarPropMap_.size();
    if (!parcel.WriteUint32(static_cast<uint32_t>(size))) {
        return false;
    }
    for (auto it : sysBarPropMap_) {
        // write key(type)
        if (!parcel.WriteUint32(static_cast<uint32_t>(it.first))) {
            return false;
        }
        // write val(sysBarProps)
        if (!(parcel.WriteBool(it.second.enable_) && parcel.WriteUint32(it.second.backgroundColor_) &&
            parcel.WriteUint32(it.second.contentColor_))) {
            return false;
        }
    }
    return true;
}

void WindowProperty::MapUnmarshalling(Parcel& parcel, sptr<WindowProperty>& property)
{
    std::unordered_map<WindowType, SystemBarProperty> sysBarPropMap;
    uint32_t size = parcel.ReadUint32();
    for (uint32_t i = 0; i < size; i++) {
        WindowType type = static_cast<WindowType>(parcel.ReadUint32());
        SystemBarProperty prop = { parcel.ReadBool(), parcel.ReadUint32(), parcel.ReadUint32() };
        property->SetSystemBarProperty(type, prop);
    }
}

bool WindowProperty::Marshalling(Parcel& parcel) const
{
    // write windowRect_
    if (!(parcel.WriteInt32(windowRect_.posX_) && parcel.WriteInt32(windowRect_.posY_) &&
        parcel.WriteUint32(windowRect_.width_) && parcel.WriteUint32(windowRect_.height_))) {
        return false;
    }

    // write type_
    if (!parcel.WriteUint32(static_cast<uint32_t>(type_))) {
        return false;
    }

    // write mode_
    if (!parcel.WriteUint32(static_cast<uint32_t>(mode_))) {
        return false;
    }

    // write flags_
    if (!parcel.WriteUint32(flags_)) {
        return false;
    }

    // write isFullScreen_
    if (!parcel.WriteBool(isFullScreen_)) {
        return false;
    }

    // write focusable_
    if (!parcel.WriteBool(focusable_)) {
        return false;
    }

    // write touchable_
    if (!parcel.WriteBool(touchable_)) {
        return false;
    }

    // write isPrivacyMode_
    if (!parcel.WriteBool(isPrivacyMode_)) {
        return false;
    }

    // write isTransparent_
    if (!parcel.WriteBool(isTransparent_)) {
        return false;
    }

    // write alpha_
    if (!parcel.WriteFloat(alpha_)) {
        return false;
    }

    // write displayId_
    if (!parcel.WriteInt32(displayId_)) {
        return false;
    }

    // write windowId_
    if (!parcel.WriteUint32(windowId_)) {
        return false;
    }

    // write parentId_
    if (!parcel.WriteUint32(parentId_)) {
        return false;
    }

    // write sysUIStateMap_
    if (!MapMarshalling(parcel)) {
        return false;
    }
    // write lastMode_
    if (!parcel.WriteUint32(static_cast<uint32_t>(lastMode_))) {
        return false;
    }
    return true;
}

sptr<WindowProperty> WindowProperty::Unmarshalling(Parcel& parcel)
{
    sptr<WindowProperty> property(new WindowProperty());
    Rect rect = { parcel.ReadInt32(), parcel.ReadInt32(), parcel.ReadUint32(), parcel.ReadUint32() };
    property->SetWindowRect(rect);
    property->SetWindowType(static_cast<WindowType>(parcel.ReadUint32()));
    property->SetWindowMode(static_cast<WindowMode>(parcel.ReadUint32()));
    property->SetWindowFlags(parcel.ReadUint32());
    property->SetFullScreen(parcel.ReadBool());
    property->SetFocusable(parcel.ReadBool());
    property->SetTouchable(parcel.ReadBool());
    property->SetPrivacyMode(parcel.ReadBool());
    property->SetTransparent(parcel.ReadBool());
    property->SetAlpha(parcel.ReadFloat());
    property->SetDisplayId(parcel.ReadInt32());
    property->SetWindowId(parcel.ReadUint32());
    property->SetParentId(parcel.ReadUint32());
    MapUnmarshalling(parcel, property);
    property->SetLastWindowMode(static_cast<WindowMode>(parcel.ReadUint32()));
    return property;
}
}
}
