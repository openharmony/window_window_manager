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
class SceneSessionManagerTest4 : public testing::Test {
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

sptr<SceneSessionManager> SceneSessionManagerTest4::ssm_ = nullptr;
bool SceneSessionManagerTest4::gestureNavigationEnabled_ = true;

ProcessGestureNavigationEnabledChangeFunc SceneSessionManagerTest4::callbackFunc_ = [](bool enable) {
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

void SceneSessionManagerTest4::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerTest4::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerTest4::SetUp()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerTest4::TearDown()
{
    ssm_->sceneSessionMap_.clear();
    usleep(WAIT_SYNC_IN_NS);
}


namespace {
/**
 * @tc.name: UpdateSceneSessionWant01
 * @tc.desc: SceneSesionManager test UpdateSceneSessionWant
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, UpdateSceneSessionWant01, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.persistentId_ = 0;
    ssm_->UpdateSceneSessionWant(info);
    ASSERT_NE(ssm_, nullptr);
}

/**
 * @tc.name: UpdateSceneSessionWant02
 * @tc.desc: SceneSesionManager test UpdateSceneSessionWant
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, UpdateSceneSessionWant02, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.persistentId_ = 1;
    ssm_->UpdateSceneSessionWant(info);
    ASSERT_NE(ssm_, nullptr);
}

/**
 * @tc.name: UpdateSceneSessionWant03
 * @tc.desc: SceneSesionManager test UpdateSceneSessionWant
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, UpdateSceneSessionWant03, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.persistentId_ = 1;
    auto sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->UpdateSceneSessionWant(info);
    ASSERT_NE(ssm_, nullptr);
}

/**
 * @tc.name: UpdateSceneSessionWant04
 * @tc.desc: SceneSesionManager test UpdateSceneSessionWant
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, UpdateSceneSessionWant04, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.persistentId_ = 1;
    std::shared_ptr<AAFwk::Want> want = std::make_shared<AAFwk::Want>();
    info.want = want;
    auto sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->UpdateSceneSessionWant(info);
    ASSERT_NE(ssm_, nullptr);
}

/**
 * @tc.name: UpdateSceneSessionWant05
 * @tc.desc: SceneSesionManager test UpdateSceneSessionWant
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, UpdateSceneSessionWant05, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.persistentId_ = 1;
    std::shared_ptr<AAFwk::Want> want = std::make_shared<AAFwk::Want>();
    info.want = want;
    auto sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    int32_t collaboratorType = CollaboratorType::RESERVE_TYPE;
    sceneSession->SetCollaboratorType(collaboratorType);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->UpdateSceneSessionWant(info);
    ASSERT_NE(ssm_, nullptr);
}

/**
 * @tc.name: UpdateSceneSessionWant06
 * @tc.desc: SceneSesionManager test UpdateSceneSessionWant
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, UpdateSceneSessionWant06, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.persistentId_ = 1;
    std::shared_ptr<AAFwk::Want> want = std::make_shared<AAFwk::Want>();
    info.want = want;
    auto sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    int32_t collaboratorType = -1;
    sceneSession->SetCollaboratorType(collaboratorType);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->UpdateSceneSessionWant(info);
    ASSERT_NE(ssm_, nullptr);
}

/**
 * @tc.name: GetSceneSessionByName01
 * @tc.desc: SceneSesionManager test GetSceneSessionByName
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, GetSceneSessionByName01, Function | SmallTest | Level3)
{
    std::string abilityName = "test1";
    std::string bundleName = "test2";
    std::string moduleName = "test3";
    int32_t appIndex = 10;
    SessionInfo info;
    info.abilityName_ = abilityName;
    info.bundleName_ = bundleName;
    info.moduleName_ = moduleName;
    info.appIndex_ = appIndex;
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_NE(ssm_->GetSceneSessionByName(bundleName, moduleName, abilityName, appIndex), nullptr);
}

/**
 * @tc.name: DestroyAndDisconnectSpecificSession01
 * @tc.desc: SceneSesionManager test DestroyAndDisconnectSpecificSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, DestroyAndDisconnectSpecificSession01, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "DestroyAndDisconnectSpecificSession";
    info.bundleName_ = "DestroyAndDisconnectSpecificSession";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_NE(ssm_->DestroyAndDisconnectSpecificSession(1), WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: GetAllSessionDumpInfo
 * @tc.desc: SceneSesionManager test GetAllSessionDumpInfo
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, GetAllSessionDumpInfo, Function | SmallTest | Level3)
{
    SessionInfo info1;
    info1.abilityName_ = "GetAllSessionDumpInfo1";
    info1.bundleName_ = "GetAllSessionDumpInfo1";
    info1.persistentId_ = 1;
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(info1, nullptr);
    ASSERT_NE(sceneSession1, nullptr);
    sceneSession1->UpdateNativeVisibility(true);

    SessionInfo info2;
    info2.abilityName_ = "GetAllSessionDumpInfo2";
    info2.bundleName_ = "GetAllSessionDumpInfo2";
    info2.persistentId_ = 2;
    sptr<SceneSession> sceneSession2 = sptr<SceneSession>::MakeSptr(info2, nullptr);
    ASSERT_NE(sceneSession2, nullptr);
    sceneSession2->UpdateNativeVisibility(false);

    ssm_->sceneSessionMap_.insert({1, sceneSession1});
    ssm_->sceneSessionMap_.insert({2, sceneSession2});
    std::string dumpInfo;
    ASSERT_EQ(ssm_->GetAllSessionDumpInfo(dumpInfo), WSError::WS_OK);
}

/**
 * @tc.name: GetSpecifiedSessionDumpInfo
 * @tc.desc: SceneSesionManager test GetSpecifiedSessionDumpInfo
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, GetSpecifiedSessionDumpInfo, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "GetSpecifiedSessionDumpInfo";
    info.bundleName_ = "GetSpecifiedSessionDumpInfo";
    info.persistentId_ = 1234;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1234, sceneSession});
    std::string dumpInfo;
    std::string strId = "1234";
    std::vector<std::string> params_(5, "");
    ASSERT_EQ(ssm_->GetSpecifiedSessionDumpInfo(dumpInfo, params_, strId), WSError::WS_OK);
}

/**
 * @tc.name: NotifyEnterRecentTask02
 * @tc.desc: NotifyEnterRecentTask
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, NotifyEnterRecentTask02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    EXPECT_EQ(ssm_->NotifyEnterRecentTask(false), WSError::WS_OK);
}

/**
 * @tc.name: GetWindowStatus
 * @tc.desc: GetWindowStatus
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, GetWindowStatus, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    WindowMode mode = WindowMode::WINDOW_MODE_FLOATING;
    SessionState sessionState = SessionState::STATE_FOREGROUND;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    auto result = ssm_->GetWindowStatus(mode, sessionState, nullptr);
    EXPECT_EQ(result, WindowStatus::WINDOW_STATUS_UNDEFINED);

    property->SetMaximizeMode(MaximizeMode::MODE_AVOID_SYSTEM_BAR);
    result = ssm_->GetWindowStatus(mode, sessionState, property);
    EXPECT_EQ(result, WindowStatus::WINDOW_STATUS_MAXIMIZE);

    property->SetMaximizeMode(MaximizeMode::MODE_FULL_FILL);
    result = ssm_->GetWindowStatus(mode, sessionState, property);
    EXPECT_EQ(result, WindowStatus::WINDOW_STATUS_FLOATING);

    mode = WindowMode::WINDOW_MODE_SPLIT_PRIMARY;
    result = ssm_->GetWindowStatus(mode, sessionState, property);
    EXPECT_EQ(result, WindowStatus::WINDOW_STATUS_SPLITSCREEN);
}

/**
 * @tc.name: UpdateDisplayRegion
 * @tc.desc: UpdateDisplayRegion
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, UpdateDisplayRegion, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->UpdateDisplayRegion(nullptr);
    sptr<DisplayInfo> displayInfo = sptr<DisplayInfo>::MakeSptr();
    ASSERT_NE(displayInfo, nullptr);
    displayInfo->SetWidth(0);
    ssm_->UpdateDisplayRegion(displayInfo);

    displayInfo->SetWidth(1);
    displayInfo->SetHeight(0);
    ssm_->UpdateDisplayRegion(displayInfo);

    displayInfo->SetHeight(1);
    ssm_->UpdateDisplayRegion(displayInfo);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, ssm_->HandleSecureSessionShouldHide(nullptr));
}

/**
 * @tc.name: GetCustomDecorHeight
 * @tc.desc: GetCustomDecorHeight
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, GetCustomDecorHeight, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->SetCustomDecorHeight(50);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->sceneSessionMap_.insert(std::make_pair(2, nullptr));
    EXPECT_EQ(0, ssm_->GetCustomDecorHeight(2));
    EXPECT_EQ(50, ssm_->GetCustomDecorHeight(1));
}

/**
 * @tc.name: IsVectorSame
 * @tc.desc: IsVectorSame
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, IsVectorSame, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    std::vector<VisibleWindowNumInfo> lastInfo;
    std::vector<VisibleWindowNumInfo> currentInfo;
    VisibleWindowNumInfo num;
    lastInfo.push_back(num);
    EXPECT_EQ(false, ssm_->IsVectorSame(lastInfo, currentInfo));
    lastInfo.clear();

    lastInfo.reserve(2);
    VisibleWindowNumInfo oneNum;
    oneNum.displayId = 0;
    oneNum.visibleWindowNum = 3;
    lastInfo.push_back(oneNum);
    currentInfo.push_back(oneNum);
    EXPECT_EQ(true, ssm_->IsVectorSame(lastInfo, currentInfo));

    currentInfo.clear();
    VisibleWindowNumInfo twoNum;
    twoNum.displayId = 0;
    twoNum.visibleWindowNum = 2;
    currentInfo.push_back(twoNum);
    EXPECT_EQ(false, ssm_->IsVectorSame(lastInfo, currentInfo));

    currentInfo.clear();
    twoNum.displayId = 1;
    twoNum.visibleWindowNum = 3;
    currentInfo.push_back(twoNum);
    EXPECT_EQ(false, ssm_->IsVectorSame(lastInfo, currentInfo));
}

/**
 * @tc.name: ReportWindowProfileInfos
 * @tc.desc: ReportWindowProfileInfos
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, ReportWindowProfileInfos, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->sessionInfo_.isSystem_ = false;
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->ReportWindowProfileInfos();

    ssm_->focusedSessionId_ = 123;
    ssm_->ReportWindowProfileInfos();
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, ssm_->HandleSecureSessionShouldHide(nullptr));
}

/**
 * @tc.name: ReportWindowProfileInfos02
 * @tc.desc: ReportWindowProfileInfos
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, ReportWindowProfileInfos02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->sessionInfo_.isSystem_ = false;
    sptr<Session> session = sptr<Session>::MakeSptr(info);
    ASSERT_NE(session, nullptr);
    ASSERT_NE(sceneSession->property_, nullptr);
    sceneSession->property_->type_ = WindowType::WINDOW_TYPE_MEDIA;
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->ReportWindowProfileInfos();
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, ssm_->HandleSecureSessionShouldHide(nullptr));
}

/**
 * @tc.name: ReportWindowProfileInfos03
 * @tc.desc: ReportWindowProfileInfos
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, ReportWindowProfileInfos03, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->sessionInfo_.isSystem_ = true;
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->ReportWindowProfileInfos();
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, ssm_->HandleSecureSessionShouldHide(nullptr));
}

/**
 * @tc.name: ReportWindowProfileInfos04
 * @tc.desc: ReportWindowProfileInfos
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, ReportWindowProfileInfos04, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, nullptr));
    ssm_->ReportWindowProfileInfos();
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, ssm_->HandleSecureSessionShouldHide(nullptr));
}

/**
 * @tc.name: CacVisibleWindowNum
 * @tc.desc: CacVisibleWindowNum
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, CacVisibleWindowNum, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession01 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession02 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession03 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession04 = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession01, nullptr);
    ASSERT_NE(sceneSession02, nullptr);
    ASSERT_NE(sceneSession03, nullptr);
    ASSERT_NE(sceneSession04, nullptr);
    ASSERT_NE(sceneSession01->property_, nullptr);
    sceneSession01->property_->type_ = WindowType::WINDOW_TYPE_WALLPAPER;
    sceneSession01->SetRSVisible(true);
    sceneSession02->state_ = SessionState::STATE_BACKGROUND;
    ASSERT_NE(sceneSession03->property_, nullptr);
    sceneSession03->property_->type_ = WindowType::APP_MAIN_WINDOW_END;
    ASSERT_NE(sceneSession04->property_, nullptr);
    sceneSession04->property_->type_ = WindowType::WINDOW_TYPE_WALLPAPER;
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession01));
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession02));
    ssm_->sceneSessionMap_.insert(std::make_pair(3, sceneSession03));
    ssm_->sceneSessionMap_.insert(std::make_pair(4, sceneSession04));
    ssm_->sceneSessionMap_.insert(std::make_pair(5, nullptr));

    ssm_->CacVisibleWindowNum();
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, ssm_->HandleSecureSessionShouldHide(nullptr));
}

/**
 * @tc.name: GetAppMainSceneSession
 * @tc.desc: GetAppMainSceneSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, GetAppMainSceneSession, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession;
    sptr<SceneSession> sceneSession01 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession02 = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession01, nullptr);
    ASSERT_NE(sceneSession02, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(0, sceneSession));
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession01));
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession02));
    EXPECT_EQ(ssm_->GetAppMainSceneSession(sceneSession, 0), WSError::WS_ERROR_INVALID_SESSION);

    EXPECT_EQ(ssm_->GetAppMainSceneSession(sceneSession01, 1), WSError::WS_OK);

    ASSERT_NE(sceneSession01->property_, nullptr);
    sceneSession01->property_->type_ = WindowType::WINDOW_TYPE_MEDIA;
    EXPECT_EQ(ssm_->GetAppMainSceneSession(sceneSession01, 1), WSError::WS_ERROR_INVALID_CALLING);

    sceneSession01->property_->type_ = WindowType::WINDOW_TYPE_APP_SUB_WINDOW;
    sceneSession01->property_->SetParentPersistentId(2);
    EXPECT_EQ(ssm_->GetAppMainSceneSession(sceneSession01, 1), WSError::WS_OK);
}

/**
 * @tc.name: GetImmersiveState02
 * @tc.desc: GetImmersiveState
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, GetImmersiveState02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);

    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession01;
    sptr<SceneSession> sceneSession02 = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_EQ(sceneSession01, nullptr);
    ASSERT_NE(sceneSession02, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession01));
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession02));

    sceneSession02->property_ = nullptr;
    EXPECT_EQ(false, ssm_->GetImmersiveState());
    sceneSession02->property_ = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(sceneSession02->property_, nullptr);
    sceneSession02->property_->type_ = WindowType::APP_MAIN_WINDOW_END;
    EXPECT_EQ(false, ssm_->GetImmersiveState());
    sceneSession02->property_->type_ = WindowType::APP_MAIN_WINDOW_BASE;
    EXPECT_EQ(false, ssm_->GetImmersiveState());
    sceneSession02->state_ = SessionState::STATE_ACTIVE;
    EXPECT_EQ(false, ssm_->GetImmersiveState());
    sceneSession02->state_ = SessionState::STATE_FOREGROUND;
    EXPECT_EQ(false, ssm_->GetImmersiveState());
    sceneSession02->property_->SetWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);
    EXPECT_EQ(false, ssm_->GetImmersiveState());
    sceneSession02->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sceneSession02->property_->sysBarPropMap_[WindowType::WINDOW_TYPE_STATUS_BAR].enable_ = false;
    EXPECT_EQ(true, ssm_->GetImmersiveState());
    sceneSession02->property_->sysBarPropMap_[WindowType::WINDOW_TYPE_STATUS_BAR].enable_ = true;
    EXPECT_EQ(false, ssm_->GetImmersiveState());
}

/**
 * @tc.name: UpdateSessionDisplayId
 * @tc.desc: UpdateSessionDisplayId
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, UpdateSessionDisplayId, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    auto result = ssm_->UpdateSessionDisplayId(0, 0);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_WINDOW);

    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    sceneSession->sessionInfo_.screenId_ = 6;
    result = ssm_->UpdateSessionDisplayId(1, 2);
    EXPECT_EQ(result, WSError::WS_OK);

    sceneSession->property_ = nullptr;
    result = ssm_->UpdateSessionDisplayId(1, 2);
    EXPECT_EQ(result, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: UpdateSessionWindowVisibilityListener02
 * @tc.desc: UpdateSessionWindowVisibilityListener
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, UpdateSessionWindowVisibilityListener02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    int32_t persistentId = 1;
    auto result = ssm_->UpdateSessionWindowVisibilityListener(persistentId, true);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: NotifySessionAINavigationBarChange
 * @tc.desc: NotifySessionAINavigationBarChange
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, NotifySessionAINavigationBarChange, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->NotifySessionAINavigationBarChange(0);
    ssm_->NotifySessionAINavigationBarChange(1);

    ASSERT_NE(sceneSession->property_, nullptr);
    sceneSession->property_->type_ = WindowType::APP_SUB_WINDOW_END;
    sceneSession->state_ = SessionState::STATE_ACTIVE;
    ssm_->NotifySessionAINavigationBarChange(1);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, ssm_->HandleSecureSessionShouldHide(nullptr));
}

/**
 * @tc.name: UpdateNormalSessionAvoidArea
 * @tc.desc: UpdateNormalSessionAvoidArea
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, UpdateNormalSessionAvoidArea, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    int32_t persistentId = 1;
    sptr<SceneSession> sceneSession = nullptr;
    bool needUpdate = true;
    ssm_->UpdateNormalSessionAvoidArea(persistentId, sceneSession, needUpdate);
    EXPECT_EQ(needUpdate, false);

    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ASSERT_NE(sceneSession->property_, nullptr);
    sceneSession->property_->type_ = WindowType::APP_SUB_WINDOW_BASE;
    needUpdate = true;
    ssm_->UpdateNormalSessionAvoidArea(persistentId, sceneSession, needUpdate);
    EXPECT_EQ(needUpdate, false);

    sceneSession->property_->type_ = WindowType::APP_SUB_WINDOW_END;
    sceneSession->isVisible_ = true;
    needUpdate = true;
    ssm_->UpdateNormalSessionAvoidArea(persistentId, sceneSession, needUpdate);
    EXPECT_EQ(needUpdate, false);

    ssm_->avoidAreaListenerSessionSet_.insert(1);
    ssm_->UpdateNormalSessionAvoidArea(persistentId, sceneSession, needUpdate);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, ssm_->HandleSecureSessionShouldHide(nullptr));
}

/**
 * @tc.name: UpdateAvoidSessionAvoidArea
 * @tc.desc: UpdateAvoidSessionAvoidArea
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, UpdateAvoidSessionAvoidArea, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->avoidAreaListenerSessionSet_.insert(0);
    ssm_->avoidAreaListenerSessionSet_.insert(1);
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    WindowType type = WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT;
    bool needUpdate = true;
    ssm_->UpdateAvoidSessionAvoidArea(type, needUpdate);

    ASSERT_NE(sceneSession->property_, nullptr);
    sceneSession->property_->type_ = WindowType::APP_MAIN_WINDOW_END;
    sceneSession->isVisible_ = true;
    ssm_->UpdateAvoidSessionAvoidArea(type, needUpdate);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, ssm_->HandleSecureSessionShouldHide(nullptr));
}

/**
 * @tc.name: FindSessionByAffinity
 * @tc.desc: FindSessionByAffinity
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, FindSessionByAffinity, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    std::string affinity = "";
    sptr<SceneSession> sceneSession = ssm_->FindSessionByAffinity(affinity);
    EXPECT_EQ(sceneSession, nullptr);
}

/**
 * @tc.name: SetSystemAnimatedScenes
 * @tc.desc: SetSystemAnimatedScenes
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, SetSystemAnimatedScenes, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SystemAnimatedSceneType sceneType = SystemAnimatedSceneType::SCENE_ENTER_MISSION_CENTER;
    auto result = ssm_->SetSystemAnimatedScenes(sceneType);
    EXPECT_EQ(result, WMError::WM_OK);
}

/**
 * @tc.name: GetProcessDrawingState
 * @tc.desc: GetProcessDrawingState
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, GetProcessDrawingState, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    uint64_t windowId = 10;
    int32_t pid = 1;
    bool currentDrawingContentState = true;
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession01 = nullptr;
    sptr<SceneSession> sceneSession02 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession03 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession04 = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession02, nullptr);
    ASSERT_NE(sceneSession03, nullptr);
    ASSERT_NE(sceneSession04, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession01));
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession02));
    ssm_->sceneSessionMap_.insert(std::make_pair(3, sceneSession03));
    ssm_->sceneSessionMap_.insert(std::make_pair(4, sceneSession04));
    sceneSession02->SetCallingPid(pid);
    struct RSSurfaceNodeConfig config;
    sceneSession02->surfaceNode_ = RSSurfaceNode::Create(config);
    ASSERT_NE(sceneSession02->surfaceNode_, nullptr);
    sceneSession03->SetCallingPid(pid);
    sceneSession03->surfaceNode_ = nullptr;
    sceneSession04->SetCallingPid(6);
    auto result = ssm_->GetProcessDrawingState(windowId, pid, currentDrawingContentState);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: GetPreWindowDrawingState
 * @tc.desc: GetPreWindowDrawingState
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, GetPreWindowDrawingState, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    uint64_t surfaceId = 0;
    int32_t pid = 10;
    bool result = ssm_->GetPreWindowDrawingState(surfaceId, pid, true);
    EXPECT_EQ(result, false);

    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession01 = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession01, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession01));
    struct RSSurfaceNodeConfig config;
    sceneSession01->surfaceNode_ = RSSurfaceNode::Create(config);
    ASSERT_NE(sceneSession01->surfaceNode_, nullptr);
    sceneSession01->surfaceNode_->id_ = 10;
    surfaceId = 10;
    result = ssm_->GetPreWindowDrawingState(surfaceId, pid, true);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: GetWindowDrawingContentChangeInfo
 * @tc.desc: GetWindowDrawingContentChangeInfo
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, GetWindowDrawingContentChangeInfo, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(0, nullptr));
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));

    std::vector<std::pair<uint64_t, bool>> currDrawingContentData;
    currDrawingContentData.push_back(std::make_pair(0, false));
    currDrawingContentData.push_back(std::make_pair(1, true));

    struct RSSurfaceNodeConfig config;
    sceneSession->surfaceNode_ = RSSurfaceNode::Create(config);
    ASSERT_NE(sceneSession->surfaceNode_, nullptr);
    sceneSession->surfaceNode_->id_ = 0;
    sceneSession->SetCallingPid(0);
    sceneSession->SetDrawingContentState(true);

    auto result = ssm_->GetWindowDrawingContentChangeInfo(currDrawingContentData);
    EXPECT_EQ(result, currDrawingContentData);

    sceneSession->SetCallingPid(2);
    result = ssm_->GetWindowDrawingContentChangeInfo(currDrawingContentData);
    EXPECT_NE(result, currDrawingContentData);
}

/**
 * @tc.name: DealwithDrawingContentChange
 * @tc.desc: DealwithDrawingContentChange
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, DealwithDrawingContentChange, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(0, nullptr));

    std::vector<std::pair<uint64_t, bool>> drawingContentChangeInfo;
    drawingContentChangeInfo.push_back(std::make_pair(0, true));
    drawingContentChangeInfo.push_back(std::make_pair(1, true));
    ssm_->DealwithDrawingContentChange(drawingContentChangeInfo);

    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    struct RSSurfaceNodeConfig config;
    sceneSession->surfaceNode_ = RSSurfaceNode::Create(config);
    ASSERT_NE(sceneSession->surfaceNode_, nullptr);
    sceneSession->surfaceNode_->id_ = 1;
    ssm_->DealwithDrawingContentChange(drawingContentChangeInfo);

    ssm_->openDebugTrace = true;
    ssm_->DealwithDrawingContentChange(drawingContentChangeInfo);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, ssm_->HandleSecureSessionShouldHide(nullptr));
}

/**
 * @tc.name: GetSubSceneSession
 * @tc.desc: GetSubSceneSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, GetSubSceneSession, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession01 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession02 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession03 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<Session> session04 = sptr<Session>::MakeSptr(info);
    sptr<Session> session05 = sptr<Session>::MakeSptr(info);
    ASSERT_NE(sceneSession01, nullptr);
    ASSERT_NE(sceneSession02, nullptr);
    ASSERT_NE(sceneSession03, nullptr);
    ASSERT_NE(session04, nullptr);
    ASSERT_NE(session05, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(0, nullptr));
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession01));
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession02));
    ssm_->sceneSessionMap_.insert(std::make_pair(3, sceneSession03));
    int32_t parentWindowId = INVALID_SESSION_ID;
    sceneSession01->parentSession_ = session04;
    sceneSession02->parentSession_ = session05;
    session05->persistentId_ = 5;
    std::vector<sptr<SceneSession>> subSessions = ssm_->GetSubSceneSession(parentWindowId);
    EXPECT_EQ(subSessions.size(), 1);
}

/**
 * @tc.name: RemoveDuplicateSubSession
 * @tc.desc: RemoveDuplicateSubSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, RemoveDuplicateSubSession, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    std::vector<std::pair<uint64_t, WindowVisibilityState>> visibilityChangeInfo;
    std::vector<sptr<SceneSession>> subSessions;
    visibilityChangeInfo.push_back(std::make_pair(0, WINDOW_VISIBILITY_STATE_NO_OCCLUSION));
    visibilityChangeInfo.push_back(std::make_pair(1, WINDOW_VISIBILITY_STATE_PARTICALLY_OCCLUSION));

    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession01 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession02 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession03 = nullptr;
    ASSERT_NE(sceneSession01, nullptr);
    ASSERT_NE(sceneSession02, nullptr);
    struct RSSurfaceNodeConfig config;
    sceneSession01->surfaceNode_ = RSSurfaceNode::Create(config);
    ASSERT_NE(sceneSession01->surfaceNode_, nullptr);
    sceneSession01->surfaceNode_->id_ = 0;

    ssm_->sceneSessionMap_.insert(std::make_pair(0, nullptr));
    ssm_->RemoveDuplicateSubSession(visibilityChangeInfo, subSessions);

    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession01));
    sceneSession02->persistentId_ = 2;
    subSessions.push_back(sceneSession01);
    subSessions.push_back(sceneSession02);
    subSessions.push_back(sceneSession03);
    ssm_->RemoveDuplicateSubSession(visibilityChangeInfo, subSessions);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, ssm_->HandleSecureSessionShouldHide(nullptr));
}

/**
 * @tc.name: UpdateSubWindowVisibility
 * @tc.desc: UpdateSubWindowVisibility
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, UpdateSubWindowVisibility, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    WindowVisibilityState visibleState = WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION;
    std::vector<std::pair<uint64_t, WindowVisibilityState>> visibilityChangeInfo;
    std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
    std::string visibilityInfo = "";
    ssm_->UpdateSubWindowVisibility(sceneSession, visibleState, visibilityChangeInfo,
                                    windowVisibilityInfos, visibilityInfo);

    ASSERT_NE(sceneSession->property_, nullptr);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ssm_->UpdateSubWindowVisibility(sceneSession, visibleState, visibilityChangeInfo,
                                    windowVisibilityInfos, visibilityInfo);

    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    visibleState = WINDOW_VISIBILITY_STATE_PARTICALLY_OCCLUSION;
    ssm_->sceneSessionMap_.insert(std::make_pair(0, nullptr));
    ssm_->UpdateSubWindowVisibility(sceneSession, visibleState, visibilityChangeInfo,
                                    windowVisibilityInfos, visibilityInfo);

    sptr<SceneSession> sceneSession01 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession02 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession03 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<Session> session03 = sptr<Session>::MakeSptr(info);
    ASSERT_NE(sceneSession01, nullptr);
    ASSERT_NE(sceneSession02, nullptr);
    ASSERT_NE(sceneSession03, nullptr);
    ASSERT_NE(session03, nullptr);
    sceneSession01->SetParentSession(session03);
    sceneSession02->SetParentSession(session03);
    sceneSession03->SetParentSession(session03);
    sceneSession02->SetCallingPid(2);
    sceneSession03->SetCallingPid(3);
    sceneSession03->state_ = SessionState::STATE_FOREGROUND;
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession01));
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession02));
    ssm_->sceneSessionMap_.insert(std::make_pair(3, sceneSession03));
    ssm_->UpdateSubWindowVisibility(sceneSession, visibleState, visibilityChangeInfo,
                                    windowVisibilityInfos, visibilityInfo);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, ssm_->HandleSecureSessionShouldHide(nullptr));
}

/**
 * @tc.name: SelectSesssionFromMap
 * @tc.desc: SelectSesssionFromMap
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, SelectSesssionFromMap, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    uint64_t surfaceId = 6;
    uint64_t surfaceId02 = 7;
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession;
    sptr<SceneSession> sceneSession01 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession02 = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession01, nullptr);
    ASSERT_NE(sceneSession02, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(0, sceneSession));
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession01));
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession02));
    sceneSession01->surfaceNode_ = nullptr;
    struct RSSurfaceNodeConfig config;
    sceneSession02->surfaceNode_ = RSSurfaceNode::Create(config);
    ASSERT_NE(sceneSession02->surfaceNode_, nullptr);
    NodeId id = 6;
    sceneSession02->surfaceNode_->SetId(id);
    EXPECT_EQ(sceneSession02->surfaceNode_->id_, 6);

    sptr<SceneSession> result = ssm_->SelectSesssionFromMap(surfaceId);
    EXPECT_EQ(result, sceneSession02);
    result = ssm_->SelectSesssionFromMap(surfaceId02);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetAccessibilityWindowInfo
 * @tc.desc: GetAccessibilityWindowInfo
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, GetAccessibilityWindowInfo, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    sptr<SceneSession> sceneSession;
    sptr<SceneSession> sceneSession01 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession02 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession03 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession04 = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession01, nullptr);
    ASSERT_NE(sceneSession02, nullptr);
    ASSERT_NE(sceneSession03, nullptr);
    ASSERT_NE(sceneSession04, nullptr);
    sceneSession01->SetForceTouchable(true);
    sceneSession01->isVisible_ = true;
    ASSERT_NE(sceneSession01->property_, nullptr);
    sceneSession01->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sceneSession01->property_->SetParentPersistentId(4);
    sceneSession02->SetForceTouchable(false);
    sceneSession03->SetForceTouchable(true);
    sceneSession03->isVisible_ = true;
    ASSERT_NE(sceneSession03->property_, nullptr);
    sceneSession03->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sceneSession03->property_->SetParentPersistentId(6);
    ssm_->sceneSessionMap_.insert(std::make_pair(0, sceneSession));
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession01));
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession02));
    ssm_->sceneSessionMap_.insert(std::make_pair(3, sceneSession03));
    ssm_->sceneSessionMap_.insert(std::make_pair(4, sceneSession04));
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    auto result = ssm_->GetAccessibilityWindowInfo(accessibilityInfo);
    EXPECT_EQ(result, WMError::WM_OK);
}

/**
 * @tc.name: ShiftAppWindowFocus02
 * @tc.desc: ShiftAppWindowFocus
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, ShiftAppWindowFocus02, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->SetFocusedSessionId(INVALID_SESSION_ID);
    int32_t sourcePersistentId = INVALID_SESSION_ID;
    int32_t targetPersistentId = INVALID_SESSION_ID;
    auto result = ssm_->ShiftAppWindowFocus(sourcePersistentId, targetPersistentId);
    EXPECT_EQ(result, WSError::WS_DO_NOTHING);

    targetPersistentId = 1;
    result = ssm_->ShiftAppWindowFocus(sourcePersistentId, targetPersistentId);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);

    SessionInfo info;
    info.abilityName_ = "abilityName";
    info.bundleName_ = "bundleName";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(INVALID_SESSION_ID, sceneSession));
    ASSERT_NE(sceneSession->property_, nullptr);
    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    result = ssm_->ShiftAppWindowFocus(sourcePersistentId, targetPersistentId);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);

    SessionInfo info01;
    info01.abilityName_ = "abilityName01";
    info01.bundleName_ = "bundleName01";
    sptr<SceneSession> sceneSession01 = sptr<SceneSession>::MakeSptr(info01, nullptr);
    ASSERT_NE(sceneSession01, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession01));
    ASSERT_NE(sceneSession01->property_, nullptr);
    sceneSession01->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    result = ssm_->ShiftAppWindowFocus(sourcePersistentId, targetPersistentId);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_CALLING);
}

/**
 * @tc.name: UpdateTitleInTargetPos
 * @tc.desc: UpdateTitleInTargetPos
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, UpdateTitleInTargetPos, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "abilityName";
    info.bundleName_ = "bundleName";
    info.isSystem_ = true;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(0, sceneSession));
    auto result = ssm_->UpdateTitleInTargetPos(1, true, 1);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_WINDOW);

    result = ssm_->UpdateTitleInTargetPos(0, true, 1);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);
}

/**
 * @tc.name: GetIsLayoutFullScreen
 * @tc.desc: GetIsLayoutFullScreen
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, GetIsLayoutFullScreen, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "abilityName";
    info.bundleName_ = "bundleName";
    sptr<SceneSession> sceneSession01 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession02 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession03 = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SceneSession> sceneSession04 = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession01, nullptr);
    ASSERT_NE(sceneSession02, nullptr);
    ASSERT_NE(sceneSession03, nullptr);
    ASSERT_NE(sceneSession04, nullptr);
    ASSERT_NE(sceneSession01->property_, nullptr);
    sceneSession01->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);

    ASSERT_NE(sceneSession02->property_, nullptr);
    sceneSession02->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession02->SetSessionState(SessionState::STATE_DISCONNECT);

    ASSERT_NE(sceneSession03->property_, nullptr);
    sceneSession03->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession03->SetSessionState(SessionState::STATE_ACTIVE);
    sceneSession03->property_->SetWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);

    ASSERT_NE(sceneSession04->property_, nullptr);
    sceneSession04->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession04->SetSessionState(SessionState::STATE_FOREGROUND);
    sceneSession04->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sceneSession04->property_->SetIsLayoutFullScreen(true);

    ASSERT_NE(sceneSession04->property_, nullptr);
    sceneSession04->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession04->SetSessionState(SessionState::STATE_FOREGROUND);
    sceneSession04->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sceneSession04->property_->SetIsLayoutFullScreen(false);

    ssm_->sceneSessionMap_.insert(std::make_pair(0, nullptr));
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession01));
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession02));
    ssm_->sceneSessionMap_.insert(std::make_pair(3, sceneSession03));
    bool isLayoutFullScreen = true;
    auto result = ssm_->GetIsLayoutFullScreen(isLayoutFullScreen);
    EXPECT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdateExtWindowFlags
 * @tc.desc: UpdateExtWindowFlags
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, UpdateExtWindowFlags, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    uint32_t extWindowFlags = 0;
    uint32_t extWindowActions = 0;
    auto ret = ssm_->UpdateExtWindowFlags(nullptr, extWindowFlags, extWindowActions);
    EXPECT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: AddOrRemoveSecureSession02
 * @tc.desc: AddOrRemoveSecureSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, AddOrRemoveSecureSession02, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    int32_t persistentId = 1;
    bool shouldHide = true;
    SessionInfo info;
    info.abilityName_ = "secureSession";
    info.bundleName_ = "secureSession";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(0, nullptr));
    ssm_->sceneSessionMap_.insert(std::make_pair(persistentId, sceneSession));
    auto result = ssm_->AddOrRemoveSecureSession(0, shouldHide);
    EXPECT_EQ(result, WSError::WS_OK);
    result = ssm_->AddOrRemoveSecureSession(persistentId, shouldHide);
    EXPECT_EQ(result, WSError::WS_OK);
    static constexpr uint32_t WAIT_SYNC_IN_NS = 500000;
    usleep(WAIT_SYNC_IN_NS);
}

/**
 * @tc.name: GetSessionSnapshotPixelMap
 * @tc.desc: GetSessionSnapshotPixelMap
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, GetSessionSnapshotPixelMap, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "GetPixelMap";
    info.bundleName_ = "GetPixelMap1";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    int32_t persistentId = 1;
    float scaleParam = 0.5f;
    auto result = ssm_->GetSessionSnapshotPixelMap(persistentId, scaleParam);
    EXPECT_EQ(result, nullptr);

    sceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    std::string bundleName = "testBundleName";
    int32_t testpersistentId = 1;
    sceneSession->scenePersistence_ = sptr<ScenePersistence>::MakeSptr(bundleName, testpersistentId);
    ASSERT_NE(sceneSession->scenePersistence_, nullptr);
    struct RSSurfaceNodeConfig config;
    sceneSession->surfaceNode_ = RSSurfaceNode::Create(config);
    ASSERT_NE(sceneSession->surfaceNode_, nullptr);
    sceneSession->bufferAvailable_ = true;
    result = ssm_->GetSessionSnapshotPixelMap(persistentId, scaleParam);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetStartupPageFromResource
 * @tc.desc: GetStartupPageFromResource
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, GetStartupPageFromResource, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    AppExecFwk::AbilityInfo abilityInfo;
    EXPECT_EQ(ssm_->GetResourceManager(abilityInfo), nullptr);
    std::string path = "testPath";
    uint32_t bgColor = 0;
    bool result = ssm_->GetStartupPageFromResource(abilityInfo, path, bgColor);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: GetStartupPage
 * @tc.desc: GetStartupPage
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, GetStartupPage, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo sessionInfo;
    sessionInfo.moduleName_ = "moduleName";
    sessionInfo.abilityName_ = "abilityName";
    sessionInfo.bundleName_ = "bundleName";
    std::string path = "testPath";
    uint32_t bgColor = 0;
    bool result = ssm_->GetStartingWindowInfoFromCache(sessionInfo, path, bgColor);
    EXPECT_EQ(result, false);

    std::map<std::string, StartingWindowInfo> startingWindowInfoMap;
    StartingWindowInfo startingWindowInfo;
    auto key = sessionInfo.moduleName_ + sessionInfo.abilityName_;
    startingWindowInfoMap.insert(std::make_pair(key, startingWindowInfo));
    ssm_->startingWindowMap_.insert({sessionInfo.bundleName_, startingWindowInfoMap});
    result = ssm_->GetStartingWindowInfoFromCache(sessionInfo, path, bgColor);
    ssm_->GetStartupPage(sessionInfo, path, bgColor);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: UpdatePropertyDragEnabled
 * @tc.desc: UpdatePropertyDragEnabled
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, UpdatePropertyDragEnabled, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "abilityName";
    info.bundleName_ = "bundleName";
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetSystemCalling(true);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    auto result = ssm_->UpdatePropertyDragEnabled(property, sceneSession);
    ASSERT_EQ(result, WMError::WM_OK);
}

/**
 * @tc.name: UpdatePropertyRaiseEnabled
 * @tc.desc: UpdatePropertyRaiseEnabled
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, UpdatePropertyRaiseEnabled, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "abilityName";
    info.bundleName_ = "bundleName";
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    property->SetSystemCalling(true);
    auto result = ssm_->UpdatePropertyRaiseEnabled(property, sceneSession);
    EXPECT_EQ(result, WMError::WM_OK);
}

/**
 * @tc.name: HandleHideNonSystemFloatingWindows
 * @tc.desc: HandleHideNonSystemFloatingWindows
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, HandleHideNonSystemFloatingWindows, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "abilityName";
    info.bundleName_ = "bundleName";
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    sceneSession->property_->hideNonSystemFloatingWindows_ = true;
    property->SetHideNonSystemFloatingWindows(false);
    sceneSession->isVisible_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    ssm_->HandleHideNonSystemFloatingWindows(property, sceneSession);

    sceneSession->property_->hideNonSystemFloatingWindows_ = false;
    property->SetHideNonSystemFloatingWindows(true);
    ssm_->HandleHideNonSystemFloatingWindows(property, sceneSession);

    sceneSession->isVisible_ = false;
    ssm_->HandleHideNonSystemFloatingWindows(property, sceneSession);
    EXPECT_EQ(Session::IsScbCoreEnabled(), true);
}

/**
 * @tc.name: GetAllClearableSessions
 * @tc.desc: GetAllClearableSessions
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest4, GetAllClearableSessions, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "bundleName";
    sessionInfo.abilityName_ = "abilityName";
    sessionInfo.abilityInfo = nullptr;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, ssm_);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession));
    SessionInfo sessionInfo1;
    sessionInfo1.bundleName_ = "bundleName";
    sessionInfo1.abilityName_ = "abilityName";
    sessionInfo1.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    ASSERT_NE(nullptr, sessionInfo1.abilityInfo);
    sessionInfo1.abilityInfo->excludeFromMissions = false;
    sessionInfo1.abilityInfo->unclearableMission = false;
    sessionInfo1.isSystem_ = false;
    sessionInfo1.lockedState = false;
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo1, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(3, sceneSession1));
    std::vector<sptr<SceneSession>> sessionVector;
    sessionVector.clear();
    ssm_->GetAllClearableSessions(sessionVector);
    EXPECT_FALSE(sessionVector.empty());
}

/**
 * @tc.name: UpdateBrightness
 * @tc.desc: UpdateBrightness
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest4, UpdateBrightness, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo sessionInfo;
    sessionInfo.isSystem_ = false;
    sessionInfo.bundleName_ = "bundleName";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ASSERT_NE(sceneSession->property_, nullptr);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    WSError result = ssm_->UpdateBrightness(1);
    EXPECT_EQ(result, WSError::WS_DO_NOTHING);

    SessionInfo sessionInfo02;
    sessionInfo02.isSystem_ = true;
    sessionInfo02.bundleName_ = "bundleName";
    sptr<SceneSession> sceneSession02 = sptr<SceneSession>::MakeSptr(sessionInfo02, nullptr);
    ASSERT_NE(sceneSession02, nullptr);
    ASSERT_NE(sceneSession02->property_, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession02));
    sceneSession02->property_->SetBrightness(50.f);
    ssm_->SetDisplayBrightness(40.f);
    result = ssm_->UpdateBrightness(2);
    EXPECT_EQ(ssm_->displayBrightness_, 50);
    EXPECT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: RegisterSessionExceptionFunc
 * @tc.desc: RegisterSessionExceptionFunc
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest4, RegisterSessionExceptionFunc, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "bundleName";
    sessionInfo.persistentId_ = 1;
    sessionInfo.isSystem_ = true;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(sessionInfo.persistentId_, sceneSession));
    std::shared_ptr<SessionListenerController> listenerController =
        std::make_shared<SessionListenerController>();
    ssm_->listenerController_ = listenerController;
    ASSERT_NE(ssm_->listenerController_, nullptr);
    ssm_->RegisterSessionExceptionFunc(sceneSession);

    sptr<AAFwk::SessionInfo> abilitySessionInfo = sptr<AAFwk::SessionInfo>::MakeSptr();
    ASSERT_NE(abilitySessionInfo, nullptr);
    WSError result = sceneSession->NotifySessionExceptionInner(abilitySessionInfo, false, false);
    EXPECT_EQ(result, WSError::WS_OK);

    abilitySessionInfo->errorCode = 1;
    result = sceneSession->NotifySessionExceptionInner(abilitySessionInfo, false, false);
    EXPECT_EQ(result, WSError::WS_OK);

    abilitySessionInfo->errorCode = 0;
    result = sceneSession->NotifySessionExceptionInner(abilitySessionInfo, false, false);
    EXPECT_EQ(result, WSError::WS_OK);

    sessionInfo.isSystem_ = false;
    result = sceneSession->NotifySessionExceptionInner(abilitySessionInfo, false, false);
    EXPECT_EQ(result, WSError::WS_OK);

    ssm_->listenerController_ = nullptr;
    result = sceneSession->NotifySessionExceptionInner(abilitySessionInfo, false, false);
    EXPECT_EQ(result, WSError::WS_OK);

    sessionInfo.persistentId_ = 2;
    result = sceneSession->NotifySessionExceptionInner(abilitySessionInfo, false, false);
    EXPECT_EQ(result, WSError::WS_OK);
    usleep(WAIT_SYNC_IN_NS);
}

/**
 * @tc.name: RegisterSessionSnapshotFunc
 * @tc.desc: RegisterSessionSnapshotFunc
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest4, RegisterSessionSnapshotFunc, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "bundleName";
    sessionInfo.persistentId_ = 1;
    sessionInfo.isSystem_ = false;
    sessionInfo.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    ASSERT_NE(sessionInfo.abilityInfo, nullptr);
    sessionInfo.abilityInfo->excludeFromMissions = true;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->scenePersistence_ = sptr<ScenePersistence>::MakeSptr("bundleName", 1);
    ASSERT_NE(sceneSession->scenePersistence_, nullptr);
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    sceneSession->surfaceNode_ = std::make_shared<RSSurfaceNode>(rsSurfaceNodeConfig, true, 0);
    ASSERT_NE(sceneSession->surfaceNode_, nullptr);
    sceneSession->surfaceNode_->bufferAvailable_ = true;
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->listenerController_ = std::make_shared<SessionListenerController>();
    ASSERT_NE(ssm_->listenerController_, nullptr);
    ssm_->RegisterSessionSnapshotFunc(sceneSession);
    EXPECT_EQ(sceneSession->Snapshot(1.f), nullptr);

    sessionInfo.abilityInfo->excludeFromMissions = false;
    EXPECT_EQ(sceneSession->Snapshot(1.f), nullptr);

    ssm_->listenerController_ = nullptr;
    EXPECT_EQ(sceneSession->Snapshot(1.f), nullptr);

    sessionInfo.abilityInfo = nullptr;
    EXPECT_EQ(sceneSession->Snapshot(1.f), nullptr);

    sessionInfo.isSystem_ = true;
    EXPECT_EQ(sceneSession->Snapshot(1.f), nullptr);

    sessionInfo.persistentId_ = 2;
    EXPECT_EQ(sceneSession->Snapshot(1.f), nullptr);
    usleep(WAIT_SYNC_IN_NS);
}

/**
 * @tc.name: GetSessionDumpInfo
 * @tc.desc: GetSessionDumpInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest4, GetSessionDumpInfo, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    std::string dumpInfo = "testDumpInfo";
    std::vector<std::string> params = {"testDumpInfo"};
    WSError result = ssm_->GetSessionDumpInfo(params, dumpInfo);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_OPERATION);

    params.clear();
    params.push_back("-w");
    params.push_back("a");
    result = ssm_->GetSessionDumpInfo(params, dumpInfo);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_OPERATION);

    params.clear();
    params.push_back("-b");
    params.push_back("a");
    result = ssm_->GetSessionDumpInfo(params, dumpInfo);
    EXPECT_EQ(result, WSError::WS_OK);

    params.clear();
    params.push_back("-p");
    params.push_back("1");
    result = ssm_->GetSessionDumpInfo(params, dumpInfo);
    EXPECT_EQ(result, WSError::WS_OK);

    params.clear();
    params.push_back("-b");
    params.push_back("a");
    result = ssm_->GetSessionDumpInfo(params, dumpInfo);
    EXPECT_EQ(result, WSError::WS_OK);

    params.clear();
    params.push_back("testDumpInfo");
    params.push_back("a");
    result = ssm_->GetSessionDumpInfo(params, dumpInfo);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_OPERATION);
}

/**
 * @tc.name: CheckParentSessionVisible
 * @tc.desc: CheckParentSessionVisible
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest4, CheckParentSessionVisible, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "bundleName";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ASSERT_NE(sceneSession->property_, nullptr);
    sceneSession->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    sceneSession->property_->SetParentPersistentId(1);
    sceneSession->isVisible_ = false;
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    bool result = ssm_->CheckParentSessionVisible(sceneSession);
    EXPECT_EQ(result, false);

    bool testRet = sceneSession->IsScbCoreEnabled();
    EXPECT_EQ(testRet, true);

    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    sceneSession->isVisible_ = true;
    result = ssm_->CheckParentSessionVisible(sceneSession);
    EXPECT_EQ(result, true);

    sceneSession->property_->SetParentPersistentId(2);
    result = ssm_->CheckParentSessionVisible(sceneSession);
    EXPECT_EQ(result, true);

    sceneSession->property_->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    result = ssm_->CheckParentSessionVisible(sceneSession);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: GetTopFocusableNonAppSession
 * @tc.desc: GetTopFocusableNonAppSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest4, GetTopFocusableNonAppSession, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "bundleName";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ASSERT_NE(sceneSession->property_, nullptr);

    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    sceneSession->property_->SetFocusable(true);
    sceneSession->isVisible_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    sptr<SceneSession> result = ssm_->GetTopFocusableNonAppSession();
    EXPECT_EQ(result, sceneSession);

    sceneSession->isVisible_ = false;
    result = ssm_->GetTopFocusableNonAppSession();
    EXPECT_EQ(result, nullptr);

    sceneSession->property_->SetFocusable(false);
    result = ssm_->GetTopFocusableNonAppSession();
    EXPECT_EQ(result, nullptr);

    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    result = ssm_->GetTopFocusableNonAppSession();
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetNextFocusableSession
 * @tc.desc: GetNextFocusableSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest4, GetNextFocusableSession, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "bundleName";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sptr<SceneSession> sceneSession02 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sptr<SceneSession> sceneSession03 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sptr<SceneSession> sceneSession04 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sptr<SceneSession> sceneSession05 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ASSERT_NE(sceneSession02, nullptr);
    ASSERT_NE(sceneSession03, nullptr);
    ASSERT_NE(sceneSession04, nullptr);
    ASSERT_NE(sceneSession05, nullptr);
    ASSERT_NE(sceneSession->property_, nullptr);

    sceneSession->SetForceHideState(ForceHideState::NOT_HIDDEN);
    sceneSession->property_->SetFocusable(true);
    sceneSession->property_->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sceneSession->isVisible_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    sceneSession->SetZOrder(1);

    sceneSession02->SetFocusable(false);
    sceneSession02->SetZOrder(2);

    sceneSession03->SetZOrder(3);

    sceneSession04->SetForceHideState(ForceHideState::HIDDEN_WHEN_FOCUSED);
    sceneSession04->SetZOrder(4);

    sceneSession05->persistentId_ = 1;
    sceneSession05->SetZOrder(5);

    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession02));
    ssm_->sceneSessionMap_.insert(std::make_pair(3, sceneSession03));
    ssm_->sceneSessionMap_.insert(std::make_pair(4, sceneSession04));
    ssm_->sceneSessionMap_.insert(std::make_pair(5, sceneSession05));
    sptr<SceneSession> result = ssm_->GetNextFocusableSession(1);
    EXPECT_EQ(result, sceneSession);
}

/**
 * @tc.name: GetTopNearestBlockingFocusSession
 * @tc.desc: GetTopNearestBlockingFocusSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest4, GetTopNearestBlockingFocusSession, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "bundleName";
    sessionInfo.isSystem_ = true;
    sptr<SceneSession> sceneSession01 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sptr<SceneSession> sceneSession02 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sptr<SceneSession> sceneSession03 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sptr<SceneSession> sceneSession04 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    sptr<SceneSession> parentSceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(sceneSession01, nullptr);
    ASSERT_NE(sceneSession02, nullptr);
    ASSERT_NE(sceneSession03, nullptr);
    ASSERT_NE(sceneSession04, nullptr);
    ASSERT_NE(parentSceneSession, nullptr);

    sceneSession01->SetZOrder(1);
    sceneSession01->isVisible_ = true;
    sceneSession01->SetSessionState(SessionState::STATE_FOREGROUND);

    sceneSession02->SetZOrder(2);
    sceneSession02->isVisible_ = true;
    sceneSession02->SetSessionState(SessionState::STATE_FOREGROUND);
    sceneSession02->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    sceneSession02->blockingFocus_ = false;

    sceneSession03->SetZOrder(3);
    sceneSession03->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    parentSceneSession->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    sceneSession03->property_->SetParentPersistentId(8);

    sceneSession04->SetZOrder(4);
    sceneSession04->property_->SetParentPersistentId(1);

    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession01));
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession02));
    ssm_->sceneSessionMap_.insert(std::make_pair(3, sceneSession03));
    ssm_->sceneSessionMap_.insert(std::make_pair(4, sceneSession04));
    ssm_->sceneSessionMap_.insert(std::make_pair(8, parentSceneSession));

    sptr<SceneSession> ret = ssm_->GetTopNearestBlockingFocusSession(0, true);
    EXPECT_EQ(ret, sceneSession01);

    ret = ssm_->GetTopNearestBlockingFocusSession(10, true);
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name: RequestFocusSpecificCheck
 * @tc.desc: RequestFocusSpecificCheck
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest4, RequestFocusSpecificCheck, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "bundleName";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    bool byForeground = true;
    FocusChangeReason reason = FocusChangeReason::CLIENT_REQUEST;
    sceneSession->SetForceHideState(ForceHideState::HIDDEN_WHEN_FOCUSED);
    WSError result = ssm_->RequestFocusSpecificCheck(sceneSession, byForeground, reason);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_OPERATION);

    sceneSession->SetForceHideState(ForceHideState::NOT_HIDDEN);
    sptr<SceneSession> sceneSession01 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(sceneSession01, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(0, sceneSession01));
    sceneSession01->parentSession_ = sceneSession;
    result = ssm_->RequestFocusSpecificCheck(sceneSession, byForeground, reason);
    EXPECT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: ProcessModalExtensionPointDown
 * @tc.desc: ProcessModalExtensionPointDown
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest4, ProcessModalExtensionPointDown, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<IRemoteObject> token = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(token, nullptr);
    int32_t posX = 1;
    int32_t posY = 1;
    ExtensionWindowAbilityInfo extensionWindowAbilityInfo;
    extensionWindowAbilityInfo.persistentId = 1;
    extensionWindowAbilityInfo.parentId = 2;
    ssm_->extSessionInfoMap_.insert(std::make_pair(token, extensionWindowAbilityInfo));

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "bundleName";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession));
    ssm_->ProcessModalExtensionPointDown(token, posX, posY);

    ExtensionWindowEventInfo eventInfo;
    eventInfo.pid = 0;
    eventInfo.persistentId = 1;
    sceneSession->modalUIExtensionInfoList_.push_back(eventInfo);
    ssm_->ProcessModalExtensionPointDown(token, posX, posY);

    eventInfo.persistentId = 4;
    ssm_->ProcessModalExtensionPointDown(token, posX, posY);

    eventInfo.pid = 4;
    ssm_->ProcessModalExtensionPointDown(token, posX, posY);

    sceneSession->modalUIExtensionInfoList_.clear();
    ssm_->ProcessModalExtensionPointDown(token, posX, posY);
    bool result = true;
    EXPECT_EQ(WSError::WS_OK, ssm_->GetFreeMultiWindowEnableState(result));
    usleep(WAIT_SYNC_IN_NS);
}
}
} // namespace Rosen
} // namespace OHOS
