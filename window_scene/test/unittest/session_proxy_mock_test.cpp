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
#include "session_proxy.h"

#include <gtest/gtest.h>
#include "accessibility_event_info.h"
#include "iremote_object_mocker.h"
#include "mock_message_parcel.h"
#include "window_manager_hilog.h"
using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionProxyMockTest"};
}
class SessionProxyMockTest : public testing::Test {
  public:
    SessionProxyMockTest() {}
    ~SessionProxyMockTest() {}
};
namespace {
/**
 * @tc.name: TransferAccessibilityEvent01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyMockTest, TransferAccessibilityEvent01, Function | SmallTest | Level2)
{
    WLOGI("TransferAccessibilityEvent01 begin");
    MockMessageParcel::ClearAllErrorFlag();
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    Accessibility::AccessibilityEventInfo info;
    int32_t uiExtensionIdLevel = 0;
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    WSError res = sProxy->TransferAccessibilityEvent(info, uiExtensionIdLevel);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    WLOGI("TransferAccessibilityEvent01 end");
}

/**
 * @tc.name: TransferAccessibilityEvent02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyMockTest, TransferAccessibilityEvent02, Function | SmallTest | Level2)
{
    WLOGI("TransferAccessibilityEvent02 begin");
    MockMessageParcel::ClearAllErrorFlag();
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    Accessibility::AccessibilityEventInfo info;
    int32_t uiExtensionIdLevel = 0;

    MockMessageParcel::SetWriteParcelableErrorFlag(true);
    WSError res = sProxy->TransferAccessibilityEvent(info, uiExtensionIdLevel);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    WLOGI("TransferAccessibilityEvent02 end");
}

/**
 * @tc.name: TransferAccessibilityEvent02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyMockTest, TransferAccessibilityEvent03, Function | SmallTest | Level2)
{
    WLOGI("TransferAccessibilityEvent03 begin");
    MockMessageParcel::ClearAllErrorFlag();
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    Accessibility::AccessibilityEventInfo info;
    int32_t uiExtensionIdLevel = 0;

    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    WSError res = sProxy->TransferAccessibilityEvent(info, uiExtensionIdLevel);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    WLOGI("TransferAccessibilityEvent03 end");
}
} // namespace
} // namespace Rosen
} // namespace OHOS
