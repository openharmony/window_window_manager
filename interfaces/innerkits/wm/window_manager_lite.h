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

#ifndef OHOS_ROSEN_WINDOW_MANAGER_LITE_H
#define OHOS_ROSEN_WINDOW_MANAGER_LITE_H

#include <iremote_object.h>
#include <memory>
#include <mutex>
#include <refbase.h>
#include <vector>
#include "focus_change_info.h"
#include "window_drawing_content_info.h"
#include "window_manager.h"
#include "window_visibility_info.h"
#include "wm_common.h"
#include "wm_single_instance.h"

namespace OHOS {
namespace Rosen {
/**
 * @class WindowManagerLite
 *
 * @brief WindowManagerLite used to manage window.
 */
class WindowManagerLite {
WM_DECLARE_SINGLE_INSTANCE_BASE(WindowManagerLite);
friend class WindowManagerAgentLite;
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
     * @brief Get visibility window info.
     *
     * @param infos Visible window infos
     * @return WM_OK means get success, others means get failed.
     */
    WMError GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos) const;
    /**
     * @brief Get focus window.
     *
     * @param focusInfo Focus window info.
     * @return FocusChangeInfo object about focus window.
     */
    void GetFocusWindowInfo(FocusChangeInfo& focusInfo);
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
     * @brief Register window mode listener.
     *
     * @param listener IWindowModeChangedListener.
     * @return WM_OK means register success, others means register failed.
     */
    WMError RegisterWindowModeChangedListener(const sptr<IWindowModeChangedListener>& listener);
    /**
     * @brief Unregister window mode listener.
     *
     * @param listener IWindowModeChangedListener.
     * @return WM_OK means unregister success, others means unregister failed.
     */
    WMError UnregisterWindowModeChangedListener(const sptr<IWindowModeChangedListener>& listener);

    /**
    * @brief Register camera window changed listener.
    *
    * @param listener ICameraWindowChangedListener.
    * @return WM_OK means register success, others means register failed.
    */
    WMError RegisterCameraWindowChangedListener(const sptr<ICameraWindowChangedListener>& listener);

    /**
   * @brief Unregister camera window changed listener.
   *
   * @param listener ICameraWindowChangedListener.
   * @return WM_OK means unregister success, others means unregister failed.
   */
    WMError UnregisterCameraWindowChangedListener(const sptr<ICameraWindowChangedListener>& listener);

    /**
     * @brief Get window mode type.
     *
     * @param void
     * @return WM_OK means get success, others means get failed.
     */
    WMError GetWindowModeType(WindowModeType& windowModeType) const;

    /**
     * @brief Get top num main window info.
     *
     * @param topNum the num of top window
     * @param topNInfo the top num window infos
     * @return WM_OK means get success, others means get failed.
     */
    WMError GetMainWindowInfos(int32_t topNum, std::vector<MainWindowInfo>& topNInfo);

    /**
     * @brief Get all main window info.
     *
     * @param infos the all main window info.
     * @return WM_OK means get success, others means get failed.
     */
    WMError GetAllMainWindowInfos(std::vector<MainWindowInfo>& infos) const;

    /**
     * @brief Clear a specified set of sessions.
     *
     * @param persistentIds a vector of session persistentId.
     * @return WM_OK means clear session success, others means clear failed.
     */
    WMError ClearMainSessions(const std::vector<int32_t>& persistentIds);

    /**
     * @brief Clear a specified set of sessions.
     *
     * @param persistentIds a vector of session persistentId.
     * @param clearFailedIds a vector of session persistentId which is clear failed.
     * @return WM_OK means clear session success, others means clear failed.
     */
    WMError ClearMainSessions(const std::vector<int32_t>& persistentIds, std::vector<int32_t>& clearFailedIds);

    /**
     * @brief raise window to top by windowId
     *
     * @param persistentId this window to raise
     * @return WM_OK if raise success
     */
    WMError RaiseWindowToTop(int32_t persistentId);
    
    /**
     * @brief Register WMS connection status changed listener.
     * @attention Callable only by u0 system user. A process only supports successful registration once.
     * When the foundation service restarts, you need to re-register the listener.
     * If you want to re-register, please call UnregisterWMSConnectionChangedListener first.
     *
     * @param listener IWMSConnectionChangedListener.
     * @return WM_OK means register success, others means register failed.
     */
    WMError RegisterWMSConnectionChangedListener(const sptr<IWMSConnectionChangedListener>& listener);

    /**
     * @brief Unregister WMS connection status changed listener.
     * @attention Callable only by u0 system user.
     *
     * @return WM_OK means unregister success, others means unregister failed.
     */
    WMError UnregisterWMSConnectionChangedListener();

    /**
     * @brief Register WindowStyle changed listener.
     *
     * @param listener IWindowStyleChangedListener
     * @return WM_OK means register success, others means unregister failed.
     */
    WMError RegisterWindowStyleChangedListener(const sptr<IWindowStyleChangedListener>& listener);
    /**
    * @brief Unregister WindowStyle changed listener.
    *
    * @param listener IWindowStyleChangedListener
    * @return WM_OK means unregister success, others means unregister failed.
    */
    WMError UnregisterWindowStyleChangedListener(const sptr<IWindowStyleChangedListener>& listener);
    /**
     * @brief Get window style type.
     *
     * @param windowStyleType WindowType
     * @return @return WM_OK means get window style success, others means failed.
     */
    WindowStyleType GetWindowStyleType();

    /**
     * @brief Terminate session by persistentId and start caller.
     * @persistentId persistentId to be terminated.
     *
     * @return WM_OK means Terminate success, others means Terminate failed.
     */
    WMError TerminateSessionByPersistentId(int32_t persistentId);

    /**
     * @brief Close target float window.
     *
     * @param bundleName the target float window need to be closed.
     * @return WM_OK means Close success, others means Close failed.
     */
    WMError CloseTargetFloatWindow(const std::string& bundleName);

    /**
     * @brief Register listener for PiP window state changed.
     *
     * @param listener the PiP state changed listener.
     * @return WM_OK means Register success, others means Register failed.
     */
    WMError RegisterPiPStateChangedListener(const sptr<IPiPStateChangedListener>& listener);

    /**
     * @brief Unregister listener for PiP window state changed.
     *
     * @param listener the PiP state changed listener.
     * @return WM_OK means Unregister success, others means Unregister failed.
     */
    WMError UnregisterPiPStateChangedListener(const sptr<IPiPStateChangedListener>& listener);

    /**
     * @brief Close target PiP Window by bundleName.
     *
     * @param bundleName the target PiP Window need to be closed.
     * @return WM_OK means Close success, others means Close failed.
     */
    WMError CloseTargetPiPWindow(const std::string& bundleName);

    /**
     * @brief Get current show PiP Window info.
     *
     * @param bundleName the current PiP window bundleName.
     * @return WM_OK means Get success, others means Get failed.
     */
    WMError GetCurrentPiPWindowInfo(std::string& bundleName);

private:
    WindowManagerLite();
    ~WindowManagerLite();
    std::recursive_mutex mutex_;
    class Impl;
    std::unique_ptr<Impl> pImpl_;
    bool destroyed_ = false;

    void UpdateFocusStatus(uint32_t windowId, const sptr<IRemoteObject>& abilityToken, WindowType windowType,
        DisplayId displayId, bool focused) const;
    void UpdateFocusChangeInfo(const sptr<FocusChangeInfo>& focusChangeInfo, bool focused) const;
    void UpdateWindowVisibilityInfo(
        const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfos) const;
    void UpdateWindowDrawingContentInfo(
        const std::vector<sptr<WindowDrawingContentInfo>>& windowDrawingContentInfos) const;
    void UpdateWindowModeTypeInfo(WindowModeType type) const;
    void UpdateCameraWindowStatus(uint32_t accessTokenId, bool isShowing) const;
    void UpdatePiPWindowStateChanged(const std::string& bundleName, bool isForeground) const;
    void OnRemoteDied();
    void OnWMSConnectionChanged(int32_t userId, int32_t screenId, bool isConnected) const;
    WMError NotifyWindowStyleChange(WindowStyleType type);
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_WINDOW_MANAGER_LITE_H
