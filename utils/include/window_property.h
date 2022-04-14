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
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class WindowProperty : public Parcelable {
public:
    WindowProperty() = default;
    WindowProperty(const sptr<WindowProperty>& property);
    ~WindowProperty() = default;

    void CopyFrom(const sptr<WindowProperty>& property);

    void SetWindowName(const std::string& name);
    void SetWindowRect(const struct Rect& rect);
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
    void SetSystemBarProperty(WindowType type, const SystemBarProperty& state);
    void SetDecorEnable(bool decorEnable);
    void SetHitOffset(const PointInfo& offset);
    void SetAnimationFlag(uint32_t animationFlag);
    void SetWindowSizeChangeReason(WindowSizeChangeReason reason);
    WindowSizeChangeReason GetWindowSizeChangeReason() const;

    const std::string& GetWindowName() const;
    Rect GetWindowRect() const;
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

    virtual bool Marshalling(Parcel& parcel) const override;
    static sptr<WindowProperty> Unmarshalling(Parcel& parcel);
private:
    bool MapMarshalling(Parcel& parcel) const;
    static void MapUnmarshalling(Parcel& parcel, sptr<WindowProperty>& property);

    std::string windowName_;
    Rect windowRect_ { 0, 0, 0, 0 };
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
    WindowSizeChangeReason windowSizeChangeReason_ = WindowSizeChangeReason::UNDEFINED;
    std::unordered_map<WindowType, SystemBarProperty> sysBarPropMap_ {
        { WindowType::WINDOW_TYPE_STATUS_BAR,     SystemBarProperty() },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SystemBarProperty() },
    };
    bool isDecorEnable_ { false };
};
}
}
#endif // OHOS_ROSEN_WINDOW_PROPERTY_H
