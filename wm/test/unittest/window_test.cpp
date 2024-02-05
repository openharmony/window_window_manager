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
#include "key_event.h"
#include "accessibility_event_info.h"

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
    auto window = Window::Create("WindowTest02", option);
    if (window != nullptr)
    {
        ASSERT_NE(nullptr, window);
    }
    if (window != nullptr)
    {
        ASSERT_EQ(WMError::WM_OK, window->Destroy());
    }
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
    auto window = Window::Create("WindowTest03", option);
    if (window != nullptr) {
        ASSERT_EQ(nullptr, Window::Create("WindowTest03", option));
    }
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

    auto window = Window::Create("WindowTest03", option);
    if (window != nullptr) {
        ASSERT_NE(nullptr, window);
    }
    if (Window::Find("WindowTest03") != nullptr) {
        ASSERT_NE(nullptr, Window::Find("WindowTest03"));
    }

    if (window != nullptr) {
        ASSERT_EQ(WMError::WM_OK, window->Destroy());
    }
}

/**
 * @tc.name: GetSurfaceNode
 * @tc.desc: get node
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetSurfaceNode, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(nullptr, window->GetSurfaceNode());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetContext
 * @tc.desc: get context
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetContext, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(WindowMode::WINDOW_MODE_UNDEFINED, window->GetMode());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
    
    auto window_ = new (std::nothrow)Window();
    ASSERT_NE(nullptr, window_);
    ASSERT_EQ(WindowMode::WINDOW_MODE_UNDEFINED, window_->GetMode());
}

/**
 * @tc.name: GetAlpha
 * @tc.desc: get alpha
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetAlpha, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(0.0f, window->GetAlpha());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetFocusable
 * @tc.desc: get focusable
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetFocusable, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(false, window->GetFocusable());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetFocusable
 * @tc.desc: set Focusable
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetFocusable, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(false, window->GetTouchable());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetTouchable
 * @tc.desc: set Touchable
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetTouchable, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    auto ret = window->SetBackgroundColor("0x00000000");
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetTransparent
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetTransparent, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    auto ret = window->SetTransparent(true);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IsTransparent
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IsTransparent, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    auto ret = window->GetBrightness();
    ASSERT_EQ(0.0f, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetPrivacyMode
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetPrivacyMode, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    auto ret = window->RaiseToAppTop();
    ASSERT_EQ(WmErrorCode::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetSnapshotSkip
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetSnapshotSkip, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    auto ret = window->SetShadowColor("0x00000000");
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetShadowOffsetX
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetShadowOffsetX, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    auto ret = window->RequestFocus();
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IsFocused
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IsFocused, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    auto ret = window->UpdateSurfaceNodeAfterCustomAnimation(false);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetInputEventConsumer
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetInputEventConsumer, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    auto ret = WMError::WM_OK;
    std::shared_ptr<MMI::KeyEvent> inputEvent = nullptr;
    window->ConsumeKeyEvent(inputEvent);
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    auto ret = WMError::WM_OK;
    std::shared_ptr<MMI::PointerEvent> inputEvent = nullptr;
    window->ConsumePointerEvent(inputEvent);
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    std::shared_ptr<VsyncCallback> vsyncCallback = nullptr;
    auto ret = WMError::WM_OK;
    window->RequestVsync(vsyncCallback);
    // no return
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    std::shared_ptr<AppExecFwk::Configuration> conf = nullptr;
    auto ret = WMError::WM_OK;
    window->UpdateConfiguration(conf);
    // no return
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    sptr<IWindowLifeCycle> listener = nullptr;
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    sptr<IWindowLifeCycle> listener = nullptr;
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    sptr<IWindowChangeListener> listener = nullptr;
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    sptr<IWindowChangeListener> listener = nullptr;
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    sptr<IAvoidAreaChangedListener> listener = nullptr;
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    sptr<IAvoidAreaChangedListener> listener = nullptr;
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    sptr<IWindowDragListener> listener = nullptr;
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    sptr<IWindowDragListener> listener = nullptr;
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    sptr<IDisplayMoveListener> listener = nullptr;
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    sptr<IDisplayMoveListener> listener = nullptr;
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    NotifyNativeWinDestroyFunc func = nullptr;
    auto ret = WMError::WM_OK;
    window->RegisterWindowDestroyedListener(func);
    // no return
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    sptr<IOccupiedAreaChangeListener> listener = nullptr;
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    sptr<IOccupiedAreaChangeListener> listener = nullptr;
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    sptr<ITouchOutsideListener> listener = nullptr;
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    sptr<ITouchOutsideListener> listener = nullptr;
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    sptr<IAnimationTransitionController> listener = nullptr;
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    sptr<IScreenshotListener> listener = nullptr;
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    sptr<IScreenshotListener> listener = nullptr;
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    sptr<IDialogTargetTouchListener> listener = nullptr;
    auto ret = window->RegisterDialogTargetTouchListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());

    auto window_ = new (std::nothrow)Window();
    ASSERT_NE(nullptr, window_);
    sptr<IDialogTargetTouchListener> listener_;
    auto ret_ = window_->RegisterDialogTargetTouchListener(listener_);
    ASSERT_EQ(WMError::WM_OK, ret_);
    ASSERT_EQ(WMError::WM_OK, window_->Destroy());
}

/**
 * @tc.name: UnregisterDialogTargetTouchListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, UnregisterDialogTargetTouchListener, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    sptr<IDialogTargetTouchListener> listener = nullptr;
    auto ret = window->UnregisterDialogTargetTouchListener(listener);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());

    auto window_ = new (std::nothrow)Window();
    ASSERT_NE(nullptr, window_);
    ASSERT_EQ(WMError::WM_OK, window_->UnregisterDialogTargetTouchListener(listener));
    ASSERT_EQ(WMError::WM_OK, window_->Destroy());
}

/**
 * @tc.name: RegisterDialogDeathRecipientListener
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, RegisterDialogDeathRecipientListener, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    auto ret = WMError::WM_OK;
    sptr<IDialogDeathRecipientListener> listener = nullptr;
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    auto ret = WMError::WM_OK;
    sptr<IDialogDeathRecipientListener> listener = nullptr;
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    auto ret = WMError::WM_OK;
    sptr<IDialogTargetTouchListener> listener = nullptr;
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    auto ret = WMError::WM_OK;
    sptr<IAceAbilityHandler> handler = nullptr;
    window->SetAceAbilityHandler(handler);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: NapiSetUIContent
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, NapiSetUIContent, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    napi_env env = nullptr;
    napi_value storage = nullptr;
    auto ret = window->NapiSetUIContent("info", env, storage);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetUIContentByAbc
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetUIContentByAbc, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    napi_env env = nullptr;
    napi_value storage = nullptr;
    auto ret = window->SetUIContentByAbc("/system/etc/window/resources/test.abc", env, storage);
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
    sptr<WindowOption> option = new (std::nothrow)WindowOption();
    option->SetMainHandlerAvailable(false);
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    auto ret = window->SetAPPWindowLabel("");
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());

    auto window_ = new (std::nothrow)Window();
    ASSERT_NE(nullptr, window_);
    ASSERT_EQ(WMError::WM_OK,  window_->SetAPPWindowLabel("000111"));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: IsDecorEnable
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IsDecorEnable, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    auto ret = window->Minimize();
    ASSERT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Recover
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, Recover, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    auto ret = window->Recover();
    ASSERT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Close
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, Close, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    auto ret = window->NotifyMemoryLevel(0);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());

    auto window_ = new (std::nothrow) Window();
    ASSERT_NE(nullptr, window_);
    ASSERT_EQ(WMError::WM_OK, window_->NotifyMemoryLevel(22));
    ASSERT_EQ(WMError::WM_OK, window_->Destroy());
}

/**
 * @tc.name: IsAllowHaveSystemSubWindow
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, IsAllowHaveSystemSubWindow, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    window->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    auto ret = window->SetAspectRatio(0.0f);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());

    auto window_ = new (std::nothrow) Window();
    ASSERT_NE(nullptr, window_);
    ASSERT_EQ(WMError::WM_OK, window_->SetAspectRatio(0.1f));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: ResetAspectRatio
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, ResetAspectRatio, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    auto ret = true;
    std::shared_ptr<IInputEventConsumer> listener = std::make_shared<IInputEventConsumer>();
    std::shared_ptr<MMI::KeyEvent> keyEvent = nullptr;
    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    std::shared_ptr<MMI::AxisEvent> axisEvent = nullptr;
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
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
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    auto ret = true;
    sptr<IDispatchInputEventListener> listener = new IDispatchInputEventListener();
    std::shared_ptr<MMI::KeyEvent> keyEvent = nullptr;
    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    std::shared_ptr<MMI::AxisEvent> axisEvent = nullptr;
    listener->OnDispatchPointerEvent(pointerEvent);
    listener->OnDispatchKeyEvent(keyEvent);
    ASSERT_EQ(true, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: Marshalling
 * @tc.desc: keyboardAnimationConfig marshalling
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, KeyboardAnimationConfigMarshalling, Function | SmallTest | Level3)
{
    MessageParcel data;
    KeyboardAnimationConfig config;
    auto ret = data.WriteParcelable(&config);
    Parcel parcel;
    config.Unmarshalling(parcel);
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: BackgroundFailed
 * @tc.desc: window life cycle BackgroundFailed
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, WindowLifeCycleBackgroundFailed, Function | SmallTest | Level3)
{
    IWindowLifeCycle windowLifeCycle;
    int32_t  ret = 0;
    windowLifeCycle.BackgroundFailed(ret);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: GetVSyncPeriod
 * @tc.desc: window GetVSyncPeriod
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetVSyncPeriod, Function | SmallTest | Level3)
{
    sptr<WindowOption> winOption = nullptr;
    winOption = new (std::nothrow) OHOS::Rosen::WindowOption();
    ASSERT_NE(nullptr, winOption);
    winOption->SetWindowType(OHOS::Rosen::WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);

    sptr<WindowOption> option = new WindowOption;
    sptr<Window> window = Window::Create("win", option);
    if (window != nullptr) {
        ASSERT_NE(nullptr, window);
        int64_t period = window->GetVSyncPeriod();
        ASSERT_LE(-1, period);
    }
    sptr<Window> window_ = new Window();
    ASSERT_NE(nullptr, window_);
    int64_t period_ = window_->GetVSyncPeriod();
    ASSERT_LE(-1, period_);
}

/**
 * @tc.name: performBack
 * @tc.desc: window performBack
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, performBack, Function | SmallTest | Level3)
{
    sptr<WindowOption> winOption = nullptr;
    winOption = new (std::nothrow) OHOS::Rosen::WindowOption();
    ASSERT_NE(nullptr, winOption);
    winOption->SetWindowType(OHOS::Rosen::WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);

    sptr<WindowOption> option = new WindowOption;
    sptr<Window> window = Window::Create("performBack", option);
    if (window != nullptr) {
        ASSERT_NE(nullptr, window);
        window->PerformBack()
        ;
    }
    sptr<Window> window_ = new Window();
    ASSERT_NE(nullptr, window_);
    window_->PerformBack();
}

/**
 * @tc.name: SetResizeByDragEnabled
 * @tc.desc: set dragEnabled flag
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetResizeByDragEnabled, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, window->SetResizeByDragEnabled(true));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetRaiseByClickEnabled
 * @tc.desc: set raiseEnabled flag
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetRaiseByClickEnabled, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, window->SetRaiseByClickEnabled(true));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: RaiseAboveTarget
 * @tc.desc: RaiseAboveTarget flag
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, RaiseAboveTarget, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(WmErrorCode::WM_ERROR_DEVICE_NOT_SUPPORT, window->RaiseAboveTarget(2));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: HideNonSystemFloatingWindows
 * @tc.desc: set shouldHide flag
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, HideNonSystemFloatingWindows, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, window->HideNonSystemFloatingWindows(false));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: GetWindowLimits
 * @tc.desc: window GetWindowLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, GetWindowLimits, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    WindowLimits windowLimits;
    auto ret = window->GetWindowLimits(windowLimits);
    ASSERT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetWindowLimits
 * @tc.desc: window SetWindowLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetWindowLimits, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    WindowLimits windowLimits;
    auto ret = window->SetWindowLimits(windowLimits);
    ASSERT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, ret);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: RegisterWindowVisibilityChangeListener
 * @tc.desc: Register window visibility change listener
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, RegisterWindowVisibilityChangeListener, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, window->RegisterWindowVisibilityChangeListener(nullptr));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: UnregisterWindowVisibilityChangeListener
 * @tc.desc: Unregister window visibility change listener
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, UnregisterWindowVisibilityChangeListener, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, window->UnregisterWindowVisibilityChangeListener(nullptr));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: TransferAccessibilityEvent
 * @tc.desc: get
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, TransferAccessibilityEvent, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    Accessibility::AccessibilityEventInfo info;
    int64_t uiExtensionIdLevel = 0;
    ASSERT_EQ(WMError::WM_OK, window->TransferAccessibilityEvent(info, uiExtensionIdLevel));
}

/**
 * @tc.name: SetSingleFrameComposerEnabled
 * @tc.desc: set single frame composer enable flag
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, SetSingleFrameComposerEnabled, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, window->SetSingleFrameComposerEnabled(false));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: FlushFrameRate
 * @tc.desc: FlushFrameRate Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTest, FlushFrameRate, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ASSERT_NE(nullptr, window);
    uint32_t rate = 120;
    window->FlushFrameRate(rate);
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}
}
} // namespace Rosen
} // namespace OHOS