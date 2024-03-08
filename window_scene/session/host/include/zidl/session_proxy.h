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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_PROXY_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_PROXY_H

#include <iremote_proxy.h>

#include "session/host/include/zidl/session_interface.h"

namespace OHOS::Rosen {
class SessionProxy : public IRemoteProxy<ISession> {
public:
    explicit SessionProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<ISession>(impl) {}
    virtual ~SessionProxy() = default;

    WSError Foreground(sptr<WindowSessionProperty> property) override;
    WSError Background() override;
    WSError Disconnect(bool isFromClient = false) override;
    WSError Show(sptr<WindowSessionProperty> property) override;
    WSError Hide() override;
    WSError Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
        sptr<WindowSessionProperty> property = nullptr, sptr<IRemoteObject> token = nullptr,
        int32_t pid = -1, int32_t uid = -1) override;
    WSError UpdateActiveStatus(bool isActive) override;
    WSError PendingSessionActivation(const sptr<AAFwk::SessionInfo> abilitySessionInfo) override;
    bool WriteAbilitySessionInfoBasic(MessageParcel& data, const sptr<AAFwk::SessionInfo> abilitySessionInfo);
    WSError TerminateSession(const sptr<AAFwk::SessionInfo> abilitySessionInfo) override;
    WSError NotifySessionException(
        const sptr<AAFwk::SessionInfo> abilitySessionInfo, bool needRemoveSession = false) override;
    WSError OnSessionEvent(SessionEvent event) override;
    WSError RaiseToAppTop() override;
    WSError UpdateSessionRect(const WSRect& rect, const SizeChangeReason& reason) override;
    WSError OnNeedAvoid(bool status) override;
    AvoidArea GetAvoidAreaByType(AvoidAreaType type) override;
    WSError RequestSessionBack(bool needMoveToBackground) override;
    WSError MarkProcessed(int32_t eventId) override;
    WSError SetGlobalMaximizeMode(MaximizeMode mode) override;
    WSError GetGlobalMaximizeMode(MaximizeMode& mode) override;
    WSError SetSessionProperty(const sptr<WindowSessionProperty>& property) override;
    WSError SetAspectRatio(float ratio) override;
    WSError UpdateWindowAnimationFlag(bool needDefaultAnimationFlag) override;
    WSError UpdateWindowSceneAfterCustomAnimation(bool isAdd) override;
    WSError RaiseAboveTarget(int32_t subWindowId) override;
    WSError RaiseAppMainWindowToTop() override;

    WSError TransferAbilityResult(uint32_t resultCode, const AAFwk::Want& want) override;
    WSError TransferExtensionData(const AAFwk::WantParams& wantParams) override;
    WSError TransferAccessibilityEvent(const Accessibility::AccessibilityEventInfo& info,
        int64_t uiExtensionIdLevel) override;
    void NotifyRemoteReady() override;
    void NotifySyncOn() override;
    void NotifyAsyncOn() override;
    void NotifyExtensionDied() override;
    void TriggerBindModalUIExtension() override;

    void NotifyPiPWindowPrepareClose() override;
    WSError UpdatePiPRect(const Rect& rect, SizeChangeReason reason) override;
    WSError RecoveryPullPiPMainWindow(int32_t persistentId, const Rect& rect) override;
    WSError ProcessPointDownSession(int32_t posX, int32_t posY) override;
    WSError SendPointEventForMoveDrag(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override;
private:
    static inline BrokerDelegator<SessionProxy> delegator_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_PROXY_H
