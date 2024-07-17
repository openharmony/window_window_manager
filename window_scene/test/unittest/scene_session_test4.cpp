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
    sceneSession->HandleActionUpdateStatusProps(property, sceneSession, action);
    sceneSession->HandleActionUpdateNavigationProps(property, sceneSession, action);
    sceneSession->HandleActionUpdateNavigationIndicatorProps(property, sceneSession, action);
    sceneSession->HandleActionUpdateFlags(property, sceneSession, action);

    auto ret = sceneSession->HandleActionUpdateFlags(property, sceneSession, action);
    ASSERT_NE(ret, WMError::WM_ERROR_NOT_SYSTEM_APP);
    OHOS::Rosen::WindowSessionProperty windowSessionProperty;
    windowSessionProperty.isSystemCalling_ = {true};
    sceneSession->HandleActionUpdateFlags(property, sceneSession, action);
    windowSessionProperty.isSystemCalling_ = {true};
    OHOS::Rosen::Session session(info);
    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateFlags(property, sceneSession, action);
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
    sceneSession->HandleActionUpdateTouchHotArea(property, sceneSession, action);
    OHOS::Rosen::Session session(info);
    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateTouchHotArea(property, sceneSession, action);
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
    sceneSession->HandleActionUpdateDecorEnable(property, sceneSession, action);
    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateDecorEnable(property, sceneSession, action);

    OHOS::Rosen::WindowSessionProperty windowSessionProperty;
    auto ret = sceneSession->HandleActionUpdateDecorEnable(property, sceneSession, action);
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
    sceneSession->HandleActionUpdateWindowLimits(property, sceneSession, action);
    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateWindowLimits(property, sceneSession, action);
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

    sceneSession->HandleActionUpdateDragenabled(property, sceneSession, action);

    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateDragenabled(property, sceneSession, action);

    OHOS::Rosen::WindowSessionProperty windowSessionProperty;
    windowSessionProperty.isSystemCalling_ = {true};
    sceneSession->HandleActionUpdateDragenabled(property, sceneSession, action);
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
    sceneSession->HandleActionUpdateRaiseenabled(property, sceneSession, action);

    OHOS::Rosen::WindowSessionProperty windowSessionProperty;
    windowSessionProperty.isSystemCalling_ = {true};
    sceneSession->HandleActionUpdateRaiseenabled(property, sceneSession, action);

    windowSessionProperty.isSystemCalling_ = {true};
    OHOS::Rosen::Session session(info);
    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateRaiseenabled(property, sceneSession, action);

    sceneSession->HandleActionUpdateHideNonSystemFloatingWindows(property, sceneSession, action);
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
    sceneSession->HandleActionUpdateTextfieldAvoidInfo(property, sceneSession, action);
    sceneSession->HandleActionUpdateWindowMask(property, sceneSession, action);

    OHOS::Rosen::Session session(info);
    session.property_ = new WindowSessionProperty();
    sceneSession->HandleActionUpdateTextfieldAvoidInfo(property, sceneSession, action);
    sceneSession->HandleActionUpdateWindowMask(property, sceneSession, action);
    sceneSession->HandleActionUpdateTopmost(property, sceneSession, action);
}

/**
 * @tc.name: isNeedSystemPermissionByAction
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, isNeedSystemPermissionByAction, Function | SmallTest | Level2)
{
    ASSERT_TRUE(sceneSession->isNeedSystemPermissionByAction(WSPropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON,
        property));
    ASSERT_TRUE(sceneSession->isNeedSystemPermissionByAction(WSPropertyChangeAction::ACTION_UPDATE_FLAGS,
        property));
    ASSERT_TRUE(sceneSession->isNeedSystemPermissionByAction(WSPropertyChangeAction::ACTION_UPDATE_SNAPSHOT_SKIP,
        property));
    ASSERT_TRUE(sceneSession->isNeedSystemPermissionByAction(WSPropertyChangeAction::ACTION_UPDATE_TOPMOST,
        property));
    ASSERT_TRUE(sceneSession->isNeedSystemPermissionByAction(WSPropertyChangeAction::ACTION_UPDATE_DECOR_ENABLE,
        property));
    ASSERT_TRUE(sceneSession->isNeedSystemPermissionByAction(WSPropertyChangeAction::ACTION_UPDATE_DRAGENABLED,
        property));
    ASSERT_TRUE(sceneSession->isNeedSystemPermissionByAction(WSPropertyChangeAction::ACTION_UPDATE_RAISEENABLED,
        property));
    ASSERT_TRUE(sceneSession->isNeedSystemPermissionByAction(WSPropertyChangeAction::ACTION_UPDATE_MODE_SUPPORT_INFO,
        property));
    auto sessionProperty = sceneSession->GetSessionProperty();
    uint32_t remove = sessionProperty->GetWindowFlags() &
        (~(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK)));
    uint32_t add = sessionProperty->GetWindowFlags() | (static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK));
    sessionProperty.SetWindowFlags(remove);
    property.SetWindowFlags(add);
    ASSERT_TRUE(sceneSession->isNeedSystemPermissionByAction(WSPropertyChangeAction::ACTION_UPDATE_FLAGS,
        property));
    ASSERT_FALSE(sceneSession->isNeedSystemPermissionByAction(WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE,
        property));
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
    sptr<SceneSession> sceneSession1 = nullptr;
    sceneSession->SetWindowFlags(sceneSession1, property);
    sceneSession1 = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    sceneSession->SetWindowFlags(sceneSession1, property);

    OHOS::Rosen::Session session(info);
    session.property_ = new WindowSessionProperty();
    sceneSession->SetWindowFlags(sceneSession1, property);
    sceneSession->NotifySessionChangeByActionNotifyManager(sceneSession1, property, action);
}

/**
 * @tc.name: TerminateSession
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, TerminateSession, Function | SmallTest | Level2)
{
    sptr<AAFwk::SessionInfo> abilitySessionInfo = new AAFwk::SessionInfo();
    ASSERT_NE(nullptr, abilitySessionInfo);
    OHOS::Rosen::Session session(info);
    session.isTerminating = true;
    sceneSession->TerminateSession(abilitySessionInfo);
}

/**
 * @tc.name: NotifySessionException
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest4, NotifySessionException, Function | SmallTest | Level2)
{
    sptr<AAFwk::SessionInfo> abilitySessionInfo = new AAFwk::SessionInfo();
    ASSERT_NE(nullptr, abilitySessionInfo);
    bool needRemoveSession = true;
    OHOS::Rosen::Session session(info);
    session.isTerminating = true;
    sceneSession->NotifySessionException(abilitySessionInfo, needRemoveSession);
    sceneSession->GetLastSafeRect();
    WSRect rect;
    sceneSession->SetLastSafeRect(rect);
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
}
}
}