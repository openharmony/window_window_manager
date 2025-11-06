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

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override {}
    void TearDown() override
    {
        std::lock_guard<std::mutex> lock(ssmClient_.screenSessionMapMutex_);
        ssmClient_.screenSessionMap_.clear();
    }

protected:
    ScreenSessionManagerClient& ssmClient_;
    static sptr<ScreenSession> CreateScreen(
        ScreenId id, int32_t x, int32_t y, RectT<float> rect, float vpr = 1.0f);
};

void SessionCoordinateHelperTest::SetUpTestCase() {}

void SessionCoordinateHelperTest::TearDownTestCase()
{
}

sptr<ScreenSession> SessionCoordinateHelperTest::CreateScreen(
    ScreenId id, int32_t x, int32_t y, RectT<float> rect, float vpr)
{
    auto property = ScreenProperty();
    property.SetX(x);
    property.SetY(y);
    RRect bounds;
    bounds.rect_ = rect;
    property.SetBounds(bounds);
    property.SetVirtualPixelRatio(vpr);
    return sptr<ScreenSession>::MakeSptr(id, property, id);
}

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
    auto screenSession = CreateScreen(screenId, 500, 600, { 500.f, 600.f, 1000.f, 1000.f });
    {
        std::lock_guard<std::mutex> lock(ssmClient_.screenSessionMapMutex_);
        ssmClient_.screenSessionMap_[screenId] = screenSession;
    }
    result = SessionCoordinateHelper::RelativeToGlobalDisplayRect(screenId, relativeRect);
    WSRect expectedRect { 550, 660, 300, 200 };
    EXPECT_EQ(result, expectedRect);
}

/**
 * @tc.name: TestGlobalToScreenRelativeRectEarlyReturns
 * @tc.desc: Verify GlobalToScreenRelativeRect returns early for invalid screenId or VPR=0
 * @tc.type: FUNC
 */
HWTEST_F(SessionCoordinateHelperTest, TestGlobalToScreenRelativeRectEarlyReturns, TestSize.Level1)
{
    constexpr ScreenId screenId = 1;
    auto screen = CreateScreen(screenId, 0, 0, {0.f, 0.f, 500.f, 500.f});
    {
        std::lock_guard<std::mutex> lock(ssmClient_.screenSessionMapMutex_);
        ssmClient_.screenSessionMap_[screenId] = screen;
    }

    // Case 1: originalScreenId is invalid
    constexpr ScreenId invalidScreenId = 9999;
    WSRect globalRect { 0, 0, 100, 100 };
    auto result = SessionCoordinateHelper::GlobalToScreenRelativeRect(invalidScreenId, globalRect);
    EXPECT_EQ(result.screenId, MAIN_SCREEN_ID_DEFAULT);
    EXPECT_EQ(result.rect, globalRect);

    // Case 2: originalScreen has VPR of 0
    screen->property_.SetVirtualPixelRatio(0.0f);
    result = SessionCoordinateHelper::GlobalToScreenRelativeRect(screenId, globalRect);
    EXPECT_EQ(result.screenId, MAIN_SCREEN_ID_DEFAULT);
    EXPECT_EQ(result.rect, globalRect);
}

/**
 * @tc.name: TestGlobalToScreenRelativeRectNormalCases
 * @tc.desc: Verify GlobalToScreenRelativeRect correctly converts global rect to relative rect
 * @tc.type: FUNC
 */
HWTEST_F(SessionCoordinateHelperTest, TestGlobalToScreenRelativeRectNormalCases, TestSize.Level1)
{
    constexpr ScreenId screenIdOfA = 1;
    auto screenA = CreateScreen(screenIdOfA, 0, 0, {0.f, 0.f, 500.f, 500.f});

    constexpr ScreenId screenIdOfB = 2;
    auto screenB = CreateScreen(screenIdOfB, 500, 0, {500.f, 0.f, 500.f, 500.f});

    {
        std::lock_guard<std::mutex> lock(ssmClient_.screenSessionMapMutex_);
        ssmClient_.screenSessionMap_[screenIdOfA] = screenA;
        ssmClient_.screenSessionMap_[screenIdOfB] = screenB;
    }

    // Case 1: globalRect intersects with screen A only
    WSRect globalRect { 0, 0, 400, 400 };
    auto result = SessionCoordinateHelper::GlobalToScreenRelativeRect(screenIdOfB, globalRect);
    EXPECT_EQ(result.screenId, screenIdOfA);
    EXPECT_EQ(result.rect, globalRect);

    // Case 2: globalRect intersects with screen B only
    globalRect = { 500, 100, 300, 300 };
    result = SessionCoordinateHelper::GlobalToScreenRelativeRect(screenIdOfA, globalRect);
    EXPECT_EQ(result.screenId, screenIdOfB);
    WSRect expectedRect { 0, 100, 300, 300 };
    EXPECT_EQ(result.rect, expectedRect);

    // Case 3: globalRect intersects with both screen A and B, but more with A
    globalRect = { 300, 100, 300, 100 };
    result = SessionCoordinateHelper::GlobalToScreenRelativeRect(screenIdOfA, globalRect);
    EXPECT_EQ(result.screenId, screenIdOfA);
    EXPECT_EQ(result.rect, globalRect);

    // Case 4: globalRect intersects with both screen A and B, but more with B
    globalRect = { 400, 100, 300, 300 };
    result = SessionCoordinateHelper::GlobalToScreenRelativeRect(screenIdOfA, globalRect);
    EXPECT_EQ(result.screenId, screenIdOfB);
    expectedRect = { -100, 100, 300, 300 };
    EXPECT_EQ(result.rect, expectedRect);

    // Case 5: globalRect intersects with both screen A and B equally, should pick the one with smaller screenId
    globalRect = { 400, 100, 200, 300 };
    result = SessionCoordinateHelper::GlobalToScreenRelativeRect(screenIdOfB, globalRect);
    EXPECT_EQ(result.screenId, screenIdOfA);
    EXPECT_EQ(result.rect, globalRect);

    // Case 6: globalRect does not intersect with any screen, should fallback to originalScreen
    globalRect = { 2000, 2000, 100, 100 };
    result = SessionCoordinateHelper::GlobalToScreenRelativeRect(screenIdOfA, globalRect);
    EXPECT_EQ(result.screenId, screenIdOfA);
    EXPECT_EQ(result.rect, globalRect);
    result = SessionCoordinateHelper::GlobalToScreenRelativeRect(screenIdOfB, globalRect);
    EXPECT_EQ(result.screenId, screenIdOfB);
    expectedRect = { 1500, 2000, 100, 100 };
    EXPECT_EQ(result.rect, expectedRect);
}
} // namespace Rosen
} // namespace OHOS