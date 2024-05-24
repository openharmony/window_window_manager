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

    property->SetRequestedOrientation(Orientation::FOLLOW_DESKTOP);
    Orientation ret6 = property->GetRequestedOrientation();
    ASSERT_EQ(ret6, Orientation::FOLLOW_DESKTOP);
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

/**
 * @tc.name: SetHideNonSystemFloatingWindows
 * @tc.desc: SetHideNonSystemFloatingWindows and GetHideNonSystemFloatingWindows to check the value
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetHideNonSystemFloatingWindows, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    ASSERT_EQ(property->GetHideNonSystemFloatingWindows(), false);
    property->SetHideNonSystemFloatingWindows(true);
    ASSERT_EQ(property->GetHideNonSystemFloatingWindows(), true);
}

/**
 * @tc.name: KeepKeyboardOnFocus
 * @tc.desc: KeepKeyboardOnFocus and GetKeepKeyboardFlag to check the value
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, KeepKeyboardOnFocus, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    ASSERT_EQ(property->GetKeepKeyboardFlag(), false);
    property->KeepKeyboardOnFocus(true);
    ASSERT_EQ(property->GetKeepKeyboardFlag(), true);
}

/**
 * @tc.name: SetTextFieldPositionY
 * @tc.desc: SetTextFieldPositionY and GetTextFieldPositionY to check the value
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetTextFieldPositionY, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    property->SetTextFieldPositionY(5.5);
    ASSERT_EQ(property->GetTextFieldPositionY(), 5.5);
}

/**
 * @tc.name: SetTextFieldHeight
 * @tc.desc: SetTextFieldHeight and GetTextFieldHeight to check the value
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetTextFieldHeight, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    property->SetTextFieldHeight(5.5);
    ASSERT_EQ(property->GetTextFieldHeight(), 5.5);
}

/**
 * @tc.name: SetIsLayoutFullScreen
 * @tc.desc: SetIsLayoutFullScreen and IsLayoutFullScreen to check the value
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetIsLayoutFullScreen, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    ASSERT_EQ(property->IsLayoutFullScreen(), false);
    property->SetIsLayoutFullScreen(true);
    ASSERT_EQ(property->IsLayoutFullScreen(), true);
}

/**
 * @tc.name: Read
 * @tc.desc: Read test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, Read, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    if (property != nullptr) {
        Parcel parcel = Parcel();
        property->Read(parcel, WSPropertyChangeAction::ACTION_UPDATE_MODE);
        ASSERT_EQ(property->GetPersistentId(), INVALID_SESSION_ID);
        delete property;
    }
}

/**
 * @tc.name: Write
 * @tc.desc: Write and Read to check the value
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, Write, Function | SmallTest | Level2)
{
    WindowSessionProperty *oldProperty = new (std::nothrow) WindowSessionProperty();
    WindowSessionProperty *newProperty = new (std::nothrow) WindowSessionProperty();
    if ((oldProperty != nullptr) && (newProperty != nullptr)) {
        int32_t persistentId = 2;
        oldProperty->SetPersistentId(persistentId);
        oldProperty->SetFocusable(true);
        Parcel parcel = Parcel();
        oldProperty->Write(parcel, WSPropertyChangeAction::ACTION_UPDATE_FOCUSABLE);

        newProperty->Read(parcel, WSPropertyChangeAction::ACTION_UPDATE_FOCUSABLE);
        ASSERT_EQ(newProperty->GetPersistentId(), persistentId);
        ASSERT_EQ(newProperty->GetFocusable(), true);
        delete oldProperty;
        delete newProperty;
    }
}

/**
 * @tc.name: GetWindowName
 * @tc.desc: GetWindowName
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, GetWindowName, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    if (property == nullptr) {
        return;
    }
    std::string name = "test";
    property->SetWindowName(name);
    auto result = property->GetWindowName();
    ASSERT_EQ(result, name);
    delete property;
}

/**
 * @tc.name: GetSessionInfo
 * @tc.desc: GetSessionInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, GetSessionInfo, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    if (property == nullptr) {
        return;
    }
    SessionInfo *info = new SessionInfo();
    if (info == nullptr) {
        return;
    }
    property->SetSessionInfo(*info);
    auto result = property->GetSessionInfo();
    ASSERT_EQ(property->GetRaiseEnabled(), true);
    delete property;
}

/**
 * @tc.name: GetWindowRect
 * @tc.desc: GetWindowRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, GetWindowRect, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    if (property == nullptr) {
        return;
    }
    Rect rect = {0, 0, 0, 0};
    property->SetWindowRect(rect);
    auto result = property->GetWindowRect();
    ASSERT_EQ(result, rect);
    delete property;
}

/**
 * @tc.name: GetRequestRect
 * @tc.desc: GetRequestRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, GetRequestRect, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    if (property == nullptr) {
        return;
    }
    Rect requestRect = {0, 0, 0, 0};
    property->SetWindowRect(requestRect);
    auto result = property->GetWindowRect();
    ASSERT_EQ(result, requestRect);
    delete property;
}

/**
 * @tc.name: GetWindowType
 * @tc.desc: GetWindowType
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, GetWindowType, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    if (property == nullptr) {
        return;
    }
    WindowType type = WindowType::APP_WINDOW_BASE;
    property->SetWindowType(type);
    auto result = property->GetWindowType();
    ASSERT_EQ(result, type);
    delete property;
}

/**
 * @tc.name: GetDisplayId
 * @tc.desc: GetDisplayId
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, GetDisplayId, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    if (property == nullptr) {
        return;
    }
    DisplayId displayId = 1;
    property->SetDisplayId(displayId);
    auto result = property->GetDisplayId();
    ASSERT_EQ(result, displayId);
    delete property;
}

/**
 * @tc.name: GetPersistentId
 * @tc.desc: GetPersistentId
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, GetPersistentId, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    if (property == nullptr) {
        return;
    }
    int32_t persistentId = 1;
    property->SetPersistentId(persistentId);
    auto result = property->GetPersistentId();
    ASSERT_EQ(result, persistentId);
    delete property;
}

/**
 * @tc.name: GetParentPersistentId
 * @tc.desc: GetParentPersistentId
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, GetParentPersistentId, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    if (property == nullptr) {
        return;
    }
    int32_t persistentId = 1;
    property->SetParentPersistentId(persistentId);
    auto result = property->GetParentPersistentId();
    ASSERT_EQ(result, persistentId);
    delete property;
}

/**
 * @tc.name: SetTurnScreenOn
 * @tc.desc: SetTurnScreenOn
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetTurnScreenOn, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    if (property == nullptr) {
        return;
    }
    bool turnScreenOn = false;
    property->SetTurnScreenOn(turnScreenOn);
    ASSERT_EQ(property->turnScreenOn_, turnScreenOn);
    delete property;
}

/**
 * @tc.name: SetKeepScreenOn
 * @tc.desc: SetKeepScreenOn
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetKeepScreenOn, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    if (property == nullptr) {
        return;
    }
    bool keepScreenOn = false;
    property->SetKeepScreenOn(keepScreenOn);
    ASSERT_EQ(property->keepScreenOn_, keepScreenOn);
    delete property;
}

/**
 * @tc.name: SetAccessTokenId
 * @tc.desc: SetAccessTokenId
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetAccessTokenId, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    if (property == nullptr) {
        return;
    }
    uint32_t accessTokenId = 1;
    property->SetAccessTokenId(accessTokenId);
    ASSERT_EQ(property->accessTokenId_, accessTokenId);
    delete property;
}

/**
 * @tc.name: GetWindowState
 * @tc.desc: GetWindowState
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, GetWindowState, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    if (property == nullptr) {
        return;
    }
    WindowState state = WindowState::STATE_INITIAL;
    property->SetWindowState(state);
    auto result = property->GetWindowState();
    ASSERT_EQ(result, state);
    delete property;
}

/**
 * @tc.name: SetSystemPrivacyMode02
 * @tc.desc: SetSystemPrivacyMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetSystemPrivacyMode02, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    if (property == nullptr) {
        return;
    }
    bool isSystemPrivate = false;
    property->SetSystemPrivacyMode(isSystemPrivate);
    ASSERT_EQ(property->isSystemPrivacyMode_, isSystemPrivate);
    delete property;
}

/**
 * @tc.name: SetTokenState02
 * @tc.desc: SetTokenState
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetTokenState02, Function | SmallTest | Level2)
{
    WindowSessionProperty *property = new (std::nothrow) WindowSessionProperty();
    if (property == nullptr) {
        return;
    }
    bool hasToken = false;
    property->SetTokenState(hasToken);
    ASSERT_EQ(property->tokenState_, hasToken);
    delete property;
}

/**
 * @tc.name: MarshallingTouchHotAreas
 * @tc.desc: MarshallingTouchHotAreas test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, MarshallingTouchHotAreas, Function | SmallTest | Level2)
{
    Parcel parcel = Parcel();
    WindowSessionProperty *property = new WindowSessionProperty();
    if (property == nullptr) {
        return;
    }
    for (int i = 0; i < 13; i++) {
        struct Rect rect[i];
        property->touchHotAreas_.push_back(rect[i]);
    }
    bool result = property->MarshallingTouchHotAreas(parcel);
    ASSERT_EQ(result, false);
    delete property;
}

/**
 * @tc.name: UnmarshallingPiPTemplateInfo02
 * @tc.desc: UnmarshallingPiPTemplateInfo test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, UnmarshallingPiPTemplateInfo02, Function | SmallTest | Level2)
{
    Parcel parcel = Parcel();
    WindowSessionProperty *property = new WindowSessionProperty();
    if (property == nullptr) {
        return;
    }
    property->type_ = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.UnmarshallingPiPTemplateInfo(parcel, property);
    ASSERT_EQ(property->GetTokenState(), false);
    delete property;
}

/**
 * @tc.name: MarshallingPiPTemplateInfo
 * @tc.desc: MarshallingPiPTemplateInfo test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, MarshallingPiPTemplateInfo, Function | SmallTest | Level2)
{
    Parcel parcel = Parcel();
    WindowSessionProperty *property = new WindowSessionProperty();
    if (property == nullptr) {
        return;
    }
    property->type_ = WindowType::WINDOW_TYPE_PIP;
    for (int i = 0; i < 10; i++) {
        property->pipTemplateInfo_.controlGroup.push_back(i);
    }
    bool result = property->MarshallingPiPTemplateInfo(parcel);
    ASSERT_EQ(result, false);
    delete property;
}
} // namespace
} // namespace Rosen
} // namespace OHOS
