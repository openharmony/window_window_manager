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
    windowMask = {{1, 1, 1}, {2, 2, 2}, {3, 3, 3}};
    ret = windowSceneSessionImpl->SetWindowMask(windowMask);
    EXPECT_EQ(WMError::WM_OK, ret);
    EXPECT_FALSE(nullptr == windowSceneSessionImpl->HandleWindowMask(windowMask));
    Rect rect = {9, 9, 9, 9};
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetRequestRect(rect);
    EXPECT_TRUE(nullptr == windowSceneSessionImpl->HandleWindowMask(windowMask));
    rect = {3, 3, 3, 3};
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetRequestRect(rect);
    EXPECT_FALSE(nullptr == windowSceneSessionImpl->HandleWindowMask(windowMask));
    rect = {2, 2, 2, 2};
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetRequestRect(rect);
    EXPECT_TRUE(nullptr == windowSceneSessionImpl->HandleWindowMask(windowMask));
    rect = {0, 0, 0, 0};
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetRequestRect(rect);
    EXPECT_FALSE(nullptr == windowSceneSessionImpl->HandleWindowMask(windowMask));
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
    EXPECT_EQ(WMError::WM_OK, ret);

    windowSceneSessionImpl->hostSession_ = nullptr;
    ret = windowSceneSessionImpl->AdjustKeyboardLayout(params);
    EXPECT_EQ(WMError::WM_OK, ret);
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
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARENT, ret);

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetParentPersistentId(6);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    windowSceneSessionImpl->hostSession_ = nullptr;
    ret = windowSceneSessionImpl->RaiseToAppTop();
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
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
 * @tc.name: SetWindowLimits01
 * @tc.desc: SetWindowLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, SetWindowLimits01, Function | SmallTest | Level2)
{
    sptr<WindowOption> subWindow = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, subWindow);
    subWindow->SetWindowName("SetWindowLimits01");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(subWindow);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    WindowLimits windowLimits = {1000, 1000, 1000, 1000, 0.0f, 0.0f};
    windowSceneSessionImpl->SetWindowLimits(windowLimits);
    windowSceneSessionImpl->property_->SetPersistentId(1004);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> subSession = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, subSession);
    windowSceneSessionImpl->hostSession_ = subSession;
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_SUB_WINDOW_END);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_CALLING, windowSceneSessionImpl->SetWindowLimits(windowLimits));
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
    type = WindowType::WINDOW_TYPE_HANDWRITE;
    EXPECT_FALSE(!windowSceneSessionImpl->IsValidSystemWindowType(type));
    type = WindowType::WINDOW_TYPE_NEGATIVE_SCREEN;
    EXPECT_TRUE(!windowSceneSessionImpl->IsValidSystemWindowType(type));
    type = WindowType::WINDOW_TYPE_THEME_EDITOR;
    EXPECT_TRUE(!windowSceneSessionImpl->IsValidSystemWindowType(type));
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
 * @tc.name: UpdateFloatingWindowSizeBySizeLimits01
 * @tc.desc: UpdateFloatingWindowSizeBySizeLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, UpdateFloatingWindowSizeBySizeLimits01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("UpdateFloatingWindowSizeBySizeLimits01");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    uint32_t maxWidth = 32;
    WindowLimits windowLimits = {1, 1, 1, 1, 0.0f, 2.0f};
    windowSceneSessionImpl->property_->SetWindowLimits(windowLimits);
    windowSceneSessionImpl->UpdateFloatingWindowSizeBySizeLimits(maxWidth, maxWidth);
    WindowLimits windowLimits1 = {1, 2, 2, 2, 0.0f, 0.0f};
    windowSceneSessionImpl->property_->SetWindowLimits(windowLimits1);
    windowSceneSessionImpl->UpdateFloatingWindowSizeBySizeLimits(maxWidth, maxWidth);
    WindowLimits windowLimits2 = {1, 2, 2, 2, 0.0f, 2.0f};
    windowSceneSessionImpl->property_->SetWindowLimits(windowLimits2);
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
 * @tc.name: Resize01
 * @tc.desc: Resize
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, Resize01, Function | SmallTest | Level2)
{
    sptr<WindowOption> subOption = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, subOption);
    subOption->SetWindowName("Resize01SubWindow");
    subOption->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowSceneSessionImpl> subWindow = new (std::nothrow) WindowSceneSessionImpl(subOption);
    ASSERT_NE(nullptr, subWindow);
    ASSERT_NE(nullptr, subWindow->property_);
    subWindow->property_->SetPersistentId(1002);
    SessionInfo subSessionInfo = {"CreateSubTestBundle", "CreateSubTestModule", "CreateSubTestAbility"};
    sptr<SessionMocker> subSession = new (std::nothrow) SessionMocker(subSessionInfo);
    ASSERT_NE(nullptr, subSession);
    subWindow->hostSession_ = subSession;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_OPERATION, subWindow->Resize(100, 100));
    subWindow->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    subWindow->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("Resize01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    ASSERT_NE(nullptr, window->property_);
    window->property_->SetPersistentId(1003);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    Rect request = {100, 100, 100, 100};
    subWindow->property_->SetRequestRect(request);
    subWindow->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    ASSERT_EQ(WMError::WM_OK, subWindow->Resize(100, 100));
    WindowSceneSessionImpl::windowSessionMap_.insert(std::make_pair(window->GetWindowName(),
    std::pair<uint64_t, sptr<WindowSessionImpl>>(window->GetWindowId(), window)));
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    ASSERT_EQ(WMError::WM_OK, subWindow->Resize(100, 100));
    ASSERT_EQ(WMError::WM_OK, subWindow->Resize(200, 200));
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    ASSERT_EQ(WMError::WM_OK, subWindow->Resize(100, 200));
    ASSERT_EQ(WMError::WM_OK, subWindow->Resize(200, 200));
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

/**
 * @tc.name: StartMove
 * @tc.desc: StartMove
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, StartMove, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("StartMove");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    windowSceneSessionImpl->hostSession_ = nullptr;
    windowSceneSessionImpl->StartMove();
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    windowSceneSessionImpl->windowSystemConfig_.freeMultiWindowSupport_ = true;
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    windowSceneSessionImpl->StartMove();
}

/**
 * @tc.name: DisableAppWindowDecor
 * @tc.desc: DisableAppWindowDecor
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, DisableAppWindowDecor, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("DisableAppWindowDecor");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    auto ret = windowSceneSessionImpl->DisableAppWindowDecor();
    EXPECT_EQ(WMError::WM_OK, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ret = windowSceneSessionImpl->DisableAppWindowDecor();
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);
}

/**
 * @tc.name: SetShadowRadius
 * @tc.desc: SetShadowRadius
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, SetShadowRadius, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("SetShadowRadius");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    windowSceneSessionImpl->surfaceNode_ = nullptr;
    auto ret = windowSceneSessionImpl->SetShadowRadius(1.0f);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    ret = windowSceneSessionImpl->SetShadowOffsetY(1.0f);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);

    Transform trans;
    windowSceneSessionImpl->hostSession_ = nullptr;
    ret = windowSceneSessionImpl->SetTransform(trans);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    ret = windowSceneSessionImpl->SetTransform(trans);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: UpdateMaximizeMode
 * @tc.desc: UpdateMaximizeMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, UpdateMaximizeMode, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("UpdateMaximizeMode");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    windowSceneSessionImpl->uiContent_ = nullptr;
    auto ret = windowSceneSessionImpl->UpdateMaximizeMode(MaximizeMode::MODE_RECOVER);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_PARAM, ret);
    windowSceneSessionImpl->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_NE(nullptr, windowSceneSessionImpl->uiContent_);
    ret = windowSceneSessionImpl->UpdateMaximizeMode(MaximizeMode::MODE_RECOVER);
    EXPECT_EQ(WSError::WS_OK, ret);
}

/**
 * @tc.name: GetSystemBarProperties
 * @tc.desc: GetSystemBarProperties
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, GetSystemBarProperties, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("GetSystemBarProperties");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    std::map<WindowType, SystemBarProperty> properties;

    auto ret = windowSceneSessionImpl->GetSystemBarProperties(properties);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: Hide
 * @tc.desc: Hide
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, Hide, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("Hide");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    windowSceneSessionImpl->hostSession_ = nullptr;
    auto ret = windowSceneSessionImpl->Hide(2, false, false);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSessionImpl->hostSession_ = session;
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(0);
    ret = windowSceneSessionImpl->Hide(2, false, false);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ret = windowSceneSessionImpl->Hide(2, false, false);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
}

/**
 * @tc.name: IsSessionMainWindow
 * @tc.desc: IsSessionMainWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, IsSessionMainWindow, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("IsSessionMainWindow");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    sptr<WindowSessionImpl> windowSession = sptr<WindowSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSession);
    ASSERT_NE(nullptr, windowSession->property_);
    windowSession->property_->SetPersistentId(1);
    ASSERT_NE(nullptr, windowSession->property_);
    windowSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    windowSceneSessionImpl->windowSessionMap_.insert(std::make_pair("window1", std::make_pair(1, windowSession)));
    auto ret = windowSceneSessionImpl->IsSessionMainWindow(1);
    EXPECT_EQ(true, ret);
    windowSceneSessionImpl->windowSessionMap_.insert(std::make_pair("window1", std::make_pair(1, windowSession)));
    ASSERT_NE(nullptr, windowSession->property_);
    windowSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ret = windowSceneSessionImpl->IsSessionMainWindow(1);
    EXPECT_EQ(false, ret);
    ret = windowSceneSessionImpl->IsSessionMainWindow(0);
    EXPECT_EQ(false, ret);
    windowSession = nullptr;
    ret = windowSceneSessionImpl->IsSessionMainWindow(0);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.name: Show
 * @tc.desc: Show
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, Show, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("Show");
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
    auto ret = windowSceneSessionImpl->Show(2, true);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = windowSceneSessionImpl->Maximize();
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = windowSceneSessionImpl->Minimize();
    EXPECT_EQ(WMError::WM_OK, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ret = windowSceneSessionImpl->Minimize();
    EXPECT_EQ(WMError::WM_DO_NOTHING, ret);
    ret = windowSceneSessionImpl->Maximize();
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = windowSceneSessionImpl->Minimize();
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
}

/**
 * @tc.name: Show
 * @tc.desc: Show01
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, Show01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("Show01");
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
    auto ret = windowSceneSessionImpl->Show(2, true);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: NotifySpecificWindowSessionProperty
 * @tc.desc: NotifySpecificWindowSessionProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, NotifySpecificWindowSessionProperty, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("NotifySpecificWindowSessionProperty");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    WindowType type = WindowType::WINDOW_TYPE_STATUS_BAR;
    SystemBarProperty property;
    windowSceneSessionImpl->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    auto ret = windowSceneSessionImpl->NotifySpecificWindowSessionProperty(type, property);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = windowSceneSessionImpl->SetWindowFlags(0);
    EXPECT_EQ(WMError::WM_OK, ret);
    type = WindowType::WINDOW_TYPE_NAVIGATION_BAR;
    ret = windowSceneSessionImpl->NotifySpecificWindowSessionProperty(type, property);
    EXPECT_EQ(WMError::WM_OK, ret);
    type = WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR;
    ret = windowSceneSessionImpl->NotifySpecificWindowSessionProperty(type, property);
    EXPECT_EQ(WMError::WM_OK, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ret = windowSceneSessionImpl->SetSpecificBarProperty(type, property);
    EXPECT_EQ(WMError::WM_OK, ret);
    windowSceneSessionImpl->hostSession_ = nullptr;
    ret = windowSceneSessionImpl->SetWindowFlags(0);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
}

/**
 * @tc.name: Close
 * @tc.desc: Close
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, Close, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("Close");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    windowSceneSessionImpl->hostSession_ = nullptr;
    auto ret = windowSceneSessionImpl->Close();
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    ASSERT_NE(nullptr, abilityContext_);
    windowSceneSessionImpl->context_ = abilityContext_;
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    ret = windowSceneSessionImpl->Close();
    EXPECT_EQ(WMError::WM_OK, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ret = windowSceneSessionImpl->Close();
    EXPECT_EQ(WMError::WM_OK, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    ret = windowSceneSessionImpl->Close();
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ret = windowSceneSessionImpl->Close();
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
}

/**
 * @tc.name: GetWindowWithId
 * @tc.desc: GetWindowWithId
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, GetWindowWithId, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("GetWindowWithId");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    sptr<WindowSessionImpl> windowSession = sptr<WindowSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSession);
    ASSERT_NE(nullptr, windowSession->property_);
    windowSession->property_->SetPersistentId(1);
    ASSERT_NE(nullptr, windowSession->property_);
    windowSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    windowSceneSessionImpl->windowSessionMap_.insert(std::make_pair("window1", std::make_pair(1, windowSession)));
    auto ret = windowSceneSessionImpl->GetWindowWithId(1);
    EXPECT_FALSE(ret != nullptr);
    ret = windowSceneSessionImpl->GetWindowWithId(0);
    EXPECT_FALSE(ret == nullptr);
    windowSession = nullptr;
    ret = windowSceneSessionImpl->GetWindowWithId(0);
    EXPECT_FALSE(ret == nullptr);
}

/**
 * @tc.name: PreNotifyKeyEvent
 * @tc.desc: PreNotifyKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, PreNotifyKeyEvent, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("PreNotifyKeyEvent");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    std::shared_ptr<MMI::KeyEvent> keyEvent = nullptr;
    windowSceneSessionImpl->uiContent_ = nullptr;
    auto ret = windowSceneSessionImpl->PreNotifyKeyEvent(keyEvent);
    EXPECT_EQ(false, ret);
    windowSceneSessionImpl->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_NE(nullptr, windowSceneSessionImpl->uiContent_);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.name: Recover01
 * @tc.desc: Recover
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, Recover01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("Recover");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    windowSceneSessionImpl->hostSession_ = nullptr;
    auto ret = windowSceneSessionImpl->Recover(1);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    windowSceneSessionImpl->windowSystemConfig_.isPhoneWindow_ = true;
    ret = windowSceneSessionImpl->Recover(1);
    EXPECT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, ret);
    windowSceneSessionImpl->windowSystemConfig_.freeMultiWindowEnable_ = true;
    ret = windowSceneSessionImpl->Recover(1);
    EXPECT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, ret);
    windowSceneSessionImpl->windowSystemConfig_.freeMultiWindowSupport_ = true;
    windowSceneSessionImpl->windowSystemConfig_.isPcWindow_ = true;
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetMaximizeMode(MaximizeMode::MODE_RECOVER);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    ret = windowSceneSessionImpl->Recover(1);
    EXPECT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetMaximizeMode(MaximizeMode::MODE_AVOID_SYSTEM_BAR);
    ret = windowSceneSessionImpl->Recover(1);
    EXPECT_EQ(WMError::WM_OK, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetMaximizeMode(MaximizeMode::MODE_FULL_FILL);
    ret = windowSceneSessionImpl->Recover(1);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = windowSceneSessionImpl->Recover(0);
    EXPECT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ret = windowSceneSessionImpl->Recover(0);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);
}

/**
 * @tc.name: FindParentSessionByParentId
 * @tc.desc: FindParentSessionByParentId
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, FindParentSessionByParentId, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("FindParentSessionByParentId");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    sptr<WindowSessionImpl> windowSession = sptr<WindowSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSession);
    ASSERT_NE(nullptr, windowSession->property_);
    windowSession->property_->SetPersistentId(1);

    ASSERT_NE(nullptr, windowSession->property_);
    windowSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    windowSceneSessionImpl->windowSessionMap_.insert(std::make_pair("window1", std::make_pair(1, windowSession)));
    EXPECT_FALSE(nullptr != windowSceneSessionImpl->FindParentSessionByParentId(1));
    ASSERT_NE(nullptr, windowSession->property_);
    windowSession->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ASSERT_NE(nullptr, windowSession->property_);
    windowSession->property_->SetParentPersistentId(1);
    EXPECT_FALSE(nullptr != windowSceneSessionImpl->FindParentSessionByParentId(1));
    ASSERT_NE(nullptr, windowSession->property_);
    windowSession->property_->SetParentPersistentId(0);
    ASSERT_NE(nullptr, windowSession->property_);
    windowSession->property_->SetExtensionFlag(true);
    EXPECT_FALSE(nullptr != windowSceneSessionImpl->FindParentSessionByParentId(1));
}

/**
 * @tc.name: PreLayoutOnShow
 * @tc.desc: PreLayoutOnShow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, PreLayoutOnShow, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("PreLayoutOnShow");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    ASSERT_NE(nullptr, window->property_);
    window->property_->SetPersistentId(1);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    window->hostSession_ = session;
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_NE(nullptr, window->uiContent_);
    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);

    Rect request = { 100, 100, 100, 100 };
    window->property_->SetRequestRect(request);
    window->PreLayoutOnShow(window->property_->GetWindowType());
    request = { 100, 100, 0, 100 };
    window->property_->SetRequestRect(request);
    window->PreLayoutOnShow(window->property_->GetWindowType());
    request = { 100, 100, 100, 0 };
    window->property_->SetRequestRect(request);
    window->PreLayoutOnShow(window->property_->GetWindowType());
}

/**
 * @tc.name: InitSystemSessionEnableDrag
 * @tc.desc: InitSystemSessionEnableDrag Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, InitSystemSessionEnableDrag, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("InitSystemSessionEnableDrag");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    ASSERT_NE(nullptr, window->property_);
    window->property_->SetPersistentId(1);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_GLOBAL_SEARCH);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    window->hostSession_ = session;
    window->InitSystemSessionEnableDrag();
}
}
} // namespace Rosen
} // namespace OHOS