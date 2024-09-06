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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_INTERFACE_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_INTERFACE_H

#include <iremote_broker.h>
#include <session_info.h>

#include "interfaces/include/ws_common.h"
#include "common/include/window_session_property.h"
#include "session/container/include/zidl/session_stage_interface.h"
#include "session/container/include/zidl/window_event_channel_interface.h"

namespace OHOS::Accessibility {
class AccessibilityEventInfo;
}
namespace OHOS::Rosen {
class RSSurfaceNode;
class ISession : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.ISession");

    virtual WSError Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
        sptr<WindowSessionProperty> property = nullptr, sptr<IRemoteObject> token = nullptr,
        const std::string& identityToken = "") { return WSError::WS_OK; }
    virtual WSError Foreground(sptr<WindowSessionProperty> property, bool isFromClient = false) = 0;
    virtual WSError Background(bool isFromClient = false) = 0;
    virtual WSError Disconnect(bool isFromClient = false) = 0;
    virtual WSError Show(sptr<WindowSessionProperty> property) = 0;
    virtual WSError Hide() = 0;
    virtual WSError DrawingCompleted() = 0;

    // scene session
    /**
     * @brief Receive session event from application.
     *
     * This function provides the ability for applications to move window.\n
     * This interface will take effect after touch down event.\n
     *
     * @param event Indicates the {@link SessionEvent}
     * @return Returns WSError::WS_OK if called success, otherwise failed.
     */
    virtual WSError OnSessionEvent(SessionEvent event) { return WSError::WS_OK; }

    /**
     * @brief Receive session event from system application.
     *
     * This function provides the ability for system applications to move system window.\n
     * This interface will take effect after touch down event.\n
     *
     * @return Returns WSError::WS_OK if called success, otherwise failed.
     * @permission Make sure the caller has system permission.
     */
    virtual WSError OnSystemSessionEvent(SessionEvent event) { return WSError::WS_OK; }
    virtual WMError SetSystemWindowEnableDrag(bool enableDrag) { return WMError::WM_OK; }

    /**
     * @brief Callback for processing full-screen layout changes.
     *
     * @param isLayoutFullScreen Indicates the {@link bool}
     * @return Returns WSError::WS_OK if called success, otherwise failed.
     */
    virtual WSError OnLayoutFullScreenChange(bool isLayoutFullScreen) { return WSError::WS_OK; }

    /**
     * @brief Raise the application subwindow to the top layer of the application.
     *
     * @return Returns WSError::WS_OK if called success, otherwise failed.
     * @permission Make sure the caller has system permission.
     */
    virtual WSError RaiseToAppTop() { return WSError::WS_OK; }

    /**
     * @brief Update window size and position.
     *
     * @param rect Indicates the {@link WSRect} structure containing required size and position.
     * @param reason Indicates the {@link SizeChangeReason} reason.
     * @param isGlobal Indicates the {@link bool}.
     * @return Returns WSError::WS_OK if called success, otherwise failed.
     */
    virtual WSError UpdateSessionRect(
        const WSRect& rect, const SizeChangeReason& reason, bool isGlobal = false) { return WSError::WS_OK; }
    virtual WSError OnNeedAvoid(bool status) { return WSError::WS_OK; }
    virtual AvoidArea GetAvoidAreaByType(AvoidAreaType type) { return {}; }
    virtual WSError RequestSessionBack(bool needMoveToBackground) { return WSError::WS_OK; }
    virtual WSError MarkProcessed(int32_t eventId) { return WSError::WS_OK; }

    /**
     * @brief Sets the global maximization mode of window.
     *
     * @param mode Indicates the {@link MaximizeMode}.
     * @return Returns WSError::WS_OK if called success, otherwise failed.
     */
    virtual WSError SetGlobalMaximizeMode(MaximizeMode mode) { return WSError::WS_OK; }

    /**
     * @brief Obtains the global maximization mode of window.
     *
     * @param mode Indicates the {@link MaximizeMode}.
     * @return Returns WSError::WS_OK if called success, otherwise failed.
     */
    virtual WSError GetGlobalMaximizeMode(MaximizeMode& mode) { return WSError::WS_OK; }

    /**
     * @brief Sets the aspect ratio of window.
     *
     * @param ratio Indicates the {@link float}
     * @return Returns WSError::WS_OK if called success, otherwise failed.
     */
    virtual WSError SetAspectRatio(float ratio) { return WSError::WS_OK; }
    virtual WSError UpdateWindowAnimationFlag(bool needDefaultAnimationFlag) { return WSError::WS_OK; }
    virtual WSError UpdateWindowSceneAfterCustomAnimation(bool isAdd) { return WSError::WS_OK; }

    /**
     * @brief Raise a subwindow above a target subwindow.
     *
     * @param subWindowId Indicates the {@link int32_t} id of the target subwindow.
     * @return Returns WSError::WS_OK if called success, otherwise failed.
     * @permission Make sure the caller has system permission.
     */
    virtual WSError RaiseAboveTarget(int32_t subWindowId) { return WSError::WS_OK; }

    /**
     * @brief Raise the application main window to the top layer of the application.
     *
     * @return Returns WSError::WS_OK if called success, otherwise failed.
     */
    virtual WSError RaiseAppMainWindowToTop() { return WSError::WS_OK; }
    virtual WSError PendingSessionActivation(const sptr<AAFwk::SessionInfo> abilitySessionInfo)
        { return WSError::WS_OK; }
    virtual WSError TerminateSession(const sptr<AAFwk::SessionInfo> abilitySessionInfo) { return WSError::WS_OK; }
    virtual WSError SetLandscapeMultiWindow(bool isLandscapeMultiWindow) { return WSError::WS_OK; }
    virtual WSError NotifySessionException(
        const sptr<AAFwk::SessionInfo> abilitySessionInfo, bool needRemoveSession = false) { return WSError::WS_OK; }

    // extension session
    virtual WSError TransferAbilityResult(uint32_t resultCode, const AAFwk::Want& want) { return WSError::WS_OK; }
    virtual WSError TransferExtensionData(const AAFwk::WantParams& wantParams) { return WSError::WS_OK; }
    virtual WSError TransferAccessibilityEvent(const Accessibility::AccessibilityEventInfo& info,
        int64_t uiExtensionIdLevel)
    {
        return WSError::WS_OK;
    }
    virtual WSError NotifyFrameLayoutFinishFromApp(bool notifyListener, const WSRect& rect)
    {
        return WSError::WS_OK;
    }
    virtual void NotifyExtensionDied() {}
    virtual void NotifyExtensionTimeout(int32_t errorCode) {}
    virtual void TriggerBindModalUIExtension() {}
    virtual void NotifySyncOn() {}
    virtual void NotifyAsyncOn() {}
    virtual void NotifyTransferAccessibilityEvent(const Accessibility::AccessibilityEventInfo& info,
        int64_t uiExtensionIdLevel) {}

    /**
     * @brief Close pip window while stopPip is called.
     *
     * Notify system that pip window is stopping and execute animation.
     */
    virtual void NotifyPiPWindowPrepareClose() {}

    /**
     * @brief Update the required params to system.
     *
     * Update the required rect and reason to determine the final size of pip window. Called when start pip,
     * show pip window, update pip size and pip restore.\n
     * Make sure the caller's process is same with the process which created pip window.\n
     *
     * @param rect Indicates the {@link Rect} structure containing required size and position.
     * @param reason Indicates the {@link SizeChangeReason} reason.
     * @return Returns WSError::WS_OK if called success, otherwise failed.
     */
    virtual WSError UpdatePiPRect(const Rect& rect, SizeChangeReason reason) { return WSError::WS_OK; }

    /**
     * @brief Update the pip control status to pip control panel.
     *
     * Called when the specified component's status needs to be updated.\n
     * Make sure the caller's process is same with the process which created pip window.\n
     *
     * @param controlType Indicates the {@link WsPiPControlType} component in pip control panel.
     * @param status Indicates the {@link WsPiPControlStatus} status of specified component.
     * @return Returns WSError::WS_OK if called success, otherwise failed.
     */
    virtual WSError UpdatePiPControlStatus(WsPiPControlType controlType, WsPiPControlStatus status)
    {
        return WSError::WS_OK;
    }
    virtual WSError ProcessPointDownSession(int32_t posX, int32_t posY) { return WSError::WS_OK; }
    virtual WSError SendPointEventForMoveDrag(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
    {
        return WSError::WS_OK;
    }
    virtual WSError ChangeSessionVisibilityWithStatusBar(const sptr<AAFwk::SessionInfo> abilitySessionInfo,
        bool isShow) { return WSError::WS_OK; }

    /**
     * @brief Instruct the application to update the listening flag for registering rect changes.
     *
     * @param isRegister Indicates the {@link bool}
     * @return Returns WSError::WS_OK if called success, otherwise failed.
     */
    virtual WSError UpdateRectChangeListenerRegistered(bool isRegister)
    {
        return WSError::WS_OK;
    }
    virtual WSError SetKeyboardSessionGravity(SessionGravity gravity, uint32_t percent)
    {
        return WSError::WS_OK;
    }
    virtual void SetCallingSessionId(uint32_t callingSessionId) {};
    virtual void SetCustomDecorHeight(int32_t height) {};
    virtual WMError UpdateSessionPropertyByAction(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action) { return WMError::WM_OK; }
    virtual WMError GetAppForceLandscapeConfig(AppForceLandscapeConfig& config) { return WMError::WM_OK; }
    virtual WSError AdjustKeyboardLayout(const KeyboardLayoutParams& params) { return WSError::WS_OK; }
    virtual int32_t GetStatusBarHeight() { return 0; }
    virtual WSError SetDialogSessionBackGestureEnabled(bool isEnabled) { return WSError::WS_OK; }
    virtual void NotifyExtensionEventAsync(uint32_t notifyEvent) {};
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_INTERFACE_H
