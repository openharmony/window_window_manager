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
#include "common_test_utils.h"
#include "display_info.h"
#include "mock_session.h"
#include "mock_uicontent.h"
#include "mock_window.h"
#include "mock_window_adapter.h"
#include "pointer_event.h"
#include "singleton_mocker.h"
#include "wm_common_inner.h"
#include "window_scene_session_impl.h"
#include "window_session_impl.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowSceneSessionImplTest5 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WindowSceneSessionImplTest5::SetUpTestCase() {}

void WindowSceneSessionImplTest5::TearDownTestCase() {}

void WindowSceneSessionImplTest5::SetUp() {}

void WindowSceneSessionImplTest5::TearDown() {}

namespace {
/**
 * @tc.name: IgnoreClickEvent
 * @tc.desc: IgnoreClickEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, IgnoreClickEvent, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("IgnoreClickEvent");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, window);

    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    pointerEvent->SetPointerAction(1);
    window->IgnoreClickEvent(pointerEvent);

    pointerEvent->SetPointerAction(4);
    window->IgnoreClickEvent(pointerEvent);

    window->isOverTouchSlop_ = true;
    MMI::PointerEvent::PointerItem pointerItem;
    pointerEvent->pointers_.clear();
    pointerEvent->pointers_.push_back(pointerItem);
    window->IgnoreClickEvent(pointerEvent);

    window->isOverTouchSlop_ = false;
    ASSERT_EQ(pointerEvent->GetPointerAction(), MMI::PointerEvent::POINTER_ACTION_CANCEL);
    window->IgnoreClickEvent(pointerEvent);
}

/**
 * @tc.name: HandleUpForCompatibleMode
 * @tc.desc: HandleUpForCompatibleMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, HandleUpForCompatibleMode, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("HandleUpForCompatibleMode");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, window);

    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    MMI::PointerEvent::PointerItem pointerItem;
    window->HandleUpForCompatibleMode(pointerEvent, pointerItem);

    window->isDown_ = true;
    window->eventMapTriggerByDisplay_.clear();
    window->eventMapTriggerByDisplay_.insert(std::pair<int32_t, std::vector<bool>>(-2, { true }));
    window->HandleUpForCompatibleMode(pointerEvent, pointerItem);
}

/**
 * @tc.name: HandleMoveForCompatibleMode
 * @tc.desc: HandleMoveForCompatibleMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, HandleMoveForCompatibleMode, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("HandleMoveForCompatibleMode");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, window);

    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    MMI::PointerEvent::PointerItem pointerItem;
    window->HandleMoveForCompatibleMode(pointerEvent, pointerItem);

    window->isDown_ = true;
    window->eventMapTriggerByDisplay_.clear();
    window->eventMapTriggerByDisplay_.insert(std::pair<int32_t, std::vector<bool>>(-2, { true }));
    window->HandleMoveForCompatibleMode(pointerEvent, pointerItem);
}

/**
 * @tc.name: HandleDownForCompatibleMode
 * @tc.desc: HandleDownForCompatibleMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, HandleDownForCompatibleMode, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("HandleDownForCompatibleMode");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, window);

    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    ASSERT_NE(pointerEvent, nullptr);
    MMI::PointerEvent::PointerItem pointerItem;
    window->HandleDownForCompatibleMode(pointerEvent, pointerItem);

    pointerEvent->pointers_.clear();
    window->HandleDownForCompatibleMode(pointerEvent, pointerItem);

    pointerEvent->pointers_.push_back(pointerItem);
    sptr<DisplayInfo> displayInfo = nullptr;
    auto ret = window->GetVirtualPixelRatio(displayInfo);
    ASSERT_EQ(ret, 1.0f);
}

/**
 * @tc.name: NotifyCompatibleModeEnableInPad
 * @tc.desc: NotifyCompatibleModeEnableInPad
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, NotifyCompatibleModeEnableInPad, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("HandleDownForCompatibleMode");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, window);

    window->hostSession_ = nullptr;
    window->property_->persistentId_ = INVALID_SESSION_ID;
    window->state_ = WindowState::STATE_DESTROYED;

    auto ret = window->NotifyCompatibleModeEnableInPad(true);
    ASSERT_EQ(ret, WSError::WS_ERROR_INVALID_WINDOW);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    window->property_->persistentId_ = ROTATE_ANIMATION_DURATION;
    window->state_ = WindowState::STATE_CREATED;
    ret = window->NotifyCompatibleModeEnableInPad(true);
    ASSERT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: Maximize
 * @tc.desc: Maximize
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, Maximize, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("Maximize");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, window);

    window->hostSession_ = nullptr;
    MaximizePresentation presentation = MaximizePresentation::ENTER_IMMERSIVE;
    auto ret = window->Maximize(presentation);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_WINDOW);
}

/**
 * @tc.name: Maximize01
 * @tc.desc: Maximize
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, Maximize01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("Maximize");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, window);

    MaximizePresentation presentation = MaximizePresentation::ENTER_IMMERSIVE;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    window->property_->persistentId_ = ROTATE_ANIMATION_DURATION;
    window->state_ = WindowState::STATE_CREATED;

    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    auto ret = window->Maximize(presentation);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_CALLING);

    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->property_->SetWindowModeSupportType(0);
    ret = window->Maximize(presentation);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_WINDOW);

    window->property_->SetWindowModeSupportType(1);
    SystemSessionConfig systemSessionConfig;
    systemSessionConfig.windowUIType_ = WindowUIType::INVALID_WINDOW;
    ret = window->Maximize(presentation);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: Maximize02
 * @tc.desc: Maximize
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, Maximize02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("Maximize");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, window);

    MaximizePresentation presentation = MaximizePresentation::ENTER_IMMERSIVE;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    window->property_->persistentId_ = ROTATE_ANIMATION_DURATION;
    window->state_ = WindowState::STATE_CREATED;
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->property_->SetWindowModeSupportType(1);
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;

    presentation = MaximizePresentation::ENTER_IMMERSIVE;
    auto ret = window->Maximize(presentation);
    ASSERT_EQ(ret, WMError::WM_OK);

    ASSERT_EQ(window->enableImmersiveMode_, true);

    presentation = MaximizePresentation::EXIT_IMMERSIVE;
    window->Maximize(presentation);
    ASSERT_EQ(window->enableImmersiveMode_, false);

    presentation = MaximizePresentation::ENTER_IMMERSIVE_DISABLE_TITLE_AND_DOCK_HOVER;
    window->Maximize(presentation);
    ASSERT_EQ(window->enableImmersiveMode_, true);

    presentation = MaximizePresentation::FOLLOW_APP_IMMERSIVE_SETTING;
    window->Maximize(presentation);
}

/**
 * @tc.name: MoveWindowToGlobal
 * @tc.desc: MoveWindowToGlobal
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, MoveWindowToGlobal, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("MoveWindowToGlobal");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, window);

    window->hostSession_ = nullptr;
    MoveConfiguration presentation;
    auto ret = window->MoveWindowToGlobal(1, 1, presentation);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_WINDOW);
}

/**
 * @tc.name: MoveWindowToGlobal01
 * @tc.desc: MoveWindowToGlobal
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, MoveWindowToGlobal01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("Maximize");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, window);

    MoveConfiguration presentation;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    window->property_->persistentId_ = ROTATE_ANIMATION_DURATION;
    window->state_ = WindowState::STATE_CREATED;

    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    auto ret = window->MoveWindowToGlobal(1, 1, presentation);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_OP_IN_CUR_STATUS);

    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    ret = window->MoveWindowToGlobal(1, 1, presentation);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_OPERATION);

    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->MoveWindowToGlobal(1, 1, presentation);

    window->state_ = WindowState::STATE_SHOWN;
    ret = window->MoveWindowToGlobal(1, 1, presentation);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: SetCustomDensity01
 * @tc.desc: SetCustomDensity
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, SetCustomDensity01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    float density = 0.4f;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetCustomDensity(density));

    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->property_->SetWindowName("SetCustomDensity01");
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->state_ = WindowState::STATE_CREATED;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetCustomDensity(density));

    density = 1.5f;
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_CALLING, window->SetCustomDensity(density));

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_EQ(WMError::WM_OK, window->SetCustomDensity(density));
    ASSERT_EQ(density, window->customDensity_);
    ASSERT_EQ(WMError::WM_OK, window->SetCustomDensity(density));
}

/**
 * @tc.name: IsDefaultDensityEnabled01
 * @tc.desc: IsDefaultDensityEnabled
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, IsDefaultDensityEnabled01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->SetDefaultDensityEnabled(true);
    EXPECT_EQ(true, window->GetDefaultDensityEnabled());
    window->SetDefaultDensityEnabled(false);
    EXPECT_EQ(false, window->GetDefaultDensityEnabled());
}

/**
 * @tc.name: IsDefaultDensityEnabled02
 * @tc.desc: IsDefaultDensityEnabled
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, IsDefaultDensityEnabled02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    EXPECT_EQ(false, window->GetDefaultDensityEnabled());
}


/**
 * @tc.name: GetCustomDensity01
 * @tc.desc: GetCustomDensity
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, GetCustomDensity01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    window->property_->SetWindowName("SetCustomDensity01");
    window->state_ = WindowState::STATE_CREATED;
    float density = 1.5f;
    window->SetCustomDensity(density);
    EXPECT_EQ(density, window->GetCustomDensity());
}

/**
 * @tc.name: GetMainWindowCustomDensity02
 * @tc.desc: GetMainWindowCustomDensity
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, GetMainWindowCustomDensity01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    window->property_->SetWindowName("SetCustomDensity01");
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->state_ = WindowState::STATE_CREATED;
    float density = 1.5f;
    window->SetCustomDensity(density);
    EXPECT_EQ(density, window->GetMainWindowCustomDensity());
}

/**
 * @tc.name: GetWindowDensityInfo01
 * @tc.desc: GetWindowDensityInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, GetWindowDensityInfo01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    WindowDensityInfo densityInfo;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->GetWindowDensityInfo(densityInfo));

    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->property_->SetWindowName("GetWindowDensityInfo01");
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->state_ = WindowState::STATE_CREATED;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->GetWindowDensityInfo(densityInfo));
}

/**
 * @tc.name: SwitchFreeMultiWindow01
 * @tc.desc: SwitchFreeMultiWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, SwitchFreeMultiWindow01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, window);
    auto ref = window->SwitchFreeMultiWindow(false);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_WINDOW, ref);

    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    window->property_->SetWindowName("SwitchFreeMultiWindow01");
    window->windowSystemConfig_.freeMultiWindowEnable_ = false;
    ref = window->SwitchFreeMultiWindow(false);
    ASSERT_EQ(WSError::WS_ERROR_REPEAT_OPERATION, ref);
    window->windowSystemConfig_.freeMultiWindowEnable_ = true;
    ref = window->SwitchFreeMultiWindow(true);
    ASSERT_EQ(WSError::WS_ERROR_REPEAT_OPERATION, ref);
    
    window->windowSystemConfig_.freeMultiWindowEnable_ = false;
    WindowSceneSessionImpl::windowSessionMap_.insert(std::make_pair(window->GetWindowName(),
        std::pair<uint64_t, sptr<WindowSessionImpl>>(window->GetWindowId(), window)));
    ASSERT_EQ(WSError::WS_OK, window->SwitchFreeMultiWindow(true));
    ASSERT_EQ(true, window->windowSystemConfig_.freeMultiWindowEnable_);
    ASSERT_EQ(WSError::WS_OK, window->SwitchFreeMultiWindow(false));
    ASSERT_EQ(false, window->windowSystemConfig_.freeMultiWindowEnable_);
    WindowSceneSessionImpl::windowSessionMap_.erase(window->GetWindowName());
}

/**
 * @tc.name: SwitchFreeMultiWindow02
 * @tc.desc: SwitchFreeMultiWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, SwitchFreeMultiWindow02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);

    sptr<WindowSceneSessionImpl> mainWindow = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    mainWindow->property_->SetPersistentId(1);
    mainWindow->hostSession_ = session;
    mainWindow->property_->SetWindowName("SwitchFreeMultiWindow02_mainWindow");
    mainWindow->windowSystemConfig_.freeMultiWindowEnable_ = false;
    mainWindow->windowSystemConfig_.freeMultiWindowSupport_ = true;
    mainWindow->windowSystemConfig_.windowUIType_ = WindowUIType::PAD_WINDOW;
    mainWindow->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    WindowSceneSessionImpl::windowSessionMap_.insert(std::make_pair(mainWindow->GetWindowName(),
        std::pair<uint64_t, sptr<WindowSessionImpl>>(mainWindow->GetWindowId(), mainWindow)));

    sptr<WindowSceneSessionImpl> floatWindow = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    floatWindow->property_->SetPersistentId(2);
    floatWindow->hostSession_ = session;
    floatWindow->property_->SetWindowName("SwitchFreeMultiWindow02_floatWindow");
    floatWindow->windowSystemConfig_.freeMultiWindowEnable_ = false;
    floatWindow->windowSystemConfig_.freeMultiWindowSupport_ = true;
    floatWindow->windowSystemConfig_.windowUIType_ = WindowUIType::PAD_WINDOW;
    floatWindow->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    WindowSceneSessionImpl::windowSessionMap_.insert(std::make_pair(floatWindow->GetWindowName(),
        std::pair<uint64_t, sptr<WindowSessionImpl>>(floatWindow->GetWindowId(), floatWindow)));

    sptr<WindowSceneSessionImpl> subWindow = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    subWindow->property_->SetPersistentId(3);
    subWindow->hostSession_ = session;
    subWindow->property_->SetWindowName("SwitchFreeMultiWindow03_subWindow");
    subWindow->windowSystemConfig_.freeMultiWindowEnable_ = false;
    subWindow->windowSystemConfig_.freeMultiWindowSupport_ = true;
    subWindow->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    WindowSceneSessionImpl::windowSessionMap_.insert(std::make_pair(subWindow->GetWindowName(),
        std::pair<uint64_t, sptr<WindowSessionImpl>>(subWindow->GetWindowId(), subWindow)));

    EXPECT_EQ(false, mainWindow->IsPcOrPadFreeMultiWindowMode());
    EXPECT_EQ(false, floatWindow->IsPcOrPadFreeMultiWindowMode());
    EXPECT_EQ(false, subWindow->IsPcOrPadFreeMultiWindowMode());
    EXPECT_EQ(WSError::WS_OK, mainWindow->SwitchFreeMultiWindow(true));
    EXPECT_EQ(true, mainWindow->IsPcOrPadFreeMultiWindowMode());
    EXPECT_EQ(true, floatWindow->IsPcOrPadFreeMultiWindowMode());
    EXPECT_EQ(true, subWindow->IsPcOrPadFreeMultiWindowMode());

    EXPECT_EQ(WMError::WM_OK, mainWindow->Destroy(true));
    EXPECT_EQ(WMError::WM_OK, floatWindow->Destroy(true));
    EXPECT_EQ(WMError::WM_OK, subWindow->Destroy(true));
}

/**
 * @tc.name: ShowKeyboard01
 * @tc.desc: SwitchFreeMultiWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, ShowKeyboard01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);

    sptr<WindowSceneSessionImpl> keyboardWindow = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    keyboardWindow->property_->SetPersistentId(1000);
    keyboardWindow->hostSession_ = session;
    keyboardWindow->property_->SetWindowName("SwitchFreeMultiWindow02_mainWindow");
    keyboardWindow->property_->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    keyboardWindow->state_ = WindowState::STATE_DESTROYED;

    // normal value
    ASSERT_EQ(keyboardWindow->ShowKeyboard(KeyboardViewMode::DARK_IMMERSIVE_MODE), WMError::WM_ERROR_INVALID_WINDOW);

    // exception value
    ASSERT_EQ(keyboardWindow->ShowKeyboard(KeyboardViewMode::VIEW_MODE_END), WMError::WM_ERROR_INVALID_WINDOW);
    ASSERT_EQ(keyboardWindow->property_->GetKeyboardViewMode(), KeyboardViewMode::NON_IMMERSIVE_MODE);
}

/**
 * @tc.name: ChangeKeyboardViewMode01
 * @tc.desc: SwitchFreeMultiWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, ChangeKeyboardViewMode01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);

    sptr<WindowSceneSessionImpl> keyboardWindow = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    keyboardWindow->property_->SetPersistentId(1000);
    keyboardWindow->hostSession_ = session;
    keyboardWindow->property_->SetWindowName("SwitchFreeMultiWindow02_mainWindow");
    keyboardWindow->property_->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    keyboardWindow->property_->SetKeyboardViewMode(KeyboardViewMode::NON_IMMERSIVE_MODE);

    auto result = WMError::WM_OK;
    // exception mode value
    result = keyboardWindow->ChangeKeyboardViewMode(KeyboardViewMode::VIEW_MODE_END);
    ASSERT_EQ(result, WMError::WM_ERROR_INVALID_PARAM);

    // same mode
    result = keyboardWindow->ChangeKeyboardViewMode(KeyboardViewMode::NON_IMMERSIVE_MODE);
    ASSERT_EQ(result, WMError::WM_DO_NOTHING);

    // invalid window state
    keyboardWindow->state_ = WindowState::STATE_DESTROYED;
    result = keyboardWindow->ChangeKeyboardViewMode(KeyboardViewMode::LIGHT_IMMERSIVE_MODE);
    ASSERT_EQ(result, WMError::WM_ERROR_INVALID_WINDOW);

    // window state not shown
    keyboardWindow->state_ = WindowState::STATE_HIDDEN;
    result = keyboardWindow->ChangeKeyboardViewMode(KeyboardViewMode::LIGHT_IMMERSIVE_MODE);
    ASSERT_EQ(result, WMError::WM_ERROR_INVALID_WINDOW);

    keyboardWindow->state_ = WindowState::STATE_SHOWN;
    result = keyboardWindow->ChangeKeyboardViewMode(KeyboardViewMode::DARK_IMMERSIVE_MODE);
    ASSERT_EQ(result, WMError::WM_OK);
    auto currentMode = keyboardWindow->property_->GetKeyboardViewMode();
    ASSERT_EQ(currentMode, KeyboardViewMode::DARK_IMMERSIVE_MODE);
}

/**
 * @tc.name: StartMoveWindowWithCoordinate_01
 * @tc.desc: StartMoveWindowWithCoordinate
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, StartMoveWindowWithCoordinate_01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ASSERT_EQ(window->StartMoveWindowWithCoordinate(100, 50), WmErrorCode::WM_ERROR_DEVICE_NOT_SUPPORT);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ASSERT_NE(window->StartMoveWindowWithCoordinate(100, 50), WmErrorCode::WM_ERROR_DEVICE_NOT_SUPPORT);

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PAD_WINDOW;
    window->windowSystemConfig_.freeMultiWindowEnable_ = true;
    window->windowSystemConfig_.freeMultiWindowSupport_ = true;
    ASSERT_NE(window->StartMoveWindowWithCoordinate(100, 50), WmErrorCode::WM_ERROR_DEVICE_NOT_SUPPORT);
}

/**
 * @tc.name: StartMoveWindowWithCoordinate_02
 * @tc.desc: StartMoveWindowWithCoordinate
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, StartMoveWindowWithCoordinate_02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(1);
    Rect windowRect = { 200, 200, 1000, 1000 };
    window->property_->SetWindowRect(windowRect);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;

    ASSERT_EQ(window->StartMoveWindowWithCoordinate(-1, 50), WmErrorCode::WM_ERROR_INVALID_PARAM);
    ASSERT_EQ(window->StartMoveWindowWithCoordinate(100, -1), WmErrorCode::WM_ERROR_INVALID_PARAM);
    ASSERT_EQ(window->StartMoveWindowWithCoordinate(1500, 50), WmErrorCode::WM_ERROR_INVALID_PARAM);
    ASSERT_EQ(window->StartMoveWindowWithCoordinate(100, 1500), WmErrorCode::WM_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: UpdateSystemBarProperties
 * @tc.desc: UpdateSystemBarProperties test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, UpdateSystemBarProperties, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("UpdateSystemBarProperties");
    option->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    option->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    std::unordered_map<WindowType, SystemBarProperty> systemBarProperties;
    std::unordered_map<WindowType, SystemBarPropertyFlag> systemBarPropertyFlags;
    ASSERT_EQ(WMError::WM_OK, window->UpdateSystemBarProperties(systemBarProperties, systemBarPropertyFlags));
}

/**
 * @tc.name: NotifyAfterDidForeground
 * @tc.desc: NotifyAfterDidForeground
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, NotifyAfterDidForeground, Function | SmallTest | Level2)
{
    sptr<MockWindowLifeCycleListener> mockListener = sptr<MockWindowLifeCycleListener>::MakeSptr();
    sptr<IWindowLifeCycle> listener = static_cast<sptr<IWindowLifeCycle>>(mockListener);

    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("Test");
    option->SetDisplayId(0);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    window->RegisterLifeCycleListener(listener);

    EXPECT_CALL(*mockListener, AfterDidForeground()).Times(1);
    ASSERT_EQ(WMError::WM_OK, window->Show(static_cast<uint32_t>(WindowStateChangeReason::ABILITY_CALL), false));
}

/**
 * @tc.name: NotifyAfterDidBackground
 * @tc.desc: NotifyAfterDidBackground
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, NotifyAfterDidBackground, Function | SmallTest | Level2)
{
    sptr<MockWindowLifeCycleListener> mockListener = sptr<MockWindowLifeCycleListener>::MakeSptr();
    sptr<IWindowLifeCycle> listener = static_cast<sptr<IWindowLifeCycle>>(mockListener);

    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("Test");
    option->SetDisplayId(0);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    window->RegisterLifeCycleListener(listener);

    EXPECT_CALL(*mockListener, AfterDidBackground()).Times(1);
    ASSERT_EQ(WMError::WM_OK, window->Hide(static_cast<uint32_t>(WindowStateChangeReason::ABILITY_CALL), false, false));
}

/**
 * @tc.name: Resume
 * @tc.desc: Resume
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest5, Resume, Function | SmallTest | Level2)
{
    sptr<MockWindowLifeCycleListener> mockListener = sptr<MockWindowLifeCycleListener>::MakeSptr();
    sptr<IWindowLifeCycle> listener = static_cast<sptr<IWindowLifeCycle>>(mockListener);

    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("Test");
    option->SetDisplayId(0);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    window->RegisterLifeCycleListener(listener);
    window->SetTargetAPIVersion(16);

    EXPECT_CALL(*mockListener, AfterResumed()).Times(1);
    window->Resume();
}
}
} // namespace Rosen
} // namespace OHOS