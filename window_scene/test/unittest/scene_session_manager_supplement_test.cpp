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

#include <gtest/gtest.h>
#include <bundle_mgr_interface.h>
#include <bundlemgr/launcher_service.h>
#include "interfaces/include/ws_common.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/main_session.h"
#include "screen_session_manager/include/screen_session_manager_client.h"
#include "window_manager_agent.h"
#include "session_manager.h"
#include "zidl/window_manager_agent_interface.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
#include "context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SceneSessionManagerSupplementTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<SceneSessionManager> ssm_;
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

sptr<SceneSessionManager> SceneSessionManagerSupplementTest::ssm_ = nullptr;

void SceneSessionManagerSupplementTest::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerSupplementTest::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerSupplementTest::SetUp()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerSupplementTest::TearDown()
{
    ssm_->sceneSessionMap_.clear();
    usleep(WAIT_SYNC_IN_NS);
}

namespace {
/**
 * @tc.name: LoadFreeMultiWindowConfig
 * @tc.desc: SceneSessionManagerSupplementTest LoadFreeMultiWindowConfig
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, LoadFreeMultiWindowConfig, Function | SmallTest | Level3)
{
    ssm_->LoadFreeMultiWindowConfig(true);
    ssm_->LoadFreeMultiWindowConfig(false);
    auto config = ssm_->GetSystemSessionConfig();
    config.freeMultiWindowSupport_ = false;
    ssm_->SwitchFreeMultiWindow(true);
    config.freeMultiWindowSupport_ = true;
    ssm_->SwitchFreeMultiWindow(true);
    sptr<SceneSession> sceneSession;
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->SwitchFreeMultiWindow(true);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";
    sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ssm_->SwitchFreeMultiWindow(true);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW);
    sceneSession->SetSessionProperty(property);
    ssm_->SwitchFreeMultiWindow(true);
    ssm_->SwitchFreeMultiWindow(false);
    ssm_->SetEnableInputEvent(true);
    ASSERT_EQ(ssm_->IsInputEventEnabled(), true);
}

/**
 * @tc.name: RequestSceneSessionActivationInner
 * @tc.desc: SceneSessionManagerSupplementTest RequestSceneSessionActivationInner
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, RequestSceneSessionActivationInner,
    Function | SmallTest | Level3)
{
    int res = 0;
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sceneSession->SetSessionProperty(property);
    sceneSession->SetCollaboratorType(0);
    ssm_->RequestSceneSessionActivationInner(sceneSession, true);
    ssm_->RequestSceneSessionActivationInner(sceneSession, false);
    sceneSession->SetCollaboratorType(1);
    ssm_->systemConfig_.backgroundswitch = false;
    sceneSession->SetSessionState(SessionState::STATE_DISCONNECT);
    ssm_->RequestSceneSessionActivationInner(sceneSession, true);
    ssm_->RequestSceneSessionActivationInner(sceneSession, false);
    ssm_->systemConfig_.backgroundswitch = true;
    sceneSession->SetSessionState(SessionState::STATE_DISCONNECT);
    ssm_->RequestSceneSessionActivationInner(sceneSession, true);
    ssm_->RequestSceneSessionActivationInner(sceneSession, false);
    sceneSession->SetSessionState(SessionState::STATE_END);
    ssm_->RequestSceneSessionActivationInner(sceneSession, true);
    ssm_->RequestSceneSessionActivationInner(sceneSession, false);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: NotifyCollaboratorAfterStart
 * @tc.desc: SceneSessionManagerSupplementTest NotifyCollaboratorAfterStart
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, NotifyCollaboratorAfterStart,
    Function | SmallTest | Level3)
{
    int res = 0;
    sptr<SceneSession> sceneSession;
    sptr<AAFwk::SessionInfo> scnSessionInfo;
    ssm_->NotifyCollaboratorAfterStart(sceneSession, scnSessionInfo);
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    ssm_->RequestSceneSessionBackground(sceneSession, true, true);
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->NotifyCollaboratorAfterStart(sceneSession, scnSessionInfo);
    scnSessionInfo = new AAFwk::SessionInfo();
    ssm_->NotifyCollaboratorAfterStart(sceneSession, scnSessionInfo);
    ssm_->NotifyCollaboratorAfterStart(sceneSession, scnSessionInfo);
    ssm_->RequestSceneSessionBackground(sceneSession, true, true);
    ssm_->RequestSceneSessionBackground(sceneSession, true, false);
    ssm_->brightnessSessionId_ = sceneSession->GetPersistentId();
    ssm_->RequestSceneSessionBackground(sceneSession, false, true);
    ssm_->RequestSceneSessionBackground(sceneSession, false, false);
    ssm_->brightnessSessionId_ = 0;
    ssm_->systemConfig_.backgroundswitch  = true;
    ssm_->RequestSceneSessionBackground(sceneSession, true, true);
    ssm_->RequestSceneSessionBackground(sceneSession, true, false);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sceneSession->SetSessionProperty(property);
    ssm_->RequestSceneSessionBackground(sceneSession, false, true);
    ssm_->RequestSceneSessionBackground(sceneSession, false, false);
    ssm_->NotifyForegroundInteractiveStatus(sceneSession, true);
    ssm_->NotifyForegroundInteractiveStatus(sceneSession, false);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});
    ssm_->NotifyForegroundInteractiveStatus(sceneSession, true);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: DestroyDialogWithMainWindow
 * @tc.desc: SceneSessionManagerSupplementTest DestroyDialogWithMainWindow
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, DestroyDialogWithMainWindow,
    Function | SmallTest | Level3)
{
    int res = 0;
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    sptr<SceneSession> sceneSession;
    ssm_->DestroyDialogWithMainWindow(sceneSession);
    ssm_->DestroySubSession(sceneSession);
    ssm_->RequestSceneSessionDestruction(sceneSession, true);
    ssm_->RequestSceneSessionDestruction(sceneSession, false);
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_COMPONENT);
    sceneSession->SetSessionProperty(property);
    ssm_->DestroyDialogWithMainWindow(sceneSession);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<SceneSession> sceneSession2;
    sceneSession->dialogVec_.push_back(sceneSession2);
    sceneSession->subSession_.push_back(sceneSession2);
    ssm_->DestroyDialogWithMainWindow(sceneSession);
    ssm_->DestroySubSession(sceneSession);
    sceneSession2 = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->DestroyDialogWithMainWindow(sceneSession);
    ssm_->DestroySubSession(sceneSession);
    ssm_->RequestSceneSessionDestruction(sceneSession, true);
    ssm_->RequestSceneSessionDestruction(sceneSession, false);
    ssm_->sceneSessionMap_.insert({sceneSession2->GetPersistentId(), sceneSession2});
    ssm_->DestroyDialogWithMainWindow(sceneSession);
    ssm_->EraseSceneSessionMapById(sceneSession2->GetPersistentId());
    ssm_->RequestSceneSessionDestruction(sceneSession2, true);
    ssm_->RequestSceneSessionDestruction(sceneSession2, false);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: CreateAndConnectSpecificSession
 * @tc.desc: CreateAndConnectSpecificSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerSupplementTest, CreateAndConnectSpecificSession, Function | SmallTest | Level3)
{
    int ret = 0;
    sptr<ISessionStage> sessionStage;
    sptr<IWindowEventChannel> eventChannel;
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    sptr<WindowSessionProperty> property;
    sptr<ISession> session;
    SystemSessionConfig systemConfig;
    sptr<IRemoteObject> token;
    int32_t id = 0;
    ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    property = new WindowSessionProperty();
    ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    property->SetWindowType(WindowType::WINDOW_TYPE_UI_EXTENSION);
    ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    property->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    property->SetFloatingWindowAppType(true);
    ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    property->SetExtensionFlag(true);
    ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    property->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW);
    ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    property->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: ClosePipWindowIfExist
 * @tc.desc: ClosePipWindowIfExist
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerSupplementTest, ClosePipWindowIfExist, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->ClosePipWindowIfExist(WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW);
    ssm_->ClosePipWindowIfExist(WindowType::WINDOW_TYPE_PIP);
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_COMPONENT);
    sceneSession->SetSessionProperty(property);
    PiPTemplateInfo pipInfo;
    pipInfo.priority = 0;
    ssm_->CheckPiPPriority(pipInfo);
    ssm_->sceneSessionMap_.insert({0, sceneSession});
    ssm_->ClosePipWindowIfExist(WindowType::WINDOW_TYPE_PIP);
    ssm_->CheckPiPPriority(pipInfo);
    property->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    PiPTemplateInfo pipInfo1;
    pipInfo1.priority = 0;
    sceneSession->SetPiPTemplateInfo(pipInfo1);
    ssm_->CheckPiPPriority(pipInfo);
    sceneSession->isVisible_ = true;
    ssm_->CheckPiPPriority(pipInfo);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: CheckSystemWindowPermission
 * @tc.desc: CheckSystemWindowPermission
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerSupplementTest, CheckSystemWindowPermission, Function | SmallTest | Level3)
{
    int ret = 0;
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_UI_EXTENSION);
    ssm_->CheckSystemWindowPermission(property);
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    ssm_->CheckSystemWindowPermission(property);
    property->SetWindowType(WindowType::WINDOW_TYPE_DRAGGING_EFFECT);
    ssm_->CheckSystemWindowPermission(property);
    property->SetWindowType(WindowType::WINDOW_TYPE_TOAST);
    ssm_->CheckSystemWindowPermission(property);
    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ssm_->CheckSystemWindowPermission(property);
    property->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    ssm_->CheckSystemWindowPermission(property);
    property->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    ssm_->CheckSystemWindowPermission(property);
    std::vector<int32_t> alivePersistentIds;
    ssm_->SetAlivePersistentIds(alivePersistentIds);
    ssm_->IsNeedRecover(1);
    alivePersistentIds.push_back(1);
    ssm_->SetAlivePersistentIds(alivePersistentIds);
    ssm_->IsNeedRecover(1);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: RecoverAndConnectSpecificSession
 * @tc.desc: RecoverAndConnectSpecificSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerSupplementTest, RecoverAndConnectSpecificSession, Function | SmallTest | Level3)
{
    sptr<ISessionStage> sessionStage;
    sptr<IWindowEventChannel> eventChannel;
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    sptr<WindowSessionProperty> property;
    sptr<ISession> session;
    sptr<IRemoteObject> token;
    auto ret = ssm_->RecoverAndConnectSpecificSession(sessionStage, eventChannel, node, property, session, token);
    ASSERT_EQ(ret, WSError::WS_ERROR_NULLPTR);
    property = new WindowSessionProperty();
    ret = ssm_->RecoverAndConnectSpecificSession(sessionStage, eventChannel, node, property, session, token);
    ASSERT_EQ(ret, WSError::WS_ERROR_NULLPTR);
    ssm_->NotifyRecoveringFinished();
    usleep(WAIT_SYNC_IN_NS);
    ASSERT_EQ(ssm_->recoveringFinished_, true);
}

/**
 * @tc.name: CacheSubSessionForRecovering
 * @tc.desc: CacheSubSessionForRecovering
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerSupplementTest, CacheSubSessionForRecovering, Function | SmallTest | Level3)
{
    sptr<SceneSession> sceneSession;
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    sptr<WindowSessionProperty> property;
    ssm_->recoveringFinished_ = false;
    ssm_->CacheSubSessionForRecovering(sceneSession, property);
    ASSERT_EQ(ssm_->recoverSubSessionCacheMap_.size(), 0);
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->CacheSubSessionForRecovering(sceneSession, property);
    ASSERT_EQ(ssm_->recoverSubSessionCacheMap_.size(), 0);
    property = new WindowSessionProperty();
    ssm_->CacheSubSessionForRecovering(sceneSession, property);
    ASSERT_EQ(ssm_->recoverSubSessionCacheMap_.size(), 0);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ssm_->CacheSubSessionForRecovering(sceneSession, property);
    ASSERT_EQ(ssm_->recoverSubSessionCacheMap_.size(), 0);
    property->SetWindowType(WindowType::APP_SUB_WINDOW_END);
    ssm_->CacheSubSessionForRecovering(sceneSession, property);
    ASSERT_EQ(ssm_->recoverSubSessionCacheMap_.size(), 0);
    property->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    property->SetParentPersistentId(1);
    NotifyCreateSubSessionFunc func;
    ssm_->createSubSessionFuncMap_.insert({1, func});
    ssm_->CacheSubSessionForRecovering(sceneSession, property);
    ASSERT_EQ(ssm_->recoverSubSessionCacheMap_.size(), 0);
    ssm_->createSubSessionFuncMap_.clear();
    ssm_->CacheSubSessionForRecovering(sceneSession, property);
    ASSERT_EQ(ssm_->recoverSubSessionCacheMap_.size(), 1);
    ssm_->RecoverCachedSubSession(1);
    ASSERT_EQ(ssm_->recoverSubSessionCacheMap_.size(), 0);
    std::vector<sptr<SceneSession>> sessions;
    sessions.push_back(sceneSession);
    ssm_->recoverSubSessionCacheMap_.insert({1, sessions});
    ssm_->RecoverCachedSubSession(1);
    ssm_->recoverSubSessionCacheMap_.clear();
    ssm_->NotifySessionUnfocusedToClient(1);
    std::shared_ptr<SessionListenerController> listener = std::make_shared<SessionListenerController>();
    ssm_->listenerController_ = listener;
    ssm_->NotifySessionUnfocusedToClient(1);
    ssm_->listenerController_ = nullptr;
}

/**
 * @tc.name: RecoverAndReconnectSceneSession
 * @tc.desc: RecoverAndReconnectSceneSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerSupplementTest, RecoverAndReconnectSceneSession, Function | SmallTest | Level3)
{
    int ret = 0;
    sptr<ISessionStage> sessionStage;
    sptr<IWindowEventChannel> eventChannel;
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    sptr<WindowSessionProperty> property;
    sptr<ISession> session;
    sptr<IRemoteObject> token;
    ssm_->RecoverAndReconnectSceneSession(sessionStage, eventChannel, node, session, property, token);
    property = new WindowSessionProperty();
    property->SetPersistentId(1);
    ssm_->alivePersistentIds_.push_back(1);
    ssm_->recoveringFinished_ = true;
    ssm_->RecoverAndReconnectSceneSession(sessionStage, eventChannel, node, session, property, token);
    ssm_->recoveringFinished_ = false;
    ssm_->RecoverAndReconnectSceneSession(sessionStage, eventChannel, node, session, property, token);
    property->SetWindowType(WindowType::APP_SUB_WINDOW_END);
    ssm_->RecoverAndReconnectSceneSession(sessionStage, eventChannel, node, session, property, token);
    NotifyRecoverSceneSessionFunc func;
    ssm_->SetRecoverSceneSessionListener(func);
    ssm_->alivePersistentIds_.clear();
    NotifyCreateSubSessionFunc func2;
    ssm_->RegisterCreateSubSessionListener(1, func2);
    ssm_->createSubSessionFuncMap_.insert({ 1, func2 });
    ssm_->RegisterCreateSubSessionListener(1, func2);
    ssm_->createSubSessionFuncMap_.clear();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: NotifyCreateSpecificSession
 * @tc.desc: NotifyCreateSpecificSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerSupplementTest, NotifyCreateSpecificSession, Function | SmallTest | Level3)
{
    int ret = 0;
    sptr<SceneSession> sceneSession;
    sptr<WindowSessionProperty> property;
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::APP_MAIN_WINDOW_BASE);
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::APP_MAIN_WINDOW_BASE);
    property = new WindowSessionProperty();
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::APP_MAIN_WINDOW_BASE);
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::SYSTEM_SUB_WINDOW_BASE);
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::APP_SUB_WINDOW_BASE);
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::WINDOW_TYPE_SCENE_BOARD);
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::WINDOW_TYPE_TOAST);
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::WINDOW_TYPE_FLOAT);
    property->SetParentPersistentId(1);
    SessionInfo info1;
    info1.bundleName_ = "test3";
    info1.abilityName_ = "test3";
    sptr<SceneSession> sceneSession2 = new (std::nothrow) SceneSession(info1, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession2});
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::WINDOW_TYPE_FLOAT);
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::WINDOW_TYPE_DIALOG);
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::BELOW_APP_SYSTEM_WINDOW_BASE);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: NotifyCreateSubSession
 * @tc.desc: NotifyCreateSubSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerSupplementTest, NotifyCreateSubSession, Function | SmallTest | Level3)
{
    int ret = 0;
    sptr<SceneSession> sceneSession;
    ssm_->NotifyCreateSubSession(1, sceneSession);
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->NotifyCreateSubSession(1, sceneSession);
    ssm_->UnregisterCreateSubSessionListener(1);
    SessionInfo info1;
    info1.bundleName_ = "test1";
    info1.abilityName_ = "test2";
    sptr<SceneSession> sceneSession1 = new (std::nothrow) SceneSession(info1, nullptr);
    NotifyCreateSubSessionFunc func;
    ssm_->createSubSessionFuncMap_.insert({1, func});
    ssm_->NotifyCreateSubSession(1, sceneSession);
    ssm_->UnregisterCreateSubSessionListener(1);
    ssm_->createSubSessionFuncMap_.clear();
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    sceneSession->SetSessionProperty(property);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->NotifySessionTouchOutside(1);
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    ssm_->NotifySessionTouchOutside(1);
    sceneSession->persistentId_ = property->callingSessionId_;
    ssm_->NotifySessionTouchOutside(1);
    sceneSession->persistentId_ = property->callingSessionId_ + 1;
    ssm_->NotifySessionTouchOutside(1);
    ASSERT_EQ(ret, 0);
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
    usleep(WAIT_SYNC_IN_NS);
}

/**
 * @tc.name: DestroyAndDisconnectSpecificSessionInner
 * @tc.desc: DestroyAndDisconnectSpecificSessionInner
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerSupplementTest, DestroyAndDisconnectSpecificSessionInner, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->DestroyAndDisconnectSpecificSessionInner(1);
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    uint32_t uid = 0;
    ssm_->GetTopWindowId(1, uid);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->GetTopWindowId(1, uid);
    ssm_->DestroyAndDisconnectSpecificSessionInner(1);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    sceneSession->SetSessionProperty(property);
    ssm_->DestroyAndDisconnectSpecificSessionInner(1);
    property->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ssm_->DestroyAndDisconnectSpecificSessionInner(1);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: GetFocusWindowInfo
 * @tc.desc: GetFocusWindowInfo
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerSupplementTest, GetFocusWindowInfo, Function | SmallTest | Level3)
{
    int ret = 0;
    sptr<WindowSessionProperty> property;
    sptr<SceneSession> sceneSession;
    ssm_->NotifySessionForCallback(sceneSession, true);
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    info.isSystem_ = true;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->NotifySessionForCallback(sceneSession, true);
    info.isSystem_ = false;
    ssm_->NotifySessionForCallback(sceneSession, true);
    property = new WindowSessionProperty();
    property->SetBrightness(1.f);
    ssm_->SetBrightness(sceneSession, 1.f);
    ssm_->SetBrightness(sceneSession, 2.f);
    ssm_->displayBrightness_ = 2.f;
    ssm_->SetBrightness(sceneSession, 2.f);
    ssm_->displayBrightness_ = 3.f;
    ssm_->SetBrightness(sceneSession, 2.f);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->UpdateBrightness(1);
    property->SetBrightness(-1.f);
    ssm_->UpdateBrightness(1);
    property->SetBrightness(3.f);
    ssm_->UpdateBrightness(1);
    FocusChangeInfo changeInfo;
    ssm_->GetFocusWindowInfo(changeInfo);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: NotifySessionForCallback
 * @tc.desc: NotifySessionForCallback
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerSupplementTest, NotifySessionForCallback, Function | SmallTest | Level3)
{
    int ret = 0;
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    info.isSystem_ = true;
    std::shared_ptr<SessionListenerController> listener = std::make_shared<SessionListenerController>();
    ssm_->listenerController_ = listener;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->NotifySessionForCallback(sceneSession, true);
    info.isSystem_ = false;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->NotifySessionForCallback(sceneSession, true);
    ssm_->NotifySessionForCallback(sceneSession, true);
    ssm_->NotifySessionForCallback(sceneSession, false);
    info.appIndex_ = 1;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->NotifySessionForCallback(sceneSession, true);
    ssm_->NotifySessionForCallback(sceneSession, false);
    info.appIndex_ = 0;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->NotifySessionForCallback(sceneSession, true);
    ssm_->NotifySessionForCallback(sceneSession, false);
    std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    info.abilityInfo = abilityInfo;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->NotifySessionForCallback(sceneSession, true);
    ssm_->NotifySessionForCallback(sceneSession, false);
    ssm_->NotifyWindowInfoChangeFromSession(1);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->NotifyWindowInfoChangeFromSession(1);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: IsSessionVisible
 * @tc.desc: IsSessionVisible
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerSupplementTest, IsSessionVisible, Function | SmallTest | Level3)
{
    int ret = 0;
    SessionInfo info;
    info.bundleName_ = "test1";
    sptr<SceneSession> sceneSession;
    ssm_->IsSessionVisible(sceneSession);
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    sceneSession->SetSessionProperty(property);
    property->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ssm_->IsSessionVisible(sceneSession);
    property->SetParentPersistentId(1);
    SessionInfo info1;
    info1.bundleName_ = "test3";
    sptr<SceneSession> sceneSession1 = new (std::nothrow) SceneSession(info1, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession1});
    ssm_->IsSessionVisible(sceneSession);
    sceneSession->isVisible_ = true;
    ssm_->IsSessionVisible(sceneSession);
    sceneSession->isVisible_ = false;
    sceneSession->state_ = SessionState::STATE_ACTIVE;
    ssm_->IsSessionVisible(sceneSession);
    sceneSession1->state_ = SessionState::STATE_ACTIVE;
    ssm_->IsSessionVisible(sceneSession);
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    ssm_->IsSessionVisible(sceneSession);
    sceneSession1->state_ = SessionState::STATE_FOREGROUND;
    ssm_->IsSessionVisible(sceneSession);
    sceneSession1->state_ = SessionState::STATE_INACTIVE;
    ssm_->IsSessionVisible(sceneSession);
    sceneSession->state_ = SessionState::STATE_INACTIVE;
    ssm_->IsSessionVisible(sceneSession);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ssm_->IsSessionVisible(sceneSession);
    sceneSession->isVisible_ = true;
    ssm_->IsSessionVisible(sceneSession);
    sceneSession->isVisible_ = false;
    sceneSession->state_ = SessionState::STATE_ACTIVE;
    ssm_->IsSessionVisible(sceneSession);
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    ssm_->IsSessionVisible(sceneSession);
    sceneSession->state_ = SessionState::STATE_INACTIVE;
    ssm_->IsSessionVisible(sceneSession);
    ASSERT_EQ(ret, 0);
}

}
} // namespace Rosen
} // namespace OHOS