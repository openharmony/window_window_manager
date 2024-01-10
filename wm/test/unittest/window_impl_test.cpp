/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "mock_window_adapter.h"
#include "singleton_mocker.h"
#include "window_impl.h"
#include "mock_uicontent.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
class MockAceAbilityHandler : public IAceAbilityHandler {
public:
    MOCK_METHOD1(SetBackgroundColor, void(uint32_t color));
    MOCK_METHOD0(GetBackgroundColor, uint32_t());
};

class MockAnimationTransitionController : public IAnimationTransitionController {
public:
    MOCK_METHOD0(AnimationForShown, void());
    MOCK_METHOD0(AnimationForHidden, void());
};

class MockDialogDeathRecipientListener : public IDialogDeathRecipientListener {
public:
    MOCK_CONST_METHOD0(OnDialogDeathRecipient, void());
};

class MockScreenshotListener : public IScreenshotListener {
public:
    MOCK_METHOD0(OnScreenshot, void());
};

class MockDialogTargetTouchListener : public IDialogTargetTouchListener {
public:
    MOCK_CONST_METHOD0(OnDialogTargetTouch, void());
};

class MockWindowChangeListener : public IWindowChangeListener {
public:
    MOCK_METHOD3(OnSizeChange, void(Rect rect, WindowSizeChangeReason reason,
        const std::shared_ptr<RSTransaction>& rsTransaction));
    MOCK_METHOD2(OnModeChange, void(WindowMode mode, bool hasDeco));
};

class MockAvoidAreaChangedListener : public IAvoidAreaChangedListener {
public:
    MOCK_METHOD2(OnAvoidAreaChanged, void(const AvoidArea avoidArea, AvoidAreaType type));
};

class MockDisplayMoveListener : public IDisplayMoveListener {
public:
    MOCK_METHOD2(OnDisplayMove, void(DisplayId from, DisplayId to));
};

class MockInputEventConsumer : public IInputEventConsumer {
public:
    MOCK_CONST_METHOD1(OnInputEvent, bool(const std::shared_ptr<MMI::KeyEvent>&));
    MOCK_CONST_METHOD1(OnInputEvent, bool(const std::shared_ptr<MMI::PointerEvent>&));
    MOCK_CONST_METHOD1(OnInputEvent, bool(const std::shared_ptr<MMI::AxisEvent>&));
};

class MockKeyEvent : public MMI::KeyEvent {
public:
    MockKeyEvent() : MMI::KeyEvent(0) {}
};

class MockPointerEvent : public MMI::PointerEvent {
public:
    MockPointerEvent() : MMI::PointerEvent(0) {}
};

class WindowImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    void CreateStretchableWindow(sptr<WindowImpl>& window, const Rect& rect);

    static inline std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_;
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
};
void WindowImplTest::SetUpTestCase()
{
}

void WindowImplTest::TearDownTestCase()
{
}

void WindowImplTest::SetUp()
{
}

void WindowImplTest::TearDown()
{
}

void WindowImplTest::CreateStretchableWindow(sptr<WindowImpl>& window, const Rect& rect)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("StretchableWindowTest");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowRect({ 1, 1, 1, 1 });
    if (option == nullptr) {
        window = nullptr;
        return;
    }
    window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    window->windowSystemConfig_.isStretchable_ = true;
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    window->UpdateRect(rect, true, WindowSizeChangeReason::UNDEFINED);
    ASSERT_EQ(window->GetWindowProperty()->GetOriginRect(), rect);
}

namespace {
/**
 * @tc.name: CreateWindow01
 * @tc.desc: Create window with no parentId
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, CreateWindow01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateWindow01");
    sptr<WindowImpl> window = new WindowImpl(option);

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: CreateWindow02
 * @tc.desc: Create window with no parentId and no abilityContext
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, CreateWindow02, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateWindow02");
    sptr<WindowImpl> window = new WindowImpl(option);

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_SAMGR));
    ASSERT_EQ(WMError::WM_ERROR_SAMGR, window->Create(INVALID_WINDOW_ID));
}

/**
 * @tc.name: CreateWindow03
 * @tc.desc: Create window with illegal parentId
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, CreateWindow03, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateWindow03");
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARENT, window->Create(1234));
}

/**
 * @tc.name: CreateWindow04
 * @tc.desc: Create window with repeated windowName
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, CreateWindow04, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateWindow04");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Create(INVALID_WINDOW_ID);

    sptr<WindowOption> option_other = new WindowOption();
    option_other->SetWindowName("CreateWindow04");
    sptr<WindowImpl> window_other = new WindowImpl(option_other);

    ASSERT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, window_other->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
    ASSERT_EQ(WMError::WM_OK, window_other->Destroy());
}

/**
 * @tc.name: CreateWindow05
 * @tc.desc: Create window with exist parentId
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, CreateWindow05, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateWindow05_parent");
    sptr<WindowImpl> window = new WindowImpl(option);

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    sptr<WindowOption> option2 = new WindowOption();
    option2->SetWindowName("CreateWindow05");
    sptr<WindowImpl> window2 = new WindowImpl(option2);

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window2->Create(window->GetWindowId()));

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
    ASSERT_EQ(WMError::WM_OK, window2->Destroy());
}

/**
 * @tc.name: CreateWindow06
 * @tc.desc: Create window with no default option, get and check Property
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, CreateWindow06, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateWindow06");
    struct Rect rect = {1, 2, 3u, 4u};
    option->SetWindowRect(rect);
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    ASSERT_EQ(1, window->GetRequestRect().posX_);
    ASSERT_EQ(2, window->GetRequestRect().posY_);
    ASSERT_EQ(3u, window->GetRequestRect().width_);
    ASSERT_EQ(4u, window->GetRequestRect().height_);
    ASSERT_EQ(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW, window->GetType());
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetMode());
    ASSERT_EQ("CreateWindow06", window->GetWindowName());

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: FindWindow01
 * @tc.desc: Find one exit window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, FindWindow01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("FindWindow01");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Create(INVALID_WINDOW_ID);
    ASSERT_NE(nullptr, WindowImpl::Find("FindWindow01"));
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: FindWindow02
 * @tc.desc: Add another window, find both two windows
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, FindWindow02, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("FindWindow02");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    option->SetWindowName("FindWindow02_other");
    sptr<WindowImpl> window2 = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window2->Create(INVALID_WINDOW_ID));

    ASSERT_NE(nullptr, WindowImpl::Find("FindWindow02_other"));
    ASSERT_NE(nullptr, WindowImpl::Find("FindWindow02"));

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window2->Destroy());
}

/**
 * @tc.name: FindWindow03
 * @tc.desc: Find one no exit window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, FindWindow03, Function | SmallTest | Level2)
{
    ASSERT_EQ(nullptr, WindowImpl::Find("FindWindow03"));
}

/**
 * @tc.name: FindWindow04
 * @tc.desc: Find window with empty name
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, FindWindow04, Function | SmallTest | Level2)
{
    ASSERT_EQ(nullptr, WindowImpl::Find(""));
}

/**
 * @tc.name: FindWindow05
 * @tc.desc: Find one destroyed window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, FindWindow05, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("FindWindow05");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
    ASSERT_EQ(nullptr, WindowImpl::Find("FindWindow05"));
}

/**
 * @tc.name: FindWindowById01
 * @tc.desc: Find one top window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, FindWindowById01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("FindWindowById01");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    ASSERT_NE(nullptr, window->FindWindowById(window->property_->GetWindowId()));

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetWindowType01
 * @tc.desc: SetWindowType
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetWindowType01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetWindowType01");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    ASSERT_EQ(WMError::WM_OK, window->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW));
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetWindowMode01
 * @tc.desc: SetWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetWindowMode01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetWindowType01");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    ASSERT_EQ(WMError::WM_OK, window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetWindowMode02
 * @tc.desc: Set window mode to split primary
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetWindowMode02, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetMode());
    ASSERT_EQ(WMError::WM_OK, window->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY));
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_PRIMARY, window->GetMode());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetWindowMode03
 * @tc.desc: Set window mode to split secondary
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetWindowMode03, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetMode());
    ASSERT_EQ(WMError::WM_OK, window->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY));
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_SECONDARY, window->GetMode());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetWindowMode04
 * @tc.desc: Set window mode to floating
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetWindowMode04, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetMode());
    ASSERT_EQ(WMError::WM_OK, window->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING));
    ASSERT_EQ(WindowMode::WINDOW_MODE_FLOATING, window->GetMode());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetWindowMode05
 * @tc.desc: Set window mode to pip
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetWindowMode05, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetMode());
    ASSERT_EQ(WMError::WM_OK, window->SetWindowMode(WindowMode::WINDOW_MODE_PIP));
    ASSERT_EQ(WindowMode::WINDOW_MODE_PIP, window->GetMode());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: ShowHideWindow01
 * @tc.desc: Show and hide window with add and remove window ok
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, ShowHideWindow01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("ShowHideWindow01");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    window->NotifyForeground();
    EXPECT_CALL(m->Mock(), RemoveWindow(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Hide());
    window->NotifyBackground();
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: ShowHideWindow02
 * @tc.desc: Show window with add window WM_ERROR_SAMGR
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, ShowHideWindow02, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("ShowHideWindow02");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_SAMGR));
    ASSERT_EQ(WMError::WM_ERROR_SAMGR, window->Show());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: ShowHideWindow03
 * @tc.desc: Show window with add window WM_ERROR_IPC_FAILED
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, ShowHideWindow03, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("ShowHideWindow03");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_IPC_FAILED));
    ASSERT_EQ(WMError::WM_ERROR_IPC_FAILED, window->Show());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: ShowHideWindow04
 * @tc.desc: Show window with add window OK & Hide window with remove window WM_ERROR_SAMGR
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, ShowHideWindow04, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("ShowHideWindow04");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    EXPECT_CALL(m->Mock(), RemoveWindow(_, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_SAMGR));
    ASSERT_EQ(WMError::WM_ERROR_SAMGR, window->Hide());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: ShowHideWindow05
 * @tc.desc: Hide window with remove window WM_ERROR_IPC_FAILED
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, ShowHideWindow05, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("ShowHideWindow05");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    EXPECT_CALL(m->Mock(), RemoveWindow(_, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_IPC_FAILED));
    ASSERT_EQ(WMError::WM_ERROR_IPC_FAILED, window->Hide());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: ShowHideWindow06
 * @tc.desc: Hide window with remove window OK
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, ShowHideWindow06, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("ShowHideWindow06");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    EXPECT_CALL(m->Mock(), RemoveWindow(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Hide());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetSystemBarProperty01
 * @tc.desc: SetSystemBarProperty with default param
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetSystemBarProperty01, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetSystemBarProperty01");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    WindowType type = WindowType::WINDOW_TYPE_STATUS_BAR;
    SystemBarProperty prop;
    ASSERT_EQ(WMError::WM_OK, window->SetSystemBarProperty(type, prop));
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetSystemBarProperty02
 * @tc.desc: SetSystemBarProperty with adapter return WM_ERROR_SAMGR
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetSystemBarProperty02, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetSystemBarProperty02");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Show();
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_SAMGR));
    WindowType type = WindowType::WINDOW_TYPE_STATUS_BAR;
    const SystemBarProperty SYS_BAR_PROP(false, 0xE5222222, 0xE5333333);
    ASSERT_EQ(WMError::WM_ERROR_SAMGR, window->SetSystemBarProperty(type, SYS_BAR_PROP));
    EXPECT_CALL(m->Mock(), RemoveWindow(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Hide();
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetSystemBarProperty03
 * @tc.desc: SetSystemBarProperty to invalid window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetSystemBarProperty03, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetSystemBarProperty03");
    sptr<WindowImpl> window = new WindowImpl(option);
    WindowType type = WindowType::WINDOW_TYPE_STATUS_BAR;
    SystemBarProperty prop;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetSystemBarProperty(type, prop));
}

/**
 * @tc.name: GetSystemBarPropertyByType01
 * @tc.desc: GetSystemBarPropertyByType with exist key
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, GetSystemBarPropertyByType01, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("GetSystemBarPropertyByType01");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    WindowType type = WindowType::WINDOW_TYPE_STATUS_BAR;
    const SystemBarProperty SYS_BAR_PROP(false, 0xE5222222, 0xE5333344);
    ASSERT_EQ(WMError::WM_OK, window->SetSystemBarProperty(type, SYS_BAR_PROP));
    ASSERT_EQ(SYS_BAR_PROP, window->GetSystemBarPropertyByType(type));
    EXPECT_CALL(m->Mock(), RemoveWindow(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Hide();
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetSystemBarPropertyByType02
 * @tc.desc: GetSystemBarPropertyByType with nonexist key
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, GetSystemBarPropertyByType02, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("GetSystemBarPropertyByType02");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Show();
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    const SystemBarProperty SYS_BAR_PROP(false, 0xE5222222, 0xE5333333);
    const SystemBarProperty DEFAULT_PROP;
    ASSERT_EQ(WMError::WM_OK, window->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, SYS_BAR_PROP));
    ASSERT_EQ(DEFAULT_PROP, window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_BAR));
    EXPECT_CALL(m->Mock(), RemoveWindow(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Hide();
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetSystemBarPropertyByType03
 * @tc.desc: GetSystemBarPropertyByType with not systemBar type
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, GetSystemBarPropertyByType03, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("GetSystemBarPropertyByType03");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Show();
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    const SystemBarProperty SYS_BAR_PROP(false, 0xE5222222, 0xE5333366);
    const SystemBarProperty DEFAULT_PROP;
    ASSERT_EQ(WMError::WM_OK, window->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, SYS_BAR_PROP));
    ASSERT_EQ(DEFAULT_PROP, window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW));
    EXPECT_CALL(m->Mock(), RemoveWindow(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Hide();
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Maximize02
 * @tc.desc: Maximize the sub window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Maximize02, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Maximize02");
    sptr<WindowImpl> window = new WindowImpl(option);
    window->Maximize();
    ASSERT_EQ(WindowMode::WINDOW_MODE_UNDEFINED, window->GetMode());
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Show();
    window->Maximize();
    ASSERT_EQ(WindowMode::WINDOW_MODE_FLOATING, window->GetMode());

    EXPECT_CALL(m->Mock(), RemoveWindow(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Hide();
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Recover01
 * @tc.desc: Recover the main window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Recover01, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Recover01");
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Recover());
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Create(INVALID_WINDOW_ID);
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Show();
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Recover();
    ASSERT_EQ(WindowMode::WINDOW_MODE_FLOATING, window->GetMode());
    EXPECT_CALL(m->Mock(), RemoveWindow(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Hide();
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Recover02
 * @tc.desc: Recover the sub window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Recover02, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Recover02");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Create(INVALID_WINDOW_ID);
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Show();
    window->Recover();
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetMode());
    EXPECT_CALL(m->Mock(), RemoveWindow(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Hide();
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Minimize01
 * @tc.desc: Minimize the main window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Minimize01, Function | SmallTest | Level3)
{
    auto option = new WindowOption();
    option->SetWindowName("Minimize01");
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Minimize());
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Create(INVALID_WINDOW_ID);
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Show();
    ASSERT_TRUE((window->GetWindowState() == WindowState::STATE_SHOWN));
    EXPECT_CALL(m->Mock(), RemoveWindow(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Minimize();
    ASSERT_FALSE((window->GetWindowState() == WindowState::STATE_SHOWN));
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_EQ(WMError::WM_OK, window->Minimize());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    EXPECT_CALL(*content, Destroy());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Minimize02
 * @tc.desc: Minimize the sub window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Minimize02, Function | SmallTest | Level3)
{
    auto option = new WindowOption();
    option->SetWindowName("Minimize02");
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Create(INVALID_WINDOW_ID);
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    window->Show();
    ASSERT_TRUE((window->GetWindowState() == WindowState::STATE_SHOWN));
    window->Minimize();
    ASSERT_TRUE((window->GetWindowState() == WindowState::STATE_SHOWN));
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IsSupportWideGamut01
 * @tc.desc: IsSupportWideGamut
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, IsSupportWideGamut01, Function | SmallTest | Level3)
{
    auto option = new WindowOption();
    option->SetWindowName("IsSupportWideGamut01");
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Create(INVALID_WINDOW_ID);
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    ASSERT_TRUE(window->IsSupportWideGamut());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetColorSpace01
 * @tc.desc: SetColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetColorSpace01, Function | SmallTest | Level3)
{
    auto option = new WindowOption();
    option->SetWindowName("SetColorSpace01");
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Create(INVALID_WINDOW_ID);
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    window->SetColorSpace(ColorSpace::COLOR_SPACE_WIDE_GAMUT);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetColorSpace01
 * @tc.desc: GetColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, GetColorSpace01, Function | SmallTest | Level3)
{
    auto option = new WindowOption();
    option->SetWindowName("GetColorSpace01");
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Create(INVALID_WINDOW_ID);
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    window->SetColorSpace(ColorSpace::COLOR_SPACE_DEFAULT);
    ASSERT_EQ(ColorSpace::COLOR_SPACE_DEFAULT, window->GetColorSpace());
    window->SetColorSpace(ColorSpace::COLOR_SPACE_WIDE_GAMUT);
    ASSERT_EQ(ColorSpace::COLOR_SPACE_WIDE_GAMUT, window->GetColorSpace());
    uint32_t invalidColorSpace = 54321u;
    window->SetColorSpace(static_cast<ColorSpace>(invalidColorSpace));
    ASSERT_EQ(ColorSpace::COLOR_SPACE_DEFAULT, window->GetColorSpace());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: StartMove01
 * @tc.desc: start move main fullscreen window, test startMoveFlag
 * @tc.type: FUNC
 * @tc.require: issueI5J8IB
 */
HWTEST_F(WindowImplTest, StartMove01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("StartMove01");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Show();
    window->StartMove();
    ASSERT_FALSE(window->moveDragProperty_->startMoveFlag_);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: StartMove02
 * @tc.desc: start move main fullscreen window, test startMoveFlag
 * @tc.type: FUNC
 * @tc.require: issueI5J8IB
 */
HWTEST_F(WindowImplTest, StartMove02, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("StartMove02");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Show();

    window->StartMove();
    ASSERT_FALSE(window->moveDragProperty_->startMoveFlag_);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: StartMove03
 * @tc.desc: start move divider, test startMoveFlag
 * @tc.type: FUNC
 * @tc.require: issueI5J8IB
 */
HWTEST_F(WindowImplTest, StartMove03, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("StartMove03");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowType(WindowType::WINDOW_TYPE_DOCK_SLICE);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Show();
    window->StartMove();
    ASSERT_FALSE(window->moveDragProperty_->startMoveFlag_);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetBackgroundColor01
 * @tc.desc: test SetBackgroundColor withow uiContent
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetBackgroundColor01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetBackgroundColor01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Show();
    ASSERT_FALSE(window->IsTransparent());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetBackgroundColor("#000"));
    ASSERT_FALSE(window->IsTransparent());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_OPERATION, window->SetBackgroundColor("#00FF00"));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_OPERATION, window->SetBackgroundColor("#FF00FF00"));
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetTurnScreenOn01
 * @tc.desc: create window but not show, test SetTurnScreenOn
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetTurnScreenOn01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetTurnScreenOn01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetTurnScreenOn(true));
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    ASSERT_FALSE(window->IsTurnScreenOn());
    ASSERT_EQ(WMError::WM_OK, window->SetTurnScreenOn(true));
    ASSERT_TRUE(window->IsTurnScreenOn());
    ASSERT_EQ(WMError::WM_OK, window->SetTurnScreenOn(false));
    ASSERT_FALSE(window->IsTurnScreenOn());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}


/**
 * @tc.name: SetTurnScreenOn02
 * @tc.desc: create window with show, test SetTurnScreenOn
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetTurnScreenOn02, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetTurnScreenOn02");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_FALSE(window->IsTurnScreenOn());
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(2).WillOnce(Return(WMError::WM_OK))
        .WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->SetTurnScreenOn(true));
    ASSERT_TRUE(window->IsTurnScreenOn());
    ASSERT_EQ(WMError::WM_OK, window->SetTurnScreenOn(false));
    ASSERT_FALSE(window->IsTurnScreenOn());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetKeepScreenOn01
 * @tc.desc: create window but not show, test SetKeepScreenOn
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetKeepScreenOn01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetKeepScreenOn01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetKeepScreenOn(true));
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    ASSERT_FALSE(window->IsKeepScreenOn());
    ASSERT_EQ(WMError::WM_OK, window->SetKeepScreenOn(true));
    ASSERT_TRUE(window->IsKeepScreenOn());
    ASSERT_EQ(WMError::WM_OK, window->SetKeepScreenOn(false));
    ASSERT_FALSE(window->IsKeepScreenOn());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetKeepScreenOn02
 * @tc.desc: create window with show, test SetKeepScreenOn
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetKeepScreenOn02, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetKeepScreenOn02");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_FALSE(window->IsKeepScreenOn());
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(2).WillOnce(Return(WMError::WM_OK))
        .WillOnce(Return(WMError::WM_OK));;
    ASSERT_EQ(WMError::WM_OK, window->SetKeepScreenOn(true));
    ASSERT_TRUE(window->IsKeepScreenOn());
    ASSERT_EQ(WMError::WM_OK, window->SetKeepScreenOn(false));
    ASSERT_FALSE(window->IsKeepScreenOn());

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetBrightness01
 * @tc.desc: test SetBrightness with invalid brightness
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetBrightness01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetBrightness01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetBrightness(0.f));
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    ASSERT_EQ(WMError::WM_OK, window->SetBrightness(MAXIMUM_BRIGHTNESS));

    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_EQ(MAXIMUM_BRIGHTNESS, window->GetBrightness());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetBrightness(2.0f)); // 2.0f: brightness
    ASSERT_EQ(MAXIMUM_BRIGHTNESS, window->GetBrightness());

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetBrightness02
 * @tc.desc: test SetBrightness with valid brightness
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetBrightness02, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetBrightness02");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_EQ(UNDEFINED_BRIGHTNESS, window->GetBrightness());
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(2).WillOnce(Return(WMError::WM_OK))
        .WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->SetBrightness(MAXIMUM_BRIGHTNESS));
    ASSERT_EQ(MAXIMUM_BRIGHTNESS, window->GetBrightness());
    ASSERT_EQ(WMError::WM_OK, window->SetBrightness(MINIMUM_BRIGHTNESS));
    ASSERT_EQ(MINIMUM_BRIGHTNESS, window->GetBrightness());

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetBrightness03
 * @tc.desc: test SetBrightness with invalid type window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetBrightness03, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetBrightness03");
    option->SetWindowType(WindowType::WINDOW_TYPE_STATUS_BAR);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_EQ(UNDEFINED_BRIGHTNESS, window->GetBrightness());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_TYPE, window->SetBrightness(MAXIMUM_BRIGHTNESS));
    ASSERT_EQ(UNDEFINED_BRIGHTNESS, window->GetBrightness());

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetFocusable01
 * @tc.desc: create window but not show, test SetFocusable
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetFocusable01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetFocusable01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->RequestFocus());
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    ASSERT_TRUE(window->GetFocusable());
    EXPECT_CALL(m->Mock(), RequestFocus(_)).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->RequestFocus());
    ASSERT_EQ(WMError::WM_OK, window->SetFocusable(false));
    ASSERT_FALSE(window->GetFocusable());
    ASSERT_EQ(WMError::WM_OK, window->SetFocusable(true));
    ASSERT_TRUE(window->GetFocusable());
    WindowState state = window->GetWindowState();
    window->SetWindowState(WindowState::STATE_DESTROYED);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetFocusable(false));
    ASSERT_TRUE(window->GetFocusable());
    window->SetWindowState(state);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetFocusable02
 * @tc.desc: create window with show, test SetFocusable
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetFocusable02, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetFocusable02");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_TRUE(window->GetFocusable());
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(2).WillOnce(Return(WMError::WM_OK))
        .WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->SetFocusable(false));
    ASSERT_FALSE(window->GetFocusable());
    ASSERT_EQ(WMError::WM_OK, window->SetFocusable(true));
    ASSERT_TRUE(window->GetFocusable());

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetTouchable01
 * @tc.desc: create window but not show, test SetTouchable
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetTouchable01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetTouchable01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    ASSERT_TRUE(window->GetTouchable());
    ASSERT_EQ(WMError::WM_OK, window->SetTouchable(false));
    ASSERT_FALSE(window->GetTouchable());
    ASSERT_EQ(WMError::WM_OK, window->SetTouchable(true));
    ASSERT_TRUE(window->GetTouchable());
    WindowState state = window->GetWindowState();
    window->SetWindowState(WindowState::STATE_DESTROYED);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetTouchable(false));
    ASSERT_TRUE(window->GetTouchable());
    window->SetWindowState(state);

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetTouchable02
 * @tc.desc: create window with show, test SetTouchable
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetTouchable02, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetTouchable02");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_TRUE(window->GetTouchable());
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(2).WillOnce(Return(WMError::WM_OK))
        .WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->SetTouchable(false));
    ASSERT_FALSE(window->GetTouchable());
    ASSERT_EQ(WMError::WM_OK, window->SetTouchable(true));
    ASSERT_TRUE(window->GetTouchable());

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: DisableAppWindowDecor01
 * @tc.desc: disable app window decor
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, DisableAppWindowDecor01, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("DisableAppWindowDecor01");
    sptr<WindowImpl> window = new WindowImpl(option);

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    window->property_->isDecorEnable_ = true;
    window->windowSystemConfig_.isSystemDecorEnable_ = false;
    window->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    window->DisableAppWindowDecor();
    ASSERT_FALSE(window->IsDecorEnable());
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->DisableAppWindowDecor();
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_FALSE(window->IsDecorEnable());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetTouchHotAreas01
 * @tc.desc: create window with show, test SetTouchHotAreas
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetTouchHotAreas01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetTouchHotAreas01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());

    std::vector<Rect> requestedTouchHotAreas;
    window->GetRequestedTouchHotAreas(requestedTouchHotAreas);
    ASSERT_TRUE(requestedTouchHotAreas.empty());

    std::vector<Rect> rects;
    rects.emplace_back(Rect{ 0, 0, 720, 400 });
    rects.emplace_back(Rect{ 0, 800, 720, 300 });
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->SetTouchHotAreas(rects));
    window->GetRequestedTouchHotAreas(requestedTouchHotAreas);
    ASSERT_EQ(rects.size(), requestedTouchHotAreas.size());
    for (uint32_t i = 0; i < rects.size(); ++i) {
        ASSERT_TRUE(rects[i] == requestedTouchHotAreas[i]);
    }

    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    rects.clear();
    ASSERT_EQ(WMError::WM_OK, window->SetTouchHotAreas(rects));
    window->GetRequestedTouchHotAreas(requestedTouchHotAreas);
    ASSERT_TRUE(requestedTouchHotAreas.empty());

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetTransform01
 * @tc.desc: set transform
 * @tc.type: FUNC
 * @tc.require: issueI5NDLK
 */
HWTEST_F(WindowImplTest, SetTransform01, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetTransform01");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    window->DisableAppWindowDecor();
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_SAMGR));
    Transform trans_;
    window->UpdateZoomTransform(trans_, true);
    window->SetTransform(trans_);
    window->UpdateZoomTransform(trans_, false);
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_SAMGR));
    window->SetTransform(trans_);
    ASSERT_TRUE(trans_ == window->GetTransform());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetTransform02
 * @tc.desc: set transform and getTransform
 * @tc.type: FUNC
 * @tc.require: issueI5NDLK
 */
HWTEST_F(WindowImplTest, SetTransform02, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetTransform02");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    window->DisableAppWindowDecor();
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_SAMGR));
    Transform trans_;
    trans_.pivotX_ = 1.0f;
    trans_.pivotY_ = 0.6f;
    window->SetTransform(trans_);
    ASSERT_TRUE(trans_ != window->GetTransform());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetTransform03
 * @tc.desc: set transform and getTransform
 * @tc.type: FUNC
 * @tc.require: issueI5NDLK
 */
HWTEST_F(WindowImplTest, SetTransform03, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetTransform03");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    window->DisableAppWindowDecor();
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    Transform trans_;
    trans_.pivotX_ = 1.0f;
    trans_.pivotY_ = 0.6f;
    window->SetTransform(trans_);
    ASSERT_TRUE(trans_ == window->GetTransform());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetTransform04
 * @tc.desc: set transform and getTransform
 * @tc.type: FUNC
 * @tc.require: issueI5NDLK
 */
HWTEST_F(WindowImplTest, SetTransform04, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetTransform04");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    window->DisableAppWindowDecor();
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    Transform trans_;
    trans_.pivotX_ = 1.0f;
    trans_.pivotY_ = 0.6f;
    Transform defaultTrans_;
    window->SetTransform(trans_);
    ASSERT_TRUE(defaultTrans_ != window->GetTransform());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetAPPWindowLabel
 * @tc.desc: set window label to ace
 * @tc.type: FUNC
 * @tc.require: issueI5Q2KW
 */
HWTEST_F(WindowImplTest, SetAPPWindowLabel, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetAPPWindowLabel");
    sptr<WindowImpl> window = new WindowImpl(option);
    std::string label = "openharmony";

    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    EXPECT_CALL(*content, SetAppWindowTitle(_));
    ASSERT_EQ(WMError::WM_OK, window->SetAPPWindowLabel(label));
    window->uiContent_ = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->SetAPPWindowLabel(label));
}

/**
 * @tc.name: SetAPPWindowIcon
 * @tc.desc: set window Icon to ace
 * @tc.type: FUNC
 * @tc.require: issueI5Q2KW
 */
HWTEST_F(WindowImplTest, SetAPPWindowIcon, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetAPPWindowIcon");
    sptr<WindowImpl> window = new WindowImpl(option);

    std::shared_ptr<Media::PixelMap> icon1(nullptr);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->SetAPPWindowIcon(icon1));
    std::shared_ptr<Media::PixelMap> icon2 = std::make_shared<Media::PixelMap>();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->SetAPPWindowIcon(icon2));
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    EXPECT_CALL(*content, SetAppWindowIcon(_));
    ASSERT_EQ(WMError::WM_OK, window->SetAPPWindowIcon(icon2));
}

/**
 * @tc.name: NotifyMemoryLevel01
 * @tc.desc: NotifyMemoryLevel without mainWindow
 * @tc.type: FUNC
 * @tc.require: issueI5JQ04
 */
HWTEST_F(WindowImplTest, NotifyMemoryLevel01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("NotifyMemoryLevel01");
    int32_t level = 0;
    sptr<WindowImpl> window = new WindowImpl(option);

    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->NotifyMemoryLevel(level));
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    EXPECT_CALL(*content, NotifyMemoryLevel(_));
    ASSERT_EQ(WMError::WM_OK, window->NotifyMemoryLevel(level));
}

/**
 * @tc.name: StretchableUpdateRectDragStartTest
 * @tc.desc: UpdateRect test for stretchable window when drag start.
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, StretchableUpdateRectDragStartTest, Function | SmallTest | Level3)
{
    Rect rect1 { 10, 10, 10, 10 };
    sptr<WindowImpl> window;
    CreateStretchableWindow(window, rect1);
    ASSERT_NE(window, nullptr);
    Rect rect2 { 100, 100, 100, 100 };
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    EXPECT_CALL(*content, UpdateViewportConfig(_, _, _));
    window->UpdateRect(rect2, true, WindowSizeChangeReason::DRAG_START);
    ASSERT_EQ(window->GetWindowProperty()->GetOriginRect(), rect1);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*content, Destroy());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: StretchableUpdateRectDragTest
 * @tc.desc: UpdateRect test for stretchable window when drag.
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, StretchableUpdateRectDragTest, Function | SmallTest | Level3)
{
    Rect rect1 { 10, 10, 10, 10 };
    sptr<WindowImpl> window;
    CreateStretchableWindow(window, rect1);
    ASSERT_NE(window, nullptr);
    Rect rect2 { 100, 100, 100, 100 };
    window->UpdateRect(rect2, true, WindowSizeChangeReason::DRAG);
    ASSERT_EQ(window->GetWindowProperty()->GetOriginRect(), rect1);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: StretchableUpdateRectDragEndTest
 * @tc.desc: UpdateRect test for stretchable window when drag end.
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, StretchableUpdateRectDragEndTest, Function | SmallTest | Level3)
{
    Rect rect1 { 10, 10, 10, 10 };
    sptr<WindowImpl> window;
    CreateStretchableWindow(window, rect1);
    ASSERT_NE(window, nullptr);
    Rect rect2 { 100, 100, 100, 100 };
    window->UpdateRect(rect2, true, WindowSizeChangeReason::DRAG_END);
    ASSERT_EQ(window->GetWindowProperty()->GetOriginRect(), rect1);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: StretchableUpdateRectRecoverTest
 * @tc.desc: UpdateRect test for stretchable window when recover.
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, StretchableUpdateRectRecoverTest, Function | SmallTest | Level3)
{
    Rect rect1 { 10, 10, 10, 10 };
    sptr<WindowImpl> window;
    CreateStretchableWindow(window, rect1);
    ASSERT_NE(window, nullptr);
    Rect rect2 { 100, 100, 100, 100 };
    window->UpdateRect(rect2, true, WindowSizeChangeReason::RECOVER);
    ASSERT_EQ(window->GetWindowProperty()->GetOriginRect(), rect1);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: StretchableUpdateRectRecoverTest
 * @tc.desc: UpdateRect test for stretchable window when move.
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, StretchableUpdateRectMoveTest, Function | SmallTest | Level3)
{
    Rect rect1 { 10, 10, 10, 10 };
    sptr<WindowImpl> window;
    CreateStretchableWindow(window, rect1);
    ASSERT_NE(window, nullptr);
    Rect rect2 { 100, 100, 100, 100 };
    window->UpdateRect(rect2, true, WindowSizeChangeReason::MOVE);
    ASSERT_EQ(window->GetWindowProperty()->GetOriginRect(), rect1);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: StretchableUpdateRectResizeTest
 * @tc.desc: UpdateRect test for stretchable window when resize.
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, StretchableUpdateRectResizeTest, Function | SmallTest | Level3)
{
    Rect rect1 { 110, 110, 10, 10 };
    sptr<WindowImpl> window;
    CreateStretchableWindow(window, rect1);
    ASSERT_NE(window, nullptr);
    Rect rect2 { 100, 100, 100, 100 };
    ASSERT_EQ(true, rect1.IsInsideOf(rect2));
    ASSERT_EQ(true, rect1 != rect2);
    window->UpdateRect(rect2, true, WindowSizeChangeReason::RESIZE);
    ASSERT_EQ(window->GetWindowProperty()->GetOriginRect(), rect2);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: PrivacyMode01
 * @tc.desc: Set window privacy mode
 * @tc.type: FUNC
 * @tc.require: issueI5MYNX
 */
HWTEST_F(WindowImplTest, PrivacyMode01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("PrivacyMode01");
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(nullptr, window);

    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(8).WillRepeatedly(Return(WMError::WM_OK));

    window->SetPrivacyMode(true);
    window->SetSystemPrivacyMode(true);
    ASSERT_EQ(true, window->IsPrivacyMode());

    window->SetPrivacyMode(true);
    window->SetSystemPrivacyMode(false);
    ASSERT_EQ(true, window->IsPrivacyMode());

    window->SetPrivacyMode(false);
    window->SetSystemPrivacyMode(true);
    ASSERT_EQ(false, window->IsPrivacyMode());

    window->SetPrivacyMode(false);
    window->SetSystemPrivacyMode(false);
    ASSERT_EQ(false, window->IsPrivacyMode());
}

/**
 * @tc.name: CalculatePointerDirection
 * @tc.desc: calculate mouse style id
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, CalculatePointerDirection, Function | SmallTest | Level3)
{
    Rect rect1 { 1, 1, 100, 100 };
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CalculatePointerDirection");
    sptr<WindowImpl> window = new WindowImpl(option);

    window->moveDragProperty_->startRectExceptCorner_ = rect1;
    ASSERT_EQ(12, window->CalculatePointerDirection(0, 0));
    ASSERT_EQ(6, window->CalculatePointerDirection(50, 0));
    ASSERT_EQ(11, window->CalculatePointerDirection(102, 0));
    ASSERT_EQ(5, window->CalculatePointerDirection(102, 50));
}

 /*
 * @tc.name: FindWindowById
 * @tc.desc: FindWindowById Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, FindWindowById, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("FindWindowById");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    ASSERT_EQ(nullptr, window->FindWindowById(INVALID_WINDOW_ID));
    ASSERT_EQ(sptr<Window>(window), window->FindWindowById(window->GetWindowId()));

    ASSERT_EQ(nullptr, sptr<Window>(window)->GetTopWindowWithId(INVALID_WINDOW_ID));

    ASSERT_EQ(nullptr, sptr<Window>(window)->GetTopWindowWithId(INVALID_WINDOW_ID));
    std::shared_ptr<AbilityRuntime::Context> context;
    ASSERT_EQ(nullptr, sptr<Window>(window)->GetTopWindowWithContext(context));
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

 /*
 * @tc.name: GetSubWindow
 * @tc.desc: GetSubWindow Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, GetSubWindow, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("main");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    option = new WindowOption();
    option->SetWindowName("sub");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowImpl> subWindow = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, subWindow->Create(window->GetWindowId()));

    auto subWindowVec = sptr<Window>(window)->GetSubWindow(window->GetWindowId());
    if (subWindowVec.size() == 1) {
        ASSERT_EQ(1, subWindowVec.size());
        ASSERT_EQ(sptr<Window>(subWindow), subWindowVec[0]);
    }
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

 /*
 * @tc.name: UpdateConfigurationForAll
 * @tc.desc: UpdateConfigurationForAll Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, UpdateConfigurationForAll, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    std::shared_ptr<AppExecFwk::Configuration> configuration;
    sptr<Window>(window)->UpdateConfigurationForAll(configuration);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

 /*
 * @tc.name: SetAlpha
 * @tc.desc: SetAlpha | GetAlpha
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetAlpha, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowImpl> window = new WindowImpl(option);

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    float alpha = 0.5f;
    window->SetAlpha(alpha);
    ASSERT_EQ(alpha, window->GetAlpha());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

 /*
 * @tc.name: GetAvoidAreaByType
 * @tc.desc: Test GetAvoidAreaByType when ret is not ok
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, GetAvoidAreaByType, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowImpl> window = new WindowImpl(option);

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), GetAvoidAreaByType(_, _, _)).Times(1).WillOnce(Return(WMError::WM_DO_NOTHING));
    AvoidArea avoidarea;
    ASSERT_EQ(WMError::WM_DO_NOTHING, window->GetAvoidAreaByType(AvoidAreaType::TYPE_CUTOUT, avoidarea));
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: SetWindowType
 * @tc.desc: SetWindowType Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetWindowType, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowImpl> window = new WindowImpl(option);

    window->SetWindowState(WindowState::STATE_DESTROYED);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW));

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetWindowType(WindowType::SYSTEM_WINDOW_END));

    window->SetWindowState(WindowState::STATE_FROZEN);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW));

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: SetWindowMode
 * @tc.desc: SetWindowMode Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetWindowMode, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);

    window->SetWindowState(WindowState::STATE_DESTROYED);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    EXPECT_CALL(*content, HideWindowTitleButton(_, _, _));
    EXPECT_CALL(*content, UpdateWindowMode(_, _));
    ASSERT_EQ(WMError::WM_OK, window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
    window->uiContent_ = nullptr;

    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    window->UpdateModeSupportInfo(0);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW_MODE_OR_SIZE,
        window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
    window->UpdateModeSupportInfo(WindowModeSupport::WINDOW_MODE_SUPPORT_ALL);
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_DO_NOTHING));
    ASSERT_EQ(WMError::WM_DO_NOTHING, window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
    window->SetWindowState(WindowState::STATE_FROZEN);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY));

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: WindowFlag
 * @tc.desc: GetWindowFlags | SetWindowFlags | AddWindowFlag | RemoveWindowFlag
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, WindowFlag, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowFlag");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetWindowFlags(0));
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->AddWindowFlag(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED));
    ASSERT_EQ(static_cast<uint32_t>(0), window->GetWindowFlags());
    ASSERT_EQ(WindowState::STATE_SHOWN, window->GetWindowState());
    ASSERT_EQ(WMError::WM_OK, window->SetWindowFlags(static_cast<uint32_t>(0)));
    ASSERT_EQ(static_cast<uint32_t>(0), window->GetWindowFlags());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: OnNewWant
 * @tc.desc: OnNewWant test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, OnNewWant, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("OnNewWant");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetWindowFlags(0));
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    AAFwk::Want want;
    EXPECT_CALL(*content, OnNewWant(_)).Times(1).WillOnce(Return());
    window->OnNewWant(want);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*content, Destroy());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: GetContentInfo
 * @tc.desc: GetContentInfo test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, GetContentInfo, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("GetContentInfo");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetWindowFlags(0));
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    ASSERT_EQ(std::string(""), window->GetContentInfo());
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    EXPECT_CALL(*content, GetContentInfo()).Times(1).WillOnce(Return("info"));
    ASSERT_EQ(std::string("info"), window->GetContentInfo());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*content, Destroy());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: Snapshot
 * @tc.desc: Snapshot test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Snapshot, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Snapshot");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetWindowFlags(0));
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());

    ASSERT_EQ(nullptr, window->Snapshot());

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: DumpInfo
 * @tc.desc: DumpInfo test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, DumpInfo, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("DumpInfo");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetWindowFlags(0));
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    std::vector<std::string> params{"-h"};
    std::vector<std::string> info{""};
    window->DumpInfo(params, info);
    params.push_back("");
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    EXPECT_CALL(*content, DumpInfo(_, _)).Times(1).WillOnce(Return());
    window->DumpInfo(params, info);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*content, Destroy());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: SetLayoutFullScreen
 * @tc.desc: SetLayoutFullScreen test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetLayoutFullScreen, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetLayoutFullScreen");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetWindowFlags(0));
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    window->UpdateModeSupportInfo(0);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetLayoutFullScreen(true));
    window->UpdateModeSupportInfo(WindowModeSupport::WINDOW_MODE_SUPPORT_ALL);

    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());

    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_DO_NOTHING));
    ASSERT_EQ(WMError::WM_DO_NOTHING, window->SetLayoutFullScreen(true));

    window->property_->SetWindowFlags(window->property_->GetWindowFlags() |
        (static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID)));
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(2)
        .WillOnce(Return(WMError::WM_OK))
        .WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->SetLayoutFullScreen(true));

    window->property_->SetWindowFlags(window->property_->GetWindowFlags() |
        (static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID)));
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(2)
        .WillOnce(Return(WMError::WM_OK))
        .WillOnce(Return(WMError::WM_DO_NOTHING));
    ASSERT_EQ(WMError::WM_DO_NOTHING, window->SetLayoutFullScreen(true));

    window->property_->SetWindowFlags(window->property_->GetWindowFlags() &
        (~static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID)));
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(2)
        .WillOnce(Return(WMError::WM_OK))
        .WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->SetLayoutFullScreen(false));

    window->property_->SetWindowFlags(window->property_->GetWindowFlags() &
        (~static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID)));
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(2)
        .WillOnce(Return(WMError::WM_OK))
        .WillOnce(Return(WMError::WM_DO_NOTHING));
    ASSERT_EQ(WMError::WM_DO_NOTHING, window->SetLayoutFullScreen(false));

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: SetFullScreen
 * @tc.desc: SetFullScreen test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetFullScreen, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetFullScreen");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetWindowFlags(0));
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    window->UpdateModeSupportInfo(0);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetFullScreen(true));
    window->UpdateModeSupportInfo(WindowModeSupport::WINDOW_MODE_SUPPORT_ALL);

    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());

    window->property_->sysBarPropMap_[WindowType::WINDOW_TYPE_STATUS_BAR].enable_ = true;
    window->property_->sysBarPropMap_[WindowType::WINDOW_TYPE_NAVIGATION_BAR].enable_ = true;
    window->property_->SetWindowFlags(window->property_->GetWindowFlags() |
        (static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID)));
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(3).WillRepeatedly(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->SetFullScreen(true));

    window->property_->sysBarPropMap_[WindowType::WINDOW_TYPE_STATUS_BAR].enable_ = false;
    window->property_->sysBarPropMap_[WindowType::WINDOW_TYPE_NAVIGATION_BAR].enable_ = false;
    window->property_->SetWindowFlags(window->property_->GetWindowFlags() &
        (~static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID)));
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(3)
        .WillRepeatedly(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->SetFullScreen(false));

    window->property_->sysBarPropMap_[WindowType::WINDOW_TYPE_STATUS_BAR].enable_ = true;
    window->property_->sysBarPropMap_[WindowType::WINDOW_TYPE_NAVIGATION_BAR].enable_ = true;
    window->property_->SetWindowFlags(window->property_->GetWindowFlags() |
        (static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID)));
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(2)
        .WillOnce(Return(WMError::WM_OK))
        .WillOnce(Return(WMError::WM_DO_NOTHING));
    ASSERT_EQ(WMError::WM_DO_NOTHING, window->SetFullScreen(true));

    window->property_->sysBarPropMap_[WindowType::WINDOW_TYPE_STATUS_BAR].enable_ = true;
    window->property_->sysBarPropMap_[WindowType::WINDOW_TYPE_NAVIGATION_BAR].enable_ = true;
    window->property_->SetWindowFlags(window->property_->GetWindowFlags() |
        (static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID)));
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(3)
        .WillOnce(Return(WMError::WM_DO_NOTHING))
        .WillOnce(Return(WMError::WM_OK))
        .WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->SetFullScreen(true));

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: UpdateTitleButtonVisibility
 * @tc.desc: UpdateTitleButtonVisibility test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, UpdateTitleButtonVisibility, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("BindDialogTarget");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetWindowFlags(0));
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    EXPECT_CALL(*content, HideWindowTitleButton(_, _, _));
    window->UpdateTitleButtonVisibility();

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*content, Destroy());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: WindowCreateCheck
 * @tc.desc: WindowCreateCheck test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, WindowCreateCheck, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW);
    sptr<WindowImpl> window = new WindowImpl(option);

    ASSERT_EQ(WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW, window->GetType());
    ASSERT_NE(WMError::WM_OK, window->WindowCreateCheck(INVALID_WINDOW_ID));
    ASSERT_NE(WMError::WM_OK, window->WindowCreateCheck(static_cast<uint32_t>(-1)));
}

/*
 * @tc.name: BindDialogTarget
 * @tc.desc: BindDialogTarget test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, BindDialogTarget, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("BindDialogTarget");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);

    window->SetWindowState(WindowState::STATE_DESTROYED);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    EXPECT_CALL(m->Mock(), BindDialogTarget(_, _)).Times(2)
        .WillOnce(Return(WMError::WM_OK))
        .WillOnce(Return(WMError::WM_DO_NOTHING));
    sptr<IRemoteObject> targetToken;
    ASSERT_EQ(WMError::WM_OK, window->BindDialogTarget(targetToken));
    ASSERT_EQ(WMError::WM_DO_NOTHING, window->BindDialogTarget(targetToken));

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: Destroy
 * @tc.desc: Destroy test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Destroy, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Destroy");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_DO_NOTHING));
    ASSERT_EQ(WMError::WM_DO_NOTHING, window->Destroy());

    window->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ASSERT_EQ(WindowType::WINDOW_TYPE_DIALOG, window->GetType());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_DO_NOTHING));
    ASSERT_EQ(WMError::WM_DO_NOTHING, window->Destroy());
}

/*
 * @tc.name: UpdateSurfaceNodeAfterCustomAnimation
 * @tc.desc: UpdateSurfaceNodeAfterCustomAnimation test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, UpdateSurfaceNodeAfterCustomAnimation, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("UpdateSurfaceNodeAfterCustomAnimation");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->UpdateSurfaceNodeAfterCustomAnimation(true));

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_OPERATION, window->UpdateSurfaceNodeAfterCustomAnimation(true));

    window->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).WillOnce(Return(WMError::WM_DO_NOTHING));
    ASSERT_EQ(WMError::WM_DO_NOTHING, window->UpdateSurfaceNodeAfterCustomAnimation(true));

    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), UpdateRsTree(_, _)).WillOnce(Return(WMError::WM_DO_NOTHING));
    ASSERT_EQ(WMError::WM_DO_NOTHING, window->UpdateSurfaceNodeAfterCustomAnimation(true));

    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), UpdateRsTree(_, _)).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->UpdateSurfaceNodeAfterCustomAnimation(true));

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: ShowHide
 * @tc.desc: Show and Hide test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, ShowHide, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("ShowHide");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Show());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Hide());
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    auto state = window->GetWindowState();
    window->SetWindowState(WindowState::STATE_FROZEN);
    ASSERT_EQ(WindowState::STATE_FROZEN, window->GetWindowState());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_OPERATION, window->PreProcessShow(0, false));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_OPERATION, window->Show());

    window->SetWindowState(state);
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_EQ(WindowState::STATE_SHOWN, window->GetWindowState());

    ASSERT_EQ(WMError::WM_OK, window->Show(static_cast<uint32_t>(WindowStateChangeReason::KEYGUARD)));
    EXPECT_CALL(m->Mock(), ProcessPointDown(_, _));
    ASSERT_EQ(WMError::WM_OK, window->Show());

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_DESKTOP);
    ASSERT_EQ(WindowType::WINDOW_TYPE_DESKTOP, window->GetType());
    EXPECT_CALL(m->Mock(), MinimizeAllAppWindows(_));
    ASSERT_EQ(WMError::WM_OK, window->Show());

    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).WillOnce(Return(WMError::WM_DO_NOTHING));
    ASSERT_EQ(WMError::WM_DO_NOTHING, window->Hide());
    ASSERT_EQ(WindowState::STATE_SHOWN, window->GetWindowState());

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);

    ASSERT_EQ(WMError::WM_OK, window->Hide(static_cast<uint32_t>(WindowStateChangeReason::TOGGLING)));
    ASSERT_EQ(WindowState::STATE_HIDDEN, window->GetWindowState());

    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_INVALID_WINDOW_MODE_OR_SIZE));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW_MODE_OR_SIZE, window->Show());
    ASSERT_EQ(WindowState::STATE_HIDDEN, window->GetWindowState());

    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_DO_NOTHING));
    ASSERT_EQ(WMError::WM_DO_NOTHING, window->Show());
    ASSERT_EQ(WindowState::STATE_HIDDEN, window->GetWindowState());

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: CloseWindow
 * @tc.desc: CloseWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, CloseWindow, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CloseWindow");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_EQ(WMError::WM_OK, window->Close());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: PendingCloseWindow
 * @tc.desc: PendingCloseWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, PendingCloseWindow, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowImpl> window = new WindowImpl(option);
    window->PendingClose();
    ASSERT_EQ(nullptr, window->context_);
}

/*
 * @tc.name: InvalidWindow
 * @tc.desc: InvalidWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, InvalidWindow, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->MoveTo(0, 0));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Resize(0, 0));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetCallingWindow(INVALID_WINDOW_ID));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetBackgroundColor(std::string("???")));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Close());
}

/*
 * @tc.name: BackgroundColor
 * @tc.desc: GetBackgroundColor | SetBackGroundColor test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, BackgroundColor, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);

    MockAceAbilityHandler* aceAbilityHandler = new MockAceAbilityHandler();
    window->aceAbilityHandler_ = aceAbilityHandler;
    EXPECT_CALL(*aceAbilityHandler, SetBackgroundColor(_));
    ASSERT_EQ(WMError::WM_OK, window->SetBackgroundColor(0));
    EXPECT_CALL(*aceAbilityHandler, GetBackgroundColor()).WillOnce(Return(0));
    ASSERT_EQ(0, window->GetBackgroundColor());

    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    EXPECT_CALL(*content, SetBackgroundColor(_));
    ASSERT_EQ(WMError::WM_OK, window->SetBackgroundColor(0));
    EXPECT_CALL(*content, GetBackgroundColor()).WillOnce(Return(0));
    ASSERT_EQ(0, window->GetBackgroundColor());
}

/*
 * @tc.name: SetTransparent
 * @tc.desc: SetTransparent test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetTransparent, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetTransparent");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetTransparent(true));
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    ASSERT_EQ(WMError::WM_ERROR_INVALID_OPERATION, window->SetTransparent(true));
    ASSERT_EQ(WMError::WM_OK, window->SetTransparent(false));

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: TransferLifeCycleEventToString
 * @tc.desc: TransferLifeCycleEventToString test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, TransferLifeCycleEventToString, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(std::string("DESTROY"), window->TransferLifeCycleEventToString(LifeCycleEvent::DESTROY_EVENT));
    ASSERT_EQ(std::string("UNDEFINE"), window->TransferLifeCycleEventToString(
        static_cast<LifeCycleEvent>(uint32_t(-1))));
}

/*
 * @tc.name: NotifyWindowTransition
 * @tc.desc: NotifyWindowTransition test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, NotifyWindowTransition, Function | SmallTest | Level3)
{
    NotifyNativeWinDestroyFunc func = [](std::string) {};
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("MoveDrag");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    window->RegisterWindowDestroyedListener(func);
    window->RestoreSplitWindowMode(0u);

    ASSERT_EQ(WMError::WM_ERROR_NO_MEM, window->NotifyWindowTransition(TransitionReason::ABILITY_TRANSITION));
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Close());
}

/*
 * @tc.name: RegisterAnimationTransitionController
 * @tc.desc: RegisterAnimationTransitionController test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, RegisterAnimationTransitionController, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    sptr<IAnimationTransitionController> listener;
    ASSERT_EQ(nullptr, listener);
    window->RegisterAnimationTransitionController(listener);
    listener = new MockAnimationTransitionController();
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    EXPECT_CALL(*content, SetNextFrameLayoutCallback(_));
    window->RegisterAnimationTransitionController(listener);
    window->property_ = new WindowProperty();
    EXPECT_CALL(*content, SetNextFrameLayoutCallback(_));
    window->RegisterAnimationTransitionController(listener);
    window->property_->SetAnimationFlag(static_cast<uint32_t>(WindowAnimation::CUSTOM));
    EXPECT_CALL(*content, SetNextFrameLayoutCallback(_));
    window->RegisterAnimationTransitionController(listener);
}

/*
 * @tc.name: RegisterDialogDeathRecipientListener
 * @tc.desc: RegisterDialogDeathRecipientListener | NotifyDestroy test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, RegisterDialogDeathRecipientListener, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);

    sptr<MockDialogDeathRecipientListener> listener;
    ASSERT_EQ(nullptr, listener);
    window->RegisterDialogDeathRecipientListener(sptr<IDialogDeathRecipientListener>(listener));
    listener = new MockDialogDeathRecipientListener();
    ASSERT_NE(nullptr, listener);
    window->RegisterDialogDeathRecipientListener(sptr<IDialogDeathRecipientListener>(listener));
    EXPECT_CALL(*listener, OnDialogDeathRecipient());
    window->NotifyDestroy();
    window->UnregisterDialogDeathRecipientListener(sptr<IDialogDeathRecipientListener>(listener));
    window->NotifyDestroy();
}

/*
 * @tc.name: NotifyScreenshot
 * @tc.desc: NotifyScreenshot test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, NotifyScreenshot, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);

    sptr<MockScreenshotListener> listener;
    window->screenshotListeners_[window->GetWindowId()].push_back(sptr<IScreenshotListener>(listener));
    listener = new MockScreenshotListener;
    window->screenshotListeners_[window->GetWindowId()].push_back(sptr<IScreenshotListener>(listener));
    EXPECT_CALL(*listener, OnScreenshot()).Times(1);
    window->NotifyScreenshot();
    window->screenshotListeners_[window->GetWindowId()].clear();
}

/*
 * @tc.name: NotifyTouchDialogTarget
 * @tc.desc: NotifyTouchDialogTarget test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, NotifyTouchDialogTarget, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);

    sptr<MockDialogTargetTouchListener> listener;
    window->dialogTargetTouchListeners_[window->GetWindowId()].push_back(sptr<IDialogTargetTouchListener>(listener));
    listener = new MockDialogTargetTouchListener;
    window->dialogTargetTouchListeners_[window->GetWindowId()].push_back(sptr<IDialogTargetTouchListener>(listener));
    EXPECT_CALL(*listener, OnDialogTargetTouch());
    EXPECT_CALL(m->Mock(), ProcessPointDown(_, _));
    window->NotifyTouchDialogTarget();
    window->dialogTargetTouchListeners_[window->GetWindowId()].clear();
}

/*
 * @tc.name: NotifySizeChange
 * @tc.desc: NotifySizeChange test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, NotifySizeChange, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);

    sptr<MockWindowChangeListener> listener;
    window->windowChangeListeners_[window->GetWindowId()].push_back(sptr<IWindowChangeListener>(listener));
    listener = new MockWindowChangeListener;
    window->windowChangeListeners_[window->GetWindowId()].push_back(sptr<IWindowChangeListener>(listener));
    EXPECT_CALL(*listener, OnSizeChange(_, _, _));
    Rect rect;
    window->NotifySizeChange(rect, WindowSizeChangeReason::UNDEFINED);
    window->windowChangeListeners_[window->GetWindowId()].clear();
}

/*
 * @tc.name: NotifyModeChange
 * @tc.desc: NotifyModeChange test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, NotifyModeChange, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);

    sptr<MockWindowChangeListener> listener;
    window->windowChangeListeners_[window->GetWindowId()].push_back(sptr<IWindowChangeListener>(listener));
    listener = new MockWindowChangeListener;
    window->windowChangeListeners_[window->GetWindowId()].push_back(sptr<IWindowChangeListener>(listener));
    EXPECT_CALL(*listener, OnModeChange(_, _));
    window->NotifyModeChange(WindowMode::WINDOW_MODE_UNDEFINED);
    window->windowChangeListeners_[window->GetWindowId()].clear();
}

/*
 * @tc.name: NotifyAvoidAreaChange
 * @tc.desc: NotifyAvoidAreaChange test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, NotifyAvoidAreaChange, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);

    sptr<MockAvoidAreaChangedListener> listener;
    window->avoidAreaChangeListeners_[window->GetWindowId()].push_back(sptr<IAvoidAreaChangedListener>(listener));
    listener = new MockAvoidAreaChangedListener;
    window->avoidAreaChangeListeners_[window->GetWindowId()].push_back(sptr<IAvoidAreaChangedListener>(listener));
    EXPECT_CALL(*listener, OnAvoidAreaChanged(_, _));
    sptr<AvoidArea> avoidArea = new AvoidArea;
    window->NotifyAvoidAreaChange(avoidArea, AvoidAreaType::TYPE_CUTOUT);
    window->avoidAreaChangeListeners_[window->GetWindowId()].clear();
}

/*
 * @tc.name: NotifyDisplayMoveChange
 * @tc.desc: NotifyDisplayMoveChange test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, NotifyDisplayMoveChange, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);

    sptr<MockDisplayMoveListener> listener;
    window->displayMoveListeners_.push_back(sptr<IDisplayMoveListener>(listener));
    listener = new MockDisplayMoveListener;
    window->displayMoveListeners_.push_back(sptr<IDisplayMoveListener>(listener));
    EXPECT_CALL(*listener, OnDisplayMove(_, _));
    window->NotifyDisplayMoveChange(DisplayId{}, DisplayId{});
}

/*
 * @tc.name: SetAceAbilityHandler
 * @tc.desc: SetAceAbilityHandler test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetAceAbilityHandler, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);

    sptr<IAceAbilityHandler> handler;
    ASSERT_EQ(nullptr, handler);
    window->SetAceAbilityHandler(handler);
    handler = new MockAceAbilityHandler();
    ASSERT_NE(nullptr, handler);
    window->SetAceAbilityHandler(handler);
}

/*
 * @tc.name: HandleBackKeyPressedEvent
 * @tc.desc: HandleBackKeyPressedEvent test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, HandleBackKeyPressedEvent, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW, window->GetType());
    std::shared_ptr<MMI::KeyEvent> keyEvent;
    window->HandleBackKeyPressedEvent(keyEvent);

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    EXPECT_CALL(*content, ProcessBackPressed()).WillOnce(Return(false));
    window->HandleBackKeyPressedEvent(keyEvent);

    window->inputEventConsumer_.reset(new MockInputEventConsumer);
    EXPECT_CALL(*reinterpret_cast<MockInputEventConsumer*>(window->inputEventConsumer_.get()),
        OnInputEvent(keyEvent)).WillOnce(Return(true));
    window->HandleBackKeyPressedEvent(keyEvent);
}

/*
 * @tc.name: ConsumeKeyEvent
 * @tc.desc: ConsumeKeyEvent test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, ConsumeKeyEvent, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_COMPONENT);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::shared_ptr<MMI::KeyEvent> keyEvent = std::make_shared<MockKeyEvent>();
    EXPECT_CALL(m->Mock(), DispatchKeyEvent(_, _));
    window->ConsumeKeyEvent(keyEvent);
    window->property_->type_ = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    EXPECT_CALL(*content, ProcessKeyEvent(_));
    window->ConsumeKeyEvent(keyEvent);

    window->inputEventConsumer_.reset(new MockInputEventConsumer);
    EXPECT_CALL(*reinterpret_cast<MockInputEventConsumer*>(window->inputEventConsumer_.get()),
        OnInputEvent(keyEvent));
    window->ConsumeKeyEvent(keyEvent);

    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_BACK);
    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
    window->inputEventConsumer_ = nullptr;
    EXPECT_CALL(*content, ProcessBackPressed());
    window->ConsumeKeyEvent(keyEvent);
}

/*
 * @tc.name: ConsumePointerEvent
 * @tc.desc: ConsumePointerEvent test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, ConsumePointerEvent, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_LAUNCHER_RECENT);
    sptr<WindowImpl> window = new WindowImpl(option);
    Rect rect{ 0, 0, 10u, 10u };
    window->property_->SetWindowRect(rect);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = std::make_shared<MockPointerEvent>();
    MMI::PointerEvent::PointerItem item;
    pointerEvent->SetPointerId(0);
    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    window->ConsumePointerEvent(pointerEvent);

    item.SetPointerId(0);
    item.SetDisplayX(15); // 15 : position x
    item.SetDisplayY(15); // 15 : position y
    pointerEvent->AddPointerItem(item);
    window->ConsumePointerEvent(pointerEvent);

    item.SetDisplayX(5); // 5 : position x
    item.SetDisplayY(5); // 5 : position y
    pointerEvent->UpdatePointerItem(0, item);
    EXPECT_CALL(m->Mock(), ProcessPointDown(_, _));
    window->ConsumePointerEvent(pointerEvent);
}

/*
 * @tc.name: HandleModeChangeHotZones
 * @tc.desc: HandleModeChangeHotZones test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, HandleModeChangeHotZones, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    window->HandleModeChangeHotZones(0, 0);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    EXPECT_CALL(m->Mock(), GetModeChangeHotZones(_, _)).Times(1).WillOnce(Return(WMError::WM_DO_NOTHING));
    window->HandleModeChangeHotZones(0, 0);
    EXPECT_CALL(m->Mock(), GetModeChangeHotZones(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->HandleModeChangeHotZones(0, 0);
}

/*
 * @tc.name: UpdatePointerEventForStretchableWindow
 * @tc.desc: UpdatePointerEventForStretchableWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, UpdatePointerEventForStretchableWindow, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    window->property_->SetWindowRect(Rect{ 0, 0, 10, 10 });
    window->property_->SetOriginRect(Rect{ 0, 0, 100, 100 });
    std::shared_ptr<MMI::PointerEvent> pointerEvent =  std::make_shared<MockPointerEvent>();
    MMI::PointerEvent::PointerItem item;
    ASSERT_FALSE(pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), item));
    window->UpdatePointerEventForStretchableWindow(pointerEvent);
    pointerEvent->SetPointerId(0);
    item.SetPointerId(0);
    item.SetDisplayX(5); // 5 : position x
    item.SetDisplayY(5); // 5 : position y
    pointerEvent->AddPointerItem(item);
    window->UpdatePointerEventForStretchableWindow(pointerEvent);
    ASSERT_TRUE(pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), item));
    ASSERT_EQ(50, item.GetDisplayX());
    ASSERT_EQ(50, item.GetDisplayY());
}

/**
 * @tc.name: MoveDrag
 * @tc.desc: StartMove | ReadyToMoveOrDragWindow |  EndMoveOrDragWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, MoveDrag, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("MoveDrag");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    window->RestoreSplitWindowMode(0u);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Show();
    window->moveDragProperty_->startDragFlag_ = false;
    window->moveDragProperty_->pointEventStarted_ = true;
    window->StartMove();
    window->moveDragProperty_->pointEventStarted_ = false;
    window->StartMove();

    std::shared_ptr<MMI::PointerEvent> pointerEvent =  std::make_shared<MockPointerEvent>();
    MMI::PointerEvent::PointerItem item;
    pointerEvent->SetTargetDisplayId(0);
    item.SetDisplayX(10000);
    item.SetDisplayY(10000);

    window->moveDragProperty_->pointEventStarted_ = true;
    window->ReadyToMoveOrDragWindow(pointerEvent, item);
    window->moveDragProperty_->startMoveFlag_ = true;
    window->moveDragProperty_->startDragFlag_ = true;
    EXPECT_CALL(m->Mock(), ProcessPointUp(_)).Times(2);
    window->EndMoveOrDragWindow(uint32_t(), uint32_t(),
        window->moveDragProperty_->startPointerId_, window->moveDragProperty_->sourceType_);

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: TransferPointerEvent
 * @tc.desc: TransferPointerEvent test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, TransferPointerEvent, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::shared_ptr<MMI::PointerEvent> pointerEvent =  std::make_shared<MockPointerEvent>();
    window->windowSystemConfig_.isStretchable_ = true;
    window->TransferPointerEvent(pointerEvent);
    window->windowSystemConfig_.isStretchable_ = false;
    window->TransferPointerEvent(pointerEvent);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    EXPECT_CALL(*content, ProcessPointerEvent(_));
    window->TransferPointerEvent(pointerEvent);

    window->inputEventConsumer_.reset(new MockInputEventConsumer);
    EXPECT_CALL(*reinterpret_cast<MockInputEventConsumer*>(window->inputEventConsumer_.get()),
        OnInputEvent(pointerEvent));
    window->TransferPointerEvent(pointerEvent);
}

/*
 * @tc.name: RequestVsync
 * @tc.desc: RequestVsync test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, RequestVsync, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("RequestVsync");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    window->RestoreSplitWindowMode(0u);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));


    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show());
    std::shared_ptr<VsyncCallback> callback;
    window->RequestVsync(callback);

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
    ASSERT_EQ(WindowState::STATE_DESTROYED, window->GetWindowState());
    window->RequestVsync(callback);
}

/*
 * @tc.name: UpdateConfiguration
 * @tc.desc: UpdateConfiguration test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, UpdateConfiguration, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("UpdateConfiguration");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    window->RestoreSplitWindowMode(0u);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    option->SetWindowName("subwindow");
    sptr<WindowImpl> subWindow = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, subWindow->Create(window->GetWindowId()));
    std::shared_ptr<AppExecFwk::Configuration> configuration;
    window->UpdateConfiguration(configuration);

    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    subWindow->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* subContent = reinterpret_cast<Ace::UIContentMocker*>(subWindow->uiContent_.get());
    EXPECT_CALL(*content, UpdateConfiguration(_));
    EXPECT_CALL(*subContent, UpdateConfiguration(_));
    window->UpdateConfiguration(configuration);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*content, Destroy());
    EXPECT_CALL(*subContent, Destroy());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: UpdateWindowState
 * @tc.desc: UpdateWindowState test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, UpdateWindowState, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("UpdateWindowState");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    window->RestoreSplitWindowMode(0u);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    window->UpdateWindowState(WindowState::STATE_FROZEN);
    window->UpdateWindowState(WindowState::STATE_UNFROZEN);
    window->UpdateWindowState(WindowState::STATE_SHOWN);
    EXPECT_CALL(m->Mock(), RemoveWindow(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->UpdateWindowState(WindowState::STATE_HIDDEN);
    window->UpdateWindowState(WindowState::STATE_INITIAL);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: RestoreSplitWindowMode
 * @tc.desc: RestoreSplitWindowMode test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, RestoreSplitWindowMode, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("RestoreSplitWindowMode");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    window->RestoreSplitWindowMode(0u);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    window->RestoreSplitWindowMode(static_cast<uint32_t>(WindowMode::WINDOW_MODE_SPLIT_PRIMARY));
    window->RestoreSplitWindowMode(static_cast<uint32_t>(WindowMode::WINDOW_MODE_SPLIT_SECONDARY));
    window->RestoreSplitWindowMode(static_cast<uint32_t>(WindowMode::WINDOW_MODE_UNDEFINED));
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: IsFocused
 * @tc.desc: IsFocused test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, IsFocused, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("IsFocused");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    window->UpdateFocusStatus(false);
    ASSERT_FALSE(window->IsFocused());
    window->UpdateFocusStatus(true);
    ASSERT_TRUE(window->IsFocused());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: UpdateSubWindowStateAndNotify
 * @tc.desc: update subwindow state
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, UpdateSubWindowStateAndNotify, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("main");
    sptr<WindowImpl> mainWindow = new WindowImpl(option);

    ASSERT_EQ(WMError::WM_OK, mainWindow->Create(INVALID_WINDOW_ID));
    ASSERT_EQ(WmErrorCode::WM_OK, mainWindow->UpdateSubWindowStateAndNotify(mainWindow->GetWindowId()));

    option = new WindowOption();
    option->SetWindowName("sub");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowImpl> subWindow = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, subWindow->Create(mainWindow->GetWindowId()));

    // main window hide
    mainWindow->state_ = WindowState::STATE_HIDDEN;
    subWindow->state_ = WindowState::STATE_SHOWN;
    subWindow->subWindowState_ = WindowState::STATE_SHOWN;
    mainWindow->UpdateSubWindowStateAndNotify(mainWindow->GetWindowId());
    ASSERT_EQ(subWindow->subWindowState_, WindowState::STATE_HIDDEN);

    // main window show
    mainWindow->state_ = WindowState::STATE_SHOWN;
    subWindow->state_ = WindowState::STATE_SHOWN;
    subWindow->subWindowState_ = WindowState::STATE_HIDDEN;
    mainWindow->UpdateSubWindowStateAndNotify(mainWindow->GetWindowId());
    ASSERT_EQ(subWindow->subWindowState_, WindowState::STATE_SHOWN);

    subWindow->state_ = WindowState::STATE_HIDDEN;
    mainWindow->UpdateSubWindowStateAndNotify(mainWindow->GetWindowId());
    ASSERT_EQ(subWindow->subWindowState_, WindowState::STATE_HIDDEN);

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, mainWindow->Destroy());
}

/*
 * @tc.name: UpdateWindowStateWhenHide
 * @tc.desc: UpdateWindowStateWhenHide test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, UpdateWindowStateWhenHide, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("main");
    sptr<WindowImpl> mainWindow = new WindowImpl(option);
    ASSERT_EQ(WMError::WM_OK, mainWindow->Create(INVALID_WINDOW_ID));
    ASSERT_EQ(WmErrorCode::WM_OK, mainWindow->UpdateWindowStateWhenHide());

    option = new WindowOption();
    option->SetWindowName("sub");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowImpl> subWindow = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, subWindow->Create(mainWindow->GetWindowId()));
    subWindow->subWindowState_ = WindowState::STATE_SHOWN;
    subWindow->UpdateWindowStateWhenHide();
    ASSERT_EQ(subWindow->subWindowState_, WindowState::STATE_HIDDEN);
    subWindow->subWindowState_ = WindowState::STATE_HIDDEN;
    subWindow->UpdateWindowStateWhenHide();
    ASSERT_EQ(subWindow->subWindowState_, WindowState::STATE_HIDDEN);

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, mainWindow->Destroy());
    ASSERT_EQ(WMError::WM_OK, subWindow->Destroy());
}

/*
 * @tc.name: UpdateWindowStateWhenShow
 * @tc.desc: UpdateWindowStateWhenShow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, UpdateWindowStateWhenShow, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("main");
    sptr<WindowImpl> mainWindow = new WindowImpl(option);
    if (mainWindow->Create(INVALID_WINDOW_ID) != WMError::WM_OK) {
        ASSERT_NE(WMError::WM_OK, mainWindow->Create(INVALID_WINDOW_ID));
    }


    ASSERT_EQ(WmErrorCode::WM_OK, mainWindow->UpdateWindowStateWhenShow());

    option = new WindowOption();
    option->SetWindowName("sub");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowImpl> subWindow = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, subWindow->Create(mainWindow->GetWindowId()));
    ASSERT_EQ(WmErrorCode::WM_OK, subWindow->UpdateWindowStateWhenShow());

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, mainWindow->Destroy());
}

/*
 * @tc.name: RaiseToAppTop
 * @tc.desc: RaiseToAppTop test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, RaiseToAppTop, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->parentId_ = INVALID_WINDOW_ID;
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WmErrorCode::WM_ERROR_INVALID_PARENT, window->RaiseToAppTop());

    window->property_->parentId_ = 100000;
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_EQ(WmErrorCode::WM_ERROR_INVALID_CALLING, window->RaiseToAppTop());

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    window->state_ = WindowState::STATE_HIDDEN;
    ASSERT_EQ(WmErrorCode::WM_ERROR_STATE_ABNORMALLY, window->RaiseToAppTop());

    window->state_ = WindowState::STATE_SHOWN;
    EXPECT_CALL(m->Mock(), RaiseToAppTop(_)).Times(1).WillOnce(Return(WmErrorCode::WM_OK));
    ASSERT_EQ(WmErrorCode::WM_OK, window->RaiseToAppTop());

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

 /*
 * @tc.name: UpdateDecorEnable
 * @tc.desc: UpdateDecorEnable test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, UpdateDecorEnable, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    window->property_->mode_ = WindowMode::WINDOW_MODE_FULLSCREEN;
    window->property_->type_ = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    window->windowSystemConfig_.isSystemDecorEnable_ = true;
    window->windowSystemConfig_.decorModeSupportInfo_ = WINDOW_MODE_SUPPORT_FLOATING;
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker* content = reinterpret_cast<Ace::UIContentMocker*>(window->uiContent_.get());
    sptr<MockWindowChangeListener> listener = new MockWindowChangeListener;
    window->RegisterWindowChangeListener(sptr<IWindowChangeListener>(listener));

    EXPECT_CALL(*content, UpdateWindowMode(_, _));
    EXPECT_CALL(*listener, OnModeChange(_, _));
    window->UpdateDecorEnable(true);
    ASSERT_TRUE(window->IsDecorEnable());
    ASSERT_FALSE(window->property_->GetDecorEnable());
    window->windowSystemConfig_.decorModeSupportInfo_ = WINDOW_MODE_SUPPORT_ALL;
    window->UpdateDecorEnable();
    ASSERT_TRUE(window->IsDecorEnable());
    ASSERT_TRUE(window->property_->GetDecorEnable());
    window->property_->type_ = WindowType::WINDOW_TYPE_FLOAT;
    window->UpdateDecorEnable();
    ASSERT_FALSE(window->IsDecorEnable());
    ASSERT_FALSE(window->property_->GetDecorEnable());
    window->UnregisterWindowChangeListener(sptr<IWindowChangeListener>(listener));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Find01
 * @tc.desc: Find one exit window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Find01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Find01");
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(nullptr, WindowImpl::Find("Find01"));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Find02
 * @tc.desc: Find one no exit window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Find02, Function | SmallTest | Level2)
{
    ASSERT_EQ(nullptr, WindowImpl::Find("Find02"));
}

/**
 * @tc.name: Find03
 * @tc.desc: Find window with empty name
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Find03, Function | SmallTest | Level2)
{
    ASSERT_EQ(nullptr, WindowImpl::Find(""));
}

/**
 * @tc.name: FindWindowById02
 * @tc.desc: Find one top window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, FindWindowById02, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("FindWindowById02");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));

    ASSERT_NE(nullptr, window->FindWindowById(window->property_->GetWindowId()));

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: FindWindowById03
 * @tc.desc: Find one no exit window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, FindWindowById03, Function | SmallTest | Level2)
{
    ASSERT_EQ(nullptr, WindowImpl::FindWindowById(0));
}

/**
 * @tc.name: GetTopWindowWithId02
 * @tc.desc: Not get topwindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, GetTopWindowWithId02, Function | SmallTest | Level2)
{
    ASSERT_EQ(nullptr, WindowImpl::GetTopWindowWithId(0));
}

/**
 * @tc.name: UpdateConfigurationForAll01
 * @tc.desc: UpdateConfigurationForAll01 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, UpdateConfigurationForAll01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    std::shared_ptr<AppExecFwk::Configuration> configuration;
    sptr<Window>(window)->UpdateConfigurationForAll(configuration);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: GetSurfaceNode01
 * @tc.desc: GetSurfaceNode01 Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, GetSurfaceNode01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    ASSERT_NE(nullptr, sptr<Window>(window)->GetSurfaceNode());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetRequestRect
 * @tc.desc: get subwindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, GetRequestRect, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    Rect a{0, 0, 0, 0};
    ASSERT_EQ(a, window->GetRequestRect());
    delete window;
    delete option;
}

/**
 * @tc.name: GetAlpha
 * @tc.desc: GetAlpha Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, GetAlpha, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(1.0f, window->GetAlpha());
    delete window;
    delete option;
}

/**
 * @tc.name: GetWindowState
 * @tc.desc: GetWindowState Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, GetWindowState, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WindowState::STATE_INITIAL, window->GetWindowState());
    delete window;
    delete option;
}

/**
 * @tc.name: SetFocusable03
 * @tc.desc: SetFocusable Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetFocusable03, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    WMError err = window->SetFocusable(false);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, err);
    delete window;
    delete option;
}

/**
 * @tc.name: SetWindowType05
 * @tc.desc: SetWindowType Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetWindowType05, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetWindowType05");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    ASSERT_EQ(WMError::WM_OK, window->SetWindowType(WindowType::APP_WINDOW_BASE));
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetAlpha01
 * @tc.desc: SetAlpha Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetAlpha01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetAlpha01");
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    ASSERT_EQ(WMError::WM_OK, window->SetAlpha(1.0f));
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: CreateWindowImpl
 * @tc.desc: CreateSurfaceNode with different type
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, CreateWindowImpl, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateSurfaceNode");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowRect({ 1, 1, 1, 1 });
    option->SetBundleName("OK");
    ASSERT_NE(option, nullptr);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(window, nullptr);

    sptr<WindowOption> option1 = new WindowOption();
    option1->SetWindowName("CreateSurfaceNode1");
    option1->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option1->SetWindowType(WindowType::WINDOW_TYPE_BOOT_ANIMATION);
    option1->SetWindowRect({ 1, 1, 1, 1 });
    ASSERT_NE(option1, nullptr);
    sptr<WindowImpl> window1 = new WindowImpl(option1);
    ASSERT_NE(window1, nullptr);

    sptr<WindowOption> option2 = new WindowOption();
    option2->SetWindowName("CreateSurfaceNode2");
    option2->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option2->SetWindowType(WindowType::WINDOW_TYPE_POINTER);
    option2->SetWindowRect({ 1, 1, 1, 1 });
    ASSERT_NE(option2, nullptr);
    sptr<WindowImpl> window2 = new WindowImpl(option2);
    ASSERT_NE(window2, nullptr);

    sptr<WindowOption> option3 = new WindowOption();
    option3->SetWindowName("CreateSurfaceNode3");
    option3->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option3->SetWindowType(WindowType::WINDOW_TYPE_FREEZE_DISPLAY);
    option3->SetWindowRect({ 1, 1, 1, 1 });
    ASSERT_NE(option3, nullptr);
    sptr<WindowImpl> window3 = new WindowImpl(option3);
    ASSERT_NE(window3, nullptr);

    sptr<WindowOption> option4 = new WindowOption();
    option4->SetWindowName("CreateSurfaceNode4");
    option4->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option4->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    option4->SetWindowRect({ 1, 1, 1, 1 });
    ASSERT_NE(option4, nullptr);
    sptr<WindowImpl> window4 = new WindowImpl(option4);
    ASSERT_NE(window4, nullptr);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
    ASSERT_EQ(WMError::WM_OK, window1->Destroy());
    ASSERT_EQ(WMError::WM_OK, window2->Destroy());
    ASSERT_EQ(WMError::WM_OK, window3->Destroy());
    ASSERT_EQ(WMError::WM_OK, window4->Destroy());
}

/*
 * @tc.name: Create
 * @tc.desc: Create
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Create, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateSurfaceNode5");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowRect({ 1, 1, 1, 1 });
    option->SetBundleName("OK");
    ASSERT_NE(option, nullptr);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(window, nullptr);

    WMError res = window->Create(INVALID_WINDOW_ID);
    ASSERT_EQ(WMError::WM_OK, res);

    sptr<WindowImpl> window1 = new WindowImpl(option); // the same name
    ASSERT_NE(window, nullptr);
    res = window1->Create(INVALID_WINDOW_ID);
    ASSERT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, res);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
    ASSERT_EQ(WMError::WM_OK, window1->Destroy());
}

/*
 * @tc.name: Create1
 * @tc.desc: Create WindowCreateCheck
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Create1, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowCreateCheck");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA);
    option->SetWindowRect({ 1, 1, 1, 1 });
    option->SetBundleName("OK");
    ASSERT_NE(option, nullptr);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(window, nullptr);

    WMError res = window->Create(INVALID_WINDOW_ID);
    ASSERT_EQ(WMError::WM_OK, res);

    option->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    sptr<WindowImpl> window1 = new WindowImpl(option);
    ASSERT_NE(window1, nullptr);
    res = window1->Create(INVALID_WINDOW_ID);
    ASSERT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, res);

    option->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    sptr<WindowImpl> window2 = new WindowImpl(option);
    ASSERT_NE(window2, nullptr);
    res = window1->Create(INVALID_WINDOW_ID);
    ASSERT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, res);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
    ASSERT_EQ(WMError::WM_OK, window1->Destroy());
    ASSERT_EQ(WMError::WM_OK, window2->Destroy());
}

/*
 * @tc.name: Create2
 * @tc.desc: Create WindowCreateCheck
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Create2, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowCreateCheck1");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_COMPONENT);
    option->SetWindowRect({ 1, 1, 1, 1 });
    option->SetBundleName("OK");
    ASSERT_NE(option, nullptr);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(window, nullptr);

    WMError res = window->Create(1);
    ASSERT_EQ(WMError::WM_OK, res);

    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowImpl> window1 = new WindowImpl(option);
    ASSERT_NE(window1, nullptr);
    res = window1->Create(1);
    ASSERT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, res);

    option->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    sptr<WindowImpl> window2 = new WindowImpl(option);
    ASSERT_NE(window2, nullptr);
    res = window2->Create(1);
    ASSERT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, res);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
    ASSERT_EQ(WMError::WM_OK, window1->Destroy());
    ASSERT_EQ(WMError::WM_OK, window2->Destroy());
}

/*
 * @tc.name: Create3
 * @tc.desc: Create3 WindowCreateCheck
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Create3, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowCreateCheck2");
    option->SetWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);
    option->SetWindowType(WindowType::WINDOW_TYPE_VOLUME_OVERLAY);
    option->SetWindowRect({ 1, 1, 1, 1 });
    option->SetBundleName("OK");
    ASSERT_NE(option, nullptr);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(window, nullptr);

    std::shared_ptr<AbilityRuntime::AbilityContext> context =
        std::make_shared<AbilityRuntime::AbilityContextImpl>();
    WMError res = window->Create(1, context);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARENT, res);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: GetTopWindowWithId
 * @tc.desc: CreateSurfaceNode with different type
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, GetTopWindowWithId, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("GetTopWindowWithId");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowRect({ 1, 1, 1, 1 });
    option->SetBundleName("OK");
    ASSERT_NE(option, nullptr);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(window, nullptr);
    sptr<Window> topWindow = window->GetTopWindowWithId(INVALID_WINDOW_ID);

    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: NotifyForegroundInteractiveStatus
 * @tc.desc: NotifyForegroundInteractiveStatus Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, NotifyForegroundInteractiveStatus, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(window, nullptr);
    bool interactive = false;
    window->NotifyForegroundInteractiveStatus(interactive);
    window->SetWindowState(WindowState::STATE_DESTROYED);
    interactive = true;
    window->NotifyForegroundInteractiveStatus(interactive);
    ASSERT_EQ(WindowState::STATE_DESTROYED, window->GetWindowState());
}

/*
 * @tc.name: GetTopWindowWithContext
 * @tc.desc: GetTopWindowWithContexttest01
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, GetTopWindowWithContext01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("GetTopWindowWithContext01");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowRect({ 1, 1, 1, 1 });
    option->SetBundleName("OK");
    ASSERT_NE(option, nullptr);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(window, nullptr);
    std::shared_ptr<AbilityRuntime::Context> context;
    ASSERT_EQ(nullptr, sptr<Window>(window)->GetTopWindowWithContext(context));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: GetSubWindow
 * @tc.desc: GetSubWindowtest02
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, GetSubWindowtext02, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("GetSubWindowtest02");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowRect({ 1, 1, 1, 1 });
    option->SetBundleName("OK");
    ASSERT_NE(option, nullptr);
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    ASSERT_NE(window, nullptr);
    auto subWindowVec = sptr<Window>(window)->GetSubWindow(window->GetWindowId());
    if (subWindowVec.size() == 1) {
        ASSERT_EQ(1, subWindowVec.size());
    }
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: SetFloatingMaximize
 * @tc.desc: SetFloatingMaximize test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetFloatingMaximize, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetFloatingMaximize");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetWindowFlags(0));
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    window->UpdateModeSupportInfo(0);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetFloatingMaximize(true));
}

/**
 * @tc.name: SetAspectRatio
 * @tc.desc: SetAspectRatio Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetAspectRatio, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetAspectRatio");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_NE(option, nullptr);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(window, nullptr);
    ASSERT_EQ(WMError::WM_OK, window->SetAspectRatio(1.1));
    option->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ASSERT_EQ(WMError::WM_OK, window->SetAspectRatio(1.1));
}

/**
 * @tc.name: SetAspectRatio02
 * @tc.desc: SetAspectRatio Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetAspectRatio02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetAspectRatio02");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    ASSERT_NE(option, nullptr);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(window, nullptr);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetAspectRatio(0.0));
}

/*
 * @tc.name: MapDialogWindowToAppIfNeeded
 * @tc.desc: MapDialogWindowToAppIfNeededtest
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, MapDialogWindowToAppIfNeededtest, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("MapDialogWindowToAppIfNeededtest");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    option->SetWindowRect({ 1, 1, 1, 1 });
    option->SetBundleName("OK");
    ASSERT_NE(option, nullptr);
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(window, nullptr);
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    window->MapDialogWindowToAppIfNeeded();
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/*
 * @tc.name: GetConfigurationFromAbilityInfo
 * @tc.desc: GetConfigurationFromAbilityInfotest
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, GetConfigurationFromAbilityInfotest, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("GetConfigurationFromAbilityInfotest");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ASSERT_NE(option, nullptr);
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    ASSERT_NE(window, nullptr);
    window->MapDialogWindowToAppIfNeeded();
    window->GetConfigurationFromAbilityInfo();
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetSnapshotSkip
 * @tc.desc: SetSnapshotSkip test
 * @tc.type: FUNC
 * @tc.require: issueI5MYNX
 */
HWTEST_F(WindowImplTest, SetSnapshotSkip, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetSnapshotSkip");
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(nullptr, window);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillRepeatedly(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->SetSnapshotSkip(true));
}

/**
 * @tc.name: MaximizeFloating
 * @tc.desc: MaximizeFloating test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, MaximizeFloating, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("MaximizeFloating");
    sptr<WindowImpl> window = new WindowImpl(option);
    window->MaximizeFloating();
    ASSERT_EQ(WindowMode::WINDOW_MODE_UNDEFINED, window->GetMode());
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(INVALID_WINDOW_ID));
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Show();
    window->MaximizeFloating();
    ASSERT_EQ(WindowMode::WINDOW_MODE_FLOATING, window->GetMode());
    EXPECT_CALL(m->Mock(), RemoveWindow(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Hide();
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}
}
} // namespace Rosen
} // namespace OHOS
