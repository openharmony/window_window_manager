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
#include <mutex>
#include <refbase.h>
#include <vector>
#include <iremote_object.h>
#include "wm_single_instance.h"
#include "wm_common.h"
#include "focus_change_info.h"
#include "window_visibility_info.h"
#include "window_drawing_content_info.h"

namespace OHOS {
namespace Rosen {
struct SystemBarRegionTint {
    WindowType type_;
    SystemBarProperty prop_;
    Rect region_;
    SystemBarRegionTint()
        : type_(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW), prop_(), region_{ 0, 0, 0, 0 } {}
    SystemBarRegionTint(WindowType type, SystemBarProperty prop, Rect region)
        : type_(type), prop_(prop), region_(region) {}
};
using SystemBarRegionTints = std::vector<SystemBarRegionTint>;

/**
 * @class IFocusChangedListener
 *
 * @brief Listener to observe focus changed.
 */
class IFocusChangedListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when window get focus
     *
     * @param focusChangeInfo Window info while its focus status changed.
     */
    virtual void OnFocused(const sptr<FocusChangeInfo>& focusChangeInfo) = 0;
    /**
     * @brief Notify caller when window lose focus
     *
     * @param focusChangeInfo Window info while its focus status changed.
     */
    virtual void OnUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo) = 0;
};

/**
 * @class ISystemBarChangedListener
 *
 * @brief Listener to observe systembar changed.
 */
class ISystemBarChangedListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when system bar property changed
     *
     * @param displayId ID of display.
     * @param tints Tint of system bar region.
     */
    virtual void OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints) = 0;
};

/**
 * @class IGestureNavigationEnabledChangedListener
 *
 * @brief Listener to observe GestureNavigationEnabled changed.
 */
class IGestureNavigationEnabledChangedListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when GestureNavigationEnabled changed.
     *
     * @param enable True means set Gesture on, false means set Gesture off.
     */
    virtual void OnGestureNavigationEnabledUpdate(bool enable) = 0;
};

/**
 * @class IVisibilityChangedListener
 *
 * @brief Listener to observe visibility changed.
 */
class IVisibilityChangedListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when window visibility changed.
     *
     * @param windowVisibilityInfo Window visibility info.
     */
    virtual void OnWindowVisibilityChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo) = 0;
};

/**
 * @class IDrawingContentChangedListener
 *
 * @brief Listener to observe visibility changed.
 */
class IDrawingContentChangedListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when window visibility changed.
     *
     * @param DrawingContentInfo Window visibility info.
     */
    virtual void OnWindowDrawingContentChanged(const WindowDrawingContentInfo& DrawingContentInfo) = 0;
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
     * @brief Marshalling AccessibilityWindowInfo.
     *
     * @param parcel Package of AccessibilityWindowInfo.
     * @return True means marshall success, false means marshall failed.
     */
    virtual bool Marshalling(Parcel& parcel) const override;
    /**
     * @brief Unmarshalling AccessibilityWindowInfo.
     *
     * @param parcel Package of AccessibilityWindowInfo.
     * @return AccessibilityWindowInfo object.
     */
    static AccessibilityWindowInfo* Unmarshalling(Parcel& parcel);

    int32_t wid_;
    int32_t innerWid_;
    int32_t uiNodeId_;
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
 * @brief Listener to observe window update.
 */
class IWindowUpdateListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when AccessibilityWindowInfo update.
     *
     * @param infos Window info used for Accessibility.
     * @param type Type for window update.
     */
    virtual void OnWindowUpdate(const std::vector<sptr<AccessibilityWindowInfo>>& infos, WindowUpdateType type) = 0;
};

/**
 * @class IWaterMarkFlagChangedListener
 *
 * @brief Listener to observe water mark flag changed.
 */
class IWaterMarkFlagChangedListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when water mark flag changed.
     *
     * @param showWaterMark True means show water mark, false means the opposite.
     */
    virtual void OnWaterMarkFlagUpdate(bool showWaterMark) = 0;
};

/**
 * @class ICameraFloatWindowChangedListener
 *
 * @brief Listener to observe camera window changed.
 */
class ICameraFloatWindowChangedListener : virtual public RefBase {
public:
    /**
     * @brief Notify caller when camera window changed.
     *
     * @param accessTokenId Token id of camera window.
     * @param isShowing True means camera is shown, false means the opposite.
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
friend class SSMDeathRecipient;
public:
    /**
     * @brief Register focus changed listener.
     *
     * @param listener IFocusChangedListener.
     * @return WM_OK means register success, others means register failed.
     */
    WMError RegisterFocusChangedListener(const sptr<IFocusChangedListener>& listener);
    /**
     * @brief Unregister focus changed listener.
     *
     * @param listener IFocusChangedListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    WMError UnregisterFocusChangedListener(const sptr<IFocusChangedListener>& listener);
    /**
     * @brief Register system bar changed listener.
     *
     * @param listener ISystemBarChangedListener.
     * @return WM_OK means register success, others means register failed.
     */
    WMError RegisterSystemBarChangedListener(const sptr<ISystemBarChangedListener>& listener);
    /**
     * @brief Unregister system bar changed listener.
     *
     * @param listener ISystemBarChangedListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    WMError UnregisterSystemBarChangedListener(const sptr<ISystemBarChangedListener>& listener);
    /**
     * @brief Register window updated listener.
     *
     * @param listener IWindowUpdateListener.
     * @return WM_OK means register success, others means register failed.
     */
    WMError RegisterWindowUpdateListener(const sptr<IWindowUpdateListener>& listener);
    /**
     * @brief Unregister window updated listener.
     *
     * @param listener IWindowUpdateListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    WMError UnregisterWindowUpdateListener(const sptr<IWindowUpdateListener>& listener);
    /**
     * @brief Register visibility changed listener.
     *
     * @param listener IVisibilityChangedListener.
     * @return WM_OK means register success, others means register failed.
     */
    WMError RegisterVisibilityChangedListener(const sptr<IVisibilityChangedListener>& listener);
    /**
     * @brief Unregister visibility changed listener.
     *
     * @param listener IVisibilityChangedListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    WMError UnregisterVisibilityChangedListener(const sptr<IVisibilityChangedListener>& listener);
    /**
     * @brief Register camera float window changed listener.
     *
     * @param listener ICameraFloatWindowChangedListener.
     * @return WM_OK means register success, others means register failed.
     */
    WMError RegisterCameraFloatWindowChangedListener(const sptr<ICameraFloatWindowChangedListener>& listener);
    /**
     * @brief Unregister camera float window changed listener.
     *
     * @param listener ICameraFloatWindowChangedListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    WMError UnregisterCameraFloatWindowChangedListener(const sptr<ICameraFloatWindowChangedListener>& listener);
    /**
     * @brief Register water mark flag changed listener.
     *
     * @param listener IWaterMarkFlagChangedListener.
     * @return WM_OK means register success, others means register failed.
     */
    WMError RegisterWaterMarkFlagChangedListener(const sptr<IWaterMarkFlagChangedListener>& listener);
    /**
     * @brief Unregister water mark flag changed listener.
     *
     * @param listener IWaterMarkFlagChangedListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    WMError UnregisterWaterMarkFlagChangedListener(const sptr<IWaterMarkFlagChangedListener>& listener);
    /**
     * @brief Register gesture navigation enabled changed listener.
     *
     * @param listener IGestureNavigationEnabledChangedListener.
     * @return WM_OK means register success, others means register failed.
     */
    WMError RegisterGestureNavigationEnabledChangedListener(
        const sptr<IGestureNavigationEnabledChangedListener>& listener);
    /**
     * @brief Unregister gesture navigation enabled changed listener.
     *
     * @param listener IGestureNavigationEnabledChangedListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    WMError UnregisterGestureNavigationEnabledChangedListener(
        const sptr<IGestureNavigationEnabledChangedListener>& listener);
    /**
     * @brief Minimize all app window.
     *
     * @param displayId Display id.
     * @return WM_OK means minimize success, others means minimize failed.
     */
    WMError MinimizeAllAppWindows(DisplayId displayId);
    /**
     * @brief Toggle all app windows to the foreground.
     *
     * @return WM_OK means toggle success, others means toggle failed.
     */
    WMError ToggleShownStateForAllAppWindows();
    /**
     * @brief Set window layout mode.
     *
     * @param mode Window layout mode.
     * @return WM_OK means set success, others means set failed.
     */
    WMError SetWindowLayoutMode(WindowLayoutMode mode);
    /**
     * @brief Get accessibility window info.
     *
     * @param infos WindowInfos used for Accessibility.
     * @return WM_OK means get success, others means get failed.
     */
    WMError GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos) const;
    /**
     * @brief Get visibility window info.
     *
     * @param infos Visible window infos
     * @return WM_OK means get success, others means get failed.
     */
    WMError GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos) const;
    /**
     * @brief Set gesture navigaion enabled.
     *
     * @param enable True means set gesture on, false means set gesture off.
     * @return WM_OK means set success, others means set failed.
     */
    WMError SetGestureNavigaionEnabled(bool enable) const;

    /**
     * @brief Get focus window.
     *
     * @param focusInfo Focus window info.
     * @return FocusChangeInfo object about focus window.
     */
    void GetFocusWindowInfo(FocusChangeInfo& focusInfo);

    /**
     * @brief Dump all session info
     *
     * @param infos session infos
     * @return WM_OK means set success, others means set failed.
     */
    WMError DumpSessionAll(std::vector<std::string> &infos);

    /**
     * @brief Dump session info with id
     *
     * @param infos session infos
     * @return WM_OK means set success, others means set failed.
     */
    WMError DumpSessionWithId(int32_t persistentId, std::vector<std::string> &infos);

    /**
     * @brief raise window to top by windowId
     *
     * @param persistentId this window to raise
     * @return WM_OK if raise success
     */
    WMError RaiseWindowToTop(int32_t persistentId);

    /**
     * @brief notify window extension visibility change
     *
     * @param pid process id
     * @param uid user id
     * @param visible visibility
     * @return WM_OK means notify success, others means notify failed.
    */
    WMError NotifyWindowExtensionVisibilityChange(int32_t pid, int32_t uid, bool visible);

    /**
     * @brief Register drawingcontent changed listener.
     *
     * @param listener IDrawingContentChangedListener.
     * @return WM_OK means register success, others means register failed.
     */
    WMError RegisterDrawingContentChangedListener(const sptr<IDrawingContentChangedListener>& listener);

    /**
     * @brief Unregister drawingcontent changed listener.
     *
     * @param listener IDrawingContentChangedListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    WMError UnregisterDrawingContentChangedListener(const sptr<IDrawingContentChangedListener>& listener);

    /**
     * @brief NotifyWindowDrawingContentInfoChanged.
     *
     * @param info DrawingContent window info
     * @return WM_OK means get success, others means get failed.
     */
    void NotifyWindowDrawingContentInfoChanged(const WindowDrawingContentInfo& info);

    /**
     * @brief UpdateWindowDrawingContentInfo.
     *
     * @param info DrawingContent window info
     * @return WM_OK means get success, others means get failed.
     */
    void UpdateWindowDrawingContentInfo(const WindowDrawingContentInfo& info) const;

private:
    WindowManager();
    ~WindowManager();
    std::recursive_mutex mutex_;
    class Impl;
    std::unique_ptr<Impl> pImpl_;
    bool destroyed_ = false;

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
    void OnRemoteDied();
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_WINDOW_MANAGER_H
