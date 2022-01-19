/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
    EXPECT_CALL(m_->Mock(), CreateWindow(_, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
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
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowImplTest, CreateWindow01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateWindow01");
    sptr<WindowImpl> window = new WindowImpl(option);

    EXPECT_CALL(m_->Mock(), CreateWindow(_, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(""));

    EXPECT_CALL(m_->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: CreateWindow02
 * @tc.desc: Create window with no parentName and no abilityContext
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowImplTest, CreateWindow02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateWindow02");
    sptr<WindowImpl> window = new WindowImpl(option);

    EXPECT_CALL(m_->Mock(), CreateWindow(_, _, _, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_SAMGR));
    ASSERT_EQ(WMError::WM_ERROR_SAMGR, window->Create(""));
}

/**
 * @tc.name: CreateWindow03
 * @tc.desc: Create window with illegal parentName
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
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
 * @tc.require: AR000GGTVJ
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
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowImplTest, CreateWindow05, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateWindow05");
    sptr<WindowImpl> window = new WindowImpl(option);

    EXPECT_CALL(m_->Mock(), CreateWindow(_, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create("WindowImplTest"));

    EXPECT_CALL(m_->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: CreateWindow06
 * @tc.desc: Create window with no default option, get and check Property
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
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

    EXPECT_CALL(m_->Mock(), CreateWindow(_, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
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
 * @tc.name: CreateWindow07
 * @tc.desc: Create window with no parentName and abilityContext
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowImplTest, CreateWindow07, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateWindow07");
    sptr<WindowImpl> window = new WindowImpl(option);

    EXPECT_CALL(m_->Mock(), SaveAbilityToken(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m_->Mock(), CreateWindow(_, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create("", abilityContext_));
}

/**
 * @tc.name: CreateWindow08
 * @tc.desc: Mock SaveAbilityToken return WM_ERROR_NULLPTR, create window with no parentName and abilityContext
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowImplTest, CreateWindow08, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("CreateWindow08");
    sptr<WindowImpl> window = new WindowImpl(option);

    EXPECT_CALL(m_->Mock(), SaveAbilityToken(_, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_NULLPTR));
    EXPECT_CALL(m_->Mock(), CreateWindow(_, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->Create("", abilityContext_));
}

/**
 * @tc.name: FindWindow01
 * @tc.desc: Find one exit window
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowImplTest, FindWindow01, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, WindowImpl::Find("WindowImplTest"));
}

/**
 * @tc.name: FindWindow02
 * @tc.desc: Add another window, find both two windows
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowImplTest, FindWindow02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("FindWindow02");
    sptr<WindowImpl> window = new WindowImpl(option);

    EXPECT_CALL(m_->Mock(), CreateWindow(_, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));

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
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowImplTest, FindWindow03, Function | SmallTest | Level2)
{
    ASSERT_EQ(nullptr, WindowImpl::Find("FindWindow03"));
}

/**
 * @tc.name: FindWindow04
 * @tc.desc: Find window with empty name
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowImplTest, FindWindow04, Function | SmallTest | Level2)
{
    ASSERT_EQ(nullptr, WindowImpl::Find(""));
}

/**
 * @tc.name: FindWindow05
 * @tc.desc: Find one destroyed window
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowImplTest, FindWindow05, Function | SmallTest | Level2)
{
    ASSERT_EQ(nullptr, WindowImpl::Find("FindWindow02"));
}

/**
 * @tc.name: SetWindowType01
 * @tc.desc: SetWindowType
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowImplTest, SetWindowType01, Function | SmallTest | Level2)
{
    ASSERT_EQ(WMError::WM_OK, window_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW));
}

/**
 * @tc.name: SetWindowMode01
 * @tc.desc: SetWindowMode
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowImplTest, SetWindowMode01, Function | SmallTest | Level2)
{
    ASSERT_EQ(WMError::WM_OK, window_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
}

/**
 * @tc.name: SetWindowMode02
 * @tc.desc: Set window mode to split primary
 * @tc.type: FUNC
 * @tc.require: AR000GGTV7
 */
HWTEST_F(WindowImplTest, SetWindowMode02, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
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
 * @tc.require: AR000GGTV7
 */
HWTEST_F(WindowImplTest, SetWindowMode03, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
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
 * @tc.require: AR000GGTV7
 */
HWTEST_F(WindowImplTest, SetWindowMode04, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
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
 * @tc.require: AR000GGTV7
 */
HWTEST_F(WindowImplTest, SetWindowMode05, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
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
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowImplTest, ShowHideWindow01, Function | SmallTest | Level2)
{
    EXPECT_CALL(m_->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    EXPECT_CALL(m_->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: ShowHideWindow02
 * @tc.desc: Show window with add window WM_ERROR_SAMGR
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowImplTest, ShowHideWindow02, Function | SmallTest | Level2)
{
    EXPECT_CALL(m_->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_SAMGR));
    ASSERT_EQ(WMError::WM_ERROR_SAMGR, window_->Show());
}

/**
 * @tc.name: ShowHideWindow03
 * @tc.desc: Show window with add window WM_ERROR_IPC_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowImplTest, ShowHideWindow03, Function | SmallTest | Level3)
{
    EXPECT_CALL(m_->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_IPC_FAILED));
    ASSERT_EQ(WMError::WM_ERROR_IPC_FAILED, window_->Show());
}

/**
 * @tc.name: ShowHideWindow04
 * @tc.desc: Show window with add window OK & Hide window with remove window WM_ERROR_SAMGR
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowImplTest, ShowHideWindow04, Function | SmallTest | Level3)
{
    EXPECT_CALL(m_->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window_->Show());
    EXPECT_CALL(m_->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_SAMGR));
    ASSERT_EQ(WMError::WM_ERROR_SAMGR, window_->Hide());
}

/**
 * @tc.name: ShowHideWindow05
 * @tc.desc: Hide window with remove window WM_ERROR_IPC_FAILED
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowImplTest, ShowHideWindow05, Function | SmallTest | Level3)
{
    EXPECT_CALL(m_->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_IPC_FAILED));
    ASSERT_EQ(WMError::WM_ERROR_IPC_FAILED, window_->Hide());
}

/**
 * @tc.name: ShowHideWindow06
 * @tc.desc: Hide window with remove window OK
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowImplTest, ShowHideWindow06, Function | SmallTest | Level3)
{
    EXPECT_CALL(m_->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window_->Hide());
}

/**
 * @tc.name: SetSystemBarProperty01
 * @tc.desc: SetSystemBarProperty with default param
 * @tc.type: FUNC
 * @tc.require: AR000GGTVD
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
 * @tc.require: AR000GGTVD
 */
HWTEST_F(WindowImplTest, SetSystemBarProperty02, Function | SmallTest | Level3)
{
    EXPECT_CALL(m_->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window_->Show();
    EXPECT_CALL(m_->Mock(), SetSystemBarProperty(_, _, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_SAMGR));
    WindowType type = WindowType::WINDOW_TYPE_STATUS_BAR;
    const SystemBarProperty SYS_BAR_PROP(false, 0xE5222222, 0xE5333333);
    ASSERT_EQ(WMError::WM_ERROR_SAMGR, window_->SetSystemBarProperty(type, SYS_BAR_PROP));
    EXPECT_CALL(m_->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_IPC_FAILED));
    window_->Hide();
}

/**
 * @tc.name: SetSystemBarProperty03
 * @tc.desc: SetSystemBarProperty to invalid window
 * @tc.type: FUNC
 * @tc.require: AR000GGTVD
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
 * @tc.require: AR000GGTVD
 */
HWTEST_F(WindowImplTest, GetSystemBarPropertyByType01, Function | SmallTest | Level3)
{
    EXPECT_CALL(m_->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window_->Show();
    EXPECT_CALL(m_->Mock(), SetSystemBarProperty(_, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    WindowType type = WindowType::WINDOW_TYPE_STATUS_BAR;
    const SystemBarProperty SYS_BAR_PROP(false, 0xE5222222, 0xE5333333);
    ASSERT_EQ(WMError::WM_OK, window_->SetSystemBarProperty(type, SYS_BAR_PROP));
    ASSERT_EQ(SYS_BAR_PROP, window_->GetSystemBarPropertyByType(type));
    EXPECT_CALL(m_->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_IPC_FAILED));
    window_->Hide();
}

/**
 * @tc.name: GetSystemBarPropertyByType02
 * @tc.desc: GetSystemBarPropertyByType with nonexist key
 * @tc.type: FUNC
 * @tc.require: AR000GGTVD
 */
HWTEST_F(WindowImplTest, GetSystemBarPropertyByType02, Function | SmallTest | Level3)
{
    EXPECT_CALL(m_->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window_->Show();
    EXPECT_CALL(m_->Mock(), SetSystemBarProperty(_, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    const SystemBarProperty SYS_BAR_PROP(false, 0xE5222222, 0xE5333333);
    const SystemBarProperty DEFAULT_PROP;
    ASSERT_EQ(WMError::WM_OK, window_->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, SYS_BAR_PROP));
    ASSERT_EQ(DEFAULT_PROP, window_->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_BAR));
    EXPECT_CALL(m_->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_IPC_FAILED));
    window_->Hide();
}

/**
 * @tc.name: GetSystemBarPropertyByType03
 * @tc.desc: GetSystemBarPropertyByType with not systemBar type
 * @tc.type: FUNC
 * @tc.require: AR000GGTVD
 */
HWTEST_F(WindowImplTest, GetSystemBarPropertyByType03, Function | SmallTest | Level3)
{
    EXPECT_CALL(m_->Mock(), AddWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window_->Show();
    EXPECT_CALL(m_->Mock(), SetSystemBarProperty(_, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    const SystemBarProperty SYS_BAR_PROP(false, 0xE5222222, 0xE5333333);
    const SystemBarProperty DEFAULT_PROP;
    ASSERT_EQ(WMError::WM_OK, window_->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, SYS_BAR_PROP));
    ASSERT_EQ(DEFAULT_PROP, window_->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW));
    EXPECT_CALL(m_->Mock(), RemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_IPC_FAILED));
    window_->Hide();
}
}
} // namespace Rosen
} // namespace OHOS
