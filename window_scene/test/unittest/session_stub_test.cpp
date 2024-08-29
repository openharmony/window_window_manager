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
    session_ = new (std::nothrow) SessionStubMocker();
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
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_FOREGROUND), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_BACKGROUND), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_DISCONNECT), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
    EXPECT_NE(data.WriteRemoteObject(iRemoteObjectMocker), false);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_CONNECT), data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    ASSERT_EQ(data.WriteUint32(1), true);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SESSION_EVENT), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    AAFwk::Want options;
    EXPECT_NE(data.WriteString("HandleSessionException"), false);
    EXPECT_NE(data.WriteParcelable(&options), false);
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
    data.WriteInt32(1);
    auto res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_WINDOW_ANIMATION_FLAG), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_APP_MAIN_WINDOW), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_BACKPRESSED), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_MARK_PROCESSED), data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_MAXIMIZE_MODE), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_MAXIMIZE_MODE), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NEED_AVOID), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_AVOID_AREA), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_ASPECT_RATIO), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_CUSTOM_ANIMATION), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRANSFER_ABILITY_RESULT), data, reply, option);
    ASSERT_EQ(ERR_INVALID_VALUE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRANSFER_EXTENSION_DATA), data, reply, option);
    ASSERT_EQ(ERR_INVALID_VALUE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_EXTENSION_DIED), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_EXTENSION_TIMEOUT), data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
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
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
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
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
    EXPECT_NE(data.WriteRemoteObject(iRemoteObjectMocker), false);
    ASSERT_EQ(data.WriteUint32(1), true);
    AAFwk::Want options;
    EXPECT_NE(data.WriteString("HandleSessionException"), false);
    EXPECT_NE(data.WriteParcelable(&options), false);
    ASSERT_EQ(data.WriteUint64(2), true);
    auto res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_EXCEPTION), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_PROCESS_POINT_DOWN_SESSION), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SEND_POINTEREVENT_FOR_MOVE_DRAG), data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_KEYBOARD_SESSION_GRAVITY), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_CALLING_SESSION_ID), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_CUSTOM_DECOR_HEIGHT), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
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
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
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
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_PIP_RECT), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_PIP_CONTROL_STATUS), data, reply, option);
    ASSERT_EQ(ERR_INVALID_DATA, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_LAYOUT_FULL_SCREEN_CHANGE), data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->ProcessRemoteRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_DIALOG_SESSION_BACKGESTURE_ENABLE),
        data,
        reply,
        option);
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
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
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
    res = session_->HandleMarkProcessed(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->HandleSetGlobalMaximizeMode(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->HandleGetGlobalMaximizeMode(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->HandleNeedAvoid(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->HandleGetAvoidAreaByType(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->HandleSetAspectRatio(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->HandleUpdateWindowSceneAfterCustomAnimation(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    session_->HandleTransferAbilityResult(data, reply);
    res = session_->HandleTransferExtensionData(data, reply);
    ASSERT_EQ(ERR_INVALID_VALUE, res);
    res = session_->HandleNotifyExtensionDied(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->HandleNotifyExtensionTimeout(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->HandleGetStatusBarHeight(data, reply);
    ASSERT_EQ(ERR_NONE, res);
    res = session_->HandleSetSystemEnableDrag(data, reply);
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
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
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
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
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
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
    auto res = session_->HandleNotifyPiPWindowPrepareClose(data, reply);
    ASSERT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleUpdatePiPRect004
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleUpdatePiPRect004, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
    auto res = session_->HandleUpdatePiPRect(data, reply);
    ASSERT_EQ(ERR_NONE, res);
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
    auto controlType = WsPiPControlType::VIDEO_PLAY_PAUSE;
    auto status = WsPiPControlStatus::PLAY;
    data.WriteUint32(static_cast<uint32_t>(controlType));
    data.WriteInt32(static_cast<int32_t>(status));
    auto res = session_->HandleUpdatePiPControlStatus(data, reply);
    ASSERT_EQ(ERR_NONE, res);
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
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
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
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
    auto res = session_->HandleSendPointerEvenForMoveDrag(data, reply);
    ASSERT_EQ(ERR_INVALID_DATA, res);
}

/**
 * @tc.name: HandleUpdateRectChangeListenerRegistered008
 * @tc.desc: sessionStub sessionStubTest
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(SessionStubTest, HandleUpdateRectChangeListenerRegistered008, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(true);
    sptr<IRemoteObjectMocker> iRemoteObjectMocker = new IRemoteObjectMocker();
    auto res = session_->HandleUpdateRectChangeListenerRegistered(data, reply);
    ASSERT_EQ(ERR_NONE, res);
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
}
} // namespace Rosen
} // namespace OHOS
