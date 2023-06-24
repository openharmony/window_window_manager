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
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class WindowSessionProperty : public Parcelable {
public:
    WindowSessionProperty() = default;
    ~WindowSessionProperty() = default;

    void SetWindowName(const std::string& name);
    void SetSessionInfo(const SessionInfo& info);
    void SetRequestRect(const struct Rect& rect);
    void SetWindowRect(const struct Rect& rect);
    void SetFocusable(bool isFocusable);
    void SetTouchable(bool isTouchable);
    void SetPrivacyMode(bool isPrivate);
    void SetSystemPrivacyMode(bool isSystemPrivate);
    void SetDisplayId(uint64_t displayId);
    void SetWindowType(WindowType type);
    void SetParentId(uint32_t parentId);
    void SetPersistentId(uint64_t persistentId);
    void SetParentPersistentId(uint64_t persistentId);
    void SetAccessTokenId(uint32_t accessTokenId);
    void SetTokenState(bool hasToken);

    const std::string& GetWindowName() const;
    const SessionInfo& GetSessionInfo() const;
    Rect GetWindowRect() const;
    Rect GetRequestRect() const;
    WindowType GetWindowType() const;
    bool GetFocusable() const;
    bool GetTouchable() const;
    bool GetPrivacyMode() const;
    bool GetSystemPrivacyMode() const;
    uint32_t GetParentId() const;
    uint64_t GetDisplayId() const;
    uint64_t GetPersistentId() const;
    uint64_t GetParentPersistentId() const;
    uint32_t GetAccessTokenId() const;
    bool GetTokenState() const;

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
    bool isPrivacyMode_ { false };
    bool isSystemPrivacyMode_ { false };
    uint64_t displayId_ = 0;
    uint32_t parentId_ = INVALID_SESSION_ID; // parentId of sceneSession, which is low 32 bite of parentPersistentId_
    uint64_t persistentId_ = INVALID_SESSION_ID;
    uint64_t parentPersistentId_ = INVALID_SESSION_ID;
    uint32_t accessTokenId_ = INVALID_SESSION_ID;
};

struct SystemSessionConfig : public Parcelable {
    bool isSystemDecorEnable_ = true;
    uint32_t decorModeSupportInfo_ = WindowModeSupport::WINDOW_MODE_SUPPORT_ALL;
    bool isStretchable_ = false;
    WindowMode defaultWindowMode_ = WindowMode::WINDOW_MODE_FULLSCREEN;
    KeyboardAnimationConfig keyboardAnimationConfig_;

    virtual bool Marshalling(Parcel& parcel) const override
    {
        if (!parcel.WriteBool(isSystemDecorEnable_) || !parcel.WriteBool(isStretchable_) ||
            !parcel.WriteUint32(decorModeSupportInfo_)) {
            return false;
        }

        if (!parcel.WriteUint32(static_cast<uint32_t>(defaultWindowMode_)) ||
            !parcel.WriteParcelable(&keyboardAnimationConfig_)) {
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
        return config;
    }
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_PROPERTY_H
