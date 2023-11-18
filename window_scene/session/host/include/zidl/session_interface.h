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
        int32_t pid = -1, int32_t uid = -1) = 0;
    virtual WSError Foreground(sptr<WindowSessionProperty> property) = 0;
    virtual WSError Background() = 0;
    virtual WSError Disconnect() = 0;

    // scene session
    virtual WSError UpdateActiveStatus(bool isActive) { return WSError::WS_OK; }
    virtual WSError OnSessionEvent(SessionEvent event) { return WSError::WS_OK; }
    virtual WSError RaiseToAppTop() { return WSError::WS_OK; }
    virtual WSError UpdateSessionRect(const WSRect& rect, const SizeChangeReason& reason) { return WSError::WS_OK; }
    virtual WSError CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session,
        sptr<IRemoteObject> token = nullptr) { return WSError::WS_OK; }
    virtual WSError DestroyAndDisconnectSpecificSession(const int32_t& persistentId) { return WSError::WS_OK; }
    virtual WSError OnNeedAvoid(bool status) { return WSError::WS_OK; }
    virtual AvoidArea GetAvoidAreaByType(AvoidAreaType type) { return {}; }
    virtual WSError RequestSessionBack(bool needMoveToBackground) { return WSError::WS_OK; }
    virtual WSError MarkProcessed(int32_t eventId) { return WSError::WS_OK; }
    virtual WSError SetGlobalMaximizeMode(MaximizeMode mode) { return WSError::WS_OK; }
    virtual WSError GetGlobalMaximizeMode(MaximizeMode& mode) { return WSError::WS_OK; }
    virtual WSError SetSessionProperty(const sptr<WindowSessionProperty>& property) { return WSError::WS_OK; }
    virtual WSError SetAspectRatio(float ratio) { return WSError::WS_OK; }
    virtual WSError UpdateWindowAnimationFlag(bool needDefaultAnimationFlag) { return WSError::WS_OK; }
    virtual WSError UpdateWindowSceneAfterCustomAnimation(bool isAdd) { return WSError::WS_OK; }
    virtual WSError RaiseAboveTarget(int32_t subWindowId) { return WSError::WS_OK; }
    virtual WSError PendingSessionActivation(const sptr<AAFwk::SessionInfo> abilitySessionInfo)
        { return WSError::WS_OK; }
    virtual WSError TerminateSession(const sptr<AAFwk::SessionInfo> abilitySessionInfo) { return WSError::WS_OK; }
    virtual WSError NotifySessionException(const sptr<AAFwk::SessionInfo> abilitySessionInfo) { return WSError::WS_OK; }
    virtual WSError SetTextFieldAvoidInfo(double textFieldPositionY, double textFieldHeight) { return WSError::WS_OK; }

    // extension session
    virtual WSError TransferAbilityResult(uint32_t resultCode, const AAFwk::Want& want) { return WSError::WS_OK; }
    virtual WSError TransferExtensionData(const AAFwk::WantParams& wantParams) { return WSError::WS_OK; }
    virtual WSError TransferAccessibilityEvent(const Accessibility::AccessibilityEventInfo& info,
        const std::vector<int32_t>& uiExtensionIdLevelVec)
    {
        return WSError::WS_OK;
    }
    virtual void NotifyRemoteReady() {}
    virtual void NotifyExtensionDied() {}
    virtual void NotifySyncOn() {}
    virtual void NotifyAsyncOn() {}
    virtual void NotifyTransferAccessibilityEvent(const Accessibility::AccessibilityEventInfo& info,
        const std::vector<int32_t>& uiExtensionIdLevelVec) {}

    // PictureInPicture
    virtual void NotifyPiPWindowPrepareClose() {}
    virtual WSError UpdatePiPRect(uint32_t width, uint32_t height, PiPRectUpdateReason reason)
        { return WSError::WS_OK; }
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_INTERFACE_H
