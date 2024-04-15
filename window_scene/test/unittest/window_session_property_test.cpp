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
#include "window_session_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class WindowSessionPropertyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void WindowSessionPropertyTest::SetUpTestCase()
{
}

void WindowSessionPropertyTest::TearDownTestCase()
{
}

namespace {
/**
 * @tc.name: SetDragEnabled001
 * @tc.desc: SetDragEnabled and GetDragEnabled to check the value
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetDragEnabled001, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new WindowSessionProperty();
    ASSERT_EQ(property->GetDragEnabled(), true);
    property->SetDragEnabled(false);
    ASSERT_EQ(property->GetDragEnabled(), false);
}

/**
 * @tc.name: SetRaiseEnabled001
 * @tc.desc: SetRaiseEnabled and GetRaiseEnabled to check the value
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetRaiseEnabled001, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new WindowSessionProperty();
    ASSERT_EQ(property->GetRaiseEnabled(), true);
    property->SetRaiseEnabled(false);
    ASSERT_EQ(property->GetRaiseEnabled(), false);
}

/**
 * @tc.name: WindowSessionProperty
 * @tc.desc: WindowSessionProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, WindowSessionProperty, Function | SmallTest | Level2)
{
    const sptr<WindowSessionProperty> property = new WindowSessionProperty();
    ASSERT_EQ(property->GetDragEnabled(), true);
}

/**
 * @tc.name: SetSessionInfo
 * @tc.desc: SetSessionInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetSessionInfo, Function | SmallTest | Level2)
{
    SessionInfo *info = new SessionInfo();
    WindowSessionProperty *property = new WindowSessionProperty();
    property->SetSessionInfo(*info);
    property->SetRaiseEnabled(true);
    ASSERT_EQ(property->GetRaiseEnabled(), true);
}
/**
 * @tc.name: SetRequestedOrientation
 * @tc.desc: SetRequestedOrientation test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, SetRequestedOrientation, Function | SmallTest | Level2)
{
    Orientation orientation = Orientation::REVERSE_HORIZONTAL;
    WindowSessionProperty *property = new WindowSessionProperty();
    property->SetRequestedOrientation(orientation);
    Orientation ret = property->GetRequestedOrientation();
    ASSERT_EQ(ret, orientation);

    property->SetRequestedOrientation(Orientation::AUTO_ROTATION_UNSPECIFIED);
    Orientation ret1 = property->GetRequestedOrientation();
    ASSERT_EQ(ret1, Orientation::AUTO_ROTATION_UNSPECIFIED);

    property->SetRequestedOrientation(Orientation::USER_ROTATION_PORTRAIT);
    Orientation ret2 = property->GetRequestedOrientation();
    ASSERT_EQ(ret2, Orientation::USER_ROTATION_PORTRAIT);

    property->SetRequestedOrientation(Orientation::USER_ROTATION_LANDSCAPE);
    Orientation ret3 = property->GetRequestedOrientation();
    ASSERT_EQ(ret3, Orientation::USER_ROTATION_LANDSCAPE);

    property->SetRequestedOrientation(Orientation::USER_ROTATION_PORTRAIT_INVERTED);
    Orientation ret4 = property->GetRequestedOrientation();
    ASSERT_EQ(ret4, Orientation::USER_ROTATION_PORTRAIT_INVERTED);

    property->SetRequestedOrientation(Orientation::USER_ROTATION_LANDSCAPE_INVERTED);
    Orientation ret5 = property->GetRequestedOrientation();
    ASSERT_EQ(ret5, Orientation::USER_ROTATION_LANDSCAPE_INVERTED);
}

/**
 * @tc.name: SetPrivacyMode
 * @tc.desc: SetPrivacyMode test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetPrivacyMode, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new WindowSessionProperty();
    ASSERT_EQ(property->GetPrivacyMode(), false);
    property->SetPrivacyMode(false);
    ASSERT_EQ(property->GetPrivacyMode(), false);
}

/**
 * @tc.name: SetSystemPrivacyMode
 * @tc.desc: SetSystemPrivacyMode test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetSystemPrivacyMode, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new WindowSessionProperty();
    ASSERT_EQ(property->GetSystemPrivacyMode(), false);
}

/**
 * @tc.name: SetBrightness
 * @tc.desc: SetBrightness test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetBrightness, Function | SmallTest | Level2)
{
    float brightness = 0.02;
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.SetBrightness(brightness);
    WindowSessionProperty *property = new WindowSessionProperty();
    ASSERT_NE(property->GetBrightness(), 0);
}

/**
 * @tc.name: SetTopmost
 * @tc.desc: SetTopmost test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetTopmost, Function | SmallTest | Level2)
{
    bool topmost = true;
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.SetTopmost(topmost);
    ASSERT_TRUE(windowSessionProperty.IsTopmost());
}

/**
 * @tc.name: GetParentId
 * @tc.desc: GetParentId test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, GetParentId, Function | SmallTest | Level2)
{
    WindowSessionProperty windowSessionProperty;
    int32_t result = windowSessionProperty.GetParentId();
    ASSERT_EQ(0, result);
}

/**
 * @tc.name: SetWindowFlags
 * @tc.desc: SetWindowFlags test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetWindowFlags, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new WindowSessionProperty();
    ASSERT_EQ(property->GetWindowFlags(), 0);
}

/**
 * @tc.name: SetAndGetPipTemplateInfo
 * @tc.desc: SetAndGetPipTemplateInfo test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetAndGetPipTemplateInfo, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new WindowSessionProperty();
    PiPTemplateInfo pipTemplateInfo;
    pipTemplateInfo.pipTemplateType = static_cast<uint32_t>(PiPTemplateType::VIDEO_CALL);
    property->SetPiPTemplateInfo(pipTemplateInfo);
    ASSERT_EQ(property->GetPiPTemplateInfo().pipTemplateType,
        static_cast<uint32_t>(PiPTemplateType::VIDEO_CALL));
}

/**
 * @tc.name: AddWindowFlag
 * @tc.desc: AddWindowFlag test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, AddWindowFlag, Function | SmallTest | Level2)
{
    WindowFlag flags=WindowFlag();
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.AddWindowFlag(flags);
    WindowSessionProperty *property = new WindowSessionProperty();
    ASSERT_EQ(property->GetWindowFlags(), false);
}


/**
 * @tc.name: IsTurnScreenOn
 * @tc.desc: IsTurnScreenOn test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, IsTurnScreenOn, Function | SmallTest | Level2)
{
    WindowSessionProperty windowSessionProperty;
    bool result = windowSessionProperty.IsTurnScreenOn();
    ASSERT_EQ(false, result);
}

/**
 * @tc.name: IsKeepScreenOn
 * @tc.desc: IsKeepScreenOn test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, IsKeepScreenOn, Function | SmallTest | Level2)
{
    WindowSessionProperty windowSessionProperty;
    bool result = windowSessionProperty.IsKeepScreenOn();
    ASSERT_EQ(false, result);
}

/**
 * @tc.name: GetAccessTokenId
 * @tc.desc: GetAccessTokenId test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, GetAccessTokenId, Function | SmallTest | Level2)
{
    WindowSessionProperty windowSessionProperty;
    auto result = windowSessionProperty.GetAccessTokenId();
    ASSERT_EQ(false, result);
}

/**
 * @tc.name: SetTokenState
 * @tc.desc: SetTokenState test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetTokenState, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new WindowSessionProperty();
    ASSERT_EQ(property->GetTokenState(), false);
}

/**
 * @tc.name: SetMaximizeMode
 * @tc.desc: SetMaximizeMode test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, SetMaximizeMode, Function | SmallTest | Level2)
{
    WindowSessionProperty windowSessionProperty;
    MaximizeMode mode = MaximizeMode::MODE_RECOVER;
    windowSessionProperty.SetMaximizeMode(mode);
    WindowSessionProperty *property = new WindowSessionProperty();
    ASSERT_EQ(property->GetMaximizeMode(), mode);
}

/**
 * @tc.name: SetSystemBarProperty
 * @tc.desc: SetSystemBarProperty test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, SetSystemBarProperty, Function | SmallTest | Level2)
{
    SystemBarProperty *systemBarProperty = new SystemBarProperty();
    WindowType windowtype = WindowType::APP_WINDOW_BASE;
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.SetSystemBarProperty(windowtype, *systemBarProperty);
    WindowSessionProperty *property = new WindowSessionProperty();
    ASSERT_EQ(property->GetTokenState(), false);
}

/**
 * @tc.name: SetKeyboardSessionGravity
 * @tc.desc: SetKeyboardSessionGravity test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, SetKeyboardSessionGravity, Function | SmallTest | Level2)
{
    SessionGravity sessionGravity = SessionGravity::SESSION_GRAVITY_FLOAT;
    uint32_t percent = 1234567890;
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.SetKeyboardSessionGravity(sessionGravity, percent);
    WindowSessionProperty *property = new WindowSessionProperty();
    ASSERT_EQ(property->GetTokenState(), false);
}


/**
 * @tc.name: IsDecorEnable
 * @tc.desc: IsDecorEnable test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, IsDecorEnable, Function | SmallTest | Level2)
{
    WindowSessionProperty windowSessionProperty;
    auto result = windowSessionProperty.IsDecorEnable();
    ASSERT_EQ(false, result);
}

/**
 * @tc.name: SetModeSupportInfo
 * @tc.desc: SetModeSupportInfo test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, SetModeSupportInfo, Function | SmallTest | Level2)
{
    uint32_t modeSupportInfo = 1234567890;
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.SetModeSupportInfo(modeSupportInfo);
    WindowSessionProperty *property = new WindowSessionProperty();
    ASSERT_NE(property->GetModeSupportInfo(), 0);
}
/**
 * @tc.name: IsFloatingWindowAppType
 * @tc.desc: IsFloatingWindowAppType test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, IsFloatingWindowAppType, Function | SmallTest | Level2)
{
    WindowSessionProperty windowSessionProperty;
    auto result = windowSessionProperty.IsFloatingWindowAppType();
    ASSERT_EQ(false, result);
}

/**
 * @tc.name: SetTouchHotAreas
 * @tc.desc: SetTouchHotAreas test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, SetTouchHotAreas, Function | SmallTest | Level2)
{
    std::vector<Rect> *rects = new std::vector<Rect>;
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.SetTouchHotAreas(*rects);
    WindowSessionProperty *property = new WindowSessionProperty();
    ASSERT_EQ(property->GetTokenState(), false);
}

/**
 * @tc.name: UnmarshallingWindowLimits
 * @tc.desc: UnmarshallingWindowLimits test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, UnmarshallingWindowLimits, Function | SmallTest | Level2)
{
    Parcel parcel = Parcel();
    WindowSessionProperty *property = new WindowSessionProperty();
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.UnmarshallingWindowLimits(parcel, property);
    ASSERT_EQ(property->GetTokenState(), false);
}

/**
 * @tc.name: UnMarshallingSystemBarMap
 * @tc.desc: UnMarshallingSystemBarMap test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, UnMarshallingSystemBarMap, Function | SmallTest | Level2)
{
    Parcel parcel = Parcel();
    WindowSessionProperty *property = new WindowSessionProperty();
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.UnMarshallingSystemBarMap(parcel, property);
    ASSERT_EQ(property->GetTokenState(), false);
}

/**
 * @tc.name: UnmarshallingTouchHotAreas
 * @tc.desc: UnmarshallingTouchHotAreas test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, UnmarshallingTouchHotAreas, Function | SmallTest | Level2)
{
    Parcel parcel = Parcel();
    WindowSessionProperty *property = new WindowSessionProperty();
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.UnmarshallingTouchHotAreas(parcel, property);
    ASSERT_EQ(property->GetTokenState(), false);
}

/**
 * @tc.name: UnmarshallingPiPTemplateInfo
 * @tc.desc: UnmarshallingPiPTemplateInfo test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, UnmarshallingPiPTemplateInfo, Function | SmallTest | Level2)
{
    Parcel parcel = Parcel();
    WindowSessionProperty *property = new WindowSessionProperty();
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.UnmarshallingPiPTemplateInfo(parcel, property);
    ASSERT_EQ(property->GetTokenState(), false);
}

/**
 * @tc.name: Unmarshalling
 * @tc.desc: Unmarshalling test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, Unmarshalling, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    Parcel parcel = Parcel();
    WindowSessionProperty windowSessionProperty;
    auto result = windowSessionProperty.Unmarshalling(parcel);
    ASSERT_NE(nullptr, property);
    ASSERT_NE(nullptr, result);
}

/**
 * @tc.name: CopyFrom
 * @tc.desc: CopyFrom test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, CopyFrom, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.CopyFrom(property);
    WindowSessionProperty *wproperty = new WindowSessionProperty();
    ASSERT_EQ(wproperty->GetTokenState(), false);
}

/**
 * @tc.name: SetFocusable
 * @tc.desc: SetFocusable and GetFocusable to check the value
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetFocusable, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    ASSERT_EQ(property->GetFocusable(), true);
    property->SetFocusable(false);
    ASSERT_EQ(property->GetFocusable(), false);
}

/**
 * @tc.name: SetTouchable
 * @tc.desc: SetTouchable and GetTouchable to check the value
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetTouchable, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    ASSERT_EQ(property->GetTouchable(), true);
    property->SetTouchable(false);
    ASSERT_EQ(property->GetTouchable(), false);
}

/**
 * @tc.name: SetForceHide
 * @tc.desc: SetForceHide and GetForceHide to check the value
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetForceHide, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    ASSERT_EQ(property->GetForceHide(), false);
    property->SetForceHide(true);
    ASSERT_EQ(property->GetForceHide(), true);
}

/**
 * @tc.name: SetSystemCalling
 * @tc.desc: SetSystemCalling and GetSystemCalling to check the value
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetSystemCalling, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    ASSERT_EQ(property->GetSystemCalling(), false);
    property->SetSystemCalling(true);
    ASSERT_EQ(property->GetSystemCalling(), true);
}

/**
 * @tc.name: SetIsNeedUpdateWindowMode
 * @tc.desc: SetIsNeedUpdateWindowMode and GetIsNeedUpdateWindowMode to check the value
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetIsNeedUpdateWindowMode, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    ASSERT_EQ(property->GetIsNeedUpdateWindowMode(), false);
    property->SetIsNeedUpdateWindowMode(true);
    ASSERT_EQ(property->GetIsNeedUpdateWindowMode(), true);
}

/**
 * @tc.name: SetIsShaped
 * @tc.desc: SetIsShaped and GetIsShaped to check the value
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetIsShaped, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    ASSERT_EQ(property->GetIsShaped(), false);
    property->SetIsShaped(true);
    ASSERT_EQ(property->GetIsShaped(), true);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
