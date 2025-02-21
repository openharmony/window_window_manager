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
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

sptr<SceneSessionManager> SceneSessionManagerTest12::ssm_ = nullptr;

void SceneSessionManagerTest12::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance;
}

void SceneSessionManagerTest12::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerTest12::SetUp()
{
}

void ScenenSessionManagerTest12::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
}

namespace {
/**
 * @tc.name: HasFloatingWindowForeground01
 * @tc.desc: test HasFloatingWindowForeground with null abilityToken
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, HasFloatingWindowForeground01, Function | SmallTest | Level3)
{
    bool hasFloatWindowForeground = false;
    WMError result = ssm_->HasFloatingWindowForeground(nullptr, hasFloatWindowForeground);
    EXPECT_EQ(result, WMError::WM_ERROR_NULLPTR);
    EXPECT_EQ(hasFloatWindowForeground, false);
}

/**
 * @tc.name: HasFloatingWindowForeground02
 * @tc.desc: test HasFloatingWindowForeground with not existed abilityToken
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, HasFloatingWindowForeground02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.sessionState_ = SessionState::STATE_ACTIVE;
    sessionInfo.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);

    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    sptr<IRemoteObject> token1 = sptr<MockIRemoteObject>::MakeSptr();
    sceneSession->SetAbilityToken(token1);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);
    ssm_->sceneSessionMap_.insert({ sceneSession->GetPersistentId(), sceneSession });

    bool hasFloatWindowForeground = false;
    sptr<IRemoteObject> token2 = sptr<MockIRemoteObject>::MakeSptr();
    WMError result = ssm_->HasFloatingWindowForeground(token2, hasFloatWindowForeground);
    EXPECT_EQ(result, WMError::WM_OK);
    EXPECT_EQ(hasFloatWindowForeground, false);
}

/**
 * @tc.name: HasFloatingWindowForeground03
 * @tc.desc: test HasFloatingWindowForeground with existed foreground float window
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, HasFloatingWindowForeground03, Function | SmallTest | Level3)
{
    // create first test sceneSession
    SessionInfo sessionInfo1;
    sessionInfo1.sessionState_ = SessionState::STATE_ACTIVE;
    sessionInfo1.persistentId_ = 1;

    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo1, nullptr);
    sptr<IRemoteObject> token1 = sptr<MockIRemoteObject>::MakeSptr();
    sceneSession1->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    sceneSession1->SetAbilityToken(token1);
    sceneSession1->SetSessionState(SessionState::STATE_ACTIVE);

    // create second test sceneSession
    SessionInfo sessionInfo2;
    sessionInfo2.sessionState_ = SessionState::STATE_FOREGROUND;
    sessionInfo2.persistentId_ = 2;

    sptr<SceneSession> sceneSession2 = sptr<SceneSession>::MakeSptr(sessionInfo2, nullptr);
    sptr<IRemoteObject> token2 = sptr<MockIRemoteObject>::MakeSptr();
    sceneSession2->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    sceneSession2->SetAbilityToken(token2);
    sceneSession2->SetSessionState(SessionState::STATE_FOREGROUND);

    ssm_->sceneSessionMap_.insert({ sceneSession1->GetPersistentId(), sceneSession1 });
    ssm_->sceneSessionMap_.insert({ sceneSession2->GetPersistentId(), sceneSession2 });

    bool hasFloatWindowForeground = false;
    WMError result = ssm_->HasFloatingWindowForeground(token1, hasFloatWindowForeground);
    EXPECT_EQ(result, WMError::WM_OK);
    EXPECT_EQ(hasFloatWindowForeground, true);

    hasFloatWindowForeground = false;
    result = ssm_->HasFloatingWindowForeground(token2, hasFloatWindowForeground);
    EXPECT_EQ(result, WMError::WM_OK);
    EXPECT_EQ(hasFloatWindowForeground, true);
}

/**
 * @tc.name: HasFloatingWindowForeground04
 * @tc.desc: test HasFloatingWindowForeground with existed background float window
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, HasFloatingWindowForeground04, Function | SmallTest | Level3)
{
    // create first test sceneSession
    SessionInfo sessionInfo1;
    sessionInfo1.sessionState_ = SessionState::STATE_INACTIVE;
    sessionInfo1.persistentId_ = 1;

    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo1, nullptr);
    sptr<IRemoteObject> token1 = sptr<MockIRemoteObject>::MakeSptr();
    sceneSession1->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    sceneSession1->SetAbilityToken(token1);
    sceneSession1->SetSessionState(SessionState::STATE_INACTIVE);

    // create second test sceneSession
    SessionInfo sessionInfo2;
    sessionInfo2.sessionState_ = SessionState::STATE_BACKGROUND;
    sessionInfo2.persistentId_ = 2;

    sptr<SceneSession> sceneSession2 = sptr<SceneSession>::MakeSptr(sessionInfo2, nullptr);
    sptr<IRemoteObject> token2 = sptr<MockIRemoteObject>::MakeSptr();
    sceneSession2->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    sceneSession2->SetAbilityToken(token2);
    sceneSession2->SetSessionState(SessionState::STATE_BACKGROUND);

    ssm_->sceneSessionMap_.insert({ sceneSession1->GetPersistentId(), sceneSession1 });
    ssm_->sceneSessionMap_.insert({ sceneSession2->GetPersistentId(), sceneSession2 });

    bool hasFloatWindowForeground = false;
    WMError result = ssm_->HasFloatingWindowForeground(token1, hasFloatWindowForeground);
    EXPECT_EQ(result, WMError::WM_OK);
    EXPECT_EQ(hasFloatWindowForeground, false);

    hasFloatWindowForeground = false;
    result = ssm_->HasFloatingWindowForeground(token2, hasFloatWindowForeground);
    EXPECT_EQ(result, WMError::WM_OK);
    EXPECT_EQ(hasFloatWindowForeground, false);
}

/**
 * @tc.name: HasFloatingWindowForeground05
 * @tc.desc: test HasFloatingWindowForeground with existed forground toast window
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, HasFloatingWindowForeground05, Function | SmallTest | Level3)
{
    // create first test sceneSession
    SessionInfo sessionInfo;
    sessionInfo.sessionState_ = SessionState::STATE_INACTIVE;
    sessionInfo.persistentId_ = 1;

    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sptr<IRemoteObject> token = sptr<MockIRemoteObject>::MakeSptr();
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    sceneSession->SetAbilityToken(token);
    sceneSession->SetSessionState(SessionState::STATE_INACTIVE);

    ssm_->sceneSessionMap_.insert({ sceneSession->GetPersistentId(), sceneSession });

    bool hasFloatWindowForeground = false;
    WMError result = ssm_->HasFloatingWindowForeground(token, hasFloatWindowForeground);
    EXPECT_EQ(result, WMError::WM_OK);
    EXPECT_EQ(hasFloatWindowForeground, false);
}

/**
 * @tc.name: HasFloatingWindowForeground06
 * @tc.desc: test HasFloatingWindowForeground with other foreground float window
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest12, HasFloatingWindowForeground06, Function | SmallTest | Level3)
{
    // create first test sceneSession
    SessionInfo sessionInfo1;
    sessionInfo1.sessionState_ = SessionState::STATE_ACTIVE;
    sessionInfo1.persistentId_ = 1;

    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo1, nullptr);
    sptr<IRemoteObject> token1 = sptr<MockIRemoteObject>::MakeSptr();
    sceneSession1->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    sceneSession1->SetAbilityToken(token1);
    sceneSession1->SetSessionState(SessionState::STATE_ACTIVE);

    // create second test sceneSession
    SessionInfo sessionInfo2;
    sessionInfo2.sessionState_ = SessionState::STATE_BACKGROUND;
    sessionInfo2.persistentId_ = 2;

    sptr<SceneSession> sceneSession2 = sptr<SceneSession>::MakeSptr(sessionInfo2, nullptr);
    sptr<IRemoteObject> token2 = sptr<MockIRemoteObject>::MakeSptr();
    sceneSession2->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    sceneSession2->SetAbilityToken(token2);
    sceneSession2->SetSessionState(SessionState::STATE_BACKGROUND);

    ssm_->sceneSessionMap_.insert({ sceneSession1->GetPersistentId(), sceneSession1 });
    ssm_->sceneSessionMap_.insert({ sceneSession2->GetPersistentId(), sceneSession2 });

    bool hasFloatWindowForeground = false;
    WMError result = ssm_->HasFloatingWindowForeground(token2, hasFloatWindowForeground);
    EXPECT_EQ(result, WMError::WM_OK);
    EXPECT_EQ(hasFloatWindowForeground, false);
}
}
} // namespace Rosen
} // namespace OHOS