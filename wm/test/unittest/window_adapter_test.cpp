/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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
#include "mock_RSIWindowAnimationController.h"

#include "remote_animation.h"
#include "starting_window.h"
#include "window_transition_info.h"
#include "window_property.h"
#include "window_agent.h"
#include "window_adapter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowAdapterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WindowAdapterTest::SetUpTestCase()
{
}

void WindowAdapterTest::TearDownTestCase()
{
}

void WindowAdapterTest::SetUp()
{
}

void WindowAdapterTest::TearDown()
{
}

namespace {
/**
 * @tc.name: AddWindow
 * @tc.desc: WindowAdapter/AddWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, AddWindow, Function | SmallTest | Level2)
{
    sptr<WindowProperty> windowProperty = nullptr;
    WindowAdapter windowAdapter;

    windowAdapter.AddWindow(windowProperty);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: RemoveWindow
 * @tc.desc: WindowAdapter/RemoveWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, RemoveWindow, Function | SmallTest | Level2)
{
    uint32_t windowId = 0;
    bool isFromInnerkits = false;
    WindowAdapter windowAdapter;

    windowAdapter.RemoveWindow(windowId, isFromInnerkits);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: RequestFocus
 * @tc.desc: WindowAdapter/RequestFocus
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, RequestFocus, Function | SmallTest | Level2)
{
    uint32_t windowId = 0;
    WindowAdapter windowAdapter;

    windowAdapter.RequestFocus(windowId);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: CheckWindowId
 * @tc.desc: WindowAdapter/CheckWindowId
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, CheckWindowId, Function | SmallTest | Level2)
{
    int32_t windowId = 0;
    int32_t pid = 0;
    WindowAdapter windowAdapter;

    windowAdapter.CheckWindowId(windowId, pid);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: SetWindowAnimationController
 * @tc.desc: WindowAdapter/SetWindowAnimationController
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, SetWindowAnimationController, Function | SmallTest | Level2)
{
    sptr<RSIWindowAnimationController> controller = nullptr;
    WindowAdapter windowAdapter;

    windowAdapter.SetWindowAnimationController(controller);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: GetAvoidAreaByType
 * @tc.desc: WindowAdapter/GetAvoidAreaByType
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, GetAvoidAreaByType, Function | SmallTest | Level2)
{
    uint32_t windowId = 0;
    AvoidAreaType type = AvoidAreaType::TYPE_CUTOUT;
    AvoidArea avoidArea;
    WindowAdapter windowAdapter;

    windowAdapter.GetAvoidAreaByType(windowId, type, avoidArea);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: NotifyServerReadyToMoveOrDrag
 * @tc.desc: WindowAdapter/NotifyServerReadyToMoveOrDrag
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, NotifyServerReadyToMoveOrDrag, Function | SmallTest | Level2)
{
    uint32_t windowId = 0;
    sptr<WindowProperty> windowProperty = nullptr;
    sptr<MoveDragProperty> moveDragProperty = nullptr;
    WindowAdapter windowAdapter;

    windowAdapter.NotifyServerReadyToMoveOrDrag(windowId, windowProperty, moveDragProperty);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: ProcessPointDown
 * @tc.desc: WindowAdapter/ProcessPointDown
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, ProcessPointDown, Function | SmallTest | Level2)
{
    uint32_t windowId = 0;
    bool isPointDown = false;
    WindowAdapter windowAdapter;

    windowAdapter.ProcessPointDown(windowId, isPointDown);
    windowAdapter.ProcessPointUp(windowId);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: ToggleShownStateForAllAppWindows
 * @tc.desc: WindowAdapter/ToggleShownStateForAllAppWindows
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, ToggleShownStateForAllAppWindows, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;

    windowAdapter.ToggleShownStateForAllAppWindows();
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: InitWMSProxy
 * @tc.desc: WindowAdapter/InitWMSProxy
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, InitWMSProxy, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}
}
}
}