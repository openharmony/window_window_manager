/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "input_manager.h"
#include "input_transfer_station.h"
#include "mock_window_adapter.h"
#include "singleton_mocker.h"
#include "window_impl.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    std::string g_errLog;
    void MyLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char *tag,
        const char *msg)
    {
        g_errLog = msg;
    }
using WindowMocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
class InputTransferStationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    sptr<WindowImpl> window_;
    std::shared_ptr<InputEventListener> listener;
};
void InputTransferStationTest::SetUpTestCase() {}

void InputTransferStationTest::TearDownTestCase()
{
}

void InputTransferStationTest::SetUp()
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("inputwindow");
    window_ = new WindowImpl(option);
    window_->Create(INVALID_WINDOW_ID);
    listener = std::make_shared<InputEventListener>(InputEventListener());
}

void InputTransferStationTest::TearDown()
{
    window_->Destroy();
    window_ = nullptr;
}

namespace {
/**
 * @tc.name: AddInputWindow
 * @tc.desc: add input window in station.
 * @tc.type: FUNC
 * @tc.require: issueI5I5L4
 */
HWTEST_F(InputTransferStationTest, AddInputWindow, TestSize.Level0)
{
    InputTransferStation::GetInstance().isRegisteredMMI_ = true;
    InputTransferStation::GetInstance().AddInputWindow(window_);
    InputTransferStation::GetInstance().isRegisteredMMI_ = false;
    window_->GetWindowProperty()->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    InputTransferStation::GetInstance().destroyed_ = true;
    InputTransferStation::GetInstance().AddInputWindow(window_);
    InputTransferStation::GetInstance().destroyed_ = false;
    InputTransferStation::GetInstance().AddInputWindow(window_);
    InputTransferStation::GetInstance().inputListener_ = listener;
    InputTransferStation::GetInstance().AddInputWindow(window_);
    InputTransferStation::GetInstance().isGameControllerLoaded_ = true;
    InputTransferStation::GetInstance().AddInputWindow(window_);
    InputTransferStation::GetInstance().isGameControllerLoaded_ = false;
    InputTransferStation::GetInstance().AddInputWindow(window_);
    InputTransferStation::GetInstance().LoadTouchPredictor();
    InputTransferStation::GetInstance().LoadTouchPredictor();
    ASSERT_EQ(true, InputTransferStation::GetInstance().isGameControllerLoaded_);
}

/**
 * @tc.name: RemoveInputWindow
 * @tc.desc: remove input window in station.
 * @tc.type: FUNC
 * @tc.require: issueI5I5L4
 */
HWTEST_F(InputTransferStationTest, RemoveInputWindow, TestSize.Level0)
{
    InputTransferStation::GetInstance().destroyed_ = true;
    InputTransferStation::GetInstance().RemoveInputWindow(window_->GetWindowId());

    InputTransferStation::GetInstance().destroyed_ = false;
    sptr<WindowInputChannel> inputChannel = sptr<WindowInputChannel>::MakeSptr(window_);
    InputTransferStation::GetInstance().windowInputChannels_.insert({ window_->GetWindowId(), inputChannel });
    InputTransferStation::GetInstance().RemoveInputWindow(window_->GetWindowId());
    auto iter = InputTransferStation::GetInstance().windowInputChannels_.find(window_->GetWindowId());
    ASSERT_EQ(iter, InputTransferStation::GetInstance().windowInputChannels_.end());
}

/**
 * @tc.name: OnInputEvent
 * @tc.desc: OnInputEvent keyEvent
 * @tc.type: FUNC
 */
HWTEST_F(InputTransferStationTest, OnInputEvent1, TestSize.Level1)
{
    auto keyEvent = MMI::KeyEvent::Create();
    auto tempKeyEvent = keyEvent;
    keyEvent = nullptr;
    listener->OnInputEvent(keyEvent);
    keyEvent = tempKeyEvent;
    InputTransferStation::GetInstance().destroyed_ = true;
    auto channel = InputTransferStation::GetInstance().GetInputChannel(0);
    listener->OnInputEvent(keyEvent);
    ASSERT_EQ(channel, nullptr);
}

/**
 * @tc.name: OnInputEvent
 * @tc.desc: OnInputEvent axisEvent
 * @tc.type: FUNC
 */
HWTEST_F(InputTransferStationTest, OnInputEvent2, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    auto axisEvent = MMI::AxisEvent::Create();
    auto tempAxisEvent = axisEvent;
    axisEvent = nullptr;
    listener->OnInputEvent(axisEvent);
    EXPECT_TRUE(g_errLog.find("AxisEvent is nullptr") != std::string::npos);
    axisEvent = tempAxisEvent;
    listener->OnInputEvent(axisEvent);
    EXPECT_FALSE(g_errLog.find("Receive axisEvent, windowId: %{public}d") != std::string::npos);
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: OnInputEvent
 * @tc.desc: OnInputEvent pointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(InputTransferStationTest, OnInputEvent3, TestSize.Level1)
{
    auto pointerEvent = MMI::PointerEvent::Create();
    auto tempPointerEvent = pointerEvent;
    pointerEvent = nullptr;
    listener->OnInputEvent(pointerEvent);
    pointerEvent = tempPointerEvent;
    pointerEvent->SetPointerAction(static_cast<int32_t>(MMI::PointerEvent::POINTER_ACTION_DOWN));
    listener->OnInputEvent(pointerEvent);
    pointerEvent->SetPointerAction(static_cast<int32_t>(MMI::PointerEvent::POINTER_ACTION_MOVE));
    pointerEvent->SetAgentWindowId(0);
    listener->OnInputEvent(pointerEvent);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerAction(static_cast<int32_t>(MMI::PointerEvent::POINTER_ACTION_DOWN));
    pointerEvent->AddFlag(MMI::InputEvent::EVENT_FLAG_GESTURE_SUPPLEMENT);
    listener->OnInputEvent(pointerEvent);
    EXPECT_EQ(pointerEvent->GetSourceType(), MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetAgentWindowId(static_cast<uint32_t>(-1));
    InputTransferStation::GetInstance().destroyed_ = true;
    auto channel = InputTransferStation::GetInstance().GetInputChannel(0);
    listener->OnInputEvent(pointerEvent);
    ASSERT_EQ(channel, nullptr);
}

/**
 * @tc.name: GetInputChannel
 * @tc.desc: GetInputChannel
 * @tc.type: FUNC
 */
HWTEST_F(InputTransferStationTest, GetInputChannel, TestSize.Level0)
{
    InputTransferStation::GetInstance().destroyed_ = true;
    auto channel = InputTransferStation::GetInstance().GetInputChannel(0);
    ASSERT_EQ(channel, nullptr);
    InputTransferStation::GetInstance().destroyed_ = false;
    ASSERT_EQ(channel, nullptr);
    InputTransferStation::GetInstance().AddInputWindow(window_);
    InputTransferStation::GetInstance().GetInputChannel(0);
}
} // namespace
}
} // namespace Rosen
} // namespace OHOS
