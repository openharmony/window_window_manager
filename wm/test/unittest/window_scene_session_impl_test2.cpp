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

#include <application_context.h>
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
#include "wm_common.h"

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
/**
 * @tc.name: SetBackdropBlur
 * @tc.desc: SetBackdropBlur test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetBackdropBlur, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetWindowName("SetBackdropBlur");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);

    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetBackdropBlur(-1.0));
    ASSERT_EQ(WMError::WM_OK, window->SetBackdropBlur(1.0));
    ASSERT_EQ(WMError::WM_OK, window->SetBackdropBlur(0.0));
    window->surfaceNode_ = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->SetBackdropBlur(1.0));
}

/**
 * @tc.name: SetShadowColor
 * @tc.desc: SetShadowColor test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetShadowColor, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetWindowName("SetShadowColor");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);

    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowColor("111ff22ee44"));
    ASSERT_EQ(WMError::WM_OK, window->SetShadowColor("#ff22ee44"));
    ASSERT_EQ(WMError::WM_OK, window->SetShadowColor("#000999"));
    window->surfaceNode_ = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->SetShadowColor("#ff22ee44"));
}

/**
 * @tc.name: SetCornerRadius
 * @tc.desc: SetCornerRadius test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetCornerRadius, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetWindowName("SetCornerRadius");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);

    ASSERT_EQ(WMError::WM_OK, window->SetCornerRadius(1.0));
    window->surfaceNode_ = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->SetCornerRadius(1.0));
}

/**
 * @tc.name: SetShadowRadius
 * @tc.desc: SetShadowRadius test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetShadowRadius, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetWindowName("SetShadowRadius");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);

    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowRadius(-1.0));
    ASSERT_EQ(WMError::WM_OK, window->SetShadowRadius(0.0));
    ASSERT_EQ(WMError::WM_OK, window->SetShadowRadius(1.0));
    window->surfaceNode_ = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->SetShadowRadius(1.0));
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetTransform01");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(1);
    Transform trans_;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetTransform(trans_));
}

/**
 * @tc.name: SetTransform01
 * @tc.desc: set transform
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetTransform02, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetTransform01");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    window->property_->SetPersistentId(1);
    window->state_ = WindowState::STATE_CREATED;
    Transform trans_;
    ASSERT_EQ(WMError::WM_OK, window->SetTransform(trans_));
    ASSERT_EQ(trans_, window->GetTransform());
}

/**
 * @tc.name: RegisterAnimationTransitionController01
 * @tc.desc: RegisterAnimationTransitionController
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, RegisterAnimationTransitionController01, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("RegisterAnimationTransitionController01");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetNeedDefaultAnimation01");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    auto ret = true;
    window->property_->SetPersistentId(1);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("UpdateSurfaceNodeAfterCustomAnimation");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(1);

    window->UpdateSurfaceNodeAfterCustomAnimation(false);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->UpdateSurfaceNodeAfterCustomAnimation(false));
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetAlpha01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    windowSceneSession->property_->SetPersistentId(11);
    windowSceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowSceneSession->SetAlpha(1.0));
}

/**
 * @tc.name: SetAlpha02
 * @tc.desc: SetAlpha
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetAlpha02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetAlpha02");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestMode", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    windowSceneSession->hostSession_ = session;
    windowSceneSession->property_->SetPersistentId(1);
    windowSceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->SetAlpha(1.0));
}

/**
 * @tc.name: DestroySubWindow01
 * @tc.desc: DestroySubWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, DestroySubWindow01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("DestroySubWindow01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    windowSceneSession->DestroySubWindow();
    ASSERT_EQ(INVALID_SESSION_ID, windowSceneSession->property_->GetParentPersistentId());
}

/**
 * @tc.name: UpdateFloatingWindowSizeBySizeLimits01
 * @tc.desc: UpdateFloatingWindowSizeBySizeLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, UpdateFloatingWindowSizeBySizeLimits01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("UpdateFloatingWindowSizeBySizeLimits01");
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("UpdateAnimationFlagProperty01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowSceneSession->UpdateAnimationFlagProperty(false));
}

/**
 * @tc.name: UpdateAnimationFlagProperty02
 * @tc.desc: UpdateAnimationFlagProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, UpdateAnimationFlagProperty02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("UpdateAnimationFlagProperty01");
    option->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->UpdateAnimationFlagProperty(false));
}

/**
 * @tc.name: UpdateWindowModeImmediately01
 * @tc.desc: UpdateWindowModeImmediately
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, UpdateWindowModeImmediately01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("UpdateWindowModeImmediately01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);

    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("UpdateWindowMode01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);

    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_WINDOW,
              windowSceneSession->UpdateWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
    windowSceneSession->state_ = WindowState::STATE_CREATED;
    ASSERT_EQ(WSError::WS_ERROR_INVALID_WINDOW,
              windowSceneSession->UpdateWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    windowSceneSession->hostSession_ = session;
    ASSERT_EQ(WSError::WS_ERROR_INVALID_WINDOW,
              windowSceneSession->UpdateWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
    windowSceneSession->property_->SetPersistentId(1);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_WINDOW_MODE_OR_SIZE,
              windowSceneSession->UpdateWindowMode(WindowMode::WINDOW_MODE_UNDEFINED));
}

/**
 * @tc.name: UpdateWindowMode02
 * @tc.desc: UpdateWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, UpdateWindowMode02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("UpdateWindowMode02");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);

    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_WINDOW,
              windowSceneSession->UpdateWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
    windowSceneSession->state_ = WindowState::STATE_CREATED;
    ASSERT_EQ(WSError::WS_ERROR_INVALID_WINDOW,
              windowSceneSession->UpdateWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    windowSceneSession->hostSession_ = session;
    ASSERT_EQ(WSError::WS_ERROR_INVALID_WINDOW,
              windowSceneSession->UpdateWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
    windowSceneSession->property_->SetPersistentId(1);
    ASSERT_EQ(WSError::WS_OK, windowSceneSession->UpdateWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
}

/**
 * @tc.name: UpdateDefaultStatusBarColor01
 * @tc.desc: test UpdateDefaultStatusBarColor
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, UpdateDefaultStatusBarColor01, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("UpdateDefaultStatusBarColor01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    auto appContext = AbilityRuntime::ApplicationContext::GetInstance();
    ASSERT_NE(appContext, nullptr);
    std::shared_ptr<AppExecFwk::Configuration> configuration;
    configuration->AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE, "light");
    appContext->SetConfiguration(configuration);
    windowSceneSession->UpdateDefaultStatusBarColor();
    configuration->AddItem(AAFwk::GlobalConfigurationKey::COLORMODE_IS_SET_BY_APP, "light");
    windowSceneSession->UpdateDefaultStatusBarColor();
}

/**
 * @tc.name: RemoveWindowFlag01
 * @tc.desc: RemoveWindowFlag
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, RemoveWindowFlag01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("RemoveWindowFlag01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);

    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW,
              windowSceneSession->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID));
    windowSceneSession->state_ = WindowState::STATE_CREATED;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW,
              windowSceneSession->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID));

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetConfigurationFromAbilityInfo01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("PreProcessCreate01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetDefaultProperty01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("UpdateConfiguration01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    int ret = 0;
    std::shared_ptr<AppExecFwk::Configuration> configuration;
    windowSceneSession->UpdateConfiguration(configuration);
    windowSceneSession->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    windowSceneSession->UpdateConfiguration(configuration);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: UpdateConfigurationForSpecified
 * @tc.desc: UpdateConfigurationForSpecified
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, UpdateConfigurationForSpecified, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("UpdateConfiguration01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    int ret = 0;
    std::shared_ptr<AppExecFwk::Configuration> configuration;
    std::shared_ptr<Global::Resource::ResourceManager> resourceManager;
    windowSceneSession->UpdateConfigurationForSpecified(configuration, resourceManager);
    windowSceneSession->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    windowSceneSession->UpdateConfigurationForSpecified(configuration, resourceManager);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: UpdateConfigurationForAll01
 * @tc.desc: UpdateConfigurationForAll
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, UpdateConfigurationForAll01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("UpdateConfigurationForAll01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    int ret = 0;
    std::shared_ptr<AppExecFwk::Configuration> configuration;
    windowSceneSession->UpdateConfigurationForAll(configuration);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: UpdateConfigurationForAll02
 * @tc.desc: UpdateConfigurationForAll02 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, UpdateConfigurationForAll02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("UpdateConfigurationForAll02");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    auto abilityContext = std::make_shared<AbilityRuntime::AbilityContextImpl>();
    ASSERT_NE(nullptr, abilityContext);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext, session));
    std::shared_ptr<AppExecFwk::Configuration> configuration;
    std::vector<std::shared_ptr<AbilityRuntime::Context>> ignoreWindowContexts;
    ignoreWindowContexts.push_back(abilityContext);
    window->UpdateConfigurationForAll(configuration, ignoreWindowContexts);
}

/**
 * @tc.name: GetTopWindowWithContext01
 * @tc.desc: GetTopWindowWithContext
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, GetTopWindowWithContext01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetTopWindowWithContext01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    std::shared_ptr<AbilityRuntime::Context> context;
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    if (windowSceneSession->GetTopWindowWithContext(context) == nullptr) {
        ASSERT_EQ(nullptr, windowSceneSession->GetTopWindowWithContext(context));
    }
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetMainWindowWithContext01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    std::shared_ptr<AbilityRuntime::Context> context;
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    if (windowSceneSession->GetMainWindowWithContext(context) == nullptr) {
        ASSERT_EQ(nullptr, windowSceneSession->GetMainWindowWithContext(context));
    }
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("NotifyMemoryLevel01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetSystemSizeLimits01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("DumpSessionElementInfo1");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("DumpSessionElementInfo2");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("DumpSessionElementInfo3");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("DisableAppWindowDecor02");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
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
    option->SetWindowName("RaiseAboveTarget01");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    windowSceneSessionImpl->property_->SetPersistentId(6);
    windowSceneSessionImpl->property_->SetParentPersistentId(0);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    windowSceneSessionImpl->hostSession_ = session;
    auto ret = windowSceneSessionImpl->RaiseAboveTarget(1);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARENT, ret);

    windowSceneSessionImpl->property_->SetParentPersistentId(1);
    ret = windowSceneSessionImpl->RaiseAboveTarget(1);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);

    sptr<WindowSessionImpl> winSession = sptr<WindowSessionImpl>::MakeSptr(option);
    WindowSessionImpl::subWindowSessionMap_.insert(
        std::make_pair<int32_t, std::vector<sptr<WindowSessionImpl>>>(1, { winSession }));
    winSession->property_->SetPersistentId(6);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ret = windowSceneSessionImpl->RaiseAboveTarget(6);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);

    winSession->state_ = WindowState::STATE_CREATED;
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    ret = windowSceneSessionImpl->RaiseAboveTarget(6);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);

    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;
    winSession->state_ = WindowState::STATE_SHOWN;
    ret = windowSceneSessionImpl->RaiseAboveTarget(6);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);

    windowSceneSessionImpl->property_->SetPersistentId(3);
    ret = windowSceneSessionImpl->RaiseAboveTarget(6);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);

    WindowSessionImpl::subWindowSessionMap_.erase(1);
}

/**
 * @tc.name: SetSubWindowZLevel
 * @tc.desc: SetSubWindowZLevel
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetSubWindowZLevel, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetSubWindowZLevel01");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    windowSceneSessionImpl->hostSession_ = session;

    session->property_->SetPersistentId(INVALID_SESSION_ID);
    auto ret = windowSceneSessionImpl->SetSubWindowZLevel(10001);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    session->property_->SetPersistentId(2);

    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ret = windowSceneSessionImpl->SetSubWindowZLevel(10001);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);

    ret = windowSceneSessionImpl->SetSubWindowZLevel(10001);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);

    windowSceneSessionImpl->property_->zLevel_ = 1;
    ret = windowSceneSessionImpl->SetSubWindowZLevel(1);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);

    ret = windowSceneSessionImpl->SetSubWindowZLevel(2);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
}

/**
 * @tc.name: GetSubWindowZLevel
 * @tc.desc: GetSubWindowZLevel
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, GetSubWindowZLevel, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetSubWindowZLevel01");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->property_->zLevel_ = 1;
    int32_t zLevel = 0;

    session->property_->SetPersistentId(INVALID_SESSION_ID);
    auto ret = windowSceneSessionImpl->GetSubWindowZLevel(zLevel);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    session->property_->SetPersistentId(2);

    ret = windowSceneSessionImpl->GetSubWindowZLevel(zLevel);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
}

/**
 * @tc.name: FindParentSessionByParentId02
 * @tc.desc: FindParentSessionByParentId
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, FindParentSessionByParentId02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowTag(WindowTag::MAIN_WINDOW);
    option->SetWindowName("FindParentSessionByParentId02");
    sptr<WindowSceneSessionImpl> parentSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_EQ(nullptr, parentSceneSession->FindParentSessionByParentId(-1));
}

/**
 * @tc.name: GetConfigurationFromAbilityInfo02
 * @tc.desc: GetConfigurationFromAbilityInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, GetConfigurationFromAbilityInfo02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetConfigurationFromAbilityInfo02");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("NotifyWindowSessionProperty01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    int ret = 0;
    windowSceneSession->NotifyWindowSessionProperty();
    ASSERT_EQ(ret, 0);
}

HWTEST_F(WindowSceneSessionImplTest2, IsTransparent01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("IsTransparent01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetTransparent02");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("Snapshot01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    int ret = 0;
    windowSceneSession->Snapshot();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: SnapshotIgnorePrivacy
 * @tc.desc: SnapshotIgnorePrivacy
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SnapshotIgnorePrivacy, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowSceneSession->SnapshotIgnorePrivacy(pixelMap));

    windowSceneSession->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreatTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    windowSceneSession->hostSession_ = session;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowSceneSession->SnapshotIgnorePrivacy(pixelMap));

    windowSceneSession->surfaceNode_ = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_OPERATION, windowSceneSession->SnapshotIgnorePrivacy(pixelMap));
}

/**
 * @tc.name: BindDialogTarget01
 * @tc.desc: BindDialogTarget
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, BindDialogTarget01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("BindDialogTarget01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    sptr<IRemoteObject> targetToken;
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowSceneSession->BindDialogTarget(targetToken));
}

/**
 * @tc.name: BindDialogTarget02
 * @tc.desc: BindDialogTarget
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, BindDialogTarget02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("BindDialogTarget01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestMode", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    windowSceneSession->property_->SetPersistentId(1);
    windowSceneSession->hostSession_ = session;
    sptr<IRemoteObject> targetToken;
    WMError ret = windowSceneSession->BindDialogTarget(targetToken);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: SetDialogBackGestureEnabled01
 * @tc.desc: SetDialogBackGestureEnabled
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetDialogBackGestureEnabled01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetDialogBackGestureEnabled01");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetDialogBackGestureEnabled02");
    option->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    WMError ret = windowSceneSession->SetDialogBackGestureEnabled(true);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: SetDialogBackGestureEnabled03
 * @tc.desc: SetDialogBackGestureEnabled
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetDialogBackGestureEnabled03, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetDialogBackGestureEnabled02");
    option->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    windowSceneSession->hostSession_ = session;
    windowSceneSession->property_->SetPersistentId(1);
    WMError ret = windowSceneSession->SetDialogBackGestureEnabled(true);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: NotifySessionForeground
 * @tc.desc: NotifySessionForeground
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, NotifySessionForeground, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("NotifySessionForeground");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSession);

    uint32_t reason = 1;
    bool withAnimation = true;
    windowSceneSession->NotifySessionForeground(reason, withAnimation);
    ASSERT_EQ(false, windowSceneSession->GetDefaultDensityEnabled());
}

/**
 * @tc.name: NotifySessionBackground
 * @tc.desc: NotifySessionBackground
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, NotifySessionBackground, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("NotifySessionBackground");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, windowSceneSession);

    uint32_t reason = 1;
    bool withAnimation = true;
    bool isFromInnerkits = true;
    windowSceneSession->NotifySessionBackground(reason, withAnimation, isFromInnerkits);
    ASSERT_EQ(false, windowSceneSession->GetDefaultDensityEnabled());
}

/**
 * @tc.name: NotifySessionFullScreen
 * @tc.desc: NotifySessionFullScreen
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, NotifySessionFullScreen, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("NotifySessionFullScreen");
    option->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("NotifyPrepareClosePiPWindow01");
    option->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetDefaultDensityEnabled01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_SHOWN;
    window->property_->SetDisplayId(0);

    WindowSceneSessionImpl::windowSessionMap_.insert(std::make_pair(
        window->GetWindowName(), std::pair<uint64_t, sptr<WindowSessionImpl>>(window->GetWindowId(), window)));
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetDefaultDensityEnabled02");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetDefaultDensityEnabled03");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetDefaultDensityEnabled04");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetDefaultDensityEnabled01");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetDefaultDensityEnabled02");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_EQ(false, window->GetDefaultDensityEnabled());
}

/**
 * @tc.name: GetVirtualPixelRatio01
 * @tc.desc: main window isDefaultDensityEnabled_ true
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, GetVirtualPixelRatio01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetVirtualPixelRatio01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_SHOWN;
    window->property_->SetDisplayId(0);

    window->isDefaultDensityEnabled_ = true;
    sptr<DisplayInfo> displayInfo = sptr<DisplayInfo>::MakeSptr();
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetVirtualPixelRatio02");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_SHOWN;
    window->property_->SetDisplayId(0);

    window->isDefaultDensityEnabled_ = false;
    sptr<DisplayInfo> displayInfo = sptr<DisplayInfo>::MakeSptr();
    auto defautDensity = 1.0f;
    auto density = 2.0f;
    displayInfo->SetDefaultVirtualPixelRatio(defautDensity);
    displayInfo->SetVirtualPixelRatio(density);
    ASSERT_EQ(density, window->GetVirtualPixelRatio(displayInfo));

    window->useUniqueDensity_ = true;
    ASSERT_EQ(window->virtualPixelRatio_, window->GetVirtualPixelRatio(displayInfo));
}

/**
 * @tc.name: GetVirtualPixelRatio03
 * @tc.desc: sub window isDefaultDensityEnabled_ true
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, GetVirtualPixelRatio03, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetVirtualPixelRatio03");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_SHOWN;
    window->property_->SetDisplayId(0);

    window->isDefaultDensityEnabled_ = true;
    sptr<DisplayInfo> displayInfo = sptr<DisplayInfo>::MakeSptr();
    auto defautDensity = 1.0f;
    auto density = 2.0f;
    displayInfo->SetDefaultVirtualPixelRatio(defautDensity);
    displayInfo->SetVirtualPixelRatio(density);

    sptr<WindowOption> subOption = sptr<WindowOption>::MakeSptr();
    subOption->SetWindowName("GetVirtualPixelRatio03SubWindow");
    subOption->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowSceneSessionImpl> subWindow = sptr<WindowSceneSessionImpl>::MakeSptr(subOption);
    subWindow->property_->SetParentPersistentId(window->GetWindowId());

    WindowSceneSessionImpl::windowSessionMap_.insert(std::make_pair(
        window->GetWindowName(), std::pair<uint64_t, sptr<WindowSessionImpl>>(window->GetWindowId(), window)));
    ASSERT_EQ(density, subWindow->GetVirtualPixelRatio(displayInfo));
    WindowSceneSessionImpl::windowSessionMap_.erase(window->GetWindowName());
}

/**
 * @tc.name: GetVirtualPixelRatio04
 * @tc.desc: sub window isDefaultDensityEnabled_ false
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, GetVirtualPixelRatio04, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetVirtualPixelRatio04");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_SHOWN;
    window->property_->SetDisplayId(0);

    window->isDefaultDensityEnabled_ = false;
    sptr<DisplayInfo> displayInfo = sptr<DisplayInfo>::MakeSptr();
    auto defautDensity = 1.0f;
    auto density = 2.0f;
    displayInfo->SetDefaultVirtualPixelRatio(defautDensity);
    displayInfo->SetVirtualPixelRatio(density);

    sptr<WindowOption> subOption = sptr<WindowOption>::MakeSptr();
    subOption->SetWindowName("GetVirtualPixelRatio04SubWindow");
    subOption->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowSceneSessionImpl> subWindow = sptr<WindowSceneSessionImpl>::MakeSptr(subOption);
    subWindow->property_->SetParentPersistentId(window->GetWindowId());

    WindowSceneSessionImpl::windowSessionMap_.insert(std::make_pair(
        window->GetWindowName(), std::pair<uint64_t, sptr<WindowSessionImpl>>(window->GetWindowId(), window)));
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetWindowLimits01");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    WindowLimits windowLimits = { 1000, 1000, 1000, 1000, 0.0f, 0.0f };
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->property_->SetWindowLimits(windowLimits);
    window->property_->SetPersistentId(1);
    window->state_ = WindowState::STATE_HIDDEN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;

    WindowLimits windowSizeLimits;
    ASSERT_EQ(WMError::WM_OK, window->GetWindowLimits(windowSizeLimits));
    ASSERT_EQ(windowSizeLimits.maxWidth_, 1000);
    ASSERT_EQ(windowSizeLimits.maxHeight_, 1000);
    ASSERT_EQ(windowSizeLimits.minWidth_, 1000);
    ASSERT_EQ(windowSizeLimits.minHeight_, 1000);
}

/**
 * @tc.name: AdjustKeyboardLayout01
 * @tc.desc: adjust keyboard layout
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, AdjustKeyboardLayout01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("AdjustKeyboardLayout01");
    option->SetDisplayId(0);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(123);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->state_ = WindowState::STATE_FROZEN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;

    KeyboardLayoutParams params;
    params.gravity_ = WindowGravity::WINDOW_GRAVITY_FLOAT;
    params.LandscapeKeyboardRect_ = { 1, 2, 3, 4 };
    params.PortraitKeyboardRect_ = { 1, 2, 3, 4 };
    params.LandscapePanelRect_ = { 1, 2, 3, 4 };
    params.PortraitPanelRect_ = { 1, 2, 3, 4 };
    ASSERT_EQ(WMError::WM_OK, window->AdjustKeyboardLayout(params));
}

/**
 * @tc.name: HideNonSecureWindows01
 * @tc.desc: HideNonSecureWindows
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, HideNonSecureWindows01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("HideNonSecureWindows01");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_EQ(WMError::WM_OK, window->HideNonSecureWindows(true));
}

/**
 * @tc.name: SetGrayScale01
 * @tc.desc: SetGrayScale
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetGrayScale01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    window->state_ = WindowState::STATE_SHOWN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->state_ = WindowState::STATE_SHOWN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->SetGrayScale(0.5));
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
 * @tc.name: SetGrayScale04
 * @tc.desc: Infinite non-circulating decimals
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetGrayScale04, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->state_ = WindowState::STATE_SHOWN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->SetGrayScale(0.5));
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    std::vector<WindowType> types = { WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
                                      WindowType::WINDOW_TYPE_APP_SUB_WINDOW,
                                      WindowType::SYSTEM_WINDOW_BASE };
    for (WindowType type : types) {
        window->SetWindowType(type);
        float grayScale = 1.0f / 3.0f;
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("Maximize02");
    option->SetDisplayId(0);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    MaximizePresentation presentation = MaximizePresentation::ENTER_IMMERSIVE;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Maximize(presentation));

    window->property_->SetWindowName("Maximize02");
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    window->property_->SetPersistentId(2);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    // not support subWinodw call
    ASSERT_EQ(WMError::WM_ERROR_INVALID_CALLING, window->Maximize(presentation));

    // window not support fullscreen
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->property_->SetWindowModeSupportType(WindowModeSupport::WINDOW_MODE_SUPPORT_PIP);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Maximize(presentation));
}

/**
 * @tc.name: Maximize03
 * @tc.desc: test presentation
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, Maximize03, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("Maximize03");
    option->SetDisplayId(0);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetWindowName("Maximize03");
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->property_->SetPersistentId(2);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;

    // case1: only set maximize()
    MaximizePresentation presentation = MaximizePresentation::ENTER_IMMERSIVE;
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    auto ret = window->Maximize(presentation);
    ASSERT_EQ(WMError::WM_OK, ret);
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ret = window->Maximize(presentation);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(window->GetImmersiveModeEnabledState(), true);

    // case2: maximize(EXIT_IMMERSIVE) and the immersive value will be set ad false
    presentation = MaximizePresentation::EXIT_IMMERSIVE;
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ret = window->Maximize(presentation);
    ASSERT_EQ(WMError::WM_OK, ret);
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ret = window->Maximize(presentation);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(window->GetImmersiveModeEnabledState(), false);

    // case3: maximize(FOLLOW_APP_IMMERSIVE_SETTING) and the immersive value will be set as client set
    presentation = MaximizePresentation::FOLLOW_APP_IMMERSIVE_SETTING;
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ret = window->Maximize(presentation);
    ASSERT_EQ(WMError::WM_OK, ret);
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ret = window->Maximize(presentation);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(window->GetImmersiveModeEnabledState(), false);

    // case4: maximize(ENTER_IMMERSIVE_DISABLE_TITLE_AND_DOCK_HOVER) and the immersive value will be set as true
    presentation = MaximizePresentation::ENTER_IMMERSIVE_DISABLE_TITLE_AND_DOCK_HOVER;
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ret = window->Maximize(presentation);
    ASSERT_EQ(WMError::WM_OK, ret);
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
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
    WindowLimits windowLimits = { 3, 3, 3, 3, 2.0, 2.0 };
    ASSERT_EQ(windowLimits.IsEmpty(), false);
    Rect rect = { 2, 2, 2, 2 };
    Rect rect1 = { 2, 2, 2, 2 };
    ASSERT_EQ(rect.IsInsideOf(rect1), true);
    TitleButtonRect titleButtonRect = { 3, 3, 3, 3 };
    TitleButtonRect titleButtonRect1 = { 3, 3, 3, 3 };
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
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    window->RegisterSessionRecoverListener(false);
    ASSERT_EQ(SingletonContainer::Get<WindowAdapter>().sessionRecoverCallbackFuncMap_.size(), 0);
}

/**
 * @tc.name: RegisterSessionRecoverListenerNonDefaultCollaboratorType01
 * @tc.desc: Register session recover listener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2,
         RegisterSessionRecoverListenerNonDefaultCollaboratorType01,
         Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->property_->SetCollaboratorType(CollaboratorType::OTHERS_TYPE);
    window->RegisterSessionRecoverListener(false); // false is main window
    ASSERT_EQ(SingletonContainer::Get<WindowAdapter>().sessionRecoverCallbackFuncMap_.size(), 0);
}

/**
 * @tc.name: RegisterSessionRecoverListenerNonDefaultCollaboratorType02
 * @tc.desc: Register session recover listener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2,
         RegisterSessionRecoverListenerNonDefaultCollaboratorType02,
         Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->property_->SetCollaboratorType(CollaboratorType::OTHERS_TYPE);
    window->RegisterSessionRecoverListener(true); // true is sub window
    ASSERT_EQ(SingletonContainer::Get<WindowAdapter>().sessionRecoverCallbackFuncMap_.size(), 0);
}

/**
 * @tc.name: RegisterSessionRecoverListenerSuccess01
 * @tc.desc: Register session recover listener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, RegisterSessionRecoverListenerSuccess01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->property_->SetCollaboratorType(CollaboratorType::DEFAULT_TYPE);
    window->RegisterSessionRecoverListener(false); // false is main window
    ASSERT_EQ(SingletonContainer::Get<WindowAdapter>().sessionRecoverCallbackFuncMap_.size(), 1);
    SingletonContainer::Get<WindowAdapter>().sessionRecoverCallbackFuncMap_.clear();
}

/**
 * @tc.name: RegisterSessionRecoverListenerSuccess02
 * @tc.desc: Register session recover listener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, RegisterSessionRecoverListenerSuccess02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->property_->SetCollaboratorType(CollaboratorType::DEFAULT_TYPE);
    window->RegisterSessionRecoverListener(true); // true is sub window
    ASSERT_EQ(SingletonContainer::Get<WindowAdapter>().sessionRecoverCallbackFuncMap_.size(), 1);
    SingletonContainer::Get<WindowAdapter>().sessionRecoverCallbackFuncMap_.clear();
}

/**
 * @tc.name: GetTitleButtonVisible01
 * @tc.desc: GetTitleButtonVisible
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, GetTitleButtonVisible01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetTitleButtonVisible01");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    uint32_t windowModeSupportType = 1 | (1 << 1) | (1 << 2);
    window->property_->SetWindowModeSupportType(windowModeSupportType);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    // show Maximize, Minimize, Split buttons.
    window->windowTitleVisibleFlags_ = { false, false, false, false };
    bool hideMaximizeButton = false;
    bool hideMinimizeButton = false;
    bool hideSplitButton = false;
    bool hideCloseButton = false;
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    window->GetTitleButtonVisible(hideMaximizeButton, hideMinimizeButton, hideSplitButton, hideCloseButton);
    ASSERT_EQ(hideMaximizeButton, true);
    ASSERT_EQ(hideMinimizeButton, true);
    ASSERT_EQ(hideSplitButton, true);
    ASSERT_EQ(hideCloseButton, true);
}

/**
 * @tc.name: GetTitleButtonVisible02
 * @tc.desc: GetTitleButtonVisible
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, GetTitleButtonVisible02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetTitleButtonVisible02");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    // only not support WINDOW_MODE_SUPPORT_SPLIT
    uint32_t windowModeSupportType = 1 | (1 << 1);
    window->property_->SetWindowModeSupportType(windowModeSupportType);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    // show Maximize, Minimize, Split buttons.
    window->windowTitleVisibleFlags_ = { true, true, true, true };
    bool hideMaximizeButton = false;
    bool hideMinimizeButton = false;
    bool hideSplitButton = false;
    bool hideCloseButton = false;
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    window->GetTitleButtonVisible(hideMaximizeButton, hideMinimizeButton, hideSplitButton, hideCloseButton);
    ASSERT_EQ(hideMaximizeButton, false);
    ASSERT_EQ(hideMinimizeButton, false);
    ASSERT_EQ(hideSplitButton, false);
    ASSERT_EQ(hideCloseButton, false);
}

/**
 * @tc.name: GetTitleButtonVisible03
 * @tc.desc: GetTitleButtonVisible
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, GetTitleButtonVisible03, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetTitleButtonVisible03");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    // only not support WINDOW_MODE_SUPPORT_SPLIT
    uint32_t windowModeSupportType = 1 | (1 << 1) | (1 << 2);
    window->property_->SetWindowModeSupportType(windowModeSupportType);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    // show Maximize, Minimize, Split buttons.
    window->windowTitleVisibleFlags_ = { false, false, false, false };
    bool hideMaximizeButton = true;
    bool hideMinimizeButton = true;
    bool hideSplitButton = true;
    bool hideCloseButton = true;
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    window->GetTitleButtonVisible(hideMaximizeButton, hideMinimizeButton, hideSplitButton, hideCloseButton);
    ASSERT_EQ(hideMaximizeButton, true);
    ASSERT_EQ(hideMinimizeButton, true);
    ASSERT_EQ(hideSplitButton, true);
    ASSERT_EQ(hideCloseButton, true);
}

/**
 * @tc.name: SetTitleButtonVisible01
 * @tc.desc: SetTitleButtonVisible
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetTitleButtonVisible01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetTitleButtonVisible");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(window, nullptr);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    window->SetTitleButtonVisible(false, false, false, true);
}

/**
 * @tc.name: SetTitleButtonVisible02
 * @tc.desc: SetTitleButtonVisible
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetTitleButtonVisible02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetTitleButtonVisible");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    WMError res = window->SetTitleButtonVisible(false, false, false, true);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_WINDOW);
}

/**
 * @tc.name: SetTitleButtonVisible03
 * @tc.desc: SetTitleButtonVisible
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetTitleButtonVisible03, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetTitleButtonVisible");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    window->property_->SetPersistentId(1);
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->windowSystemConfig_.freeMultiWindowSupport_ = true;
    window->windowSystemConfig_.isSystemDecorEnable_ = false;
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    WMError res = window->SetTitleButtonVisible(false, false, false, true);
    ASSERT_EQ(res, WMError::WM_ERROR_DEVICE_NOT_SUPPORT);
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    res = window->SetTitleButtonVisible(false, false, false, true);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_WINDOW);
    window->windowSystemConfig_.isSystemDecorEnable_ = true;
    res = window->SetTitleButtonVisible(false, false, false, true);
    ASSERT_EQ(res, WMError::WM_OK);
}

/**
 * @tc.name: IsWindowRectAutoSave
 * @tc.desc: IsWindowRectAutoSave
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, IsWindowRectAutoSave, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSceneSessionImplTest2: IsWindowRectAutoSave start";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("IsWindowRectAutoSave");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    bool enabled = false;
    auto ret = windowSceneSessionImpl->IsWindowRectAutoSave(enabled);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    enabled = true;
    ret = windowSceneSessionImpl->IsWindowRectAutoSave(enabled);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->context_ = abilityContext_;
    windowSceneSessionImpl->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    enabled = false;
    ret = windowSceneSessionImpl->IsWindowRectAutoSave(enabled);
    EXPECT_EQ(WMError::WM_OK, ret);
    enabled = true;
    ret = windowSceneSessionImpl->IsWindowRectAutoSave(enabled);
    EXPECT_EQ(WMError::WM_OK, ret);
    windowSceneSessionImpl->context_ = nullptr;
    enabled = false;
    ret = windowSceneSessionImpl->IsWindowRectAutoSave(enabled);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    enabled = true;
    ret = windowSceneSessionImpl->IsWindowRectAutoSave(enabled);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ret = windowSceneSessionImpl->IsWindowRectAutoSave(enabled);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_CALLING, ret);
    windowSceneSessionImpl->windowSystemConfig_.windowUIType_ = WindowUIType::PAD_WINDOW;
    ret = windowSceneSessionImpl->IsWindowRectAutoSave(enabled);
    EXPECT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, ret);
    windowSceneSessionImpl->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ret = windowSceneSessionImpl->IsWindowRectAutoSave(enabled);
    EXPECT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, ret);
    GTEST_LOG_(INFO) << "WindowSceneSessionImplTest2: IsWindowRectAutoSave end";
}

/**
 * @tc.name: IsWindowRectAutoSave
 * @tc.desc: IsWindowRectAutoSave
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, IsWindowRectAutoSave002, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("IsWindowRectAutoSave002");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->context_ = abilityContext_;
    windowSceneSessionImpl->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    bool enabled = false;
    auto ret = windowSceneSessionImpl->IsWindowRectAutoSave(enabled);
    EXPECT_EQ(WMError::WM_OK, ret);

    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ret = windowSceneSessionImpl->IsWindowRectAutoSave(enabled);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_CALLING, ret);

    option->SetWindowName("IsWindowRectAutoSave002");
    windowSceneSessionImpl->state_ = WindowState::STATE_INITIAL;
    windowSceneSessionImpl->property_->SetPersistentId(1);
    ret = windowSceneSessionImpl->IsWindowRectAutoSave(enabled);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_CALLING, ret);
}

/**
 * @tc.name: SetWindowShadowRadius
 * @tc.desc: SetWindowShadowRadius
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetWindowShadowRadius, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetWindowShadowRadius");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetWindowShadowRadius(0.0f));
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);

    window->surfaceNode_ = nullptr;
    auto ret = window->SetWindowShadowRadius(1.0f);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    ret = window->SetWindowShadowRadius(1.0f);
    EXPECT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, ret);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ret = window->SetWindowShadowRadius(1.0f);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_CALLING, ret);

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    auto surfaceNodeMocker = CreateRSSurfaceNode();
    ASSERT_NE(nullptr, surfaceNodeMocker);
    window->surfaceNode_ = surfaceNodeMocker;
    ret = window->SetWindowShadowRadius(-1.0f);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PAD_WINDOW;
    ret = window->SetWindowShadowRadius(1.0f);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: SetWindowCornerRadius
 * @tc.desc: SetWindowCornerRadius
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, SetWindowCornerRadius, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetWindowCornerRadius");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetWindowCornerRadius(0.0f));
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    auto ret = window->SetWindowCornerRadius(1.0f);
    EXPECT_EQ(WMError::WM_OK, ret);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ret = window->SetWindowCornerRadius(1.0f);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = window->SetWindowShadowRadius(-1.0f);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ret = window->SetWindowShadowRadius(1.0f);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_CALLING, ret);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    ret = window->SetWindowShadowRadius(1.0f);
    EXPECT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, ret);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PAD_WINDOW;
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    ret = window->SetWindowShadowRadius(1.0f);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: GetWindowCornerRadius
 * @tc.desc: GetWindowCornerRadius
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest2, GetWindowCornerRadius, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetWindowCornerRadius");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    float cornerRadius = 0.0f;
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->GetWindowCornerRadius(cornerRadius));
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    window->property_->SetWindowCornerRadius(1.0f);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    auto ret = window->GetWindowCornerRadius(cornerRadius);
    EXPECT_EQ(1.0f, cornerRadius);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ret = window->GetWindowCornerRadius(cornerRadius);
    EXPECT_EQ(1.0f, cornerRadius);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ret = window->GetWindowCornerRadius(cornerRadius);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_CALLING, ret);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ret = window->GetWindowCornerRadius(cornerRadius);
    EXPECT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, ret);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PAD_WINDOW;
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    ret = window->GetWindowCornerRadius(cornerRadius);
    EXPECT_EQ(1.0f, cornerRadius);
}
} // namespace
} // namespace Rosen
} // namespace OHOS