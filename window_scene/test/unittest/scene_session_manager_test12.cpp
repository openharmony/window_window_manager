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
}
} // namespace Rosen
} // namespace OHOS
