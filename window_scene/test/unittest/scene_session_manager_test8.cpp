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
    sptr<SceneSessionManager> ssm_;
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
    ssm_ = sptr<SceneSessionManager>::MakeSptr();
    EXPECT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerTest8::TearDown()
{
    ssm_->sceneSessionMap_.clear();
    usleep(WAIT_SYNC_IN_NS);
    ssm_ = nullptr;
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
    ssm_->SetDumpUITreeFunc(nullptr);
    EXPECT_EQ(WSError::WS_OK, ssm_->GetTotalUITreeInfo(strId, dumpInfo));
    DumpUITreeFunc func = [](uint64_t, std::string& dumpInfo) {
        return;
    };
    ssm_->SetDumpUITreeFunc(func);
    EXPECT_EQ(WSError::WS_OK, ssm_->GetTotalUITreeInfo(strId, dumpInfo));
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
    WMError res = ssm_->RequestFocusStatusBySCB(8, false, false, reason);
    EXPECT_EQ(WMError::WM_OK, res);

    reason = FocusChangeReason::FOREGROUND;
    res = ssm_->RequestFocusStatusBySCB(8, true, false, reason);
    EXPECT_EQ(WMError::WM_OK, res);

    reason = FocusChangeReason::MOVE_UP;
    ssm_->sceneSessionMap_.insert({ 5, sceneSession });
    res = ssm_->RequestFocusStatusBySCB(5, true, false, reason);
    EXPECT_EQ(WMError::WM_OK, res);

    res = ssm_->RequestFocusStatusBySCB(8, true, false, reason);
    EXPECT_EQ(WMError::WM_OK, res);

    reason = FocusChangeReason::DEFAULT;
    res = ssm_->RequestFocusStatusBySCB(8, true, true, reason);
    EXPECT_EQ(WMError::WM_OK, res);

    res = ssm_->RequestFocusStatusBySCB(5, true, true, reason);
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
    std::string deviceId = "";
    int res = ssm_->GetRemoteSessionSnapshotInfo(deviceId, 8, sessionSnapshot);
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
    ssm_->WindowLayerInfoChangeCallback(rsData);

    rsData = std::make_shared<RSOcclusionData>();
    ASSERT_NE(nullptr, rsData);
    ssm_->WindowLayerInfoChangeCallback(rsData);

    VisibleData visibleData;
    visibleData.push_back(std::make_pair(0, WINDOW_LAYER_INFO_TYPE::ALL_VISIBLE));
    visibleData.push_back(std::make_pair(1, WINDOW_LAYER_INFO_TYPE::SEMI_VISIBLE));
    visibleData.push_back(std::make_pair(2, WINDOW_LAYER_INFO_TYPE::INVISIBLE));
    visibleData.push_back(std::make_pair(3, WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_DYNAMIC_STATUS));
    visibleData.push_back(std::make_pair(4, WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_STATIC_STATUS));
    visibleData.push_back(std::make_pair(5, WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_UNKNOWN_TYPE));
    rsData = std::make_shared<RSOcclusionData>(visibleData);
    ASSERT_NE(nullptr, rsData);
    ssm_->WindowLayerInfoChangeCallback(rsData);
}

/**
 * @tc.name: DealwithVisibilityChange
 * @tc.desc: test function : DealwithVisibilityChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, DealwithVisibilityChange, Function | SmallTest | Level3)
{
    std::vector<std::pair<uint64_t, WindowVisibilityState>> currVisibleData;
    ssm_->DealwithVisibilityChange(currVisibleData);

    currVisibleData.push_back(std::make_pair(0, WindowVisibilityState::WINDOW_VISIBILITY_STATE_NO_OCCLUSION));
    ssm_->DealwithVisibilityChange(currVisibleData);

    currVisibleData.push_back(std::make_pair(2, WindowVisibilityState::WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION));

    SessionInfo sessionInfo;
    sessionInfo.windowType_ = static_cast<uint32_t>(WindowType::APP_SUB_WINDOW_END);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    sceneSession->surfaceNode_ = std::make_shared<RSSurfaceNode>(rsSurfaceNodeConfig, true, 0);
    EXPECT_EQ(WindowType::APP_SUB_WINDOW_END, sceneSession->GetWindowType());
    ssm_->sceneSessionMap_.emplace(0, sceneSession);

    SessionInfo sessionInfo1;
    sessionInfo1.windowType_ = static_cast<uint32_t>(WindowType::APP_SUB_WINDOW_BASE);
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo1, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    sceneSession1->surfaceNode_ = std::make_shared<RSSurfaceNode>(rsSurfaceNodeConfig, true, 2);
    EXPECT_EQ(WindowType::APP_SUB_WINDOW_BASE, sceneSession1->GetWindowType());
    ssm_->sceneSessionMap_.emplace(2, sceneSession);
    ssm_->DealwithVisibilityChange(currVisibleData);
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
    ssm_->sceneSessionMap_.emplace(0, sceneSession);

    SessionInfo sessionInfo1;
    sessionInfo1.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_DIALOG);
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    sceneSession1->surfaceNode_ = std::make_shared<RSSurfaceNode>(rsSurfaceNodeConfig, true, 1);
    sceneSession1->SetParentSession(sceneSession1);
    ssm_->sceneSessionMap_.emplace(1, sceneSession);
    ssm_->DealwithVisibilityChange(currVisibleData);
}

/**
 * @tc.name: PostProcessFocus
 * @tc.desc: test function : PostProcessFocus
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, PostProcessFocus, Function | SmallTest | Level3)
{
    ssm_->sceneSessionMap_.emplace(0, nullptr);
    ssm_->PostProcessFocus();
    ssm_->sceneSessionMap_.clear();

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "PostProcessFocus";
    sessionInfo.abilityName_ = "PostProcessFocus";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    PostProcessFocusState state;
    EXPECT_EQ(false, state.enabled_);
    sceneSession->SetPostProcessFocusState(state);
    ssm_->sceneSessionMap_.emplace(0, sceneSession);
    ssm_->PostProcessFocus();

    state.enabled_ = true;
    state.isFocused_ = false;
    sceneSession->SetPostProcessFocusState(state);
    ssm_->PostProcessFocus();

    state.isFocused_ = true;
    state.reason_ = FocusChangeReason::SCB_START_APP;
    sceneSession->SetPostProcessFocusState(state);
    ssm_->PostProcessFocus();

    sceneSession->SetPostProcessFocusState(state);
    state.reason_ = FocusChangeReason::DEFAULT;
    ssm_->PostProcessFocus();
}

/**
 * @tc.name: PostProcessProperty
 * @tc.desc: test function : PostProcessProperty
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, PostProcessProperty, Function | SmallTest | Level3)
{
    ssm_->sceneSessionMap_.emplace(0, nullptr);
    ssm_->PostProcessProperty();
    ssm_->sceneSessionMap_.clear();

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "PostProcessProperty";
    sessionInfo.abilityName_ = "PostProcessProperty";
    sessionInfo.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_DIALOG);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    PostProcessFocusState state;
    EXPECT_EQ(false, state.enabled_);
    sceneSession->SetPostProcessFocusState(state);
    ssm_->sceneSessionMap_.emplace(0, sceneSession);
    ssm_->PostProcessFocus();

    state.enabled_ = true;
    sceneSession->SetPostProcessFocusState(state);
    ssm_->PostProcessFocus();

    sessionInfo.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ssm_->PostProcessFocus();
}

/**
 * @tc.name: NotifyUpdateRectAfterLayout
 * @tc.desc: test function : NotifyUpdateRectAfterLayout
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, NotifyUpdateRectAfterLayout, Function | SmallTest | Level3)
{
    ssm_->sceneSessionMap_.emplace(0, nullptr);
    ssm_->NotifyUpdateRectAfterLayout();
    ssm_->sceneSessionMap_.clear();

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "NotifyUpdateRectAfterLayout";
    sessionInfo.abilityName_ = "NotifyUpdateRectAfterLayout";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.emplace(0, sceneSession);
    ssm_->NotifyUpdateRectAfterLayout();
    constexpr uint32_t NOT_WAIT_SYNC_IN_NS = 500000;
    usleep(NOT_WAIT_SYNC_IN_NS);
}

/**
 * @tc.name: DestroyExtensionSession
 * @tc.desc: test function : DestroyExtensionSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, DestroyExtensionSession, Function | SmallTest | Level3)
{
    ssm_->remoteExtSessionMap_.clear();
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    sptr<IRemoteObject> token = new IRemoteObjectMocker();
    EXPECT_NE(nullptr, iRemoteObjectMocker);
    ssm_->DestroyExtensionSession(iRemoteObjectMocker);
    ssm_->remoteExtSessionMap_.emplace(iRemoteObjectMocker, token);

    ssm_->extSessionInfoMap_.clear();
    ssm_->DestroyExtensionSession(iRemoteObjectMocker);

    ExtensionWindowAbilityInfo extensionWindowAbilituInfo;
    ssm_->extSessionInfoMap_.emplace(token, extensionWindowAbilituInfo);

    ssm_->sceneSessionMap_.emplace(0, nullptr);
    ssm_->DestroyExtensionSession(iRemoteObjectMocker);
    ssm_->sceneSessionMap_.clear();

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "DestroyExtensionSession";
    sessionInfo.abilityName_ = "DestroyExtensionSession";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.emplace(0, sceneSession);

    ExtensionWindowFlags extensionWindowFlags;
    sceneSession->combinedExtWindowFlags_ = extensionWindowFlags;
    ssm_->DestroyExtensionSession(iRemoteObjectMocker);

    extensionWindowFlags.waterMarkFlag = false;
    extensionWindowFlags.privacyModeFlag = false;
    sceneSession->combinedExtWindowFlags_ = extensionWindowFlags;
    EXPECT_EQ(false, sceneSession->combinedExtWindowFlags_.privacyModeFlag);
    ssm_->DestroyExtensionSession(iRemoteObjectMocker);
}

/**
 * @tc.name: FilterSceneSessionCovered
 * @tc.desc: test function : FilterSceneSessionCovered
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, FilterSceneSessionCovered, Function | SmallTest | Level3)
{
    std::vector<sptr<SceneSession>> sceneSessionList;
    sptr<SceneSession> sceneSession = nullptr;
    sceneSessionList.emplace_back(sceneSession);
    EXPECT_EQ(1, sceneSessionList.size());
    ssm_->FilterSceneSessionCovered(sceneSessionList);

    SessionInfo sessionInfo;
    sceneSessionList.clear();
    sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    EXPECT_NE(nullptr, sceneSession);
    EXPECT_EQ(WSError::WS_OK, sceneSession->SetSessionProperty(nullptr));
    sceneSessionList.emplace_back(sceneSession);
    ssm_->FilterSceneSessionCovered(sceneSessionList);
}

/**
 * @tc.name: WindowDestroyNotifyVisibility
 * @tc.desc: test function : WindowDestroyNotifyVisibility
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, WindowDestroyNotifyVisibility, Function | SmallTest | Level3)
{
    sptr<SceneSession> sceneSession = nullptr;
    ssm_->WindowDestroyNotifyVisibility(sceneSession);

    SessionInfo sessionInfo;
    sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    EXPECT_NE(nullptr, sceneSession);
    EXPECT_EQ(WSError::WS_OK, sceneSession->SetRSVisible(true));
    ssm_->WindowDestroyNotifyVisibility(sceneSession);
}

/**
 * @tc.name: UpdateSubWindowVisibility
 * @tc.desc: test function : UpdateSubWindowVisibility
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, UpdateSubWindowVisibility, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "UpdateSubWindowVisibility";
    sessionInfo.abilityName_ = "UpdateSubWindowVisibility";
    sessionInfo.windowType_ = static_cast<uint32_t>(WindowType::APP_SUB_WINDOW_BASE);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    EXPECT_NE(nullptr, sceneSession);
    WindowVisibilityState visibleState = WindowVisibilityState::WINDOW_VISIBILITY_STATE_NO_OCCLUSION;
    std::vector<std::pair<uint64_t, WindowVisibilityState>> visibilityChangeInfo;
    std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
    std::string visibilityInfo = "";
    sceneSession->persistentId_ = 1998;
    sceneSession->SetCallingUid(1998);
    SessionState state = SessionState::STATE_CONNECT;
    sceneSession->SetSessionState(state);
    sceneSession->SetParentSession(sceneSession);
    EXPECT_EQ(1998, sceneSession->GetParentSession()->GetWindowId());
    ssm_->sceneSessionMap_.emplace(0, sceneSession);

    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    EXPECT_NE(nullptr, sceneSession1);
    sceneSession1->persistentId_ = 1998;
    sceneSession1->SetCallingUid(1024);
    SessionState state1 = SessionState::STATE_CONNECT;
    sceneSession1->SetSessionState(state1);
    sceneSession1->SetParentSession(sceneSession1);
    EXPECT_EQ(1998, sceneSession1->GetParentSession()->GetWindowId());
    ssm_->sceneSessionMap_.emplace(0, sceneSession1);

    sptr<SceneSession> sceneSession2 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    EXPECT_NE(nullptr, sceneSession2);
    sceneSession2->persistentId_ = 1998;
    sceneSession2->SetCallingUid(1998);
    SessionState state2 = SessionState::STATE_FOREGROUND;
    sceneSession2->SetSessionState(state2);
    sceneSession2->SetParentSession(sceneSession2);
    EXPECT_EQ(1998, sceneSession2->GetParentSession()->GetWindowId());
    ssm_->sceneSessionMap_.emplace(0, sceneSession2);
    ssm_->UpdateSubWindowVisibility(sceneSession,
        visibleState, visibilityChangeInfo, windowVisibilityInfos, visibilityInfo);
}

/**
 * @tc.name: GetOrientationFromResourceManager
 * @tc.desc: test function : GetOrientationFromResourceManager
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, GetOrientationFromResourceManager, Function | SmallTest | Level3)
{
    AppExecFwk::AbilityInfo abilityInfo;
    abilityInfo.orientationId = 0;
    ssm_->GetOrientationFromResourceManager(abilityInfo);

    abilityInfo.orientationId = 20;
    abilityInfo.hapPath = "";
    ssm_->GetOrientationFromResourceManager(abilityInfo);
    EXPECT_EQ(true, abilityInfo.hapPath.empty());
}

/**
 * @tc.name: RegisterSessionChangeByActionNotifyManagerFunc
 * @tc.desc: test function : RegisterSessionChangeByActionNotifyManagerFunc
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, RegisterSessionChangeByActionNotifyManagerFunc, Function | SmallTest | Level3)
{
    sptr<SceneSession> sceneSession = nullptr;
    ssm_->RegisterSessionChangeByActionNotifyManagerFunc(sceneSession);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "RegisterSessionChangeByActionNotifyManagerFunc";
    sessionInfo.abilityName_ = "RegisterSessionChangeByActionNotifyManagerFunc";
    sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    EXPECT_NE(nullptr, sceneSession);
    ssm_->RegisterSessionChangeByActionNotifyManagerFunc(sceneSession);
    EXPECT_NE(nullptr, sceneSession->sessionChangeByActionNotifyManagerFunc_);

    sptr<SceneSession> sceneSession1 = nullptr;
    sptr<WindowSessionProperty> property = nullptr;

    sceneSession->NotifySessionChangeByActionNotifyManager(sceneSession1, property,
        WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON);
    
    sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    EXPECT_NE(nullptr, sceneSession1);
    sceneSession->NotifySessionChangeByActionNotifyManager(sceneSession1, property,
        WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON);

    property = sptr<WindowSessionProperty>::MakeSptr();
    EXPECT_NE(nullptr, property);

    sceneSession->NotifySessionChangeByActionNotifyManager(sceneSession1, property,
        WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON);
}

/**
 * @tc.name: RegisterSessionChangeByActionNotifyManagerFunc1
 * @tc.desc: test function : RegisterSessionChangeByActionNotifyManagerFunc1
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, RegisterSessionChangeByActionNotifyManagerFunc1, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "RegisterSessionChangeByActionNotifyManagerFunc1";
    sessionInfo.abilityName_ = "RegisterSessionChangeByActionNotifyManagerFunc1";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    EXPECT_NE(nullptr, sceneSession);

    ssm_->RegisterSessionChangeByActionNotifyManagerFunc(sceneSession);
    EXPECT_NE(nullptr, sceneSession->sessionChangeByActionNotifyManagerFunc_);

    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    EXPECT_NE(nullptr, sceneSession1);

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    EXPECT_NE(nullptr, property);

    sceneSession->NotifySessionChangeByActionNotifyManager(sceneSession1, property,
        WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON);

    sceneSession->NotifySessionChangeByActionNotifyManager(sceneSession1, property,
        WSPropertyChangeAction::ACTION_UPDATE_NAVIGATION_INDICATOR_PROPS);
    
    sceneSession->NotifySessionChangeByActionNotifyManager(sceneSession1, property,
        WSPropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS);
    
    sceneSession->NotifySessionChangeByActionNotifyManager(sceneSession1, property,
        WSPropertyChangeAction::ACTION_UPDATE_SYSTEM_PRIVACY_MODE);
    
    sceneSession->NotifySessionChangeByActionNotifyManager(sceneSession1, property,
        WSPropertyChangeAction::ACTION_UPDATE_FLAGS);

    sceneSession->NotifySessionChangeByActionNotifyManager(sceneSession1, property,
        WSPropertyChangeAction::ACTION_UPDATE_MODE);
    
    sceneSession->NotifySessionChangeByActionNotifyManager(sceneSession1, property,
        WSPropertyChangeAction::ACTION_UPDATE_HIDE_NON_SYSTEM_FLOATING_WINDOWS);
    
    sceneSession->NotifySessionChangeByActionNotifyManager(sceneSession1, property,
        WSPropertyChangeAction::ACTION_UPDATE_WINDOW_MASK);

    sceneSession->NotifySessionChangeByActionNotifyManager(sceneSession1, property,
        WSPropertyChangeAction::ACTION_UPDATE_TOPMOST);
}

/**
 * @tc.name: RegisterRequestFocusStatusNotifyManagerFunc
 * @tc.desc: test function : RegisterRequestFocusStatusNotifyManagerFunc
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, RegisterRequestFocusStatusNotifyManagerFunc, Function | SmallTest | Level3)
{
    sptr<SceneSession> sceneSession = nullptr;
    ssm_->RegisterRequestFocusStatusNotifyManagerFunc(sceneSession);
    EXPECT_EQ(nullptr, sceneSession);
}

/**
 * @tc.name: CheckRequestFocusImmdediately
 * @tc.desc: test function : CheckRequestFocusImmdediately
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, CheckRequestFocusImmdediately, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "CheckRequestFocusImmdediately";
    sessionInfo.abilityName_ = "CheckRequestFocusImmdediately";
    sessionInfo.windowType_ = static_cast<uint32_t>(WindowType::APP_SUB_WINDOW_BASE);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    EXPECT_NE(nullptr, sceneSession);
    EXPECT_EQ(WindowType::APP_SUB_WINDOW_BASE, sceneSession->GetWindowType());
    ssm_->CheckRequestFocusImmdediately(sceneSession);
}

/**
 * @tc.name: HandleTurnScreenOn
 * @tc.desc: test function : HandleTurnScreenOn
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, HandleTurnScreenOn, Function | SmallTest | Level3)
{
    sptr<SceneSession> sceneSession = nullptr;
    ssm_->HandleTurnScreenOn(sceneSession);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "HandleTurnScreenOn";
    sessionInfo.abilityName_ = "HandleTurnScreenOn";
    sessionInfo.windowType_ = static_cast<uint32_t>(WindowType::APP_SUB_WINDOW_BASE);
    sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    EXPECT_NE(nullptr, sceneSession);
    sceneSession->GetSessionProperty()->SetTurnScreenOn(false);
    ssm_->HandleTurnScreenOn(sceneSession);
    EXPECT_EQ(false, sceneSession->GetSessionProperty()->IsTurnScreenOn());
    sceneSession->GetSessionProperty()->SetTurnScreenOn(true);
    ssm_->HandleTurnScreenOn(sceneSession);
    constexpr uint32_t NOT_WAIT_SYNC_IN_NS = 500000;
    usleep(NOT_WAIT_SYNC_IN_NS);
}
/**
 * @tc.name: HandleKeepScreenOn
 * @tc.desc: test function : HandleKeepScreenOn
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, HandleKeepScreenOn, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "HandleTurnScreenOn";
    sessionInfo.abilityName_ = "HandleTurnScreenOn";
    sessionInfo.windowType_ = static_cast<uint32_t>(WindowType::APP_SUB_WINDOW_BASE);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    EXPECT_NE(nullptr, sceneSession);

    ssm_->HandleKeepScreenOn(sceneSession, false);
    sceneSession->keepScreenLock_ = nullptr;
    ssm_->HandleKeepScreenOn(sceneSession, true);
    bool enable = true;
    EXPECT_EQ(WSError::WS_OK, ssm_->GetFreeMultiWindowEnableState(enable));
}

HWTEST_F(SceneSessionManagerTest8, HandleKeepScreenOn, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "HandleTurnScreenOn";
    sessionInfo.abilityName_ = "HandleTurnScreenOn";
    sessionInfo.windowType_ = static_cast<uint32_t>(WindowType::APP_SUB_WINDOW_BASE);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    EXPECT_NE(nullptr, sceneSession);

    ssm_->HandleKeepScreenOn(sceneSession, false);
    sceneSession->keepScreenLock_ = nullptr;
    ssm_->HandleKeepScreenOn(sceneSession, true);
    bool enable = true;
    EXPECT_EQ(WSError::WS_OK, ssm_->GetFreeMultiWindowEnableState(enable));
}

}
} // namespace Rosen
} // namespace OHOS