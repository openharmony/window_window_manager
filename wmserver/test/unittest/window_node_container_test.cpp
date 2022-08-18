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

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowNodeContainerTest"};
}

class WindowNodeContainerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    static sptr<WindowNodeContainer> container;
    static Rect windowRect;
};

sptr<WindowNodeContainer> WindowNodeContainerTest::container = nullptr;
Rect WindowNodeContainerTest::windowRect;

void WindowNodeContainerTest::SetUpTestCase()
{
    // DisplayManagerConfig::LoadConfigXml();
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    WLOGFI("GetDefaultDisplay: id %{public}" PRIu64", w %{public}d, h %{public}d, fps %{public}u",
        display->GetId(), display->GetWidth(), display->GetHeight(), display->GetRefreshRate());

    container = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    windowRect = {0, 0, 100, 200};
}

void WindowNodeContainerTest::TearDownTestCase()
{
}

void WindowNodeContainerTest::SetUp()
{
}

void WindowNodeContainerTest::TearDown()
{
}

sptr<WindowProperty> createWindowProperty(uint32_t windowId, const std::string& windowName,
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
    sptr<WindowProperty> parentProperty = createWindowProperty(110u, "test1",
        WindowType::WINDOW_TYPE_APP_LAUNCHING, WindowMode::WINDOW_MODE_FLOATING, windowRect);
    sptr<WindowNode> parentNode = new WindowNode(parentProperty, nullptr, nullptr);

    sptr<WindowProperty> subProperty = createWindowProperty(111u, "test2",
        WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW, WindowMode::WINDOW_MODE_FLOATING, windowRect);
    sptr<WindowNode> subNode = new WindowNode(subProperty, nullptr, nullptr);

    ASSERT_EQ(WMError::WM_OK, container->AddWindowNodeOnWindowTree(subNode, parentNode));
}

/**
 * @tc.name: AddWindowNodeOnWindowTree02
 * @tc.desc: add system sub window to system sub window
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeContainerTest, AddWindowNodeOnWindowTree02, Function | SmallTest | Level2)
{
    sptr<WindowProperty> parentProperty = createWindowProperty(110u, "test1",
        WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW, WindowMode::WINDOW_MODE_FLOATING, windowRect);
    sptr<WindowNode> parentNode = new WindowNode(parentProperty, nullptr, nullptr);

    sptr<WindowProperty> subProperty = createWindowProperty(111u, "test2",
        WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW, WindowMode::WINDOW_MODE_FLOATING, windowRect);
    sptr<WindowNode> subNode = new WindowNode(subProperty, nullptr, nullptr);

    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, container->AddWindowNodeOnWindowTree(subNode, parentNode));
}

/**
 * @tc.name: AddWindowNodeOnWindowTree03
 * @tc.desc: add system sub window without parent
 * @tc.type: FUNC
 */
HWTEST_F(WindowNodeContainerTest, AddWindowNodeOnWindowTree03, Function | SmallTest | Level2)
{
    sptr<WindowProperty> subProperty = createWindowProperty(110u, "test1",
        WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW, WindowMode::WINDOW_MODE_FLOATING, windowRect);
    sptr<WindowNode> subNode = new WindowNode(subProperty, nullptr, nullptr);

    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, container->AddWindowNodeOnWindowTree(subNode, nullptr));
}
}
}
}