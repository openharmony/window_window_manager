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
#include "minimize_app.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class MinimizeAppTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    sptr<WindowProperty> CreateWindowProperty(uint32_t windowId);
};

void MinimizeAppTest::SetUpTestCase()
{
}

void MinimizeAppTest::TearDownTestCase()
{
}

void MinimizeAppTest::SetUp()
{
}

void MinimizeAppTest::TearDown()
{
}

sptr<WindowProperty> MinimizeAppTest::CreateWindowProperty(uint32_t windowId)
{
    sptr<WindowProperty> property = new WindowProperty();
    property->SetWindowId(windowId);
    return property;
}
namespace {
/**
 * @tc.name: MinimizeAppTest01
 * @tc.desc: add and excute
 * @tc.type: FUNC
 */
HWTEST_F(MinimizeAppTest, MinimizeAppTest01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(1));
    sptr<WindowNode> node2 = new WindowNode(CreateWindowProperty(2));
    sptr<WindowNode> node3 = new WindowNode(CreateWindowProperty(3));
    sptr<WindowNode> node4 = new WindowNode(CreateWindowProperty(4));
    sptr<WindowNode> node5 = new WindowNode(CreateWindowProperty(5));
    sptr<WindowNode> node6 = new WindowNode(CreateWindowProperty(6));
    sptr<WindowNode> node7 = new WindowNode(CreateWindowProperty(7));
    sptr<WindowNode> node8 = new WindowNode(CreateWindowProperty(8));
    sptr<WindowNode> node9 = new WindowNode(CreateWindowProperty(9));

    sptr<WindowNode> node10 = new WindowNode();
    sptr<WindowNode> node11 = nullptr;
    sptr<WindowNode> conflictNode = new WindowNode(CreateWindowProperty(2));

    MinimizeApp::AddNeedMinimizeApp(node1, MinimizeReason::MINIMIZE_BUTTON);
    MinimizeApp::AddNeedMinimizeApp(node2, MinimizeReason::MINIMIZE_ALL);
    MinimizeApp::AddNeedMinimizeApp(node3, MinimizeReason::LAYOUT_TILE);
    MinimizeApp::AddNeedMinimizeApp(node4, MinimizeReason::LAYOUT_CASCADE);
    MinimizeApp::AddNeedMinimizeApp(node5, MinimizeReason::MAX_APP_COUNT);
    MinimizeApp::AddNeedMinimizeApp(node6, MinimizeReason::SPLIT_REPLACE);
    MinimizeApp::AddNeedMinimizeApp(node7, MinimizeReason::SPLIT_QUIT);
    MinimizeApp::AddNeedMinimizeApp(node8, MinimizeReason::GESTURE_ANIMATION);
    MinimizeApp::AddNeedMinimizeApp(node9, MinimizeReason::OTHER_WINDOW);
    MinimizeApp::AddNeedMinimizeApp(conflictNode, MinimizeReason::MINIMIZE_ALL);

    ASSERT_EQ(true, MinimizeApp::IsNodeNeedMinimize(node1));
    ASSERT_EQ(true, MinimizeApp::IsNodeNeedMinimize(node2));
    ASSERT_EQ(true, MinimizeApp::IsNodeNeedMinimize(node3));
    ASSERT_EQ(true, MinimizeApp::IsNodeNeedMinimize(node4));
    ASSERT_EQ(true, MinimizeApp::IsNodeNeedMinimize(node5));
    ASSERT_EQ(true, MinimizeApp::IsNodeNeedMinimize(node6));
    ASSERT_EQ(true, MinimizeApp::IsNodeNeedMinimize(node7));
    ASSERT_EQ(true, MinimizeApp::IsNodeNeedMinimize(node8));
    ASSERT_EQ(true, MinimizeApp::IsNodeNeedMinimize(node9));

    ASSERT_EQ(false, MinimizeApp::IsNodeNeedMinimize(node10));
    ASSERT_EQ(false, MinimizeApp::IsNodeNeedMinimize(node11));
    ASSERT_EQ(false, MinimizeApp::IsNodeNeedMinimize(conflictNode));

    MinimizeApp::ExecuteMinimizeTargetReason(MinimizeReason::SPLIT_REPLACE);
    MinimizeApp::ExecuteMinimizeAll();
}
/**
 * @tc.name: MinimizeAppTest02
 * @tc.desc: add, find and clear
 * @tc.type: FUNC
 */
HWTEST_F(MinimizeAppTest, MinimizeAppTest02, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode();

    MinimizeApp::AddNeedMinimizeApp(node1, MinimizeReason::MINIMIZE_ALL);

    auto getNodes = MinimizeApp::GetNeedMinimizeAppNodesWithReason(MinimizeReason::MINIMIZE_ALL);
    ASSERT_EQ(node1, getNodes[0]);

    MinimizeApp::ClearNodesWithReason(MinimizeReason::MINIMIZE_ALL);
}
/**
 * @tc.name: MinimizeAppTest03
 * @tc.desc: add and find
 * @tc.type: FUNC
 */
HWTEST_F(MinimizeAppTest, MinimizeAppTest03, Function | SmallTest | Level2)
{
    MinimizeApp::SetMinimizedByOtherConfig(false);
    sptr<WindowNode> node1 = new WindowNode();

    MinimizeApp::AddNeedMinimizeApp(node1, MinimizeReason::OTHER_WINDOW);
    ASSERT_EQ(false, MinimizeApp::IsNodeNeedMinimize(node1));
    MinimizeApp::SetMinimizedByOtherConfig(true);

    MinimizeApp::ClearNodesWithReason(MinimizeReason::OTHER_WINDOW);
}
/**
 * @tc.name: MinimizeAppTest04
 * @tc.desc: add and recover
 * @tc.type: FUNC
 */
HWTEST_F(MinimizeAppTest, MinimizeAppTest04, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode();
    MinimizeApp::AddNeedMinimizeApp(node1, MinimizeReason::LAYOUT_TILE);

    ASSERT_EQ(node1, MinimizeApp::GetRecoverdNodeFromMinimizeList());
    ASSERT_EQ(nullptr, MinimizeApp::GetRecoverdNodeFromMinimizeList());

    MinimizeApp::ClearNodesWithReason(MinimizeReason::LAYOUT_TILE);
}
}
}
}
