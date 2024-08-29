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
    int64_t uiExtensionIdLevel = 0;
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    WSError res = sProxy->TransferAccessibilityEvent(info, uiExtensionIdLevel);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    MockMessageParcel::ClearAllErrorFlag();
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
    int64_t uiExtensionIdLevel = 0;

    MockMessageParcel::SetWriteParcelableErrorFlag(true);
    WSError res = sProxy->TransferAccessibilityEvent(info, uiExtensionIdLevel);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);
    MockMessageParcel::ClearAllErrorFlag();
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
    int64_t uiExtensionIdLevel = 0;

    MockMessageParcel::SetWriteInt64ErrorFlag(true);
    WSError res = sProxy->TransferAccessibilityEvent(info, uiExtensionIdLevel);
    ASSERT_EQ(res, WSError::WS_ERROR_IPC_FAILED);

    MockMessageParcel::ClearAllErrorFlag();
    WLOGI("TransferAccessibilityEvent03 end");
}

/**
 * @tc.name: UpdateSessionPropertyByAction
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyMockTest, UpdateSessionPropertyByAction, Function | SmallTest | Level2)
{
    MockMessageParcel::ClearAllErrorFlag();
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    SessionProxy* sessionProxy = new(std::nothrow) SessionProxy(iRemoteObjectMocker);
    ASSERT_NE(sessionProxy, nullptr);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    ASSERT_EQ(WMError::WM_ERROR_IPC_FAILED, sessionProxy->UpdateSessionPropertyByAction(nullptr,
        WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON));
    MockMessageParcel::ClearAllErrorFlag();

    MockMessageParcel::SetWriteUint32ErrorFlag(true);
    ASSERT_EQ(WMError::WM_ERROR_IPC_FAILED, sessionProxy->UpdateSessionPropertyByAction(nullptr,
        WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON));
    MockMessageParcel::ClearAllErrorFlag();

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    MockMessageParcel::SetWriteBoolErrorFlag(true);
    ASSERT_EQ(WMError::WM_ERROR_IPC_FAILED, sessionProxy->UpdateSessionPropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON));
    MockMessageParcel::ClearAllErrorFlag();

    MockMessageParcel::SetWriteBoolErrorFlag(false);
    ASSERT_EQ(WMError::WM_ERROR_IPC_FAILED, sessionProxy->UpdateSessionPropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON));
    MockMessageParcel::ClearAllErrorFlag();

    MockMessageParcel::SetWriteBoolErrorFlag(true);
    ASSERT_EQ(WMError::WM_ERROR_IPC_FAILED, sessionProxy->UpdateSessionPropertyByAction(nullptr,
        WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON));
    MockMessageParcel::ClearAllErrorFlag();

    MockMessageParcel::SetWriteBoolErrorFlag(false);
    ASSERT_EQ(WMError::WM_ERROR_IPC_FAILED, sessionProxy->UpdateSessionPropertyByAction(nullptr,
        WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON));
    MockMessageParcel::ClearAllErrorFlag();
}
} // namespace
} // namespace Rosen
} // namespace OHOS
