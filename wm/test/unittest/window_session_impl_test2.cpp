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

#include "window_session_impl.h"

#include <gtest/gtest.h>

#include "ability_context_impl.h"
#include "display_info.h"
#include "mock_session.h"
#include "mock_uicontent.h"
#include "mock_window.h"
#include "parameters.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowSessionImplTest2 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<WindowSessionImpl> window_;

private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 50000;
};

void WindowSessionImplTest2::SetUpTestCase()
{
}

void WindowSessionImplTest2::TearDownTestCase()
{
}

void WindowSessionImplTest2::SetUp()
{
}

void WindowSessionImplTest2::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    if (window_ != nullptr) {
        window_->Destroy();
    }
}

namespace {
sptr<WindowSessionImpl> GetTestWindowImpl(const std::string& name)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    if (option == nullptr) {
        return nullptr;
    }
    option->SetWindowName(name);
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    if (window == nullptr) {
        return nullptr;
    }

    SessionInfo sessionInfo = {name, name, name};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    if (session == nullptr) {
        return nullptr;
    }

    window->hostSession_ = session;
    return window;
}

template <typename TListener, typename MockListener>
std::vector<sptr<TListener>> GetListenerList()
{
    std::vector<sptr<TListener>> listeners;
    sptr<TListener> listener = new (std::nothrow) MockListener();
    if (listener == nullptr) {
        return listeners;
    }

    listeners.insert(listeners.begin(), listener);
    return listeners;
}

/**
 * @tc.name: GetTitleButtonVisible
 * @tc.desc: GetTitleButtonVisible
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, GetTitleButtonVisible, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("GetTitleButtonVisible");
    ASSERT_NE(window, nullptr);
    bool isPC = false;
    bool isMaximizeVisible = false;
    bool isMinimizeVisible = false;
    bool isSplitVisible = false;
    window->GetTitleButtonVisible(isPC, isMaximizeVisible, isMinimizeVisible, isSplitVisible);
    ASSERT_FALSE(isSplitVisible);

    isPC = true;
    window->windowTitleVisibleFlags_.isSplitVisible = false;
    window->windowTitleVisibleFlags_.isMaximizeVisible = false;
    window->windowTitleVisibleFlags_.isMinimizeVisible = false;
    window->GetTitleButtonVisible(isPC, isMaximizeVisible, isMinimizeVisible, isSplitVisible);
    ASSERT_TRUE(isSplitVisible);
    ASSERT_TRUE(isMaximizeVisible);
    ASSERT_TRUE(isMinimizeVisible);

    window->windowTitleVisibleFlags_.isSplitVisible = true;
    window->windowTitleVisibleFlags_.isMaximizeVisible = true;
    window->windowTitleVisibleFlags_.isMinimizeVisible = true;
    window->GetTitleButtonVisible(isPC, isMaximizeVisible, isMinimizeVisible, isSplitVisible);
    ASSERT_TRUE(isSplitVisible);
    ASSERT_TRUE(isMaximizeVisible);
    ASSERT_TRUE(isMinimizeVisible);
}

/**
 * @tc.name: GetSystemSessionConfig
 * @tc.desc: GetSystemSessionConfig
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, GetSystemSessionConfig, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("GetSystemSessionConfig");
    ASSERT_NE(window, nullptr);
    window->GetSystemSessionConfig();
    window->Destroy();
}

/**
 * @tc.name: GetColorSpaceFromSurfaceGamut
 * @tc.desc: GetColorSpaceFromSurfaceGamut
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, GetColorSpaceFromSurfaceGamut, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("GetColorSpaceFromSurfaceGamut");
    ASSERT_NE(window, nullptr);
    ASSERT_EQ(window->GetColorSpaceFromSurfaceGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB),
        ColorSpace::COLOR_SPACE_DEFAULT);
    ASSERT_EQ(window->GetColorSpaceFromSurfaceGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3),
        ColorSpace::COLOR_SPACE_WIDE_GAMUT);
    ASSERT_EQ(window->GetColorSpaceFromSurfaceGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_INVALID),
        ColorSpace::COLOR_SPACE_DEFAULT);
    window->Destroy();
}

/**
 * @tc.name: GetSurfaceGamutFromColorSpace
 * @tc.desc: GetSurfaceGamutFromColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, GetSurfaceGamutFromColorSpace, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("GetSurfaceGamutFromColorSpace");
    ASSERT_NE(window, nullptr);
    ASSERT_EQ(window->GetSurfaceGamutFromColorSpace(ColorSpace::COLOR_SPACE_DEFAULT),
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    ASSERT_EQ(window->GetSurfaceGamutFromColorSpace(ColorSpace::COLOR_SPACE_WIDE_GAMUT),
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3);
    ASSERT_EQ(window->GetSurfaceGamutFromColorSpace(ColorSpace(uint32_t(3))),
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    window->Destroy();
}

/**
 * @tc.name: Create
 * @tc.desc: Create
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, Create, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("Create");
    ASSERT_NE(window, nullptr);
    std::shared_ptr<AbilityRuntime::Context> context;
    sptr<Rosen::ISession> ISession;
    ASSERT_EQ(window->Create(context, ISession), WMError::WM_OK);
    window->Destroy();
}

/**
 * @tc.name: Destroy
 * @tc.desc: Destroy
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, Destroy, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("Destroy");
    ASSERT_NE(window, nullptr);

    window->property_->SetPersistentId(INVALID_SESSION_ID);
    ASSERT_EQ(window->Destroy(true, true), WMError::WM_ERROR_INVALID_WINDOW);

    window->property_->SetPersistentId(1);
    window->state_ = WindowState::STATE_DESTROYED;
    ASSERT_EQ(window->Destroy(true, true), WMError::WM_ERROR_INVALID_WINDOW);

    window->state_ = WindowState::STATE_INITIAL;
    ASSERT_EQ(window->Destroy(true, true), WMError::WM_OK);

    window = GetTestWindowImpl("Destroy");
    ASSERT_NE(window, nullptr);
    window->state_ = WindowState::STATE_INITIAL;
    window->property_->SetPersistentId(1);
    ASSERT_EQ(window->Destroy(true, false), WMError::WM_OK);

    window->hostSession_ = nullptr;
    ASSERT_EQ(window->Destroy(true, true), WMError::WM_ERROR_INVALID_WINDOW);

    window = GetTestWindowImpl("Destroy");
    ASSERT_NE(window, nullptr);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> hostSession = new (std::nothrow) SessionMocker(sessionInfo);
    window->hostSession_ = hostSession;
    ASSERT_EQ(window->Destroy(true, true), WMError::WM_ERROR_INVALID_WINDOW);

    window = GetTestWindowImpl("Destroy");
    ASSERT_NE(window, nullptr);
    window->hostSession_ = hostSession;
    window->state_ = WindowState::STATE_INITIAL;
    window->property_->SetPersistentId(1);
    ASSERT_FALSE(window->IsWindowSessionInvalid());
    window->context_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();
    ASSERT_EQ(window->Destroy(true, true), WMError::WM_OK);
}

/**
 * @tc.name: GetWindowState
 * @tc.desc: GetWindowState
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, GetWindowState, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("GetWindowState");
    ASSERT_NE(window, nullptr);
    window->state_ = WindowState::STATE_DESTROYED;
    ASSERT_EQ(window->GetWindowState(), WindowState::STATE_DESTROYED);
    window->Destroy();
}

/**
 * @tc.name: RecoverSessionListener
 * @tc.desc: RecoverSessionListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, RecoverSessionListener, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("RecoverSessionListener");
    ASSERT_NE(window, nullptr);
    int32_t id = 1;
    window->property_->SetPersistentId(id);
    window->RecoverSessionListener();

    std::vector<sptr<IAvoidAreaChangedListener>> iAvoidAreaChangedListeners;
    std::vector<sptr<ITouchOutsideListener>> iTouchOutsideListeners;
    window->avoidAreaChangeListeners_.insert({id, iAvoidAreaChangedListeners});
    window->touchOutsideListeners_.insert({id, iTouchOutsideListeners});
    window->RecoverSessionListener();

    window->avoidAreaChangeListeners_.clear();
    window->touchOutsideListeners_.clear();
    sptr<MockAvoidAreaChangedListener> changedListener = new (std::nothrow) MockAvoidAreaChangedListener();
    ASSERT_NE(nullptr, changedListener);
    sptr<MockTouchOutsideListener> touchOutsideListener = new (std::nothrow) MockTouchOutsideListener();
    ASSERT_NE(nullptr, touchOutsideListener);
    iAvoidAreaChangedListeners.insert(iAvoidAreaChangedListeners.begin(), changedListener);
    iTouchOutsideListeners.insert(iTouchOutsideListeners.begin(), touchOutsideListener);
    window->avoidAreaChangeListeners_.insert({id, iAvoidAreaChangedListeners});
    window->touchOutsideListeners_.insert({id, iTouchOutsideListeners});
    window->RecoverSessionListener();
    ASSERT_TRUE(window->avoidAreaChangeListeners_.find(id) != window->avoidAreaChangeListeners_.end() &&
                !window->avoidAreaChangeListeners_[id].empty());
    ASSERT_TRUE(window->touchOutsideListeners_.find(id) != window->touchOutsideListeners_.end() &&
                !window->touchOutsideListeners_[id].empty());
    window->Destroy();
}

/**
 * @tc.name: NotifyUIContentFocusStatus
 * @tc.desc: NotifyUIContentFocusStatus
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyUIContentFocusStatus, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("NotifyUIContentFocusStatus");
    ASSERT_NE(window, nullptr);
    window->isFocused_ = false;
    window->NotifyUIContentFocusStatus();

    window->isFocused_ = true;
    window->NotifyUIContentFocusStatus();

    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->NotifyUIContentFocusStatus();
    window->Destroy();
}

/**
 * @tc.name: NotifyAfterFocused
 * @tc.desc: NotifyAfterFocused
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyAfterFocused, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("NotifyAfterFocused");
    ASSERT_NE(window, nullptr);
    window->NotifyAfterFocused();
    ASSERT_TRUE(window->shouldReNotifyFocus_);

    window->shouldReNotifyFocus_ = false;
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->NotifyAfterFocused();
    ASSERT_FALSE(window->shouldReNotifyFocus_);
    window->Destroy();
}

/**
 * @tc.name: NotifyForegroundFailed
 * @tc.desc: NotifyForegroundFailed
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyForegroundFailed, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("NotifyForegroundFailed");
    ASSERT_NE(window, nullptr);
    window->NotifyForegroundFailed(WMError::WM_OK);
    window->Destroy();
}

/**
 * @tc.name: NotifyTransferComponentDataSync
 * @tc.desc: NotifyTransferComponentDataSync
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyTransferComponentDataSync, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("NotifyTransferComponentDataSync");
    ASSERT_NE(window, nullptr);
    AAFwk::WantParams wantParams;
    AAFwk::WantParams reWantParams;
    ASSERT_EQ(WSErrorCode::WS_OK, window->NotifyTransferComponentDataSync(wantParams, reWantParams));
    window->Destroy();
}

/**
 * @tc.name: UpdateAvoidArea
 * @tc.desc: UpdateAvoidArea
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, UpdateAvoidArea, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("UpdateAvoidArea");
    ASSERT_NE(window, nullptr);
    sptr<AvoidArea> avoidArea = new AvoidArea();
    avoidArea->topRect_ = {1, 0, 0, 0};
    avoidArea->leftRect_ = {0, 1, 0, 0};
    avoidArea->rightRect_ = {0, 0, 1, 0};
    avoidArea->bottomRect_ = {0, 0, 0, 1};
    AvoidAreaType type = AvoidAreaType::TYPE_SYSTEM;
    ASSERT_EQ(WSError::WS_OK, window->UpdateAvoidArea(avoidArea, type));
    window->Destroy();
}

/**
 * @tc.name: DispatchKeyEventCallback
 * @tc.desc: DispatchKeyEventCallback
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, DispatchKeyEventCallback, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("DispatchKeyEventCallback");
    ASSERT_NE(window, nullptr);

    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_VIRTUAL_MULTITASK);
    bool isConsumed = false;
    window->DispatchKeyEventCallback(keyEvent, isConsumed);
    ASSERT_FALSE(isConsumed);

    std::shared_ptr<MockInputEventConsumer> inputEventConsumer = std::make_shared<MockInputEventConsumer>();
    window->inputEventConsumer_ = inputEventConsumer;
    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_BACK);
    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    window->DispatchKeyEventCallback(keyEvent, isConsumed);

    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
    window->DispatchKeyEventCallback(keyEvent, isConsumed);

    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
    window->inputEventConsumer_ = nullptr;
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->DispatchKeyEventCallback(keyEvent, isConsumed);

    keyEvent->SetKeyAction(MMI::KeyEvent::KEYCODE_ESCAPE);
    window->DispatchKeyEventCallback(keyEvent, isConsumed);
    window->Destroy();
}

/**
 * @tc.name: HandleBackEvent01
 * @tc.desc: HandleBackEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, HandleBackEvent01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("HandleBackEvent01");
    sptr<WindowSessionImpl> windowSession = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(nullptr, windowSession);

    windowSession->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_EQ(WSError::WS_OK, windowSession->HandleBackEvent());
}

/**
 * @tc.name: IsKeyboardEvent
 * @tc.desc: IsKeyboardEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, IsKeyboardEvent, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("IsKeyboardEvent");
    ASSERT_NE(window, nullptr);

    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_VIRTUAL_MULTITASK);
    ASSERT_FALSE(window->IsKeyboardEvent(keyEvent));

    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_NUMPAD_RIGHT_PAREN);
    ASSERT_TRUE(window->IsKeyboardEvent(keyEvent));

    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_BACK);
    ASSERT_TRUE(window->IsKeyboardEvent(keyEvent));

    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_FN);
    ASSERT_TRUE(window->IsKeyboardEvent(keyEvent));

    window->Destroy();
}

/**
 * @tc.name: GetVSyncPeriod
 * @tc.desc: GetVSyncPeriod
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, GetVSyncPeriod, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("GetVSyncPeriod");
    ASSERT_NE(window, nullptr);

    auto vsyncStation = window->vsyncStation_;
    if (vsyncStation == nullptr) {
        vsyncStation = std::make_shared<VsyncStation>(DisplayId(0));
    }
    window->vsyncStation_ = nullptr;
    ASSERT_EQ(window->GetVSyncPeriod(), 0);

    window->vsyncStation_ = vsyncStation;
    window->GetVSyncPeriod();
    window->Destroy();
}

/**
 * @tc.name: FlushFrameRate
 * @tc.desc: FlushFrameRate
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, FlushFrameRate, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("FlushFrameRate");
    ASSERT_NE(window, nullptr);

    auto vsyncStation = window->vsyncStation_;
    if (vsyncStation == nullptr) {
        vsyncStation = std::make_shared<VsyncStation>(DisplayId(0));
    }
    window->vsyncStation_ = nullptr;
    window->FlushFrameRate(1, -1);

    window->vsyncStation_ = vsyncStation;
    window->FlushFrameRate(1, -1);
    window->Destroy();
}

/**
 * @tc.name: FindWindowById
 * @tc.desc: FindWindowById
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, FindWindowById, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("FindWindowById");
    ASSERT_NE(window, nullptr);
    window->windowSessionMap_.clear();
    ASSERT_EQ(window->FindWindowById(0), nullptr);

    window->windowSessionMap_.insert(
        {"test1", std::pair<int32_t, sptr<WindowSessionImpl>>(1, window)}
    );
    window->windowSessionMap_.insert(
        {"test2", std::pair<int32_t, sptr<WindowSessionImpl>>(2, window)}
    );
    ASSERT_EQ(window->FindWindowById(0), nullptr);

    window->windowSessionMap_.insert(
        {"test0", std::pair<int32_t, sptr<WindowSessionImpl>>(0, window)}
    );
    ASSERT_NE(window->FindWindowById(0), nullptr);
    window->Destroy();
}

/**
 * @tc.name: SetLayoutFullScreenByApiVersion
 * @tc.desc: SetLayoutFullScreenByApiVersion
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, SetLayoutFullScreenByApiVersion, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("SetLayoutFullScreenByApiVersion");
    ASSERT_NE(window, nullptr);
    window->windowSessionMap_.clear();
    ASSERT_EQ(window->SetLayoutFullScreenByApiVersion(true), WMError::WM_OK);
    window->Destroy();
}

/**
 * @tc.name: SetSystemBarProperty
 * @tc.desc: SetSystemBarProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, SetSystemBarProperty, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("SetSystemBarProperty");
    ASSERT_NE(window, nullptr);
    window->windowSessionMap_.clear();
    SystemBarProperty property;
    ASSERT_EQ(window->SetSystemBarProperty(WindowType::APP_MAIN_WINDOW_END, property), WMError::WM_OK);
    window->Destroy();
}

/**
 * @tc.name: SetSpecificBarProperty
 * @tc.desc: SetSpecificBarProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, SetSpecificBarProperty, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("SetSpecificBarProperty");
    ASSERT_NE(window, nullptr);
    window->windowSessionMap_.clear();
    SystemBarProperty property;
    ASSERT_EQ(window->SetSpecificBarProperty(WindowType::APP_MAIN_WINDOW_END, property), WMError::WM_OK);
    window->Destroy();
}

/**
 * @tc.name: NotifyOccupiedAreaChangeInfo
 * @tc.desc: NotifyOccupiedAreaChangeInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyOccupiedAreaChangeInfo, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("NotifyOccupiedAreaChangeInfo");
    ASSERT_NE(window, nullptr);

    auto listeners = GetListenerList<IOccupiedAreaChangeListener, MockIOccupiedAreaChangeListener>();
    ASSERT_NE(listeners.size(), 0);
    listeners.insert(listeners.begin(), nullptr);
    window->occupiedAreaChangeListeners_.insert({window->GetPersistentId(), listeners});

    sptr<OccupiedAreaChangeInfo> info = new (std::nothrow) OccupiedAreaChangeInfo();
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->NotifyOccupiedAreaChangeInfo(info);

    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    window->NotifyOccupiedAreaChangeInfo(info);

    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    window->windowSessionMap_.insert(
        {"test1", std::pair<int32_t, sptr<WindowSessionImpl>>(window->GetPersistentId(), nullptr)}
    );
    window->NotifyOccupiedAreaChangeInfo(info);
    window->windowSessionMap_.clear();

    window->windowSessionMap_.insert(
        {"test1", std::pair<int32_t, sptr<WindowSessionImpl>>(window->GetPersistentId(), window)}
    );
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    window->NotifyOccupiedAreaChangeInfo(info);

    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    window->NotifyOccupiedAreaChangeInfo(info);
    window->Destroy();
}

/**
 * @tc.name: NotifyWindowStatusChange
 * @tc.desc: NotifyWindowStatusChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyWindowStatusChange, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("NotifyWindowStatusChange");
    ASSERT_NE(window, nullptr);

    auto listeners = GetListenerList<IWindowStatusChangeListener, MockWindowStatusChangeListener>();
    ASSERT_NE(listeners.size(), 0);
    listeners.insert(listeners.begin(), nullptr);
    window->windowStatusChangeListeners_.insert({window->GetPersistentId(), listeners});

    WindowMode mode = WindowMode::WINDOW_MODE_FLOATING;
    window->state_ = WindowState::STATE_HIDDEN;
    window->property_->SetMaximizeMode(MaximizeMode::MODE_AVOID_SYSTEM_BAR);
    window->NotifyWindowStatusChange(mode);

    window->state_ = WindowState::STATE_FROZEN;
    window->property_->SetMaximizeMode(MaximizeMode::MODE_FULL_FILL);
    window->NotifyWindowStatusChange(mode);

    mode = WindowMode::WINDOW_MODE_SPLIT_PRIMARY;
    window->NotifyWindowStatusChange(mode);

    mode = WindowMode::WINDOW_MODE_SPLIT_SECONDARY;
    window->NotifyWindowStatusChange(mode);

    mode = WindowMode::WINDOW_MODE_PIP;
    window->NotifyWindowStatusChange(mode);
    window->Destroy();
}

/**
 * @tc.name: UpdatePiPRect
 * @tc.desc: UpdatePiPRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, UpdatePiPRect, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("UpdatePiPRect");
    ASSERT_NE(window, nullptr);

    window->property_->SetPersistentId(1);
    window->state_ = WindowState::STATE_FROZEN;
    Rect rect;
    window->UpdatePiPRect(rect, WindowSizeChangeReason::PIP_RATIO_CHANGE);

    window->hostSession_ = nullptr;
    window->UpdatePiPRect(rect, WindowSizeChangeReason::PIP_RATIO_CHANGE);
    window->Destroy();
}

/**
 * @tc.name: NotifyTransformChange
 * @tc.desc: NotifyTransformChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyTransformChange, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("NotifyTransformChange");
    ASSERT_NE(window, nullptr);

    Transform transform;
    window->NotifyTransformChange(transform);

    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->NotifyTransformChange(transform);
    window->Destroy();
}

/**
 * @tc.name: SubmitNoInteractionMonitorTask
 * @tc.desc: SubmitNoInteractionMonitorTask
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, SubmitNoInteractionMonitorTask, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("SubmitNoInteractionMonitorTask");
    ASSERT_NE(window, nullptr);

    IWindowNoInteractionListenerSptr listener = new (std::nothrow) MockWindowNoInteractionListener();
    window->SubmitNoInteractionMonitorTask(window->lastInteractionEventId_.load() + 1, listener);

    window->state_ = WindowState::STATE_SHOWN;
    window->SubmitNoInteractionMonitorTask(window->lastInteractionEventId_.load(), listener);

    window->state_ = WindowState::STATE_FROZEN;
    window->SubmitNoInteractionMonitorTask(window->lastInteractionEventId_.load(), listener);
    window->Destroy();
}

/**
 * @tc.name: RefreshNoInteractionTimeoutMonitor
 * @tc.desc: RefreshNoInteractionTimeoutMonitor
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, RefreshNoInteractionTimeoutMonitor, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("RefreshNoInteractionTimeoutMonitor");
    ASSERT_NE(window, nullptr);
    window->RefreshNoInteractionTimeoutMonitor();
    ASSERT_TRUE(window->windowNoInteractionListeners_[window->GetPersistentId()].empty());
    ASSERT_NE(window->property_, nullptr);
    window->property_->SetPersistentId(1);
    sptr<IWindowNoInteractionListener> listener = new (std::nothrow) MockWindowNoInteractionListener();
    ASSERT_EQ(window->RegisterWindowNoInteractionListener(listener), WMError::WM_OK);
    window->RefreshNoInteractionTimeoutMonitor();
    ASSERT_EQ(window->GetPersistentId(), 1);
    ASSERT_FALSE(window->windowNoInteractionListeners_[window->GetPersistentId()].empty());
    window->Destroy();
}

/**
 * @tc.name: IsUserOrientation
 * @tc.desc: IsUserOrientation
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, IsUserOrientation, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("IsUserOrientation");
    ASSERT_NE(window, nullptr);

    ASSERT_FALSE(window->IsUserOrientation(Orientation::FOLLOW_DESKTOP));
    ASSERT_TRUE(window->IsUserOrientation(Orientation::USER_ROTATION_PORTRAIT));
    ASSERT_TRUE(window->IsUserOrientation(Orientation::USER_ROTATION_LANDSCAPE));
    ASSERT_TRUE(window->IsUserOrientation(Orientation::USER_ROTATION_PORTRAIT_INVERTED));
    ASSERT_TRUE(window->IsUserOrientation(Orientation::USER_ROTATION_LANDSCAPE_INVERTED));
    window->Destroy();
}

/**
 * @tc.name: WindowSessionCreateCheck
 * @tc.desc: WindowSessionCreateCheck
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, WindowSessionCreateCheck, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("WindowSessionCreateCheck");
    ASSERT_NE(window, nullptr);

    int32_t nullWindowTestId = 1001;
    int32_t nullPropertyId = 1002;
    int32_t displayId = 1003;
    int32_t cameraId = 1004;
    
    window->windowSessionMap_.clear();
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA);
    window->windowSessionMap_.insert(
        std::make_pair<std::string, std::pair<int32_t, sptr<WindowSessionImpl>>>(
            "nullWindow",
            std::pair<int32_t, sptr<WindowSessionImpl>>(nullWindowTestId, nullptr)
        )
    );
    auto nullPropertyWindow = GetTestWindowImpl("nullPropertyWindow");
    ASSERT_NE(nullPropertyWindow, nullptr);
    nullPropertyWindow->property_ = nullptr;
    window->windowSessionMap_.insert(
        std::make_pair<std::string, std::pair<int32_t, sptr<WindowSessionImpl>>>(
            "nullPropertyWindow",
            std::pair<int32_t, sptr<WindowSessionImpl>>(nullPropertyId, nullPropertyWindow)
        )
    );

    auto displayWindow = GetTestWindowImpl("displayWindow");
    ASSERT_NE(displayWindow, nullptr);
    displayWindow->property_->SetWindowType(WindowType::WINDOW_TYPE_FREEZE_DISPLAY);
    window->windowSessionMap_.insert(
        std::make_pair<std::string, std::pair<int32_t, sptr<WindowSessionImpl>>>(
            "displayWindow",
            std::pair<int32_t, sptr<WindowSessionImpl>>(displayId, displayWindow)
        )
    );
    ASSERT_EQ(window->WindowSessionCreateCheck(), WMError::WM_OK);

    window->windowSessionMap_.clear();
    auto cameraWindow = GetTestWindowImpl("cameraWindow");
    ASSERT_NE(cameraWindow, nullptr);
    cameraWindow->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA);
    window->windowSessionMap_.insert(
        std::make_pair<std::string, std::pair<int32_t, sptr<WindowSessionImpl>>>(
            "cameraWindow",
            std::pair<int32_t, sptr<WindowSessionImpl>>(cameraId, cameraWindow)
        )
    );
    ASSERT_EQ(window->WindowSessionCreateCheck(), WMError::WM_ERROR_REPEAT_OPERATION);
    window->Destroy();
    nullPropertyWindow->Destroy();
    displayWindow->Destroy();
    cameraWindow->Destroy();
}

/**
 * @tc.name: NotifyForegroundInteractiveStatus
 * @tc.desc: NotifyForegroundInteractiveStatus
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyForegroundInteractiveStatus, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("NotifyForegroundInteractiveStatus");
    ASSERT_NE(window, nullptr);

    window->property_->SetPersistentId(1);
    window->state_ = WindowState::STATE_SHOWN;
    window->NotifyForegroundInteractiveStatus(true);
    window->NotifyForegroundInteractiveStatus(false);

    window->state_ = WindowState::STATE_DESTROYED;
    window->NotifyForegroundInteractiveStatus(true);
    window->state_ = WindowState::STATE_FROZEN;
    window->NotifyForegroundInteractiveStatus(true);
    window->Destroy();
}

/**
 * @tc.name: UpdateDecorEnableToAce
 * @tc.desc: UpdateDecorEnableToAce
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, UpdateDecorEnableToAce, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("UpdateDecorEnableToAce");
    ASSERT_NE(window, nullptr);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    auto listeners = GetListenerList<IWindowChangeListener, MockWindowChangeListener>();
    sptr<MockWindowChangeListener> nullListener;
    listeners.insert(listeners.begin(), nullListener);
    window->windowChangeListeners_.insert({window->GetPersistentId(), listeners});
    window->windowSystemConfig_.freeMultiWindowSupport_ = false;
    window->UpdateDecorEnableToAce(false);

    window->uiContent_ = nullptr;
    window->UpdateDecorEnableToAce(false);
    window->Destroy();
}

/**
 * @tc.name: UpdateDecorEnable
 * @tc.desc: UpdateDecorEnable
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, UpdateDecorEnable, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("UpdateDecorEnable");
    ASSERT_NE(window, nullptr);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->windowSystemConfig_.freeMultiWindowSupport_ = true;
    window->UpdateDecorEnable(true, WindowMode::WINDOW_MODE_UNDEFINED);

    window->windowSystemConfig_.freeMultiWindowSupport_ = false;
    window->UpdateDecorEnable(true, WindowMode::WINDOW_MODE_FULLSCREEN);

    window->uiContent_ = nullptr;
    window->UpdateDecorEnable(true, WindowMode::WINDOW_MODE_FULLSCREEN);
    window->UpdateDecorEnable(false, WindowMode::WINDOW_MODE_FULLSCREEN);
    window->Destroy();
}

/**
 * @tc.name: NotifyModeChange
 * @tc.desc: NotifyModeChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyModeChange, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("NotifyModeChange");
    ASSERT_NE(window, nullptr);
    auto listeners = GetListenerList<IWindowChangeListener, MockWindowChangeListener>();
    sptr<MockWindowChangeListener> nullListener;
    listeners.insert(listeners.begin(), nullListener);
    window->windowChangeListeners_.insert({window->GetPersistentId(), listeners});

    window->NotifyModeChange(WindowMode::WINDOW_MODE_FULLSCREEN, true);
    window->Destroy();
}

/**
 * @tc.name: SetRequestedOrientation
 * @tc.desc: SetRequestedOrientation
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, SetRequestedOrientation, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("SetRequestedOrientation");
    ASSERT_NE(window, nullptr);
    window->property_->SetRequestedOrientation(Orientation::BEGIN);
    window->SetRequestedOrientation(Orientation::END);
    
    window->property_->SetRequestedOrientation(Orientation::USER_ROTATION_PORTRAIT);
    window->SetRequestedOrientation(Orientation::USER_ROTATION_PORTRAIT);
    
    window->property_->SetRequestedOrientation(Orientation::BEGIN);
    window->SetRequestedOrientation(Orientation::BEGIN);
    window->Destroy();
}

/**
 * @tc.name: GetRequestedOrientation
 * @tc.desc: GetRequestedOrientation
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, GetRequestedOrientation, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("GetRequestedOrientation");
    ASSERT_NE(window, nullptr);
    window->property_ = nullptr;
    ASSERT_EQ(window->GetRequestedOrientation(), Orientation::UNSPECIFIED);
    window->Destroy();
}

/**
 * @tc.name: GetContentInfo
 * @tc.desc: GetContentInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, GetContentInfo, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("GetContentInfo");
    ASSERT_NE(window, nullptr);
    
    ASSERT_EQ(window->GetContentInfo(BackupAndRestoreType::CONTINUATION), "");
    ASSERT_EQ(window->GetContentInfo(BackupAndRestoreType::APP_RECOVERY), "");
    ASSERT_EQ(window->GetContentInfo(BackupAndRestoreType::NONE), "");
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->GetContentInfo(BackupAndRestoreType::NONE);
    window->Destroy();
}

/**
 * @tc.name: GetDecorHeight
 * @tc.desc: GetDecorHeight
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, GetDecorHeight, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("GetDecorHeight");
    ASSERT_NE(window, nullptr);
    int32_t height = -1;
    ASSERT_EQ(window->GetDecorHeight(height), WMError::WM_ERROR_NULLPTR);
    
    auto uiContent = std::make_unique<Ace::UIContentMocker>();
    EXPECT_CALL(*uiContent, GetContainerModalTitleHeight()).WillRepeatedly(Return(-1));
    window->uiContent_ = std::move(uiContent);
    ASSERT_EQ(window->GetDecorHeight(height), WMError::WM_OK);
    height = 1;
    window->GetDecorHeight(height);
    window->Destroy();
}

/**
 * @tc.name: GetTitleButtonArea
 * @tc.desc: GetTitleButtonArea
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, GetTitleButtonArea, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("GetTitleButtonArea");
    ASSERT_NE(window, nullptr);
    auto uiContent = std::make_unique<Ace::UIContentMocker>();
    EXPECT_CALL(*uiContent, GetContainerModalButtonsRect(testing::_, testing::_)).WillRepeatedly(Return(false));
    window->uiContent_ = std::move(uiContent);
    TitleButtonRect titleButtonRect;
    ASSERT_EQ(window->GetTitleButtonArea(titleButtonRect), WMError::WM_OK);
    window->Destroy();
}

/**
 * @tc.name: RegisterWindowTitleButtonRectChangeListener
 * @tc.desc: RegisterWindowTitleButtonRectChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, RegisterWindowTitleButtonRectChangeListener, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("RegisterWindowTitleButtonRectChangeListener");
    ASSERT_NE(window, nullptr);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    sptr<IWindowTitleButtonRectChangedListener> listener =
        new (std::nothrow) MockWindowTitleButtonRectChangedListener();
    ASSERT_NE(listener, nullptr);
    window->RegisterWindowTitleButtonRectChangeListener(listener);
    window->Destroy();
}

/**
 * @tc.name: UnregisterWindowTitleButtonRectChangeListener
 * @tc.desc: UnregisterWindowTitleButtonRectChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, UnregisterWindowTitleButtonRectChangeListener, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("UnregisterWindowTitleButtonRectChangeListener");
    ASSERT_NE(window, nullptr);
    sptr<IWindowTitleButtonRectChangedListener> listener =
        new (std::nothrow) MockWindowTitleButtonRectChangedListener();
    ASSERT_NE(listener, nullptr);
    window->UnregisterWindowTitleButtonRectChangeListener(listener);

    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->UnregisterWindowTitleButtonRectChangeListener(listener);
    window->Destroy();
}

/**
 * @tc.name: NotifyWindowTitleButtonRectChange
 * @tc.desc: NotifyWindowTitleButtonRectChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyWindowTitleButtonRectChange, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("NotifyWindowTitleButtonRectChange");
    ASSERT_NE(window, nullptr);
    auto listeners = GetListenerList<IWindowTitleButtonRectChangedListener,
        MockWindowTitleButtonRectChangedListener>();
    listeners.insert(listeners.begin(), nullptr);
    window->windowTitleButtonRectChangeListeners_.insert({window->GetPersistentId(), listeners});
    TitleButtonRect titleButtonRect;
    window->NotifyWindowTitleButtonRectChange(titleButtonRect);
    window->Destroy();
}

/**
 * @tc.name: RegisterWindowRectChangeListener
 * @tc.desc: RegisterWindowRectChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, RegisterWindowRectChangeListener, Function | SmallTest | Level2)
{
    auto window = GetTestWindowImpl("RegisterWindowRectChangeListener");
    ASSERT_NE(window, nullptr);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->RegisterWindowRectChangeListener(nullptr));

    sptr<IWindowRectChangeListener> listener = new (std::nothrow) MockWindowRectChangeListener();
    ASSERT_NE(listener, nullptr);
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowRectChangeListener(listener));

    window->hostSession_ = nullptr;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowRectChangeListener(listener));
    window->Destroy();
}

/**
 * @tc.name: UnregisterWindowRectChangeListener
 * @tc.desc: UnregisterWindowRectChangeListener01
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, UnregisterWindowRectChangeListener01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: UnregisterWindowRectChangeListener01 start";
    auto window = GetTestWindowImpl("UnregisterWindowRectChangeListener01");
    ASSERT_NE(window, nullptr);
    window->hostSession_ = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->RegisterWindowRectChangeListener(nullptr));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->UnregisterWindowRectChangeListener(nullptr));

    sptr<IWindowRectChangeListener> listener = new (std::nothrow) MockWindowRectChangeListener();
    ASSERT_NE(listener, nullptr);
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowRectChangeListener(listener));

    SessionInfo sessionInfo = {"CreateTestBunble", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> hostSession = new (std::nothrow) SessionMocker(sessionInfo);
    window->hostSession_ = hostSession;
    ASSERT_EQ(WMError::WM_OK, window->RegisterWindowRectChangeListener(listener));
    ASSERT_EQ(WMError::WM_OK, window->UnregisterWindowRectChangeListener(listener));
    window->Destroy();
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: UnregisterWindowRectChangeListener01 end";
}

/**
 * @tc.name: GetVirtualPixelRatio
 * @tc.desc: GetVirtualPixelRatio
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, GetVirtualPixelRatio, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: GetVirtualPixelRatio start";
    auto window = GetTestWindowImpl("GetVirtualPixelRatio");
    ASSERT_NE(nullptr, window);
    sptr<DisplayInfo> displayInfo = new (std::nothrow) DisplayInfo();
    float vpr = window->GetVirtualPixelRatio(displayInfo);
    ASSERT_EQ(1.0, vpr);
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: GetVirtualPixelRatio end";
}

/**
 * @tc.name: InitUIContent
 * @tc.desc: InitUIContent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, InitUIContent, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: InitUIContent start";
    auto window = GetTestWindowImpl("InitUIContent_Default");
    ASSERT_NE(window, nullptr);
    std::string contentInfo = "contentInfo";
    napi_env env = nullptr;
    napi_value storage = nullptr;
    WindowSetUIContentType type = WindowSetUIContentType::DEFAULT;
    AppExecFwk::Ability* ability = nullptr;
    OHOS::Ace::UIContentErrorCode aceRet;
    BackupAndRestoreType restoreType = BackupAndRestoreType::NONE;

    window->uiContent_ = nullptr;
    EXPECT_EQ(window->InitUIContent(contentInfo, env, storage, type, restoreType, ability, aceRet), WMError::WM_OK);

    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    EXPECT_EQ(window->InitUIContent(contentInfo, env, storage, type, restoreType, ability, aceRet), WMError::WM_OK);

    type = WindowSetUIContentType::RESTORE;
    EXPECT_EQ(window->InitUIContent(contentInfo, env, storage, type, restoreType, ability, aceRet), WMError::WM_OK);

    type = WindowSetUIContentType::BY_NAME;
    EXPECT_EQ(window->InitUIContent(contentInfo, env, storage, type, restoreType, ability, aceRet), WMError::WM_OK);

    type = WindowSetUIContentType::BY_ABC;
    EXPECT_EQ(window->InitUIContent(contentInfo, env, storage, type, restoreType, ability, aceRet), WMError::WM_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: InitUIContent end";
}

/**
 * @tc.name: NotifyScreenshot
 * @tc.desc: NotifyScreenshot01 listener==nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyScreenshot01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyScreenshot01 start";
    class MockIScrenshotListener : public IScreenshotListener
    {
    public:
        MOCK_METHOD0(OnScreenshot, void());
    };
    window_ = GetTestWindowImpl("NotifyScreenshot01");
    auto listeners = GetListenerList<IScreenshotListener, MockIScrenshotListener>();
    listeners[0] = nullptr;
    ASSERT_EQ(listeners.size(), 1);
    window_->screenshotListeners_.insert({window_->GetPersistentId(), listeners});
    window_->NotifyScreenshot();
    std::vector<sptr<IScreenshotListener>> screenshotListeners =
        window_->screenshotListeners_[window_->GetPersistentId()];
    ASSERT_NE(std::find(screenshotListeners.begin(), screenshotListeners.end(), nullptr), screenshotListeners.end());
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyScreenshot01 end";
}

/**
 * @tc.name: NotifyScreenshot
 * @tc.desc: NotifyScreenshot02 listener!=nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyScreenshot02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyScreenshot02 start";
    class ScreenshotListener : public IScreenshotListener
    {
    public:
        void OnScreenshot()
        {
            GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyScreenshot02 OnScreenshot";
            SUCCEED();
        }
    };
    window_ = GetTestWindowImpl("NotifyScreenshot02");
    sptr<IScreenshotListener> listener = new (std::nothrow) ScreenshotListener();
    window_->RegisterScreenshotListener(listener);
    window_->NotifyScreenshot();
    window_->UnregisterScreenshotListener(listener);
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyScreenshot02 end";
}

/**
 * @tc.name: NotifyTouchDialogTarget
 * @tc.desc: NotifyTouchDialogTarget01 hostSession_==nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyTouchDialogTarget01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyTouchDialogTarget01 start";
    window_ = GetTestWindowImpl("NotifyTouchDialogTarget01");
    int32_t posX = 100;
    int32_t posY = 100;
    window_->hostSession_ = nullptr;
    window_->NotifyTouchDialogTarget(posX, posY);
    sptr<ISession> hostSession = window_->GetHostSession();
    ASSERT_EQ(nullptr, hostSession);
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyTouchDialogTarget01 end";
}

/**
 * @tc.name: NotifyTouchDialogTarget
 * @tc.desc: NotifyTouchDialogTarget02 hostSession_!=nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyTouchDialogTarget02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyTouchDialogTarget02 start";
    window_ = GetTestWindowImpl("NotifyTouchDialogTarget02");
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> hostSession = new (std::nothrow) SessionMocker(sessionInfo);
    window_->hostSession_ = hostSession;
    int32_t posX = 100;
    int32_t posY = 100;
    window_->NotifyTouchDialogTarget(posX, posY);
    sptr<ISession> hostSession1 = window_->GetHostSession();
    ASSERT_NE(nullptr, hostSession1);
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyTouchDialogTarget02 end";
}

/**
 * @tc.name: NotifyTouchDialogTarget
 * @tc.desc: NotifyTouchDialogTarget03 hostSession_==nullptr listener==nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyTouchDialogTarget03, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyTouchDialogTarget03 start";
    class MockIDialogTargetTouchListener : public IDialogTargetTouchListener
    {
    public:
        MOCK_CONST_METHOD0(OnDialogTargetTouch, void());
    };
    window_ = GetTestWindowImpl("NotifyTouchDialogTarget03");
    window_->hostSession_ = nullptr;
    auto listeners = GetListenerList<IDialogTargetTouchListener, MockIDialogTargetTouchListener>();
    listeners[0] = nullptr;
    (window_->dialogTargetTouchListener_)[window_->GetPersistentId()] = listeners;
    int32_t posX = 100;
    int32_t posY = 100;
    window_->NotifyTouchDialogTarget(posX, posY);
    std::vector<sptr<IDialogTargetTouchListener>> dialogTargetTouchListeners =
        (window_->dialogTargetTouchListener_)[window_->GetPersistentId()];
    ASSERT_NE(std::find(dialogTargetTouchListeners.begin(), dialogTargetTouchListeners.end(), nullptr),
              dialogTargetTouchListeners.end());
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyTouchDialogTarget03 end";
}

/**
 * @tc.name: NotifyTouchDialogTarget
 * @tc.desc: NotifyTouchDialogTarget04 hostSession_==nullptr listener!=nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyTouchDialogTarget04, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyTouchDialogTarget04 start";
    class MockIDialogTargetTouchListener : public IDialogTargetTouchListener
    {
    public:
        void OnDialogTargetTouch() const
        {
            GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyTouchDialogTarget04 OnDialogTargetTouch";
            SUCCEED();
        }
    };
    window_ = GetTestWindowImpl("NotifyTouchDialogTarget04");
    window_->hostSession_ = nullptr;
    sptr<IDialogTargetTouchListener> dialogTargetTouchListener = new (std::nothrow) MockIDialogTargetTouchListener();
    window_->RegisterDialogTargetTouchListener(dialogTargetTouchListener);
    int32_t posX = 100;
    int32_t posY = 100;
    window_->NotifyTouchDialogTarget(posX, posY);
    window_->UnregisterDialogTargetTouchListener(dialogTargetTouchListener);
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyTouchDialogTarget04 end";
}

/**
 * @tc.name: NotifyDisplayMove
 * @tc.desc: NotifyDisplayMove01 listener==nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyDisplayMove01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyDisplayMove01 start";
    class MockIDisplayMoveListener : public IDisplayMoveListener
    {
    public:
        MOCK_METHOD2(OnDisplayMove, void(DisplayId from, DisplayId to));
    };
    window_ = GetTestWindowImpl("NotifyDisplayMove01");
    auto listeners = GetListenerList<IDisplayMoveListener, MockIDisplayMoveListener>();
    listeners[0] = nullptr;
    (window_->displayMoveListeners_)[window_->GetPersistentId()] = listeners;
    int32_t posX = 100;
    int32_t posY = 100;
    window_->NotifyTouchDialogTarget(posX, posY);
    std::vector<sptr<IDisplayMoveListener>> displayMoveListeners =
        (window_->displayMoveListeners_)[window_->GetPersistentId()];
    ASSERT_NE(std::find(displayMoveListeners.begin(), displayMoveListeners.end(), nullptr),
              displayMoveListeners.end());
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyDisplayMove01 end";
}

/**
 * @tc.name: NotifyDisplayMove
 * @tc.desc: NotifyDisplayMove02 listener!=nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyDisplayMove02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyDisplayMove02 start";
    class MockIDisplayMoveListener : public IDisplayMoveListener
    {
    public:
        void OnDisplayMove(DisplayId from, DisplayId to)
        {
            GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyDisplayMove02 OnDisplayMove";
            SUCCEED();
        }
    };
    window_ = GetTestWindowImpl("NotifyDisplayMove02");
    sptr<IDisplayMoveListener> displayMoveListener = new (std::nothrow) MockIDisplayMoveListener();
    EXPECT_EQ(window_->RegisterDisplayMoveListener(displayMoveListener), WMError::WM_OK);
    int32_t posX = 100;
    int32_t posY = 100;
    window_->NotifyTouchDialogTarget(posX, posY);
    EXPECT_EQ(window_->UnregisterDisplayMoveListener(displayMoveListener), WMError::WM_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyDisplayMove02 end";
}

/**
 * @tc.name: NotifyDestroy
 * @tc.desc: NotifyDestroy01 listener==nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyDestroy01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyDestroy01 start";
    class MockIDialogDeathRecipientListener : public IDialogDeathRecipientListener
    {
    public:
        MOCK_CONST_METHOD0(OnDialogDeathRecipient, void());
    };
    window_ = GetTestWindowImpl("NotifyDestroy01");
    auto listeners = GetListenerList<IDialogDeathRecipientListener, MockIDialogDeathRecipientListener>();
    listeners[0] = nullptr;
    (window_->dialogDeathRecipientListeners_)[window_->GetPersistentId()] = listeners;
    window_->NotifyDestroy();
    std::vector<sptr<IDialogDeathRecipientListener>> dialogDeathRecipientListeners =
        (window_->dialogDeathRecipientListeners_)[window_->GetPersistentId()];
    ASSERT_NE(std::find(dialogDeathRecipientListeners.begin(), dialogDeathRecipientListeners.end(), nullptr),
              dialogDeathRecipientListeners.end());
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyDestroy01 end";
}

/**
 * @tc.name: NotifyDestroy
 * @tc.desc: NotifyDestroy02 listener!=nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyDestroy02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyDestroy02 start";
    class MockIDialogDeathRecipientListener : public IDialogDeathRecipientListener
    {
    public:
        void OnDialogDeathRecipient() const
        {
            GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyDestroy02 OnDialogDeathRecipient";
            SUCCEED();
        }
    };
    window_ = GetTestWindowImpl("NotifyDestroy02");
    sptr<IDialogDeathRecipientListener> listener = new (std::nothrow) MockIDialogDeathRecipientListener();
    window_->RegisterDialogDeathRecipientListener(listener);
    window_->NotifyDestroy();
    window_->UnregisterDialogDeathRecipientListener(listener);
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyDestroy02 end";
}

/**
 * @tc.name: RegisterDialogTargetTouchListener
 * @tc.desc: RegisterDialogTargetTouchListener01 listener!=nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, RegisterDialogTargetTouchListener01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: RegisterDialogTargetTouchListener01 start";
    class MockIDialogTargetTouchListener : public IDialogTargetTouchListener
    {
    public:
        void OnDialogTargetTouch() const
        {
        }
    };
    window_ = GetTestWindowImpl("RegisterDialogTargetTouchListener01");
    sptr<IDialogTargetTouchListener> listener = new (std::nothrow) MockIDialogTargetTouchListener();
    WMError res = window_->RegisterDialogTargetTouchListener(listener);
    ASSERT_EQ(WMError::WM_OK, res);
    window_->UnregisterDialogTargetTouchListener(listener);
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: RegisterDialogTargetTouchListener01 end";
}

/**
 * @tc.name: RegisterDialogDeathRecipientListener
 * @tc.desc: RegisterDialogDeathRecipientListener01 listener!=nullptr
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, RegisterDialogDeathRecipientListener01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: RegisterDialogDeathRecipientListener01 start";
    class MockIDialogDeathRecipientListener : public IDialogDeathRecipientListener
    {
    public:
        void OnDialogDeathRecipient() const
        {
        }
    };
    window_ = GetTestWindowImpl("RegisterDialogDeathRecipientListener01");
    sptr<IDialogDeathRecipientListener> listener = new MockIDialogDeathRecipientListener();
    int32_t count = (window_->dialogDeathRecipientListeners_)[window_->GetPersistentId()].size();
    window_->RegisterDialogDeathRecipientListener(listener);
    std::vector<sptr<IDialogDeathRecipientListener>> dialogDeathRecipientListeners =
        (window_->dialogDeathRecipientListeners_)[window_->GetPersistentId()];
    ASSERT_EQ(++count, dialogDeathRecipientListeners.size());
    window_->UnregisterDialogDeathRecipientListener(listener);
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: RegisterDialogDeathRecipientListener01 end";
}

/**
 * @tc.name: RegisterSubWindowCloseListeners
 * @tc.desc: RegisterSubWindowCloseListeners01
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, RegisterSubWindowCloseListeners01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: RegisterSubWindowCloseListeners01 start";
    class MockISubWindowCloseListener : public ISubWindowCloseListener
    {
    public:
        void OnSubWindowClose(bool& terminateCloseProcess) {}
    };
    window_ = GetTestWindowImpl("RegisterSubWindowCloseListeners01");
    ASSERT_NE(window_, nullptr);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window_->RegisterSubWindowCloseListeners(nullptr));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window_->UnregisterSubWindowCloseListeners(nullptr));

    sptr<ISubWindowCloseListener> listener = new MockISubWindowCloseListener();
    window_->property_->SetWindowType(WindowType::WINDOW_TYPE_UI_EXTENSION);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_CALLING, window_->RegisterSubWindowCloseListeners(listener));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_CALLING, window_->UnregisterSubWindowCloseListeners(listener));

    window_->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    ASSERT_EQ(WMError::WM_OK, window_->RegisterSubWindowCloseListeners(listener));
    ASSERT_EQ(WMError::WM_OK, window_->UnregisterSubWindowCloseListeners(listener));
    window_->Destroy();
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: RegisterSubWindowCloseListeners01 end";
}

/**
 * @tc.name: GetListeners
 * @tc.desc: GetListeners01 IWindowLifeCycle
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, GetListeners01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: GetListeners01 start";
    window_ = GetTestWindowImpl("GetListeners01");
    ASSERT_NE(window_, nullptr);
    window_->lifecycleListeners_.clear();
    window_->NotifyWindowAfterFocused();
    ASSERT_TRUE(window_->lifecycleListeners_[window_->GetPersistentId()].empty());
    sptr<IWindowLifeCycle> listener = new (std::nothrow) MockWindowLifeCycleListener();
    window_->RegisterLifeCycleListener(listener);
    window_->NotifyWindowAfterFocused();
    ASSERT_FALSE(window_->lifecycleListeners_[window_->GetPersistentId()].empty());
    window_->Destroy();
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: GetListeners01 end";
}

/**
 * @tc.name: GetListeners
 * @tc.desc: GetListeners02 IOccupiedAreaChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, GetListeners02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: GetListeners02 start";
    window_ = GetTestWindowImpl("GetListeners02");
    ASSERT_NE(window_, nullptr);
    window_->occupiedAreaChangeListeners_.clear();
    sptr<OccupiedAreaChangeInfo> occupiedAreaChangeInfo = new OccupiedAreaChangeInfo();
    window_->NotifyOccupiedAreaChangeInfo(occupiedAreaChangeInfo, nullptr);
    ASSERT_TRUE(window_->occupiedAreaChangeListeners_[window_->GetPersistentId()].empty());
    sptr<IOccupiedAreaChangeListener> listener = new (std::nothrow) MockIOccupiedAreaChangeListener();
    window_->RegisterOccupiedAreaChangeListener(listener);
    window_->NotifyOccupiedAreaChangeInfo(occupiedAreaChangeInfo, nullptr);
    ASSERT_FALSE(window_->occupiedAreaChangeListeners_[window_->GetPersistentId()].empty());
    window_->Destroy();
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: GetListeners02 end";
}

/**
 * @tc.name: GetUIContent
 * @tc.desc: GetUIContent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, GetUIContent, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("GetUIContent");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    Ace::UIContent *res = window->GetUIContent();
    ASSERT_EQ(res, nullptr);
    ASSERT_EQ(window->Destroy(), WMError::WM_ERROR_INVALID_WINDOW);
}

/**
 * @tc.name: NotifySizeChange
 * @tc.desc: NotifySizeChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifySizeChange, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("NotifySizeChange");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    EXPECT_EQ(WMError::WM_OK, window->Create(nullptr, session));

    Rect rect;
    sptr<IWindowChangeListener> listener = new (std::nothrow) MockWindowChangeListener();
    ASSERT_NE(nullptr, listener);
    window->RegisterWindowChangeListener(listener);
    window->NotifySizeChange(rect, WindowSizeChangeReason::PIP_RATIO_CHANGE);

    sptr<IWindowRectChangeListener> listener1 = new (std::nothrow) MockWindowRectChangeListener();
    ASSERT_NE(nullptr, listener1);
    window->RegisterWindowRectChangeListener(listener1);
    window->NotifySizeChange(rect, WindowSizeChangeReason::PIP_RATIO_CHANGE);
    window->Destroy(true);
}

/**
 * @tc.name: AvoidAreaChangeListener
 * @tc.desc: AvoidAreaChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, AvoidAreaChangeListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("AvoidAreaChangeListener");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    EXPECT_EQ(WMError::WM_OK, window->Create(nullptr, session));

    sptr<IAvoidAreaChangedListener> nullListener = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->UnregisterAvoidAreaChangeListener(nullListener));

    sptr<IAvoidAreaChangedListener> listener = new (std::nothrow) MockAvoidAreaChangedListener();
    ASSERT_NE(nullptr, listener);
    window->UnregisterAvoidAreaChangeListener(listener);

    window->RegisterAvoidAreaChangeListener(nullListener);
    window->RegisterAvoidAreaChangeListener(listener);

    sptr<IAvoidAreaChangedListener> listener1 = new (std::nothrow) MockAvoidAreaChangedListener();
    ASSERT_NE(nullptr, listener1);
    window->RegisterAvoidAreaChangeListener(listener1);

    window->UnregisterAvoidAreaChangeListener(listener);
    window->UnregisterAvoidAreaChangeListener(listener1);
}

/**
 * @tc.name: TouchOutsideListener
 * @tc.desc: TouchOutsideListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, TouchOutsideListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("TouchOutsideListener");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    EXPECT_EQ(WMError::WM_OK, window->Create(nullptr, session));

    sptr<ITouchOutsideListener> nullListener = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->UnregisterTouchOutsideListener(nullListener));

    sptr<ITouchOutsideListener> listener = new (std::nothrow) MockTouchOutsideListener();
    ASSERT_NE(nullptr, listener);
    window->UnregisterTouchOutsideListener(listener);

    window->RegisterTouchOutsideListener(nullListener);
    window->RegisterTouchOutsideListener(listener);

    sptr<ITouchOutsideListener> listener1 = new (std::nothrow) MockTouchOutsideListener();
    ASSERT_NE(nullptr, listener1);
    window->RegisterTouchOutsideListener(listener1);

    window->UnregisterTouchOutsideListener(listener);
    window->UnregisterTouchOutsideListener(listener1);
}

/**
 * @tc.name: NotifyDialogStateChange
 * @tc.desc: NotifyDialogStateChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifyDialogStateChange, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyDialogStateChange start";
    window_ = GetTestWindowImpl("NotifyDialogStateChange");
    ASSERT_NE(window_, nullptr);
    ASSERT_EQ(window_->NotifyDialogStateChange(true), WSError::WS_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifyDialogStateChange end";
}

/**
 * @tc.name: SwitchFreeMultiWindow
 * @tc.desc: SwitchFreeMultiWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, SwitchFreeMultiWindow, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: SwitchFreeMultiWindow start";
    window_ = GetTestWindowImpl("SwitchFreeMultiWindow");
    ASSERT_NE(window_, nullptr);
    ASSERT_EQ(window_->SwitchFreeMultiWindow(true), WSError::WS_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: SwitchFreeMultiWindow end";
}

/**
 * @tc.name: UpdateTitleInTargetPos
 * @tc.desc: UpdateTitleInTargetPos
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, UpdateTitleInTargetPos, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: UpdateTitleInTargetPos start";
    window_ = GetTestWindowImpl("UpdateTitleInTargetPos");
    ASSERT_NE(window_, nullptr);
    ASSERT_EQ(window_->UpdateTitleInTargetPos(true, 100), WSError::WS_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: UpdateTitleInTargetPos end";
}

/**
 * @tc.name: NotifySessionBackground
 * @tc.desc: NotifySessionBackground
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, NotifySessionBackground, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifySessionBackground start";
    window_ = GetTestWindowImpl("NotifySessionBackground");
    ASSERT_NE(window_, nullptr);
    window_->NotifySessionBackground(true, true, true);
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: NotifySessionBackground end";
}

/**
 * @tc.name: UpdateMaximizeMode
 * @tc.desc: UpdateMaximizeMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, UpdateMaximizeMode, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: UpdateMaximizeMode start";
    window_ = GetTestWindowImpl("UpdateMaximizeMode");
    ASSERT_NE(window_, nullptr);
    ASSERT_EQ(window_->UpdateMaximizeMode(MaximizeMode::MODE_AVOID_SYSTEM_BAR), WSError::WS_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: UpdateMaximizeMode end";
}

/**
 * @tc.name: DumpSessionElementInfo
 * @tc.desc: DumpSessionElementInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, DumpSessionElementInfo, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: DumpSessionElementInfo start";
    window_ = GetTestWindowImpl("DumpSessionElementInfo");
    ASSERT_NE(window_, nullptr);
    std::vector<std::string> params;
    params.push_back("test");
    window_->DumpSessionElementInfo(params);
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: DumpSessionElementInfo end";
}

/**
 * @tc.name: GetKeyboardAnimationConfig
 * @tc.desc: GetKeyboardAnimationConfig
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, GetKeyboardAnimationConfig, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: GetKeyboardAnimationConfig start";
    window_ = GetTestWindowImpl("GetKeyboardAnimationConfig");
    ASSERT_NE(window_, nullptr);
    window_->GetKeyboardAnimationConfig();
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: GetKeyboardAnimationConfig end";
}

/**
 * @tc.name: SetWindowGravity
 * @tc.desc: SetWindowGravity
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, SetWindowGravity, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: SetWindowGravity start";
    window_ = GetTestWindowImpl("SetWindowGravity");
    ASSERT_NE(window_, nullptr);
    ASSERT_NE(window_->GetHostSession(), nullptr);
    window_->hostSession_ = nullptr;
    ASSERT_EQ(window_->SetWindowGravity(WindowGravity::WINDOW_GRAVITY_BOTTOM, 100), WMError::WM_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: SetWindowGravity end";
}

/**
 * @tc.name: GetSubWindow
 * @tc.desc: GetSubWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, GetSubWindow, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: GetSubWindow start";
    window_ = GetTestWindowImpl("GetSubWindow");
    ASSERT_NE(window_, nullptr);
    ASSERT_TRUE(window_->subWindowSessionMap_.empty());
    std::vector<sptr<WindowSessionImpl>> vec;
    vec.push_back(window_);
    int32_t parentId = 111;
    window_->subWindowSessionMap_.insert(std::pair<int32_t, std::vector<sptr<WindowSessionImpl>>>(parentId, vec));
    std::vector<sptr<Window>> subWindows = window_->GetSubWindow(parentId);
    ASSERT_EQ(subWindows.size(), 1);
    GTEST_LOG_(INFO) << "WindowSessionImplTest2: GetSubWindow end";
}

/**
 * @tc.name: SetRestoredRouterStack_0200
 * @tc.desc: basic function test of set or get restored router stack.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(WindowSessionImplTest2, SetRestoredRouterStack_0200, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(option, nullptr);
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    ASSERT_NE(window, nullptr);
    std::string routerStack = "stackInfo:{}";
    EXPECT_EQ(window->SetRestoredRouterStack(routerStack), WMError::WM_OK);
    std::string gettedStack = window->GetRestoredRouterStack();
    EXPECT_EQ(gettedStack, routerStack);
    EXPECT_TRUE(window->GetRestoredRouterStack().empty());
}

/**
 * @tc.name: SetUiDvsyncSwitch
 * @tc.desc: SetUiDvsyncSwitch
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest2, SetUiDvsyncSwitch, Function | SmallTest | Level2) {
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetUiDvsyncSwitch");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    window->SetUiDvsyncSwitch(true);
    window->vsyncStation_ = nullptr;
    window->SetUiDvsyncSwitch(true);
}

/**
 * @tc.name: SetUiDvsyncSwitchSucc
 * @tc.desc: SetUiDvsyncSwitch Test Succ
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionImplTest2, SetUiDvsyncSwitchSucc, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SetUiDvsyncSwitchSucc");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    window->SetUiDvsyncSwitch(true);
    window->SetUiDvsyncSwitch(false);
}

/**
 * @tc.name: SetUiDvsyncSwitchErr
 * @tc.desc: SetUiDvsyncSwitch Test Err
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionImplTest2, SetUiDvsyncSwitchErr, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SetUiDvsyncSwitchErr");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    window->vsyncStation_ = nullptr;
    window->SetUiDvsyncSwitch(true);
    window->SetUiDvsyncSwitch(false);
}

/*
 * @tc.name: SetRestoredRouterStack_0100
 * @tc.desc: basic function test of set or get restored router stack.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(WindowSessionImplTest2, SetRestoredRouterStack_0100, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(option, nullptr);
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    ASSERT_NE(window, nullptr);
    std::string routerStack = "stackInfo:{}";
    EXPECT_EQ(window->SetRestoredRouterStack(routerStack), WMError::WM_OK);
    EXPECT_EQ(window->NapiSetUIContent("info", nullptr, nullptr, BackupAndRestoreType::NONE, nullptr, nullptr),
        WMError::WM_ERROR_INVALID_WINDOW);
}
}
} // namespace Rosen
} // namespace OHOS
