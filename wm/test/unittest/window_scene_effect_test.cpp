/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "mock_session.h"
#include "window_scene_session_impl.h"
#include "mock_uicontent.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class WindowSceneEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WindowSceneEffectTest::SetUpTestCase()
{
}

void WindowSceneEffectTest::TearDownTestCase()
{
}

void WindowSceneEffectTest::SetUp()
{
}

void WindowSceneEffectTest::TearDown()
{
}

class WindowEffectTestUtils {
public:
    static sptr<WindowSceneSessionImpl> CreateTestWindow(const std::string& name)
    {
        sptr<WindowOption> option = new (std::nothrow) WindowOption();
        option->SetWindowName(name);
        option->SetDisplayId(0);
        sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
        window->property_->SetPersistentId(1);
        SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
        sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
        window->hostSession_ = session;
        return window;
    }
};

namespace {
using Utils = WindowEffectTestUtils;

/**
 * @tc.name: WindowEffect01
 * @tc.desc: Set window corner radius
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneEffectTest, WindowEffect01, Function | MediumTest | Level3)
{
    const sptr<WindowSceneSessionImpl>& window = Utils::CreateTestWindow("CornerRadius");
    ASSERT_NE(nullptr, window);

    ASSERT_EQ(WMError::WM_OK, window->SetCornerRadius(0.0));
    ASSERT_EQ(WMError::WM_OK, window->SetCornerRadius(16.0));
    ASSERT_EQ(WMError::WM_OK, window->SetCornerRadius(1000.0));
    ASSERT_EQ(WMError::WM_OK, window->SetCornerRadius(-1.0));

    ASSERT_EQ(WMError::WM_OK, window->Destroy(true));
}

/**
 * @tc.name: WindowEffect02
 * @tc.desc: Set window shadow radius
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneEffectTest, WindowEffect02, Function | MediumTest | Level3)
{
    const sptr<WindowSceneSessionImpl>& window = Utils::CreateTestWindow("shadowRadius");
    ASSERT_NE(nullptr, window);

    ASSERT_EQ(WMError::WM_OK, window->SetShadowRadius(0.0));
    ASSERT_EQ(WMError::WM_OK, window->SetShadowRadius(16.0));
    ASSERT_EQ(WMError::WM_OK, window->SetShadowRadius(1000.0));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowRadius(-1.0));

    ASSERT_EQ(WMError::WM_OK, window->Destroy(true));
}

/**
 * @tc.name: WindowEffect03
 * @tc.desc: Set window shadow color
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneEffectTest, WindowEffect03, Function | MediumTest | Level3)
{
    const sptr<WindowSceneSessionImpl>& window = Utils::CreateTestWindow("WindowEffect03");
    ASSERT_NE(nullptr, window);

    ASSERT_EQ(WMError::WM_OK, window->SetShadowColor("#FF22EE44"));
    ASSERT_EQ(WMError::WM_OK, window->SetShadowColor("#22EE44"));
    ASSERT_EQ(WMError::WM_OK, window->SetShadowColor("#ff22ee44"));

    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowColor("ff22ee44"));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowColor("22ee44"));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowColor("#ppEE44"));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowColor("#eepp44"));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowColor("#ffeePP44"));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowColor("#ff22ee4422"));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowColor("#ff"));

    ASSERT_EQ(WMError::WM_OK, window->Destroy(true));
}

/**
 * @tc.name: WindowEffect04
 * @tc.desc: Set window shadow offset
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneEffectTest, WindowEffect04, Function | MediumTest | Level3)
{
    const sptr<WindowSceneSessionImpl>& window = Utils::CreateTestWindow("WindowEffect04");
    ASSERT_NE(nullptr, window);

    ASSERT_EQ(WMError::WM_OK, window->SetShadowOffsetX(0.0));
    ASSERT_EQ(WMError::WM_OK, window->SetShadowOffsetX(16.0));
    ASSERT_EQ(WMError::WM_OK, window->SetShadowOffsetX(1000.0));
    ASSERT_EQ(WMError::WM_OK, window->SetShadowOffsetX(-1.0));

    ASSERT_EQ(WMError::WM_OK, window->SetShadowOffsetY(0.0));
    ASSERT_EQ(WMError::WM_OK, window->SetShadowOffsetY(16.0));
    ASSERT_EQ(WMError::WM_OK, window->SetShadowOffsetY(1000.0));
    ASSERT_EQ(WMError::WM_OK, window->SetShadowOffsetY(-1.0));

    ASSERT_EQ(WMError::WM_OK, window->Destroy(true));
}

/**
 * @tc.name: WindowEffect05
 * @tc.desc: Set window blur radius
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneEffectTest, WindowEffect05, Function | MediumTest | Level3)
{
    const sptr<WindowSceneSessionImpl>& window = Utils::CreateTestWindow("WindowEffect05");
    ASSERT_NE(nullptr, window);

    ASSERT_EQ(WMError::WM_OK, window->SetBlur(0.0));
    ASSERT_EQ(WMError::WM_OK, window->SetBlur(16.0));
    ASSERT_EQ(WMError::WM_OK, window->SetBlur(1000.0));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetBlur(-1.0));

    ASSERT_EQ(WMError::WM_OK, window->Destroy(true));
}

/**
 * @tc.name: WindowEffect06
 * @tc.desc: Set window backdrop blur radius
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneEffectTest, WindowEffect06, Function | MediumTest | Level3)
{
    const sptr<WindowSceneSessionImpl>& window = Utils::CreateTestWindow("WindowEffect06");
    ASSERT_NE(nullptr, window);

    ASSERT_EQ(WMError::WM_OK, window->SetBackdropBlur(0.0));
    ASSERT_EQ(WMError::WM_OK, window->SetBackdropBlur(16.0));
    ASSERT_EQ(WMError::WM_OK, window->SetBackdropBlur(1000.0));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetBackdropBlur(-1.0));

    ASSERT_EQ(WMError::WM_OK, window->Destroy(true));
}

/**
 * @tc.name: WindowEffect07
 * @tc.desc: Set window backdrop blur style
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneEffectTest, WindowEffect07, Function | MediumTest | Level3)
{
    const sptr<WindowSceneSessionImpl>& window = Utils::CreateTestWindow("WindowEffect07");
    ASSERT_NE(nullptr, window);

    ASSERT_EQ(WMError::WM_OK, window->SetBackdropBlurStyle(WindowBlurStyle::WINDOW_BLUR_OFF));
    ASSERT_EQ(WMError::WM_OK, window->SetBackdropBlurStyle(WindowBlurStyle::WINDOW_BLUR_THIN));
    ASSERT_EQ(WMError::WM_OK, window->SetBackdropBlurStyle(WindowBlurStyle::WINDOW_BLUR_REGULAR));
    ASSERT_EQ(WMError::WM_OK, window->SetBackdropBlurStyle(WindowBlurStyle::WINDOW_BLUR_THICK));

    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetBackdropBlurStyle(static_cast<WindowBlurStyle>(-1)));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetBackdropBlurStyle(static_cast<WindowBlurStyle>(5)));

    ASSERT_EQ(WMError::WM_OK, window->Destroy(true));
}

}
} // namespace Rosen
} // namespace OHOS
