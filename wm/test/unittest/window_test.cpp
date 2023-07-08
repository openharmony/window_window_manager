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
#include "window.h"
#include "mock_window_adapter.h"
#include "singleton_mocker.h"
#include "scene_board_judgement.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
class WindowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    static inline std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_;
};
void WindowTest::SetUpTestCase()
{
    abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();
}

void WindowTest::TearDownTestCase()
{
}

void WindowTest::SetUp()
{
}

void WindowTest::TearDown()
{
}

namespace {
/**
 * @tc.name: Create01
 * @tc.desc: Create window with no WindowName and no abilityToken
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, Create01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    ASSERT_EQ(nullptr, Window::Create("", option));
}

/**
 * @tc.name: Create02
 * @tc.desc: Create window with WindowName and no abilityToken
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, Create02, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    auto window = Window::Create("WindowTest02", option);
    ASSERT_NE(nullptr, window);
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Create03
 * @tc.desc: Mock CreateWindow return WM_ERROR_SAMGR, create window with WindowName and no abilityToken
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, Create03, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_SAMGR));
    ASSERT_EQ(nullptr, Window::Create("WindowTest03", option));
}

/**
 * @tc.name: Create04
 * @tc.desc: Create window with WindowName and no option
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, Create04, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = nullptr;
    ASSERT_EQ(nullptr, Window::Create("", option));
}

/**
 * @tc.name: Find01
 * @tc.desc: Find with no name
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, Find01, Function | SmallTest | Level2)
{
    ASSERT_EQ(nullptr, Window::Find(""));
}

/**
 * @tc.name: Find02
 * @tc.desc: Find with name
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, Find02, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    EXPECT_CALL(m->Mock(), GetSystemConfig(_)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    auto window = Window::Create("WindowTest03", option);
    ASSERT_NE(nullptr, window);
    ASSERT_NE(nullptr, Window::Find("WindowTest03"));
    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetSurfaceNode
 * @tc.desc: get node
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetSurfaceNode, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest04", option);
    ASSERT_NE(nullptr, window);
    ASSERT_NE(nullptr, window->GetSurfaceNode());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetContext
 * @tc.desc: get context
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetContext, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest05", option);
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(nullptr, window->GetContext());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetRect
 * @tc.desc: get rect
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetRect, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest06", option);
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(Rect(), window->GetRect());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetRequestRect
 * @tc.desc: get rect
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetRequestRect, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest07", option);
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(Rect(), window->GetRequestRect());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetType
 * @tc.desc: get type
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetType, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest08", option);
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW, window->GetType());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetMode
 * @tc.desc: get mode
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetMode, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest09", option);
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FLOATING, window->GetMode());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetAlpha
 * @tc.desc: get alpha
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetAlpha, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest10", option);
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(1.0f, window->GetAlpha());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetFocusable
 * @tc.desc: get focusable
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetFocusable, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest11", option);
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(true, window->GetFocusable());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetFocusable
 * @tc.desc: set Focusable
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetFocusable, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest12", option);
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(WMError::WM_OK, window->SetFocusable(true));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetTouchable
 * @tc.desc: get Touchable
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetTouchable, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest13", option);
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(true, window->GetTouchable());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetTouchable
 * @tc.desc: set Touchable
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetTouchable, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest14", option);
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(WMError::WM_OK, window->SetTouchable(true));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetSystemBarPropertyByType
 * @tc.desc: get SystemBarPropertyByType
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetSystemBarPropertyByType, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest15", option);
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(SystemBarProperty(), window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetSystemBarProperty
 * @tc.desc: set SystemBarProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetSystemBarProperty, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest16", option);
    SystemBarProperty prop;
    ASSERT_NE(nullptr, window);
    auto ret = window->SetSystemBarProperty(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW, prop);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IsFullScreen
 * @tc.desc: get FullScreen
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IsFullScreen, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest17", option);
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(false, window->IsFullScreen());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IsLayoutFullScreen
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IsLayoutFullScreen, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest18", option);
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(false, window->IsLayoutFullScreen());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetAlpha
 * @tc.desc: set
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetAlpha, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest19", option);
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(WMError::WM_OK, window->SetAlpha(0.0f));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetTransform
 * @tc.desc: set
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetTransform, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest20", option);
    ASSERT_NE(nullptr, window);
    Transform trans;
    ASSERT_EQ(WMError::WM_OK, window->SetTransform(trans));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetTransform
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetTransform, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest21", option);
    ASSERT_NE(nullptr, window);
    Transform trans;
    ASSERT_EQ(trans, window->GetTransform());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetAvoidAreaByType
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetAvoidAreaByType, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest22", option);
    ASSERT_NE(nullptr, window);
    AvoidArea avoidArea;
    auto ret = window->GetAvoidAreaByType(AvoidAreaType::TYPE_CUTOUT, avoidArea);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetLayoutFullScreen
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetLayoutFullScreen, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest23", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->SetLayoutFullScreen(true);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetFullScreen
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetFullScreen, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest24", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->SetFullScreen(true);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Destroy
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, Destroy, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest25", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->Destroy();
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Show
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, Show, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest26", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->Show();
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Hide
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, Hide, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest27", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->Hide();
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: MoveTo
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, MoveTo, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest28", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->MoveTo(0, 0);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Resize
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, Resize, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest29", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->Resize(0, 0);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetKeepScreenOn
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetKeepScreenOn, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest30", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->SetKeepScreenOn(true);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IsKeepScreenOn
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IsKeepScreenOn, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest31", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->IsKeepScreenOn();
    ASSERT_EQ(false, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetTurnScreenOn
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetTurnScreenOn, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest32", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->SetTurnScreenOn(true);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IsTurnScreenOn
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IsTurnScreenOn, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest33", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->IsTurnScreenOn();
    ASSERT_EQ(false, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetBackgroundColor
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetBackgroundColor, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest34", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->SetBackgroundColor("0x00000000");
    ASSERT_NE(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetTransparent
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetTransparent, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest35", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->SetTransparent(true);
    ASSERT_NE(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IsTransparent
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IsTransparent, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest36", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->IsTransparent();
    ASSERT_EQ(false, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetBrightness
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetBrightness, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest37", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->SetBrightness(0.0f);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetBrightness
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetBrightness, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest38", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->GetBrightness();
    ASSERT_EQ(-1.f, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetPrivacyMode
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetPrivacyMode, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest39", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->SetPrivacyMode(0.0f);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IsPrivacyMode
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IsPrivacyMode, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest40", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->IsPrivacyMode();
    ASSERT_EQ(false, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetSystemPrivacyMode
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetSystemPrivacyMode, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest41", option);
    ASSERT_NE(nullptr, window);
    auto ret = false;
    window->SetSystemPrivacyMode(true);
    ASSERT_EQ(false, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: BindDialogTarget
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, BindDialogTarget, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest42", option);
    ASSERT_NE(nullptr, window);
    sptr<IRemoteObject> targetToken;
    auto ret = window->BindDialogTarget(targetToken);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: RaiseToAppTop
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, RaiseToAppTop, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest43", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->RaiseToAppTop();
    ASSERT_NE(WmErrorCode::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetSnapshotSkip
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetSnapshotSkip, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest44", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->SetSnapshotSkip(true);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetCornerRadius
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetCornerRadius, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest45", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->SetCornerRadius(1.0f);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetShadowRadius
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetShadowRadius, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest46", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->SetShadowRadius(1.0f);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetShadowColor
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetShadowColor, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest47", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->SetShadowColor("0x00000000");
    ASSERT_NE(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetShadowOffsetX
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetShadowOffsetX, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest48", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->SetShadowOffsetX(0.0f);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetShadowOffsetY
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetShadowOffsetY, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest49", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->SetShadowOffsetY(0.0f);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetBlur
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetBlur, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest50", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->SetBlur(0.0f);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetBackdropBlur
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetBackdropBlur, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest51", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->SetBackdropBlur(0.0f);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetBackdropBlurStyle
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetBackdropBlurStyle, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest52", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->SetBackdropBlurStyle(WindowBlurStyle::WINDOW_BLUR_OFF);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: RequestFocus
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, RequestFocus, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest53", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->RequestFocus();
    ASSERT_NE(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IsFocused
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IsFocused, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest54", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->IsFocused();
    ASSERT_EQ(false, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: UpdateSurfaceNodeAfterCustomAnimation
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, UpdateSurfaceNodeAfterCustomAnimation, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest55", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->UpdateSurfaceNodeAfterCustomAnimation(false);
    ASSERT_NE(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetInputEventConsumer
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetInputEventConsumer, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest56", option);
    ASSERT_NE(nullptr, window);
    auto ret = true;
    std::shared_ptr<IInputEventConsumer> inputEventConsumer;
    window->SetInputEventConsumer(inputEventConsumer);
    ASSERT_EQ(true, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: ConsumeKeyEvent
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, ConsumeKeyEvent, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest57", option);
    ASSERT_NE(nullptr, window);
    auto ret = WMError::WM_OK;   
    std::shared_ptr<MMI::KeyEvent> keyEvent=MMI::KeyEvent::Create();
    window->ConsumeKeyEvent(keyEvent);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: ConsumePointerEvent
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, ConsumePointerEvent, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest58", option);
    ASSERT_NE(nullptr, window);
    auto ret = WMError::WM_OK;
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    window->ConsumePointerEvent(pointerEvent);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: RequestVsync
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, RequestVsync, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest59", option);
    ASSERT_NE(nullptr, window);
    std::shared_ptr<VsyncCallback> vsyncCallback;
    auto ret = WMError::WM_OK;
    window->RequestVsync(vsyncCallback);
    // no reture
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: UpdateConfiguration
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, UpdateConfiguration, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest60", option);
    ASSERT_NE(nullptr, window);
    std::shared_ptr<AppExecFwk::Configuration> conf;
    auto ret = WMError::WM_OK;
    window->UpdateConfiguration(conf);
    // no reture
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: RegisterLifeCycleListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, RegisterLifeCycleListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest61", option);
    ASSERT_NE(nullptr, window);
    sptr<IWindowLifeCycle> listener;
    auto ret = window->RegisterLifeCycleListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: UnregisterLifeCycleListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, UnregisterLifeCycleListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest62", option);
    ASSERT_NE(nullptr, window);
    sptr<IWindowLifeCycle> listener;
    auto ret = window->UnregisterLifeCycleListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: RegisterWindowChangeListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, RegisterWindowChangeListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest63", option);
    ASSERT_NE(nullptr, window);
    sptr<IWindowChangeListener> listener;
    auto ret = window->RegisterWindowChangeListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: UnregisterWindowChangeListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, UnregisterWindowChangeListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest64", option);
    ASSERT_NE(nullptr, window);
    sptr<IWindowChangeListener> listener;
    auto ret = window->UnregisterWindowChangeListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: RegisterAvoidAreaChangeListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, RegisterAvoidAreaChangeListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest65", option);
    ASSERT_NE(nullptr, window);
    sptr<IAvoidAreaChangedListener> listener;
    auto ret = window->RegisterAvoidAreaChangeListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: UnregisterAvoidAreaChangeListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, UnregisterAvoidAreaChangeListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest66", option);
    ASSERT_NE(nullptr, window);
    sptr<IAvoidAreaChangedListener> listener;
    auto ret = window->UnregisterAvoidAreaChangeListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: RegisterDragListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, RegisterDragListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest67", option);
    ASSERT_NE(nullptr, window);
    sptr<IWindowDragListener> listener;
    auto ret = window->RegisterDragListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: UnregisterDragListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, UnregisterDragListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest68", option);
    ASSERT_NE(nullptr, window);
    sptr<IWindowDragListener> listener;
    auto ret = window->UnregisterDragListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: RegisterDisplayMoveListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, RegisterDisplayMoveListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest69", option);
    ASSERT_NE(nullptr, window);
    sptr<IDisplayMoveListener> listener;
    auto ret = window->RegisterDisplayMoveListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: UnregisterDisplayMoveListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, UnregisterDisplayMoveListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest70", option);
    ASSERT_NE(nullptr, window);
    sptr<IDisplayMoveListener> listener;
    auto ret = window->UnregisterDisplayMoveListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: RegisterWindowDestroyedListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, RegisterWindowDestroyedListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest71", option);
    ASSERT_NE(nullptr, window);
    NotifyNativeWinDestroyFunc func;
    auto ret = WMError::WM_OK;
    window->RegisterWindowDestroyedListener(func);
    // no reture
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: RegisterOccupiedAreaChangeListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, RegisterOccupiedAreaChangeListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest72", option);
    ASSERT_NE(nullptr, window);
    sptr<IOccupiedAreaChangeListener> listener;
    auto ret = window->RegisterOccupiedAreaChangeListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: UnregisterOccupiedAreaChangeListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, UnregisterOccupiedAreaChangeListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest73", option);
    ASSERT_NE(nullptr, window);
    sptr<IOccupiedAreaChangeListener> listener;
    auto ret = window->UnregisterOccupiedAreaChangeListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: RegisterTouchOutsideListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, RegisterTouchOutsideListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest74", option);
    ASSERT_NE(nullptr, window);
    sptr<ITouchOutsideListener> listener;
    auto ret = window->RegisterTouchOutsideListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: UnregisterTouchOutsideListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, UnregisterTouchOutsideListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest75", option);
    ASSERT_NE(nullptr, window);
    sptr<ITouchOutsideListener> listener;
    auto ret = window->UnregisterTouchOutsideListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: RegisterAnimationTransitionController
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, RegisterAnimationTransitionController, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest76", option);
    ASSERT_NE(nullptr, window);
    sptr<IAnimationTransitionController> listener;
    auto ret = window->RegisterAnimationTransitionController(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: RegisterScreenshotListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, RegisterScreenshotListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest77", option);
    ASSERT_NE(nullptr, window);
    sptr<IScreenshotListener> listener;
    auto ret = window->RegisterScreenshotListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: UnregisterScreenshotListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, UnregisterScreenshotListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest78", option);
    ASSERT_NE(nullptr, window);
    sptr<IScreenshotListener> listener;
    auto ret = window->UnregisterScreenshotListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: RegisterDialogTargetTouchListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, RegisterDialogTargetTouchListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest79", option);
    ASSERT_NE(nullptr, window);
    sptr<IDialogTargetTouchListener> listener;
    auto ret = window->RegisterDialogTargetTouchListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: UnregisterDialogTargetTouchListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, UnregisterDialogTargetTouchListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest80", option);
    ASSERT_NE(nullptr, window);
    sptr<IDialogTargetTouchListener> listener;
    auto ret = window->UnregisterDialogTargetTouchListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: RegisterDialogDeathRecipientListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, RegisterDialogDeathRecipientListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest81", option);
    ASSERT_NE(nullptr, window);
    auto ret = WMError::WM_OK;
    sptr<IDialogDeathRecipientListener> listener;
    window->RegisterDialogDeathRecipientListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: UnregisterDialogDeathRecipientListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, UnregisterDialogDeathRecipientListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest82", option);
    ASSERT_NE(nullptr, window);
    auto ret = WMError::WM_OK;
    sptr<IDialogDeathRecipientListener> listener;
    window->UnregisterDialogDeathRecipientListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: NotifyTouchDialogTarget
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, NotifyTouchDialogTarget, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest83", option);
    ASSERT_NE(nullptr, window);
    auto ret = WMError::WM_OK;
    sptr<IDialogTargetTouchListener> listener;
    window->NotifyTouchDialogTarget();
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetAceAbilityHandler
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetAceAbilityHandler, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest84", option);
    ASSERT_NE(nullptr, window);
    auto ret = WMError::WM_OK;
    sptr<IAceAbilityHandler> handler;
    window->SetAceAbilityHandler(handler);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetUIContent
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetUIContent, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest85", option);
    ASSERT_NE(nullptr, window);
    NativeEngine* engine = nullptr;
    NativeValue* storage = nullptr;
    auto ret = window->SetUIContent("info", engine, storage);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetContentInfo
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetContentInfo, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest86", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->GetContentInfo();
    ASSERT_EQ(std::string(), ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetUIContent
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetUIContent, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest87", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->GetUIContent();
    ASSERT_EQ(nullptr, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: OnNewWant
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, OnNewWant, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest88", option);
    ASSERT_NE(nullptr, window);
    AAFwk::Want want;
    auto ret = true;
    window->OnNewWant(want);
    ASSERT_EQ(true, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetRequestedOrientation
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetRequestedOrientation, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest89", option);
    ASSERT_NE(nullptr, window);
    auto ret = true;
    Orientation ori = Orientation::UNSPECIFIED;
    window->SetRequestedOrientation(ori);
    ASSERT_EQ(true, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetRequestedOrientation
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetRequestedOrientation, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest90", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->GetRequestedOrientation();
    ASSERT_EQ(Orientation::UNSPECIFIED, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetRequestModeSupportInfo
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetRequestModeSupportInfo, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest91", option);
    ASSERT_NE(nullptr, window);
    uint32_t modeSupportInfo = 0;
    window->SetRequestModeSupportInfo(modeSupportInfo);
    ASSERT_EQ(static_cast<uint32_t>(Orientation::UNSPECIFIED), modeSupportInfo);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetRequestModeSupportInfo
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetRequestModeSupportInfo, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest92", option);
    ASSERT_NE(nullptr, window);
    uint32_t ret = window->GetRequestModeSupportInfo();
    ASSERT_EQ(true, ret == 0);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetTouchHotAreas
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetTouchHotAreas, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest93", option);
    ASSERT_NE(nullptr, window);
    std::vector<Rect> rects;
    auto ret = window->SetTouchHotAreas(rects);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetRequestedTouchHotAreas
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetRequestedTouchHotAreas, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest94", option);
    ASSERT_NE(nullptr, window);
    std::vector<Rect> rects;
    auto ret = WMError::WM_OK;
    window->GetRequestedTouchHotAreas(rects);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IsMainHandlerAvailable
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IsMainHandlerAvailable, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest95", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->IsMainHandlerAvailable();
    ASSERT_EQ(false, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetAPPWindowLabel
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetAPPWindowLabel, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest96", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->SetAPPWindowLabel("");
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IsDecorEnable
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IsDecorEnable, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest97", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->IsDecorEnable();
    ASSERT_EQ(false, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Maximize
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, Maximize, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest98", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->Maximize();
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: MaximizeFloating
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, MaximizeFloating, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTest99", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->MaximizeFloating();
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Minimize
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, Minimize, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTes100", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->Minimize();
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Recover
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, Recover, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTes100", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->Recover();
    ASSERT_EQ(true, ret == WMError::WM_OK);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Close
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, Close, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTes102", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->Close();
    ASSERT_EQ(true, ret == WMError::WM_OK);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: StartMove
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, StartMove, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTes103", option);
    ASSERT_NE(nullptr, window);
    auto ret = WMError::WM_OK;
    window->StartMove();
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetGlobalMaximizeMode
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetGlobalMaximizeMode, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTes104", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->SetGlobalMaximizeMode(MaximizeMode::MODE_AVOID_SYSTEM_BAR);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetGlobalMaximizeMode
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetGlobalMaximizeMode, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTes105", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->GetGlobalMaximizeMode();
    ASSERT_EQ(MaximizeMode::MODE_FULL_FILL, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IsSupportWideGamut
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IsSupportWideGamut, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTes106", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->IsSupportWideGamut();
    ASSERT_EQ(false, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetColorSpace
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetColorSpace, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTes107", option);
    ASSERT_NE(nullptr, window);
    bool ret = true;
    window->SetColorSpace(ColorSpace::COLOR_SPACE_DEFAULT);
    ASSERT_EQ(true, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetColorSpace
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetColorSpace, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTes108", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->GetColorSpace();
    ASSERT_EQ(ColorSpace::COLOR_SPACE_DEFAULT, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: DumpInfo
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, DumpInfo, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTes109", option);
    ASSERT_NE(nullptr, window);
    std::vector<std::string> params;
    std::vector<std::string> info;
    auto ret = true;
    window->DumpInfo(params, info);
    ASSERT_EQ(true, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Snapshot
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, Snapshot, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTes110", option);
    ASSERT_NE(nullptr, window);
    auto pixmap = window->Snapshot();
    ASSERT_EQ(pixmap, nullptr);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: NotifyMemoryLevel
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, NotifyMemoryLevel, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTes111", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->NotifyMemoryLevel(0);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IsAllowHaveSystemSubWindow
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IsAllowHaveSystemSubWindow, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTes112", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->IsAllowHaveSystemSubWindow();
    ASSERT_EQ(false, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetAspectRatio
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetAspectRatio, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTes113", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->SetAspectRatio(0.0f);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: ResetAspectRatio
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, ResetAspectRatio, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTes114", option);
    ASSERT_NE(nullptr, window);
    auto ret = window->ResetAspectRatio();
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetKeyboardAnimationConfig
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetKeyboardAnimationConfig, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTes115", option);
    ASSERT_NE(nullptr, window);
    KeyboardAnimationConfig config;
    auto ret = window->GetKeyboardAnimationConfig();
    ASSERT_EQ(true, ret.durationIn_ == config.durationIn_);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetNeedDefaultAnimation
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetNeedDefaultAnimation, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTes116", option);
    ASSERT_NE(nullptr, window);
    auto ret = true;
    window->SetNeedDefaultAnimation(true);
    ASSERT_EQ(true, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: TransferAbilityResult
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, TransferAbilityResult, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTes117", option);
    ASSERT_NE(nullptr, window);
    AAFwk::Want want;
    auto ret = window->TransferAbilityResult(0, want);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: TransferExtensionData
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, TransferExtensionData, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTes118", option);
    ASSERT_NE(nullptr, window);
    AAFwk::WantParams wantParams;
    auto ret = window->TransferExtensionData(wantParams);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: RegisterTransferComponentDataListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, RegisterTransferComponentDataListener, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    auto window = Window::Create("WindowTes119", option);
    ASSERT_NE(nullptr, window);
    NotifyTransferComponentDataFunc func;
    auto ret = true;
    window->RegisterTransferComponentDataListener(func);
    ASSERT_EQ(true, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: WindowChangeListener
 * @tc.desc: WindowChangeListener01 fun
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, WindowChangeListener01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    auto window = Window::Create("WindowChangeListener01", option);
    ASSERT_NE(nullptr, window);
    auto ret = true;
    sptr<IWindowChangeListener> listener = new IWindowChangeListener();
    window->RegisterWindowChangeListener(listener);
    listener->OnModeChange(WindowMode::WINDOW_MODE_UNDEFINED, false);
    window->UnregisterWindowChangeListener(listener);
    ASSERT_EQ(true, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IOccupiedAreaChangeListener
 * @tc.desc: IOccupiedAreaChangeListener fun
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IOccupiedAreaChangeListener, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    auto window = Window::Create("IOccupiedAreaChangeListener", option);
    ASSERT_NE(nullptr, window);
    auto ret = true;
    sptr<IOccupiedAreaChangeListener> listener = new IOccupiedAreaChangeListener();
    Rect rect_ = {0, 0, 0, 0};
    window->RegisterOccupiedAreaChangeListener(listener);
    sptr<OccupiedAreaChangeInfo> info = new OccupiedAreaChangeInfo(OccupiedAreaType::TYPE_INPUT, rect_, 80);
    listener->OnSizeChange(info, nullptr);
    window->UnregisterOccupiedAreaChangeListener(listener);
    ASSERT_EQ(true, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: WindowChangeListener
 * @tc.desc: WindowChangeListener02 fun
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, WindowChangeListener02, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    auto window = Window::Create("WindowChangeListener02", option);
    ASSERT_NE(nullptr, window);
    auto ret = true;
    sptr<IWindowChangeListener> listener = new IWindowChangeListener();
    window->RegisterWindowChangeListener(listener);
    Rect rect_ = {0, 0, 0, 0};
    std::shared_ptr<RSTransaction> rstransaction;
    listener->OnSizeChange(rect_, WindowSizeChangeReason::UNDEFINED, rstransaction);
    window->UnregisterWindowChangeListener(listener);
    ASSERT_EQ(true, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IAnimationTransitionController
 * @tc.desc: IAnimationTransitionController fun
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IAnimationTransitionController, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    auto window = Window::Create("IAnimationTransitionController", option);
    ASSERT_NE(nullptr, window);
    auto ret = true;
    sptr<IAnimationTransitionController> listener = new IAnimationTransitionController();
    window->RegisterAnimationTransitionController(listener);
    listener->AnimationForShown();
    listener->AnimationForHidden();
    ASSERT_EQ(true, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IInputEventConsumer
 * @tc.desc: IInputEventConsumer fun
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IInputEventConsumer, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    auto window = Window::Create("IInputEventConsumer", option);
    ASSERT_NE(nullptr, window);
    auto ret = true;
    std::shared_ptr<IInputEventConsumer> listener = std::make_shared<IInputEventConsumer>();
    std::shared_ptr<MMI::KeyEvent> keyEvent;
    std::shared_ptr<MMI::PointerEvent> pointerEvent;
    std::shared_ptr<MMI::AxisEvent> axisEvent;
    listener->OnInputEvent(keyEvent);
    listener->OnInputEvent(pointerEvent);
    listener->OnInputEvent(axisEvent);
    ASSERT_EQ(true, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IDialogDeathRecipientListener
 * @tc.desc: IDialogDeathRecipientListener fun
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IDialogDeathRecipientListener, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    auto window = Window::Create("IDialogDeathRecipientListener", option);
    ASSERT_NE(nullptr, window);
    auto ret = true;
    sptr<IDialogDeathRecipientListener> listener = new IDialogDeathRecipientListener();
    Rect rect_ = {0, 0, 0, 0};
    sptr<OccupiedAreaChangeInfo> info = new OccupiedAreaChangeInfo(OccupiedAreaType::TYPE_INPUT, rect_, 80);
    listener->OnDialogDeathRecipient();
    ASSERT_EQ(true, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IAceAbilityHandler
 * @tc.desc: IAceAbilityHandler fun
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IAceAbilityHandler, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    auto window = Window::Create("IAceAbilityHandler", option);
    ASSERT_NE(nullptr, window);
    auto ret = true;
    sptr<IAceAbilityHandler> listener = new IAceAbilityHandler();
    uint32_t color = 66;
    listener->SetBackgroundColor(color);
    listener->GetBackgroundColor();
    ASSERT_EQ(true, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IDispatchInputEventListener
 * @tc.desc: IDispatchInputEventListener fun
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IDispatchInputEventListener, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    auto window = Window::Create("IDispatchInputEventListener", option);
    ASSERT_NE(nullptr, window);
    auto ret = true;
    sptr<IDispatchInputEventListener> listener = new IDispatchInputEventListener();
    std::shared_ptr<MMI::KeyEvent> keyEvent;
    std::shared_ptr<MMI::PointerEvent> pointerEvent;
    std::shared_ptr<MMI::AxisEvent> axisEvent;
    listener->OnDispatchPointerEvent(pointerEvent);
    listener->OnDispatchKeyEvent(keyEvent);
    ASSERT_EQ(true, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IsWindowSessionEnabled
 * @tc.desc: IsWindowSessionEnabled fun
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IsWindowSessionEnabled, Function | SmallTest | Level3)
{
    ASSERT_EQ(false, Rosen::SceneBoardJudgement::IsWindowSessionEnabled());
}
}
} // namespace Rosen
} // namespace OHOS
