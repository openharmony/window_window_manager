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
#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/session_manager_agent_controller.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "window_manager_agent.h"
#include "zidl/window_manager_agent_interface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionManagerAgentControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SessionManagerAgentControllerTest::SetUpTestCase()
{
}

void SessionManagerAgentControllerTest::TearDownTestCase()
{
}

void SessionManagerAgentControllerTest::SetUp()
{
}

void SessionManagerAgentControllerTest::TearDown()
{
}

/**
 * @tc.name: RegisterWindowManagerAgent
 * @tc.desc: SesionManagerAgentController rigister window manager agent
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerAgentControllerTest, RegisterWindowManagerAgent, Function | SmallTest | Level3)
{
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    int32_t pid = 65535;
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().RegisterWindowManagerAgent(
        windowManagerAgent, type, pid));
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().UnregisterWindowManagerAgent(
        windowManagerAgent, type, pid));
}

/**
 * @tc.name: UpdateCameraFloatWindowStatus
 * @tc.desc: UpdateCameraFloatWindowStatus Test
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerAgentControllerTest, UpdateCameraFloatWindowStatus, Function | SmallTest | Level3)
{
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    int32_t pid = 65535;
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().RegisterWindowManagerAgent(
        windowManagerAgent, type, pid));
    SessionManagerAgentController::GetInstance().UpdateCameraFloatWindowStatus(0, false);
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().UnregisterWindowManagerAgent(
        windowManagerAgent, type, pid));
}

/**
 * @tc.name: UpdateFocusChangeInfo
 * @tc.desc: UpdateFocusChangeInfo Test
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerAgentControllerTest, UpdateFocusChangeInfo, Function | SmallTest | Level3)
{
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    int32_t pid = 65535;
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().RegisterWindowManagerAgent(
        windowManagerAgent, type, pid));
    sptr<FocusChangeInfo> focusChangeInfo = new FocusChangeInfo();
    SessionManagerAgentController::GetInstance().UpdateFocusChangeInfo(focusChangeInfo, false);
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().UnregisterWindowManagerAgent(
        windowManagerAgent, type, pid));
}

/**
 * @tc.name: UpdateWindowModeTypeInfo
 * @tc.desc: UpdateWindowModeTypeInfo Test
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerAgentControllerTest, UpdateWindowModeTypeInfo, Function | SmallTest | Level3)
{
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_MODE;
    int32_t pid = 65535;
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().RegisterWindowManagerAgent(
        windowManagerAgent, type, pid));
    SessionManagerAgentController::GetInstance().UpdateWindowModeTypeInfo(WindowModeType::WINDOW_MODE_SPLIT);
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().UnregisterWindowManagerAgent(
        windowManagerAgent, type, pid));
}

/**
 * @tc.name: NotifyAccessibilityWindowInfo
 * @tc.desc: NotifyAccessibilityWindowInfo Test
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerAgentControllerTest, NotifyAccessibilityWindowInfo, Function | SmallTest | Level3)
{
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    int32_t pid = 65535;
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().RegisterWindowManagerAgent(
        windowManagerAgent, type, pid));
    std::vector<sptr<AccessibilityWindowInfo>> infos;
    SessionManagerAgentController::GetInstance().NotifyAccessibilityWindowInfo(infos,
        WindowUpdateType::WINDOW_UPDATE_ACTIVE);
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().UnregisterWindowManagerAgent(
        windowManagerAgent, type, pid));
}

/**
 * @tc.name: NotifyWaterMarkFlagChangedResult
 * @tc.desc: NotifyWaterMarkFlagChangedResult Test
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerAgentControllerTest, NotifyWaterMarkFlagChangedResult, Function | SmallTest | Level3)
{
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    int32_t pid = 65535;
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().RegisterWindowManagerAgent(
        windowManagerAgent, type, pid));
    SessionManagerAgentController::GetInstance().NotifyWaterMarkFlagChangedResult(false);
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().UnregisterWindowManagerAgent(
        windowManagerAgent, type, pid));
}

/**
 * @tc.name: UpdateWindowVisibilityInfo
 * @tc.desc: UpdateWindowVisibilityInfo Test
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerAgentControllerTest, UpdateWindowVisibilityInfo, Function | SmallTest | Level3)
{
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    int32_t pid = 65535;
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().RegisterWindowManagerAgent(
        windowManagerAgent, type, pid));
    std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
    SessionManagerAgentController::GetInstance().UpdateWindowVisibilityInfo(windowVisibilityInfos);
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().UnregisterWindowManagerAgent(
        windowManagerAgent, type, pid));
}

/**
 * @tc.name: UpdateVisibleWindowNum
 * @tc.desc: UpdateVisibleWindowNum Test
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerAgentControllerTest, UpdateVisibleWindowNum, Function | SmallTest | Level3)
{
    std::vector<VisibleWindowNumInfo> visibleWindowNumInfo;
    SessionManagerAgentController::GetInstance().UpdateVisibleWindowNum(visibleWindowNumInfo);
    int32_t pid = 65535;
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_VISIBLE_WINDOW_NUM;
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().RegisterWindowManagerAgent(
        windowManagerAgent, type, pid));
}

/**
 * @tc.name: UpdateWindowDrawingContentInfo
 * @tc.desc: UpdateWindowDrawingContentInfo Test
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerAgentControllerTest, UpdateWindowDrawingContentInfo, Function | SmallTest | Level3)
{
    std::vector<sptr<WindowDrawingContentInfo>> windowDrawingContentInfos;
    SessionManagerAgentController::GetInstance().UpdateWindowDrawingContentInfo(windowDrawingContentInfos);
    int32_t pid = 65535;
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().RegisterWindowManagerAgent(
        windowManagerAgent, type, pid));
}

/**
 * @tc.name: UpdateCameraWindowStatus
 * @tc.desc: UpdateCameraWindowStatus Test
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerAgentControllerTest, UpdateCameraWindowStatus, Function | SmallTest | Level3)
{
    sptr<IWindowManagerAgent> windowMangerAgent = new WindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_CAMERA_WINDOW;
    int32_t pid = 65535;
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().RegisterWindowManagerAgent(
        windowMangerAgent, type, pid));
    SessionManagerAgentController::GetInstance().UpdateCameraWindowStatus(0, false);
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().UnregisterWindowManagerAgent(
        windowMangerAgent, type, pid));
}

/**
 * @tc.name: NotifyWindowStyleChange
 * @tc.desc: NotifyWindowStyleChange Test
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerAgentControllerTest, NotifyWindowStyleChange, Function | SmallTest | Level3)
{
    int32_t pid = 65535;
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_STYLE;
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().RegisterWindowManagerAgent(
        windowManagerAgent, type, pid));
    SessionManagerAgentController::GetInstance().NotifyWindowStyleChange(Rosen::WindowStyleType::WINDOW_STYLE_DEFAULT);
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().UnregisterWindowManagerAgent(
        windowManagerAgent, type, pid));
}

} // namespace Rosen
} // namespace OHOS

