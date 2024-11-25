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
 * @tc.name: CreateAndBackgroundSceneSession
 * @tc.desc: CreateAndBackgroundSceneSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, CreateAndBackgroundSceneSession01, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "CreateAndBackgroundSceneSession01";
    info.bundleName_ = "CreateAndBackgroundSceneSession01";
    info.moduleName_ = "CreateAndBackgroundSceneSession01";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sceneSession->SetSessionProperty(property);
    ssm_->CreateSceneSession(info, nullptr);
    info.isSystem_ = true;
    info.windowType_ = 3;
    sptr<SceneSession> getSceneSession1 = ssm_->CreateSceneSession(info, property);
    ASSERT_NE(nullptr, getSceneSession1);
    ssm_->NotifySessionUpdate(info, ActionType::SINGLE_START, 0);
    ssm_->UpdateSceneSessionWant(info);

    ssm_->RequestSceneSessionActivation(sceneSession, true);

    sptr<AAFwk::SessionInfo> abilitySessionInfo;
    ssm_->NotifyCollaboratorAfterStart(sceneSession, abilitySessionInfo);
    ASSERT_EQ(WSError::WS_OK, ssm_->RequestSceneSessionBackground(sceneSession, true, true));
}

/**
 * @tc.name: CreateAndBackgroundSceneSession
 * @tc.desc: CreateAndBackgroundSceneSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, CreateAndBackgroundSceneSession02, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "CreateAndBackgroundSceneSession02";
    info.bundleName_ = "CreateAndBackgroundSceneSession02";
    info.moduleName_ = "CreateAndBackgroundSceneSession02";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sceneSession->SetSessionProperty(property);
    ssm_->CreateSceneSession(info, nullptr);
    info.isSystem_ = true;
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    info.windowType_ = 1001;
    sptr<SceneSession> getSceneSession2 = ssm_->CreateSceneSession(info, property);
    ASSERT_NE(nullptr, getSceneSession2);

    ASSERT_EQ(WindowType::WINDOW_TYPE_APP_SUB_WINDOW, getSceneSession2->GetWindowType());

    ssm_->NotifySessionUpdate(info, ActionType::SINGLE_START, 0);
    ssm_->UpdateSceneSessionWant(info);

    ssm_->RequestSceneSessionActivation(sceneSession, true);

    sptr<AAFwk::SessionInfo> abilitySessionInfo;
    ssm_->NotifyCollaboratorAfterStart(sceneSession, abilitySessionInfo);
    ASSERT_EQ(WSError::WS_OK, ssm_->RequestSceneSessionBackground(sceneSession, true, true));
}

/**
 * @tc.name: CreateAndBackgroundSceneSession
 * @tc.desc: CreateAndBackgroundSceneSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, CreateAndBackgroundSceneSession03, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "CreateAndBackgroundSceneSession03";
    info.bundleName_ = "CreateAndBackgroundSceneSession03";
    info.moduleName_ = "CreateAndBackgroundSceneSession03";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sceneSession->SetSessionProperty(property);
    ssm_->CreateSceneSession(info, nullptr);
    info.isSystem_ = true;
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    info.windowType_ = 2105;
    sptr<SceneSession> getSceneSession3 = ssm_->CreateSceneSession(info, property);
    ASSERT_NE(nullptr, getSceneSession3);

    ASSERT_EQ(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT, getSceneSession3->GetWindowType());

    ssm_->NotifySessionUpdate(info, ActionType::SINGLE_START, 0);
    ssm_->UpdateSceneSessionWant(info);

    ssm_->RequestSceneSessionActivation(sceneSession, true);

    sptr<AAFwk::SessionInfo> abilitySessionInfo;
    ssm_->NotifyCollaboratorAfterStart(sceneSession, abilitySessionInfo);
    ASSERT_EQ(WSError::WS_OK, ssm_->RequestSceneSessionBackground(sceneSession, true, true));
}

/**
 * @tc.name: CreateAndBackgroundSceneSession
 * @tc.desc: CreateAndBackgroundSceneSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, CreateAndBackgroundSceneSession04, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "CreateAndBackgroundSceneSession04";
    info.bundleName_ = "CreateAndBackgroundSceneSession04";
    info.moduleName_ = "CreateAndBackgroundSceneSession04";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sceneSession->SetSessionProperty(property);
    ssm_->CreateSceneSession(info, nullptr);
    info.isSystem_ = true;
    property->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    info.windowType_ = 2500;
    sptr<SceneSession> getSceneSession4 = ssm_->CreateSceneSession(info, property);
    ASSERT_NE(nullptr, getSceneSession4);
    ASSERT_EQ(WindowType::SYSTEM_SUB_WINDOW_BASE, getSceneSession4->GetWindowType());

    ssm_->NotifySessionUpdate(info, ActionType::SINGLE_START, 0);
    ssm_->UpdateSceneSessionWant(info);

    ssm_->RequestSceneSessionActivation(sceneSession, true);

    sptr<AAFwk::SessionInfo> abilitySessionInfo;
    ssm_->NotifyCollaboratorAfterStart(sceneSession, abilitySessionInfo);
    ASSERT_EQ(WSError::WS_OK, ssm_->RequestSceneSessionBackground(sceneSession, true, true));
}

/**
 * @tc.name: ForegroundAndDisconnect01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, ForegroundAndDisconnect01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ForegroundAndDisconnect01";
    info.bundleName_ = "ForegroundAndDisconnect01";
    info.moduleName_ = "ForegroundAndDisconnect01";

    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    ASSERT_NE(specificCallback_, nullptr);

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    sceneSession->isActive_ = true;
    sceneSession->specificCallback_ = specificCallback_;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);

    auto result = sceneSession->Foreground(property);
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession->UpdateSessionState(SessionState::STATE_CONNECT);
    result = sceneSession->DisconnectTask(true, true);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: ForegroundAndDisconnect02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, ForegroundAndDisconnect02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ForegroundAndDisconnect02";
    info.bundleName_ = "ForegroundAndDisconnect02";
    info.moduleName_ = "ForegroundAndDisconnect02";

    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    ASSERT_NE(specificCallback_, nullptr);

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    sceneSession->isActive_ = true;
    sptr<WindowSessionProperty> property = nullptr;
    auto result = sceneSession->Foreground(property);
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession->UpdateSessionState(SessionState::STATE_ACTIVE);
    result = sceneSession->BackgroundTask(true);
    ASSERT_EQ(result, WSError::WS_OK);

    result = sceneSession->DisconnectTask(true, true);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: ForegroundAndDisconnect03
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, ForegroundAndDisconnect03, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ForegroundAndDisconnect03";
    info.bundleName_ = "ForegroundAndDisconnect03";
    info.moduleName_ = "ForegroundAndDisconnect03";
    info.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);

    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    ASSERT_NE(specificCallback_, nullptr);

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    sceneSession->isActive_ = true;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    property->SetAnimationFlag(static_cast<uint32_t>(WindowAnimation::CUSTOM));
    auto result = sceneSession->Foreground(property);
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession->UpdateSessionState(SessionState::STATE_ACTIVE);
    result = sceneSession->BackgroundTask(true);
    ASSERT_EQ(result, WSError::WS_OK);

    result = sceneSession->DisconnectTask(true, true);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: ForegroundAndDisconnect04
 * @tc.desc: ForegroundAndDisconnect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, ForegroundAndDisconnect04, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ForegroundAndDisconnect04";
    info.bundleName_ = "ForegroundAndDisconnect04";
    info.moduleName_ = "ForegroundAndDisconnect04";
    info.windowType_ = static_cast<uint32_t>(WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE);

    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    ASSERT_NE(specificCallback_, nullptr);

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    sceneSession->isActive_ = true;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    property->SetAnimationFlag(static_cast<uint32_t>(WindowAnimation::CUSTOM));
    auto result = sceneSession->Foreground(property);
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession->UpdateSessionState(SessionState::STATE_INACTIVE);
    result = sceneSession->BackgroundTask(true);
    ASSERT_EQ(result, WSError::WS_OK);

    result = sceneSession->DisconnectTask(true, true);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: ForegroundAndDisconnect05
 * @tc.desc: ForegroundAndDisconnect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, ForegroundAndDisconnect05, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ForegroundAndDisconnect05";
    info.bundleName_ = "ForegroundAndDisconnect05";
    info.moduleName_ = "ForegroundAndDisconnect05";

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);

    sceneSession->Session::SetSessionState(SessionState::STATE_CONNECT);

    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    sceneSession->SetLeashWinSurfaceNode(surfaceNode);
    auto result = sceneSession->Foreground(property);
    ASSERT_EQ(result, WSError::WS_OK);

    result = sceneSession->BackgroundTask(true);
    ASSERT_EQ(result, WSError::WS_OK);

    result = sceneSession->DisconnectTask(true, true);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: ForegroundAndDisconnect06
 * @tc.desc: ForegroundAndDisconnect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, ForegroundAndDisconnect06, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ForegroundAndDisconnect06";
    info.bundleName_ = "ForegroundAndDisconnect06";
    info.moduleName_ = "ForegroundAndDisconnect06";

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sceneSession->Session::SetSessionState(SessionState::STATE_CONNECT);

    sceneSession->SetLeashWinSurfaceNode(nullptr);
    auto result = sceneSession->Foreground(property);
    ASSERT_EQ(result, WSError::WS_OK);

    result = sceneSession->BackgroundTask(true);
    ASSERT_EQ(result, WSError::WS_OK);

    result = sceneSession->DisconnectTask(true, true);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: RequestKeyboardPanelSession
 * @tc.desc: test RequestKeyboardPanelSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, RequestKeyboardPanelSession, Function | SmallTest | Level2)
{
    sptr<SceneSessionManager> ssm = new (std::nothrow) SceneSessionManager();
    ASSERT_NE(nullptr, ssm);
    std::string panelName = "SystemKeyboardPanel";
    ASSERT_NE(nullptr, ssm->RequestKeyboardPanelSession(panelName, 0)); // 0 is screenId
    ssm->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ASSERT_NE(nullptr, ssm->RequestKeyboardPanelSession(panelName, 0)); // 0 is screenId
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
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(keyboardInfo, nullptr, nullptr);
    ASSERT_NE(nullptr, keyboardSession);
    ASSERT_EQ(nullptr, keyboardSession->GetKeyboardPanelSession());

    sptr<SceneSessionManager> ssm = new (std::nothrow) SceneSessionManager();
    ASSERT_NE(nullptr, ssm);

    // the keyboard panel enabled flag of ssm is false
    ssm->CreateKeyboardPanelSession(keyboardSession);
    ASSERT_EQ(nullptr, keyboardSession->GetKeyboardPanelSession());

    // keyboard session is nullptr
    ssm->isKeyboardPanelEnabled_ = true;
    ssm->CreateKeyboardPanelSession(nullptr);
    ASSERT_EQ(nullptr, keyboardSession->GetKeyboardPanelSession());

    // the property of keyboard session is nullptr
    ASSERT_EQ(WSError::WS_OK, keyboardSession->SetSessionProperty(nullptr));
    ASSERT_EQ(nullptr, keyboardSession->GetSessionProperty());
    ssm->CreateKeyboardPanelSession(keyboardSession);
    ASSERT_EQ(nullptr, keyboardSession->GetKeyboardPanelSession());

    // the keyboard session is system keyboard
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
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
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(keyboardInfo, nullptr, nullptr);
    ASSERT_NE(nullptr, keyboardSession);
    ASSERT_EQ(nullptr, keyboardSession->GetKeyboardPanelSession());

    sptr<SceneSessionManager> ssm = new (std::nothrow) SceneSessionManager();
    ASSERT_NE(nullptr, ssm);

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
 * @tc.name: CheckSystemWindowPermission02
 * @tc.desc: test CheckSystemWindowPermission
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, CheckSystemWindowPermission02, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, ssm_);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);

    property->SetWindowType(WindowType::WINDOW_TYPE_UI_EXTENSION);
    ASSERT_EQ(false, ssm_->CheckSystemWindowPermission(property));
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW); // main window is not system window
    ASSERT_EQ(true, ssm_->CheckSystemWindowPermission(property));

    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    ASSERT_EQ(false, ssm_->CheckSystemWindowPermission(property));
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_STATUS_BAR);
    ASSERT_EQ(false, ssm_->CheckSystemWindowPermission(property));

    property->SetIsSystemKeyboard(true);
    ASSERT_EQ(false, ssm_->CheckSystemWindowPermission(property));
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    ASSERT_EQ(false, ssm_->CheckSystemWindowPermission(property));

    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ASSERT_EQ(true, ssm_->CheckSystemWindowPermission(property));
    property->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    ASSERT_EQ(true, ssm_->CheckSystemWindowPermission(property));

    property->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    ASSERT_EQ(true, ssm_->CheckSystemWindowPermission(property));

    property->SetWindowType(WindowType::WINDOW_TYPE_TOAST);
    ASSERT_EQ(true, ssm_->CheckSystemWindowPermission(property));
}
}
} // namespace Rosen
} // namespace OHOS
