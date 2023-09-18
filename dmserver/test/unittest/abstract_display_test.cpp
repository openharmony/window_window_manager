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

#include "abstract_display.h"
#include "abstract_screen_controller.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class AbstractDisplayTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    DisplayId id = 1;
    std::string name = "abstract_display_test";
    SupportedScreenModes modesInfo;
    std::recursive_mutex mutex;
    sptr<AbstractScreenController> absController;
    sptr<AbstractScreen> absScreen;
    sptr<AbstractDisplay> absDisplay;
    sptr<AbstractDisplay> absDisplay2;
    sptr<AbstractDisplay> absDisplay3;
};

void AbstractDisplayTest::SetUpTestCase()
{
}

void AbstractDisplayTest::TearDownTestCase()
{
}

void AbstractDisplayTest::SetUp()
{
    modesInfo.width_ = 2160;
    modesInfo.height_ = 1600;
    modesInfo.refreshRate_ = 60;
    sptr<SupportedScreenModes> info = new SupportedScreenModes(modesInfo);
    absController = nullptr;
    absScreen = new AbstractScreen(absController, name, 1, 1);
    absDisplay = new AbstractDisplay(id, info, absScreen);
    modesInfo.width_ = 800;
    modesInfo.height_ = 2560;
    absDisplay2 = new AbstractDisplay(id, info, absScreen);
    modesInfo.width_ = 2560;
    modesInfo.height_ = 2560;
    absDisplay3 = new AbstractDisplay(id, info, absScreen);
}

void AbstractDisplayTest::TearDown()
{
}

namespace {
/**
 * @tc.name: BindAbstractScreen
 * @tc.desc: BindAbstractScreen test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractDisplayTest, BindAbstractScreen01, Function | SmallTest | Level3)
{
    sptr<AbstractScreen> abstractScreen = nullptr;
    ASSERT_EQ(false, absDisplay->BindAbstractScreen(abstractScreen));
}
/**
 * @tc.name: BindAbstractScreen
 * @tc.desc: BindAbstractScreen test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractDisplayTest, BindAbstractScreen02, Function | SmallTest | Level3)
{
    sptr<AbstractScreen> abstractScreen = absScreen;
    abstractScreen->activeIdx_ = -1;
    ASSERT_EQ(false, absDisplay->BindAbstractScreen(abstractScreen));
}
/**
 * @tc.name: CalculateXYDpi
 * @tc.desc: CalculateXYDpi test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractDisplayTest, CalculateXYDpi, Function | SmallTest | Level3)
{
    uint32_t phyWidth = 0;
    uint32_t phyHeight = 0;
    absDisplay->CalculateXYDpi(phyWidth, phyHeight);
    phyWidth = 1;
    absDisplay->CalculateXYDpi(phyWidth, phyHeight);
    phyHeight = 1;
    absDisplay->CalculateXYDpi(phyWidth, phyHeight);
    phyWidth = 0;
    absDisplay->CalculateXYDpi(phyWidth, phyHeight);
    ASSERT_EQ(1, absDisplay->phyHeight_);
}
/**
 * @tc.name: GetRefreshRate
 * @tc.desc: GetRefreshRate test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractDisplayTest, GetRefreshRate, Function | SmallTest | Level3)
{
    uint32_t refreshRate = 1;
    absDisplay->SetRefreshRate(refreshRate);
    absDisplay->GetRefreshRate();
    ASSERT_EQ(1, absDisplay->refreshRate_);
}
/**
 * @tc.name: GetOffsetX
 * @tc.desc: GetOffsetX test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractDisplayTest, GetOffsetX, Function | SmallTest | Level3)
{
    int32_t offsetX = 1;
    absDisplay->SetOffsetX(offsetX);
    ASSERT_EQ(1, absDisplay->GetOffsetX());
}
/**
 * @tc.name: GetOffsetY
 * @tc.desc: GetOffsetX test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractDisplayTest, GetOffsetY, Function | SmallTest | Level3)
{
    int32_t offsetY = 1;
    absDisplay->SetOffsetY(offsetY);
    ASSERT_EQ(1, absDisplay->GetOffsetY());
}
/**
 * @tc.name: UpdateXDpi
 * @tc.desc: UpdateXDpi test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractDisplayTest, UpdateXDpi, Function | SmallTest | Level3)
{
    uint32_t phyWidth = UINT32_MAX;
    uint32_t phyHeight = 0;
    absDisplay->CalculateXYDpi(phyWidth, phyHeight);
    absDisplay->UpdateXDpi();
    ASSERT_EQ(UINT32_MAX, absDisplay->phyWidth_);
}
/**
 * @tc.name: UpdateYDpi
 * @tc.desc: UpdateYDpi test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractDisplayTest, UpdateYDpi, Function | SmallTest | Level3)
{
    uint32_t phyWidth = UINT32_MAX;
    uint32_t phyHeight = UINT32_MAX;
    absDisplay->CalculateXYDpi(phyWidth, phyHeight);
    absDisplay->UpdateYDpi();
    ASSERT_EQ(UINT32_MAX, absDisplay->phyHeight_);
}
/**
 * @tc.name: SetId
 * @tc.desc: SetId test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractDisplayTest, SetId, Function | SmallTest | Level3)
{
    DisplayId id = 1;
    absDisplay->SetId(id);
    ASSERT_EQ(1, absDisplay->GetId());
}
/**
 * @tc.name: SetDisplayOrientation
 * @tc.desc: SetDisplayOrientation test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractDisplayTest, SetDisplayOrientation, Function | SmallTest | Level3)
{
    DisplayOrientation displayOrientation = DisplayOrientation::PORTRAIT;
    absDisplay->SetDisplayOrientation(displayOrientation);
    ASSERT_EQ(DisplayOrientation::PORTRAIT, absDisplay->GetDisplayOrientation());
}
/**
 * @tc.name: GetRotationAndGetOrientation
 * @tc.desc: GetRotationAndGetOrientation test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractDisplayTest, GetRotationAndGetOrientation, Function | SmallTest | Level3)
{
    DisplayId id = 1;
    absDisplay->SetId(id);
    absDisplay->GetRotation();
    absDisplay->GetOrientation();
    ASSERT_EQ(1, absDisplay->GetId());
}
/**
 * @tc.name: SetFreezeFlag
 * @tc.desc: SetFreezeFlag test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractDisplayTest, SetFreezeFlag, Function | SmallTest | Level3)
{
    FreezeFlag freezeFlag = FreezeFlag::FREEZING;
    absDisplay->SetFreezeFlag(freezeFlag);
    ASSERT_EQ(FreezeFlag::FREEZING, absDisplay->GetFreezeFlag());
}
}
} // namespace Rosen
} // namespace OHOS
