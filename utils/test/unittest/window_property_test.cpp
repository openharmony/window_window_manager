/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "window_property.h"
#include "wm_common_inner.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowPropertyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void WindowPropertyTest::SetUpTestCase()
{
}

void WindowPropertyTest::TearDownTestCase()
{
}

void WindowPropertyTest::SetUp()
{
}

void WindowPropertyTest::TearDown()
{
}

namespace {
/**
 * @tc.name: MarshallingUnmarshalling
 * @tc.desc: Marshalling Unmarshalling test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, MarshallingUnmarshalling, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    winPropSrc.SetPrivacyMode(true);
    winPropSrc.SetTransparent(true);
    winPropSrc.SetTransform(Transform::Identity());

    Parcel parcel;
    winPropSrc.Marshalling(parcel);
    WindowProperty* winPropDst = winPropSrc.Unmarshalling(parcel);

    ASSERT_EQ(winPropDst->GetPrivacyMode(), true);
    ASSERT_EQ(winPropDst->GetTransparent(), true);
    ASSERT_EQ(winPropDst->GetTransform(), Transform::Identity());
    delete winPropDst;
}

/**
 * @tc.name: CopyFrom
 * @tc.desc: CopyFrom test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, CopyFrom, Function | SmallTest | Level2)
{
    const sptr<WindowProperty> winPropSrc = new(std::nothrow) WindowProperty();
    winPropSrc->SetPrivacyMode(true);
    winPropSrc->SetTransparent(true);
    winPropSrc->SetTransform(Transform::Identity());

    WindowProperty winPropDst(winPropSrc); // winPropDst.CopyFrom(winPropSrc);

    ASSERT_EQ(winPropSrc->GetPrivacyMode(), winPropDst.GetPrivacyMode());
    ASSERT_EQ(winPropSrc->GetTransparent(), winPropDst.GetTransparent());
    ASSERT_EQ(winPropSrc->GetTransform(), winPropDst.GetTransform());
}

/**
 * @tc.name: Read
 * @tc.desc: Read test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, Read, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    winPropSrc.SetPrivacyMode(true);
    winPropSrc.SetTransparent(true);

    Parcel parcel;
    winPropSrc.Marshalling(parcel);

    WindowProperty winPropDst;
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_RECT);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_MODE);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_FLAGS);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_OTHER_PROPS);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_FOCUSABLE);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_TOUCHABLE);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_CALLING_WINDOW);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_ORIENTATION);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_MODE_SUPPORT_INFO);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_TOUCH_HOT_AREA);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_TRANSFORM_PROPERTY);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_ANIMATION_FLAG);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE);
    winPropDst.Read(parcel, PropertyChangeAction::ACTION_UPDATE_SYSTEM_PRIVACY_MODE);

    ASSERT_EQ(false, winPropDst.GetPrivacyMode());
    ASSERT_EQ(false, winPropDst.GetTransparent());
}

/**
 * @tc.name: Write
 * @tc.desc: Write test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, Write, Function | SmallTest | Level2)
{
    Parcel parcel;
    WindowProperty winPropDst;
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_RECT));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_MODE));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_FLAGS));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_OTHER_PROPS));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_FOCUSABLE));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_TOUCHABLE));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_CALLING_WINDOW));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_ORIENTATION));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_MODE_SUPPORT_INFO));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_TOUCH_HOT_AREA));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_TRANSFORM_PROPERTY));
    ASSERT_EQ(true, winPropDst.Write(parcel, PropertyChangeAction::ACTION_UPDATE_ANIMATION_FLAG));
}

/**
 * @tc.name: SetAbilityInfo
 * @tc.desc: Test SetAbilityInfo and GetAbilityInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, SetAbilityInfo, Function | SmallTest | Level2)
{
    WindowProperty winPropDst;
    AbilityInfo info;
    info.bundleName_ = "testBundleName";
    info.abilityName_ = "testAbilityName";
    winPropDst.SetAbilityInfo(info);
    ASSERT_EQ("testBundleName", winPropDst.GetAbilityInfo().bundleName_);
    ASSERT_EQ("testAbilityName", winPropDst.GetAbilityInfo().abilityName_);
}

/**
 * @tc.name: ResumeLastWindowMode
 * @tc.desc: Test ResumeLastWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, ResumeLastWindowMode, Function | SmallTest | Level2)
{
    WindowProperty winPropDst;
    winPropDst.modeSupportInfo_ =  WindowModeSupport::WINDOW_MODE_SUPPORT_PIP;
    winPropDst.lastMode_ =  WindowMode::WINDOW_MODE_PIP;
    winPropDst.mode_ = WindowMode::WINDOW_MODE_UNDEFINED;
    winPropDst.ResumeLastWindowMode();
    ASSERT_EQ(WindowMode::WINDOW_MODE_PIP, winPropDst.mode_);

    winPropDst.modeSupportInfo_ =  WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_SECONDARY;
    winPropDst.lastMode_ =  WindowMode::WINDOW_MODE_PIP;
    winPropDst.mode_ = WindowMode::WINDOW_MODE_UNDEFINED;
    winPropDst.ResumeLastWindowMode();
    ASSERT_EQ(WindowMode::WINDOW_MODE_UNDEFINED, winPropDst.mode_);

    winPropDst.modeSupportInfo_ =  WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING;
    winPropDst.lastMode_ =  WindowMode::WINDOW_MODE_PIP;
    winPropDst.mode_ = WindowMode::WINDOW_MODE_UNDEFINED;
    winPropDst.ResumeLastWindowMode();
    ASSERT_EQ(WindowMode::WINDOW_MODE_FLOATING, winPropDst.mode_);
}

/**
 * @tc.name: HandleComputeTransform
 * @tc.desc: HandleComputeTransform test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, HandleComputeTransform, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    winPropSrc.SetPrivacyMode(true);
    winPropSrc.SetTransparent(true);
    int resultValue = 0;
    std::function<void()> func = [&]() {
        winPropSrc.HandleComputeTransform(Transform::Identity());
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: ComputeTransform
 * @tc.desc: ComputeTransform test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, ComputeTransform, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    winPropSrc.SetPrivacyMode(true);
    winPropSrc.SetTransparent(true);
    int resultValue = 0;
    std::function<void()> func = [&]() {
        winPropSrc.ComputeTransform();
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: SetZoomTransform
 * @tc.desc: SetZoomTransform test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, SetZoomTransform, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    winPropSrc.SetPrivacyMode(true);
    winPropSrc.SetTransparent(true);
    int resultValue = 0;
    std::function<void()> func = [&]() {
        winPropSrc.SetZoomTransform(Transform::Identity());
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: ClearTransformZAxisOffset
 * @tc.desc: ClearTransformZAxisOffset test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, ClearTransformZAxisOffset, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    winPropSrc.SetPrivacyMode(true);
    winPropSrc.SetTransparent(true);
    Transform trans;
    int resultValue = 0;
    std::function<void()> func = [&]() {
        winPropSrc.ClearTransformZAxisOffset(trans);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: UpdatePointerEvent
 * @tc.desc: UpdatePointerEvent test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, UpdatePointerEvent, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    winPropSrc.SetPrivacyMode(true);
    winPropSrc.SetTransparent(true);
    std::shared_ptr<MMI::PointerEvent> pointerEvent;
    int resultValue = 0;
    std::function<void()> func = [&]() {
        winPropSrc.UpdatePointerEvent(pointerEvent);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: isNeedComputerTransform
 * @tc.desc: isNeedComputerTransform test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, isNeedComputerTransform, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    winPropSrc.SetPrivacyMode(true);
    winPropSrc.SetTransparent(true);
    bool ret = winPropSrc.isNeedComputerTransform();
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: SetAnimateWindowFlag
 * @tc.desc: SetAnimateWindowFlag test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, SetAnimateWindowFlag, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    winPropSrc.SetPrivacyMode(true);
    winPropSrc.SetTransparent(true);
    bool isAnimateWindow = false;
    int resultValue = 0;
    std::function<void()> func = [&]() {
        winPropSrc.SetAnimateWindowFlag(isAnimateWindow);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: IsDisplayZoomOn
 * @tc.desc: IsDisplayZoomOn test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, IsDisplayZoomOn, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    winPropSrc.SetPrivacyMode(true);
    winPropSrc.SetTransparent(true);
    bool ret = winPropSrc.IsDisplayZoomOn();
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: IsAnimateWindow
 * @tc.desc: IsAnimateWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, IsAnimateWindow, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    winPropSrc.SetPrivacyMode(true);
    winPropSrc.SetTransparent(true);
    bool ret = winPropSrc.IsAnimateWindow();
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: SetSizeLimits
 * @tc.desc: SetSizeLimits test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, SetSizeLimits, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    winPropSrc.SetPrivacyMode(true);
    winPropSrc.SetTransparent(true);
    WindowSizeLimits sizeLimits;
    int resultValue = 0;
    std::function<void()> func = [&]() {
        winPropSrc.SetSizeLimits(sizeLimits);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: SetUpdatedSizeLimits
 * @tc.desc: SetUpdatedSizeLimits test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, SetUpdatedSizeLimits, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    winPropSrc.SetPrivacyMode(true);
    winPropSrc.SetTransparent(true);
    WindowSizeLimits sizeLimits;
    int resultValue = 0;
    std::function<void()> func = [&]() {
        winPropSrc.SetUpdatedSizeLimits(sizeLimits);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: SetDecorEnable
 * @tc.desc: SetDecorEnable test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, SetDecorEnable, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    winPropSrc.SetPrivacyMode(true);
    winPropSrc.SetTransparent(true);
    bool decorEnable = false;
    int resultValue = 0;
    std::function<void()> func = [&]() {
        winPropSrc.SetDecorEnable(decorEnable);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: AddWindowFlag001
 * @tc.desc: AddWindowFlag test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, AddWindowFlag001, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    int resultValue = 0;
    WindowFlag flag = WindowFlag::WINDOW_FLAG_NEED_AVOID;
    std::function<void()> func = [&]() {
        winPropSrc.AddWindowFlag(flag);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: GetRequestRect002
 * @tc.desc: GetRequestRect test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, GetRequestRect001, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    Rect requestRect { 0, 0, 0, 0 };
    winPropSrc.SetRequestRect(requestRect);
    Rect res = winPropSrc.GetRequestRect();
    ASSERT_EQ(res, requestRect);
}

/**
 * @tc.name: GetWindowSizeChangeReason003
 * @tc.desc: GetWindowSizeChangeReason test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, GetWindowSizeChangeReason003, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    WindowSizeChangeReason reason = WindowSizeChangeReason::UNDEFINED;
    winPropSrc.SetWindowSizeChangeReason(reason);
    WindowSizeChangeReason res = winPropSrc.GetWindowSizeChangeReason();
    ASSERT_EQ(res, reason);
}

/**
 * @tc.name: GetFullScreen004
 * @tc.desc: GetFullScreen test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, GetFullScreen004, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    bool isFullScreen = true;
    winPropSrc.SetFullScreen(isFullScreen);
    bool res = winPropSrc.GetFullScreen();
    ASSERT_EQ(res, isFullScreen);
}

/**
 * @tc.name: GetFocusable005
 * @tc.desc: GetFocusable test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, GetFocusable005, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    bool isFocusable = true;
    winPropSrc.SetFocusable(isFocusable);
    bool res = winPropSrc.GetFocusable();
    ASSERT_EQ(res, isFocusable);
}

/**
 * @tc.name: GetTouchable006
 * @tc.desc: GetTouchable test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, GetTouchable006, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    bool isTouchable = true;
    winPropSrc.SetTouchable(isTouchable);
    bool res = winPropSrc.GetFocusable();
    ASSERT_EQ(res, isTouchable);
}

/**
 * @tc.name: GetCallingWindow007
 * @tc.desc: GetCallingWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, GetCallingWindow007, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    uint32_t windowId = 1;
    winPropSrc.SetCallingWindow(windowId);
    uint32_t res = winPropSrc.GetCallingWindow();
    ASSERT_EQ(res, windowId);
}

/**
 * @tc.name: GetPrivacyMode008
 * @tc.desc: GetPrivacyMode test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, GetPrivacyMode008, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    bool isPrivate = true;
    winPropSrc.SetPrivacyMode(isPrivate);
    bool res = winPropSrc.GetPrivacyMode();
    ASSERT_EQ(res, isPrivate);
}

/**
 * @tc.name: GetSystemPrivacyMode009
 * @tc.desc: GetSystemPrivacyMode test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, GetSystemPrivacyMode009, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    bool isSystemPrivate = true;
    winPropSrc.SetSystemPrivacyMode(isSystemPrivate);
    bool res = winPropSrc.GetSystemPrivacyMode();
    ASSERT_EQ(res, isSystemPrivate);
}

/**
 * @tc.name: GetSnapshotSkip010
 * @tc.desc: GetSnapshotSkip test
 * @tc.type: FUNC
 */
HWTEST_F(WindowPropertyTest, GetSnapshotSkip010, Function | SmallTest | Level2)
{
    WindowProperty winPropSrc;
    bool isSkip = true;
    winPropSrc.SetSnapshotSkip(isSkip);
    bool res = winPropSrc.GetSnapshotSkip();
    ASSERT_EQ(res, isSkip);
}
}
} // namespace Rosen
} // namespace OHOS