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
#include <pointer_event.h>
#include "iremote_object_mocker.h"
#include "mock/mock_session_stub.h"
#include "session/host/include/zidl/session_stub.h"
#include "ability_start_setting.h"
#include <ipc_types.h>
#include "want.h"
#include "session/host/include/zidl/session_ipc_interface_code.h"
#include "accessibility_event_info_parcel.h"
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
class SessionStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    sptr<SessionStub> session_ = nullptr;
};

void SessionStubTest::SetUpTestCase()
{
}

void SessionStubTest::TearDownTestCase()
{
}

void SessionStubTest::SetUp()
{
    session_ = new (std::nothrow) SessionStubMocker();
    EXPECT_NE(nullptr, session_);
}

void SessionStubTest::TearDown()
{
    session_ = nullptr;
}

namespace {
/**
 * @tc.name: HandleTransferAccessibilityEvent
 * @tc.desc: sessionStub HandleTransferAccessibilityEvent
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleTransferAccessibilityEvent, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    Accessibility::AccessibilityEventInfo info;
    Accessibility::AccessibilityEventInfoParcel infoParcel(info);
    data.WriteParcelable(&infoParcel);

    int32_t uiExtensionIdLevel = 0;
    data.WriteInt32(uiExtensionIdLevel);
    ASSERT_EQ(ERR_NONE, session_->HandleTransferAccessibilityEvent(data, reply));
}

/**
 * @tc.name: OnRemoteRequest01
 * @tc.desc: sessionStub OnRemoteRequest01
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, OnRemoteRequest01, Function | SmallTest | Level2)
{
    uint32_t code = 1;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    auto res = session_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(0, res);
    data.WriteInterfaceToken(u"OHOS.ISession");
    res = session_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: sessionStubTest01
 * @tc.desc: sessionStub sessionStubTest01
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, sessionStubTest01, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    auto res = session_->HandleSetWindowAnimationFlag(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleForeground(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleBackground(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleDisconnect(data, reply);
    ASSERT_EQ(0, res);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
    EXPECT_NE(data.WriteRemoteObject(iRemoteObjectMocker), false);
    res = session_->HandleConnect(data, reply);
    ASSERT_EQ(5, res);
    ASSERT_EQ(data.WriteUint32(1), true);
    res = session_->HandleSessionEvent(data, reply);
    ASSERT_EQ(0, res);
    AAFwk::Want options;
    EXPECT_NE(data.WriteString("HandleSessionException"), false);
    EXPECT_NE(data.WriteParcelable(&options), false);
    res = session_->HandleTerminateSession(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleUpdateActivateStatus(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleUpdateSessionRect(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleRaiseToAppTop(data, reply);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: sessionStubTest02
 * @tc.desc: sessionStub sessionStubTest02
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, sessionStubTest02, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
    EXPECT_NE(data.WriteRemoteObject(iRemoteObjectMocker), false);
    ASSERT_EQ(data.WriteUint32(1), true);
    AAFwk::Want options;
    EXPECT_NE(data.WriteString("HandleSessionException"), false);
    EXPECT_NE(data.WriteParcelable(&options), false);
    ASSERT_EQ(data.WriteUint64(2), true);
    auto res = session_->HandleRaiseAboveTarget(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleRaiseAppMainWindowToTop(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleBackPressed(data, reply);
    ASSERT_EQ(5, res);
    res = session_->HandleMarkProcessed(data, reply);
    ASSERT_EQ(5, res);
    res = session_->HandleSetGlobalMaximizeMode(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleGetGlobalMaximizeMode(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleNeedAvoid(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleGetAvoidAreaByType(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleSetSessionProperty(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleSetAspectRatio(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleUpdateWindowSceneAfterCustomAnimation(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleTransferAbilityResult(data, reply);
    ASSERT_EQ(22, res);
    res = session_->HandleTransferExtensionData(data, reply);
    ASSERT_EQ(22, res);
    res = session_->HandleNotifyRemoteReady(data, reply);
    ASSERT_EQ(0, res);
    res = session_->HandleNotifyExtensionDied(data, reply);
    ASSERT_EQ(0, res);
}
}
} // namespace Rosen
} // namespace OHOS