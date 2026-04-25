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
#include "session/host/include/keyboard_session.h"
#include "interfaces/innerkits/wm/wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class ShowKeyboardTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<KeyboardSession> CreateKeyboardSession(const std::string& abilityName, const std::string& bundleName);
};

void ShowKeyboardTest::SetUpTestCase() {}
void ShowKeyboardTest::TearDownTestCase() {}
void ShowKeyboardTest::SetUp() {}
void ShowKeyboardTest::TearDown() {}

sptr<KeyboardSession> ShowKeyboardTest::CreateKeyboardSession(
    const std::string& abilityName, const std::string& bundleName)
{
    SessionInfo info;
    info.abilityName_ = abilityName;
    info.bundleName_ = bundleName;
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, nullptr);
    return keyboardSession;
}

namespace {

HWTEST_F(ShowKeyboardTest, ShowKeyboard01, TestSize.Level1)
{
    auto keyboardSession = CreateKeyboardSession("ShowKeyboard01", "TestBundle01");
    ASSERT_NE(keyboardSession, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    keyboardSession->property_ = property;
    uint32_t callingWindowId = 100;
    KeyboardEffectOption effectOption;
    WMError result = keyboardSession->ShowKeyboard(callingWindowId, effectOption);
    EXPECT_EQ(result, WMError::WM_OK);
}

HWTEST_F(ShowKeyboardTest, ShowKeyboard02, TestSize.Level1)
{
    auto keyboardSession = CreateKeyboardSession("ShowKeyboard02", "TestBundle02");
    ASSERT_NE(keyboardSession, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    keyboardSession->property_ = property;
    uint32_t callingWindowId = INVALID_WINDOW_ID;
    KeyboardEffectOption effectOption;
    WMError result = keyboardSession->ShowKeyboard(callingWindowId, effectOption);
    EXPECT_EQ(result, WMError::WM_OK);
}

HWTEST_F(ShowKeyboardTest, ShowKeyboard03, TestSize.Level1)
{
    auto keyboardSession = CreateKeyboardSession("ShowKeyboard03", "TestBundle03");
    ASSERT_NE(keyboardSession, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    keyboardSession->property_ = property;
    uint32_t callingWindowId = 100;
    KeyboardEffectOption effectOption(
        KeyboardViewMode::DARK_IMMERSIVE_MODE,
        KeyboardFlowLightMode::NONE,
        KeyboardGradientMode::NONE,
        0);
    WMError result = keyboardSession->ShowKeyboard(callingWindowId, effectOption);
    EXPECT_EQ(result, WMError::WM_OK);
}

HWTEST_F(ShowKeyboardTest, ShowKeyboard04, TestSize.Level1)
{
    auto keyboardSession = CreateKeyboardSession("ShowKeyboard04", "TestBundle04");
    ASSERT_NE(keyboardSession, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    keyboardSession->property_ = property;
    uint32_t callingWindowId = 100;
    KeyboardEffectOption effectOption(
        KeyboardViewMode::LIGHT_IMMERSIVE_MODE,
        KeyboardFlowLightMode::NONE,
        KeyboardGradientMode::NONE,
        0);
    WMError result = keyboardSession->ShowKeyboard(callingWindowId, effectOption);
    EXPECT_EQ(result, WMError::WM_OK);
}

HWTEST_F(ShowKeyboardTest, ShowKeyboard05, TestSize.Level1)
{
    auto keyboardSession = CreateKeyboardSession("ShowKeyboard05", "TestBundle05");
    ASSERT_NE(keyboardSession, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    keyboardSession->property_ = property;
    uint32_t callingWindowId = 100;
    KeyboardEffectOption effectOption(
        KeyboardViewMode::DARK_IMMERSIVE_MODE,
        KeyboardFlowLightMode::FLOW_LIGHT_UP,
        KeyboardGradientMode::GRADIENT_UP,
        150);
    WMError result = keyboardSession->ShowKeyboard(callingWindowId, effectOption);
    EXPECT_EQ(result, WMError::WM_OK);
}

HWTEST_F(ShowKeyboardTest, ShowKeyboard06, TestSize.Level1)
{
    auto keyboardSession = CreateKeyboardSession("ShowKeyboard06", "TestBundle06");
    ASSERT_NE(keyboardSession, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    keyboardSession->property_ = property;
    uint32_t callingWindowId = UINT32_MAX;
    KeyboardEffectOption effectOption;
    WMError result = keyboardSession->ShowKeyboard(callingWindowId, effectOption);
    EXPECT_EQ(result, WMError::WM_OK);
}

HWTEST_F(ShowKeyboardTest, ShowKeyboard07, TestSize.Level1)
{
    auto keyboardSession = CreateKeyboardSession("ShowKeyboard07", "TestBundle07");
    ASSERT_NE(keyboardSession, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    keyboardSession->property_ = property;
    KeyboardEffectOption effectOption;
    WMError result1 = keyboardSession->ShowKeyboard(100, effectOption);
    EXPECT_EQ(result1, WMError::WM_OK);
    WMError result2 = keyboardSession->ShowKeyboard(200, effectOption);
    EXPECT_EQ(result2, WMError::WM_OK);
}
} // namespace
} // namespace Rosen
} // namespace OHOS