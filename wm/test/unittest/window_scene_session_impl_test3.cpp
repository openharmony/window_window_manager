/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <parameters.h>
#include "ability_context_impl.h"
#include "display_info.h"
#include "mock_session.h"
#include "mock_uicontent.h"
#include "mock_window_adapter.h"
#include "singleton_mocker.h"
#include "window_scene_session_impl.h"
#include "window_session_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
class WindowSceneSessionImplTest3 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_;
    std::unique_ptr<Mocker> mocker_ = std::make_unique<Mocker>();

private:
    RSSurfaceNode::SharedPtr CreateRSSurfaceNode();
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

void WindowSceneSessionImplTest3::SetUpTestCase() {}

void WindowSceneSessionImplTest3::TearDownTestCase() {}

void WindowSceneSessionImplTest3::SetUp()
{
    abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();
}

void WindowSceneSessionImplTest3::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    abilityContext_ = nullptr;
}

RSSurfaceNode::SharedPtr WindowSceneSessionImplTest3::CreateRSSurfaceNode()
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "startingWindowTestSurfaceNode";
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig, RSSurfaceNodeType::DEFAULT);
    return surfaceNode;
}

namespace {
/**
 * @tc.name: UpdateOrientation
 * @tc.desc: UpdateOrientation
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, UpdateOrientation, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("UpdateOrientation");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    auto ret = windowSceneSessionImpl->UpdateOrientation();
    EXPECT_EQ(WSError::WS_OK, ret);

    windowSceneSessionImpl->hostSession_ = nullptr;
    windowSceneSessionImpl->UpdateDensity();

    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->UpdateDensity();

    windowSceneSessionImpl->userLimitsSet_ = true;
    windowSceneSessionImpl->UpdateDensity();
}

/**
 * @tc.name: SetWindowMask
 * @tc.desc: SetWindowMask
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, SetWindowMask, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("SetWindowMask");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    std::vector<std::vector<uint32_t>> windowMask;

    windowSceneSessionImpl->hostSession_ = nullptr;
    auto ret = windowSceneSessionImpl->SetWindowMask(windowMask);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle0", "CreateTestModule0", "CreateTestAbility0"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    ret = windowSceneSessionImpl->SetWindowMask(windowMask);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
}

/**
 * @tc.name: UpdateTitleInTargetPos
 * @tc.desc: UpdateTitleInTargetPos
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, UpdateTitleInTargetPos, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("UpdateTitleInTargetPos");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    auto ret = windowSceneSessionImpl->UpdateTitleInTargetPos(true, 0);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_WINDOW, ret);
    ret = windowSceneSessionImpl->UpdateTitleInTargetPos(false, 0);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_WINDOW, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle0", "CreateTestModule0", "CreateTestAbility0"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    ret = windowSceneSessionImpl->SwitchFreeMultiWindow(true);
    EXPECT_EQ(WSError::WS_OK, ret);
    ret = windowSceneSessionImpl->SwitchFreeMultiWindow(false);
    EXPECT_EQ(WSError::WS_OK, ret);

    windowSceneSessionImpl->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_NE(nullptr, windowSceneSessionImpl->uiContent_);
    ret = windowSceneSessionImpl->UpdateTitleInTargetPos(true, 0);
    EXPECT_EQ(WSError::WS_OK, ret);
    ret = windowSceneSessionImpl->UpdateTitleInTargetPos(false, 0);
    EXPECT_EQ(WSError::WS_OK, ret);

    windowSceneSessionImpl->uiContent_ = nullptr;
    ret = windowSceneSessionImpl->UpdateTitleInTargetPos(true, 0);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_PARAM, ret);
    ret = windowSceneSessionImpl->UpdateTitleInTargetPos(false, 0);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_PARAM, ret);
}

/**
 * @tc.name: Destroy
 * @tc.desc: Destroy
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, Destroy, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("Destroy");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    windowSceneSessionImpl->property_ = nullptr;
    auto ret = windowSceneSessionImpl->Destroy(true, true);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: CheckParmAndPermission01
 * @tc.desc: CheckParmAndPermission
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, CheckParmAndPermission01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("CheckParmAndPermission01");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    windowSceneSessionImpl->surfaceNode_ = nullptr;
    
    auto ret = windowSceneSessionImpl->CheckParmAndPermission();
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    ret = windowSceneSessionImpl->SetCornerRadius(1.0f);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    auto surfaceNode_mocker = CreateRSSurfaceNode();
    ASSERT_NE(nullptr, surfaceNode_mocker);
    windowSceneSessionImpl->surfaceNode_ = surfaceNode_mocker;
    ret = windowSceneSessionImpl->CheckParmAndPermission();
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: PerformBack
 * @tc.desc: PerformBack
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, PerformBack, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("PerformBack");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    windowSceneSessionImpl->PerformBack();

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->hostSession_ = session;
    ASSERT_NE(nullptr, abilityContext_);
    windowSceneSessionImpl->context_ = abilityContext_;
    windowSceneSessionImpl->PerformBack();
    windowSceneSessionImpl->context_ = nullptr;
    windowSceneSessionImpl->PerformBack();

    windowSceneSessionImpl->hostSession_ = nullptr;
    windowSceneSessionImpl->PerformBack();
}

/**
 * @tc.name: SetShadowOffsetX02
 * @tc.desc: SetShadowOffsetX02
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, SetShadowOffsetX02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("SetShadowOffsetX02");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    auto ret = windowSceneSessionImpl->SetShadowOffsetX(1.0f);
    EXPECT_EQ(WMError::WM_OK, ret);
    std::string color = "#ff22ee44";
    ret = windowSceneSessionImpl->SetShadowColor(color);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: AdjustKeyboardLayout
 * @tc.desc: AdjustKeyboardLayout
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, AdjustKeyboardLayout, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("AdjustKeyboardLayout");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    KeyboardLayoutParams params;

    auto ret = windowSceneSessionImpl->AdjustKeyboardLayout(params);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);

    windowSceneSessionImpl->property_ = nullptr;
    ret = windowSceneSessionImpl->AdjustKeyboardLayout(params);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);

    windowSceneSessionImpl->hostSession_ = nullptr;
    ret = windowSceneSessionImpl->AdjustKeyboardLayout(params);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: UpdateSubWindowState
 * @tc.desc: UpdateSubWindowState
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, UpdateSubWindowState, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("UpdateSubWindowState");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    WindowType type = WindowType::APP_SUB_WINDOW_BASE;

    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    windowSceneSessionImpl->UpdateSubWindowState(type);
    windowSceneSessionImpl->state_ = WindowState::STATE_CREATED;
    windowSceneSessionImpl->UpdateSubWindowState(type);
    type = WindowType::SYSTEM_WINDOW_BASE;
    windowSceneSessionImpl->UpdateSubWindowState(type);
    type = WindowType::APP_MAIN_WINDOW_BASE;
    windowSceneSessionImpl->UpdateSubWindowState(type);
}

/**
 * @tc.name: GetWindowFlags
 * @tc.desc: GetWindowFlags
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, GetWindowFlags, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("GetWindowFlags");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowFlags(0);
    auto ret = windowSceneSessionImpl->GetWindowFlags();
    EXPECT_EQ(0, ret);
}

/**
 * @tc.name: NotifyPrepareClosePiPWindow
 * @tc.desc: NotifyPrepareClosePiPWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, NotifyPrepareClosePiPWindow, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("NotifyPrepareClosePiPWindow");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    auto ret = windowSceneSessionImpl->NotifyPrepareClosePiPWindow();
    EXPECT_EQ(WMError::WM_DO_NOTHING, ret);

    auto ret1 = windowSceneSessionImpl->KeepKeyboardOnFocus(true);
    EXPECT_EQ(WmErrorCode::WM_OK, ret1);
    windowSceneSessionImpl->property_ = nullptr;
    ret1 = windowSceneSessionImpl->KeepKeyboardOnFocus(true);
    EXPECT_EQ(WmErrorCode::WM_ERROR_STATE_ABNORMALLY, ret1);
}

/**
 * @tc.name: NotifyWindowSessionProperty
 * @tc.desc: NotifyWindowSessionProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, NotifyWindowSessionProperty, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("NotifyWindowSessionProperty");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    auto ret = windowSceneSessionImpl->NotifyWindowSessionProperty();
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->hostSession_ = session;
    ret = windowSceneSessionImpl->NotifyWindowSessionProperty();
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: AddWindowFlag
 * @tc.desc: AddWindowFlag
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, AddWindowFlag, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("AddWindowFlag");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    auto ret = windowSceneSessionImpl->AddWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    ret = windowSceneSessionImpl->AddWindowFlag(WindowFlag::WINDOW_FLAG_HANDWRITING);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    ret = windowSceneSessionImpl->AddWindowFlag(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
}

/**
 * @tc.name: SetDefaultProperty
 * @tc.desc: SetDefaultProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, SetDefaultProperty, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("SetDefaultProperty");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_TOAST);
    windowSceneSessionImpl->SetDefaultProperty();
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_POINTER);
    windowSceneSessionImpl->SetDefaultProperty();

    auto ret = windowSceneSessionImpl->UpdateDisplayId(0);
    EXPECT_EQ(WSError::WS_OK, ret);
}

/**
 * @tc.name: SetAspectRatio01
 * @tc.desc: SetAspectRatio
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, SetAspectRatio01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("SetAspectRatio01");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSessionImpl->hostSession_ = session;
    auto ret = windowSceneSessionImpl->SetAspectRatio(MathHelper::INF);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
    ret = windowSceneSessionImpl->SetAspectRatio(MathHelper::NAG_INF);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
    ret = windowSceneSessionImpl->SetAspectRatio(std::sqrt(-1.0));
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
    ret = windowSceneSessionImpl->SetAspectRatio(0.0f);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
    ret = windowSceneSessionImpl->SetAspectRatio(1.0f);
    EXPECT_EQ(WMError::WM_OK, ret);
    windowSceneSessionImpl->hostSession_ = nullptr;
    ret = windowSceneSessionImpl->SetAspectRatio(1.0f);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    windowSceneSessionImpl->property_ = nullptr;
    ret = windowSceneSessionImpl->SetAspectRatio(1.0f);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: SetCallingWindow
 * @tc.desc: SetCallingWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, SetCallingWindow, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("SetCallingWindow");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    windowSceneSessionImpl->hostSession_ = nullptr;
    auto ret = windowSceneSessionImpl->SetCallingWindow(0);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);

    SessionInfo sessionInfo = {"CreateTestBundle0", "CreateTestModule0", "CreateTestAbility0"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSessionImpl->hostSession_ = session;
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ret = windowSceneSessionImpl->NotifyPrepareClosePiPWindow();
    EXPECT_EQ(WMError::WM_DO_NOTHING, ret);
}

/**
 * @tc.name: RaiseToAppTop
 * @tc.desc: RaiseToAppTop
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, RaiseToAppTop, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("RaiseToAppTop");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(6);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetParentPersistentId(0);
    auto ret = windowSceneSessionImpl->RaiseToAppTop();
    EXPECT_EQ(WmErrorCode::WM_ERROR_INVALID_PARENT, ret);

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetParentPersistentId(6);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    windowSceneSessionImpl->hostSession_ = nullptr;
    ret = windowSceneSessionImpl->RaiseToAppTop();
    EXPECT_EQ(WmErrorCode::WM_ERROR_STATE_ABNORMALLY, ret);
}

/**
 * @tc.name: SetBlur
 * @tc.desc: SetBlur
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, SetBlur, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("SetBlur");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    windowSceneSessionImpl->surfaceNode_ = nullptr;
    auto ret = windowSceneSessionImpl->SetBlur(1.0f);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    ret = windowSceneSessionImpl->SetBackdropBlur(1.0f);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    ret = windowSceneSessionImpl->SetBackdropBlurStyle(WindowBlurStyle::WINDOW_BLUR_OFF);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: SetTouchHotAreas
 * @tc.desc: SetTouchHotAreas
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, SetTouchHotAreas, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("SetTouchHotAreas");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    std::vector<Rect> rects;
    Rect rect = {800, 800, 1200, 1200};
    rects.push_back(rect);
    auto ret = windowSceneSessionImpl->SetTouchHotAreas(rects);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    windowSceneSessionImpl->property_ = nullptr;
    ret = windowSceneSessionImpl->SetTouchHotAreas(rects);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: GetWindowLimits
 * @tc.desc: GetWindowLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, GetWindowLimits, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("GetWindowLimits");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    WindowLimits windowLimits = {1000, 1000, 1000, 1000, 0.0f, 0.0f};

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_CREATED;
    auto ret = windowSceneSessionImpl->GetWindowLimits(windowLimits);
    EXPECT_EQ(WMError::WM_OK, ret);

    windowSceneSessionImpl->property_ = nullptr;
    ret = windowSceneSessionImpl->GetWindowLimits(windowLimits);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    windowSceneSessionImpl->hostSession_ = nullptr;
    ret = windowSceneSessionImpl->GetWindowLimits(windowLimits);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
}

/**
 * @tc.name: SetWindowLimits
 * @tc.desc: SetWindowLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, SetWindowLimits, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("SetWindowLimits");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    WindowLimits windowLimits = {1000, 1000, 1000, 1000, 0.0f, 0.0f};

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_CREATED;
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_SUB_WINDOW_END);
    auto ret = windowSceneSessionImpl->SetWindowLimits(windowLimits);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_CALLING, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    ret = windowSceneSessionImpl->MoveTo(0, 0);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ret = windowSceneSessionImpl->MoveTo(0, 0);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: IsValidSystemWindowType
 * @tc.desc: IsValidSystemWindowType
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, IsValidSystemWindowType, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("IsValidSystemWindowType");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    WindowType type = WindowType::WINDOW_TYPE_DRAGGING_EFFECT;
    EXPECT_FALSE(!windowSceneSessionImpl->IsValidSystemWindowType(type));
    type = WindowType::WINDOW_TYPE_SEARCHING_BAR;
    EXPECT_FALSE(!windowSceneSessionImpl->IsValidSystemWindowType(type));
    type = WindowType::WINDOW_TYPE_PANEL;
    EXPECT_FALSE(!windowSceneSessionImpl->IsValidSystemWindowType(type));
    type = WindowType::WINDOW_TYPE_VOLUME_OVERLAY;
    EXPECT_FALSE(!windowSceneSessionImpl->IsValidSystemWindowType(type));
    type = WindowType::WINDOW_TYPE_INPUT_METHOD_STATUS_BAR;
    EXPECT_FALSE(!windowSceneSessionImpl->IsValidSystemWindowType(type));
    type = WindowType::WINDOW_TYPE_SYSTEM_TOAST;
    EXPECT_FALSE(!windowSceneSessionImpl->IsValidSystemWindowType(type));
    type = WindowType::WINDOW_TYPE_SYSTEM_FLOAT;
    EXPECT_FALSE(!windowSceneSessionImpl->IsValidSystemWindowType(type));
    type = WindowType::WINDOW_TYPE_SYSTEM_FLOAT;
    EXPECT_FALSE(!windowSceneSessionImpl->IsValidSystemWindowType(type));
    type = WindowType::WINDOW_TYPE_SYSTEM_FLOAT;
    EXPECT_FALSE(!windowSceneSessionImpl->IsValidSystemWindowType(type));
    type = WindowType::WINDOW_TYPE_HANDWRITE;
    EXPECT_FALSE(!windowSceneSessionImpl->IsValidSystemWindowType(type));
}

/**
 * @tc.name: UpdateFloatingWindowSizeBySizeLimits
 * @tc.desc: UpdateFloatingWindowSizeBySizeLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, UpdateFloatingWindowSizeBySizeLimits, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("UpdateFloatingWindowSizeBySizeLimits");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::BELOW_APP_SYSTEM_WINDOW_END);
    uint32_t maxWidth = 32;
    windowSceneSessionImpl->UpdateFloatingWindowSizeBySizeLimits(maxWidth, maxWidth);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    WindowLimits windowLimits = {0, 0, 0, 0, 0.0f, 0.0f};
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowLimits(windowLimits);
    windowSceneSessionImpl->UpdateFloatingWindowSizeBySizeLimits(maxWidth, maxWidth);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA);
    windowSceneSessionImpl->UpdateFloatingWindowSizeBySizeLimits(maxWidth, maxWidth);
}

/**
 * @tc.name: IsDecorEnable
 * @tc.desc: IsDecorEnable
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, IsDecorEnable, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("IsDecorEnable");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    windowSceneSessionImpl->windowSystemConfig_.freeMultiWindowSupport_ = true;
    windowSceneSessionImpl->DisableAppWindowDecor();
    auto ret = windowSceneSessionImpl->IsDecorEnable();
    EXPECT_EQ(false, ret);
    windowSceneSessionImpl->windowSystemConfig_.freeMultiWindowSupport_ = false;
    ret = windowSceneSessionImpl->IsDecorEnable();
    EXPECT_EQ(false, ret);
}

/**
 * @tc.name: SetDefaultDensityEnabled
 * @tc.desc: SetDefaultDensityEnabled
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, SetDefaultDensityEnabled, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("SetDefaultDensityEnabled");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    windowSceneSessionImpl->hostSession_ = nullptr;
    auto ret = windowSceneSessionImpl->SetDefaultDensityEnabled(true);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ret = windowSceneSessionImpl->SetDefaultDensityEnabled(true);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_CALLING, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    ret = windowSceneSessionImpl->SetDefaultDensityEnabled(false);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = windowSceneSessionImpl->SetDefaultDensityEnabled(true);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: RecoverAndReconnectSceneSession
 * @tc.desc: RecoverAndReconnectSceneSession
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, RecoverAndReconnectSceneSession, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("RecoverAndReconnectSceneSession");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    auto ret = windowSceneSessionImpl->RecoverAndReconnectSceneSession();
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    windowSceneSessionImpl->isFocused_ = true;
    ret = windowSceneSessionImpl->RecoverAndReconnectSceneSession();
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    windowSceneSessionImpl->property_ = nullptr;
    ret = windowSceneSessionImpl->RecoverAndReconnectSceneSession();
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: UpdateWindowState
 * @tc.desc: UpdateWindowState
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, UpdateWindowState, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("UpdateWindowState");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    windowSceneSessionImpl->windowSystemConfig_.maxFloatingWindowSize_ = UINT32_MAX;
    windowSceneSessionImpl->UpdateWindowState();

    windowSceneSessionImpl->windowSystemConfig_.maxFloatingWindowSize_ = 1920;
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetIsNeedUpdateWindowMode(true);
    windowSceneSessionImpl->UpdateWindowState();
    EXPECT_EQ(1920, windowSceneSessionImpl->maxFloatingWindowSize_);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    auto ret = windowSceneSessionImpl->property_->GetIsNeedUpdateWindowMode();
    EXPECT_EQ(false, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetIsNeedUpdateWindowMode(false);
    windowSceneSessionImpl->UpdateWindowState();
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetDragEnabled(true);
    windowSceneSessionImpl->UpdateWindowState();
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetDragEnabled(false);
    windowSceneSessionImpl->UpdateWindowState();
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetDragEnabled(true);
    windowSceneSessionImpl->UpdateWindowState();
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetDragEnabled(false);
    windowSceneSessionImpl->UpdateWindowState();
}

/**
 * @tc.name: Resize
 * @tc.desc: Resize
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, Resize, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("Resize");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    auto ret = windowSceneSessionImpl->Resize(100, 100);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    ret = windowSceneSessionImpl->Resize(100, 100);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    ret = windowSceneSessionImpl->Resize(100, 100);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: ResetAspectRatio
 * @tc.desc: ResetAspectRatio
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, ResetAspectRatio, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("ResetAspectRatio");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    windowSceneSessionImpl->hostSession_ = nullptr;
    auto ret = windowSceneSessionImpl->ResetAspectRatio();
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_CREATED;
    ret = windowSceneSessionImpl->ResetAspectRatio();
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: GetAvoidAreaByType
 * @tc.desc: GetAvoidAreaByType
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, GetAvoidAreaByType, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("GetAvoidAreaByType");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSessionImpl->hostSession_ = session;
    AvoidArea avoidArea;
    auto ret = windowSceneSessionImpl->GetAvoidAreaByType(AvoidAreaType::TYPE_CUTOUT, avoidArea);
    EXPECT_EQ(WMError::WM_OK, ret);
    windowSceneSessionImpl->hostSession_ = nullptr;
    ret = windowSceneSessionImpl->GetAvoidAreaByType(AvoidAreaType::TYPE_CUTOUT, avoidArea);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: IsLayoutFullScreen
 * @tc.desc: IsLayoutFullScreen
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, IsLayoutFullScreen, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("IsLayoutFullScreen");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    auto ret = windowSceneSessionImpl->IsLayoutFullScreen();
    EXPECT_EQ(false, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ret = windowSceneSessionImpl->IsLayoutFullScreen();
    EXPECT_EQ(false, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ret = windowSceneSessionImpl->IsLayoutFullScreen();
    EXPECT_EQ(false, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_SUB_WINDOW_END);
    ret = windowSceneSessionImpl->IsLayoutFullScreen();
    EXPECT_EQ(false, ret);
}

/**
 * @tc.name: MaximizeFloating
 * @tc.desc: MaximizeFloating
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, MaximizeFloating, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("MaximizeFloating");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    auto ret = windowSceneSessionImpl->MaximizeFloating();
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    windowSceneSessionImpl->SetGlobalMaximizeMode(MaximizeMode::MODE_AVOID_SYSTEM_BAR);
    ret = windowSceneSessionImpl->MaximizeFloating();
    EXPECT_EQ(WMError::WM_OK, ret);
    auto ret1 = windowSceneSessionImpl->GetGlobalMaximizeMode();
    EXPECT_EQ(MaximizeMode::MODE_RECOVER, ret1);
    windowSceneSessionImpl->SetGlobalMaximizeMode(MaximizeMode::MODE_FULL_FILL);
    ret = windowSceneSessionImpl->MaximizeFloating();
    EXPECT_EQ(WMError::WM_OK, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetModeSupportInfo(WINDOW_MODE_SUPPORT_FLOATING);
    ret = windowSceneSessionImpl->MaximizeFloating();
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    ret1 = windowSceneSessionImpl->GetGlobalMaximizeMode();
    EXPECT_EQ(MaximizeMode::MODE_RECOVER, ret1);
    windowSceneSessionImpl->hostSession_ = nullptr;
    ret1 = windowSceneSessionImpl->GetGlobalMaximizeMode();
    EXPECT_EQ(MaximizeMode::MODE_RECOVER, ret1);
}

/**
 * @tc.name: Recover
 * @tc.desc: Recover
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, Recover, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("Recover");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetMaximizeMode(MaximizeMode::MODE_RECOVER);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    auto ret = windowSceneSessionImpl->Recover();
    EXPECT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    ret = windowSceneSessionImpl->Recover();
    EXPECT_EQ(WMError::WM_OK, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetMaximizeMode(MaximizeMode::MODE_FULL_FILL);
    ret = windowSceneSessionImpl->Recover();
    EXPECT_EQ(WMError::WM_OK, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    ret = windowSceneSessionImpl->Recover();
    EXPECT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ret = windowSceneSessionImpl->Recover();
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);
}
}
} // namespace Rosen
} // namespace OHOS