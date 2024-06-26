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

    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    windowSceneSessionImpl->PerformBack();

    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
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

    windowSceneSessionImpl->property_ = nullptr;
    ret = windowSceneSessionImpl->AdjustKeyboardLayout(params);
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

    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    auto ret = windowSceneSessionImpl->NotifyPrepareClosePiPWindow();
    EXPECT_EQ(WMError::WM_DO_NOTHING, ret);

    EXPECT_EQ(WmErrorCode::WM_OK, windowSceneSessionImpl->KeepKeyboardOnFocus(true));
    windowSceneSessionImpl->property_ = nullptr;
    EXPECT_EQ(WmErrorCode::WM_ERROR_STATE_ABNORMALLY, windowSceneSessionImpl->KeepKeyboardOnFocus(true));
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

    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_TOAST);
    windowSceneSessionImpl->SetDefaultProperty();
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
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ret = windowSceneSessionImpl->NotifyPrepareClosePiPWindow();
    EXPECT_EQ(WMError::WM_DO_NOTHING, ret);
}

/*
 * @tc.name: SetShadowOffsetY
 * @tc.desc: SetShadowOffsetY test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, SetShadowOffsetY, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("SetShadowOffsetY");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);

    auto surfaceNode = window->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->CheckParmAndPermission());
    } else {
        ASSERT_EQ(WMError::WM_OK, window->SetShadowOffsetY(1.0));
    }
}

/**
 * @tc.name: TestGetUIContentWithId
 * @tc.desc: Get uicontent with id
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, TestGetUIContentWithId, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("TestGetUIContentWithId");
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(666);
 
    SessionInfo sessionInfo = { "CreateTestBundle", "TestGetUIContentWithId", "CreateTestAbility" };
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_OK, window->Create(nullptr, session));
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->Destroy(true);
}

/**
 * @tc.name: NotifyDisplayInfoChange
 * @tc.desc: NotifyDisplayInfoChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, NotifyDisplayInfoChange, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("NotifyDisplayInfoChange");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->NotifyDisplayInfoChange();
}

/**
 * @tc.name: RecoverAndConnectSpecificSession
 * @tc.desc: RecoverAndConnectSpecificSession
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, RecoverAndConnectSpecificSession, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("RecoverAndConnectSpecificSession");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    auto ret = windowSceneSessionImpl->RecoverAndConnectSpecificSession();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_SUB_WINDOW_END);
    ret = windowSceneSessionImpl->RecoverAndConnectSpecificSession();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    windowSceneSessionImpl->property_ = nullptr;
    ret = windowSceneSessionImpl->RecoverAndConnectSpecificSession();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: Create
 * @tc.desc: Create
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, Create, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("Create");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    std::string identityToken = "testToken";
    auto ret = windowSceneSessionImpl->Create(abilityContext_, session, identityToken);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowSceneSessionImpl->Destroy(true));
    windowSceneSessionImpl->property_ = nullptr;
    ret = windowSceneSessionImpl->Create(abilityContext_, session, identityToken);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowSceneSessionImpl->Destroy(true));
}

/**
 * @tc.name: UpdateConfigVal
 * @tc.desc: UpdateConfigVal
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, UpdateConfigVal, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("UpdateConfigVal");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    auto ret = windowSceneSessionImpl->UpdateConfigVal(0, 0, 0, 0, 0.0f);
    ASSERT_EQ(0, ret);
    ret = windowSceneSessionImpl->UpdateConfigVal(0, 2, 1, 0, 1.0f);
    ASSERT_EQ(1, ret);
}

/**
 * @tc.name: MoveTo02
 * @tc.desc: MoveTo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, MoveTo02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("MoveTo02");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    windowSceneSessionImpl->hostSession_ = nullptr;
    auto ret = windowSceneSessionImpl->MoveTo(0, 0);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->state_ = WindowState::STATE_INITIAL;
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    ret = windowSceneSessionImpl->MoveTo(0, 0);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);
}

/**
 * @tc.name: TransformSurfaceNode
 * @tc.desc: TransformSurfaceNode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, TransformSurfaceNode, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("TransformSurfaceNode");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    windowSceneSessionImpl->property_->SetAnimationFlag(0);
    Transform trans = windowSceneSessionImpl->property_->GetTransform();
    windowSceneSessionImpl->TransformSurfaceNode(trans);
    windowSceneSessionImpl->property_->SetAnimationFlag(3);
    windowSceneSessionImpl->TransformSurfaceNode(trans);
    windowSceneSessionImpl->surfaceNode_ = nullptr;
    windowSceneSessionImpl->TransformSurfaceNode(trans);
    std::vector<std::vector<uint32_t>> windowMask;
    auto ret = windowSceneSessionImpl->HandleWindowMask(windowMask);
    ASSERT_EQ(nullptr, ret);
}

/**
 * @tc.name: SetTextFieldAvoidInfo
 * @tc.desc: SetTextFieldAvoidInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, SetTextFieldAvoidInfo, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("SetTextFieldAvoidInfo");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    auto ret = windowSceneSessionImpl->SetTextFieldAvoidInfo(0, 0);
    ASSERT_EQ(WMError::WM_OK, ret);
    sptr<DisplayInfo> displayInfo = nullptr;
    WindowLimits windowLimits = {1000, 1000, 1000, 1000, 0.0f, 0.0f};
    windowSceneSessionImpl->fillWindowLimits(windowLimits);
    ASSERT_NEAR(1.0f, windowSceneSessionImpl->GetVirtualPixelRatio(displayInfo), 0.001f);
}

/**
 * @tc.name: NotifyDialogStateChange
 * @tc.desc: NotifyDialogStateChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, NotifyDialogStateChange, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("NotifyDialogStateChange");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle0", "CreateTestModule0", "CreateTestAbility0" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    auto ret = windowSceneSessionImpl->NotifyDialogStateChange(true);
    ASSERT_EQ(WSError::WS_OK, ret);
    ret = windowSceneSessionImpl->NotifyDialogStateChange(false);
    ASSERT_EQ(WSError::WS_OK, ret);
    windowSceneSessionImpl->state_ = WindowState::STATE_HIDDEN;
    ret = windowSceneSessionImpl->NotifyDialogStateChange(true);
    ASSERT_EQ(WSError::WS_OK, ret);
    ret = windowSceneSessionImpl->NotifyDialogStateChange(false);
    ASSERT_EQ(WSError::WS_OK, ret);
    windowSceneSessionImpl->hostSession_ = nullptr;
    ret = windowSceneSessionImpl->NotifyDialogStateChange(false);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_WINDOW, ret);
    windowSceneSessionImpl->property_ = nullptr;
    ret = windowSceneSessionImpl->NotifyDialogStateChange(false);
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: UpdateNewSize
 * @tc.desc: UpdateNewSize
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, UpdateNewSize, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("UpdateNewSize");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    windowSceneSessionImpl->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    WindowLimits windowLimits = {1000, 1000, 1000, 1000, 0.0f, 0.0f};
    windowSceneSessionImpl->property_->SetWindowLimits(windowLimits);
    Rect rect = {800, 800, 1200, 1200};
    windowSceneSessionImpl->property_->SetWindowRect(rect);
    windowSceneSessionImpl->UpdateNewSize();
    Rect rect1 = {1200, 1200, 800, 800};
    windowSceneSessionImpl->property_->SetWindowRect(rect1);
    windowSceneSessionImpl->UpdateNewSize();
    auto ret = windowSceneSessionImpl->SwitchFreeMultiWindow(true);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_WINDOW, ret);
    ret = windowSceneSessionImpl->SwitchFreeMultiWindow(false);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_WINDOW, ret);
}

/**
 * @tc.name: SwitchFreeMultiWindow
 * @tc.desc: SwitchFreeMultiWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, SwitchFreeMultiWindow, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("SwitchFreeMultiWindow");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    windowSceneSessionImpl->UpdateNewSize();
    windowSceneSessionImpl->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle0", "CreateTestModule0", "CreateTestAbility0" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    auto ret = windowSceneSessionImpl->SwitchFreeMultiWindow(true);
    ASSERT_EQ(WSError::WS_OK, ret);
    ret = windowSceneSessionImpl->SwitchFreeMultiWindow(false);
    ASSERT_EQ(WSError::WS_OK, ret);
}

/**
 * @tc.name: UpdateWindowState
 * @tc.desc: UpdateWindowState
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest3, UpdateWindowState, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("UpdateWindowState");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    windowSceneSessionImpl->UpdateWindowState();
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    windowSceneSessionImpl->windowSystemConfig_.maxFloatingWindowSize_ = UINT32_MAX;
    windowSceneSessionImpl->property_->SetIsNeedUpdateWindowMode(false);
    windowSceneSessionImpl->UpdateWindowState();
    windowSceneSessionImpl->property_->SetIsNeedUpdateWindowMode(true);
    windowSceneSessionImpl->UpdateWindowState();
    auto ret = windowSceneSessionImpl->property_->GetIsNeedUpdateWindowMode();
    windowSceneSessionImpl->windowSystemConfig_.maxFloatingWindowSize_ = 0;
    windowSceneSessionImpl->UpdateWindowState();
    ASSERT_EQ(false, ret);
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
    option->SetDisplayId(0);
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
    Rect request = { 100, 100, 0, 100 };
    window->property_->SetRequestRect(request);
    window->PreLayoutOnShow(window->property_->GetWindowType());
    Rect request = { 100, 100, 100, 0 };
    window->property_->SetRequestRect(request);
    window->PreLayoutOnShow(window->property_->GetWindowType());
}
}
} // namespace Rosen
} // namespace OHOS