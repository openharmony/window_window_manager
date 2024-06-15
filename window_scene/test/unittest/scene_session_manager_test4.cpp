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
}


namespace {
/**
 * @tc.name: RequestSceneSession01
 * @tc.desc: SceneSesionManager test RequestSceneSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, RequestSceneSession01, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "RequestSceneSession01";
    info.bundleName_ = "RequestSceneSession01";
    info.persistentId_ = 0;
    auto windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();

    sptr<SceneSession> sceneSession = ssm_->RequestSceneSession(info, windowSessionProperty);
    ASSERT_EQ(sceneSession, nullptr);
}

/**
 * @tc.name: RequestSceneSession02
 * @tc.desc: SceneSesionManager test RequestSceneSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, RequestSceneSession02, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "RequestSceneSession";
    info.bundleName_ = "RequestSceneSession";
    info.persistentId_ = 1;
    info.isPersistentRecover_ = false;
    auto windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();

    sptr<SceneSession> sceneSession = ssm_->RequestSceneSession(info, windowSessionProperty);
    ASSERT_EQ(sceneSession, nullptr);
}

/**
 * @tc.name: RequestSceneSession03
 * @tc.desc: SceneSesionManager test RequestSceneSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, RequestSceneSession03, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "RequestSceneSession";
    info.bundleName_ = "RequestSceneSession";
    info.persistentId_ = 1;
    info.isPersistentRecover_ = true;
    auto windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    auto sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});

    sptr<SceneSession> getSceneSession = ssm_->RequestSceneSession(info, windowSessionProperty);
    ASSERT_EQ(getSceneSession, nullptr);
}

/**
 * @tc.name: RequestSceneSession04
 * @tc.desc: SceneSesionManager test RequestSceneSession
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, RequestSceneSession04, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "RequestSceneSession";
    info.bundleName_ = "RequestSceneSession";
    info.persistentId_ = 1;
    info.isPersistentRecover_ = true;
    auto windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();

    sptr<SceneSession> sceneSession = ssm_->RequestSceneSession(info, windowSessionProperty);
    ASSERT_EQ(sceneSession, nullptr);
}

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
 * @tc.name: RequestSceneSessionBackground01
 * @tc.desc: SceneSesionManager test RequestSceneSessionBackground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, RequestSceneSessionBackground01, Function | SmallTest | Level3)
{
    bool isDelegator = false;
    bool isToDesktop = false;
    bool isSaveSnapshot = true;
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionBackground";
    info.bundleName_ = "RequestSceneSessionBackground";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ASSERT_EQ(ssm_->RequestSceneSessionBackground(
        sceneSession, isDelegator, isToDesktop, isSaveSnapshot), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionBackground02
 * @tc.desc: SceneSesionManager test RequestSceneSessionBackground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, RequestSceneSessionBackground02, Function | SmallTest | Level3)
{
    bool isDelegator = false;
    bool isToDesktop = true;
    bool isSaveSnapshot = true;
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionBackground";
    info.bundleName_ = "RequestSceneSessionBackground";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_EQ(ssm_->RequestSceneSessionBackground(
        sceneSession, isDelegator, isToDesktop, isSaveSnapshot), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionBackground03
 * @tc.desc: SceneSesionManager test RequestSceneSessionBackground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, RequestSceneSessionBackground03, Function | SmallTest | Level3)
{
    bool isDelegator = false;
    bool isToDesktop = true;
    bool isSaveSnapshot = true;
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionBackground";
    info.bundleName_ = "RequestSceneSessionBackground";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->SetBrightness(sceneSession, 0.5);
    ASSERT_EQ(ssm_->RequestSceneSessionBackground(
        sceneSession, isDelegator, isToDesktop, isSaveSnapshot), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionBackground04
 * @tc.desc: SceneSesionManager test RequestSceneSessionBackground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, RequestSceneSessionBackground04, Function | SmallTest | Level3)
{
    bool isDelegator = true;
    bool isToDesktop = true;
    bool isSaveSnapshot = true;
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionBackground";
    info.bundleName_ = "RequestSceneSessionBackground";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_EQ(ssm_->RequestSceneSessionBackground(
        sceneSession, isDelegator, isToDesktop, isSaveSnapshot), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionDestruction01
 * @tc.desc: SceneSesionManager test RequestSceneSessionDestruction
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, RequestSceneSessionDestruction01, Function | SmallTest | Level3)
{
    bool needRemoveSession = false;
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionDestruction";
    info.bundleName_ = "RequestSceneSessionDestruction";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ASSERT_EQ(ssm_->RequestSceneSessionDestruction(
        sceneSession, needRemoveSession), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionDestruction02
 * @tc.desc: SceneSesionManager test RequestSceneSessionDestruction
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, RequestSceneSessionDestruction02, Function | SmallTest | Level3)
{
    bool needRemoveSession = false;
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionDestruction";
    info.bundleName_ = "RequestSceneSessionDestruction";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_EQ(ssm_->RequestSceneSessionDestruction(
        sceneSession, needRemoveSession), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionByCall01
 * @tc.desc: SceneSesionManager test RequestSceneSessionByCall
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, RequestSceneSessionByCall01, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionByCall";
    info.bundleName_ = "RequestSceneSessionByCall";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_EQ(ssm_->RequestSceneSessionByCall(sceneSession), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionByCall02
 * @tc.desc: SceneSesionManager test RequestSceneSessionByCall
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, RequestSceneSessionByCall02, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionByCall";
    info.bundleName_ = "RequestSceneSessionByCall";
    info.persistentId_ = 1;
    info.callState_ = static_cast<uint32_t>(AAFwk::CallToState::BACKGROUND);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_EQ(ssm_->RequestSceneSessionByCall(sceneSession), WSError::WS_OK);
}

/**
 * @tc.name: RequestSceneSessionByCall03
 * @tc.desc: SceneSesionManager test RequestSceneSessionByCall
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, RequestSceneSessionByCall03, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "RequestSceneSessionByCall";
    info.bundleName_ = "RequestSceneSessionByCall";
    info.persistentId_ = 1;
    info.callState_ = static_cast<uint32_t>(AAFwk::CallToState::FOREGROUND);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ASSERT_EQ(ssm_->RequestSceneSessionByCall(sceneSession), WSError::WS_OK);
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
 * @tc.name: ProcessBackEvent01
 * @tc.desc: SceneSesionManager test ProcessBackEvent
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest4, ProcessBackEvent01, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "ProcessBackEvent";
    info.bundleName_ = "ProcessBackEvent";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->SetFocusedSessionId(1);
    ASSERT_EQ(ssm_->ProcessBackEvent(), WSError::WS_OK);
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
}
} // namespace Rosen
} // namespace OHOS
