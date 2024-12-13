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
#include "scene_board_judgement.h"
#include "screen_session_manager/include/screen_session_manager.h"
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
 * @tc.name: LoadFreeMultiWindowConfigWithTrue
 * @tc.desc: Test LoadFreeMultiWindowConfig for input true
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, LoadFreeMultiWindowConfigWithTrue, Function | SmallTest | Level3)
{
    ssm_->systemConfig_.freeMultiWindowEnable_ = false;
    ssm_->LoadFreeMultiWindowConfig(true);
    auto config = ssm_->GetSystemSessionConfig();
    ASSERT_EQ(config.freeMultiWindowEnable_, true);
}

/**
 * @tc.name: LoadFreeMultiWindowConfigWithFalse
 * @tc.desc: Test LoadFreeMultiWindowConfig for input false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, LoadFreeMultiWindowConfigWithFalse, Function | SmallTest | Level3)
{
    ssm_->systemConfig_.freeMultiWindowEnable_ = true;
    ssm_->LoadFreeMultiWindowConfig(false);
    auto config = ssm_->GetSystemSessionConfig();
    ASSERT_EQ(config.freeMultiWindowEnable_, false);
}

/**
 * @tc.name: SwitchFreeMultiWindowWithTrue
 * @tc.desc: Test LoadFreeMultiWindowConfig for input false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, SwitchFreeMultiWindowWithTrue, Function | SmallTest | Level3)
{
    ssm_->systemConfig_.freeMultiWindowSupport_ = false;
    auto res = ssm_->SwitchFreeMultiWindow(true);
    ASSERT_EQ(res, WSError::WS_ERROR_DEVICE_NOT_SUPPORT);
}

/**
 * @tc.name: SwitchFreeMultiWindowWithFalse
 * @tc.desc: Test LoadFreeMultiWindowConfig for input false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, SwitchFreeMultiWindowWithFalse, Function | SmallTest | Level3)
{
    ssm_->systemConfig_.freeMultiWindowSupport_ = true;

    auto res = ssm_->SwitchFreeMultiWindow(true);
    ASSERT_EQ(res, WSError::WS_OK);
    auto config = ssm_->GetSystemSessionConfig();
    ASSERT_EQ(config.freeMultiWindowEnable_, true);
}

/**
 * @tc.name: TestSwitchWindowWithNullSceneSession
 * @tc.desc: Test LoadFreeMultiWindowConfig with sceneSession is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, TestSwitchWindowWithNullSceneSession, Function | SmallTest | Level3)
{
    ssm_->systemConfig_.freeMultiWindowSupport_ = true;
    sptr<SceneSession> sceneSession;
    ssm_->sceneSessionMap_.insert({1, sceneSession});

    auto res = ssm_->SwitchFreeMultiWindow(false);
    ASSERT_EQ(res, WSError::WS_OK);
    auto config = ssm_->GetSystemSessionConfig();
    ASSERT_EQ(config.freeMultiWindowEnable_, false);
}

/**
 * @tc.name: TestSwitchWindowWithSceneSession
 * @tc.desc: Test LoadFreeMultiWindowConfig with sceneSession is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, TestSwitchWindowWithSceneSession, Function | SmallTest | Level3)
{
    ssm_->systemConfig_.freeMultiWindowSupport_ = true;
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});

    auto res = ssm_->SwitchFreeMultiWindow(false);
    ASSERT_EQ(res, WSError::WS_OK);
    auto config = ssm_->GetSystemSessionConfig();
    ASSERT_EQ(config.freeMultiWindowEnable_, false);
}

/**
 * @tc.name: TestSwitchWindowWithProperty
 * @tc.desc: Test LoadFreeMultiWindowConfig with property is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, TestSwitchWindowWithProperty, Function | SmallTest | Level3)
{
    ssm_->systemConfig_.freeMultiWindowSupport_ = true;
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW);
    sceneSession->SetSessionProperty(property);

    auto res = ssm_->SwitchFreeMultiWindow(false);
    ASSERT_EQ(res, WSError::WS_OK);
    auto config = ssm_->GetSystemSessionConfig();
    ASSERT_EQ(config.freeMultiWindowEnable_, false);
}

/**
 * @tc.name: TestSwitchWindowWithPropertyInputTrue
 * @tc.desc: Test LoadFreeMultiWindowConfig with property is not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, TestSwitchWindowWithPropertyInputTrue, Function | SmallTest | Level3)
{
    ssm_->systemConfig_.freeMultiWindowSupport_ = true;
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW);
    sceneSession->SetSessionProperty(property);

    auto res = ssm_->SwitchFreeMultiWindow(true);
    ASSERT_EQ(res, WSError::WS_OK);
    auto config = ssm_->GetSystemSessionConfig();
    ASSERT_EQ(config.freeMultiWindowEnable_, true);
}

/**
 * @tc.name: TestSetEnableInputEventWithInputTrue
 * @tc.desc: Test LoadFreeMultiWindowConfig for input true
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, TestSetEnableInputEventWithInputTrue, Function | SmallTest | Level3)
{
    ssm_->enableInputEvent_ = false;
    ssm_->SetEnableInputEvent(true);
    ASSERT_EQ(ssm_->enableInputEvent_, true);
    auto res = ssm_->IsInputEventEnabled();
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: RequestSceneSessionActivationInner
 * @tc.desc: SceneSessionManagerSupplementTest RequestSceneSessionActivationInner
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, RequestSceneSessionActivationInner,
    Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sceneSession->SetSessionProperty(property);
    sceneSession->SetCollaboratorType(0);
    auto ret = ssm_->RequestSceneSessionActivationInner(sceneSession, true);
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(ret, WSError::WS_OK);
    } else {
        ASSERT_EQ(ret, WSError::WS_ERROR_PRE_HANDLE_COLLABORATOR_FAILED);
    }
    ret = ssm_->RequestSceneSessionActivationInner(sceneSession, false);
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(ret, WSError::WS_OK);
    } else {
        ASSERT_EQ(ret, WSError::WS_ERROR_PRE_HANDLE_COLLABORATOR_FAILED);
    }
}

/**
 * @tc.name: RequestSceneSessionActivationInnerTest01
 * @tc.desc: SceneSessionManagerSupplementTest RequestSceneSessionActivationInner
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, RequestSceneSessionActivationInnerTest_01,
    Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";
    sessionInfo.ancoSceneState = AncoSceneState::NOTIFY_FOREGROUND;
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sceneSession->SetSessionProperty(property);
    sceneSession->SetScbCoreEnabled(true);
    auto ret = ssm_->RequestSceneSessionActivationInner(sceneSession, true);
    ASSERT_EQ(ret, WSError::WS_OK);
    ret = ssm_->RequestSceneSessionActivationInner(sceneSession, false);
    ASSERT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: NotifyCollaboratorAfterStart
 * @tc.desc: SceneSessionManagerSupplementTest NotifyCollaboratorAfterStart
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, NotifyCollaboratorAfterStart,
    Function | SmallTest | Level3)
{
    sptr<SceneSession> sceneSession;
    sptr<AAFwk::SessionInfo> sceneSessionInfo;
    ssm_->NotifyCollaboratorAfterStart(sceneSession, sceneSessionInfo);
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    auto ret = ssm_->RequestSceneSessionBackground(sceneSession, true, true);
    ASSERT_EQ(ret, WSError::WS_OK);
    sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->NotifyCollaboratorAfterStart(sceneSession, sceneSessionInfo);
    sceneSessionInfo = sptr<AAFwk::SessionInfo>::MakeSptr();
    ssm_->NotifyCollaboratorAfterStart(sceneSession, sceneSessionInfo);
    ssm_->NotifyCollaboratorAfterStart(sceneSession, sceneSessionInfo);
    ret = ssm_->RequestSceneSessionBackground(sceneSession, true, true);
    ASSERT_EQ(ret, WSError::WS_OK);
    ret = ssm_->RequestSceneSessionBackground(sceneSession, true, false);
    ASSERT_EQ(ret, WSError::WS_OK);
    ssm_->brightnessSessionId_ = sceneSession->GetPersistentId();
    ret = ssm_->RequestSceneSessionBackground(sceneSession, false, true);
    ASSERT_EQ(ret, WSError::WS_OK);
    ret = ssm_->RequestSceneSessionBackground(sceneSession, false, false);
    ASSERT_EQ(ret, WSError::WS_OK);
    ssm_->brightnessSessionId_ = 0;
    ssm_->systemConfig_.backgroundswitch  = true;
    ret = ssm_->RequestSceneSessionBackground(sceneSession, true, true);
    ASSERT_EQ(ret, WSError::WS_OK);
    ret = ssm_->RequestSceneSessionBackground(sceneSession, true, false);
    ASSERT_EQ(ret, WSError::WS_OK);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sceneSession->SetSessionProperty(property);
    ret = ssm_->RequestSceneSessionBackground(sceneSession, false, true);
    ASSERT_EQ(ret, WSError::WS_OK);
    ret = ssm_->RequestSceneSessionBackground(sceneSession, false, false);
    ASSERT_EQ(ret, WSError::WS_OK);
    ssm_->NotifyForegroundInteractiveStatus(sceneSession, true);
    ssm_->NotifyForegroundInteractiveStatus(sceneSession, false);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});
    ssm_->NotifyForegroundInteractiveStatus(sceneSession, true);
}

/**
 * @tc.name: DestroyDialogWithMainWindow
 * @tc.desc: SceneSessionManagerSupplementTest DestroyDialogWithMainWindow
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, DestroyDialogWithMainWindow,
    Function | SmallTest | Level3)
{
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    sptr<SceneSession> sceneSession;
    auto res = ssm_->DestroyDialogWithMainWindow(sceneSession);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
    ssm_->DestroySubSession(sceneSession);
    ssm_->RequestSceneSessionDestruction(sceneSession, true);
    ssm_->RequestSceneSessionDestruction(sceneSession, false);
    sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_COMPONENT);
    sceneSession->SetSessionProperty(property);
    res = ssm_->DestroyDialogWithMainWindow(sceneSession);
    ASSERT_EQ(res, WSError::WS_ERROR_INVALID_SESSION);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<SceneSession> sceneSession2;
    sceneSession->dialogVec_.push_back(sceneSession2);
    sceneSession->subSession_.push_back(sceneSession2);
    ssm_->DestroyDialogWithMainWindow(sceneSession);
    ssm_->DestroySubSession(sceneSession);
    sceneSession2 = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession2, nullptr);
    res = ssm_->DestroyDialogWithMainWindow(sceneSession);
    ASSERT_EQ(res, WSError::WS_OK);
    ssm_->DestroySubSession(sceneSession);
    ssm_->RequestSceneSessionDestruction(sceneSession, true);
    ssm_->RequestSceneSessionDestruction(sceneSession, false);
    ssm_->sceneSessionMap_.insert({sceneSession2->GetPersistentId(), sceneSession2});
    res = ssm_->DestroyDialogWithMainWindow(sceneSession);
    ASSERT_EQ(res, WSError::WS_OK);
    ssm_->EraseSceneSessionMapById(sceneSession2->GetPersistentId());
    ssm_->RequestSceneSessionDestruction(sceneSession2, true);
    ssm_->RequestSceneSessionDestruction(sceneSession2, false);
}

/**
 * @tc.name: DestroyDialogWithSessionIsNull
 * @tc.desc: DestroyDialogWithMainWindow with with session is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, DestroyDialogWithSessionIsNull, Function | SmallTest | Level3)
{
    sptr<SceneSession> sceneSession;
    auto res = ssm_->DestroyDialogWithMainWindow(sceneSession);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: DestroyDialogWithFalseType
 * @tc.desc: DestroyDialogWithMainWindow with type is WINDOW_TYPE_APP_COMPONENT
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, DestroyDialogWithFalseType, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_COMPONENT);
    sceneSession->SetSessionProperty(property);
    auto res = ssm_->DestroyDialogWithMainWindow(sceneSession);
    ASSERT_EQ(res, WSError::WS_ERROR_INVALID_SESSION);
}

/**
 * @tc.name: DestroyDialogWithTrueType
 * @tc.desc: DestroyDialogWithMainWindow with type is WINDOW_TYPE_APP_MAIN_WINDOW
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, DestroyDialogWithTrueType, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sceneSession->SetSessionProperty(property);
    sptr<SceneSession> sceneSession2;
    sceneSession->dialogVec_.push_back(sceneSession2);
    sceneSession->subSession_.push_back(sceneSession2);
    auto res = ssm_->DestroyDialogWithMainWindow(sceneSession);
    ASSERT_EQ(res, WSError::WS_OK);
}

/**
 * @tc.name: DestroyDialogWithTrueType_01
 * @tc.desc: DestroyDialogWithMainWindow with type is WINDOW_TYPE_APP_MAIN_WINDOW
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, DestroyDialogWithTrueType_01, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sceneSession->SetSessionProperty(property);
    sptr<SceneSession> sceneSession2;
    sceneSession->dialogVec_.push_back(sceneSession2);
    sceneSession->subSession_.push_back(sceneSession2);
    sceneSession2 = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession2, nullptr);
    ssm_->sceneSessionMap_.insert({sceneSession2->GetPersistentId(), sceneSession2});
    auto res = ssm_->DestroyDialogWithMainWindow(sceneSession);
    ASSERT_EQ(res, WSError::WS_OK);
}

/**
 * @tc.name: TestCreateAndConnectSession_01
 * @tc.desc: Test for CreateAndConnectSpecificSession with property is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, TestCreateAndConnectSession_01, Function | SmallTest | Level3)
{
    sptr<ISessionStage> sessionStage;
    sptr<IWindowEventChannel> eventChannel;
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    sptr<WindowSessionProperty> property;
    sptr<ISession> session;
    SystemSessionConfig systemConfig;
    sptr<IRemoteObject> token;
    int32_t id = 0;

    auto res = ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: TestCreateAndConnectSession_02
 * @tc.desc: Test for CreateAndConnectSpecificSession with WindowType is WINDOW_TYPE_UI_EXTENSION
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, TestCreateAndConnectSession_02, Function | SmallTest | Level3)
{
    sptr<ISessionStage> sessionStage;
    sptr<IWindowEventChannel> eventChannel;
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    sptr<ISession> session;
    SystemSessionConfig systemConfig;
    sptr<IRemoteObject> token;
    int32_t id = 0;
    property->SetWindowType(WindowType::WINDOW_TYPE_UI_EXTENSION);

    auto res = ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    ASSERT_EQ(res, WSError::WS_ERROR_NOT_SYSTEM_APP);
}

/**
 * @tc.name: TestCreateAndConnectSession_03
 * @tc.desc: Test for CreateAndConnectSpecificSession with WindowType is WINDOW_TYPE_INPUT_METHOD_FLOAT
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, TestCreateAndConnectSession_03, Function | SmallTest | Level3)
{
    sptr<ISessionStage> sessionStage;
    sptr<IWindowEventChannel> eventChannel;
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    sptr<ISession> session;
    SystemSessionConfig systemConfig;
    sptr<IRemoteObject> token;
    int32_t id = 0;
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);

    auto res = ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: TestCreateAndConnectSession_04
 * @tc.desc: Test for CreateAndConnectSpecificSession with WindowType is WINDOW_TYPE_FLOAT
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, TestCreateAndConnectSession_04, Function | SmallTest | Level3)
{
    sptr<ISessionStage> sessionStage;
    sptr<IWindowEventChannel> eventChannel;
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    sptr<ISession> session;
    SystemSessionConfig systemConfig;
    sptr<IRemoteObject> token;
    int32_t id = 0;
    property->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    property->SetFloatingWindowAppType(true);

    auto res = ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: TestCreateAndConnectSession_05
 * @tc.desc: Test for CreateAndConnectSpecificSession with WindowType is WINDOW_TYPE_APP_SUB_WINDOW
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, TestCreateAndConnectSession_05, Function | SmallTest | Level3)
{
    sptr<ISessionStage> sessionStage;
    sptr<IWindowEventChannel> eventChannel;
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    sptr<ISession> session;
    SystemSessionConfig systemConfig;
    sptr<IRemoteObject> token;
    int32_t id = 0;
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    property->SetFloatingWindowAppType(true);
    property->SetIsUIExtFirstSubWindow(true);

    auto res = ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: TestCreateAndConnectSession_06
 * @tc.desc: Test for CreateAndConnectSpecificSession with WindowType is WINDOW_TYPE_SYSTEM_ALARM_WINDOW
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, TestCreateAndConnectSession_06, Function | SmallTest | Level3)
{
    sptr<ISessionStage> sessionStage;
    sptr<IWindowEventChannel> eventChannel;
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    sptr<ISession> session;
    SystemSessionConfig systemConfig;
    sptr<IRemoteObject> token;
    int32_t id = 0;
    property->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW);
    property->SetFloatingWindowAppType(true);
    property->SetIsUIExtFirstSubWindow(true);

    auto res = ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    ASSERT_EQ(res, WSError::WS_ERROR_INVALID_WINDOW);
}

/**
 * @tc.name: TestCreateAndConnectSession_07
 * @tc.desc: Test for CreateAndConnectSpecificSession with WindowType is WINDOW_TYPE_PIP
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, TestCreateAndConnectSession_07, Function | SmallTest | Level3)
{
    sptr<ISessionStage> sessionStage;
    sptr<IWindowEventChannel> eventChannel;
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    sptr<ISession> session;
    SystemSessionConfig systemConfig;
    sptr<IRemoteObject> token;
    int32_t id = 0;
    property->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    property->SetFloatingWindowAppType(true);
    property->SetIsUIExtFirstSubWindow(true);

    auto res = ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    ASSERT_EQ(res, WSError::WS_DO_NOTHING);
}

/**
 * @tc.name: ClosePipWindowIfExist
 * @tc.desc: ClosePipWindowIfExist
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, ClosePipWindowIfExist, Function | SmallTest | Level3)
{
    ssm_->ClosePipWindowIfExist(WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW);
    ssm_->ClosePipWindowIfExist(WindowType::WINDOW_TYPE_PIP);
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_COMPONENT);
    sceneSession->SetSessionProperty(property);
    PiPTemplateInfo pipInfo;
    pipInfo.priority = 0;
    auto res = ssm_->CheckPiPPriority(pipInfo);
    ASSERT_EQ(res, true);
    ssm_->sceneSessionMap_.insert({0, sceneSession});
    ssm_->ClosePipWindowIfExist(WindowType::WINDOW_TYPE_PIP);
    res = ssm_->CheckPiPPriority(pipInfo);
    ASSERT_EQ(res, true);
    property->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    PiPTemplateInfo pipInfo1;
    pipInfo1.priority = 0;
    sceneSession->SetPiPTemplateInfo(pipInfo1);
    res = ssm_->CheckPiPPriority(pipInfo);
    ASSERT_EQ(res, true);
    sceneSession->isVisible_ = true;
    res = ssm_->CheckPiPPriority(pipInfo);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: CheckSysWinPermissionWithUIExtTypeThenFalse
 * @tc.desc: Check system window permission while type is WINDOW_TYPE_UI_EXTENSION return false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, CheckSysWinPermissionWithUIExtTypeThenFalse, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_UI_EXTENSION);

    ASSERT_NE(ssm_, nullptr);
    bool res = ssm_->CheckSystemWindowPermission(property);
    ASSERT_EQ(res, false);
}

/**
 * @tc.name: CheckSysWinPermissionWithInputTypeThenFalse
 * @tc.desc: Check system window permission while type is WINDOW_TYPE_INPUT_METHOD_FLOAT return false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, CheckSysWinPermissionWithInputTypeThenFalse, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);

    ASSERT_NE(ssm_, nullptr);
    bool res = ssm_->CheckSystemWindowPermission(property);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: CheckSysWinPermWithInputStatusTypeThenFalse
 * @tc.desc: Check system window permission while type is WINDOW_TYPE_INPUT_METHOD_STATUS_BAR return false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, CheckSysWinPermWithInputStatusTypeThenFalse, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_STATUS_BAR);

    ASSERT_NE(ssm_, nullptr);
    bool res = ssm_->CheckSystemWindowPermission(property);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: CheckSysWinPermWithDraggingTypeThenTrue
 * @tc.desc: Check system window permission while type is WINDOW_TYPE_DRAGGING_EFFECT return false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, CheckSysWinPermWithDraggingTypeThenTrue, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_DRAGGING_EFFECT);

    ASSERT_NE(ssm_, nullptr);
    bool res = ssm_->CheckSystemWindowPermission(property);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: CheckSysWinPermWithToastTypeThenTrue
 * @tc.desc: Check system window permission while type is WINDOW_TYPE_TOAST return false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, CheckSysWinPermWithToastTypeThenTrue, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_TOAST);

    ASSERT_NE(ssm_, nullptr);
    bool res = ssm_->CheckSystemWindowPermission(property);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: CheckSysWinPermWithDialgTypeThenTrue
 * @tc.desc: Check system window permission while type is WINDOW_TYPE_DIALOG return true
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, CheckSysWinPermWithDialgTypeThenTrue, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);

    ASSERT_NE(ssm_, nullptr);
    bool res = ssm_->CheckSystemWindowPermission(property);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: CheckSysWinPermWithPipTypeThenTrue
 * @tc.desc: Check system window permission while type is WINDOW_TYPE_DIALOG return true
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, CheckSysWinPermWithPipTypeThenTrue, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_PIP);

    ASSERT_NE(ssm_, nullptr);
    bool res = ssm_->CheckSystemWindowPermission(property);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: CheckSysWinPermWithFloatTypeThenFalse
 * @tc.desc: Check system window permission while type is WINDOW_TYPE_FLOAT return false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, CheckSysWinPermWithFloatTypeThenFalse, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);

    ASSERT_NE(ssm_, nullptr);
    bool res = ssm_->CheckSystemWindowPermission(property);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: SetAlivePersistentIdsWithNoId
 * @tc.desc: Set Alive Persistent Ids With no persistentId then IsNeedRecover check return false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, SetAlivePersistentIdsWithNoId, Function | SmallTest | Level1)
{
    std::vector<int32_t> alivePersistentIds;
    ASSERT_NE(ssm_, nullptr);
    ssm_->SetAlivePersistentIds(alivePersistentIds);

    int persistentId = 1;
    bool res = ssm_->IsNeedRecover(persistentId);
    ASSERT_EQ(res, false);
}

/**
 * @tc.name: SetAlivePersistentIdsWithIds
 * @tc.desc: Set Alive Persistent Ids With persistentIds then IsNeedRecover check return true
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, SetAlivePersistentIdsWithIds, Function | SmallTest | Level1)
{
    std::vector<int32_t> alivePersistentIds = {0, 1, 2};
    ASSERT_NE(ssm_, nullptr);
    ssm_->SetAlivePersistentIds(alivePersistentIds);
    ASSERT_EQ(ssm_->alivePersistentIds_, alivePersistentIds);

    bool res = ssm_->IsNeedRecover(0);
    ASSERT_EQ(res, true);
    res = ssm_->IsNeedRecover(1);
    ASSERT_EQ(res, true);
    res = ssm_->IsNeedRecover(2);
    ASSERT_EQ(res, true);
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
    property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    ret = ssm_->RecoverAndConnectSpecificSession(sessionStage, eventChannel, node, property, session, token);
    ASSERT_EQ(ret, WSError::WS_ERROR_NULLPTR);
    ssm_->NotifyRecoveringFinished();
    usleep(WAIT_SYNC_IN_NS);
    ASSERT_EQ(ssm_->recoveringFinished_, true);
}

/**
 * @tc.name: CacheSpecificSessionForRecovering
 * @tc.desc: CacheSpecificSessionForRecovering
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, CacheSpecificSessionForRecovering, Function | SmallTest | Level3)
{
    sptr<SceneSession> sceneSession;
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    sptr<WindowSessionProperty> property;
    ssm_->recoveringFinished_ = false;
    ssm_->CacheSpecificSessionForRecovering(sceneSession, property);
    ASSERT_EQ(ssm_->recoverSubSessionCacheMap_.size(), 0);
    sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->CacheSpecificSessionForRecovering(sceneSession, property);
    ASSERT_EQ(ssm_->recoverSubSessionCacheMap_.size(), 0);
    property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    ssm_->CacheSpecificSessionForRecovering(sceneSession, property);
    ASSERT_EQ(ssm_->recoverSubSessionCacheMap_.size(), 0);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ssm_->CacheSpecificSessionForRecovering(sceneSession, property);
    ASSERT_EQ(ssm_->recoverSubSessionCacheMap_.size(), 0);
    property->SetWindowType(WindowType::APP_SUB_WINDOW_END);
    ssm_->CacheSpecificSessionForRecovering(sceneSession, property);
    ASSERT_EQ(ssm_->recoverSubSessionCacheMap_.size(), 0);
    property->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    property->SetParentPersistentId(1);
    NotifyCreateSubSessionFunc func;
    ssm_->createSubSessionFuncMap_.insert({1, func});
    ssm_->CacheSpecificSessionForRecovering(sceneSession, property);
    ASSERT_EQ(ssm_->recoverSubSessionCacheMap_.size(), 0);
    ssm_->createSubSessionFuncMap_.clear();
    ssm_->CacheSpecificSessionForRecovering(sceneSession, property);
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
    sptr<ISessionStage> sessionStage;
    sptr<IWindowEventChannel> eventChannel;
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    sptr<WindowSessionProperty> property;
    sptr<ISession> session;
    sptr<IRemoteObject> token;
    auto ret = ssm_->RecoverAndReconnectSceneSession(sessionStage, eventChannel, node, session, property, token);
    ASSERT_EQ(ret, WSError::WS_ERROR_NULLPTR);
    property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetPersistentId(1);
    ssm_->alivePersistentIds_.push_back(1);
    ssm_->recoveringFinished_ = true;
    ret = ssm_->RecoverAndReconnectSceneSession(sessionStage, eventChannel, node, session, property, token);
    ASSERT_EQ(ret, WSError::WS_ERROR_INVALID_OPERATION);
    ssm_->recoveringFinished_ = false;
    ret = ssm_->RecoverAndReconnectSceneSession(sessionStage, eventChannel, node, session, property, token);
    ASSERT_EQ(ret, WSError::WS_ERROR_NULLPTR);
    property->SetWindowType(WindowType::APP_SUB_WINDOW_END);
    ret = ssm_->RecoverAndReconnectSceneSession(sessionStage, eventChannel, node, session, property, token);
    ASSERT_EQ(ret, WSError::WS_ERROR_NULLPTR);
    NotifyRecoverSceneSessionFunc func;
    ssm_->SetRecoverSceneSessionListener(func);
    ssm_->alivePersistentIds_.clear();
    NotifyCreateSubSessionFunc func2;
    ssm_->RegisterCreateSubSessionListener(1, func2);
    ssm_->createSubSessionFuncMap_.insert({ 1, func2 });
    ssm_->RegisterCreateSubSessionListener(1, func2);
    ssm_->createSubSessionFuncMap_.clear();
}

/**
 * @tc.name: NotifyCreateSpecificSession
 * @tc.desc: NotifyCreateSpecificSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, NotifyCreateSpecificSession, Function | SmallTest | Level3)
{
    sptr<SceneSession> sceneSession;
    sptr<WindowSessionProperty> property;
    ASSERT_NE(ssm_, nullptr);
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::APP_MAIN_WINDOW_BASE);
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::APP_MAIN_WINDOW_BASE);
    property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::APP_MAIN_WINDOW_BASE);
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::SYSTEM_SUB_WINDOW_BASE);
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::APP_SUB_WINDOW_BASE);
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::WINDOW_TYPE_SCENE_BOARD);
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::WINDOW_TYPE_TOAST);
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::WINDOW_TYPE_FLOAT);
    property->SetParentPersistentId(1);
    ASSERT_EQ(property->GetParentPersistentId(), 1);
    SessionInfo info1;
    info1.bundleName_ = "test3";
    info1.abilityName_ = "test3";
    sptr<SceneSession> sceneSession2 = sptr<SceneSession>::MakeSptr(info1, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession2});
    ASSERT_TRUE(ssm_->sceneSessionMap_.find(1) != ssm_->sceneSessionMap_.end());
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::WINDOW_TYPE_FLOAT);
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::WINDOW_TYPE_DIALOG);
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    ssm_->NotifyCreateSpecificSession(sceneSession, property, WindowType::BELOW_APP_SYSTEM_WINDOW_BASE);
}

/**
 * @tc.name: NotifyCreateSubSession
 * @tc.desc: NotifyCreateSubSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, NotifyCreateSubSession, Function | SmallTest | Level3)
{
    sptr<SceneSession> sceneSession;
    ssm_->NotifyCreateSubSession(1, sceneSession);
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->NotifyCreateSubSession(1, sceneSession);
    ssm_->UnregisterSpecificSessionCreateListener(1);
    ASSERT_TRUE(ssm_->createSubSessionFuncMap_.find(1) == ssm_->createSubSessionFuncMap_.end());
    SessionInfo info1;
    info1.bundleName_ = "test1";
    info1.abilityName_ = "test2";
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(info1, nullptr);
    ASSERT_NE(sceneSession1, nullptr);
    NotifyCreateSubSessionFunc func;
    ssm_->createSubSessionFuncMap_.insert({1, func});
    ssm_->NotifyCreateSubSession(1, sceneSession);
    ssm_->UnregisterSpecificSessionCreateListener(1);
    ssm_->createSubSessionFuncMap_.clear();
    ASSERT_TRUE(ssm_->createSubSessionFuncMap_.find(1) == ssm_->createSubSessionFuncMap_.end());
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    sceneSession->SetSessionProperty(property);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_TRUE(ssm_->sceneSessionMap_.find(1) != ssm_->sceneSessionMap_.end());
    ssm_->NotifySessionTouchOutside(1);
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    ssm_->NotifySessionTouchOutside(1);
    sceneSession->persistentId_ = property->callingSessionId_;
    ssm_->NotifySessionTouchOutside(1);
    sceneSession->persistentId_ = property->callingSessionId_ + 1;
    ssm_->NotifySessionTouchOutside(1);
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
    ssm_->sceneSessionMap_.clear();
    auto ret = ssm_->DestroyAndDisconnectSpecificSessionInner(1);
    ASSERT_EQ(ret, WSError::WS_ERROR_NULLPTR);
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    uint32_t uid = 0;
    ssm_->GetTopWindowId(1, uid);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->GetTopWindowId(1, uid);
    ret = ssm_->DestroyAndDisconnectSpecificSessionInner(1);
    ASSERT_EQ(ret, WSError::WS_OK);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    property->SetParentPersistentId(100);
    ASSERT_EQ(property->GetParentPersistentId(), 100);
    sceneSession->SetSessionProperty(property);
    ret = ssm_->DestroyAndDisconnectSpecificSessionInner(1);
    ASSERT_EQ(ret, WSError::WS_ERROR_NULLPTR);
    property->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ret = ssm_->DestroyAndDisconnectSpecificSessionInner(1);
    ASSERT_EQ(ret, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: GetFocusWindowInfo
 * @tc.desc: GetFocusWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, GetFocusWindowInfo, Function | SmallTest | Level3)
{
    sptr<WindowSessionProperty> property;
    sptr<SceneSession> sceneSession;
    ssm_->NotifySessionForCallback(sceneSession, true);
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    info.isSystem_ = true;
    sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->NotifySessionForCallback(sceneSession, true);
    info.isSystem_ = false;
    ssm_->NotifySessionForCallback(sceneSession, true);
    property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetBrightness(1.f);
    auto ret = ssm_->SetBrightness(sceneSession, 1.f);
    ASSERT_EQ(ret, WSError::WS_OK);
    ret = ssm_->SetBrightness(sceneSession, 2.f);
    ASSERT_EQ(ret, WSError::WS_OK);
    ssm_->displayBrightness_ = 2.f;
    ret = ssm_->SetBrightness(sceneSession, 2.f);
    ASSERT_EQ(ret, WSError::WS_OK);
    ssm_->displayBrightness_ = 3.f;
    ret = ssm_->SetBrightness(sceneSession, 2.f);
    ASSERT_EQ(ret, WSError::WS_OK);
    ret = ssm_->UpdateBrightness(1);
    ASSERT_EQ(ret, WSError::WS_ERROR_NULLPTR);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    property->SetBrightness(-1.f);
    property->SetWindowType(WindowType::APP_WINDOW_BASE);
    sceneSession->SetSessionProperty(property);
    ret = ssm_->UpdateBrightness(1);
    ASSERT_EQ(ret, WSError::WS_OK);
    property->SetBrightness(3.f);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sceneSession->SetSessionProperty(property);
    ret = ssm_->UpdateBrightness(1);
    ASSERT_EQ(ret, WSError::WS_OK);
    FocusChangeInfo changeInfo;
    ssm_->GetFocusWindowInfo(changeInfo);
}

/**
 * @tc.name: IsSessionVisible
 * @tc.desc: IsSessionVisible
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, IsSessionVisible, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.bundleName_ = "test1";
    sptr<SceneSession> sceneSession;
    bool ret = ssm_->IsSessionVisible(sceneSession);
    ASSERT_EQ(ret, false);
    sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ssm_->IsSessionVisible(sceneSession);
    property->SetParentPersistentId(1);
    sceneSession->SetSessionProperty(property);
    SessionInfo info1;
    info1.bundleName_ = "test3";
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(info1, nullptr);
    ASSERT_NE(sceneSession1, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession1});
    ret = ssm_->IsSessionVisible(sceneSession);
    EXPECT_EQ(ret, false);
    sceneSession->isVisible_ = true;
    ret = ssm_->IsSessionVisible(sceneSession);
    EXPECT_EQ(ret, true);
    sceneSession->isVisible_ = false;
    sceneSession->state_ = SessionState::STATE_ACTIVE;
    ret = ssm_->IsSessionVisible(sceneSession);
    EXPECT_EQ(ret, false);
    sceneSession1->state_ = SessionState::STATE_ACTIVE;
    ret = ssm_->IsSessionVisible(sceneSession);
    EXPECT_EQ(ret, false);
    sceneSession->state_ = SessionState::STATE_INACTIVE;
    ret = ssm_->IsSessionVisible(sceneSession);
    EXPECT_EQ(ret, false);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ret = ssm_->IsSessionVisible(sceneSession);
    EXPECT_EQ(ret, false);
    sceneSession->isVisible_ = true;
    ret = ssm_->IsSessionVisible(sceneSession);
    EXPECT_EQ(ret, true);
    sceneSession->isVisible_ = false;
    sceneSession->state_ = SessionState::STATE_ACTIVE;
    ret = ssm_->IsSessionVisible(sceneSession);
    EXPECT_EQ(ret, false);
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    ret = ssm_->IsSessionVisible(sceneSession);
    EXPECT_EQ(ret, false);
    sceneSession->state_ = SessionState::STATE_INACTIVE;
    ret = ssm_->IsSessionVisible(sceneSession);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: RegisterBindDialogTargetListener
 * @tc.desc: RegisterBindDialogTargetListener
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerSupplementTest, RegisterBindDialogTargetListener, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.bundleName_ = "test1";
    info.abilityName_ = "test2";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    int32_t persistentId = 1;
    ssm_->UnregisterSpecificSessionCreateListener(persistentId);
    NotifyBindDialogSessionFunc func1;
    ssm_->RegisterBindDialogTargetListener(sceneSession, std::move(func1));
    ssm_->UnregisterSpecificSessionCreateListener(persistentId);

    NotifyBindDialogSessionFunc func2;
    ssm_->bindDialogTargetFuncMap_.insert({ persistentId, func2 });
    ASSERT_NE(ssm_->bindDialogTargetFuncMap_.find(persistentId), ssm_->bindDialogTargetFuncMap_.end());
    ssm_->bindDialogTargetFuncMap_.erase(persistentId);
    ASSERT_EQ(ssm_->bindDialogTargetFuncMap_.find(persistentId), ssm_->bindDialogTargetFuncMap_.end());
    ssm_->bindDialogTargetFuncMap_.insert({ persistentId, func2 });
    ASSERT_NE(ssm_->bindDialogTargetFuncMap_.find(persistentId), ssm_->bindDialogTargetFuncMap_.end());
    ssm_->bindDialogTargetFuncMap_.clear();
    ASSERT_EQ(ssm_->bindDialogTargetFuncMap_.find(persistentId), ssm_->bindDialogTargetFuncMap_.end());
}
}
} // namespace Rosen
} // namespace OHOS