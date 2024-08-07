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
class SceneSessionManagerTest7 : public testing::Test {
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

sptr<SceneSessionManager> SceneSessionManagerTest7::ssm_ = nullptr;

bool SceneSessionManagerTest7::gestureNavigationEnabled_ = true;
ProcessGestureNavigationEnabledChangeFunc SceneSessionManagerTest7::callbackFunc_ = [](bool enable) {
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

void SceneSessionManagerTest7::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerTest7::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerTest7::SetUp()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerTest7::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    ssm_->sceneSessionMap_.clear();
}

namespace {
/**
 * @tc.name: UpdateSessionWindowVisibilityListener
 * @tc.desc: UpdateSessionWindowVisibilityListener
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, UpdateSessionWindowVisibilityListener, Function | SmallTest | Level3)
{
    int32_t persistentId = 1;
    bool haveListener = true;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    auto ret = ssm_->UpdateSessionWindowVisibilityListener(persistentId, haveListener);
    EXPECT_EQ(ret, WSError::WS_DO_NOTHING);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "UpdateSessionWindowVisibilityListener";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->callingPid_ = 65535;
    ssm_->sceneSessionMap_.insert(std::make_pair(persistentId, sceneSession));
    ret = ssm_->UpdateSessionWindowVisibilityListener(persistentId, haveListener);
    EXPECT_EQ(ret, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: ProcessVirtualPixelRatioChange
 * @tc.desc: ProcessVirtualPixelRatioChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, ProcessVirtualPixelRatioChange, Function | SmallTest | Level3)
{
    DisplayId defaultDisplayId = 0;
    sptr<DisplayInfo> displayInfo = nullptr;
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap;
    DisplayStateChangeType type = DisplayStateChangeType::BEFORE_SUSPEND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    displayInfo = sptr<DisplayInfo>::MakeSptr();
    ASSERT_NE(nullptr, displayInfo);
    ssm_->sceneSessionMap_.clear();
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest6";
    sessionInfo.abilityName_ = "UpdateAvoidArea";
    sessionInfo.isSystem_ = true;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    sessionInfo.isSystem_ = false;
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    sceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    sceneSession->SetSessionState(SessionState::STATE_INACTIVE);
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    sceneSession = nullptr;
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
}

/**
 * @tc.name: ProcessVirtualPixelRatioChange01
 * @tc.desc: ProcessVirtualPixelRatioChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, ProcessVirtualPixelRatioChange01, Function | SmallTest | Level3)
{
    DisplayId defaultDisplayId = 0;
    sptr<DisplayInfo> displayInfo = sptr<DisplayInfo>::MakeSptr();
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap;
    DisplayStateChangeType type = DisplayStateChangeType::BEFORE_SUSPEND;
    ASSERT_NE(nullptr, displayInfo);
    ASSERT_NE(nullptr, ssm_);
    ssm_->processVirtualPixelRatioChangeFunc_ = nullptr;
    displayInfo->SetVirtualPixelRatio(0.1f);
    displayInfo->SetDensityInCurResolution(0.1f);
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    ProcessVirtualPixelRatioChangeFunc func = [](float ratio, const OHOS::Rosen::Rect& rect) {};
    ssm_->SetVirtualPixelRatioChangeListener(func);
    ASSERT_NE(nullptr, ssm_->processVirtualPixelRatioChangeFunc_);
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    displayInfo->SetDensityInCurResolution(0.2f);
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    ssm_->processVirtualPixelRatioChangeFunc_ = nullptr;
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
}

/**
 * @tc.name: ProcessUpdateRotationChange
 * @tc.desc: ProcessUpdateRotationChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, ProcessUpdateRotationChange, Function | SmallTest | Level3)
{
    DisplayId defaultDisplayId = 0;
    sptr<DisplayInfo> displayInfo = nullptr;
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap;
    DisplayStateChangeType type = DisplayStateChangeType::BEFORE_SUSPEND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessUpdateRotationChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    displayInfo = sptr<DisplayInfo>::MakeSptr();
    ASSERT_NE(nullptr, displayInfo);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest6";
    sessionInfo.abilityName_ = "UpdateAvoidArea";
    sessionInfo.isSystem_ = true;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    sceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    ssm_->ProcessUpdateRotationChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);
    ssm_->ProcessUpdateRotationChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    sceneSession->SetSessionState(SessionState::STATE_INACTIVE);
    ssm_->ProcessUpdateRotationChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    WSRectF bounds = { 0, 0, 0, 0 };
    sceneSession->SetBounds(bounds);
    displayInfo->width_ = 0;
    displayInfo->height_ = 0;
    Rotation rotation = Rotation::ROTATION_0;
    sceneSession->SetRotation(rotation);
    displayInfo->SetRotation(rotation);
    ssm_->ProcessUpdateRotationChange(defaultDisplayId, displayInfo, displayInfoMap, type);
}
}
} // namespace Rosen
} // namespace OHOS