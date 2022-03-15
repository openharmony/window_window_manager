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

#include "window_impl_test.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
void WindowImplTest::SetUpTestCase()
{
    option_ = new WindowOption();
    option_->SetWindowName("WindowImplTest");
    window_ = new WindowImpl(option_);
    abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();

    m_ = std::make_unique<Mocker>();
    EXPECT_CALL(m_->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window_->Create("");
}

void WindowImplTest::TearDownTestCase()
{
    EXPECT_CALL(m_->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window_->Destroy();
}

void WindowImplTest::SetUp()
{
}

void WindowImplTest::TearDown()
{
}

namespace {
/**
 * @tc.name: CreateWindow01
 * @tc.desc: Create window with no parentName
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, CreateWindow01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateWindow01");
    sptr<WindowImpl> window = new WindowImpl(option);

    EXPECT_CALL(m_->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(""));

    EXPECT_CALL(m_->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: CreateWindow02
 * @tc.desc: Create window with no parentName and no abilityContext
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, CreateWindow02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateWindow02");
    sptr<WindowImpl> window = new WindowImpl(option);

    EXPECT_CALL(m_->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_SAMGR));
    ASSERT_EQ(WMError::WM_ERROR_SAMGR, window->Create(""));
}

/**
 * @tc.name: CreateWindow03
 * @tc.desc: Create window with illegal parentName
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, CreateWindow03, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateWindow03");
    sptr<WindowImpl> window = new WindowImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->Create("illegal"));
}

/**
 * @tc.name: CreateWindow04
 * @tc.desc: Create window with repeated windowName
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, CreateWindow04, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowImplTest");
    sptr<WindowImpl> window = new WindowImpl(option);

    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->Create(""));
}

/**
 * @tc.name: CreateWindow05
 * @tc.desc: Create window with exist parentName
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, CreateWindow05, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateWindow05");
    sptr<WindowImpl> window = new WindowImpl(option);

    EXPECT_CALL(m_->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create("WindowImplTest"));

    EXPECT_CALL(m_->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: CreateWindow06
 * @tc.desc: Create window with no default option, get and check Property
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, CreateWindow06, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateWindow06");
    struct Rect rect = {1, 2, 3u, 4u};
    option->SetWindowRect(rect);
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);

    EXPECT_CALL(m_->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(""));

    ASSERT_EQ(1, window->GetRect().posX_);
    ASSERT_EQ(2, window->GetRect().posY_);
    ASSERT_EQ(3u, window->GetRect().width_);
    ASSERT_EQ(4u, window->GetRect().height_);
    ASSERT_EQ(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW, window->GetType());
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetMode());
    ASSERT_EQ("CreateWindow06", window->GetWindowName());

    EXPECT_CALL(m_->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: FindWindow01
 * @tc.desc: Find one exit window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, FindWindow01, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, WindowImpl::Find("WindowImplTest"));
}

/**
 * @tc.name: FindWindow02
 * @tc.desc: Add another window, find both two windows
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, FindWindow02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("FindWindow02");
    sptr<WindowImpl> window = new WindowImpl(option);

    EXPECT_CALL(m_->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));

    ASSERT_EQ(WMError::WM_OK, window->Create(""));

    ASSERT_NE(nullptr, WindowImpl::Find("WindowImplTest"));
    ASSERT_NE(nullptr, WindowImpl::Find("FindWindow02"));

    EXPECT_CALL(m_->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
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
    ASSERT_EQ(nullptr, WindowImpl::Find("FindWindow02"));
}

/**
 * @tc.name: SetWindowType01
 * @tc.desc: SetWindowType
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetWindowType01, Function | SmallTest | Level2)
{
    ASSERT_EQ(WMError::WM_OK, window_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW));
}

/**
 * @tc.name: SetWindowMode01
 * @tc.desc: SetWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetWindowMode01, Function | SmallTest | Level2)
{
    ASSERT_EQ(WMError::WM_OK, window_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
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
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(""));
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
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(""));
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
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(""));
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
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(""));
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
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    EXPECT_CALL(m->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: ShowHideWindow02
 * @tc.desc: Show window with add window WM_ERROR_SAMGR
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, ShowHideWindow02, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_SAMGR));
    ASSERT_EQ(WMError::WM_ERROR_SAMGR, window_->Show());
}

/**
 * @tc.name: ShowHideWindow03
 * @tc.desc: Show window with add window WM_ERROR_IPC_FAILED
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, ShowHideWindow03, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_IPC_FAILED));
    ASSERT_EQ(WMError::WM_ERROR_IPC_FAILED, window_->Show());
}

/**
 * @tc.name: ShowHideWindow04
 * @tc.desc: Show window with add window OK & Hide window with remove window WM_ERROR_SAMGR
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, ShowHideWindow04, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    EXPECT_CALL(m->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_SAMGR));
    ASSERT_EQ(WMError::WM_ERROR_SAMGR, window_->Hide());
}

/**
 * @tc.name: ShowHideWindow05
 * @tc.desc: Hide window with remove window WM_ERROR_IPC_FAILED
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, ShowHideWindow05, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_IPC_FAILED));
    ASSERT_EQ(WMError::WM_ERROR_IPC_FAILED, window_->Hide());
}

/**
 * @tc.name: ShowHideWindow06
 * @tc.desc: Hide window with remove window OK
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, ShowHideWindow06, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: SetSystemBarProperty01
 * @tc.desc: SetSystemBarProperty with default param
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetSystemBarProperty01, Function | SmallTest | Level3)
{
    WindowType type = WindowType::WINDOW_TYPE_STATUS_BAR;
    SystemBarProperty prop;
    ASSERT_EQ(WMError::WM_OK, window_->SetSystemBarProperty(type, prop));
}

/**
 * @tc.name: SetSystemBarProperty02
 * @tc.desc: SetSystemBarProperty with adapter return WM_ERROR_SAMGR
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetSystemBarProperty02, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window_->Show();
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_SAMGR));
    WindowType type = WindowType::WINDOW_TYPE_STATUS_BAR;
    const SystemBarProperty SYS_BAR_PROP(false, 0xE5222222, 0xE5333333);
    ASSERT_EQ(WMError::WM_ERROR_SAMGR, window_->SetSystemBarProperty(type, SYS_BAR_PROP));
    EXPECT_CALL(m->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window_->Hide();
}

/**
 * @tc.name: SetSystemBarProperty03
 * @tc.desc: SetSystemBarProperty to invalid window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetSystemBarProperty03, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetSystemBarPropertyWin03");
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
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    WindowType type = WindowType::WINDOW_TYPE_STATUS_BAR;
    const SystemBarProperty SYS_BAR_PROP(false, 0xE5222222, 0xE5333344);
    ASSERT_EQ(WMError::WM_OK, window_->SetSystemBarProperty(type, SYS_BAR_PROP));
    ASSERT_EQ(SYS_BAR_PROP, window_->GetSystemBarPropertyByType(type));
    EXPECT_CALL(m->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window_->Hide();
}

/**
 * @tc.name: GetSystemBarPropertyByType02
 * @tc.desc: GetSystemBarPropertyByType with nonexist key
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, GetSystemBarPropertyByType02, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window_->Show();
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    const SystemBarProperty SYS_BAR_PROP(false, 0xE5222222, 0xE5333333);
    const SystemBarProperty DEFAULT_PROP;
    ASSERT_EQ(WMError::WM_OK, window_->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, SYS_BAR_PROP));
    ASSERT_EQ(DEFAULT_PROP, window_->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_BAR));
    EXPECT_CALL(m->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window_->Hide();
}

/**
 * @tc.name: GetSystemBarPropertyByType03
 * @tc.desc: GetSystemBarPropertyByType with not systemBar type
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, GetSystemBarPropertyByType03, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window_->Show();
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    const SystemBarProperty SYS_BAR_PROP(false, 0xE5222222, 0xE5333366);
    const SystemBarProperty DEFAULT_PROP;
    ASSERT_EQ(WMError::WM_OK, window_->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, SYS_BAR_PROP));
    ASSERT_EQ(DEFAULT_PROP, window_->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW));
    EXPECT_CALL(m->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window_->Hide();
}

/**
 * @tc.name: IsDecorEnable01
 * @tc.desc: IsDecorEnable default value is false
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, IsDecorEnable01, Function | SmallTest | Level3)
{
    ASSERT_FALSE(window_->IsDecorEnable());
}

/**
 * @tc.name: Maximize01
 * @tc.desc: Maximize the main window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Maximize01, Function | SmallTest | Level3)
{
    auto option = new WindowOption();
    option->SetWindowName("WindowImplTest_Maximize01");
    auto window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Create("");
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    window->Show();
    EXPECT_CALL(m->Mock(), MaxmizeWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Maximize();
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetMode());
    EXPECT_CALL(m->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Hide();
}

/**
 * @tc.name: Maximize02
 * @tc.desc: Maximize the sub window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Maximize02, Function | SmallTest | Level3)
{
    auto option = new WindowOption();
    option->SetWindowName("WindowImplTest_Maximize02");
    auto window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Create("");
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    window->Show();
    window->Maximize();
    ASSERT_EQ(WindowMode::WINDOW_MODE_FLOATING, window->GetMode());
    EXPECT_CALL(m->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Hide();
}

/**
 * @tc.name: Recover01
 * @tc.desc: Recover the main window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Recover01, Function | SmallTest | Level3)
{
    auto option = new WindowOption();
    option->SetWindowName("WindowImplTest_Recover01");
    auto window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Create("");
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    window->Show();
    EXPECT_CALL(m->Mock(), UpdateProperty(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Recover();
    ASSERT_EQ(WindowMode::WINDOW_MODE_FLOATING, window->GetMode());
    EXPECT_CALL(m->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Hide();
}

/**
 * @tc.name: Recover02
 * @tc.desc: Recover the sub window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Recover02, Function | SmallTest | Level3)
{
    auto option = new WindowOption();
    option->SetWindowName("WindowImplTest_Recover02");
    auto window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Create("");
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    window->Show();
    window->Recover();
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetMode());
    EXPECT_CALL(m->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Hide();
}

/**
 * @tc.name: Minimize01
 * @tc.desc: Minimize the main window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Minimize01, Function | SmallTest | Level3)
{
    auto option = new WindowOption();
    option->SetWindowName("WindowImplTest_Minimize01");
    auto window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Create("");
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Show();
    ASSERT_TRUE(window->GetShowState());
    EXPECT_CALL(m->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Minimize();
    ASSERT_FALSE(window->GetShowState());
}

/**
 * @tc.name: Minimize02
 * @tc.desc: Minimize the sub window
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Minimize02, Function | SmallTest | Level3)
{
    auto option = new WindowOption();
    option->SetWindowName("WindowImplTest_Minimize02");
    auto window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Create("");
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    window->Show();
    ASSERT_TRUE(window->GetShowState());
    window->Minimize();
    ASSERT_TRUE(window->GetShowState());
}

/**
 * @tc.name: IsSupportWideGamut01
 * @tc.desc: IsSupportWideGamut
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, IsSupportWideGamut01, Function | SmallTest | Level3)
{
    auto option = new WindowOption();
    option->SetWindowName("WindowImplTest_IsSupportWideGamut01");
    auto window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Create("");
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    ASSERT_TRUE(window->IsSupportWideGamut());
}

/**
 * @tc.name: SetColorSpace01
 * @tc.desc: SetColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, SetColorSpace01, Function | SmallTest | Level3)
{
    auto option = new WindowOption();
    option->SetWindowName("WindowImplTest_SetColorSpace01");
    auto window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Create("");
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    window->SetColorSpace(ColorSpace::COLOR_SPACE_WIDE_GAMUT);
}

/**
 * @tc.name: GetColorSpace01
 * @tc.desc: GetColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, GetColorSpace01, Function | SmallTest | Level3)
{
    auto option = new WindowOption();
    option->SetWindowName("WindowImplTest_GetColorSpace01");
    auto window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Create("");
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    window->SetColorSpace(ColorSpace::COLOR_SPACE_DEFAULT);
    ASSERT_EQ(ColorSpace::COLOR_SPACE_DEFAULT, window->GetColorSpace());
    window->SetColorSpace(ColorSpace::COLOR_SPACE_WIDE_GAMUT);
    ASSERT_EQ(ColorSpace::COLOR_SPACE_WIDE_GAMUT, window->GetColorSpace());
}

/**
 * @tc.name: MoveTo01
 * @tc.desc: create window but not show, move window, test rect
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, MoveTo01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowImplTest_MoveTo01");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    Rect winRect = {10, 20, 30u, 40u}; // set window rect: 10, 20, 30, 40
    option->SetWindowRect(winRect);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(""));
    ASSERT_EQ(WindowMode::WINDOW_MODE_FLOATING, window->GetMode());
    ASSERT_FALSE(window->GetShowState());
    const float moveRatio = 0.5;
    Rect newRect = {winRect.posX_ * moveRatio, winRect.posY_ * moveRatio, winRect.width_, winRect.height_};
    window->MoveTo(newRect.posX_, newRect.posY_);
    ASSERT_EQ(newRect, window->GetRect());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Resize01
 * @tc.desc: create window but not show, resize window, test rect
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, Resize01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowImplTest_Resize01");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    Rect winRect = {10, 20, 30u, 40u}; // set window rect: 10, 20, 30, 40
    option->SetWindowRect(winRect);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(""));
    ASSERT_EQ(WindowMode::WINDOW_MODE_FLOATING, window->GetMode());
    ASSERT_FALSE(window->GetShowState());
    const float resizeRatio = 0.5;
    Rect newRect = {winRect.posX_, winRect.posY_, winRect.width_ * resizeRatio, winRect.height_ * resizeRatio};
    window->Resize(newRect.width_, newRect.height_);
    ASSERT_EQ(newRect, window->GetRect());
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: StartMove01
 * @tc.desc: start move main fullscreen window, test startMoveFlag
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, StartMove01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowImplTest_StartMove01");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(""));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Show();
    window->StartMove();
    ASSERT_FALSE(window->startMoveFlag_);
}

/**
 * @tc.name: StartMove02
 * @tc.desc: start move main fullscreen window, test startMoveFlag
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, StartMove02, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowImplTest_StartMove02");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(""));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Show();

    window->StartMove();
    ASSERT_TRUE(window->startMoveFlag_);
}

/**
 * @tc.name: StartMove03
 * @tc.desc: start move divider, test startMoveFlag
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplTest, StartMove03, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowImplTest_StartMove03");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowType(WindowType::WINDOW_TYPE_DOCK_SLICE);
    sptr<WindowImpl> window = new WindowImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();

    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(""));
    EXPECT_CALL(m->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Show();
    window->StartMove();
    ASSERT_FALSE(window->startMoveFlag_);
}
}
} // namespace Rosen
} // namespace OHOS
