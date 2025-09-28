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
#include <gmock/gmock.h>
#include <ipc_types.h>
#include <pointer_event.h>
#include "iremote_object_mocker.h"
#include "mock/mock_session_stub.h"
#include "session/host/include/zidl/session_stub.h"
#include "ability_start_setting.h"
#include "parcel/accessibility_event_info_parcel.h"
#include "session/host/include/zidl/session_ipc_interface_code.h"
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
    session_ = sptr<SessionStubMocker>::MakeSptr();
    EXPECT_NE(nullptr, session_);
}

void SessionStubTest::TearDown()
{
    session_ = nullptr;
}

namespace {
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
    ASSERT_EQ(ERR_NONE, res);
    data.WriteInterfaceToken(u"OHOS.ISession");
    res = session_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: ProcessRemoteRequestTest01
 * @tc.desc: sessionStub ProcessRemoteRequestTest01
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, ProcessRemoteRequestTest01, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    data.WriteBool(true);
    auto res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_WINDOW_ANIMATION_FLAG), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);

    data.WriteBool(false);
    data.WriteBool(true);
    data.WriteString("");
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_FOREGROUND), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);

    data.WriteBool(true);
    data.WriteString("");
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_BACKGROUND), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);

    data.WriteBool(true);
    data.WriteString("");
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_DISCONNECT), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    EXPECT_NE(data.WriteRemoteObject(iRemoteObjectMocker), false);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_CONNECT), data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    ASSERT_EQ(data.WriteUint32(100), true);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SESSION_EVENT), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    AAFwk::Want options;
    EXPECT_NE(data.WriteString("HandleSessionException"), false);
    EXPECT_NE(data.WriteParcelable(&options), false);
    EXPECT_NE(data.WriteBool(false), false);
    EXPECT_NE(data.WriteInt32(33), false);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TERMINATE), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_SESSION_RECT), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_TO_APP_TOP), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: ProcessRemoteRequestTest02
 * @tc.desc: sessionStub ProcessRemoteRequestTest02
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, ProcessRemoteRequestTest02, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    data.WriteBool(true);
    auto res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_WINDOW_ANIMATION_FLAG), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_APP_MAIN_WINDOW), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    data.WriteBool(true);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_BACKPRESSED), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_MARK_PROCESSED), data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_MAXIMIZE_MODE), data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_MAXIMIZE_MODE), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NEED_AVOID), data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_AVOID_AREA), data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_ASPECT_RATIO), data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_CUSTOM_ANIMATION), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRANSFER_EXTENSION_DATA), data, reply, option);
    ASSERT_EQ(ERR_INVALID_VALUE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_EXTENSION_DIED), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    data.WriteInt32(1);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_EXTENSION_TIMEOUT), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: ProcessRemoteRequestTest03
 * @tc.desc: sessionStub ProcessRemoteRequestTest03
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, ProcessRemoteRequestTest03, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    EXPECT_NE(data.WriteRemoteObject(iRemoteObjectMocker), false);
    ASSERT_EQ(data.WriteUint32(1), true);
    AAFwk::Want options;
    EXPECT_NE(data.WriteString("HandleSessionException"), false);
    EXPECT_NE(data.WriteParcelable(&options), false);
    ASSERT_EQ(data.WriteUint64(2), true);
    auto res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SHOW), data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_HIDE), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_DRAWING_COMPLETED), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_APP_REMOVE_STARTING_WINDOW), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_RECTCHANGE_LISTENER_REGISTERED),
        data,
        reply,
        option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_LANDSCAPE_MULTI_WINDOW), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_ACTIVE_PENDING_SESSION), data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    MessageParcel tmp;
    tmp.WriteUint32(1);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRANSFER_ABILITY_RESULT), tmp, reply, option);
    ASSERT_EQ(ERR_INVALID_VALUE, res);
}

/**
 * @tc.name: ProcessRemoteRequestTest04
 * @tc.desc: sessionStub ProcessRemoteRequestTest04
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, ProcessRemoteRequestTest04, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ASSERT_EQ(data.WriteInt32(1), true);
    ASSERT_EQ(data.WriteInt32(2), true);
    auto res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_PROCESS_POINT_DOWN_SESSION), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SEND_POINTEREVENT_FOR_MOVE_DRAG), data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_CALLING_SESSION_ID), data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_CUSTOM_DECOR_HEIGHT), data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_ADJUST_KEYBOARD_LAYOUT), data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_ASYNC_ON), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_SYNC_ON), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRIGGER_BIND_MODAL_UI_EXTENSION), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: ProcessRemoteRequestTest05
 * @tc.desc: sessionStub ProcessRemoteRequestTest05
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, ProcessRemoteRequestTest05, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    EXPECT_NE(data.WriteRemoteObject(iRemoteObjectMocker), false);
    ASSERT_EQ(data.WriteUint32(1), true);
    AAFwk::Want options;
    EXPECT_NE(data.WriteString("HandleSessionException"), false);
    EXPECT_NE(data.WriteParcelable(&options), false);
    ASSERT_EQ(data.WriteUint64(2), true);
    auto res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_PIP_WINDOW_PREPARE_CLOSE), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_LAYOUT_FULL_SCREEN_CHANGE), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RESTORE_MAIN_WINDOW), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_DIALOG_SESSION_BACKGESTURE_ENABLE),
        data,
        reply,
        option);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: ProcessRemoteRequestTest06
 * @tc.desc: sessionStub ProcessRemoteRequestTest06
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, ProcessRemoteRequestTest06, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    AAFwk::Want want;
    data.WriteParcelable(&want);
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    EXPECT_NE(data.WriteRemoteObject(iRemoteObjectMocker), false);
    ASSERT_EQ(data.WriteUint32(1), true);
    ASSERT_EQ(data.WriteUint32(1), true);
    AAFwk::Want options;
    EXPECT_NE(data.WriteString("HandleSessionException"), false);
    EXPECT_NE(data.WriteString("HandleSessionException"), false);
    EXPECT_NE(data.WriteParcelable(&options), false);
    ASSERT_EQ(data.WriteUint64(2), true);
    auto res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_EXCEPTION), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    ASSERT_EQ(data.WriteUint32(1), true);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_CALLING_SESSION_ID), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    ASSERT_EQ(data.WriteInt32(1), true);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_CUSTOM_DECOR_HEIGHT), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: ProcessRemoteRequestTest07
 * @tc.desc: sessionStub ProcessRemoteRequestTest07
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, ProcessRemoteRequestTest07, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_SYNC};
    ASSERT_EQ(data.WriteInt32(1), true);
    ASSERT_EQ(data.WriteInt32(1), true);
    ASSERT_EQ(data.WriteUint32(1), true);
    ASSERT_EQ(data.WriteUint32(1), true);
    ASSERT_EQ(data.WriteUint32(1), true);
    auto res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_PIP_RECT), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    ASSERT_EQ(data.WriteUint32(1), true);
    ASSERT_EQ(data.WriteInt32(1), true);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_PIP_CONTROL_STATUS), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    ASSERT_EQ(data.WriteBool(true), true);
    ASSERT_EQ(data.WriteBool(true), true);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TITLE_AND_DOCK_HOVER_SHOW_CHANGE), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    ASSERT_EQ(data.WriteBool(true), true);
    ASSERT_EQ(data.WriteBool(false), true);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_WINDOW_RECT_AUTO_SAVE), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    ASSERT_EQ(data.WriteInt32(1), true);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_EXTENSION_EVENT_ASYNC), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    ASSERT_EQ(data.WriteFloat(1.0f), true);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_WINDOW_CORNER_RADIUS), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    ASSERT_EQ(data.WriteBool(true), true);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_FOLLOW_PARENT_MULTI_SCREEN_POLICY),
        data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    ASSERT_EQ(data.WriteString("test"), true);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_FLAG), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
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
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    EXPECT_NE(data.WriteRemoteObject(iRemoteObjectMocker), false);
    ASSERT_EQ(data.WriteUint32(1), true);
    AAFwk::Want options;
    EXPECT_NE(data.WriteString("HandleSessionException"), false);
    EXPECT_NE(data.WriteParcelable(&options), false);
    ASSERT_EQ(data.WriteUint64(2), true);
    auto res = session_->HandleRaiseAboveTarget(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->HandleRaiseAppMainWindowToTop(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->HandleBackPressed(data, reply);
    if (!data.ReadBool()) {
        ASSERT_EQ(ERR_INVALID_DATA, res);
    }
    res = session_->HandleGetGlobalMaximizeMode(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->HandleUpdateWindowSceneAfterCustomAnimation(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->HandleNotifyExtensionDied(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->HandleGetStatusBarHeight(data, reply);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: sessionStubTest03
 * @tc.desc: sessionStub sessionStubTest03
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, sessionStubTest03, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    ASSERT_EQ(data.WriteUint32(1), true);
    auto res = session_->HandleSetGlobalMaximizeMode(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->HandleTransferExtensionData(data, reply);
    ASSERT_EQ(ERR_INVALID_VALUE, res);
    ASSERT_EQ(data.WriteInt32(2), true);
    res = session_->HandleMarkProcessed(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    ASSERT_EQ(data.WriteBool(true), true);
    res = session_->HandleNeedAvoid(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    ASSERT_EQ(data.WriteUint32(2), true);
    ASSERT_EQ(data.WriteInt32(0), true);
    ASSERT_EQ(data.WriteInt32(0), true);
    ASSERT_EQ(data.WriteInt32(10), true);
    ASSERT_EQ(data.WriteInt32(10), true);
    ASSERT_EQ(data.WriteInt32(12), true);
    res = session_->HandleGetAvoidAreaByType(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    ASSERT_EQ(data.WriteFloat(2.0f), true);
    res = session_->HandleSetAspectRatio(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    ASSERT_EQ(data.WriteInt32(2), true);
    res = session_->HandleNotifyExtensionTimeout(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    ASSERT_EQ(data.WriteInt32(2), true);
    AAFwk::Want want;
    data.WriteParcelable(&want);
    res = session_->HandleTransferAbilityResult(data, reply);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleTriggerBindModalUIExtension001
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleTriggerBindModalUIExtension001, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    auto res = session_->HandleTriggerBindModalUIExtension(data, reply);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleTransferAccessibilityEvent002
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleTransferAccessibilityEvent003, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    auto res = session_->HandleTransferAccessibilityEvent(data, reply);
    ASSERT_EQ(ERR_INVALID_DATA, res);
}

/**
 * @tc.name: HandleNotifyPiPWindowPrepareClose003
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleNotifyPiPWindowPrepareClose003, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    auto res = session_->HandleNotifyPiPWindowPrepareClose(data, reply);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleUpdatePiPRect001
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleUpdatePiPRect001, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    float width = 0.55;
    float height = 0.55;
    ASSERT_EQ(ERR_INVALID_DATA, session_->HandleUpdatePiPRect(data, reply));
    data.WriteInt32(-1);
    ASSERT_EQ(ERR_INVALID_DATA, session_->HandleUpdatePiPRect(data, reply));
    data.WriteInt32(10);
    data.WriteInt32(-1);
    ASSERT_EQ(ERR_INVALID_DATA, session_->HandleUpdatePiPRect(data, reply));
    data.WriteInt32(10);
    data.WriteInt32(20);
    data.WriteUint32(width);
    ASSERT_EQ(ERR_INVALID_DATA, session_->HandleUpdatePiPRect(data, reply));
    data.WriteInt32(10);
    data.WriteInt32(20);
    data.WriteUint32(30);
    data.WriteUint32(height);
    ASSERT_EQ(ERR_INVALID_DATA, session_->HandleUpdatePiPRect(data, reply));
}

/**
 * @tc.name: HandleUpdatePiPRect002
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleUpdatePiPRect002, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteInt32(10);
    data.WriteInt32(20);
    data.WriteUint32(30);
    data.WriteUint32(40);
    ASSERT_EQ(ERR_INVALID_DATA, session_->HandleUpdatePiPRect(data, reply));
    data.WriteInt32(10);
    data.WriteInt32(20);
    data.WriteUint32(30);
    data.WriteUint32(40);
    data.WriteUint32(3);
    ASSERT_EQ(ERR_NONE, session_->HandleUpdatePiPRect(data, reply));
}

/**
 * @tc.name: HandleUpdatePiPControlStatus
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleUpdatePiPControlStatus, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    uint32_t controlType = -1;
    int32_t status = 1;
    data.WriteUint32(controlType);
    data.WriteInt32(status);
    auto res = session_->HandleUpdatePiPControlStatus(data, reply);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    data.WriteUint32(10);
    data.WriteInt32(status);
    res = session_->HandleUpdatePiPControlStatus(data, reply);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    data.WriteUint32(2);
    data.WriteInt32(-4);
    res = session_->HandleUpdatePiPControlStatus(data, reply);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    data.WriteUint32(2);
    data.WriteInt32(4);
    res = session_->HandleUpdatePiPControlStatus(data, reply);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    data.WriteUint32(2);
    data.WriteInt32(1);
    res = session_->HandleUpdatePiPControlStatus(data, reply);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleSetAutoStartPiP
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleSetAutoStartPiP, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    MessageParcel data;
    MessageParcel reply;
    ASSERT_EQ(ERR_INVALID_DATA, session_->HandleSetAutoStartPiP(data, reply));
    bool isAutoStartValid = true;
    uint32_t priority = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    data.WriteInt32(10);
    ASSERT_EQ(ERR_INVALID_DATA, session_->HandleSetAutoStartPiP(data, reply));
    data.WriteBool(isAutoStartValid);
    data.WriteUint32(priority);
    data.WriteUint32(width);
    data.WriteUint32(height);
    ASSERT_EQ(ERR_NONE, session_->HandleSetAutoStartPiP(data, reply));
}

/**
 * @tc.name: HandleProcessPointDownSession006
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleProcessPointDownSession006, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteInt32(10);
    data.WriteInt32(20);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    auto res = session_->HandleProcessPointDownSession(data, reply);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleSendPointerEvenForMoveDrag007
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleSendPointerEvenForMoveDrag007, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    auto res = session_->HandleSendPointerEvenForMoveDrag(data, reply);
    ASSERT_EQ(ERR_INVALID_DATA, res);
}

/**
 * @tc.name: HandleNotifySyncOn012
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleNotifySyncOn012, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    auto res = session_->HandleNotifySyncOn(data, reply);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleNotifyAsyncOn013
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleNotifyAsyncOn013, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    auto res = session_->HandleNotifyAsyncOn(data, reply);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleNotifyExtensionTimeout014
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleNotifyExtensionTimeout014, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    auto res = session_->HandleNotifyExtensionTimeout(data, reply);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleGetAppForceLandscapeConfig
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleGetAppForceLandscapeConfig, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    ASSERT_NE(session_, nullptr);
    auto res = session_->HandleGetAppForceLandscapeConfig(data, reply);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleSetDialogSessionBackGestureEnabled01
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleSetDialogSessionBackGestureEnabled01, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    auto res = session_->HandleSetDialogSessionBackGestureEnabled(data, reply);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleRequestFocus
 * @tc.desc: sessionStub HandleRequestFocusTest
 * @tc.type: FUNC
 * @tc.require: #IAPLFA
 */
HWTEST_F(SessionStubTest, HandleRequestFocus, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(false);
    ASSERT_NE(session_, nullptr);
    auto res = session_->HandleRequestFocus(data, reply);
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: HandleSyncSessionEvent1
 * @tc.desc: sessionStub HandleSyncSessionEvent
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleSyncSessionEvent1, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(static_cast<uint32_t>(SessionEvent::EVENT_MAXIMIZE));
    auto result = session_->HandleSyncSessionEvent(data, reply);
    ASSERT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: HandleSyncSessionEvent2
 * @tc.desc: sessionStub HandleSyncSessionEvent
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleSyncSessionEvent2, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(0xFFFFFFFF);
    auto result = session_->HandleSyncSessionEvent(data, reply);
    ASSERT_EQ(result, ERR_NONE);

    result = session_->HandleSyncSessionEvent(data, reply);
    ASSERT_EQ(result, ERR_INVALID_DATA);
}

/**
 * @tc.name: GetIsMidScene
 * @tc.desc: sessionStub GetIsMidScene
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, GetIsMidScene, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    auto result = session_->HandleGetIsMidScene(data, reply);
    ASSERT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: HandleNotifyFrameLayoutFinish
 * @tc.desc: sessionStub HandleNotifyFrameLayoutFinish
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleNotifyFrameLayoutFinish, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    auto result = session_->HandleNotifyFrameLayoutFinish(data, reply);
    ASSERT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: HandleSyncSessionEvent
 * @tc.desc: sessionStub HandleSyncSessionEvent
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleSyncSessionEvent, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    auto result = session_->HandleSyncSessionEvent(data, reply);
    ASSERT_EQ(result, ERR_INVALID_DATA);
}

/**
 * @tc.name: HandlePendingSessionActivation
 * @tc.desc: sessionStub HandlePendingSessionActivation
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandlePendingSessionActivation, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    auto result = session_->HandlePendingSessionActivation(data, reply);
    ASSERT_EQ(result, ERR_INVALID_DATA);
}

/**
 * @tc.name: HandleGetGlobalScaledRect
 * @tc.desc: sessionStub HandleGetGlobalScaledRect
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleGetGlobalScaledRect, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    auto result = session_->HandleGetGlobalScaledRect(data, reply);
    ASSERT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: HandleGetAllAvoidAreas
 * @tc.desc: sessionStub HandleGetAllAvoidAreas
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleGetAllAvoidAreas, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    auto result = session_->HandleGetAllAvoidAreas(data, reply);
    ASSERT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: HandleIsStartMoving
 * @tc.desc: sessionStub HandleIsStartMoving
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleIsStartMoving, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    auto result = session_->HandleIsStartMoving(data, reply);
    ASSERT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: HandleSetSessionLabelAndIcon01
 * @tc.desc: HandleSetSessionLabelAndIcon
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleSetSessionLabelAndIcon01, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    std::shared_ptr<Media::PixelMap> icon = std::make_shared<Media::PixelMap>();
    data.WriteParcelable(icon.get());

    auto res = session_->HandleSetSessionLabelAndIcon(data, reply);
    ASSERT_EQ(ERR_INVALID_DATA, res);
}

/**
 * @tc.name: HandleSetSessionLabelAndIcon02
 * @tc.desc: HandleSetSessionLabelAndIcon
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleSetSessionLabelAndIcon02, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    std::string label = "demo label";
    data.WriteString(label);
    std::shared_ptr<Media::PixelMap> icon = nullptr;
    data.WriteParcelable(icon.get());

    auto res = session_->HandleSetSessionLabelAndIcon(data, reply);
    ASSERT_EQ(ERR_INVALID_DATA, res);
}

/**
 * @tc.name: HandleSetSessionLabelAndIcon03
 * @tc.desc: HandleSetSessionLabelAndIcon
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleSetSessionLabelAndIcon03, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    std::string label = "demo label";
    data.WriteString(label);
    const uint32_t color[] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};
    uint32_t len = sizeof(color) / sizeof(color[0]);
    Media::InitializationOptions opts;
    opts.size.width = 2;
    opts.size.height = 3;
    opts.pixelFormat = Media::PixelFormat::UNKNOWN;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    std::shared_ptr<Media::PixelMap> icon = Media::PixelMap::Create(color, len, 0, opts.size.width, opts);
    data.WriteParcelable(icon.get());

    auto res = session_->HandleSetSessionLabelAndIcon(data, reply);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleGetCrossAxisState
 * @tc.desc: HandleGetCrossAxisState
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleGetCrossAxisState, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    sptr<SessionStubMocker> session = sptr<SessionStubMocker>::MakeSptr();
    EXPECT_CALL(*session, GetCrossAxisState(_)).
        WillOnce(DoAll(SetArgReferee<0>(CrossAxisState::STATE_CROSS), Return(WSError::WS_OK)));
    session->HandleGetCrossAxisState(data, reply);
    uint32_t state = 0;
    reply.ReadUint32(state);
    ASSERT_EQ(state, static_cast<uint32_t>(CrossAxisState::STATE_CROSS));
}

/**
 * @tc.name: HandleContainerModalEvent
 * @tc.desc: sessionStub HandleContainerModalEvent
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleContainerModalEvent, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString("name");
    data.WriteString("value");
    auto result = session_->HandleContainerModalEvent(data, reply);
    ASSERT_EQ(result, ERR_NONE);
    result = session_->HandleContainerModalEvent(data, reply);
    ASSERT_EQ(result, ERR_INVALID_DATA);
}

/**
 * @tc.name: GetIsHighlighted
 * @tc.desc: sessionStub GetIsHighlighted
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, GetIsHighlighted, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    auto result = session_->HandleGetIsHighlighted(data, reply);
    ASSERT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: HandleNotifyIsFullScreenInForceSplitMode
 * @tc.desc: HandleNotifyIsFullScreenInForceSplitMode test
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubTest, HandleNotifyIsFullScreenInForceSplitMode, TestSize.Level1)
{
    {
        MessageOption option;
        MessageParcel data;
        MessageParcel reply;
        EXPECT_EQ(session_->HandleNotifyIsFullScreenInForceSplitMode(data, reply), ERR_INVALID_DATA);
    }
    {
        MessageOption option;
        MessageParcel data;
        MessageParcel reply;
        bool isFullScreen = true;
        data.WriteBool(isFullScreen);
        EXPECT_EQ(session_->HandleNotifyIsFullScreenInForceSplitMode(data, reply), ERR_NONE);
    }
}
}
} // namespace Rosen
} // namespace OHOS