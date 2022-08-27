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

#include "display_manager.h"
#include "display_manager_config.h"
#include "window_node_container.h"
#include "future.h"
#include "window_node.h"
#include "wm_common.h"
#include "singleton_mocker.h"
#include "mock_window_node_container.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowNodeContainerTest"};
    using Mocker = SingletonMocker<WindowNodeContainer,MockWindowNodeContainer>;
}

class WindowNodeContainerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    static sptr<WindowNodeContainer> container_;
    static Rect windowRect_;
};

sptr<WindowNodeContainer> WindowNodeContainerTest::container_ = nullptr;
Rect WindowNodeContainerTest::windowRect_;

void WindowNodeContainerTest::SetUpTestCase()
{
    // DisplayManagerConfig::LoadConfigXml();
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    WLOGFI("GetDefaultDisplay: id %{public}" PRIu64", w %{public}d, h %{public}d, fps %{public}u",
        display->GetId(), display->GetWidth(), display->GetHeight(), display->GetRefreshRate());

    container_ = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRect_ = {0, 0, 100, 200};
}

void WindowNodeContainerTest::TearDownTestCase()
{
    container_ = nullptr;
}

void WindowNodeContainerTest::SetUp()
{
}

void WindowNodeContainerTest::TearDown()
{
}

sptr<WindowProperty> CreateWindowProperty(uint32_t windowId, const std::string& windowName,
    WindowType type, WindowMode mode, const Rect& screenRect)
{
    sptr<WindowProperty> property = new WindowProperty();
    property->SetWindowId(windowId);
    property->SetWindowName(windowName);
    property->SetWindowType(type);
    property->SetWindowMode(mode);
    property->SetWindowRect(screenRect);
    return property;
}

namespace {
/**
 * @tc.name: AddWindowNodeOnWindowTree01
 * @tc.desc: add system sub window to system window
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeContainerTest, AddWindowNodeOnWindowTree01, Function | SmallTest | Level2)
{
    sptr<WindowProperty> parentProperty = CreateWindowProperty(110u, "test1",
        WindowType::WINDOW_TYPE_APP_LAUNCHING, WindowMode::WINDOW_MODE_FLOATING, windowRect_);
    sptr<WindowNode> parentNode = new WindowNode(parentProperty, nullptr, nullptr);

    sptr<WindowProperty> subProperty = CreateWindowProperty(111u, "test2",
        WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW, WindowMode::WINDOW_MODE_FLOATING, windowRect_);
    sptr<WindowNode> subNode = new WindowNode(subProperty, nullptr, nullptr);

    ASSERT_EQ(WMError::WM_OK, container_->AddWindowNodeOnWindowTree(subNode, parentNode));
}

/**
 * @tc.name: AddWindowNodeOnWindowTree02
 * @tc.desc: add system sub window to system sub window
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeContainerTest, AddWindowNodeOnWindowTree02, Function | SmallTest | Level2)
{
    sptr<WindowProperty> parentProperty = CreateWindowProperty(110u, "test1",
        WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW, WindowMode::WINDOW_MODE_FLOATING, windowRect_);
    sptr<WindowNode> parentNode = new WindowNode(parentProperty, nullptr, nullptr);

    sptr<WindowProperty> subProperty = CreateWindowProperty(111u, "test2",
        WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW, WindowMode::WINDOW_MODE_FLOATING, windowRect_);
    sptr<WindowNode> subNode = new WindowNode(subProperty, nullptr, nullptr);

    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, container_->AddWindowNodeOnWindowTree(subNode, parentNode));
}

/**
 * @tc.name: AddWindowNodeOnWindowTree03
 * @tc.desc: add system sub window without parent
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeContainerTest, AddWindowNodeOnWindowTree03, Function | SmallTest | Level2)
{
    sptr<WindowProperty> subProperty = CreateWindowProperty(110u, "test1",
        WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW, WindowMode::WINDOW_MODE_FLOATING, windowRect_);
    sptr<WindowNode> subNode = new WindowNode(subProperty, nullptr, nullptr);

    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, container_->AddWindowNodeOnWindowTree(subNode, nullptr));
}
/**
 * @tc.name: MinimizeAppNodeExceptOptions
 * @tc.desc: minimize app node
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeContainerTest, MinimizeAppNodeExceptOptions, Function | SmallTest | Level2)
{
    std::vector<uint32_t> exceptionalIds;
    std::vector<WindowMode> exceptionalModes;
    ASSERT_EQ(WMError::WM_OK, container_->MinimizeAppNodeExceptOptions(MinimizeReason::OTHER_WINDOW,
        exceptionalIds, exceptionalModes));

    sptr<WindowProperty> property1 = CreateWindowProperty(110u, "test1",
        WindowType::WINDOW_TYPE_APP_MAIN_WINDOW, WindowMode::WINDOW_MODE_FULLSCREEN, windowRect_);
    sptr<WindowNode> node1 = new WindowNode(property1, nullptr, nullptr);

    sptr<WindowProperty> property2 = CreateWindowProperty(111u, "test2",
        WindowType::WINDOW_TYPE_APP_MAIN_WINDOW, WindowMode::WINDOW_MODE_FLOATING, windowRect_);
    sptr<WindowNode> node2 = new WindowNode(property2, nullptr, nullptr);

    ASSERT_EQ(WMError::WM_OK, container_->AddWindowNodeOnWindowTree(node1, nullptr));
    ASSERT_EQ(WMError::WM_OK, container_->AddWindowNodeOnWindowTree(node2, nullptr));
    ASSERT_EQ(WMError::WM_OK, container_->MinimizeAppNodeExceptOptions(MinimizeReason::OTHER_WINDOW,
        exceptionalIds, exceptionalModes));
}
/**
 * @tc.name: DropShowWhenLockedWindowIfNeeded
 * @tc.desc: drop show when locken window
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeContainerTest, DropShowWhenLockedWindowIfNeeded, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = CreateWindowProperty(110u, "test1",
        WindowType::WINDOW_TYPE_KEYGUARD, WindowMode::WINDOW_MODE_FULLSCREEN, windowRect_);
    sptr<WindowNode> node = new WindowNode(property, nullptr, nullptr);
    ASSERT_NE(nullptr, node);
    container_->DropShowWhenLockedWindowIfNeeded(node);
}
/**
 * @tc.name: GetModeChangeHotZones
 * @tc.desc: get mode change hot zones
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeContainerTest, GetModeChangeHotZones, Function | SmallTest | Level2)
{
    ModeChangeHotZonesConfig hotZonesConfig { true, 10, 20, 30 };
    ModeChangeHotZones hotZones;
    container_->GetModeChangeHotZones(0, hotZones, hotZonesConfig);
    ASSERT_EQ(hotZones.fullscreen_.height_, 10);
    ASSERT_EQ(hotZones.primary_.width_, 20);
    ASSERT_EQ(hotZones.secondary_.width_, 30);
}
/**
 * @tc.name: UpdateCameraFloatWindowStatus
 * @tc.desc: update camera float window status
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeContainerTest, UpdateCameraFloatWindowStatus, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = CreateWindowProperty(110u, "test1",
        WindowType::WINDOW_TYPE_FLOAT_CAMERA, WindowMode::WINDOW_MODE_FULLSCREEN, windowRect_);
    sptr<WindowNode> node = new WindowNode(property, nullptr, nullptr);
    ASSERT_NE(nullptr, node);
    container_->UpdateCameraFloatWindowStatus(node, true);
}
/**
 * @tc.name: AddWindowNode
 * @tc.desc: add windownode, windowType is APP_WINDOW_BASE
 * @tc.type: FUNC
 */
 HWTEST_F(WindowNodeContainerTest, AddWindowNode01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker>m=std::make_unique<Mocker>();
    sptr<WindowProperty> property = CreateWindowProperty(110u, "test1",
        WindowType::APP_WINDOW_BASE, WindowMode::WINDOW_MODE_FULLSCREEN, windowRect_);
    sptr<WindowNode> node = new WindowNode(property, nullptr, nullptr);
    sptr<WindowNode> parentNode = new WindowNode(property, nullptr, nullptr);

    node->startingWindowShown_=false;
    EXPECT_CALL(m->Mock(), AddWindowNodeOnWindowTree(_, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_INVALID_TYPE));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_TYPE, container_->AddWindowNode(node,parentNode));
    EXPECT_CALL(m->Mock(), AddWindowNodeOnWindowTree(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, container_->AddWindowNode(node,parentNode));

    node->startingWindowShown_=true;
    node->SetDisplayId(100);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, container_->AddWindowNode(node,parentNode));
    node->SetDisplayId(0);
    ASSERT_EQ(WMError::WM_OK, container_->AddWindowNode(node,parentNode));
}
/**
 * @tc.name: AddWindowNode
 * @tc.desc: add windownode, windowType is SYSTEM_WINDOW_BASE
 * @tc.type: FUNC
 */
 HWTEST_F(WindowNodeContainerTest, AddWindowNode02, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker>m=std::make_unique<Mocker>();
    sptr<WindowProperty> property = CreateWindowProperty(110u, "test2",
        WindowType::SYSTEM_WINDOW_BASE, WindowMode::WINDOW_MODE_FULLSCREEN, windowRect_);
    sptr<WindowNode> node = new WindowNode(property, nullptr, nullptr);
    sptr<WindowNode> parentNode = new WindowNode(property, nullptr, nullptr);

    node->startingWindowShown_=false;
    EXPECT_CALL(m->Mock(), AddWindowNodeOnWindowTree(_, _)).Times(1).WillOnce(Return(WMError::WM_ERROR_INVALID_TYPE));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_TYPE, container_->AddWindowNode(node,parentNode));
    EXPECT_CALL(m->Mock(), AddWindowNodeOnWindowTree(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, container_->AddWindowNode(node,parentNode));

    node->startingWindowShown_=true;
    node->SetDisplayId(100);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, container_->AddWindowNode(node,parentNode));
    node->SetDisplayId(0);
    ASSERT_EQ(WMError::WM_OK, container_->AddWindowNode(node,parentNode));
}
/**
 * @tc.name: RemoveWindowNode
 * @tc.desc: Remove windownode
 * @tc.type: FUNC
 */
 HWTEST_F(WindowNodeContainerTest, RemoveWindowNode, Function | SmallTest | Level2)
{
    sptr<WindowNode> node=nullptr;
    ASSERT_EQ(WMError::WM_ERROR_DESTROYED_OBJECT, container_->RemoveWindowNode(node));

    std::unique_ptr<Mocker>m=std::make_unique<Mocker>();
    sptr<WindowProperty> property = CreateWindowProperty(110u, "test1",
        WindowType::SYSTEM_WINDOW_BASE, WindowMode::WINDOW_MODE_FULLSCREEN, windowRect_);
    sptr<WindowNode> node2 = new WindowNode(property, nullptr, nullptr);
    EXPECT_CALL(m->Mock(), HandleRemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_INVALID_TYPE));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, container_->RemoveWindowNode(node2));
    EXPECT_CALL(m->Mock(), HandleRemoveWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, container_->RemoveWindowNode(node2));
}
/**
 * @tc.name: UpdateWindowNode
 * @tc.desc: preprocess node and update RSTree
 * @tc.type: FUNC
 */
 HWTEST_F(WindowNodeContainerTest, UpdateWindowNode, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = CreateWindowProperty(110u, "test1",
        WindowType::SYSTEM_WINDOW_BASE, WindowMode::WINDOW_MODE_FULLSCREEN, windowRect_);
    sptr<WindowNode> node = new WindowNode(property, nullptr, nullptr);
    ASSERT_EQ(WMError::WM_OK, container_->UpdateWindowNode(node,WindowUpdateReason::UPDATE_ALL));
    ASSERT_EQ(WMError::WM_OK, container_->UpdateWindowNode(node,WindowUpdateReason::UPDATE_MODE));
}
/**
 * @tc.name: DestroyWindowNode
 * @tc.desc: Destroy WindowNode
 * @tc.type: FUNC
 */
 HWTEST_F(WindowNodeContainerTest, DestroyWindowNode, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker>m=std::make_unique<Mocker>();
    sptr<WindowProperty> property = CreateWindowProperty(110u, "test1",
        WindowType::APP_WINDOW_BASE, WindowMode::WINDOW_MODE_FULLSCREEN, windowRect_);
    sptr<WindowNode> node = new WindowNode(property, nullptr, nullptr);
    std::vector<uint32_t> windowId;

    EXPECT_CALL(m->Mock(), RemoveWindowNode(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, container_->DestroyWindowNode(node,windowId));
    EXPECT_CALL(m->Mock(), RemoveWindowNode(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_INVALID_TYPE));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_TYPE, container_->DestroyWindowNode(node,windowId));
}
/**
 * @tc.name: Destroy
 * @tc.desc: clear vector cache completely, swap with empty vector
 * @tc.type: FUNC
 */
  HWTEST_F(WindowNodeContainerTest, Destroy, Function | SmallTest | Level2)
{
    ASSERT_EQ(0, container_->Destroy().size());
}
}
}
}
