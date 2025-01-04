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
    keyboardSession->property_->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
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

/**
 * @tc.name: ShiftAppWindowPointerEvent01
 * @tc.desc: ShiftAppWindowPointerEvent,
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, ShiftAppWindowPointerEvent_01, Function | SmallTest | Level3)
{
    SessionInfo sourceInfo;
    sourceInfo.windowType_ = 1;
    sptr<SceneSession> sourceSceneSession = sptr<SceneSession>::MakeSptr(sourceInfo, nullptr);
    ssm_->sceneSessionMap_.insert({sourceSceneSession->GetPersistentId(), sourceSceneSession});

    SessionInfo targetInfo;
    targetInfo.windowType_ = 1;
    sptr<SceneSession> targetSceneSession = sptr<SceneSession>::MakeSptr(targetInfo, nullptr);
    ssm_->sceneSessionMap_.insert({targetSceneSession->GetPersistentId(), targetSceneSession});

    ssm_->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    WMError result = ssm_->ShiftAppWindowPointerEvent(INVALID_SESSION_ID, targetSceneSession->GetPersistentId());
    EXPECT_EQ(result, WMError::WM_ERROR_INVALID_SESSION);
    result = ssm_->ShiftAppWindowPointerEvent(sourceSceneSession->GetPersistentId(), INVALID_SESSION_ID);
    EXPECT_EQ(result, WMError::WM_ERROR_INVALID_SESSION);
    ssm_->sceneSessionMap_.erase(sourceSceneSession->GetPersistentId());
    ssm_->sceneSessionMap_.erase(targetSceneSession->GetPersistentId());
}

/**
 * @tc.name: ShiftAppWindowPointerEvent02
 * @tc.desc: ShiftAppWindowPointerEvent,
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, ShiftAppWindowPointerEvent_02, Function | SmallTest | Level3)
{
    SessionInfo systemWindowInfo;
    systemWindowInfo.windowType_ = 2000;
    sptr<SceneSession> systemWindowSession = sptr<SceneSession>::MakeSptr(systemWindowInfo, nullptr);
    ssm_->sceneSessionMap_.insert({systemWindowSession->GetPersistentId(), systemWindowSession});

    SessionInfo mainWindowInfo;
    mainWindowInfo.windowType_ = 1;
    sptr<SceneSession> mainWindowSession = sptr<SceneSession>::MakeSptr(mainWindowInfo, nullptr);
    ssm_->sceneSessionMap_.insert({mainWindowSession->GetPersistentId(), mainWindowSession});

    int mainWindowPersistentId = mainWindowSession->GetPersistentId();
    int systemWindowPersistentId = systemWindowSession->GetPersistentId();
    ssm_->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    WMError result = ssm_->ShiftAppWindowPointerEvent(mainWindowPersistentId, systemWindowPersistentId);
    EXPECT_EQ(result, WMError::WM_ERROR_INVALID_CALLING);
    result = ssm_->ShiftAppWindowPointerEvent(systemWindowPersistentId, mainWindowPersistentId);
    EXPECT_EQ(result, WMError::WM_ERROR_INVALID_CALLING);
    ssm_->sceneSessionMap_.erase(systemWindowSession->GetPersistentId());
    ssm_->sceneSessionMap_.erase(mainWindowSession->GetPersistentId());
}

/**
 * @tc.name: ShiftAppWindowPointerEvent03
 * @tc.desc: ShiftAppWindowPointerEvent,
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, ShiftAppWindowPointerEvent_03, Function | SmallTest | Level3)
{
    SessionInfo sourceInfo;
    sourceInfo.windowType_ = 1;
    sptr<SceneSession> sourceSceneSession = sptr<SceneSession>::MakeSptr(sourceInfo, nullptr);
    ssm_->sceneSessionMap_.insert({sourceSceneSession->GetPersistentId(), sourceSceneSession});

    int32_t sourcePersistentId = sourceSceneSession->GetPersistentId();
    ssm_->systemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    WMError result = ssm_->ShiftAppWindowPointerEvent(sourcePersistentId, sourcePersistentId);
    EXPECT_EQ(result, WMError::WM_ERROR_DEVICE_NOT_SUPPORT);

    ssm_->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    result = ssm_->ShiftAppWindowPointerEvent(sourcePersistentId, sourcePersistentId);
    EXPECT_EQ(result, WMError::WM_ERROR_INVALID_CALLING);
    ssm_->sceneSessionMap_.erase(sourceSceneSession->GetPersistentId());
}

/**
 * @tc.name: ShiftAppWindowPointerEvent04
 * @tc.desc: ShiftAppWindowPointerEvent,
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, ShiftAppWindowPointerEvent_04, Function | SmallTest | Level3)
{
    SessionInfo sourceInfo;
    sourceInfo.windowType_ = 1;
    sptr<SceneSession> sourceSceneSession = sptr<SceneSession>::MakeSptr(sourceInfo, nullptr);
    ssm_->sceneSessionMap_.insert({sourceSceneSession->GetPersistentId(), sourceSceneSession});

    SessionInfo otherSourceInfo;
    otherSourceInfo.bundleName_ = "other";
    otherSourceInfo.windowType_ = 1;
    sptr<SceneSession> otherSourceSession = sptr<SceneSession>::MakeSptr(otherSourceInfo, nullptr);
    ssm_->sceneSessionMap_.insert({otherSourceSession->GetPersistentId(), otherSourceSession});

    SessionInfo otherTargetInfo;
    otherTargetInfo.bundleName_ = "other";
    otherTargetInfo.windowType_ = 1;
    sptr<SceneSession> otherTargetSession = sptr<SceneSession>::MakeSptr(otherTargetInfo, nullptr);
    ssm_->sceneSessionMap_.insert({otherTargetSession->GetPersistentId(), otherTargetSession});

    int32_t sourcePersistentId = sourceSceneSession->GetPersistentId();
    int32_t otherSourcePersistentId = otherSourceSession->GetPersistentId();
    int32_t otherTargetPersistentId = otherTargetSession->GetPersistentId();
    ssm_->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    WMError result = ssm_->ShiftAppWindowPointerEvent(sourcePersistentId, otherTargetPersistentId);
    EXPECT_EQ(result, WMError::WM_ERROR_INVALID_CALLING);
    result = ssm_->ShiftAppWindowPointerEvent(otherSourcePersistentId, otherTargetPersistentId);
    EXPECT_EQ(result, WMError::WM_ERROR_INVALID_CALLING);
    ssm_->sceneSessionMap_.erase(sourceSceneSession->GetPersistentId());
    ssm_->sceneSessionMap_.erase(otherSourceSession->GetPersistentId());
    ssm_->sceneSessionMap_.erase(otherTargetSession->GetPersistentId());
}

/**
 * @tc.name: GetKeyboardSession
 * @tc.desc: test GetKeyboardSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, GetKeyboardSession, Function | SmallTest | Level3)
{
    sptr<SceneSessionManager> ssm = sptr<SceneSessionManager>::MakeSptr();
    SessionInfo info;
    info.abilityName_ = "GetKeyboardSession";
    info.bundleName_ = "GetKeyboardSession";
    info.windowType_ = 2105; // 2105 is WINDOW_TYPE_INPUT_METHOD_FLOAT
    info.screenId_ = 1; // 1 is screenId
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_EQ(false, keyboardSession->IsSystemKeyboard());
    ssm->sceneSessionMap_.insert({ keyboardSession->GetPersistentId(), keyboardSession });
    sptr<KeyboardSession> systemKeyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    systemKeyboardSession->SetIsSystemKeyboard(true);
    ASSERT_EQ(true, systemKeyboardSession->IsSystemKeyboard());
    ssm->sceneSessionMap_.insert({ systemKeyboardSession->GetPersistentId(), systemKeyboardSession });

    ASSERT_EQ(nullptr, ssm->GetKeyboardSession(DISPLAY_ID_INVALID, false));
    ASSERT_NE(nullptr, ssm->GetKeyboardSession(keyboardSession->GetScreenId(), false));
    ASSERT_NE(nullptr, ssm->GetKeyboardSession(systemKeyboardSession->GetScreenId(), true));
}

/**
 * @tc.name: UpdateKeyboardAvoidAreaActive
 * @tc.desc: test UpdateKeyboardAvoidAreaActive
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, UpdateKeyboardAvoidAreaActive, Function | SmallTest | Level3)
{
    sptr<SceneSessionManager> ssm = sptr<SceneSessionManager>::MakeSptr();
    SessionInfo info;
    info.abilityName_ = "UpdateKeyboardAvoidAreaActive";
    info.bundleName_ = "UpdateKeyboardAvoidAreaActive";
    info.windowType_ = 2105; // 2105 is WINDOW_TYPE_INPUT_METHOD_FLOAT
    info.screenId_ = 1; // 1 is screenId
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(nullptr, keyboardSession->GetSessionProperty());
    keyboardSession->GetSessionProperty()->SetDisplayId(info.screenId_);
    ASSERT_EQ(false, keyboardSession->IsSystemKeyboard());
    ssm->sceneSessionMap_.insert({ keyboardSession->GetPersistentId(), keyboardSession });
    sptr<KeyboardSession> systemKeyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(nullptr, systemKeyboardSession->GetSessionProperty());
    systemKeyboardSession->GetSessionProperty()->SetDisplayId(info.screenId_);
    systemKeyboardSession->SetIsSystemKeyboard(true);
    ASSERT_EQ(true, systemKeyboardSession->IsSystemKeyboard());
    ssm->sceneSessionMap_.insert({ systemKeyboardSession->GetPersistentId(), systemKeyboardSession });

    ssm->UpdateKeyboardAvoidAreaActive(DISPLAY_ID_INVALID, true);
    ASSERT_EQ(true, keyboardSession->keyboardAvoidAreaActive_);
    ASSERT_EQ(true, systemKeyboardSession->keyboardAvoidAreaActive_);
    ssm->UpdateKeyboardAvoidAreaActive(systemKeyboardSession->GetScreenId(), false);
    ASSERT_EQ(true, keyboardSession->keyboardAvoidAreaActive_);
    ASSERT_EQ(false, systemKeyboardSession->keyboardAvoidAreaActive_);
    ssm->UpdateKeyboardAvoidAreaActive(systemKeyboardSession->GetScreenId(), true);
    ASSERT_EQ(false, keyboardSession->keyboardAvoidAreaActive_);
    ASSERT_EQ(true, systemKeyboardSession->keyboardAvoidAreaActive_);
}

/**
 * @tc.name: HandleKeyboardAvoidChange
 * @tc.desc: test HandleKeyboardAvoidChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, HandleKeyboardAvoidChange, Function | SmallTest | Level3)
{
    sptr<SceneSessionManager> ssm = sptr<SceneSessionManager>::MakeSptr();
    SessionInfo info;
    info.abilityName_ = "HandleKeyboardAvoidChange";
    info.bundleName_ = "HandleKeyboardAvoidChange";
    info.windowType_ = 2105; // 2105 is WINDOW_TYPE_INPUT_METHOD_FLOAT
    info.screenId_ = 1; // 1 is screenId
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(nullptr, keyboardSession->GetSessionProperty());
    keyboardSession->GetSessionProperty()->SetDisplayId(info.screenId_);
    ASSERT_EQ(false, keyboardSession->IsSystemKeyboard());
    ssm->sceneSessionMap_.insert({ keyboardSession->GetPersistentId(), keyboardSession });
    sptr<KeyboardSession> systemKeyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(nullptr, systemKeyboardSession->GetSessionProperty());
    systemKeyboardSession->GetSessionProperty()->SetDisplayId(info.screenId_);
    systemKeyboardSession->SetIsSystemKeyboard(true);
    ASSERT_EQ(true, systemKeyboardSession->IsSystemKeyboard());
    ssm->sceneSessionMap_.insert({ systemKeyboardSession->GetPersistentId(), systemKeyboardSession });

    ssm->systemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ssm->HandleKeyboardAvoidChange(keyboardSession, keyboardSession->GetScreenId(),
                                   SystemKeyboardAvoidChangeReason::KEYBOARD_CREATED);
    ASSERT_EQ(true, keyboardSession->keyboardAvoidAreaActive_);

    ssm->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ssm->HandleKeyboardAvoidChange(keyboardSession, keyboardSession->GetScreenId(),
                                   SystemKeyboardAvoidChangeReason::KEYBOARD_CREATED);
    ASSERT_EQ(true, keyboardSession->keyboardAvoidAreaActive_);

    ssm->HandleKeyboardAvoidChange(systemKeyboardSession, systemKeyboardSession->GetScreenId(),
                                    SystemKeyboardAvoidChangeReason::KEYBOARD_SHOW);
    ASSERT_EQ(false, keyboardSession->keyboardAvoidAreaActive_);
    ASSERT_EQ(true, systemKeyboardSession->keyboardAvoidAreaActive_);

    ssm->HandleKeyboardAvoidChange(systemKeyboardSession, systemKeyboardSession->GetScreenId(),
                                    SystemKeyboardAvoidChangeReason::KEYBOARD_GRAVITY_BOTTOM);
    ASSERT_EQ(false, keyboardSession->keyboardAvoidAreaActive_);
    ASSERT_EQ(true, systemKeyboardSession->keyboardAvoidAreaActive_);

    ssm->HandleKeyboardAvoidChange(systemKeyboardSession, systemKeyboardSession->GetScreenId(),
                                    SystemKeyboardAvoidChangeReason::KEYBOARD_HIDE);
    ASSERT_EQ(true, keyboardSession->keyboardAvoidAreaActive_);
    ASSERT_EQ(false, systemKeyboardSession->keyboardAvoidAreaActive_);

    ssm->HandleKeyboardAvoidChange(systemKeyboardSession, systemKeyboardSession->GetScreenId(),
                                    SystemKeyboardAvoidChangeReason::KEYBOARD_DISCONNECT);
    ASSERT_EQ(true, keyboardSession->keyboardAvoidAreaActive_);
    ASSERT_EQ(false, systemKeyboardSession->keyboardAvoidAreaActive_);

    ssm->HandleKeyboardAvoidChange(systemKeyboardSession, systemKeyboardSession->GetScreenId(),
                                    SystemKeyboardAvoidChangeReason::KEYBOARD_GRAVITY_FLOAT);
    ASSERT_EQ(true, keyboardSession->keyboardAvoidAreaActive_);
    ASSERT_EQ(false, systemKeyboardSession->keyboardAvoidAreaActive_);
}

/**
 * @tc.name: GetAllWindowLayoutInfo01
 * @tc.desc: HALF_FOLDED
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, GetAllWindowLayoutInfo01, Function | SmallTest | Level3)
{
    PcFoldScreenManager::GetInstance().UpdateFoldScreenStatus(0, SuperFoldStatus::HALF_FOLDED,
        { 0, 0, 2472, 1648 }, { 0, 1648, 2472, 1648 }, { 0, 1624, 2472, 1648 });
    SessionInfo sessionInfo;
    sessionInfo.isSystem_ = false;

    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sceneSession1->SetVisibilityState(WINDOW_VISIBILITY_STATE_NO_OCCLUSION);
    WSRect rect = { 0, 1500, 120, 1000 };
    sceneSession1->SetSessionRect(rect);
    int32_t zOrder = 100;
    sceneSession1->SetZOrder(zOrder);
    ssm_->sceneSessionMap_.insert({sceneSession1->GetPersistentId(), sceneSession1});

    constexpr DisplayId VIRTUAL_DISPLAY_ID = 999;
    std::vector<sptr<WindowLayoutInfo>> info;
    ssm_->GetAllWindowLayoutInfo(VIRTUAL_DISPLAY_ID, info);
    ssm_->sceneSessionMap_.clear();
    ASSERT_EQ(-2472, info[0]->rect.posY_);
}

/**
 * @tc.name: FilterForGetAllWindowLayoutInfo01
 * @tc.desc: test return by zOrder
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, FilterForGetAllWindowLayoutInfo01, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.isSystem_ = false;

    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sceneSession1->SetVisibilityState(WINDOW_VISIBILITY_STATE_NO_OCCLUSION);
    WSRect rect = { 0, 0, 120, 120 };
    sceneSession1->SetSessionRect(rect);
    int32_t zOrder = 100;
    sceneSession1->SetZOrder(zOrder);
    ssm_->sceneSessionMap_.insert({sceneSession1->GetPersistentId(), sceneSession1});
    sptr<SceneSession> sceneSession2 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sceneSession2->SetVisibilityState(WINDOW_VISIBILITY_STATE_NO_OCCLUSION);
    rect = { 0, 130, 120, 120 };
    sceneSession2->SetSessionRect(rect);
    zOrder = 101;
    sceneSession2->SetZOrder(zOrder);
    ssm_->sceneSessionMap_.insert({sceneSession2->GetPersistentId(), sceneSession2});
    
    constexpr DisplayId DEFAULT_DISPLAY_ID = 0;
    std::vector<sptr<SceneSession>> filteredSessions;
    ssm_->FilterForGetAllWindowLayoutInfo(DEFAULT_DISPLAY_ID, false, filteredSessions);
    ssm_->sceneSessionMap_.clear();
    ASSERT_EQ(130, filteredSessions[0]->GetSessionRect().posY_);
}

/**
 * @tc.name: FilterForGetAllWindowLayoutInfo02
 * @tc.desc: test isSystem_
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, FilterForGetAllWindowLayoutInfo02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.isSystem_ = false;
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sceneSession1->SetVisibilityState(WINDOW_VISIBILITY_STATE_NO_OCCLUSION);
    WSRect rect = { 0, 0, 120, 120 };
    sceneSession1->SetSessionRect(rect);
    int32_t zOrder = 100;
    sceneSession1->SetZOrder(zOrder);
    ssm_->sceneSessionMap_.insert({sceneSession1->GetPersistentId(), sceneSession1});

    sessionInfo.isSystem_ = true;
    sessionInfo.abilityName_ = "SCBSmartDock";
    sptr<SceneSession> sceneSession2 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sceneSession2->SetVisibilityState(WINDOW_VISIBILITY_STATE_NO_OCCLUSION);
    rect = { 0, 130, 120, 120 };
    sceneSession2->SetSessionRect(rect);
    zOrder = 101;
    sceneSession2->SetZOrder(zOrder);
    ssm_->sceneSessionMap_.insert({sceneSession2->GetPersistentId(), sceneSession2});

    sessionInfo.abilityName_ = "TestAbility";
    sptr<SceneSession> sceneSession3 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sceneSession3->SetVisibilityState(WINDOW_VISIBILITY_STATE_NO_OCCLUSION);
    rect = { 0, 250, 120, 120 };
    sceneSession3->SetSessionRect(rect);
    zOrder = 102;
    sceneSession3->SetZOrder(zOrder);
    ssm_->sceneSessionMap_.insert({sceneSession3->GetPersistentId(), sceneSession3});
    
    constexpr DisplayId DEFAULT_DISPLAY_ID = 0;
    std::vector<sptr<SceneSession>> filteredSessions;
    ssm_->FilterForGetAllWindowLayoutInfo(DEFAULT_DISPLAY_ID, false, filteredSessions);
    ssm_->sceneSessionMap_.clear();
    ASSERT_EQ(2, filteredSessions.size());
}

/**
 * @tc.name: FilterForGetAllWindowLayoutInfo03
 * @tc.desc: test VisibilityState
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, FilterForGetAllWindowLayoutInfo03, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.isSystem_ = false;
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sceneSession1->SetVisibilityState(WINDOW_VISIBILITY_STATE_NO_OCCLUSION);
    WSRect rect = { 0, 0, 120, 120 };
    sceneSession1->SetSessionRect(rect);
    int32_t zOrder = 101;
    sceneSession1->SetZOrder(zOrder);
    ssm_->sceneSessionMap_.insert({sceneSession1->GetPersistentId(), sceneSession1});

    sptr<SceneSession> sceneSession2 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sceneSession2->SetVisibilityState(WINDOW_VISIBILITY_STATE_PARTICALLY_OCCLUSION);
    rect = { 0, 0, 130, 120 };
    sceneSession2->SetSessionRect(rect);
    zOrder = 100;
    sceneSession2->SetZOrder(zOrder);
    ssm_->sceneSessionMap_.insert({sceneSession2->GetPersistentId(), sceneSession2});

    sptr<SceneSession> sceneSession3 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sceneSession3->SetVisibilityState(WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION);
    rect = { 0, 0, 100, 100 };
    sceneSession3->SetSessionRect(rect);
    zOrder = 99;
    sceneSession3->SetZOrder(zOrder);
    ssm_->sceneSessionMap_.insert({sceneSession3->GetPersistentId(), sceneSession3});
    
    constexpr DisplayId DEFAULT_DISPLAY_ID = 0;
    std::vector<sptr<SceneSession>> filteredSessions;
    ssm_->FilterForGetAllWindowLayoutInfo(DEFAULT_DISPLAY_ID, false, filteredSessions);
    ssm_->sceneSessionMap_.clear();
    ASSERT_EQ(2, filteredSessions.size());
}

/**
 * @tc.name: FilterForGetAllWindowLayoutInfo04
 * @tc.desc: HALF_FOLDED
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, FilterForGetAllWindowLayoutInfo04, Function | SmallTest | Level3)
{
    PcFoldScreenManager::GetInstance().UpdateFoldScreenStatus(0, SuperFoldStatus::HALF_FOLDED,
        { 0, 0, 2472, 1648 }, { 0, 1648, 2472, 1648 }, { 0, 1624, 2472, 1648 });
    SessionInfo sessionInfo;
    sessionInfo.isSystem_ = false;

    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sceneSession1->SetVisibilityState(WINDOW_VISIBILITY_STATE_NO_OCCLUSION);
    WSRect rect = { 0, 0, 120, 120 };
    sceneSession1->SetSessionRect(rect);
    int32_t zOrder = 100;
    sceneSession1->SetZOrder(zOrder);
    ssm_->sceneSessionMap_.insert({sceneSession1->GetPersistentId(), sceneSession1});

    sptr<SceneSession> sceneSession2 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sceneSession2->SetVisibilityState(WINDOW_VISIBILITY_STATE_NO_OCCLUSION);
    rect = { 0, 4000, 120, 120 };
    sceneSession2->SetSessionRect(rect);
    zOrder = 101;
    sceneSession2->SetZOrder(zOrder);
    ssm_->sceneSessionMap_.insert({sceneSession2->GetPersistentId(), sceneSession2});

    sptr<SceneSession> sceneSession3 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sceneSession3->SetVisibilityState(WINDOW_VISIBILITY_STATE_NO_OCCLUSION);
    rect = { 0, 1500, 120, 1000 };
    sceneSession3->SetSessionRect(rect);
    zOrder = 102;
    sceneSession3->SetZOrder(zOrder);
    ssm_->sceneSessionMap_.insert({sceneSession3->GetPersistentId(), sceneSession3});

    constexpr DisplayId DEFAULT_DISPLAY_ID = 0;

    std::vector<sptr<SceneSession>> filteredSessions1;
    ssm_->FilterForGetAllWindowLayoutInfo(DEFAULT_DISPLAY_ID, false, filteredSessions1);
    EXPECT_EQ(2, filteredSessions1.size());
    std::vector<sptr<SceneSession>> filteredSessions2;
    ssm_->FilterForGetAllWindowLayoutInfo(DEFAULT_DISPLAY_ID, true, filteredSessions2);
    ssm_->sceneSessionMap_.clear();
    ASSERT_EQ(2, filteredSessions2.size());
}

/**
 * @tc.name: FilterForGetAllWindowLayoutInfo05
 * @tc.desc: session is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, FilterForGetAllWindowLayoutInfo05, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sceneSession = nullptr;
    ssm_->sceneSessionMap_.insert({ 1, sceneSession });
    constexpr DisplayId DEFAULT_DISPLAY_ID = 0;
    std::vector<sptr<SceneSession>> filteredSessions;
    ssm_->FilterForGetAllWindowLayoutInfo(DEFAULT_DISPLAY_ID, false, filteredSessions);
    ssm_->sceneSessionMap_.clear();
    ASSERT_EQ(0, filteredSessions.size());
}

/**
 * @tc.name: GetFoldLowerScreenPosY01
 * @tc.desc: test get fold lower screen posY
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, GetFoldLowerScreenPosY01, Function | SmallTest | Level3)
{
    PcFoldScreenManager::GetInstance().UpdateFoldScreenStatus(0, SuperFoldStatus::HALF_FOLDED,
        { 0, 0, 2472, 1648 }, { 0, 1648, 2472, 1648 }, { 0, 1624, 2472, 1648 });
    ASSERT_EQ(2472, ssm_->GetFoldLowerScreenPosY());
}

/**
 * @tc.name: IsGetWindowLayoutInfoNeeded01
 * @tc.desc: not System
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, IsGetWindowLayoutInfoNeeded01, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.isSystem_ = false;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_EQ(true, ssm_->IsGetWindowLayoutInfoNeeded(sceneSession));
}

/**
 * @tc.name: IsGetWindowLayoutInfoNeeded02
 * @tc.desc: is System, not in whitelist
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, IsGetWindowLayoutInfoNeeded02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.isSystem_ = true;
    sessionInfo.abilityName_ = "TestAbility";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_EQ(false, ssm_->IsGetWindowLayoutInfoNeeded(sceneSession));
}

/**
 * @tc.name: IsGetWindowLayoutInfoNeeded03
 * @tc.desc: is System, in whitelist
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, IsGetWindowLayoutInfoNeeded03, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.isSystem_ = true;
    sessionInfo.abilityName_ = "SCBSmartDock";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_EQ(true, ssm_->IsGetWindowLayoutInfoNeeded(sceneSession));
}
}
} // namespace Rosen
} // namespace OHOS
