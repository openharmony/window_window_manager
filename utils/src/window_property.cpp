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

#include "window_property.h"
#include "window_helper.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
WindowProperty::WindowProperty(const sptr<WindowProperty>& property)
{
    CopyFrom(property);
}

void WindowProperty::SetWindowName(const std::string& name)
{
    windowName_ = name;
}

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
    if (!WindowHelper::IsValidWindowMode(mode)) {
        return;
    }
    if (!WindowHelper::IsSplitWindowMode(mode_)) {
        lastMode_ = mode_;
    }
    mode_ = mode;
}

void WindowProperty::SetWindowBackgroundBlur(WindowBlurLevel level)
{
    if (!WindowHelper::IsValidWindowBlurLevel(level)) {
        return;
    }
    level_ = level;
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

void WindowProperty::SetDisplayId(DisplayId displayId)
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

void WindowProperty::SetDecorEnable(bool decorEnable)
{
    isDecorEnable_ = decorEnable;
}

void WindowProperty::SetHitOffset(const PointInfo& offset)
{
    hitOffset_ = offset;
}

void WindowProperty::SetAnimationFlag(uint32_t animationFlag)
{
    animationFlag_ = animationFlag;
}

void WindowProperty::SetWindowSizeChangeReason(WindowSizeChangeReason reason)
{
    windowSizeChangeReason_ = reason;
}

WindowSizeChangeReason WindowProperty::GetWindowSizeChangeReason() const
{
    return windowSizeChangeReason_;
}

void WindowProperty::ResumeLastWindowMode()
{
    mode_ = lastMode_;
}

const std::string& WindowProperty::GetWindowName() const
{
    return windowName_ ;
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

WindowBlurLevel WindowProperty::GetWindowBackgroundBlur() const
{
    return level_;
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

DisplayId WindowProperty::GetDisplayId() const
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

bool WindowProperty::GetDecorEnable() const
{
    return isDecorEnable_;
}

void WindowProperty::SetWindowId(uint32_t windowId)
{
    windowId_ = windowId;
}

void WindowProperty::SetParentId(uint32_t parentId)
{
    parentId_ = parentId;
}

void WindowProperty::SetTokenState(bool hasToken)
{
    tokenState_ = hasToken;
}

uint32_t WindowProperty::GetWindowId() const
{
    return windowId_;
}

uint32_t WindowProperty::GetParentId() const
{
    return parentId_;
}

const PointInfo& WindowProperty::GetHitOffset() const
{
    return hitOffset_;
}

uint32_t WindowProperty::GetAnimationFlag() const
{
    return animationFlag_;
}

bool WindowProperty::GetTokenState() const
{
    return tokenState_;
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
    // write windowName_
    if (!parcel.WriteString(windowName_)) {
        return false;
    }

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

    // write blur level_
    if (!parcel.WriteUint32(static_cast<uint32_t>(level_))) {
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
    if (!parcel.WriteUint64(displayId_)) {
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

    // write isDecorEnable_
    if (!parcel.WriteBool(isDecorEnable_)) {
        return false;
    }

    // write hitOffset_
    if (!(parcel.WriteInt32(hitOffset_.x) and parcel.WriteInt32(hitOffset_.y))) {
        return false;
    }

    // write parentId_
    if (!parcel.WriteUint32(animationFlag_)) {
        return false;
    }

    // write windowSizeChangeReason_
    if (!parcel.WriteUint32(static_cast<uint32_t>(windowSizeChangeReason_))) {
        return false;
    }

    // write tokenState
    if (!parcel.WriteBool(tokenState_)) {
        return false;
    }
    return true;
}

sptr<WindowProperty> WindowProperty::Unmarshalling(Parcel& parcel)
{
    sptr<WindowProperty> property(new WindowProperty());
    property->SetWindowName(parcel.ReadString());
    Rect rect = { parcel.ReadInt32(), parcel.ReadInt32(), parcel.ReadUint32(), parcel.ReadUint32() };
    property->SetWindowRect(rect);
    property->SetWindowType(static_cast<WindowType>(parcel.ReadUint32()));
    property->SetWindowMode(static_cast<WindowMode>(parcel.ReadUint32()));
    property->SetWindowBackgroundBlur(static_cast<WindowBlurLevel>(parcel.ReadUint32()));
    property->SetWindowFlags(parcel.ReadUint32());
    property->SetFullScreen(parcel.ReadBool());
    property->SetFocusable(parcel.ReadBool());
    property->SetTouchable(parcel.ReadBool());
    property->SetPrivacyMode(parcel.ReadBool());
    property->SetTransparent(parcel.ReadBool());
    property->SetAlpha(parcel.ReadFloat());
    property->SetDisplayId(parcel.ReadUint64());
    property->SetWindowId(parcel.ReadUint32());
    property->SetParentId(parcel.ReadUint32());
    MapUnmarshalling(parcel, property);
    property->SetDecorEnable(parcel.ReadBool());
    PointInfo offset = {parcel.ReadInt32(), parcel.ReadInt32()};
    property->SetHitOffset(offset);
    property->SetAnimationFlag(parcel.ReadUint32());
    property->SetWindowSizeChangeReason(static_cast<WindowSizeChangeReason>(parcel.ReadUint32()));
    property->SetTokenState(parcel.ReadBool());
    return property;
}

void WindowProperty::CopyFrom(const sptr<WindowProperty>& property)
{
    windowName_ = property->windowName_;
    windowRect_ = property->windowRect_;
    type_ = property->type_;
    mode_ = property->mode_;
    level_ = property->level_;
    lastMode_ = property->lastMode_;
    flags_ = property->flags_;
    isFullScreen_ = property->isFullScreen_;
    focusable_ = property->focusable_;
    touchable_ = property->touchable_;
    isPrivacyMode_ = property->isPrivacyMode_;
    isTransparent_ = property->isTransparent_;
    alpha_ = property->alpha_;
    displayId_ = property->displayId_;
    windowId_ = property->windowId_;
    parentId_ = property->parentId_;
    hitOffset_ = property->hitOffset_;
    animationFlag_ = property->animationFlag_;
    windowSizeChangeReason_ = property->windowSizeChangeReason_;
    sysBarPropMap_ = property->sysBarPropMap_;
    isDecorEnable_ = property->isDecorEnable_;
    tokenState_ = property->tokenState_;
}
}
}
