/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "ability_context_impl.h"
#include "display_manager_proxy.h"
#include "mock_uicontent.h"
#include "mock_window_adapter.h"
#include "singleton_mocker.h"
#include "window_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;

class WindowImplTest5 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};
void WindowImplTest5::SetUpTestCase()
{
}

void WindowImplTest5::TearDownTestCase()
{
}

void WindowImplTest5::SetUp()
{
}

void WindowImplTest5::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
}

namespace {
/**
 * @tc.name: IsAllowHaveSystemSubWindow
 * @tc.desc: IsAllowHaveSystemSubWindow desc
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, IsAllowHaveSystemSubWindow, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("IsAllowHaveSystemSubWindow");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW);
    EXPECT_EQ(window->IsAllowHaveSystemSubWindow(), false);

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    EXPECT_EQ(window->IsAllowHaveSystemSubWindow(), false);

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    EXPECT_EQ(window->IsAllowHaveSystemSubWindow(), false);

    window->property_->SetWindowType(WindowType::APP_WINDOW_BASE);
    EXPECT_EQ(window->IsAllowHaveSystemSubWindow(), true);
}

/**
 * @tc.name: NotifyMemoryLevel
 * @tc.desc: NotifyMemoryLevel desc
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, NotifyMemoryLevel, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("NotifyMemoryLevel");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetWindowFlags(0));
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();

    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    EXPECT_CALL(*content, NotifyMemoryLevel(_)).Times(1).WillOnce(Return());
    window->NotifyMemoryLevel(1);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*content, Destroy());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetRequestedOrientation
 * @tc.desc: SetRequestedOrientation desc
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, SetRequestedOrientation, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetRequestedOrientation");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);
    window->property_->SetRequestedOrientation(Orientation::BEGIN);
    auto orientation = Orientation::BEGIN;
    window->SetRequestedOrientation(orientation);

    orientation = Orientation::VERTICAL;
    window->state_ = WindowState::STATE_CREATED;
    window->SetRequestedOrientation(orientation);

    window->state_ = WindowState::STATE_SHOWN;
    window->SetRequestedOrientation(orientation);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
}

/**
 * @tc.name: GetSystemAlarmWindowDefaultSize
 * @tc.desc: GetSystemAlarmWindowDefaultSize desc
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, GetSystemAlarmWindowDefaultSize, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("GetSystemAlarmWindowDefaultSize");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    Rect defaultRect { 10, 10, 10, 10 };

    SingletonContainer::GetInstance().destroyed_ = true;
    window->GetSystemAlarmWindowDefaultSize(defaultRect);

    SingletonContainer::GetInstance().destroyed_ = false;
    window->GetSystemAlarmWindowDefaultSize(defaultRect);
}

/**
 * @tc.name: CheckCameraFloatingWindowMultiCreated
 * @tc.desc: CheckCameraFloatingWindowMultiCreated desc
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, CheckCameraFloatingWindowMultiCreated, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("CheckCameraFloatingWindowMultiCreated");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);
    auto type = WindowType::WINDOW_TYPE_FLOAT;
    EXPECT_EQ(window->CheckCameraFloatingWindowMultiCreated(type), false);

    type = WindowType::WINDOW_TYPE_FLOAT_CAMERA;
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_EQ(window->CheckCameraFloatingWindowMultiCreated(type), false);

    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA);
    EXPECT_EQ(window->CheckCameraFloatingWindowMultiCreated(type), false);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetDefaultOption01
 * @tc.desc: SetDefaultOption desc
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, SetDefaultOption01, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetDefaultOption01");
    option->SetWindowType(WindowType::WINDOW_TYPE_STATUS_BAR);
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_VOLUME_OVERLAY);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_STATUS_BAR);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_KEYGUARD);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_DRAGGING_EFFECT);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_APP_COMPONENT);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_TOAST);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_FLOAT);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA);
    window->SetDefaultOption();
}

/**
 * @tc.name: SetDefaultOption02
 * @tc.desc: SetDefaultOption desc
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, SetDefaultOption02, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetDefaultOption02");
    option->SetWindowType(WindowType::WINDOW_TYPE_VOICE_INTERACTION);
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_LAUNCHER_DOCK);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_SEARCHING_BAR);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_SCREENSHOT);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_GLOBAL_SEARCH);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_BOOT_ANIMATION);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_POINTER);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_DOCK_SLICE);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_TOAST);
    window->SetDefaultOption();

    option->SetWindowType(WindowType::APP_WINDOW_BASE);
    window->SetDefaultOption();
}

/**
 * @tc.name: UpdateActiveStatus
 * @tc.desc: UpdateActiveStatus desc
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, UpdateActiveStatus, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("UpdateActiveStatus");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    bool isActive = true;
    window->UpdateActiveStatus(isActive);

    isActive = false;
    window->UpdateActiveStatus(isActive);
}

/**
 * @tc.name: NotifyForegroundInteractiveStatus
 * @tc.desc: NotifyForegroundInteractiveStatus desc
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, NotifyForegroundInteractiveStatus, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("NotifyForegroundInteractiveStatus");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    bool interactive = false;
    window->state_ = WindowState::STATE_INITIAL;
    window->NotifyForegroundInteractiveStatus(interactive);

    window->state_ = WindowState::STATE_CREATED;
    window->NotifyForegroundInteractiveStatus(interactive);

    window->state_ = WindowState::STATE_SHOWN;
    window->NotifyForegroundInteractiveStatus(interactive);

    interactive = true;
    window->NotifyForegroundInteractiveStatus(interactive);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
}

/**
 * @tc.name: UpdateWindowState02
 * @tc.desc: UpdateWindowState desc
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, UpdateWindowState02, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("UpdateWindowState02");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    window->state_ = WindowState::STATE_CREATED;
    std::shared_ptr<AbilityRuntime::Context> context = std::make_shared<AbilityRuntime::AbilityContextImpl>();
    window->context_ = context;

    WindowState state = WindowState::STATE_FROZEN;

    window->windowTag_ = WindowTag::MAIN_WINDOW;
    window->UpdateWindowState(state);

    window->windowTag_ = WindowTag::SUB_WINDOW;
    window->UpdateWindowState(state);

    state = WindowState::STATE_SHOWN;

    window->windowTag_ = WindowTag::MAIN_WINDOW;
    window->UpdateWindowState(state);

    window->windowTag_ = WindowTag::SUB_WINDOW;
    window->UpdateWindowState(state);

    EXPECT_CALL(m->Mock(), RemoveWindow(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));

    state = WindowState::STATE_HIDDEN;

    window->windowTag_ = WindowTag::MAIN_WINDOW;
    window->UpdateWindowState(state);

    window->windowTag_ = WindowTag::SUB_WINDOW;
    window->UpdateWindowState(state);

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: UpdateWindowStateUnfrozen
 * @tc.desc: UpdateWindowStateUnfrozen desc
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, UpdateWindowStateUnfrozen, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("UpdateWindowStateUnfrozen");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);
    window->context_ = nullptr;

    window->state_ = WindowState::STATE_CREATED;
    window->UpdateWindowStateUnfrozen();

    window->state_ = WindowState::STATE_SHOWN;
    window->UpdateWindowStateUnfrozen();

    std::shared_ptr<AbilityRuntime::Context> context = std::make_shared<AbilityRuntime::AbilityContextImpl>();
    window->context_ = context;

    window->windowTag_ = WindowTag::MAIN_WINDOW;
    window->UpdateWindowStateUnfrozen();

    window->windowTag_ = WindowTag::SUB_WINDOW;
    window->UpdateWindowStateUnfrozen();

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: UpdateFocusStatus
 * @tc.desc: UpdateFocusStatus desc
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, UpdateFocusStatus, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("UpdateFocusStatus");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    bool focused = true;

    window->state_ = WindowState::STATE_CREATED;
    window->UpdateFocusStatus(focused);

    window->state_ = WindowState::STATE_HIDDEN;
    window->UpdateFocusStatus(focused);

    window->state_ = WindowState::STATE_SHOWN;
    window->UpdateFocusStatus(focused);

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}
}
} // namespace Rosen
} // namespace OHOS