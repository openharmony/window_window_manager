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

#include "avoid_area_controller.h"
#include "display_manager.h"
#include "display_manager_config.h"
#include "future.h"
#include "window_node.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "AvoidAreaControllerTest"};

    const Rect EMPTY_RECT = { 0, 0, 0, 0 };
    const float BARRATIO = 0.3;
    const long TIME_OUT = 1000;
    const AvoidArea EMPTY_AVOID_AREA = {};
}

class AvoidAreaControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    static sptr<WindowNode> statusbarWindowNode;
    static sptr<WindowNode> navigationBarWindowNode;
    static sptr<WindowNode> keyboardWindowNode;
    static Rect screenRect;
    static Rect cut_out_rect;
};

sptr<WindowNode> AvoidAreaControllerTest::statusbarWindowNode = nullptr;
sptr<WindowNode> AvoidAreaControllerTest::navigationBarWindowNode = nullptr;
sptr<WindowNode> AvoidAreaControllerTest::keyboardWindowNode = nullptr;
Rect AvoidAreaControllerTest::screenRect;
Rect AvoidAreaControllerTest::cut_out_rect = { 0, 0, 0, 0 };

class WindowListener : public IWindow {
public:
    void UpdateWindowRect(const struct Rect& rect, bool decoStatus, WindowSizeChangeReason reason) override {}
    void UpdateWindowMode(WindowMode mode) override {}
    void UpdateFocusStatus(bool focused) override {}
    void UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type) override
    {
        if (type == AvoidAreaType::TYPE_SYSTEM) {
            statusBarAvoidAreaFuture_.SetValue(*avoidArea);
        }
        if (type == AvoidAreaType::TYPE_CUTOUT) {
            cutoutAvoidAreaFuture_.SetValue(*avoidArea);
        }
        if (type == AvoidAreaType::TYPE_KEYBOARD) {
            keyboardAvoidAreaFuture_.SetValue(*avoidArea);
        }
    }
    void UpdateWindowModeSupportInfo(uint32_t modeSupportInfo) override {}
    void UpdateWindowState(WindowState state) override {}
    void UpdateWindowDragInfo(const PointInfo& point, DragEvent event) override {}
    void UpdateDisplayId(DisplayId from, DisplayId to) override {}
    void UpdateOccupiedAreaChangeInfo(const sptr<OccupiedAreaChangeInfo>& info) override {}
    void UpdateActiveStatus(bool isActive) override {}
    sptr<WindowProperty> GetWindowProperty() override
    {
        return nullptr;
    }
    void NotifyTouchOutside() override {}
    void NotifyScreenshot() override {}
    void NotifyDestroy(void) override {}
    void UpdateZoomTransform(const Transform& trans, bool isDisplayZoomOn) override {}
    void DumpInfo(const std::vector<std::string>& params, std::vector<std::string>& info) override {}
    void NotifyWindowClientPointUp(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override {}
    void RestoreSplitWindowMode(uint32_t mode) override {}
    RunnableFuture<AvoidArea> statusBarAvoidAreaFuture_;
    RunnableFuture<AvoidArea> keyboardAvoidAreaFuture_;
    RunnableFuture<AvoidArea> cutoutAvoidAreaFuture_;

    sptr<IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

void AvoidAreaControllerTest::SetUpTestCase()
{
    DisplayManagerConfig::LoadConfigXml();
    auto numbersConfig = DisplayManagerConfig::GetIntNumbersConfig();
    if (numbersConfig.count("cutoutArea") > 0) {
        std::vector<int> cutoutArea = numbersConfig["cutoutArea"];
        // 0, 1, 2, 3 means the index in the vector.
        cut_out_rect =  { cutoutArea[0], cutoutArea[1], (uint32_t)cutoutArea[2], (uint32_t)cutoutArea[3] };
    }
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    WLOGFI("GetDefaultDisplay: id %{public}" PRIu64", w %{public}d, h %{public}d, fps %{public}u",
        display->GetId(), display->GetWidth(), display->GetHeight(), display->GetRefreshRate());
    screenRect = { 0, 0, static_cast<uint32_t>(display->GetWidth()), static_cast<uint32_t>(display->GetHeight()) };
    auto barHeight = static_cast<uint32_t>(screenRect.height_ * BARRATIO);
    Rect statusBarRect = { 0, 0, screenRect.width_, barHeight };
    Rect navigationRect = { 0, static_cast<int32_t>(screenRect.height_ - barHeight), screenRect.width_, barHeight };

    sptr<WindowProperty> statusbarProperty = new WindowProperty();
    statusbarProperty->SetWindowId(100u);
    statusbarProperty->SetWindowName("status bar");
    statusbarProperty->SetWindowType(WindowType::WINDOW_TYPE_STATUS_BAR);
    statusbarProperty->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    statusbarProperty->SetWindowRect(statusBarRect);
    sptr<WindowListener> windowListener = new WindowListener();
    statusbarWindowNode = new WindowNode(statusbarProperty, windowListener, nullptr);

    sptr<WindowProperty> navigationBarProperty = new WindowProperty();
    navigationBarProperty->SetWindowId(101u);
    navigationBarProperty->SetWindowName("navigation bar");
    navigationBarProperty->SetWindowType(WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    navigationBarProperty->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    navigationBarProperty->SetWindowRect(navigationRect);
    windowListener = new WindowListener();
    navigationBarWindowNode = new WindowNode(navigationBarProperty, windowListener, nullptr);

    sptr<WindowProperty> keyboardProperty = new WindowProperty();
    keyboardProperty->SetWindowId(101u);
    keyboardProperty->SetWindowName("navigation bar");
    keyboardProperty->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    keyboardProperty->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    Rect keyboardRect = { 0, static_cast<int32_t>(screenRect.height_ / 2), screenRect.width_, screenRect.height_ / 2 };
    keyboardProperty->SetWindowRect(keyboardRect);
    windowListener = new WindowListener();
    keyboardWindowNode = new WindowNode(keyboardProperty, windowListener, nullptr);
}

void AvoidAreaControllerTest::TearDownTestCase()
{
}

void AvoidAreaControllerTest::SetUp()
{
}

void AvoidAreaControllerTest::TearDown()
{
}

bool CheckSameArea(AvoidArea avoidArea, Rect t, Rect l, Rect r, Rect b)
{
    return avoidArea.topRect_ == t && avoidArea.bottomRect_ == b
        && avoidArea.leftRect_ == l && avoidArea.rightRect_ == r;
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
 * @tc.name: SystemBarAndCutOutAvoidArea01
 * @tc.desc: Get avoid areas with TYPE_SYSTEM and TYPE_CUTOUT
 * @tc.type: FUNC
 */
HWTEST_F(AvoidAreaControllerTest, SystemBarAndCutOutAvoidArea01, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = createWindowProperty(110u, "test",
        WindowType::APP_WINDOW_BASE, WindowMode::WINDOW_MODE_FULLSCREEN, screenRect);
    sptr<WindowListener> listener = new WindowListener();
    sptr<WindowNode> appWindow = new WindowNode(property, listener, nullptr);
    uint32_t focusedWindow = appWindow->GetWindowId();
    sptr<AvoidAreaController> avoidAreaController = new AvoidAreaController(focusedWindow);
    avoidAreaController->ProcessWindowChange(statusbarWindowNode, AvoidControlType::AVOID_NODE_ADD, nullptr);
    avoidAreaController->ProcessWindowChange(navigationBarWindowNode, AvoidControlType::AVOID_NODE_ADD, nullptr);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_ADD, nullptr);
    auto avoidArea = avoidAreaController->GetAvoidAreaByType(appWindow, AvoidAreaType::TYPE_SYSTEM);
    ASSERT_EQ(true, CheckSameArea(avoidArea, statusbarWindowNode->GetWindowRect(),
        EMPTY_RECT, EMPTY_RECT, navigationBarWindowNode->GetWindowRect()));
    avoidArea = avoidAreaController->GetAvoidAreaByType(appWindow, AvoidAreaType::TYPE_CUTOUT);
    ASSERT_EQ(true, CheckSameArea(avoidArea, cut_out_rect, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));

    // set rect
    Rect statusBarRect = statusbarWindowNode->GetWindowRect();
    Rect navigationBarRect = navigationBarWindowNode->GetWindowRect();
    Rect windowRect = { 0, static_cast<int32_t>(statusBarRect.height_), statusBarRect.width_,
        static_cast<uint32_t>(navigationBarRect.posY_ - statusBarRect.height_) };
    property->SetWindowRect(windowRect);
    avoidArea = avoidAreaController->GetAvoidAreaByType(appWindow, AvoidAreaType::TYPE_SYSTEM);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));
    avoidArea = avoidAreaController->GetAvoidAreaByType(appWindow, AvoidAreaType::TYPE_CUTOUT);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));

    // restore rect
    property->SetWindowRect(screenRect);
    avoidArea = avoidAreaController->GetAvoidAreaByType(appWindow, AvoidAreaType::TYPE_SYSTEM);
    ASSERT_EQ(true, CheckSameArea(avoidArea, statusbarWindowNode->GetWindowRect(),
        EMPTY_RECT, EMPTY_RECT, navigationBarWindowNode->GetWindowRect()));
    avoidArea = avoidAreaController->GetAvoidAreaByType(appWindow, AvoidAreaType::TYPE_CUTOUT);
    ASSERT_EQ(true, CheckSameArea(avoidArea, cut_out_rect, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));

    avoidAreaController->ProcessWindowChange(statusbarWindowNode, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
    avoidAreaController->ProcessWindowChange(navigationBarWindowNode, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
    avoidArea = avoidAreaController->GetAvoidAreaByType(appWindow, AvoidAreaType::TYPE_SYSTEM);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));
}

/**
 * @tc.name: SystemBarAndCutOutAvoidArea02
 * @tc.desc: Get avoid areas with listener, TYPE_SYSTEM and TYPE_CUTOUT.
 * @tc.type: FUNC
 */
HWTEST_F(AvoidAreaControllerTest, SystemBarAndCutOutAvoidArea02, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = createWindowProperty(110u, "test",
        WindowType::APP_WINDOW_BASE, WindowMode::WINDOW_MODE_FULLSCREEN, screenRect);
    sptr<WindowListener> windowListener = new WindowListener();
    sptr<WindowNode> appWindow = new WindowNode(property, windowListener, nullptr);
    uint32_t focusedWindow = appWindow->GetWindowId();
    sptr<AvoidAreaController> avoidAreaController = new AvoidAreaController(focusedWindow);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_ADD, nullptr);
    avoidAreaController->UpdateAvoidAreaListener(appWindow, true);

    // add status bar
    avoidAreaController->ProcessWindowChange(statusbarWindowNode, AvoidControlType::AVOID_NODE_ADD, nullptr);
    auto avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, statusbarWindowNode->GetWindowRect(),
        EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));

    // add navigation bar
    avoidAreaController->ProcessWindowChange(navigationBarWindowNode, AvoidControlType::AVOID_NODE_ADD, nullptr);
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, statusbarWindowNode->GetWindowRect(), EMPTY_RECT,
        EMPTY_RECT, navigationBarWindowNode->GetWindowRect()));

    // update appWindow rect
    Rect statusBarRect = statusbarWindowNode->GetWindowRect();
    Rect navigationBarRect = navigationBarWindowNode->GetWindowRect();
    Rect windowRect = { 0, static_cast<int32_t>(statusBarRect.height_), statusBarRect.width_,
        static_cast<uint32_t>(navigationBarRect.posY_ - statusBarRect.height_) };
    property->SetWindowRect(windowRect);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_UPDATE, nullptr);
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));
    avoidArea = windowListener->cutoutAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->cutoutAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));

    // restore appWindow rect
    property->SetWindowRect(screenRect);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_UPDATE, nullptr);
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, statusbarWindowNode->GetWindowRect(),
        EMPTY_RECT, EMPTY_RECT, navigationBarWindowNode->GetWindowRect()));
    avoidArea = windowListener->cutoutAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->cutoutAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, cut_out_rect, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));

    avoidAreaController->ProcessWindowChange(statusbarWindowNode, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
    avoidAreaController->ProcessWindowChange(navigationBarWindowNode, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
}

/**
 * @tc.name: SystemBarAndCutOutAvoidArea03
 * @tc.desc: Get avoid areas with listener, TYPE_SYSTEM and TYPE_CUTOUT.
 * @tc.type: FUNC
 */
HWTEST_F(AvoidAreaControllerTest, SystemBarAndCutOutAvoidArea03, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = createWindowProperty(110u, "test",
        WindowType::APP_WINDOW_BASE, WindowMode::WINDOW_MODE_FULLSCREEN, screenRect);
    sptr<WindowListener> windowListener = new WindowListener();
    sptr<WindowNode> appWindow = new WindowNode(property, windowListener, nullptr);
    uint32_t focusedWindow = appWindow->GetWindowId();
    sptr<AvoidAreaController> avoidAreaController = new AvoidAreaController(focusedWindow);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_ADD, nullptr);
    avoidAreaController->UpdateAvoidAreaListener(appWindow, true);

    // add status bar
    avoidAreaController->ProcessWindowChange(statusbarWindowNode, AvoidControlType::AVOID_NODE_ADD, nullptr);
    auto avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, statusbarWindowNode->GetWindowRect(), EMPTY_RECT,
        EMPTY_RECT, EMPTY_RECT));

    // add navigation bar
    avoidAreaController->ProcessWindowChange(navigationBarWindowNode, AvoidControlType::AVOID_NODE_ADD, nullptr);
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, statusbarWindowNode->GetWindowRect(), EMPTY_RECT,
        EMPTY_RECT, navigationBarWindowNode->GetWindowRect()));

    // remove status bar
    avoidAreaController->ProcessWindowChange(statusbarWindowNode, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT,
        navigationBarWindowNode->GetWindowRect()));
    avoidArea = windowListener->cutoutAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->cutoutAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));

    // remove navigation bar
    avoidAreaController->ProcessWindowChange(navigationBarWindowNode, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
}

/**
 * @tc.name: SystemBarAvoidArea01
 * @tc.desc: Get avoid areas with listener, TYPE_SYSTEM.
 * @tc.type: FUNC
 */
HWTEST_F(AvoidAreaControllerTest, SystemBarAvoidArea01, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = createWindowProperty(110u, "test",
        WindowType::APP_WINDOW_BASE, WindowMode::WINDOW_MODE_FULLSCREEN, screenRect);
    sptr<WindowListener> windowListener = new WindowListener();
    sptr<WindowNode> appWindow = new WindowNode(property, windowListener, nullptr);
    uint32_t focusedWindow = appWindow->GetWindowId();
    sptr<AvoidAreaController> avoidAreaController = new AvoidAreaController(focusedWindow);
    avoidAreaController->ProcessWindowChange(statusbarWindowNode, AvoidControlType::AVOID_NODE_ADD, nullptr);
    avoidAreaController->ProcessWindowChange(navigationBarWindowNode, AvoidControlType::AVOID_NODE_ADD, nullptr);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_ADD, nullptr);
    avoidAreaController->UpdateAvoidAreaListener(appWindow, true);

    // update status bar window Rect
    Rect statusbarWindowNodeRect = statusbarWindowNode->GetWindowRect();
    statusbarWindowNode->SetWindowRect(EMPTY_RECT);
    avoidAreaController->ProcessWindowChange(statusbarWindowNode, AvoidControlType::AVOID_NODE_UPDATE,
        [](sptr<WindowNode> windowNode) { return true; });
    auto avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT,
        navigationBarWindowNode->GetWindowRect()));

    // update navigation bar window Rect
    Rect navigationBarWindowNodeRect = navigationBarWindowNode->GetWindowRect();
    navigationBarWindowNode->SetWindowRect(EMPTY_RECT);
    avoidAreaController->ProcessWindowChange(navigationBarWindowNode, AvoidControlType::AVOID_NODE_UPDATE,
        [](sptr<WindowNode> windowNode) { return true; });
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));

    // restore status bar window Rect
    statusbarWindowNode->SetWindowRect(statusbarWindowNodeRect);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_UPDATE, nullptr);
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, statusbarWindowNode->GetWindowRect(), EMPTY_RECT,
        EMPTY_RECT, EMPTY_RECT));

    // restore navigation bar window Rect
    navigationBarWindowNode->SetWindowRect(navigationBarWindowNodeRect);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_UPDATE, nullptr);
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, statusbarWindowNode->GetWindowRect(), EMPTY_RECT,
        EMPTY_RECT, navigationBarWindowNode->GetWindowRect()));

    // remove status bar
    avoidAreaController->ProcessWindowChange(statusbarWindowNode, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT,
        navigationBarWindowNode->GetWindowRect()));

    // remove navigation bar
    avoidAreaController->ProcessWindowChange(navigationBarWindowNode, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
    avoidArea = windowListener->statusBarAvoidAreaFuture_.GetResult(TIME_OUT);
    windowListener->statusBarAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
    ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));
}

/**
 * @tc.name: KeyboardAvoidArea01
 * @tc.desc: Get avoid areas with TYPE_KEYBOARD.
 * @tc.type: FUNC
 */
HWTEST_F(AvoidAreaControllerTest, KeyboardAvoidArea01, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = createWindowProperty(110u, "test",
        WindowType::APP_WINDOW_BASE, WindowMode::WINDOW_MODE_FULLSCREEN, screenRect);
    sptr<WindowListener> listener = new WindowListener();
    sptr<WindowNode> appWindow = new WindowNode(property, listener, nullptr);
    uint32_t focusedWindow = 0u;
    sptr<AvoidAreaController> avoidAreaController = new AvoidAreaController(focusedWindow);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_ADD, nullptr);

    uint32_t start = static_cast<uint32_t>(WindowMode::WINDOW_MODE_FULLSCREEN);
    uint32_t end = static_cast<uint32_t>(WindowMode::WINDOW_MODE_FLOATING);
    for (uint32_t i = start; i <= end; i++) {
        focusedWindow = 0u;
        appWindow->SetWindowMode(static_cast<WindowMode>(i));
        avoidAreaController->ProcessWindowChange(keyboardWindowNode, AvoidControlType::AVOID_NODE_ADD, nullptr);
        auto avoidArea = avoidAreaController->GetAvoidAreaByType(appWindow, AvoidAreaType::TYPE_KEYBOARD);
        ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));

        keyboardWindowNode->SetCallingWindow(appWindow->GetWindowId());
        avoidArea = avoidAreaController->GetAvoidAreaByType(appWindow, AvoidAreaType::TYPE_KEYBOARD);
        ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT,
            keyboardWindowNode->GetWindowRect()));

        keyboardWindowNode->SetCallingWindow(0);
        focusedWindow = appWindow->GetWindowId();
        avoidArea = avoidAreaController->GetAvoidAreaByType(appWindow, AvoidAreaType::TYPE_KEYBOARD);
        ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT,
            keyboardWindowNode->GetWindowRect()));

        avoidAreaController->ProcessWindowChange(keyboardWindowNode, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
        avoidArea = avoidAreaController->GetAvoidAreaByType(appWindow, AvoidAreaType::TYPE_KEYBOARD);
        ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));
    }
}

/**
 * @tc.name: KeyboardAvoidArea02
 * @tc.desc: Get avoid areas with listener, TYPE_KEYBOARD.
 * @tc.type: FUNC
 */
HWTEST_F(AvoidAreaControllerTest, KeyboardAvoidArea02, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = createWindowProperty(110u, "test",
        WindowType::APP_WINDOW_BASE, WindowMode::WINDOW_MODE_FULLSCREEN, screenRect);
    sptr<WindowListener> listener = new WindowListener();
    sptr<WindowNode> appWindow = new WindowNode(property, listener, nullptr);
    uint32_t focusedWindow = appWindow->GetWindowId();
    sptr<AvoidAreaController> avoidAreaController = new AvoidAreaController(focusedWindow);
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_ADD, nullptr);
    avoidAreaController->UpdateAvoidAreaListener(appWindow, true);
    uint32_t start = static_cast<uint32_t>(WindowMode::WINDOW_MODE_FULLSCREEN);
    uint32_t end = static_cast<uint32_t>(WindowMode::WINDOW_MODE_FLOATING);
    for (uint32_t i = start; i <= end; i++) {
        avoidAreaController->ProcessWindowChange(keyboardWindowNode, AvoidControlType::AVOID_NODE_ADD, nullptr);
        auto avoidArea = listener->keyboardAvoidAreaFuture_.GetResult(TIME_OUT);
        listener->keyboardAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
        ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT,
            keyboardWindowNode->GetWindowRect()));
        avoidAreaController->ProcessWindowChange(keyboardWindowNode, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
        avoidArea = listener->keyboardAvoidAreaFuture_.GetResult(TIME_OUT);
        listener->keyboardAvoidAreaFuture_.Reset(EMPTY_AVOID_AREA);
        ASSERT_EQ(true, CheckSameArea(avoidArea, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT, EMPTY_RECT));
    }
    avoidAreaController->ProcessWindowChange(appWindow, AvoidControlType::AVOID_NODE_REMOVE, nullptr);
}
}
} // namespace Rosen
} // namespace OHOS
