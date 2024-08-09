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

#include "iremote_object_mocker.h"
#include "interfaces/include/ws_common.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "session_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class SceneSessionManagerTest8 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

void SceneSessionManagerTest8::SetUpTestCase()
{
}

void SceneSessionManagerTest8::TearDownTestCase()
{
}

void SceneSessionManagerTest8::SetUp()
{
    SceneSessionManager::GetInstance().sceneSessionMap_.clear();
}

void SceneSessionManagerTest8::TearDown()
{
    SceneSessionManager::GetInstance().sceneSessionMap_.clear();
    usleep(WAIT_SYNC_IN_NS);
}

namespace {
/**
 * @tc.name: GetTotalUITreeInfo
 * @tc.desc: GetTotalUITreeInfo set gesture navigation enabled
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, GetTotalUITreeInfo, Function | SmallTest | Level3)
{
    std::string strId = "1234";
    std::string dumpInfo = "dumpInfo";
    SceneSessionManager::GetInstance().SetDumpUITreeFunc(nullptr);
    EXPECT_EQ(WSError::WS_OK, SceneSessionManager::GetInstance()
        .GetTotalUITreeInfo(strId, dumpInfo));
    DumpUITreeFunc func = [](uint64_t, std::string& dumpInfo) {
        return;
    };
    SceneSessionManager::GetInstance().SetDumpUITreeFunc(func);
    EXPECT_EQ(WSError::WS_OK, SceneSessionManager::GetInstance()
        .GetTotalUITreeInfo(strId, dumpInfo));
}

/**
 * @tc.name: RequestFocusStatusBySCB
 * @tc.desc: RequestFocusStatusBySCB set gesture navigation enabled
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, RequestFocusStatusBySCB, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "RequestFocusStatusBySCB";
    sessionInfo.abilityName_ = "RequestFocusStatusBySCB";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    EXPECT_NE(nullptr, sceneSession);
    sceneSession->SetFocusable(true);

    FocusChangeReason reason = FocusChangeReason::DEFAULT;
    WMError res = SceneSessionManager::GetInstance()
        .RequestFocusStatusBySCB(8, false, false, reason);
    EXPECT_EQ(WMError::WM_OK, res);

    reason = FocusChangeReason::FOREGROUND;
    res = SceneSessionManager::GetInstance()
        .RequestFocusStatusBySCB(8, true, false, reason);
    EXPECT_EQ(WMError::WM_OK, res);

    reason = FocusChangeReason::MOVE_UP;
    SceneSessionManager::GetInstance().sceneSessionMap_.insert({ 5, sceneSession });
    res = SceneSessionManager::GetInstance()
        .RequestFocusStatusBySCB(5, true, false, reason);
    EXPECT_EQ(WMError::WM_OK, res);

    res = SceneSessionManager::GetInstance()
        .RequestFocusStatusBySCB(8, true, false, reason);
    EXPECT_EQ(WMError::WM_OK, res);

    reason = FocusChangeReason::DEFAULT;
    res = SceneSessionManager::GetInstance()
        .RequestFocusStatusBySCB(8, true, true, reason);
    EXPECT_EQ(WMError::WM_OK, res);

    res = SceneSessionManager::GetInstance()
        .RequestFocusStatusBySCB(5, true, true, reason);
    EXPECT_EQ(WMError::WM_OK, res);
}

/**
 * @tc.name: GetRemoteSessionSnapshotInfo
 * @tc.desc: GetRemoteSessionSnapshotInfo set gesture navigation enabled
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, GetRemoteSessionSnapshotInfo, Function | SmallTest | Level3)
{
    AAFwk::MissionSnapshot sessionSnapshot;
    std::string deviceId = "deviceId";
    int res = SceneSessionManager::GetInstance().GetRemoteSessionSnapshotInfo(
        deviceId, 8, sessionSnapshot);
    EXPECT_EQ(ERR_NULL_OBJECT, res);
}

/**
 * @tc.name: WindowLayerInfoChangeCallback
 * @tc.desc: test function : WindowLayerInfoChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, WindowLayerInfoChangeCallback, Function | SmallTest | Level3)
{
    std::shared_ptr<RSOcclusionData> rsData = nullptr;
    SceneSessionManager::GetInstance().WindowLayerInfoChangeCallback(rsData);

    rsData = std::make_shared<RSOcclusionData>();
    ASSERT_NE(nullptr, rsData);
    SceneSessionManager::GetInstance().WindowLayerInfoChangeCallback(rsData);

    VisibleData visibleData;
    visibleData.push_back(std::make_pair(0, WINDOW_LAYER_INFO_TYPE::ALL_VISIBLE));
    visibleData.push_back(std::make_pair(1, WINDOW_LAYER_INFO_TYPE::SEMI_VISIBLE));
    visibleData.push_back(std::make_pair(2, WINDOW_LAYER_INFO_TYPE::INVISIBLE));
    visibleData.push_back(std::make_pair(3, WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_DYNAMIC_STATUS));
    visibleData.push_back(std::make_pair(4, WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_STATIC_STATUS));
    visibleData.push_back(std::make_pair(5, WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_UNKNOWN_TYPE));
    rsData = std::make_shared<RSOcclusionData>(visibleData);
    ASSERT_NE(nullptr, rsData);
    SceneSessionManager::GetInstance().WindowLayerInfoChangeCallback(rsData);
}

/**
 * @tc.name: DealwithVisibilityChange
 * @tc.desc: test function : DealwithVisibilityChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, DealwithVisibilityChange, Function | SmallTest | Level3)
{
    std::vector<std::pair<uint64_t, WindowVisibilityState>> currVisibleData;
    SceneSessionManager::GetInstance().DealwithVisibilityChange(currVisibleData);

    currVisibleData.push_back(std::make_pair(0, WindowVisibilityState::WINDOW_VISIBILITY_STATE_NO_OCCLUSION));
    SceneSessionManager::GetInstance().DealwithVisibilityChange(currVisibleData);

    currVisibleData.push_back(std::make_pair(2, WindowVisibilityState::WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION));

    SessionInfo sessionInfo;
    sessionInfo.windowType_ = static_cast<uint32_t>(WindowType::APP_SUB_WINDOW_END);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    sceneSession->surfaceNode_ = std::make_shared<RSSurfaceNode>(rsSurfaceNodeConfig, true, 0);
    EXPECT_EQ(WindowType::APP_SUB_WINDOW_END, sceneSession->GetWindowType());
    SceneSessionManager::GetInstance().sceneSessionMap_.emplace(0, sceneSession);

    SessionInfo sessionInfo1;
    sessionInfo1.windowType_ = static_cast<uint32_t>(WindowType::APP_SUB_WINDOW_BASE);
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo1, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    sceneSession1->surfaceNode_ = std::make_shared<RSSurfaceNode>(rsSurfaceNodeConfig, true, 2);
    EXPECT_EQ(WindowType::APP_SUB_WINDOW_BASE, sceneSession1->GetWindowType());
    SceneSessionManager::GetInstance().sceneSessionMap_.emplace(2, sceneSession);
    SceneSessionManager::GetInstance().DealwithVisibilityChange(currVisibleData);
}

/**
 * @tc.name: DealwithVisibilityChange1
 * @tc.desc: test function : DealwithVisibilityChange1
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, DealwithVisibilityChange1, Function | SmallTest | Level3)
{
    std::vector<std::pair<uint64_t, WindowVisibilityState>> currVisibleData;
    currVisibleData.push_back(std::make_pair(0, WindowVisibilityState::WINDOW_VISIBILITY_STATE_NO_OCCLUSION));
    currVisibleData.push_back(std::make_pair(1, WindowVisibilityState::WINDOW_VISIBILITY_STATE_PARTICALLY_OCCLUSION));

    SessionInfo sessionInfo;
    sessionInfo.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_DIALOG);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    sceneSession->surfaceNode_ = std::make_shared<RSSurfaceNode>(rsSurfaceNodeConfig, true, 0);
    EXPECT_EQ(WindowType::WINDOW_TYPE_DIALOG, sceneSession->GetWindowType());
    sceneSession->SetParentSession(nullptr);
    SceneSessionManager::GetInstance().sceneSessionMap_.emplace(0, sceneSession);

    SessionInfo sessionInfo1;
    sessionInfo1.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_DIALOG);
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    sceneSession1->surfaceNode_ = std::make_shared<RSSurfaceNode>(rsSurfaceNodeConfig, true, 1);
    sceneSession1->SetParentSession(sceneSession1);
    SceneSessionManager::GetInstance().sceneSessionMap_.emplace(1, sceneSession);
    SceneSessionManager::GetInstance().DealwithVisibilityChange(currVisibleData);
}

/**
 * @tc.name: PostProcessFocus
 * @tc.desc: test function : PostProcessFocus
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, PostProcessFocus, Function | SmallTest | Level3)
{
    SceneSessionManager::GetInstance().sceneSessionMap_.emplace(0, nullptr);
    SceneSessionManager::GetInstance().PostProcessFocus();
    SceneSessionManager::GetInstance().sceneSessionMap_.clear();

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    PostProcessFocusState state;
    EXPECT_EQ(false, state.enabled_);
    sceneSession->SetPostProcessFocusState(state);
    SceneSessionManager::GetInstance().sceneSessionMap_.emplace(0, sceneSession);
    SceneSessionManager::GetInstance().PostProcessFocus();

    state.enabled_ = true;
    state.isFocused_ = false;
    sceneSession->SetPostProcessFocusState(state);
    SceneSessionManager::GetInstance().PostProcessFocus();

    state.isFocused_ = true;
    state.reason_ = FocusChangeReason::SCB_START_APP;
    sceneSession->SetPostProcessFocusState(state);
    SceneSessionManager::GetInstance().PostProcessFocus();

    sceneSession->SetPostProcessFocusState(state);
    state.reason_ = FocusChangeReason::DEFAULT;
    SceneSessionManager::GetInstance().PostProcessFocus();
}

/**
 * @tc.name: PostProcessProperty
 * @tc.desc: test function : PostProcessProperty
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, PostProcessProperty, Function | SmallTest | Level3)
{
    SceneSessionManager::GetInstance().sceneSessionMap_.emplace(0, nullptr);
    SceneSessionManager::GetInstance().PostProcessProperty();
    SceneSessionManager::GetInstance().sceneSessionMap_.clear();

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sessionInfo.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_DIALOG);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    PostProcessFocusState state;
    EXPECT_EQ(false, state.enabled_);
    sceneSession->SetPostProcessFocusState(state);
    SceneSessionManager::GetInstance().sceneSessionMap_.emplace(0, sceneSession);
    SceneSessionManager::GetInstance().PostProcessFocus();

    state.enabled_ = true;
    sceneSession->SetPostProcessFocusState(state);
    SceneSessionManager::GetInstance().PostProcessFocus();

    sessionInfo.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    SceneSessionManager::GetInstance().PostProcessFocus();
}

/**
 * @tc.name: NotifyUpdateRectAfterLayout
 * @tc.desc: test function : NotifyUpdateRectAfterLayout
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, NotifyUpdateRectAfterLayout, Function | SmallTest | Level3)
{
    SceneSessionManager::GetInstance().sceneSessionMap_.emplace(0, nullptr);
    SceneSessionManager::GetInstance().NotifyUpdateRectAfterLayout();
    SceneSessionManager::GetInstance().sceneSessionMap_.clear();

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    SceneSessionManager::GetInstance().sceneSessionMap_.emplace(0, sceneSession);
    SceneSessionManager::GetInstance().NotifyUpdateRectAfterLayout();
}

/**
 * @tc.name: DestroyExtensionSession
 * @tc.desc: test function : DestroyExtensionSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, DestroyExtensionSession, Function | SmallTest | Level3)
{
    SceneSessionManager::GetInstance().remoteExtSessionMap_.clear();
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    sptr<IRemoteObject> token = new IRemoteObjectMocker();
    EXPECT_NE(nullptr, iRemoteObjectMocker);
    SceneSessionManager::GetInstance().DestroyExtensionSession(iRemoteObjectMocker);
    SceneSessionManager::GetInstance().remoteExtSessionMap_.emplace(iRemoteObjectMocker, token);

    SceneSessionManager::GetInstance().extSessionInfoMap_.clear();
    SceneSessionManager::GetInstance().DestroyExtensionSession(iRemoteObjectMocker);

    ExtensionWindowAbilityInfo extensionWindowAbilituInfo;
    SceneSessionManager::GetInstance().extSessionInfoMap_.emplace(token, extensionWindowAbilituInfo);

    SceneSessionManager::GetInstance().sceneSessionMap_.emplace(0, nullptr);
    SceneSessionManager::GetInstance().DestroyExtensionSession(iRemoteObjectMocker);
    SceneSessionManager::GetInstance().sceneSessionMap_.clear();

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest2";
    sessionInfo.abilityName_ = "DumpSessionWithId";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    SceneSessionManager::GetInstance().sceneSessionMap_.emplace(0, sceneSession);

    ExtensionWindowFlags extensionWindowFlags;
    sceneSession->combinedExtWindowFlags_ = extensionWindowFlags;
    SceneSessionManager::GetInstance().DestroyExtensionSession(iRemoteObjectMocker);

    extensionWindowFlags.waterMarkFlag = false;
    extensionWindowFlags.privacyModeFlag = false;
    sceneSession->combinedExtWindowFlags_ = extensionWindowFlags;
    EXPECT_EQ(false, sceneSession->combinedExtWindowFlags_.privacyModeFlag);
    SceneSessionManager::GetInstance().DestroyExtensionSession(iRemoteObjectMocker);
}

/**
 * @tc.name: GetRemoteSessionSnapshotInfo
 * @tc.desc: GetRemoteSessionSnapshotInfo set gesture navigation enabled
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, GetRemoteSessionSnapshotInfo, Function | SmallTest | Level3)
{
    AAFwk::MissionSnapshot sessionSnapshot;
    std::string deviceId = "deviceId";
    int res = SceneSessionManager::GetInstance().GetRemoteSessionSnapshotInfo(
        deviceId, 8, sessionSnapshot);
    EXPECT_EQ(ERR_NULL_OBJECT, res);
}

}
}
}