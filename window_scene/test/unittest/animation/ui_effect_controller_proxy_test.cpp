/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "mock/mock_message_parcel.h"
#include "ui_effect_controller_client.h"
#include "ui_effect_controller.h"
#include "ui_effect_controller_proxy.h"
#include "ui_effect_controller_client_proxy.h"
#include "iremote_object_mocker.h"

bool g_setWriteStrongParcelableErrorFlag = false;
int32_t g_setWriteStrongParcelableErrorIndex = 0;
int32_t g_setWriteStrongParcelableCurrentIndex = -1;

void ResetWriteStrongParcelableState()
{
    g_setWriteStrongParcelableErrorFlag = false;
    g_setWriteStrongParcelableErrorIndex = 0;
    g_setWriteStrongParcelableCurrentIndex = -1;
}

void SetWriteStrongParcelableErrorFlag(bool flag)
{
    g_setWriteStrongParcelableErrorFlag = flag;
}

void SetWriteStrongParcelableErrorIndex(int32_t num)
{
    g_setWriteStrongParcelableErrorIndex = num;
}

namespace OHOS {
bool Parcel::WriteStrongParcelable(const sptr<Parcelable>& object)
{
    g_setWriteStrongParcelableCurrentIndex++;
    if (g_setWriteStrongParcelableErrorFlag &&
        g_setWriteStrongParcelableCurrentIndex == g_setWriteStrongParcelableErrorIndex) {
        return false;
    }
    return true;
}
} // namespace OHOS

namespace OHOS::Rosen {
class UIEffectControllerProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void UIEffectControllerProxyTest::SetUpTestCase() {}

void UIEffectControllerProxyTest::TearDownTestCase() {}

void UIEffectControllerProxyTest::SetUp()
{
    ResetWriteStrongParcelableState();
    MockMessageParcel::ClearAllErrorFlag();
}

void UIEffectControllerProxyTest::TearDown() {}

namespace {
using namespace testing;
using namespace testing::ext;
// 需要ENABLE_MOCK_READ_UINT32 启用, Remote() == nullptr 可以通过设置nullptr来覆盖
HWTEST_F(UIEffectControllerProxyTest, UIEffectControllerProxySetParam, TestSize.Level1)
{
    sptr<MockIRemoteObject> iRemoteObjectMocker = sptr<MockIRemoteObject>::MakeSptr();
    sptr<UIEffectControllerProxy> proxy =
        sptr<UIEffectControllerProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(proxy, nullptr);
    sptr<UIEffectParams> params = sptr<UIEffectParams>::MakeSptr();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    EXPECT_EQ(proxy->SetParams(params), WMError::WM_ERROR_IPC_FAILED);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);
    MockMessageParcel::ClearAllErrorFlag();
    SetWriteStrongParcelableErrorFlag(true);
    SetWriteStrongParcelableErrorIndex(0);
    EXPECT_EQ(proxy->SetParams(nullptr), WMError::WM_ERROR_IPC_FAILED);
    SetWriteStrongParcelableErrorFlag(false);
    iRemoteObjectMocker->SetRequestResult(10);
    EXPECT_EQ(proxy->SetParams(params), WMError::WM_ERROR_IPC_FAILED);
    iRemoteObjectMocker->SetRequestResult(0);
    MockMessageParcel::SetReadUint32ErrorFlag(true);
    EXPECT_EQ(proxy->SetParams(params), WMError::WM_ERROR_IPC_FAILED);
    MockMessageParcel::SetReadUint32ErrorFlag(false);
    proxy->SetParams(params);
    sptr<UIEffectControllerProxy> proxyInValid =
        sptr<UIEffectControllerProxy>::MakeSptr(nullptr);
    EXPECT_EQ(proxyInValid->SetParams(params), WMError::WM_ERROR_IPC_FAILED);
}

HWTEST_F(UIEffectControllerProxyTest, UIEffectControllerProxyAnimateTo, TestSize.Level1)
{
    sptr<MockIRemoteObject> iRemoteObjectMocker = sptr<MockIRemoteObject>::MakeSptr();
    sptr<UIEffectControllerProxy> proxy =
        sptr<UIEffectControllerProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(proxy, nullptr);
    MockMessageParcel::ClearAllErrorFlag();
    sptr<UIEffectParams> params = sptr<UIEffectParams>::MakeSptr();
    sptr<WindowAnimationOption> config = sptr<WindowAnimationOption>::MakeSptr();
    sptr<WindowAnimationOption> interruptOption = sptr<WindowAnimationOption>::MakeSptr();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    EXPECT_EQ(proxy->AnimateTo(params, config, interruptOption), WMError::WM_ERROR_IPC_FAILED);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);
    // index 0 failed
    ResetWriteStrongParcelableState();
    SetWriteStrongParcelableErrorFlag(true);
    SetWriteStrongParcelableErrorIndex(0);
    EXPECT_EQ(proxy->AnimateTo(params, config, interruptOption), WMError::WM_ERROR_IPC_FAILED);
    SetWriteStrongParcelableErrorFlag(false);
    // index 1 failed
    ResetWriteStrongParcelableState();
    SetWriteStrongParcelableErrorFlag(true);
    SetWriteStrongParcelableErrorIndex(1);
    EXPECT_EQ(proxy->AnimateTo(params, config, interruptOption), WMError::WM_ERROR_IPC_FAILED);
    SetWriteStrongParcelableErrorFlag(false);
    // index 2 bool failed
    MockMessageParcel::SetWriteBoolErrorFlag(true);
    EXPECT_EQ(proxy->AnimateTo(params, config, interruptOption), WMError::WM_ERROR_IPC_FAILED);
    MockMessageParcel::SetWriteBoolErrorFlag(false);
    // index 2 failed
    ResetWriteStrongParcelableState();
    SetWriteStrongParcelableErrorFlag(true);
    SetWriteStrongParcelableErrorIndex(2);
    EXPECT_EQ(proxy->AnimateTo(params, config, interruptOption), WMError::WM_ERROR_IPC_FAILED);
    SetWriteStrongParcelableErrorFlag(false);
    // index 2 nullptr
    proxy->AnimateTo(params, config, nullptr);
    // send request failed
    iRemoteObjectMocker->SetRequestResult(10);
    EXPECT_EQ(proxy->AnimateTo(params, config, interruptOption), WMError::WM_ERROR_IPC_FAILED);
    iRemoteObjectMocker->SetRequestResult(0);
    // read result failed
    MockMessageParcel::SetReadUint32ErrorFlag(true);
    EXPECT_EQ(proxy->AnimateTo(params, config, interruptOption), WMError::WM_ERROR_IPC_FAILED);
    MockMessageParcel::SetReadUint32ErrorFlag(false);
    // read result ok
    proxy->AnimateTo(params, config, interruptOption);
    sptr<UIEffectControllerProxy> proxyInValid =
        sptr<UIEffectControllerProxy>::MakeSptr(nullptr);
    EXPECT_EQ(proxyInValid->AnimateTo(params, config, interruptOption), WMError::WM_ERROR_IPC_FAILED);
}

HWTEST_F(UIEffectControllerProxyTest, UIEffectControllerClientProxySetParam, TestSize.Level1)
{
    sptr<MockIRemoteObject> iRemoteObjectMocker = sptr<MockIRemoteObject>::MakeSptr();
    sptr<UIEffectControllerClientProxy> proxy =
        sptr<UIEffectControllerClientProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(proxy, nullptr);
    MockMessageParcel::ClearAllErrorFlag();
    sptr<UIEffectParams> params = sptr<UIEffectParams>::MakeSptr();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    EXPECT_EQ(proxy->SetParams(params), WMError::WM_ERROR_IPC_FAILED);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);
    ResetWriteStrongParcelableState();
    SetWriteStrongParcelableErrorFlag(true);
    SetWriteStrongParcelableErrorIndex(0);
    EXPECT_EQ(proxy->SetParams(nullptr), WMError::WM_ERROR_IPC_FAILED);
    SetWriteStrongParcelableErrorFlag(false);
    iRemoteObjectMocker->SetRequestResult(10);
    EXPECT_EQ(proxy->SetParams(params), WMError::WM_ERROR_IPC_FAILED);
    iRemoteObjectMocker->SetRequestResult(0);
    EXPECT_EQ(proxy->SetParams(params), WMError::WM_OK);
    sptr<UIEffectControllerClientProxy> proxyInValid =
        sptr<UIEffectControllerClientProxy>::MakeSptr(nullptr);
    EXPECT_EQ(proxyInValid->SetParams(params), WMError::WM_ERROR_IPC_FAILED);
}
}
} // namespace OHOS::Rosen