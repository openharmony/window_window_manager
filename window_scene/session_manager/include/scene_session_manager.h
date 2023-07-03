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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_H
#define OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_H

#include "common/include/task_scheduler.h"
#include "interfaces/include/ws_common.h"
#include "session_manager/include/zidl/scene_session_manager_stub.h"
#include "session/host/include/root_scene_session.h"
#include "session_manager/include/zidl/scene_session_manager_stub.h"
#include "wm_single_instance.h"
#include "window_scene_config.h"

namespace OHOS::AAFwk {
class SessionInfo;
} // namespace OHOS::AAFwk

namespace OHOS::AppExecFwk {
class IBundleMgr;
struct AbilityInfo;
} // namespace OHOS::AppExecFwk

namespace OHOS::Global::Resource {
class ResourceManager;
} // namespace OHOS::Global::Resource

namespace OHOS::Rosen {
class SceneSession;
using NotifyCreateSpecificSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using ProcessGestureNavigationEnabledChangeFunc = std::function<void(bool enable)>;
using NotifySetFocusSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using EventHandler = OHOS::AppExecFwk::EventHandler;
using EventRunner = OHOS::AppExecFwk::EventRunner;
class SceneSessionManager : public SceneSessionManagerStub {
WM_DECLARE_SINGLE_INSTANCE_BASE(SceneSessionManager)
public:
    sptr<SceneSession> RequestSceneSession(const SessionInfo& sessionInfo, sptr<WindowSessionProperty> property = nullptr);
    WSError RequestSceneSessionActivation(const sptr<SceneSession>& sceneSession);
    WSError RequestSceneSessionBackground(const sptr<SceneSession>& sceneSession);
    WSError RequestSceneSessionDestruction(const sptr<SceneSession>& sceneSession);
    WSError RequestSceneSessionByCall(const sptr<SceneSession>& sceneSession);
    void StartAbilityBySpecified(const SessionInfo& sessionInfo);
    sptr<RootSceneSession> GetRootSceneSession();
    sptr<SceneSession> GetSceneSession(uint64_t persistentId);
    WSError CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, uint64_t& persistentId, sptr<ISession>& session);
    WSError DestroyAndDisconnectSpecificSession(const uint64_t& persistentId);
    WSError UpdateProperty(sptr<WindowSessionProperty>& property, WSPropertyChangeAction action);
    void SetCreateSpecificSessionListener(const NotifyCreateSpecificSessionFunc& func);
    void SetGestureNavigationEnabledChangeListener(const ProcessGestureNavigationEnabledChangeFunc& func);
    const AppWindowSceneConfig& GetWindowSceneConfig() const;
    WSError ProcessBackEvent();
    void GetStartPage(const SessionInfo& sessionInfo, std::string& path, uint32_t& bgColor);
    WMError SetGestureNavigaionEnabled(bool enable);
    WMError RegisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent);
    WMError UnregisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent);

    WSError SetFocusedSession(uint64_t persistentId);
    uint64_t GetFocusedSession() const;
    WSError UpdateFocus(uint64_t persistentId, bool isFocused);
    WSError SwitchUser(int32_t oldUserId, int32_t newUserId, std::string &fileDir);
    int32_t GetCurrentUserId() const;
    void StartWindowInfoReportLoop();
    void GetFocusWindowInfo(FocusChangeInfo& focusInfo);
    WSError SetSessionLabel(const sptr<IRemoteObject> &token, const std::string &label);
    WSError SetSessionIcon(const sptr<IRemoteObject> &token, const std::shared_ptr<Media::PixelMap> &icon);
    WSError RegisterSessionListener(const sptr<ISessionListener> sessionListener);
    void UnregisterSessionListener();
    void HandleTurnScreenOn(const sptr<SceneSession>& sceneSession);
    void HandleKeepScreenOn(const sptr<SceneSession>& sceneSession, bool requireLock);
    void UpdatePrivateStateAndNotify(bool isAddingPrivateSession);
    void InitPersistentStorage();
    std::string GetSessionSnapshot(uint64_t persistentId);

    WSError SetWindowFlags(const sptr<SceneSession>& sceneSession, uint32_t flags);

    void SetWaterMarkSessionCount(int32_t count);
    int32_t GetWaterMarkSessionCount() const;
protected:
    SceneSessionManager();
    virtual ~SceneSessionManager() = default;

private:
    bool Init();
    void LoadWindowSceneXml();
    void ConfigWindowSceneXml();
    void ConfigWindowEffect(const WindowSceneConfig::ConfigItem& effectConfig);
    void ConfigKeyboardAnimation(const WindowSceneConfig::ConfigItem& animationConfig);
    bool ConfigAppWindowCornerRadius(const WindowSceneConfig::ConfigItem& item, float& out);
    bool ConfigAppWindowShadow(const WindowSceneConfig::ConfigItem& shadowConfig, WindowShadowConfig& outShadow);
    void ConfigDecor(const WindowSceneConfig::ConfigItem& decorConfig);

    sptr<AAFwk::SessionInfo> SetAbilitySessionInfo(const sptr<SceneSession>& scnSession);
    WSError DestroyDialogWithMainWindow(const sptr<SceneSession>& scnSession);
    WSError UpdateParentSession(const sptr<SceneSession>& sceneSession, sptr<WindowSessionProperty> property);
    void UpdateCameraFloatWindowStatus(uint32_t accessTokenId, bool isShowing);
    void UpdateFocusableProperty(uint64_t persistentId);

    sptr<AppExecFwk::IBundleMgr> GetBundleManager();
    std::shared_ptr<Global::Resource::ResourceManager> CreateResourceManager(
        const AppExecFwk::AbilityInfo& abilityInfo);
    void GetStartPageFromResource(const AppExecFwk::AbilityInfo& abilityInfo, std::string& path, uint32_t& bgColor);
    std::string CreateCurve(const WindowSceneConfig::ConfigItem& curveConfig);

    WSError SetBrightness(const sptr<SceneSession>& sceneSession, float brightness);
    WSError UpdateBrightness(uint64_t persistentId);
    void SetDisplayBrightness(float brightness);
    float GetDisplayBrightness() const;

    sptr<RootSceneSession> rootSceneSession_;
    std::map<uint64_t, sptr<SceneSession>> sceneSessionMap_;

    NotifyCreateSpecificSessionFunc createSpecificSessionFunc_;
    ProcessGestureNavigationEnabledChangeFunc gestureNavigationEnabledChangeFunc_;
    AppWindowSceneConfig appWindowSceneConfig_;
    SystemSessionConfig systemConfig_;
    uint64_t activeSessionId_ = INVALID_SESSION_ID;
    uint64_t focusedSessionId_ = INVALID_SESSION_ID;
    uint64_t brightnessSessionId_ = INVALID_SESSION_ID;
    float displayBrightness_ = UNDEFINED_BRIGHTNESS;
    int32_t currentUserId_;

    std::shared_ptr<TaskScheduler> taskScheduler_;
    sptr<AppExecFwk::IBundleMgr> bundleMgr_;
    
    std::shared_ptr<EventRunner> eventLoop_;
    std::shared_ptr<EventHandler> eventHandler_;
    bool isReportTaskStart_ = false;
    void RegisterSessionStateChangeNotifyManagerFunc(sptr<SceneSession>& sceneSession);
    void OnSessionStateChange(uint64_t persistentId);
    sptr<ISessionListener> sessionListener_;

    void CheckAndNotifyWaterMarkChangedResult(bool isAddingWaterMark);
    WSError NotifyWaterMarkFlagChangedResult(bool hasWaterMark);
    int32_t waterMarkSessionCount_ { 0 };
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_H
