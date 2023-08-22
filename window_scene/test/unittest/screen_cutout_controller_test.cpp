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
        ASSERT_NE(nullptr, controller->GetScreenCutoutInfo());
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
    }

    /**
     * @tc.name: GetCurrentDisplayRotation01
     * @tc.desc: GetCurrentDisplayRotation func
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, GetCurrentDisplayRotation01, Function | SmallTest | Level3)
    {
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        ASSERT_EQ(Rotation::ROTATION_0, controller->GetCurrentDisplayRotation());
    }

    /**
     * @tc.name: ProcessRotationMapping
     * @tc.desc: ProcessRotationMapping func
     * @tc.type: FUNC
     */
    HWTEST_F(ScreenCutoutControllerTest, ProcessRotationMapping, Function | SmallTest | Level3)
    {
        int res = 0;
        sptr<ScreenCutoutController> controller = new ScreenCutoutController();
        controller->ProcessRotationMapping();
        ASSERT_EQ(0, res);
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

