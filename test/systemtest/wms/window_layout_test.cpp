/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

// gtest
#include <gtest/gtest.h>
#include "display_manager_proxy.h"
#include "window_manager.h"
#include "window_test_utils.h"
#include "wm_common.h"
#include "window_adapter.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Utils = WindowTestUtils;
class WindowLayoutTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    DisplayId displayId_ = 0;
    std::vector<sptr<Window>> activeWindows_;
    static vector<Rect> fullScreenExpecteds_;
    static inline float virtualPixelRatio_ = 0.0;
private:
    static constexpr uint32_t WAIT_SYANC_US = 100000;
    static void InitAvoidArea();
};

vector<Rect> WindowLayoutTest::fullScreenExpecteds_;

void WindowLayoutTest::SetUpTestCase()
{
    SingletonContainer::Get<WindowAdapter>().MinimizeAllAppWindows(0);
    sleep(2);
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    ASSERT_TRUE((display != nullptr));
    Rect displayRect = {0, 0, display->GetWidth(), display->GetHeight()};
    Utils::InitByDisplayRect(displayRect);

    virtualPixelRatio_ = WindowTestUtils::GetVirtualPixelRatio(0);

    // calc expected rects
    Rect expected = { // 0. only statusBar
        0,
        Utils::statusBarRect_.height_,
        Utils::displayRect_.width_,
        Utils::displayRect_.height_ - Utils::statusBarRect_.height_,
    };
    fullScreenExpecteds_.push_back(expected);
    expected = { // 1. both statusBar and naviBar
        0,
        Utils::statusBarRect_.height_,
        Utils::displayRect_.width_,
        Utils::displayRect_.height_ - Utils::statusBarRect_.height_ - Utils::naviBarRect_.height_,
    };
    fullScreenExpecteds_.push_back(expected);
    expected = { // 2. only naviBar
        0,
        0,
        Utils::displayRect_.width_,
        Utils::displayRect_.height_ - Utils::naviBarRect_.height_,
    };
    fullScreenExpecteds_.push_back(expected);
    InitAvoidArea();
    sleep(2);
}

void WindowLayoutTest::InitAvoidArea()
{
    Utils::TestWindowInfo info = {
        .name = "avoidArea",
        .rect = {0, 0, 0, 0},
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };
    const sptr<Window>& window = Utils::CreateTestWindow(info);
    window->Show();
    window->SetLayoutFullScreen(true);
    window->GetAvoidAreaByType(AvoidAreaType::TYPE_SYSTEM, WindowTestUtils::systemAvoidArea_);
    window->Hide();
    window->Destroy();
}

void WindowLayoutTest::TearDownTestCase()
{
}

void WindowLayoutTest::SetUp()
{
    activeWindows_.clear();
}

void WindowLayoutTest::TearDown()
{
    while (!activeWindows_.empty()) {
        ASSERT_EQ(WMError::WM_OK, activeWindows_.back()->Destroy());
        activeWindows_.pop_back();
    }
}

namespace {
}
} // namespace Rosen
} // namespace OHOS
