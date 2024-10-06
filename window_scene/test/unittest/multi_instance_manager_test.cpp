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
#include <bundlemgr/launcher_service.h>
#include "session/host/include/multi_instance_manager.h"
#include "session/host/include/scene_session.h"
#include "mock/mock_ibundle_mgr.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    const std::string BUNDLE_NAME = "bundleName";
    const int32_t USER_ID { 100 };
}
class MultiInstanceManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    sptr<SceneSession> GetSceneSession(const std::string& instanceKey = "");
    sptr<IBundleMgrMocker> GetBundleMgr(AppExecFwk::MultiAppModeType modeType, uint32_t maxCount);
};

void MultiInstanceManagerTest::SetUpTestCase()
{
}

void MultiInstanceManagerTest::TearDownTestCase()
{
}

void MultiInstanceManagerTest::SetUp()
{
}

void MultiInstanceManagerTest::TearDown()
{
    sptr<IBundleMgrMocker> bundleMgrMocker = sptr<IBundleMgrMocker>::MakeSptr();
    EXPECT_CALL(*bundleMgrMocker, GetApplicationInfo(_, _, _, _)).WillOnce(Return(false));
    MultiInstanceManager::GetInstance().Init(bundleMgrMocker);
    MultiInstanceManager::GetInstance().SetCurrentUserId(USER_ID);
    MultiInstanceManager::GetInstance().RefreshAppInfo(BUNDLE_NAME);
}

sptr<SceneSession> MultiInstanceManagerTest::GetSceneSession(const std::string& instanceKey)
{
    SessionInfo info;
    info.bundleName_ = BUNDLE_NAME;
    info.appInstanceKey_ = instanceKey;
    info.isNewAppInstance_ = true;
    sptr<SceneSession::SpecificSessionCallback> specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCb);
    return sceneSession;
}

sptr<IBundleMgrMocker> MultiInstanceManagerTest::GetBundleMgr(AppExecFwk::MultiAppModeType modeType, uint32_t maxCount)
{
    sptr<IBundleMgrMocker> bundleMgrMocker = sptr<IBundleMgrMocker>::MakeSptr();
    EXPECT_CALL(*bundleMgrMocker, GetApplicationInfo(_, _, _, _)).WillOnce([modeType, maxCount](
        const std::string& appName, const AppExecFwk::ApplicationFlag flag,
        const int32_t userId, AppExecFwk::ApplicationInfo& appInfo) {
        appInfo.multiAppMode.multiAppModeType = modeType;
        appInfo.multiAppMode.maxCount = maxCount;
        return true;
    });
    return bundleMgrMocker;
}

namespace {
/**
 * @tc.name: IsMultiInstance
 * @tc.desc: test function : IsMultiInstance
 * @tc.type: FUNC
 */
HWTEST_F(MultiInstanceManagerTest, IsMultiInstance, Function | SmallTest | Level1)
{
    AppExecFwk::MultiAppModeType modeType = AppExecFwk::MultiAppModeType::MULTI_INSTANCE;
    uint32_t maxCount = 5;
    MultiInstanceManager::GetInstance().Init(GetBundleMgr(modeType, maxCount));
    MultiInstanceManager::GetInstance().SetCurrentUserId(USER_ID);
    ASSERT_TRUE(MultiInstanceManager::GetInstance().IsMultiInstance(BUNDLE_NAME));
    ASSERT_FALSE(MultiInstanceManager::GetInstance().IsMultiInstance(nullptr));
}

/**
 * @tc.name: IsMultiInstance02
 * @tc.desc: test function : IsMultiInstance
 * @tc.type: FUNC
 */
HWTEST_F(MultiInstanceManagerTest, IsMultiInstance02, Function | SmallTest | Level1)
{
    AppExecFwk::MultiAppModeType modeType = AppExecFwk::MultiAppModeType::UNSPECIFIED;
    uint32_t maxCount = 5;
    MultiInstanceManager::GetInstance().Init(GetBundleMgr(modeType, maxCount));
    MultiInstanceManager::GetInstance().SetCurrentUserId(USER_ID);
    ASSERT_FALSE(MultiInstanceManager::GetInstance().IsMultiInstance(BUNDLE_NAME));
}

/**
 * @tc.name: IsValidInstanceKey
 * @tc.desc: test function : IsValidInstanceKey
 * @tc.type: FUNC
 */
HWTEST_F(MultiInstanceManagerTest, IsValidInstanceKey, Function | SmallTest | Level1)
{
    AppExecFwk::MultiAppModeType modeType = AppExecFwk::MultiAppModeType::MULTI_INSTANCE;
    uint32_t maxCount = 5;
    MultiInstanceManager::GetInstance().Init(GetBundleMgr(modeType, maxCount));
    MultiInstanceManager::GetInstance().SetCurrentUserId(USER_ID);
    std::string validInstanceKey = "app_instance_0";
    std::string validInstanceKey2 = "app_instance_4";
    ASSERT_TRUE(MultiInstanceManager::GetInstance().IsValidInstanceKey(BUNDLE_NAME, validInstanceKey));
    ASSERT_TRUE(MultiInstanceManager::GetInstance().IsValidInstanceKey(BUNDLE_NAME, validInstanceKey2));

    std::string invalidInstanceKey = "app_instance0";
    std::string invalidInstanceKey2 = "app_instance_-1";
    std::string invalidInstanceKey3 = "app_instance_5";
    std::string invalidInstanceKey4 = "app_instance_x";
    ASSERT_FALSE(MultiInstanceManager::GetInstance().IsValidInstanceKey(BUNDLE_NAME, invalidInstanceKey));
    ASSERT_FALSE(MultiInstanceManager::GetInstance().IsValidInstanceKey(BUNDLE_NAME, invalidInstanceKey2));
    ASSERT_FALSE(MultiInstanceManager::GetInstance().IsValidInstanceKey(BUNDLE_NAME, invalidInstanceKey3));
    ASSERT_FALSE(MultiInstanceManager::GetInstance().IsValidInstanceKey(BUNDLE_NAME, invalidInstanceKey4));
}

/**
 * @tc.name: FillInstanceKeyIfNeed
 * @tc.desc: test function : FillInstanceKeyIfNeed
 * @tc.type: FUNC
 */
HWTEST_F(MultiInstanceManagerTest, FillInstanceKeyIfNeed, Function | SmallTest | Level1)
{
    AppExecFwk::MultiAppModeType modeType = AppExecFwk::MultiAppModeType::MULTI_INSTANCE;
    uint32_t maxCount = 5;
    MultiInstanceManager::GetInstance().Init(GetBundleMgr(modeType, maxCount));
    MultiInstanceManager::GetInstance().SetCurrentUserId(USER_ID);
    sptr<SceneSession> sceneSession = GetSceneSession();
    MultiInstanceManager::GetInstance().FillInstanceKeyIfNeed(sceneSession);
    MultiInstanceManager::GetInstance().IncreaseInstanceKeyRefCount(sceneSession);
    ASSERT_EQ(MultiInstanceManager::GetInstance().GetLastInstanceKey(BUNDLE_NAME), "app_instance_0");
    MultiInstanceManager::GetInstance().DecreaseInstanceKeyRefCount(sceneSession);
}

/**
 * @tc.name: FillInstanceKeyIfNeed02
 * @tc.desc: test function : FillInstanceKeyIfNeed
 * @tc.type: FUNC
 */
HWTEST_F(MultiInstanceManagerTest, FillInstanceKeyIfNeed02, Function | SmallTest | Level1)
{
    AppExecFwk::MultiAppModeType modeType = AppExecFwk::MultiAppModeType::MULTI_INSTANCE;
    uint32_t maxCount = 5;
    MultiInstanceManager::GetInstance().Init(GetBundleMgr(modeType, maxCount));
    MultiInstanceManager::GetInstance().SetCurrentUserId(USER_ID);
    std::string instanceKey = "app_instance_4";
    sptr<SceneSession> sceneSession = GetSceneSession(instanceKey);
    MultiInstanceManager::GetInstance().FillInstanceKeyIfNeed(sceneSession);
    MultiInstanceManager::GetInstance().IncreaseInstanceKeyRefCount(sceneSession);
    ASSERT_EQ(MultiInstanceManager::GetInstance().GetLastInstanceKey(BUNDLE_NAME), instanceKey);
    MultiInstanceManager::GetInstance().DecreaseInstanceKeyRefCount(sceneSession);
}

/**
 * @tc.name: FillInstanceKeyIfNeed03
 * @tc.desc: test function : FillInstanceKeyIfNeed
 * @tc.type: FUNC
 */
HWTEST_F(MultiInstanceManagerTest, FillInstanceKeyIfNeed03, Function | SmallTest | Level1)
{
    AppExecFwk::MultiAppModeType modeType = AppExecFwk::MultiAppModeType::MULTI_INSTANCE;
    uint32_t maxCount = 2;
    MultiInstanceManager::GetInstance().Init(GetBundleMgr(modeType, maxCount));
    MultiInstanceManager::GetInstance().SetCurrentUserId(USER_ID);
    sptr<SceneSession> sceneSession = GetSceneSession();
    MultiInstanceManager::GetInstance().FillInstanceKeyIfNeed(sceneSession);
    MultiInstanceManager::GetInstance().IncreaseInstanceKeyRefCount(sceneSession);
    ASSERT_EQ(MultiInstanceManager::GetInstance().GetLastInstanceKey(BUNDLE_NAME), "app_instance_0");
    sptr<SceneSession> sceneSession1 = GetSceneSession();
    MultiInstanceManager::GetInstance().FillInstanceKeyIfNeed(sceneSession1);
    MultiInstanceManager::GetInstance().IncreaseInstanceKeyRefCount(sceneSession1);
    ASSERT_EQ(MultiInstanceManager::GetInstance().GetLastInstanceKey(BUNDLE_NAME), "app_instance_1");
    sptr<SceneSession> sceneSession2 = GetSceneSession();
    MultiInstanceManager::GetInstance().FillInstanceKeyIfNeed(sceneSession2);
    MultiInstanceManager::GetInstance().IncreaseInstanceKeyRefCount(sceneSession2);
    ASSERT_EQ(MultiInstanceManager::GetInstance().GetLastInstanceKey(BUNDLE_NAME), "app_instance_1");
    MultiInstanceManager::GetInstance().DecreaseInstanceKeyRefCount(sceneSession);
    MultiInstanceManager::GetInstance().DecreaseInstanceKeyRefCount(sceneSession1);
    MultiInstanceManager::GetInstance().DecreaseInstanceKeyRefCount(sceneSession2);
}

/**
 * @tc.name: MultiInstancePendingSessionActivation
 * @tc.desc: test function : MultiInstancePendingSessionActivation
 * @tc.type: FUNC
 */
HWTEST_F(MultiInstanceManagerTest, MultiInstancePendingSessionActivation, Function | SmallTest | Level1)
{
    AppExecFwk::MultiAppModeType modeType = AppExecFwk::MultiAppModeType::UNSPECIFIED;
    uint32_t maxCount = 0;
    MultiInstanceManager::GetInstance().Init(GetBundleMgr(modeType, maxCount));
    MultiInstanceManager::GetInstance().SetCurrentUserId(USER_ID);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = BUNDLE_NAME;
    ASSERT_TRUE(MultiInstanceManager::GetInstance().MultiInstancePendingSessionActivation(sessionInfo));
}

/**
 * @tc.name: MultiInstancePendingSessionActivation02
 * @tc.desc: test function : MultiInstancePendingSessionActivation
 * @tc.type: FUNC
 */
HWTEST_F(MultiInstanceManagerTest, MultiInstancePendingSessionActivation02, Function | SmallTest | Level1)
{
    AppExecFwk::MultiAppModeType modeType = AppExecFwk::MultiAppModeType::MULTI_INSTANCE;
    uint32_t maxCount = 5;
    MultiInstanceManager::GetInstance().Init(GetBundleMgr(modeType, maxCount));
    MultiInstanceManager::GetInstance().SetCurrentUserId(USER_ID);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = BUNDLE_NAME;
    sessionInfo.persistentId_ = 100;
    ASSERT_FALSE(MultiInstanceManager::GetInstance().MultiInstancePendingSessionActivation(sessionInfo));
}

/**
 * @tc.name: MultiInstancePendingSessionActivation03
 * @tc.desc: test function : MultiInstancePendingSessionActivation
 * @tc.type: FUNC
 */
HWTEST_F(MultiInstanceManagerTest, MultiInstancePendingSessionActivation03, Function | SmallTest | Level1)
{
    AppExecFwk::MultiAppModeType modeType = AppExecFwk::MultiAppModeType::MULTI_INSTANCE;
    uint32_t maxCount = 5;
    MultiInstanceManager::GetInstance().Init(GetBundleMgr(modeType, maxCount));
    MultiInstanceManager::GetInstance().SetCurrentUserId(USER_ID);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = BUNDLE_NAME;
    sessionInfo.appInstanceKey_ = "app_instance_xx";
    ASSERT_FALSE(MultiInstanceManager::GetInstance().MultiInstancePendingSessionActivation(sessionInfo));
}

/**
 * @tc.name: GetMaxInstanceCount
 * @tc.desc: test function : GetMaxInstanceCount
 * @tc.type: FUNC
 */
HWTEST_F(MultiInstanceManagerTest, GetMaxInstanceCount, Function | SmallTest | Level1)
{
    AppExecFwk::MultiAppModeType modeType = AppExecFwk::MultiAppModeType::UNSPECIFIED;
    uint32_t maxCount = 5;
    MultiInstanceManager::GetInstance().Init(GetBundleMgr(modeType, maxCount));
    MultiInstanceManager::GetInstance().SetCurrentUserId(USER_ID);
    ASSERT_EQ(MultiInstanceManager::GetInstance().GetMaxInstanceCount(BUNDLE_NAME), 0);
}

/**
 * @tc.name: GetMaxInstanceCount02
 * @tc.desc: test function : GetMaxInstanceCount
 * @tc.type: FUNC
 */
HWTEST_F(MultiInstanceManagerTest, GetMaxInstanceCount02, Function | SmallTest | Level1)
{
    AppExecFwk::MultiAppModeType modeType = AppExecFwk::MultiAppModeType::MULTI_INSTANCE;
    uint32_t maxCount = 5;
    MultiInstanceManager::GetInstance().Init(GetBundleMgr(modeType, maxCount));
    MultiInstanceManager::GetInstance().SetCurrentUserId(USER_ID);
    ASSERT_EQ(MultiInstanceManager::GetInstance().GetMaxInstanceCount(BUNDLE_NAME), maxCount);
}

/**
 * @tc.name: GetInstanceCount
 * @tc.desc: test function : GetInstanceCount
 * @tc.type: FUNC
 */
HWTEST_F(MultiInstanceManagerTest, GetInstanceCount, Function | SmallTest | Level1)
{
    AppExecFwk::MultiAppModeType modeType = AppExecFwk::MultiAppModeType::MULTI_INSTANCE;
    uint32_t maxCount = 5;
    MultiInstanceManager::GetInstance().Init(GetBundleMgr(modeType, maxCount));
    MultiInstanceManager::GetInstance().SetCurrentUserId(USER_ID);
    ASSERT_EQ(MultiInstanceManager::GetInstance().GetInstanceCount(BUNDLE_NAME), 0);
    std::string instanceKey0 = "app_instance_0";
    sptr<SceneSession> sceneSession = GetSceneSession(instanceKey0);
    ASSERT_EQ(MultiInstanceManager::GetInstance().CreateNewInstanceKey(BUNDLE_NAME), instanceKey0);
    MultiInstanceManager::GetInstance().IncreaseInstanceKeyRefCount(sceneSession);
    ASSERT_EQ(MultiInstanceManager::GetInstance().GetInstanceCount(BUNDLE_NAME), 1);
    MultiInstanceManager::GetInstance().DecreaseInstanceKeyRefCount(sceneSession);
    ASSERT_EQ(MultiInstanceManager::GetInstance().GetInstanceCount(BUNDLE_NAME), 0);
}


/**
 * @tc.name: GetLastInstanceKey
 * @tc.desc: test function : GetLastInstanceKey
 * @tc.type: FUNC
 */
HWTEST_F(MultiInstanceManagerTest, GetLastInstanceKey, Function | SmallTest | Level1)
{
    AppExecFwk::MultiAppModeType modeType = AppExecFwk::MultiAppModeType::MULTI_INSTANCE;
    uint32_t maxCount = 5;
    MultiInstanceManager::GetInstance().Init(GetBundleMgr(modeType, maxCount));
    MultiInstanceManager::GetInstance().SetCurrentUserId(USER_ID);
    ASSERT_EQ(MultiInstanceManager::GetInstance().GetLastInstanceKey(BUNDLE_NAME), "");
    std::string instanceKey0 = "app_instance_0";
    sptr<SceneSession> sceneSession = GetSceneSession(instanceKey0);
    ASSERT_EQ(MultiInstanceManager::GetInstance().CreateNewInstanceKey(BUNDLE_NAME), instanceKey0);
    MultiInstanceManager::GetInstance().IncreaseInstanceKeyRefCount(sceneSession);
    ASSERT_EQ(MultiInstanceManager::GetInstance().GetLastInstanceKey(BUNDLE_NAME), instanceKey0);
    MultiInstanceManager::GetInstance().DecreaseInstanceKeyRefCount(sceneSession);
    ASSERT_EQ(MultiInstanceManager::GetInstance().GetLastInstanceKey(BUNDLE_NAME), "");
}

/**
 * @tc.name: CreateNewInstanceKey
 * @tc.desc: test function : CreateNewInstanceKey
 * @tc.type: FUNC
 */
HWTEST_F(MultiInstanceManagerTest, CreateNewInstanceKey, Function | SmallTest | Level1)
{
    AppExecFwk::MultiAppModeType modeType = AppExecFwk::MultiAppModeType::MULTI_INSTANCE;
    uint32_t maxCount = 5;
    MultiInstanceManager::GetInstance().Init(GetBundleMgr(modeType, maxCount));
    MultiInstanceManager::GetInstance().SetCurrentUserId(USER_ID);
    std::string instanceKey0 = "app_instance_0";
    ASSERT_EQ(MultiInstanceManager::GetInstance().CreateNewInstanceKey(BUNDLE_NAME), instanceKey0);
    sptr<SceneSession> sceneSession = GetSceneSession(instanceKey0);
    MultiInstanceManager::GetInstance().IncreaseInstanceKeyRefCount(sceneSession);
    std::string instanceKey1 = "app_instance_1";
    ASSERT_EQ(MultiInstanceManager::GetInstance().CreateNewInstanceKey(BUNDLE_NAME), instanceKey1);
    sptr<SceneSession> sceneSession1 = GetSceneSession(instanceKey1);
    MultiInstanceManager::GetInstance().IncreaseInstanceKeyRefCount(sceneSession1);
    std::string instanceKey2 = "app_instance_2";
    ASSERT_EQ(MultiInstanceManager::GetInstance().CreateNewInstanceKey(BUNDLE_NAME), instanceKey2);
    sptr<SceneSession> sceneSession2 = GetSceneSession(instanceKey2);
    MultiInstanceManager::GetInstance().IncreaseInstanceKeyRefCount(sceneSession2);
    std::string instanceKey3 = "app_instance_3";
    ASSERT_EQ(MultiInstanceManager::GetInstance().CreateNewInstanceKey(BUNDLE_NAME), instanceKey3);
    sptr<SceneSession> sceneSession3 = GetSceneSession(instanceKey3);
    MultiInstanceManager::GetInstance().IncreaseInstanceKeyRefCount(sceneSession3);
    std::string instanceKey4 = "app_instance_4";
    ASSERT_EQ(MultiInstanceManager::GetInstance().CreateNewInstanceKey(BUNDLE_NAME), instanceKey4);
    sptr<SceneSession> sceneSession4 = GetSceneSession(instanceKey4);
    MultiInstanceManager::GetInstance().IncreaseInstanceKeyRefCount(sceneSession4);
    MultiInstanceManager::GetInstance().DecreaseInstanceKeyRefCount(sceneSession);
    MultiInstanceManager::GetInstance().DecreaseInstanceKeyRefCount(sceneSession1);
    MultiInstanceManager::GetInstance().DecreaseInstanceKeyRefCount(sceneSession2);
    MultiInstanceManager::GetInstance().DecreaseInstanceKeyRefCount(sceneSession3);
    MultiInstanceManager::GetInstance().DecreaseInstanceKeyRefCount(sceneSession4);
}

/**
 * @tc.name: IsInstanceKeyExist
 * @tc.desc: test function : IsInstanceKeyExist
 * @tc.type: FUNC
 */
HWTEST_F(MultiInstanceManagerTest, IsInstanceKeyExist, Function | SmallTest | Level1)
{
    AppExecFwk::MultiAppModeType modeType = AppExecFwk::MultiAppModeType::MULTI_INSTANCE;
    uint32_t maxCount = 5;
    MultiInstanceManager::GetInstance().Init(GetBundleMgr(modeType, maxCount));
    MultiInstanceManager::GetInstance().SetCurrentUserId(USER_ID);
    std::string instanceKey0 = "app_instance_0";
    std::string instanceKey3 = "app_instance_3";
    ASSERT_FALSE(MultiInstanceManager::GetInstance().IsInstanceKeyExist(BUNDLE_NAME, instanceKey0));
    ASSERT_EQ(MultiInstanceManager::GetInstance().CreateNewInstanceKey(BUNDLE_NAME), instanceKey0);
    sptr<SceneSession> sceneSession = GetSceneSession(instanceKey0);
    MultiInstanceManager::GetInstance().IncreaseInstanceKeyRefCount(sceneSession);
    ASSERT_TRUE(MultiInstanceManager::GetInstance().IsInstanceKeyExist(BUNDLE_NAME, instanceKey0));
    ASSERT_FALSE(MultiInstanceManager::GetInstance().IsInstanceKeyExist(BUNDLE_NAME, instanceKey3));
    ASSERT_EQ(MultiInstanceManager::GetInstance().CreateNewInstanceKey(BUNDLE_NAME, instanceKey3), instanceKey3);
    sptr<SceneSession> sceneSession3 = GetSceneSession(instanceKey3);
    MultiInstanceManager::GetInstance().IncreaseInstanceKeyRefCount(sceneSession3);
    ASSERT_TRUE(MultiInstanceManager::GetInstance().IsInstanceKeyExist(BUNDLE_NAME, instanceKey3));
    MultiInstanceManager::GetInstance().DecreaseInstanceKeyRefCount(sceneSession);
    MultiInstanceManager::GetInstance().DecreaseInstanceKeyRefCount(sceneSession3);
}
}
}
}