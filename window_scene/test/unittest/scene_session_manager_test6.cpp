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
class SceneSessionManagerTest6 public testing::Test{
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static bool gestureNavigationEnabled_;
    ststic ProcessGestureNavigationEnabledChangeFunc callbackFunc_;
    static sptr<SceneSessionManager> ssm_;
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

sptr<SceneSessionManager> SceneSessionManagerTest6::ssm_ = nullptr;

bool SceneSessionManagerTest6::gestureNavigationEnabled_ = true;
ProcessGestureNavigationEnabledChangeFunc SceneSessionManagerTest6::callbackFunc_ = [](bool enable){
    gestureNavigationEnabled_ = enanle;
};

void WindowChangedFuncTest(int32_t persistentId, WindowUpdateType type)
{

}

void ProcessStatusBarEnabledChangeFuncTest(bool enable)
{

}

void DumpRootSceneElementInfoFuncTest(const sttd::vector<std::string> &params, std::vector<std::string>& infos)
{

}

void SceneSessionManagerTest6::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerTest6::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    ssm_->sceneSessionMap_.clear();
}

namespace{
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
    
}
}
} // namespace Rosen
} // namespace OHOS






