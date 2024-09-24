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

#include "session/container/include/zidl/session_stage_stub.h"
#include "session/container/include/zidl/session_stage_ipc_interface_code.h"
#include <ipc_types.h>
#include <iremote_stub.h>

#include "mock/mock_session_stage.h"
#include <message_option.h>
#include <message_parcel.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "session_manager/include/zidl/scene_session_manager_interface.h"
#include "window_manager.h"
#include "window_manager_agent.h"
#include "ws_common.h"
#include "zidl/window_manager_agent_interface.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionStageStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<SessionStageStub> sessionStageStub_ = new SessionStageMocker();
};

void SessionStageStubTest::SetUpTestCase()
{
}

void SessionStageStubTest::TearDownTestCase()
{
}

void SessionStageStubTest::SetUp()
{
}

void SessionStageStubTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnRemoteRequest
 * @tc.desc: test function : OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, OnRemoteRequest, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        ISceneSessionManager::SceneSessionManagerMessage::TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(22, sessionStageStub_->OnRemoteRequest(code, data, reply, option));

    MessageParcel errorData;
    errorData.WriteInterfaceToken(u"OHOS.ISessionStage.TEST");
    code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_SET_ACTIVE);
    ASSERT_EQ(ERR_TRANSACTION_FAILED, sessionStageStub_->OnRemoteRequest(code, errorData, reply, option));
}

/**
 * @tc.name: HandleSetActive
 * @tc.desc: test function : HandleSetActive
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleSetActive, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteBool(false);
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_SET_ACTIVE);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleUpdateRect
 * @tc.desc: test function : HandleUpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleUpdateRect, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteUint32(1);
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_SIZE_CHANGE);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleUpdateSessionViewportConfig
 * @tc.desc: test function : HandleUpdateSessionViewportConfig
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleUpdateSessionViewportConfig, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteBool(false);
    data.WriteFloat(1.0f);
    data.WriteUint64(0);
    data.WriteInt32(0);
    data.WriteUint32(0);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->HandleUpdateSessionViewportConfig(data, reply));
}

/**
 * @tc.name: HandleUpdateDensity01
 * @tc.desc: test function : HandleUpdateDensity
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleUpdateDensity01, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_DENSITY_CHANGE);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleUpdateDensity02
 * @tc.desc: test function : HandleUpdateDensity
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleUpdateDensity02, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->HandleUpdateDensity(data, reply));
}

/**
 * @tc.name: HandleUpdateOrientation
 * @tc.desc: test function : HandleUpdateOrientation
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleUpdateOrientation, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_ORIENTATION_CHANGE);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleBackEventInner
 * @tc.desc: test function : HandleBackEventInner
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleBackEventInner, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteBool(false);
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_HANDLE_BACK_EVENT);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleNotifyDestroy
 * @tc.desc: test function : HandleNotifyDestroy
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleNotifyDestroy, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_DESTROY);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleUpdateFocus
 * @tc.desc: test function : HandleUpdateFocus
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleUpdateFocus, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteBool(false);
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_FOCUS_CHANGE);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleNotifyTransferComponentData
 * @tc.desc: test function : HandleNotifyTransferComponentData
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleNotifyTransferComponentData, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteUint32(1);
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_TRANSFER_COMPONENT_DATA);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_VALUE, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleNotifyTransferComponentDataSync
 * @tc.desc: test function : HandleNotifyTransferComponentDataSync
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleNotifyTransferComponentDataSync, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteUint32(1);
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_TRANSFER_COMPONENT_DATA_SYNC);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(static_cast<int>(WSErrorCode::WS_ERROR_TRANSFER_DATA_FAILED),
        sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleNotifyOccupiedAreaChange
 * @tc.desc: test function : HandleNotifyOccupiedAreaChange
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleNotifyOccupiedAreaChange, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteUint32(1);
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_OCCUPIED_AREA_CHANGE_INFO);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_VALUE, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleUpdateAvoidArea
 * @tc.desc: test function : HandleUpdateAvoidArea
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleUpdateAvoidArea, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteUint32(1);
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_UPDATE_AVOID_AREA);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_VALUE, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleNotifyScreenshot
 * @tc.desc: test function : HandleNotifyScreenshot
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleNotifyScreenshot, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_SCREEN_SHOT);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleDumpSessionElementInfo
 * @tc.desc: test function : HandleDumpSessionElementInfo
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleDumpSessionElementInfo, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    std::vector<std::string> params;
    params.push_back("test1");
    params.push_back("test2");
    data.WriteStringVector(params);
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_DUMP_SESSSION_ELEMENT_INFO);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleNotifyTouchOutside
 * @tc.desc: test function : HandleNotifyTouchOutside
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleNotifyTouchOutside, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_TOUCH_OUTSIDE);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleUpdateWindowMode
 * @tc.desc: test function : HandleUpdateWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleUpdateWindowMode, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteUint32(1);
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_WINDOW_MODE_CHANGE);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleNotifyForegroundInteractiveStatus
 * @tc.desc: test function : HandleNotifyForegroundInteractiveStatus
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleNotifyForegroundInteractiveStatus, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteBool(false);
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_FOREGROUND_INTERACTIVE_STATUS);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleUpdateMaximizeMode
 * @tc.desc: test function : HandleUpdateMaximizeMode
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleUpdateMaximizeMode, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_MAXIMIZE_MODE_CHANGE);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleNotifyCloseExistPipWindow
 * @tc.desc: test function : HandleNotifyCloseExistPipWindow
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleNotifyCloseExistPipWindow, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_CLOSE_EXIST_PIP_WINDOW);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleNotifySessionForeground
 * @tc.desc: test function : HandleNotifySessionForeground
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleNotifySessionForeground, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_SESSION_FOREGROUND);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteUint32(1);
    data.WriteBool(false);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleNotifySessionBackground
 * @tc.desc: test function : HandleNotifySessionBackground
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleNotifySessionBackground, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_SESSION_BACKGROUND);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteUint32(1);
    data.WriteBool(false);
    data.WriteBool(false);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleUpdateTitleInTargetPos
 * @tc.desc: test function : HandleUpdateTitleInTargetPos
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleUpdateTitleInTargetPos, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_TITLE_POSITION_CHANGE);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleNotifyDensityFollowHost
 * @tc.desc: test function : HandleNotifyDensityFollowHost
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleNotifyDensityFollowHost, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_DENSITY_FOLLOW_HOST);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleNotifyWindowVisibilityChange
 * @tc.desc: test function : HandleNotifyWindowVisibilityChange
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleNotifyWindowVisibilityChange, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_WINDOW_VISIBILITY_CHANGE);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteBool(true);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleNotifyTransformChange
 * @tc.desc: test function : HandleNotifyTransformChange
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleNotifyTransformChange, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_TRANSFORM_CHANGE);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteBool(true);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleNotifyDialogStateChange
 * @tc.desc: test function : HandleNotifyDialogStateChange
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleNotifyDialogStateChange, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_DIALOG_STATE_CHANGE);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteBool(true);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleSetPipActionEvent
 * @tc.desc: test function : HandleSetPipActionEvent
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleSetPipActionEvent, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_SET_PIP_ACTION_EVENT);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteString("str");
    data.WriteInt32(1);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleSetPiPControlEvent
 * @tc.desc: test function : HandleSetPiPControlEvent
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleSetPiPControlEvent, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_SET_PIP_CONTROL_EVENT);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    auto controlType = WsPiPControlType::VIDEO_PLAY_PAUSE;
    auto status = WsPiPControlStatus::PLAY;
    data.WriteUint32(static_cast<uint32_t>(controlType));
    data.WriteInt32(static_cast<int32_t>(status));
    ASSERT_TRUE(sessionStageStub_ != nullptr);
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleUpdateDisplayId
 * @tc.desc: test function : HandleUpdateDisplayId
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleUpdateDisplayId, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_DISPLAYID_CHANGE);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteUint64(2);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleNotifyDisplayMove
 * @tc.desc: test function : HandleNotifyDisplayMove
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleNotifyDisplayMove, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_DISPLAY_MOVE);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteUint64(0); // from
    data.WriteUint64(1); // to
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleSwitchFreeMultiWindow
 * @tc.desc: test function : HandleSwitchFreeMultiWindow
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleSwitchFreeMultiWindow, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_SWITCH_FREEMULTIWINDOW);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteBool(true); // enable
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleGetUIContentRemoteObj
 * @tc.desc: test function : HandleGetUIContentRemoteObj
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleGetUIContentRemoteObj, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_GET_UI_CONTENT_REMOTE_OBJ);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    ASSERT_NE(sessionStageStub_, nullptr);
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleNotifyKeyboardPanelInfoChange
 * @tc.desc: test function : HandleNotifyKeyboardPanelInfoChange
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleNotifyKeyboardPanelInfoChange, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_KEYBOARD_INFO_CHANGE);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    KeyboardPanelInfo* keyboardPanelInfo = new KeyboardPanelInfo();
    data.WriteParcelable(keyboardPanelInfo);
    ASSERT_NE(sessionStageStub_, nullptr);
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
    delete keyboardPanelInfo;
}

/**
 * @tc.name: HandleCompatibleFullScreenRecover
 * @tc.desc: test function : HandleCompatibleFullScreenRecover
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleCompatibleFullScreenRecover, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_COMPATIBLE_FULLSCREEN_RECOVER);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    ASSERT_NE(sessionStageStub_, nullptr);
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleCompatibleFullScreenMinimize
 * @tc.desc: test function : HandleCompatibleFullScreenMinimize
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleCompatibleFullScreenMinimize, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_COMPATIBLE_FULLSCREEN_MINIMIZE);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    ASSERT_NE(sessionStageStub_, nullptr);
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleCompatibleFullScreenClose
 * @tc.desc: test function : HandleCompatibleFullScreenClose
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleCompatibleFullScreenClose, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_COMPATIBLE_FULLSCREEN_CLOSE);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    ASSERT_NE(sessionStageStub_, nullptr);
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleSetUniqueVirtualPixelRatio
 * @tc.desc: test function : HandleSetUniqueVirtualPixelRatio
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleSetUniqueVirtualPixelRatio, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_DENSITY_UNIQUE);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteBool(true);
    data.WriteFloat(3.25f);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleNotifySessionFullScreen
 * @tc.desc: test function : HandleNotifySessionFullScreen
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleNotifySessionFullScreen, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_SESSION_FULLSCREEN);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteBool(true);
    ASSERT_TRUE((sessionStageStub_ != nullptr));
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
    data.WriteBool(false);
    ASSERT_EQ(0, sessionStageStub_->HandleNotifySessionFullScreen(data, reply));
}

/**
 * @tc.name: HandleNotifyCompatibleModeEnableInPad
 * @tc.desc: test function : HandleNotifyCompatibleModeEnableInPad
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleNotifyCompatibleModeEnableInPad, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_COMPATIBLE_MODE_ENABLE);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteBool(true);
    ASSERT_NE(sessionStageStub_, nullptr);
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleNotifyDumpInfo
 * @tc.desc: test function : HandleNotifyDumpInfo
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleNotifyDumpInfo, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_DUMP_INFO);
    std::vector<std::string> params;
    std::vector<std::string> info;
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteStringVector(params);
    ASSERT_TRUE(sessionStageStub_ != nullptr);
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleNotifyDumpInfo
 * @tc.desc: test function : HandleNotifyDumpInfo
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageStubTest, HandleSetSplitButtonVisible, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_SET_SPLIT_BUTTON_VISIBLE);
    data.WriteInterfaceToken(SessionStageStub::GetDescriptor());
    data.WriteBool(true);
    ASSERT_TRUE(sessionStageStub_ != nullptr);
    ASSERT_EQ(0, sessionStageStub_->OnRemoteRequest(code, data, reply, option));
    data.WriteBool(false);
    ASSERT_EQ(0, sessionStageStub_->HandleSetSplitButtonVisible(data, reply));
}
}
}
}