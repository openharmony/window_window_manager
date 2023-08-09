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
#include "window_manager.h"
#include "window_test_utils.h"
#include "wm_common.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowImmersiveTest"};

    const Rect SYS_BAR_REGION_NULL = { 0, 0, 0, 0 };
    const SystemBarProperty SYS_BAR_PROP_DEFAULT;
    const SystemBarProperty SYS_BAR_PROP_1(true, 0xE5111111, 0xE5222222);
    const SystemBarProperty SYS_BAR_PROP_2(false, 0xE5222222, 0xE5333333);
    const SystemBarProperty SYS_BAR_PROP_3(false, 0xE5333333, 0xE5444444);
    const SystemBarProperty SYS_BAR_PROP_4(true, 0xE5444444, 0x66555555);
    const SystemBarRegionTints TEST_PROPS_DEFAULT = {
        { WindowType::WINDOW_TYPE_STATUS_BAR, SYS_BAR_PROP_DEFAULT, SYS_BAR_REGION_NULL },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SYS_BAR_PROP_DEFAULT, SYS_BAR_REGION_NULL },
    };
    const SystemBarRegionTints TEST_PROPS_1 = {
        { WindowType::WINDOW_TYPE_STATUS_BAR, SYS_BAR_PROP_1, SYS_BAR_REGION_NULL },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SYS_BAR_PROP_2, SYS_BAR_REGION_NULL },
    };
    const SystemBarRegionTints TEST_PROPS_2 = {
        { WindowType::WINDOW_TYPE_STATUS_BAR, SYS_BAR_PROP_1, SYS_BAR_REGION_NULL },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SYS_BAR_PROP_3, SYS_BAR_REGION_NULL },
    };

    const Rect EMPTY_RECT = {0, 0, 0, 0};
    const float RATIO = 0.3;
}

using Utils = WindowTestUtils;
const int WAIT_ASYNC_US = 100000;  // 100000us

class TestSystemBarChangedListener : public ISystemBarChangedListener {
public:
    SystemBarRegionTints tints_ = TEST_PROPS_DEFAULT;
    void OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints) override;
};

class TestAvoidAreaChangedListener : public IAvoidAreaChangedListener {
public:
    AvoidArea avoidArea_;
    void OnAvoidAreaChanged(const AvoidArea avoidArea, AvoidAreaType type) override;
};

class WindowImmersiveTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    void SetWindowSystemProps(const sptr<Window>& window, const SystemBarRegionTints& props);
    bool SystemBarPropsEqualsTo(const SystemBarRegionTints& expect);
    void DumpFailedInfo(const SystemBarRegionTints& expect);
    void DumpFailedInfo(bool expectStatus, bool expectNav);
    bool SystemBarEnableState(bool expectStatus, bool expectNav);
    DisplayId displayId_ = 0;
    std::vector<sptr<Window>> activeWindows_;
    static vector<Rect> fullScreenExpecteds_;
    static sptr<TestSystemBarChangedListener> testSystemBarChangedListener_;
    static sptr<TestAvoidAreaChangedListener> testAvoidAreaChangedListener_;
    Utils::TestWindowInfo fullScreenAppinfo_;
    Utils::TestWindowInfo avoidBarInfo_;
    uint32_t leftAvoidW_;
    uint32_t leftAvoidH_;
    uint32_t topAvoidW_;
    uint32_t topAvoidH_;
    sptr<Window> backgroundWindow_;
};

vector<Rect> WindowImmersiveTest::fullScreenExpecteds_;
sptr<TestSystemBarChangedListener> WindowImmersiveTest::testSystemBarChangedListener_ =
    new TestSystemBarChangedListener();
sptr<TestAvoidAreaChangedListener> WindowImmersiveTest::testAvoidAreaChangedListener_ =
    new TestAvoidAreaChangedListener();

void WindowImmersiveTest::SetWindowSystemProps(const sptr<Window>& window, const SystemBarRegionTints& tints)
{
    for (auto tint : tints) {
        window->SetSystemBarProperty(tint.type_, tint.prop_);
    }
}

void WindowImmersiveTest::DumpFailedInfo(const SystemBarRegionTints& expect)
{
    auto act = testSystemBarChangedListener_->tints_;
    WLOGI("WindowImmersiveTest Expected:");
    for (auto tint : expect) {
        WLOGI("WindowType: %{public}4d, Enable: %{public}4d, Color: %{public}x | %{public}x",
            static_cast<uint32_t>(tint.type_), tint.prop_.enable_,
            tint.prop_.backgroundColor_, tint.prop_.contentColor_);
    }
    WLOGI("WindowImmersiveTest Act: ");
    for (auto tint : act) {
        WLOGI("WindowType: %{public}4d, Enable: %{public}4d, Color: %{public}x | %{public}x",
            static_cast<uint32_t>(tint.type_), tint.prop_.enable_,
            tint.prop_.backgroundColor_, tint.prop_.contentColor_);
    }
}

void WindowImmersiveTest::DumpFailedInfo(bool expectStatus, bool expectNav)
{
    auto act = testSystemBarChangedListener_->tints_;
    WLOGI("WindowImmersiveTest Expected:");
    WLOGI("expectStatus: %{public}4d, expectNav: %{public}4d", expectStatus, expectNav);
    WLOGI("WindowImmersiveTest Act: ");
    for (auto tint : act) {
        WLOGI("WindowType: %{public}4d, Enable: %{public}4d, Color: %{public}x | %{public}x",
            static_cast<uint32_t>(tint.type_), tint.prop_.enable_,
            tint.prop_.backgroundColor_, tint.prop_.contentColor_);
    }
}

bool WindowImmersiveTest::SystemBarPropsEqualsTo(const SystemBarRegionTints& expect)
{
    usleep(WAIT_ASYNC_US);
    auto act = testSystemBarChangedListener_->tints_;
    if (act.size() != expect.size()) {
        DumpFailedInfo(expect);
        return false;
    }
    for (auto item : expect) {
        bool check = false;
        for (auto tint : act) {
            if (item.prop_ == tint.prop_ && item.type_ == tint.type_) {
                check = true;
                break;
            }
        }
        if (!check) {
            DumpFailedInfo(expect);
            return false;
        }
        check = false;
    }
    return true;
}

bool WindowImmersiveTest::SystemBarEnableState(bool expectStatus, bool expectNav)
{
    usleep(WAIT_ASYNC_US);
    auto act = testSystemBarChangedListener_->tints_;
    bool check = false;
    for (auto tint : act) {
        if ((tint.type_ == WindowType::WINDOW_TYPE_STATUS_BAR && tint.prop_.enable_ == expectStatus)
            || (tint.type_ == WindowType::WINDOW_TYPE_NAVIGATION_BAR && tint.prop_.enable_ == expectNav)) {
            check = true;
        } else {
            check = false;
        }
    }
    if (!check) {
        DumpFailedInfo(expectStatus, expectNav);
    }
    return check;
}

void TestSystemBarChangedListener::OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints)
{
    WLOGI("TestSystemBarChangedListener Display ID: %{public}" PRIu64"", displayId);
    WLOGI("TestSystemBarChangedListener tints size: %{public}zu", tints.size());
    for (auto tint : tints) {
        auto type = tint.type_;
        for (uint32_t i = 0; i < tints_.size(); i++) {
            if (tints_[i].type_ == type) {
                tints_[i] = tint;
            }
        }
    }
}

void TestAvoidAreaChangedListener::OnAvoidAreaChanged(const AvoidArea avoidArea, AvoidAreaType type)
{
    avoidArea_ = avoidArea;
}

void WindowImmersiveTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    ASSERT_TRUE((display != nullptr));
    WLOGI("GetDefaultDisplay: id %{public}" PRIu64", w %{public}d, h %{public}d, fps %{public}u",
        display->GetId(), display->GetWidth(), display->GetHeight(), display->GetRefreshRate());
    Rect displayRect = {0, 0, display->GetWidth(), display->GetHeight()};
    Utils::InitByDisplayRect(displayRect);
}

void WindowImmersiveTest::TearDownTestCase()
{
}

void WindowImmersiveTest::SetUp()
{
    fullScreenAppinfo_ = {
        .name = "main",
        .rect = Utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN, // immersive setting
        .needAvoid = false, // immersive setting
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };
    avoidBarInfo_ = {
        .name = "LeftAvoidTest",
        .rect = EMPTY_RECT,
        .type = WindowType::WINDOW_TYPE_STATUS_BAR,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
    };
    // makesure left avoid win w < h
    leftAvoidW_ = std::min(Utils::displayRect_.width_, static_cast<uint32_t>(Utils::displayRect_.height_ * RATIO));
    leftAvoidH_ = Utils::displayRect_.height_;
    // makesure top avoid win h < w
    topAvoidW_ = Utils::displayRect_.width_;
    topAvoidH_ = std::min(Utils::displayRect_.height_, static_cast<uint32_t>(Utils::displayRect_.width_ * RATIO));

    WindowManager::GetInstance().RegisterSystemBarChangedListener(testSystemBarChangedListener_);
    activeWindows_.clear();
    sleep(1);
}

void WindowImmersiveTest::TearDown()
{
    while (!activeWindows_.empty()) {
        ASSERT_EQ(WMError::WM_OK, activeWindows_.back()->Destroy());
        activeWindows_.pop_back();
    }
    WindowManager::GetInstance().UnregisterSystemBarChangedListener(testSystemBarChangedListener_);
    sleep(1);
}

namespace {
}
} // namespace Rosen
} // namespace OHOS
