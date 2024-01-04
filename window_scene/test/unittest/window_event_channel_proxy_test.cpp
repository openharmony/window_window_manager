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

#include "session/container/include/zidl/window_event_channel_proxy.h"
#include "session/container/include/zidl/window_event_ipc_interface_code.h"
#include "iremote_object_mocker.h"
#include <gtest/gtest.h>
#include <axis_event.h>
#include <ipc_types.h>
#include <key_event.h>
#include <message_option.h>
#include <message_parcel.h>
#include <pointer_event.h>
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;
using namespace std;
namespace OHOS::Accessibility {
class AccessibilityElementInfo;
}
namespace OHOS {
namespace Rosen {
class WindowEventChannelProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<WindowEventChannelProxy> windowEventChannelProxy_ = new WindowEventChannelProxy(iRemoteObjectMocker);
};

void WindowEventChannelProxyTest::SetUpTestCase()
{
}

void WindowEventChannelProxyTest::TearDownTestCase()
{
}

void WindowEventChannelProxyTest::SetUp()
{
}

void WindowEventChannelProxyTest::TearDown()
{
}

namespace {
/**
 * @tc.name: TransferKeyEvent
 * @tc.desc: test function : TransferKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyTest, TransferKeyEvent, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    MMI::KeyEvent::KeyItem item = {};
    item.SetPressed(true);
    keyEvent->AddKeyItem(item);
    keyEvent->SetKeyCode(1);
    ASSERT_TRUE((windowEventChannelProxy_ != nullptr));
    WSError res = windowEventChannelProxy_->TransferKeyEvent(keyEvent);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: TransferPointerEvent
 * @tc.desc: test function : TransferPointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyTest, TransferPointerEvent, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    ASSERT_TRUE((windowEventChannelProxy_ != nullptr));
    WSError res = windowEventChannelProxy_->TransferPointerEvent(pointerEvent);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: TransferKeyEventForConsumed
 * @tc.desc: test function : TransferKeyEventForConsumed
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyTest, TransferKeyEventForConsumed, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    MMI::KeyEvent::KeyItem item = {};
    item.SetPressed(true);
    keyEvent->AddKeyItem(item);
    keyEvent->SetKeyCode(1);
    bool isConsumed = false;
    WSError res = windowEventChannelProxy_->TransferKeyEventForConsumed(keyEvent, isConsumed);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: TransferFocusActiveEvent
 * @tc.desc: test function : TransferFocusActiveEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyTest, TransferFocusActiveEvent, Function | SmallTest | Level1)
{
    bool isFocusActive = false;
    WSError res = windowEventChannelProxy_->TransferFocusActiveEvent(isFocusActive);
    ASSERT_EQ(WSError::WS_OK, res);
    isFocusActive = true;
    res = windowEventChannelProxy_->TransferFocusActiveEvent(isFocusActive);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: TransferFocusState
 * @tc.desc: test function : TransferFocusState
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyTest, TransferFocusState, Function | SmallTest | Level1)
{
    bool focusState = false;
    WSError res = windowEventChannelProxy_->TransferFocusActiveEvent(focusState);
    ASSERT_EQ(WSError::WS_OK, res);
    focusState = true;
    res = windowEventChannelProxy_->TransferFocusState(focusState);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: TransferFindFocusedElementInfo
 * @tc.desc: test function : TransferFindFocusedElementInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyTest, TransferFindFocusedElementInfo, Function | SmallTest | Level1)
{
    int64_t elementId = 0;
    int32_t focusType = 0;
    int64_t baseParent = 0;
    Accessibility::AccessibilityElementInfo info;

    WSError res = windowEventChannelProxy_->TransferFindFocusedElementInfo(elementId, focusType, baseParent, info);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: TransferFocusMoveSearch
 * @tc.desc: test function : TransferFocusMoveSearch
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyTest, TransferFocusMoveSearch, Function | SmallTest | Level1)
{
    int64_t elementId = 0;
    int32_t direction = 0;
    int64_t baseParent = 0;
    Accessibility::AccessibilityElementInfo info;

    WSError res = windowEventChannelProxy_->TransferFocusMoveSearch(elementId, direction, baseParent, info);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: TransferExecuteAction
 * @tc.desc: test function : TransferExecuteAction
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyTest, TransferExecuteAction, Function | SmallTest | Level1)
{
    int64_t elementId = 0;
    map<string, string> actionArguments;
    int32_t action = 0;
    int64_t baseParent = 0;

    WSError res = windowEventChannelProxy_->TransferExecuteAction(elementId, actionArguments, action, baseParent);
    ASSERT_EQ(WSError::WS_OK, res);
}
}
}
}