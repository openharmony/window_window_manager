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
constexpr uint32_t TOUCH_HOT_AREA_MAX_NUM = 50;
constexpr uint32_t MAX_SIZE_PIP_CONTROL_GROUP = 8;
constexpr uint32_t MAX_SIZE_PIP_CONTROL = 9;
}

const std::map<uint32_t, HandlWritePropertyFunc> WindowSessionProperty::writeFuncMap_ {
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON),
        &WindowSessionProperty::WriteActionUpdateTurnScreenOn),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON),
        &WindowSessionProperty::WriteActionUpdateKeepScreenOn),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_FOCUSABLE),
        &WindowSessionProperty::WriteActionUpdateFocusable),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_TOUCHABLE),
        &WindowSessionProperty::WriteActionUpdateTouchable),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS),
        &WindowSessionProperty::WriteActionUpdateSetBrightness),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_ORIENTATION),
        &WindowSessionProperty::WriteActionUpdateOrientation),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE),
        &WindowSessionProperty::WriteActionUpdatePrivacyMode),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_SYSTEM_PRIVACY_MODE),
        &WindowSessionProperty::WriteActionUpdatePrivacyMode),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_SNAPSHOT_SKIP),
        &WindowSessionProperty::WriteActionUpdateSnapshotSkip),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_MAXIMIZE_STATE),
        &WindowSessionProperty::WriteActionUpdateMaximizeState),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_OTHER_PROPS),
        &WindowSessionProperty::WriteActionUpdateSystemBar),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_STATUS_PROPS),
        &WindowSessionProperty::WriteActionUpdateSystemBar),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_NAVIGATION_PROPS),
        &WindowSessionProperty::WriteActionUpdateSystemBar),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_NAVIGATION_INDICATOR_PROPS),
        &WindowSessionProperty::WriteActionUpdateSystemBar),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_FLAGS),
        &WindowSessionProperty::WriteActionUpdateFlags),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_MODE),
        &WindowSessionProperty::WriteActionUpdateMode),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_ANIMATION_FLAG),
        &WindowSessionProperty::WriteActionUpdateAnimationFlag),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_TOUCH_HOT_AREA),
        &WindowSessionProperty::WriteActionUpdateTouchHotArea),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_DECOR_ENABLE),
        &WindowSessionProperty::WriteActionUpdateDecorEnable),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_WINDOW_LIMITS),
        &WindowSessionProperty::WriteActionUpdateWindowLimits),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_DRAGENABLED),
        &WindowSessionProperty::WriteActionUpdateDragenabled),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_RAISEENABLED),
        &WindowSessionProperty::WriteActionUpdateRaiseenabled),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_HIDE_NON_SYSTEM_FLOATING_WINDOWS),
        &WindowSessionProperty::WriteActionUpdateHideNonSystemFloatingWindows),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_TEXTFIELD_AVOID_INFO),
        &WindowSessionProperty::WriteActionUpdateTextfieldAvoidInfo),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_WINDOW_MASK),
        &WindowSessionProperty::WriteActionUpdateWindowMask),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_TOPMOST),
        &WindowSessionProperty::WriteActionUpdateTopmost),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_MODE_SUPPORT_INFO),
        &WindowSessionProperty::WriteActionUpdateModeSupportInfo),
};

const std::map<uint32_t, HandlReadPropertyFunc> WindowSessionProperty::readFuncMap_ {
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON),
        &WindowSessionProperty::ReadActionUpdateTurnScreenOn),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON),
        &WindowSessionProperty::ReadActionUpdateKeepScreenOn),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_FOCUSABLE),
        &WindowSessionProperty::ReadActionUpdateFocusable),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_TOUCHABLE),
        &WindowSessionProperty::ReadActionUpdateTouchable),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS),
        &WindowSessionProperty::ReadActionUpdateSetBrightness),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_ORIENTATION),
        &WindowSessionProperty::ReadActionUpdateOrientation),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE),
        &WindowSessionProperty::ReadActionUpdatePrivacyMode),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_SYSTEM_PRIVACY_MODE),
        &WindowSessionProperty::ReadActionUpdatePrivacyMode),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_SNAPSHOT_SKIP),
        &WindowSessionProperty::ReadActionUpdateSnapshotSkip),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_MAXIMIZE_STATE),
        &WindowSessionProperty::ReadActionUpdateMaximizeState),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_OTHER_PROPS),
        &WindowSessionProperty::ReadActionUpdateSystemBar),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_STATUS_PROPS),
        &WindowSessionProperty::ReadActionUpdateSystemBar),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_NAVIGATION_PROPS),
        &WindowSessionProperty::ReadActionUpdateSystemBar),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_NAVIGATION_INDICATOR_PROPS),
        &WindowSessionProperty::ReadActionUpdateSystemBar),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_FLAGS),
        &WindowSessionProperty::ReadActionUpdateFlags),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_MODE),
        &WindowSessionProperty::ReadActionUpdateMode),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_ANIMATION_FLAG),
        &WindowSessionProperty::ReadActionUpdateAnimationFlag),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_TOUCH_HOT_AREA),
        &WindowSessionProperty::ReadActionUpdateTouchHotArea),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_DECOR_ENABLE),
        &WindowSessionProperty::ReadActionUpdateDecorEnable),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_WINDOW_LIMITS),
        &WindowSessionProperty::ReadActionUpdateWindowLimits),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_DRAGENABLED),
        &WindowSessionProperty::ReadActionUpdateDragenabled),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_RAISEENABLED),
        &WindowSessionProperty::ReadActionUpdateRaiseenabled),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_HIDE_NON_SYSTEM_FLOATING_WINDOWS),
        &WindowSessionProperty::ReadActionUpdateHideNonSystemFloatingWindows),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_TEXTFIELD_AVOID_INFO),
        &WindowSessionProperty::ReadActionUpdateTextfieldAvoidInfo),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_WINDOW_MASK),
        &WindowSessionProperty::ReadActionUpdateWindowMask),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_TOPMOST),
        &WindowSessionProperty::ReadActionUpdateTopmost),
    std::make_pair(static_cast<uint32_t>(WSPropertyChangeAction::ACTION_UPDATE_MODE_SUPPORT_INFO),
        &WindowSessionProperty::ReadActionUpdateModeSupportInfo),
};

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

void WindowSessionProperty::SetDefaultRequestedOrientation(Orientation orientation)
{
    defaultRequestedOrientation_ = orientation;
}

void WindowSessionProperty::SetPrivacyMode(bool isPrivate)
{
    isPrivacyMode_ = isPrivate;
}

void WindowSessionProperty::SetSystemPrivacyMode(bool isSystemPrivate)
{
    isSystemPrivacyMode_ = isSystemPrivate;
}

void WindowSessionProperty::SetSnapshotSkip(bool isSkip)
{
    isSnapshotSkip_ = isSkip;
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

SessionInfo& WindowSessionProperty::EditSessionInfo()
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

Orientation WindowSessionProperty::GetDefaultRequestedOrientation() const
{
    return defaultRequestedOrientation_;
}

bool WindowSessionProperty::GetPrivacyMode() const
{
    return isPrivacyMode_;
}

bool WindowSessionProperty::GetSystemPrivacyMode() const
{
    return isSystemPrivacyMode_;
}

bool WindowSessionProperty::GetSnapshotSkip() const
{
    return isSnapshotSkip_;
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

void WindowSessionProperty::SetTopmost(bool topmost)
{
    topmost_ = topmost;
}

bool WindowSessionProperty::IsTopmost() const
{
    return topmost_;
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

void WindowSessionProperty::SetKeyboardSessionGravity(SessionGravity gravity, uint32_t percent)
{
    sessionGravity_ = gravity;
    sessionGravitySizePercent_ = percent;
}

void WindowSessionProperty::GetSessionGravity(SessionGravity& gravity, uint32_t& percent)
{
    gravity = sessionGravity_;
    percent = sessionGravitySizePercent_;
}

void WindowSessionProperty::SetKeyboardLayoutParams(const KeyboardLayoutParams& params)
{
    keyboardLayoutParams_.gravity_ = params.gravity_;
    keyboardLayoutParams_.LandscapeKeyboardRect_ = params.LandscapeKeyboardRect_;
    keyboardLayoutParams_.PortraitKeyboardRect_ = params.PortraitKeyboardRect_;
    keyboardLayoutParams_.LandscapePanelRect_ = params.LandscapePanelRect_;
    keyboardLayoutParams_.PortraitPanelRect_ = params.PortraitPanelRect_;
}

KeyboardLayoutParams WindowSessionProperty::GetKeyboardLayoutParams() const
{
    return keyboardLayoutParams_;
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
    {
        std::lock_guard<std::mutex> lock(touchHotAreasMutex_);
        if (GetPersistentId() != 0 && rects != touchHotAreas_) {
            std::string rectStr;
            for (const auto& rect : rects) {
                rectStr = rectStr + " hot : [ " + std::to_string(rect.posX_) + " , " + std::to_string(rect.posY_) +
                    " , " + std::to_string(rect.width_) + " , " + std::to_string(rect.height_) + "]";
            }
            TLOGI(WmsLogTag::WMS_EVENT, "id:%{public}d rects:%{public}s", GetPersistentId(), rectStr.c_str());
        }
        touchHotAreas_ = rects;
    }
    if (touchHotAreasChangeCallback_) {
        touchHotAreasChangeCallback_();
    }
}

void WindowSessionProperty::GetTouchHotAreas(std::vector<Rect>& rects) const
{
    std::lock_guard<std::mutex> lock(touchHotAreasMutex_);
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

void WindowSessionProperty::SetCallingSessionId(uint32_t sessionId)
{
    callingSessionId_ = sessionId;
}

uint32_t WindowSessionProperty::GetCallingSessionId() const
{
    return callingSessionId_;
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
        parcel.WriteFloat(limits_.minRatio_) && parcel.WriteFloat(limits_.vpRatio_)) {
        return true;
    }
    return false;
}

void WindowSessionProperty::UnmarshallingWindowLimits(Parcel& parcel, WindowSessionProperty* property)
{
    WindowLimits windowLimits = { parcel.ReadUint32(), parcel.ReadUint32(), parcel.ReadUint32(),
        parcel.ReadUint32(), parcel.ReadFloat(), parcel.ReadFloat(), parcel.ReadFloat() };
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
              parcel.WriteUint32(it.second.contentColor_) && parcel.WriteBool(it.second.enableAnimation_) &&
              parcel.WriteUint32(static_cast<uint32_t>(it.second.settingFlag_)))) {
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
        SystemBarProperty prop = { parcel.ReadBool(), parcel.ReadUint32(), parcel.ReadUint32(), parcel.ReadBool(),
            static_cast<SystemBarSettingFlag>(parcel.ReadUint32()) };
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
    if (size > MAX_SIZE_PIP_CONTROL_GROUP) {
        return false;
    }
    if (!parcel.WriteUint32(static_cast<uint32_t>(size))) {
        return false;
    }
    for (uint32_t i = 0; i < size; i++) {
        if (!parcel.WriteUint32(pipTemplateInfo_.controlGroup[i])) {
            return false;
        }
    }
    auto controlStatusSize = pipTemplateInfo_.pipControlStatusInfoList.size();
    if (controlStatusSize > MAX_SIZE_PIP_CONTROL) {
        return false;
    }
    if (!parcel.WriteUint32(static_cast<uint32_t>(controlStatusSize))) {
        return false;
    }
    for (uint32_t i = 0; i < controlStatusSize; i++) {
        if (!parcel.WriteUint32(static_cast<uint32_t>(pipTemplateInfo_.pipControlStatusInfoList[i].controlType)) ||
            !parcel.WriteInt32(static_cast<int32_t>(pipTemplateInfo_.pipControlStatusInfoList[i].status))) {
            return false;
        }
    }
    auto controlEnableSize = pipTemplateInfo_.pipControlEnableInfoList.size();
    if (controlEnableSize > MAX_SIZE_PIP_CONTROL) {
        return false;
    }
    if (!parcel.WriteUint32(static_cast<uint32_t>(controlEnableSize))) {
        return false;
    }
    for (uint32_t i = 0; i < controlEnableSize; i++) {
        if (!parcel.WriteUint32(static_cast<uint32_t>(pipTemplateInfo_.pipControlEnableInfoList[i].controlType)) ||
            !parcel.WriteInt32(static_cast<int32_t>(pipTemplateInfo_.pipControlEnableInfoList[i].enabled))) {
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
    if (size > MAX_SIZE_PIP_CONTROL_GROUP) {
        return;
    }
    for (uint32_t i = 0; i < size; i++) {
        uint32_t controlGroupId = 0;
        if (!parcel.ReadUint32(controlGroupId)) {
            return;
        }
        pipTemplateInfo.controlGroup.push_back(controlGroupId);
    }
    auto controlStatusSize = parcel.ReadUint32();
    if (controlStatusSize > MAX_SIZE_PIP_CONTROL) {
        return;
    }
    for (uint32_t i = 0; i < controlStatusSize; i++) {
        PiPControlStatusInfo pipControlStatusInfo;
        uint32_t controlType = 0;
        int32_t status = 0;
        if (!parcel.ReadUint32(controlType) || !parcel.ReadInt32(status)) {
            return;
        }
        pipControlStatusInfo.controlType = static_cast<PiPControlType>(controlType);
        pipControlStatusInfo.status = static_cast<PiPControlStatus>(status);
        pipTemplateInfo.pipControlStatusInfoList.push_back(pipControlStatusInfo);
    }
    auto controlEnableSize = parcel.ReadUint32();
    if (controlEnableSize > MAX_SIZE_PIP_CONTROL) {
        return;
    }
    for (uint32_t i = 0; i < controlEnableSize; i++) {
        PiPControlEnableInfo pipControlEnableInfo;
        uint32_t controlType = 0;
        int32_t enabled = 0;
        if (!parcel.ReadUint32(controlType) || !parcel.ReadInt32(enabled)) {
            return;
        }
        pipControlEnableInfo.controlType = static_cast<PiPControlType>(controlType);
        pipControlEnableInfo.enabled = static_cast<PiPControlStatus>(enabled);
        pipTemplateInfo.pipControlEnableInfoList.push_back(pipControlEnableInfo);
    }
    property->SetPiPTemplateInfo(pipTemplateInfo);
}

bool WindowSessionProperty::MarshallingWindowMask(Parcel& parcel) const
{
    if (!parcel.WriteBool(isShaped_)) {
        return false;
    }
    if (isShaped_) {
        if (!windowMask_->Marshalling(parcel)) {
            return false;
        }
    }
    return true;
}

void WindowSessionProperty::UnmarshallingWindowMask(Parcel& parcel, WindowSessionProperty* property)
{
    bool isShaped = parcel.ReadBool();
    property->SetIsShaped(isShaped);
    if (isShaped) {
        Media::PixelMap* windowMask = Media::PixelMap::Unmarshalling(parcel);
        if (windowMask != nullptr) {
            property->SetWindowMask(std::shared_ptr<Media::PixelMap>(windowMask));
        }
    }
}

bool WindowSessionProperty::MarshallingSessionInfo(Parcel& parcel) const
{
    if (!parcel.WriteString(sessionInfo_.bundleName_) || !parcel.WriteString(sessionInfo_.moduleName_) ||
        !parcel.WriteString(sessionInfo_.abilityName_) ||
        !parcel.WriteInt32(static_cast<int32_t>(sessionInfo_.continueState))) {
        return false;
    }
    const auto& want = sessionInfo_.want;
    bool hasWant = want != nullptr;
    if (!parcel.WriteBool(hasWant)) {
        return false;
    }
    if (hasWant && !parcel.WriteParcelable(want.get())) {
        return false;
    }
    return true;
}

bool WindowSessionProperty::UnmarshallingSessionInfo(Parcel& parcel, WindowSessionProperty* property)
{
    std::string bundleName;
    std::string moduleName;
    std::string abilityName;
    if (!parcel.ReadString(bundleName) || !parcel.ReadString(moduleName) || !parcel.ReadString(abilityName)) {
        TLOGE(WmsLogTag::DEFAULT, "Failed to read String!");
        return false;
    }
    SessionInfo info = { bundleName, moduleName, abilityName };
    int32_t continueState;
    if (!parcel.ReadInt32(continueState)) {
        TLOGE(WmsLogTag::DEFAULT, "Failed to read continueState!");
        return false;
    }
    info.continueState = static_cast<ContinueState>(continueState);
    bool hasWant;
    if (!parcel.ReadBool(hasWant)) {
        TLOGE(WmsLogTag::DEFAULT, "Failed to read hasWant!");
        return false;
    }
    if (hasWant) {
        std::shared_ptr<AAFwk::Want> want(parcel.ReadParcelable<AAFwk::Want>());
        if (want == nullptr) {
            TLOGE(WmsLogTag::DEFAULT, "Failed to read want!");
            return false;
        }
        info.want = want;
    }
    property->SetSessionInfo(info);
    return true;
}

void WindowSessionProperty::SetCompatibleModeInPc(bool compatibleModeInPc)
{
    compatibleModeInPc_ = compatibleModeInPc;
}

bool WindowSessionProperty::GetCompatibleModeInPc() const
{
    return compatibleModeInPc_;
}

void WindowSessionProperty::SetCompatibleWindowSizeInPc(int32_t portraitWidth,
    int32_t portraitHeight, int32_t landscapeWidth, int32_t landscapeHeight)
{
    compatibleInPcPortraitWidth_ = portraitWidth;
    compatibleInPcPortraitHeight_ = portraitHeight;
    compatibleInPcLandscapeWidth_ = landscapeWidth;
    compatibleInPcLandscapeHeight_ = landscapeHeight;
}

int32_t WindowSessionProperty::GetCompatibleInPcPortraitWidth() const
{
    return compatibleInPcPortraitWidth_;
}

int32_t WindowSessionProperty::GetCompatibleInPcPortraitHeight() const
{
    return compatibleInPcPortraitHeight_;
}

int32_t WindowSessionProperty::GetCompatibleInPcLandscapeWidth() const
{
    return compatibleInPcLandscapeWidth_;
}

int32_t WindowSessionProperty::GetCompatibleInPcLandscapeHeight() const
{
    return compatibleInPcLandscapeHeight_;
}

void WindowSessionProperty::SetIsAppSupportPhoneInPc(bool isSupportPhone)
{
    isAppSupportPhoneInPc_ = isSupportPhone;
}

bool WindowSessionProperty::GetIsAppSupportPhoneInPc() const
{
    return isAppSupportPhoneInPc_;
}

void WindowSessionProperty::SetIsPcAppInPad(bool isPcAppInPad)
{
    isPcAppInPad_ = isPcAppInPad;
}

bool WindowSessionProperty::GetIsPcAppInPad() const
{
    return isPcAppInPad_;
}

void WindowSessionProperty::SetSubWindowLevel(uint32_t subWindowLevel)
{
    subWindowLevel_ = subWindowLevel;
}

uint32_t WindowSessionProperty::GetSubWindowLevel() const
{
    return subWindowLevel_;
}

void WindowSessionProperty::SetIsSupportDragInPcCompatibleMode(bool isSupportDragInPcCompatibleMode)
{
    isSupportDragInPcCompatibleMode_ = isSupportDragInPcCompatibleMode;
}

bool WindowSessionProperty::GetIsSupportDragInPcCompatibleMode() const
{
    return isSupportDragInPcCompatibleMode_;
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
        parcel.WriteBool(isSnapshotSkip_) &&
        parcel.WriteUint64(displayId_) && parcel.WriteInt32(persistentId_) &&
        MarshallingSessionInfo(parcel) &&
        parcel.WriteInt32(parentPersistentId_) &&
        parcel.WriteUint32(accessTokenId_) && parcel.WriteUint32(static_cast<uint32_t>(maximizeMode_)) &&
        parcel.WriteUint32(static_cast<uint32_t>(requestedOrientation_)) &&
        parcel.WriteUint32(static_cast<uint32_t>(windowMode_)) &&
        parcel.WriteUint32(flags_) && parcel.WriteBool(raiseEnabled_) && parcel.WriteBool(topmost_) &&
        parcel.WriteBool(isDecorEnable_) && parcel.WriteBool(dragEnabled_) &&
        parcel.WriteBool(hideNonSystemFloatingWindows_) && parcel.WriteBool(forceHide_) &&
        MarshallingWindowLimits(parcel) && parcel.WriteFloat(brightness_) &&
        MarshallingSystemBarMap(parcel) && parcel.WriteUint32(animationFlag_) &&
        MarshallingPiPTemplateInfo(parcel) &&
        parcel.WriteBool(isFloatingWindowAppType_) && MarshallingTouchHotAreas(parcel) &&
        parcel.WriteBool(isSystemCalling_) &&
        parcel.WriteUint32(static_cast<uint32_t>(sessionGravity_)) && parcel.WriteUint32(sessionGravitySizePercent_) &&
        parcel.WriteDouble(textFieldPositionY_) && parcel.WriteDouble(textFieldHeight_) &&
        parcel.WriteUint32(static_cast<uint32_t>(windowState_)) &&
        parcel.WriteBool(isNeedUpdateWindowMode_) && parcel.WriteUint32(callingSessionId_) &&
        parcel.WriteBool(isLayoutFullScreen_) &&
        parcel.WriteInt32(realParentId_) &&
        parcel.WriteBool(isExtensionFlag_) &&
        parcel.WriteBool(isUIExtensionAbilityProcess_) &&
        parcel.WriteUint32(static_cast<uint32_t>(uiExtensionUsage_)) &&
        parcel.WriteUint32(static_cast<uint32_t>(parentWindowType_)) &&
        MarshallingWindowMask(parcel) &&
        parcel.WriteParcelable(&keyboardLayoutParams_) &&
        parcel.WriteBool(compatibleModeInPc_) &&
        parcel.WriteInt32(compatibleInPcPortraitWidth_) && parcel.WriteInt32(compatibleInPcPortraitHeight_) &&
        parcel.WriteInt32(compatibleInPcLandscapeWidth_) && parcel.WriteInt32(compatibleInPcLandscapeHeight_) &&
        parcel.WriteBool(isAppSupportPhoneInPc_) &&
        parcel.WriteBool(isSupportDragInPcCompatibleMode_) &&
        parcel.WriteBool(isPcAppInPad_);
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
    property->SetSnapshotSkip(parcel.ReadBool());
    property->SetDisplayId(parcel.ReadUint64());
    property->SetPersistentId(parcel.ReadInt32());
    if (!UnmarshallingSessionInfo(parcel, property)) {
        delete property;
        return nullptr;
    }
    property->SetParentPersistentId(parcel.ReadInt32());
    property->SetAccessTokenId(parcel.ReadUint32());
    property->SetMaximizeMode(static_cast<MaximizeMode>(parcel.ReadUint32()));
    property->SetRequestedOrientation(static_cast<Orientation>(parcel.ReadUint32()));
    property->SetWindowMode(static_cast<WindowMode>(parcel.ReadUint32()));
    property->SetWindowFlags(parcel.ReadUint32());
    property->SetRaiseEnabled(parcel.ReadBool());
    property->SetTopmost(parcel.ReadBool());
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
    property->SetKeyboardSessionGravity(static_cast<SessionGravity>(parcel.ReadUint32()), parcel.ReadUint32());
    property->SetTextFieldPositionY(parcel.ReadDouble());
    property->SetTextFieldHeight(parcel.ReadDouble());
    property->SetWindowState(static_cast<WindowState>(parcel.ReadUint32()));
    property->SetIsNeedUpdateWindowMode(parcel.ReadBool());
    property->SetCallingSessionId(parcel.ReadUint32());
    property->SetIsLayoutFullScreen(parcel.ReadBool());
    property->SetRealParentId(parcel.ReadInt32());
    property->SetExtensionFlag(parcel.ReadBool());
    property->SetIsUIExtensionAbilityProcess(parcel.ReadBool());
    property->SetUIExtensionUsage(static_cast<UIExtensionUsage>(parcel.ReadUint32()));
    property->SetParentWindowType(static_cast<WindowType>(parcel.ReadUint32()));
    UnmarshallingWindowMask(parcel, property);
    sptr<KeyboardLayoutParams> keyboardLayoutParams = parcel.ReadParcelable<KeyboardLayoutParams>();
    if (keyboardLayoutParams == nullptr) {
        delete property;
        return nullptr;
    }
    property->SetKeyboardLayoutParams(*keyboardLayoutParams);
    property->SetCompatibleModeInPc(parcel.ReadBool());
    property->SetCompatibleWindowSizeInPc(parcel.ReadInt32(), parcel.ReadInt32(),
                                          parcel.ReadInt32(), parcel.ReadInt32());
    property->SetIsAppSupportPhoneInPc(parcel.ReadBool());
    property->SetIsSupportDragInPcCompatibleMode(parcel.ReadBool());
    property->SetIsPcAppInPad(parcel.ReadBool());
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
    topmost_ = property->topmost_;
    tokenState_ = property->tokenState_;
    turnScreenOn_ = property->turnScreenOn_;
    keepScreenOn_ = property->keepScreenOn_;
    requestedOrientation_ = property->requestedOrientation_;
    defaultRequestedOrientation_ = property->defaultRequestedOrientation_;
    isPrivacyMode_ = property->isPrivacyMode_;
    isSystemPrivacyMode_ = property->isSystemPrivacyMode_;
    isSnapshotSkip_ = property->isSnapshotSkip_;
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
    callingSessionId_ = property->callingSessionId_;
    isLayoutFullScreen_ = property->isLayoutFullScreen_;
    windowMask_ = property->windowMask_;
    isShaped_ = property->isShaped_;
}

bool WindowSessionProperty::Write(Parcel& parcel, WSPropertyChangeAction action)
{
    const auto funcIter = writeFuncMap_.find(static_cast<uint32_t>(action));
    if (funcIter == writeFuncMap_.end()) {
        TLOGE(WmsLogTag::DEFAULT, "Failed to find func handler!");
        return false;
    }
    bool ret = parcel.WriteUint32(static_cast<uint32_t>(persistentId_));
    return ret && (this->*(funcIter->second))(parcel);
}

bool WindowSessionProperty::WriteActionUpdateTurnScreenOn(Parcel& parcel)
{
    return parcel.WriteBool(turnScreenOn_);
}

bool WindowSessionProperty::WriteActionUpdateKeepScreenOn(Parcel& parcel)
{
    return parcel.WriteBool(keepScreenOn_);
}

bool WindowSessionProperty::WriteActionUpdateFocusable(Parcel& parcel)
{
    return parcel.WriteBool(focusable_);
}

bool WindowSessionProperty::WriteActionUpdateTouchable(Parcel& parcel)
{
    return parcel.WriteBool(touchable_);
}

bool WindowSessionProperty::WriteActionUpdateSetBrightness(Parcel& parcel)
{
    return parcel.WriteFloat(brightness_);
}

bool WindowSessionProperty::WriteActionUpdateOrientation(Parcel& parcel)
{
    return parcel.WriteUint32(static_cast<uint32_t>(requestedOrientation_));
}

bool WindowSessionProperty::WriteActionUpdatePrivacyMode(Parcel& parcel)
{
    return parcel.WriteBool(isPrivacyMode_) && parcel.WriteBool(isSystemPrivacyMode_);
}

bool WindowSessionProperty::WriteActionUpdateSnapshotSkip(Parcel& parcel)
{
    return parcel.WriteBool(isSnapshotSkip_);
}

bool WindowSessionProperty::WriteActionUpdateMaximizeState(Parcel& parcel)
{
    return parcel.WriteUint32(static_cast<uint32_t>(maximizeMode_)) &&
        parcel.WriteBool(isLayoutFullScreen_);
}

bool WindowSessionProperty::WriteActionUpdateSystemBar(Parcel& parcel)
{
    return MarshallingSystemBarMap(parcel);
}

bool WindowSessionProperty::WriteActionUpdateFlags(Parcel& parcel)
{
    return parcel.WriteUint32(flags_);
}

bool WindowSessionProperty::WriteActionUpdateMode(Parcel& parcel)
{
    return parcel.WriteUint32(static_cast<uint32_t>(windowMode_));
}

bool WindowSessionProperty::WriteActionUpdateAnimationFlag(Parcel& parcel)
{
    return parcel.WriteUint32(animationFlag_);
}

bool WindowSessionProperty::WriteActionUpdateTouchHotArea(Parcel& parcel)
{
    return MarshallingTouchHotAreas(parcel);
}

bool WindowSessionProperty::WriteActionUpdateDecorEnable(Parcel& parcel)
{
    return parcel.WriteBool(isDecorEnable_);
}

bool WindowSessionProperty::WriteActionUpdateWindowLimits(Parcel& parcel)
{
    return MarshallingWindowLimits(parcel);
}

bool WindowSessionProperty::WriteActionUpdateDragenabled(Parcel& parcel)
{
    return parcel.WriteBool(dragEnabled_);
}

bool WindowSessionProperty::WriteActionUpdateRaiseenabled(Parcel& parcel)
{
    return parcel.WriteBool(raiseEnabled_);
}

bool WindowSessionProperty::WriteActionUpdateHideNonSystemFloatingWindows(Parcel& parcel)
{
    return parcel.WriteBool(hideNonSystemFloatingWindows_) &&
        parcel.WriteBool(isFloatingWindowAppType_) && parcel.WriteBool(forceHide_);
}

bool WindowSessionProperty::WriteActionUpdateTextfieldAvoidInfo(Parcel& parcel)
{
    return parcel.WriteDouble(textFieldPositionY_) && parcel.WriteDouble(textFieldHeight_);
}

bool WindowSessionProperty::WriteActionUpdateWindowMask(Parcel& parcel)
{
    return MarshallingWindowMask(parcel);
}

bool WindowSessionProperty::WriteActionUpdateTopmost(Parcel& parcel)
{
    return parcel.WriteBool(topmost_);
}


bool WindowSessionProperty::WriteActionUpdateModeSupportInfo(Parcel& parcel)
{
    return parcel.WriteUint32(modeSupportInfo_);
}

void WindowSessionProperty::Read(Parcel& parcel, WSPropertyChangeAction action)
{
    const auto funcIter = readFuncMap_.find(static_cast<uint32_t>(action));
    if (funcIter == readFuncMap_.end()) {
        TLOGE(WmsLogTag::DEFAULT, "Failed to find func handler!");
        return;
    }
    SetPersistentId(parcel.ReadUint32());
    (this->*(funcIter->second))(parcel);
}

void WindowSessionProperty::ReadActionUpdateTurnScreenOn(Parcel& parcel)
{
    SetTurnScreenOn(parcel.ReadBool());
}

void WindowSessionProperty::ReadActionUpdateKeepScreenOn(Parcel& parcel)
{
    SetKeepScreenOn(parcel.ReadBool());
}

void WindowSessionProperty::ReadActionUpdateFocusable(Parcel& parcel)
{
    SetFocusable(parcel.ReadBool());
}

void WindowSessionProperty::ReadActionUpdateTouchable(Parcel& parcel)
{
    SetTouchable(parcel.ReadBool());
}

void WindowSessionProperty::ReadActionUpdateSetBrightness(Parcel& parcel)
{
    SetBrightness(parcel.ReadFloat());
}

void WindowSessionProperty::ReadActionUpdateOrientation(Parcel& parcel)
{
    SetRequestedOrientation(static_cast<Orientation>(parcel.ReadUint32()));
}

void WindowSessionProperty::ReadActionUpdatePrivacyMode(Parcel& parcel)
{
    SetPrivacyMode(parcel.ReadBool());
    SetSystemPrivacyMode(parcel.ReadBool());
}

void WindowSessionProperty::ReadActionUpdateSnapshotSkip(Parcel& parcel)
{
    SetSnapshotSkip(parcel.ReadBool());
}

void WindowSessionProperty::ReadActionUpdateMaximizeState(Parcel& parcel)
{
    SetMaximizeMode(static_cast<MaximizeMode>(parcel.ReadUint32()));
    SetIsLayoutFullScreen(parcel.ReadBool());
}

void WindowSessionProperty::ReadActionUpdateSystemBar(Parcel& parcel)
{
    UnMarshallingSystemBarMap(parcel, this);
}

void WindowSessionProperty::ReadActionUpdateFlags(Parcel& parcel)
{
    SetWindowFlags(parcel.ReadUint32());
}

void WindowSessionProperty::ReadActionUpdateMode(Parcel& parcel)
{
    SetWindowMode(static_cast<WindowMode>(parcel.ReadUint32()));
}

void WindowSessionProperty::ReadActionUpdateAnimationFlag(Parcel& parcel)
{
    SetAnimationFlag(parcel.ReadUint32());
}

void WindowSessionProperty::ReadActionUpdateTouchHotArea(Parcel& parcel)
{
    UnmarshallingTouchHotAreas(parcel, this);
}

void WindowSessionProperty::ReadActionUpdateDecorEnable(Parcel& parcel)
{
    SetDecorEnable(parcel.ReadBool());
}

void WindowSessionProperty::ReadActionUpdateWindowLimits(Parcel& parcel)
{
    UnmarshallingWindowLimits(parcel, this);
}

void WindowSessionProperty::ReadActionUpdateDragenabled(Parcel& parcel)
{
    SetDragEnabled(parcel.ReadBool());
}

void WindowSessionProperty::ReadActionUpdateRaiseenabled(Parcel& parcel)
{
    SetRaiseEnabled(parcel.ReadBool());
}

void WindowSessionProperty::ReadActionUpdateHideNonSystemFloatingWindows(Parcel& parcel)
{
    SetHideNonSystemFloatingWindows(parcel.ReadBool());
    SetFloatingWindowAppType(parcel.ReadBool());
    SetForceHide(parcel.ReadBool());
}

void WindowSessionProperty::ReadActionUpdateTextfieldAvoidInfo(Parcel& parcel)
{
    SetTextFieldPositionY(parcel.ReadDouble());
    SetTextFieldHeight(parcel.ReadDouble());
}

void WindowSessionProperty::ReadActionUpdateWindowMask(Parcel& parcel)
{
    UnmarshallingWindowMask(parcel, this);
}

void WindowSessionProperty::ReadActionUpdateTopmost(Parcel& parcel)
{
    SetTopmost(parcel.ReadBool());
}

void WindowSessionProperty::ReadActionUpdateModeSupportInfo(Parcel& parcel)
{
    SetModeSupportInfo(parcel.ReadUint32());
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

void WindowSessionProperty::SetRealParentId(int32_t realParentId)
{
    realParentId_ = realParentId;
}

int32_t WindowSessionProperty::GetRealParentId() const
{
    return realParentId_;
}

void WindowSessionProperty::SetExtensionFlag(bool isExtensionFlag)
{
    isExtensionFlag_ = isExtensionFlag;
}

bool WindowSessionProperty::GetExtensionFlag() const
{
    return isExtensionFlag_;
}

void WindowSessionProperty::SetIsUIExtensionAbilityProcess(bool isUIExtensionAbilityProcess)
{
    isUIExtensionAbilityProcess_ = isUIExtensionAbilityProcess;
}

bool WindowSessionProperty::GetIsUIExtensionAbilityProcess() const
{
    return isUIExtensionAbilityProcess_;
}

void WindowSessionProperty::SetUIExtensionUsage(UIExtensionUsage uiExtensionUsage)
{
    uiExtensionUsage_ = uiExtensionUsage;
}

UIExtensionUsage WindowSessionProperty::GetUIExtensionUsage() const
{
    return uiExtensionUsage_;
}

void WindowSessionProperty::SetParentWindowType(WindowType parentWindowType)
{
    parentWindowType_= parentWindowType;
}

WindowType WindowSessionProperty::GetParentWindowType() const
{
    return parentWindowType_;
}

void WindowSessionProperty::SetWindowMask(const std::shared_ptr<Media::PixelMap>& windowMask)
{
    windowMask_ = windowMask;
}

std::shared_ptr<Media::PixelMap> WindowSessionProperty::GetWindowMask() const
{
    return windowMask_;
}

void WindowSessionProperty::SetIsShaped(bool isShaped)
{
    isShaped_ = isShaped;
}

bool WindowSessionProperty::GetIsShaped() const
{
    return isShaped_;
}

int32_t WindowSessionProperty::GetCollaboratorType() const
{
    return collaboratorType_;
}

void WindowSessionProperty::SetCollaboratorType(int32_t collaboratorType)
{
    collaboratorType_ = collaboratorType;
}

void WindowSessionProperty::SetUserWindowLimits(const WindowLimits& windowUserLimits)
{
    userLimits_ = windowUserLimits;
}

WindowLimits WindowSessionProperty::GetUserWindowLimits() const
{
    return userLimits_;
}

void WindowSessionProperty::SetConfigWindowLimitsVP(const WindowLimits& windowConfigLimitsVP)
{
    configLimitsVP_ = windowConfigLimitsVP;
}

WindowLimits WindowSessionProperty::GetConfigWindowLimitsVP() const
{
    return configLimitsVP_;
}

void WindowSessionProperty::SetLastLimitsVpr(float vpr)
{
    lastVpr_ = vpr;
}

float WindowSessionProperty::GetLastLimitsVpr() const
{
    return lastVpr_;
}

bool WindowSessionProperty::GetFullScreenStart() const
{
    return fullScreenStart_;
}

void WindowSessionProperty::SetFullScreenStart(bool fullScreenStart)
{
    fullScreenStart_ = fullScreenStart;
}
} // namespace Rosen
} // namespace OHOS
