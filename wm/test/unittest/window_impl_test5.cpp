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
class MockOccupiedAreaChangeListener : public IOccupiedAreaChangeListener {
public:
    MOCK_METHOD2(OnSizeChange, void(const sptr<OccupiedAreaChangeInfo>& info,
        const std::shared_ptr<RSTransaction>& rsTransaction));
};

class MockMmiPointerEvent : public MMI::PointerEvent {
public:
    MockMmiPointerEvent(): MMI::PointerEvent(0) {}
};

class MockWindowDragListener : public IWindowDragListener {
public:
    MOCK_METHOD3(OnDrag, void(int32_t x, int32_t y, DragEvent event));
};

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

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    window->SetDefaultOption();

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_VOLUME_OVERLAY);
    window->SetDefaultOption();

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    window->SetDefaultOption();

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_STATUS_BAR);
    window->SetDefaultOption();

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW);
    window->SetDefaultOption();

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_KEYGUARD);
    window->SetDefaultOption();

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_DRAGGING_EFFECT);
    window->SetDefaultOption();

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_COMPONENT);
    window->SetDefaultOption();

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_TOAST);
    window->SetDefaultOption();

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    window->SetDefaultOption();

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_FLOAT);
    window->SetDefaultOption();

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA);
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

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_LAUNCHER_DOCK);
    window->SetDefaultOption();

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_SEARCHING_BAR);
    window->SetDefaultOption();

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_SCREENSHOT);
    window->SetDefaultOption();

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_GLOBAL_SEARCH);
    window->SetDefaultOption();

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    window->SetDefaultOption();

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_BOOT_ANIMATION);
    window->SetDefaultOption();

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_POINTER);
    window->SetDefaultOption();

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_DOCK_SLICE);
    window->SetDefaultOption();

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_TOAST);
    window->SetDefaultOption();

    window->property_->SetWindowType(WindowType::APP_WINDOW_BASE);
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

/**
 * @tc.name: UnregisterListener
 * @tc.desc: UnregisterListener | RegisterListener desc
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, UnregisterListener, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("UnregisterListener");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    sptr<MockOccupiedAreaChangeListener> listener1;
    window->occupiedAreaChangeListeners_[window->GetWindowId()].push_back(listener1);
    sptr<MockOccupiedAreaChangeListener> listener2 = new (std::nothrow) MockOccupiedAreaChangeListener();
    ASSERT_NE(listener2, nullptr);
    window->UnregisterOccupiedAreaChangeListener(listener2);
    window->RegisterOccupiedAreaChangeListener(nullptr);
    window->occupiedAreaChangeListeners_[window->GetWindowId()].clear();
}

/**
 * @tc.name: Close
 * @tc.desc: Close test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, Close, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("Close");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    window->state_ = WindowState::STATE_CREATED;
    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ASSERT_EQ(window->Close(), WMError::WM_OK);

    std::shared_ptr<AbilityRuntime::Context> context = std::make_shared<AbilityRuntime::AbilityContextImpl>();
    window->context_ = context;
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->Close();
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
}

/**
 * @tc.name: GetVSyncPeriod
 * @tc.desc: GetVSyncPeriod test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, GetVSyncPeriod, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("GetVSyncPeriod");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    window->SetWindowState(WindowState::STATE_DESTROYED);
    ASSERT_EQ(window->GetWindowState(), WindowState::STATE_DESTROYED);
    SingletonContainer::GetInstance().destroyed_ = true;
    ASSERT_EQ(window->GetVSyncPeriod(), 0);

    SingletonContainer::GetInstance().destroyed_ = false;
    window->vsyncStation_ = std::make_shared<VsyncStation>(1);
    window->GetVSyncPeriod();

    window->vsyncStation_ = nullptr;
    ASSERT_EQ(window->GetVSyncPeriod(), 0);
}

/**
 * @tc.name: RequestVsync02
 * @tc.desc: RequestVsync test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, RequestVsync02, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("RequestVsync02");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    window->state_ = WindowState::STATE_CREATED;
    std::shared_ptr<VsyncCallback> callback = std::make_shared<VsyncCallback>();

    SingletonContainer::GetInstance().destroyed_ = true;
    window->RequestVsync(callback);

    SingletonContainer::GetInstance().destroyed_ = false;
    window->vsyncStation_ = std::make_shared<VsyncStation>(1);
    window->RequestVsync(callback);

    window->vsyncStation_ = nullptr;
    window->RequestVsync(callback);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
}

/**
 * @tc.name: ConsumePointerEvent02
 * @tc.desc: ConsumePointerEvent test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, ConsumePointerEvent02, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("ConsumePointerEvent02");
    option->SetWindowType(WindowType::WINDOW_TYPE_LAUNCHER_RECENT);
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    Rect rect { 0, 0, 10u, 10u };
    window->property_->SetWindowRect(rect);

    std::shared_ptr<MMI::PointerEvent> pointerEvent = std::make_shared<MockMmiPointerEvent>();
    MMI::PointerEvent::PointerItem item;
    pointerEvent->SetPointerId(0);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    window->ConsumePointerEvent(pointerEvent);

    item.SetPointerId(0);
    item.SetDisplayX(15);
    item.SetDisplayY(15);
    pointerEvent->AddPointerItem(item);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_UP);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->ConsumePointerEvent(pointerEvent);

    item.SetDisplayX(5);
    item.SetDisplayY(5);
    pointerEvent->UpdatePointerItem(0, item);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    window->ConsumePointerEvent(pointerEvent);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_CANCEL);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_DOCK_SLICE);
    window->ConsumePointerEvent(pointerEvent);

    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->ConsumePointerEvent(pointerEvent);

    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    window->ConsumePointerEvent(pointerEvent);

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_LAUNCHER_RECENT);
    window->ConsumePointerEvent(pointerEvent);
}

/**
 * @tc.name: PerfLauncherHotAreaIfNeed
 * @tc.desc: PerfLauncherHotAreaIfNeed test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, PerfLauncherHotAreaIfNeed, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("PerfLauncherHotAreaIfNeed");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    std::shared_ptr<MMI::PointerEvent> pointerEvent = std::make_shared<MockMmiPointerEvent>();
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_CANCEL);
    pointerEvent->SetPointerId(0);

    SingletonContainer::GetInstance().destroyed_ = true;
    window->PerfLauncherHotAreaIfNeed(pointerEvent);

    SingletonContainer::GetInstance().destroyed_ = false;
    window->PerfLauncherHotAreaIfNeed(pointerEvent);
}

/**
 * @tc.name: NotifyOccupiedAreaChange
 * @tc.desc: NotifyOccupiedAreaChange test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, NotifyOccupiedAreaChange, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("NotifyOccupiedAreaChange");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    sptr<MockOccupiedAreaChangeListener> listener;
    window->occupiedAreaChangeListeners_[window->GetWindowId()].push_back(listener);
    listener = new (std::nothrow) MockOccupiedAreaChangeListener();
    ASSERT_NE(listener, nullptr);
    window->occupiedAreaChangeListeners_[window->GetWindowId()].push_back(listener);
    EXPECT_CALL(*listener, OnSizeChange(_, _));
    sptr<OccupiedAreaChangeInfo> info = new (std::nothrow) OccupiedAreaChangeInfo();
    ASSERT_NE(info, nullptr);
    std::shared_ptr<RSTransaction> rsTransaction;
    window->NotifyOccupiedAreaChange(info, rsTransaction);
    window->occupiedAreaChangeListeners_[window->GetWindowId()].clear();
}

/**
 * @tc.name: NotifyDragEvent
 * @tc.desc: NotifyDragEvent test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, NotifyDragEvent, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("NotifyDragEvent");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    sptr<MockWindowDragListener> listener;
    window->windowDragListeners_.push_back(listener);
    listener = new (std::nothrow) MockWindowDragListener();
    ASSERT_NE(listener, nullptr);
    window->windowDragListeners_.push_back(listener);
    EXPECT_CALL(*listener, OnDrag(_, _, _));
    PointInfo point({10, 20});
    window->NotifyDragEvent(point, DragEvent::DRAG_EVENT_OUT);
    window->windowDragListeners_.clear();
}

/**
 * @tc.name: TransferPointerEvent02
 * @tc.desc: TransferPointerEvent test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, TransferPointerEvent02, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("TransferPointerEvent02");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    window->TransferPointerEvent(nullptr);

    window->windowSystemConfig_.isStretchable_ = true;
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);

    std::shared_ptr<MMI::PointerEvent> pointerEvent = std::make_shared<MockMmiPointerEvent>();
    window->TransferPointerEvent(pointerEvent);
}

/**
 * @tc.name: ReadyToMoveOrDragWindow
 * @tc.desc: ReadyToMoveOrDragWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, ReadyToMoveOrDragWindow, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("ReadyToMoveOrDragWindow");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    window->moveDragProperty_->pointEventStarted_ = false;
    std::shared_ptr<MMI::PointerEvent> pointerEvent = std::make_shared<MockMmiPointerEvent>();
    MMI::PointerEvent::PointerItem item;
    SingletonContainer::GetInstance().destroyed_ = true;
    window->ReadyToMoveOrDragWindow(pointerEvent, item);

    SingletonContainer::GetInstance().destroyed_ = false;
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_DOCK_SLICE);
    window->ReadyToMoveOrDragWindow(pointerEvent, item);

    window->property_->SetMaximizeMode(MaximizeMode::MODE_RECOVER);
    window->ReadyToMoveOrDragWindow(pointerEvent, item);

    window->property_->SetMaximizeMode(MaximizeMode::MODE_AVOID_SYSTEM_BAR);
    window->ReadyToMoveOrDragWindow(pointerEvent, item);
}

/**
 * @tc.name: StartMove04
 * @tc.desc: StartMove test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, StartMove04, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("StartMove04");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    window->moveDragProperty_->pointEventStarted_ = true;
    window->moveDragProperty_->startDragFlag_ = false;
    window->StartMove();

    window->moveDragProperty_->startDragFlag_ = true;
    window->StartMove();

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IsPointInDragHotZone
 * @tc.desc: IsPointInDragHotZone test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, IsPointInDragHotZone, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("IsPointInDragHotZone");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    Rect rect{ 10, 10, 10, 10 };
    window->property_->SetWindowRect(rect);
    window->moveDragProperty_->startRectExceptFrame_ = { 6, 6, 18, 18 };
    window->moveDragProperty_->startRectExceptCorner_ = { 6, 6, 18, 18 };
    ASSERT_EQ(window->IsPointInDragHotZone(6, 6, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN), true);

    ASSERT_EQ(window->IsPointInDragHotZone(5, 5, MMI::PointerEvent::SOURCE_TYPE_MOUSE), false);

    ASSERT_EQ(window->IsPointInDragHotZone(5, 5, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN), true);

    ASSERT_EQ(window->IsPointInDragHotZone(10, 10, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN), false);

    window->moveDragProperty_->startRectExceptCorner_ = { 16, 16, 18, 18 };
    ASSERT_EQ(window->IsPointInDragHotZone(10, 10, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN), true);
}

/**
 * @tc.name: CalculateStartRectExceptHotZone
 * @tc.desc: CalculateStartRectExceptHotZone test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, CalculateStartRectExceptHotZone, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("CalculateStartRectExceptHotZone");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    window->property_->SetDisplayZoomState(true);
    window->property_->SetTransform(Transform::Identity());
    window->CalculateStartRectExceptHotZone(1.0);

    window->property_->SetDisplayZoomState(false);
    window->CalculateStartRectExceptHotZone(1.0);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: PendingClose02
 * @tc.desc: PendingClose test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, PendingClose02, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("PendingClose02");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->context_ = nullptr;
    window->PendingClose();
}

/**
 * @tc.name: Recover03
 * @tc.desc: Recover test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, Recover03, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("Recover03");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Create(INVALID_WINDOW_ID);
    window->state_ = WindowState::STATE_CREATED;
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    window->property_->SetMaximizeMode(MaximizeMode::MODE_AVOID_SYSTEM_BAR);
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(window->Recover(), WMError::WM_OK);

    window->property_->SetMaximizeMode(MaximizeMode::MODE_FULL_FILL);
    ASSERT_EQ(window->Recover(), WMError::WM_OK);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Minimize03
 * @tc.desc: Minimize test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, Minimize03, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("Minimize03");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    window->state_ = WindowState::STATE_CREATED;
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    std::shared_ptr<AbilityRuntime::Context> context = std::make_shared<AbilityRuntime::AbilityContextImpl>();
    window->context_ = context;
    window->Minimize();
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
}

/**
 * @tc.name: RegisterListener
 * @tc.desc: UnregisterListener | RegisterListener desc
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, RegisterListener, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("RegisterListener");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    sptr<MockOccupiedAreaChangeListener> listener1;
    window->occupiedAreaChangeListeners_[window->GetWindowId()].push_back(listener1);
    sptr<MockOccupiedAreaChangeListener> listener2 = new (std::nothrow) MockOccupiedAreaChangeListener();
    ASSERT_NE(listener2, nullptr);
    window->UnregisterOccupiedAreaChangeListener(nullptr);
    window->occupiedAreaChangeListeners_[window->GetWindowId()].push_back(listener2);
    window->RegisterOccupiedAreaChangeListener(listener2);
    window->occupiedAreaChangeListeners_[window->GetWindowId()].clear();
}

/**
 * @tc.name: SetImmersiveModeEnabledState02
 * @tc.desc: SetImmersiveModeEnabledState test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, SetImmersiveModeEnabledState02, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetImmersiveModeEnabledState02");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    window->state_ = WindowState::STATE_INITIAL;
    EXPECT_EQ(window->SetImmersiveModeEnabledState(true), WMError::WM_ERROR_INVALID_WINDOW);

    window->state_ = WindowState::STATE_CREATED;
    window->UpdateModeSupportInfo(WindowModeSupport::WINDOW_MODE_SUPPORT_ALL);

    window->property_->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    EXPECT_EQ(window->SetImmersiveModeEnabledState(true), WMError::WM_ERROR_INVALID_WINDOW);

    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    EXPECT_EQ(window->SetImmersiveModeEnabledState(true), WMError::WM_OK);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
}

/**
 * @tc.name: SetGlobalMaximizeMode
 * @tc.desc: SetGlobalMaximizeMode test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, SetGlobalMaximizeMode, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetGlobalMaximizeMode");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    window->state_ = WindowState::STATE_INITIAL;
    EXPECT_EQ(window->SetGlobalMaximizeMode(MaximizeMode::MODE_RECOVER), WMError::WM_ERROR_INVALID_WINDOW);

    window->state_ = WindowState::STATE_CREATED;
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    EXPECT_EQ(window->SetGlobalMaximizeMode(MaximizeMode::MODE_RECOVER), WMError::WM_OK);

    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    EXPECT_EQ(window->SetGlobalMaximizeMode(MaximizeMode::MODE_RECOVER), WMError::WM_ERROR_INVALID_PARAM);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
}

/**
 * @tc.name: MaximizeFloating02
 * @tc.desc: MaximizeFloating test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, MaximizeFloating02, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("MaximizeFloating02");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    window->state_ = WindowState::STATE_CREATED;
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->MaximizeFloating();
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
}

/**
 * @tc.name: SetCallingWindow
 * @tc.desc: SetCallingWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, SetCallingWindow, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetCallingWindow");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    window->state_ = WindowState::STATE_INITIAL;
    window->SetCallingWindow(1);
}

/**
 * @tc.name: Resize
 * @tc.desc: Resize test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, Resize, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("Resize");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    window->state_ = WindowState::STATE_CREATED;
    EXPECT_EQ(window->Resize(10, 10), WMError::WM_OK);

    window->state_ = WindowState::STATE_HIDDEN;
    EXPECT_EQ(window->Resize(10, 10), WMError::WM_OK);

    window->state_ = WindowState::STATE_SHOWN;
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    EXPECT_EQ(window->Resize(10, 10), WMError::WM_ERROR_INVALID_OPERATION);

    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    window->Resize(10, 10);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
}

/**
 * @tc.name: MoveTo
 * @tc.desc: MoveTo test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, MoveTo, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("MoveTo");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    window->state_ = WindowState::STATE_CREATED;
    EXPECT_EQ(window->MoveTo(10, 10), WMError::WM_OK);

    window->state_ = WindowState::STATE_HIDDEN;
    EXPECT_EQ(window->MoveTo(10, 10), WMError::WM_OK);

    window->state_ = WindowState::STATE_SHOWN;
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    EXPECT_EQ(window->MoveTo(10, 10), WMError::WM_ERROR_INVALID_OPERATION);

    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    window->MoveTo(10, 10);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
}

/**
 * @tc.name: AdjustWindowAnimationFlag
 * @tc.desc: AdjustWindowAnimationFlag test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, AdjustWindowAnimationFlag, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("AdjustWindowAnimationFlag");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    window->property_->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    sptr<IAnimationTransitionController> animationTransitionController = new (std::nothrow)
        IAnimationTransitionController();
    ASSERT_NE(animationTransitionController, nullptr);
    window->animationTransitionController_ = animationTransitionController;

    window->AdjustWindowAnimationFlag(true);

    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->needDefaultAnimation_ = true;
    window->AdjustWindowAnimationFlag(true);

    window->animationTransitionController_ = nullptr;
    window->needDefaultAnimation_ = false;
    window->AdjustWindowAnimationFlag(true);

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    window->AdjustWindowAnimationFlag(false);
}

/**
 * @tc.name: NeedToStopShowing
 * @tc.desc: NeedToStopShowing test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, NeedToStopShowing, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("NeedToStopShowing");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);
    EXPECT_EQ(window->NeedToStopShowing(), true);

    window->UpdateModeSupportInfo(WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    window->property_->SetWindowFlags(1 << 2);
    EXPECT_EQ(window->NeedToStopShowing(), false);

    window->property_->SetWindowFlags(1);
    EXPECT_EQ(window->NeedToStopShowing(), false);
}

/**
 * @tc.name: DestroyFloatingWindow
 * @tc.desc: DestroyFloatingWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, DestroyFloatingWindow, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("DestroyFloatingWindow");
    option->SetWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);
    option->SetWindowType(WindowType::WINDOW_TYPE_VOLUME_OVERLAY);
    option->SetWindowRect({ 1, 1, 1, 1 });
    option->SetBundleName("OK");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);
    window->DestroyFloatingWindow();

    std::map<uint32_t, std::vector<sptr<WindowImpl>>> appFloatingWindowMap;
    sptr<WindowImpl> windowImpl = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(windowImpl, nullptr);
    std::vector<sptr<WindowImpl>> v;
    std::vector<sptr<WindowImpl>> v2;
    v.push_back(windowImpl);
    appFloatingWindowMap.insert({0, v});
    appFloatingWindowMap.insert({0, v2});
    window->appFloatingWindowMap_ = appFloatingWindowMap;
    window->DestroyFloatingWindow();
}

/**
 * @tc.name: DestroyDialogWindow
 * @tc.desc: DestroyDialogWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, DestroyDialogWindow, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("DestroyDialogWindow");
    option->SetWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);
    option->SetWindowType(WindowType::WINDOW_TYPE_VOLUME_OVERLAY);
    option->SetWindowRect({ 1, 1, 1, 1 });
    option->SetBundleName("OK");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);
    window->DestroyFloatingWindow();

    std::map<uint32_t, std::vector<sptr<WindowImpl>>> appDialogWindowMap;
    sptr<WindowImpl> windowImpl = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(windowImpl, nullptr);
    std::vector<sptr<WindowImpl>> v;
    std::vector<sptr<WindowImpl>> v2;
    v.push_back(windowImpl);
    appDialogWindowMap.insert({0, v});
    appDialogWindowMap.insert({0, v2});
    window->appDialogWindowMap_ = appDialogWindowMap;
    window->DestroyFloatingWindow();
}

/**
 * @tc.name: GetOriginalAbilityInfo
 * @tc.desc: GetOriginalAbilityInfo test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, GetOriginalAbilityInfo, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("GetOriginalAbilityInfo");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    std::shared_ptr<AbilityRuntime::Context> context = std::make_shared<AbilityRuntime::AbilityContextImpl>();
    window->context_ = context;
    window->GetOriginalAbilityInfo();
}

/**
 * @tc.name: WindowCreateCheck05
 * @tc.desc: WindowCreateCheck test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, WindowCreateCheck05, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("WindowCreateCheck05");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    window->property_->SetWindowType(WindowType::APP_WINDOW_BASE);
    EXPECT_EQ(window->WindowCreateCheck(INVALID_WINDOW_ID), WMError::WM_OK);

    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    EXPECT_EQ(window->WindowCreateCheck(INVALID_WINDOW_ID), WMError::WM_ERROR_INVALID_PARENT);

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA);
    sptr<WindowImpl> windowImpl1 = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(windowImpl1, nullptr);
    windowImpl1->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA);
    WindowImpl::windowMap_.insert(std::make_pair("test", std::pair<uint32_t, sptr<Window>>(1, windowImpl1)));
    EXPECT_EQ(window->WindowCreateCheck(INVALID_WINDOW_ID), WMError::WM_ERROR_REPEAT_OPERATION);

    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    sptr<WindowImpl> windowImpl2 = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(windowImpl2, nullptr);
    windowImpl2->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    WindowImpl::windowMap_.insert(std::make_pair("test", std::pair<uint32_t, sptr<Window>>(0, windowImpl2)));
    EXPECT_EQ(window->WindowCreateCheck(0), WMError::WM_ERROR_INVALID_PARENT);

    sptr<WindowImpl> windowImpl3 = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(windowImpl3, nullptr);
    windowImpl3->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    WindowImpl::windowMap_.insert(std::make_pair("test", std::pair<uint32_t, sptr<Window>>(1, windowImpl3)));
    EXPECT_EQ(window->WindowCreateCheck(1), WMError::WM_OK);
}

/**
 * @tc.name: IsAppMainOrSubOrFloatingWindow
 * @tc.desc: IsAppMainOrSubOrFloatingWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, IsAppMainOrSubOrFloatingWindow, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("IsAppMainOrSubOrFloatingWindow");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    window->property_->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    EXPECT_EQ(window->IsAppMainOrSubOrFloatingWindow(), false);

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    WindowImpl::windowMap_.insert(std::make_pair("test", std::pair<uint32_t, sptr<Window>>(1, nullptr)));
    EXPECT_EQ(window->IsAppMainOrSubOrFloatingWindow(), false);

    sptr<WindowImpl> windowImpl1 = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(windowImpl1, nullptr);
    windowImpl1->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    WindowImpl::windowMap_.insert(std::make_pair("test", std::pair<uint32_t, sptr<Window>>(1, windowImpl1)));
    EXPECT_EQ(window->IsAppMainOrSubOrFloatingWindow(), false);

    sptr<WindowImpl> windowImpl2 = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(windowImpl2, nullptr);
    windowImpl2->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    WindowImpl::windowMap_.insert(std::make_pair("test", std::pair<uint32_t, sptr<Window>>(1, windowImpl2)));
    EXPECT_EQ(window->IsAppMainOrSubOrFloatingWindow(), false);

    sptr<WindowImpl> windowImpl3 = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(windowImpl3, nullptr);
    windowImpl3->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    std::shared_ptr<AbilityRuntime::Context> context = std::make_shared<AbilityRuntime::AbilityContextImpl>();
    window->context_ = context;
    windowImpl3->context_ = context;
    WindowImpl::windowMap_.insert(std::make_pair("test", std::pair<uint32_t, sptr<Window>>(1, windowImpl3)));
    EXPECT_EQ(window->IsAppMainOrSubOrFloatingWindow(), false);
}

/**
 * @tc.name: UpdateTitleButtonVisibility02
 * @tc.desc: UpdateTitleButtonVisibility test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, UpdateTitleButtonVisibility02, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("UpdateTitleButtonVisibility02");
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);

    window->uiContent_ = nullptr;
    window->UpdateTitleButtonVisibility();

    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    EXPECT_CALL(*content, HideWindowTitleButton(_, _, _, _));
    window->windowSystemConfig_.isSystemDecorEnable_ = false;
    window->UpdateTitleButtonVisibility();

    window->windowSystemConfig_.isSystemDecorEnable_ = true;
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->UpdateTitleButtonVisibility();
}

/**
 * @tc.name: GetConfigurationFromAbilityInfo02
 * @tc.desc: GetConfigurationFromAbilityInfo test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest5, GetConfigurationFromAbilityInfo02, Function | SmallTest | Level1)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("GetConfigurationFromAbilityInfo02");
    option->SetDisplayId(1);
    sptr<WindowImpl> window = new (std::nothrow) WindowImpl(option);
    ASSERT_NE(window, nullptr);
    ASSERT_EQ(1, window->GetDisplayId());

    std::shared_ptr<AbilityRuntime::AbilityContextImpl> context =
        std::make_shared<AbilityRuntime::AbilityContextImpl>();
    window->context_ = context;
    window->GetConfigurationFromAbilityInfo();

    std::shared_ptr<AppExecFwk::AbilityInfo> info = std::make_shared<AppExecFwk::AbilityInfo>();
    context->SetAbilityInfo(info);
    window->GetConfigurationFromAbilityInfo();

    std::vector<AppExecFwk::SupportWindowMode> supportModes;
    supportModes.push_back(AppExecFwk::SupportWindowMode::SPLIT);
    context->GetAbilityInfo()->windowModes = supportModes;
    window->GetConfigurationFromAbilityInfo();
}
}
} // namespace Rosen
} // namespace OHOS