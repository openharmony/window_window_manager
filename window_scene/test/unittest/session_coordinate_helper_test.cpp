/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "session/host/include/session_coordinate_helper.h"

#include "screen_session_manager_client/include/screen_session_manager_client.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class SessionCoordinateHelperTest : public Test {
public:
    SessionCoordinateHelperTest() : ssmClient_(ScreenSessionManagerClient::GetInstance()) {}

    void SetUp() override {}

    void TearDown() override
    {
        std::lock_guard<std::mutex> lock(ssmClient_.screenSessionMapMutex_);
        ssmClient_.screenSessionMap_.clear();
    }

protected:
    ScreenSessionManagerClient& ssmClient_;
};

/**
 * @tc.name: TestRelativeToGlobalDisplayRect
 * @tc.desc: Verify RelativeToGlobalDisplayRect correctly converts relative rect to global rect
 * @tc.type: FUNC
 */
HWTEST_F(SessionCoordinateHelperTest, TestRelativeToGlobalDisplayRect, TestSize.Level1)
{
    constexpr ScreenId invalidScreenId = 9999;
    WSRect relativeRect { 50, 60, 300, 200 };
    WSRect result = SessionCoordinateHelper::RelativeToGlobalDisplayRect(invalidScreenId, relativeRect);
    EXPECT_EQ(result, relativeRect);

    constexpr ScreenId screenId = 1001;
    auto screenProperty = ScreenProperty();
    screenProperty.SetX(500);
    screenProperty.SetY(600);
    auto screenSession = sptr<ScreenSession>::MakeSptr(screenId, screenProperty, screenId);
    {
        std::lock_guard<std::mutex> lock(ssmClient_.screenSessionMapMutex_);
        ssmClient_.screenSessionMap_[screenId] = screenSession;
    }
    result = SessionCoordinateHelper::RelativeToGlobalDisplayRect(screenId, relativeRect);
    WSRect expectedRect { 550, 660, 300, 200 };
    EXPECT_EQ(result, expectedRect);
}

/**
 * @tc.name: TestGlobalToScreenRelativeRect
 * @tc.desc: Verify GlobalToScreenRelativeRect correctly converts global rect to relative rect
 * @tc.type: FUNC
 */
HWTEST_F(SessionCoordinateHelperTest, TestGlobalToScreenRelativeRect, TestSize.Level1)
{
    constexpr ScreenId invalidScreenId = 9999;
    WSRect globalRect { 600, 600, 100, 100 };
    auto result = SessionCoordinateHelper::GlobalToScreenRelativeRect(invalidScreenId, globalRect);
    EXPECT_EQ(result.screenId, MAIN_SCREEN_ID_DEFAULT);
    EXPECT_EQ(result.rect, globalRect);

    ScreenProperty propA;
    propA.SetX(0);
    propA.SetY(0);
    RRect boundsA;
    boundsA.rect_ = { 0, 0, 500, 500 };
    propA.SetBounds(boundsA);
    constexpr ScreenId screenIdOfA = 1;
    auto screenA = sptr<ScreenSession>::MakeSptr(screenIdOfA, propA, screenIdOfA);

    ScreenProperty propB;
    propB.SetX(600);
    propB.SetY(600);
    RRect boundsB;
    boundsB.rect_ = { 600, 600, 400, 400 };
    propB.SetBounds(boundsB);
    constexpr ScreenId screenIdOfB = 2;
    auto screenB = sptr<ScreenSession>::MakeSptr(screenIdOfB, propB, screenIdOfB);
    {
        std::lock_guard<std::mutex> lock(ssmClient_.screenSessionMapMutex_);
        ssmClient_.screenSessionMap_[screenIdOfA] = screenA;
        ssmClient_.screenSessionMap_[screenIdOfB] = screenB;
    }

    screenA->property_.SetVirtualPixelRatio(0.0f);
    result = SessionCoordinateHelper::GlobalToScreenRelativeRect(screenIdOfA, globalRect);
    EXPECT_EQ(result.screenId, MAIN_SCREEN_ID_DEFAULT);
    EXPECT_EQ(result.rect, globalRect);

    screenA->property_.SetVirtualPixelRatio(1.0f);
    screenB->property_.SetVirtualPixelRatio(1.0f);
    result = SessionCoordinateHelper::GlobalToScreenRelativeRect(screenIdOfA, globalRect);
    EXPECT_EQ(result.screenId, screenIdOfB);
    WSRect expectedRect { 0, 0, 100, 100 }; // relative to screen B
    EXPECT_EQ(result.rect, expectedRect);
}
} // namespace Rosen
} // namespace OHOS