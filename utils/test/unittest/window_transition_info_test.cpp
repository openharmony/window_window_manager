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
#include "window_transition_info.h"
#include "iremote_object_mocker.h"

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

void WindowTransitionInfoTest::SetUpTestCase()
{
}

void WindowTransitionInfoTest::TearDownTestCase()
{
}

void WindowTransitionInfoTest::SetUp()
{
}

void WindowTransitionInfoTest::TearDown()
{
}

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
HWTEST_F(WindowTransitionInfoTest, WindowTransitionInfo01, Function | SmallTest | Level2)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);
    info->bundleName_ = "TestAbilityTransitionInfo1";
    sptr<WindowTransitionInfo> winTransitionInfo1 = new WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo1);
    ASSERT_EQ(info->bundleName_, winTransitionInfo1->bundleName_);
    ASSERT_EQ(3, winTransitionInfo1->supportWindowModes_.size());
    ASSERT_EQ(AppExecFwk::SupportWindowMode::FULLSCREEN, winTransitionInfo1->supportWindowModes_[0]);
    ASSERT_EQ(AppExecFwk::SupportWindowMode::SPLIT, winTransitionInfo1->supportWindowModes_[1]);
    ASSERT_EQ(AppExecFwk::SupportWindowMode::FLOATING, winTransitionInfo1->supportWindowModes_[2]);

    info->bundleName_ = "TestAbilityTransitionInfo2";
    info->windowModes_.emplace_back(AppExecFwk::SupportWindowMode::SPLIT);
    sptr<WindowTransitionInfo> winTransitionInfo2 = new WindowTransitionInfo(info);
    ASSERT_EQ(info->bundleName_, winTransitionInfo2->bundleName_);
    ASSERT_EQ(1, winTransitionInfo2->supportWindowModes_.size());
    ASSERT_EQ(AppExecFwk::SupportWindowMode::SPLIT, winTransitionInfo2->supportWindowModes_[0]);
}
/**
 * @tc.name: Marshalling
 * @tc.desc: WindowTransitionInfo::Marshalling test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, Marshalling01, Function | SmallTest | Level2)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);

    winTransitionInfo->bundleName_ = "bundleNameValue";
    winTransitionInfo->abilityName_ = "abilityNameValue";

    bool result = winTransitionInfo->Marshalling(parcel);
    ASSERT_EQ(true, result);

    sptr<IRemoteObject> romote = new RemoteMocker();
    winTransitionInfo->abilityToken_ = romote;
    result = winTransitionInfo->Marshalling(parcel);
    ASSERT_EQ(true, result);
}

/**
 * @tc.name: GetBundleName
 * @tc.desc: WindowTransitionInfo::GetBundleName test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetBundleName, Function | SmallTest | Level2)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new WindowTransitionInfo(info);
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
HWTEST_F(WindowTransitionInfoTest, GetAbilityName, Function | SmallTest | Level2)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new WindowTransitionInfo(info);
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
HWTEST_F(WindowTransitionInfoTest, GetWindowMode, Function | SmallTest | Level2)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);
    WindowMode mode = WindowMode{1};
    winTransitionInfo->SetWindowMode(mode);
    auto ret = winTransitionInfo->GetWindowMode();
    ASSERT_EQ(ret, mode);
}

/**
 * @tc.name: GetWindowRect
 * @tc.desc: WindowTransitionInfo::GetWindowRect test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetWindowRect, Function | SmallTest | Level2)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);
    Rect rect = {0, 0, 50, 100};
    winTransitionInfo->SetWindowRect(rect);
    auto ret = winTransitionInfo->GetWindowRect();
    ASSERT_EQ(ret, rect);
}

/**
 * @tc.name: GetAbilityToken
 * @tc.desc: WindowTransitionInfo::GetAbilityToken test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetAbilityToken, Function | SmallTest | Level2)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new WindowTransitionInfo(info);
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
HWTEST_F(WindowTransitionInfoTest, GetDisplayId, Function | SmallTest | Level2)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new WindowTransitionInfo(info);
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
HWTEST_F(WindowTransitionInfoTest, GetWindowType, Function | SmallTest | Level2)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);
    WindowType windowType = WindowType{1};
    winTransitionInfo->SetWindowType(windowType);
    auto ret = winTransitionInfo->GetWindowType();
    ASSERT_EQ(ret, windowType);
}

/**
 * @tc.name: GetShowFlagWhenLocked
 * @tc.desc: WindowTransitionInfo::GetShowFlagWhenLocked test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetShowFlagWhenLocked, Function | SmallTest | Level2)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);
    winTransitionInfo->SetShowFlagWhenLocked(false);
    auto ret = winTransitionInfo->GetShowFlagWhenLocked();
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: GetIsRecent
 * @tc.desc: WindowTransitionInfo::GetIsRecent test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetIsRecent, Function | SmallTest | Level2)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new WindowTransitionInfo(info);
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
HWTEST_F(WindowTransitionInfoTest, GetApiCompatibleVersion, Function | SmallTest | Level2)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);
    auto ret = winTransitionInfo->GetApiCompatibleVersion();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: GetMissionId
 * @tc.desc: WindowTransitionInfo::GetMissionId test
 * @tc.type: FUNC
 */
HWTEST_F(WindowTransitionInfoTest, GetMissionId, Function | SmallTest | Level2)
{
    sptr<AAFwk::AbilityTransitionInfo> info = new AAFwk::AbilityTransitionInfo();
    ASSERT_NE(nullptr, info);

    Parcel parcel;
    sptr<WindowTransitionInfo> winTransitionInfo = new WindowTransitionInfo(info);
    ASSERT_NE(nullptr, winTransitionInfo);
    winTransitionInfo->SetMissionId(0);
    auto ret = winTransitionInfo->GetMissionId();
    ASSERT_EQ(ret, 0);
}
}
} // namespace Rosen
} // namespace OHOS