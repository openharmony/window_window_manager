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
#include "interfaces/include/ws_common.h"
#include "screen_scene_config.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "window_manager_agent.h"
#include "session_manager.h"
#include "screen_cutout_controller.h"
#include "zidl/window_manager_agent_interface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint32_t SLEEP_TIME_US = 100000;
}

class ScreenCutoutControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ScreenCutoutControllerTest::SetUpTestCase()
{
}

void ScreenCutoutControllerTest::TearDownTestCase()
{
}

void ScreenCutoutControllerTest::SetUp()
{
}

void ScreenCutoutControllerTest::TearDown()
{
    usleep(SLEEP_TIME_US);
}

namespace {

    /**
     * @tc.name: CreateWaterfallRect
     * @tc.desc: CreateWaterfallRect func
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, CreateWaterfallRect, Function | SmallTest | Level3)
    {
        DMRect emptyRect = {0, 0, 0, 0};
        DMRect emptyRect_ = {1, 2, 3, 3};
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();

        DMRect result = controller->CreateWaterfallRect(0, 0, 0, 0);
        ASSERT_EQ(result, emptyRect);
        DMRect result_ = controller->CreateWaterfallRect(1, 2, 3, 3);
        ASSERT_EQ(result_, emptyRect_);
    }

    /**
     * @tc.name: GetScreenCutoutInfo01
     * @tc.desc: GetScreenCutoutInfo func
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, GetScreenCutoutInfo01, Function | SmallTest | Level3)
    {
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        DisplayId displayId = 0;
        ASSERT_NE(nullptr, controller->GetScreenCutoutInfo(displayId));
    }

    /**
     * @tc.name: GetScreenCutoutInfo02
     * @tc.desc: GetScreenCutoutInfo func
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, GetScreenCutoutInfo02, Function | SmallTest | Level3)
    {
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        DisplayId displayId = 1;
        DMRect emptyRect = {0, 0, 0, 0};
        DMRect emptyRect_ = {1, 2, 3, 3};
        std::vector<DMRect> boundaryRects = {emptyRect, emptyRect_};
        ASSERT_NE(nullptr, controller->GetScreenCutoutInfo(displayId));
    }

    /**
     * @tc.name: ConvertBoundaryRectsByRotation
     * @tc.desc: ScreenCutoutController convert boundary rects by rotation
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, ConvertBoundaryRectsByRotation, Function | SmallTest | Level3)
    {
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        DMRect emptyRect = {0, 0, 0, 0};
        DMRect emptyRect_ = {1, 2, 3, 3};
        DisplayId displayId = 0;
        std::vector<DMRect> boundaryRects = {emptyRect, emptyRect_};
        ASSERT_TRUE(controller != nullptr);
        controller->ConvertBoundaryRectsByRotation(boundaryRects, displayId);
    }

      /**
     * @tc.name: CurrentRotation90
     * @tc.desc: CurrentRotation90 func
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, CurrentRotation90, Function | SmallTest | Level3)
    {
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        std::vector<DMRect> displayBoundaryRects;
        DMRect emptyRect = {10, 10, 20, 20};
        DMRect emptyRect_ = {30, 30, 40, 40};
        displayBoundaryRects.push_back(emptyRect);
        displayBoundaryRects.push_back(emptyRect_);

        std::vector<DMRect> finalVector;
        uint32_t displayWidth = 100;
        controller->CurrentRotation90(displayBoundaryRects, finalVector, displayWidth);

        ASSERT_EQ(finalVector.size(), 2);
        ASSERT_EQ(finalVector[0].posX_, 70);
        ASSERT_EQ(finalVector[0].posY_, 10);
        ASSERT_EQ(finalVector[0].width_, 20);
        ASSERT_EQ(finalVector[0].height_, 20);
    }

     /**
     * @tc.name: CurrentRotation180
     * @tc.desc: CurrentRotation180 func
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, CurrentRotation180, Function | SmallTest | Level3)
    {
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        std::vector<DMRect> displayBoundaryRects;
        DMRect emptyRect = {10, 10, 20, 20};
        DMRect emptyRect_ = {30, 30, 40, 40};
        displayBoundaryRects.push_back(emptyRect);
        displayBoundaryRects.push_back(emptyRect_);

        std::vector<DMRect> finalVector;
        uint32_t displayWidth = 100;
        uint32_t displayHeight = 200;
        controller->CurrentRotation180(displayBoundaryRects, finalVector, displayWidth, displayHeight);

        ASSERT_EQ(finalVector.size(), 2);
        ASSERT_EQ(finalVector[0].posX_, 70);
        ASSERT_EQ(finalVector[0].posY_, 170);
        ASSERT_EQ(finalVector[0].width_, 20);
        ASSERT_EQ(finalVector[0].height_, 20);
    }

    /**
     * @tc.name: CurrentRotation270
     * @tc.desc: CurrentRotation270 func
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, CurrentRotation270, Function | SmallTest | Level3)
    {
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        std::vector<DMRect> displayBoundaryRects;
        DMRect emptyRect = {10, 10, 20, 20};
        DMRect emptyRect_ = {30, 30, 40, 40};
        displayBoundaryRects.push_back(emptyRect);
        displayBoundaryRects.push_back(emptyRect_);

        std::vector<DMRect> finalVector;
        uint32_t displayHeight = 200;
        controller->CurrentRotation270(displayBoundaryRects, finalVector, displayHeight);

        ASSERT_EQ(finalVector.size(), 2);
        ASSERT_EQ(finalVector[0].posX_, 10);
        ASSERT_EQ(finalVector[0].posY_, 170);
        ASSERT_EQ(finalVector[0].width_, 20);
        ASSERT_EQ(finalVector[0].height_, 20);
    }

    /**
     * @tc.name: CheckBoundaryRects
     * @tc.desc: ScreenCutoutController check boundary rects
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, CheckBoundaryRects, Function | SmallTest | Level3)
    {
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        DMRect emptyRect = {-15, -15, 8, 8};
        DMRect emptyRect_ = {21, 21, 3, 3};
        std::vector<DMRect> boundaryRects = {emptyRect_, emptyRect};
        ScreenProperty screenProperty;
        auto screenBounds = RRect({ 0, 0, 35, 35 }, 0.0f, 0.0f);
        screenProperty.SetBounds(screenBounds);
        ASSERT_TRUE(controller != nullptr);
        controller->CheckBoundaryRects(boundaryRects, screenProperty);
    }

    /**
     * @tc.name: CalcWaterfallRects01
     * @tc.desc: ScreenCutoutController calc waterfall rects
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, CalcWaterfallRects01, Function | SmallTest | Level3)
    {
        DisplayId displayId = 0;
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        ASSERT_TRUE(controller != nullptr);
        controller->CalcWaterfallRects(displayId);
    }

    /**
     * @tc.name: CalcWaterfallRects02
     * @tc.desc: ScreenCutoutController calc waterfall rects
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, CalcWaterfallRects02, Function | SmallTest | Level3)
    {
        DisplayId displayId = 1;
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        ASSERT_TRUE(controller != nullptr);
        controller->CalcWaterfallRects(displayId);
    }

    /**
     * @tc.name: CalcWaterfallRects03
     * @tc.desc: ScreenCutoutController calc waterfall rects
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, CalcWaterfallRects03, Function | SmallTest | Level3)
    {
        DisplayId displayId = 1;
        ScreenSceneConfig::GetCurvedScreenBoundaryConfig() = {0, 0, 0, 0};
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        ASSERT_TRUE(controller != nullptr);
        controller->CalcWaterfallRects(displayId);
    }

    /**
     * @tc.name: CalcWaterfallRects04
     * @tc.desc: ScreenCutoutController calc waterfall rects
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, CalcWaterfallRects04, Function | SmallTest | Level3)
    {
        DisplayId displayId = 1;
        ScreenSceneConfig::GetCurvedScreenBoundaryConfig() = {};
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        ASSERT_TRUE(controller != nullptr);
        controller->CalcWaterfallRects(displayId);
    }

    /**
     * @tc.name: CalcWaterfallRectsByRotation
     * @tc.desc: ScreenCutoutController calc waterfall rects by rotation
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, CalcWaterfallRectsByRotation, Function | SmallTest | Level3)
    {
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        Rotation rotation;
        uint32_t displayHeight = 1024;
        uint32_t displayWidth = 512;
        std::vector<uint32_t> realNumVec = {16, 32, 8, 8};
        rotation = Rotation::ROTATION_0;
        ASSERT_TRUE(controller != nullptr);
        controller->CalcWaterfallRectsByRotation(rotation, displayHeight, displayWidth, realNumVec);
        rotation = Rotation::ROTATION_90;
        controller->CalcWaterfallRectsByRotation(rotation, displayHeight, displayWidth, realNumVec);
        rotation = Rotation::ROTATION_180;
        controller->CalcWaterfallRectsByRotation(rotation, displayHeight, displayWidth, realNumVec);
        rotation = Rotation::ROTATION_270;
        controller->CalcWaterfallRectsByRotation(rotation, displayHeight, displayWidth, realNumVec);
    }

    /**
     * @tc.name: CalculateCurvedCompression
     * @tc.desc: ScreenCutoutController calculate curved compression
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, CalculateCurvedCompression, Function | SmallTest | Level3)
    {
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        RectF finalRect = RectF(0, 0, 0, 0);
        ScreenProperty screenProperty;
        RectF result = controller->CalculateCurvedCompression(screenProperty);
        ASSERT_EQ(finalRect.left_, result.left_);
        ASSERT_EQ(finalRect.top_, result.top_);
        ASSERT_EQ(finalRect.width_, result.width_);
        ASSERT_EQ(finalRect.height_, result.height_);
    }

    /**
     * @tc.name: IsDisplayRotationHorizontal
     * @tc.desc: IsDisplayRotationHorizontal func
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, IsDisplayRotationHorizontal, Function | SmallTest | Level3)
    {
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        ASSERT_EQ(false, controller->IsDisplayRotationHorizontal(Rotation::ROTATION_0));
        ASSERT_EQ(false, controller->IsDisplayRotationHorizontal(Rotation::ROTATION_180));
        ASSERT_EQ(true, controller->IsDisplayRotationHorizontal(Rotation::ROTATION_90));
        ASSERT_EQ(true, controller->IsDisplayRotationHorizontal(Rotation::ROTATION_270));
    }

    /**
     * @tc.name: ConvertDeviceToDisplayRotation01
     * @tc.desc: ConvertDeviceToDisplayRotation func
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, ConvertDeviceToDisplayRotation01, Function | SmallTest | Level3)
    {
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        ASSERT_EQ(Rotation::ROTATION_0, controller->ConvertDeviceToDisplayRotation(DeviceRotationValue::INVALID));
        DeviceRotationValue deviceRotation;
        deviceRotation = DeviceRotationValue::ROTATION_PORTRAIT;
        Rotation result01 = controller->ConvertDeviceToDisplayRotation(deviceRotation);
        ASSERT_EQ(result01, Rotation::ROTATION_0);
    }

    /**
     * @tc.name: GetCurrentDisplayRotation01
     * @tc.desc: GetCurrentDisplayRotation func
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, GetCurrentDisplayRotation01, Function | SmallTest | Level3)
    {
        DisplayId displayId = 0;
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        ASSERT_EQ(Rotation::ROTATION_0, controller->GetCurrentDisplayRotation(displayId));
    }

    /**
     * @tc.name: ProcessRotationMapping
     * @tc.desc: ProcessRotationMapping func
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, ProcessRotationMapping, Function | SmallTest | Level3)
    {
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        ASSERT_TRUE(controller != nullptr);
        controller->ProcessRotationMapping();
    }

    /**
     * @tc.name: GetOffsetY
     * @tc.desc: GetOffsetY func
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, GetOffsetY, Function | SmallTest | Level3)
    {
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        ASSERT_EQ(0, controller->GetOffsetY());
    }
}
} // namespace Rosen
} // namespace OHOS

