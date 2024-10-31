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
#include "window_impl.h"
#include "mock_window_adapter.h"
#include "singleton_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using WindowMocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
class InputTransferStationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    sptr<WindowImpl> window_;
    std::shared_ptr<MMI::IInputEventConsumer> listener;
};
void InputTransferStationTest::SetUpTestCase()
{
}

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
HWTEST_F(InputTransferStationTest, AddInputWindow, Function | SmallTest | Level2)
{
    if (!window_) {
        GTEST_LOG_(INFO) << "Null Pointer";
        return;
    }
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
}

/**
 * @tc.name: RemoveInputWindow
 * @tc.desc: remove input window in station.
 * @tc.type: FUNC
 * @tc.require: issueI5I5L4
 */
HWTEST_F(InputTransferStationTest, RemoveInputWindow, Function | SmallTest | Level2)
{
    InputTransferStation::GetInstance().destroyed_ = true;
    InputTransferStation::GetInstance().RemoveInputWindow(window_->GetWindowId());
    InputTransferStation::GetInstance().destroyed_ = false;
    InputTransferStation::GetInstance().RemoveInputWindow(window_->GetWindowId());
    InputTransferStation::GetInstance().AddInputWindow(window_);
    InputTransferStation::GetInstance().RemoveInputWindow(window_->GetWindowId());
}

/**
 * @tc.name: OnInputEvent
 * @tc.desc: OnInputEvent keyEvent
 * @tc.type: FUNC
 */
HWTEST_F(InputTransferStationTest, OnInputEvent1, Function | SmallTest | Level2)
{
    auto keyEvent = MMI::KeyEvent::Create();
    if (!keyEvent || !listener) {
        GTEST_LOG_(INFO) << "Null Pointer";
        return;
    }
    auto tempKeyEvent = keyEvent;
    keyEvent = nullptr;
    listener->OnInputEvent(keyEvent);
    keyEvent = tempKeyEvent;
    auto channel = InputTransferStation::GetInstance().GetInputChannel(1);
    listener->OnInputEvent(keyEvent);
    EXPECT_NE(nullptr, channel);
}

/**
 * @tc.name: OnInputEvent
 * @tc.desc: OnInputEvent axisEvent
 * @tc.type: FUNC
 */
HWTEST_F(InputTransferStationTest, OnInputEvent2, Function | SmallTest | Level2)
{
    auto axisEvent = MMI::AxisEvent::Create();
    if (!axisEvent || !listener) {
        GTEST_LOG_(INFO) << "Null Pointer";
        return;
    }
    auto tempAxisEvent = axisEvent;
    axisEvent = nullptr;
    listener->OnInputEvent(axisEvent);
    axisEvent = tempAxisEvent;
    listener->OnInputEvent(axisEvent);
}

/**
 * @tc.name: OnInputEvent
 * @tc.desc: OnInputEvent pointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(InputTransferStationTest, OnInputEvent3, Function | SmallTest | Level2)
{
    auto pointerEvent = MMI::PointerEvent::Create();
    if (!pointerEvent || !listener) {
        GTEST_LOG_(INFO) << "Null Pointer";
        return;
    }
    auto tempPointerEvent = pointerEvent;
    pointerEvent = nullptr;
    listener->OnInputEvent(pointerEvent);
    pointerEvent = tempPointerEvent;
    pointerEvent->SetPointerAction(static_cast<int32_t>(MMI::PointerEvent::POINTER_ACTION_DOWN));
    listener->OnInputEvent(pointerEvent);
    pointerEvent->SetPointerAction(static_cast<int32_t>(MMI::PointerEvent::POINTER_ACTION_MOVE));
    pointerEvent->SetAgentWindowId(0);
    listener->OnInputEvent(pointerEvent);
    pointerEvent->SetAgentWindowId(static_cast<uint32_t>(-1));
    auto channel = InputTransferStation::GetInstance().GetInputChannel(1);
    listener->OnInputEvent(pointerEvent);
    EXPECT_NE(nullptr, channel);
}

/**
 * @tc.name: GetInputChannel
 * @tc.desc: GetInputChannel
 * @tc.type: FUNC
 */
HWTEST_F(InputTransferStationTest, GetInputChannel, Function | SmallTest | Level2)
{
    InputTransferStation::GetInstance().destroyed_ = true;
    auto channel = InputTransferStation::GetInstance().GetInputChannel(0);
    ASSERT_EQ(channel, nullptr);
    InputTransferStation::GetInstance().destroyed_ = false;
    ASSERT_EQ(channel, nullptr);
    InputTransferStation::GetInstance().AddInputWindow(window_);
    InputTransferStation::GetInstance().GetInputChannel(0);
}
}
} // namespace Rosen
} // namespace OHOS
