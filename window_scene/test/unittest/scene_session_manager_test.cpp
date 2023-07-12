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
#include "interfaces/include/ws_common.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "window_manager_agent.h"
#include "session_manager.h"
#include "zidl/window_manager_agent_interface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr int WAIT_SLEEP_TIME = 1;
}
class SceneSessionManagerTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp() override;

    void TearDown() override;

    static bool gestureNavigationEnabled_;
    static ProcessGestureNavigationEnabledChangeFunc callbackFunc_;
};

bool SceneSessionManagerTest::gestureNavigationEnabled_ = true;
ProcessGestureNavigationEnabledChangeFunc SceneSessionManagerTest::callbackFunc_ = [](bool enable) {
    gestureNavigationEnabled_ = enable;
};

void SceneSessionManagerTest::SetUpTestCase()
{
}

void SceneSessionManagerTest::TearDownTestCase()
{
}

void SceneSessionManagerTest::SetUp()
{
}

void SceneSessionManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: SetBrightness
 * @tc.desc: ScreenSesionManager set session brightness
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, SetBrightness, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    WSError result = SceneSessionManager::GetInstance().SetBrightness(sceneSession, 0.5);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);
}

/**
 * @tc.name: SetGestureNavigaionEnabled
 * @tc.desc: SceneSessionManager set gesture navigation enabled
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, SetGestureNavigaionEnabled, Function | SmallTest | Level3)
{
    ASSERT_NE(callbackFunc_, nullptr);

    WMError result00 = SceneSessionManager::GetInstance().SetGestureNavigaionEnabled(true);
    ASSERT_EQ(result00, WMError::WM_DO_NOTHING);

    SceneSessionManager::GetInstance().SetGestureNavigationEnabledChangeListener(callbackFunc_);
    WMError result01 = SceneSessionManager::GetInstance().SetGestureNavigaionEnabled(true);
    ASSERT_EQ(result01, WMError::WM_OK);
    sleep(WAIT_SLEEP_TIME);
    ASSERT_EQ(gestureNavigationEnabled_, true);

    WMError result02 = SceneSessionManager::GetInstance().SetGestureNavigaionEnabled(false);
    ASSERT_EQ(result02, WMError::WM_OK);
    sleep(WAIT_SLEEP_TIME);
    ASSERT_EQ(gestureNavigationEnabled_, false);

    SceneSessionManager::GetInstance().SetGestureNavigationEnabledChangeListener(nullptr);
    WMError result03 = SceneSessionManager::GetInstance().SetGestureNavigaionEnabled(true);
    ASSERT_EQ(result03, WMError::WM_DO_NOTHING);
}

/**
 * @tc.name: RegisterWindowManagerAgent
 * @tc.desc: SceneSesionManager rigister window manager agent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, RegisterWindowManagerAgent, Function | SmallTest | Level3)
{
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;

    ASSERT_EQ(WMError::WM_OK, SceneSessionManager::GetInstance().RegisterWindowManagerAgent(type, windowManagerAgent));
    ASSERT_EQ(WMError::WM_OK, SceneSessionManager::GetInstance().UnregisterWindowManagerAgent(
        type, windowManagerAgent));
}

}
} // namespace Rosen
} // namespace OHOS

