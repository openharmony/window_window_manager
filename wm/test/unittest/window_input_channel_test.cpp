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
#include "mock_window_adapter.h"
#include "singleton_mocker.h"
#include "window_impl.h"
#include "window_input_channel.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class MockWindow : public Window {
public:
    MockWindow() {};
    ~MockWindow() {};
    MOCK_METHOD(bool, IsAnco, (), (const, override));
    MOCK_METHOD(bool, OnPointDown, (int32_t eventId, int32_t posX, int32_t posY), (override));
};

using WindowMocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
class WindowInputChannelTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    sptr<WindowImpl> window_;

private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 300000;
};
void WindowInputChannelTest::SetUpTestCase() {}

void WindowInputChannelTest::TearDownTestCase() {}

void WindowInputChannelTest::SetUp()
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("window");
    window_ = new WindowImpl(option);
    window_->Create(INVALID_WINDOW_ID);
}

void WindowInputChannelTest::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    window_->Destroy();
    window_ = nullptr;
}

namespace {
/**
 * @tc.name: HandlePointerEvent
 * @tc.desc: consume pointer event when receive callback from input
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputChannelTest, HandlePointerEvent, TestSize.Level1)
{
    auto pointerEvent = MMI::PointerEvent::Create();
    sptr<WindowInputChannel> inputChannel = new WindowInputChannel(window_);
    ASSERT_NE(window_, nullptr);
    ASSERT_NE(pointerEvent, nullptr);
    ASSERT_NE(inputChannel, nullptr);
    window_->ConsumePointerEvent(pointerEvent);
    auto tempPointer = pointerEvent;
    pointerEvent = nullptr;
    inputChannel->HandlePointerEvent(pointerEvent);
    pointerEvent = tempPointer;
    inputChannel->window_ = nullptr;
    inputChannel->HandlePointerEvent(pointerEvent);
    inputChannel->window_ = window_;
    window_->GetWindowProperty()->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    pointerEvent->SetAgentWindowId(0);
    pointerEvent->SetTargetWindowId(1);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    inputChannel->HandlePointerEvent(pointerEvent);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    inputChannel->HandlePointerEvent(pointerEvent);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_PULL_MOVE);
    inputChannel->HandlePointerEvent(pointerEvent);
    pointerEvent->SetTargetWindowId(0);
    inputChannel->HandlePointerEvent(pointerEvent);
    window_->GetWindowProperty()->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    window_->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK));
    inputChannel->HandlePointerEvent(pointerEvent);
    window_->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_IS_MODAL));
    inputChannel->HandlePointerEvent(pointerEvent);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    MMI::PointerEvent::PointerItem item;
    pointerEvent->AddPointerItem(item);
    inputChannel->HandlePointerEvent(pointerEvent);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    inputChannel->HandlePointerEvent(pointerEvent);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_PULL_MOVE);
    inputChannel->HandlePointerEvent(pointerEvent);

    window_->GetWindowProperty()->SetWindowRect({ 0, 0, 8, 8 });
    inputChannel->HandlePointerEvent(pointerEvent);
    inputChannel->Destroy();
}

/**
 * @tc.name: HandlePointEvent
 * @tc.desc: consume key event when receive callback from input
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputChannelTest, HandlePointEvent01, TestSize.Level1)
{
    auto pointerEvent = MMI::PointerEvent::Create();
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("window");
    auto window = sptr<MockWindow>::MakeSptr();
    sptr<WindowInputChannel> inputChannel = sptr<WindowInputChannel>::MakeSptr(window);

    EXPECT_CALL(*(window), IsAnco()).Time(1).WillOnce(Return(true));
    EXPECT_CALL(*(window), OnPointDown(_, _, _)).Time(0);
    inputChannel->HandlePointEvent(pointerEvent);
    testing::Mock::VerifyAndClearExpectations(window);

    MMI::PointerEvent::PointerItem item;
    inputChannel->HandlePointEvent(pointerEvent);

    pointerEvent->AddPointerItem(item);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    EXPECT_CALL(*(window), IsAnco()).Time(1).WillOnce(Return(true));
    EXPECT_CALL(*(window), OnPointDown(_, _, _)).Time(1);
    inputChannel->HandlePointEvent(pointerEvent);
    testing::Mock::VerifyAndClearExpectations(window);
}

/**
 * @tc.name: HandleKeyEvent
 * @tc.desc: consume key event when receive callback from input
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputChannelTest, HandleKeyEvent, TestSize.Level1)
{
    auto keyEvent = MMI::KeyEvent::Create();
    sptr<WindowInputChannel> inputChannel = new WindowInputChannel(window_);
    ASSERT_NE(window_, nullptr);
    ASSERT_NE(keyEvent, nullptr);
    ASSERT_NE(inputChannel, nullptr);
    window_->ConsumeKeyEvent(keyEvent);
    auto tempKeyEvent = keyEvent;
    keyEvent = nullptr;
    inputChannel->HandleKeyEvent(keyEvent);
    keyEvent = tempKeyEvent;
    window_->GetWindowProperty()->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    keyEvent->SetAgentWindowId(0);
    keyEvent->SetTargetWindowId(1);
    inputChannel->HandleKeyEvent(keyEvent);
    keyEvent->SetTargetWindowId(0);
    inputChannel->HandleKeyEvent(keyEvent);
    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_BACK);
    inputChannel->HandleKeyEvent(keyEvent);
    window_->GetWindowProperty()->SetWindowType(WindowType::WINDOW_TYPE_GLOBAL_SEARCH);
    inputChannel->HandleKeyEvent(keyEvent);
    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_FN);
    inputChannel->HandleKeyEvent(keyEvent);
}

/**
 * @tc.name: DispatchKeyEventCallback
 * @tc.desc: DispatchKeyEventCallback
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputChannelTest, DispatchKeyEventCallback, TestSize.Level1)
{
    sptr<WindowInputChannel> inputChannel = new WindowInputChannel(window_);
    auto keyEvent = MMI::KeyEvent::Create();
    ASSERT_NE(inputChannel, nullptr);
    ASSERT_NE(keyEvent, nullptr);
    auto tempKeyEvent = keyEvent;
    keyEvent = nullptr;
    inputChannel->DispatchKeyEventCallback(keyEvent, false);
    keyEvent = tempKeyEvent;
    inputChannel->DispatchKeyEventCallback(keyEvent, true);
    inputChannel->DispatchKeyEventCallback(keyEvent, false);
    inputChannel->window_ = nullptr;
    inputChannel->DispatchKeyEventCallback(keyEvent, false);
    inputChannel->window_ = window_;
    inputChannel->DispatchKeyEventCallback(keyEvent, false);
}

/**
 * @tc.name: GetWindowRect
 * @tc.desc: GetWindowRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputChannelTest, GetWindowRect, TestSize.Level1)
{
    sptr<WindowInputChannel> inputChannel = new WindowInputChannel(window_);
    ASSERT_NE(inputChannel, nullptr);
    inputChannel->window_ = nullptr;
    auto rect = inputChannel->GetWindowRect();
    Rect tempTect;
    ASSERT_EQ(tempTect, rect);
    inputChannel->window_ = window_;
    auto rect2 = inputChannel->GetWindowRect();
    ASSERT_EQ(tempTect, rect2);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
