/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "common_test_utils.h"
#include "context.h"
#include "interfaces/include/ws_common.h"
#include "iremote_object_mocker.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_resource_manager.h"
#include "mock/mock_root_scene_context.h"
#include "mock/mock_window_event_channel.h"
#include "session_info.h"
#include "session_manager.h"
#include "session_manager/include/scene_session_manager.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/main_session.h"
#include "window_manager_agent.h"
#include "zidl/window_manager_agent_interface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SceneSessionManagerTest12 : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp() override;

    void TearDown() override;

    static sptr<SceneSessionManager> ssm_;
    std::shared_ptr<AbilityRuntime::RootSceneContextMocker> mockRootSceneContext_;
    std::string path;
    uint32_t bgColor;
    AppExecFwk::AbilityInfo abilityInfo;

private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

sptr<SceneSessionManager> SceneSessionManagerTest12::ssm_ = nullptr;

void SceneSessionManagerTest12::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerTest12::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerTest12::SetUp()
{
    mockRootSceneContext_ = std::make_shared<AbilityRuntime::RootSceneContextMocker>();
    path = "testPath";
    bgColor = 0;
    abilityInfo.bundleName = "testBundle";
    abilityInfo.moduleName = "testmodule";
    abilityInfo.resourcePath = "/test/resource/path";
    abilityInfo.startWindowBackgroundId = 1;
    abilityInfo.startWindowIconId = 1;
}

void SceneSessionManagerTest12::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
}

std::shared_ptr<Global::Resource::ResourceManagerMocker>
    mockResourceManager_ = std::make_shared<Global::Resource::ResourceManagerMocker>();

class SceneSessionManagerMocker : public SceneSessionManager {
public:
    SceneSessionManagerMocker() {};
    ~SceneSessionManagerMocker() {};

    std::shared_ptr<Global::Resource::ResourceManager> GetResourceManager(const AppExecFwk::AbilityInfo& abilityInfo)
    {
        return mockResourceManager_;
    };
};
std::shared_ptr<SceneSessionManagerMocker> mockSceneSessionManager_ = std::make_shared<SceneSessionManagerMocker>();

namespace {
/**
 * @tc.name: GetResourceManager01
 * @tc.desc: GetResourceManager context is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, GetResourceManager01, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    auto result = ssm_->GetResourceManager(abilityInfo);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetResourceManager02
 * @tc.desc: GetResourceManager resourceManager is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, GetResourceManager02, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ASSERT_NE(mockRootSceneContext_, nullptr);
    ssm_->rootSceneContextWeak_ = std::weak_ptr<AbilityRuntime::RootSceneContextMocker>(mockRootSceneContext_);
    EXPECT_CALL(*mockRootSceneContext_, GetResourceManager()).WillOnce(Return(nullptr));
    auto result = ssm_->GetResourceManager(abilityInfo);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetResourceManager03
 * @tc.desc: GetResourceManager
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, GetResourceManager03, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ASSERT_NE(mockRootSceneContext_, nullptr);
    ssm_->rootSceneContextWeak_ = std::weak_ptr<AbilityRuntime::RootSceneContextMocker>(mockRootSceneContext_);
    EXPECT_CALL(*mockRootSceneContext_, GetResourceManager()).WillOnce(Return(mockResourceManager_));
    auto result = ssm_->GetResourceManager(abilityInfo);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: GetStartupPageFromResource01
 * @tc.desc: GetStartupPageFromResource ResourceManager nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, GetStartupPageFromResource01, Function | SmallTest | Level3)
{
    ASSERT_NE(mockSceneSessionManager_, nullptr);
    mockResourceManager_ = nullptr;
    EXPECT_EQ(mockSceneSessionManager_->GetResourceManager(abilityInfo), nullptr);
    bool result = mockSceneSessionManager_->GetStartupPageFromResource(abilityInfo, path, bgColor);
    mockResourceManager_ = std::make_shared<Global::Resource::ResourceManagerMocker>();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: GetStartupPageFromResource02
 * @tc.desc: GetStartupPageFromResource ResourceManager GetColorById ERROR
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, GetStartupPageFromResource02, Function | SmallTest | Level3)
{
    ASSERT_NE(mockSceneSessionManager_, nullptr);
    ASSERT_NE(mockResourceManager_, nullptr);
    EXPECT_EQ(mockSceneSessionManager_->GetResourceManager(abilityInfo), mockResourceManager_);
    bool result = mockSceneSessionManager_->GetStartupPageFromResource(abilityInfo, path, bgColor);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: GetStartupPageFromResource03
 * @tc.desc: GetStartupPageFromResource ResourceManager GetMediaById ERROR
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, GetStartupPageFromResource03, Function | SmallTest | Level3)
{
    ASSERT_NE(mockSceneSessionManager_, nullptr);
    ASSERT_NE(mockResourceManager_, nullptr);
    EXPECT_EQ(mockSceneSessionManager_->GetResourceManager(abilityInfo), mockResourceManager_);
    EXPECT_CALL(*mockResourceManager_, GetColorById(abilityInfo.startWindowBackgroundId,
        bgColor)).WillOnce(Return(Global::Resource::RState::SUCCESS));
    bool result = mockSceneSessionManager_->GetStartupPageFromResource(abilityInfo, path, bgColor);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: GetStartupPageFromResource04
 * @tc.desc: GetStartupPageFromResource
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, GetStartupPageFromResource04, Function | SmallTest | Level3)
{
    ASSERT_NE(mockSceneSessionManager_, nullptr);
    ASSERT_NE(mockResourceManager_, nullptr);
    EXPECT_EQ(mockSceneSessionManager_->GetResourceManager(abilityInfo), mockResourceManager_);
    EXPECT_CALL(*mockResourceManager_, GetColorById(abilityInfo.startWindowBackgroundId,
        bgColor)).WillOnce(Return(Global::Resource::RState::SUCCESS));
    EXPECT_CALL(*mockResourceManager_, GetMediaById(abilityInfo.startWindowIconId, path,
        0)).WillOnce(Return(Global::Resource::RState::SUCCESS));
    bool result = mockSceneSessionManager_->GetStartupPageFromResource(abilityInfo, path, bgColor);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: RequestKeyboardPanelSession
 * @tc.desc: test RequestKeyboardPanelSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, RequestKeyboardPanelSession, Function | SmallTest | Level2)
{
    sptr<SceneSessionManager> ssm = sptr<SceneSessionManager>::MakeSptr();
    ASSERT_NE(nullptr, ssm);

    std::string panelName = "SystemKeyboardPanel";
    ASSERT_NE(nullptr, ssm->RequestKeyboardPanelSession(panelName, 0)); // 0 is screenId
    ssm->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ASSERT_NE(nullptr, ssm->RequestKeyboardPanelSession(panelName, 0)); // 0 is screenId
}

/**
 * @tc.name: CreateKeyboardPanelSession
 * @tc.desc: CreateKeyboardPanelSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, CreateKeyboardPanelSession02, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    info.screenId_ = SCREEN_ID_INVALID;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_KEYBOARD_PANEL);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ssm_->CreateKeyboardPanelSession(sceneSession);
}

/**
 * @tc.name: CreateKeyboardPanelSession03
 * @tc.desc: test CreateKeyboardPanelSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, CreateKeyboardPanelSession03, Function | SmallTest | Level2)
{
    SessionInfo keyboardInfo;
    keyboardInfo.abilityName_ = "CreateKeyboardPanelSession03";
    keyboardInfo.bundleName_ = "CreateKeyboardPanelSession03";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(keyboardInfo, nullptr, nullptr);
    ASSERT_EQ(nullptr, keyboardSession->GetKeyboardPanelSession());

    sptr<SceneSessionManager> ssm = sptr<SceneSessionManager>::MakeSptr();
    // the keyboard panel enabled flag of ssm is false
    ssm->CreateKeyboardPanelSession(keyboardSession);
    ASSERT_EQ(nullptr, keyboardSession->GetKeyboardPanelSession());

    // keyboard session is nullptr
    ssm->isKeyboardPanelEnabled_ = true;
    ssm->CreateKeyboardPanelSession(nullptr);
    ASSERT_EQ(nullptr, keyboardSession->GetKeyboardPanelSession());

    // the keyboard session is system keyboard
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    ASSERT_EQ(WSError::WS_OK, keyboardSession->SetSessionProperty(property));
    ASSERT_NE(nullptr, keyboardSession->GetSessionProperty());
    keyboardSession->SetIsSystemKeyboard(true);
    ASSERT_EQ(true, keyboardSession->IsSystemKeyboard());
    ssm->CreateKeyboardPanelSession(keyboardSession);
    ASSERT_NE(nullptr, keyboardSession->GetKeyboardPanelSession());
}

/**
 * @tc.name: CreateKeyboardPanelSession04
 * @tc.desc: test CreateKeyboardPanelSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, CreateKeyboardPanelSession04, Function | SmallTest | Level2)
{
    SessionInfo keyboardInfo;
    keyboardInfo.abilityName_ = "CreateKeyboardPanelSession04";
    keyboardInfo.bundleName_ = "CreateKeyboardPanelSession04";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(keyboardInfo, nullptr, nullptr);
    ASSERT_EQ(nullptr, keyboardSession->GetKeyboardPanelSession());

    sptr<SceneSessionManager> ssm = sptr<SceneSessionManager>::MakeSptr();
    // the keyboard panel enabled flag of ssm is true
    ssm->isKeyboardPanelEnabled_ = true;
    ASSERT_NE(nullptr, keyboardSession->GetSessionProperty());
    ASSERT_EQ(false, keyboardSession->IsSystemKeyboard());
    ssm->CreateKeyboardPanelSession(keyboardSession);
    ASSERT_NE(nullptr, keyboardSession->GetKeyboardPanelSession());

    // keyboard panel session is already exists
    ssm->CreateKeyboardPanelSession(keyboardSession);
    ASSERT_NE(nullptr, keyboardSession->GetKeyboardPanelSession());
}

/**
 * @tc.name: TestCheckSystemWindowPermission_01
 * @tc.desc: Test CheckSystemWindowPermission with windowType WINDOW_TYPE_UI_EXTENSION then false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, TestCheckSystemWindowPermission_01, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, ssm_);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, property);

    property->SetWindowType(WindowType::WINDOW_TYPE_UI_EXTENSION);
    ASSERT_EQ(false, ssm_->CheckSystemWindowPermission(property));
}

/**
 * @tc.name: TestCheckSystemWindowPermission_02
 * @tc.desc: Test CheckSystemWindowPermission with windowType WINDOW_TYPE_APP_MAIN_WINDOW then true
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, TestCheckSystemWindowPermission_02, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, ssm_);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();

    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW); // main window is not system window
    ASSERT_EQ(true, ssm_->CheckSystemWindowPermission(property));
}

/**
 * @tc.name: TestCheckSystemWindowPermission_03
 * @tc.desc: Test CheckSystemWindowPermission with windowType WINDOW_TYPE_INPUT_METHOD_FLOAT then true
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, TestCheckSystemWindowPermission_03, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, ssm_);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();

    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    ASSERT_EQ(true, ssm_->CheckSystemWindowPermission(property));
}

/**
 * @tc.name: TestCheckSystemWindowPermission_04
 * @tc.desc: Test CheckSystemWindowPermission with windowType WINDOW_TYPE_INPUT_METHOD_STATUS_BAR then true
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, TestCheckSystemWindowPermission_04, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, ssm_);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();

    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_STATUS_BAR);
    ASSERT_EQ(true, ssm_->CheckSystemWindowPermission(property));
}

/**
 * @tc.name: TestCheckSystemWindowPermission_05
 * @tc.desc: Test CheckSystemWindowPermission with windowType WINDOW_TYPE_INPUT_METHOD_FLOAT then false
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, TestCheckSystemWindowPermission_05, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, ssm_);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();

    property->SetIsSystemKeyboard(true);
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    ASSERT_EQ(true, ssm_->CheckSystemWindowPermission(property));
}

/**
 * @tc.name: TestCheckSystemWindowPermission_06
 * @tc.desc: Test CheckSystemWindowPermission with windowType WINDOW_TYPE_DIALOG then true
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, TestCheckSystemWindowPermission_06, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, ssm_);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();

    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ASSERT_EQ(true, ssm_->CheckSystemWindowPermission(property));
}

/**
 * @tc.name: TestCheckSystemWindowPermission_07
 * @tc.desc: Test CheckSystemWindowPermission with windowType WINDOW_TYPE_PIP then true
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, TestCheckSystemWindowPermission_07, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, ssm_);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();

    property->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    ASSERT_EQ(true, ssm_->CheckSystemWindowPermission(property));
}

/**
 * @tc.name: TestCheckSystemWindowPermission_08
 * @tc.desc: Test CheckSystemWindowPermission with windowType WINDOW_TYPE_FLOAT then true
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, TestCheckSystemWindowPermission_08, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, ssm_);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();

    property->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    ASSERT_EQ(false, ssm_->CheckSystemWindowPermission(property));
}

/**
 * @tc.name: TestCheckSystemWindowPermission_09
 * @tc.desc: Test CheckSystemWindowPermission with windowType WINDOW_TYPE_TOAST then true
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, TestCheckSystemWindowPermission_09, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, ssm_);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();

    property->SetWindowType(WindowType::WINDOW_TYPE_TOAST);
    ASSERT_EQ(true, ssm_->CheckSystemWindowPermission(property));
}

/**
 * @tc.name: CreateAndConnectSpecificSession
 * @tc.desc: CreateAndConnectSpecificSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, CreateAndConnectSpecificSession03, Function | SmallTest | Level3)
{
    sptr<ISessionStage> sessionStage;
    sptr<IWindowEventChannel> eventChannel;
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    sptr<ISession> session;
    SystemSessionConfig systemConfig;
    sptr<IRemoteObject> token;
    int32_t id = 0;
    ASSERT_NE(ssm_, nullptr);
    
    property->SetWindowType(WindowType::WINDOW_TYPE_UI_EXTENSION);
    auto res = ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    ASSERT_EQ(WSError::WS_ERROR_NOT_SYSTEM_APP, res);

    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    property->SetTopmost(true);
    uint32_t flags = property->GetWindowFlags() & (~(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_IS_MODAL)));
    property->SetWindowFlags(flags);
    res = ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    ASSERT_EQ(WSError::WS_ERROR_NOT_SYSTEM_APP, res);

    property->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    property->SetFloatingWindowAppType(true);
    ssm_->shouldHideNonSecureFloatingWindows_.store(true);
    res = ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    ASSERT_EQ(WSError::WS_ERROR_NOT_SYSTEM_APP, res);

    property->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW);
    res = ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_WINDOW, res);

    property->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    ssm_->isScreenLocked_ = true;
    res = ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    ASSERT_EQ(WSError::WS_DO_NOTHING, res);
}

/**
 * @tc.name: SetCreateKeyboardSessionListener
 * @tc.desc: SetCreateKeyboardSessionListener
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, SetCreateKeyboardSessionListener, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->SetCreateSystemSessionListener(nullptr);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "test1";
    sessionInfo.abilityName_ = "test2";
    sessionInfo.abilityInfo = nullptr;
    sessionInfo.isAtomicService_ = true;
    sessionInfo.screenId_ = SCREEN_ID_INVALID;
    ssm_->NotifySessionTouchOutside(123);
}

/**
 * @tc.name: DestroyAndDisconnectSpecificSessionInner
 * @tc.desc: check func DestroyAndDisconnectSpecificSessionInner
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, DestroyAndDisconnectSpecificSessionInner02, Function | SmallTest | Level2)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, property);
    std::vector<int32_t> recoveredPersistentIds = {0, 1, 2};
    ssm_->SetAlivePersistentIds(recoveredPersistentIds);
    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ssm_->DestroyAndDisconnectSpecificSessionInner(1);
}

/**
 * @tc.name: DestroyToastSession
 * @tc.desc: DestroyToastSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, DestroyToastSession, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    info.screenId_ = SCREEN_ID_INVALID;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_KEYBOARD_PANEL);
    sptr<SceneSession> sceneSession = nullptr;
    ssm_->DestroyToastSession(sceneSession);
    sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ssm_->DestroyToastSession(sceneSession);
    ssm_->StartUIAbilityBySCB(sceneSession);
    int32_t ret = ssm_->ChangeUIAbilityVisibilityBySCB(sceneSession, true);
    EXPECT_NE(ret, ERR_OK);
}

/**
 * @tc.name: DestroyToastSession
 * @tc.desc: DestroyToastSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, DestroyToastSession02, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    info.screenId_ = SCREEN_ID_INVALID;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_KEYBOARD_PANEL);
    sptr<SceneSession> sceneSession = nullptr;
    ssm_->DestroyToastSession(sceneSession);
    sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    ssm_->DestroyToastSession(sceneSession);
}

/**
 * @tc.name: CheckModalSubWindowPermission
 * @tc.desc: CheckModalSubWindowPermission
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, CheckModalSubWindowPermission, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    property->SetWindowFlags(123);
    ssm_->CheckModalSubWindowPermission(property);
    property->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ssm_->CheckModalSubWindowPermission(property);
}

/**
 * @tc.name: CheckModalSubWindowPermission
 * @tc.desc: CheckModalSubWindowPermission
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, CheckModalSubWindowPermission02, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    property->SetWindowFlags(123);
    property->SetTopmost(true);
    ssm_->CheckModalSubWindowPermission(property);
    property->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ssm_->CheckModalSubWindowPermission(property);
}

/**
 * @tc.name: DestroyAndDisconnectSpecificSessionInner
 * @tc.desc: check func DestroyAndDisconnectSpecificSessionInner
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, DestroyAndDisconnectSpecificSessionInner, Function | SmallTest | Level2)
{
    sptr<ISession> session;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, property);
    std::vector<int32_t> recoveredPersistentIds = {0, 1, 2};
    ssm_->SetAlivePersistentIds(recoveredPersistentIds);
    ProcessShiftFocusFunc shiftFocusFunc_;
    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ssm_->DestroyAndDisconnectSpecificSessionInner(1);
    property->SetPersistentId(1);
    ssm_->DestroyAndDisconnectSpecificSessionInner(1);

    property->SetWindowType(WindowType::WINDOW_TYPE_TOAST);
    ssm_->DestroyAndDisconnectSpecificSessionInner(1);
}

/**
 * @tc.name: DestroyAndDisconnectSpecificSessionWithDetachCallback
 * @tc.desc: SceneSesionManager destroy and disconnect specific session with detach callback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, DestroyAndDetachCallback, Function | SmallTest | Level3)
{
    int32_t persistentId = 0;
    ASSERT_NE(ssm_, nullptr);
    sptr<IRemoteObject> callback = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(callback, nullptr);
    ssm_->DestroyAndDisconnectSpecificSessionWithDetachCallback(persistentId, callback);
    sptr<WindowSessionProperty> property;
    ssm_->recoveringFinished_ = false;
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(info, property);
    ssm_->DestroyAndDisconnectSpecificSessionWithDetachCallback(persistentId, callback);
}

/**
 * @tc.name: IsKeyboardForeground
 * @tc.desc: IsKeyboardForeground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, IsKeyboardForeground, Function | SmallTest | Level3)
{
    SceneSessionManager* sceneSessionManager = sptr<SceneSessionManager>::MakeSptr();
    ASSERT_NE(sceneSessionManager, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    sceneSessionManager->IsKeyboardForeground();
    property->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW);
    sceneSessionManager->IsKeyboardForeground();
}

/**
 * @tc.name: RegisterWatchGestureConsumeResultCallback
 * @tc.desc: RegisterWatchGestureConsumeResultCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, RegisterWatchGestureConsumeResultCallback, Function | SmallTest | Level3)
{
    NotifyWatchGestureConsumeResultFunc func = [](int32_t keyCode, bool isConsumed) {};
    ssm_->RegisterWatchGestureConsumeResultCallback(std::move(func));
    ASSERT_NE(ssm_->onWatchGestureConsumeResultFunc_, nullptr);
}

/**
 * @tc.name: RegisterWatchFocusActiveChangeCallback
 * @tc.desc: RegisterWatchFocusActiveChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, RegisterWatchFocusActiveChangeCallback, Function | SmallTest | Level3)
{
    NotifyWatchFocusActiveChangeFunc func = [](bool isActive) {};
    ssm_->RegisterWatchFocusActiveChangeCallback(std::move(func));
    ASSERT_NE(ssm_->onWatchFocusActiveChangeFunc_, nullptr);
}

/**
 * @tc.name: NotifyWatchGestureConsumeResult
 * @tc.desc: NotifyWatchGestureConsumeResult
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, NotifyWatchGestureConsumeResult, Function | SmallTest | Level3)
{
    int32_t keyCode = 2049;
    bool isConsumed = true;
    ssm_->onWatchGestureConsumeResultFunc_ = [](int32_t keyCode, bool isConsumed) {};
    auto ret = ssm_->NotifyWatchGestureConsumeResult(keyCode, isConsumed);
    ASSERT_EQ(WMError::WM_OK, ret);
    ssm_->onWatchGestureConsumeResultFunc_ = nullptr;
    ret = ssm_->NotifyWatchGestureConsumeResult(keyCode, isConsumed);
    ASSERT_NE(ret, WMError::WM_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: NotifyWatchFocusActiveChange
 * @tc.desc: NotifyWatchFocusActiveChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, NotifyWatchFocusActiveChange, Function | SmallTest | Level3)
{
    bool isActive = true;
    ssm_->onWatchFocusActiveChangeFunc_ = [](bool isActive) {};
    auto ret = ssm_->NotifyWatchFocusActiveChange(isActive);
    ASSERT_EQ(WMError::WM_OK, ret);
    ssm_->onWatchFocusActiveChangeFunc_ = nullptr;
    ret = ssm_->NotifyWatchFocusActiveChange(isActive);
    ASSERT_NE(ret, WMError::WM_ERROR_INVALID_PARAM);
}
}
} // namespace Rosen
} // namespace OHOS
