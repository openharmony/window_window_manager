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
 * @tc.name: TransferAccessibilityChildTreeRegister
 * @tc.desc: test function : TransferAccessibilityChildTreeRegister
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferAccessibilityChildTreeRegister, Function | SmallTest | Level1)
{
    WLOGI("TransferAccessibilityChildTreeRegister begin");
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    uint32_t windowId = 0;
    int32_t treeId = 0;
    int64_t accessibilityId = 0;
    WSError res = windowEventChannelProxy_->TransferAccessibilityChildTreeRegister(windowId, treeId, accessibilityId);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    MockMessageParcel::ClearAllErrorFlag();
    WLOGI("TransferAccessibilityChildTreeRegister end");
}

/**
 * @tc.name: TransferAccessibilityChildTreeRegister
 * @tc.desc: test function : TransferAccessibilityChildTreeRegister
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferAccessibilityChildTreeRegister1, Function | SmallTest | Level1)
{
    WLOGI("TransferAccessibilityChildTreeRegister1 begin");
    uint32_t windowId = 0;
    int32_t treeId = -1;
    int64_t accessibilityId = 0;
    WSError res = windowEventChannelProxy_->TransferAccessibilityChildTreeRegister(windowId, treeId, accessibilityId);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    WLOGI("TransferAccessibilityChildTreeRegister1 end");
}

/**
 * @tc.name: TransferAccessibilityChildTreeRegister
 * @tc.desc: test function : TransferAccessibilityChildTreeRegister
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferAccessibilityChildTreeRegister2, Function | SmallTest | Level1)
{
    WLOGI("TransferAccessibilityChildTreeRegister2 begin");
    uint32_t windowId = 0;
    int32_t treeId = 0;
    int64_t accessibilityId = -1;
    WSError res = windowEventChannelProxy_->TransferAccessibilityChildTreeRegister(windowId, treeId, accessibilityId);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    WLOGI("TransferAccessibilityChildTreeRegister2 end");
}

/**
 * @tc.name: TransferAccessibilityChildTreeUnregister
 * @tc.desc: test function : TransferAccessibilityChildTreeUnregister
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferAccessibilityChildTreeUnregister, Function | SmallTest | Level1)
{
    WLOGI("TransferAccessibilityChildTreeUnregister begin");
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    WSError res = windowEventChannelProxy_->TransferAccessibilityChildTreeUnregister();
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    MockMessageParcel::ClearAllErrorFlag();
    WLOGI("TransferAccessibilityChildTreeUnregister end");
}

/**
 * @tc.name: TransferAccessibilityChildTreeUnregister
 * @tc.desc: test function : TransferAccessibilityChildTreeUnregister
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferAccessibilityChildTreeUnregister1, Function | SmallTest | Level1)
{
    WLOGI("TransferAccessibilityChildTreeUnregister1 begin");
    WSError res = windowEventChannelProxy_->TransferAccessibilityChildTreeUnregister();
    ASSERT_EQ(WSError::WS_OK, res);
    WLOGI("TransferAccessibilityChildTreeUnregister1 end");
}

/**
 * @tc.name: TransferAccessibilityDumpChildInfo
 * @tc.desc: test function : TransferAccessibilityDumpChildInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferAccessibilityDumpChildInfo, Function | SmallTest | Level1)
{
    WLOGI("TransferAccessibilityDumpChildInfo begin");
    std::vector<std::string> params;
    std::vector<std::string> info;
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    WSError res = windowEventChannelProxy_->TransferAccessibilityDumpChildInfo(params, info);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    MockMessageParcel::ClearAllErrorFlag();
    WLOGI("TransferAccessibilityDumpChildInfo end");
}

/**
 * @tc.name: TransferAccessibilityDumpChildInfo
 * @tc.desc: test function : TransferAccessibilityDumpChildInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferAccessibilityDumpChildInfo1, Function | SmallTest | Level1)
{
    WLOGI("TransferAccessibilityDumpChildInfo1 begin");
    std::vector<std::string> params;
    std::vector<std::string> info;
    WSError res = windowEventChannelProxy_->TransferAccessibilityDumpChildInfo(params, info);
    ASSERT_EQ(WSError::WS_OK, res);
    WLOGI("TransferAccessibilityDumpChildInfo1 end");
}

/**
 * @tc.name: TransferAccessibilityDumpChildInfo
 * @tc.desc: test function : TransferAccessibilityDumpChildInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelProxyMockTest, TransferAccessibilityDumpChildInfo2, Function | SmallTest | Level1)
{
    WLOGI("TransferAccessibilityDumpChildInfo2 begin");
    std::vector<std::string> params;
    params.push_back("test");
    std::vector<std::string> info;
    WSError res = windowEventChannelProxy_->TransferAccessibilityDumpChildInfo(params, info);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
    WLOGI("TransferAccessibilityDumpChildInfo2 end");
}
}
}
}