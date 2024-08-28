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
#include "display_info.h"
#include "ability_context_impl.h"
#include "mock_session.h"
#include "window_session_impl.h"
#include "mock_uicontent.h"
#include "window_scene_session_impl.h"
#include "mock_window_adapter.h"
#include "singleton_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
uint32_t MaxWith = 32;

class WindowSceneSessionImplTest2 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_;
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

private:
    RSSurfaceNode::SharedPtr CreateRSSurfaceNode();
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

void WindowSceneSessionImplTest2::SetUpTestCase() {}

void WindowSceneSessionImplTest2::TearDownTestCase() {}

void WindowSceneSessionImplTest2::SetUp()
{
    abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();
}

void WindowSceneSessionImplTest2::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    abilityContext_ = nullptr;
}

RSSurfaceNode::SharedPtr WindowSceneSessionImplTest2::CreateRSSurfaceNode()
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "startingWindowTestSurfaceNode";
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig, RSSurfaceNodeType::DEFAULT);
    return surfaceNode;
}

namespace {
/*
 * @tc.name: SetBackdropBlur
 * @tc.desc: SetBackdropBlur test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetBackdropBlur, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("SetBackdropBlur");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);

    auto surfaceNode = window->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->CheckParmAndPermission());
    } else {
        ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetBackdropBlur(-1.0));
        ASSERT_EQ(WMError::WM_OK, window->SetBackdropBlur(1.0));
    }
}

/*
 * @tc.name: SetShadowColor
 * @tc.desc: SetShadowColor test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetShadowColor, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("SetShadowColor");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);

    auto surfaceNode = window->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->CheckParmAndPermission());
    } else {
        ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowColor("111ff22ee44"));
        ASSERT_EQ(WMError::WM_OK, window->SetShadowColor("#ff22ee44"));
        ASSERT_EQ(WMError::WM_OK, window->SetShadowColor("#000999"));
    }
}

/*
 * @tc.name: SetCornerRadius
 * @tc.desc: SetCornerRadius test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetCornerRadius, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("SetCornerRadius");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);

    auto surfaceNode = window->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->SetCornerRadius(1.0));
    } else {
        ASSERT_EQ(WMError::WM_OK, window->SetCornerRadius(1.0));
    }
}

/*
 * @tc.name: SetShadowRadius
 * @tc.desc: SetShadowRadius test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetShadowRadius, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("SetShadowRadius");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);

    auto surfaceNode = window->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->CheckParmAndPermission());
    } else {
        ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowRadius(-1.0));
        ASSERT_EQ(WMError::WM_OK, window->SetShadowRadius(1.0));
    }
}

/**
 * @tc.name: SetTransform01
 * @tc.desc: set transform
 * @tc.type: FUNC
 * @tc.require:issueI7IJVV
 */
HWTEST_F(WindowSceneSessionImplTest2, SetTransform01, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetTransform01");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(1);
    Transform trans_;
    window->SetTransform(trans_);
    ASSERT_TRUE(trans_ == window->GetTransform());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Destroy(false));
}

/**
 * @tc.name: RegisterAnimationTransitionController01
 * @tc.desc: RegisterAnimationTransitionController
 * @tc.type: FUNC
 * @tc.require:issueI7IJVV
 */
HWTEST_F(WindowSceneSessionImplTest2, RegisterAnimationTransitionController01, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("RegisterAnimationTransitionController01");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(1);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->RegisterAnimationTransitionController(nullptr));
}

/**
 * @tc.name: SetNeedDefaultAnimation01
 * @tc.desc: SetNeedDefaultAnimation
 * @tc.type: FUNC
 * @tc.require:issueI7IJVV
 */
HWTEST_F(WindowSceneSessionImplTest2, SetNeedDefaultAnimation01, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetNeedDefaultAnimation01");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    auto ret = true;
    window->property_->SetPersistentId(1);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->SetNeedDefaultAnimation(false);
    ASSERT_TRUE(ret);
}

/**
 * @tc.desc: UpdateSurfaceNodeAfterCustomAnimation01
 * @tc.desc: UpdateSurfaceNodeAfterCustomAnimation
 * @tc.type: FUNC
 * @tc.require:issueI7IJVV
 */
HWTEST_F(WindowSceneSessionImplTest2, UpdateSurfaceNodeAfterCustomAnimation, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("UpdateSurfaceNodeAfterCustomAnimation");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    window->property_->SetPersistentId(1);

    window->UpdateSurfaceNodeAfterCustomAnimation(false);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->UpdateSurfaceNodeAfterCustomAnimation(false));
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_OPERATION, window->UpdateSurfaceNodeAfterCustomAnimation(false));
}

/**
 * @tc.name: SetAlpha01
 * @tc.desc: SetAlpha
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetAlpha01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SetAlpha01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);

    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);

    ASSERT_NE(nullptr, windowSceneSession);
    windowSceneSession->property_->SetPersistentId(11);
    windowSceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);

    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowSceneSession->SetAlpha(1.0));
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    ASSERT_EQ(WMError::WM_OK, windowSceneSession->Create(abilityContext_, session));
    windowSceneSession->hostSession_ = session;

    auto surfaceNode = windowSceneSession->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowSceneSession->CheckParmAndPermission());
    } else {
        ASSERT_EQ(WMError::WM_OK, windowSceneSession->SetAlpha(1.0));
    }
    windowSceneSession->Destroy(true);
}

/**
 * @tc.name: DestroySubWindow01
 * @tc.desc: DestroySubWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, DestroySubWindow01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("DestroySubWindow01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);

    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);

    ASSERT_NE(nullptr, windowSceneSession);
    int ret = 0;
    windowSceneSession->DestroySubWindow();
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: UpdateFloatingWindowSizeBySizeLimits01
 * @tc.desc: UpdateFloatingWindowSizeBySizeLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, UpdateFloatingWindowSizeBySizeLimits01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("UpdateFloatingWindowSizeBySizeLimits01");
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA);

    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    int ret = 0;
    windowSceneSession->UpdateFloatingWindowSizeBySizeLimits(MaxWith, MaxWith);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: UpdateAnimationFlagProperty01
 * @tc.desc: UpdateAnimationFlagProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, UpdateAnimationFlagProperty01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("UpdateAnimationFlagProperty01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowSceneSession->UpdateAnimationFlagProperty(false));
}

/**
 * @tc.name: UpdateWindowModeImmediately01
 * @tc.desc: UpdateWindowModeImmediately
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, UpdateWindowModeImmediately01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("UpdateWindowModeImmediately01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);

    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->UpdateWindowModeImmediately(WindowMode::WINDOW_MODE_UNDEFINED));
    windowSceneSession->state_ = WindowState::STATE_CREATED;
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->UpdateWindowModeImmediately(WindowMode::WINDOW_MODE_UNDEFINED));
}

/**
 * @tc.name: UpdateWindowMode01
 * @tc.desc: UpdateWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, UpdateWindowMode01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("UpdateWindowMode01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);

    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_WINDOW,
              windowSceneSession->UpdateWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
    windowSceneSession->state_ = WindowState::STATE_CREATED;
    ASSERT_EQ(WSError::WS_ERROR_INVALID_WINDOW,
              windowSceneSession->UpdateWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSession->hostSession_ = session;
    ASSERT_EQ(WSError::WS_ERROR_INVALID_WINDOW,
              windowSceneSession->UpdateWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
}

/**
 * @tc.name: UpdateWindowMode02
 * @tc.desc: UpdateWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, UpdateWindowMode02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("UpdateWindowMode02");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);

    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_WINDOW,
              windowSceneSession->UpdateWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
    windowSceneSession->state_ = WindowState::STATE_CREATED;
    ASSERT_EQ(WSError::WS_ERROR_INVALID_WINDOW,
              windowSceneSession->UpdateWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSession->hostSession_ = session;
    ASSERT_EQ(WSError::WS_ERROR_INVALID_WINDOW,
              windowSceneSession->UpdateWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
    windowSceneSession->property_->SetPersistentId(1);
    ASSERT_EQ(WSError::WS_OK,
              windowSceneSession->UpdateWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
}

/**
 * @tc.name: RemoveWindowFlag01
 * @tc.desc: RemoveWindowFlag
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, RemoveWindowFlag01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("RemoveWindowFlag01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);

    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW,
              windowSceneSession->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID));
    windowSceneSession->state_ = WindowState::STATE_CREATED;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW,
              windowSceneSession->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID));

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSession->hostSession_ = session;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW,
              windowSceneSession->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID));
}

/**
 * @tc.name: GetConfigurationFromAbilityInfo01
 * @tc.desc: GetConfigurationFromAbilityInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, GetConfigurationFromAbilityInfo01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("GetConfigurationFromAbilityInfo01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    int ret = 0;
    windowSceneSession->GetConfigurationFromAbilityInfo();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: PreProcessCreate01
 * @tc.desc: PreProcessCreate
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, PreProcessCreate01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("PreProcessCreate01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    int ret = 0;
    windowSceneSession->PreProcessCreate();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: SetDefaultProperty01
 * @tc.desc: SetDefaultProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetDefaultProperty01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SetDefaultProperty01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    int ret = 0;
    windowSceneSession->SetDefaultProperty();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: UpdateConfiguration01
 * @tc.desc: UpdateConfiguration
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, UpdateConfiguration01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("UpdateConfiguration01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    int ret = 0;
    std::shared_ptr<AppExecFwk::Configuration> configuration;
    windowSceneSession->UpdateConfiguration(configuration);
    windowSceneSession->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    windowSceneSession->UpdateConfiguration(configuration);
    ASSERT_EQ(ret, 0);
}


/**
 * @tc.name: UpdateConfigurationForAll01
 * @tc.desc: UpdateConfigurationForAll
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, UpdateConfigurationForAll01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("UpdateConfigurationForAll01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    int ret = 0;
    std::shared_ptr<AppExecFwk::Configuration> configuration;
    windowSceneSession->UpdateConfigurationForAll(configuration);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: GetTopWindowWithContext01
 * @tc.desc: GetTopWindowWithContext
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, GetTopWindowWithContext01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("GetTopWindowWithContext01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    std::shared_ptr<AbilityRuntime::Context> context;
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    if (windowSceneSession->GetTopWindowWithContext(context) == nullptr) {
        ASSERT_EQ(nullptr, windowSceneSession->GetTopWindowWithContext(context));
    }
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->Create(abilityContext_, session));
    windowSceneSession->GetTopWindowWithContext(context);
    windowSceneSession->Destroy(true);
}

/**
 * @tc.name: GetMainWindowWithContext01
 * @tc.desc: GetMainWindowWithContext
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, GetMainWindowWithContext01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("GetMainWindowWithContext01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    std::shared_ptr<AbilityRuntime::Context> context;
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    if (windowSceneSession->GetMainWindowWithContext(context) == nullptr) {
        ASSERT_EQ(nullptr, windowSceneSession->GetMainWindowWithContext(context));
    }
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->Create(abilityContext_, session));
    windowSceneSession->GetMainWindowWithContext(context);
    windowSceneSession->Destroy(true);
}

/**
 * @tc.name: NotifyMemoryLevel01
 * @tc.desc: NotifyMemoryLevel
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, NotifyMemoryLevel01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("NotifyMemoryLevel01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    std::shared_ptr<AppExecFwk::Configuration> configuration;
    windowSceneSession->NotifyMemoryLevel(2);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowSceneSession->NotifyMemoryLevel(2));
    windowSceneSession->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->NotifyMemoryLevel(2));
}

/**
 * @tc.name: GetSystemSizeLimits01
 * @tc.desc: GetSystemSizeLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, GetSystemSizeLimits01, Function | SmallTest | Level2)
{
    constexpr uint32_t minMainWidth = 10;
    constexpr uint32_t minMainHeight = 20;
    constexpr uint32_t minSubWidth = 30;
    constexpr uint32_t minSubHeight = 40;
    constexpr uint32_t displayWidth = 100;
    constexpr uint32_t displayHeight = 100;
    constexpr float displayVpr = 1;

    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("GetSystemSizeLimits01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);

    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);

    ASSERT_NE(nullptr, windowSceneSession);
    windowSceneSession->windowSystemConfig_.miniWidthOfMainWindow_ = minMainWidth;
    windowSceneSession->windowSystemConfig_.miniHeightOfMainWindow_ = minMainHeight;
    windowSceneSession->windowSystemConfig_.miniWidthOfSubWindow_ = minSubWidth;
    windowSceneSession->windowSystemConfig_.miniHeightOfSubWindow_ = minSubHeight;
    
    WindowLimits limits = windowSceneSession->GetSystemSizeLimits(displayWidth, displayHeight, displayVpr);
    ASSERT_EQ(limits.minWidth_, minMainWidth);
    ASSERT_EQ(limits.minHeight_, minMainHeight);

    windowSceneSession->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    limits = windowSceneSession->GetSystemSizeLimits(displayWidth, displayHeight, displayVpr);
    ASSERT_EQ(limits.minWidth_, minMainWidth);
    ASSERT_EQ(limits.minHeight_, minMainHeight);

    windowSceneSession->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    limits = windowSceneSession->GetSystemSizeLimits(displayWidth, displayHeight, displayVpr);
    ASSERT_EQ(limits.minWidth_, minMainWidth);
    ASSERT_EQ(limits.minHeight_, minMainHeight);
}

/**
 * @tc.name: DumpSessionElementInfo
 * @tc.desc: DumpSessionElementInfo 1: params num
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, DumpSessionElementInfo1, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("DumpSessionElementInfo1");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    std::vector<std::string> params;
    params.push_back("-h");
    window->DumpSessionElementInfo(params);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_EQ(WMError::WM_OK, window->NotifyMemoryLevel(2));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Destroy(false));
}

/**
 * @tc.name: DumpSessionElementInfo
 * @tc.desc: DumpSessionElementInfo2
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, DumpSessionElementInfo2, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("DumpSessionElementInfo2");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    std::vector<std::string> params;
    params.push_back("-h");
    window->DumpSessionElementInfo(params);
    params.push_back("-s");
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->DumpSessionElementInfo(params);
    ASSERT_EQ(WMError::WM_OK, window->NotifyMemoryLevel(2));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Destroy(false));
}

/**
 * @tc.name: DumpSessionElementInfo
 * @tc.desc: DumpSessionElementInfo3
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, DumpSessionElementInfo3, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("DumpSessionElementInfo3");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    std::vector<std::string> params;
    params.push_back("-s");
    window->DumpSessionElementInfo(params);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_EQ(WMError::WM_OK, window->NotifyMemoryLevel(2));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Destroy(false));
}

/**
 * @tc.name: DisableAppWindowDecor02
 * @tc.desc: DisableAppWindowDecor
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, DisableAppWindowDecor02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("DisableAppWindowDecor02");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    int ret = 0;
    windowSceneSession->DisableAppWindowDecor();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: RaiseAboveTarget01
 * @tc.desc: RaiseAboveTarget
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, RaiseAboveTarget01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("RaiseAboveTarget01");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSessionImpl);

    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetPersistentId(6);
    ASSERT_NE(nullptr, windowSceneSessionImpl->property_);
    windowSceneSessionImpl->property_->SetParentPersistentId(0);
    auto ret = windowSceneSessionImpl->RaiseAboveTarget(1);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARENT, ret);
}

/**
 * @tc.name: FindParentSessionByParentId02
 * @tc.desc: FindParentSessionByParentId
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, FindParentSessionByParentId02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowTag(WindowTag::MAIN_WINDOW);
    option->SetWindowName("FindParentSessionByParentId02");
    sptr<WindowSceneSessionImpl> parentSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, parentSceneSession);

    ASSERT_EQ(nullptr, parentSceneSession->FindParentSessionByParentId(-1));
}

/**
 * @tc.name: GetConfigurationFromAbilityInfo02
 * @tc.desc: GetConfigurationFromAbilityInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, GetConfigurationFromAbilityInfo02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("GetConfigurationFromAbilityInfo02");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    int ret = 0;
    windowSceneSession->GetConfigurationFromAbilityInfo();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: NotifyWindowSessionProperty01
 * @tc.desc: NotifyWindowSessionProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, NotifyWindowSessionProperty01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("NotifyWindowSessionProperty01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    int ret = 0;
    windowSceneSession->NotifyWindowSessionProperty();
    ASSERT_EQ(ret, 0);
}

HWTEST_F(WindowSceneSessionImplTest2, IsTransparent01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("IsTransparent01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    int ret = 0;
    windowSceneSession->IsTransparent();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: SetTransparent02
 * @tc.desc: SetTransparent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetTransparent02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SetTransparent02");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    int ret = 0;
    bool isTransparent = false;
    windowSceneSession->SetTransparent(isTransparent);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: Snapshot01
 * @tc.desc: Snapshot
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, Snapshot01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Snapshot01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    int ret = 0;
    windowSceneSession->Snapshot();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: BindDialogTarget01
 * @tc.desc: BindDialogTarget
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, BindDialogTarget01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("BindDialogTarget01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    sptr<IRemoteObject> targetToken;
    WMError ret = windowSceneSession->BindDialogTarget(targetToken);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: SetDialogBackGestureEnabled01
 * @tc.desc: SetDialogBackGestureEnabled
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetDialogBackGestureEnabled01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("SetDialogBackGestureEnabled01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    WMError ret = windowSceneSession->SetDialogBackGestureEnabled(true);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_CALLING);
}

/**
 * @tc.name: SetDialogBackGestureEnabled02
 * @tc.desc: SetDialogBackGestureEnabled
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetDialogBackGestureEnabled02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("SetDialogBackGestureEnabled02");
    option->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);

    WMError ret = windowSceneSession->SetDialogBackGestureEnabled(true);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: NotifySessionForeground
 * @tc.desc: NotifySessionForeground
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, NotifySessionForeground, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("NotifySessionForeground");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);

    uint32_t reason = 1;
    bool withAnimation = true;
    windowSceneSession->NotifySessionForeground(reason, withAnimation);
}

/**
 * @tc.name: NotifySessionBackground
 * @tc.desc: NotifySessionBackground
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, NotifySessionBackground, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("NotifySessionBackground");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);

    uint32_t reason = 1;
    bool withAnimation = true;
    bool isFromInnerkits = true;
    windowSceneSession->NotifySessionBackground(reason, withAnimation, isFromInnerkits);
}

/**
 * @tc.name: NotifySessionFullScreen
 * @tc.desc: NotifySessionFullScreen
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, NotifySessionFullScreen, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("NotifySessionFullScreen");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);

    bool fullScreen = true;
    windowSceneSession->NotifySessionFullScreen(fullScreen);
}

/**
 * @tc.name: NotifyPrepareClosePiPWindow01
 * @tc.desc: NotifyPrepareClosePiPWindow
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, NotifyPrepareClosePiPWindow01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("NotifyPrepareClosePiPWindow01");
    option->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSession->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->NotifyPrepareClosePiPWindow());
}

/**
 * @tc.name: SetDefaultDensityEnabled01
 * @tc.desc: normal test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, SetDefaultDensityEnabled01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SetDefaultDensityEnabled01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_SHOWN;
    window->property_->SetDisplayId(0);

    WindowSceneSessionImpl::windowSessionMap_.insert(std::make_pair(window->GetWindowName(),
        std::pair<uint64_t, sptr<WindowSessionImpl>>(window->GetWindowId(), window)));
    ASSERT_EQ(WMError::WM_OK, window->SetDefaultDensityEnabled(true));
    ASSERT_EQ(true, window->GetDefaultDensityEnabled());
    WindowSceneSessionImpl::windowSessionMap_.erase(window->GetWindowName());
}

/**
 * @tc.name: SetDefaultDensityEnabled02
 * @tc.desc: window session is invalid
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, SetDefaultDensityEnabled02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SetDefaultDensityEnabled02");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->property_->SetPersistentId(1);
    window->state_ = WindowState::STATE_SHOWN;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetDefaultDensityEnabled(true));
}

/**
 * @tc.name: SetDefaultDensityEnabled03
 * @tc.desc: not app main window
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, SetDefaultDensityEnabled03, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SetDefaultDensityEnabled03");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_SHOWN;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_CALLING, window->SetDefaultDensityEnabled(true));
}

/**
 * @tc.name: SetDefaultDensityEnabled04
 * @tc.desc: isDefaultDensityEnabled_ not change
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, SetDefaultDensityEnabled04, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SetDefaultDensityEnabled04");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_SHOWN;
    ASSERT_EQ(WMError::WM_OK, window->SetDefaultDensityEnabled(false));
}

/**
 * @tc.name: GetDefaultDensityEnabled01
 * @tc.desc: normal test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, GetDefaultDensityEnabled01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("GetDefaultDensityEnabled01");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->isDefaultDensityEnabled_ = true;
    ASSERT_EQ(true, window->GetDefaultDensityEnabled());
}

/**
 * @tc.name: GetDefaultDensityEnabled02
 * @tc.desc: test default value
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, GetDefaultDensityEnabled02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("GetDefaultDensityEnabled02");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_EQ(false, window->GetDefaultDensityEnabled());
}

/**
 * @tc.name: GetVirtualPixelRatio01
 * @tc.desc: main window isDefaultDensityEnabled_ true
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, GetVirtualPixelRatio01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("GetVirtualPixelRatio01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_SHOWN;
    window->property_->SetDisplayId(0);

    window->isDefaultDensityEnabled_ = true;
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    auto defautDensity = 1.0f;
    auto density = 2.0f;
    displayInfo->SetDefaultVirtualPixelRatio(defautDensity);
    displayInfo->SetVirtualPixelRatio(density);
    ASSERT_EQ(defautDensity, window->GetVirtualPixelRatio(displayInfo));
}

/**
 * @tc.name: GetVirtualPixelRatio02
 * @tc.desc: main window isDefaultDensityEnabled_ false
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, GetVirtualPixelRatio02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("GetVirtualPixelRatio02");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_SHOWN;
    window->property_->SetDisplayId(0);

    window->isDefaultDensityEnabled_ = false;
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    auto defautDensity = 1.0f;
    auto density = 2.0f;
    displayInfo->SetDefaultVirtualPixelRatio(defautDensity);
    displayInfo->SetVirtualPixelRatio(density);
    ASSERT_EQ(density, window->GetVirtualPixelRatio(displayInfo));
}

/**
 * @tc.name: GetVirtualPixelRatio03
 * @tc.desc: sub window isDefaultDensityEnabled_ true
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, GetVirtualPixelRatio03, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("GetVirtualPixelRatio03");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_SHOWN;
    window->property_->SetDisplayId(0);

    window->isDefaultDensityEnabled_ = true;
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    auto defautDensity = 1.0f;
    auto density = 2.0f;
    displayInfo->SetDefaultVirtualPixelRatio(defautDensity);
    displayInfo->SetVirtualPixelRatio(density);

    sptr<WindowOption> subOption = new (std::nothrow) WindowOption();
    subOption->SetWindowName("GetVirtualPixelRatio03SubWindow");
    subOption->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowSceneSessionImpl> subWindow = new (std::nothrow) WindowSceneSessionImpl(subOption);
    ASSERT_NE(nullptr, subWindow);
    subWindow->property_->SetParentPersistentId(window->GetWindowId());

    WindowSceneSessionImpl::windowSessionMap_.insert(std::make_pair(window->GetWindowName(),
        std::pair<uint64_t, sptr<WindowSessionImpl>>(window->GetWindowId(), window)));
    ASSERT_EQ(defautDensity, subWindow->GetVirtualPixelRatio(displayInfo));
    WindowSceneSessionImpl::windowSessionMap_.erase(window->GetWindowName());
}

/**
 * @tc.name: GetVirtualPixelRatio04
 * @tc.desc: sub window isDefaultDensityEnabled_ false
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, GetVirtualPixelRatio04, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("GetVirtualPixelRatio04");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_SHOWN;
    window->property_->SetDisplayId(0);

    window->isDefaultDensityEnabled_ = false;
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    auto defautDensity = 1.0f;
    auto density = 2.0f;
    displayInfo->SetDefaultVirtualPixelRatio(defautDensity);
    displayInfo->SetVirtualPixelRatio(density);

    sptr<WindowOption> subOption = new (std::nothrow) WindowOption();
    subOption->SetWindowName("GetVirtualPixelRatio04SubWindow");
    subOption->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowSceneSessionImpl> subWindow = new (std::nothrow) WindowSceneSessionImpl(subOption);
    ASSERT_NE(nullptr, subWindow);
    subWindow->property_->SetParentPersistentId(window->GetWindowId());

    WindowSceneSessionImpl::windowSessionMap_.insert(std::make_pair(window->GetWindowName(),
        std::pair<uint64_t, sptr<WindowSessionImpl>>(window->GetWindowId(), window)));
    ASSERT_EQ(density, subWindow->GetVirtualPixelRatio(displayInfo));
    WindowSceneSessionImpl::windowSessionMap_.erase(window->GetWindowName());
}

/**
 * @tc.name: GetWindowLimits01
 * @tc.desc: GetWindowLimits
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, GetWindowLimits01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("GetWindowLimits01");

    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    WindowLimits windowLimits = {1000, 1000, 1000, 1000, 0.0f, 0.0f};
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->property_->SetWindowLimits(windowLimits);
    window->property_->SetPersistentId(1);
    window->state_ = WindowState::STATE_HIDDEN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;

    WindowLimits windowSizeLimits;
    ASSERT_EQ(WMError::WM_OK, window->GetWindowLimits(windowSizeLimits));
    ASSERT_EQ(windowSizeLimits.maxWidth_, 1000);
    ASSERT_EQ(windowSizeLimits.maxHeight_, 1000);
    ASSERT_EQ(windowSizeLimits.minWidth_, 1000);
    ASSERT_EQ(windowSizeLimits.minHeight_, 1000);
}

/**
 * @tc.name: SetWindowLimits01
 * @tc.desc: SetWindowLimits
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, SetWindowLimits01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SetWindowLimits01");
    option->SetDisplayId(0);

    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    window->property_->SetPersistentId(1);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->state_ = WindowState::STATE_FROZEN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;

    WindowLimits windowLimits = {2000, 2000, 2000, 2000, 0.0f, 0.0f};
    if (WMError::WM_OK == window->SetWindowLimits(windowLimits)) {
        WindowLimits windowSizeLimits = window->property_->GetWindowLimits();
        ASSERT_EQ(windowSizeLimits.maxWidth_, 2000);
        ASSERT_EQ(windowSizeLimits.maxHeight_, 2000);
        ASSERT_EQ(windowSizeLimits.minWidth_, 2000);
        ASSERT_EQ(windowSizeLimits.minHeight_, 2000);
    }
}

/**
 * @tc.name: AdjustKeyboardLayout01
 * @tc.desc: adjust keyboard layout
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, AdjustKeyboardLayout01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("AdjustKeyboardLayout01");
    option->SetDisplayId(0);

    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    window->property_->SetPersistentId(123);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->state_ = WindowState::STATE_FROZEN;
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;

    KeyboardLayoutParams params;
    params.gravity_ = WindowGravity::WINDOW_GRAVITY_FLOAT;
    params.LandscapeKeyboardRect_ = {1, 2, 3, 4};
    params.PortraitKeyboardRect_ = {1, 2, 3, 4};
    params.LandscapePanelRect_ = {1, 2, 3, 4};
    params.PortraitPanelRect_ = {1, 2, 3, 4};
    ASSERT_EQ(WMError::WM_OK, window->AdjustKeyboardLayout(params));
}
/**
 * @tc.name: HideNonSecureWindows01
 * @tc.desc: HideNonSecureWindows
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, HideNonSecureWindows01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("HideNonSecureWindows01");

    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    ASSERT_EQ(WMError::WM_OK, window->HideNonSecureWindows(true));
}

/**
 * @tc.name: SetGrayScale01
 * @tc.desc: SetGrayScale
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetGrayScale01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    constexpr float grayScale = 0.5f;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetGrayScale(grayScale));
}

/**
 * @tc.name: SetGrayScale02
 * @tc.desc: SetGrayScale
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetGrayScale02, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    window->state_ = WindowState::STATE_SHOWN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();

    std::vector<WindowType> types = { WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
                                      WindowType::WINDOW_TYPE_APP_SUB_WINDOW,
                                      WindowType::SYSTEM_WINDOW_BASE };
    for (WindowType type : types) {
        window->SetWindowType(type);
        float grayScale = -0.001f;
        ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetGrayScale(grayScale));
        grayScale = 1.001f;
        ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetGrayScale(grayScale));
    }
}

/**
 * @tc.name: SetGrayScale03
 * @tc.desc: SetGrayScale
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetGrayScale03, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    window->state_ = WindowState::STATE_SHOWN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();

    std::vector<WindowType> types = { WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
                                      WindowType::WINDOW_TYPE_APP_SUB_WINDOW,
                                      WindowType::SYSTEM_WINDOW_BASE };
    for (WindowType type : types) {
        window->SetWindowType(type);
        float grayScale = 0.0f;
        ASSERT_EQ(WMError::WM_OK, window->SetGrayScale(grayScale));
        grayScale = 1.0f;
        ASSERT_EQ(WMError::WM_OK, window->SetGrayScale(grayScale));
        grayScale = 0.5f;
        ASSERT_EQ(WMError::WM_OK, window->SetGrayScale(grayScale));
    }
}

/**
 * @tc.name: Maximize02
 * @tc.desc: test errorCode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, Maximize02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Maximize02");
    option->SetDisplayId(0);

    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    window->property_->SetWindowName("Maximize02");
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    window->property_->SetPersistentId(2);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;

    MaximizePresentation presentation = MaximizePresentation::ENTER_IMMERSIVE;
    // not support subWinodw call
    ASSERT_EQ(WMError::WM_ERROR_INVALID_CALLING, window->Maximize(presentation));
 
    // window not support fullscreen
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->property_->SetModeSupportInfo(WindowModeSupport::WINDOW_MODE_SUPPORT_PIP);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Maximize(presentation));
}

/**
 * @tc.name: Maximize03
 * @tc.desc: test presentation
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, Maximize03, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Maximize03");
    option->SetDisplayId(0);

    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    window->property_->SetWindowName("Maximize03");
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->property_->SetPersistentId(2);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;

    ASSERT_NE(nullptr, window);
    // case1: only set maximize()
    MaximizePresentation presentation = MaximizePresentation::ENTER_IMMERSIVE;
    auto ret = window->Maximize(presentation);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(window->GetImmersiveModeEnabledState(), true);

    // case2: maximize(EXIT_IMMERSIVE) and the immersive value will be set ad false
    presentation = MaximizePresentation::EXIT_IMMERSIVE;
    ret = window->Maximize(presentation);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(window->GetImmersiveModeEnabledState(), false);
    
    // case3: maximize(FOLLOW_APP_IMMERSIVE_SETTING) and the immersive value will be set as client set
    presentation = MaximizePresentation::FOLLOW_APP_IMMERSIVE_SETTING;
    ret = window->Maximize(presentation);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(window->GetImmersiveModeEnabledState(), false);

    // case4: maximize(ENTER_IMMERSIVE) and the immersive value will be set as true
    presentation = MaximizePresentation::ENTER_IMMERSIVE;
    ret = window->Maximize(presentation);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(window->GetImmersiveModeEnabledState(), true);
}

/**
 * @tc.name: Test01
 * @tc.desc: Test01
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, Test01, Function | SmallTest | Level3)
{
    WindowLimits windowLimits = {3, 3, 3, 3, 2.0, 2.0};
    ASSERT_EQ(windowLimits.IsEmpty(), false);
    Rect rect = {2, 2, 2, 2};
    Rect rect1 = {2, 2, 2, 2};
    ASSERT_EQ(rect.IsInsideOf(rect1), true);
    TitleButtonRect titleButtonRect = {3, 3, 3, 3};
    TitleButtonRect titleButtonRect1 = {3, 3, 3, 3};
    ASSERT_EQ(titleButtonRect.IsUninitializedRect(), false);
    ASSERT_EQ(titleButtonRect.IsInsideOf(titleButtonRect1), true);
    AvoidArea avoidArea;
    AvoidArea avoidArea1;
    ASSERT_EQ((avoidArea != avoidArea1), false);
}

/**
 * @tc.name: RegisterSessionRecoverListenerInputMethodFloat
 * @tc.desc: Register session recover listener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, RegisterSessionRecoverListenerInputMethodFloat, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    ASSERT_NE(nullptr, window->property_);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    window->RegisterSessionRecoverListener(false);
}
 
/**
 * @tc.name: RegisterSessionRecoverListenerNonDefaultCollaboratorType01
 * @tc.desc: Register session recover listener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, RegisterSessionRecoverListenerNonDefaultCollaboratorType01,
    Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    ASSERT_NE(nullptr, window->property_);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->property_->SetCollaboratorType(CollaboratorType::OTHERS_TYPE);
    window->RegisterSessionRecoverListener(false); // false is main window
}

/**
 * @tc.name: RegisterSessionRecoverListenerNonDefaultCollaboratorType02
 * @tc.desc: Register session recover listener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, RegisterSessionRecoverListenerNonDefaultCollaboratorType02,
    Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    ASSERT_NE(nullptr, window->property_);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->property_->SetCollaboratorType(CollaboratorType::OTHERS_TYPE);
    window->RegisterSessionRecoverListener(true); // true is sub window
}

/**
 * @tc.name: RegisterSessionRecoverListenerSuccess01
 * @tc.desc: Register session recover listener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, RegisterSessionRecoverListenerSuccess01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    ASSERT_NE(nullptr, window->property_);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->property_->SetCollaboratorType(CollaboratorType::DEFAULT_TYPE);
    window->RegisterSessionRecoverListener(false); // false is main window
}

/**
 * @tc.name: RegisterSessionRecoverListenerSuccess02
 * @tc.desc: Register session recover listener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, RegisterSessionRecoverListenerSuccess02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    ASSERT_NE(nullptr, window->property_);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->property_->SetCollaboratorType(CollaboratorType::DEFAULT_TYPE);
    window->RegisterSessionRecoverListener(true); // true is sub window
}

/**
 * @tc.name: SyncDestroyAndDisconnectSpecificSession
 * @tc.desc: SyncDestroyAndDisconnectSpecificSession test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, SyncDestroyAndDisconnectSpecificSession, Function | SmallTest | Level2)
{
    sptr option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SyncDestroyAndDisconnectSpecificSession");
    sptr window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(window, nullptr);
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    WMError ret = window->SyncDestroyAndDisconnectSpecificSession(100);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: GetTitleButtonVisible01
 * @tc.desc: GetTitleButtonVisible
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, GetTitleButtonVisible01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("GetTitleButtonVisible01");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    uint32_t modeSupportInfo = 1 | (1 << 1) | (1 << 2);
    window->property_->SetModeSupportInfo(modeSupportInfo);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    // show Maximize, Minimize, Split buttons.
    window->windowTitleVisibleFlags_ = { false, false, false };
    bool hideMaximizeButton = false;
    bool hideMinimizeButton = false;
    bool hideSplitButton = false;
    window->GetTitleButtonVisible(true, hideMaximizeButton, hideMinimizeButton, hideSplitButton);
    ASSERT_EQ(hideMaximizeButton, true);
    ASSERT_EQ(hideMinimizeButton, true);
    ASSERT_EQ(hideSplitButton, true);
}

/**
 * @tc.name: GetTitleButtonVisible02
 * @tc.desc: GetTitleButtonVisible
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, GetTitleButtonVisible02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("GetTitleButtonVisible02");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    // only not support WINDOW_MODE_SUPPORT_SPLIT
    uint32_t modeSupportInfo = 1 | (1 << 1);
    window->property_->SetModeSupportInfo(modeSupportInfo);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    // show Maximize, Minimize, Split buttons.
    window->windowTitleVisibleFlags_ = { true, true, true };
    bool hideMaximizeButton = false;
    bool hideMinimizeButton = false;
    bool hideSplitButton = false;
    window->GetTitleButtonVisible(true, hideMaximizeButton, hideMinimizeButton, hideSplitButton);
    ASSERT_EQ(hideMaximizeButton, false);
    ASSERT_EQ(hideMinimizeButton, false);
    ASSERT_EQ(hideSplitButton, false);
}

/**
 * @tc.name: GetTitleButtonVisible03
 * @tc.desc: GetTitleButtonVisible
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, GetTitleButtonVisible03, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("GetTitleButtonVisible03");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    // only not support WINDOW_MODE_SUPPORT_SPLIT
    uint32_t modeSupportInfo = 1 | (1 << 1) | (1 << 2);
    window->property_->SetModeSupportInfo(modeSupportInfo);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    // show Maximize, Minimize, Split buttons.
    window->windowTitleVisibleFlags_ = { false, false, false };
    bool hideMaximizeButton = true;
    bool hideMinimizeButton = true;
    bool hideSplitButton = true;
    window->GetTitleButtonVisible(false, hideMaximizeButton, hideMinimizeButton, hideSplitButton);
    ASSERT_EQ(hideMaximizeButton, true);
    ASSERT_EQ(hideMinimizeButton, true);
    ASSERT_EQ(hideSplitButton, true);
}

/**
 * @tc.name: SetTitleButtonVisible01
 * @tc.desc: SetTitleButtonVisible
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, SetTitleButtonVisible01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetTitleButtonVisible");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(window, nullptr);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    window->SetTitleButtonVisible(false, false, false);
}

/**
 * @tc.name: SetTitleButtonVisible02
 * @tc.desc: SetTitleButtonVisible
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, SetTitleButtonVisible02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetTitleButtonVisible");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(window, nullptr);
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    WMError res = window->SetTitleButtonVisible(false, false, false);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_WINDOW);
}

/**
 * @tc.name: SetTitleButtonVisible03
 * @tc.desc: SetTitleButtonVisible
 * @tc.type: FUNC
*/
HWTEST_F(WindowSceneSessionImplTest2, SetTitleButtonVisible03, Function | SmallTest | Level2)
{
    sptr option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetTitleButtonVisible");
    sptr window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(window, nullptr);
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->windowSystemConfig_.freeMultiWindowSupport_ = true;
    window->windowSystemConfig_.isSystemDecorEnable_ = true;
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    WMError res = window->SetTitleButtonVisible(false, false, false);
    ASSERT_EQ(res, WMError::WM_ERROR_DEVICE_NOT_SUPPORT);
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    res = window->SetTitleButtonVisible(false, false, false);
    ASSERT_EQ(res, WMError::WM_OK);
}

}
} // namespace Rosen
} // namespace OHOS