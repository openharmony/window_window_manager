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

#include "session/container/include/zidl/session_stage_proxy.h"
#include "iremote_object_mocker.h"
#include <gtest/gtest.h>
#include "proto.h"
#include "string_wrapper.h"
#include "util.h"
#include "window_manager.h"
#include "window_manager_hilog.h"
#include "wm_common.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionStageProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SessionStageProxy> sessionStage_ = new SessionStageProxy(iRemoteObjectMocker);
};

void SessionStageProxyTest::SetUpTestCase()
{
}

void SessionStageProxyTest::TearDownTestCase()
{
}

void SessionStageProxyTest::SetUp()
{
}

void SessionStageProxyTest::TearDown()
{
}

namespace {
/**
 * @tc.name: SetActive
 * @tc.desc: test function : SetActive
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, SetActive, Function | SmallTest | Level1)
{
    bool active = false;
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSError res = sessionStage_->SetActive(active);
    ASSERT_EQ(WSError::WS_OK, res);
    active = true;
    ASSERT_TRUE((sessionStage_ != nullptr));
    res = sessionStage_->SetActive(active);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: UpdateDisplayId
 * @tc.desc: test function : UpdateDisplayId
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, UpdateDisplayId, Function | SmallTest | Level1)
{
    uint64_t displayID = 0;
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSError res = sessionStage_->UpdateDisplayId(displayID);
    ASSERT_EQ(WSError::WS_OK, res);
    displayID = 1;
    res = sessionStage_->UpdateDisplayId(displayID);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: UpdateRect
 * @tc.desc: test function : UpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, UpdateRect, Function | SmallTest | Level1)
{
    WSRect rect;
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSError res = sessionStage_->UpdateRect(rect, reason);
    ASSERT_EQ(WSError::WS_OK, res);
    std::shared_ptr<RSTransaction> rsTransaction = std::make_shared<RSTransaction>();
    res = sessionStage_->UpdateRect(rect, reason, rsTransaction);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: UpdateDensity
 * @tc.desc: test function : UpdateDensity
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, UpdateDensity, Function | SmallTest | Level1)
{
    ASSERT_TRUE((sessionStage_ != nullptr));
    sessionStage_->UpdateDensity();
}

/**
 * @tc.name: UpdateOrientation
 * @tc.desc: test function : UpdateOrientation
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, UpdateOrientation, Function | SmallTest | Level1)
{
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSError res = sessionStage_->UpdateOrientation();
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: HandleBackEvent
 * @tc.desc: test function : HandleBackEvent
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, HandleBackEvent, Function | SmallTest | Level1)
{
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSError res = sessionStage_->HandleBackEvent();
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: SwitchFreeMultiWindow
 * @tc.desc: test function : SwitchFreeMultiWindow
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, SwitchFreeMultiWindow, Function | SmallTest | Level1)
{
    ASSERT_TRUE((sessionStage_ != nullptr));
    bool enable = true;
    WSError res = sessionStage_->SwitchFreeMultiWindow(enable);
    ASSERT_EQ(WSError::WS_OK, res);
    enable = false;
    res = sessionStage_->SwitchFreeMultiWindow(enable);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: GetUIContentRemoteObj
 * @tc.desc: test function : GetUIContentRemoteObj
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, GetUIContentRemoteObj, Function | SmallTest | Level1)
{
    ASSERT_NE(sessionStage_, nullptr);
    sptr<IRemoteObject> remoteObj;
    WSError res = sessionStage_->GetUIContentRemoteObj(remoteObj);
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, res);
}

/**
 * @tc.name: MarkProcessed
 * @tc.desc: test function : MarkProcessed
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, MarkProcessed, Function | SmallTest | Level1)
{
    int32_t eventId = 1;
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSError res = sessionStage_->MarkProcessed(eventId);
    ASSERT_EQ(WSError::WS_DO_NOTHING, res);
}

/**
 * @tc.name: UpdateFocus
 * @tc.desc: test function : UpdateFocus
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, UpdateFocus, Function | SmallTest | Level1)
{
    bool focus = false;
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSError res = sessionStage_->UpdateFocus(focus);
    ASSERT_EQ(WSError::WS_OK, res);
    focus = true;
    ASSERT_TRUE((sessionStage_ != nullptr));
    res = sessionStage_->UpdateFocus(focus);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: NotifyDestroy
 * @tc.desc: test function : NotifyDestroy
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, NotifyDestroy, Function | SmallTest | Level1)
{
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSError res = sessionStage_->NotifyDestroy();
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: NotifyCloseExistPipWindow
 * @tc.desc: test function : NotifyCloseExistPipWindow
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, NotifyCloseExistPipWindow, Function | SmallTest | Level1)
{
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSError res = sessionStage_->NotifyCloseExistPipWindow();
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: NotifyTransferComponentData
 * @tc.desc: test function : NotifyTransferComponentData
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, NotifyTransferComponentData, Function | SmallTest | Level1)
{
    AAFwk::WantParams wantParams;
    std::string testValue = "testValue";
    wantParams.SetParam("keyStr", OHOS::AAFwk::String::Box(testValue));
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSError res = sessionStage_->NotifyTransferComponentData(wantParams);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: NotifyTransferComponentDataSync
 * @tc.desc: test function : NotifyTransferComponentDataSync
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, NotifyTransferComponentDataSync, Function | SmallTest | Level1)
{
    AAFwk::WantParams wantParams;
    std::string testValue = "testValue";
    wantParams.SetParam("keyStr", OHOS::AAFwk::String::Box(testValue));
    AAFwk::WantParams reWantParams;
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSErrorCode res = sessionStage_->NotifyTransferComponentDataSync(wantParams, reWantParams);
    ASSERT_EQ(WSErrorCode::WS_ERROR_TRANSFER_DATA_FAILED, res);
}

/**
 * @tc.name: NotifyOccupiedAreaChangeInfo
 * @tc.desc: test function : NotifyOccupiedAreaChangeInfo
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, NotifyOccupiedAreaChangeInfo, Function | SmallTest | Level1)
{
    sptr<OccupiedAreaChangeInfo> info = new OccupiedAreaChangeInfo();
    ASSERT_TRUE((sessionStage_ != nullptr));
    sessionStage_->NotifyOccupiedAreaChangeInfo(info);
}

/**
 * @tc.name: UpdateAvoidArea
 * @tc.desc: test function : UpdateAvoidArea
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, UpdateAvoidArea, Function | SmallTest | Level1)
{
    sptr<AvoidArea> avoidArea = new AvoidArea();
    AvoidAreaType type = AvoidAreaType::TYPE_SYSTEM;
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSError res = sessionStage_->UpdateAvoidArea(avoidArea, type);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: DumpSessionElementInfo
 * @tc.desc: test function : DumpSessionElementInfo
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, DumpSessionElementInfo, Function | SmallTest | Level1)
{
    std::vector<std::string> params;
    params.push_back("test1");
    params.push_back("test2");
    ASSERT_TRUE((sessionStage_ != nullptr));
    sessionStage_->DumpSessionElementInfo(params);
}

/**
 * @tc.name: NotifyScreenshot
 * @tc.desc: test function : NotifyScreenshot
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, NotifyScreenshot, Function | SmallTest | Level1)
{
    ASSERT_TRUE((sessionStage_ != nullptr));
    sessionStage_->NotifyScreenshot();
}

/**
 * @tc.name: NotifyTouchOutside
 * @tc.desc: test function : NotifyTouchOutside
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, NotifyTouchOutside, Function | SmallTest | Level1)
{
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSError res = sessionStage_->NotifyTouchOutside();
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: UpdateWindowMode
 * @tc.desc: test function : UpdateWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, UpdateWindowMode, Function | SmallTest | Level1)
{
    WindowMode mode = WindowMode::WINDOW_MODE_UNDEFINED;
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSError res = sessionStage_->UpdateWindowMode(mode);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: NotifySessionFullScreen
 * @tc.desc: test function : NotifySessionFullScreen
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, NotifySessionFullScreen, Function | SmallTest | Level1)
{
    bool fullScreen = true;
    ASSERT_TRUE((sessionStage_ != nullptr));
    sessionStage_->NotifySessionFullScreen(fullScreen);
    fullScreen = false;
    sessionStage_->NotifySessionFullScreen(fullScreen);
}

/**
 * @tc.name: NotifyForegroundInteractiveStatus
 * @tc.desc: test function : NotifyForegroundInteractiveStatus
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, NotifyForegroundInteractiveStatus, Function | SmallTest | Level1)
{
    bool interactive = true;
    ASSERT_TRUE((sessionStage_ != nullptr));
    sessionStage_->NotifyForegroundInteractiveStatus(interactive);
    interactive = false;
    sessionStage_->NotifyForegroundInteractiveStatus(interactive);
}

/**
 * @tc.name: UpdateMaximizeMode
 * @tc.desc: test function : UpdateMaximizeMode
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, UpdateMaximizeMode, Function | SmallTest | Level1)
{
    MaximizeMode mode = MaximizeMode::MODE_FULL_FILL;
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSError res = sessionStage_->UpdateMaximizeMode(mode);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: NotifySessionForeground
 * @tc.desc: test function : NotifySessionForeground
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, NotifySessionForeground, Function | SmallTest | Level1)
{
    uint32_t reason = 1;
    bool withAnimation = true;
    ASSERT_TRUE((sessionStage_ != nullptr));
    sessionStage_->NotifySessionForeground(reason, withAnimation);
}

/**
 * @tc.name: NotifySessionBackground
 * @tc.desc: test function : NotifySessionBackground
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, NotifySessionBackground, Function | SmallTest | Level1)
{
    uint32_t reason = 1;
    bool withAnimation = true;
    bool isFromInnerkits = true;
    ASSERT_TRUE((sessionStage_ != nullptr));
    sessionStage_->NotifySessionBackground(reason, withAnimation, isFromInnerkits);
}

/**
 * @tc.name: UpdateTitleInTargetPos
 * @tc.desc: test function : UpdateTitleInTargetPos
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, UpdateTitleInTargetPos, Function | SmallTest | Level1)
{
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSError res = sessionStage_->UpdateTitleInTargetPos(true, 1);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: NotifyTransformChange
 * @tc.desc: test function : NotifyTransformChange
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, NotifyTransformChange, Function | SmallTest | Level1)
{
    Transform transform;
    ASSERT_TRUE((sessionStage_ != nullptr));
    sessionStage_->NotifyTransformChange(transform);
}

/**
 * @tc.name: NotifyWindowVisibility
 * @tc.desc: test function : NotifyWindowVisibility
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, NotifyWindowVisibility, Function | SmallTest | Level1)
{
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSError res = sessionStage_->NotifyWindowVisibility(true);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: NotifyDensityFollowHost
 * @tc.desc: test function : NotifyDensityFollowHost
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, NotifyDensityFollowHost, Function | SmallTest | Level1)
{
    ASSERT_TRUE((sessionStage_ != nullptr));
    ASSERT_EQ(WSError::WS_OK, sessionStage_->NotifyDensityFollowHost(true, 1.0f));
}

/**
 * @tc.name: NotifyDialogStateChange
 * @tc.desc: test function : NotifyDialogStateChange
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, NotifyDialogStateChange, Function | SmallTest | Level1)
{
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSError res = sessionStage_->NotifyDialogStateChange(true);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: SetPipActionEvent
 * @tc.desc: test function : SetPipActionEvent
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, SetPipActionEvent, Function | SmallTest | Level1)
{
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSError res = sessionStage_->SetPipActionEvent("close", 0);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: SetPiPControlEvent
 * @tc.desc: test function : SetPiPControlEvent
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, SetPiPControlEvent, Function | SmallTest | Level1)
{
    ASSERT_TRUE(sessionStage_ != nullptr);
    auto controlType = WsPiPControlType::VIDEO_PLAY_PAUSE;
    auto status = WsPiPControlStatus::PLAY;
    WSError res = sessionStage_->SetPiPControlEvent(controlType, status);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: NotifyDisplayMove
 * @tc.desc: test function : NotifyDisplayMove
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, NotifyDisplayMove, Function | SmallTest | Level1)
{
    ASSERT_TRUE(sessionStage_ != nullptr);
    DisplayId from = 0;
    DisplayId to = 1;
    sessionStage_->NotifyDisplayMove(from, to);
}

/**
 * @tc.name: NotifyKeyboardPanelInfoChange
 * @tc.desc: test function : NotifyKeyboardPanelInfoChange
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, NotifyKeyboardPanelInfoChange, Function | SmallTest | Level1)
{
    ASSERT_TRUE(sessionStage_ != nullptr);
    KeyboardPanelInfo keyboardPanelInfo;
    sessionStage_->NotifyKeyboardPanelInfoChange(keyboardPanelInfo);
}

/**
 * @tc.name: CompatibleFullScreenRecover
 * @tc.desc: test function : CompatibleFullScreenRecover
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, CompatibleFullScreenRecover, Function | SmallTest | Level1)
{
    ASSERT_TRUE(sessionStage_ != nullptr);
    WSError res = sessionStage_->CompatibleFullScreenRecover();
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: CompatibleFullScreenMinimize
 * @tc.desc: test function : CompatibleFullScreenMinimize
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, CompatibleFullScreenMinimize, Function | SmallTest | Level1)
{
    ASSERT_TRUE(sessionStage_ != nullptr);
    WSError res = sessionStage_->CompatibleFullScreenMinimize();
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: CompatibleFullScreenClose
 * @tc.desc: test function : CompatibleFullScreenClose
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, CompatibleFullScreenClose, Function | SmallTest | Level1)
{
    ASSERT_TRUE(sessionStage_ != nullptr);
    WSError res = sessionStage_->CompatibleFullScreenClose();
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: SetUniqueVirtualPixelRatio
 * @tc.desc: test function : SetUniqueVirtualPixelRatio
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyTest, SetUniqueVirtualPixelRatio, Function | SmallTest | Level1)
{
    ASSERT_TRUE(sessionStage_ != nullptr);
    sessionStage_->SetUniqueVirtualPixelRatio(true, 0.1f);
}
}
}
}