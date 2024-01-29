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
#include "display_manager_lite.h"
#include "mock_display_manager_adapter_lite.h"
#include "singleton_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<DisplayManagerAdapterLite, MockDisplayManagerAdapterLite>;
class DisplayTestLite : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DisplayTestLite::SetUpTestCase()
{
}

void DisplayTestLite::TearDownTestCase()
{
}

void DisplayTestLite::SetUp()
{
}

void DisplayTestLite::TearDown()
{
}

namespace {
/**
 * @tc.name: UpdateDisplayInfo01
 * @tc.desc: UpdateDisplayInfo with nullptr
 * @tc.type: FUNC
 * @tc.require: issueI5K0JP
 */
HWTEST_F(DisplayTestLite, UpdateDisplayInfo01, Function | SmallTest | Level1)
{
    sptr<DisplayInfo> displayInfo = nullptr;
    sptr<DisplayLite> display = new DisplayLite("", displayInfo);
    display->UpdateDisplayInfo(displayInfo);
    ASSERT_EQ(displayInfo, nullptr);
}

/**
 * @tc.name: UpdateDisplayInfopImpl_
 * @tc.desc: UpdateDisplayInfo with nullptr
 * @tc.type: FUNC
 * @tc.require: issueI5K0JP
 */
HWTEST_F(DisplayTestLite, UpdateDisplayInfopImpl_, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "DisplayLite::UpdateDisplayInfo start";
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    sptr<DisplayLite> display = new DisplayLite("", displayInfo);
    display->UpdateDisplayInfo(displayInfo);
    ASSERT_NE(displayInfo, nullptr);
    GTEST_LOG_(INFO) << "DisplayLite::UpdateDisplayInfo end";
}

/**
 * @tc.name: UpdateDisplayInfo03
 * @tc.desc: UpdateDisplayInfo with nullptr
 * @tc.type: FUNC
 * @tc.require: issueI5K0JP
 */
HWTEST_F(DisplayTestLite, UpdateDisplayInfo03, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "DisplayLite::UpdateDisplayInfo start";
    sptr<DisplayInfo> displayInfo = nullptr;
    sptr<DisplayLite> display = new DisplayLite("", displayInfo);
    display->UpdateDisplayInfo();
    ASSERT_EQ(displayInfo, nullptr);
    GTEST_LOG_(INFO) << "DisplayLite::UpdateDisplayInfo end";
}

/**
 * @tc.name: GetId01
 * @tc.desc: UpdateDisplayInfo with nullptr
 * @tc.type: FUNC
 * @tc.require: issueI5K0JP
 */
HWTEST_F(DisplayTestLite, GetId01, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "DisplayLite::GetId start";
    sptr<DisplayInfo> displayInfo = nullptr;
    sptr<DisplayLite> display = new DisplayLite("", displayInfo);
    uint64_t id;
    id = display->GetId();
    ASSERT_EQ(id, 0);
    GTEST_LOG_(INFO) << "DisplayLite::GetId end";
}

/**
 * @tc.name: GetId02
 * @tc.desc: UpdateDisplayInfo with nullptr
 * @tc.type: FUNC
 * @tc.require: issueI5K0JP
 */
HWTEST_F(DisplayTestLite, GetId02, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "DisplayLite::GetId start";
    DisplayInfo *displayInfo = new DisplayInfo();
    sptr<DisplayLite> display = new DisplayLite("", displayInfo);
    uint64_t id;
    id = display->GetId();
    ASSERT_NE(id, 0);
    GTEST_LOG_(INFO) << "DisplayLite::GetId end";
}

/**
 * @tc.name: GetDisplayInfo01
 * @tc.desc: UpdateDisplayInfo with nullptr
 * @tc.type: FUNC
 * @tc.require: issueI5K0JP
 */
HWTEST_F(DisplayTestLite, GetDisplayInfo01, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "DisplayLite::GetId start";
    sptr<DisplayLite> display = nullptr;
    sptr<DisplayInfo> displayInfo = new DisplayLite("", displayInfo);;
    displayInfo = display->GetDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    GTEST_LOG_(INFO) << "DisplayLite::GetId end";
}

/**
 * @tc.name: GetDisplayInfo02
 * @tc.desc: UpdateDisplayInfo with nullptr
 * @tc.type: FUNC
 * @tc.require: issueI5K0JP
 */
HWTEST_F(DisplayTestLite, GetDisplayInfo02, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "DisplayLite::GetId start";
    DisplayInfo *displayInfo = new DisplayInfo();
    sptr<DisplayLite> display = new DisplayLite("", displayInfo);;
    displayInfo = display->GetDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    GTEST_LOG_(INFO) << "DisplayLite::GetId end";
}

/**
 * @tc.name: GetWidth01
 * @tc.desc: UpdateDisplayInfo with nullptr
 * @tc.type: FUNC
 * @tc.require: issueI5K0JP
 */
HWTEST_F(DisplayTestLite, GetWidth01, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "DisplayLite::GetWidth start";
    sptr<DisplayLite> display = nullptr;
    sptr<DisplayInfo> displayInfo = new DisplayLite("", displayInfo);
    int32_t width;
    width = display->GetWidth();
    ASSERT_NE(width, 0);
    GTEST_LOG_(INFO) << "DisplayLite::GetWidth end";
}

/**
 * @tc.name: GetWidth02
 * @tc.desc: UpdateDisplayInfo with nullptr
 * @tc.type: FUNC
 * @tc.require: issueI5K0JP
 */
HWTEST_F(DisplayTestLite, GetWidth02, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "DisplayLite::GetWidth start";
    sptr<DisplayLite> display = new DisplayInfo();
    sptr<DisplayInfo> displayInfo = new DisplayLite("", displayInfo);
    int32_t width;
    width = display->GetWidth();
    ASSERT_EQ(width, 0);
    GTEST_LOG_(INFO) << "DisplayLite::GetWidth end";
}

/**
 * @tc.name: GetHeight01
 * @tc.desc: UpdateDisplayInfo with nullptr
 * @tc.type: FUNC
 * @tc.require: issueI5K0JP
 */
HWTEST_F(DisplayTestLite, GetHeight01, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "DisplayLite::GetWidth start";
    sptr<DisplayLite> display = nullptr;
    sptr<DisplayInfo> displayInfo = new DisplayLite("", displayInfo);
    int32_t Height;
    Height = display->GetWidth();
    ASSERT_NE(Height, 0);
    GTEST_LOG_(INFO) << "DisplayLite::GetWidth end";
}

/**
 * @tc.name: GetHeight02
 * @tc.desc: UpdateDisplayInfo with nullptr
 * @tc.type: FUNC
 * @tc.require: issueI5K0JP
 */
HWTEST_F(DisplayTestLite, GetHeight02, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "DisplayLite::GetWidth start";
    sptr<DisplayLite> display = new DisplayInfo();
    sptr<DisplayInfo> displayInfo = new DisplayLite("", displayInfo);
    int32_t Height;
    Height = display->GetWidth();
    ASSERT_EQ(Height, 0);
    GTEST_LOG_(INFO) << "DisplayLite::GetWidth end";
}

/**
 * @tc.name: GetCutoutInfo
 * @tc.desc: UpdateDisplayInfo with nullptr
 * @tc.type: FUNC
 * @tc.require: issueI5K0JP
 */
HWTEST_F(DisplayTestLite, GetCutoutInfo, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "DisplayLite::GetCutoutInfo start";
    sptr<DisplayLite> display = new DisplayInfo();
    sptr<DisplayInfo> displayInfo = new DisplayLite("", displayInfo);
    sptr<CutoutInfo> info = display->GetCutoutInfo();
    ASSERT_EQ(info, nullptr);
    GTEST_LOG_(INFO) << "DisplayLite::GetCutoutInfo end";
}
}
} // namespace Rosen
} // namespace OHOS