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

#include "common/include/window_session_property.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "window_helper.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t TOUCH_HOT_AREA_MAX_NUM = 10;
}

WindowSessionProperty::WindowSessionProperty(const sptr<WindowSessionProperty>& property)
{
    CopyFrom(property);
}

void WindowSessionProperty::SetWindowName(const std::string& name)
{
    windowName_ = name;
}

void WindowSessionProperty::SetSessionInfo(const SessionInfo& info)
{
    sessionInfo_ = info;
}

void WindowSessionProperty::SetWindowRect(const struct Rect& rect)
{
    windowRect_ = rect;
}

void WindowSessionProperty::SetRequestRect(const Rect& requestRect)
{
    requestRect_ = requestRect;
}

void WindowSessionProperty::SetWindowType(WindowType type)
{
    type_ = type;
}

void WindowSessionProperty::SetFocusable(bool isFocusable)
{
    focusable_ = isFocusable;
}

void WindowSessionProperty::SetTouchable(bool isTouchable)
{
    touchable_ = isTouchable;
}

void WindowSessionProperty::SetDragEnabled(bool dragEnabled)
{
    dragEnabled_ = dragEnabled;
}

void WindowSessionProperty::SetHideNonSystemFloatingWindows(bool hide)
{
    hideNonSystemFloatingWindows_ = hide;
}

void WindowSessionProperty::SetForceHide(bool hide)
{
    forceHide_ = hide;
}

void WindowSessionProperty::SetRaiseEnabled(bool raiseEnabled)
{
    raiseEnabled_ = raiseEnabled;
}

void WindowSessionProperty::SetRequestedOrientation(Orientation orientation)
{
    requestedOrientation_ = orientation;
}

void WindowSessionProperty::SetPrivacyMode(bool isPrivate)
{
    isPrivacyMode_ = isPrivate;
}

void WindowSessionProperty::SetSystemPrivacyMode(bool isSystemPrivate)
{
    isSystemPrivacyMode_ = isSystemPrivate;
}

void WindowSessionProperty::SetBrightness(float brightness)
{
    brightness_ = brightness;
}

void WindowSessionProperty::SetSystemCalling(bool isSystemCalling)
{
    isSystemCalling_ = isSystemCalling;
}

void WindowSessionProperty::SetDisplayId(DisplayId displayId)
{
    displayId_ = displayId;
}

void WindowSessionProperty::SetFloatingWindowAppType(bool isAppType)
{
    isFloatingWindowAppType_ = isAppType;
}

const std::string& WindowSessionProperty::GetWindowName() const
{
    return windowName_;
}

const SessionInfo& WindowSessionProperty::GetSessionInfo() const
{
    return sessionInfo_;
}

Rect WindowSessionProperty::GetWindowRect() const
{
    return windowRect_;
}

Rect WindowSessionProperty::GetRequestRect() const
{
    return requestRect_;
}

WindowType WindowSessionProperty::GetWindowType() const
{
    return type_;
}

bool WindowSessionProperty::GetFocusable() const
{
    return focusable_;
}

bool WindowSessionProperty::GetTouchable() const
{
    return touchable_;
}

bool WindowSessionProperty::GetDragEnabled() const
{
    return dragEnabled_;
}

bool WindowSessionProperty::GetHideNonSystemFloatingWindows() const
{
    return hideNonSystemFloatingWindows_;
}

bool WindowSessionProperty::GetForceHide() const
{
    return forceHide_;
}

bool WindowSessionProperty::GetRaiseEnabled() const
{
    return raiseEnabled_;
}

Orientation WindowSessionProperty::GetRequestedOrientation() const
{
    return requestedOrientation_;
}

bool WindowSessionProperty::GetPrivacyMode() const
{
    return isPrivacyMode_;
}

bool WindowSessionProperty::GetSystemPrivacyMode() const
{
    return isSystemPrivacyMode_;
}

float WindowSessionProperty::GetBrightness() const
{
    return brightness_;
}

bool WindowSessionProperty::GetSystemCalling() const
{
    return isSystemCalling_;
}

DisplayId WindowSessionProperty::GetDisplayId() const
{
    return displayId_;
}

void WindowSessionProperty::SetParentId(int32_t parentId)
{
    parentId_ = parentId;
}

int32_t WindowSessionProperty::GetParentId() const
{
    return parentId_;
}

void WindowSessionProperty::SetWindowFlags(uint32_t flags)
{
    flags_ = flags;
}

void WindowSessionProperty::AddWindowFlag(WindowFlag flag)
{
    flags_ |= static_cast<uint32_t>(flag);
}

uint32_t WindowSessionProperty::GetWindowFlags() const
{
    return flags_;
}

void WindowSessionProperty::SetPersistentId(int32_t persistentId)
{
    persistentId_ = persistentId;
}

int32_t WindowSessionProperty::GetPersistentId() const
{
    return persistentId_;
}

void WindowSessionProperty::SetParentPersistentId(int32_t persistentId)
{
    parentPersistentId_ = persistentId;
}

int32_t WindowSessionProperty::GetParentPersistentId() const
{
    return parentPersistentId_;
}

void WindowSessionProperty::SetTurnScreenOn(bool turnScreenOn)
{
    turnScreenOn_ = turnScreenOn;
}

bool WindowSessionProperty::IsTurnScreenOn() const
{
    return turnScreenOn_;
}

void WindowSessionProperty::SetKeepScreenOn(bool keepScreenOn)
{
    keepScreenOn_ = keepScreenOn;
}

bool WindowSessionProperty::IsKeepScreenOn() const
{
    return keepScreenOn_;
}

void WindowSessionProperty::SetAccessTokenId(uint32_t accessTokenId)
{
    accessTokenId_ = accessTokenId;
}

uint32_t WindowSessionProperty::GetAccessTokenId() const
{
    return accessTokenId_;
}

void WindowSessionProperty::SetTokenState(bool hasToken)
{
    tokenState_ = hasToken;
}

bool WindowSessionProperty::GetTokenState() const
{
    return tokenState_;
}

MaximizeMode WindowSessionProperty::GetMaximizeMode() const
{
    return maximizeMode_;
}

void WindowSessionProperty::SetMaximizeMode(MaximizeMode mode)
{
    maximizeMode_ = mode;
}

void WindowSessionProperty::SetSystemBarProperty(WindowType type, const SystemBarProperty& property)
{
    if (type == WindowType::WINDOW_TYPE_STATUS_BAR
        || type ==WindowType::WINDOW_TYPE_NAVIGATION_BAR
        || type == WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR) {
        sysBarPropMap_[type] = property;
    }
}

std::unordered_map<WindowType, SystemBarProperty> WindowSessionProperty::GetSystemBarProperty() const
{
    return sysBarPropMap_;
}

void WindowSessionProperty::SetWindowLimits(const WindowLimits& windowLimits)
{
    limits_ = windowLimits;
}

WindowLimits WindowSessionProperty::GetWindowLimits() const
{
    return limits_;
}

void WindowSessionProperty::SetWindowMode(WindowMode mode)
{
    windowMode_ = mode;
}

WindowMode WindowSessionProperty::GetWindowMode() const
{
    return windowMode_;
}

WindowState WindowSessionProperty::GetWindowState() const
{
    return windowState_;
}

void WindowSessionProperty::SetWindowState(WindowState state)
{
    windowState_ = state;
}

void WindowSessionProperty::SetSessionGravity(SessionGravity gravity, uint32_t percent)
{
    sessionGravity_ = gravity;
    sessionGravitySizePercent_ = percent;
}

void WindowSessionProperty::GetSessionGravity(SessionGravity& gravity, uint32_t& percent)
{
    gravity = sessionGravity_;
    percent = sessionGravitySizePercent_;
}

void WindowSessionProperty::SetDecorEnable(bool isDecorEnable)
{
    isDecorEnable_ = isDecorEnable;
}

bool WindowSessionProperty::IsDecorEnable()
{
    return isDecorEnable_;
}

void WindowSessionProperty::SetModeSupportInfo(uint32_t modeSupportInfo)
{
    modeSupportInfo_ = modeSupportInfo;
}

uint32_t WindowSessionProperty::GetModeSupportInfo() const
{
    return modeSupportInfo_;
}

void WindowSessionProperty::SetAnimationFlag(uint32_t animationFlag)
{
    animationFlag_ = animationFlag;
}

uint32_t WindowSessionProperty::GetAnimationFlag() const
{
    return animationFlag_;
}

bool WindowSessionProperty::IsFloatingWindowAppType() const
{
    return isFloatingWindowAppType_;
}

void WindowSessionProperty::SetTouchHotAreas(const std::vector<Rect>& rects)
{
    touchHotAreas_ = rects;
    if (touchHotAreasChangeCallback_) {
        touchHotAreasChangeCallback_();
    }
}

void WindowSessionProperty::GetTouchHotAreas(std::vector<Rect>& rects) const
{
    rects = touchHotAreas_;
}

void WindowSessionProperty::KeepKeyboardOnFocus(bool keepKeyboardFlag)
{
    keepKeyboardFlag_ = keepKeyboardFlag;
}

bool WindowSessionProperty::GetKeepKeyboardFlag() const
{
    return keepKeyboardFlag_;
}

void WindowSessionProperty::SetCallingWindow(uint32_t windowId)
{
    callingWindowId_ = windowId;
}

uint32_t WindowSessionProperty::GetCallingWindow() const
{
    return callingWindowId_;
}

void WindowSessionProperty::SetPiPTemplateInfo(const PiPTemplateInfo& pipTemplateInfo)
{
    pipTemplateInfo_ = pipTemplateInfo;
}

PiPTemplateInfo WindowSessionProperty::GetPiPTemplateInfo() const
{
    return pipTemplateInfo_;
}

void WindowSessionProperty::SetIsNeedUpdateWindowMode(bool isNeedUpdateWindowMode)
{
    isNeedUpdateWindowMode_ = isNeedUpdateWindowMode;
}

bool WindowSessionProperty::GetIsNeedUpdateWindowMode() const
{
    return isNeedUpdateWindowMode_;
}

bool WindowSessionProperty::MarshallingWindowLimits(Parcel& parcel) const
{
    if (parcel.WriteUint32(limits_.maxWidth_) &&
        parcel.WriteUint32(limits_.maxHeight_) && parcel.WriteUint32(limits_.minWidth_) &&
        parcel.WriteUint32(limits_.minHeight_) && parcel.WriteFloat(limits_.maxRatio_) &&
        parcel.WriteFloat(limits_.minRatio_)) {
        return true;
    }
    return false;
}

void WindowSessionProperty::UnmarshallingWindowLimits(Parcel& parcel, WindowSessionProperty* property)
{
    WindowLimits windowLimits = { parcel.ReadUint32(), parcel.ReadUint32(), parcel.ReadUint32(),
                                  parcel.ReadUint32(), parcel.ReadFloat(), parcel.ReadFloat() };
    property->SetWindowLimits(windowLimits);
}

bool WindowSessionProperty::MarshallingSystemBarMap(Parcel& parcel) const
{
    auto size = sysBarPropMap_.size();
    uint32_t maxSystemBarNumber = 3;
    if (size > maxSystemBarNumber) { // max systembar number
        return false;
    }

    if (!parcel.WriteUint32(static_cast<uint32_t>(size))) {
        return false;
    }
    for (auto it : sysBarPropMap_) {
        if (!parcel.WriteUint32(static_cast<uint32_t>(it.first))) {
            return false;
        }
        if (!(parcel.WriteBool(it.second.enable_) && parcel.WriteUint32(it.second.backgroundColor_) &&
            parcel.WriteUint32(it.second.contentColor_))) {
            return false;
        }
    }
    return true;
}

void WindowSessionProperty::UnMarshallingSystemBarMap(Parcel& parcel, WindowSessionProperty* property)
{
    uint32_t size = parcel.ReadUint32();
    uint32_t maxSystemBarNumber = 3;
    if (size > maxSystemBarNumber) { // max systembar number
        return;
    }

    for (uint32_t i = 0; i < size; i++) {
        WindowType type = static_cast<WindowType>(parcel.ReadUint32());
        SystemBarProperty prop = { parcel.ReadBool(), parcel.ReadUint32(), parcel.ReadUint32() };
        property->SetSystemBarProperty(type, prop);
    }
}

bool WindowSessionProperty::MarshallingTouchHotAreas(Parcel& parcel) const
{
    auto size = touchHotAreas_.size();
    if (size > TOUCH_HOT_AREA_MAX_NUM) {
        return false;
    }
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

void WindowSessionProperty::UnmarshallingTouchHotAreas(Parcel& parcel, WindowSessionProperty* property)
{
    uint32_t size = parcel.ReadUint32();
    if (size > TOUCH_HOT_AREA_MAX_NUM) {
        return;
    }
    for (uint32_t i = 0; i < size; i++) {
        property->touchHotAreas_.emplace_back(
            Rect{ parcel.ReadInt32(), parcel.ReadInt32(), parcel.ReadUint32(), parcel.ReadUint32() });
    }
}

bool WindowSessionProperty::MarshallingPiPTemplateInfo(Parcel& parcel) const
{
    if (!WindowHelper::IsPipWindow(type_)) {
        return true;
    }
    if (!parcel.WriteUint32(pipTemplateInfo_.pipTemplateType)) {
        return false;
    }
    if (!parcel.WriteUint32(pipTemplateInfo_.priority)) {
        return false;
    }
    auto size = pipTemplateInfo_.controlGroup.size();
    if (!parcel.WriteUint32(static_cast<uint32_t>(size))) {
        return false;
    }
    for (uint32_t i = 0; i < size; i++) {
        if (!parcel.WriteUint32(pipTemplateInfo_.controlGroup[i])) {
            return false;
        }
    }
    return true;
}

void WindowSessionProperty::UnmarshallingPiPTemplateInfo(Parcel& parcel, WindowSessionProperty* property)
{
    if (!WindowHelper::IsPipWindow(property->GetWindowType())) {
        return;
    }
    PiPTemplateInfo pipTemplateInfo;
    pipTemplateInfo.pipTemplateType = parcel.ReadUint32();
    pipTemplateInfo.priority = parcel.ReadUint32();
    auto size = parcel.ReadUint32();
    for (uint32_t i = 0; i < size; i++) {
        pipTemplateInfo.controlGroup.push_back(parcel.ReadUint32());
    }
    property->SetPiPTemplateInfo(pipTemplateInfo);
}

bool WindowSessionProperty::Marshalling(Parcel& parcel) const
{
    return parcel.WriteString(windowName_) && parcel.WriteInt32(windowRect_.posX_) &&
        parcel.WriteInt32(windowRect_.posY_) && parcel.WriteUint32(windowRect_.width_) &&
        parcel.WriteUint32(windowRect_.height_) && parcel.WriteInt32(requestRect_.posX_) &&
        parcel.WriteInt32(requestRect_.posY_) && parcel.WriteUint32(requestRect_.width_) &&
        parcel.WriteUint32(requestRect_.height_) &&
        parcel.WriteUint32(static_cast<uint32_t>(type_)) &&
        parcel.WriteBool(focusable_) && parcel.WriteBool(touchable_) && parcel.WriteBool(tokenState_) &&
        parcel.WriteBool(turnScreenOn_) && parcel.WriteBool(keepScreenOn_) &&
        parcel.WriteBool(isPrivacyMode_) && parcel.WriteBool(isSystemPrivacyMode_) &&
        parcel.WriteUint64(displayId_) && parcel.WriteInt32(persistentId_) &&
        parcel.WriteString(sessionInfo_.bundleName_) && parcel.WriteString(sessionInfo_.moduleName_) &&
        parcel.WriteString(sessionInfo_.abilityName_) &&
        parcel.WriteInt32(parentPersistentId_) &&
        parcel.WriteUint32(accessTokenId_) && parcel.WriteUint32(static_cast<uint32_t>(maximizeMode_)) &&
        parcel.WriteUint32(static_cast<uint32_t>(requestedOrientation_)) &&
        parcel.WriteUint32(static_cast<uint32_t>(windowMode_)) &&
        parcel.WriteUint32(flags_) && parcel.WriteBool(raiseEnabled_) &&
        parcel.WriteBool(isDecorEnable_) && parcel.WriteBool(dragEnabled_) &&
        parcel.WriteBool(hideNonSystemFloatingWindows_) && parcel.WriteBool(forceHide_) &&
        MarshallingWindowLimits(parcel) && parcel.WriteFloat(brightness_) &&
        MarshallingSystemBarMap(parcel) && parcel.WriteUint32(animationFlag_) &&
        MarshallingPiPTemplateInfo(parcel) &&
        parcel.WriteBool(isFloatingWindowAppType_) && MarshallingTouchHotAreas(parcel) &&
        parcel.WriteBool(isSystemCalling_) &&
        parcel.WriteDouble(textFieldPositionY_) && parcel.WriteDouble(textFieldHeight_) &&
        parcel.WriteUint32(static_cast<uint32_t>(windowState_)) &&
        parcel.WriteBool(isNeedUpdateWindowMode_) && parcel.WriteUint32(callingWindowId_) &&
        parcel.WriteBool(isLayoutFullScreen_);
}

WindowSessionProperty* WindowSessionProperty::Unmarshalling(Parcel& parcel)
{
    WindowSessionProperty* property = new(std::nothrow) WindowSessionProperty();
    if (property == nullptr) {
        return nullptr;
    }
    property->SetWindowName(parcel.ReadString());
    Rect rect = { parcel.ReadInt32(), parcel.ReadInt32(), parcel.ReadUint32(), parcel.ReadUint32() };
    property->SetWindowRect(rect);
    Rect reqRect = { parcel.ReadInt32(), parcel.ReadInt32(), parcel.ReadUint32(), parcel.ReadUint32() };
    property->SetRequestRect(reqRect);
    property->SetWindowType(static_cast<WindowType>(parcel.ReadUint32()));
    property->SetFocusable(parcel.ReadBool());
    property->SetTouchable(parcel.ReadBool());
    property->SetTokenState(parcel.ReadBool());
    property->SetTurnScreenOn(parcel.ReadBool());
    property->SetKeepScreenOn(parcel.ReadBool());
    property->SetPrivacyMode(parcel.ReadBool());
    property->SetSystemPrivacyMode(parcel.ReadBool());
    property->SetDisplayId(parcel.ReadUint64());
    property->SetPersistentId(parcel.ReadInt32());
    SessionInfo info = { parcel.ReadString(), parcel.ReadString(), parcel.ReadString() };
    property->SetSessionInfo(info);
    property->SetParentPersistentId(parcel.ReadInt32());
    property->SetAccessTokenId(parcel.ReadUint32());
    property->SetMaximizeMode(static_cast<MaximizeMode>(parcel.ReadUint32()));
    property->SetRequestedOrientation(static_cast<Orientation>(parcel.ReadUint32()));
    property->SetWindowMode(static_cast<WindowMode>(parcel.ReadUint32()));
    property->SetWindowFlags(parcel.ReadUint32());
    property->SetRaiseEnabled(parcel.ReadBool());
    property->SetDecorEnable(parcel.ReadBool());
    property->SetDragEnabled(parcel.ReadBool());
    property->SetHideNonSystemFloatingWindows(parcel.ReadBool());
    property->SetForceHide(parcel.ReadBool());
    UnmarshallingWindowLimits(parcel, property);
    property->SetBrightness(parcel.ReadFloat());
    UnMarshallingSystemBarMap(parcel, property);
    property->SetAnimationFlag(parcel.ReadUint32());
    UnmarshallingPiPTemplateInfo(parcel, property);
    property->SetFloatingWindowAppType(parcel.ReadBool());
    UnmarshallingTouchHotAreas(parcel, property);
    property->SetSystemCalling(parcel.ReadBool());
    property->SetTextFieldPositionY(parcel.ReadDouble());
    property->SetTextFieldHeight(parcel.ReadDouble());
    property->SetWindowState(static_cast<WindowState>(parcel.ReadUint32()));
    property->SetIsNeedUpdateWindowMode(parcel.ReadBool());
    property->SetCallingWindow(parcel.ReadUint32());
    property->SetIsLayoutFullScreen(parcel.ReadBool());
    return property;
}

void WindowSessionProperty::CopyFrom(const sptr<WindowSessionProperty>& property)
{
    windowName_ = property->windowName_;
    sessionInfo_ = property->sessionInfo_;
    requestRect_ = property->requestRect_;
    windowRect_ = property->windowRect_;
    type_ = property->type_;
    focusable_= property->focusable_;
    touchable_ = property->touchable_;
    dragEnabled_ = property->dragEnabled_;
    hideNonSystemFloatingWindows_ = property->hideNonSystemFloatingWindows_;
    forceHide_ = property->forceHide_;
    raiseEnabled_ = property->raiseEnabled_;
    tokenState_ = property->tokenState_;
    turnScreenOn_ = property->turnScreenOn_;
    keepScreenOn_ = property->keepScreenOn_;
    requestedOrientation_ = property->requestedOrientation_;
    isPrivacyMode_ = property->isPrivacyMode_;
    isSystemPrivacyMode_ = property->isSystemPrivacyMode_;
    brightness_ = property->brightness_;
    displayId_ = property->displayId_;
    parentId_ = property->parentId_;
    flags_ = property->flags_;
    persistentId_ = property->persistentId_;
    parentPersistentId_ = property->parentPersistentId_;
    accessTokenId_ = property->accessTokenId_;
    maximizeMode_ = property->maximizeMode_;
    windowMode_ = property->windowMode_;
    limits_ = property->limits_;
    sysBarPropMap_ = property->sysBarPropMap_;
    isDecorEnable_ = property->isDecorEnable_;
    animationFlag_ = property->animationFlag_;
    isFloatingWindowAppType_ = property->isFloatingWindowAppType_;
    touchHotAreas_ = property->touchHotAreas_;
    isSystemCalling_ = property->isSystemCalling_;
    textFieldPositionY_ = property->textFieldPositionY_;
    textFieldHeight_ = property->textFieldHeight_;
    isNeedUpdateWindowMode_ = property->isNeedUpdateWindowMode_;
    isLayoutFullScreen_ = property->isLayoutFullScreen_;
}

void WindowSessionProperty::SetTransform(const Transform& trans)
{
    trans_ = trans;
}

const Transform& WindowSessionProperty::GetTransform() const
{
    return trans_;
}

void WindowSessionProperty::SetTextFieldPositionY(double textFieldPositionY)
{
    textFieldPositionY_ = textFieldPositionY;
}

void WindowSessionProperty::SetTextFieldHeight(double textFieldHeight)
{
    textFieldHeight_ = textFieldHeight;
}

double WindowSessionProperty::GetTextFieldPositionY() const
{
    return textFieldPositionY_;
}

double WindowSessionProperty::GetTextFieldHeight() const
{
    return textFieldHeight_;
}

void WindowSessionProperty::SetSessionPropertyChangeCallback(std::function<void()>&& callback)
{
    touchHotAreasChangeCallback_ = std::move(callback);
}

bool WindowSessionProperty::IsLayoutFullScreen() const
{
    return isLayoutFullScreen_;
}

void WindowSessionProperty::SetIsLayoutFullScreen(bool isLayoutFullScreen)
{
    isLayoutFullScreen_ = isLayoutFullScreen;
}
} // namespace Rosen
} // namespace OHOS
