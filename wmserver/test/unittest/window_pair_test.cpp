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
#include "window_pair.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowPairTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WindowPairTest::SetUpTestCase()
{
}

void WindowPairTest::TearDownTestCase()
{
}

void WindowPairTest::SetUp()
{
}

void WindowPairTest::TearDown()
{
}

namespace {
/**
 * @tc.name: IsPaired
 * @tc.desc: Get whether the window pair is paired
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsPaired01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    windowPair->primary_ = nullptr;
    ASSERT_EQ(false, windowPair->IsPaired());

    windowPair->secondary_ = nullptr;
    ASSERT_EQ(false, windowPair->IsPaired());
}

/**
 * @tc.name: IsPaired
 * @tc.desc: Get whether the window pair is paired
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsPaired02, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowProperty> property2 = new WindowProperty();
    property2->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);

    sptr<WindowNode> node1 = new WindowNode(property1);
    windowPair->primary_ = node1;
    sptr<WindowNode> node2 = new WindowNode(property2);
    windowPair->secondary_ = node2;
    windowPair->divider_ = node1;
    ASSERT_EQ(true, windowPair->IsPaired());
}

/**
 * @tc.name: IsPaired
 * @tc.desc: Get whether the window pair is paired
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsPaired03, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowProperty> property2 = new WindowProperty();
    property2->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);

    sptr<WindowNode> node1 = new WindowNode(property1);
    windowPair->primary_ = node1;
    sptr<WindowNode> node2 = new WindowNode(property2);
    windowPair->secondary_ = node2;
    windowPair->divider_ = nullptr;
    ASSERT_EQ(false, windowPair->IsPaired());
}

/**
 * @tc.name: Find
 * @tc.desc: Find window node from window pair
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, Find01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowNode> node1 = nullptr;
    ASSERT_EQ(nullptr, windowPair->Find(node1));
}

/**
 * @tc.name: Find
 * @tc.desc: Find window node from window pair
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, Find02, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowId(1);
    sptr<WindowNode> node1 = new WindowNode(property1);
    windowPair->primary_ = node1;
    ASSERT_EQ(node1, windowPair->Find(node1));
}

/**
 * @tc.name: Find
 * @tc.desc: Find window node from window pair
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, Find03, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowId(1);
    sptr<WindowNode> node1 = new WindowNode(property1);
    windowPair->primary_ = nullptr;
    windowPair->secondary_ = node1;
    ASSERT_EQ(node1, windowPair->Find(node1));
}

/**
 * @tc.name: Find
 * @tc.desc: Find window node from window pair
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, Find04, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowId(1);
    sptr<WindowNode> node1 = new WindowNode(property1);
    windowPair->primary_ = nullptr;
    windowPair->secondary_ = nullptr;
    windowPair->divider_ = node1;
    ASSERT_EQ(node1, windowPair->Find(node1));
}

/**
 * @tc.name: GetSplitRatio
 * @tc.desc: Get split ratio
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, GetSplitRatio01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    windowPair->ratio_ = 0;
    ASSERT_EQ(0, windowPair->GetSplitRatio());
    windowPair->ratio_ = 5;
    ASSERT_EQ(5, windowPair->GetSplitRatio());
}

/**
 * @tc.name: IsForbidDockSliceMove
 * @tc.desc: Get whether dock slice is forbidden to move
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsForbidDockSliceMove01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    windowPair->status_ = WindowPairStatus::STATUS_PAIRED_DONE;
    ASSERT_EQ(true, windowPair->IsForbidDockSliceMove());
    windowPair->status_ = WindowPairStatus::STATUS_EMPTY;
    ASSERT_EQ(false, windowPair->IsForbidDockSliceMove());
}

/**
 * @tc.name: IsForbidDockSliceMove
 * @tc.desc: Get whether dock slice is forbidden to move
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsForbidDockSliceMove02, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    windowPair->status_ = WindowPairStatus::STATUS_PAIRED_DONE;
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
    windowPair->primary_ = new WindowNode(property1);
    windowPair->secondary_ = new WindowNode(property1);
    ASSERT_EQ(false, windowPair->IsForbidDockSliceMove());
}

/**
 * @tc.name: IsForbidDockSliceMove
 * @tc.desc: Get whether dock slice is forbidden to move
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsForbidDockSliceMove03, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    windowPair->status_ = WindowPairStatus::STATUS_PAIRED_DONE;
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
    windowPair->primary_ = nullptr;
    windowPair->secondary_ = new WindowNode(property1);
    ASSERT_EQ(true, windowPair->IsForbidDockSliceMove());
}

/**
 * @tc.name: IsForbidDockSliceMove
 * @tc.desc: Get whether dock slice is forbidden to move
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsForbidDockSliceMove04, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    windowPair->status_ = WindowPairStatus::STATUS_PAIRED_DONE;
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_FORBID_SPLIT_MOVE));
    windowPair->primary_ = new WindowNode(property1);
    windowPair->secondary_ = new WindowNode(property1);
    ASSERT_EQ(true, windowPair->IsForbidDockSliceMove());
}

/**
 * @tc.name: IsDockSliceInExitSplitModeArea
 * @tc.desc: whether dock slice in exit split screen mode area
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsDockSliceInExitSplitModeArea01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    std::vector<int32_t> points {0, 0};
    windowPair->primary_ = nullptr;
    ASSERT_EQ(false, windowPair->IsDockSliceInExitSplitModeArea(points));
}

/**
 * @tc.name: IsDockSliceInExitSplitModeArea
 * @tc.desc: whether dock slice in exit split screen mode area
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsDockSliceInExitSplitModeArea02, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    std::vector<int32_t> points {0, 0};
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowProperty> property2 = new WindowProperty();
    property2->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    sptr<WindowProperty> property3 = new WindowProperty();

    sptr<WindowNode> node1 = new WindowNode(property1);
    windowPair->primary_ = node1;
    sptr<WindowNode> node2 = new WindowNode(property2);
    windowPair->secondary_ = node2;
    Rect rect1 = {1, 1, 10, 20};
    Rect rect2 = {1, 1, 20, 10};
    property2->SetWindowRect(rect1);
    property3->SetWindowRect(rect2);
    windowPair->divider_ = new WindowNode(property2);

    ASSERT_EQ(true, windowPair->IsPaired());
    ASSERT_EQ(true, windowPair->IsDockSliceInExitSplitModeArea(points));
    windowPair->divider_ = new WindowNode(property3);
    ASSERT_EQ(true, windowPair->IsDockSliceInExitSplitModeArea(points));
}

/**
 * @tc.name: IsDockSliceInExitSplitModeArea
 * @tc.desc: whether dock slice in exit split screen mode area
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsDockSliceInExitSplitModeArea03, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    std::vector<int32_t> points {0, 50};
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowProperty> property2 = new WindowProperty();
    property2->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);

    sptr<WindowNode> node1 = new WindowNode(property1);
    windowPair->primary_ = node1;
    sptr<WindowNode> node2 = new WindowNode(property2);
    windowPair->secondary_ = node2;
    Rect rect1 = {1, 1, 10, 20};
    property2->SetWindowRect(rect1);
    windowPair->divider_ = new WindowNode(property2);

    ASSERT_EQ(true, windowPair->IsPaired());
    ASSERT_EQ(false, windowPair->IsDockSliceInExitSplitModeArea(points));
}

/**
 * @tc.name: IsSplitRelated
 * @tc.desc: Gets whether the window is related to split window.
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsSplitRelated01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowNode> node = nullptr;
    ASSERT_EQ(false, windowPair->IsSplitRelated(node));
}

/**
 * @tc.name: IsSplitRelated
 * @tc.desc: Gets whether the window is related to split window.
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsSplitRelated02, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowNode> node1 = new WindowNode(property1);
    ASSERT_EQ(true, windowPair->IsSplitRelated(node1));
    property1->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    property1->SetWindowType(WindowType::WINDOW_TYPE_DOCK_SLICE);
    sptr<WindowNode> node2 = new WindowNode(property1);
    ASSERT_EQ(true, windowPair->IsSplitRelated(node2));
    property1->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    property1->SetWindowType(WindowType::APP_WINDOW_BASE);
    sptr<WindowNode> node3 = new WindowNode(property1);
    ASSERT_EQ(false, windowPair->IsSplitRelated(node3));
}
}
}
}
