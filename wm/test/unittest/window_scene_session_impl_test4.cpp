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
class WindowSceneSessionImplTest4 : public testing::Test {
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

void WindowSceneSessionImplTest4::SetUpTestCase() {}

void WindowSceneSessionImplTest4::TearDownTestCase() {}

void WindowSceneSessionImplTest4::SetUp()
{
    abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();
}

void WindowSceneSessionImplTest4::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    abilityContext_ = nullptr;
}

RSSurfaceNode::SharedPtr WindowSceneSessionImplTest4::CreateRSSurfaceNode()
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "startingWindowTestSurfaceNode";
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig, RSSurfaceNodeType::DEFAULT);
    return surfaceNode;
}

namespace {
/**
 * @tc.name: UpdateSurfaceNodeAfterCustomAnimation
 * @tc.desc: UpdateSurfaceNodeAfterCustomAnimation
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, UpdateSurfaceNodeAfterCustomAnimation, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("UpdateSurfaceNodeAfterCustomAnimation");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    windowSceneSessionImpl->hostSession_ = nullptr;
    auto ret = windowSceneSessionImpl->UpdateSurfaceNodeAfterCustomAnimation(true);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_END);
    ret = windowSceneSessionImpl->UpdateSurfaceNodeAfterCustomAnimation(true);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE);
    ret = windowSceneSessionImpl->UpdateSurfaceNodeAfterCustomAnimation(true);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: RegisterSessionRecoverListener
 * @tc.desc: RegisterSessionRecoverListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, RegisterSessionRecoverListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("RegisterSessionRecoverListener");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    windowSceneSessionImpl->RegisterSessionRecoverListener(true);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetCollaboratorType(CollaboratorType::RESERVE_TYPE);
    windowSceneSessionImpl->RegisterSessionRecoverListener(true);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetCollaboratorType(CollaboratorType::DEFAULT_TYPE);
    windowSceneSessionImpl->RegisterSessionRecoverListener(true);
    windowSceneSessionImpl->state_ = WindowState::STATE_DESTROYED;
    windowSceneSessionImpl->RegisterSessionRecoverListener(true);
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    windowSceneSessionImpl->RegisterSessionRecoverListener(true);
}

/**
 * @tc.name: ConsumePointerEvent
 * @tc.desc: ConsumePointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, ConsumePointerEvent, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("ConsumePointerEvent");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSessionImpl->hostSession_ = session;
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_NE(nullptr, pointerEvent);
    windowSceneSessionImpl->ConsumePointerEvent(pointerEvent);
    windowSceneSessionImpl->hostSession_ = nullptr;
    windowSceneSessionImpl->ConsumePointerEvent(pointerEvent);
    pointerEvent = nullptr;
    windowSceneSessionImpl->ConsumePointerEvent(pointerEvent);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetTurnScreenOn(true);
    auto ret = windowSceneSessionImpl->IsTurnScreenOn();
    EXPECT_EQ(true, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetTurnScreenOn(false);
    ret = windowSceneSessionImpl->IsTurnScreenOn();
    EXPECT_EQ(false, ret);
}

/**
 * @tc.name: GetSystemSizeLimits01
 * @tc.desc: GetSystemSizeLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, GetSystemSizeLimits01, Function | SmallTest | Level2)
{
    uint32_t minMainWidth = 10;
    uint32_t minMainHeight = 20;
    uint32_t minSubWidth = 30;
    uint32_t minSubHeight = 40;
    uint32_t displayWidth = 100;
    uint32_t displayHeight = 100;
    float displayVpr = 1.0f;
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("GetSystemSizeLimits01");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);
    windowSceneSessionImpl->windowSystemConfig_.miniWidthOfMainWindow_ = minMainWidth;
    windowSceneSessionImpl->windowSystemConfig_.miniHeightOfMainWindow_ = minMainHeight;
    windowSceneSessionImpl->windowSystemConfig_.miniWidthOfSubWindow_ = minSubWidth;
    windowSceneSessionImpl->windowSystemConfig_.miniHeightOfSubWindow_ = minSubHeight;

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    WindowLimits limits = windowSceneSessionImpl->GetSystemSizeLimits(displayWidth, displayHeight, displayVpr);
    EXPECT_EQ(limits.minWidth_, minMainWidth);
    EXPECT_EQ(limits.minHeight_, minMainHeight);

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    limits = windowSceneSessionImpl->GetSystemSizeLimits(displayWidth, displayHeight, displayVpr);
    EXPECT_EQ(limits.minWidth_, minSubWidth);
    EXPECT_EQ(limits.minHeight_, minSubHeight);

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    limits = windowSceneSessionImpl->GetSystemSizeLimits(displayWidth, displayHeight, displayVpr);
    EXPECT_EQ(limits.minWidth_, MIN_FLOATING_WIDTH);
    EXPECT_EQ(limits.minHeight_, MIN_FLOATING_HEIGHT);
}

/**
 * @tc.name: CalculateNewLimitsByRatio
 * @tc.desc: CalculateNewLimitsByRatio
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, CalculateNewLimitsByRatio, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("CalculateNewLimitsByRatio");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    WindowLimits newLimits = {0, 0, 0, 0, 0.0, 0.0};
    WindowLimits customizedLimits = {3, 3, 3, 3, 0.0, 0.0};
    windowSceneSessionImpl->CalculateNewLimitsByRatio(newLimits, customizedLimits);
    newLimits = {500, 500, 500, 500, 0.0, 0.0};
    customizedLimits = {3, 3, 3, 3, 2.0, 2.0};
    windowSceneSessionImpl->CalculateNewLimitsByRatio(newLimits, customizedLimits);
    customizedLimits = {3, 3, 3, 3, 1.0, 1.0};
    windowSceneSessionImpl->CalculateNewLimitsByRatio(newLimits, customizedLimits);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::BELOW_APP_SYSTEM_WINDOW_END);
    auto ret = windowSceneSessionImpl->UpdateAnimationFlagProperty(true);
    EXPECT_EQ(WMError::WM_OK, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::BELOW_APP_SYSTEM_WINDOW_BASE);
    ret = windowSceneSessionImpl->UpdateAnimationFlagProperty(true);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
}

/**
 * @tc.name: HandlePointDownEvent
 * @tc.desc: HandlePointDownEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, HandlePointDownEvent, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("HandlePointDownEvent");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    MMI::PointerEvent::PointerItem pointerItem;
    int32_t sourceType = 1;
    float vpr = 1.0f;
    WSRect rect = {1, 1, 1, 1};

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSessionImpl->hostSession_ = session;
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::BELOW_APP_SYSTEM_WINDOW_BASE);

    windowSceneSessionImpl->property_->SetDragEnabled(false);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    windowSceneSessionImpl->property_->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    auto ret = windowSceneSessionImpl->HandlePointDownEvent(pointerEvent, pointerItem, sourceType, vpr, rect);
    EXPECT_EQ(true, ret);

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetDragEnabled(false);
    ret = windowSceneSessionImpl->HandlePointDownEvent(pointerEvent, pointerItem, sourceType, vpr, rect);
    EXPECT_EQ(true, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);

    ret = windowSceneSessionImpl->HandlePointDownEvent(pointerEvent, pointerItem, sourceType, vpr, rect);
    EXPECT_EQ(true, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);

    pointerItem.SetWindowX(100);
    pointerItem.SetWindowY(100);
    ret = windowSceneSessionImpl->HandlePointDownEvent(pointerEvent, pointerItem, sourceType, vpr, rect);
    EXPECT_EQ(true, ret);
    pointerItem.SetWindowX(1);
    ret = windowSceneSessionImpl->HandlePointDownEvent(pointerEvent, pointerItem, sourceType, vpr, rect);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: UpdateWindowModeImmediately
 * @tc.desc: UpdateWindowModeImmediately
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, UpdateWindowModeImmediately, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("UpdateWindowModeImmediately");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    windowSceneSessionImpl->state_ = WindowState::STATE_CREATED;
    auto ret = windowSceneSessionImpl->UpdateWindowModeImmediately(WindowMode::WINDOW_MODE_FLOATING);
    EXPECT_EQ(WMError::WM_OK, ret);
    windowSceneSessionImpl->state_ = WindowState::STATE_HIDDEN;
    ret = windowSceneSessionImpl->UpdateWindowModeImmediately(WindowMode::WINDOW_MODE_FLOATING);
    EXPECT_EQ(WMError::WM_OK, ret);
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    windowSceneSessionImpl->hostSession_ = nullptr;
    ret = windowSceneSessionImpl->UpdateWindowModeImmediately(WindowMode::WINDOW_MODE_FLOATING);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->hostSession_ = session;
    ret = windowSceneSessionImpl->UpdateWindowModeImmediately(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = windowSceneSessionImpl->UpdateWindowModeImmediately(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    EXPECT_EQ(WMError::WM_OK, ret);
    windowSceneSessionImpl->state_ = WindowState::STATE_UNFROZEN;
    ret = windowSceneSessionImpl->UpdateWindowModeImmediately(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: AdjustWindowAnimationFlag
 * @tc.desc: AdjustWindowAnimationFlag
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, AdjustWindowAnimationFlag, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("AdjustWindowAnimationFlag");
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
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    windowSceneSessionImpl->animationTransitionController_ = sptr<IAnimationTransitionController>::MakeSptr();
    ASSERT_NE(nullptr, windowSceneSessionImpl->animationTransitionController_);
    windowSceneSessionImpl->AdjustWindowAnimationFlag(true);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    auto ret = windowSceneSessionImpl->property_->GetAnimationFlag();
    EXPECT_EQ(3, ret);
    windowSceneSessionImpl->animationTransitionController_ = nullptr;
    windowSceneSessionImpl->AdjustWindowAnimationFlag(true);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    ret = windowSceneSessionImpl->property_->GetAnimationFlag();
    EXPECT_EQ(1, ret);
    windowSceneSessionImpl->AdjustWindowAnimationFlag(false);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    ret = windowSceneSessionImpl->property_->GetAnimationFlag();
    EXPECT_EQ(0, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    windowSceneSessionImpl->AdjustWindowAnimationFlag(false);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    ret = windowSceneSessionImpl->property_->GetAnimationFlag();
    EXPECT_EQ(1, ret);
    windowSceneSessionImpl->enableDefaultAnimation_ = false;
    windowSceneSessionImpl->AdjustWindowAnimationFlag(false);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    ret = windowSceneSessionImpl->property_->GetAnimationFlag();
    EXPECT_EQ(0, ret);
}

/**
 * @tc.name: UpdateConfigurationForAll
 * @tc.desc: UpdateConfigurationForAll
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, UpdateConfigurationForAll, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("UpdateConfigurationForAll");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    std::shared_ptr<AppExecFwk::Configuration> configuration = std::make_shared<AppExecFwk::Configuration>();
    ASSERT_NE(nullptr, configuration);
    sptr<WindowSessionImpl> windowSession = sptr<WindowSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSession);
    SessionInfo sessionInfo = {"CreateTestBundle1", "CreateTestModule1", "CreateTestAbility1"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSession->hostSession_ = session;
    ASSERT_NE(nullptr, windowSession->property_);
    windowSession->property_->SetPersistentId(1);
    windowSession->state_ = WindowState::STATE_SHOWN;
    ASSERT_EQ(WMError::WM_OK, windowSession->Create(abilityContext_, session));
    windowSceneSessionImpl->UpdateConfigurationForAll(configuration);
    ASSERT_EQ(WMError::WM_OK, windowSession->Destroy(true));
}

/**
 * @tc.name: SetWindowFlags
 * @tc.desc: SetWindowFlags
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, SetWindowFlags, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("SetWindowFlags");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    windowSceneSessionImpl->SetWindowFlags(1);
    auto ret = windowSceneSessionImpl->SetWindowFlags(1);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = windowSceneSessionImpl->SetWindowFlags(2);
    EXPECT_EQ(WMError::WM_OK, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    ret = windowSceneSessionImpl->NotifyPrepareClosePiPWindow();
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: ConsumePointerEventInner
 * @tc.desc: ConsumePointerEventInner
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, ConsumePointerEventInner, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("ConsumePointerEventInner");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->hostSession_ = session;
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_NE(nullptr, pointerEvent);
    MMI::PointerEvent::PointerItem pointerItem;
    pointerEvent->SetPointerAction(10);
    windowSceneSessionImpl->ConsumePointerEventInner(pointerEvent, pointerItem);
    pointerEvent->SetPointerAction(4);
    windowSceneSessionImpl->ConsumePointerEventInner(pointerEvent, pointerItem);
    pointerEvent->SetPointerAction(2);
    windowSceneSessionImpl->ConsumePointerEventInner(pointerEvent, pointerItem);
}

/**
 * @tc.name: RegisterKeyboardPanelInfoChangeListener
 * @tc.desc: RegisterKeyboardPanelInfoChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, RegisterKeyboardPanelInfoChangeListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("RegisterKeyboardPanelInfoChangeListener");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    windowSceneSessionImpl->keyboardPanelInfoChangeListeners_ = sptr<IKeyboardPanelInfoChangeListener>::MakeSptr();
    ASSERT_NE(nullptr, windowSceneSessionImpl->keyboardPanelInfoChangeListeners_);
    sptr<IKeyboardPanelInfoChangeListener> listener = nullptr;
    auto ret = windowSceneSessionImpl->RegisterKeyboardPanelInfoChangeListener(listener);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);
}

/**
 * @tc.name: GetSystemBarPropertyByType
 * @tc.desc: GetSystemBarPropertyByType
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, GetSystemBarPropertyByType, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("GetSystemBarPropertyByType");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    windowSceneSessionImpl->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
}

/**
 * @tc.name: SetSpecificBarProperty
 * @tc.desc: SetSpecificBarProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, SetSpecificBarProperty, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("SetSpecificBarProperty");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    SystemBarProperty property;
    windowSceneSessionImpl->state_ = WindowState::STATE_INITIAL;
    auto type = WindowType::WINDOW_TYPE_STATUS_BAR;
    auto ret = windowSceneSessionImpl->SetSpecificBarProperty(type, property);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    windowSceneSessionImpl->state_ = WindowState::STATE_BOTTOM;
    ret = windowSceneSessionImpl->SetSpecificBarProperty(type, property);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    windowSceneSessionImpl->state_ = WindowState::STATE_CREATED;
    ret = windowSceneSessionImpl->SetSpecificBarProperty(type, property);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    property = SystemBarProperty();
    property.settingFlag_ = SystemBarSettingFlag::DEFAULT_SETTING;
    ret = windowSceneSessionImpl->SetSpecificBarProperty(type, property);
    EXPECT_EQ(WMError::WM_OK, ret);
    property.settingFlag_ = SystemBarSettingFlag::COLOR_SETTING;
    ret = windowSceneSessionImpl->SetSpecificBarProperty(type, property);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
}

/**
 * @tc.name: MoveToAsync01
 * @tc.desc: MoveToAsync
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest4, MoveToAsync01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("MoveToAsync01");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    option->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(WMError::WM_ERROR_OPER_FULLSCREEN_FAILED, window->MoveToAsync(10, 10));
}

/**
 * @tc.name: MoveToAsync02
 * @tc.desc: MoveToAsync
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest4, MoveToAsync02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("MoveToAsync02");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(998);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;

    sptr<WindowOption> subOption = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, subOption);
    subOption->SetWindowName("subMoveToAsync02");
    subOption->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    subOption->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowSceneSessionImpl> subWindow = new (std::nothrow) WindowSceneSessionImpl(subOption);
    ASSERT_NE(nullptr, subWindow);
    subWindow->property_->SetPersistentId(999);
    SessionInfo subSessionInfo = { "subCreateTestBundle", "subCreateTestModule", "subCreateTestAbility" };
    sptr<SessionMocker> subSession = new (std::nothrow) SessionMocker(subSessionInfo);
    ASSERT_NE(nullptr, subSession);
    subWindow->hostSession_ = subSession;
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveToAsync(10, 10));

    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));
    ASSERT_EQ(WMError::WM_OK, subWindow->Create(abilityContext_, subSession));
    subWindow->windowSessionMap_.insert(std::make_pair("MoveToAsync02", std::make_pair(998, window)));
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveToAsync(10, 10));
    window->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveToAsync(10, 10));
    window->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveToAsync(10, 10));
    Rect request = { 100, 100, 100, 100 };
    subWindow->property_->SetRequestRect(request);
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveToAsync(10, 10));
    window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    subWindow->state_ = WindowState::STATE_HIDDEN;
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveToAsync(10, 10));
    subWindow->state_ = WindowState::STATE_SHOWN;
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveToAsync(10, 10));
    ASSERT_EQ(WMError::WM_OK, subWindow->Destroy(true));
    ASSERT_EQ(WMError::WM_OK, window->Destroy(true));
}

/**
 * @tc.name: ResizeAsync01
 * @tc.desc: ResizeAsync
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest4, ResizeAsync01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("ResizeAsync01");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    option->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(WMError::WM_ERROR_OPER_FULLSCREEN_FAILED, window->ResizeAsync(500, 500));
}

/**
 * @tc.name: ResizeAsync02
 * @tc.desc: ResizeAsync
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest4, ResizeAsync02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("ResizeAsync02");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(991);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;

    sptr<WindowOption> subOption = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, subOption);
    subOption->SetWindowName("subResizeAsync02");
    subOption->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    subOption->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowSceneSessionImpl> subWindow = new (std::nothrow) WindowSceneSessionImpl(subOption);
    ASSERT_NE(nullptr, subWindow);
    subWindow->property_->SetPersistentId(992);
    SessionInfo subSessionInfo = { "subCreateTestBundle", "subCreateTestModule", "subCreateTestAbility" };
    sptr<SessionMocker> subSession = new (std::nothrow) SessionMocker(subSessionInfo);
    ASSERT_NE(nullptr, subSession);
    subWindow->hostSession_ = subSession;
    ASSERT_EQ(WMError::WM_OK, subWindow->ResizeAsync(500, 500));

    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));
    ASSERT_EQ(WMError::WM_OK, subWindow->Create(abilityContext_, subSession));
    subWindow->windowSessionMap_.insert(std::make_pair("ResizeAsync02", std::make_pair(998, window)));
    ASSERT_EQ(WMError::WM_OK, subWindow->ResizeAsync(500, 500));
    window->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    ASSERT_EQ(WMError::WM_OK, subWindow->ResizeAsync(500, 500));
    window->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    ASSERT_EQ(WMError::WM_OK, subWindow->ResizeAsync(500, 500));
    Rect request = { 100, 100, 600, 600 };
    subWindow->property_->SetRequestRect(request);
    ASSERT_EQ(WMError::WM_OK, subWindow->ResizeAsync(500, 500));
    window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    subWindow->state_ = WindowState::STATE_HIDDEN;
    ASSERT_EQ(WMError::WM_OK, subWindow->ResizeAsync(500, 500));
    subWindow->state_ = WindowState::STATE_SHOWN;
    ASSERT_EQ(WMError::WM_OK, subWindow->ResizeAsync(500, 500));
    ASSERT_EQ(WMError::WM_OK, subWindow->Destroy(true));
    ASSERT_EQ(WMError::WM_OK, window->Destroy(true));
}

/**
 * @tc.name: NotifyDialogStateChange
 * @tc.desc: NotifyDialogStateChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, NotifyDialogStateChange, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("NotifyDialogStateChange");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    auto ret = windowSceneSessionImpl->NotifyDialogStateChange(true);
    EXPECT_EQ(WSError::WS_OK, ret);
    ret = windowSceneSessionImpl->NotifyDialogStateChange(false);
    EXPECT_EQ(WSError::WS_OK, ret);
    ret = windowSceneSessionImpl->NotifyDialogStateChange(false);
    EXPECT_EQ(WSError::WS_OK, ret);
    ret = windowSceneSessionImpl->NotifyDialogStateChange(true);
    EXPECT_EQ(WSError::WS_OK, ret);
    windowSceneSessionImpl->property_->SetPersistentId(0);
    windowSceneSessionImpl->hostSession_ = nullptr;
    windowSceneSessionImpl->state_ = WindowState::STATE_DESTROYED;
    ret = windowSceneSessionImpl->NotifyDialogStateChange(true);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_WINDOW, ret);
}

/**
 * @tc.name: MoveTo02
 * @tc.desc: MoveTo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, MoveTo02, Function | SmallTest | Level2)
{
    sptr<WindowOption> subOption = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, subOption);
    subOption->SetWindowName("MoveTo02SubWindow");
    subOption->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowSceneSessionImpl> subWindow = sptr<WindowSceneSessionImpl>::MakeSptr(subOption);
    ASSERT_NE(nullptr, subWindow);
    ASSERT_NE(nullptr, subWindow->property_);
    subWindow->property_->SetPersistentId(1001);
    SessionInfo subSessionInfo = {"CreateSubTestBundle", "CreateSubTestModule", "CreateSubTestAbility"};
    sptr<SessionMocker> subSession = sptr<SessionMocker>::MakeSptr(subSessionInfo);
    ASSERT_NE(nullptr, subSession);
    subWindow->hostSession_ = subSession;
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(2, 2));

    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("MoveTo02");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, window);
    ASSERT_NE(nullptr, window->property_);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle2", "CreateTestModule2", "CreateTestAbility2"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_SHOWN;
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(3, 3));
    ASSERT_NE(nullptr, window->property_);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(3, 3));
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(3, 4));
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(4, 4));
    ASSERT_NE(nullptr, window->property_);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(4, 4));
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(4, 5));
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(5, 5));
    ASSERT_EQ(WMError::WM_OK, window->Destroy(true));
}

/**
 * @tc.name: GetWindowStatus01
 * @tc.desc: GetWindowStatus
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, GetWindowStatus01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("GetWindowStatus01");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;

    WindowStatus windowStatus;
    ASSERT_EQ(WMError::WM_OK, window->GetWindowStatus(windowStatus));
    ASSERT_EQ(WindowStatus::WINDOW_STATUS_UNDEFINED, windowStatus);
}

/**
 * @tc.name: GetWindowStatus02
 * @tc.desc: GetWindowStatus
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, GetWindowStatus02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("GetWindowStatus02");
    option->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;

    WindowStatus windowStatus;
    ASSERT_EQ(WMError::WM_OK, window->GetWindowStatus(windowStatus));
    ASSERT_EQ(WindowStatus::WINDOW_STATUS_SPLITSCREEN, windowStatus);
}

/**
 * @tc.name: GetWindowStatus03
 * @tc.desc: GetWindowStatus
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, GetWindowStatus03, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("GetWindowStatus03");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetDisplayId(0);
    option->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;

    WindowStatus windowStatus;
    ASSERT_EQ(WMError::WM_OK, window->GetWindowStatus(windowStatus));
    ASSERT_EQ(WindowStatus::WINDOW_STATUS_FLOATING, windowStatus);
    window->property_->SetMaximizeMode(MaximizeMode::MODE_AVOID_SYSTEM_BAR);
    ASSERT_EQ(WMError::WM_OK, window->GetWindowStatus(windowStatus));
    ASSERT_EQ(WindowStatus::WINDOW_STATUS_MAXIMIZE, windowStatus);
    window->state_ = WindowState::STATE_HIDDEN;
    ASSERT_EQ(WMError::WM_OK, window->GetWindowStatus(windowStatus));
    ASSERT_EQ(WindowStatus::WINDOW_STATUS_MINIMIZE, windowStatus);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    window->state_ = WindowState::STATE_SHOWN;
    ASSERT_EQ(WMError::WM_OK, window->GetWindowStatus(windowStatus));
    ASSERT_EQ(WindowStatus::WINDOW_STATUS_FULLSCREEN, windowStatus);
}

/**
 * @tc.name: VerifySubWindowLevel
 * @tc.desc: VerifySubWindowLevel Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, VerifySubWindowLevel, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    EXPECT_NE(nullptr, option);
    option->SetWindowName("VerifySubWindowLevel");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetDisplayId(0);
    option->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    EXPECT_NE(nullptr, window);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    EXPECT_NE(nullptr, session);
    window->hostSession_ = session;
    ASSERT_EQ(false, window->VerifySubWindowLevel(window->GetParentId()));
}

/**
 * @tc.name: MoveTo03
 * @tc.desc: MoveTo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, MoveTo03, Function | SmallTest | Level2)
{
    sptr<WindowOption> subOption = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, subOption);
    subOption->SetWindowName("MoveTo01SubWindow");
    subOption->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowSceneSessionImpl> subWindow = new (std::nothrow) WindowSceneSessionImpl(subOption);
    ASSERT_NE(nullptr, subWindow);
    ASSERT_NE(nullptr, subWindow->property_);
    subWindow->property_->SetPersistentId(1001);
    SessionInfo subSessionInfo = {"CreateSubTestBundle", "CreateSubTestModule", "CreateSubTestAbility"};
    sptr<SessionMocker> subSession = new (std::nothrow) SessionMocker(subSessionInfo);
    ASSERT_NE(nullptr, subSession);
    subWindow->hostSession_ = subSession;
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(2, 2));

    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("MoveTo02");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    ASSERT_NE(nullptr, window->property_);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    WindowSceneSessionImpl::windowSessionMap_.insert(std::make_pair(window->GetWindowName(),
        std::pair<uint64_t, sptr<WindowSessionImpl>>(window->GetWindowId(), window)));
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(3, 3));
    ASSERT_NE(nullptr, window->property_);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(4, 3));
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(4, 4));
    ASSERT_NE(nullptr, window->property_);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(5, 4));
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(5, 4));
    WindowSceneSessionImpl::windowSessionMap_.erase(window->GetWindowName());
}

/**
 * @tc.name: UpdateNewSize01
 * @tc.desc: UpdateNewSize
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, UpdateNewSize01, Function | SmallTest | Level2)
{
    sptr<WindowOption> subOption = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, subOption);
    subOption->SetWindowName("UpdateNewSize01SubWindow");
    sptr<WindowSceneSessionImpl> subWindow = new (std::nothrow) WindowSceneSessionImpl(subOption);
    ASSERT_NE(nullptr, subWindow);
    ASSERT_NE(nullptr, subWindow->property_);
    subWindow->property_->SetPersistentId(1003);
    SessionInfo subSessionInfo = {"CreateSubTestBundle", "CreateSubTestModule", "CreateSubTestAbility"};
    sptr<SessionMocker> subSession = new (std::nothrow) SessionMocker(subSessionInfo);
    ASSERT_NE(nullptr, subSession);
    subWindow->hostSession_ = subSession;
    subWindow->UpdateNewSize();
    Rect windowRect = { 0, 0, 0, 0 };
    WindowLimits windowLimits = { 0, 0, 0, 0, 0.0, 0, 0 };
    subWindow->property_->SetRequestRect(windowRect);
    subWindow->property_->SetWindowRect(windowRect);
    subWindow->property_->SetWindowLimits(windowLimits);
    subWindow->UpdateNewSize();
    windowRect.width_ = 10;
    windowRect.height_ = 10;
    subWindow->UpdateNewSize();
    windowRect.width_ = 0;
    windowRect.height_ = 0;
    ASSERT_NE(nullptr, subWindow->property_);
    subWindow->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    subWindow->UpdateNewSize();
    Rect windowRect1 = { 10, 10, 10, 10 };
    WindowLimits windowLimits1 = { 100, 100, 100, 100, 0.0, 0, 0 };
    subWindow->property_->SetRequestRect(windowRect1);
    subWindow->property_->SetWindowLimits(windowLimits1);
    subWindow->UpdateNewSize();
    Rect windowRect2 = { 200, 200, 200, 200 };
    subWindow->property_->SetRequestRect(windowRect2);
    subWindow->UpdateNewSize();
}

/**
 * @tc.name: UpdateSubWindowStateAndNotify01
 * @tc.desc: UpdateSubWindowStateAndNotify
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, UpdateSubWindowStateAndNotify01, Function | SmallTest | Level2)
{
    sptr<WindowOption> subOption = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, subOption);
    subOption->SetWindowName("UpdateSubWindowStateAndNotify01");
    subOption->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowSceneSessionImpl> subWindow = new (std::nothrow) WindowSceneSessionImpl(subOption);
    ASSERT_NE(nullptr, subWindow);
    ASSERT_NE(nullptr, subWindow->property_);
    subWindow->property_->SetPersistentId(1005);
    SessionInfo subSessionInfo = {"CreateSubTestBundle", "CreateSubTestModule", "CreateSubTestAbility"};
    sptr<SessionMocker> subSession = new (std::nothrow) SessionMocker(subSessionInfo);
    ASSERT_NE(nullptr, subSession);
    subWindow->hostSession_ = subSession;

    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("UpdateSubWindowStateAndNotify02");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    ASSERT_NE(nullptr, window->property_);
    window->property_->SetPersistentId(1006);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    std::vector<sptr<WindowSessionImpl>> vec;
    WindowSceneSessionImpl::subWindowSessionMap_.insert(std::pair<int32_t,
        std::vector<sptr<WindowSessionImpl>>>(1006, vec));
    subWindow->UpdateSubWindowStateAndNotify(1006, WindowState::STATE_HIDDEN);
    WindowSceneSessionImpl::subWindowSessionMap_[1006].push_back(subWindow);
    subWindow->state_ = WindowState::STATE_SHOWN;
    subWindow->UpdateSubWindowStateAndNotify(1006, WindowState::STATE_HIDDEN);
    subWindow->state_ = WindowState::STATE_HIDDEN;
    subWindow->UpdateSubWindowStateAndNotify(1006, WindowState::STATE_HIDDEN);
    subWindow->state_ = WindowState::STATE_SHOWN;
    subWindow->UpdateSubWindowStateAndNotify(1006, WindowState::STATE_SHOWN);
    subWindow->state_ = WindowState::STATE_SHOWN;
    subWindow->UpdateSubWindowStateAndNotify(1006, WindowState::STATE_SHOWN);
}

/**
 * @tc.name: SetWindowMode01
 * @tc.desc: SetWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest4, SetWindowMode01, Function | SmallTest | Level2)
{
    sptr<WindowOption> subOption = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, subOption);
    subOption->SetWindowName("SetWindowMode01");
    subOption->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowSceneSessionImpl> subWindow = new (std::nothrow) WindowSceneSessionImpl(subOption);
    ASSERT_NE(nullptr, subWindow);
    ASSERT_NE(nullptr, subWindow->property_);
    subWindow->property_->SetPersistentId(1007);
    SessionInfo subSessionInfo = {"CreateSubTestBundle", "CreateSubTestModule", "CreateSubTestAbility"};
    sptr<SessionMocker> subSession = new (std::nothrow) SessionMocker(subSessionInfo);
    ASSERT_NE(nullptr, subSession);
    subWindow->hostSession_ = subSession;
    subWindow->property_->SetModeSupportInfo(0);
    auto ret = subWindow->SetWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW_MODE_OR_SIZE, ret);
}

/**
 * @tc.name: PreLayoutOnShow01
 * @tc.desc: PreLayoutOnShow
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest4, PreLayoutOnShow01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("PreLayoutOnShow01");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(window->property_, nullptr);
    window->property_->SetPersistentId(2345);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = nullptr;
    sptr<DisplayInfo> displayInfo = sptr<DisplayInfo>::MakeSptr();
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_NE(nullptr, window->uiContent_);
    window->PreLayoutOnShow(WindowType::WINDOW_TYPE_APP_SUB_WINDOW, displayInfo);
    window->hostSession_ = session;
    window->PreLayoutOnShow(WindowType::WINDOW_TYPE_APP_SUB_WINDOW, displayInfo);
}

/**
 * @tc.name: KeepKeyboardOnFocus01
 * @tc.desc: KeepKeyboardOnFocus
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest4, KeepKeyboardOnFocus01, Function | SmallTest | Level2)
{
    sptr<WindowOption> keyboardOption = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, keyboardOption);
    keyboardOption->SetWindowName("KeepKeyboardOnFocus01");
    keyboardOption->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    sptr<WindowSceneSessionImpl> keyboardWindow = new (std::nothrow) WindowSceneSessionImpl(keyboardOption);
    ASSERT_NE(nullptr, keyboardWindow);

    keyboardWindow->KeepKeyboardOnFocus(false);
    ASSERT_EQ(keyboardWindow->property_->keepKeyboardFlag_, false);

    keyboardWindow->KeepKeyboardOnFocus(true);
    ASSERT_EQ(keyboardWindow->property_->keepKeyboardFlag_, true);
}

/**
 * @tc.name: MoveAndResizeKeyboard01
 * @tc.desc: MoveAndResizeKeyboard
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest4, MoveAndResizeKeyboard01, Function | SmallTest | Level2)
{
    sptr<WindowOption> keyboardOption = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, keyboardOption);
    keyboardOption->SetWindowName("MoveAndResizeKeyboard01");
    keyboardOption->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    sptr<WindowSceneSessionImpl> keyboardWindow = new (std::nothrow) WindowSceneSessionImpl(keyboardOption);
    ASSERT_NE(nullptr, keyboardWindow);

    bool isLandscape = false;
    keyboardWindow->property_->displayId_ = 0;
    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(0);
    if (display != nullptr) {
        isLandscape = display->GetWidth() > display->GetHeight();
    }
    KeyboardLayoutParams param;
    param.LandscapeKeyboardRect_ = { 100, 100, 100, 200 };
    param.PortraitKeyboardRect_ = { 200, 200, 200, 100 };
    auto result = keyboardWindow->MoveAndResizeKeyboard(param);
    auto expectRect = isLandscape ? param.LandscapeKeyboardRect_ : param.PortraitKeyboardRect_;
    ASSERT_EQ(keyboardWindow->property_->requestRect_, expectRect);
    ASSERT_EQ(result, WMError::WM_OK);
}

/**
 * @tc.name: MoveAndResizeKeyboard02
 * @tc.desc: MoveAndResizeKeyboard
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest4, MoveAndResizeKeyboard02, Function | SmallTest | Level2)
{
    sptr<WindowOption> keyboardOption = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, keyboardOption);
    keyboardOption->SetWindowName("MoveAndResizeKeyboard02");
    keyboardOption->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    sptr<WindowSceneSessionImpl> keyboardWindow = new (std::nothrow) WindowSceneSessionImpl(keyboardOption);
    ASSERT_NE(nullptr, keyboardWindow);

    bool isLandscape = false;
    keyboardWindow->property_->displayId_ = DISPLAY_ID_INVALID;
    auto defaultDisplayInfo = DisplayManager::GetInstance().GetDefaultDisplay();
    if (defaultDisplayInfo != nullptr) {
        isLandscape = defaultDisplayInfo->GetWidth() > defaultDisplayInfo->GetHeight();
    }
    KeyboardLayoutParams param;
    param.LandscapeKeyboardRect_ = { 100, 100, 100, 200 };
    param.PortraitKeyboardRect_ = { 200, 200, 200, 100 };
    auto result = keyboardWindow->MoveAndResizeKeyboard(param);
    auto expectRect = isLandscape ? param.LandscapeKeyboardRect_ : param.PortraitKeyboardRect_;
    ASSERT_EQ(keyboardWindow->property_->requestRect_, expectRect);
    ASSERT_EQ(result, WMError::WM_OK);
}

}
} // namespace Rosen
} // namespace OHOS