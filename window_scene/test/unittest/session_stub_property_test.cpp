/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "mock/mock_session_stub.h"
#include "session/host/include/zidl/session_ipc_interface_code.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionStubPropertyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    sptr<SessionStubMocker> session_ = nullptr;
};

void SessionStubPropertyTest::SetUpTestCase() {}

void SessionStubPropertyTest::TearDownTestCase() {}

void SessionStubPropertyTest::SetUp()
{
    session_ = sptr<SessionStubMocker>::MakeSptr();
    EXPECT_NE(nullptr, session_);

    EXPECT_CALL(*session_, OnRemoteRequest(_, _, _, _))
        .WillOnce(Invoke([&](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) -> int {
            return session_->SessionStub::OnRemoteRequest(code, data, reply, option);
        }));
}

void SessionStubPropertyTest::TearDown()
{
    session_ = nullptr;
}

namespace {

/**
 * @tc.name: HandleUpdatePropertyByAction01
 * @tc.desc: No error
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubPropertyTest, HandleUpdatePropertyByAction01, TestSize.Level1)
{
    EXPECT_CALL(*session_, UpdateSessionPropertyByAction(_, _)).WillOnce(Return(WMError::WM_OK));

    MessageParcel data;
    MessageParcel reply;
    MessageOption option{ MessageOption::TF_SYNC };
    data.WriteInterfaceToken(u"OHOS.ISession");
    data.WriteUint64(static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_MAIN_WINDOW_TOPMOST));
    uint32_t code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_SESSION_PROPERTY);

    int ret = session_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, ret);
}

/**
 * @tc.name: HandleUpdatePropertyByAction02
 * @tc.desc: Invalid data
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubPropertyTest, HandleUpdatePropertyByAction02, TestSize.Level1)
{
    EXPECT_CALL(*session_, UpdateSessionPropertyByAction(_, _)).Times(0);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option{ MessageOption::TF_SYNC };
    data.WriteInterfaceToken(u"OHOS.ISession");
    const uint32_t invalidData = 0;
    data.WriteUint32(invalidData);
    uint32_t code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_SESSION_PROPERTY);

    int ret = session_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, ret);
}

/**
 * @tc.name: HandleUpdatePropertyByAction03
 * @tc.desc: No action
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubPropertyTest, HandleUpdatePropertyByAction03, TestSize.Level1)
{
    EXPECT_CALL(*session_, UpdateSessionPropertyByAction(_, _)).Times(0);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option{ MessageOption::TF_SYNC };
    data.WriteInterfaceToken(u"OHOS.ISession");
    uint32_t code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_SESSION_PROPERTY);

    int ret = session_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, ret);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
