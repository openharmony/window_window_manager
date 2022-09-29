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
#include "display_group_controller.h"
#include "display_group_info.h"
#include "display_manager.h"
#include "window_layout_policy.h"
#include "window_layout_policy_cascade.h"
#include "window_node_container.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowLayoutPolicyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    sptr<WindowNode> CreateDragNode(DragType dragType, const Rect& winRect);

    static sptr<WindowNodeContainer> container_;
    static sptr<DisplayGroupInfo> displayGroupInfo_;
    static sptr<DisplayGroupController> displayGroupController_;
    static sptr<WindowLayoutPolicy> policy_;
};

sptr<WindowNodeContainer> WindowLayoutPolicyTest::container_ = nullptr;
sptr<DisplayGroupInfo> WindowLayoutPolicyTest::displayGroupInfo_ = nullptr;
sptr<DisplayGroupController> WindowLayoutPolicyTest::displayGroupController_ = nullptr;
sptr<WindowLayoutPolicy> WindowLayoutPolicyTest::policy_ = nullptr;

void WindowLayoutPolicyTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));

    container_ = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    displayGroupInfo_ = new DisplayGroupInfo(0, display->GetDisplayInfo());
    displayGroupController_ = new DisplayGroupController(container_, displayGroupInfo_);
    policy_ = new WindowLayoutPolicyCascade(displayGroupInfo_, displayGroupController_->displayGroupWindowTree_);
}

void WindowLayoutPolicyTest::TearDownTestCase()
{
    container_ = nullptr;
    displayGroupInfo_ = nullptr;
    displayGroupController_ = nullptr;
    policy_ = nullptr;
}

void WindowLayoutPolicyTest::SetUp()
{
}

void WindowLayoutPolicyTest::TearDown()
{
}

sptr<WindowNode> WindowLayoutPolicyTest::CreateDragNode(DragType dragType, const Rect& winRect)
{
    sptr<WindowProperty> property = new WindowProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetWindowRect(winRect);
    property->SetOriginRect(winRect);
    sptr<WindowNode> node = new WindowNode(property, nullptr, nullptr);
    node->SetWindowSizeChangeReason(WindowSizeChangeReason::DRAG);
    node->SetDragType(dragType);
    return node;
}
namespace {
/**
 * @tc.name: CalcEntireWindowHotZone
 * @tc.desc: calc entire window hot zone
 * @tc.type: FUNC
 * @tc.require issueI5LYDC
 */
HWTEST_F(WindowLayoutPolicyTest, CalcEntireWindowHotZone, Function | SmallTest | Level2)
{
    Rect winRect = {50, 100, 400, 500};
    TransformHelper::Vector2 hotZoneScale = {1.f, 1.f};
    sptr<WindowProperty> property = new WindowProperty();

    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sptr<WindowNode> node0 = new WindowNode(property, nullptr, nullptr);
    auto actRect0 = policy_->CalcEntireWindowHotZone(node0, winRect, 10, 2.f, hotZoneScale);
    Rect expRect0 = {30, 80, 440, 540};
    ASSERT_EQ(expRect0, actRect0);

    property->SetWindowType(WindowType::WINDOW_TYPE_DOCK_SLICE);
    sptr<WindowNode> node1 = new WindowNode(property, nullptr, nullptr);
    auto actRect1 = policy_->CalcEntireWindowHotZone(node1, winRect, 10, 2.f, hotZoneScale);
    Rect expRect1 = {30, 100, 440, 500};
    ASSERT_EQ(expRect1, actRect1);

    property->SetWindowType(WindowType::WINDOW_TYPE_LAUNCHER_RECENT);
    sptr<WindowNode> node2 = new WindowNode(property, nullptr, nullptr);
    auto actRect2 = policy_->CalcEntireWindowHotZone(node2, winRect, 10, 2.f, hotZoneScale);
    Rect expRect2 = displayGroupInfo_->GetDisplayRect(0);;
    ASSERT_EQ(expRect2, actRect2);
}

/**
 * @tc.name: UpdateFloatingWindowSizeForStretchableWindow01
 * @tc.desc: UpdateFloatingWindowSizeForStretchableWindow test for drag width
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateFloatingWindowSizeForStretchableWindow01, Function | SmallTest | Level2)
{
    Rect winRect = {50, 50, 100, 150};
    Rect newWinRect = { 50, 50, 200, 200 };
    sptr<WindowNode> node = CreateDragNode(DragType::DRAG_LEFT_OR_RIGHT, winRect);
    policy_->UpdateFloatingWindowSizeForStretchableWindow(node, { 0, 0, 0, 0 }, newWinRect);
    Rect expRect = { 50, 50, 200, 300 };
    ASSERT_EQ(expRect, newWinRect);
}

/**
 * @tc.name: UpdateFloatingWindowSizeForStretchableWindow02
 * @tc.desc: UpdateFloatingWindowSizeForStretchableWindow test for drag coner
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateFloatingWindowSizeForStretchableWindow02, Function | SmallTest | Level2)
{
    Rect winRect = {50, 50, 100, 150};
    Rect newWinRect = { 50, 50, 200, 200 };
    sptr<WindowNode> node = CreateDragNode(DragType::DRAG_CORNER, winRect);
    policy_->UpdateFloatingWindowSizeForStretchableWindow(node, { 0, 0, 0, 0 }, newWinRect);
    Rect expRect = { 50, 50, 200, 300 };
    ASSERT_EQ(expRect, newWinRect);
}

/**
 * @tc.name: UpdateFloatingWindowSizeForStretchableWindow03
 * @tc.desc: UpdateFloatingWindowSizeForStretchableWindow test for drag height
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateFloatingWindowSizeForStretchableWindow03, Function | SmallTest | Level2)
{
    Rect winRect = {50, 50, 100, 150};
    Rect newWinRect = { 50, 50, 150, 300 };
    sptr<WindowNode> node = CreateDragNode(DragType::DRAG_BOTTOM_OR_TOP, winRect);
    policy_->UpdateFloatingWindowSizeForStretchableWindow(node, { 0, 0, 0, 0 }, newWinRect);
    Rect expRect = { 50, 50, 200, 300 };
    ASSERT_EQ(expRect, newWinRect);
}
}
}
}
