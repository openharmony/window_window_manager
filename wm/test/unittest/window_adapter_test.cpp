/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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
#include "mock_RSIWindowAnimationController.h"

#include "remote_animation.h"
#include "starting_window.h"
#include "window_transition_info.h"
#include "window_property.h"
#include "window_agent.h"
#include "window_adapter.h"
#include "scene_board_judgement.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr uint32_t WINDOW_ID = 1000;
class WindowAdapterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WindowAdapterTest::SetUpTestCase()
{
}

void WindowAdapterTest::TearDownTestCase()
{
}

void WindowAdapterTest::SetUp()
{
}

void WindowAdapterTest::TearDown()
{
}

namespace {
/**
 * @tc.name: AddWindow
 * @tc.desc: WindowAdapter/AddWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, AddWindow, Function | SmallTest | Level2)
{
    sptr<WindowProperty> windowProperty = nullptr;
    WindowAdapter windowAdapter;

    windowAdapter.AddWindow(windowProperty);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: RemoveWindow
 * @tc.desc: WindowAdapter/RemoveWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, RemoveWindow, Function | SmallTest | Level2)
{
    uint32_t windowId = 0;
    bool isFromInnerkits = false;
    WindowAdapter windowAdapter;

    windowAdapter.RemoveWindow(windowId, isFromInnerkits);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: RequestFocus
 * @tc.desc: WindowAdapter/RequestFocus
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, RequestFocus, Function | SmallTest | Level2)
{
    uint32_t windowId = 0;
    WindowAdapter windowAdapter;

    windowAdapter.RequestFocus(windowId);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: GetUIContentRemoteObj
 * @tc.desc: WindowAdapter/GetUIContentRemoteObj
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, GetUIContentRemoteObj, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    windowAdapter.isProxyValid_ = true;
    windowAdapter.windowManagerServiceProxy_ = nullptr;
    sptr<IRemoteObject> remoteObj;
    auto ret = windowAdapter.GetUIContentRemoteObj(WINDOW_ID, remoteObj);
    ASSERT_EQ(ret, WMError::WM_ERROR_SAMGR);
}

/**
 * @tc.name: CheckWindowId
 * @tc.desc: WindowAdapter/CheckWindowId
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, CheckWindowId, Function | SmallTest | Level2)
{
    int32_t windowId = 0;
    int32_t pid = 0;
    WindowAdapter windowAdapter;

    windowAdapter.CheckWindowId(windowId, pid);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: SkipSnapshotForAppProcess
 * @tc.desc: WindowAdapter/SkipSnapshotForAppProcess
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, SkipSnapshotForAppProcess, Function | SmallTest | Level2)
{
    int32_t pid = 1000;
    bool skip = true;
    WindowAdapter windowAdapter;
    windowAdapter.SkipSnapshotForAppProcess(pid, skip);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: SetWindowAnimationController
 * @tc.desc: WindowAdapter/SetWindowAnimationController
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, SetWindowAnimationController, Function | SmallTest | Level2)
{
    sptr<RSIWindowAnimationController> controller = nullptr;
    WindowAdapter windowAdapter;

    windowAdapter.SetWindowAnimationController(controller);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: GetAvoidAreaByType
 * @tc.desc: WindowAdapter/GetAvoidAreaByType
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, GetAvoidAreaByType, Function | SmallTest | Level2)
{
    uint32_t windowId = 0;
    AvoidAreaType type = AvoidAreaType::TYPE_CUTOUT;
    AvoidArea avoidArea;
    WindowAdapter windowAdapter;

    windowAdapter.GetAvoidAreaByType(windowId, type, avoidArea);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: NotifyServerReadyToMoveOrDrag
 * @tc.desc: WindowAdapter/NotifyServerReadyToMoveOrDrag
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, NotifyServerReadyToMoveOrDrag, Function | SmallTest | Level2)
{
    uint32_t windowId = 0;
    sptr<WindowProperty> windowProperty = nullptr;
    sptr<MoveDragProperty> moveDragProperty = nullptr;
    WindowAdapter windowAdapter;

    windowAdapter.NotifyServerReadyToMoveOrDrag(windowId, windowProperty, moveDragProperty);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: ProcessPointDown
 * @tc.desc: WindowAdapter/ProcessPointDown
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, ProcessPointDown, Function | SmallTest | Level2)
{
    uint32_t windowId = 0;
    bool isPointDown = false;
    WindowAdapter windowAdapter;

    windowAdapter.ProcessPointDown(windowId, isPointDown);
    windowAdapter.ProcessPointUp(windowId);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: ToggleShownStateForAllAppWindows
 * @tc.desc: WindowAdapter/ToggleShownStateForAllAppWindows
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, ToggleShownStateForAllAppWindows, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;

    windowAdapter.ToggleShownStateForAllAppWindows();
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: GetSnapshotByWindowId
 * @tc.desc: WindowAdapter/GetSnapshotByWindowId
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, GetSnapshotByWindowId, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    int32_t persistentId = -1;
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    windowAdapter.GetSnapshotByWindowId(persistentId, pixelMap);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: InitWMSProxy
 * @tc.desc: WindowAdapter/InitWMSProxy
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, InitWMSProxy, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: GetUnreliableWindowInfo
 * @tc.desc: WindowAdapter/GetUnreliableWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, GetUnreliableWindowInfo, Function | SmallTest | Level2)
{
    int32_t windowId = 0;
    std::vector<sptr<UnreliableWindowInfo>> infos;
    WindowAdapter windowAdapter;
    windowAdapter.GetUnreliableWindowInfo(windowId, infos);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: DestroyAndDisconnectSpecificSession
 * @tc.desc: WindowAdapter/DestroyAndDisconnectSpecificSession
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, DestroyAndDisconnectSpecificSession, Function | SmallTest | Level2)
{
    uint32_t persistentId = 0;
    WindowAdapter windowAdapter;
    windowAdapter.DestroyAndDisconnectSpecificSession(persistentId);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: DestroyAndDisconnectSpecificSessionWithDetachCallback
 * @tc.desc: WindowAdapter/DestroyAndDisconnectSpecificSessionWithDetachCallback
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, DestroyAndDisconnectSpecificSessionWithDetachCallback, Function | SmallTest | Level2)
{
    uint32_t persistentId = 0;
    WindowAdapter windowAdapter;
    windowAdapter.DestroyAndDisconnectSpecificSessionWithDetachCallback(persistentId, nullptr);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: UpdateModalExtensionRect
 * @tc.desc: WindowAdapter/UpdateModalExtensionRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, UpdateModalExtensionRect, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    Rect rect { 1, 2, 3, 4 };
    sptr<IRemoteObject> token = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(token, nullptr);
    windowAdapter.UpdateModalExtensionRect(token, rect);
    windowAdapter.UpdateModalExtensionRect(nullptr, rect);
    auto ret = windowAdapter.InitWMSProxy();
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: ProcessModalExtensionPointDown
 * @tc.desc: WindowAdapter/ProcessModalExtensionPointDown
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, ProcessModalExtensionPointDown, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    sptr<IRemoteObject> token = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(token, nullptr);
    windowAdapter.ProcessModalExtensionPointDown(token, 0, 0);
    windowAdapter.ProcessModalExtensionPointDown(nullptr, 0, 0);
    auto ret = windowAdapter.InitWMSProxy();
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: AddExtensionWindowStageToSCB
 * @tc.desc: WindowAdapter/AddExtensionWindowStageToSCB
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, AddExtensionWindowStageToSCB, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    windowAdapter.AddExtensionWindowStageToSCB(nullptr, nullptr, 0);
    auto ret = windowAdapter.InitWMSProxy();
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: RemoveExtensionWindowStageFromSCB
 * @tc.desc: WindowAdapter/RemoveExtensionWindowStageFromSCB
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, RemoveExtensionWindowStageFromSCB, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    windowAdapter.RemoveExtensionWindowStageFromSCB(nullptr, nullptr);
    auto ret = windowAdapter.InitWMSProxy();
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: AddOrRemoveSecureSession
 * @tc.desc: WindowAdapter/AddOrRemoveSecureSession
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, AddOrRemoveSecureSession, Function | SmallTest | Level2)
{
    int32_t persistentId = 0;
    WindowAdapter windowAdapter;
    windowAdapter.AddOrRemoveSecureSession(persistentId, true);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: UpdateExtWindowFlags
 * @tc.desc: WindowAdapter/UpdateExtWindowFlags
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, UpdateExtWindowFlags, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    sptr<IRemoteObject> token = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(token, nullptr);
    windowAdapter.UpdateExtWindowFlags(token, 7, 7);
    windowAdapter.UpdateExtWindowFlags(nullptr, 7, 7);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: GetVisibilityWindowInfo
 * @tc.desc: WindowAdapter/GetVisibilityWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, GetVisibilityWindowInfo, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    std::vector<sptr<WindowVisibilityInfo>> infos;
    auto ret = windowAdapter.GetVisibilityWindowInfo(infos);
    windowAdapter.WindowManagerAndSessionRecover();
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: ReregisterWindowManagerAgent
 * @tc.desc: WindowAdapter/ReregisterWindowManagerAgent
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, ReregisterWindowManagerAgent, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    auto displayId = 0;
    ModeChangeHotZones hotZones;
    auto ret = windowAdapter.GetModeChangeHotZones(displayId, hotZones);
    windowAdapter.ReregisterWindowManagerAgent();
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: UpdateProperty
 * @tc.desc: WindowAdapter/UpdateProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, UpdateProperty, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    sptr<WindowProperty> windowProperty = sptr<WindowProperty>::MakeSptr();
    PropertyChangeAction action = PropertyChangeAction::ACTION_UPDATE_RECT;
    auto ret = windowAdapter.UpdateProperty(windowProperty, action);
    windowAdapter.OnUserSwitch();
    windowAdapter.ClearWindowAdapter();
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: SetWindowGravity
 * @tc.desc: WindowAdapter/SetWindowGravity
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, SetWindowGravity, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    windowAdapter.isProxyValid_ = true;
    windowAdapter.windowManagerServiceProxy_ = nullptr;
    uint32_t windowId = 0;
    WindowGravity gravity = WindowGravity::WINDOW_GRAVITY_FLOAT;
    uint32_t percent = 0;
    auto ret = windowAdapter.SetWindowGravity(windowId, gravity, percent);
    ASSERT_EQ(WMError::WM_ERROR_SAMGR, ret);
}

/**
 * @tc.name: NotifyWindowTransition
 * @tc.desc: WindowAdapter/NotifyWindowTransition
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, NotifyWindowTransition, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    windowAdapter.isProxyValid_ = true;
    windowAdapter.windowManagerServiceProxy_ = nullptr;
    sptr<WindowTransitionInfo> from = nullptr;
    sptr<WindowTransitionInfo> to = nullptr;
    auto ret = windowAdapter.NotifyWindowTransition(from, to);
    ASSERT_EQ(WMError::WM_ERROR_SAMGR, ret);
}

/**
 * @tc.name: RaiseToAppTop
 * @tc.desc: WindowAdapter/RaiseToAppTop
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, RaiseToAppTop, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    uint32_t windowId = 0;
    auto ret = windowAdapter.RaiseToAppTop(windowId);
    std::shared_ptr<MMI::KeyEvent> event = nullptr;
    windowAdapter.DispatchKeyEvent(windowId, event);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: GetSnapshot
 * @tc.desc: WindowAdapter/GetSnapshot
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, GetSnapshot, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    int32_t windowId = 0;
    auto ret = windowAdapter.GetSnapshot(windowId);
    ASSERT_EQ(nullptr, ret);
}

/**
 * @tc.name: GetWindowAnimationTargets
 * @tc.desc: WindowAdapter/GetWindowAnimationTargets
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, GetWindowAnimationTargets, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    std::vector<uint32_t> missionIds;
    std::vector<sptr<RSWindowAnimationTarget>> targets;
    auto ret = windowAdapter.GetWindowAnimationTargets(missionIds, targets);
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(WMError::WM_OK, ret);
    } else {
        ASSERT_EQ(WMError::WM_ERROR_NO_MEM, ret);
    }
}

/**
 * @tc.name: GetMaximizeMode
 * @tc.desc: WindowAdapter/GetMaximizeMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, GetMaximizeMode, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    windowAdapter.isProxyValid_ = true;
    windowAdapter.windowManagerServiceProxy_ = nullptr;
    auto ret = windowAdapter.GetMaximizeMode();
    ASSERT_EQ(MaximizeMode::MODE_FULL_FILL, ret);
}

/**
 * @tc.name: UpdateSessionAvoidAreaListener
 * @tc.desc: WindowAdapter/UpdateSessionAvoidAreaListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, UpdateSessionAvoidAreaListener, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    windowAdapter.isProxyValid_ = true;
    windowAdapter.windowManagerServiceProxy_ = nullptr;
    int32_t persistentId = 0;
    bool haveListener = true;
    auto ret = windowAdapter.UpdateSessionAvoidAreaListener(persistentId, haveListener);
    ASSERT_EQ(WMError::WM_DO_NOTHING, ret);
}

/**
 * @tc.name: UpdateSessionTouchOutsideListener
 * @tc.desc: WindowAdapter/UpdateSessionTouchOutsideListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, UpdateSessionTouchOutsideListener, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    windowAdapter.isProxyValid_ = true;
    windowAdapter.windowManagerServiceProxy_ = nullptr;
    int32_t persistentId = 0;
    bool haveListener = true;
    auto ret = windowAdapter.UpdateSessionTouchOutsideListener(persistentId, haveListener);
    ASSERT_EQ(WMError::WM_DO_NOTHING, ret);
}

/**
 * @tc.name: SetSessionGravity
 * @tc.desc: WindowAdapter/SetSessionGravity
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, SetSessionGravity, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    int32_t persistentId = 0;
    SessionGravity gravity = SessionGravity::SESSION_GRAVITY_FLOAT;
    uint32_t percent = 0;
    auto ret = windowAdapter.SetSessionGravity(persistentId, gravity, percent);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: BindDialogSessionTarget
 * @tc.desc: WindowAdapter/BindDialogSessionTarget
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, BindDialogSessionTarget, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    windowAdapter.isProxyValid_ = true;
    windowAdapter.windowManagerServiceProxy_ = nullptr;
    uint64_t persistentId = 0;
    sptr<IRemoteObject> targetToken = nullptr;
    auto ret = windowAdapter.BindDialogSessionTarget(persistentId, targetToken);
    ASSERT_EQ(WMError::WM_DO_NOTHING, ret);
}

/**
 * @tc.name: GetHostWindowRect
 * @tc.desc: WindowAdapter/GetHostWindowRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, GetHostWindowRect, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    int32_t hostWindowId = 0;
    Rect rect = {0, 0, 0, 0};
    auto ret = windowAdapter.GetHostWindowRect(hostWindowId, rect);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: GetWindowStyleType
 * @tc.desc: WindowAdapter/GetWindowStyleType
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, GetWindowStyleType, Function | SmallTest | Level2)
{
    WindowAdapter windowAdapter;
    WindowStyleType windowStyleType = Rosen::WindowStyleType::WINDOW_STYLE_DEFAULT;
    windowAdapter.GetWindowStyleType(windowStyleType);
    ASSERT_EQ(Rosen::WindowStyleType::WINDOW_STYLE_DEFAULT, windowStyleType);
}

/**
 * @tc.name: SetProcessWatermark
 * @tc.desc: WindowAdapter/SetProcessWatermark
 * @tc.type: FUNC
 */
HWTEST_F(WindowAdapterTest, SetProcessWatermark, Function | SmallTest | Level2)
{
    int32_t pid = 1000;
    const std::string watermarkName = "SetProcessWatermarkName";
    bool isEnabled = true;
    WindowAdapter windowAdapter;
    windowAdapter.SetProcessWatermark(pid, watermarkName, isEnabled);
    auto ret = windowAdapter.InitWMSProxy();
    ASSERT_EQ(true, ret);
}

}
}
}