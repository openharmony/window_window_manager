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
#include <regex>
#include <bundle_mgr_interface.h>
#include <bundlemgr/launcher_service.h>
#include "interfaces/include/ws_common.h"
#include "screen_fold_data.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/main_session.h"
#include "window_manager_agent.h"
#include "session_manager.h"
#include "zidl/window_manager_agent_interface.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
#include "application_info.h"
#include "context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    const std::string EMPTY_DEVICE_ID = "";
}
class SceneSessionManagerLifecycleTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp() override;

    void TearDown() override;

    static void SetVisibleForAccessibility(sptr<SceneSession>& sceneSession);
    int32_t GetTaskCount(sptr<SceneSession>& session);
    static sptr<SceneSessionManager> ssm_;
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

sptr<SceneSessionManager> SceneSessionManagerLifecycleTest::ssm_ = nullptr;

void WindowChangedFuncTest(int32_t persistentId, WindowUpdateType type)
{
}

void ProcessStatusBarEnabledChangeFuncTest(bool enable)
{
}

void DumpRootSceneElementInfoFuncTest(const std::vector<std::string>& params, std::vector<std::string>& infos)
{
}

void SceneSessionManagerLifecycleTest::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerLifecycleTest::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerLifecycleTest::SetUp()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerLifecycleTest::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerLifecycleTest::SetVisibleForAccessibility(sptr<SceneSession>& sceneSession)
{
    sceneSession->SetTouchable(true);
    sceneSession->forceTouchable_ = true;
    sceneSession->systemTouchable_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    sceneSession->foregroundInteractiveStatus_.store(true);
}

int32_t SceneSessionManagerLifecycleTest::GetTaskCount(sptr<SceneSession>& session)
{
    std::string dumpInfo = session->handler_->GetEventRunner()->GetEventQueue()->DumpCurrentQueueSize();
    std::regex pattern("\\d+");
    std::smatch matches;
    int32_t taskNum = 0;
    while (std::regex_search(dumpInfo, matches, pattern)) {
        taskNum += std::stoi(matches.str());
        dumpInfo = matches.suffix();
    }
    return taskNum;
}

namespace {
/**
 * @tc.name: NotifySessionForeground
 * @tc.desc: SceneSesionManager NotifySessionForeground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, NotifySessionForeground, Function | SmallTest | Level3)
{
    sptr<SceneSession> scensession = nullptr;
    SessionInfo info;
    info.bundleName_ = "bundleName";
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, scensession);
    uint32_t reason = 1;
    bool withAnimation = true;
    scensession->NotifySessionForeground(reason, withAnimation);
}

/**
 * @tc.name: NotifySessionForeground
 * @tc.desc: SceneSesionManager NotifySessionForeground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, NotifySessionBackground, Function | SmallTest | Level3)
{
    sptr<SceneSession> scensession = nullptr;
    SessionInfo info;
    info.bundleName_ = "bundleName";
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, scensession);
    uint32_t reason = 1;
    bool withAnimation = true;
    bool isFromInnerkits = true;
    scensession->NotifySessionBackground(reason, withAnimation, isFromInnerkits);
}

/**
 * @tc.name: ClearUnrecoveredSessions
 * @tc.desc: test func ClearUnrecoveredSessions
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLifecycleTest, ClearUnrecoveredSessions, Function | SmallTest | Level1)
{
    ssm_->alivePersistentIds_.push_back(23);
    ssm_->alivePersistentIds_.push_back(24);
    ssm_->alivePersistentIds_.push_back(25);
    EXPECT_FALSE(ssm_->alivePersistentIds_.empty());
    std::vector<int32_t> recoveredPersistentIds;
    recoveredPersistentIds.push_back(23);
    recoveredPersistentIds.push_back(24);
    ssm_->ClearUnrecoveredSessions(recoveredPersistentIds);
}

/**
 * @tc.name: RecoverSessionInfo
 * @tc.desc: test func RecoverSessionInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLifecycleTest, RecoverSessionInfo, Function | SmallTest | Level1)
{
    SessionInfo info = ssm_->RecoverSessionInfo(nullptr);

    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    info = ssm_->RecoverSessionInfo(property);
}

/**
 * @tc.name: NotifySessionMovedToFront
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, NotifySessionMovedToFront, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->NotifySessionMovedToFront(0);

    SessionInfo info;
    info.abilityName_ = "BackgroundTask02";
    info.bundleName_ = "BackgroundTask02";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->sceneSessionMap_.insert({100, sceneSession});
    ssm_->NotifySessionMovedToFront(100);
}

/**
 * @tc.name: PendingSessionToBackgroundForDelegator
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, PendingSessionToBackgroundForDelegator, Function | SmallTest | Level3)
{
    WSError ret;
    ret = ssm_->PendingSessionToBackgroundForDelegator(nullptr);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_PARAM, ret);

    SessionInfo info;
    info.abilityName_ = "BackgroundTask02";
    info.bundleName_ = "BackgroundTask02";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->sceneSessionMap_.insert({100, sceneSession});
    ret = ssm_->PendingSessionToBackgroundForDelegator(nullptr);
    ASSERT_EQ(WSError::WS_OK, ret);
}

/**
 * @tc.name: OnSessionStateChange
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, OnSessionStateChange, Function | SmallTest | Level3)
{
    ssm_->sceneSessionMap_.clear();
    ssm_->OnSessionStateChange(100, SessionState::STATE_END);
    SessionInfo info;
    info.abilityName_ = "BackgroundTask02";
    info.bundleName_ = "BackgroundTask02";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert({100, sceneSession});
    ssm_->OnSessionStateChange(100, SessionState::STATE_END);
    ssm_->OnSessionStateChange(100, SessionState::STATE_FOREGROUND);
    ssm_->OnSessionStateChange(0, SessionState::STATE_FOREGROUND);
    sceneSession->focusedOnShow_ = false;
    ssm_->OnSessionStateChange(0, SessionState::STATE_FOREGROUND);
    ssm_->OnSessionStateChange(100, SessionState::STATE_BACKGROUND);
    ssm_->OnSessionStateChange(0, SessionState::STATE_BACKGROUND);

    sptr<WindowSessionProperty> property_ = new WindowSessionProperty();
    ASSERT_NE(nullptr, property_);
    property_->type_ = WindowType::APP_MAIN_WINDOW_END;
    sceneSession->property_ = property_;
    ssm_->OnSessionStateChange(100, SessionState::STATE_END);
    ssm_->OnSessionStateChange(100, SessionState::STATE_FOREGROUND);
    ssm_->OnSessionStateChange(0, SessionState::STATE_FOREGROUND);
    ssm_->OnSessionStateChange(100, SessionState::STATE_BACKGROUND);
    ssm_->OnSessionStateChange(0, SessionState::STATE_BACKGROUND);
    ASSERT_NE(nullptr, ssm_);
}

/**
 * @tc.name: RecoverAndReconnectSceneSession
 * @tc.desc: check func RecoverAndReconnectSceneSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLifecycleTest, RecoverAndReconnectSceneSession, Function | SmallTest | Level2)
{
    sptr<ISession> session;
    auto result = ssm_->RecoverAndReconnectSceneSession(nullptr, nullptr, nullptr, session, nullptr, nullptr);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    std::vector<int32_t> recoveredPersistentIds = { 0, 1, 2 };
    ssm_->SetAlivePersistentIds(recoveredPersistentIds);
    property->SetPersistentId(1);
    result = ssm_->RecoverAndReconnectSceneSession(nullptr, nullptr, nullptr, session, property, nullptr);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: RecoverAndReconnectSceneSession
 * @tc.desc: check func RecoverAndReconnectSceneSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLifecycleTest, RecoverAndReconnectSceneSession02, Function | SmallTest | Level2)
{
    sptr<ISession> session;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    std::vector<int32_t> recoveredPersistentIds = {0, 1, 2};
    ssm_->SetAlivePersistentIds(recoveredPersistentIds);
    property->SetPersistentId(1);
    ProcessShiftFocusFunc shiftFocusFunc_;
    auto result = ssm_->RecoverAndReconnectSceneSession(nullptr, nullptr, nullptr, session, property, nullptr);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    result = ssm_->RecoverAndReconnectSceneSession(nullptr, nullptr, nullptr, session, property, nullptr);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: CreateSceneSession
 * @tc.desc: CreateSceneSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, CreateSceneSession, Function | SmallTest | Level3)
{
    int ret = 0;
    SessionInfo info;
    ssm_->CreateSceneSession(info, nullptr);
    info.isSystem_ = true;
    info.windowType_ = 3000;
    ssm_->CreateSceneSession(info, nullptr);
    info.windowType_ = 3;
    ssm_->CreateSceneSession(info, nullptr);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    ssm_->CreateSceneSession(info, property);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    ssm_->CreateSceneSession(info, property);
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    ssm_->CreateSceneSession(info, property);
    property->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    ssm_->CreateSceneSession(info, property);
    ssm_->NotifySessionUpdate(info, ActionType::SINGLE_START, 0);
    info.persistentId_ = 0;
    ssm_->UpdateSceneSessionWant(info);
    info.persistentId_ = 1;
    ssm_->UpdateSceneSessionWant(info);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->UpdateSceneSessionWant(info);
    std::shared_ptr<AAFwk::Want> want = std::make_shared<AAFwk::Want>();
    ASSERT_NE(nullptr, want);
    info.want = want;
    ssm_->UpdateSceneSessionWant(info);
    sceneSession->SetCollaboratorType(0);
    ssm_->UpdateSceneSessionWant(info);
    sceneSession->SetCollaboratorType(1);
    ssm_->UpdateSceneSessionWant(info);
    ssm_->sceneSessionMap_.erase(1);
    sptr<SceneSession> sceneSession1;
    ssm_->UpdateCollaboratorSessionWant(sceneSession1, 1);
    ssm_->UpdateCollaboratorSessionWant(sceneSession, 1);
    SessionInfo info1;
    info1.ancoSceneState = 0;
    sceneSession1 = new (std::nothrow) SceneSession(info1, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    ssm_->UpdateCollaboratorSessionWant(sceneSession1, 1);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: RequestSceneSessionBackground
 * @tc.desc: RequestSceneSessionBackground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, RequestSceneSessionBackground, Function | SmallTest | Level3)
{
    int ret = 0;
    sptr<SceneSession> sceneSession;
    ssm_->RequestSceneSessionActivation(sceneSession, true);
    SessionInfo info;
    ret++;
    sptr<AAFwk::SessionInfo> abilitySessionInfo;
    ssm_->NotifyCollaboratorAfterStart(sceneSession, abilitySessionInfo);
    ssm_->RequestSceneSessionBackground(sceneSession, true, false);
    ssm_->RequestSceneSessionBackground(sceneSession, true, true);
    ssm_->RequestSceneSessionBackground(sceneSession, false, true);
    ssm_->RequestSceneSessionBackground(sceneSession, false, false);
    ssm_->NotifyForegroundInteractiveStatus(sceneSession, true);
    ssm_->NotifyForegroundInteractiveStatus(sceneSession, false);
    ssm_->DestroyDialogWithMainWindow(sceneSession);
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->RequestSceneSessionBackground(sceneSession, true, false);
    ssm_->RequestSceneSessionBackground(sceneSession, true, true);
    ssm_->RequestSceneSessionBackground(sceneSession, false, true);
    ssm_->RequestSceneSessionBackground(sceneSession, false, false);
    ssm_->NotifyForegroundInteractiveStatus(sceneSession, true);
    ssm_->NotifyForegroundInteractiveStatus(sceneSession, false);
    ssm_->NotifyCollaboratorAfterStart(sceneSession, abilitySessionInfo);
    ssm_->DestroyDialogWithMainWindow(sceneSession);
    ssm_->DestroySubSession(sceneSession);
    ssm_->SetAbilitySessionInfo(sceneSession);
    ssm_->RequestSceneSessionActivation(sceneSession, true);
    ssm_->RequestInputMethodCloseKeyboard(1);
    ssm_->sceneSessionMap_.insert({1, nullptr});
    ssm_->RequestInputMethodCloseKeyboard(1);
    ssm_->RequestSceneSessionActivation(sceneSession, true);
    ssm_->RequestSceneSessionActivation(sceneSession, false);
    ssm_->StartUIAbilityBySCB(sceneSession);
    ssm_->RequestSceneSessionActivation(sceneSession, true);
    abilitySessionInfo = new (std::nothrow) AAFwk::SessionInfo();
    ASSERT_NE(nullptr, abilitySessionInfo);
    ssm_->NotifyCollaboratorAfterStart(sceneSession, abilitySessionInfo);
    ssm_->DestroySubSession(sceneSession);
    ssm_->EraseSceneSessionMapById(2);
    ASSERT_EQ(ret, 1);
}

/**
 * @tc.name: RequestSceneSessionDestruction
 * @tc.desc: RequestSceneSessionDestruction
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, RequestSceneSessionDestruction, Function | SmallTest | Level3)
{
    int ret = 0;
    SessionInfo info;
    info.abilityName_ = "111";
    info.bundleName_ = "222";
    sptr<SceneSession> sceneSession;
    sptr<ISessionStage> sessionStage;
    ssm_->AddClientDeathRecipient(sessionStage, sceneSession);
    ssm_->RequestSceneSessionDestruction(sceneSession, true);
    ssm_->RequestSceneSessionDestruction(sceneSession, false);
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->AddClientDeathRecipient(sessionStage, sceneSession);
    ssm_->RequestSceneSessionDestruction(sceneSession, true);
    ssm_->RequestSceneSessionDestruction(sceneSession, false);
    ssm_->HandleCastScreenDisConnection(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});
    ssm_->RequestSceneSessionDestruction(sceneSession, true);
    ssm_->RequestSceneSessionDestruction(sceneSession, false);
    ssm_->sceneSessionMap_.erase(sceneSession->GetPersistentId());
    sptr<AAFwk::SessionInfo> scnSessionInfo = new (std::nothrow) AAFwk::SessionInfo();
    ASSERT_NE(nullptr, scnSessionInfo);
    ssm_->RequestSceneSessionDestructionInner(sceneSession, scnSessionInfo, true);
    ssm_->RequestSceneSessionDestructionInner(sceneSession, scnSessionInfo, false);
    std::shared_ptr<SessionListenerController> listenerController =
        std::make_shared<SessionListenerController>();
    ASSERT_NE(nullptr, listenerController);
    ssm_->listenerController_ = listenerController;
    ssm_->RequestSceneSessionDestructionInner(sceneSession, scnSessionInfo, true);
    ssm_->RequestSceneSessionDestructionInner(sceneSession, scnSessionInfo, false);
    ssm_->AddClientDeathRecipient(sessionStage, sceneSession);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: RequestSceneSessionBackground002
 * @tc.desc: SceneSesionManager request scene session background
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, RequestSceneSessionBackground002, Function | SmallTest | Level3)
{
    bool isDelegator = false;
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionBackground002";
    info.bundleName_ = "RequestSceneSessionBackground002";
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_EQ(WSError::WS_OK, ssm_->RequestSceneSessionBackground(sceneSession, isDelegator));
}

/**
 * @tc.name: RequestSceneSessionDestruction002
 * @tc.desc: SceneSesionManager request scene session destruction
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, RequestSceneSessionDestruction002, Function | SmallTest | Level3)
{
    bool needRemoveSession = false;
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionDestruction002";
    info.bundleName_ = "RequestSceneSessionDestruction002";
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_EQ(WSError::WS_OK, ssm_->RequestSceneSessionDestruction(sceneSession, needRemoveSession));
}

/**
 * @tc.name: NotifyForegroundInteractiveStatus
 * @tc.desc: SceneSesionManager notify scene session interactive status
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, NotifyForegroundInteractiveStatus, Function | SmallTest | Level3)
{
    bool interactive = false;
    SessionInfo info;
    info.abilityName_ = "NotifyForegroundInteractiveStatus";
    info.bundleName_ = "NotifyForegroundInteractiveStatus";
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    int ret = 0;
    ssm_->NotifyForegroundInteractiveStatus(sceneSession, interactive);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: NotifySessionCreate
 * @tc.desc: SceneSesionManager notify session create
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, NotifySessionCreate, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "NotifySessionCreate";
    info.bundleName_ = "NotifySessionCreate";
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->NotifySessionCreate(nullptr, info);
    EXPECT_EQ(info.want, nullptr);
    ssm_->NotifySessionCreate(sceneSession, info);
    info.want = std::make_shared<AAFwk::Want>();
    sceneSession = nullptr;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->NotifySessionCreate(sceneSession, info);
    EXPECT_NE(info.want, nullptr);
    AppExecFwk::AbilityInfo aInfo;
    sptr<AAFwk::SessionInfo> abilitySessionInfo = new AAFwk::SessionInfo();
    std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo =
        std::make_shared<AppExecFwk::AbilityInfo>(aInfo);
    ASSERT_NE(nullptr, abilityInfo);
    int32_t collaboratorType = CollaboratorType::RESERVE_TYPE;
    ssm_->NotifyLoadAbility(collaboratorType, abilitySessionInfo, abilityInfo);
}

/**
 * @tc.name: UpdateRecoveredSessionInfo
 * @tc.desc: SceneSessionManager load window scene xml
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLifecycleTest, UpdateRecoveredSessionInfo02, Function | SmallTest | Level3)
{
    std::vector<int32_t> recoveredPersistentIds;
    ASSERT_NE(ssm_, nullptr);
    ssm_->UpdateRecoveredSessionInfo(recoveredPersistentIds);
    recoveredPersistentIds.push_back(0);
    ssm_->UpdateRecoveredSessionInfo(recoveredPersistentIds);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test1";
    sptr<KeyboardSession::SpecificSessionCallback> specificCallback;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCallback);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({0, sceneSession});
    ssm_->UpdateRecoveredSessionInfo(recoveredPersistentIds);
    constexpr uint32_t WAIT_SYNC_IN_NS = 50000;
    usleep(WAIT_SYNC_IN_NS);
}
/**
 * @tc.name: RequestSceneSession01
 * @tc.desc: SceneSesionManager test RequestSceneSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, RequestSceneSession01, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "RequestSceneSession01";
    info.bundleName_ = "RequestSceneSession01";
    info.persistentId_ = 0;
    auto windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();

    sptr<SceneSession> sceneSession = ssm_->RequestSceneSession(info, windowSessionProperty);
    ASSERT_EQ(sceneSession, nullptr);
}

/**
 * @tc.name: RequestSceneSession02
 * @tc.desc: SceneSesionManager test RequestSceneSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, RequestSceneSession02, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "RequestSceneSession";
    info.bundleName_ = "RequestSceneSession";
    info.persistentId_ = 1;
    info.isPersistentRecover_ = false;
    auto windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();

    sptr<SceneSession> sceneSession = ssm_->RequestSceneSession(info, windowSessionProperty);
    ASSERT_EQ(sceneSession, nullptr);
}
/**
 * @tc.name: RequestSceneSession03
 * @tc.desc: SceneSesionManager test RequestSceneSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, RequestSceneSession03, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "RequestSceneSession";
    info.bundleName_ = "RequestSceneSession";
    info.persistentId_ = 1;
    info.isPersistentRecover_ = true;
    auto windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    auto sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});

    sptr<SceneSession> getSceneSession = ssm_->RequestSceneSession(info, windowSessionProperty);
    ASSERT_EQ(getSceneSession, nullptr);
}

/**
 * @tc.name: RequestSceneSession04
 * @tc.desc: SceneSesionManager test RequestSceneSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, RequestSceneSession04, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "RequestSceneSession";
    info.bundleName_ = "RequestSceneSession";
    info.persistentId_ = 1;
    info.isPersistentRecover_ = true;
    auto windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();

    sptr<SceneSession> sceneSession = ssm_->RequestSceneSession(info, windowSessionProperty);
    ASSERT_EQ(sceneSession, nullptr);
}

/**
 * @tc.name: RequestSceneSessionBackground01
 * @tc.desc: SceneSesionManager test RequestSceneSessionBackground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, RequestSceneSessionBackground01, Function | SmallTest | Level3)
{
    bool isDelegator = false;
    bool isToDesktop = false;
    bool isSaveSnapshot = true;
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionBackground";
    info.bundleName_ = "RequestSceneSessionBackground";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ASSERT_EQ(ssm_->RequestSceneSessionBackground(
        sceneSession, isDelegator, isToDesktop, isSaveSnapshot), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionBackground02
 * @tc.desc: SceneSesionManager test RequestSceneSessionBackground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, RequestSceneSessionBackground02, Function | SmallTest | Level3)
{
    bool isDelegator = false;
    bool isToDesktop = true;
    bool isSaveSnapshot = true;
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionBackground";
    info.bundleName_ = "RequestSceneSessionBackground";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_EQ(ssm_->RequestSceneSessionBackground(
        sceneSession, isDelegator, isToDesktop, isSaveSnapshot), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionBackground03
 * @tc.desc: SceneSesionManager test RequestSceneSessionBackground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, RequestSceneSessionBackground03, Function | SmallTest | Level3)
{
    bool isDelegator = false;
    bool isToDesktop = true;
    bool isSaveSnapshot = true;
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionBackground";
    info.bundleName_ = "RequestSceneSessionBackground";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->SetBrightness(sceneSession, 0.5);
    ASSERT_EQ(ssm_->RequestSceneSessionBackground(
        sceneSession, isDelegator, isToDesktop, isSaveSnapshot), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionBackground04
 * @tc.desc: SceneSesionManager test RequestSceneSessionBackground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, RequestSceneSessionBackground04, Function | SmallTest | Level3)
{
    bool isDelegator = true;
    bool isToDesktop = true;
    bool isSaveSnapshot = true;
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionBackground";
    info.bundleName_ = "RequestSceneSessionBackground";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_EQ(ssm_->RequestSceneSessionBackground(
        sceneSession, isDelegator, isToDesktop, isSaveSnapshot), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionDestruction01
 * @tc.desc: SceneSesionManager test RequestSceneSessionDestruction
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, RequestSceneSessionDestruction01, Function | SmallTest | Level3)
{
    bool needRemoveSession = false;
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionDestruction";
    info.bundleName_ = "RequestSceneSessionDestruction";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ASSERT_EQ(ssm_->RequestSceneSessionDestruction(
        sceneSession, needRemoveSession), WSError::WS_OK);
}
/**
 * @tc.name: RequestSceneSessionDestruction02
 * @tc.desc: SceneSesionManager test RequestSceneSessionDestruction
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, RequestSceneSessionDestruction02, Function | SmallTest | Level3)
{
    bool needRemoveSession = false;
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionDestruction";
    info.bundleName_ = "RequestSceneSessionDestruction";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_EQ(ssm_->RequestSceneSessionDestruction(
        sceneSession, needRemoveSession), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionByCall01
 * @tc.desc: SceneSesionManager test RequestSceneSessionByCall
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, RequestSceneSessionByCall01, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionByCall";
    info.bundleName_ = "RequestSceneSessionByCall";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_EQ(ssm_->RequestSceneSessionByCall(sceneSession), WSError::WS_OK);
}
/**
 * @tc.name: RequestSceneSessionByCall02
 * @tc.desc: SceneSesionManager test RequestSceneSessionByCall
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, RequestSceneSessionByCall02, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionByCall";
    info.bundleName_ = "RequestSceneSessionByCall";
    info.persistentId_ = 1;
    info.callState_ = static_cast<uint32_t>(AAFwk::CallToState::BACKGROUND);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_EQ(ssm_->RequestSceneSessionByCall(sceneSession), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionByCall03
 * @tc.desc: SceneSesionManager test RequestSceneSessionByCall
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, RequestSceneSessionByCall03, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionByCall";
    info.bundleName_ = "RequestSceneSessionByCall";
    info.persistentId_ = 1;
    info.callState_ = static_cast<uint32_t>(AAFwk::CallToState::FOREGROUND);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_EQ(ssm_->RequestSceneSessionByCall(sceneSession), WSError::WS_OK);
}

/**
 * @tc.name: removeFailRecoveredSession
 * @tc.desc: removeFailRecoveredSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, removeFailRecoveredSession, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession01 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession02 = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession01, nullptr);
    ASSERT_NE(sceneSession02, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession01));
    ssm_->sceneSessionMap_.insert(std::make_pair(2, nullptr));
    ssm_->sceneSessionMap_.insert(std::make_pair(3, sceneSession02));
    ssm_->failRecoveredPersistentIdSet_.insert(1);
    ssm_->failRecoveredPersistentIdSet_.insert(2);
    ssm_->failRecoveredPersistentIdSet_.insert(3);
    ssm_->removeFailRecoveredSession();
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, ssm_->HandleSecureSessionShouldHide(nullptr));
}

/**
 * @tc.name: ClearSession
 * @tc.desc: ClearSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerLifecycleTest, ClearSession, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, ssm_->ClearSession(nullptr));
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, ssm_->ClearSession(sceneSession));
}
}
} // namespace Rosen
} // namespace OHOS
