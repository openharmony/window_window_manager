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

#include <iremote_object.h>
#include <gtest/gtest.h>
#include "iremote_object_mocker.h"
#include "window_root.h"
#include "window_manager.h"
#include "window_manager_service.h"
#include "display_manager.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class WindowRootTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static sptr<WindowRoot> windowRoot_;
};

sptr<WindowRoot> WindowRootTest::windowRoot_ = nullptr;

void WindowRootTest::SetUpTestCase()
{
}

void WindowRootTest::TearDownTestCase()
{
}

void WindowRootTest::SetUp()
{
    windowRoot_ = new WindowRoot(nullptr);
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    ASSERT_TRUE((displayInfo != nullptr));
}

void WindowRootTest::TearDown()
{
    windowRoot_ = nullptr;
}

namespace {
/**
 * @tc.name: WindowRootTest01
 * @tc.desc: test WindowRoot GetTotalWindowNum
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest01, Function | SmallTest | Level2)
{
    uint32_t size = windowRoot_->GetTotalWindowNum();
    ASSERT_EQ(size, 0);
}

/**
 * @tc.name: WindowRootTest02
 * @tc.desc: test WindowRoot GetWindowForDumpAceHelpInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest02, Function | SmallTest | Level2)
{
    windowRoot_->GetWindowForDumpAceHelpInfo();
    sptr<WindowProperty> property = new WindowProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_DESKTOP);
    sptr<WindowNode> windowNode;
    windowRoot_->DestroyWindowInner(windowNode);
    windowNode = new WindowNode(property);
    windowRoot_->SaveWindow(windowNode);
    sptr<WindowNode> node = windowRoot_->GetWindowForDumpAceHelpInfo();
    ASSERT_NE(node, nullptr);
    windowRoot_->DestroyWindowInner(windowNode);

    property = new WindowProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    windowNode = new WindowNode(property);
    windowRoot_->SaveWindow(windowNode);
    node = windowRoot_->GetWindowForDumpAceHelpInfo();
    ASSERT_NE(node, nullptr);
    windowRoot_->DestroyWindowInner(windowNode);

    property = new WindowProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_STATUS_BAR);
    windowNode = new WindowNode(property);
    windowRoot_->SaveWindow(windowNode);
    node = windowRoot_->GetWindowForDumpAceHelpInfo();
    ASSERT_NE(node, nullptr);
    windowRoot_->DestroyWindowInner(windowNode);

    property = new WindowProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_KEYGUARD);
    windowNode = new WindowNode(property);
    windowRoot_->SaveWindow(windowNode);
    node = windowRoot_->GetWindowForDumpAceHelpInfo();
    ASSERT_NE(node, nullptr);
    windowRoot_->DestroyWindowInner(windowNode);
    windowNode->isVisible_ = true;
    windowRoot_->DestroyWindowInner(windowNode);
}

/**
 * @tc.name: WindowRootTest03
 * @tc.desc: test WindowRoot CreateWindowNodeContainer
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest03, Function | SmallTest | Level2)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();

    displayInfo->SetWidth(49);
    auto container = windowRoot_->CreateWindowNodeContainer(0, displayInfo);
    ASSERT_EQ(container, nullptr);

    displayInfo->SetWidth(7681);
    container = windowRoot_->CreateWindowNodeContainer(0, displayInfo);
    ASSERT_EQ(container, nullptr);

    displayInfo->SetWidth(50);
    displayInfo->SetHeight(49);
    container = windowRoot_->CreateWindowNodeContainer(0, displayInfo);
    ASSERT_EQ(container, nullptr);

    displayInfo->SetHeight(7681);
    container = windowRoot_->CreateWindowNodeContainer(0, displayInfo);
    ASSERT_EQ(container, nullptr);
}

/**
 * @tc.name: WindowRootTest04
 * @tc.desc: test WindowRoot GetWindowNodeContainer
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest04, Function | SmallTest | Level2)
{
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_NE(display, nullptr);
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    displayInfo->SetDisplayId(0);
    displayInfo->SetScreenGroupId(SCREEN_ID_INVALID);
    auto container = windowRoot_->CreateWindowNodeContainer(0, displayInfo);
    ASSERT_NE(container, nullptr);

    windowRoot_->GetWindowNodeContainer(DISPLAY_ID_INVALID);
}

/**
 * @tc.name: WindowRootTest05
 * @tc.desc: test WindowRoot GetBackgroundNodesByScreenId
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest05, Function | SmallTest | Level2)
{
    std::vector<sptr<WindowNode>> windowNodes;

    sptr<WindowProperty> property = new WindowProperty();
    property->SetDisplayId(DISPLAY_ID_INVALID);
    sptr<WindowNode> windowNode1 = new WindowNode(property);
    windowRoot_->SaveWindow(windowNode1);
    property->SetDisplayId(0);
    sptr<WindowNode> windowNode2 = new WindowNode(property);
    windowRoot_->SaveWindow(windowNode2);

    auto screenGroupId = DisplayManagerServiceInner::GetInstance().GetScreenGroupIdByDisplayId(DISPLAY_ID_INVALID);
    windowRoot_->GetBackgroundNodesByScreenId(screenGroupId, windowNodes);
    sptr<WindowNode> windowNode3 = new WindowNode();
    windowRoot_->SaveWindow(windowNode3);
    windowRoot_->windowNodeMap_.insert(std::make_pair(windowNode3->GetWindowId(), windowNode3));
    windowNodes.push_back(windowNode1);
    windowNodes.push_back(windowNode2);
    windowNodes.push_back(windowNode3);
    windowRoot_->GetBackgroundNodesByScreenId(screenGroupId, windowNodes);

    windowRoot_->DestroyWindowInner(windowNode1);
    windowRoot_->DestroyWindowInner(windowNode2);

    ASSERT_EQ(true, true);
}

/**
 * @tc.name: WindowRootTest06
 * @tc.desc: test WindowRoot AddDeathRecipient
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest06, Function | SmallTest | Level2)
{
    windowRoot_->AddDeathRecipient(nullptr);

    ASSERT_EQ(true, true);

    sptr<WindowNode> windowNode = new WindowNode();
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    sptr<IWindow> iWindow = iface_cast<IWindow>(iRemoteObjectMocker);
    windowNode->SetWindowToken(iWindow);
    windowRoot_->windowDeath_ = nullptr;
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->AddDeathRecipient(windowNode);
}

/**
 * @tc.name: WindowRootTest07
 * @tc.desc: test WindowRoot SaveWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest07, Function | SmallTest | Level2)
{
    windowRoot_->SaveWindow(nullptr);

    ASSERT_EQ(true, true);
}

/**
 * @tc.name: WindowRootTest08
 * @tc.desc: test WindowRoot MinimizeStructuredAppWindowsExceptSelf
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest08, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = new WindowProperty();
    property->SetDisplayId(DISPLAY_ID_INVALID);
    sptr<WindowNode> windowNode = new WindowNode(property);

    WMError ret = windowRoot_->MinimizeStructuredAppWindowsExceptSelf(windowNode);

    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: WindowRootTest09
 * @tc.desc: test WindowRoot MinimizeTargetWindows
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest09, Function | SmallTest | Level2)
{
    std::vector<uint32_t> windowIds;

    windowRoot_->MinimizeTargetWindows(windowIds);

    windowIds.push_back(INVALID_WINDOW_ID);
    windowRoot_->MinimizeTargetWindows(windowIds);

    sptr<WindowProperty> property = new WindowProperty();
    property->SetWindowId(1);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    auto windowNode = new WindowNode(property);
    windowRoot_->SaveWindow(windowNode);
    property = new WindowProperty();
    property->SetWindowId(2);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    windowNode = new WindowNode(property);
    windowRoot_->SaveWindow(windowNode);
    windowIds.push_back(1);
    windowIds.push_back(2);
    windowRoot_->MinimizeTargetWindows(windowIds);

    ASSERT_EQ(true, true);
}

/**
 * @tc.name: WindowRootTest10
 * @tc.desc: test WindowRoot GetSplitScreenWindowNodes
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest10, Function | SmallTest | Level2)
{
    std::vector<sptr<WindowNode>> windowNodes = windowRoot_->GetSplitScreenWindowNodes(DISPLAY_ID_INVALID);
    ASSERT_EQ(windowNodes.empty(), true);
    DisplayId defaultDisplayId = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    windowNodes = windowRoot_->GetSplitScreenWindowNodes(defaultDisplayId);
    ASSERT_EQ(windowNodes.empty(), true);
}

/**
 * @tc.name: WindowRootTest11
 * @tc.desc: test WindowRoot IsForbidDockSliceMove
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest11, Function | SmallTest | Level2)
{
    bool ret = windowRoot_->IsForbidDockSliceMove(DISPLAY_ID_INVALID);
    ASSERT_EQ(ret, true);

    ret = windowRoot_->IsForbidDockSliceMove(0);
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: WindowRootTest12
 * @tc.desc: test WindowRoot IsDockSliceInExitSplitModeArea
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest12, Function | SmallTest | Level2)
{
    bool ret = windowRoot_->IsDockSliceInExitSplitModeArea(DISPLAY_ID_INVALID);
    ASSERT_EQ(ret, false);

    ret = windowRoot_->IsDockSliceInExitSplitModeArea(0);
    ASSERT_EQ(ret, false);

    DisplayId defaultDisplayId = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    ret = windowRoot_->IsDockSliceInExitSplitModeArea(defaultDisplayId);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: WindowRootTest13
 * @tc.desc: test WindowRoot ExitSplitMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest13, Function | SmallTest | Level2)
{
    windowRoot_->ExitSplitMode(DISPLAY_ID_INVALID);

    windowRoot_->ExitSplitMode(0);
    ASSERT_EQ(true, true);
    DisplayId defaultDisplayId = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->ExitSplitMode(defaultDisplayId);
}

/**
 * @tc.name: WindowRootTest14
 * @tc.desc: test WindowRoot AddSurfaceNodeIdWindowNodePair
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest14, Function | SmallTest | Level2)
{
    windowRoot_->AddSurfaceNodeIdWindowNodePair(INVALID_WINDOW_ID, nullptr);

    ASSERT_EQ(true, true);
}

/**
 * @tc.name: WindowRootTest15
 * @tc.desc: test WindowRoot GetVisibilityWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest15, Function | SmallTest | Level2)
{
    std::vector<sptr<WindowVisibilityInfo>> infos = {};

    windowRoot_->GetVisibilityWindowInfo(infos);

    ASSERT_EQ(true, true);

    windowRoot_->lastOcclusionData_ =  std::make_shared<RSOcclusionData>();
    windowRoot_->lastOcclusionData_->visibleData_ = {1, 2, 3}; //1, 2, 3
    windowRoot_->GetVisibilityWindowInfo(infos);
}

/**
 * @tc.name: WindowRootTest16
 * @tc.desc: test WindowRoot GetAvoidAreaByType
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest16, Function | SmallTest | Level2)
{
    AvoidArea emptyArea;
    AvoidArea area;
    sptr<WindowNode> node = new WindowNode();

    area = windowRoot_->GetAvoidAreaByType(node->GetWindowId(), AvoidAreaType::TYPE_CUTOUT);
    ASSERT_EQ(area, emptyArea);

    windowRoot_->windowNodeMap_.insert(std::make_pair(node->GetWindowId(), node));
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(node->GetDisplayId(), container));
    area = windowRoot_->GetAvoidAreaByType(node->GetWindowId(), AvoidAreaType::TYPE_CUTOUT);
    ASSERT_EQ(area, emptyArea);
}

/**
 * @tc.name: WindowRootTest17
 * @tc.desc: test WindowRoot MinimizeAllAppWindows
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest17, Function | SmallTest | Level2)
{
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(DISPLAY_ID_INVALID, container));
    windowRoot_->MinimizeAllAppWindows(DISPLAY_ID_INVALID);

    ASSERT_EQ(true, true);
}

/**
 * @tc.name: WindowRootTest18
 * @tc.desc: test WindowRoot DestroyLeakStartingWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest18, Function | SmallTest | Level2)
{
    windowRoot_->DestroyLeakStartingWindow();

    ASSERT_EQ(true, true);
    sptr<WindowNode> node = new WindowNode();
    node->startingWindowShown_ = true;
    windowRoot_->windowNodeMap_.insert(std::make_pair(node->GetWindowId(), node));
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->DestroyLeakStartingWindow();
}

/**
 * @tc.name: WindowRootTest20
 * @tc.desc: test WindowRoot LayoutWhenAddWindowNode
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest20, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = nullptr;
    windowRoot_->LayoutWhenAddWindowNode(node, true);

    node = new WindowNode();
    windowRoot_->LayoutWhenAddWindowNode(node, true);

    ASSERT_EQ(true, true);
}

/**
 * @tc.name: WindowRootTest21
 * @tc.desc: test WindowRoot AddWindowNode
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest21, Function | SmallTest | Level2)
{
    WMError ret;
    sptr<WindowNode> node = nullptr;

    ret = windowRoot_->AddWindowNode(INVALID_WINDOW_ID, node, true);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: WindowRootTest22
 * @tc.desc: test WindowRoot RemoveWindowNode
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest22, Function | SmallTest | Level2)
{
    WMError ret;
    sptr<WindowNode> node = new WindowNode();

    windowRoot_->windowNodeMap_.insert(std::make_pair(node->GetWindowId(), node));
    ret = windowRoot_->RemoveWindowNode(node->GetWindowId(), true);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_DISPLAY);
    sptr<WindowNode> node1 = new WindowNode();
    node->children_.push_back(node1);
    ret = windowRoot_->RemoveWindowNode(node->GetWindowId(), true);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_DISPLAY);
}

/**
 * @tc.name: WindowRootTest23
 * @tc.desc: test WindowRoot UpdateWindowNode
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest23, Function | SmallTest | Level2)
{
    WMError ret;

    ret = windowRoot_->UpdateWindowNode(INVALID_WINDOW_ID, WindowUpdateReason::UPDATE_MODE);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);

    sptr<WindowNode> node = new WindowNode();
    windowRoot_->windowNodeMap_.insert(std::make_pair(node->GetWindowId(), node));
    ret = windowRoot_->UpdateWindowNode(node->GetWindowId(), WindowUpdateReason::UPDATE_MODE);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_DISPLAY);
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(node->GetDisplayId(), container));
    ret = windowRoot_->UpdateWindowNode(node->GetWindowId(), WindowUpdateReason::UPDATE_MODE);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_DISPLAY);
}

/**
 * @tc.name: WindowRootTest24
 * @tc.desc: test WindowRoot UpdateSizeChangeReason
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest24, Function | SmallTest | Level2)
{
    WMError ret;

    ret = windowRoot_->UpdateSizeChangeReason(INVALID_WINDOW_ID, WindowSizeChangeReason::UNDEFINED);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);

    sptr<WindowNode> node = new WindowNode();
    windowRoot_->windowNodeMap_.insert(std::make_pair(node->GetWindowId(), node));
    ret = windowRoot_->UpdateSizeChangeReason(node->GetWindowId(), WindowSizeChangeReason::UNDEFINED);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_DISPLAY);
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(node->GetDisplayId(), container));
    ret = windowRoot_->UpdateSizeChangeReason(node->GetWindowId(), WindowSizeChangeReason::UNDEFINED);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_DISPLAY);
}

/**
 * @tc.name: WindowRootTest25
 * @tc.desc: test WindowRoot SetBrightness
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest25, Function | SmallTest | Level2)
{
    windowRoot_->SetBrightness(INVALID_WINDOW_ID, 0);

    sptr<WindowNode> node = new WindowNode();
    windowRoot_->windowNodeMap_.insert(std::make_pair(node->GetWindowId(), node));
    windowRoot_->SetBrightness(node->GetWindowId(), 0);

    ASSERT_EQ(true, true);
}

/**
 * @tc.name: CheckAndNotifyWaterMarkChangedResult
 * @tc.desc: test WindowRoot CheckAndNotifyWaterMarkChangedResult
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, CheckAndNotifyWaterMarkChangedResult, Function | SmallTest | Level2)
{
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_NE(display, nullptr);
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    auto container = windowRoot_->CreateWindowNodeContainer(display->GetId(), displayInfo);
    ASSERT_NE(container, nullptr);

    windowRoot_->lastWaterMarkShowStates_ = false;
    windowRoot_->CheckAndNotifyWaterMarkChangedResult();
    ASSERT_EQ(windowRoot_->lastWaterMarkShowStates_, false);

    auto windowNode = new (std::nothrow)WindowNode();
    ASSERT_NE(windowNode, nullptr);
    windowNode->isVisible_ = true;
    windowNode->SetDisplayId(displayInfo->GetDisplayId());
    windowNode->property_->flags_ |= static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK);
    container->appWindowNode_->children_.push_back(windowNode);

    windowRoot_->CheckAndNotifyWaterMarkChangedResult();
    ASSERT_EQ(windowRoot_->lastWaterMarkShowStates_, true);

    container->appWindowNode_->children_.clear();
    windowRoot_->CheckAndNotifyWaterMarkChangedResult();
    ASSERT_EQ(windowRoot_->lastWaterMarkShowStates_, false);
}

/**
 * @tc.name: SetGestureNavigaionEnabled
 * @tc.desc: test WindowRoot SetGestureNavigaionEnabled
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, SetGestureNavigaionEnabled, Function | SmallTest | Level2)
{
    windowRoot_->lastGestureNativeEnabled_ = false;
    auto ret = windowRoot_->SetGestureNavigaionEnabled(false);
    ASSERT_EQ(ret, WMError::WM_DO_NOTHING);

    ret = windowRoot_->SetGestureNavigaionEnabled(true);
    ASSERT_EQ(ret, WMError::WM_OK);

    windowRoot_->lastGestureNativeEnabled_ = false;
}

/**
 * @tc.name: GetWindowVisibilityChangeInfo
 * @tc.desc: test WindowRoot GetWindowVisibilityChangeInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, GetWindowVisibilityChangeInfo, Function | SmallTest | Level2)
{
    std::shared_ptr<RSOcclusionData> occlusionData = std::make_shared<RSOcclusionData>();
    ASSERT_NE(occlusionData, nullptr);
    windowRoot_->GetWindowVisibilityChangeInfo(occlusionData);
    windowRoot_->lastOcclusionData_ = std::make_shared<RSOcclusionData>();
    windowRoot_->lastOcclusionData_->visibleData_ = {1, 2, 3}; //1, 2, 3
    occlusionData->visibleData_ = {4, 5, 6}; //4, 5, 6
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->GetWindowVisibilityChangeInfo(occlusionData);
}

/**
 * @tc.name: NotifyWindowVisibilityChange
 * @tc.desc: test WindowRoot NotifyWindowVisibilityChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, NotifyWindowVisibilityChange, Function | SmallTest | Level2)
{
    std::shared_ptr<RSOcclusionData> occlusionData = std::make_shared<RSOcclusionData>();
    ASSERT_NE(occlusionData, nullptr);
    windowRoot_->NotifyWindowVisibilityChange(occlusionData);
    windowRoot_->lastOcclusionData_ = std::make_shared<RSOcclusionData>();
    windowRoot_->lastOcclusionData_->visibleData_ = {1, 2, 3}; //1, 2, 3
    occlusionData->visibleData_ = {4, 5, 6}; //4, 5, 6
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->NotifyWindowVisibilityChange(occlusionData);
}

/**
 * @tc.name: ToggleShownStateForAllAppWindows
 * @tc.desc: test WindowRoot ToggleShownStateForAllAppWindows
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, ToggleShownStateForAllAppWindows, Function | SmallTest | Level2)
{
    ScreenId displayGroupId = 1;
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(displayGroupId, container));
    auto ret = windowRoot_->ToggleShownStateForAllAppWindows();
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: PostProcessAddWindowNode01
 * @tc.desc: test WindowRoot PostProcessAddWindowNode01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, PostProcessAddWindowNode01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = new WindowNode();
    sptr<WindowNode> parentNode = new WindowNode();
    sptr<WindowNodeContainer> container;
    auto ret = windowRoot_->PostProcessAddWindowNode(node, parentNode, container);
    ASSERT_EQ(ret, WMError::WM_DO_NOTHING);
    node->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    node->property_->SetWindowId(1);
    ret = windowRoot_->PostProcessAddWindowNode(node, parentNode, container);
    ASSERT_EQ(ret, WMError::WM_DO_NOTHING);
}

/**
 * @tc.name: PostProcessAddWindowNode02
 * @tc.desc: test WindowRoot PostProcessAddWindowNode02
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, PostProcessAddWindowNode02, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = new WindowNode();
    sptr<WindowNode> parentNode = nullptr;
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    sptr<WindowNodeContainer> container;
    node->currentVisibility_ = false;
    node->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    auto ret = windowRoot_->PostProcessAddWindowNode(node, parentNode, container);
    ASSERT_EQ(ret, WMError::WM_DO_NOTHING);
}

/**
 * @tc.name: PostProcessAddWindowNode03
 * @tc.desc: test WindowRoot PostProcessAddWindowNode03
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, PostProcessAddWindowNode03, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = new WindowNode();
    sptr<WindowNode> parentNode = new WindowNode();
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(node->GetDisplayId(), container));
    node->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    node->property_->SetWindowId(1);
    node->property_->SetFocusable(true);
    node->currentVisibility_ = true;
    auto ret = windowRoot_->PostProcessAddWindowNode(node, parentNode, container);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: PostProcessAddWindowNode04
 * @tc.desc: test WindowRoot PostProcessAddWindowNode04
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, PostProcessAddWindowNode04, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = new WindowNode();
    sptr<WindowNode> parentNode = new WindowNode();
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(node->GetDisplayId(), container));
    node->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    node->property_->SetWindowId(1);
    node->property_->SetFocusable(true);
    node->currentVisibility_ = true;
    sptr<WindowNode> node1 = new WindowNode();
    node->children_.push_back(node1);
    auto ret = windowRoot_->PostProcessAddWindowNode(node, parentNode, container);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: BindDialogToParent01
 * @tc.desc: test WindowRoot BindDialogToParent01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, BindDialogToParent01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = new WindowNode();
    node->dialogTargetToken_ = nullptr;
    sptr<WindowNode> parentNode = new WindowNode();
    auto ret = windowRoot_->BindDialogToParent(node, parentNode);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: BindDialogToParent02
 * @tc.desc: test WindowRoot BindDialogToParent02
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, BindDialogToParent02, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = new WindowNode();
    sptr<WindowNode> parentNode = new WindowNode();
    node->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    node->dialogTargetToken_ = new (std::nothrow) IRemoteObjectMocker();
    sptr<WindowNode> windowNode3 = new WindowNode();
    windowNode3->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    windowNode3->abilityToken_ = node->dialogTargetToken_;
    windowRoot_->SaveWindow(windowNode3);
    windowRoot_->windowNodeMap_.insert(std::make_pair(windowNode3->GetWindowId(), windowNode3));
    auto ret = windowRoot_->BindDialogToParent(node, parentNode);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: UpdateFocusableProperty
 * @tc.desc: test WindowRoot UpdateFocusableProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, UpdateFocusableProperty, Function | SmallTest | Level2)
{
    uint32_t windowId = 1;
    windowRoot_->UpdateFocusableProperty(windowId);
    auto node = windowRoot_->GetWindowNode(windowId);
    ASSERT_EQ(node, nullptr);
}

/**
 * @tc.name: SetWindowMode
 * @tc.desc: test WindowRoot SetWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, SetWindowMode, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = new WindowNode();
    WindowMode dstMode = WindowMode::WINDOW_MODE_UNDEFINED;
    auto ret = windowRoot_->SetWindowMode(node, dstMode);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_DISPLAY);
    dstMode = WindowMode::WINDOW_MODE_FULLSCREEN;
    ret = windowRoot_->SetWindowMode(node, dstMode);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_DISPLAY);
}

/**
 * @tc.name: DestroyWindowSelf
 * @tc.desc: test WindowRoot DestroyWindowSelf
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, DestroyWindowSelf, Function | SmallTest | Level2)
{
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<WindowNode> node = new WindowNode();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    auto ret = windowRoot_->DestroyWindowSelf(node, container);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: IsVerticalDisplay
 * @tc.desc: test WindowRoot IsVerticalDisplay
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, IsVerticalDisplay, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = new WindowNode();
    ASSERT_EQ(false, windowRoot_->IsVerticalDisplay(node));
}

/**
 * @tc.name: RequestFocus
 * @tc.desc: test WindowRoot RequestFocus
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, RequestFocus, Function | SmallTest | Level2)
{
    uint32_t windowId = 1;
    auto ret = windowRoot_->RequestFocus(windowId);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: RequestActiveWindow
 * @tc.desc: test WindowRoot RequestActiveWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, RequestActiveWindow, Function | SmallTest | Level2)
{
    uint32_t windowId = 1;
    auto ret = windowRoot_->RequestActiveWindow(windowId);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: ProcessWindowStateChange
 * @tc.desc: test WindowRoot ProcessWindowStateChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, ProcessWindowStateChange, Function | SmallTest | Level2)
{
    WindowState state = WindowState::STATE_INITIAL;
    WindowStateChangeReason reason = WindowStateChangeReason::NORMAL;
    windowRoot_->ProcessWindowStateChange(state, reason);
    ASSERT_EQ(reason, WindowStateChangeReason::NORMAL);
}

/**
 * @tc.name: NotifySystemBarTints
 * @tc.desc: test WindowRoot NotifySystemBarTints
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, NotifySystemBarTints, Function | SmallTest | Level2)
{
    ScreenId displayGroupId = 1;
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(displayGroupId, container));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    DisplayId displayId = displayInfo->GetDisplayId();
    std::vector<DisplayId> displayVec = { displayId };
    windowRoot_->displayIdMap_.insert(std::make_pair(displayGroupId, displayVec));
    windowRoot_->NotifySystemBarTints();
}

/**
 * @tc.name: NotifyDesktopUnfrozen
 * @tc.desc: test WindowRoot NotifyDesktopUnfrozen
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, NotifyDesktopUnfrozen, Function | SmallTest | Level2)
{
    sptr<WindowNode> windowNode3 = new WindowNode();
    windowRoot_->SaveWindow(windowNode3);
    windowRoot_->windowNodeMap_.insert(std::make_pair(windowNode3->GetWindowId(), windowNode3));
    auto ret = windowRoot_->NotifyDesktopUnfrozen();
    ASSERT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);
}

/**
 * @tc.name: RaiseZOrderForAppWindow
 * @tc.desc: test WindowRoot RaiseZOrderForAppWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, RaiseZOrderForAppWindow, Function | SmallTest | Level2)
{
    sptr<WindowNode> windowNode2 = nullptr;
    auto ret = windowRoot_->RaiseZOrderForAppWindow(windowNode2);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    sptr<WindowNode> windowNode3 = new WindowNode();
    ret = windowRoot_->RaiseZOrderForAppWindow(windowNode3);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: DispatchKeyEvent
 * @tc.desc: test WindowRoot DispatchKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, DispatchKeyEvent, Function | SmallTest | Level2)
{
    sptr<WindowNode> windowNode = new WindowNode();
    std::shared_ptr<MMI::KeyEvent> event = nullptr;
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->DispatchKeyEvent(windowNode, event);
}

/**
 * @tc.name: GetTopWindowId
 * @tc.desc: test WindowRoot GetTopWindowId
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, GetTopWindowId, Function | SmallTest | Level2)
{
    uint32_t mainWinId = 0;
    uint32_t topWinId = 1;
    auto ret = windowRoot_->GetTopWindowId(mainWinId, topWinId);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
}

/**
 * @tc.name: SetWindowLayoutMode
 * @tc.desc: test WindowRoot SetWindowLayoutMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, SetWindowLayoutMode, Function | SmallTest | Level2)
{
    DisplayId displayId = 1;
    WindowLayoutMode mode = WindowLayoutMode::BASE;
    auto ret = windowRoot_->SetWindowLayoutMode(displayId, mode);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: GetAllDisplayIds
 * @tc.desc: test WindowRoot GetAllDisplayIds,RemoveSingleUserWindowNodes
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, GetAllDisplayIds, Function | SmallTest | Level2)
{
    ScreenId displayGroupId = 1;
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(displayGroupId, container));
    windowRoot_->GetAllDisplayIds();
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    DisplayId displayId = displayInfo->GetDisplayId();
    std::vector<DisplayId> displayVec = { displayId };
    windowRoot_->displayIdMap_.insert(std::make_pair(displayGroupId, displayVec));
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->GetAllDisplayIds();
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->RemoveSingleUserWindowNodes(displayGroupId);
}

/**
 * @tc.name: GenAllWindowsLogInfo
 * @tc.desc: test WindowRoot GenAllWindowsLogInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, GenAllWindowsLogInfo, Function | SmallTest | Level2)
{
    ScreenId displayGroupId = 1;
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(displayGroupId, container));
    windowRoot_->GenAllWindowsLogInfo();
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    DisplayId displayId = displayInfo->GetDisplayId();
    std::vector<DisplayId> displayVec = { displayId };
    windowRoot_->displayIdMap_.insert(std::make_pair(displayGroupId, displayVec));
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->GenAllWindowsLogInfo();
}

/**
 * @tc.name: FocusFaultDetection
 * @tc.desc: test WindowRoot FocusFaultDetection
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, FocusFaultDetection, Function | SmallTest | Level2)
{
    ScreenId displayGroupId = 1;
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(displayGroupId, container));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    DisplayId displayId = displayInfo->GetDisplayId();
    std::vector<DisplayId> displayVec = { displayId };
    windowRoot_->displayIdMap_.insert(std::make_pair(displayGroupId, displayVec));
    windowRoot_->needCheckFocusWindow = false;
    windowRoot_->FocusFaultDetection();
    windowRoot_->needCheckFocusWindow = true;
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->FocusFaultDetection();
}

/**
 * @tc.name: ProcessExpandDisplayCreate
 * @tc.desc: test WindowRoot ProcessExpandDisplayCreate
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, ProcessExpandDisplayCreate, Function | SmallTest | Level2)
{
    ScreenId displayGroupId = 1;
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(displayGroupId, container));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    DisplayId displayId = displayInfo->GetDisplayId();
    std::vector<DisplayId> displayVec = { displayId };
    windowRoot_->displayIdMap_.insert(std::make_pair(displayGroupId, displayVec));
    DisplayId defaultDisplayId = DisplayGroupInfo::GetInstance().GetDefaultDisplayId();
    ASSERT_NE(display, nullptr);
    displayInfo->SetWidth(100);
    displayInfo->SetHeight(100);
    std::map<DisplayId, Rect> displayRectMap = {};
    windowRoot_->ProcessExpandDisplayCreate(defaultDisplayId, displayInfo, displayRectMap);
}

/**
 * @tc.name: GetAllDisplayRectsByDisplayInfo
 * @tc.desc: test WindowRoot GetAllDisplayRectsByDisplayInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, GetAllDisplayRectsByDisplayInfo, Function | SmallTest | Level2)
{
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap;
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->GetAllDisplayRectsByDisplayInfo(displayInfoMap);
}

/**
 * @tc.name: ProcessDisplayCreate
 * @tc.desc: test WindowRoot ProcessDisplayCreate
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, ProcessDisplayCreate, Function | SmallTest | Level2)
{
    DisplayId defaultDisplayId = DisplayGroupInfo::GetInstance().GetDefaultDisplayId();
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap;
    windowRoot_->ProcessDisplayCreate(defaultDisplayId, displayInfo, displayInfoMap);
}

/**
 * @tc.name: MoveNotShowingWindowToDefaultDisplay
 * @tc.desc: test WindowRoot MoveNotShowingWindowToDefaultDisplay
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, MoveNotShowingWindowToDefaultDisplay, Function | SmallTest | Level2)
{
    DisplayId defaultDisplayId = DisplayGroupInfo::GetInstance().GetDefaultDisplayId();
    ASSERT_TRUE((windowRoot_ != nullptr));
    DisplayId displayId = 1;
    sptr<WindowNode> node = new WindowNode();
    windowRoot_->windowNodeMap_.insert(std::make_pair(node->GetWindowId(), node));
    windowRoot_->MoveNotShowingWindowToDefaultDisplay(defaultDisplayId, displayId);
}

/**
 * @tc.name: ProcessDisplayDestroy
 * @tc.desc: test WindowRoot ProcessDisplayDestroy
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, ProcessDisplayDestroy, Function | SmallTest | Level2)
{
    DisplayId defaultDisplayId = DisplayGroupInfo::GetInstance().GetDefaultDisplayId();
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap;
    windowRoot_->ProcessDisplayDestroy(defaultDisplayId, displayInfo, displayInfoMap);
}

/**
 * @tc.name: ProcessDisplayChange
 * @tc.desc: test WindowRoot ProcessDisplayChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, ProcessDisplayChange, Function | SmallTest | Level2)
{
    DisplayId defaultDisplayId = DisplayGroupInfo::GetInstance().GetDefaultDisplayId();
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap;
    DisplayStateChangeType type = DisplayStateChangeType::BEFORE_SUSPEND;
    windowRoot_->ProcessDisplayChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    displayInfo = nullptr;
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->ProcessDisplayChange(defaultDisplayId, displayInfo, displayInfoMap, type);
}

/**
 * @tc.name: GetDisplayGroupRect
 * @tc.desc: test WindowRoot GetDisplayGroupRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, GetDisplayGroupRect, Function | SmallTest | Level2)
{
    DisplayId displayId = DisplayGroupInfo::GetInstance().GetDefaultDisplayId();
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->GetDisplayGroupRect(displayId);
}

/**
 * @tc.name: RemoveSingleUserWindowNodes
 * @tc.desc: test WindowRoot RemoveSingleUserWindowNodes
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, RemoveSingleUserWindowNodes, Function | SmallTest | Level2)
{
    int accountId = 1;
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->RemoveSingleUserWindowNodes(accountId);
}

/**
 * @tc.name: TakeWindowPairSnapshot
 * @tc.desc: test WindowRoot TakeWindowPairSnapshot
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, TakeWindowPairSnapshot, Function | SmallTest | Level2)
{
    DisplayId displayId = DisplayGroupInfo::GetInstance().GetDefaultDisplayId();
    bool flag = windowRoot_->TakeWindowPairSnapshot(displayId);
    ASSERT_EQ(false, flag);
}

/**
 * @tc.name: ClearWindowPairSnapshot
 * @tc.desc: test WindowRoot ClearWindowPairSnapshot
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, ClearWindowPairSnapshot, Function | SmallTest | Level2)
{
    DisplayId displayId = DisplayGroupInfo::GetInstance().GetDefaultDisplayId();
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->ClearWindowPairSnapshot(displayId);
}

/**
 * @tc.name: CheckAddingModeAndSize01
 * @tc.desc: test WindowRoot CheckAddingModeAndSize01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, CheckAddingModeAndSize01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = new WindowNode();
    node->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    sptr<WindowNodeContainer> container;
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->CheckAddingModeAndSize(node, container);
}

/**
 * @tc.name: CheckAddingModeAndSize02
 * @tc.desc: test WindowRoot CheckAddingModeAndSize02
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, CheckAddingModeAndSize02, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = new WindowNode();
    node->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sptr<Display> defaultDisplay_ = DisplayManager::GetInstance().GetDefaultDisplay();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(
        defaultDisplay_->GetDisplayInfo(), defaultDisplay_->GetScreenId());
    container->layoutMode_ = WindowLayoutMode::TILE;
    node->property_->SetModeSupportInfo(0);
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->CheckAddingModeAndSize(node, container);
}

/**
 * @tc.name: CheckAddingModeAndSize03
 * @tc.desc: test WindowRoot CheckAddingModeAndSize03
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, CheckAddingModeAndSize03, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = new WindowNode();
    node->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sptr<Display> defaultDisplay_ = DisplayManager::GetInstance().GetDefaultDisplay();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(
        defaultDisplay_->GetDisplayInfo(), defaultDisplay_->GetScreenId());
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->CheckAddingModeAndSize(node, container);
}

/**
 * @tc.name: GetDisplayRectWithoutSystemBarAreas
 * @tc.desc: test WindowRoot GetDisplayRectWithoutSystemBarAreas
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, GetDisplayRectWithoutSystemBarAreas, Function | SmallTest | Level2)
{
    sptr<WindowNode> dstNode = new WindowNode();
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->GetDisplayRectWithoutSystemBarAreas(dstNode);
}

/**
 * @tc.name: AddWindowNode01
 * @tc.desc: test WindowRoot AddWindowNode01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, AddWindowNode01, Function | SmallTest | Level2)
{
    WMError ret;
    sptr<WindowNode> node = new WindowNode();
    node->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);

    ret = windowRoot_->AddWindowNode(INVALID_WINDOW_ID, node, true);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_DISPLAY);
}

/**
 * @tc.name: RemoveWindowNode01
 * @tc.desc: test WindowRoot RemoveWindowNode01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, RemoveWindowNode01, Function | SmallTest | Level2)
{
    uint32_t windowId = 10;
    WMError ret = windowRoot_->RemoveWindowNode(windowId, true);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: SetBrightness01
 * @tc.desc: test WindowRoot SetBrightness01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, SetBrightness01, Function | SmallTest | Level2)
{
    windowRoot_->SetBrightness(INVALID_WINDOW_ID, 0);
    sptr<WindowNode> node = new WindowNode();
    node->SetDisplayId(0);
    node->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    windowRoot_->windowNodeMap_.insert(std::make_pair(node->GetDisplayId(), node));
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(node->GetDisplayId(), container));
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->SetBrightness(node->GetDisplayId(), 0);
}

/**
 * @tc.name: SetBrightness02
 * @tc.desc: test WindowRoot SetBrightness02
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, SetBrightness02, Function | SmallTest | Level2)
{
    windowRoot_->SetBrightness(INVALID_WINDOW_ID, 0);
    sptr<WindowNode> node = new WindowNode();
    node->SetDisplayId(0);
    node->property_->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    windowRoot_->windowNodeMap_.insert(std::make_pair(node->GetDisplayId(), node));
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(node->GetDisplayId(), container));
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->SetBrightness(node->GetDisplayId(), 0);
}

/**
 * @tc.name: HandleKeepScreenOn01
 * @tc.desc: test WindowRoot HandleKeepScreenOn01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, HandleKeepScreenOn01, Function | SmallTest | Level2)
{
    uint32_t windowId = 1;
    bool requireLock = false;
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->HandleKeepScreenOn(windowId, requireLock);
}

/**
 * @tc.name: UpdateFocusableProperty01
 * @tc.desc: test WindowRoot UpdateFocusableProperty01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, UpdateFocusableProperty01, Function | SmallTest | Level2)
{
    sptr<WindowNode> windowNode = new WindowNode();
    windowNode->SetDisplayId(0);
    windowNode->property_->SetWindowId(0);
    windowRoot_->windowNodeMap_.insert(std::make_pair(windowNode->GetDisplayId(), windowNode));
    windowRoot_->windowNodeMap_.insert(std::make_pair(windowNode->GetWindowId(), windowNode));
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(windowNode->GetDisplayId(), container));
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->UpdateFocusableProperty(windowNode->GetWindowId());
}

/**
 * @tc.name: SetWindowMode01
 * @tc.desc: test WindowRoot SetWindowMode01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, SetWindowMode01, Function | SmallTest | Level2)
{
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<WindowNode> node = new WindowNode();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(node->GetDisplayId(), container));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    DisplayId displayId = displayInfo->GetDisplayId();
    std::vector<DisplayId> displayVec = { displayId };
    windowRoot_->displayIdMap_.insert(std::make_pair(node->GetDisplayId(), displayVec));
    WindowMode dstMode = WindowMode::WINDOW_MODE_UNDEFINED;
    auto ret = windowRoot_->SetWindowMode(node, dstMode);
    ASSERT_EQ(ret, WMError::WM_OK);
    node->SetWindowMode(dstMode);
    ret = windowRoot_->SetWindowMode(node, dstMode);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: DestroyWindowSelf01
 * @tc.desc: test WindowRoot DestroyWindowSelf01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, DestroyWindowSelf01, Function | SmallTest | Level2)
{
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<WindowNode> node = new WindowNode();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    node->property_->SetWindowId(0);
    sptr<WindowNode> node1 = new WindowNode();
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    sptr<IWindow> iWindow = iface_cast<IWindow>(iRemoteObjectMocker);
    node1->SetWindowToken(iWindow);
    node1->property_->SetWindowId(1);
    node->abilityToken_ = new (std::nothrow) IRemoteObjectMocker();
    node1->abilityToken_ = new (std::nothrow) IRemoteObjectMocker();
    node1->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    node->children_.push_back(node1);
    auto ret = windowRoot_->DestroyWindowSelf(node, container);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: DestroyWindow
 * @tc.desc: test WindowRoot DestroyWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, DestroyWindow, Function | SmallTest | Level2)
{
    uint32_t windowId = 1;
    bool onlySelf = false;
    auto ret = windowRoot_->DestroyWindow(windowId, onlySelf);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: UpdateFocusWindowWithWindowRemoved01
 * @tc.desc: test WindowRoot UpdateFocusWindowWithWindowRemoved01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, UpdateFocusWindowWithWindowRemoved01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = nullptr;
    sptr<WindowNodeContainer> container = nullptr;
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->UpdateFocusWindowWithWindowRemoved(node, container);
}

/**
 * @tc.name: UpdateFocusWindowWithWindowRemoved02
 * @tc.desc: test WindowRoot UpdateFocusWindowWithWindowRemoved02
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, UpdateFocusWindowWithWindowRemoved02, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = new WindowNode();
    node->property_->SetWindowType(WindowType::WINDOW_TYPE_DOCK_SLICE);
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->UpdateFocusWindowWithWindowRemoved(node, container);

    node->property_->SetWindowId(1);
    sptr<WindowNode> node1 = new WindowNode();
    node->children_.push_back(node1);
    container->SetFocusWindow(1);
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->UpdateFocusWindowWithWindowRemoved(node, container);
}

/**
 * @tc.name: UpdateBrightnessWithWindowRemoved
 * @tc.desc: test WindowRoot UpdateBrightnessWithWindowRemoved
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, UpdateBrightnessWithWindowRemoved, Function | SmallTest | Level2)
{
    uint32_t windowId = 1;
    sptr<WindowNodeContainer> container;
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->UpdateBrightnessWithWindowRemoved(windowId, container);
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    container->SetBrightnessWindow(windowId);
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->UpdateBrightnessWithWindowRemoved(windowId, container);
}

/**
 * @tc.name: IsVerticalDisplay01
 * @tc.desc: test WindowRoot IsVerticalDisplay01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, IsVerticalDisplay01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = new WindowNode();
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(node->GetDisplayId(), container));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    DisplayId displayId = displayInfo->GetDisplayId();
    std::vector<DisplayId> displayVec = { displayId };
    windowRoot_->displayIdMap_.insert(std::make_pair(node->GetDisplayId(), displayVec));
    ASSERT_EQ(true, windowRoot_->IsVerticalDisplay(node));
}

/**
 * @tc.name: RequestFocus01
 * @tc.desc: test WindowRoot RequestFocus01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, RequestFocus01, Function | SmallTest | Level2)
{
    sptr<WindowNode> windowNode = new WindowNode();
    windowNode->currentVisibility_ = true;
    windowRoot_->windowNodeMap_.insert(std::make_pair(windowNode->GetWindowId(), windowNode));
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowNode->property_->SetFocusable(true);
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(windowNode->GetDisplayId(), container));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    DisplayId displayId = displayInfo->GetDisplayId();
    std::vector<DisplayId> displayVec = { displayId };
    windowRoot_->displayIdMap_.insert(std::make_pair(windowNode->GetDisplayId(), displayVec));
    auto ret = windowRoot_->RequestFocus(windowNode->GetWindowId());
    ASSERT_EQ(WMError::WM_DO_NOTHING, ret);
    windowNode->property_->SetFocusable(false);
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(windowNode->GetDisplayId(), container));
    ret = windowRoot_->RequestFocus(windowNode->GetWindowId());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);
}

/**
 * @tc.name: RequestActiveWindow01
 * @tc.desc: test WindowRoot RequestActiveWindow01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, RequestActiveWindow01, Function | SmallTest | Level2)
{
    sptr<WindowNode> windowNode = new WindowNode();
    windowNode->property_->SetWindowType(WindowType::WINDOW_TYPE_STATUS_BAR);
    windowRoot_->windowNodeMap_.insert(std::make_pair(windowNode->GetWindowId(), windowNode));
    auto ret = windowRoot_->RequestActiveWindow(windowNode->GetWindowId());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_TYPE, ret);
}

/**
 * @tc.name: RequestActiveWindow02
 * @tc.desc: test WindowRoot RequestActiveWindow02
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, RequestActiveWindow02, Function | SmallTest | Level2)
{
    sptr<WindowNode> windowNode = new WindowNode();
    windowNode->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    windowRoot_->windowNodeMap_.insert(std::make_pair(windowNode->GetWindowId(), windowNode));
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(windowNode->GetDisplayId(), container));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    DisplayId displayId = displayInfo->GetDisplayId();
    std::vector<DisplayId> displayVec = { displayId };
    windowRoot_->displayIdMap_.insert(std::make_pair(windowNode->GetDisplayId(), displayVec));
    auto ret = windowRoot_->RequestActiveWindow(windowNode->GetWindowId());
    ASSERT_EQ(WMError::WM_DO_NOTHING, ret);
}

/**
 * @tc.name: ProcessWindowStateChange01
 * @tc.desc: test WindowRoot ProcessWindowStateChange01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, ProcessWindowStateChange01, Function | SmallTest | Level2)
{
    sptr<WindowNode> windowNode = new WindowNode();
    WindowState state = WindowState::STATE_INITIAL;
    WindowStateChangeReason reason = WindowStateChangeReason::NORMAL;
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(windowNode->GetDisplayId(), container));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    DisplayId displayId = displayInfo->GetDisplayId();
    std::vector<DisplayId> displayVec = { displayId };
    windowRoot_->displayIdMap_.insert(std::make_pair(windowNode->GetDisplayId(), displayVec));
    windowRoot_->ProcessWindowStateChange(state, reason);
    ASSERT_EQ(reason, WindowStateChangeReason::NORMAL);
}

/**
 * @tc.name: FindWallpaperWindow
 * @tc.desc: test WindowRoot FindWallpaperWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, FindWallpaperWindow, Function | SmallTest | Level2)
{
    sptr<WindowNode> windowNode = new WindowNode();
    windowNode->property_->SetWindowType(WindowType::WINDOW_TYPE_WALLPAPER);
    windowRoot_->windowNodeMap_.insert(std::make_pair(windowNode->GetWindowId(), windowNode));
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->FindWallpaperWindow();
}

/**
 * @tc.name: RaiseZOrderForAppWindow01
 * @tc.desc: test WindowRoot RaiseZOrderForAppWindow01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, RaiseZOrderForAppWindow01, Function | SmallTest | Level2)
{
    sptr<WindowNode> windowNode = new WindowNode();
    windowNode->property_->SetWindowType(WindowType::WINDOW_TYPE_DOCK_SLICE);
    auto ret = windowRoot_->RaiseZOrderForAppWindow(windowNode);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowNode->property_->SetFocusable(true);
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(windowNode->GetDisplayId(), container));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    DisplayId displayId = displayInfo->GetDisplayId();
    std::vector<DisplayId> displayVec = { displayId };
    windowRoot_->displayIdMap_.insert(std::make_pair(windowNode->GetDisplayId(), displayVec));
    ret = windowRoot_->RaiseZOrderForAppWindow(windowNode);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: RaiseZOrderForAppWindow02
 * @tc.desc: test WindowRoot RaiseZOrderForAppWindow02
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, RaiseZOrderForAppWindow02, Function | SmallTest | Level2)
{
    sptr<WindowNode> windowNode = new WindowNode();
    windowNode->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    auto ret = windowRoot_->RaiseZOrderForAppWindow(windowNode);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowNode->property_->SetFocusable(true);
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(windowNode->GetDisplayId(), container));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    DisplayId displayId = displayInfo->GetDisplayId();
    std::vector<DisplayId> displayVec = { displayId };
    windowRoot_->displayIdMap_.insert(std::make_pair(windowNode->GetDisplayId(), displayVec));
    ret = windowRoot_->RaiseZOrderForAppWindow(windowNode);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: RaiseZOrderForAppWindow03
 * @tc.desc: test WindowRoot RaiseZOrderForAppWindow03
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, RaiseZOrderForAppWindow03, Function | SmallTest | Level2)
{
    sptr<WindowNode> windowNode = new WindowNode();
    windowNode->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    auto ret = windowRoot_->RaiseZOrderForAppWindow(windowNode);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_TYPE, ret);
}

/**
 * @tc.name: DispatchKeyEvent01
 * @tc.desc: test WindowRoot DispatchKeyEvent01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, DispatchKeyEvent01, Function | SmallTest | Level2)
{
    sptr<WindowNode> windowNode = new WindowNode();
    std::shared_ptr<MMI::KeyEvent> event = nullptr;
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(windowNode->GetDisplayId(), container));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    DisplayId displayId = displayInfo->GetDisplayId();
    std::vector<DisplayId> displayVec = { displayId };
    windowRoot_->displayIdMap_.insert(std::make_pair(windowNode->GetDisplayId(), displayVec));
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->DispatchKeyEvent(windowNode, event);
}

/**
 * @tc.name: GetTopWindowId01
 * @tc.desc: test WindowRoot GetTopWindowId01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, GetTopWindowId01, Function | SmallTest | Level2)
{
    sptr<WindowNode> windowNode1 = new WindowNode();
    windowNode1->currentVisibility_ = false;
    windowRoot_->SaveWindow(windowNode1);
    windowRoot_->windowNodeMap_.insert(std::make_pair(windowNode1->GetWindowId(), windowNode1));
    sptr<WindowNode> windowNode2 = new WindowNode();
    windowNode2->currentVisibility_ = true;
    windowRoot_->SaveWindow(windowNode2);
    windowRoot_->windowNodeMap_.insert(std::make_pair(windowNode2->GetWindowId(), windowNode2));
    sptr<WindowNode> windowNode3 = new WindowNode();
    windowNode3->currentVisibility_ = true;
    windowNode3->children_.push_back(windowNode2);
    windowRoot_->SaveWindow(windowNode3);
    windowRoot_->windowNodeMap_.insert(std::make_pair(windowNode3->GetWindowId(), windowNode3));
    uint32_t topWinId = windowNode1->GetWindowId();
    auto ret = windowRoot_->GetTopWindowId(windowNode3->GetWindowId(), topWinId);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    ret = windowRoot_->GetTopWindowId(windowNode1->GetWindowId(), topWinId);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    ret = windowRoot_->GetTopWindowId(windowNode2->GetWindowId(), topWinId);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
}

/**
 * @tc.name: SetWindowLayoutMode01
 * @tc.desc: test WindowRoot SetWindowLayoutMode01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, SetWindowLayoutMode01, Function | SmallTest | Level2)
{
    WindowLayoutMode mode = WindowLayoutMode::BASE;
    sptr<WindowNode> windowNode = new WindowNode();
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(windowNode->GetDisplayId(), container));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    DisplayId displayId = displayInfo->GetDisplayId();
    std::vector<DisplayId> displayVec = { displayId };
    windowRoot_->displayIdMap_.insert(std::make_pair(windowNode->GetDisplayId(), displayVec));
    auto ret = windowRoot_->SetWindowLayoutMode(windowNode->GetDisplayId(), mode);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: GetAllDisplayRectsByDMS
 * @tc.desc: test WindowRoot GetAllDisplayRectsByDMS
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, GetAllDisplayRectsByDMS, Function | SmallTest | Level2)
{
    ScreenId displayGroupId = 1;
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(displayGroupId, container));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    DisplayId displayId = displayInfo->GetDisplayId();
    std::vector<DisplayId> displayVec = { displayId };
    windowRoot_->displayIdMap_.insert(std::make_pair(displayGroupId, displayVec));
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->GetAllDisplayRectsByDMS(displayInfo);
}

/**
 * @tc.name: MoveNotShowingWindowToDefaultDisplay01
 * @tc.desc: test WindowRoot MoveNotShowingWindowToDefaultDisplay01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, MoveNotShowingWindowToDefaultDisplay01, Function | SmallTest | Level2)
{
    DisplayId defaultDisplayId = DisplayGroupInfo::GetInstance().GetDefaultDisplayId();
    ASSERT_TRUE((windowRoot_ != nullptr));
    sptr<WindowNode> node = new WindowNode();
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    sptr<IWindow> iWindow = iface_cast<IWindow>(iRemoteObjectMocker);
    node->SetWindowToken(iWindow);
    node->currentVisibility_ = false;
    windowRoot_->windowNodeMap_.insert(std::make_pair(node->GetWindowId(), node));
    windowRoot_->MoveNotShowingWindowToDefaultDisplay(defaultDisplayId, node->GetDisplayId());
}

/**
 * @tc.name: ProcessDisplayDestroy01
 * @tc.desc: test WindowRoot ProcessDisplayDestroy01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, ProcessDisplayDestroy01, Function | SmallTest | Level2)
{
    sptr<WindowNode> windowNode = new WindowNode();
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(windowNode->GetDisplayId(), container));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    DisplayId displayId = displayInfo->GetDisplayId();
    std::vector<DisplayId> displayVec = { displayId };
    windowRoot_->displayIdMap_.insert(std::make_pair(windowNode->GetDisplayId(), displayVec));
    ASSERT_TRUE((display != nullptr));
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap;
    displayInfoMap.insert(std::make_pair(windowNode->GetDisplayId(), display->GetDisplayInfo()));
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->ProcessDisplayDestroy(windowNode->GetDisplayId(), display->GetDisplayInfo(), displayInfoMap);
}

/**
 * @tc.name: GetDisplayGroupRect
 * @tc.desc: test WindowRoot GetDisplayGroupRect,HasPrivateWindow,HasMainFullScreenWindowShown
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, GetDisplayGroupRect01, Function | SmallTest | Level2)
{
    sptr<WindowNode> windowNode = new WindowNode();
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(windowNode->GetDisplayId(), container));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    DisplayId displayId = displayInfo->GetDisplayId();
    std::vector<DisplayId> displayVec = { displayId };
    windowRoot_->displayIdMap_.insert(std::make_pair(windowNode->GetDisplayId(), displayVec));
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->GetDisplayGroupRect(windowNode->GetDisplayId());
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->HasPrivateWindow(windowNode->GetDisplayId());
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->HasMainFullScreenWindowShown(windowNode->GetDisplayId());
}

/**
 * @tc.name: UpdateRsTree
 * @tc.desc: test WindowRoot UpdateRsTree
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, UpdateRsTree, Function | SmallTest | Level2)
{
    sptr<WindowNode> windowNode = new WindowNode();
    windowRoot_->windowNodeMap_.insert(std::make_pair(windowNode->GetWindowId(), windowNode));
    ASSERT_TRUE((windowRoot_ != nullptr));
    auto res = windowRoot_->UpdateRsTree(windowNode->GetWindowId(), true);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, res);
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(windowNode->GetDisplayId(), container));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    DisplayId displayId = displayInfo->GetDisplayId();
    std::vector<DisplayId> displayVec = { displayId };
    windowRoot_->displayIdMap_.insert(std::make_pair(windowNode->GetDisplayId(), displayVec));
    res = windowRoot_->UpdateRsTree(windowNode->GetWindowId(), true);
    ASSERT_EQ(WMError::WM_OK, res);
}

/**
 * @tc.name: CheckMultiDialogWindows
 * @tc.desc: test WindowRoot CheckMultiDialogWindows
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, CheckMultiDialogWindows, Function | SmallTest | Level2)
{
    sptr<WindowNode> windowNode = new WindowNode();
    windowNode->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    windowNode->abilityToken_ = new IRemoteObjectMocker();
    auto res = windowRoot_->CheckMultiDialogWindows(WindowType::WINDOW_TYPE_DIALOG, windowNode->abilityToken_);
    ASSERT_EQ(false, res);
    windowRoot_->windowNodeMap_.insert(std::make_pair(windowNode->GetWindowId(), windowNode));
    res = windowRoot_->CheckMultiDialogWindows(WindowType::WINDOW_TYPE_DIALOG, windowNode->abilityToken_);
    ASSERT_EQ(false, res);
}


/**
 * @tc.name: GetSplitScreenWindowNodes
 * @tc.desc: test WindowRoot GetSplitScreenWindowNodes
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, GetSplitScreenWindowNodes, Function | SmallTest | Level2)
{
    ScreenId displayGroupId = 1;
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(displayGroupId, container));
    std::vector<sptr<WindowNode>> windowNodes = windowRoot_->GetSplitScreenWindowNodes(DISPLAY_ID_INVALID);
    ASSERT_EQ(windowNodes.empty(), true);
    windowNodes = windowRoot_->GetSplitScreenWindowNodes(displayGroupId);
    ASSERT_EQ(windowNodes.empty(), true);
}

/**
 * @tc.name: IsForbidDockSliceMove01
 * @tc.desc: test WindowRoot IsForbidDockSliceMove01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, IsForbidDockSliceMove01, Function | SmallTest | Level2)
{
    ScreenId displayGroupId = 1;
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(displayGroupId, container));
    bool ret = windowRoot_->IsForbidDockSliceMove(displayGroupId);
    ASSERT_EQ(ret, true);
    ret = windowRoot_->IsDockSliceInExitSplitModeArea(displayGroupId);
    ASSERT_EQ(ret, false);
    windowRoot_->ExitSplitMode(displayGroupId);
}

/**
 * @tc.name: GetVisibilityWindowInfo01
 * @tc.desc: test WindowRoot GetVisibilityWindowInfo01
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, GetVisibilityWindowInfo01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = new WindowNode();
    windowRoot_->AddSurfaceNodeIdWindowNodePair(node->GetDisplayId(), node);
    std::vector<sptr<WindowVisibilityInfo>> infos = {};
    windowRoot_->lastOcclusionData_ =  std::make_shared<RSOcclusionData>();
    windowRoot_->lastOcclusionData_->visibleData_ = {node->GetDisplayId()};
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->GetVisibilityWindowInfo(infos);
}

/**
 * @tc.name: AddWindowNode05
 * @tc.desc: test WindowRoot AddWindowNode05
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, AddWindowNode05, Function | SmallTest | Level2)
{
    WMError ret;
    sptr<WindowNode> node = new WindowNode();
    node->property_->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    node->property_->SetModeSupportInfo(0);

    ret = windowRoot_->AddWindowNode(INVALID_WINDOW_ID, node, false);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_DISPLAY);
}

/**
 * @tc.name: UpdateDisplayOrientationWhenHideWindow
 * @tc.desc: test WindowRoot UpdateDisplayOrientationWhenHideWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, UpdateDisplayOrientationWhenHideWindow, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = new WindowNode();
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<WindowNodeContainer> container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(node->GetDisplayId(), container));
    ASSERT_TRUE((windowRoot_ != nullptr));
    windowRoot_->UpdateDisplayOrientationWhenHideWindow(node);
}
}
}
}
