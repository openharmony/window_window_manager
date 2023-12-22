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
#include "iremote_object_mocker.h"
#include <gtest/gtest.h>
#include <message_option.h>
#include <message_parcel.h>
#include "mock_message_parcel.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;
using namespace std;
namespace OHOS::Accessibility {
class AccessibilityElementInfo;
}
namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowEventChannelProxyMockTest"};
}
class WindowEventChannelProxyMockTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<WindowEventChannelProxy> windowEventChannelProxy_ = new WindowEventChannelProxy(iRemoteObjectMocker);
};

void WindowEventChannelProxyMockTest::SetUpTestCase()
{
}

void WindowEventChannelProxyMockTest::TearDownTestCase()
{
}

void WindowEventChannelProxyMockTest::SetUp()
{
}

void WindowEventChannelProxyMockTest::TearDown()
{
}

namespace {
/**
 * @tc.name: TransferSearchElementInfo
 * @tc.desc: test function : TransferSearchElementInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferSearchElementInfo, Function | SmallTest | Level1)
{
    WLOGI("TransferSearchElementInfo begin");
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    int32_t elementId = 0;
    int32_t mode = 0;
    int32_t baseParent = 0;
    std::list<Accessibility::AccessibilityElementInfo> infos;
    
    WSError res = windowEventChannelProxy_->TransferSearchElementInfo(elementId, mode, baseParent, infos);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    MockMessageParcel::ClearAllErrorFlag();
    WLOGI("TransferSearchElementInfo end");
}
/**
 * @tc.name: TransferSearchElementInfo
 * @tc.desc: test function : TransferSearchElementInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferSearchElementInfo1, Function | SmallTest | Level1)
{
    WLOGI("TransferSearchElementInfo1 begin");
    int32_t elementId = -1;
    int32_t mode = 0;
    int32_t baseParent = 0;
    std::list<Accessibility::AccessibilityElementInfo> infos;

    WSError res = windowEventChannelProxy_->TransferSearchElementInfo(elementId, mode, baseParent, infos);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    WLOGI("TransferSearchElementInfo1 end");
}

/**
 * @tc.name: TransferSearchElementInfo2
 * @tc.desc: test function : TransferSearchElementInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferSearchElementInfo2, Function | SmallTest | Level1)
{
    WLOGI("TransferSearchElementInfo2 begin");
    int32_t elementId = 0;
    int32_t mode = -1;
    int32_t baseParent = 0;
    std::list<Accessibility::AccessibilityElementInfo> infos;

    WSError res = windowEventChannelProxy_->TransferSearchElementInfo(elementId, mode, baseParent, infos);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    WLOGI("TransferSearchElementInfo2 end");
}

/**
 * @tc.name: TransferSearchElementInfo3
 * @tc.desc: test function : TransferSearchElementInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferSearchElementInfo3, Function | SmallTest | Level1)
{
    WLOGI("TransferSearchElementInfo3 begin");
    int32_t elementId = 0;
    int32_t mode = 0;
    int32_t baseParent = -1;
    std::list<Accessibility::AccessibilityElementInfo> infos;

    WSError res = windowEventChannelProxy_->TransferSearchElementInfo(elementId, mode, baseParent, infos);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    WLOGI("TransferSearchElementInfo3 end");
}

/**
 * @tc.name: TransferSearchElementInfo4
 * @tc.desc: test function : TransferSearchElementInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferSearchElementInfo4, Function | SmallTest | Level1)
{
    WLOGI("TransferSearchElementInfo4 begin");
    MockMessageParcel::SetReadInt32ErrorFlag(true);
    int32_t elementId = 0;
    int32_t mode = 0;
    int32_t baseParent = 0;
    std::list<Accessibility::AccessibilityElementInfo> infos;

    WSError res = windowEventChannelProxy_->TransferSearchElementInfo(elementId, mode, baseParent, infos);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    MockMessageParcel::ClearAllErrorFlag();
    WLOGI("TransferSearchElementInfo4 end");
}

/**
 * @tc.name: TransferSearchElementInfosByText
 * @tc.desc: test function : TransferSearchElementInfosByText
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferSearchElementInfosByText, Function | SmallTest | Level1)
{
    WLOGI("TransferSearchElementInfosByText begin");
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    int32_t elementId = 0;
    string text;
    int32_t baseParent = 0;
    std::list<Accessibility::AccessibilityElementInfo> infos;

    WSError res = windowEventChannelProxy_->TransferSearchElementInfosByText(elementId, text, baseParent, infos);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    MockMessageParcel::ClearAllErrorFlag();
    WLOGI("TransferSearchElementInfosByText end");
}

/**
 * @tc.name: TransferSearchElementInfosByText1
 * @tc.desc: test function : TransferSearchElementInfosByText
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferSearchElementInfosByText1, Function | SmallTest | Level1)
{
    WLOGI("TransferSearchElementInfosByText1 begin");
    int32_t elementId = -1;
    string text;
    int32_t baseParent = 0;
    std::list<Accessibility::AccessibilityElementInfo> infos;

    WSError res = windowEventChannelProxy_->TransferSearchElementInfosByText(elementId, text, baseParent, infos);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    WLOGI("TransferSearchElementInfosByText1 end");
}

/**
 * @tc.name: TransferSearchElementInfosByText2
 * @tc.desc: test function : TransferSearchElementInfosByText
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferSearchElementInfosByText2, Function | SmallTest | Level1)
{
    WLOGI("TransferSearchElementInfosByText2 begin");
    int32_t elementId = 0;
    string text = "error";
    int32_t baseParent = 0;
    std::list<Accessibility::AccessibilityElementInfo> infos;

    WSError res = windowEventChannelProxy_->TransferSearchElementInfosByText(elementId, text, baseParent, infos);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    WLOGI("TransferSearchElementInfosByText2 end");
}

/**
 * @tc.name: TransferSearchElementInfosByText3
 * @tc.desc: test function : TransferSearchElementInfosByText
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferSearchElementInfosByText3, Function | SmallTest | Level1)
{
    WLOGI("TransferSearchElementInfosByText3 begin");
    int32_t elementId = 0;
    string text;
    int32_t baseParent = -1;
    std::list<Accessibility::AccessibilityElementInfo> infos;

    WSError res = windowEventChannelProxy_->TransferSearchElementInfosByText(elementId, text, baseParent, infos);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    WLOGI("TransferSearchElementInfosByText3 end");
}

/**
 * @tc.name: TransferSearchElementInfosByText4
 * @tc.desc: test function : TransferSearchElementInfosByText
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferSearchElementInfosByText4, Function | SmallTest | Level1)
{
    WLOGI("TransferSearchElementInfosByText4 begin");
    MockMessageParcel::SetReadInt32ErrorFlag(true);
    int32_t elementId = 0;
    string text;
    int32_t baseParent = 0;
    std::list<Accessibility::AccessibilityElementInfo> infos;

    WSError res = windowEventChannelProxy_->TransferSearchElementInfosByText(elementId, text, baseParent, infos);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    MockMessageParcel::ClearAllErrorFlag();
    WLOGI("TransferSearchElementInfosByText4 end");
}

/**
 * @tc.name: TransferFindFocusedElementInfo
 * @tc.desc: test function : TransferFindFocusedElementInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferFindFocusedElementInfo, Function | SmallTest | Level1)
{
    WLOGI("TransferFindFocusedElementInfo begin");
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    int32_t elementId = 0;
    int32_t focusType = 0;
    int32_t baseParent = 0;
    Accessibility::AccessibilityElementInfo info;

    WSError res = windowEventChannelProxy_->TransferFindFocusedElementInfo(elementId, focusType, baseParent, info);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    MockMessageParcel::ClearAllErrorFlag();
    WLOGI("TransferFindFocusedElementInfo end");
}

/**
 * @tc.name: TransferFindFocusedElementInfo1
 * @tc.desc: test function : TransferFindFocusedElementInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferFindFocusedElementInfo1, Function | SmallTest | Level1)
{
    WLOGI("TransferFindFocusedElementInfo1 begin");
    int32_t elementId = -1;
    int32_t focusType = 0;
    int32_t baseParent = 0;
    Accessibility::AccessibilityElementInfo info;

    WSError res = windowEventChannelProxy_->TransferFindFocusedElementInfo(elementId, focusType, baseParent, info);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    WLOGI("TransferFindFocusedElementInfo1 end");
}

/**
 * @tc.name: TransferFindFocusedElementInfo2
 * @tc.desc: test function : TransferFindFocusedElementInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferFindFocusedElementInfo2, Function | SmallTest | Level1)
{
    WLOGI("TransferFindFocusedElementInfo2 begin");
    int32_t elementId = 0;
    int32_t focusType = -1;
    int32_t baseParent = 0;
    Accessibility::AccessibilityElementInfo info;

    WSError res = windowEventChannelProxy_->TransferFindFocusedElementInfo(elementId, focusType, baseParent, info);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    WLOGI("TransferFindFocusedElementInfo2 end");
}

/**
 * @tc.name: TransferFindFocusedElementInfo3
 * @tc.desc: test function : TransferFindFocusedElementInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferFindFocusedElementInfo3, Function | SmallTest | Level1)
{
    WLOGI("TransferFindFocusedElementInfo3 begin");
    int32_t elementId = 0;
    int32_t focusType = 0;
    int32_t baseParent = -1;
    Accessibility::AccessibilityElementInfo info;

    WSError res = windowEventChannelProxy_->TransferFindFocusedElementInfo(elementId, focusType, baseParent, info);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    WLOGI("TransferFindFocusedElementInfo3 end");
}

/**
 * @tc.name: TransferFocusMoveSearch
 * @tc.desc: test function : TransferFocusMoveSearch
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferFocusMoveSearch, Function | SmallTest | Level1)
{
    WLOGI("TransferFocusMoveSearch begin");
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    int32_t elementId = 0;
    int32_t direction = 0;
    int32_t baseParent = 0;
    Accessibility::AccessibilityElementInfo info;
    
    WSError res = windowEventChannelProxy_->TransferFocusMoveSearch(elementId, direction, baseParent, info);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    MockMessageParcel::ClearAllErrorFlag();
    WLOGI("TransferFocusMoveSearch end");
}

/**
 * @tc.name: TransferFocusMoveSearch1
 * @tc.desc: test function : TransferFocusMoveSearch
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferFocusMoveSearch1, Function | SmallTest | Level1)
{
    WLOGI("TransferFocusMoveSearch1 begin");
    int32_t elementId = -1;
    int32_t direction = 0;
    int32_t baseParent = 0;
    Accessibility::AccessibilityElementInfo info;
    
    WSError res = windowEventChannelProxy_->TransferFocusMoveSearch(elementId, direction, baseParent, info);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    WLOGI("TransferFocusMoveSearch1 end");
}

/**
 * @tc.name: TransferFocusMoveSearch2
 * @tc.desc: test function : TransferFocusMoveSearch
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferFocusMoveSearch2, Function | SmallTest | Level1)
{
    WLOGI("TransferFocusMoveSearch2 begin");
    int32_t elementId = 0;
    int32_t direction = -1;
    int32_t baseParent = 0;
    Accessibility::AccessibilityElementInfo info;
    
    WSError res = windowEventChannelProxy_->TransferFocusMoveSearch(elementId, direction, baseParent, info);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    WLOGI("TransferFocusMoveSearch2 end");
}

/**
 * @tc.name: TransferFocusMoveSearch3
 * @tc.desc: test function : TransferFocusMoveSearch
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferFocusMoveSearch3, Function | SmallTest | Level1)
{
    WLOGI("TransferFocusMoveSearch3 begin");
    int32_t elementId = 0;
    int32_t direction = 0;
    int32_t baseParent = -1;
    Accessibility::AccessibilityElementInfo info;
    
    WSError res = windowEventChannelProxy_->TransferFocusMoveSearch(elementId, direction, baseParent, info);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    WLOGI("TransferFocusMoveSearch3 end");
}

/**
 * @tc.name: TransferExecuteAction
 * @tc.desc: test function : TransferExecuteAction
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferExecuteAction, Function | SmallTest | Level1)
{
    WLOGI("TransferExecuteAction begin");
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    int32_t elementId = 0;
    map<string, string> actionArguments;
    int32_t action = 0;
    int32_t baseParent = 0;

    WSError res = windowEventChannelProxy_->TransferExecuteAction(elementId, actionArguments, action, baseParent);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    MockMessageParcel::ClearAllErrorFlag();
    WLOGI("TransferExecuteAction end");
}

/**
 * @tc.name: TransferExecuteAction1
 * @tc.desc: test function : TransferExecuteAction
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferExecuteAction1, Function | SmallTest | Level1)
{
    WLOGI("TransferExecuteAction1 begin");
    int32_t elementId = -1;
    map<string, string> actionArguments;
    int32_t action = 0;
    int32_t baseParent = 0;

    WSError res = windowEventChannelProxy_->TransferExecuteAction(elementId, actionArguments, action, baseParent);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    WLOGI("TransferExecuteAction1 end");
}

/**
 * @tc.name: TransferExecuteAction2
 * @tc.desc: test function : TransferExecuteAction
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferExecuteAction2, Function | SmallTest | Level1)
{
    WLOGI("TransferExecuteAction2 begin");
    int32_t elementId = 0;
    map<string, string> actionArguments;
    actionArguments.insert(pair<string, string>("test", "test"));
    int32_t action = 0;
    int32_t baseParent = 0;

    WSError res = windowEventChannelProxy_->TransferExecuteAction(elementId, actionArguments, action, baseParent);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    WLOGI("TransferExecuteAction2 end");
}

/**
 * @tc.name: TransferExecuteAction3
 * @tc.desc: test function : TransferExecuteAction
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferExecuteAction3, Function | SmallTest | Level1)
{
    WLOGI("TransferExecuteAction3 begin");
    int32_t elementId = 0;
    map<string, string> actionArguments;
    int32_t action = -1;
    int32_t baseParent = 0;

    WSError res = windowEventChannelProxy_->TransferExecuteAction(elementId, actionArguments, action, baseParent);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    WLOGI("TransferExecuteAction3 end");
}

/**
 * @tc.name: TransferExecuteAction4
 * @tc.desc: test function : TransferExecuteAction
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferExecuteAction4, Function | SmallTest | Level1)
{
    WLOGI("TransferExecuteAction4 begin");
    int32_t elementId = 0;
    map<string, string> actionArguments;
    int32_t action = 0;
    int32_t baseParent = -1;

    WSError res = windowEventChannelProxy_->TransferExecuteAction(elementId, actionArguments, action, baseParent);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    WLOGI("TransferExecuteAction4 end");
}
}
}
}