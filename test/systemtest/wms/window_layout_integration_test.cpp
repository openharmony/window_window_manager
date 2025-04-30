/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

// gtest
#include <gtest/gtest.h>
#include "ability_context_impl.h"
#include "common_test_utils.h"
#include "mock_session.h"
#include "session/host/include/scene_session.h"
#include "window_adapter.h"
#include "window_scene_session_impl.h"
#include "window_test_utils.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Utils = WindowTestUtils;
class WindowLayoutTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_;
    static constexpr uint32_t WAIT_SERVERAL_FRAMES = 70000;
};

void WindowLayoutTest::SetUpTestCase() {}

void WindowLayoutTest::TearDownTestCase() {}

void WindowLayoutTest::SetUp() {}

void WindowLayoutTest::TearDown() {}

namespace {

/**
 * @tc.name: moveWindowTo01
 * @tc.desc: test moveWindowTo for ALN/PC with windowMode: 102, windowType: 2107
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, moveWindowTo01, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("moveWindowTo01");
    option->SetWindowType(WindowType::WINDOW_TYPE_TOAST);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    window->property_->SetPersistentId(10001);

    Rect rect;
    WMError ret;
    ret = window->Create(abilityContext_, nullptr);
    EXPECT_EQ(WMError::WM_OK, ret);

    ret = window->Show();
    EXPECT_EQ(WMError::WM_OK, ret);

    ret = window->MoveTo(-500, -500);
    usleep(WAIT_SERVERAL_FRAMES);
    EXPECT_EQ(WMError::WM_OK, ret);
    rect = window->property_->GetWindowRect();
    EXPECT_EQ(-500, rect.posX_);
    EXPECT_EQ(-500, rect.posY_);

    ret = window->MoveTo(0, 0);
    usleep(WAIT_SERVERAL_FRAMES);
    EXPECT_EQ(WMError::WM_OK, ret);
    rect = window->property_->GetWindowRect();
    EXPECT_EQ(0, rect.posX_);
    EXPECT_EQ(0, rect.posY_);

    ret = window->MoveTo(500, 500);
    usleep(WAIT_SERVERAL_FRAMES);
    EXPECT_EQ(WMError::WM_OK, ret);
    rect = window->property_->GetWindowRect();
    EXPECT_EQ(500, rect.posX_);
    EXPECT_EQ(500, rect.posY_);

    ret = window->MoveTo(20000, 20000);
    usleep(WAIT_SERVERAL_FRAMES);
    EXPECT_EQ(WMError::WM_OK, ret);
    rect = window->property_->GetWindowRect();
    EXPECT_EQ(20000, rect.posX_);
    EXPECT_EQ(20000, rect.posY_);
}

/**
 * @tc.name: moveWindowTo02
 * @tc.desc: test moveWindowTo for ALN with windowMode: 102, windowType: 1001
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, moveWindowTo02, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("moveWindowTo02");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "bundleName_moveWindowTo02",
                                "moduleName_moveWindowTo02",
                                "abilityName_moveWindowTo02" };
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);

    Rect rectOld;
    Rect rectNow;
    WMError ret;
    ret = window->Create(abilityContext_, sceneSession);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = window->Show();
    EXPECT_EQ(WMError::WM_OK, ret);
    window->property_->SetPersistentId(10002);

    rectOld = window->property_->GetWindowRect();
    ret = window->MoveTo(-500, -500);
    usleep(WAIT_SERVERAL_FRAMES);
    EXPECT_EQ(WMError::WM_OK, ret);
    rectNow = window->property_->GetWindowRect();
    EXPECT_EQ(rectOld.posX_, rectNow.posX_);
    EXPECT_EQ(rectOld.posY_, rectNow.posY_);

    rectOld = window->property_->GetWindowRect();
    ret = window->MoveTo(0, 0);
    usleep(WAIT_SERVERAL_FRAMES);
    EXPECT_EQ(WMError::WM_OK, ret);
    rectNow = window->property_->GetWindowRect();
    EXPECT_EQ(rectOld.posX_, rectNow.posX_);
    EXPECT_EQ(rectOld.posY_, rectNow.posY_);

    rectOld = window->property_->GetWindowRect();
    ret = window->MoveTo(500, 500);
    usleep(WAIT_SERVERAL_FRAMES);
    EXPECT_EQ(WMError::WM_OK, ret);
    rectNow = window->property_->GetWindowRect();
    EXPECT_EQ(rectOld.posX_, rectNow.posX_);
    EXPECT_EQ(rectOld.posY_, rectNow.posY_);
}

/**
 * @tc.name: moveWindowTo03
 * @tc.desc: test moveWindowTo for ALN with windowMode: 1, windowType: 1
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, moveWindowTo03, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("moveWindowTo03");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    window->property_->SetPersistentId(10003);

    SessionInfo sessionInfo = { "bundleName_moveWindowTo03",
                                "moduleName_moveWindowTo03",
                                "abilityName_moveWindowTo03" };
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);

    Rect rectOld;
    Rect rectNow;
    WMError ret;
    ret = window->Create(abilityContext_, sceneSession);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = window->Show();
    EXPECT_EQ(WMError::WM_OK, ret);

    rectOld = window->property_->GetWindowRect();
    ret = window->MoveTo(-500, -500);
    usleep(WAIT_SERVERAL_FRAMES);
    EXPECT_EQ(WMError::WM_OK, ret);
    rectNow = window->property_->GetWindowRect();
    EXPECT_EQ(rectOld.posX_, rectNow.posX_);
    EXPECT_EQ(rectOld.posY_, rectNow.posY_);

    rectOld = window->property_->GetWindowRect();
    ret = window->MoveTo(0, 0);
    usleep(WAIT_SERVERAL_FRAMES);
    EXPECT_EQ(WMError::WM_OK, ret);
    rectNow = window->property_->GetWindowRect();
    EXPECT_EQ(rectOld.posX_, rectNow.posX_);
    EXPECT_EQ(rectOld.posY_, rectNow.posY_);

    rectOld = window->property_->GetWindowRect();
    ret = window->MoveTo(500, 500);
    usleep(WAIT_SERVERAL_FRAMES);
    EXPECT_EQ(WMError::WM_OK, ret);
    rectNow = window->property_->GetWindowRect();
    EXPECT_EQ(rectOld.posX_, rectNow.posX_);
    EXPECT_EQ(rectOld.posY_, rectNow.posY_);

    rectOld = window->property_->GetWindowRect();
    ret = window->MoveTo(20000, 20000);
    usleep(WAIT_SERVERAL_FRAMES);
    EXPECT_EQ(WMError::WM_OK, ret);
    rectNow = window->property_->GetWindowRect();
    EXPECT_EQ(rectOld.posX_, rectNow.posX_);
    EXPECT_EQ(rectOld.posY_, rectNow.posY_);
}

/**
 * @tc.name: moveWindowTo04
 * @tc.desc: test moveWindowTo for ALN with windowMode: 100, windowType: 1
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, moveWindowTo04, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("moveWindowTo04");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    window->property_->SetPersistentId(10004);

    SessionInfo sessionInfo = { "bundleName_moveWindowTo04",
                                "moduleName_moveWindowTo04",
                                "abilityName_moveWindowTo04" };
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);

    Rect rectOld;
    Rect rectNow;
    WMError ret;
    ret = window->Create(abilityContext_, sceneSession);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = window->Show();
    EXPECT_EQ(WMError::WM_OK, ret);

    rectOld = window->property_->GetWindowRect();
    ret = window->MoveTo(-500, -500);
    usleep(WAIT_SERVERAL_FRAMES);
    EXPECT_EQ(WMError::WM_OK, ret);
    rectNow = window->property_->GetWindowRect();
    EXPECT_EQ(rectOld.posX_, rectNow.posX_);
    EXPECT_EQ(rectOld.posY_, rectNow.posY_);

    rectOld = window->property_->GetWindowRect();
    ret = window->MoveTo(0, 0);
    usleep(WAIT_SERVERAL_FRAMES);
    EXPECT_EQ(WMError::WM_OK, ret);
    rectNow = window->property_->GetWindowRect();
    EXPECT_EQ(rectOld.posX_, rectNow.posX_);
    EXPECT_EQ(rectOld.posY_, rectNow.posY_);

    rectOld = window->property_->GetWindowRect();
    ret = window->MoveTo(500, 500);
    usleep(WAIT_SERVERAL_FRAMES);
    EXPECT_EQ(WMError::WM_OK, ret);
    rectNow = window->property_->GetWindowRect();
    EXPECT_EQ(rectOld.posX_, rectNow.posX_);
    EXPECT_EQ(rectOld.posY_, rectNow.posY_);

    rectOld = window->property_->GetWindowRect();
    ret = window->MoveTo(20000, 20000);
    usleep(WAIT_SERVERAL_FRAMES);
    EXPECT_EQ(WMError::WM_OK, ret);
    rectNow = window->property_->GetWindowRect();
    EXPECT_EQ(rectOld.posX_, rectNow.posX_);
    EXPECT_EQ(rectOld.posY_, rectNow.posY_);
}

/**
 * @tc.name: resize01
 * @tc.desc: test resize for ALN/PC with windowMode: 102, windowType: 2107
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, resize01, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("resize01");
    option->SetWindowType(WindowType::WINDOW_TYPE_TOAST);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    Rect rect;
    WMError ret;
    ret = window->Create(abilityContext_, nullptr);
    EXPECT_EQ(WMError::WM_OK, ret);
    window->property_->SetPersistentId(10008);
    ret = window->Show();
    EXPECT_EQ(WMError::WM_OK, ret);

    WindowLimits windowLimits;
    ret = window->GetWindowLimits(windowLimits);
    EXPECT_EQ(WMError::WM_OK, ret);

    ret = window->Resize(-500, -500);
    EXPECT_EQ(WMError::WM_OK, ret);
    usleep(WAIT_SERVERAL_FRAMES);
    rect = window->property_->GetWindowRect();
    EXPECT_EQ(windowLimits.maxWidth_, rect.width_);
    EXPECT_EQ(windowLimits.maxHeight_, rect.height_);

    ret = window->Resize(500, 500);
    EXPECT_EQ(WMError::WM_OK, ret);
    usleep(WAIT_SERVERAL_FRAMES);
    rect = window->property_->GetWindowRect();
    EXPECT_EQ(500, rect.width_);
    EXPECT_EQ(500, rect.height_);

    ret = window->Resize(20000, 20000);
    EXPECT_EQ(WMError::WM_OK, ret);
    usleep(WAIT_SERVERAL_FRAMES);
    rect = window->property_->GetWindowRect();
    EXPECT_EQ(windowLimits.maxWidth_, rect.width_);
    EXPECT_EQ(windowLimits.maxHeight_, rect.height_);

    ret = window->Resize(0, 0);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret); // check parameter first
}

/**
 * @tc.name: resize02
 * @tc.desc: test resize for ALN with windowMode: 1, windowType: 1
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, resize02, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("resize02");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "bundleName_resize02", "moduleName_resize02", "abilityName_resize02" };
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);

    WMError ret;
    ret = window->Create(abilityContext_, sceneSession);
    EXPECT_EQ(WMError::WM_OK, ret);
    window->property_->SetPersistentId(10009);
    ret = window->Show();
    EXPECT_EQ(WMError::WM_OK, ret);

    ret = window->Resize(500, 500);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);

    ret = window->Resize(20000, 20000);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);

    ret = window->Resize(0, 0);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret); // check parameter first
}

/**
 * @tc.name: resize03
 * @tc.desc: test resize for PC with windowMode: 1, windowType: 1
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, resize03, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("resize03");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "bundleName_resize03", "moduleName_resize03", "abilityName_resize03" };
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);

    WMError ret;
    ret = window->Create(abilityContext_, sceneSession);
    EXPECT_EQ(WMError::WM_OK, ret);
    window->property_->SetPersistentId(100010);
    ret = window->Show();
    EXPECT_EQ(WMError::WM_OK, ret);

    ret = window->Resize(500, 500);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);

    ret = window->Resize(20000, 20000);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);

    ret = window->Resize(0, 0);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret); // check parameter first
}

/**
 * @tc.name: resize04
 * @tc.desc: test resize for ALN/PC with windowMode: 100, windowType: 1
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, resize04, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("resize04");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "bundleName_resize04", "moduleName_resize04", "abilityName_resize04" };
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);

    WMError ret;
    ret = window->Create(abilityContext_, sceneSession);
    EXPECT_EQ(WMError::WM_OK, ret);
    window->property_->SetPersistentId(100011);
    ret = window->Show();
    EXPECT_EQ(WMError::WM_OK, ret);

    ret = window->Resize(500, 500);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);

    ret = window->Resize(20000, 20000);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);

    ret = window->Resize(0, 0);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret); // check parameter first
}

/**
 * @tc.name: resize05
 * @tc.desc: test resize for ALN/PC with windowMode: 102, windowType: 2107
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, resize05, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("resize05");
    option->SetWindowType(WindowType::WINDOW_TYPE_TOAST);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    Rect rect;
    WMError ret;
    ret = window->Create(abilityContext_, nullptr);
    EXPECT_EQ(WMError::WM_OK, ret);
    window->property_->SetPersistentId(10012);
    ret = window->Show();
    EXPECT_EQ(WMError::WM_OK, ret);

    WindowLimits windowLimits;
    ret = window->GetWindowLimits(windowLimits);

    ret = window->Resize(windowLimits.maxWidth_ - 100, windowLimits.maxHeight_ - 100);
    EXPECT_EQ(WMError::WM_OK, ret);
    usleep(WAIT_SERVERAL_FRAMES);
    rect = window->property_->GetWindowRect();
    EXPECT_EQ(windowLimits.maxWidth_ - 100, rect.width_);
    EXPECT_EQ(windowLimits.maxHeight_ - 100, rect.height_);

    ret = window->Resize(windowLimits.maxWidth_ + 100, windowLimits.maxHeight_ + 100);
    EXPECT_EQ(WMError::WM_OK, ret);
    usleep(WAIT_SERVERAL_FRAMES);
    rect = window->property_->GetWindowRect();
    EXPECT_EQ(windowLimits.maxWidth_, rect.width_);
    EXPECT_EQ(windowLimits.maxHeight_, rect.height_);
}

/**
 * @tc.name: resize06
 * @tc.desc: test resize for ALN with windowMode: 1, windowType: 1
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, resize06, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("resize06");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "bundleName_resize06", "moduleName_resize06", "abilityName_resize06" };
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);

    WMError ret;
    ret = window->Create(abilityContext_, sceneSession);
    EXPECT_EQ(WMError::WM_OK, ret);
    window->property_->SetPersistentId(100013);
    ret = window->Show();
    EXPECT_EQ(WMError::WM_OK, ret);

    WindowLimits windowLimits;
    ret = window->GetWindowLimits(windowLimits);

    ret = window->Resize(windowLimits.maxWidth_ - 100, windowLimits.maxHeight_ - 100);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);

    ret = window->Resize(windowLimits.maxWidth_ + 100, windowLimits.maxHeight_ + 100);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);
}

/**
 * @tc.name: resize07
 * @tc.desc: test resize for PC with windowMode: 1, windowType: 1
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, resize07, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("resize07");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "bundleName_resize07", "moduleName_resize07", "abilityName_resize07" };
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);

    WMError ret;
    ret = window->Create(abilityContext_, sceneSession);
    EXPECT_EQ(WMError::WM_OK, ret);
    window->property_->SetPersistentId(100014);
    ret = window->Show();
    EXPECT_EQ(WMError::WM_OK, ret);

    WindowLimits windowLimits;
    ret = window->GetWindowLimits(windowLimits);

    ret = window->Resize(windowLimits.maxWidth_ - 100, windowLimits.maxHeight_ - 100);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);

    ret = window->Resize(windowLimits.maxWidth_ + 100, windowLimits.maxHeight_ + 100);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);
}

/**
 * @tc.name: resize08
 * @tc.desc: test resize for ALN/PC with windowMode: 100, windowType: 1
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, resize08, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("resize08");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    SessionInfo sessionInfo = { "bundleName_resize08", "moduleName_resize08", "abilityName_resize08" };
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);

    WMError ret;
    ret = window->Create(abilityContext_, sceneSession);
    EXPECT_EQ(WMError::WM_OK, ret);
    window->property_->SetPersistentId(100015);
    ret = window->Show();
    EXPECT_EQ(WMError::WM_OK, ret);

    WindowLimits windowLimits;
    ret = window->GetWindowLimits(windowLimits);

    ret = window->Resize(windowLimits.maxWidth_ - 100, windowLimits.maxHeight_ - 100);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);
    ret = window->Resize(windowLimits.maxWidth_ + 100, windowLimits.maxHeight_ + 100);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);
}

/**
 * @tc.name: SetWindowLimitsDataRoute
 * @tc.desc: test data route for SetWindowLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, SetWindowLimitsDataRoute, TestSize.Level1)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "### WindowLayoutTest::SetWindowLimitsDataRoute begin ###");
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetWindowLimitsDataRoute");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->property_->SetDisplayId(0);

    SessionInfo sessionInfo = { "CeateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);

    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;

    WindowLimits windowLimits1 = { 4000, 4000, 2000, 2000, 0.0f, 0.0f };
    WMError ret = windowSceneSessionImpl->SetWindowLimits(windowLimits1, false);
    EXPECT_EQ(WMError::WM_OK, ret);
    auto windowProperty = windowSceneSessionImpl->GetProperty();
    ASSERT_NE(nullptr, windowProperty);

    WindowLimits windowLimits2 = windowProperty->GetWindowLimits();
    EXPECT_EQ(windowLimits1.maxWidth_, windowLimits2.maxWidth_);
    EXPECT_EQ(windowLimits1.maxHeight_, windowLimits2.maxHeight_);
    EXPECT_EQ(windowLimits1.minWidth_, windowLimits2.minWidth_);
    EXPECT_EQ(windowLimits1.minHeight_, windowLimits2.minHeight_);
    TLOGI(WmsLogTag::WMS_LAYOUT, "### WindowLayoutTest::SetWindowLimitsDataRoute end ###");
}

/**
 * @tc.name: SetAspectRatioDataRoute
 * @tc.desc: test data route for SetAspectRatioDataRoute
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, SetAspectRatioDataRoute, TestSize.Level1)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "### WindowLayoutTest::SetAspectRatioDataRoute begin ###");
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetAspectRatioDataRoute");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->property_->SetDisplayId(0);

    SessionInfo sessionInfo = { "CeateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);

    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;

    WindowLimits windowLimits1 = { 4000, 4000, 2000, 2000, 0.0f, 0.0f };
    WMError ret = windowSceneSessionImpl->SetWindowLimits(windowLimits1, false);
    EXPECT_EQ(WMError::WM_OK, ret);
    const float ratio = 1.5;

    ret = windowSceneSessionImpl->SetAspectRatio(ratio);
    EXPECT_EQ(WMError::WM_OK, ret);
    TLOGI(WmsLogTag::WMS_LAYOUT, "### WindowLayoutTest::SetAspectRatioDataRoute end ###");
}

/**
 * @tc.name: moveToDataRoute
 * @tc.desc: test data route for moveTo
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, moveToDataRoute, TestSize.Level1)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "### WindowLayoutTest::moveToDataRoute begin ###");
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("moveToDataRoute");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->property_->SetDisplayId(0);

    SessionInfo sessionInfo = { "CeateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);

    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;

    WMError ret = windowSceneSessionImpl->MoveTo(500, 500);
    usleep(WAIT_SERVERAL_FRAMES);
    EXPECT_EQ(WMError::WM_OK, ret);

    Rect rect = windowSceneSessionImpl->property_->GetRequestRect();
    EXPECT_EQ(500, rect.posX_);
    EXPECT_EQ(500, rect.posY_);
    TLOGI(WmsLogTag::WMS_LAYOUT, "### WindowLayoutTest::moveToDataRoute end ###");
}

/**
 * @tc.name: ResizeDataRoute
 * @tc.desc: test data route for Resize
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, ResizeDataRoute, TestSize.Level1)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "### WindowLayoutTest::ResizeDataRoute begin ###");
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("ResizeDataRoute");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->property_->SetDisplayId(0);

    SessionInfo sessionInfo = { "CeateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    session->isActive_ = true;
    session->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    session->SetSessionState(SessionState::STATE_FOREGROUND);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;

    WMError ret = windowSceneSessionImpl->Resize(500, 500);
    EXPECT_EQ(WMError::WM_OK, ret);

    Rect rect = windowSceneSessionImpl->property_->GetRequestRect();
    EXPECT_EQ(500, rect.width_);
    EXPECT_EQ(500, rect.height_);

    WSRect wsRect = { rect.posX_, rect.posY_, rect.width_, rect.height_ };
    WSError ret2 = session->UpdateSessionRect(wsRect, SizeChangeReason::RESIZE, false);
    EXPECT_EQ(WSError::WS_OK, ret2);

    usleep(WAIT_SERVERAL_FRAMES);
    Rect rect2 = session->property_->GetRequestRect();
    EXPECT_EQ(500, rect2.width_);
    EXPECT_EQ(500, rect2.height_);
    TLOGI(WmsLogTag::WMS_LAYOUT, "### WindowLayoutTest::ResizeDataRoute end ###");
}

/**
 * @tc.name: AdjustRectByAspectRatio
 * @tc.desc: test AdjustRectByAspectRatio
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, AdjustRectByAspectRatio, TestSize.Level1)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "### WindowLayoutTest::AdjustRectByAspectRatio begin ###");
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("AdjustRectByAspectRatio");
    option->SetWindowType(WindowType::APP_WINDOW_BASE);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->property_->SetDisplayId(0);

    SessionInfo sessionInfo = { "CeateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    session->isActive_ = true;
    session->property_->SetWindowType(WindowType::APP_WINDOW_BASE);
    session->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    session->SetSessionState(SessionState::STATE_FOREGROUND);

    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;

    WindowLimits windowLimits1 = { 3000, 3000, 1200, 1200, 0.0f, 0.0f };
    WMError wmRet1 = windowSceneSessionImpl->SetWindowLimits(windowLimits1, false);
    EXPECT_EQ(WMError::WM_OK, wmRet1);

    WMError wmRet2 = windowSceneSessionImpl->Resize(1600, 1600);
    EXPECT_EQ(WMError::WM_OK, wmRet2);

    float ratio = 0.8;
    WMError wmRet3 = windowSceneSessionImpl->SetAspectRatio(ratio);
    EXPECT_EQ(WMError::WM_OK, wmRet3);

    // 服务端执行相同的逻辑
    auto property = windowSceneSessionImpl->property_;
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_WINDOW_LIMITS;
    WMError wsRet1 = session->UpdateSessionPropertyByAction(property, action);
    EXPECT_EQ(WMError::WM_OK, wsRet1);

    Rect rect = windowSceneSessionImpl->property_->GetRequestRect();
    WSRect wsRect = { rect.posX_, rect.posY_, rect.width_, rect.height_ };
    // 异步接口加一个延迟
    WSError wsRet2 = session->UpdateSessionRect(wsRect, SizeChangeReason::RESIZE, false);
    EXPECT_EQ(WSError::WS_OK, wsRet2);
    usleep(WAIT_SERVERAL_FRAMES);
    WSError wsRet3 = session->UpdateRect(wsRect, SizeChangeReason::RESIZE, "AdjustRectByAspectRatio", nullptr);
    EXPECT_EQ(WSError::WS_OK, wsRet3);
    usleep(WAIT_SERVERAL_FRAMES);
    WSError wsRet4 = session->SetAspectRatio(ratio);
    EXPECT_EQ(WSError::WS_OK, wsRet4);
    TLOGI(WmsLogTag::WMS_LAYOUT, "### WindowLayoutTest::AdjustRectByAspectRatio end ###");
}

} // namespace
} // namespace Rosen
} // namespace OHOS
