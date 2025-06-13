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

#include "display_manager.h"
#include "session/host/include/keyboard_session.h"
#include <gtest/gtest.h>
#include <ui/rs_surface_node.h>

#include "interfaces/include/ws_common.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_keyboard_session.h"
#include "session/host/include/session.h"
#include "screen_session_manager_client/include/screen_session_manager_client.h"
#include "session/host/include/scene_session.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_US = 100000; // 100ms
}
class KeyboardSessionTest3 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    sptr<KeyboardSession> GetKeyboardSession(const std::string& abilityName, const std::string& bundleName);
    sptr<SceneSession> GetSceneSession(const std::string& abilityName, const std::string& bundleName);
};

void KeyboardSessionTest3::SetUpTestCase() {}

void KeyboardSessionTest3::TearDownTestCase() {}

void KeyboardSessionTest3::SetUp() {}

void KeyboardSessionTest3::TearDown() {}

sptr<KeyboardSession> KeyboardSessionTest3::GetKeyboardSession(const std::string& abilityName,
                                                               const std::string& bundleName)
{
    SessionInfo info;
    info.abilityName_ = abilityName;
    info.bundleName_ = bundleName;
    sptr<SceneSession::SpecificSessionCallback> specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);

    sptr<WindowSessionProperty> keyboardProperty = sptr<WindowSessionProperty>::MakeSptr();
    EXPECT_NE(keyboardProperty, nullptr);
    keyboardProperty->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    keyboardSession->SetSessionProperty(keyboardProperty);

    return keyboardSession;
}

sptr<SceneSession> KeyboardSessionTest3::GetSceneSession(const std::string& abilityName, const std::string& bundleName)
{
    SessionInfo info;
    info.abilityName_ = abilityName;
    info.bundleName_ = bundleName;
    sptr<SceneSession::SpecificSessionCallback> specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCb, nullptr);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCb);

    return sceneSession;
}

namespace {
/**
 * @tc.name: IsVisibleNotBackground
 * @tc.desc: test IsVisibleNotBackground
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, IsVisibleNotBackground, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "IsVisibleNotBackground";
    info.bundleName_ = "IsVisibleNotBackground";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    keyboardSession->isVisible_ = false;
    EXPECT_EQ(false, keyboardSession->IsVisibleNotBackground());
    keyboardSession->isVisible_ = true;
    EXPECT_EQ(true, keyboardSession->IsVisibleNotBackground());
}

/**
 * @tc.name: GetRSTransaction01
 * @tc.desc: test function: GetRSTransaction
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, GetRSTransaction01, TestSize.Level1)
{
    auto keyboardSession = GetKeyboardSession("GetRSTransaction01", "GetRSTransaction01");
    ASSERT_NE(keyboardSession, nullptr);

    auto rsTransaction = keyboardSession->GetRSTransaction();
    ASSERT_EQ(rsTransaction, nullptr);
}

/**
 * @tc.name: GetSessionScreenName01
 * @tc.desc: test function: GetSessionScreenName
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, GetSessionScreenName01, TestSize.Level1)
{
    auto keyboardSession = GetKeyboardSession("GetSessionScreenName01", "GetSessionScreenName01");
    ASSERT_NE(keyboardSession, nullptr);
    keyboardSession->property_ = nullptr;
    auto resultStr = keyboardSession->GetSessionScreenName();
    ASSERT_EQ(resultStr, "");

    sptr<WindowSessionProperty> keyboardProperty = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(keyboardProperty, nullptr);
    keyboardSession->property_ = keyboardProperty;
    keyboardSession->property_->displayId_ = 100;

    sptr<ScreenSession> screenSession = sptr<ScreenSession>::MakeSptr();
    ASSERT_NE(screenSession, nullptr);
    screenSession->name_ = "testScreenSession";
    screenSession->screenId_ = 100;
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.emplace(100, screenSession);

    resultStr = keyboardSession->GetSessionScreenName();
    ASSERT_EQ(resultStr, screenSession->name_);
}

/**
 * @tc.name: UseFocusIdIfCallingSessionIdInvalid01
 * @tc.desc: test function: UseFocusIdIfCallingSessionIdInvalid
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, UseFocusIdIfCallingSessionIdInvalid01, TestSize.Level1)
{
    auto keyboardSession =
        GetKeyboardSession("UseFocusIdIfCallingSessionIdInvalid01", "UseFocusIdIfCallingSessionIdInvalid01");
    ASSERT_NE(keyboardSession, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCallback =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    ASSERT_NE(keyboardCallback, nullptr);
    keyboardSession->keyboardCallback_ = keyboardCallback;
    sptr<SceneSession> sceneSession = GetSceneSession("TestSceneSession", "TestSceneSession");
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->persistentId_ = 100;
    keyboardSession->keyboardCallback_->onGetSceneSession =
        [sceneSession](uint32_t callingSessionId) -> sptr<SceneSession> {
        if (sceneSession->persistentId_ != callingSessionId) {
            return nullptr;
        }
        return sceneSession;
    };

    keyboardSession->GetSessionProperty()->SetCallingSessionId(100);
    keyboardSession->UseFocusIdIfCallingSessionIdInvalid();
    auto resultId = keyboardSession->GetCallingSessionId();
    ASSERT_EQ(resultId, 100);

    keyboardSession->GetSessionProperty()->SetCallingSessionId(101);
    keyboardSession->keyboardCallback_->onGetFocusedSessionId = []() -> int32_t { return 100; };
    keyboardSession->UseFocusIdIfCallingSessionIdInvalid();
    resultId = keyboardSession->GetCallingSessionId();
    ASSERT_EQ(resultId, 100);
}

/**
 * @tc.name: MoveAndResizeKeyboard01
 * @tc.desc: test function: MoveAndResizeKeyboard
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, MoveAndResizeKeyboard01, TestSize.Level1)
{
    auto keyboardSession = GetKeyboardSession("MoveAndResizeKeyboard01", "MoveAndResizeKeyboard01");
    ASSERT_NE(keyboardSession, nullptr);

    KeyboardLayoutParams param;
    param.LandscapeKeyboardRect_ = { 100, 100, 100, 200 };
    param.PortraitKeyboardRect_ = { 200, 200, 200, 100 };

    // branch SESSION_GRAVITY_BOTTOM
    param.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;
    uint32_t screenWidth = 0;
    uint32_t screenHeight = 0;
    auto defaultDisplayInfo = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_NE(defaultDisplayInfo, nullptr);
    screenWidth = static_cast<uint32_t>(defaultDisplayInfo->GetWidth());
    screenHeight = static_cast<uint32_t>(defaultDisplayInfo->GetHeight());
    const Rect& expectRect = screenWidth > screenHeight ? param.LandscapeKeyboardRect_ : param.PortraitKeyboardRect_;
    keyboardSession->MoveAndResizeKeyboard(param, nullptr, false);
    ASSERT_EQ(keyboardSession->property_->requestRect_, expectRect);
}

/**
 * @tc.name: SetSurfaceBounds01
 * @tc.desc: test function: SetSurfaceBounds
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, SetSurfaceBounds01, TestSize.Level1)
{
    auto keyboardSession = GetKeyboardSession("SetSurfaceBounds01", "SetSurfaceBounds01");
    ASSERT_NE(keyboardSession, nullptr);

    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    keyboardSession->surfaceNode_ = nullptr;
    WSRect preRect = { 20, 20, 800, 800 };
    WSRect rect = { 30, 30, 900, 900 };
    keyboardSession->SetSessionRect(preRect);
    keyboardSession->SetSurfaceBounds(rect, false);

    keyboardSession->surfaceNode_ = surfaceNode;
    keyboardSession->SetSurfaceBounds(rect, false);
    EXPECT_EQ(preRect, keyboardSession->GetSessionRect());
}

/**
 * @tc.name: NotifySessionRectChange01
 * @tc.desc: test function: NotifySessionRectChange
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, NotifySessionRectChange01, TestSize.Level1)
{
    auto keyboardSession = GetKeyboardSession("NotifySessionRectChange01", "NotifySessionRectChange01");
    ASSERT_NE(keyboardSession, nullptr);

    WSRect rect = { 50, 50, 900, 900 };
    keyboardSession->NotifySessionRectChange(rect, SizeChangeReason::DRAG_END, -1);
    keyboardSession->NotifySessionRectChange(rect, SizeChangeReason::DRAG_END, 11);
    keyboardSession->sessionRectChangeFunc_ = [](const WSRect& rect,
                                                 SizeChangeReason reason,
                                                 DisplayId displayId,
                                                 const RectAnimationConfig& rectAnimationConfig) { return; };
    keyboardSession->NotifySessionRectChange(rect, SizeChangeReason::DRAG_END, -1);
    keyboardSession->NotifySessionRectChange(rect, SizeChangeReason::DRAG_END, 11);
}

/**
 * @tc.name: UpdateSizeChangeReason01
 * @tc.desc: test function: UpdateSizeChangeReason
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, UpdateSizeChangeReason01, TestSize.Level1)
{
    auto keyboardSession = GetKeyboardSession("UpdateSizeChangeReason01", "UpdateSizeChangeReason01");
    ASSERT_NE(keyboardSession, nullptr);

    keyboardSession->UpdateSizeChangeReason(SizeChangeReason::DRAG_END);
    ASSERT_EQ(keyboardSession->reason_, SizeChangeReason::DRAG_END);
    ASSERT_EQ(WSError::WS_OK, keyboardSession->UpdateSizeChangeReason(SizeChangeReason::UNDEFINED));
}

/**
 * @tc.name: RecalculatePanelRectForAvoidArea
 * @tc.desc: test function: RecalculatePanelRectForAvoidArea
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, RecalculatePanelRectForAvoidArea, TestSize.Level1)
{
    auto keyboardSession = GetKeyboardSession("RecalculatePanelRectForAvoidArea", "RecalculatePanelRectForAvoidArea");

    // if landscapeAvoidHeight_ or portraitAvoidHeight_ < 0
    WSRect panelRect = { 0, 0, 0, 0 };
    KeyboardLayoutParams params;
    keyboardSession->GetSessionProperty()->SetKeyboardLayoutParams(params);
    keyboardSession->RecalculatePanelRectForAvoidArea(panelRect);
    EXPECT_EQ(panelRect.height_, 0);
    params.landscapeAvoidHeight_ = 1;
    params.portraitAvoidHeight_ = -1;
    keyboardSession->GetSessionProperty()->SetKeyboardLayoutParams(params);
    keyboardSession->RecalculatePanelRectForAvoidArea(panelRect);
    EXPECT_EQ(panelRect.height_, 0);
    params.landscapeAvoidHeight_ = -1;
    params.portraitAvoidHeight_ = 1;
    keyboardSession->GetSessionProperty()->SetKeyboardLayoutParams(params);
    keyboardSession->RecalculatePanelRectForAvoidArea(panelRect);
    EXPECT_EQ(panelRect.height_, 0);
    params.landscapeAvoidHeight_ = 1;
    params.portraitAvoidHeight_ = 1;
    // the landscape width is same to the portrait
    keyboardSession->GetSessionProperty()->SetKeyboardLayoutParams(params);
    keyboardSession->RecalculatePanelRectForAvoidArea(panelRect);
    EXPECT_EQ(panelRect.height_, 1);
}

/**
 * @tc.name: RecalculatePanelRectForAvoidArea02
 * @tc.desc: test function: RecalculatePanelRectForAvoidArea
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, RecalculatePanelRectForAvoidArea02, TestSize.Level1)
{
    auto keyboardSession =
        GetKeyboardSession("RecalculatePanelRectForAvoidArea02", "RecalculatePanelRectForAvoidArea02");

    // if the landscape width is not same to the portrait
    KeyboardLayoutParams params;
    params.landscapeAvoidHeight_ = 1;
    params.portraitAvoidHeight_ = 2;
    params.LandscapePanelRect_.width_ = 1;
    params.PortraitPanelRect_.width_ = 2;
    keyboardSession->GetSessionProperty()->SetKeyboardLayoutParams(params);

    WSRect panelRect = { 0, 0, 0, 0 };
    panelRect.width_ = 1;
    keyboardSession->RecalculatePanelRectForAvoidArea(panelRect);
    EXPECT_EQ(panelRect.height_, 1);
    panelRect.width_ = 2;
    keyboardSession->RecalculatePanelRectForAvoidArea(panelRect);
    EXPECT_EQ(panelRect.height_, 2);
    panelRect.width_ = 3;
    params.portraitAvoidHeight_ = 1;
    keyboardSession->GetSessionProperty()->SetKeyboardLayoutParams(params);
    keyboardSession->RecalculatePanelRectForAvoidArea(panelRect);
    EXPECT_EQ(panelRect.height_, 1);
}

/**
 * @tc.name: SetSkipSelfWhenShowOnVirtualScreen
 * @tc.desc: test function: SetSkipSelfWhenShowOnVirtualScreen
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, SetSkipSelfWhenShowOnVirtualScreen, TestSize.Level1)
{
    auto keyboardSession =
        GetKeyboardSession("SetSkipSelfWhenShowOnVirtualScreen", "SetSkipSelfWhenShowOnVirtualScreen");
    bool skipResult = false;
    auto callFunc = [&skipResult](uint64_t surfaceNodeId, bool isSkip) { skipResult = isSkip; };
    keyboardSession->SetSkipSelfWhenShowOnVirtualScreen(true);
    usleep(SLEEP_TIME_US);
    ASSERT_EQ(skipResult, false);

    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->id_ = 1;
    keyboardSession->surfaceNode_ = surfaceNode;

    keyboardSession->SetSkipSelfWhenShowOnVirtualScreen(true);
    usleep(SLEEP_TIME_US);
    ASSERT_EQ(skipResult, false);

    keyboardSession->specificCallback_->onSetSkipSelfWhenShowOnVirtualScreen_ = callFunc;
    keyboardSession->SetSkipSelfWhenShowOnVirtualScreen(true);
    usleep(SLEEP_TIME_US);
    ASSERT_EQ(skipResult, true);
}

/**
 * @tc.name: SetSkipEventOnCastPlus01
 * @tc.desc: check func SetSkipEventOnCastPlus
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, SetSkipEventOnCastPlus01, Function | SmallTest | Level0)
{
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    SessionInfo info;
    info.abilityName_ = "SetSkipEventOnCastPlus";
    info.bundleName_ = "SetSkipEventOnCastPlus";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->SetSkipEventOnCastPlus(false);
    ASSERT_EQ(false, sceneSession->GetSessionProperty()->GetSkipEventOnCastPlus());
}

/**
 * @tc.name: IsNeedRaiseSubWindow01
 * @tc.desc: check func IsNeedRaiseSubWindow
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, IsNeedRaiseSubWindow01, Function | SmallTest | Level0)
{
    auto keyboardSession = GetKeyboardSession("IsNeedRaiseSubWindow", "IsNeedRaiseSubWindow");
    SessionInfo info;
    info.abilityName_ = "CallingSession";
    info.bundleName_ = "CallingSession";
    auto callingSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    auto sessionRect = callingSession->GetSessionRect();
    auto ret = keyboardSession->IsNeedRaiseSubWindow(callingSession, sessionRect);
    ASSERT_EQ(true, ret);

    callingSession->GetSessionProperty()->type_ = WindowType::WINDOW_TYPE_APP_SUB_WINDOW;
    ret = keyboardSession->IsNeedRaiseSubWindow(callingSession, sessionRect);
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: IsNeedRaiseSubWindow02
 * @tc.desc: check func IsNeedRaiseSubWindow
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, IsNeedRaiseSubWindow02, Function | SmallTest | Level0)
{
    auto keyboardSession = GetKeyboardSession("IsNeedRaiseSubWindow", "IsNeedRaiseSubWindow");
    SessionInfo info;
    info.abilityName_ = "CallingSession";
    info.bundleName_ = "CallingSession";
    auto callingSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    auto sessionRect = callingSession->GetSessionRect();
    callingSession->GetSessionProperty()->type_ = WindowType::WINDOW_TYPE_APP_SUB_WINDOW;

    SessionInfo mainInfo;
    mainInfo.abilityName_ = "mainSession";
    mainInfo.bundleName_ = "mainSession";
    auto mainSession = sptr<SceneSession>::MakeSptr(mainInfo, nullptr);
    callingSession->parentSession_ = mainSession;

    auto ret = keyboardSession->IsNeedRaiseSubWindow(callingSession, sessionRect);
    ASSERT_EQ(true, ret);

    mainSession->GetSessionProperty()->windowMode_ = WindowMode::WINDOW_MODE_SPLIT_PRIMARY;
    ret = keyboardSession->IsNeedRaiseSubWindow(callingSession, sessionRect);
    ASSERT_EQ(false, ret);

    WSRect newRect{ 0, 0, 0, 10 };
    mainSession->SetSessionRect(newRect);
    ret = keyboardSession->IsNeedRaiseSubWindow(callingSession, sessionRect);
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: CalculateOccupiedArea_MIDSCENE
 * @tc.desc: check func CalculateOccupiedArea_MIDSCENE
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, CalculateOccupiedArea_MIDSCENE, Function | SmallTest | Level1)
{
    auto keyboardSession = GetKeyboardSession("CalculateOccupiedArea_MIDSCENE",
        "CalculateOccupiedArea_MIDSCENE");
    SessionInfo info;
    info.abilityName_ = "CallingSession";
    info.bundleName_ = "CallingSession";
    auto callingSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    WSRect lastSafeRect = { 0, 0, 0, 0 };
    callingSession->SetLastSafeRect(lastSafeRect);
    auto lastSafeRectRet = callingSession->GetLastSafeRect();
    auto rectEqual = (lastSafeRect == lastSafeRectRet) ? true : false;
    EXPECT_EQ(rectEqual, true);
    callingSession->SetIsMidScene(false);
    usleep(SLEEP_TIME_US);
    EXPECT_EQ(false, callingSession->GetIsMidScene());
    WSRect zeroRect1 = { 0, 0, 0, 0 };
    WSRect zeroRect2 = { 0, 0, 0, 0 };
    sptr<OccupiedAreaChangeInfo> occupiedAreaInfo = nullptr;
    auto ret = keyboardSession->CalculateOccupiedArea(callingSession, zeroRect1, zeroRect2, occupiedAreaInfo);
    EXPECT_EQ(ret, false);
    callingSession->SetIsMidScene(true);
    usleep(SLEEP_TIME_US);
    EXPECT_EQ(true, callingSession->GetIsMidScene());
    lastSafeRect = { 1, 2, 3, 4 };
    callingSession->SetLastSafeRect(lastSafeRect);
    lastSafeRectRet = callingSession->GetLastSafeRect();
    rectEqual = (lastSafeRect == lastSafeRectRet) ? true : false;
    EXPECT_EQ(rectEqual, true);
    ret = keyboardSession->CalculateOccupiedArea(callingSession, zeroRect1, zeroRect2, occupiedAreaInfo);
    EXPECT_EQ(ret, true);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
