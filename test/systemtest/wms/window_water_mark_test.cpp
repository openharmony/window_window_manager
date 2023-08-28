/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>

#include <transaction/rs_transaction.h>
#include "display_manager.h"
#include "display_manager_proxy.h"
#include "surface_draw.h"
#include "window_test_utils.h"
#include "window_manager.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint32_t COLOR_RED = 0xffff0000;
    constexpr uint8_t ALPHA = 255;
    constexpr int NORMAL_SLEEP_TIME = 3; // 1s
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowWaterMarkTest"};
}

using Utils = WindowTestUtils;

class TestIWaterMarkFlagChangedListener : public IWaterMarkFlagChangedListener {
public:
    void OnWaterMarkFlagUpdate(bool showWaterMark) override;

    bool isShowing_ = false;
    bool isCallbackCalled_ = false;
};

void TestIWaterMarkFlagChangedListener::OnWaterMarkFlagUpdate(bool showWaterMark)
{
    WLOGFI("water mark flag update result:%{public}d", showWaterMark);
    isShowing_ = showWaterMark;
    isCallbackCalled_ = true;
}

class WaterMarkTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    bool FillColor(sptr<Window> window);

    static sptr<TestIWaterMarkFlagChangedListener> lisenter_;
    Utils::TestWindowInfo appInfo_;
    sptr<Window> CreateWindow(const Utils::TestWindowInfo& appinfo);
    static inline DisplayId displayId_;
};

sptr<TestIWaterMarkFlagChangedListener> WaterMarkTest::lisenter_ = nullptr;
void WaterMarkTest::SetUpTestCase()
{
    lisenter_= new TestIWaterMarkFlagChangedListener();
    WindowManager::GetInstance().RegisterWaterMarkFlagChangedListener(lisenter_);
    displayId_ = DisplayManager::GetInstance().GetDefaultDisplayId();
}

void WaterMarkTest::TearDownTestCase()
{
    WindowManager::GetInstance().UnregisterWaterMarkFlagChangedListener(lisenter_);
}

void WaterMarkTest::SetUp()
{
    appInfo_ = {
        .name = "testWindow",
        .rect = Utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .showWhenLocked = true,
        .parentId = INVALID_WINDOW_ID,
    };
}

void WaterMarkTest::TearDown()
{
}

sptr<Window> WaterMarkTest::CreateWindow(const Utils::TestWindowInfo& appinfo)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetDisplayId(displayId_);
    option->SetWindowRect(appinfo.rect);
    option->SetWindowType(appinfo.type);
    option->SetWindowMode(appinfo.mode);
    option->AddWindowFlag(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED);
    sptr<Window> window = Window::Create(appinfo.name, option);
    return window;
}

bool WaterMarkTest::FillColor(sptr<Window> window)
{
    if (window == nullptr) {
        return false;
    }
    auto surfaceNode = window->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        return false;
    }
    Rect rect = window->GetRect();
    bool isDrawSuccess = SurfaceDraw::DrawColor(surfaceNode, rect.width_, rect.height_, COLOR_RED);
    surfaceNode->SetAbilityBGAlpha(ALPHA);
    RSTransaction::FlushImplicitTransaction();
    return isDrawSuccess;
}

namespace {
/**
* @tc.name: WindowVisibilityInfoTest01
* @tc.desc: window show or hide
* @tc.type: FUNC
* @tc.require: issueI5FSQW
*/
HWTEST_F(WaterMarkTest, SetWaterMarkFlag01, Function | MediumTest | Level1)
{
    appInfo_.name = "window1";
    appInfo_.rect = {200, 200, 300, 300};
    sptr<Window> window = CreateWindow(appInfo_);
    if (window == nullptr) {
        return;
    }
    ASSERT_NE(window, nullptr);
    window->Show();
    sleep(NORMAL_SLEEP_TIME);
    auto drawSuccess = FillColor(window);
    sleep(NORMAL_SLEEP_TIME);
    ASSERT_TRUE(drawSuccess);

    window->AddWindowFlag(WindowFlag::WINDOW_FLAG_WATER_MARK);
    sleep(NORMAL_SLEEP_TIME);
    ASSERT_EQ(lisenter_->isShowing_, true);

    window->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_WATER_MARK);
    sleep(NORMAL_SLEEP_TIME);
    ASSERT_EQ(lisenter_->isShowing_, false);

    window->Destroy();
    sleep(NORMAL_SLEEP_TIME);
}
}
} // namespace Rosen
} // namespace OHOS

