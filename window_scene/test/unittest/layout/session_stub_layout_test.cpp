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

#include <gtest/gtest.h>
#include <ipc_types.h>
#include <pointer_event.h>

#include "ability_start_setting.h"
#include "iremote_object_mocker.h"
#include "mock/mock_session_stub.h"
#include "parcel/accessibility_event_info_parcel.h"
#include "session/host/include/zidl/session_ipc_interface_code.h"
#include "session/host/include/zidl/session_stub.h"
#include "want.h"
#include "ws_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
const std::string UNDEFINED = "undefined";
}

namespace OHOS::Accessibility {
class AccessibilityEventInfo;
}
class SessionStubLayoutTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    sptr<SessionStub> session_ = nullptr;
};

void SessionStubLayoutTest::SetUpTestCase() {}

void SessionStubLayoutTest::TearDownTestCase() {}

void SessionStubLayoutTest::SetUp()
{
    session_ = sptr<SessionStubMocker>::MakeSptr();
}

void SessionStubLayoutTest::TearDown()
{
    session_ = nullptr;
}

namespace {
/**
 * @tc.name: HandleUpdateRectChangeListenerRegistered008
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubLayoutTest, HandleUpdateRectChangeListenerRegistered008, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    auto res = session_->HandleUpdateRectChangeListenerRegistered(data, reply);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleUpdateSessionRect
 * @tc.desc: test for HandleUpdateSessionRect
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubLayoutTest, HandleUpdateSessionRect, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteUint32(10);
    data.WriteUint32(20);
    data.WriteUint32(30);
    data.WriteUint32(40);
    data.WriteUint32(0);
    data.WriteBool(true);
    data.WriteBool(true);
    auto res = session_->HandleUpdateSessionRect(data, reply);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleUpdateClientRect01
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubLayoutTest, HandleUpdateClientRect01, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = session_->HandleUpdateClientRect(data, reply);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    data.WriteInt32(100);
    data.WriteInt32(100);
    data.WriteInt32(800);
    data.WriteInt32(800);
    res = session_->HandleUpdateClientRect(data, reply);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleSetSystemEnableDrag_TestReadBool
 * @tc.desc: Check whether the enableDrag is read successfully.
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubLayoutTest, HandleSetSystemEnableDrag_TestReadBool, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = session_->HandleSetSystemEnableDrag(data, reply);
    ASSERT_EQ(ERR_INVALID_DATA, res);

    data.WriteBool(true);
    res = session_->HandleSetSystemEnableDrag(data, reply);
    ASSERT_EQ(ERR_NONE, res);
}
} // namespace
} // namespace Rosen
} // namespace OHOS