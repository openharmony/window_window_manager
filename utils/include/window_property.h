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

#ifndef OHOS_ROSEN_WINDOW_PROPERTY_H
#define OHOS_ROSEN_WINDOW_PROPERTY_H

#include <refbase.h>
#include <string>
#include <unordered_map>
#include "parcel.h"

#include "class_var_definition.h"
#include "dm_common.h"
#include "wm_common.h"
#include "wm_common_inner.h"
#include "serialize_helper.h"

namespace OHOS {
namespace Rosen {
class WindowProperty : public Parcelable {
public:
    enum WindowPropertyReplicationState : uint64_t {
        WPRS_WindowName = 1 << 0,
        WPRS_WindowRect = 1 << 1,
        WPRS_RequestRect = 1 << 2,
        WPRS_DecoStatus = 1 << 3,
        WPRS_Type = 1 << 4,
        WPRS_Mode = 1 << 5,
        WPRS_LastMode = 1 << 6,
        WPRS_Level = 1 << 7,
        WPRS_Flags = 1 << 8,
        WPRS_IsFullScreen = 1 << 9,
        WPRS_Focusable = 1 << 10,
        WPRS_Touchable = 1 << 11,
        WPRS_IsPrivacyMode = 1 << 12,
        WPRS_IsTransparent = 1 << 13,
        WPRS_Alpha = 1 << 14,
        WPRS_Brightness = 1 << 15,
        WPRS_DisplayId = 1 << 16,
        WPRS_WindowId = 1 << 17,
        WPRS_ParentId = 1 << 18,
        WPRS_SysBarPropMap = 1 << 19,
        WPRS_IsDecorEnable = 1 << 20,
        WPRS_HitOffset = 1 << 21,
        WPRS_AnimationFlag = 1 << 22,
        WPRS_WindowSizeChangeReason = 1 << 23,
        WPRS_TokenState = 1 << 24,
        WPRS_CallingWindow = 1 << 25,
        WPRS_RequestedOrientation = 1 << 26,
        WPRS_TurnScreenOn = 1 << 27,
        WPRS_KeepScreenOn = 1 << 28,
        WPRS_ModeSupportInfo = 1 << 29,
        WPRS_DragType = 1 << 30,
        WPRS_OriginRect = static_cast<uint64_t>(1) << 31,
        WPRS_IsStretchable = static_cast<uint64_t>(1) << 32,
    };
    WindowProperty() = default;
    WindowProperty(const sptr<WindowProperty>& property);
    ~WindowProperty() = default;

    void CopyFrom(const sptr<WindowProperty>& property);

    void SetWindowName(const std::string& name);
    void SetRequestRect(const struct Rect& rect);
    void SetWindowRect(const struct Rect& rect);
    void SetDecoStatus(bool decoStatus);
    void SetWindowHotZoneRect(const struct Rect& rect);
    void SetWindowType(WindowType type);
    void SetWindowMode(WindowMode mode);
    void SetLastWindowMode(WindowMode mode);
    void SetWindowBackgroundBlur(WindowBlurLevel level);
    void ResumeLastWindowMode();
    void SetFullScreen(bool isFullScreen);
    void SetFocusable(bool isFocusable);
    void SetTouchable(bool isTouchable);
    void SetPrivacyMode(bool isPrivate);
    void SetTransparent(bool isTransparent);
    void SetAlpha(float alpha);
    void SetBrightness(float brightness);
    void SetTurnScreenOn(bool turnScreenOn);
    void SetKeepScreenOn(bool keepScreenOn);
    void SetCallingWindow(uint32_t windowId);
    void SetDisplayId(DisplayId displayId);
    void SetWindowId(uint32_t windowId);
    void SetParentId(uint32_t parentId);
    void SetWindowFlags(uint32_t flags);
    void AddWindowFlag(WindowFlag flag);
    void SetSystemBarProperty(WindowType type, const SystemBarProperty& state);
    void SetDecorEnable(bool decorEnable);
    void SetHitOffset(const PointInfo& offset);
    void SetAnimationFlag(uint32_t animationFlag);
    void SetWindowSizeChangeReason(WindowSizeChangeReason reason);
    void SetTokenState(bool hasToken);
    void SetModeSupportInfo(uint32_t modeSupportInfo);
    void SetDragType(DragType dragType);
    void SetStretchable(bool stretchable);
    void SetOriginRect(const Rect& rect);
    WindowSizeChangeReason GetWindowSizeChangeReason() const;

    const std::string& GetWindowName() const;
    Rect GetRequestRect() const;
    Rect GetWindowRect() const;
    bool GetDecoStatus() const;
    Rect GetWindowHotZoneRect() const;
    WindowType GetWindowType() const;
    WindowMode GetWindowMode() const;
    WindowMode GetLastWindowMode() const;
    WindowBlurLevel GetWindowBackgroundBlur() const;
    bool GetFullScreen() const;
    bool GetFocusable() const;
    bool GetTouchable() const;
    uint32_t GetCallingWindow() const;
    bool GetPrivacyMode() const;
    bool GetTransparent() const;
    bool GetTokenState() const;
    float GetAlpha() const;
    float GetBrightness() const;
    bool IsTurnScreenOn() const;
    bool IsKeepScreenOn() const;
    DisplayId GetDisplayId() const;
    uint32_t GetWindowId() const;
    uint32_t GetParentId() const;
    uint32_t GetWindowFlags() const;
    const std::unordered_map<WindowType, SystemBarProperty>& GetSystemBarProperty() const;
    bool GetDecorEnable() const;
    const PointInfo& GetHitOffset() const;
    uint32_t GetAnimationFlag() const;
    uint32_t GetModeSupportInfo() const;
    DragType GetDragType() const;
    bool GetStretchable() const;
    const Rect& GetOriginRect() const;

    virtual bool Marshalling(Parcel& parcel) const override;
    static WindowProperty* Unmarshalling(Parcel& parcel);

    bool Write(Parcel& parcel, uint64_t inDirtyState);
    void Read(Parcel& parcel);
private:
    bool MapMarshalling(Parcel& parcel) const;
    static void MapUnmarshalling(Parcel& parcel, WindowProperty* property);

    bool WriteMemberVariable(Parcel& parcel, const MemberVariable& mv);
    void ReadMemberVariable(Parcel& parcel, const MemberVariable& mv);
    static const std::unordered_map<uint64_t, MemberVariable> dataTypeMap_;

    std::string windowName_;
    Rect requestRect_ { 0, 0, 0, 0 }; // window rect requested by the client (without decoration size)
    Rect windowRect_ { 0, 0, 0, 0 }; // actual window rect
    bool decoStatus_ { false }; // window has been decorated or not
    WindowType type_ { WindowType::WINDOW_TYPE_APP_MAIN_WINDOW };
    WindowMode mode_ { WindowMode::WINDOW_MODE_FULLSCREEN };
    WindowBlurLevel level_ { WindowBlurLevel::WINDOW_BLUR_OFF };
    WindowMode lastMode_ { WindowMode::WINDOW_MODE_FULLSCREEN };
    uint32_t flags_ { 0 };
    bool isFullScreen_ { true };
    bool focusable_ { true };
    bool touchable_ { true };
    bool isPrivacyMode_ { false };
    bool isTransparent_ { false };
    bool tokenState_ { false };
    float alpha_ { 1.0f };
    float brightness_ = UNDEFINED_BRIGHTNESS;
    bool turnScreenOn_ = false;
    bool keepScreenOn_ = false;
    uint32_t callingWindow_ = INVALID_WINDOW_ID;
    DisplayId displayId_ { 0 };
    uint32_t windowId_ { 0 };
    uint32_t parentId_ { 0 };
    PointInfo hitOffset_ { 0, 0 };
    uint32_t animationFlag_ { static_cast<uint32_t>(WindowAnimation::DEFAULT) };
    uint32_t modeSupportInfo_ {WindowModeSupport::WINDOW_MODE_SUPPORT_ALL};
    WindowSizeChangeReason windowSizeChangeReason_ = WindowSizeChangeReason::UNDEFINED;
    std::unordered_map<WindowType, SystemBarProperty> sysBarPropMap_ {
        { WindowType::WINDOW_TYPE_STATUS_BAR,     SystemBarProperty() },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SystemBarProperty() },
    };
    bool isDecorEnable_ { false };
    Rect originRect_ = { 0, 0, 0, 0 };
    bool isStretchable_ {false};
    DragType dragType_ = DragType::DRAG_UNDEFINED;
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(Orientation, RequestedOrientation, requestedOrientation, Orientation::UNSPECIFIED);
};
}
}
#endif // OHOS_ROSEN_WINDOW_PROPERTY_H
