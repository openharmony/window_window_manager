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
#include <axis_event.h>
#include <key_event.h>
#include <pointer_event.h>
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
#include "session/container/include/window_event_channel.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class WindowEventChannelTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<ISessionStage> sessionStage = new SessionStageMocker();
    sptr<WindowEventChannel> windowEventChannel_ = new WindowEventChannelMocker(sessionStage);
};

void WindowEventChannelTest::SetUpTestCase()
{
}

void WindowEventChannelTest::TearDownTestCase()
{
}

void WindowEventChannelTest::SetUp()
{
}

void WindowEventChannelTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnDispatchEventProcessed
 * @tc.desc: normal function OnDispatchEventProcessed
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelTest, OnDispatchEventProcessed, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowEventChannelTest: OnDispatchEventProcessed";
    int32_t eventId = 12;
    int64_t actionTime = 8888;
    uint32_t res = 0;
    WindowEventChannel::OnDispatchEventProcessed(eventId, actionTime);
    ASSERT_EQ(0, res);
    GTEST_LOG_(INFO) << "WindowEventChannelTest: OnDispatchEventProcessed";
}

/**
 * @tc.name: TransferKeyEvent
 * @tc.desc: normal function TransferKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelTest, TransferKeyEvent, Function | SmallTest | Level2)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_BACK);
    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
    auto res = windowEventChannel_->TransferKeyEvent(keyEvent);
    ASSERT_EQ(res, WSError::WS_OK);
}

/**
 * @tc.name: TransferPointerEvent
 * @tc.desc: normal function TransferPointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelTest, TransferPointerEvent, Function | SmallTest | Level2)
{
    auto pointerEvent = MMI::PointerEvent::Create();
    auto res = windowEventChannel_->TransferPointerEvent(pointerEvent);
    ASSERT_EQ(res, WSError::WS_OK);
}

/**
 * @tc.name: TransferBackpressedEventForConsumed
 * @tc.desc: normal function TransferBackpressedEventForConsumed
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelTest, TransferBackpressedEventForConsumed, Function | SmallTest | Level2)
{
    bool isConsumed = false;
    auto res = windowEventChannel_->TransferBackpressedEventForConsumed(isConsumed);
    ASSERT_EQ(res, WSError::WS_OK);
    isConsumed = true;
    res = windowEventChannel_->TransferBackpressedEventForConsumed(isConsumed);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: TransferKeyEventForConsumed
 * @tc.desc: normal function TransferKeyEventForConsumed
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelTest, TransferKeyEventForConsumed, Function | SmallTest | Level2)
{
    bool isConsumed = false;
    auto res = windowEventChannel_->TransferBackpressedEventForConsumed(isConsumed);
    ASSERT_EQ(res, WSError::WS_OK);
    isConsumed = true;
    res = windowEventChannel_->TransferBackpressedEventForConsumed(isConsumed);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: TransferFocusActiveEvent
 * @tc.desc: normal function TransferFocusActiveEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelTest, TransferFocusActiveEvent, Function | SmallTest | Level2)
{
    bool isFocusActive = false;
    windowEventChannel_->sessionStage_ = nullptr;
    auto res = windowEventChannel_->TransferFocusActiveEvent(isFocusActive);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: PrintKeyEvent
 * @tc.desc: normal function PrintKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelTest, PrintKeyEvent, Function | SmallTest | Level2)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    ASSERT_TRUE((windowEventChannel_ != nullptr));
    windowEventChannel_->sessionStage_ = nullptr;
    windowEventChannel_->PrintKeyEvent(keyEvent);
}

/**
 * @tc.name: PrintPointerEvent
 * @tc.desc: normal function PrintPointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelTest, PrintPointerEvent, Function | SmallTest | Level2)
{
    auto pointerEvent = MMI::PointerEvent::Create();
    ASSERT_TRUE((windowEventChannel_ != nullptr));
    windowEventChannel_->PrintPointerEvent(pointerEvent);
}

/**
 * @tc.name: TransferFocusState
 * @tc.desc: normal function TransferFocusState
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelTest, TransferFocusState, Function | SmallTest | Level2)
{
    bool focusState = false;
    windowEventChannel_->sessionStage_ = nullptr;
    ASSERT_TRUE((windowEventChannel_ != nullptr));
    auto res = windowEventChannel_->TransferFocusState(focusState);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
}
}
}
