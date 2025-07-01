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
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ui_effect_controller_client.h"
#include "ui_effect_controller.h"
#include "ui_effect_controller_common.h"

namespace OHOS::Rosen {
class UIEffectControllerStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void UIEffectControllerStubTest::SetUpTestCase() {}

void UIEffectControllerStubTest::TearDownTestCase() {}

void UIEffectControllerStubTest::SetUp() {}

void UIEffectControllerStubTest::TearDown() {}

namespace {
using namespace testing;
using namespace testing::ext;
HWTEST_F(UIEffectControllerStubTest, UIEffectControllerClientStubSetParam, Function | SmallTest | Level2)
{
    sptr<UIEffectControllerClientStub> stub = sptr<UIEffectControllerClient>::MakeSptr();
    uint32_t code = static_cast<uint32_t>(
        IUIEffectControllerClient::UIEffectControllerClientMessage::TRANS_ID_UIEFFECT_SET_PARAM);
    MessageParcel reply;
    MessageOption option;
    MessageParcel dataInvalid;
    // invalid interface token
    dataInvalid.WriteInterfaceToken(u"invalid");
    EXPECT_EQ(stub->OnRemoteRequest(code, dataInvalid, reply, option), ERR_TRANSACTION_FAILED);
    MessageParcel dataValid;
    dataValid.WriteInterfaceToken(stub->GetDescriptor());
    sptr<UIEffectParams> param = sptr<UIEffectParams>::MakeSptr();
    dataValid.WriteStrongParcelable(param);
    EXPECT_EQ(stub->OnRemoteRequest(code, dataValid, reply, option), ERR_NONE);
    MessageParcel dataValid2;
    dataValid2.WriteInterfaceToken(stub->GetDescriptor());
    dataValid2.WriteBool(true);
    EXPECT_EQ(stub->OnRemoteRequest(code, dataValid2, reply, option), ERR_INVALID_DATA);
    // last invalid case
    uint32_t codeInvalid = 10000;
    stub->OnRemoteRequest(codeInvalid, dataValid, reply, option);
}

HWTEST_F(UIEffectControllerStubTest, UIEffectControllerStubSetParam, TestSize.Level1)
{
    sptr<UIEffectControllerStub> stub = sptr<UIEffectController>::MakeSptr(1, nullptr, nullptr);
    MessageParcel data1;
    MessageParcel reply1;
    data1.WriteStrongParcelable(nullptr);
    EXPECT_EQ(stub->HandleSetParams(data1, reply1), ERR_INVALID_DATA);
    MessageParcel data2;
    MessageParcel reply2;
    sptr<UIEffectParams> effectPtr = sptr<UIEffectParams>::MakeSptr();
    data2.WriteStrongParcelable(effectPtr);
    EXPECT_EQ(stub->HandleSetParams(data2, reply2), ERR_NONE);
}

HWTEST_F(UIEffectControllerStubTest, UIEffectControllerStubAnimateTo, TestSize.Level1)
{
    sptr<UIEffectControllerStub> stub = sptr<UIEffectController>::MakeSptr(1, nullptr, nullptr);
    sptr<UIEffectParams> param = sptr<UIEffectParams>::MakeSptr();
    sptr<WindowAnimationOption> option = sptr<WindowAnimationOption>::MakeSptr();
    MessageParcel reply;
    MessageParcel data1;
    data1.WriteStrongParcelable(nullptr);
    EXPECT_EQ(stub->HandleAnimateTo(data1, reply), ERR_INVALID_DATA);
    MessageParcel data2;
    data2.WriteStrongParcelable(param);
    data2.WriteStrongParcelable(nullptr);
    EXPECT_EQ(stub->HandleAnimateTo(data2, reply), ERR_INVALID_DATA);
    MessageParcel data3;
    data3.WriteStrongParcelable(param);
    data3.WriteStrongParcelable(option);
    data3.WriteBool(true);
    data3.WriteStrongParcelable(nullptr);
    EXPECT_EQ(stub->HandleAnimateTo(data3, reply), ERR_INVALID_DATA);
    MessageParcel data4;
    data4.WriteStrongParcelable(param);
    data4.WriteStrongParcelable(option);
    data4.WriteBool(true);
    data4.WriteStrongParcelable(option);
    EXPECT_EQ(stub->HandleAnimateTo(data4, reply), ERR_NONE);
    MessageParcel data5;
    data5.WriteStrongParcelable(param);
    data5.WriteStrongParcelable(option);
    data5.WriteBool(false);
    EXPECT_EQ(stub->HandleAnimateTo(data5, reply), ERR_NONE);
}

HWTEST_F(UIEffectControllerStubTest, UIEffectControllerStubOnRemoteRequest, TestSize.Level1)
{
    sptr<UIEffectControllerStub> stub = sptr<UIEffectController>::MakeSptr(1, nullptr, nullptr);
    uint32_t code = 0;
    MessageParcel reply;
    MessageOption option;
    MessageParcel dataInvalid;
    // invalid interface token
    dataInvalid.WriteInterfaceToken(u"Invalid");
    EXPECT_EQ(stub->OnRemoteRequest(code, dataInvalid, reply, option), ERR_TRANSACTION_FAILED);
    MessageParcel dataValid;
    dataValid.WriteInterfaceToken(stub->GetDescriptor());
    code = static_cast<uint32_t>(IUIEffectController::UIEffectControllerMessage::TRANS_ID_UIEFFECT_SET_PARAM);
    stub->OnRemoteRequest(code, dataValid, reply, option);
    MessageParcel dataValid2;
    dataValid2.WriteInterfaceToken(stub->GetDescriptor());
    code = static_cast<uint32_t>(IUIEffectController::UIEffectControllerMessage::TRANS_ID_UIEFFECT_ANIMATE_TO);
    stub->OnRemoteRequest(code, dataValid2, reply, option);
    MessageParcel dataValid3;
    dataValid3.WriteInterfaceToken(stub->GetDescriptor());
    code = 10000;
    stub->OnRemoteRequest(code, dataValid3, reply, option);
}
}
} // namespace OHOS::Rosen