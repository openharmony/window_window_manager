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

#ifndef OHOS_ROSEN_WINDOW_MANAGER_H
#define OHOS_ROSEN_WINDOW_MANAGER_H

#include <memory>
#include <refbase.h>
#include <vector>
#include <iremote_object.h>
#include "wm_single_instance.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
struct SystemBarRegionTint {
    WindowType type_;
    SystemBarProperty prop_;
    Rect region_;
    SystemBarRegionTint() : prop_(SystemBarProperty()) {}
    SystemBarRegionTint(WindowType type, SystemBarProperty prop, Rect region)
        : type_(type), prop_(prop), region_(region) {}
};
using SystemBarRegionTints = std::vector<SystemBarRegionTint>;

/**
 * @class FocusChangeInfo
 *
 * @brief Window info while its focus status changed
 */
class FocusChangeInfo : public Parcelable {
public:
    /**
     * @brief Default construct of FocusChangeInfo
     */
    FocusChangeInfo() = default;
    /**
     * @brief Construct of FocusChangeInfo
     */
    FocusChangeInfo(uint32_t winId, DisplayId displayId, int32_t pid, int32_t uid, WindowType type,
        const sptr<IRemoteObject>& abilityToken): windowId_(winId), displayId_(displayId), pid_(pid), uid_(uid),
        windowType_(type),  abilityToken_(abilityToken) {};
    /**
     * @brief Deconstruct of FocusChangeInfo
     */
    ~FocusChangeInfo() = default;
    /**
     * @brief Marshalling
     * 
     * @param parcel
     * @return bool
     */
    virtual bool Marshalling(Parcel& parcel) const override;
    /**
     * @brief Unmarshalling
     * 
     * @param parcel
     */
    static FocusChangeInfo* Unmarshalling(Parcel& parcel);

    uint32_t windowId_ = INVALID_WINDOW_ID;
    DisplayId displayId_ = 0;
    int32_t pid_ = 0;
    int32_t uid_ = 0;
    WindowType windowType_ = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    sptr<IRemoteObject> abilityToken_;
};

/**
 * @class IFocusChangedListener
 * 
 * @brief listener to observe focus changed.
 */
class IFocusChangedListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when window get focus
     * 
     * @param focusChangeInfo
     */
    virtual void OnFocused(const sptr<FocusChangeInfo>& focusChangeInfo) = 0;
    /**
     * @brief Notify caller when window lose focus
     * 
     * @param focusChangeInfo
     */
    virtual void OnUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo) = 0;
};

/**
 * @class ISystemBarChangedListener
 * 
 * @brief listener to observe systembar changed.
 */
class ISystemBarChangedListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when system bar property changed
     * 
     * @param displayId
     * @param tints Vector about SystemBarRegionTint.
     */
    virtual void OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints) = 0;
};

/**
 * @class IGestureNavigationEnabledChangedListener
 * 
 * @brief listener to observe GestureNavigationEnabled changed.
 */
class IGestureNavigationEnabledChangedListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when  GestureNavigationEnabled changed.
     * 
     * @param enable
     */
    virtual void OnGestureNavigationEnabledUpdate(bool enable) = 0;
};

/**
 * @class WindowVisibilityInfo
 * 
 * @brief Visibility info of window.
 */
class WindowVisibilityInfo : public Parcelable {
public:
    /**
     * @brief Default construct of WindowVisibilityInfo.
     */
    WindowVisibilityInfo() = default;
    /**
     * @brief Construct of WindowVisibilityInfo.
     */
    WindowVisibilityInfo(uint32_t winId, int32_t pid, int32_t uid, bool visibility, WindowType winType)
        : windowId_(winId), pid_(pid), uid_(uid), isVisible_(visibility), windowType_(winType) {};
    /**
     * @brief Deconstruct of WindowVisibilityInfo.
     */
    ~WindowVisibilityInfo() = default;

    /**
     * @brief Marshalling.
     * 
     * @param parcel
     * @return bool
     */
    virtual bool Marshalling(Parcel& parcel) const override;
    /**
     * @brief Unmarshalling.
     * 
     * @param parcel
     * @return WindowVisibilityInfo
     */
    static WindowVisibilityInfo* Unmarshalling(Parcel& parcel);

    uint32_t windowId_ { INVALID_WINDOW_ID };
    int32_t pid_ { 0 };
    int32_t uid_ { 0 };
    bool isVisible_ { false };
    WindowType windowType_ { WindowType::WINDOW_TYPE_APP_MAIN_WINDOW };
};

/**
 * @class IVisibilityChangedListener
 * 
 * @brief listener to observe visibility changed.
 */
class IVisibilityChangedListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when window visibility changed.
     * 
     * @param windowVisibilityInfo
     */
    virtual void OnWindowVisibilityChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo) = 0;
};

/**
 * @class AccessibilityWindowInfo
 * 
 * @brief Window info used for Accessibility.
 */
class AccessibilityWindowInfo : public Parcelable {
public:
    /**
     * @brief Default construct of AccessibilityWindowInfo.
     */
    AccessibilityWindowInfo() = default;
    /**
     * @brief Default deconstruct of AccessibilityWindowInfo.
     */
    ~AccessibilityWindowInfo() = default;

    /**
     * @brief Marshalling.
     * 
     * @param parcel
     * @return bool
     */
    virtual bool Marshalling(Parcel& parcel) const override;
    /**
     * @brief Unmarshalling.
     * 
     * @param parcel
     * @return AccessibilityWindowInfo
     */
    static AccessibilityWindowInfo* Unmarshalling(Parcel& parcel);

    int32_t wid_;
    Rect windowRect_;
    bool focused_ { false };
    bool isDecorEnable_ { false };
    DisplayId displayId_;
    uint32_t layer_;
    WindowMode mode_;
    WindowType type_;
};

/**
 * @class IWindowUpdateListener
 * 
 * @brief listener to observe window update.
 */
class IWindowUpdateListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when AccessibilityWindowInfo update.
     * 
     * @param infos
     * @param type
     */
    virtual void OnWindowUpdate(const std::vector<sptr<AccessibilityWindowInfo>>& infos, WindowUpdateType type) = 0;
};

/**
 * @class IWaterMarkFlagChangedListener
 * 
 * @brief listener to observe water mark flag changed.
 */
class IWaterMarkFlagChangedListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when water mark flag changed.
     * 
     * @param showWaterMark
     */
    virtual void OnWaterMarkFlagUpdate(bool showWaterMark) = 0;
};

/**
 * @class ICameraFloatWindowChangedListener
 * 
 * @brief listener to observe camera window changed.
 */
class ICameraFloatWindowChangedListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when camera window changed.
     * 
     * @param accessTokenId
     * @param isShowing
     */
    virtual void OnCameraFloatWindowChange(uint32_t accessTokenId, bool isShowing) = 0;
};

/**
 * @class WindowManager
 * 
 * @brief WindowManager used to manage window.
 */
class WindowManager {
WM_DECLARE_SINGLE_INSTANCE_BASE(WindowManager);
friend class WindowManagerAgent;
friend class WMSDeathRecipient;
public:
    /**
     * @brief Register focus changed listener.
     *
     * @param listener
     * @return WMError
     */
    WMError RegisterFocusChangedListener(const sptr<IFocusChangedListener>& listener);
    /**
     * @brief Unregister focus changed listener.
     *
     * @param listener
     * @return WMError
     */
    WMError UnregisterFocusChangedListener(const sptr<IFocusChangedListener>& listener);
    /**
     * @brief Register system bar changed listener.
     *
     * @param listener
     * @return WMError
     */
    WMError RegisterSystemBarChangedListener(const sptr<ISystemBarChangedListener>& listener);
    /**
     * @brief Unregister system bar changed listener.
     *
     * @param listener
     * @return WMError
     */
    WMError UnregisterSystemBarChangedListener(const sptr<ISystemBarChangedListener>& listener);
    /**
     * @brief Register window updated listener.
     *
     * @param listener
     * @return WMError
     */
    WMError RegisterWindowUpdateListener(const sptr<IWindowUpdateListener>& listener);
    /**
     * @brief Unregister window updated listener.
     *
     * @param listener
     * @return WMError
     */
    WMError UnregisterWindowUpdateListener(const sptr<IWindowUpdateListener>& listener);
    /**
     * @brief Register visibility changed listener.
     *
     * @param listener
     * @return WMError
     */
    WMError RegisterVisibilityChangedListener(const sptr<IVisibilityChangedListener>& listener);
    /**
     * @brief Unregister visibility changed listener.
     *
     * @param listener
     * @return WMError
     */
    WMError UnregisterVisibilityChangedListener(const sptr<IVisibilityChangedListener>& listener);
    /**
     * @brief Register camera float window changed listener.
     *
     * @param listener
     * @return WMError
     */
    WMError RegisterCameraFloatWindowChangedListener(const sptr<ICameraFloatWindowChangedListener>& listener);
    /**
     * @brief Unregister camera float window changed listener.
     *
     * @param listener
     * @return WMError
     */
    WMError UnregisterCameraFloatWindowChangedListener(const sptr<ICameraFloatWindowChangedListener>& listener);
    /**
     * @brief Register water mark flag changed listener.
     *
     * @param listener
     * @return WMError
     */
    WMError RegisterWaterMarkFlagChangedListener(const sptr<IWaterMarkFlagChangedListener>& listener);
    /**
     * @brief Unregister water mark flag changed listener.
     *
     * @param listener
     * @return WMError
     */
    WMError UnregisterWaterMarkFlagChangedListener(const sptr<IWaterMarkFlagChangedListener>& listener);
    /**
     * @brief Register gesture navigation enabled changed listener.
     *
     * @param listener
     * @return WMError
     */
    WMError RegisterGestureNavigationEnabledChangedListener(
        const sptr<IGestureNavigationEnabledChangedListener>& listener);
    /**
     * @brief Unregister gesture navigation enabled changed listener.
     *
     * @param listener
     * @return WMError
     */
    WMError UnregisterGestureNavigationEnabledChangedListener(
        const sptr<IGestureNavigationEnabledChangedListener>& listener);
    /**
     * @brief Minimize all app window.
     *
     * @param displayId
     * @return WMError
     */
    WMError MinimizeAllAppWindows(DisplayId displayId);
    /**
     * @brief Toggle all app windows to the foreground.
     *
     * @return WMError
     */
    WMError ToggleShownStateForAllAppWindows();
    /**
     * @brief Set window layout mode.
     *
     * @param mode
     * @return WMError
     */
    WMError SetWindowLayoutMode(WindowLayoutMode mode);
    /**
     * @brief Get accessibility window info.
     *
     * @param infos
     * @return WMError
     */
    WMError GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos) const;
    /**
     * @brief Get visibility window info.
     *
     * @param infos
     * @return WMError
     */
    WMError GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos) const;
    /**
     * @brief Set gesture navigaion enabled.
     *
     * @param enable
     * @return WMError
     */
    WMError SetGestureNavigaionEnabled(bool enable) const;

private:
    WindowManager();
    ~WindowManager() = default;
    class Impl;
    std::unique_ptr<Impl> pImpl_;

    void UpdateFocusStatus(uint32_t windowId, const sptr<IRemoteObject>& abilityToken, WindowType windowType,
        DisplayId displayId, bool focused) const;
    void UpdateFocusChangeInfo(const sptr<FocusChangeInfo>& focusChangeInfo, bool focused) const;
    void UpdateSystemBarRegionTints(DisplayId displayId, const SystemBarRegionTints& tints) const;
    void NotifyAccessibilityWindowInfo(const std::vector<sptr<AccessibilityWindowInfo>>& infos,
        WindowUpdateType type) const;
    void UpdateWindowVisibilityInfo(
        const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfos) const;
    void UpdateCameraFloatWindowStatus(uint32_t accessTokenId, bool isShowing) const;
    void NotifyWaterMarkFlagChangedResult(bool showWaterMark) const;
    void NotifyGestureNavigationEnabledResult(bool enable) const;
    void OnRemoteDied() const;
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_WINDOW_MANAGER_H
