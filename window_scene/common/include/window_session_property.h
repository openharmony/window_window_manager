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

#ifndef OHOS_ROSEN_WINDOW_SESSION_PROPERTY_H
#define OHOS_ROSEN_WINDOW_SESSION_PROPERTY_H

#include <refbase.h>
#include <string>
#include <unordered_map>
#include <parcel.h>
#include "interfaces/include/ws_common.h"
#include "interfaces/include/ws_common_inner.h"
#include "wm_common.h"
#include "dm_common.h"
#include <cfloat>

namespace OHOS {
namespace Rosen {

struct WindowLimits {
    uint32_t maxWidth_;
    uint32_t maxHeight_;
    uint32_t minWidth_;
    uint32_t minHeight_;
    float maxRatio_;
    float minRatio_;
    WindowLimits() : maxWidth_(UINT32_MAX), maxHeight_(UINT32_MAX), minWidth_(0), minHeight_(0), maxRatio_(FLT_MAX),
        minRatio_(0.0f) {}
    WindowLimits(uint32_t maxWidth, uint32_t maxHeight, uint32_t minWidth, uint32_t minHeight, float maxRatio,
        float minRatio) : maxWidth_(maxWidth), maxHeight_(maxHeight), minWidth_(minWidth), minHeight_(minHeight),
        maxRatio_(maxRatio), minRatio_(minRatio) {}

    bool IsEmpty() const
    {
        return (maxWidth_ == 0 || minWidth_ == 0 || maxHeight_ == 0 || minHeight_ == 0);
    }
};

class WindowSessionProperty : public Parcelable {
public:
    WindowSessionProperty() = default;
    ~WindowSessionProperty() = default;
    explicit WindowSessionProperty(const sptr<WindowSessionProperty>& property);
    void CopyFrom(const sptr<WindowSessionProperty>& property);
    void SetWindowName(const std::string& name);
    void SetSessionInfo(const SessionInfo& info);
    void SetRequestRect(const struct Rect& rect);
    void SetWindowRect(const struct Rect& rect);
    void SetFocusable(bool isFocusable);
    void SetTouchable(bool isTouchable);
    void SetTurnScreenOn(bool turnScreenOn);
    void SetKeepScreenOn(bool keepScreenOn);
    void SetBrightness(float brightness);
    void SetRequestedOrientation(Orientation orientation);
    void SetPrivacyMode(bool isPrivate);
    void SetSystemPrivacyMode(bool isSystemPrivate);
    void SetDisplayId(uint64_t displayId);
    void SetWindowType(WindowType type);
    void SetParentId(uint32_t parentId);
    void SetPersistentId(uint32_t persistentId);
    void SetParentPersistentId(uint32_t persistentId);
    void SetAccessTokenId(uint32_t accessTokenId);
    void SetTokenState(bool hasToken);
    void SetMaximizeMode(MaximizeMode mode);
    void SetWindowMode(WindowMode mode);
    void SetWindowLimits(const WindowLimits& windowLimits);
    void SetSystemBarProperty(WindowType type, const SystemBarProperty& property);
    void SetSessionGravity(SessionGravity gravity_, uint32_t percent);
    void SetDecorEnable(bool isDecorEnable);
    void SetAnimationFlag(uint32_t animationFlag);
    void SetTransform(const Transform& trans);
    void SetWindowFlags(uint32_t flags);
    void AddWindowFlag(WindowFlag flag);
    void SetModeSupportInfo(uint32_t modeSupportInfo);

    const std::string& GetWindowName() const;
    const SessionInfo& GetSessionInfo() const;
    Rect GetWindowRect() const;
    Rect GetRequestRect() const;
    WindowType GetWindowType() const;
    bool GetFocusable() const;
    bool GetTouchable() const;
    bool IsTurnScreenOn() const;
    bool IsKeepScreenOn() const;
    float GetBrightness() const;
    Orientation GetRequestedOrientation() const;
    bool GetPrivacyMode() const;
    bool GetSystemPrivacyMode() const;
    uint32_t GetParentId() const;
    uint32_t GetWindowFlags() const;
    uint64_t GetDisplayId() const;
    uint32_t GetPersistentId() const;
    uint32_t GetParentPersistentId() const;
    uint32_t GetAccessTokenId() const;
    bool GetTokenState() const;
    MaximizeMode GetMaximizeMode() const;
    WindowMode GetWindowMode() const;
    WindowLimits GetWindowLimits() const;
    uint32_t GetModeSupportInfo() const;
    const std::unordered_map<WindowType, SystemBarProperty>& GetSystemBarProperty() const;
    void GetSessionGravity(SessionGravity& gravity, uint32_t& percent);
    bool IsDecorEnable();
    uint32_t GetAnimationFlag() const;
    const Transform& GetTransform() const;

    bool MarshallingWindowLimits(Parcel& parcel) const;
    static void UnmarshallingWindowLimits(Parcel& parcel, WindowSessionProperty* property);
    bool MarshallingSystemBarMap(Parcel& parcel) const;
    static void UnMarshallingSystemBarMap(Parcel& parcel, WindowSessionProperty* property);
    bool Marshalling(Parcel& parcel) const override;
    static WindowSessionProperty* Unmarshalling(Parcel& parcel);

private:
    std::string windowName_;
    SessionInfo sessionInfo_;
    Rect requestRect_ { 0, 0, 0, 0 }; // window rect requested by the client (without decoration size)
    Rect windowRect_ { 0, 0, 0, 0 }; // actual window rect
    WindowType type_ { WindowType::WINDOW_TYPE_APP_MAIN_WINDOW }; // type main window
    bool focusable_ { true };
    bool touchable_ { true };
    bool tokenState_ { false };
    bool turnScreenOn_ = false;
    bool keepScreenOn_ = false;
    float brightness_ = UNDEFINED_BRIGHTNESS;
    Orientation requestedOrientation_ = Orientation::UNSPECIFIED;
    bool isPrivacyMode_ { false };
    bool isSystemPrivacyMode_ { false };
    uint64_t displayId_ = 0;
    uint32_t parentId_ = INVALID_SESSION_ID; // parentId of sceneSession, which is low 32 bite of parentPersistentId_
    uint32_t flags_ = 0;
    uint32_t persistentId_ = INVALID_SESSION_ID;
    uint32_t parentPersistentId_ = INVALID_SESSION_ID;
    uint32_t accessTokenId_ = INVALID_SESSION_ID;
    MaximizeMode maximizeMode_ = MaximizeMode::MODE_RECOVER;
    WindowMode windowMode_ = WindowMode::WINDOW_MODE_FULLSCREEN;
    WindowLimits limits_;
    SessionGravity sessionGravity_ = SessionGravity::SESSION_GRAVITY_BOTTOM;
    uint32_t sessionGravitySizePercent_ = 0;
    uint32_t modeSupportInfo_ {WindowModeSupport::WINDOW_MODE_SUPPORT_ALL};
    std::unordered_map<WindowType, SystemBarProperty> sysBarPropMap_ {
        { WindowType::WINDOW_TYPE_STATUS_BAR,     SystemBarProperty(true, 0x00FFFFFF, 0xFF000000) },
    };
    bool isDecorEnable_ = false;
    uint32_t animationFlag_ { static_cast<uint32_t>(WindowAnimation::DEFAULT) };
    // Transform info
    Transform trans_;
};

struct SystemSessionConfig : public Parcelable {
    bool isSystemDecorEnable_ = true;
    uint32_t decorModeSupportInfo_ = WindowModeSupport::WINDOW_MODE_SUPPORT_ALL;
    bool isStretchable_ = false;
    WindowMode defaultWindowMode_ = WindowMode::WINDOW_MODE_FULLSCREEN;
    KeyboardAnimationConfig keyboardAnimationConfig_;
    int32_t maxFloatingWindowSize_ = INT32_MAX;

    virtual bool Marshalling(Parcel& parcel) const override
    {
        if (!parcel.WriteBool(isSystemDecorEnable_) || !parcel.WriteBool(isStretchable_) ||
            !parcel.WriteUint32(decorModeSupportInfo_)) {
            return false;
        }

        if (!parcel.WriteUint32(static_cast<uint32_t>(defaultWindowMode_)) ||
            !parcel.WriteParcelable(&keyboardAnimationConfig_) ||
            !parcel.WriteInt32(maxFloatingWindowSize_)) {
            return false;
        }

        return true;
    }

    static SystemSessionConfig* Unmarshalling(Parcel& parcel)
    {
        SystemSessionConfig* config = new (std::nothrow) SystemSessionConfig();
        if (config == nullptr) {
            return nullptr;
        }
        config->isSystemDecorEnable_ = parcel.ReadBool();
        config->isStretchable_ = parcel.ReadBool();
        config->decorModeSupportInfo_ = parcel.ReadUint32();
        config->defaultWindowMode_ = static_cast<WindowMode>(parcel.ReadUint32());
        sptr<KeyboardAnimationConfig> keyboardConfig = parcel.ReadParcelable<KeyboardAnimationConfig>();
        config->keyboardAnimationConfig_ = *keyboardConfig;
        config->maxFloatingWindowSize_ = parcel.ReadInt32();
        return config;
    }
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_PROPERTY_H
