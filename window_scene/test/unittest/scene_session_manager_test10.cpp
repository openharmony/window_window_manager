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
#include "iremote_object_mocker.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "session_manager.h"
#include "screen_session_manager_client/include/screen_session_manager_client.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class SceneSessionManagerTest10 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void InitTestSceneSession(DisplayId displayId, int32_t windowId, int32_t zOrder, bool visible, WSRect rect);

    static sptr<SceneSessionManager> ssm_;
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

sptr<SceneSessionManager> SceneSessionManagerTest10::ssm_ = nullptr;

void NotifyRecoverSceneSessionFuncTest(const sptr<SceneSession>& session, const SessionInfo& sessionInfo)
{
}

bool TraverseFuncTest(const sptr<SceneSession>& session)
{
    return true;
}

void WindowChangedFuncTest(int32_t persistentId, WindowUpdateType type)
{
}

void ProcessStatusBarEnabledChangeFuncTest(bool enable)
{
}

void DumpRootSceneElementInfoFuncTest(const std::vector<std::string>& params, std::vector<std::string>& infos)
{
}

void SceneSessionManagerTest10::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerTest10::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerTest10::SetUp()
{
}

void SceneSessionManagerTest10::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
}

void SceneSessionManagerTest10::InitTestSceneSession(DisplayId displayId,
    int32_t windowId, int32_t zOrder, bool visible, WSRect rect)
{
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetDisplayId(displayId);
    SessionInfo info;
    info.bundleName_ = "root";
    info.persistentId_ = windowId;
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetZOrder(zOrder);
    sceneSession->SetRSVisible(visible);
    sceneSession->SetSessionRect(rect);
    sceneSession->SetSessionProperty(property);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});
    EXPECT_EQ(windowId, sceneSession->GetPersistentId());
}

namespace {
/**
 * @tc.name: RequestSceneSessionDestructionInner
 * @tc.desc: Test RequestSceneSessionDestructionInner with CollaboratorType RESERVE_TYPE
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, RequestSceneSessionDestructionInner, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    SessionInfo info;
    sptr<SceneSession::SpecificSessionCallback> specificCallback = nullptr;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCallback);
    sptr<AAFwk::SessionInfo> sceneSessionInfo = sptr<AAFwk::SessionInfo>::MakeSptr();
    bool needRemoveSession = true;
    bool isForceClean = true;

    SessionInfo sessionInfo;
    sessionInfo.collaboratorType_ = CollaboratorType::RESERVE_TYPE;
    auto res = ssm_->RequestSceneSessionDestructionInner(sceneSession, sceneSessionInfo,
        needRemoveSession, isForceClean);
    ASSERT_EQ(res, WSError::WS_OK);
}

/**
 * @tc.name: TestRequestSceneSessionDestructionInner_01
 * @tc.desc: Test RequestSceneSessionDestructionInner with CollaboratorType DEFAULT_TYPE
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestRequestSceneSessionDestructionInner_01, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    SessionInfo info;
    sptr<SceneSession::SpecificSessionCallback> specificCallback = nullptr;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCallback);
    sptr<AAFwk::SessionInfo> sceneSessionInfo = sptr<AAFwk::SessionInfo>::MakeSptr();
    bool needRemoveSession = false;
    bool isForceClean = false;

    SessionInfo sessionInfo;
    sessionInfo.collaboratorType_ = CollaboratorType::DEFAULT_TYPE;
    sessionInfo.want = std::make_shared<AAFwk::Want>();
    ssm_->listenerController_ = std::make_shared<SessionListenerController>();
    auto res = ssm_->RequestSceneSessionDestructionInner(sceneSession, sceneSessionInfo,
        needRemoveSession, isForceClean);
    ASSERT_EQ(res, WSError::WS_OK);
}

/**
 * @tc.name: TestRegisterWindowManagerAgent_01
 * @tc.desc: Test RegisterWindowManagerAgent with WindowManagerAgentType WINDOW_MANAGER_AGENT_TYPE_SYSTEM_BAR
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestRegisterWindowManagerAgent_01, Function | SmallTest | Level3)
{
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_SYSTEM_BAR;
    sptr<IWindowManagerAgent> windowManagerAgent;
    auto res = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    ASSERT_EQ(res, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: TestRegisterWindowManagerAgent_02
 * @tc.desc: Test RegisterWindowManagerAgent with AgentType WINDOW_MANAGER_AGENT_TYPE_GESTURE_NAVIGATION_ENABLED
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestRegisterWindowManagerAgent_02, Function | SmallTest | Level3)
{
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_GESTURE_NAVIGATION_ENABLED;
    sptr<IWindowManagerAgent> windowManagerAgent;
    auto res = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    ASSERT_EQ(res, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: TestRegisterWindowManagerAgent_03
 * @tc.desc: Test RegisterWindowManagerAgent with WindowManagerAgentType WINDOW_MANAGER_AGENT_TYPE_WATER_MARK_FLAG
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestRegisterWindowManagerAgent_03, Function | SmallTest | Level3)
{
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WATER_MARK_FLAG;
    sptr<IWindowManagerAgent> windowManagerAgent;
    auto res = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    ASSERT_EQ(res, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: TestRegisterWindowManagerAgent_04
 * @tc.desc: Test RegisterWindowManagerAgent with WindowManagerAgentType WINDOW_MANAGER_AGENT_TYPE_WINDOW_UPDATE
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestRegisterWindowManagerAgent_04, Function | SmallTest | Level3)
{
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_UPDATE;
    sptr<IWindowManagerAgent> windowManagerAgent;
    auto res = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    ASSERT_EQ(res, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: TestRegisterWindowManagerAgent_05
 * @tc.desc: Test RegisterWindowManagerAgent with WindowManagerAgentType WINDOW_MANAGER_AGENT_TYPE_WINDOW_VISIBILITY
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestRegisterWindowManagerAgent_05, Function | SmallTest | Level3)
{
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_VISIBILITY;
    sptr<IWindowManagerAgent> windowManagerAgent;
    auto res = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: TestRegisterWindowManagerAgent_06
 * @tc.desc: Test RegisterWindowManagerAgent with WindowManagerAgentType WINDOW_MANAGER_AGENT_TYPE_FOCUS
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestRegisterWindowManagerAgent_06, Function | SmallTest | Level3)
{
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    sptr<IWindowManagerAgent> windowManagerAgent;
    auto res = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: TestRegisterWindowManagerAgent_07
 * @tc.desc: Test RegisterWindowManagerAgent with WindowManagerAgentType WINDOW_MANAGER_AGENT_TYPE_WINDOW_DRAWING_STATE
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestRegisterWindowManagerAgent_07, Function | SmallTest | Level3)
{
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_DRAWING_STATE;
    sptr<IWindowManagerAgent> windowManagerAgent;
    auto res = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: TestRegisterWindowManagerAgent_08
 * @tc.desc: Test RegisterWindowManagerAgent with WindowManagerAgentType WINDOW_MANAGER_AGENT_TYPE_VISIBLE_WINDOW_NUM
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestRegisterWindowManagerAgent_08, Function | SmallTest | Level3)
{
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_VISIBLE_WINDOW_NUM;
    sptr<IWindowManagerAgent> windowManagerAgent;
    auto res = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: TestRegisterWindowManagerAgent_09
 * @tc.desc: Test RegisterWindowManagerAgent with WindowManagerAgentType WINDOW_MANAGER_AGENT_TYPE_WINDOW_MODE
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestRegisterWindowManagerAgent_09, Function | SmallTest | Level3)
{
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_MODE;
    sptr<IWindowManagerAgent> windowManagerAgent;
    auto res = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: TestRegisterWindowManagerAgent_010
 * @tc.desc: Test RegisterWindowManagerAgent with WindowManagerAgentType WINDOW_MANAGER_AGENT_TYPE_WINDOW_PID_VISIBILITY
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestRegisterWindowManagerAgent_010, Function | SmallTest | Level3)
{
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_PID_VISIBILITY;
    sptr<IWindowManagerAgent> windowManagerAgent;
    auto res = ssm_->RegisterWindowManagerAgent(type, windowManagerAgent);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: TestUpdateRotateAnimationConfig_01
 * @tc.desc: Test UpdateRotateAnimationConfig with duration_ 400
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestUpdateRotateAnimationConfig_01, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    RotateAnimationConfig config = { 400 };
    ssm_->UpdateRotateAnimationConfig(config);
    usleep(WAIT_SYNC_IN_NS);
    ASSERT_EQ(ssm_->rotateAnimationConfig_.duration_, 400);
}

/**
 * @tc.name: TestUpdateRotateAnimationConfig_02
 * @tc.desc: Test UpdateRotateAnimationConfig with duration_ 600
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestUpdateRotateAnimationConfig_02, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    RotateAnimationConfig config = { 600 };
    ssm_->UpdateRotateAnimationConfig(config);
    usleep(WAIT_SYNC_IN_NS);
    ASSERT_EQ(ssm_->rotateAnimationConfig_.duration_, 600);
}

/**
 * @tc.name: RegisterAcquireRotateAnimationConfigFunc
 * @tc.desc: RegisterAcquireRotateAnimationConfigFunc
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, RegisterAcquireRotateAnimationConfigFunc, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "bundleName";
    sessionInfo.persistentId_ = 1;
    sessionInfo.isSystem_ = false;
    sessionInfo.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    ASSERT_NE(sessionInfo.abilityInfo, nullptr);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->scenePersistence_ = sptr<ScenePersistence>::MakeSptr("bundleName", 1);
    ASSERT_NE(sceneSession->scenePersistence_, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->RegisterAcquireRotateAnimationConfigFunc(sceneSession);
    WSRect rect({1, 1, 1, 1});
    SizeChangeReason reason = SizeChangeReason::ROTATION;
    WSError result = sceneSession->UpdateRect(rect, reason, "SceneSessionManagerTest10");
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: TestCheckLastFocusedAppSessionFocus_01
 * @tc.desc: Test CheckLastFocusedAppSessionFocus with GetPersistentId
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestCheckLastFocusedAppSessionFocus_01, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    SessionInfo info1;
    info1.abilityName_ = "focusedSession";
    info1.bundleName_ = "focusedSession";
    info1.windowType_ = 1;
    sptr<SceneSession> focusedSession = sptr<SceneSession>::MakeSptr(info1, nullptr);
    ASSERT_NE(focusedSession, nullptr);

    SessionInfo info2;
    info2.abilityName_ = "nextSession";
    info2.bundleName_ = "nextSession";
    info2.windowType_ = 1;
    sptr<SceneSession> nextSession = sptr<SceneSession>::MakeSptr(info2, nullptr);
    ASSERT_NE(nextSession, nullptr);

    ssm_->lastFocusedAppSessionId_ = nextSession->GetPersistentId();
    ASSERT_EQ(false, ssm_->CheckLastFocusedAppSessionFocus(focusedSession, nextSession));
}

/**
 * @tc.name: TestCheckLastFocusedAppSessionFocus_02
 * @tc.desc: Test CheckLastFocusedAppSessionFocus with WindowType WINDOW_TYPE_DIALOG
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestCheckLastFocusedAppSessionFocus_02, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    SessionInfo info1;
    info1.abilityName_ = "focusedSession";
    info1.bundleName_ = "focusedSession";
    info1.windowType_ = 1;
    sptr<SceneSession> focusedSession = sptr<SceneSession>::MakeSptr(info1, nullptr);

    SessionInfo info2;
    info2.abilityName_ = "nextSession";
    info2.bundleName_ = "nextSession";
    info2.windowType_ = 1;
    sptr<SceneSession> nextSession = sptr<SceneSession>::MakeSptr(info2, nullptr);

    ssm_->lastFocusedAppSessionId_ = 124;
    focusedSession->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ASSERT_EQ(false, ssm_->CheckLastFocusedAppSessionFocus(focusedSession, nextSession));

    nextSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    ASSERT_EQ(false, ssm_->CheckLastFocusedAppSessionFocus(focusedSession, nextSession));
    ASSERT_EQ(0, ssm_->lastFocusedAppSessionId_);
}

/**
 * @tc.name: GetWindowIdsByCoordinate01
 * @tc.desc: GetWindowIdsByCoordinate, displayId invalid
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, GetWindowIdsByCoordinate01, Function | SmallTest | Level3)
{
    std::vector<int32_t> windowIds;
    WMError result = ssm_->GetWindowIdsByCoordinate(DISPLAY_ID_INVALID, 0, 0, 0, windowIds);
    EXPECT_EQ(result, WMError::WM_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: GetWindowIdsByCoordinate02
 * @tc.desc: GetWindowIdsByCoordinate, windowNumber 0, x y invalid
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, GetWindowIdsByCoordinate02, Function | SmallTest | Level3)
{
    ssm_->sceneSessionMap_.clear();
    InitTestSceneSession(1, 101, 11, true, {100, 100, 200, 200});
    ssm_->sceneSessionMap_.insert({102, nullptr});
    InitTestSceneSession(1, 103, 14, true, {120, 120, 220, 220});
    InitTestSceneSession(1, 105, 12, true, {100, 100, 200, 200});
    auto it1 = ssm_->sceneSessionMap_.find(105);
    if (it1 != ssm_->sceneSessionMap_.end()) {
        it1->second->sessionInfo_.bundleName_ = "other";
    }
    InitTestSceneSession(1, 106, 15, true, {140, 140, 240, 240});
    InitTestSceneSession(2, 107, 15, true, {150, 150, 250, 250});
    InitTestSceneSession(1, 108, 13, false, {150, 150, 250, 250});
    InitTestSceneSession(1, 109, 13, true, {160, 160, 260, 260});
    InitTestSceneSession(1, 110, 12, true, {500, 500, 600, 600});

    std::vector<int32_t> windowIds;
    WMError result = ssm_->GetWindowIdsByCoordinate(1, 0, -1, -1, windowIds);
    EXPECT_EQ(result, WMError::WM_OK);
    EXPECT_EQ(5, windowIds.size());
    EXPECT_EQ(106, windowIds[0]);
    EXPECT_EQ(103, windowIds[1]);
    EXPECT_EQ(109, windowIds[2]);
    EXPECT_EQ(110, windowIds[3]);
    EXPECT_EQ(101, windowIds[4]);
    ssm_->sceneSessionMap_.clear();
}

/**
 * @tc.name: GetWindowIdsByCoordinate03
 * @tc.desc: GetWindowIdsByCoordinate, windowNumber 3, x y invalid
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, GetWindowIdsByCoordinate03, Function | SmallTest | Level3)
{
    ssm_->sceneSessionMap_.clear();
    InitTestSceneSession(1, 111, 11, true, {100, 100, 200, 200});
    ssm_->sceneSessionMap_.insert({102, nullptr});
    InitTestSceneSession(1, 113, 14, true, {120, 120, 220, 220});
    InitTestSceneSession(1, 114, 12, true, {100, 100, 200, 200});
    ASSERT_TRUE(ssm_->sceneSessionMap_.find(114) != ssm_->sceneSessionMap_.end());
    InitTestSceneSession(1, 115, 12, true, {100, 100, 200, 200});
    auto it1 = ssm_->sceneSessionMap_.find(115);
    if (it1 != ssm_->sceneSessionMap_.end()) {
        it1->second->sessionInfo_.bundleName_ = "other";
    }
    InitTestSceneSession(1, 116, 15, true, {140, 140, 240, 240});
    InitTestSceneSession(2, 117, 15, true, {150, 150, 250, 250});
    InitTestSceneSession(1, 118, 13, false, {150, 150, 250, 250});
    InitTestSceneSession(1, 119, 13, true, {160, 160, 260, 260});
    InitTestSceneSession(1, 120, 12, true, {500, 500, 600, 600});

    std::vector<int32_t> windowIds;
    WMError result = ssm_->GetWindowIdsByCoordinate(1, 3, -1, -1, windowIds);
    EXPECT_EQ(result, WMError::WM_OK);
    EXPECT_EQ(3, windowIds.size());
    EXPECT_EQ(116, windowIds[0]);
    EXPECT_EQ(113, windowIds[1]);
    EXPECT_EQ(119, windowIds[2]);
    ssm_->sceneSessionMap_.clear();
}

/**
 * @tc.name: GetWindowIdsByCoordinate04
 * @tc.desc: GetWindowIdsByCoordinate, windowNumber 0, x y effictive value
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, GetWindowIdsByCoordinate04, Function | SmallTest | Level3)
{
    ssm_->sceneSessionMap_.clear();
    InitTestSceneSession(1, 121, 11, true, {100, 100, 200, 200});
    ssm_->sceneSessionMap_.insert({102, nullptr});
    InitTestSceneSession(1, 123, 14, true, {120, 120, 220, 220});
    InitTestSceneSession(1, 125, 12, true, {100, 100, 200, 200});
    auto it1 = ssm_->sceneSessionMap_.find(125);
    if (it1 != ssm_->sceneSessionMap_.end()) {
        it1->second->sessionInfo_.bundleName_ = "other";
    }
    InitTestSceneSession(1, 126, 15, true, {140, 140, 240, 240});
    InitTestSceneSession(2, 127, 15, true, {150, 150, 250, 250});
    InitTestSceneSession(1, 128, 13, false, {150, 150, 250, 250});
    InitTestSceneSession(1, 129, 13, true, {160, 160, 260, 260});
    InitTestSceneSession(1, 130, 12, true, {500, 500, 600, 600});

    std::vector<int32_t> windowIds;
    WMError result = ssm_->GetWindowIdsByCoordinate(1, 0, 180, 180, windowIds);
    EXPECT_EQ(result, WMError::WM_OK);
    EXPECT_EQ(4, windowIds.size());
    EXPECT_EQ(126, windowIds[0]);
    EXPECT_EQ(123, windowIds[1]);
    EXPECT_EQ(129, windowIds[2]);
    EXPECT_EQ(121, windowIds[3]);
    ssm_->sceneSessionMap_.clear();
}

/**
 * @tc.name: GetWindowIdsByCoordinate05
 * @tc.desc: GetWindowIdsByCoordinate, windowNumber 3, x y effictive value
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, GetWindowIdsByCoordinate05, Function | SmallTest | Level3)
{
    ssm_->sceneSessionMap_.clear();
    InitTestSceneSession(1, 131, 11, true, {100, 100, 200, 200});
    ssm_->sceneSessionMap_.insert({102, nullptr});
    InitTestSceneSession(1, 133, 14, true, {120, 120, 220, 220});
    InitTestSceneSession(1, 134, 12, true, {100, 100, 200, 200});
    ASSERT_TRUE(ssm_->sceneSessionMap_.find(134) != ssm_->sceneSessionMap_.end());
    InitTestSceneSession(1, 135, 12, true, {100, 100, 200, 200});
    auto it1 = ssm_->sceneSessionMap_.find(135);
    if (it1 != ssm_->sceneSessionMap_.end()) {
        it1->second->sessionInfo_.bundleName_ = "other";
    }
    InitTestSceneSession(1, 136, 15, true, {140, 140, 240, 240});
    InitTestSceneSession(2, 137, 15, true, {150, 150, 250, 250});
    InitTestSceneSession(1, 138, 13, false, {150, 150, 250, 250});
    InitTestSceneSession(1, 139, 13, true, {160, 160, 260, 260});
    InitTestSceneSession(1, 140, 12, true, {500, 500, 600, 600});

    std::vector<int32_t> windowIds;
    WMError result = ssm_->GetWindowIdsByCoordinate(1, 3, 180, 180, windowIds);
    EXPECT_EQ(result, WMError::WM_OK);
    EXPECT_EQ(3, windowIds.size());
    EXPECT_EQ(136, windowIds[0]);
    EXPECT_EQ(133, windowIds[1]);
    EXPECT_EQ(139, windowIds[2]);
    ssm_->sceneSessionMap_.clear();
}

/**
 * @tc.name: ProcessFocusZOrderChange
 * @tc.desc: ProcessFocusZOrderChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, ProcessFocusZOrderChange, Function | SmallTest | Level3)
{
    ssm_->sceneSessionMap_.clear();
    ssm_->ProcessFocusZOrderChange(10);
    ssm_->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ssm_->ProcessFocusZOrderChange(97);

    ssm_->systemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ssm_->ProcessFocusZOrderChange(97);

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "focusedSession";
    sessionInfo.abilityName_ = "focusedSession";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.emplace(1, sceneSession);
    ssm_->focusedSessionId_ = 1;
    ssm_->ProcessFocusZOrderChange(97);

    sceneSession->lastZOrder_ = 2203;
    sceneSession->zOrder_ = 101;
    ssm_->ProcessFocusZOrderChange(97);

    SessionInfo sessionInfo1;
    sessionInfo1.bundleName_ = "voiceInteractionSession";
    sessionInfo1.abilityName_ = "voiceInteractionSession";
    sessionInfo1.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_VOICE_INTERACTION);
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo1, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    sceneSession1->zOrder_ = 2109;
    ssm_->sceneSessionMap_.emplace(2, sceneSession1);

    sceneSession->lastZOrder_ = 103;
    sceneSession->zOrder_ = 101;
    ssm_->ProcessFocusZOrderChange(97);

    sceneSession->lastZOrder_ = 2203;
    sceneSession->zOrder_ = 101;
    ssm_->ProcessFocusZOrderChange(97);
}

/**
 * @tc.name: GetAllSceneSessionForAccessibility
 * @tc.desc: GetAllSceneSessionForAccessibility
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, GetAllSceneSessionForAccessibility, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    std::vector<sptr<SceneSession>> sceneSessionList;
    ASSERT_EQ(sceneSessionList.size(), 0);
    SessionInfo info1;
    info1.abilityName_ = "test1";
    info1.bundleName_ = "test1";
    info1.windowType_ = 1;
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(info1, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    SessionInfo info2;
    info2.abilityName_ = "test2";
    info2.bundleName_ = "test2";
    info2.windowType_ = 1;
    sptr<SceneSession> sceneSession2 = sptr<SceneSession>::MakeSptr(info2, nullptr);
    ASSERT_NE(nullptr, sceneSession2);
    SessionInfo info3;
    info3.abilityName_ = "test3";
    info3.bundleName_ = "test3";
    info3.windowType_ = 1;
    sptr<SceneSession> sceneSession3 = sptr<SceneSession>::MakeSptr(info3, nullptr);
    ASSERT_NE(nullptr, sceneSession3);
    ssm_->sceneSessionMap_.emplace(1, sceneSession1);
    ssm_->sceneSessionMap_.emplace(2, sceneSession2);
    ssm_->sceneSessionMap_.emplace(3, sceneSession3);
    sceneSession1->isScbCoreEnabled_ = false;
    sceneSession2->isScbCoreEnabled_ = false;
    sceneSession3->isScbCoreEnabled_ = false;
    sceneSession1->isVisibleForAccessibility_ = true;
    sceneSession2->isVisibleForAccessibility_ = false;
    sceneSession3->isVisibleForAccessibility_ = true;
    sceneSession1->isVisible_ = true;
    sceneSession2->isVisible_ = false;
    sceneSession3->isVisible_ = true;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ASSERT_EQ(sceneSessionList.size(), 1);
}

/**
 * @tc.name: TestGetMainParentSceneSession_01
 * @tc.desc: Test GetMainParentSceneSession with invalid persistentId
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestGetMainParentSceneSession_01, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<SceneSession> ret = ssm_->GetMainParentSceneSession(0, ssm_->sceneSessionMap_);
    ASSERT_EQ(ret, nullptr);
}

/**
 * @tc.name: TestGetMainParentSceneSession_02
 * @tc.desc: Test GetMainParentSceneSession with Error found scene session
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestGetMainParentSceneSession_02, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<SceneSession> ret = ssm_->GetMainParentSceneSession(999, ssm_->sceneSessionMap_);
    ASSERT_EQ(ret, nullptr);
}

/**
 * @tc.name: TestGetMainParentSceneSession_03
 * @tc.desc: Test GetMainParentSceneSession with parent session
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestGetMainParentSceneSession_03, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "GetMainParentSceneSession001";
    info.bundleName_ = "GetMainParentSceneSession001";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSessionParent = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSessionParent->property_->SetPersistentId(100);
    ssm_->sceneSessionMap_.insert({100, sceneSessionParent});
    sceneSession->SetParentSession(sceneSessionParent);

    ssm_->sceneSessionMap_[999] = sceneSession;
    sptr<SceneSession> ret = ssm_->GetMainParentSceneSession(999, ssm_->sceneSessionMap_);
    ASSERT_NE(ret, sceneSessionParent);
}

/**
 * @tc.name: TestGetParentMainWindowId_01
 * @tc.desc: Tst GetParentMainWindowId with invalid windowId
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestGetParentMainWindowId_01, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    int32_t windowId = 0;
    int32_t mainWindowId = 0;
    WMError ret = ssm_->GetParentMainWindowId(windowId, mainWindowId);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: TestGetParentMainWindowId_02
 * @tc.desc: Tst GetParentMainWindowId with not found scene session
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestGetParentMainWindowId_02, Function | SmallTest | Level3)
{
    int32_t windowId = -1;
    int32_t mainWindowId = 0;
    WMError ret = ssm_->GetParentMainWindowId(windowId, mainWindowId);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: TestGetParentMainWindowId_03
 * @tc.desc: Tst GetParentMainWindowId with session
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestGetParentMainWindowId_03, Function | SmallTest | Level3)
{
    int32_t windowId = 200;
    int32_t mainWindowId = 0;
    SessionInfo info;
    info.abilityName_ = "test";
    info.bundleName_ = "test";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    sceneSession->property_->SetPersistentId(windowId);
    ssm_->sceneSessionMap_[windowId] = sceneSession;
    WMError ret = ssm_->GetParentMainWindowId(windowId, mainWindowId);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: TestGetParentMainWindowId_04
 * @tc.desc: Tst GetParentMainWindowId with WindowType WINDOW_TYPE_APP_SUB_WINDOW
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestGetParentMainWindowId_04, Function | SmallTest | Level3)
{
    int32_t windowId = 200;
    int32_t mainWindowId = 0;
    SessionInfo info;
    info.abilityName_ = "test";
    info.bundleName_ = "test";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->property_->SetPersistentId(windowId);
    ssm_->sceneSessionMap_[windowId] = sceneSession;
    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    WMError ret = ssm_->GetParentMainWindowId(windowId, mainWindowId);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);

    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ret = ssm_->GetParentMainWindowId(windowId, mainWindowId);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: NotifyVisibleChange
 * @tc.desc: test NotifyVisibleChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, NotifyVisibleChange, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test";
    info.bundleName_ = "test";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);

    ASSERT_FALSE(ssm_->NotifyVisibleChange(sceneSession->GetPersistentId()));
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});
    ASSERT_TRUE(ssm_->NotifyVisibleChange(sceneSession->GetPersistentId()));

    ssm_->sceneSessionMap_.erase(sceneSession->GetPersistentId());
}

/**
 * @tc.name: TestIsInDefaultScreen_01
 * @tc.desc: Test IsInDefaultScreen with not DefaultScreen id
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestIsInDefaultScreen_01, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test";
    info.bundleName_ = "test";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    DisplayId displayId = displayId = 5;
    property->SetDisplayId(displayId);
    sceneSession->SetSessionProperty(property);
    ASSERT_EQ(ssm_->IsInDefaultScreen(sceneSession), false);
}

/**
 * @tc.name: TestIsInDefaultScreen_02
 * @tc.desc: Test IsInDefaultScreen with DefaultScreen id
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestIsInDefaultScreen_02, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test";
    info.bundleName_ = "test";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, property);
    DisplayId displayId = ScreenSessionManagerClient::GetInstance().GetDefaultScreenId();
    property->SetDisplayId(displayId);
    sceneSession->SetSessionProperty(property);
    ASSERT_EQ(ssm_->IsInDefaultScreen(sceneSession), true);
}

/**
 * @tc.name: RegisterRequestVsyncFunc
 * @tc.desc: test RegisterRequestVsyncFunc01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, RegisterRequestVsyncFunc01, Function | SmallTest | Level3)
{
    ssm_->RegisterRequestVsyncFunc(nullptr);
    SessionInfo info;
    info.abilityName_ = "RegisterRequestVsyncFunc01";
    info.bundleName_ = "RegisterRequestVsyncFunc01";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->RegisterRequestVsyncFunc(sceneSession);
    ASSERT_NE(nullptr, sceneSession->requestNextVsyncFunc_);
}

/**
 * @tc.name: TestEraseSceneSessionAndMarkDirtyLockFree_01
 * @tc.desc: Test EraseSceneSessionAndMarkDirtyLockFree with erase id not exist
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestEraseSceneSessionAndMarkDirtyLockFree_01, Function | SmallTest | Level1)
{
    // init
    ssm_->sceneSessionMap_.clear();
    ssm_->sessionMapDirty_ = 0;

    SessionInfo info;
    info.abilityName_ = "EraseSceneSessionAndMarkDirtyLockFree";
    info.bundleName_ = "EraseSceneSessionAndMarkDirtyLockFree";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    const int32_t validId = 100;
    const int32_t invalidId = 101;
    ssm_->sceneSessionMap_.insert({validId, sceneSession});
    // erase id not exist
    ssm_->EraseSceneSessionAndMarkDirtyLockFree(invalidId);
    ASSERT_EQ(ssm_->sessionMapDirty_, 0);
    ASSERT_NE(ssm_->sceneSessionMap_.find(validId), ssm_->sceneSessionMap_.end());
}

/**
 * @tc.name: TestEraseSceneSessionAndMarkDirtyLockFree_02
 * @tc.desc: Test EraseSceneSessionAndMarkDirtyLockFree with erase invisible session
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestEraseSceneSessionAndMarkDirtyLockFree_02, Function | SmallTest | Level1)
{
    // init
    ssm_->sceneSessionMap_.clear();
    ssm_->sessionMapDirty_ = 0;

    SessionInfo info;
    info.abilityName_ = "TestEraseSceneSessionAndMarkDirtyLockFree_02";
    info.bundleName_ = "TestEraseSceneSessionAndMarkDirtyLockFree_02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    const int32_t validId = 100;
    ssm_->sceneSessionMap_.insert({validId, sceneSession});

    // erase invisible session
    sceneSession->isVisible_ = false;
    ssm_->EraseSceneSessionAndMarkDirtyLockFree(validId);
    ASSERT_EQ(ssm_->sessionMapDirty_, 0);
    ASSERT_EQ(ssm_->sceneSessionMap_.find(validId), ssm_->sceneSessionMap_.end());
}

/**
 * @tc.name: TestEraseSceneSessionAndMarkDirtyLockFree_03
 * @tc.desc: Test EraseSceneSessionAndMarkDirtyLockFree with erase visible session
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestEraseSceneSessionAndMarkDirtyLockFree_03, Function | SmallTest | Level1)
{
    // init
    ssm_->sceneSessionMap_.clear();
    ssm_->sessionMapDirty_ = 0;

    SessionInfo info;
    info.abilityName_ = "TestEraseSceneSessionAndMarkDirtyLockFree_03";
    info.bundleName_ = "TestEraseSceneSessionAndMarkDirtyLockFree_03";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    const int32_t validId = 100;

    // erase visible session
    ssm_->sceneSessionMap_.insert({validId, sceneSession});
    sceneSession->isVisible_ = true;
    ssm_->EraseSceneSessionAndMarkDirtyLockFree(validId);
    ASSERT_EQ(ssm_->sessionMapDirty_, static_cast<uint32_t>(SessionUIDirtyFlag::VISIBLE));
    ASSERT_EQ(ssm_->sceneSessionMap_.find(validId), ssm_->sceneSessionMap_.end());
}

/**
 * @tc.name: ProcessUpdateLastFocusedAppId
 * @tc.desc: test ProcessUpdateLastFocusedAppId
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, ProcessUpdateLastFocusedAppId, Function | SmallTest | Level1)
{
    ssm_->sceneSessionMap_.clear();
    std::vector<uint32_t> zOrderList;
    ssm_->lastFocusedAppSessionId_ = INVALID_SESSION_ID;
    ssm_->ProcessUpdateLastFocusedAppId(zOrderList);

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "lastFocusedAppSession";
    sessionInfo.abilityName_ = "lastFocusedAppSession";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ssm_->sceneSessionMap_.emplace(1, sceneSession);
    ssm_->lastFocusedAppSessionId_ = 1;
    sceneSession->zOrder_ = 101;

    ssm_->ProcessUpdateLastFocusedAppId(zOrderList);
    ASSERT_EQ(1, ssm_->lastFocusedAppSessionId_);

    zOrderList.push_back(103);
    ssm_->ProcessUpdateLastFocusedAppId(zOrderList);
    ASSERT_EQ(INVALID_SESSION_ID, ssm_->lastFocusedAppSessionId_);
}

/**
 * @tc.name: TestIsNeedSkipWindowModeTypeCheck_01
 * @tc.desc: Test IsNeedSkipWindowModeTypeCheck with sceneSession nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestIsNeedSkipWindowModeTypeCheck_01, Function | SmallTest | Level3)
{
    sptr<SceneSession> sceneSession;
    auto ret = ssm_->IsNeedSkipWindowModeTypeCheck(sceneSession, false);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: TestIsNeedSkipWindowModeTypeCheck_02
 * @tc.desc: Test IsNeedSkipWindowModeTypeCheck with sceneSession not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestIsNeedSkipWindowModeTypeCheck_02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "IsNeedSkipWindowModeTypeCheck";
    sessionInfo.abilityName_ = "IsNeedSkipWindowModeTypeCheck";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    sceneSession->SetRSVisible(true);
    sceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    auto ret = ssm_->IsNeedSkipWindowModeTypeCheck(sceneSession, false);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: TestIsNeedSkipWindowModeTypeCheck_03
 * @tc.desc: Test IsNeedSkipWindowModeTypeCheck WindowType APP_MAIN_WINDOW_BASE
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestIsNeedSkipWindowModeTypeCheck_03, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "IsNeedSkipWindowModeTypeCheck";
    sessionInfo.abilityName_ = "IsNeedSkipWindowModeTypeCheck";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->SetRSVisible(false);
    sceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    auto ret = ssm_->IsNeedSkipWindowModeTypeCheck(sceneSession, false);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: TestIsNeedSkipWindowModeTypeCheck_04
 * @tc.desc: Test IsNeedSkipWindowModeTypeCheck WindowType APP_MAIN_WINDOW_BASE and RSVisible true
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, TestIsNeedSkipWindowModeTypeCheck_04, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "TestIsNeedSkipWindowModeTypeCheck_04";
    sessionInfo.abilityName_ = "TestIsNeedSkipWindowModeTypeCheck_04";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->SetRSVisible(true);
    sceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    
    DisplayId displayId = 1001;
    sceneSession->property_->SetDisplayId(displayId);
    auto ret = ssm_->IsNeedSkipWindowModeTypeCheck(sceneSession, true);
    ASSERT_TRUE(ret);

    ret = ssm_->IsNeedSkipWindowModeTypeCheck(sceneSession, false);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: UpdateAvoidAreaByType
 * @tc.desc: test UpdateAvoidAreaByType
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, UpdateAvoidAreaByType, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test";
    info.bundleName_ = "test";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);

    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});
    sceneSession->isVisible_ = true;
    sceneSession->state_ = SessionState::STATE_ACTIVE;
    ssm_->UpdateAvoidAreaByType(sceneSession->GetPersistentId(), AvoidAreaType::TYPE_NAVIGATION_INDICATOR);
    EXPECT_EQ(ssm_->lastUpdatedAvoidArea_.find(sceneSession->GetPersistentId()), ssm_->lastUpdatedAvoidArea_.end());
    ssm_->avoidAreaListenerSessionSet_.insert(sceneSession->GetPersistentId());
    ssm_->UpdateAvoidAreaByType(sceneSession->GetPersistentId(), AvoidAreaType::TYPE_NAVIGATION_INDICATOR);
    EXPECT_EQ(ssm_->lastUpdatedAvoidArea_.find(sceneSession->GetPersistentId()), ssm_->lastUpdatedAvoidArea_.end());
    ssm_->avoidAreaListenerSessionSet_.erase(sceneSession->GetPersistentId());
    ssm_->sceneSessionMap_.erase(sceneSession->GetPersistentId());
}

/**
 * @tc.name: NotifyStatusBarShowStatus
 * @tc.desc: test NotifyStatusBarShowStatus
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, NotifyStatusBarShowStatus, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test";
    info.bundleName_ = "test";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);

    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});
    sceneSession->isStatusBarVisible_ = true;
    EXPECT_EQ(WSError::WS_OK, ssm_->NotifyStatusBarShowStatus(sceneSession->GetPersistentId(), false));
    ssm_->sceneSessionMap_.erase(sceneSession->GetPersistentId());
}

/**
 * @tc.name: NotifyAppUseControlList
 * @tc.desc: NotifyAppUseControlList
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, NotifyAppUseControlList, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    std::vector<AppUseControlInfo> controlList;
    controlList.emplace_back();
    EXPECT_EQ(WSError::WS_ERROR_INVALID_PERMISSION,
        ssm_->NotifyAppUseControlList(ControlAppType::APP_LOCK, -1, controlList));

    AppUseControlInfo appUseControlInfo;
    appUseControlInfo.bundleName_ = "bundleName";
    appUseControlInfo.appIndex_ = 1;
    appUseControlInfo.isNeedControl_ = true;
    EXPECT_EQ(WSError::WS_ERROR_INVALID_PERMISSION,
        ssm_->NotifyAppUseControlList(ControlAppType::APP_LOCK, -1, controlList));
}
}  // namespace
}
}