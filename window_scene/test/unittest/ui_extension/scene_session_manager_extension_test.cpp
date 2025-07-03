/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "interfaces/include/ws_common.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "session_manager.h"
#include "test/mock/mock_session_stage.h"
#include <ability_manager_client.h>
#include <mock_uiext_session_permission.h>
#include <mock_ability_manager_client.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    const std::string BUNDLE_NAME = "uiextension";
    const std::string BUNDLE_NAME_TEST = "test";
}
class SceneSessionManagerExtensionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<SceneSessionManager> ssm_;
};

sptr<SceneSessionManager> SceneSessionManagerExtensionTest::ssm_ = nullptr;

void SceneSessionManagerExtensionTest::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerExtensionTest::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerExtensionTest::SetUp()
{
    ssm_->systemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
}

void SceneSessionManagerExtensionTest::TearDown()
{
    ssm_->sceneSessionMap_.clear();
}

namespace {
/**
 * @tc.name: CheckSubSessionStartedByExtensionAndSetDisplayId01
 * @tc.desc: normal test
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerExtensionTest, CheckSubSessionStartedByExtensionAndSetDisplayId01, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    sptr<SceneSession::SpecificSessionCallback> callback = ssm_->CreateSpecificSessionCallback();
    sptr<SceneSession> parentSession = sptr<SceneSession>::MakeSptr(info, callback);
    ssm_->sceneSessionMap_.insert({ parentSession->GetPersistentId(), parentSession });
    sptr<IRemoteObject> token;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    sptr<ISessionStage> sessionStage = sptr<SessionStageMocker>::MakeSptr();
    EXPECT_EQ(ssm_->CheckSubSessionStartedByExtensionAndSetDisplayId(token, property, sessionStage),
        WSError::WS_ERROR_NULLPTR);
    property->SetParentPersistentId(parentSession->GetPersistentId());
    constexpr DisplayId displayId = 0;
    parentSession->GetSessionProperty()->SetDisplayId(displayId);
    EXPECT_EQ(ssm_->CheckSubSessionStartedByExtensionAndSetDisplayId(token, property, nullptr), WSError::WS_OK);
    EXPECT_EQ(ssm_->CheckSubSessionStartedByExtensionAndSetDisplayId(token, property, sessionStage), WSError::WS_OK);
    property->SetIsUIExtFirstSubWindow(true);
    EXPECT_EQ(ssm_->CheckSubSessionStartedByExtensionAndSetDisplayId(token, property, sessionStage), WSError::WS_OK);
    EXPECT_EQ(property->GetDisplayId(), displayId);
}

/**
 * @tc.name: CheckSubSessionStartedByExtensionAndSetDisplayId02
 * @tc.desc: test compare host bundleName and parent bundleName
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerExtensionTest, CheckSubSessionStartedByExtensionAndSetDisplayId02, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.bundleName_ = BUNDLE_NAME;
    sptr<SceneSession::SpecificSessionCallback> callback = ssm_->CreateSpecificSessionCallback();
    sptr<SceneSession> parentSession = sptr<SceneSession>::MakeSptr(info, callback);
    ssm_->sceneSessionMap_.insert({ parentSession->GetPersistentId(), parentSession });
    sptr<IRemoteObject> token;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    sptr<ISessionStage> sessionStage = sptr<SessionStageMocker>::MakeSptr();
    property->SetParentPersistentId(parentSession->GetPersistentId());
    property->SetIsUIExtensionAbilityProcess(true);
    EXPECT_EQ(ssm_->CheckSubSessionStartedByExtensionAndSetDisplayId(token, property, sessionStage), WSError::WS_OK);

    MockSessionPermission::SetIsStartedByUIExtensionFlag(true);
    AAFwk::UIExtensionHostInfo hostInfo;
    hostInfo.elementName_.SetBundleName(BUNDLE_NAME);
    AAFwk::MockAbilityManagerClient::SetUIExtensionRootHostInfo(hostInfo);
    AAFwk::UIExtensionSessionInfo extensionSessionInfo;
    extensionSessionInfo.hostElementName.SetBundleName(BUNDLE_NAME);
    AAFwk::MockAbilityManagerClient::SetUIExtensionSessionInfo(extensionSessionInfo);
    EXPECT_EQ(ssm_->CheckSubSessionStartedByExtensionAndSetDisplayId(token, property, sessionStage), WSError::WS_OK);

    hostInfo.elementName_.SetBundleName(BUNDLE_NAME_TEST);
    AAFwk::MockAbilityManagerClient::SetUIExtensionRootHostInfo(hostInfo);
    EXPECT_EQ(ssm_->CheckSubSessionStartedByExtensionAndSetDisplayId(token, property, sessionStage), WSError::WS_OK);

    extensionSessionInfo.hostElementName.SetBundleName(BUNDLE_NAME_TEST);
    AAFwk::MockAbilityManagerClient::SetUIExtensionSessionInfo(extensionSessionInfo);
    EXPECT_EQ(ssm_->CheckSubSessionStartedByExtensionAndSetDisplayId(token, property, sessionStage),
        WSError::WS_ERROR_INVALID_WINDOW);
    MockSessionPermission::ClearAllFlag();
    AAFwk::MockAbilityManagerClient::ClearAll();
}

/**
 * @tc.name: CreateAndConnectSpecificSession04
 * @tc.desc: CreateAndConnectSpecificSession test uiextension hideNonSecureFloatWindows
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, CreateAndConnectSpecificSession_TestHideNonSecureFloatWindow, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<ISessionStage> sessionStage = sptr<SessionStageMocker>::MakeSptr();
    sptr<IWindowEventChannel> eventChannel = sptr<WindowEventChannelMocker>::MakeSptr(sessionStage);
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    sptr<ISession> session;
    SystemSessionConfig systemConfig;
    sptr<IRemoteObject> token;
    int32_t id = 0;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();

    MockUIExtSessionPermission::SetIsSystemCallingFlag(false);
    MockUIExtSessionPermission::SetVerifyCallingPermissionFlag(true);
    MockUIExtSessionPermission::SetIsStartByHdcdFlag(true);
    property->SetWindowFlags(123);
    property->SetParentPersistentId(11);
    property->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    property->SetFloatingWindowAppType(true);
    ssm_->shouldHideNonSecureFloatingWindows_ = true;
    WSError res = ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_OPERATION, res);

    ssm_->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    res = ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    ASSERT_EQ(WSError::WS_OK, res);
    MockUIExtSessionPermission::ClearAllFlag();
}

/**
 * @tc.name: CreateAndConnectSpecificSession05
 * @tc.desc: CreateAndConnectSpecificSession test uiextension hideNonSecureSubWindows
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, CreateAndConnectSpecificSession_TestHideNonSecureSubWindow, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<ISessionStage> sessionStage = sptr<SessionStageMocker>::MakeSptr();
    sptr<IWindowEventChannel> eventChannel = sptr<WindowEventChannelMocker>::MakeSptr(sessionStage);
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    sptr<ISession> session;
    SystemSessionConfig systemConfig;
    sptr<IRemoteObject> token;
    int32_t id = 0;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();

    MockUIExtSessionPermission::SetIsSystemCallingFlag(false);
    property->SetWindowFlags(123);
    property->SetParentPersistentId(11);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    SessionInfo sessionInfo;
    sptr<SceneSession> parentSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    parentSession->persistentId_ = 11;
    ExtensionWindowFlags extWindowFlags(0);
    extWindowFlags.hideNonSecureWindowsFlag = true;
    parentSession->UpdateExtWindowFlags(parentSession->GetPersistentId(), extWindowFlags, extWindowFlags);
    parentSession->SetSessionState(SessionState::STATE_FOREGROUND);
    ssm_->sceneSessionMap_.insert( {parentSession->GetPersistentId(), parentSession} );
    WSError res = ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_OPERATION, res);

    ssm_->sceneSessionMap_.erase(parentSession->GetPersistentId());
    res = ssm_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property, id, session,
        systemConfig, token);
    ASSERT_EQ(WSError::WS_OK, res);
    MockUIExtSessionPermission::ClearAllFlag();
}
} // namespace
} // namespace Rosen
} // namespace OHOS