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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_RPOXY_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_RPOXY_H

#include <iremote_proxy.h>
#include "interfaces/include/ws_common.h"
#include "session/host/include/zidl/session_interface.h"

namespace OHOS::Rosen {
class SessionProxy : public IRemoteProxy<ISession> {
public:
    explicit SessionProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<ISession>(impl) {};
    ~SessionProxy() {};

    WSError Foreground() override;
    WSError Background() override;
    WSError Disconnect() override;
    WSError Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
        sptr<WindowSessionProperty> property = nullptr, sptr<IRemoteObject> token = nullptr) override;
    WSError UpdateActiveStatus(bool isActive) override;
    WSError PendingSessionActivation(const sptr<AAFwk::SessionInfo> abilitySessionInfo) override;
    WSError TerminateSession(const sptr<AAFwk::SessionInfo> abilitySessionInfo) override;
    WSError NotifySessionException(const sptr<AAFwk::SessionInfo> abilitySessionInfo) override;
    WSError OnSessionEvent(SessionEvent event) override;
    WSError RaiseToAppTop() override;
    WSError UpdateSessionRect(const WSRect& rect, const SizeChangeReason& reason) override;
    WSError CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, uint64_t& persistentId, sptr<ISession>& session) override;
    WSError DestroyAndDisconnectSpecificSession(const uint64_t& persistentId) override;
    WSError OnNeedAvoid(bool status) override;
    AvoidArea GetAvoidAreaByType(AvoidAreaType type) override;
    WSError RequestSessionBack() override;
    WSError MarkProcessed(int32_t eventId) override;
    WSError SetGlobalMaximizeMode(MaximizeMode mode) override;
    WSError GetGlobalMaximizeMode(MaximizeMode& mode) override;
    WSError UpdateWindowSessionProperty(sptr<WindowSessionProperty> property) override;
    WSError SetAspectRatio(float ratio) override;
    WSError TransferAbilityResult(uint32_t resultCode, const AAFwk::Want& want) override;
    WSError TransferExtensionData(const AAFwk::WantParams& wantParams) override;
private:
    static inline BrokerDelegator<SessionProxy> delegator_;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_RPOXY_H
