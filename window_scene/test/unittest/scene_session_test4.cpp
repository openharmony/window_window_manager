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

#include "display_manager.h"
#include "input_event.h"
#include "key_event.h"
#include "mock/mock_session_stage.h"
#include "pointer_event.h"

#include "session/host/include/main_session.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/sub_session.h"
#include "session/host/include/system_session.h"
#include "window_helper.h"
#include "wm_common.h"
#include "ui/rs_surface_node.h"


using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SceneSessionTest4 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<SceneSession> sceneSession;
    sptr<WindowSessionProperty> property;
    SessionInfo info;
    WSPropertyChangeAction action;
};

void SceneSessionTest4::SetUpTestCase()
{
}

void SceneSessionTest4::TearDownTestCase()
{
}

void SceneSessionTest4::SetUp()
{
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    property = new (std::nothrow) WindowSessionProperty();
    action = WSPropertyChangeAction::ACTION_UPDATE_ASPECT_RATIO;
}

void SceneSessionTest4::TearDown()
{
}

namespace {
/**
 * @tc.name: HandleActionUpdateFlags
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, HandleActionUpdateFlags, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_ASPECT_RATIO;
    sceneSession->HandleActionUpdateStatusProps(property, action);
    sceneSession->HandleActionUpdateNavigationProps(property, action);
    sceneSession->HandleActionUpdateNavigationIndicatorProps(property, action);
    sceneSession->HandleActionUpdateFlags(property, action);

    auto ret = sceneSession->HandleActionUpdateFlags(property, action);
    ASSERT_NE(ret, WMError::WM_ERROR_NOT_SYSTEM_APP);
    OHOS::Rosen::WindowSessionProperty windowSessionProperty;
    windowSessionProperty.isSystemCalling_ = {true};
    sceneSession->HandleActionUpdateFlags(property, action);
    windowSessionProperty.isSystemCalling_ = {true};
    OHOS::Rosen::Session session(info);
    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateFlags(property, action);
}

/**
 * @tc.name: HandleActionUpdateTouchHotArea
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, HandleActionUpdateTouchHotArea, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_ASPECT_RATIO;
    sceneSession->HandleActionUpdateTouchHotArea(property, action);
    OHOS::Rosen::Session session(info);
    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateTouchHotArea(property, action);

    sceneSession->SetSessionProperty(nullptr);
    sceneSession->HandleActionUpdateTouchHotArea(property, action);
}

/**
 * @tc.name: HandleActionUpdateDecorEnable
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, HandleActionUpdateDecorEnable, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_ASPECT_RATIO;

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    OHOS::Rosen::Session session(info);
    session.property_ = nullptr;
    sceneSession->HandleActionUpdateDecorEnable(property, action);
    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateDecorEnable(property, action);

    OHOS::Rosen::WindowSessionProperty windowSessionProperty;
    auto ret = sceneSession->HandleActionUpdateDecorEnable(property, action);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: HandleActionUpdateWindowLimits
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, HandleActionUpdateWindowLimits, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);

    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_ASPECT_RATIO;
    OHOS::Rosen::Session session(info);
    session.property_ = nullptr;
    sceneSession->HandleActionUpdateWindowLimits(property, action);
    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateWindowLimits(property, action);
}

/**
 * @tc.name: HandleActionUpdateDragenabled
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, HandleActionUpdateDragenabled, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_ASPECT_RATIO;
    OHOS::Rosen::Session session(info);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);

    sceneSession->HandleActionUpdateDragenabled(property, action);

    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateDragenabled(property, action);

    OHOS::Rosen::WindowSessionProperty windowSessionProperty;
    windowSessionProperty.isSystemCalling_ = {true};
    sceneSession->HandleActionUpdateDragenabled(property, action);
}

/**
 * @tc.name: HandleActionUpdateRaiseenabled
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, HandleActionUpdateRaiseenabled, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, property);
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_ANIMATION_FLAG;
    sceneSession->HandleActionUpdateRaiseenabled(property, action);

    OHOS::Rosen::WindowSessionProperty windowSessionProperty;
    windowSessionProperty.isSystemCalling_ = {true};
    sceneSession->HandleActionUpdateRaiseenabled(property, action);

    windowSessionProperty.isSystemCalling_ = {true};
    OHOS::Rosen::Session session(info);
    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateRaiseenabled(property, action);

    sceneSession->HandleActionUpdateHideNonSystemFloatingWindows(property, action);
}

/**
 * @tc.name: HandleActionUpdateTextfieldAvoidInfo
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, HandleActionUpdateTextfieldAvoidInfo, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, property);
    sceneSession->HandleActionUpdateTextfieldAvoidInfo(property, action);
    sceneSession->HandleActionUpdateWindowMask(property, action);

    OHOS::Rosen::Session session(info);
    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateTextfieldAvoidInfo(property, action);
    sceneSession->HandleActionUpdateWindowMask(property, action);
    sceneSession->HandleActionUpdateTopmost(property, action);

    SessionInfo info;
    sptr<SceneSession> sceneSession1 = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    sceneSession1->SetSessionProperty(nullptr);
    sceneSession1->HandleActionUpdateTextfieldAvoidInfo(property, action);
    sceneSession1->HandleActionUpdateWindowMask(property, action);
}

/**
 * @tc.name: SetWindowFlags
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, SetWindowFlags, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, property);
    sceneSession->SetWindowFlags(property);

    OHOS::Rosen::Session session(info);
    session.property_ = new WindowSessionProperty();
    sceneSession->SetWindowFlags(property);
    sceneSession->NotifySessionChangeByActionNotifyManager(property, action);

    session.property_ = nullptr;
    sceneSession->SetWindowFlags(property);
    sceneSession->sessionChangeByActionNotifyManagerFunc_ = [](
        const sptr<SceneSession>& sceneSession,
        const sptr<WindowSessionProperty>& property, WSPropertyChangeAction action
    ){};
    sceneSession->NotifySessionChangeByActionNotifyManager(property, action);
}

/**
 * @tc.name: SetSkipDraw
 * @tc.desc: SetSkipDraw function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, SetSkipDraw, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetSkipDraw";
    info.bundleName_ = "SetSkipDraw";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(nullptr, session);
    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    session->surfaceNode_ = surfaceNode;
    session->SetLeashWinSurfaceNode(nullptr);
    session->SetSkipDraw(true);
    session->SetLeashWinSurfaceNode(surfaceNode);
    EXPECT_EQ(surfaceNode, session->GetLeashWinSurfaceNode());
    session->SetSkipDraw(true);
}

/**
 * @tc.name: SetScale
 * @tc.desc: SetScale function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, SetScale, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetScale";
    info.bundleName_ = "SetScale";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(nullptr, session);
    session->Session::SetScale(1.0f, 2.0f, 3.0f, 4.0f);
    session->sessionStage_ = nullptr;
    session->SetScale(5.0f, 2.0f, 3.0f, 4.0f);
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    NotifyWindowInfoUpdateCallback func = [](int32_t persistentId, WindowUpdateType type) {
        return;
    };
    specificCallback->onWindowInfoUpdate_ = func;
    session->specificCallback_ = specificCallback;
    session->SetScale(5.0f, 5.0f, 3.0f, 4.0f);
    session->SetScale(5.0f, 5.0f, 5.0f, 4.0f);
    session->SetScale(5.0f, 5.0f, 5.0f, 5.0f);
    session->SetScale(5.0f, 5.0f, 5.0f, 5.0f);
    EXPECT_EQ(5.0f, session->GetScaleX());
    EXPECT_EQ(5.0f, session->GetScaleY());
    EXPECT_EQ(5.0f, session->GetPivotX());
    EXPECT_EQ(5.0f, session->GetPivotY());

    session->sessionStage_ = new SessionStageMocker();
    EXPECT_NE(nullptr, session->sessionStage_);
    session->SetScale(5.0f, 5.0f, 5.0f, 5.0f);
    EXPECT_EQ(5.0f, session->GetPivotY());
}

/**
 * @tc.name: RequestSessionBack
 * @tc.desc: RequestSessionBack function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, RequestSessionBack, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "RequestSessionBack";
    info.bundleName_ = "RequestSessionBack";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(nullptr, session);
    NotifyBackPressedFunc func = [](const bool needMoveToBackground) {
        return;
    };
    session->backPressedFunc_ = func;
    EXPECT_EQ(WSError::WS_OK, session->RequestSessionBack(true));
}

/**
 * @tc.name: SetSurfaceBounds
 * @tc.desc: SetSurfaceBounds function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, SetSurfaceBounds, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetSurfaceBounds";
    info.bundleName_ = "SetSurfaceBounds";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(nullptr, session);
    WSRect rect;
    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    session->surfaceNode_ = surfaceNode;
    session->SetSurfaceBounds(rect);
    session->SetLeashWinSurfaceNode(surfaceNode);
    session->SetSurfaceBounds(rect);
    EXPECT_NE(nullptr, session->GetLeashWinSurfaceNode());
}

/**
 * @tc.name: SetFloatingScale
 * @tc.desc: SetFloatingScale function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, SetFloatingScale, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetFloatingScale";
    info.bundleName_ = "SetFloatingScale";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(nullptr, session);
    session->floatingScale_ = 3.14f;
    session->SetFloatingScale(2.176f);
    session->SetFloatingScale(3.14f);
    EXPECT_EQ(nullptr, session->specificCallback_);
}

/**
 * @tc.name: GetSessionSnapshotFilePath
 * @tc.desc: GetSessionSnapshotFilePath function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, GetSessionSnapshotFilePath, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "GetSessionSnapshotFilePath";
    info.bundleName_ = "GetSessionSnapshotFilePath";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    session->Session::SetSessionState(SessionState::STATE_DISCONNECT);
    session->scenePersistence_ = sptr<ScenePersistence>::MakeSptr("GetSessionSnapshotFilePath", 1);
    EXPECT_EQ("GetSessionSnapshotFilePath_1.astc", session->GetSessionSnapshotFilePath());

    session->SetSessionState(SessionState::STATE_BACKGROUND);
    EXPECT_EQ("GetSessionSnapshotFilePath_1.astc", session->GetSessionSnapshotFilePath());
}

/**
 * @tc.name: SetRequestedOrientation
 * @tc.desc: SetRequestedOrientation function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, SetRequestedOrientation, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetRequestedOrientation";
    info.bundleName_ = "SetRequestedOrientation";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    Orientation orientation { Orientation::BEGIN };
    session->sessionChangeCallback_ = nullptr;
    session->SetRequestedOrientation(orientation);
    session->sessionChangeCallback_ = sptr<SceneSession::SessionChangeCallback>::MakeSptr();
    session->sessionChangeCallback_->OnRequestedOrientationChange_ = nullptr;
    session->SetRequestedOrientation(orientation);
    NotifyReqOrientationChangeFunc func = [](uint32_t orientation) {
        return;
    };
    session->sessionChangeCallback_->OnRequestedOrientationChange_ = func;
    session->SetRequestedOrientation(orientation);
    EXPECT_NE(nullptr, session->sessionChangeCallback_->OnRequestedOrientationChange_);
}

/**
 * @tc.name: UpdateSessionPropertyByAction
 * @tc.desc: UpdateSessionPropertyByAction function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, UpdateSessionPropertyByAction, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateSessionPropertyByAction";
    info.bundleName_ = "UpdateSessionPropertyByAction";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE;
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, sceneSession->UpdateSessionPropertyByAction(nullptr, action));

    sceneSession->SetSessionProperty(nullptr);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, sceneSession->UpdateSessionPropertyByAction(property, action));

    sceneSession->SetSessionProperty(property);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, sceneSession->UpdateSessionPropertyByAction(property, action));

    action = WSPropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON;
    EXPECT_EQ(WMError::WM_OK, sceneSession->UpdateSessionPropertyByAction(property, action));
}

/**
 * @tc.name: HandleUpdatePropertyByAction
 * @tc.desc: HandleUpdatePropertyByAction function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, HandleUpdatePropertyByAction, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "HandleUpdatePropertyByAction";
    info.bundleName_ = "HandleUpdatePropertyByAction";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_MODE;
    sceneSession->HandleUpdatePropertyByAction(property, action);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, sceneSession->HandleUpdatePropertyByAction(nullptr, action));
}

/**
 * @tc.name: ProcessUpdatePropertyByAction1
 * @tc.desc: ProcessUpdatePropertyByAction1 function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, ProcessUpdatePropertyByAction1, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ProcessUpdatePropertyByAction1";
    info.bundleName_ = "ProcessUpdatePropertyByAction1";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON));

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON));

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_FOCUSABLE));

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_TOUCHABLE));

    sceneSession->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS));

    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sceneSession->state_ = SessionState::STATE_END;
    EXPECT_EQ(WMError::WM_ERROR_INVALID_SESSION, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS));

    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sceneSession->state_ = SessionState::STATE_ACTIVE;
    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS));

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_ORIENTATION));

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE));

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_SYSTEM_PRIVACY_MODE));

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_SNAPSHOT_SKIP));
}

/**
 * @tc.name: ProcessUpdatePropertyByAction2
 * @tc.desc: ProcessUpdatePropertyByAction2 function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, ProcessUpdatePropertyByAction2, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ProcessUpdatePropertyByAction2";
    info.bundleName_ = "ProcessUpdatePropertyByAction2";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_MAXIMIZE_STATE));

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_OTHER_PROPS));

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_STATUS_PROPS));

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_NAVIGATION_PROPS));

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_NAVIGATION_INDICATOR_PROPS));

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_FLAGS));

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_MODE));

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_ANIMATION_FLAG));

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_TOUCH_HOT_AREA));

    property->SetSystemCalling(false);
    EXPECT_EQ(WMError::WM_ERROR_NOT_SYSTEM_APP, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_DECOR_ENABLE));

    property->SetSystemCalling(true);
    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_DECOR_ENABLE));
}

/**
 * @tc.name: ProcessUpdatePropertyByAction3
 * @tc.desc: ProcessUpdatePropertyByAction3 function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, ProcessUpdatePropertyByAction3, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ProcessUpdatePropertyByAction3";
    info.bundleName_ = "ProcessUpdatePropertyByAction3";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_WINDOW_LIMITS));

    property->SetSystemCalling(false);
    EXPECT_EQ(WMError::WM_ERROR_NOT_SYSTEM_APP, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_DRAGENABLED));

    property->SetSystemCalling(true);
    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_DRAGENABLED));
    
    sceneSession->property_ = property;
    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_DRAGENABLED));

    property->SetSystemCalling(false);
    EXPECT_EQ(WMError::WM_ERROR_NOT_SYSTEM_APP, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_RAISEENABLED));

    property->SetSystemCalling(true);
    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_RAISEENABLED));

    sceneSession->property_ = property;
    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_RAISEENABLED));

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_HIDE_NON_SYSTEM_FLOATING_WINDOWS));

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_TEXTFIELD_AVOID_INFO));

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_WINDOW_MASK));

    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_TOPMOST));

    property->SetSystemCalling(false);
    EXPECT_EQ(WMError::WM_ERROR_NOT_SYSTEM_APP, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_MODE_SUPPORT_INFO));

    property->SetSystemCalling(true);
    EXPECT_EQ(WMError::WM_OK, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_MODE_SUPPORT_INFO));

    EXPECT_EQ(WMError::WM_DO_NOTHING, sceneSession->ProcessUpdatePropertyByAction(property,
        WSPropertyChangeAction::ACTION_UPDATE_RECT));
}

/**
 * @tc.name: HandleSpecificSystemBarProperty
 * @tc.desc: HandleSpecificSystemBarProperty
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, HandleSpecificSystemBarProperty, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "HandleSpecificSystemBarProperty";
    info.bundleName_ = "HandleSpecificSystemBarProperty";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    WindowType type = WindowType::WINDOW_TYPE_STATUS_BAR;
    sceneSession->HandleSpecificSystemBarProperty(type, property);
}

/**
 * @tc.name: SetWindowFlags1
 * @tc.desc: SetWindowFlags1
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, SetWindowFlags1, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetWindowFlags1";
    info.bundleName_ = "SetWindowFlags1";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    sptr<WindowSessionProperty> sessionProperty = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, sessionProperty);
    sceneSession->SetWindowFlags(property);
    sceneSession->property_ = sessionProperty;
    property->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT));
    sceneSession->property_->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
    sceneSession->SetWindowFlags(property);
    property->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_FORBID_SPLIT_MOVE));
    sceneSession->property_->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK));
    sceneSession->SetWindowFlags(property);
}
}
}
}