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

// gtest
#include <gtest/gtest.h>
#include "window_test_utils.h"

#include "display_manager.h"
#include "display_manager_proxy.h"
#include "future.h"
#include "screen_manager.h"
#include "window_manager.h"
#include "window_accessibility_controller.h"
#include "window_impl.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Utils = WindowTestUtils;
class DisplayListener : public DisplayManager::IDisplayListener {
public:
    virtual void OnCreate(DisplayId) override;
    virtual void OnDestroy(DisplayId) override;
    virtual void OnChange(DisplayId) override;
    RunnableFuture<DisplayId> changeFuture_;
};

class ScreenListener : public ScreenManager::IScreenListener {
public:
    virtual void OnConnect(ScreenId) override;
    virtual void OnDisconnect(ScreenId) override;
    virtual void OnChange(ScreenId) override;
    RunnableFuture<ScreenId> changeFuture_;
};

class WindowRotationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    std::vector<sptr<Window>> activeWindows_;
    Utils::TestWindowInfo fullInfo_;
    sptr<DisplayListener> displayListener_;
    sptr<ScreenListener> screenListener_;
private:
    static constexpr uint32_t SPLIT_TEST_SLEEP_S = 1;
    static constexpr long FUTURE_GET_RESULT_TIMEOUT = 1000;
};

void DisplayListener::OnCreate(DisplayId displayId)
{
}

void DisplayListener::OnDestroy(DisplayId displayId)
{
}

void DisplayListener::OnChange(DisplayId displayId)
{
    changeFuture_.SetValue(displayId);
}

void ScreenListener::OnConnect(ScreenId screenId)
{
}

void ScreenListener::OnDisconnect(ScreenId screenId)
{
}

void ScreenListener::OnChange(ScreenId screenId)
{
    changeFuture_.SetValue(screenId);
}

void WindowRotationTest::SetUpTestCase()
{
}

void WindowRotationTest::TearDownTestCase()
{
}

void WindowRotationTest::SetUp()
{
    fullInfo_ = {
            .name = "",
            .rect = Utils::customAppRect_,
            .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
            .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
            .needAvoid = true,
            .parentLimit = false,
            .showWhenLocked = true,
            .parentId = INVALID_WINDOW_ID,
    };

    activeWindows_.clear();
    displayListener_ = new DisplayListener();
    DisplayManager::GetInstance().RegisterDisplayListener(displayListener_);
    screenListener_ = new ScreenListener();
    ScreenManager::GetInstance().RegisterScreenListener(screenListener_);
}

void WindowRotationTest::TearDown()
{
    while (!activeWindows_.empty()) {
        ASSERT_EQ(WMError::WM_OK, activeWindows_.back()->Destroy());
        activeWindows_.pop_back();
    }
    DisplayManager::GetInstance().UnregisterDisplayListener(displayListener_);
    ScreenManager::GetInstance().UnregisterScreenListener(screenListener_);
}

namespace {
}
} // namespace Rosen
} // namespace OHOS
