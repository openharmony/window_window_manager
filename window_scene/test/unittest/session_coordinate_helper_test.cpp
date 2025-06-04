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

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class SessionCoordinateHelperTest : public Test {
public:
    SessionCoordinateHelperTest() : ssmClient_(ScreenSessionManagerClient::GetInstance()) {}

    void SetUp() override {}

    void TearDown() override {}

protected:
    ScreenSessionManagerClient& ssmClient_;
};

/**
 * @tc.name: TestRelativeToGlobalDisplayRect
 * @tc.desc: Convert relative rect to global rect using screen property
 * @tc.type: FUNC
 */
HWTEST_F(SessionCoordinateHelperTest, TestRelativeToGlobalDisplayRect, TestSize.Level1)
{
    constexpr DisplayId displayId = 1001;
    WSRect relativeRect { 50, 60, 300, 200 };

    auto screenProperty = ScreenProperty();
    screenProperty.SetX(500);
    screenProperty.SetY(600);
    auto screenSession = sptr<ScreenSession>::MakeSptr(displayId, screenProperty, displayId);
    {
        std::lock_guard<std::mutex> lock(ssmClient_.screenSessionMapMutex_);
        ssmClient_.screenSessionMap_[displayId] = screenSession;
    }

    WSRect result = SessionCoordinateHelper::RelativeToGlobalDisplayRect(displayId, relativeRect);
    WSRect expectedRect { 550, 660, 300, 200 };
    EXPECT_EQ(result, expectedRect);

    ssmClient_.screenSessionMap_.clear();
}

/**
 * @tc.name: TestGlobalToRelativeDisplayRect
 * @tc.desc: Match top-left of global rect with display area and convert correctly
 * @tc.type: FUNC
 */
HWTEST_F(SessionCoordinateHelperTest, TestGlobalToRelativeDisplayRect, TestSize.Level1)
{
    constexpr DisplayId displayA = 1;
    constexpr DisplayId displayB = 2;

    ScreenProperty propA;
    propA.SetX(0);
    propA.SetY(0);
    RRect boundsA;
    boundsA.rect_ = { 0, 0, 500, 500 };
    propA.SetBounds(boundsA);
    auto screenA = sptr<ScreenSession>::MakeSptr(displayA, propA, displayA);

    ScreenProperty propB;
    propB.SetX(600);
    propB.SetY(600);
    RRect boundsB;
    boundsB.rect_ = { 600, 600, 400, 400 };
    propB.SetBounds(boundsB);
    auto screenB = sptr<ScreenSession>::MakeSptr(displayB, propB, displayB);

    {
        std::lock_guard<std::mutex> lock(ssmClient_.screenSessionMapMutex_);
        ssmClient_.screenSessionMap_[displayA] = screenA;
        ssmClient_.screenSessionMap_[displayB] = screenB;
    }

    WSRect globalRect { 600, 600, 100, 100 };
    WSRelativeDisplayRect result = SessionCoordinateHelper::GlobalToRelativeDisplayRect(displayA, globalRect);

    EXPECT_EQ(result.displayId, displayB);

    WSRect expectedRect { 0, 0, 100, 100 }; // relative to display B
    EXPECT_EQ(result.rect, expectedRect);

    ssmClient_.screenSessionMap_.clear();
}

/**
 * @tc.name: TestToRelative
 * @tc.desc: Test ToRelative method for converting coordinates
 * @tc.type: FUNC
 */
HWTEST_F(SessionCoordinateHelperTest, TestToRelative, TestSize.Level1)
{
    WSRect globalRect { 300, 400, 200, 150 };
    WSRect screenRect { 100, 150, 800, 600 };
    WSRect result = SessionCoordinateHelper::ToRelative(globalRect, screenRect);
    WSRect expectedRect { 200, 250, 200, 150 };
    EXPECT_EQ(result, expectedRect);
}

/**
 * @tc.name: TestBuildScreenRectMap
 * @tc.desc: Test BuildScreenRectMap constructs correct mapping
 * @tc.type: FUNC
 */
HWTEST_F(SessionCoordinateHelperTest, TestBuildScreenRectMap, TestSize.Level1)
{
    constexpr DisplayId displayA = 1;
    constexpr DisplayId displayB = 2;

    ScreenProperty propA;
    propA.SetX(0);
    propA.SetY(0);
    RRect boundsA;
    boundsA.rect_ = { 0, 0, 500, 500 };
    propA.SetBounds(boundsA);
    auto screenA = sptr<ScreenSession>::MakeSptr(displayA, propA, displayA);

    ScreenProperty propB;
    propB.SetX(600);
    propB.SetY(600);
    RRect boundsB;
    boundsB.rect_ = { 600, 600, 400, 400 };
    propB.SetBounds(boundsB);
    auto screenB = sptr<ScreenSession>::MakeSptr(displayB, propB, displayB);

    {
        std::lock_guard<std::mutex> lock(ssmClient_.screenSessionMapMutex_);
        ssmClient_.screenSessionMap_[displayA] = screenA;
        ssmClient_.screenSessionMap_[displayB] = screenB;
    }

    auto screenRectMap = SessionCoordinateHelper::BuildScreenRectMap();
    ASSERT_EQ(screenRectMap.size(), 2);

    WSRect expectedRect1 { 0, 0, 500, 500 };
    EXPECT_EQ(screenRectMap[1], expectedRect1);

    WSRect expectedRect2 { 600, 600, 400, 400 };
    EXPECT_EQ(screenRectMap[2], expectedRect2);

    ssmClient_.screenSessionMap_.clear();
}

/**
 * @tc.name: TestMatchBestIntersectionScreen
 * @tc.desc: Test MatchBestIntersectionScreen returns screen with maximum intersection
 * @tc.type: FUNC
 */
HWTEST_F(SessionCoordinateHelperTest, TestMatchBestIntersectionScreen, TestSize.Level1)
{
    std::unordered_map<uint64_t, WSRect> screenRectMap {
        { 1, { 0, 0, 300, 300 } },
        { 2, { 200, 200, 400, 400 } }
    };
    WSRect globalRect { 250, 250, 100, 100 };

    auto result = SessionCoordinateHelper::MatchBestIntersectionScreen(screenRectMap, globalRect);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->displayId, 2);

    WSRect expectedRect { 50, 50, 100, 100 }; // relative to display 2
    EXPECT_EQ(result->rect, expectedRect);
}

/**
 * @tc.name: TestFallbackToOriginalOrPrimaryScreen
 * @tc.desc: Test FallbackToOriginalOrPrimaryScreen uses original if available, otherwise primary
 * @tc.type: FUNC
 */
HWTEST_F(SessionCoordinateHelperTest, TestFallbackToOriginalOrPrimaryScreen, TestSize.Level1)
{
    std::unordered_map<uint64_t, WSRect> screenRectMap {
        { 1, { 100, 100, 500, 500 } }
    };
    WSRect globalRect { 150, 150, 100, 100 };

    // Case 1: originalDisplayId exists
    {
        auto result = SessionCoordinateHelper::FallbackToOriginalOrPrimaryScreen(screenRectMap, globalRect, 1);
        EXPECT_EQ(result.displayId, 1);
        WSRect expectedRect { 50, 50, 100, 100 };
        EXPECT_EQ(result.rect, expectedRect);
    }

    // Case 2: originalDisplayId does not exist
    {
        auto result = SessionCoordinateHelper::FallbackToOriginalOrPrimaryScreen(screenRectMap, globalRect, 999);
        EXPECT_EQ(result.displayId, MAIN_SCREEN_ID_DEFAULT);
        EXPECT_EQ(result.rect, globalRect); // fallback keeps global rect
    }
}
} // namespace Rosen
} // namespace OHOS