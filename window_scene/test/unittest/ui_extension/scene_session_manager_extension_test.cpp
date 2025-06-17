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

#include "interfaces/include/ws_common.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "session_manager.h"
#include "test/mock/mock_session_stage.h"
#include <ability_manager_client.h>

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

// sptr<SceneSession> SceneSessionManagerExtensionTest::GetSceneSession(const std::string& instanceKey)
// {
//     SessionInfo info;
//     info.bundleName_ = BUNDLE_NAME;
//     info.appInstanceKey_ = instanceKey;
//     info.isNewAppInstance_ = true;
//     sptr<SceneSession::SpecificSessionCallback> specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
//     sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCb);
//     return sceneSession;
// }

// void SceneSessionManagerExtensionTest::Init(AppExecFwk::MultiAppModeType modeType, uint32_t maxCount)
// {
//     sptr<IBundleMgrMocker> bundleMgrMocker = sptr<IBundleMgrMocker>::MakeSptr();
//     EXPECT_CALL(*bundleMgrMocker, GetApplicationInfos(_, _, _))
//         .WillOnce([modeType, maxCount](const AppExecFwk::ApplicationFlag flag,
//                                        const int32_t userId,
//                                        std::vector<AppExecFwk::ApplicationInfo>& appInfos) {
//             AppExecFwk::ApplicationInfo appInfo;
//             appInfo.bundleName = BUNDLE_NAME;
//             appInfo.multiAppMode.multiAppModeType = modeType;
//             appInfo.multiAppMode.maxCount = maxCount;
//             appInfos.push_back(appInfo);
//             return true;
//         });
//     MultiInstanceManager::GetInstance().Init(bundleMgrMocker, GetTaskScheduler());
//     MultiInstanceManager::GetInstance().SetCurrentUserId(USER_ID);
//     usleep(SLEEP_TIME);
// }

// std::shared_ptr<TaskScheduler> SceneSessionManagerExtensionTest::GetTaskScheduler()
// {
//     std::string threadName = "threadName";
//     std::shared_ptr<TaskScheduler> taskScheduler = std::make_shared<TaskScheduler>(threadName);
//     return taskScheduler;
// }

namespace {
/**
 * @tc.name: CheckSubSessionStartedByExtensionAndSetDisplayId
 * @tc.desc: normal test
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest5, CheckSubSessionStartedByExtensionAndSetDisplayId01, TestSize.Level1)
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
HWTEST_F(SceneSessionManagerTest5, CheckSubSessionStartedByExtensionAndSetDisplayId02, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.bundleName = BUNDLE_NAME;
    sptr<SceneSession::SpecificSessionCallback> callback = ssm_->CreateSpecificSessionCallback();
    sptr<SceneSession> parentSession = sptr<SceneSession>::MakeSptr(info, callback);
    ssm_->sceneSessionMap_.insert({ parentSession->GetPersistentId(), parentSession });
    sptr<IRemoteObject> token;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    sptr<ISessionStage> sessionStage = sptr<SessionStageMocker>::MakeSptr();
    property->SetParentPersistentId(parentSession->GetPersistentId());
    property->SetIsUIExtFirstSubWindow(true);
    property->SetIsUIExtensionAbilityProcess(true);
    EXPECT_EQ(ssm_->CheckSubSessionStartedByExtensionAndSetDisplayId(token, property, sessionStage), WSError::WS_OK);

    AAFwk::UIExtensionSessionInfo extensionSessionInfo;
    AAFwk::UIExtensionHostInfo hostInfo;
    hostInfo.element_SetBundleName(BUNDLE_NAME);
    extensionSessionInfo.hostElementName.GetBundleName(BUNDLE_NAME);
    EXPECT_EQ(ssm_->CheckSubSessionStartedByExtensionAndSetDisplayId(token, property, sessionStage), WSError::WS_OK);

    hostInfo.element_SetBundleName(BUNDLE_NAME_TEST);
    EXPECT_EQ(ssm_->CheckSubSessionStartedByExtensionAndSetDisplayId(token, property, sessionStage), WSError::WS_OK);
    extensionSessionInfo.hostElementName.GetBundleName(BUNDLE_NAME_TEST);
    EXPECT_EQ(ssm_->CheckSubSessionStartedByExtensionAndSetDisplayId(token, property, sessionStage),
        WSError::WS_ERROR_INVALID_WINDOW);
}
} // namespace
} // namespace Rosen
} // namespace OHOS