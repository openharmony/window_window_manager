/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "mock/mock_scene_session_manager_stub.h"
#include "session/host/include/session.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/scene_session_manager_lite.h"
#include "window_scene/interfaces/include/ws_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class SceneSessionManagerGamePrelaunchTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SceneSessionManagerGamePrelaunchTest::SetUpTestCase() {}

void SceneSessionManagerGamePrelaunchTest::TearDownTestCase() {}

void SceneSessionManagerGamePrelaunchTest::SetUp() {}

void SceneSessionManagerGamePrelaunchTest::TearDown() {}

namespace {
/**
 * @tc.name: PendingSessionToBackgroundForDelegatorWithGamePrelaunch01
 * @tc.desc: Test PendingSessionToBackgroundForDelegator with GAME_PRELAUNCH_BACKGROUND reason
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerGamePrelaunchTest, PendingSessionToBackgroundForDelegatorWithGamePrelaunch01, TestSize.Level0)
{
    auto mockStub = sptr<MockSceneSessionManagerStub>::MakeSptr();
    sptr<IRemoteObject> token = new (std::nothrow) IRemoteObject();
    
    EXPECT_CALL(*mockStub, PendingSessionToBackgroundForDelegator(_, _, 
        static_cast<int32_t>(LifeCycleChangeReason::GAME_PRELAUNCH_BACKGROUND)))
        .Times(1)
        .WillOnce(Return(WSError::WS_OK));
    
    auto ret = mockStub->PendingSessionToBackgroundForDelegator(token, true, 
        static_cast<int32_t>(LifeCycleChangeReason::GAME_PRELAUNCH_BACKGROUND));
    
    EXPECT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: PendingSessionToBackgroundForDelegatorWithDefaultReason
 * @tc.desc: Test PendingSessionToBackgroundForDe与其他reason
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerGamePrelaunchTest, PendingSessionToBackgroundForDelegatorWithDefaultReason, TestSize.Level0)
{
    auto mockStub = sptr<MockSceneSessionManagerStub>::MakeSptr();
    sptr<IRemoteObject> token = new (std::nothrow) IRemoteObject();
    
    EXPECT_CALL(*mockStub, PendingSessionToBackgroundForDelegator(_, _, 
        static_cast<int32_t>(LifeCycleChangeReason::DEFAULT)))
        .Times(1)
        .WillOnce(Return(WSError::WS_OK));
    
    auto ret = mockStub->PendingSessionToBackgroundForDelegator(token, true, 
        static_cast<int32_t>(LifeCycleChangeReason::DEFAULT));
    
    EXPECT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: SessionInfoWithGamePrelaunch01
 * @tc.desc: Test SessionInfo with isGamePrelaunch_ set to true
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerGamePrelaunchTest, SessionInfoWithGamePrelaunch01, TestSize.Level0)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "TestBundle";
    sessionInfo.abilityName_ = "TestAbility";
    sessionInfo.isGamePrelaunch_ = true;
    
    EXPECT_TRUE(sessionInfo.isGamePrelaunch_);
    EXPECT_EQ(sessionInfo.bundleName_, "TestBundle");
    EXPECT_EQ(sessionInfo.abilityName_, "TestAbility");
}

/**
 * @tc.name: SessionInfoWithGamePrelaunch02
 * @tc.desc: Test SessionInfo with isGamePrelaunch_ default value
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerGamePrelaunchTest, SessionInfoWithGamePrelaunch02, TestSize.Level0)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "TestBundle";
    sessionInfo.abilityName_ = "TestAbility";
    
    EXPECT_FALSE(sessionInfo.isGamePrelaunch_);
}

/**
 * @tc.name: SessionInfoCopyWithGamePrelaunch
 * @tc.desc: Test SessionInfo copy constructor with isGamePrelaunch_
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerGamePrelaunchTest, SessionInfoCopyWithGamePrelaunch, TestSize.Level0)
{
    SessionInfo originalInfo;
    originalInfo.bundleName_ = "TestBundle";
    originalInfo.abilityName_ = "TestAbility";
    originalInfo.isGamePrelaunch_ = true;
    
    SessionInfo copiedInfo = originalInfo;
    
    EXPECT_TRUE(copiedInfo.isGamePrelaunch_);
    EXPECT_EQ(copiedInfo.bundleName_, originalInfo.bundleName_);
    EXPECT_EQ(copiedInfo.abilityName_, originalInfo.abilityName_);
}

/**
 * @tc.name: SessionInfoSetGamePrelaunch
 * @tc.desc: Test setting isGamePrelaunch_ to different values
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerGamePrelaunchTest, SessionInfoSetGamePrelaunch, TestSize.Level0)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "TestBundle";
    sessionInfo.abilityName_ = "TestAbility";
    
    sessionInfo.isGamePrelaunch_ = true;
    EXPECT_TRUE(sessionInfo.isGamePrelaunch_);
    
    sessionInfo.isGamePrelaunch_ = false;
    EXPECT_FALSE(sessionInfo.isGamePrelaunch_);
    
    sessionInfo.isGamePrelaunch_ = true;
    EXPECT_TRUE(sessionInfo.isGamePrelaunch_);
}

/**
 * @tc.name: LifeCycleChangeReasonEnumValues
 * @tc.desc: Test LifeCycleChangeReason enum values
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerGamePrelaunchTest, LifeCycleChangeReasonEnumValues, TestSize.Level0)
{
    EXPECT_EQ(static_cast<int32_t>(LifeCycleChangeReason::DEFAULT), 0);
    
    int32_t quickBatchBackgroundValue = static_cast<int32_t>(LifeCycleChangeReason::QUICK_BATCH_BACKGROUND);
    int32_t gamePrelaunchBackgroundValue = static_cast<int32_t>(LifeCycleChangeReason::GAME_PRELAUNCH_BACKGROUND);
    
    EXPECT_EQ(gamePrelaunchBackgroundValue, quickBatchBackgroundValue + 1);
}

/**
 * @tc.name: SessionPendingSessionToBackgroundForDelegator01
 * @tc.desc: Test Session::PendingSessionToBackgroundForDelegator with GAME_PRELAUNCH_BACKGROUND
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerGamePrelaunchTest, SessionPendingSessionToBackgroundForDelegator01, TestSize.Level0)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "TestBundle";
    sessionInfo.abilityName_ = "TestAbility";
    sessionInfo.isGamePrelaunch_ = true;
    
    auto session = sptr<Session>::MakeSptr(sessionInfo);
    
    bool callbackCalled = false;
    bool callbackReason = false;
    
    session->SetPendingSessionToBackgroundForDelegatorListener(
        [&callbackCalled, &callbackReason](const SessionInfo& info, bool shouldBackToCaller, LifeCycleChangeReason reason) {
            callbackCalled = true;
            callbackReason = (reason == LifeCycleChangeReason::GAME_PRELAUNCH_BACKGROUND);
        });
    
    auto ret = session->PendingSessionToBackgroundForDelegator(true, LifeCycleChangeReason::GAME_PRELAUNCH_BACKGROUND);
    
    EXPECT_EQ(ret, WSError::WS_OK);
    EXPECT_TRUE(callbackCalled);
    EXPECT_TRUE(callbackReason);
}

/**
 * @tc.name: SessionPendingSessionToBackgroundForDelegator02
 * @tc.desc: Test Session::PendingSessionToBackgroundForDelegator with DEFAULT reason
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerGamePrelaunchTest, SessionPendingSessionToBackgroundForDelegator02, TestSize.Level0)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "TestBundle";
    sessionInfo.abilityName_ = "TestAbility";
    sessionInfo.isGamePrelaunch_ = false;
    
    auto session = sptr<Session>::MakeSptr(sessionInfo);
    
    bool callbackCalled = false;
    bool callbackReason = false;
    
    session->SetPendingSessionToBackgroundForDelegatorListener(
        [&callbackCalled, &callbackReason](const SessionInfo& info, bool shouldBackToCaller, LifeCycleChangeReason reason) {
            callbackCalled = true;
            callbackReason = (reason == LifeCycleChangeReason::DEFAULT);
        });
    
    auto ret = session->PendingSessionToBackgroundForDelegator(true, LifeCycleChangeReason::DEFAULT);
    
    EXPECT_EQ(ret, WSError::WS_OK);
    EXPECT_TRUE(callbackCalled);
    EXPECT_TRUE(callbackReason);
}

/**
 * @tc.name: SessionSetSessionInfoWithGamePrelaunch
 * @tc.desc: Test Session::SetSessionInfo with isGamePrelaunch_
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerGamePrelaunchTest, SessionSetSessionInfoWithGamePrelaunch, TestSize.Level0)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "TestBundle";
    sessionInfo.abilityName_ = "TestAbility";
    sessionInfo.isGamePrelaunch_ = true;
    
    auto session = sptr<Session>::MakeSptr();
    session->SetSessionInfo(sessionInfo);
    
    auto retrievedInfo = session->GetSessionInfo();
    EXPECT_TRUE(retrievedInfo.isGamePrelaunch_);
    EXPECT_EQ(retrievedInfo.bundleName_, "TestBundle");
    EXPECT_EQ(retrievedInfo.abilityName_, "TestAbility");
}

/**
 * @tc.name: MultipleReasonsTest
 * @tc.desc: Test different lifecycle change reasons
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerGamePrelaunchTest, MultipleReasonsTest, TestSize.Level0)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "TestBundle";
    sessionInfo.abilityName_ = "TestAbility";
    
    auto session = sptr<Session>::MakeSptr(sessionInfo);
    
    std::vector<LifeCycleChangeReason> reasons = {
        LifeCycleChangeReason::DEFAULT,
        LifeCycleChangeReason::QUICK_BATCH_BACKGROUND,
        LifeCycleChangeReason::GAME_PRELAUNCH_BACKGROUND
    };
    
    for (auto reason : reasons) {
        bool callbackCalled = false;
        LifeCycleChangeReason capturedReason = LifeCycleChangeReason::DEFAULT;
        
        session->SetPendingSessionToBackgroundForDelegatorListener(
            [&callbackCalled, &capturedReason](const SessionInfo& info, bool shouldBackToCaller, LifeCycleChangeReason r) {
                callbackCalled = true;
                capturedReason = r;
            });
        
        auto ret = session->PendingSessionToBackgroundForDelegator(true, reason);
        
        EXPECT_EQ(ret, WSError::WS_OK);
        EXPECT_TRUE(callbackCalled);
        EXPECT_EQ(capturedReason, reason);
    }
}
}
}
}
