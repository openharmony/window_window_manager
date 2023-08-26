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
    WindowSessionProperty* property = new WindowSessionProperty();
    ASSERT_EQ(property->GetDragEnabled(), false);
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
    WindowSessionProperty* property = new WindowSessionProperty();
    ASSERT_EQ(property->GetRaiseEnabled(), false);
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
    const sptr<WindowSessionProperty> property=new WindowSessionProperty();
    ASSERT_EQ(property->GetDragEnabled(), false);
}

/**
 * @tc.name: SetSessionInfo
 * @tc.desc: SetSessionInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetSessionInfo, Function | SmallTest | Level2)
{
    bool ret = true;
    SessionInfo *property = new SessionInfo;
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.SetSessionInfo(*property);
    ASSERT_EQ(true, ret);
}
/**
 * @tc.name: SetRequestedOrientation
 * @tc.desc: SetRequestedOrientation test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, SetRequestedOrientation, Function | SmallTest | Level2)
{
    Orientation orientation=Orientation();
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.SetRequestedOrientation(orientation);
}

/**
 * @tc.name: SetPrivacyMode
 * @tc.desc: SetPrivacyMode test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetPrivacyMode, Function | SmallTest | Level2)
{
    bool isPrivate = true;
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.SetPrivacyMode(isPrivate);
}

/**
 * @tc.name: SetSystemPrivacyMode
 * @tc.desc: SetSystemPrivacyMode test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetSystemPrivacyMode, Function | SmallTest | Level2)
{
    bool isSystemPrivate = true;
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.SetSystemPrivacyMode(isSystemPrivate);

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
}

/**
 * @tc.name: GetParentId
 * @tc.desc: GetParentId test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, GetParentId, Function | SmallTest | Level2)
{
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.GetParentId();
}

/**
 * @tc.name: SetWindowFlags
 * @tc.desc: SetWindowFlags test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetWindowFlags, Function | SmallTest | Level2)
{
    uint32_t flags=0;
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.SetWindowFlags(flags);
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
    ASSERT_EQ(false,result);
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
    ASSERT_EQ(false,result);
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
    ASSERT_EQ(false,result);
}

/**
 * @tc.name: SetTokenState
 * @tc.desc: SetTokenState test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetTokenState, Function | SmallTest | Level2)
{
    WindowSessionProperty windowSessionProperty;
    bool hasToken = false;
    windowSessionProperty.SetTokenState(hasToken);
}

/**
 * @tc.name: GetTokenState
 * @tc.desc: GetTokenState test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, GetTokenState, Function | SmallTest | Level2)
{
    WindowSessionProperty windowSessionProperty;
    auto result = windowSessionProperty.GetTokenState();
    ASSERT_EQ(true,result);
}

/**
 * @tc.name: GetMaximizeMode
 * @tc.desc: GetMaximizeMode test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, GetMaximizeMode, Function | SmallTest | Level2)
{
    WindowSessionProperty windowSessionProperty;
    MaximizeMode result = windowSessionProperty.GetMaximizeMode();
    ASSERT_NE(MaximizeMode::MODE_RECOVER,result);
}


/**
 * @tc.name: SetMaximizeMode
 * @tc.desc: SetMaximizeMode test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, SetMaximizeMode, Function | SmallTest | Level2)
{
    WindowSessionProperty windowSessionProperty;
    MaximizeMode mode =MaximizeMode::MODE_AVOID_SYSTEM_BAR;
    windowSessionProperty.SetMaximizeMode(mode);
}


/**
 * @tc.name: SetSystemBarProperty
 * @tc.desc: SetSystemBarProperty test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, SetSystemBarProperty, Function | SmallTest | Level2)
{
    SystemBarProperty *property=new SystemBarProperty();
    WindowType windowtype= WindowType();
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.SetSystemBarProperty(windowtype, *property);
} 

/**
 * @tc.name: GetSystemBarProperty
 * @tc.desc: GetSystemBarProperty test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, GetSystemBarProperty, Function | SmallTest | Level2)
{
    WindowSessionProperty windowSessionProperty;
    auto result = windowSessionProperty.GetSystemBarProperty();
    ASSERT_NE(0,result.size());
} 

/**
 * @tc.name: SetSessionGravity
 * @tc.desc: SetSessionGravity test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, SetSessionGravity, Function | SmallTest | Level2)
{
    SessionGravity sessionGravity= SessionGravity();
    uint32_t percent=1234567890;
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.SetSessionGravity(sessionGravity,percent);
}

/**
 * @tc.name: GetSessionGravity
 * @tc.desc: GetSessionGravity test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, GetSessionGravity, Function | SmallTest | Level2)
{
    SessionGravity *sessionGravity=new SessionGravity();
    uint32_t *percent= new uint32_t;
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.GetSessionGravity(*sessionGravity,*percent);
}
/**
 * @tc.name: IsDecorEnable
 * @tc.desc: IsDecorEnable test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, IsDecorEnable, Function | SmallTest | Level2)
{
    WindowSessionProperty windowSessionProperty;
    auto result= windowSessionProperty.IsDecorEnable();
    ASSERT_EQ(true,result);
}

/**
 * @tc.name: SetModeSupportInfo
 * @tc.desc: SetModeSupportInfo test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, SetModeSupportInfo, Function | SmallTest | Level2)
{
    uint32_t modeSupportInfo=1234567890;
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.SetModeSupportInfo(modeSupportInfo);
}

/**
 * @tc.name: GetModeSupportInfo
 * @tc.desc: GetModeSupportInfo test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, GetModeSupportInfo, Function | SmallTest | Level2)
{
    
    WindowSessionProperty windowSessionProperty;
    auto result=windowSessionProperty.GetModeSupportInfo();
    ASSERT_NE(0,result);
}

/**
 * @tc.name: IsFloatingWindowAppType
 * @tc.desc: IsFloatingWindowAppType test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, IsFloatingWindowAppType, Function | SmallTest | Level2)
{
    
    WindowSessionProperty windowSessionProperty;
    auto result=windowSessionProperty.IsFloatingWindowAppType();
    ASSERT_EQ(false,result);
}

/**
 * @tc.name: SetTouchHotAreas
 * @tc.desc: SetTouchHotAreas test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, SetTouchHotAreas, Function | SmallTest | Level2)
{
    std::vector<Rect>* rects =new std::vector<Rect>;
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.SetTouchHotAreas(*rects);
    
}
/**
 * @tc.name: GetTouchHotAreas
 * @tc.desc: GetTouchHotAreas test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, GetTouchHotAreas, Function | SmallTest | Level2)
{
    std::vector<Rect>* rects =new std::vector<Rect>;
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.GetTouchHotAreas(*rects);


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
    windowSessionProperty.UnmarshallingWindowLimits(parcel, property);
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
}

/**
 * @tc.name: Unmarshalling
 * @tc.desc: Unmarshalling test
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionPropertyTest, Unmarshalling, Function | SmallTest | Level2)
{
    WindowSessionProperty* property = new(std::nothrow) WindowSessionProperty();
    Parcel parcel = Parcel();
    WindowSessionProperty windowSessionProperty;
    auto result= windowSessionProperty.Unmarshalling(parcel);
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
    sptr<WindowSessionProperty>* property  = new sptr<WindowSessionProperty>();
    WindowSessionProperty windowSessionProperty;
    windowSessionProperty.CopyFrom(*property);
}

} // namespace
} // namespace Rosen
} // namespace OHOS
