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
#include "display_cutout_controller.h"
#include "display_manager_service_inner.h"
#include "dm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DisplayCutoutControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DisplayCutoutControllerTest::SetUpTestCase()
{
}

void DisplayCutoutControllerTest::TearDownTestCase()
{
}

void DisplayCutoutControllerTest::SetUp()
{
}

void DisplayCutoutControllerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: SetCurvedScreenBoundary
 * @tc.desc: SetCurvedScreenBoundary size < 4
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCutoutControllerTest, SetCurvedScreenBoundary, Function | SmallTest | Level1)
{
    sptr<DisplayCutoutController> controller = new DisplayCutoutController();
    std::vector<int> curvedScreenBoundary;
    curvedScreenBoundary.emplace_back(2);
    controller->SetCurvedScreenBoundary(curvedScreenBoundary);
    ASSERT_EQ(controller->curvedScreenBoundary_.size(), 4);
}

/**
 * @tc.name: SetCutoutSvgPath
 * @tc.desc: SetCutoutSvgPath
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCutoutControllerTest, SetCutoutSvgPath, Function | SmallTest | Level1)
{
    sptr<DisplayCutoutController> controller = new DisplayCutoutController();
    DisplayId displayId = 1;
    std::string svgPath = "m10";
    controller->SetCutoutSvgPath(displayId, svgPath);
    controller->SetCutoutSvgPath(displayId, svgPath);
    ASSERT_EQ(controller->svgPaths_.size(), 1);
    ASSERT_EQ(controller->svgPaths_[displayId].size(), 2);
}

/**
 * @tc.name: GetCutoutInfo
 * @tc.desc: GetCutoutInfo
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCutoutControllerTest, GetCutoutInfo, Function | SmallTest | Level1)
{
    sptr<DisplayCutoutController> controller = new DisplayCutoutController();
    DisplayId displayId = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    std::string svgPath = "m10";
    controller->SetCutoutSvgPath(displayId, svgPath);
    controller->SetIsWaterfallDisplay(true);
    sptr<CutoutInfo> cutoutInfo = controller->GetCutoutInfo(displayId);
    ASSERT_NE(cutoutInfo, nullptr);
}


/**
 * @tc.name: CalcBuiltInDisplayWaterfallRects
 * @tc.desc: CalcBuiltInDisplayWaterfallRects
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCutoutControllerTest, CalcBuiltInDisplayWaterfallRects, Function | SmallTest | Level1)
{
    sptr<DisplayCutoutController> controller = new DisplayCutoutController();
    controller->SetIsWaterfallDisplay(true);
    std::vector<int> curvedScreenBoundary;
    controller->SetCurvedScreenBoundary(curvedScreenBoundary);
    controller->CalcBuiltInDisplayWaterfallRects();
    curvedScreenBoundary.emplace_back(1);
    curvedScreenBoundary.emplace_back(2);
    curvedScreenBoundary.emplace_back(3);
    curvedScreenBoundary.emplace_back(4);
    controller->SetCurvedScreenBoundary(curvedScreenBoundary);
    controller->CalcBuiltInDisplayWaterfallRects();
}


/**
 * @tc.name: CalcBuiltInDisplayWaterfallRectsByRotation
 * @tc.desc: CalcBuiltInDisplayWaterfallRectsByRotation
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCutoutControllerTest, CalcBuiltInDisplayWaterfallRectsByRotation, Function | SmallTest | Level1)
{
    sptr<DisplayCutoutController> controller = new DisplayCutoutController();
    uint32_t displayHeight = 300;
    uint32_t displayWidth = 300;
    std::vector<int> curvedScreenBoundary;
    curvedScreenBoundary.emplace_back(0);
    curvedScreenBoundary.emplace_back(0);
    curvedScreenBoundary.emplace_back(500);
    curvedScreenBoundary.emplace_back(500);
    controller->SetCurvedScreenBoundary(curvedScreenBoundary);
    controller->CalcBuiltInDisplayWaterfallRectsByRotation(Rotation::ROTATION_270, displayHeight, displayWidth);
    controller->CalcBuiltInDisplayWaterfallRectsByRotation(Rotation::ROTATION_180, displayHeight, displayWidth);
    controller->CalcBuiltInDisplayWaterfallRectsByRotation(Rotation::ROTATION_90, displayHeight, displayWidth);
    controller->CalcBuiltInDisplayWaterfallRectsByRotation(Rotation::ROTATION_0, displayHeight, displayWidth);
    controller->CalcBuiltInDisplayWaterfallRectsByRotation(static_cast<Rotation>(10), displayHeight, displayWidth);
}

}
} // Rosen
} // OHOS