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
     * @tc.name: GetScreenCutoutInfo
     * @tc.desc: GetScreenCutoutInfo func
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, GetScreenCutoutInfo, Function | SmallTest | Level3)
    {
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        DisplayId displayId = 0;
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
        controller->ConvertBoundaryRectsByRotation(boundaryRects, displayId);
        delete controller;
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
        sptr<DisplayInfo> displayInfo = new DisplayInfo();
        displayInfo->SetWidth(35);
        displayInfo->SetHeight(35);
        controller->CheckBoundaryRects(boundaryRects, displayInfo);
        delete displayInfo;
        delete controller;
    }

    /**
     * @tc.name: CalcWaterfallRects
     * @tc.desc: ScreenCutoutController calc waterfall rects
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, CalcWaterfallRects, Function | SmallTest | Level3)
    {
        DisplayId displayId = 0;
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        controller->CalcWaterfallRects(displayId);
        delete controller;
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
        controller->CalcWaterfallRectsByRotation(rotation, displayHeight, displayWidth, realNumVec);
        rotation = Rotation::ROTATION_90;
        controller->CalcWaterfallRectsByRotation(rotation, displayHeight, displayWidth, realNumVec);
        rotation = Rotation::ROTATION_180;
        controller->CalcWaterfallRectsByRotation(rotation, displayHeight, displayWidth, realNumVec);
        rotation = Rotation::ROTATION_270;
        controller->CalcWaterfallRectsByRotation(rotation, displayHeight, displayWidth, realNumVec);
        delete controller;
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
        delete controller;
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

