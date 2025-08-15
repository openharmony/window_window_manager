/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "application_context.h"
#include "input_event.h"
#include "key_event.h"
#include "mock/mock_session_stage.h"
#include "modifier_render_thread/rs_modifiers_draw_thread.h"
#include "pointer_event.h"

#include "screen_session_manager_client/include/screen_session_manager_client.h"
#include "session/host/include/main_session.h"
#include "session/host/include/keyboard_session.h"
#define PRIVATE public
#define PROTECTED public
#include "session/host/include/scene_session.h"
#undef PRIVATE
#undef PROTECTED
#include "session/host/include/sub_session.h"
#include "session/host/include/system_session.h"
#include "ui/rs_surface_node.h"
#include "window_helper.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SceneSessionAnimationTest : public testing::Test {
public:
    SceneSessionAnimationTest();
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    sptr<ScreenSession> defaultScreenSession_;
    ScreenSessionManagerClient& ssmClient_;
};

SceneSessionAnimationTest::SceneSessionAnimationTest() : ssmClient_(ScreenSessionManagerClient::GetInstance())
{
    constexpr ScreenId defaultScreenId = 1001;
    auto screenProperty = ScreenProperty();
    defaultScreenSession_ = sptr<ScreenSession>::MakeSptr(defaultScreenId, screenProperty, defaultScreenId);
    {
        std::lock_guard<std::mutex> lock(ssmClient_.screenSessionMapMutex_);
        ssmClient_.screenSessionMap_[defaultScreenId] = defaultScreenSession_;
    }
}

void SceneSessionAnimationTest::SetUpTestCase() {}

void SceneSessionAnimationTest::TearDownTestCase()
{
#ifdef RS_ENABLE_VK
    RSModifiersDrawThread::Destroy();
#endif
}

void SceneSessionAnimationTest::SetUp() {}

void SceneSessionAnimationTest::TearDown() {}

namespace {
/**
 * @tc.name: SetWindowCornerRadiusCallback
 * @tc.desc: SetWindowCornerRadiusCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, SetWindowCornerRadiusCallback, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetWindowCornerRadiusCallback";
    info.bundleName_ = "SetWindowCornerRadiusCallback";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    NotifySetWindowCornerRadiusFunc func1 = [](float cornerRadius) { return; };
    sceneSession->SetWindowCornerRadiusCallback(std::move(func1));
    ASSERT_NE(nullptr, sceneSession->onSetWindowCornerRadiusFunc_);
    ASSERT_NE(nullptr, sceneSession->property_);
    EXPECT_EQ(WINDOW_CORNER_RADIUS_INVALID, sceneSession->property_->GetWindowCornerRadius());
}

/**
 * @tc.name: SetWindowCornerRadiusCallback01
 * @tc.desc: SetWindowCornerRadiusCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, SetWindowCornerRadiusCallback01, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetWindowCornerRadiusCallback01";
    info.bundleName_ = "SetWindowCornerRadiusCallback01";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    NotifySetWindowCornerRadiusFunc func1 = [](float cornerRadius) { return; };
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowCornerRadius(1); // 1 is valid window corner radius
    sceneSession->SetWindowCornerRadiusCallback(std::move(func1));
    ASSERT_NE(nullptr, sceneSession->onSetWindowCornerRadiusFunc_);
    EXPECT_EQ(1, sceneSession->property_->GetWindowCornerRadius()); // 1 is valid window corner radius
}

/**
 * @tc.name: OnSetWindowCornerRadius
 * @tc.desc: OnSetWindowCornerRadius
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, SetWindowCornerRadius, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetWindowCornerRadius";
    info.bundleName_ = "SetWindowCornerRadius";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    EXPECT_EQ(WSError::WS_OK, session->SetWindowCornerRadius(1.0f));

    NotifySetWindowCornerRadiusFunc func = [](float cornerRadius) { return; };
    session->onSetWindowCornerRadiusFunc_ = func;
    EXPECT_EQ(WSError::WS_OK, session->SetWindowCornerRadius(1.0f));
}

/**
 * @tc.name: AddSidebarBlur01
 * @tc.desc: AddSidebarBlur01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, AddSidebarBlur01, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "AddSidebarBlur01";
    info.bundleName_ = "AddSidebarBlur01";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    session->SetScreenId(defaultScreenSession_->GetScreenId());

    struct RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    EXPECT_EQ(nullptr, session->GetSurfaceNode());

    session->AddSidebarBlur();
    EXPECT_EQ(nullptr, session->blurRadiusValue_);
    EXPECT_EQ(nullptr, session->blurSaturationValue_);
    EXPECT_EQ(nullptr, session->blurBrightnessValue_);
    EXPECT_EQ(nullptr, session->blurMaskColorValue_);

    session->SetSurfaceNode(surfaceNode);
    EXPECT_NE(nullptr, session->GetSurfaceNode());

    AbilityRuntime::Context::applicationContext_ = std::make_shared<AbilityRuntime::ApplicationContext>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext = AbilityRuntime::Context::GetApplicationContext();
    EXPECT_NE(nullptr, appContext);

    appContext->contextImpl_ = std::make_shared<AbilityRuntime::ContextImpl>();
    appContext->contextImpl_->config_ = std::make_shared<AppExecFwk::Configuration>();
    std::shared_ptr<AppExecFwk::Configuration> appContextConfig = appContext->GetConfiguration();
    EXPECT_NE(nullptr, appContextConfig);

    appContextConfig->AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE,
                              AppExecFwk::ConfigurationInner::COLOR_MODE_DARK);

    session->AddSidebarBlur();
    EXPECT_NE(nullptr, session->blurRadiusValue_);
    EXPECT_NE(nullptr, session->blurSaturationValue_);
    EXPECT_NE(nullptr, session->blurBrightnessValue_);
    EXPECT_NE(nullptr, session->blurMaskColorValue_);
}

/**
 * @tc.name: AddSidebarBlur02
 * @tc.desc: AddSidebarBlur02
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, AddSidebarBlur02, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "AddSidebarBlur02";
    info.bundleName_ = "AddSidebarBlur02";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    session->SetScreenId(defaultScreenSession_->GetScreenId());

    struct RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    EXPECT_EQ(nullptr, session->GetSurfaceNode());

    session->AddSidebarBlur();
    EXPECT_EQ(nullptr, session->blurRadiusValue_);
    EXPECT_EQ(nullptr, session->blurSaturationValue_);
    EXPECT_EQ(nullptr, session->blurBrightnessValue_);
    EXPECT_EQ(nullptr, session->blurMaskColorValue_);

    session->SetSurfaceNode(surfaceNode);
    EXPECT_NE(nullptr, session->GetSurfaceNode());

    AbilityRuntime::Context::applicationContext_ = std::make_shared<AbilityRuntime::ApplicationContext>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext = AbilityRuntime::Context::GetApplicationContext();
    EXPECT_NE(nullptr, appContext);

    appContext->contextImpl_ = std::make_shared<AbilityRuntime::ContextImpl>();
    appContext->contextImpl_->config_ = std::make_shared<AppExecFwk::Configuration>();
    std::shared_ptr<AppExecFwk::Configuration> appContextConfig = appContext->GetConfiguration();
    EXPECT_NE(nullptr, appContextConfig);

    appContextConfig->AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE,
                              AppExecFwk::ConfigurationInner::COLOR_MODE_LIGHT);

    session->AddSidebarBlur();
    EXPECT_NE(nullptr, session->blurRadiusValue_);
    EXPECT_NE(nullptr, session->blurSaturationValue_);
    EXPECT_NE(nullptr, session->blurBrightnessValue_);
    EXPECT_NE(nullptr, session->blurMaskColorValue_);
}

/*
 * @c.name: AddSidebarBlur03
 * @c.desc: AddSidebarBlur03
 * @c.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, AddSidebarBlur03, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "AddSidebarBlur03";
    info.bundleName_ = "AddSidebarBlur03";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    session->SetScreenId(defaultScreenSession_->GetScreenId());

    struct RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    EXPECT_EQ(nullptr, session->GetSurfaceNode());
    session->SetSurfaceNode(surfaceNode);
    EXPECT_NE(nullptr, session->GetSurfaceNode());

    AbilityRuntime::Context::applicationContext_ = std::make_shared<AbilityRuntime::ApplicationContext>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext = AbilityRuntime::Context::GetApplicationContext();
    EXPECT_NE(nullptr, appContext);

    appContext->contextImpl_ = std::make_shared<AbilityRuntime::ContextImpl>();
    appContext->contextImpl_->config_ = std::make_shared<AppExecFwk::Configuration>();
    std::shared_ptr<AppExecFwk::Configuration> appContextConfig = appContext->GetConfiguration();
    EXPECT_NE(nullptr, appContextConfig);

    appContextConfig->AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE,
                              AppExecFwk::ConfigurationInner::COLOR_MODE_DARK);

    session->AddSidebarBlur();
    float radiusOpenDark = session->blurRadiusValue_->Get();
    float saturationOpenDark = session->blurSaturationValue_->Get();
    float brightnessOpenDark = session->blurBrightnessValue_->Get();
    Rosen::RSColor colorOpenDark = session->blurMaskColorValue_->Get();
    EXPECT_EQ(SIDEBAR_DEFAULT_RADIUS_DARK, radiusOpenDark);
    EXPECT_EQ(SIDEBAR_DEFAULT_SATURATION_DARK, saturationOpenDark);
    EXPECT_EQ(SIDEBAR_DEFAULT_BRIGHTNESS_DARK, brightnessOpenDark);
    EXPECT_EQ(SIDEBAR_DEFAULT_MASKCOLOR_DARK, colorOpenDark.AsArgbInt());
}

/**
 * @tc.name: AddSidebarBlur04
 * @tc.desc: AddSidebarBlur04
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, AddSidebarBlur04, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "AddSidebarBlur04";
    info.bundleName_ = "AddSidebarBlur04";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    session->SetScreenId(defaultScreenSession_->GetScreenId());

    struct RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    EXPECT_EQ(nullptr, session->GetSurfaceNode());
    session->SetSurfaceNode(surfaceNode);
    EXPECT_NE(nullptr, session->GetSurfaceNode());

    AbilityRuntime::Context::applicationContext_ = std::make_shared<AbilityRuntime::ApplicationContext>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext = AbilityRuntime::Context::GetApplicationContext();
    EXPECT_NE(nullptr, appContext);

    appContext->contextImpl_ = std::make_shared<AbilityRuntime::ContextImpl>();
    appContext->contextImpl_->config_ = std::make_shared<AppExecFwk::Configuration>();
    std::shared_ptr<AppExecFwk::Configuration> appContextConfig = appContext->GetConfiguration();
    EXPECT_NE(nullptr, appContextConfig);

    appContextConfig->AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE,
                              AppExecFwk::ConfigurationInner::COLOR_MODE_LIGHT);

    session->AddSidebarBlur();
    float radiusOpenLight = session->blurRadiusValue_->Get();
    float saturationOpenLight = session->blurSaturationValue_->Get();
    float brightnessOpenLight = session->blurBrightnessValue_->Get();
    Rosen::RSColor colorOpenLight = session->blurMaskColorValue_->Get();
    EXPECT_EQ(SIDEBAR_DEFAULT_RADIUS_LIGHT, radiusOpenLight);
    EXPECT_EQ(SIDEBAR_DEFAULT_SATURATION_LIGHT, saturationOpenLight);
    EXPECT_EQ(SIDEBAR_DEFAULT_BRIGHTNESS_LIGHT, brightnessOpenLight);
    EXPECT_EQ(SIDEBAR_DEFAULT_MASKCOLOR_LIGHT, colorOpenLight.AsArgbInt());
}

/**
 * @tc.name: SetSidebarBlur01
 * @tc.desc: SetSidebarBlur01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, SetSidebarBlur01, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetSidebarBlur01";
    info.bundleName_ = "SetSidebarBlur01";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    session->SetScreenId(defaultScreenSession_->GetScreenId());

    struct RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    EXPECT_EQ(nullptr, session->GetSurfaceNode());

    session->AddSidebarBlur();
    EXPECT_EQ(nullptr, session->blurRadiusValue_);
    EXPECT_EQ(nullptr, session->blurSaturationValue_);
    EXPECT_EQ(nullptr, session->blurBrightnessValue_);
    EXPECT_EQ(nullptr, session->blurMaskColorValue_);

    session->SetSurfaceNode(surfaceNode);
    EXPECT_NE(nullptr, session->GetSurfaceNode());

    AbilityRuntime::Context::applicationContext_ = std::make_shared<AbilityRuntime::ApplicationContext>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext = AbilityRuntime::Context::GetApplicationContext();
    EXPECT_NE(nullptr, appContext);

    appContext->contextImpl_ = std::make_shared<AbilityRuntime::ContextImpl>();
    appContext->contextImpl_->config_ = std::make_shared<AppExecFwk::Configuration>();
    std::shared_ptr<AppExecFwk::Configuration> appContextConfig = appContext->GetConfiguration();
    EXPECT_NE(nullptr, appContextConfig);

    appContextConfig->AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE,
                              AppExecFwk::ConfigurationInner::COLOR_MODE_DARK);

    session->AddSidebarBlur();
    EXPECT_NE(nullptr, session->blurRadiusValue_);
    EXPECT_NE(nullptr, session->blurSaturationValue_);
    EXPECT_NE(nullptr, session->blurBrightnessValue_);
    EXPECT_NE(nullptr, session->blurMaskColorValue_);
}

/**
 * @tc.name: SetSidebarBlur02
 * @tc.desc: SetSidebarBlur02
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, SetSidebarBlur02, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetSidebarBlur02";
    info.bundleName_ = "SetSidebarBlur02";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    session->SetScreenId(defaultScreenSession_->GetScreenId());

    struct RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    EXPECT_EQ(nullptr, session->GetSurfaceNode());

    session->AddSidebarBlur();
    EXPECT_EQ(nullptr, session->blurRadiusValue_);
    EXPECT_EQ(nullptr, session->blurSaturationValue_);
    EXPECT_EQ(nullptr, session->blurBrightnessValue_);
    EXPECT_EQ(nullptr, session->blurMaskColorValue_);

    session->SetSurfaceNode(surfaceNode);
    EXPECT_NE(nullptr, session->GetSurfaceNode());

    AbilityRuntime::Context::applicationContext_ = std::make_shared<AbilityRuntime::ApplicationContext>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext = AbilityRuntime::Context::GetApplicationContext();
    EXPECT_NE(nullptr, appContext);

    appContext->contextImpl_ = std::make_shared<AbilityRuntime::ContextImpl>();
    appContext->contextImpl_->config_ = std::make_shared<AppExecFwk::Configuration>();
    std::shared_ptr<AppExecFwk::Configuration> appContextConfig = appContext->GetConfiguration();
    EXPECT_NE(nullptr, appContextConfig);

    appContextConfig->AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE,
                              AppExecFwk::ConfigurationInner::COLOR_MODE_LIGHT);

    session->AddSidebarBlur();
    EXPECT_NE(nullptr, session->blurRadiusValue_);
    EXPECT_NE(nullptr, session->blurSaturationValue_);
    EXPECT_NE(nullptr, session->blurBrightnessValue_);
    EXPECT_NE(nullptr, session->blurMaskColorValue_);
}

/**
 * @tc.name: SetSidebarBlur03
 * @tc.desc: SetSidebarBlur03
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, SetSidebarBlur03, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetSidebarBlur03";
    info.bundleName_ = "SetSidebarBlur03";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    session->SetScreenId(defaultScreenSession_->GetScreenId());

    struct RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    EXPECT_EQ(nullptr, session->GetSurfaceNode());
    session->SetSurfaceNode(surfaceNode);
    EXPECT_NE(nullptr, session->GetSurfaceNode());

    AbilityRuntime::Context::applicationContext_ = std::make_shared<AbilityRuntime::ApplicationContext>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext = AbilityRuntime::Context::GetApplicationContext();
    EXPECT_NE(nullptr, appContext);

    appContext->contextImpl_ = std::make_shared<AbilityRuntime::ContextImpl>();
    appContext->contextImpl_->config_ = std::make_shared<AppExecFwk::Configuration>();
    std::shared_ptr<AppExecFwk::Configuration> appContextConfig = appContext->GetConfiguration();
    EXPECT_NE(nullptr, appContextConfig);

    appContextConfig->AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE,
                              AppExecFwk::ConfigurationInner::COLOR_MODE_DARK);

    session->AddSidebarBlur();
    session->SetSidebarBlur(false, false);
    float radiusCloseDark = session->blurRadiusValue_->Get();
    float saturationCloseDark = session->blurSaturationValue_->Get();
    float brightnessCloseDark = session->blurBrightnessValue_->Get();
    Rosen::RSColor colorCloseDark = session->blurMaskColorValue_->Get();
    EXPECT_EQ(SIDEBAR_BLUR_NUMBER_ZERO, radiusCloseDark);
    EXPECT_EQ(SIDEBAR_BLUR_NUMBER_ZERO, saturationCloseDark);
    EXPECT_EQ(SIDEBAR_BLUR_NUMBER_ZERO, brightnessCloseDark);
    EXPECT_EQ(SIDEBAR_SNAPSHOT_MASKCOLOR_DARK, colorCloseDark.AsArgbInt());

    session->SetSidebarBlur(true, true);
    float radiusOpenDark = session->blurRadiusValue_->Get();
    float saturationOpenDark = session->blurSaturationValue_->Get();
    float brightnessOpenDark = session->blurBrightnessValue_->Get();
    Rosen::RSColor colorOpenDark = session->blurMaskColorValue_->Get();
    EXPECT_EQ(SIDEBAR_DEFAULT_RADIUS_DARK, radiusOpenDark);
    EXPECT_EQ(SIDEBAR_DEFAULT_SATURATION_DARK, saturationOpenDark);
    EXPECT_EQ(SIDEBAR_DEFAULT_BRIGHTNESS_DARK, brightnessOpenDark);
    EXPECT_EQ(SIDEBAR_DEFAULT_MASKCOLOR_DARK, colorOpenDark.AsArgbInt());
}

/**
 * @tc.name: SetSidebarBlur04
 * @tc.desc: SetSidebarBlur04
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, SetSidebarBlur04, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetSidebarBlur04";
    info.bundleName_ = "SetSidebarBlur04";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    session->SetScreenId(defaultScreenSession_->GetScreenId());

    struct RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    EXPECT_EQ(nullptr, session->GetSurfaceNode());
    session->SetSurfaceNode(surfaceNode);
    EXPECT_NE(nullptr, session->GetSurfaceNode());

    AbilityRuntime::Context::applicationContext_ = std::make_shared<AbilityRuntime::ApplicationContext>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext = AbilityRuntime::Context::GetApplicationContext();
    EXPECT_NE(nullptr, appContext);

    appContext->contextImpl_ = std::make_shared<AbilityRuntime::ContextImpl>();
    appContext->contextImpl_->config_ = std::make_shared<AppExecFwk::Configuration>();
    std::shared_ptr<AppExecFwk::Configuration> appContextConfig = appContext->GetConfiguration();
    EXPECT_NE(nullptr, appContextConfig);

    appContextConfig->AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE,
                              AppExecFwk::ConfigurationInner::COLOR_MODE_LIGHT);

    session->AddSidebarBlur();
    session->SetSidebarBlur(false, false);
    float radiusCloseLight = session->blurRadiusValue_->Get();
    float saturationCloseLight = session->blurSaturationValue_->Get();
    float brightnessCloseLight = session->blurBrightnessValue_->Get();
    Rosen::RSColor colorCloseLight = session->blurMaskColorValue_->Get();
    EXPECT_EQ(SIDEBAR_BLUR_NUMBER_ZERO, radiusCloseLight);
    EXPECT_EQ(SIDEBAR_BLUR_NUMBER_ZERO, saturationCloseLight);
    EXPECT_EQ(SIDEBAR_BLUR_NUMBER_ZERO, brightnessCloseLight);
    EXPECT_EQ(SIDEBAR_SNAPSHOT_MASKCOLOR_LIGHT, colorCloseLight.AsArgbInt());

    session->SetSidebarBlur(true, true);
    float radiusOpenLight = session->blurRadiusValue_->Get();
    float saturationOpenLight = session->blurSaturationValue_->Get();
    float brightnessOpenLight = session->blurBrightnessValue_->Get();
    Rosen::RSColor colorOpenLight = session->blurMaskColorValue_->Get();
    EXPECT_EQ(SIDEBAR_DEFAULT_RADIUS_LIGHT, radiusOpenLight);
    EXPECT_EQ(SIDEBAR_DEFAULT_SATURATION_LIGHT, saturationOpenLight);
    EXPECT_EQ(SIDEBAR_DEFAULT_BRIGHTNESS_LIGHT, brightnessOpenLight);
    EXPECT_EQ(SIDEBAR_DEFAULT_MASKCOLOR_LIGHT, colorOpenLight.AsArgbInt());
}

/**
 * @tc.name: SetSidebarBlurMaximize01
 * @tc.desc: SetSidebarBlurMaximize01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, SetSidebarBlurMaximize01, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetSidebarBlurMaximize01";
    info.bundleName_ = "SetSidebarBlurMaximize01";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    session->SetScreenId(defaultScreenSession_->GetScreenId());

    struct RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    EXPECT_EQ(nullptr, session->GetSurfaceNode());
    session->SetSurfaceNode(surfaceNode);
    EXPECT_NE(nullptr, session->GetSurfaceNode());

    AbilityRuntime::Context::applicationContext_ = std::make_shared<AbilityRuntime::ApplicationContext>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext = AbilityRuntime::Context::GetApplicationContext();
    EXPECT_NE(nullptr, appContext);

    appContext->contextImpl_ = std::make_shared<AbilityRuntime::ContextImpl>();
    appContext->contextImpl_->config_ = std::make_shared<AppExecFwk::Configuration>();
    std::shared_ptr<AppExecFwk::Configuration> appContextConfig = appContext->GetConfiguration();
    EXPECT_NE(nullptr, appContextConfig);

    appContextConfig->AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE,
                              AppExecFwk::ConfigurationInner::COLOR_MODE_DARK);

    session->AddSidebarBlur();
    session->SetSidebarBlurMaximize(false);
    float radiusRecoverDark = session->blurRadiusValue_->Get();
    float saturationRecoverDark = session->blurSaturationValue_->Get();
    float brightnessRecoverDark = session->blurBrightnessValue_->Get();
    Rosen::RSColor colorRecoverDark = session->blurMaskColorValue_->Get();
    EXPECT_EQ(SIDEBAR_DEFAULT_RADIUS_DARK, radiusRecoverDark);
    EXPECT_EQ(SIDEBAR_DEFAULT_SATURATION_DARK, saturationRecoverDark);
    EXPECT_EQ(SIDEBAR_DEFAULT_BRIGHTNESS_DARK, brightnessRecoverDark);
    EXPECT_EQ(SIDEBAR_DEFAULT_MASKCOLOR_DARK, colorRecoverDark.AsArgbInt());

    session->SetSidebarBlurMaximize(true);
    float radiusMaximizeDark = session->blurRadiusValue_->Get();
    float saturationMaximizeDark = session->blurSaturationValue_->Get();
    float brightnessMaximizeDark = session->blurBrightnessValue_->Get();
    Rosen::RSColor colorMaximizeDark = session->blurMaskColorValue_->Get();
    EXPECT_EQ(SIDEBAR_MAXIMIZE_RADIUS_DARK, radiusMaximizeDark);
    EXPECT_EQ(SIDEBAR_MAXIMIZE_SATURATION_DARK, saturationMaximizeDark);
    EXPECT_EQ(SIDEBAR_MAXIMIZE_BRIGHTNESS_DARK, brightnessMaximizeDark);
    EXPECT_EQ(SIDEBAR_MAXIMIZE_MASKCOLOR_DARK, colorMaximizeDark.AsArgbInt());
}

/**
 * @tc.name: SetSidebarBlurMaximize02
 * @tc.desc: SetSidebarBlurMaximize02
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, SetSidebarBlurMaximize02, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetSidebarBlurMaximize02";
    info.bundleName_ = "SetSidebarBlurMaximize02";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    session->SetScreenId(defaultScreenSession_->GetScreenId());

    struct RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    EXPECT_EQ(nullptr, session->GetSurfaceNode());
    session->SetSurfaceNode(surfaceNode);
    EXPECT_NE(nullptr, session->GetSurfaceNode());

    AbilityRuntime::Context::applicationContext_ = std::make_shared<AbilityRuntime::ApplicationContext>();
    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext = AbilityRuntime::Context::GetApplicationContext();
    EXPECT_NE(nullptr, appContext);

    appContext->contextImpl_ = std::make_shared<AbilityRuntime::ContextImpl>();
    appContext->contextImpl_->config_ = std::make_shared<AppExecFwk::Configuration>();
    std::shared_ptr<AppExecFwk::Configuration> appContextConfig = appContext->GetConfiguration();
    EXPECT_NE(nullptr, appContextConfig);

    appContextConfig->AddItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE,
                              AppExecFwk::ConfigurationInner::COLOR_MODE_LIGHT);

    session->AddSidebarBlur();
    session->SetSidebarBlurMaximize(false);
    float radiusRecoverLight = session->blurRadiusValue_->Get();
    float saturationRecoverLight = session->blurSaturationValue_->Get();
    float brightnessRecoverLight = session->blurBrightnessValue_->Get();
    Rosen::RSColor colorRecoverLight = session->blurMaskColorValue_->Get();
    EXPECT_EQ(SIDEBAR_DEFAULT_RADIUS_LIGHT, radiusRecoverLight);
    EXPECT_EQ(SIDEBAR_DEFAULT_SATURATION_LIGHT, saturationRecoverLight);
    EXPECT_EQ(SIDEBAR_DEFAULT_BRIGHTNESS_LIGHT, brightnessRecoverLight);
    EXPECT_EQ(SIDEBAR_DEFAULT_MASKCOLOR_LIGHT, colorRecoverLight.AsArgbInt());

    session->SetSidebarBlurMaximize(true);
    float radiusMaximizeLight = session->blurRadiusValue_->Get();
    float saturationMaximizeLight = session->blurSaturationValue_->Get();
    float brightnessMaximizeLight = session->blurBrightnessValue_->Get();
    Rosen::RSColor colorMaximizeLight = session->blurMaskColorValue_->Get();
    EXPECT_EQ(SIDEBAR_MAXIMIZE_RADIUS_LIGHT, radiusMaximizeLight);
    EXPECT_EQ(SIDEBAR_MAXIMIZE_SATURATION_LIGHT, saturationMaximizeLight);
    EXPECT_EQ(SIDEBAR_MAXIMIZE_BRIGHTNESS_LIGHT, brightnessMaximizeLight);
    EXPECT_EQ(SIDEBAR_MAXIMIZE_MASKCOLOR_LIGHT, colorMaximizeLight.AsArgbInt());
}

/**
 * @tc.name: SetWindowShadowsCallback
 * @tc.desc: SetWindowShadowsCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, SetWindowShadowsCallback, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetWindowShadowsCallback";
    info.bundleName_ = "SetWindowShadowsCallback";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    NotifySetWindowShadowsFunc func = [](ShadowsInfo shadowsInfo) {
        return;
    };
    sceneSession->SetWindowShadowsCallback(std::move(func));
    ASSERT_NE(nullptr, sceneSession->onSetWindowShadowsFunc_);
}

/**
 * @tc.name: OnSetWindowShadows
 * @tc.desc: OnSetWindowShadows
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionAnimationTest, OnSetWindowShadows, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "OnSetWindowShadows";
    info.bundleName_ = "OnSetWindowShadows";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);

    ShadowsInfo shadowsInfo = { 20.0, "#FF0000", 0.0, 0.0, true, true, true, true };
    EXPECT_EQ(WSError::WS_OK, session->SetWindowShadows(shadowsInfo));

    NotifySetWindowShadowsFunc func = [](ShadowsInfo shadowsInfo) {
        return;
    };
    session->onSetWindowShadowsFunc_ = func;
    EXPECT_EQ(WSError::WS_OK, session->SetWindowShadows(shadowsInfo));
}
} // namespace
} // namespace Rosen
} // namespace OHOS