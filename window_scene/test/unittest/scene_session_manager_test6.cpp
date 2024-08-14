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

#include <bundle_mgr_interface.h>
#include <bundlemgr/launcher_service.h>
#include <gtest/gtest.h>
#include <regex>
#include "context.h"
#include "interfaces/include/ws_common.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/main_session.h"
#include "session_info.h"
#include "session_manager.h"
#include "session_manager/include/scene_session_manager.h"
#include "window_manager_agent.h"
#include "zidl/window_manager_agent_interface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
const std::string EMPTY_DEVICE_ID = "";
using ConfigItem = WindowSceneConfig::ConfigItem;
}
class SceneSessionManagerTest6 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static bool gestureNavigationEnabled_;
    static ProcessGestureNavigationEnabledChangeFunc callbackFunc_;
    static sptr<SceneSessionManager> ssm_;
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

sptr<SceneSessionManager> SceneSessionManagerTest6::ssm_ = nullptr;

bool SceneSessionManagerTest6::gestureNavigationEnabled_ = true;
ProcessGestureNavigationEnabledChangeFunc SceneSessionManagerTest6::callbackFunc_ = [](bool enable) {
    gestureNavigationEnabled_ = enable;
};

void WindowChangedFuncTest(int32_t persistentId, WindowUpdateType type)
{
}

void ProcessStatusBarEnabledChangeFuncTest(bool enable)
{
}

void DumpRootSceneElementInfoFuncTest(const std::vector<std::string>& params, std::vector<std::string>& infos)
{
}

void SceneSessionManagerTest6::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerTest6::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerTest6::SetUp()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerTest6::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    ssm_->sceneSessionMap_.clear();
}

namespace {
/**
 * @tc.name: MissionChanged
 * @tc.desc: MissionChanged
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, MissionChanged, Function | SmallTest | Level3)
{
    sptr<SceneSession> prevSession = nullptr;
    sptr<SceneSession> currSession = nullptr;
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->MissionChanged(prevSession, currSession);
    EXPECT_EQ(false, ret);
    SessionInfo sessionInfoFirst;
    sessionInfoFirst.bundleName_ = "privacy.test.first";
    sessionInfoFirst.abilityName_ = "privacyAbilityName";
    prevSession = sptr<SceneSession>::MakeSptr(sessionInfoFirst, nullptr);
    ASSERT_NE(nullptr, prevSession);
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->MissionChanged(prevSession, currSession);
    EXPECT_EQ(true, ret);
    SessionInfo sessionInfoSecond;
    sessionInfoSecond.bundleName_ = "privacy.test.second";
    sessionInfoSecond.abilityName_ = "privacyAbilityName";
    currSession= sptr<SceneSession>::MakeSptr(sessionInfoSecond, nullptr);
    ASSERT_NE(nullptr, currSession);
    prevSession->persistentId_ = 0;
    currSession->persistentId_ = 0;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->MissionChanged(prevSession, currSession);
    EXPECT_EQ(false, ret);
    prevSession = nullptr;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->MissionChanged(prevSession, currSession);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: GetWindowLayerChangeInfo
 * @tc.desc: GetWindowLayerChangeInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, GetWindowLayerChangeInfo, Function | SmallTest | Level3)
{
    std::vector<std::pair<uint64_t, WindowVisibilityState>> currVisibleData;
    currVisibleData.push_back(std::make_pair(0, WindowVisibilityState::WINDOW_VISIBILITY_STATE_NO_OCCLUSION));
    currVisibleData.push_back(std::make_pair(1, WindowVisibilityState::WINDOW_VISIBILITY_STATE_PARTICALLY_OCCLUSION));
    currVisibleData.push_back(std::make_pair(2, WindowVisibilityState::WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION));
    currVisibleData.push_back(std::make_pair(3, WindowVisibilityState::WINDOW_LAYER_STATE_MAX));
    std::vector<std::pair<uint64_t, bool>> currDrawingContentData;
    currDrawingContentData.push_back(std::make_pair(0, true));
    currDrawingContentData.push_back(std::make_pair(1, false));
    VisibleData visibleData;
    visibleData.push_back(std::make_pair(0, WINDOW_LAYER_INFO_TYPE::ALL_VISIBLE));
    visibleData.push_back(std::make_pair(1, WINDOW_LAYER_INFO_TYPE::SEMI_VISIBLE));
    visibleData.push_back(std::make_pair(2, WINDOW_LAYER_INFO_TYPE::INVISIBLE));
    visibleData.push_back(std::make_pair(3, WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_DYNAMIC_STATUS));
    visibleData.push_back(std::make_pair(4, WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_STATIC_STATUS));
    visibleData.push_back(std::make_pair(5, WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_UNKNOWN_TYPE));
    std::shared_ptr<RSOcclusionData> occlusionDataPtr = std::make_shared<RSOcclusionData>(visibleData);
    ASSERT_NE(nullptr, occlusionDataPtr);
    ASSERT_NE(nullptr, ssm_);
    ssm_->GetWindowLayerChangeInfo(occlusionDataPtr, currVisibleData, currDrawingContentData);
}

/**
 * @tc.name: GetWindowVisibilityChangeInfo01
 * @tc.desc: GetWindowVisibilityChangeInfo01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, GetWindowVisibilityChangeInfo01, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->lastVisibleData_.clear();
    std::vector<std::pair<uint64_t, WindowVisibilityState>> currVisibleData;
    std::vector<std::pair<uint64_t, WindowVisibilityState>> visibilityChangeInfos;
    currVisibleData.push_back(std::make_pair(1, WindowVisibilityState::WINDOW_VISIBILITY_STATE_PARTICALLY_OCCLUSION));
    currVisibleData.push_back(std::make_pair(2, WindowVisibilityState::WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION));
    currVisibleData.push_back(std::make_pair(3, WindowVisibilityState::WINDOW_LAYER_STATE_MAX));
    ssm_->lastVisibleData_.push_back(std::make_pair(0, WindowVisibilityState::WINDOW_VISIBILITY_STATE_NO_OCCLUSION));
    visibilityChangeInfos = ssm_->GetWindowVisibilityChangeInfo(currVisibleData);
    ASSERT_EQ(visibilityChangeInfos.size(), 3);
}

/**
 * @tc.name: GetWindowVisibilityChangeInfo02
 * @tc.desc: GetWindowVisibilityChangeInfo02
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, GetWindowVisibilityChangeInfo02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->lastVisibleData_.clear();
    std::vector<std::pair<uint64_t, WindowVisibilityState>> currVisibleData;
    std::vector<std::pair<uint64_t, WindowVisibilityState>> visibilityChangeInfos;
    currVisibleData.push_back(std::make_pair(0, WindowVisibilityState::WINDOW_VISIBILITY_STATE_PARTICALLY_OCCLUSION));
    ssm_->lastVisibleData_.push_back(std::make_pair(1, WindowVisibilityState::WINDOW_VISIBILITY_STATE_NO_OCCLUSION));
    visibilityChangeInfos = ssm_->GetWindowVisibilityChangeInfo(currVisibleData);
    ASSERT_EQ(visibilityChangeInfos.size(), 2);
}

/**
 * @tc.name: GetWindowVisibilityChangeInfo03
 * @tc.desc: GetWindowVisibilityChangeInfo03
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, GetWindowVisibilityChangeInfo03, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->lastVisibleData_.clear();
    std::vector<std::pair<uint64_t, WindowVisibilityState>> currVisibleData;
    std::vector<std::pair<uint64_t, WindowVisibilityState>> visibilityChangeInfos;
    currVisibleData.push_back(std::make_pair(1, WindowVisibilityState::WINDOW_VISIBILITY_STATE_PARTICALLY_OCCLUSION));
    currVisibleData.push_back(std::make_pair(2, WindowVisibilityState::WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION));
    ssm_->lastVisibleData_.push_back(
        std::make_pair(1, WindowVisibilityState::WINDOW_VISIBILITY_STATE_PARTICALLY_OCCLUSION));
    ssm_->lastVisibleData_.push_back(
        std::make_pair(2, WindowVisibilityState::WINDOW_VISIBILITY_STATE_PARTICALLY_OCCLUSION));
    visibilityChangeInfos = ssm_->GetWindowVisibilityChangeInfo(currVisibleData);
    ASSERT_EQ(visibilityChangeInfos.size(), 1);
}

/**
 * @tc.name: DealwithVisibilityChange01
 * @tc.desc: DealwithVisibilityChange01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, DealwithVisibilityChange01, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sptr<SceneSession> sceneSession2 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(sceneSession1->GetPersistentId(), sceneSession1));
    ssm_->sceneSessionMap_.insert(std::make_pair(sceneSession2->GetPersistentId(), sceneSession2));
    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode1 = RSSurfaceNode::Create(config);
    std::shared_ptr<RSSurfaceNode> surfaceNode2 = RSSurfaceNode::Create(config);
    ASSERT_NE(nullptr, surfaceNode1);
    ASSERT_NE(nullptr, surfaceNode2);
    surfaceNode1->SetId(1);
    surfaceNode2->SetId(2);
    ASSERT_NE(nullptr, sceneSession1);
    ASSERT_NE(nullptr, sceneSession2);
    sceneSession1->SetSessionState(SessionState::STATE_FOREGROUND);
    sceneSession1->surfaceNode_ = surfaceNode1;
    sceneSession1->SetCallingPid(1);
    sceneSession2->SetSessionState(SessionState::STATE_FOREGROUND);
    sceneSession2->SetParentSession(sceneSession1);
    sceneSession2->surfaceNode_ = surfaceNode2;
    sceneSession2->SetCallingPid(2);
    ASSERT_NE(nullptr, sceneSession1->property_);
    ASSERT_NE(nullptr, sceneSession2->property_);
    sceneSession1->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession2->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    sceneSession1->property_->SetWindowName("visibility1");
    sceneSession2->property_->SetWindowName("visibility2");
    std::vector<std::pair<uint64_t, WindowVisibilityState>> visibilityChangeInfos;
    visibilityChangeInfos.push_back(std::make_pair(1, WindowVisibilityState::WINDOW_VISIBILITY_STATE_NO_OCCLUSION));
    ssm_->DealwithVisibilityChange(visibilityChangeInfos);
    ASSERT_EQ(sceneSession1->GetRSVisible(), true);
    ASSERT_EQ(sceneSession2->GetRSVisible(), true);
    sceneSession2->SetSessionState(SessionState::STATE_BACKGROUND);
    sceneSession1->SetRSVisible(false);
    sceneSession2->SetRSVisible(false);
    ssm_->DealwithVisibilityChange(visibilityChangeInfos);
    ASSERT_EQ(sceneSession1->GetRSVisible(), true);
    ASSERT_EQ(sceneSession2->GetRSVisible(), false);
}

/**
 * @tc.name: DealwithVisibilityChange02
 * @tc.desc: DealwithVisibilityChange02
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, DealwithVisibilityChange02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sptr<SceneSession> sceneSession2 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(sceneSession1->GetPersistentId(), sceneSession1));
    ssm_->sceneSessionMap_.insert(std::make_pair(sceneSession2->GetPersistentId(), sceneSession2));
    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode1 = RSSurfaceNode::Create(config);
    std::shared_ptr<RSSurfaceNode> surfaceNode2 = RSSurfaceNode::Create(config);
    ASSERT_NE(nullptr, surfaceNode1);
    ASSERT_NE(nullptr, surfaceNode2);
    surfaceNode1->SetId(1);
    surfaceNode2->SetId(2);
    ASSERT_NE(nullptr, sceneSession1);
    ASSERT_NE(nullptr, sceneSession2);
    sceneSession1->SetSessionState(SessionState::STATE_FOREGROUND);
    sceneSession1->surfaceNode_ = surfaceNode1;
    sceneSession2->SetSessionState(SessionState::STATE_FOREGROUND);
    sceneSession2->SetParentSession(sceneSession1);
    sceneSession2->surfaceNode_ = surfaceNode2;
    ASSERT_NE(nullptr, sceneSession1->property_);
    ASSERT_NE(nullptr, sceneSession2->property_);
    sceneSession1->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession2->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    sceneSession1->property_->SetWindowName("visibility1");
    sceneSession2->property_->SetWindowName("visibility2");
    std::vector<std::pair<uint64_t, WindowVisibilityState>> visibilityChangeInfos;
    visibilityChangeInfos.push_back(std::make_pair(2, WindowVisibilityState::WINDOW_VISIBILITY_STATE_NO_OCCLUSION));
    sceneSession1->SetRSVisible(true);
    ssm_->DealwithVisibilityChange(visibilityChangeInfos);
    ASSERT_EQ(sceneSession2->GetRSVisible(), true);
    sceneSession2->SetSessionState(SessionState::STATE_BACKGROUND);
    sceneSession1->SetRSVisible(false);
    sceneSession2->SetRSVisible(false);
    sceneSession1->SetSessionState(SessionState::STATE_BACKGROUND);
    ssm_->DealwithVisibilityChange(visibilityChangeInfos);
    ASSERT_EQ(sceneSession2->GetRSVisible(), false);
}

/**
 * @tc.name: UpdateWindowMode
 * @tc.desc: UpdateWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, UpdateWindowMode, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession));
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->UpdateWindowMode(0, 0);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_WINDOW, ret);
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->UpdateWindowMode(2, 0);
    EXPECT_EQ(WSError::WS_OK, ret);
}

/**
 * @tc.name: SetScreenLocked && IsScreenLocked
 * @tc.desc: SceneSesionManager update screen locked state
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, IsScreenLocked, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->SetScreenLocked(true);
    ASSERT_NE(nullptr, ssm_);
    EXPECT_TRUE(ssm_->IsScreenLocked());
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessWindowModeType();
    ASSERT_NE(nullptr, ssm_);
    ssm_->SetScreenLocked(false);
    ASSERT_NE(nullptr, ssm_);
    EXPECT_FALSE(ssm_->IsScreenLocked());
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessWindowModeType();
}

/**
 * @tc.name: CheckWindowModeType
 * @tc.desc: CheckWindowModeType
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, CheckWindowModeType, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    auto ret = ssm_->CheckWindowModeType();
    EXPECT_EQ(WindowModeType::WINDOW_MODE_OTHER, ret);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "privacy.test.first";
    sessionInfo.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->CheckWindowModeType();
    EXPECT_EQ(WindowModeType::WINDOW_MODE_OTHER, ret);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->isVisible_ = false;
    sceneSession->state_ = SessionState::STATE_DISCONNECT;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->CheckWindowModeType();
    EXPECT_EQ(WindowModeType::WINDOW_MODE_OTHER, ret);
}

/**
 * @tc.name: CheckWindowModeType01
 * @tc.desc: CheckWindowModeType
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, CheckWindowModeType01, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "privacy.test.first";
    sessionInfo.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sceneSession->isVisible_ = true;
    sceneSession->state_ = SessionState::STATE_ACTIVE;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    SessionInfo sessionInfo1;
    sessionInfo1.bundleName_ = "privacy.test.first";
    sessionInfo1.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo1, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    ASSERT_NE(nullptr, sceneSession1->property_);
    sceneSession1->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    ASSERT_NE(nullptr, sceneSession1->property_);
    sceneSession1->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sceneSession1->isVisible_ = true;
    sceneSession1->state_ = SessionState::STATE_ACTIVE;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession1));
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->CheckWindowModeType();
    EXPECT_EQ(WindowModeType::WINDOW_MODE_SPLIT_FLOATING, ret);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->CheckWindowModeType();
    EXPECT_EQ(WindowModeType::WINDOW_MODE_FULLSCREEN_FLOATING, ret);
}

/**
 * @tc.name: CheckWindowModeType02
 * @tc.desc: CheckWindowModeType
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, CheckWindowModeType02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "privacy.test.first";
    sessionInfo.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sceneSession->isVisible_ = true;
    sceneSession->state_ = SessionState::STATE_ACTIVE;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ASSERT_NE(nullptr, ssm_);
    ssm_->lastWindowModeType_ = WindowModeType::WINDOW_MODE_FULLSCREEN;
    auto ret = ssm_->CheckWindowModeType();
    EXPECT_EQ(WindowModeType::WINDOW_MODE_FLOATING, ret);
    ASSERT_NE(nullptr, ssm_);
    ssm_->NotifyRSSWindowModeTypeUpdate();
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->CheckWindowModeType();
    EXPECT_EQ(WindowModeType::WINDOW_MODE_FULLSCREEN, ret);
    ASSERT_NE(nullptr, ssm_);
    ssm_->NotifyRSSWindowModeTypeUpdate();
}

/**
 * @tc.name: CheckWindowModeType03
 * @tc.desc: CheckWindowModeType
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, CheckWindowModeType03, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "privacy.test.first";
    sessionInfo.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->isVisible_ = true;
    sceneSession->state_ = SessionState::STATE_ACTIVE;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sceneSession->isVisible_ = true;
    sceneSession->state_ = SessionState::STATE_ACTIVE;
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->CheckWindowModeType();
    EXPECT_EQ(WindowModeType::WINDOW_MODE_SPLIT, ret);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->CheckWindowModeType();
    EXPECT_EQ(WindowModeType::WINDOW_MODE_SPLIT, ret);
}

/**
 * @tc.name: GetSceneSessionPrivacyModeBundles
 * @tc.desc: GetSceneSessionPrivacyModeBundles
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, GetSceneSessionPrivacyModeBundles, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    DisplayId displayId = 0;
    std::unordered_set<std::string> privacyBundles;
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundles);
    SessionInfo sessionInfoFirst;
    sessionInfoFirst.bundleName_ = "";
    sessionInfoFirst.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSessionFirst = sptr<SceneSession>::MakeSptr(sessionInfoFirst, nullptr);
    ASSERT_NE(sceneSessionFirst, nullptr);
    sceneSessionFirst->property_ = nullptr;
    ASSERT_NE(nullptr, ssm_);
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundles);
    sceneSessionFirst->property_ = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, sceneSessionFirst->property_);
    sceneSessionFirst->property_->SetDisplayId(0);
    ASSERT_NE(nullptr, ssm_);
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundles);
    sessionInfoFirst.bundleName_ = "privacy.test.first";
    sceneSessionFirst->state_ = SessionState::STATE_FOREGROUND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundles);
    sceneSessionFirst->state_ = SessionState::STATE_CONNECT;
    ASSERT_NE(nullptr, ssm_);
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundles);
}

/**
 * @tc.name: GetSceneSessionPrivacyModeBundles01
 * @tc.desc: GetSceneSessionPrivacyModeBundles
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, GetSceneSessionPrivacyModeBundles01, Function | SmallTest | Level3)
{
    DisplayId displayId = 0;
    std::unordered_set<std::string> privacyBundles;
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundles);
    SessionInfo sessionInfoFirst;
    sessionInfoFirst.bundleName_ = "privacy.test.first";
    sessionInfoFirst.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSessionFirst = sptr<SceneSession>::MakeSptr(sessionInfoFirst, nullptr);
    ASSERT_NE(sceneSessionFirst, nullptr);
    sceneSessionFirst->property_ = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, sceneSessionFirst->property_);
    sceneSessionFirst->property_->SetDisplayId(0);
    sceneSessionFirst->state_ = SessionState::STATE_ACTIVE;
    ASSERT_NE(nullptr, ssm_);
    SessionInfo sessionInfoSecond;
    sessionInfoSecond.bundleName_ = "privacy.test.second";
    sessionInfoSecond.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSessionSecond = sptr<SceneSession>::MakeSptr(sessionInfoSecond, nullptr);
    ASSERT_NE(nullptr, sceneSessionSecond);
    ssm_->sceneSessionMap_.insert({sceneSessionSecond->GetPersistentId(), sceneSessionSecond});
    ASSERT_NE(nullptr, sceneSessionSecond->property_);
    sceneSessionSecond->property_->displayId_ = 1;
    sceneSessionSecond->state_ = SessionState::STATE_ACTIVE;
    sceneSessionSecond->parentSession_ = sceneSessionFirst;
    ASSERT_NE(nullptr, ssm_);
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundles);
    sceneSessionSecond->state_ = SessionState::STATE_FOREGROUND;
    sceneSessionSecond->state_ = SessionState::STATE_CONNECT;
    ASSERT_NE(nullptr, ssm_);
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundles);
}

/**
 * @tc.name: GetSceneSessionPrivacyModeBundles02
 * @tc.desc: GetSceneSessionPrivacyModeBundles
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, GetSceneSessionPrivacyModeBundles02, Function | SmallTest | Level3)
{
    DisplayId displayId = 0;
    std::unordered_set<std::string> privacyBundles;
    ASSERT_NE(nullptr, ssm_);
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundles);
    SessionInfo sessionInfoFirst;
    sessionInfoFirst.bundleName_ = "privacy.test.first";
    sessionInfoFirst.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSessionFirst = sptr<SceneSession>::MakeSptr(sessionInfoFirst, nullptr);
    ASSERT_NE(sceneSessionFirst, nullptr);
    sceneSessionFirst->property_ = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, sceneSessionFirst->property_);
    sceneSessionFirst->property_->SetDisplayId(0);
    sceneSessionFirst->state_ = SessionState::STATE_ACTIVE;
    sceneSessionFirst->property_->isPrivacyMode_ = false;
    ASSERT_NE(nullptr, ssm_);
    ssm_->GetSceneSessionPrivacyModeBundles(displayId, privacyBundles);
}

/**
 * @tc.name: RegisterWindowManagerAgent
 * @tc.desc: RegisterWindowManagerAgent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, RegisterWindowManagerAgent, Function | SmallTest | Level3)
{
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_SYSTEM_BAR;
    sptr<IWindowManagerAgent> windowManagerAgent = nullptr;
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_GESTURE_NAVIGATION_ENABLED;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WATER_MARK_FLAG;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_VISIBILITY;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ret);
    type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_DRAWING_STATE;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ret);
    type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_VISIBLE_WINDOW_NUM;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, ret);
    type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_CAMERA_FLOAT;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: OnSessionStateChange
 * @tc.desc: OnSessionStateChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, OnSessionStateChange, Function | SmallTest | Level3)
{
    SessionState state = SessionState::STATE_FOREGROUND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    ssm_->OnSessionStateChange(1, state);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ASSERT_NE(nullptr, ssm_);
    ssm_->OnSessionStateChange(1, state);
    ssm_->focusedSessionId_ = 1;
    ssm_->OnSessionStateChange(1, state);
    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_NE(nullptr, ssm_);
    ssm_->needBlockNotifyFocusStatusUntilForeground_ = true;
    ssm_->OnSessionStateChange(1, state);
    ASSERT_NE(nullptr, ssm_);
    ssm_->needBlockNotifyFocusStatusUntilForeground_ = false;
    ssm_->OnSessionStateChange(1, state);
    ssm_->focusedSessionId_ = 0;
    ASSERT_NE(nullptr, ssm_);
    ssm_->OnSessionStateChange(1, state);
}

/**
 * @tc.name: OnSessionStateChange01
 * @tc.desc: OnSessionStateChange01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, OnSessionStateChange01, Function | SmallTest | Level3)
{
    SessionState state = SessionState::STATE_BACKGROUND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ASSERT_NE(nullptr, ssm_);
    ssm_->OnSessionStateChange(1, state);
    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_NE(nullptr, ssm_);
    ssm_->OnSessionStateChange(1, state);
    state = SessionState::STATE_END;
    ASSERT_NE(nullptr, ssm_);
    ssm_->OnSessionStateChange(1, state);
}

/**
 * @tc.name: OnSessionStateChange02
 * @tc.desc: OnSessionStateChange02
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, OnSessionStateChange02, Function | SmallTest | Level3)
{
    SessionState state = SessionState::STATE_FOREGROUND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    sceneSession->SetFocusedOnShow(true);
    ASSERT_NE(nullptr, ssm_);
    ssm_->OnSessionStateChange(1, state);
    sceneSession->SetFocusedOnShow(false);
    ASSERT_NE(nullptr, ssm_);
    ssm_->OnSessionStateChange(1, state);
}

/**
 * @tc.name: SetWindowFlags
 * @tc.desc: SetWindowFlags
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, SetWindowFlags, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sptr<WindowSessionProperty> property = nullptr;
    sceneSession->property_ = nullptr;
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->SetWindowFlags(sceneSession, property);
    EXPECT_EQ(WSError::WS_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: ProcessSubSessionForeground
 * @tc.desc: ProcessSubSessionForeground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, ProcessSubSessionForeground, Function | SmallTest | Level3)
{
    sptr<SceneSession> sceneSession = nullptr;
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessSubSessionForeground(sceneSession);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessSubSessionForeground(sceneSession);
    sptr<SceneSession> subSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, subSession);
    sceneSession->AddSubSession(subSession);
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessSubSessionForeground(sceneSession);
    ASSERT_NE(nullptr, subSession);
    subSession->SetSessionState(SessionState::STATE_FOREGROUND);
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessSubSessionForeground(sceneSession);
    ASSERT_NE(nullptr, subSession);
    subSession->SetSessionState(SessionState::STATE_ACTIVE);
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessSubSessionForeground(sceneSession);
    ASSERT_NE(nullptr, subSession);
    subSession->SetSessionState(SessionState::STATE_INACTIVE);
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessSubSessionForeground(sceneSession);
}

/**
 * @tc.name: ProcessModalTopmostRequestFocusImmdediately
 * @tc.desc: ProcessModalTopmostRequestFocusImmdediately
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, ProcessModalTopmostRequestFocusImmdediately, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->ProcessModalTopmostRequestFocusImmdediately(sceneSession);
    EXPECT_EQ(WSError::WS_DO_NOTHING, ret);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->ProcessModalTopmostRequestFocusImmdediately(sceneSession);
    EXPECT_EQ(WSError::WS_DO_NOTHING, ret);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_SUB_WINDOW_END);
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->ProcessModalTopmostRequestFocusImmdediately(sceneSession);
    EXPECT_EQ(WSError::WS_DO_NOTHING, ret);
}

/**
 * @tc.name: GetAbilityInfosFromBundleInfo
 * @tc.desc: GetAbilityInfosFromBundleInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, GetAbilityInfosFromBundleInfo, Function | SmallTest | Level3)
{
    std::vector<AppExecFwk::BundleInfo> bundleInfos;
    std::vector<SCBAbilityInfo> scbAbilityInfos;
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->GetAbilityInfosFromBundleInfo(bundleInfos, scbAbilityInfos);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_PARAM, ret);
    OHOS::AppExecFwk::BundleInfo bundleInfo;
    bundleInfo.name = "com.ix.residentservcie";
    bundleInfo.isKeepAlive = true;
    bundleInfo.applicationInfo.process = "com.ix.residentservcie";
    OHOS::AppExecFwk::HapModuleInfo hapModuleInfo;
    hapModuleInfo.isModuleJson = true;
    hapModuleInfo.mainElementName = "residentServiceAbility";
    hapModuleInfo.process = "com.ix.residentservcie";
    bundleInfo.hapModuleInfos.emplace_back(hapModuleInfo);
    bundleInfos.emplace_back(bundleInfo);
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->GetAbilityInfosFromBundleInfo(bundleInfos, scbAbilityInfos);
    EXPECT_EQ(WSError::WS_OK, ret);
}

/**
 * @tc.name: GetOrientationFromResourceManager
 * @tc.desc: GetOrientationFromResourceManager
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, GetOrientationFromResourceManager, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    OHOS::AppExecFwk::AbilityInfo abilityInfo;
    abilityInfo.bundleName = "testBundleName";
    abilityInfo.moduleName = "testModuleName";
    abilityInfo.orientationId = 123456;
    ssm_->GetOrientationFromResourceManager(abilityInfo);
    EXPECT_EQ(OHOS::AppExecFwk::DisplayOrientation::UNSPECIFIED, abilityInfo.orientation);
}

/**
 * @tc.name: NotifyCompleteFirstFrameDrawing
 * @tc.desc: NotifyCompleteFirstFrameDrawing
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, NotifyCompleteFirstFrameDrawing, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sessionInfo.abilityInfo = nullptr;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ASSERT_NE(nullptr, ssm_);
    ssm_->NotifyCompleteFirstFrameDrawing(1);
    sessionInfo.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    ASSERT_NE(nullptr, sessionInfo.abilityInfo);
    ASSERT_NE(nullptr, ssm_);
    ssm_->eventHandler_ = nullptr;
    ssm_->NotifyCompleteFirstFrameDrawing(1);
    ssm_->eventHandler_ = std::make_shared<AppExecFwk::EventHandler>();
    ASSERT_NE(nullptr, ssm_->eventHandler_);
    ASSERT_NE(nullptr, ssm_);
    ssm_->NotifyCompleteFirstFrameDrawing(1);
    ASSERT_NE(nullptr, ssm_);
    ssm_->taskScheduler_ = nullptr;
    ssm_->NotifyCompleteFirstFrameDrawing(1);
    ssm_->taskScheduler_ = std::make_shared<TaskScheduler>("OS_SceneSessionManager");
    ASSERT_NE(nullptr, ssm_->taskScheduler_);
    ASSERT_NE(nullptr, ssm_);
    ssm_->NotifyCompleteFirstFrameDrawing(1);
}

/**
 * @tc.name: NotifyCompleteFirstFrameDrawing02
 * @tc.desc: NotifyCompleteFirstFrameDrawing02:AtomicService free-install start.
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, NotifyCompleteFirstFrameDrawing02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sessionInfo.abilityInfo = nullptr;
    sessionInfo.isAtomicService_ = true;
    sessionInfo.isBackTransition_ = false;
    unsigned int flags = 11111111;
    sessionInfo.want = std::make_shared<AAFwk::Want>();
    ASSERT_NE(nullptr, sessionInfo.want);
    sessionInfo.want->SetFlags(flags);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->NotifyCompleteFirstFrameDrawing(1);
    ASSERT_EQ(nullptr, sessionInfo.abilityInfo);
}

/**
 * @tc.name: InitSceneSession01
 * @tc.desc: InitSceneSession01:AtomicService free-install start.
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, InitSceneSession01, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sessionInfo.abilityInfo = nullptr;
    sessionInfo.isAtomicService_ = true;
    sessionInfo.isBackTransition_ = false;
    unsigned int flags = 11111111;
    sessionInfo.want = std::make_shared<AAFwk::Want>();
    ASSERT_NE(nullptr, sessionInfo.want);
    sessionInfo.want->SetFlags(flags);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->InitSceneSession(sceneSession, sessionInfo, nullptr);
    ASSERT_EQ(nullptr, sessionInfo.abilityInfo);
}

/**
 * @tc.name: CheckAndNotifyWaterMarkChangedResult
 * @tc.desc: CheckAndNotifyWaterMarkChangedResult
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, CheckAndNotifyWaterMarkChangedResult, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    ssm_->CheckAndNotifyWaterMarkChangedResult();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->property_ = nullptr;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    sceneSession->property_ = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->flags_ = 1 << 4;
    sceneSession->isRSVisible_ = true;
    sceneSession->combinedExtWindowFlags_.waterMarkFlag = true;
    ASSERT_NE(nullptr, ssm_);
    ssm_->CheckAndNotifyWaterMarkChangedResult();
    sceneSession->isRSVisible_ = false;
    ASSERT_NE(nullptr, ssm_);
    ssm_->CheckAndNotifyWaterMarkChangedResult();
    sceneSession->property_->flags_ = 0;
    sceneSession->isRSVisible_ = false;
    ASSERT_NE(nullptr, ssm_);
    ssm_->CheckAndNotifyWaterMarkChangedResult();
    sceneSession->isRSVisible_ = true;
    ASSERT_NE(nullptr, ssm_);
    ssm_->CheckAndNotifyWaterMarkChangedResult();
}

/**
 * @tc.name: CheckAndNotifyWaterMarkChangedResult01
 * @tc.desc: CheckAndNotifyWaterMarkChangedResult01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, CheckAndNotifyWaterMarkChangedResult01, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    sceneSession->property_ = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->flags_ = 1 << 4;
    sceneSession->isRSVisible_ = true;
    sceneSession->combinedExtWindowFlags_.waterMarkFlag = false;
    ASSERT_NE(nullptr, ssm_);
    ssm_->CheckAndNotifyWaterMarkChangedResult();
    sceneSession->isRSVisible_ = false;
    ASSERT_NE(nullptr, ssm_);
    ssm_->CheckAndNotifyWaterMarkChangedResult();
    sceneSession->property_->flags_ = 0;
    sceneSession->isRSVisible_ = false;
    ASSERT_NE(nullptr, ssm_);
    ssm_->CheckAndNotifyWaterMarkChangedResult();
    sceneSession->isRSVisible_ = true;
    ASSERT_NE(nullptr, ssm_);
    ssm_->CheckAndNotifyWaterMarkChangedResult();
}

/**
 * @tc.name: CheckAndNotifyWaterMarkChangedResult02
 * @tc.desc: CheckAndNotifyWaterMarkChangedResult02
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, CheckAndNotifyWaterMarkChangedResult02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    ssm_->lastWaterMarkShowState_ = true;
    ssm_->CheckAndNotifyWaterMarkChangedResult();
    ASSERT_NE(nullptr, ssm_);
    ssm_->lastWaterMarkShowState_ = false;
    ssm_->CheckAndNotifyWaterMarkChangedResult();
}

/**
 * @tc.name: FillWindowInfo
 * @tc.desc: FillWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, FillWindowInfo, Function | SmallTest | Level3)
{
    std::vector<sptr<AccessibilityWindowInfo>> infos;
    sptr<SceneSession> sceneSession = nullptr;
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->FillWindowInfo(infos, sceneSession);
    EXPECT_EQ(false, ret);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, ssm_);
    ASSERT_NE(nullptr, sceneSession);
    ret = ssm_->FillWindowInfo(infos, sceneSession);
    EXPECT_EQ(true, ret);
    sessionInfo.bundleName_ = "SCBGestureBack";
    sessionInfo.isSystem_ = true;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->FillWindowInfo(infos, sceneSession);
    EXPECT_EQ(true, ret);
    sessionInfo.isSystem_ = false;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->FillWindowInfo(infos, sceneSession);
    EXPECT_EQ(true, ret);
    sceneSession->property_ = nullptr;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->FillWindowInfo(infos, sceneSession);
    EXPECT_EQ(true, ret);
    sceneSession->property_ = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, sceneSession->property_);
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->FillWindowInfo(infos, sceneSession);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: SetSessionVisibilityInfo
 * @tc.desc: SetSessionVisibilityInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, SetSessionVisibilityInfo, Function | SmallTest | Level3)
{
    sptr<SceneSession> session = nullptr;
    WindowVisibilityState visibleState = WindowVisibilityState::WINDOW_VISIBILITY_STATE_NO_OCCLUSION;
    std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
    std::string visibilityInfo = "";
    ASSERT_NE(nullptr, ssm_);
    ssm_->SetSessionVisibilityInfo(session, visibleState, windowVisibilityInfos, visibilityInfo);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    session = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, ssm_);
    ASSERT_NE(nullptr, session);
    session->persistentId_ = 1;
    ssm_->windowVisibilityListenerSessionSet_.clear();
    ssm_->SetSessionVisibilityInfo(session, visibleState, windowVisibilityInfos, visibilityInfo);
    ssm_->windowVisibilityListenerSessionSet_.insert(1);
    ssm_->SetSessionVisibilityInfo(session, visibleState, windowVisibilityInfos, visibilityInfo);
}

/**
 * @tc.name: SendTouchEvent
 * @tc.desc: SendTouchEvent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, SendTouchEvent, Function | SmallTest | Level3)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->SendTouchEvent(pointerEvent, 0);
    EXPECT_EQ(WSError::WS_ERROR_NULLPTR, ret);
    MMI::PointerEvent::PointerItem pointerItem;
    pointerItem.pointerId_ = 0;
    pointerEvent = MMI::PointerEvent::Create();
    ASSERT_NE(nullptr, pointerEvent);
    pointerEvent->pointerId_ = 0;
    pointerEvent->AddPointerItem(pointerItem);
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->SendTouchEvent(pointerEvent, 0);
    EXPECT_EQ(WSError::WS_OK, ret);
    pointerEvent->pointerId_ = 1;
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->SendTouchEvent(pointerEvent, 0);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_PARAM, ret);
    ASSERT_NE(nullptr, ssm_);
    ssm_->RegisterWindowChanged(WindowChangedFuncTest);
}

/**
 * @tc.name: JudgeNeedNotifyPrivacyInfo
 * @tc.desc: JudgeNeedNotifyPrivacyInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, JudgeNeedNotifyPrivacyInfo, Function | SmallTest | Level3)
{
    DisplayId displayId = 1;
    std::unordered_set<std::string> privacyBundles;
    ssm_->privacyBundleMap_.clear();
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->JudgeNeedNotifyPrivacyInfo(displayId, privacyBundles);
    EXPECT_EQ(true, ret);
    privacyBundles.insert("bundle1");
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->JudgeNeedNotifyPrivacyInfo(displayId, privacyBundles);
    EXPECT_EQ(true, ret);
    std::unordered_set<std::string> privacyBundles1;
    privacyBundles1.insert("bundle2");
    ASSERT_NE(nullptr, ssm_);
    ssm_->privacyBundleMap_.insert({displayId, privacyBundles1});
    ret = ssm_->JudgeNeedNotifyPrivacyInfo(displayId, privacyBundles);
    EXPECT_EQ(true, ret);
    privacyBundles.insert("bundle2");
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->JudgeNeedNotifyPrivacyInfo(displayId, privacyBundles);
    EXPECT_EQ(true, ret);
    ASSERT_NE(nullptr, ssm_);
    ssm_->InitPersistentStorage();
    ASSERT_NE(nullptr, ssm_);
    ssm_->UpdateCameraFloatWindowStatus(0, true);
    ASSERT_NE(nullptr, ssm_);
    ssm_->UpdateCameraWindowStatus(0, true);
}

/**
 * @tc.name: UpdatePrivateStateAndNotify
 * @tc.desc: UpdatePrivateStateAndNotify
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, UpdatePrivateStateAndNotify, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    ssm_->UpdatePrivateStateAndNotify(0);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    sceneSession->property_ = nullptr;
    ASSERT_NE(nullptr, ssm_);
    ssm_->UpdatePrivateStateAndNotify(1);
    ASSERT_NE(nullptr, ssm_);
    ssm_->UpdatePrivateStateAndNotifyForAllScreens();
}

/**
 * @tc.name: UpdatePrivateStateAndNotify2
 * @tc.desc: UpdatePrivateStateAndNotify2
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, UpdatePrivateStateAndNotify2, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    ssm_->privacyBundleMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    sceneSession->property_ = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetPrivacyMode(true);
    sceneSession->property_->SetDisplayId(1);
    std::unordered_set<std::string> privacyBundles1;
    std::unordered_set<std::string> privacyBundles2;
    std::unordered_set<std::string> privacyBundles3;
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->privacyBundleMap_.insert({1, privacyBundles1});
    ssm_->privacyBundleMap_.insert({2, privacyBundles2});
    ssm_->privacyBundleMap_.insert({3, privacyBundles3});
    ssm_->UpdatePrivateStateAndNotify(1);
    ASSERT_EQ(ssm_->privacyBundleMap_[1].size(), 1);
    sceneSession->SetSessionState(SessionState::STATE_BACKGROUND);
    sceneSession->property_->SetPrivacyMode(true);
    sceneSession->property_->SetDisplayId(2);
    ssm_->UpdatePrivateStateAndNotify(1);
    ASSERT_EQ(ssm_->privacyBundleMap_[2].size(), 0);
    sceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    sceneSession->property_->SetPrivacyMode(false);
    sceneSession->property_->SetDisplayId(3);
    ssm_->UpdatePrivateStateAndNotify(1);
    ASSERT_EQ(ssm_->privacyBundleMap_[3].size(), 0);
}

/**
 * @tc.name: GetCollaboratorByType
 * @tc.desc: GetCollaboratorByType
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, GetCollaboratorByType, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->collaboratorMap_.clear();
    auto ret = ssm_->GetCollaboratorByType(0);
    EXPECT_EQ(nullptr, ret);
    sptr<AAFwk::IAbilityManagerCollaborator> collaborator = nullptr;
    ASSERT_NE(nullptr, ssm_);
    ssm_->collaboratorMap_.insert(std::make_pair(1, collaborator));
    ret = ssm_->GetCollaboratorByType(1);
    EXPECT_EQ(nullptr, ret);
}

/**
 * @tc.name: RegisterGetStateFromManagerFunc
 * @tc.desc: RegisterGetStateFromManagerFunc
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, RegisterGetStateFromManagerFunc, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, ssm_);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->RegisterGetStateFromManagerFunc(sceneSession);
    sceneSession = nullptr;
    ASSERT_NE(nullptr, ssm_);
    ssm_->RegisterGetStateFromManagerFunc(sceneSession);
}

/**
 * @tc.name: ProcessDialogRequestFocusImmdediately
 * @tc.desc: ProcessDialogRequestFocusImmdediately
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, ProcessDialogRequestFocusImmdediately, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->ProcessDialogRequestFocusImmdediately(sceneSession);
    EXPECT_EQ(WSError::WS_DO_NOTHING, ret);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->ProcessDialogRequestFocusImmdediately(sceneSession);
    EXPECT_EQ(WSError::WS_DO_NOTHING, ret);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_SUB_WINDOW_END);
    ASSERT_NE(nullptr, ssm_);
    ret = ssm_->ProcessDialogRequestFocusImmdediately(sceneSession);
    EXPECT_EQ(WSError::WS_DO_NOTHING, ret);
}

/**
 * @tc.name: ProcessSubSessionBackground
 * @tc.desc: ProcessSubSessionBackground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, ProcessSubSessionBackground, Function | SmallTest | Level3)
{
    sptr<SceneSession> sceneSession = nullptr;
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessSubSessionBackground(sceneSession);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessSubSessionBackground(sceneSession);
    sptr<SceneSession> subSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, subSession);
    sceneSession->AddSubSession(subSession);
    subSession->state_ = SessionState::STATE_FOREGROUND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessSubSessionBackground(sceneSession);
    ASSERT_NE(nullptr, subSession);
    subSession->state_ = SessionState::STATE_ACTIVE;
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessSubSessionBackground(sceneSession);
    ASSERT_NE(nullptr, subSession);
    subSession->state_ = SessionState::STATE_INACTIVE;
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessSubSessionBackground(sceneSession);
}

/**
 * @tc.name: ProcessSubSessionBackground01
 * @tc.desc: ProcessSubSessionBackground01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, ProcessSubSessionBackground01, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->dialogVec_.clear();
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessSubSessionBackground(sceneSession);
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->dialogVec_.push_back(sceneSession1);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    sceneSession1->persistentId_ = 1;
    ssm_->sceneSessionMap_.insert(std::make_pair(0, sceneSession));
    ssm_->ProcessSubSessionBackground(sceneSession);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->ProcessSubSessionBackground(sceneSession);
    sptr<SceneSession> toastSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, toastSession);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->AddToastSession(toastSession);
    toastSession->state_ = SessionState::STATE_FOREGROUND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessSubSessionBackground(sceneSession);
    toastSession->state_ = SessionState::STATE_ACTIVE;
    ssm_->ProcessSubSessionBackground(sceneSession);
    toastSession->state_ = SessionState::STATE_INACTIVE;
    ssm_->ProcessSubSessionBackground(sceneSession);
}

/**
 * @tc.name: IsValidSessionIds
 * @tc.desc: IsValidSessionIds
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, IsValidSessionIds, Function | SmallTest | Level3)
{
    std::vector<int32_t> sessionIds = {1, 2, 3, 4};
    std::vector<bool> results;
    results.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    sptr<SceneSession> sceneSession1 = nullptr;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession1));
    ssm_->IsValidSessionIds(sessionIds, results);
    EXPECT_FALSE(results.empty());
    DisplayChangeListener listener;
    std::vector<uint64_t> missionIds;
    std::vector<uint64_t> surfaceNodeIds;
    listener.OnGetSurfaceNodeIdsFromMissionIds(missionIds, surfaceNodeIds);
}

/**
 * @tc.name: DeleteStateDetectTask
 * @tc.desc: DeleteStateDetectTask
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, DeleteStateDetectTask, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->SetScreenLocked(true);
    EXPECT_EQ(true, ssm_->isScreenLocked_);
    ssm_->sceneSessionMap_.clear();
    ASSERT_NE(nullptr, ssm_);
    ssm_->DeleteStateDetectTask();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->detectTaskInfo_.taskState = DetectTaskState::NO_TASK;
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ASSERT_NE(nullptr, ssm_);
    ssm_->DeleteStateDetectTask();
    sceneSession->detectTaskInfo_.taskState = DetectTaskState::ATTACH_TASK;
    ASSERT_NE(nullptr, ssm_);
    ssm_->DeleteStateDetectTask();
}

/**
 * @tc.name: GetWindowStyleType
 * @tc.desc: GetWindowStyleType
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest6, GetWindowStyleType, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    WindowStyleType windowModeType = Rosen::WindowStyleType::WINDOW_STYLE_DEFAULT;
    ssm_->GetWindowStyleType(windowModeType);
    ASSERT_EQ(windowModeType, Rosen::WindowStyleType::WINDOW_STYLE_DEFAULT);
}

/**
 * @tc.name: TerminateSessionByPersistentId
 * @tc.desc: Success to terminate session by persistentId.
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, TerminateSessionByPersistentId001, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test1";
    info.windowType_ = static_cast<uint32_t>(WindowType::APP_WINDOW_BASE);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(sceneSession->GetPersistentId(), sceneSession));
    auto result = ssm_->TerminateSessionByPersistentId(sceneSession->GetPersistentId());
    EXPECT_EQ(result, WMError::WM_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: TerminateSessionByPersistentId
 * @tc.desc: Fail to terminate session by persistentId, invalid persistentId.
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, TerminateSessionByPersistentId002, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test1";
    info.windowType_ = static_cast<uint32_t>(WindowType::APP_WINDOW_BASE);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(sceneSession->GetPersistentId(), sceneSession));
    auto result = ssm_->TerminateSessionByPersistentId(INVALID_SESSION_ID);
    EXPECT_EQ(result, WMError::WM_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: SetRootSceneProcessBackEventFunc
 * @tc.desc: test function : SetRootSceneProcessBackEventFunc
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, SetRootSceneProcessBackEventFunc, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest6";
    sessionInfo.abilityName_ = "SetRootSceneProcessBackEventFunc";
    sessionInfo.windowType_ = static_cast<uint32_t>(WindowType::APP_WINDOW_BASE);
    sessionInfo.isSystem_ = true;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(sceneSession->GetPersistentId(), sceneSession));
    ssm_->focusedSessionId_ = sceneSession->GetPersistentId();
    ssm_->needBlockNotifyFocusStatusUntilForeground_ = false;
    ssm_->ProcessBackEvent();

    RootSceneProcessBackEventFunc func = []() {};
    ssm_->SetRootSceneProcessBackEventFunc(func);
    ssm_->ProcessBackEvent();
}

/**
 * @tc.name: OnScreenFoldStatusChanged
 * @tc.desc: OnScreenFoldStatusChanged
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, OnScreenFoldStatusChanged, Function | SmallTest | Level3)
{
    std::vector<std::string> screenFoldInfo;
    sptr<IDisplayChangeListener> listener = sptr<DisplayChangeListener>::MakeSptr();
    ASSERT_NE(nullptr, listener);
    listener->OnScreenFoldStatusChanged(screenFoldInfo);
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->UpdateDisplayHookInfo(0, 50, 50, 0.0f, true);
    EXPECT_EQ(ret, WMError::WM_OK);
    ssm_->CheckSceneZOrder();
}

/**
 * @tc.name: NotifySessionForeground
 * @tc.desc: NotifySessionForeground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, NotifySessionForeground, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest6";
    sessionInfo.abilityName_ = "NotifySessionForeground";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    uint32_t reason = 0;
    bool withAnimation = false;
    ASSERT_NE(nullptr, ssm_);
    ssm_->NotifySessionForeground(sceneSession, reason, withAnimation);
    WSRect area = { 0, 0, 0, 0 };
    uint32_t type = 0;
    ssm_->AddWindowDragHotArea(type, area);
    uint64_t displayId = 0;
    ssm_->currAINavigationBarAreaMap_.clear();
    ssm_->currAINavigationBarAreaMap_.insert(std::make_pair(displayId, area));
    auto ret = ssm_->GetAINavigationBarArea(1);
    EXPECT_TRUE(ret.IsEmpty());
    ret = ssm_->GetAINavigationBarArea(displayId);
    EXPECT_EQ(ret, area);
}

/**
 * @tc.name: OnDisplayStateChange
 * @tc.desc: OnDisplayStateChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, OnDisplayStateChange, Function | SmallTest | Level3)
{
    DisplayChangeListener listener;
    DisplayId displayId = 0;
    sptr<DisplayInfo> displayInfo = sptr<DisplayInfo>::MakeSptr();
    ASSERT_NE(nullptr, displayInfo);
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap;
    displayInfoMap.insert(std::make_pair(displayId, displayInfo));
    DisplayStateChangeType type = DisplayStateChangeType::VIRTUAL_PIXEL_RATIO_CHANGE;
    listener.OnDisplayStateChange(displayId, displayInfo, displayInfoMap, type);
    type = DisplayStateChangeType::UPDATE_ROTATION;
    listener.OnDisplayStateChange(displayId, displayInfo, displayInfoMap, type);
    type = DisplayStateChangeType::UPDATE_SCALE;
    listener.OnDisplayStateChange(displayId, displayInfo, displayInfoMap, type);
    type = DisplayStateChangeType::UNKNOWN;
    listener.OnDisplayStateChange(displayId, displayInfo, displayInfoMap, type);
}

/**
 * @tc.name: UpdateSessionAvoidAreaIfNeed
 * @tc.desc: UpdateSessionAvoidAreaIfNeed
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, UpdateSessionAvoidAreaIfNeed, Function | SmallTest | Level3)
{
    int32_t persistentId = 0;
    sptr<SceneSession> sceneSession = nullptr;
    AvoidArea avoidArea;
    AvoidAreaType avoidAreaType = AvoidAreaType::TYPE_KEYBOARD;
    ASSERT_NE(nullptr, ssm_);
    ssm_->enterRecent_ = false;
    auto ret = ssm_->UpdateSessionAvoidAreaIfNeed(persistentId, sceneSession, avoidArea, avoidAreaType);
    EXPECT_EQ(ret, false);
    ssm_->enterRecent_ = true;
    ret = ssm_->UpdateSessionAvoidAreaIfNeed(persistentId, sceneSession, avoidArea, avoidAreaType);
    EXPECT_EQ(ret, false);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest6";
    sessionInfo.abilityName_ = "UpdateSessionAvoidAreaIfNeed";
    sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ret = ssm_->UpdateSessionAvoidAreaIfNeed(persistentId, sceneSession, avoidArea, avoidAreaType);
    EXPECT_EQ(ret, false);
    ssm_->enterRecent_ = false;
    ret = ssm_->UpdateSessionAvoidAreaIfNeed(persistentId, sceneSession, avoidArea, avoidAreaType);
    EXPECT_EQ(ret, true);
    ssm_->lastUpdatedAvoidArea_.clear();
    ret = ssm_->UpdateSessionAvoidAreaIfNeed(persistentId, sceneSession, avoidArea, avoidAreaType);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: UpdateSessionAvoidAreaIfNeed01
 * @tc.desc: UpdateSessionAvoidAreaIfNeed
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, UpdateSessionAvoidAreaIfNeed01, Function | SmallTest | Level3)
{
    int32_t persistentId = 0;
    AvoidArea avoidArea;
    AvoidAreaType avoidAreaType = AvoidAreaType::TYPE_KEYBOARD;
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest6";
    sessionInfo.abilityName_ = "UpdateSessionAvoidAreaIfNeed";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, ssm_);
    ssm_->enterRecent_ = false;
    std::map<AvoidAreaType, AvoidArea> mapAvoidAreaType;
    mapAvoidAreaType.insert(std::make_pair(avoidAreaType, avoidArea));
    ssm_->lastUpdatedAvoidArea_.insert(std::make_pair(persistentId, mapAvoidAreaType));
    auto ret = ssm_->UpdateSessionAvoidAreaIfNeed(persistentId, sceneSession, avoidArea, avoidAreaType);
    EXPECT_EQ(ret, false);
    avoidAreaType = AvoidAreaType::TYPE_SYSTEM;
    ret = ssm_->UpdateSessionAvoidAreaIfNeed(persistentId, sceneSession, avoidArea, avoidAreaType);
    EXPECT_EQ(ret, false);
    avoidArea.topRect_.posX_ = 1;
    ret = ssm_->UpdateSessionAvoidAreaIfNeed(persistentId, sceneSession, avoidArea, avoidAreaType);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: CheckIfReuseSession
 * @tc.desc: CheckIfReuseSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, CheckIfReuseSession, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest6";
    sessionInfo.abilityName_ = "CheckIfReuseSession";
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->CheckIfReuseSession(sessionInfo);
    EXPECT_EQ(ret, BrokerStates::BROKER_UNKOWN);
    ScreenId screenId = 0;
    std::unordered_map<int32_t, SessionUIParam> uiParams;
    ssm_->FlushUIParams(screenId, std::move(uiParams));
}

/**
 * @tc.name: UpdateAvoidArea
 * @tc.desc: UpdateAvoidArea
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, UpdateAvoidArea, Function | SmallTest | Level3)
{
    int32_t persistentId = 0;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    ssm_->UpdateAvoidArea(persistentId);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest6";
    sessionInfo.abilityName_ = "UpdateAvoidArea";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert(std::make_pair(persistentId, sceneSession));
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_STATUS_BAR);
    ssm_->UpdateAvoidArea(persistentId);
    sceneSession->property_->SetWindowType(WindowType::APP_WINDOW_BASE);
    ssm_->UpdateAvoidArea(persistentId);
}

/**
 * @tc.name: UpdateMaximizeMode
 * @tc.desc: UpdateMaximizeMode
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, UpdateMaximizeMode, Function | SmallTest | Level3)
{
    int32_t persistentId = 0;
    bool isMaximize = true;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    auto ret = ssm_->UpdateMaximizeMode(persistentId, isMaximize);
    EXPECT_EQ(ret, WSError::WS_OK);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest6";
    sessionInfo.abilityName_ = "UpdateMaximizeMode";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert(std::make_pair(persistentId, sceneSession));
    EXPECT_EQ(ret, WSError::WS_OK);
    sptr<DisplayInfo> displayInfo = nullptr;
    ssm_->ProcessDisplayScale(displayInfo);
    displayInfo = sptr<DisplayInfo>::MakeSptr();
    ASSERT_NE(nullptr, displayInfo);
    ssm_->ProcessDisplayScale(displayInfo);
    ProcessVirtualPixelRatioChangeFunc func = nullptr;
    ssm_->SetVirtualPixelRatioChangeListener(func);
}

/**
 * @tc.name: WindowDestroyNotifyVisibility
 * @tc.desc: WindowDestroyNotifyVisibility
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, WindowDestroyNotifyVisibility, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest6";
    sessionInfo.abilityName_ = "WindowDestroyNotifyVisibility";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetRSVisible(true);
    ASSERT_NE(nullptr, ssm_);
    ssm_->WindowDestroyNotifyVisibility(sceneSession);
    sceneSession->SetRSVisible(false);
    ssm_->WindowDestroyNotifyVisibility(sceneSession);
    sceneSession = nullptr;
    ssm_->WindowDestroyNotifyVisibility(sceneSession);
}

/**
 * @tc.name: RequestInputMethodCloseKeyboard
 * @tc.desc: RequestInputMethodCloseKeyboard
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, RequestInputMethodCloseKeyboard, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo info;
    sptr<SceneSession::SpecificSessionCallback> specificCallback = nullptr;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCallback);
    ssm_->sceneSessionMap_.insert({0, sceneSession});
    int32_t persistentId = 10;
    ssm_->RequestInputMethodCloseKeyboard(persistentId);

    persistentId = 0;
    sptr<Session> session = new Session(info);
    session->property_ = nullptr;
    ssm_->RequestInputMethodCloseKeyboard(persistentId);
    
    bool enable = true;
    auto result = ssm_->GetFreeMultiWindowEnableState(enable);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSession
 * @tc.desc: RequestSceneSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, RequestSceneSession, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sptr<WindowSessionProperty> property = nullptr;
    ssm_->RequestSceneSession(sessionInfo, property);

    sessionInfo.persistentId_ = 1;
    sptr<SceneSession::SpecificSessionCallback> specificCallback = nullptr;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(sessionInfo, specificCallback);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->RequestSceneSession(sessionInfo, property);

    ssm_->sceneSessionMap_.clear();
    ssm_->sceneSessionMap_.insert({0, sceneSession});
    ssm_->RequestSceneSession(sessionInfo, property);

    sessionInfo.persistentId_ = 0;
    ssm_->RequestSceneSession(sessionInfo, property);
}

/**
 * @tc.name: IsKeyboardForeground
 * @tc.desc: IsKeyboardForeground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, IsKeyboardForeground, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo info;
    sptr<SceneSession::SpecificSessionCallback> specificCallback = nullptr;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCallback);
    ASSERT_NE(sceneSession, nullptr);
    sptr<Session> session = new Session(info);
    ASSERT_NE(session, nullptr);
    session->property_ = nullptr;
    auto result = ssm_->IsKeyboardForeground();
    ASSERT_EQ(result, false);

    ssm_->sceneSessionMap_.insert({0, sceneSession});
    session->property_ = new WindowSessionProperty();
    ASSERT_NE(session->property_, nullptr);

    if (session->property_) {
        auto result1 = session->GetWindowType();
        result1 = WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT;
        ASSERT_EQ(result1, WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    }
    result = ssm_->IsKeyboardForeground();
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: DestroyDialogWithMainWindow
 * @tc.desc: DestroyDialogWithMainWindow
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, DestroyDialogWithMainWindow, Function | SmallTest | Level3)
{
    sptr<SceneSession> scnSession = nullptr;
    auto result = ssm_->DestroyDialogWithMainWindow(scnSession);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    SessionInfo info;
    sptr<SceneSession::SpecificSessionCallback> specificCallback = nullptr;
    scnSession = new (std::nothrow) SceneSession(info, specificCallback);
    ASSERT_NE(scnSession, nullptr);

    sptr<Session> session = new Session(info);
    ASSERT_NE(session, nullptr);
    session->GetDialogVector().clear();
    ssm_->DestroyDialogWithMainWindow(scnSession);

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCallback);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({0, sceneSession});
    ssm_->GetSceneSession(1);
    result = ssm_->DestroyDialogWithMainWindow(scnSession);
    ASSERT_EQ(result, WSError::WS_OK);

    WindowVisibilityInfo windowVisibilityInfo;
    windowVisibilityInfo.windowType_ = WindowType::APP_WINDOW_BASE;
    ssm_->DestroyDialogWithMainWindow(scnSession);
}

/**
 * @tc.name: RequestSceneSessionDestruction
 * @tc.desc: RequestSceneSessionDestruction
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, RequestSceneSessionDestruction, Function | SmallTest | Level3)
{
    sptr<SceneSession> sceneSession;
    ASSERT_EQ(sceneSession, nullptr);
    bool needRemoveSession = true;
    bool isSaveSnapshot = true;
    bool isForceClean = true;
    ssm_->RequestSceneSessionDestruction(sceneSession, needRemoveSession, isSaveSnapshot, isForceClean);

    SessionInfo info;
    sptr<SceneSession::SpecificSessionCallback> specificCallback = nullptr;
    sceneSession = new (std::nothrow) SceneSession(info, specificCallback);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    ssm_->RequestSceneSessionDestruction(sceneSession, needRemoveSession, isSaveSnapshot, isForceClean);
}

/**
 * @tc.name: NotifySessionAINavigationBarChange
 * @tc.desc: NotifySessionAINavigationBarChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, NotifySessionAINavigationBarChange, Function | SmallTest | Level3)
{
    int32_t persistentId = 1;
    SessionInfo info;
    sptr<SceneSession::SpecificSessionCallback> specificCallback = nullptr;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCallback);
    ssm_->sceneSessionMap_.insert({0, sceneSession});
    ssm_->NotifySessionAINavigationBarChange(persistentId);

    persistentId = 0;
    Session session(info);
    session.isVisible_ = true;
    session.state_ = SessionState::STATE_FOREGROUND;
    ssm_->NotifySessionAINavigationBarChange(persistentId);
}

/**
 * @tc.name: GetProcessSurfaceNodeIdByPersistentId
 * @tc.desc: GetProcessSurfaceNodeIdByPersistentId
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, GetProcessSurfaceNodeIdByPersistentId, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo info;
    sptr<SceneSession::SpecificSessionCallback> specificCallback = nullptr;
    sptr<SceneSession> sceneSession1 = new (std::nothrow) SceneSession(info, specificCallback);
    sptr<SceneSession> sceneSession2 = new (std::nothrow) SceneSession(info, specificCallback);
    sptr<SceneSession> sceneSession3 = new (std::nothrow) SceneSession(info, specificCallback);
    sceneSession1->SetCallingPid(123);
    sceneSession2->SetCallingPid(123);
    sceneSession3->SetCallingPid(111);

    int32_t pid = 123;
    std::vector<int32_t> persistentIds;
    std::vector<uint64_t> surfaceNodeIds;
    persistentIds.push_back(sceneSession1->GetPersistentId());
    persistentIds.push_back(sceneSession2->GetPersistentId());
    persistentIds.push_back(sceneSession3->GetPersistentId());
    ssm_->sceneSessionMap_.insert({sceneSession1->GetPersistentId(), sceneSession1});
    ssm_->sceneSessionMap_.insert({sceneSession2->GetPersistentId(), sceneSession2});
    ssm_->sceneSessionMap_.insert({sceneSession3->GetPersistentId(), sceneSession3});
    
    ASSERT_EQ(WMError::WM_OK, ssm_->GetProcessSurfaceNodeIdByPersistentId(pid, persistentIds, surfaceNodeIds));
    ASSERT_EQ(0, surfaceNodeIds.size());
}
}
} // namespace Rosen
} // namespace OHOS