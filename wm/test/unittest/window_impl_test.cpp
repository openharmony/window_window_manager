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

class WindowImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    void CreateStretchableWindow(sptr<WindowImpl>& window, const Rect& rect);

    static inline std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_;
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
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
    usleep(WAIT_SYNC_IN_NS);
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
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
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
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
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
 * @tc.name: RequestVsyncSucc
 * @tc.desc: RequestVsync Test Succ
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, RequestVsyncSucc, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("RequestVsyncSucc");
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(window, nullptr);
    std::shared_ptr<VsyncCallback> vsyncCallback = std::make_shared<VsyncCallback>();
    window->SetWindowState(WindowState::STATE_DESTROYED);
    ASSERT_EQ(WindowState::STATE_DESTROYED, window->GetWindowState());
    window->RequestVsync(vsyncCallback);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}


/**
 * @tc.name: RequestVsyncErr
 * @tc.desc: RequestVsync Test Err
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, RequestVsyncErr, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("RequestVsyncErr");
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(window, nullptr);
    std::shared_ptr<VsyncCallback> vsyncCallback = std::make_shared<VsyncCallback>();
    window->SetWindowState(WindowState::STATE_DESTROYED);
    ASSERT_EQ(WindowState::STATE_DESTROYED, window->GetWindowState());
    window->vsyncStation_ = nullptr;
    window->RequestVsync(vsyncCallback);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: ClearVsync
 * @tc.desc: Clear vsync test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, ClearVsync, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("ClearVsync");
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(window, nullptr);
    window->ClearVsyncStation();
    ASSERT_NE(window, nullptr);
}

/**
 * @tc.name: InitWindowProperty
 * @tc.desc: InitWindowProperty Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, InitWindowProperty, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("InitWindowProperty");
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_NE(window, nullptr);
    window->InitWindowProperty(option);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
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
    ASSERT_EQ(ColorSpace::COLOR_SPACE_WIDE_GAMUT, window->GetColorSpace());
    window->SetColorSpace(ColorSpace::COLOR_SPACE_DEFAULT);
    ASSERT_EQ(ColorSpace::COLOR_SPACE_DEFAULT, window->GetColorSpace());
    uint32_t invalidColorSpace = 1234u;
    window->SetColorSpace(static_cast<ColorSpace>(invalidColorSpace));
    ASSERT_EQ(ColorSpace::COLOR_SPACE_DEFAULT, window->GetColorSpace());
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
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetBrightness(-0.5f)); // -0.5f: brightness
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
    std::shared_ptr<VsyncCallback> callback = std::make_shared<VsyncCallback>();
    window->SetWindowState(WindowState::STATE_DESTROYED);
    ASSERT_EQ(WindowState::STATE_DESTROYED, window->GetWindowState());
    window->RequestVsync(callback);

    ASSERT_EQ(WMError::WM_OK, window->Destroy());
    window->SetWindowState(WindowState::STATE_DESTROYED);
    ASSERT_EQ(WindowState::STATE_DESTROYED, window->GetWindowState());
    window->RequestVsync(callback);
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

/**
 * @tc.name: SetRestoredRouterStack_0100
 * @tc.desc: basic function test of set or get restored router stack.
 * @tc.type: FUNC
 * @tc.require: issue
 */
HWTEST_F(WindowImplTest, SetRestoredRouterStack_0100, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(option, nullptr);
    sptr<WindowImpl> window = sptr<WindowImpl>::MakeSptr(option);
    ASSERT_NE(window, nullptr);
    std::string routerStack = "stackInfo:{}";
    EXPECT_EQ(window->SetRestoredRouterStack(routerStack), WMError::WM_OK);
    std::string gettedStack = window->GetRestoredRouterStack();
    EXPECT_EQ(gettedStack, routerStack);
    EXPECT_TRUE(window->GetRestoredRouterStack().empty());
}
}
} // namespace Rosen
} // namespace OHOS
