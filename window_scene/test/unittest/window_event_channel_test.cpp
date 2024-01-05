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
using namespace OHOS::Accessibility;
using namespace std;

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowEventChannelTest" };
}
class WindowEventChannelTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    WSError TransferSearchElementInfo(bool isChannelNull);
    WSError TransferSearchElementInfosByText(bool isChannelNull);
    WSError TransferFindFocusedElementInfo(bool isChannelNull);
    WSError TransferFocusMoveSearch(bool isChannelNull);
    WSError TransferExecuteAction(bool isChannelNull);

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

WSError WindowEventChannelTest::TransferSearchElementInfo(bool isChannelNull)
{
    int32_t elementId = 0;
    int32_t mode = 0;
    int32_t baseParent = 0;
    list<AccessibilityElementInfo> infos;
    if (isChannelNull) {
        windowEventChannel_->sessionStage_ = nullptr;
    }
    return windowEventChannel_->TransferSearchElementInfo(elementId, mode, baseParent, infos);
}

WSError WindowEventChannelTest::TransferSearchElementInfosByText(bool isChannelNull)
{
    int32_t elementId = 0;
    string text;
    int32_t baseParent = 0;
    list<AccessibilityElementInfo> infos;
    if (isChannelNull) {
        windowEventChannel_->sessionStage_ = nullptr;
    }
    return windowEventChannel_->TransferSearchElementInfosByText(elementId, text, baseParent, infos);
}


WSError WindowEventChannelTest::TransferFindFocusedElementInfo(bool isChannelNull)
{
    int32_t elementId = 0;
    int32_t focusType = 0;
    int32_t baseParent = 0;
    AccessibilityElementInfo info;
    if (isChannelNull) {
        windowEventChannel_->sessionStage_ = nullptr;
    }
    return windowEventChannel_->TransferFindFocusedElementInfo(elementId, focusType, baseParent, info);
}

WSError WindowEventChannelTest::TransferFocusMoveSearch(bool isChannelNull)
{
    int32_t elementId = 0;
    int32_t direction = 0;
    int32_t baseParent = 0;
    AccessibilityElementInfo info;
    if (isChannelNull) {
        windowEventChannel_->sessionStage_ = nullptr;
    }
    return windowEventChannel_->TransferFocusMoveSearch(elementId, direction, baseParent, info);
}

WSError WindowEventChannelTest::TransferExecuteAction(bool isChannelNull)
{
    int32_t elementId = 0;
    map<string, string> actionArguments;
    int32_t action = 0;
    int32_t baseParent = 0;
    if (isChannelNull) {
        windowEventChannel_->sessionStage_ = nullptr;
    }
    return windowEventChannel_->TransferExecuteAction(elementId, actionArguments, action, baseParent);
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
    ASSERT_EQ(res, WSError::WS_OK);
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
    ASSERT_EQ(res, WSError::WS_OK);
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

/**
 * @tc.name: TransferSearchElementInfo01
 * @tc.desc: normal function TransferSearchElementInfo01
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelTest, TransferSearchElementInfo01, Function | SmallTest | Level2)
{
    WLOGFI("TransferSearchElementInfo01 begin");
    auto res = TransferSearchElementInfo(true);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
    WLOGFI("TransferSearchElementInfo01 end");
}

/**
 * @tc.name: TransferSearchElementInfo02
 * @tc.desc: normal function TransferSearchElementInfo02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelTest, TransferSearchElementInfo02, Function | SmallTest | Level2)
{
    WLOGFI("TransferSearchElementInfo02 begin");
    auto res = TransferSearchElementInfo(false);
    ASSERT_EQ(res, WSError::WS_OK);
    WLOGFI("TransferSearchElementInfo02 end");
}

/**
 * @tc.name: TransferSearchElementInfosByText01
 * @tc.desc: normal function TransferSearchElementInfosByText01
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelTest, TransferSearchElementInfosByText01, Function | SmallTest | Level2)
{
    WLOGFI("TransferSearchElementInfosByText01 begin");
    ASSERT_TRUE((windowEventChannel_ != nullptr));
    auto res = TransferSearchElementInfosByText(true);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
    WLOGFI("TransferSearchElementInfosByText01 end");
}

/**
 * @tc.name: TransferSearchElementInfosByText02
 * @tc.desc: normal function TransferSearchElementInfosByText02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelTest, TransferSearchElementInfosByText02, Function | SmallTest | Level2)
{
    WLOGFI("TransferSearchElementInfosByText02 begin");
    ASSERT_TRUE((windowEventChannel_ != nullptr));
    auto res = TransferSearchElementInfosByText(false);
    ASSERT_EQ(res, WSError::WS_OK);
    WLOGFI("TransferSearchElementInfosByText02 end");
}

/**
 * @tc.name: TransferFindFocusedElementInfo01
 * @tc.desc: normal function TransferFindFocusedElementInfo01
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelTest, TransferFindFocusedElementInfo01, Function | SmallTest | Level2)
{
    WLOGFI("TransferFindFocusedElementInfo01 begin");
    ASSERT_TRUE((windowEventChannel_ != nullptr));
    auto res = TransferFindFocusedElementInfo(true);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
    WLOGFI("TransferFindFocusedElementInfo01 end");
}

/**
 * @tc.name: TransferFindFocusedElementInfo02
 * @tc.desc: normal function TransferFindFocusedElementInfo02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelTest, TransferFindFocusedElementInfo02, Function | SmallTest | Level2)
{
    WLOGFI("TransferFindFocusedElementInfo02 begin");
    ASSERT_TRUE((windowEventChannel_ != nullptr));
    auto res = TransferFindFocusedElementInfo(false);
    ASSERT_EQ(res, WSError::WS_OK);
    WLOGFI("TransferFindFocusedElementInfo02 end");
}

/**
 * @tc.name: TransferFocusMoveSearch01
 * @tc.desc: normal function TransferFocusMoveSearch01
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelTest, TransferFocusMoveSearch01, Function | SmallTest | Level2)
{
    WLOGFI("TransferFocusMoveSearch01 begin");
    ASSERT_TRUE((windowEventChannel_ != nullptr));
    auto res = TransferFocusMoveSearch(true);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
    WLOGFI("TransferFocusMoveSearch01 end");
}

/**
 * @tc.name: TransferFocusMoveSearch02
 * @tc.desc: normal function TransferFocusMoveSearch02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelTest, TransferFocusMoveSearch02, Function | SmallTest | Level2)
{
    WLOGFI("TransferFocusMoveSearch02 begin");
    ASSERT_TRUE((windowEventChannel_ != nullptr));
    auto res = TransferFocusMoveSearch(false);
    ASSERT_EQ(res, WSError::WS_OK);
    WLOGFI("TransferFocusMoveSearch02 end");
}

/**
 * @tc.name: TransferExecuteAction01
 * @tc.desc: normal function TransferExecuteAction01
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelTest, TransferExecuteAction01, Function | SmallTest | Level2)
{
    WLOGFI("TransferExecuteAction01 begin");
    ASSERT_TRUE((windowEventChannel_ != nullptr));
    auto res = TransferExecuteAction(true);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
    WLOGFI("TransferExecuteAction01 end");
}

/**
 * @tc.name: TransferExecuteAction02
 * @tc.desc: normal function TransferExecuteAction02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelTest, TransferExecuteAction02, Function | SmallTest | Level2)
{
    WLOGFI("TransferExecuteAction02 begin");
    ASSERT_TRUE((windowEventChannel_ != nullptr));
    auto res = TransferExecuteAction(false);
    ASSERT_EQ(res, WSError::WS_OK);
    WLOGFI("TransferExecuteAction02 end");
}
}
}
