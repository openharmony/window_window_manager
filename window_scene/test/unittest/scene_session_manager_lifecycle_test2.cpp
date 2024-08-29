/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "iremote_object_mocker.h"
#include "interfaces/include/ws_common.h"
#include "screen_fold_data.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/main_session.h"
#include "window_manager_agent.h"
#include "session_manager.h"
#include "zidl/window_manager_agent_interface.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
#include "application_info.h"
#include "context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
const std::string EMPTY_DEVICE_ID = "";
}
class SceneSessionManagerLifecycleTest2 : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp() override;

    void TearDown() override;

    static void SetVisibleForAccessibility(sptr<SceneSession>& sceneSession);
    int32_t GetTaskCount(sptr<SceneSession>& session);
    static sptr<SceneSessionManager> ssm_;
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

sptr<SceneSessionManager> SceneSessionManagerLifecycleTest2::ssm_ = nullptr;

void WindowChangedFuncTest(int32_t persistentId, WindowUpdateType type)
{
}

void ProcessStatusBarEnabledChangeFuncTest(bool enable)
{
}

void DumpRootSceneElementInfoFuncTest(const std::vector<std::string>& params, std::vector<std::string>& infos)
{
}

void SceneSessionManagerLifecycleTest2::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerLifecycleTest2::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerLifecycleTest2::SetUp()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerLifecycleTest2::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerLifecycleTest2::SetVisibleForAccessibility(sptr<SceneSession>& sceneSession)
{
    sceneSession->SetTouchable(true);
    sceneSession->forceTouchable_ = true;
    sceneSession->systemTouchable_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    sceneSession->foregroundInteractiveStatus_.store(true);
}

int32_t SceneSessionManagerLifecycleTest2::GetTaskCount(sptr<SceneSession>& session)
{
    std::string dumpInfo = session->handler_->GetEventRunner()->GetEventQueue()->DumpCurrentQueueSize();
    std::regex pattern("\\d+");
    std::smatch matches;
    int32_t taskNum = 0;
    while (std::regex_search(dumpInfo, matches, pattern)) {
        taskNum += std::stoi(matches.str());
        dumpInfo = matches.suffix();
    }
    return taskNum;
}

namespace {
/**
 * @tc.name: OnSessionStateChange
 * @tc.desc: OnSessionStateChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLifecycleTest2, OnSessionStateChange, Function | SmallTest | Level3)
{
    SessionState state = SessionState::STATE_FOREGROUND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    ssm_->OnSessionStateChange(1, state);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerLifecycleTest22";
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
HWTEST_F(SceneSessionManagerLifecycleTest2, OnSessionStateChange01, Function | SmallTest | Level3)
{
    SessionState state = SessionState::STATE_BACKGROUND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerLifecycleTest22";
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
HWTEST_F(SceneSessionManagerLifecycleTest2, OnSessionStateChange02, Function | SmallTest | Level3)
{
    SessionState state = SessionState::STATE_FOREGROUND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerLifecycleTest22";
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
 * @tc.name: NotifyWindowStateErrorFromMMI
 * @tc.desc: NotifyWindowStateErrorFromMMI
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLifecycleTest2, NotifyWindowStateErrorFromMMI, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->sceneSessionMap_.clear();
    SessionInfo info;
    info.abilityName_ = "SceneSessionManagerLifecycleTest2";
    info.bundleName_ = "NotifyWindowStateErrorFromMMI";
    info.screenId_ = 0;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, property);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sceneSession->property_ = property;
    sceneSession->SetCallingPid(100);

    SessionInfo info1;
    info1.abilityName_ = "SceneSessionManagerLifecycleTest2";
    info1.bundleName_ = "NotifyWindowStateErrorFromMMI1";
    info1.screenId_ = 0;
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(info1, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    sceneSession1->SetCallingPid(200);

    ssm_->sceneSessionMap_.insert({10086, sceneSession});
    ssm_->sceneSessionMap_.insert({10087, sceneSession1});
    ssm_->NotifyWindowStateErrorFromMMI(100, 10086);
    ASSERT_EQ(ret, 0);
}
}
} // namespace Rosen
} // namespace OHOS
