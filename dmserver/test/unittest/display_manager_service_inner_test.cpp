/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "display_manager_service_inner.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_US = 100000;
}
class DisplayManagerServiceInnerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DisplayManagerServiceInnerTest::SetUpTestCase()
{
}

void DisplayManagerServiceInnerTest::TearDownTestCase()
{
}

void DisplayManagerServiceInnerTest::SetUp()
{
}

void DisplayManagerServiceInnerTest::TearDown()
{
    usleep(SLEEP_TIME_US);
}

namespace {
/**
 * @tc.name: RegisterWindowInfoQueriedListener
 * @tc.desc: test function : RegisterWindowInfoQueriedListener
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceInnerTest, RegisterWindowInfoQueriedListener, Function | SmallTest | Level1)
{
    DisplayManagerServiceInner inner;
    sptr<IWindowInfoQueriedListener> listener = nullptr;
    inner.RegisterWindowInfoQueriedListener(listener);
    EXPECT_EQ(nullptr, listener);
}

/**
 * @tc.name: GetAllDisplays
 * @tc.desc: test function : GetAllDisplays
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceInnerTest, GetAllDisplays, Function | SmallTest | Level1)
{
    DisplayManagerServiceInner inner;
    auto ret = inner.GetAllDisplays();
    EXPECT_EQ(true, ret.empty());
}

/**
 * @tc.name: UpdateRSTree
 * @tc.desc: test function : UpdateRSTree
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceInnerTest, UpdateRSTree, Function | SmallTest | Level1)
{
    DisplayId displayId = 0;
    DisplayId parentDisplayId = 0;
    std::shared_ptr<RSSurfaceNode> surfaceNode = nullptr;
    bool isAdd = false;
    bool isMultiDisplay = false;

    DisplayManagerServiceInner inner;
    inner.UpdateRSTree(displayId, parentDisplayId, surfaceNode, isAdd, isMultiDisplay);
    EXPECT_EQ(nullptr, surfaceNode);
}

/**
 * @tc.name: GetScreenGroupIdByDisplayId
 * @tc.desc: test function : GetScreenGroupIdByDisplayId
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceInnerTest, GetScreenGroupIdByDisplayId, Function | SmallTest | Level1)
{
    DisplayId displayId = 0;
    DisplayManagerServiceInner inner;
    auto ret = inner.GetScreenGroupIdByDisplayId(displayId);
    EXPECT_EQ(INVALID_SCREEN_ID, ret);

    displayId = 1;
    ret = inner.GetScreenGroupIdByDisplayId(displayId);
    EXPECT_EQ(INVALID_SCREEN_ID, ret);
}

/**
 * @tc.name: GetDisplaySnapshot
 * @tc.desc: test function : GetDisplaySnapshot
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceInnerTest, GetDisplaySnapshot, Function | SmallTest | Level1)
{
    DisplayId displayId = 0;
    DmErrorCode* errorCode = nullptr;
    DisplayManagerServiceInner inner;
    auto ret = inner.GetDisplaySnapshot(displayId, errorCode);
    EXPECT_EQ(nullptr, ret);
}

/**
 * @tc.name: RegisterDisplayChangeListener
 * @tc.desc: test function : RegisterDisplayChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceInnerTest, RegisterDisplayChangeListener, Function | SmallTest | Level1)
{
    sptr<IDisplayChangeListener> listener = nullptr;
    DisplayManagerServiceInner inner;
    inner.RegisterDisplayChangeListener(listener);
    EXPECT_EQ(nullptr, listener);
}

/**
 * @tc.name: SetOrientationFromWindow
 * @tc.desc: test function : SetOrientationFromWindow
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceInnerTest, SetOrientationFromWindow, Function | SmallTest | Level1)
{
    DisplayId displayId = 0;
    Orientation orientation = Orientation::BEGIN;
    bool withAnimation = false;
    DisplayManagerServiceInner inner;
    auto ret = inner.SetOrientationFromWindow(displayId, orientation, withAnimation);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, ret);

    displayId = 1;
    ret = inner.SetOrientationFromWindow(displayId, orientation, withAnimation);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: SetRotationFromWindow
 * @tc.desc: test function : SetRotationFromWindow
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceInnerTest, SetRotationFromWindow, Function | SmallTest | Level1)
{
    DisplayId displayId = 0;
    Rotation targetRotation = Rotation::ROTATION_0;
    bool withAnimation = false;
    DisplayManagerServiceInner inner;
    auto ret = inner.SetRotationFromWindow(displayId, targetRotation, withAnimation);
    EXPECT_EQ(false, ret);

    displayId = 1;
    ret = inner.SetRotationFromWindow(displayId, targetRotation, withAnimation);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.name: SetGravitySensorSubscriptionEnabled
 * @tc.desc: test function : RegisterDisplayChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceInnerTest, SetGravitySensorSubscriptionEnabled, Function | SmallTest | Level1)
{
    DisplayManagerServiceInner inner;
    inner.SetGravitySensorSubscriptionEnabled();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetCutoutInfo
 * @tc.desc: test function : GetCutoutInfo
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceInnerTest, GetCutoutInfo, Function | SmallTest | Level1)
{
    DisplayId displayId = 0;
    DisplayManagerServiceInner inner;
    auto ret = inner.GetCutoutInfo(displayId);
    EXPECT_NE(nullptr, ret);
}

/**
 * @tc.name: NotifyPrivateWindowStateChanged
 * @tc.desc: test function : NotifyPrivateWindowStateChanged
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceInnerTest, NotifyPrivateWindowStateChanged, Function | SmallTest | Level1)
{
    bool hasPrivate = false;
    DisplayManagerServiceInner inner;
    inner.NotifyPrivateWindowStateChanged(hasPrivate);
    EXPECT_EQ(false, hasPrivate);
}
}
}
}