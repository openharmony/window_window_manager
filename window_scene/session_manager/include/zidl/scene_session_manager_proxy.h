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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_PROXY_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_PROXY_H

#include <iremote_proxy.h>

#include "session_manager/include/zidl/scene_session_manager_interface.h"

namespace OHOS::Rosen {
class SceneSessionManagerProxy : public IRemoteProxy<ISceneSessionManager> {
public:
    explicit SceneSessionManagerProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<ISceneSessionManager>(impl) {}
    virtual ~SceneSessionManagerProxy() = default;

    WSError CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session,
        sptr<IRemoteObject> token = nullptr) override;
    WSError DestroyAndDisconnectSpecificSession(const int32_t& persistentId) override;
    WMError UpdateProperty(sptr<WindowSessionProperty>& property, WSPropertyChangeAction action) override;
    WSError BindDialogTarget(uint64_t persistentId, sptr<IRemoteObject> targetToken) override;

    WMError RegisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) override;
    WMError UnregisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) override;
    void GetFocusWindowInfo(FocusChangeInfo& focusInfo) override;
    WSError SetSessionGravity(int32_t persistentId, SessionGravity gravity, uint32_t percent) override;
    WMError SetGestureNavigaionEnabled(bool enable) override;
    WSError SetSessionLabel(const sptr<IRemoteObject> &token, const std::string &label) override;
    WSError SetSessionIcon(const sptr<IRemoteObject> &token, const std::shared_ptr<Media::PixelMap> &icon) override;
    WSError IsValidSessionIds(const std::vector<int32_t> &sessionIds, std::vector<bool> &results) override;
    WSError RegisterSessionListener(const sptr<ISessionChangeListener> sessionListener) override;
    void UnregisterSessionListener() override;
    WMError GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos) override;
    WSError PendingSessionToForeground(const sptr<IRemoteObject> &token) override;
    WSError PendingSessionToBackgroundForDelegator(const sptr<IRemoteObject> &token) override;
    WSError GetFocusSessionToken(sptr<IRemoteObject> &token) override;

    WSError RegisterSessionListener(const sptr<ISessionListener>& listener) override;
    WSError UnRegisterSessionListener(const sptr<ISessionListener>& listener) override;
    WSError GetSessionInfos(const std::string& deviceId, int32_t numMax,
                            std::vector<SessionInfoBean>& sessionInfos) override;
    WSError GetSessionInfo(const std::string& deviceId, int32_t persistentId, SessionInfoBean& sessionInfo) override;

    WSError SetSessionContinueState(const sptr<IRemoteObject> &token, const ContinueState& continueState) override;
    WSError TerminateSessionNew(const sptr<AAFwk::SessionInfo> info, bool needStartCaller) override;
    WSError GetSessionDumpInfo(const std::vector<std::string>& params, std::string& info) override;
    void NotifyDumpInfoResult(const std::vector<std::string>& info) override;
    WSError UpdateSessionAvoidAreaListener(int32_t& persistentId, bool haveListener) override;
    WSError GetSessionSnapshot(const std::string& deviceId, int32_t persistentId,
                               std::shared_ptr<Media::PixelMap> &snapshot, bool isLowResolution) override;
    WSError ClearSession(int32_t persistentId) override;
    WSError ClearAllSessions() override;
    WSError RegisterIAbilityManagerCollaborator(int32_t type, const sptr<AAFwk::IAbilityManagerCollaborator> &impl) override;
    WSError UnregisterIAbilityManagerCollaborator(int32_t type) override;

private:
    template<typename T>
    WSError GetParcelableInfos(MessageParcel& reply, std::vector<T>& parcelableInfos);
    static inline BrokerDelegator<SceneSessionManagerProxy> delegator_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_PROXY_H
