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

void WindowProperty::SetDecoStatus(bool decoStatus)
{
    decoStatus_ = decoStatus;
}

void WindowProperty::SetRequestRect(const Rect& requestRect)
{
    requestRect_ = requestRect;
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

void WindowProperty::SetLastWindowMode(WindowMode mode)
{
    lastMode_ = mode;
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

void WindowProperty::SetBrightness(float brightness)
{
    brightness_ = brightness;
}

void WindowProperty::SetTurnScreenOn(bool turnScreenOn)
{
    turnScreenOn_ = turnScreenOn;
}

void WindowProperty::SetKeepScreenOn(bool keepScreenOn)
{
    keepScreenOn_ = keepScreenOn;
}

void WindowProperty::SetCallingWindow(uint32_t windowId)
{
    callingWindow_ = windowId;
}

void WindowProperty::SetDisplayId(DisplayId displayId)
{
    displayId_ = displayId;
}

void WindowProperty::SetWindowFlags(uint32_t flags)
{
    flags_ = flags;
}

void WindowProperty::AddWindowFlag(WindowFlag flag)
{
    flags_ |= static_cast<uint32_t>(flag);
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

void WindowProperty::SetDragType(DragType dragType)
{
    dragType_ = dragType;
}

void WindowProperty::SetStretchable(bool stretchable)
{
    isStretchable_ = stretchable;
}

void WindowProperty::SetOriginRect(const Rect& rect)
{
    originRect_ = rect;
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

bool WindowProperty::GetDecoStatus() const
{
    return decoStatus_;
}

Rect WindowProperty::GetRequestRect() const
{
    return requestRect_;
}

WindowType WindowProperty::GetWindowType() const
{
    return type_;
}

WindowMode WindowProperty::GetWindowMode() const
{
    return mode_;
}

WindowMode WindowProperty::GetLastWindowMode() const
{
    return lastMode_;
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

uint32_t WindowProperty::GetCallingWindow() const
{
    return callingWindow_;
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

float WindowProperty::GetBrightness() const
{
    return brightness_;
}

bool WindowProperty::IsTurnScreenOn() const
{
    return turnScreenOn_;
}

bool WindowProperty::IsKeepScreenOn() const
{
    return keepScreenOn_;
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

void WindowProperty::SetModeSupportInfo(uint32_t modeSupportInfo)
{
    modeSupportInfo_ = modeSupportInfo;
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

uint32_t WindowProperty::GetModeSupportInfo() const
{
    return modeSupportInfo_;
}

bool WindowProperty::GetTokenState() const
{
    return tokenState_;
}

DragType WindowProperty::GetDragType() const
{
    return dragType_;
}

const Rect& WindowProperty::GetOriginRect() const
{
    return originRect_;
}

bool WindowProperty::GetStretchable() const
{
    return isStretchable_;
}

void WindowProperty::SetTouchHotAreas(const std::vector<Rect>& rects)
{
    touchHotAreas_ = rects;
}

void WindowProperty::GetTouchHotAreas(std::vector<Rect>& rects) const
{
    rects = touchHotAreas_;
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

void WindowProperty::MapUnmarshalling(Parcel& parcel, WindowProperty* property)
{
    std::unordered_map<WindowType, SystemBarProperty> sysBarPropMap;
    uint32_t size = parcel.ReadUint32();
    for (uint32_t i = 0; i < size; i++) {
        WindowType type = static_cast<WindowType>(parcel.ReadUint32());
        SystemBarProperty prop = { parcel.ReadBool(), parcel.ReadUint32(), parcel.ReadUint32() };
        property->SetSystemBarProperty(type, prop);
    }
}

bool WindowProperty::MarshallingTouchHotAreas(Parcel& parcel) const
{
    auto size = touchHotAreas_.size();
    if (!parcel.WriteUint32(static_cast<uint32_t>(size))) {
        return false;
    }
    for (const auto& rect : touchHotAreas_) {
        if (!(parcel.WriteInt32(rect.posX_) && parcel.WriteInt32(rect.posY_) &&
            parcel.WriteUint32(rect.width_) && parcel.WriteUint32(rect.height_))) {
            return false;
        }
    }
    return true;
}

void WindowProperty::UnmarshallingTouchHotAreas(Parcel& parcel, WindowProperty* property)
{
    auto size = parcel.ReadUint32();
    for (uint32_t i = 0; i < size; i++) {
        property->touchHotAreas_.emplace_back(
            Rect{ parcel.ReadInt32(), parcel.ReadInt32(), parcel.ReadUint32(), parcel.ReadUint32() });
    }
}

bool WindowProperty::Marshalling(Parcel& parcel) const
{
    return parcel.WriteString(windowName_) && parcel.WriteInt32(windowRect_.posX_) &&
        parcel.WriteInt32(windowRect_.posY_) && parcel.WriteUint32(windowRect_.width_) &&
        parcel.WriteUint32(windowRect_.height_) && parcel.WriteInt32(requestRect_.posX_) &&
        parcel.WriteInt32(requestRect_.posY_) && parcel.WriteUint32(requestRect_.width_) &&
        parcel.WriteUint32(requestRect_.height_) && parcel.WriteBool(decoStatus_) &&
        parcel.WriteUint32(static_cast<uint32_t>(type_)) &&
        parcel.WriteUint32(static_cast<uint32_t>(mode_)) && parcel.WriteUint32(static_cast<uint32_t>(lastMode_)) &&
        parcel.WriteUint32(static_cast<uint32_t>(level_)) && parcel.WriteUint32(flags_) &&
        parcel.WriteBool(isFullScreen_) && parcel.WriteBool(focusable_) && parcel.WriteBool(touchable_) &&
        parcel.WriteBool(isPrivacyMode_) && parcel.WriteBool(isTransparent_) && parcel.WriteFloat(alpha_) &&
        parcel.WriteFloat(brightness_) && parcel.WriteUint64(displayId_) && parcel.WriteUint32(windowId_) &&
        parcel.WriteUint32(parentId_) && MapMarshalling(parcel) && parcel.WriteBool(isDecorEnable_) &&
        parcel.WriteInt32(hitOffset_.x) && parcel.WriteInt32(hitOffset_.y) && parcel.WriteUint32(animationFlag_) &&
        parcel.WriteUint32(static_cast<uint32_t>(windowSizeChangeReason_)) && parcel.WriteBool(tokenState_) &&
        parcel.WriteUint32(callingWindow_) && parcel.WriteUint32(static_cast<uint32_t>(requestedOrientation_)) &&
        parcel.WriteBool(turnScreenOn_) && parcel.WriteBool(keepScreenOn_) &&
        parcel.WriteUint32(modeSupportInfo_) && parcel.WriteUint32(static_cast<uint32_t>(dragType_)) &&
        parcel.WriteUint32(originRect_.width_) && parcel.WriteUint32(originRect_.height_) &&
        parcel.WriteBool(isStretchable_) && MarshallingTouchHotAreas(parcel);
}

WindowProperty* WindowProperty::Unmarshalling(Parcel& parcel)
{
    WindowProperty* property = new(std::nothrow) WindowProperty();
    if (property == nullptr) {
        return nullptr;
    }
    property->SetWindowName(parcel.ReadString());
    Rect rect = { parcel.ReadInt32(), parcel.ReadInt32(), parcel.ReadUint32(), parcel.ReadUint32() };
    property->SetWindowRect(rect);
    Rect reqRect = { parcel.ReadInt32(), parcel.ReadInt32(), parcel.ReadUint32(), parcel.ReadUint32() };
    property->SetRequestRect(reqRect);
    property->SetDecoStatus(parcel.ReadBool());
    property->SetWindowType(static_cast<WindowType>(parcel.ReadUint32()));
    property->SetWindowMode(static_cast<WindowMode>(parcel.ReadUint32()));
    property->SetLastWindowMode(static_cast<WindowMode>(parcel.ReadUint32()));
    property->SetWindowBackgroundBlur(static_cast<WindowBlurLevel>(parcel.ReadUint32()));
    property->SetWindowFlags(parcel.ReadUint32());
    property->SetFullScreen(parcel.ReadBool());
    property->SetFocusable(parcel.ReadBool());
    property->SetTouchable(parcel.ReadBool());
    property->SetPrivacyMode(parcel.ReadBool());
    property->SetTransparent(parcel.ReadBool());
    property->SetAlpha(parcel.ReadFloat());
    property->SetBrightness(parcel.ReadFloat());
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
    property->SetCallingWindow(parcel.ReadUint32());
    property->SetRequestedOrientation(static_cast<Orientation>(parcel.ReadUint32()));
    property->SetTurnScreenOn(parcel.ReadBool());
    property->SetKeepScreenOn(parcel.ReadBool());
    property->SetModeSupportInfo(parcel.ReadUint32());
    property->SetDragType(static_cast<DragType>(parcel.ReadUint32()));
    uint32_t w = parcel.ReadUint32();
    uint32_t h = parcel.ReadUint32();
    property->SetOriginRect(Rect { 0, 0, w, h });
    property->SetStretchable(parcel.ReadBool());
    UnmarshallingTouchHotAreas(parcel, property);
    return property;
}

bool WindowProperty::Write(Parcel& parcel, PropertyChangeAction action)
{
    bool ret = parcel.WriteUint32(static_cast<uint32_t>(windowId_));
    switch (action) {
        case PropertyChangeAction::ACTION_UPDATE_RECT:
            ret &= parcel.WriteBool(decoStatus_) && parcel.WriteUint32(static_cast<uint32_t>(dragType_)) &&
                parcel.WriteInt32(originRect_.posX_) && parcel.WriteInt32(originRect_.posY_) &&
                parcel.WriteUint32(originRect_.width_) && parcel.WriteUint32(originRect_.height_) &&
                parcel.WriteInt32(requestRect_.posX_) && parcel.WriteInt32(requestRect_.posY_) &&
                parcel.WriteUint32(requestRect_.width_) && parcel.WriteUint32(requestRect_.height_) &&
                parcel.WriteUint32(static_cast<uint32_t>(windowSizeChangeReason_));
            break;
        case PropertyChangeAction::ACTION_UPDATE_MODE:
            ret &= parcel.WriteUint32(static_cast<uint32_t>(mode_));
            break;
        case PropertyChangeAction::ACTION_UPDATE_FLAGS:
            ret &= parcel.WriteUint32(flags_);
            break;
        case PropertyChangeAction::ACTION_UPDATE_OTHER_PROPS:
            ret &= MapMarshalling(parcel);
            break;
        case PropertyChangeAction::ACTION_UPDATE_FOCUSABLE:
            ret &= parcel.WriteBool(focusable_);
            break;
        case PropertyChangeAction::ACTION_UPDATE_TOUCHABLE:
            ret &= parcel.WriteBool(touchable_);
            break;
        case PropertyChangeAction::ACTION_UPDATE_CALLING_WINDOW:
            ret &= parcel.WriteUint32(callingWindow_);
            break;
        case PropertyChangeAction::ACTION_UPDATE_ORIENTATION:
            ret &= parcel.WriteUint32(static_cast<uint32_t>(requestedOrientation_));
            break;
        case PropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON:
            ret &= parcel.WriteBool(turnScreenOn_);
            break;
        case PropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON:
            ret &= parcel.WriteBool(keepScreenOn_);
            break;
        case PropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS:
            ret &= parcel.WriteFloat(brightness_);
            break;
        case PropertyChangeAction::ACTION_UPDATE_MODE_SUPPORT_INFO:
            ret &= parcel.WriteUint32(modeSupportInfo_);
            break;
        case PropertyChangeAction::ACTION_UPDATE_TOUCH_HOT_AREA:
            ret &= MarshallingTouchHotAreas(parcel);
            break;
        default:
            break;
    }
    return ret;
}

void WindowProperty::Read(Parcel& parcel, PropertyChangeAction action)
{
    SetWindowId(parcel.ReadUint32());
    switch (action) {
        case PropertyChangeAction::ACTION_UPDATE_RECT:
            SetDecoStatus(parcel.ReadBool());
            SetDragType(static_cast<DragType>(parcel.ReadUint32()));
            SetOriginRect(Rect { parcel.ReadInt32(), parcel.ReadInt32(), parcel.ReadUint32(), parcel.ReadUint32() });
            SetRequestRect(Rect { parcel.ReadInt32(), parcel.ReadInt32(), parcel.ReadUint32(), parcel.ReadUint32() });
            SetWindowSizeChangeReason(static_cast<WindowSizeChangeReason>(parcel.ReadUint32()));
            break;
        case PropertyChangeAction::ACTION_UPDATE_MODE:
            SetWindowMode(static_cast<WindowMode>(parcel.ReadUint32()));
            break;
        case PropertyChangeAction::ACTION_UPDATE_FLAGS:
            SetWindowFlags(parcel.ReadUint32());
            break;
        case PropertyChangeAction::ACTION_UPDATE_OTHER_PROPS:
            MapUnmarshalling(parcel, this);
            break;
        case PropertyChangeAction::ACTION_UPDATE_FOCUSABLE:
            SetFocusable(parcel.ReadBool());
            break;
        case PropertyChangeAction::ACTION_UPDATE_TOUCHABLE:
            SetTouchable(parcel.ReadBool());
            break;
        case PropertyChangeAction::ACTION_UPDATE_CALLING_WINDOW:
            SetCallingWindow(parcel.ReadUint32());
            break;
        case PropertyChangeAction::ACTION_UPDATE_ORIENTATION:
            SetRequestedOrientation(static_cast<Orientation>(parcel.ReadUint32()));
            break;
        case PropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON:
            SetTurnScreenOn(parcel.ReadBool());
            break;
        case PropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON:
            SetKeepScreenOn(parcel.ReadBool());
            break;
        case PropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS:
            SetBrightness(parcel.ReadFloat());
            break;
        case PropertyChangeAction::ACTION_UPDATE_MODE_SUPPORT_INFO:
            SetModeSupportInfo(parcel.ReadUint32());
            break;
        case PropertyChangeAction::ACTION_UPDATE_TOUCH_HOT_AREA:
            UnmarshallingTouchHotAreas(parcel, this);
            break;
        default:
            break;
    }
}

void WindowProperty::CopyFrom(const sptr<WindowProperty>& property)
{
    windowName_ = property->windowName_;
    windowRect_ = property->windowRect_;
    requestRect_ = property->requestRect_;
    decoStatus_ = property->decoStatus_;
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
    brightness_ = property->brightness_;
    displayId_ = property->displayId_;
    windowId_ = property->windowId_;
    parentId_ = property->parentId_;
    hitOffset_ = property->hitOffset_;
    animationFlag_ = property->animationFlag_;
    windowSizeChangeReason_ = property->windowSizeChangeReason_;
    sysBarPropMap_ = property->sysBarPropMap_;
    isDecorEnable_ = property->isDecorEnable_;
    tokenState_ = property->tokenState_;
    callingWindow_ = property->callingWindow_;
    requestedOrientation_ = property->requestedOrientation_;
    turnScreenOn_ = property->turnScreenOn_;
    keepScreenOn_ = property->keepScreenOn_;
    modeSupportInfo_ = property->modeSupportInfo_;
    dragType_ = property->dragType_;
    originRect_ = property->originRect_;
    isStretchable_ = property->isStretchable_;
    touchHotAreas_ = property->touchHotAreas_;
}
}
}
