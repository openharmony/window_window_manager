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
    windowSceneSessionImpl->property_->SetDecorEnable(true);
    ret = windowSceneSessionImpl->HandlePointDownEvent(pointerEvent, pointerItem, sourceType, vpr, rect);
    EXPECT_EQ(false, ret);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetDecorEnable(false);
    pointerItem.SetWindowX(100);
    pointerItem.SetWindowY(100);
    ret = windowSceneSessionImpl->HandlePointDownEvent(pointerEvent, pointerItem, sourceType, vpr, rect);
    EXPECT_EQ(true, ret);
    pointerItem.SetWindowX(1);
    ret = windowSceneSessionImpl->HandlePointDownEvent(pointerEvent, pointerItem, sourceType, vpr, rect);
    EXPECT_EQ(true, ret);
}
}
} // namespace Rosen
} // namespace OHOS