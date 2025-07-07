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

#include <iremote_broker.h>
#include <iremote_object.h>
#include "iremote_object_mocker.h"
#include "window_transition_info.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using RemoteMocker = IRemoteObjectMocker;
class WindowTransitionInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void SetTransitionInfo(sptr<WindowTransitionInfo> info);
};

void WindowTransitionInfoTest::SetUpTestCase() {}

void WindowTransitionInfoTest::TearDownTestCase() {}

void WindowTransitionInfoTest::SetUp() {}

void WindowTransitionInfoTest::TearDown() {}

void WindowTransitionInfoTest::SetTransitionInfo(sptr<WindowTransitionInfo> info)
{
    info->bundleName_ = "bundleName";
    info->abilityName_ = "abilityName";
    info->mode_ = WindowMode::WINDOW_MODE_FULLSCREEN;
    info->windowRect_ = { 10, 10, 10, 10 };
    info->displayId_ = 0;
}

namespace {
/**
 * @tc.name: WindowTransitionInfo01
 * @tc.desc: WindowTransitionInfo::WindowTransitionInfo test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, WindowTransitionInfo01, TestSize.Level1)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new (std::nothrow) AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);
    info->bundleName_ = "TestAbilityTransitionInfo1";
    sptr<WindowTransitionInfo> winTransitionInfo1 = new (std::nothrow) WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo1);
    ASSERT_EQ(info->bundleName_, winTransitionInfo1->bundleName_);
    ASSERT_EQ(3, winTransitionInfo1->supportWindowModes_.size());
    ASSERT_EQ(AppExecFwk::SupportWindowMode::FULLSCREEN, winTransitionInfo1->supportWindowModes_[0]);
    ASSERT_EQ(AppExecFwk::SupportWindowMode::SPLIT, winTransitionInfo1->supportWindowModes_[1]);
    ASSERT_EQ(AppExecFwk::SupportWindowMode::FLOATING, winTransitionInfo1->supportWindowModes_[2]);

    info->bundleName_ = "TestAbilityTransitionInfo2";
    info->windowModes_.emplace_back(AppExecFwk::SupportWindowMode::SPLIT);
    sptr<WindowTransitionInfo> winTransitionInfo2 = new (std::nothrow) WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo2);
    ASSERT_EQ(info->bundleName_, winTransitionInfo2->bundleName_);
    ASSERT_EQ(1, winTransitionInfo2->supportWindowModes_.size());
    ASSERT_EQ(AppExecFwk::SupportWindowMode::SPLIT, winTransitionInfo2->supportWindowModes_[0]);
}

/**
 * @tc.name: Marshalling
 * @tc.desc: WindowTransitionInfo::Marshalling test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, Marshalling01, TestSize.Level1)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new (std::nothrow) AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new (std::nothrow) WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);

    winTransitionInfo->bundleName_ = "bundleNameValue";
    winTransitionInfo->abilityName_ = "abilityNameValue";

    bool result = winTransitionInfo->Marshalling(parcel);
    ASSERT_EQ(true, result);

    sptr<IRemoteObject> remote = new (std::nothrow) RemoteMocker();
    winTransitionInfo->abilityToken_ = remote;
    result = winTransitionInfo->Marshalling(parcel);
    ASSERT_EQ(true, result);
}

/**
 * @tc.name: Unmarshalling
 * @tc.desc: WindowTransitionInfo::Unmarshalling test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, Unmarshalling, TestSize.Level1)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new (std::nothrow) AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new (std::nothrow) WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);

    winTransitionInfo->bundleName_ = "bundleNameValue";
    winTransitionInfo->abilityName_ = "abilityNameValue";

    sptr<IRemoteObject> remote = new (std::nothrow) RemoteMocker();
    winTransitionInfo->abilityToken_ = remote;
    auto result = winTransitionInfo->Marshalling(parcel);
    ASSERT_EQ(true, result);

    auto transitionInfo = winTransitionInfo->Unmarshalling(parcel);
    ASSERT_NE(transitionInfo, nullptr);
    ASSERT_EQ(transitionInfo->bundleName_, "bundleNameValue");
    ASSERT_EQ(transitionInfo->abilityName_, "abilityNameValue");
    ASSERT_NE(transitionInfo->abilityToken_, nullptr);
}

/**
 * @tc.name: GetBundleName
 * @tc.desc: WindowTransitionInfo::GetBundleName test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetBundleName, TestSize.Level1)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new (std::nothrow) AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new (std::nothrow) WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);
    std::string name = "bundleNameValue";
    winTransitionInfo->SetBundleName(name);
    auto ret = winTransitionInfo->GetBundleName();
    ASSERT_EQ(ret, name);
}

/**
 * @tc.name: GetAbilityName
 * @tc.desc: WindowTransitionInfo::GetAbilityName test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetAbilityName, TestSize.Level1)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new (std::nothrow) AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new (std::nothrow) WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);
    std::string name = "abilityNameValue";
    winTransitionInfo->SetAbilityName(name);
    auto ret = winTransitionInfo->GetAbilityName();
    ASSERT_EQ(ret, name);
}

/**
 * @tc.name: GetWindowMode
 * @tc.desc: WindowTransitionInfo::GetWindowMode test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetWindowMode, TestSize.Level1)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new (std::nothrow) AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new (std::nothrow) WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);
    WindowMode mode = WindowMode{ 1 };
    winTransitionInfo->SetWindowMode(mode);
    auto ret = winTransitionInfo->GetWindowMode();
    ASSERT_EQ(ret, mode);
}

/**
 * @tc.name: GetWindowRect
 * @tc.desc: WindowTransitionInfo::GetWindowRect test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetWindowRect, TestSize.Level1)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new (std::nothrow) AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new (std::nothrow) WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);
    Rect rect = { 0, 0, 50, 100 };
    winTransitionInfo->SetWindowRect(rect);
    auto ret = winTransitionInfo->GetWindowRect();
    ASSERT_EQ(ret, rect);
}

/**
 * @tc.name: GetAbilityToken
 * @tc.desc: WindowTransitionInfo::GetAbilityToken test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetAbilityToken, TestSize.Level1)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new (std::nothrow) AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new (std::nothrow) WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);
    sptr<IRemoteObject> abilityToken;
    winTransitionInfo->SetAbilityToken(abilityToken);
    auto ret = winTransitionInfo->GetAbilityToken();
    ASSERT_EQ(ret, abilityToken);
}

/**
 * @tc.name: GetDisplayId
 * @tc.desc: WindowTransitionInfo::GetDisplayId test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetDisplayId, TestSize.Level1)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new (std::nothrow) AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new (std::nothrow) WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);
    winTransitionInfo->SetDisplayId(0);
    auto ret = winTransitionInfo->GetDisplayId();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: GetWindowType
 * @tc.desc: WindowTransitionInfo::GetWindowType test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetWindowType, TestSize.Level1)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new (std::nothrow) AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new (std::nothrow) WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);
    WindowType windowType = WindowType{ 1 };
    winTransitionInfo->SetWindowType(windowType);
    auto ret = winTransitionInfo->GetWindowType();
    ASSERT_EQ(ret, windowType);
}

/**
 * @tc.name: GetShowFlagWhenLocked
 * @tc.desc: WindowTransitionInfo::GetShowFlagWhenLocked test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetShowFlagWhenLocked, TestSize.Level1)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new (std::nothrow) AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new (std::nothrow) WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);
    winTransitionInfo->SetShowFlagWhenLocked(false);
    auto ret = winTransitionInfo->GetShowFlagWhenLocked();
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: GetWindowSupportModes
 * @tc.desc: WindowTransitionInfo::GetWindowSupportModes test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetWindowSupportModes, TestSize.Level1)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new (std::nothrow) AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);
    sptr<WindowTransitionInfo> winTransitionInfo = new (std::nothrow) WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);
    std::vector<AppExecFwk::SupportWindowMode> supportModesIn = { AppExecFwk::SupportWindowMode::FULLSCREEN,
                                                                  AppExecFwk::SupportWindowMode::SPLIT,
                                                                  AppExecFwk::SupportWindowMode::FLOATING };
    winTransitionInfo->SetWindowSupportModes(supportModesIn);
    auto supportModesOut = winTransitionInfo->GetWindowSupportModes();
    ASSERT_EQ(supportModesOut.size(), 3);
    ASSERT_EQ(supportModesOut[0], AppExecFwk::SupportWindowMode::FULLSCREEN);
    ASSERT_EQ(supportModesOut[1], AppExecFwk::SupportWindowMode::SPLIT);
    ASSERT_EQ(supportModesOut[2], AppExecFwk::SupportWindowMode::FLOATING);
}

/**
 * @tc.name: GetWindowSizeLimits
 * @tc.desc: WindowTransitionInfo::GetWindowSizeLimits test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetWindowSizeLimits, TestSize.Level1)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new (std::nothrow) AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);
    info->maxWindowRatio_ = 2.0f;
    info->minWindowRatio_ = 1.0f;
    info->maxWindowWidth_ = 2048;
    info->minWindowWidth_ = 512;
    info->maxWindowHeight_ = 2048;
    info->minWindowHeight_ = 512;
    sptr<WindowTransitionInfo> winTransitionInfo = new (std::nothrow) WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);
    auto windowSizeLimits = winTransitionInfo->GetWindowSizeLimits();
    ASSERT_FLOAT_EQ(windowSizeLimits.maxRatio_, 2.0f);
    ASSERT_FLOAT_EQ(windowSizeLimits.minRatio_, 1.0f);
    ASSERT_EQ(windowSizeLimits.maxWidth_, 2048);
    ASSERT_EQ(windowSizeLimits.minWidth_, 512);
    ASSERT_EQ(windowSizeLimits.maxHeight_, 2048);
    ASSERT_EQ(windowSizeLimits.minHeight_, 512);
}

/**
 * @tc.name: GetTransitionReason
 * @tc.desc: WindowTransitionInfo::GetTransitionReason test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetTransitionReason, TestSize.Level1)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new (std::nothrow) AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);
    sptr<WindowTransitionInfo> winTransitionInfo = new (std::nothrow) WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);

    auto reasonIn = TransitionReason::MINIMIZE;
    winTransitionInfo->SetTransitionReason(reasonIn);
    auto reasonOut = winTransitionInfo->GetTransitionReason();
    ASSERT_EQ(reasonOut, TransitionReason::MINIMIZE);

    reasonIn = TransitionReason::CLOSE;
    winTransitionInfo->SetTransitionReason(reasonIn);
    reasonOut = winTransitionInfo->GetTransitionReason();
    ASSERT_EQ(reasonOut, TransitionReason::CLOSE);

    reasonIn = TransitionReason::ABILITY_TRANSITION;
    winTransitionInfo->SetTransitionReason(reasonIn);
    reasonOut = winTransitionInfo->GetTransitionReason();
    ASSERT_EQ(reasonOut, TransitionReason::ABILITY_TRANSITION);

    reasonIn = TransitionReason::BACK_TRANSITION;
    winTransitionInfo->SetTransitionReason(reasonIn);
    reasonOut = winTransitionInfo->GetTransitionReason();
    ASSERT_EQ(reasonOut, TransitionReason::BACK_TRANSITION);

    reasonIn = TransitionReason::CLOSE_BUTTON;
    winTransitionInfo->SetTransitionReason(reasonIn);
    reasonOut = winTransitionInfo->GetTransitionReason();
    ASSERT_EQ(reasonOut, TransitionReason::CLOSE_BUTTON);

    reasonIn = TransitionReason::BACKGROUND_TRANSITION;
    winTransitionInfo->SetTransitionReason(reasonIn);
    reasonOut = winTransitionInfo->GetTransitionReason();
    ASSERT_EQ(reasonOut, TransitionReason::BACKGROUND_TRANSITION);
}

/**
 * @tc.name: GetOrientation01
 * @tc.desc: WindowTransitionInfo::GetOrientation test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetOrientation01, TestSize.Level1)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new (std::nothrow) AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);
    sptr<WindowTransitionInfo> winTransitionInfo = new (std::nothrow) WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);

    auto orientationIn = AppExecFwk::DisplayOrientation::UNSPECIFIED;
    winTransitionInfo->SetOrientation(orientationIn);
    auto orientationOut = winTransitionInfo->GetOrientation();
    ASSERT_EQ(orientationOut, AppExecFwk::DisplayOrientation::UNSPECIFIED);

    orientationIn = AppExecFwk::DisplayOrientation::LANDSCAPE;
    winTransitionInfo->SetOrientation(orientationIn);
    orientationOut = winTransitionInfo->GetOrientation();
    ASSERT_EQ(orientationOut, AppExecFwk::DisplayOrientation::LANDSCAPE);

    orientationIn = AppExecFwk::DisplayOrientation::PORTRAIT;
    winTransitionInfo->SetOrientation(orientationIn);
    orientationOut = winTransitionInfo->GetOrientation();
    ASSERT_EQ(orientationOut, AppExecFwk::DisplayOrientation::PORTRAIT);

    orientationIn = AppExecFwk::DisplayOrientation::FOLLOWRECENT;
    winTransitionInfo->SetOrientation(orientationIn);
    orientationOut = winTransitionInfo->GetOrientation();
    ASSERT_EQ(orientationOut, AppExecFwk::DisplayOrientation::FOLLOWRECENT);

    orientationIn = AppExecFwk::DisplayOrientation::LANDSCAPE_INVERTED;
    winTransitionInfo->SetOrientation(orientationIn);
    orientationOut = winTransitionInfo->GetOrientation();
    ASSERT_EQ(orientationOut, AppExecFwk::DisplayOrientation::LANDSCAPE_INVERTED);

    orientationIn = AppExecFwk::DisplayOrientation::PORTRAIT_INVERTED;
    winTransitionInfo->SetOrientation(orientationIn);
    orientationOut = winTransitionInfo->GetOrientation();
    ASSERT_EQ(orientationOut, AppExecFwk::DisplayOrientation::PORTRAIT_INVERTED);

    orientationIn = AppExecFwk::DisplayOrientation::AUTO_ROTATION;
    winTransitionInfo->SetOrientation(orientationIn);
    orientationOut = winTransitionInfo->GetOrientation();
    ASSERT_EQ(orientationOut, AppExecFwk::DisplayOrientation::AUTO_ROTATION);
}

/**
 * @tc.name: GetOrientation02
 * @tc.desc: WindowTransitionInfo::GetOrientation test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetOrientation02, TestSize.Level1)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new (std::nothrow) AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);
    sptr<WindowTransitionInfo> winTransitionInfo = new (std::nothrow) WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);

    auto orientationIn = AppExecFwk::DisplayOrientation::AUTO_ROTATION_LANDSCAPE;
    winTransitionInfo->SetOrientation(orientationIn);
    auto orientationOut = winTransitionInfo->GetOrientation();
    ASSERT_EQ(orientationOut, AppExecFwk::DisplayOrientation::AUTO_ROTATION_LANDSCAPE);

    orientationIn = AppExecFwk::DisplayOrientation::AUTO_ROTATION_PORTRAIT;
    winTransitionInfo->SetOrientation(orientationIn);
    orientationOut = winTransitionInfo->GetOrientation();
    ASSERT_EQ(orientationOut, AppExecFwk::DisplayOrientation::AUTO_ROTATION_PORTRAIT);

    orientationIn = AppExecFwk::DisplayOrientation::AUTO_ROTATION_RESTRICTED;
    winTransitionInfo->SetOrientation(orientationIn);
    orientationOut = winTransitionInfo->GetOrientation();
    ASSERT_EQ(orientationOut, AppExecFwk::DisplayOrientation::AUTO_ROTATION_RESTRICTED);

    orientationIn = AppExecFwk::DisplayOrientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED;
    winTransitionInfo->SetOrientation(orientationIn);
    orientationOut = winTransitionInfo->GetOrientation();
    ASSERT_EQ(orientationOut, AppExecFwk::DisplayOrientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED);

    orientationIn = AppExecFwk::DisplayOrientation::AUTO_ROTATION_PORTRAIT_RESTRICTED;
    winTransitionInfo->SetOrientation(orientationIn);
    orientationOut = winTransitionInfo->GetOrientation();
    ASSERT_EQ(orientationOut, AppExecFwk::DisplayOrientation::AUTO_ROTATION_PORTRAIT_RESTRICTED);

    orientationIn = AppExecFwk::DisplayOrientation::LOCKED;
    winTransitionInfo->SetOrientation(orientationIn);
    orientationOut = winTransitionInfo->GetOrientation();
    ASSERT_EQ(orientationOut, AppExecFwk::DisplayOrientation::LOCKED);
}

/**
 * @tc.name: GetIsRecent
 * @tc.desc: WindowTransitionInfo::GetIsRecent test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetIsRecent, TestSize.Level1)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new (std::nothrow) AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new (std::nothrow) WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);
    winTransitionInfo->SetIsRecent(false);
    auto ret = winTransitionInfo->GetIsRecent();
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: GetApiCompatibleVersion
 * @tc.desc: WindowTransitionInfo::GetApiCompatibleVersion test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetApiCompatibleVersion, TestSize.Level1)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new (std::nothrow) AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new (std::nothrow) WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);
    winTransitionInfo->SetApiCompatibleVersion(100);
    auto ret = winTransitionInfo->GetApiCompatibleVersion();
    ASSERT_EQ(ret, 100);
}

/**
 * @tc.name: GetMissionId
 * @tc.desc: WindowTransitionInfo::GetMissionId test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetMissionId, TestSize.Level1)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new (std::nothrow) AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new (std::nothrow) WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);
    winTransitionInfo->SetMissionId(0);
    auto ret = winTransitionInfo->GetMissionId();
    ASSERT_EQ(ret, 0);
}
} // namespace
} // namespace Rosen
} // namespace OHOS