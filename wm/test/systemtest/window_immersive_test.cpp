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
#include "window_manager.h"
#include "window_test_utils.h"
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
    const SystemBarRegionTints TEST_DIFF_PROPS_1_2 = {
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SYS_BAR_PROP_3, SYS_BAR_REGION_NULL },
    };
    const SystemBarRegionTints TEST_DIFF_PROPS_2_1 = {
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SYS_BAR_PROP_2, SYS_BAR_REGION_NULL },
    };

    const Rect EMPTY_RECT = {0, 0, 0, 0};
    const float RATIO = 0.3;
}

using utils = WindowTestUtils;
const int WAIT_ASYNC_US = 100000;  // 100000us

class TestSystemBarChangedListener : public ISystemBarChangedListener {
public:
    SystemBarRegionTints tints_;
    void OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints) override;
};

class TestAvoidAreaChangedListener : public IAvoidAreaChangedListener {
public:
    std::vector<Rect> avoidAreas_;
    void OnAvoidAreaChanged(std::vector<Rect> avoidAreas) override;
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
    DisplayId displayId_ = 0;
    std::vector<sptr<Window>> activeWindows_;
    static vector<Rect> fullScreenExpecteds_;
    static sptr<TestSystemBarChangedListener> testSystemBarChangedListener_;
    static sptr<TestAvoidAreaChangedListener> testAvoidAreaChangedListener_;
    utils::TestWindowInfo fullScreenAppinfo_;
    utils::TestWindowInfo avoidBarInfo_;
    uint32_t leftAvoidW_;
    uint32_t leftAvoidH_;
    uint32_t topAvoidW_;
    uint32_t topAvoidH_;
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
    WLOGFI("WindowImmersiveTest Expected:");
    for (auto tint : expect) {
        WLOGFI("WindowType: %{public}4d, Enable: %{public}4d, Color: %{public}x | %{public}x",
            static_cast<uint32_t>(tint.type_), tint.prop_.enable_,
            tint.prop_.backgroundColor_, tint.prop_.contentColor_);
    }
    WLOGFI("WindowImmersiveTest Act: ");
    for (auto tint : act) {
        WLOGFI("WindowType: %{public}4d, Enable: %{public}4d, Color: %{public}x | %{public}x",
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

void TestSystemBarChangedListener::OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints)
{
    WLOGFI("TestSystemBarChangedListener Display ID: %{public}" PRIu64"", displayId);
    tints_ = tints;
}

void TestAvoidAreaChangedListener::OnAvoidAreaChanged(std::vector<Rect> avoidAreas)
{
    avoidAreas_ = avoidAreas;
}

void WindowImmersiveTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    if (display == nullptr) {
        WLOGFE("GetDefaultDisplay: failed!");
    } else {
        WLOGFI("GetDefaultDisplay: id %{public}" PRIu64", w %{public}d, h %{public}d, fps %{public}u",
            display->GetId(), display->GetWidth(), display->GetHeight(), display->GetFreshRate());
    }
    Rect displayRect = {0, 0, display->GetWidth(), display->GetHeight()};
    utils::InitByDisplayRect(displayRect);
}

void WindowImmersiveTest::TearDownTestCase()
{
}

void WindowImmersiveTest::SetUp()
{
    fullScreenAppinfo_ = {
        .name = "main",
        .rect = utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN, // immersive setting
        .needAvoid = false, // immersive setting
        .parentLimit = false,
        .parentName = "",
    };
    avoidBarInfo_ = {
        .name = "LeftAvoidTest",
        .rect = EMPTY_RECT,
        .type = WindowType::WINDOW_TYPE_STATUS_BAR,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
    };
    // makesure left avoid win w < h
    leftAvoidW_ = std::min(utils::displayRect_.width_, static_cast<uint32_t>(utils::displayRect_.height_ * RATIO));
    leftAvoidH_ = utils::displayRect_.height_;
    // makesure top avoid win h < w
    topAvoidW_ = utils::displayRect_.width_;
    topAvoidH_ = std::min(utils::displayRect_.height_, static_cast<uint32_t>(utils::displayRect_.width_ * RATIO));

    WindowManager::GetInstance().RegisterSystemBarChangedListener(testSystemBarChangedListener_);
    activeWindows_.clear();
}

void WindowImmersiveTest::TearDown()
{
    while (!activeWindows_.empty()) {
        ASSERT_EQ(WMError::WM_OK, activeWindows_.back()->Destroy());
        activeWindows_.pop_back();
    }
    WindowManager::GetInstance().UnregisterSystemBarChangedListener(testSystemBarChangedListener_);
}

namespace {
/**
 * @tc.name: ImmersiveTest01
 * @tc.desc: Add one immersive window and hide
 * @tc.type: FUNC
 * @tc.require: AR000GGTVD
 */
HWTEST_F(WindowImmersiveTest, ImmersiveTest01, Function | MediumTest | Level3)
{
    const sptr<Window>& window = utils::CreateTestWindow(fullScreenAppinfo_);
    activeWindows_.push_back(window);
    SetWindowSystemProps(window, TEST_PROPS_1);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_PROPS_1));
    ASSERT_EQ(WMError::WM_OK, window->Hide());
    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_PROPS_DEFAULT));
}

/**
 * @tc.name: ImmersiveTest02
 * @tc.desc: Add two immersive window and switch
 * @tc.type: FUNC
 * @tc.require: AR000GGTVD
 */
HWTEST_F(WindowImmersiveTest, ImmersiveTest02, Function | MediumTest | Level3)
{
    const sptr<Window>& window1 = utils::CreateTestWindow(fullScreenAppinfo_);
    activeWindows_.push_back(window1);
    SetWindowSystemProps(window1, TEST_PROPS_1);
    fullScreenAppinfo_.name = "main2";
    const sptr<Window>& window2 = utils::CreateTestWindow(fullScreenAppinfo_);
    activeWindows_.push_back(window2);
    SetWindowSystemProps(window2, TEST_PROPS_2);
    ASSERT_EQ(WMError::WM_OK, window1->Show());
    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_PROPS_1));
    ASSERT_EQ(WMError::WM_OK, window2->Show());

    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_DIFF_PROPS_1_2));
    ASSERT_EQ(WMError::WM_OK, window2->Hide());
    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_DIFF_PROPS_2_1));
    ASSERT_EQ(WMError::WM_OK, window1->Hide());
    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_PROPS_DEFAULT));
}

/**
 * @tc.name: ImmersiveTest03
 * @tc.desc: Add one no immersive window
 * @tc.type: FUNC
 * @tc.require: AR000GGTVD
 */
HWTEST_F(WindowImmersiveTest, ImmersiveTest03, Function | MediumTest | Level3)
{
    const sptr<Window>& window1 = utils::CreateTestWindow(fullScreenAppinfo_);
    activeWindows_.push_back(window1);
    SetWindowSystemProps(window1, TEST_PROPS_1);
    fullScreenAppinfo_.name = "main2";
    fullScreenAppinfo_.needAvoid = true; // no immersive setting
    const sptr<Window>& window2 = utils::CreateTestWindow(fullScreenAppinfo_);
    activeWindows_.push_back(window2);
    SetWindowSystemProps(window2, TEST_PROPS_2);
    ASSERT_EQ(WMError::WM_OK, window1->Show());
    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_PROPS_1));
    ASSERT_EQ(WMError::WM_OK, window2->Show());
    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_PROPS_1));
    ASSERT_EQ(WMError::WM_OK, window1->Hide());
    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_PROPS_DEFAULT));
}

/**
 * @tc.name: ImmersiveTest04
 * @tc.desc: SetLayoutFullScreen
 * @tc.type: FUNC
 * @tc.require: AR000GGTVD
 */
HWTEST_F(WindowImmersiveTest, ImmersiveTest04, Function | MediumTest | Level3)
{
    fullScreenAppinfo_.needAvoid = true; // no immersive setting
    const sptr<Window>& window1 = utils::CreateTestWindow(fullScreenAppinfo_);
    activeWindows_.push_back(window1);
    SetWindowSystemProps(window1, TEST_PROPS_1);
    ASSERT_EQ(WMError::WM_OK, window1->Show());
    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_PROPS_DEFAULT));
    ASSERT_EQ(WMError::WM_OK, window1->SetLayoutFullScreen(true));
    ASSERT_EQ(true, window1->IsLayoutFullScreen());
    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_PROPS_1));
    ASSERT_EQ(WMError::WM_OK, window1->SetLayoutFullScreen(false));
    ASSERT_EQ(false, window1->IsLayoutFullScreen());
    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_PROPS_DEFAULT));
    ASSERT_EQ(WMError::WM_OK, window1->SetLayoutFullScreen(true));
    ASSERT_EQ(true, window1->IsLayoutFullScreen());
    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_PROPS_1));
    ASSERT_EQ(WMError::WM_OK, window1->SetLayoutFullScreen(false));
    ASSERT_EQ(false, window1->IsLayoutFullScreen());
    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_PROPS_DEFAULT));
    ASSERT_EQ(WMError::WM_OK, window1->Hide());
    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_PROPS_DEFAULT));
}

/**
 * @tc.name: ImmersiveTest05
 * @tc.desc: SetFullScreen
 * @tc.type: FUNC
 * @tc.require: AR000GGTVD
 */
HWTEST_F(WindowImmersiveTest, ImmersiveTest05, Function | MediumTest | Level3)
{
    fullScreenAppinfo_.needAvoid = true; // no immersive setting
    const sptr<Window>& window1 = utils::CreateTestWindow(fullScreenAppinfo_);
    activeWindows_.push_back(window1);
    ASSERT_EQ(WMError::WM_OK, window1->Show());
    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_PROPS_DEFAULT));
    ASSERT_EQ(WMError::WM_OK, window1->SetFullScreen(true));
    ASSERT_EQ(true, window1->IsFullScreen());
    ASSERT_EQ(WMError::WM_OK, window1->SetFullScreen(false));
    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_PROPS_DEFAULT));
    ASSERT_EQ(false, window1->IsFullScreen());
    ASSERT_EQ(WMError::WM_OK, window1->Hide());
    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_PROPS_DEFAULT));
}

/**
 * @tc.name: GetAvoidAreaByTypeTest01
 * @tc.desc: Test GetAvoidArea use unsupport Type(TYPE_CUTOUT).
 * @tc.type: FUNC
 * @tc.require: AR000GGTVD
 */
HWTEST_F(WindowImmersiveTest, GetAvoidAreaByTypeTest01, Function | MediumTest | Level3)
{
    // Add full screenwindow for call GetAvoidArea, and push_back in activeWindows_
    const sptr<Window>& win = utils::CreateTestWindow(fullScreenAppinfo_);
    activeWindows_.push_back(win);

    // Test GetAvoidArea
    AvoidArea avoidarea;
    WMError ret = win->GetAvoidAreaByType(AvoidAreaType::TYPE_CUTOUT, avoidarea);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_TRUE(utils::RectEqualToRect(EMPTY_RECT, avoidarea.leftRect));
    ASSERT_TRUE(utils::RectEqualToRect(EMPTY_RECT, avoidarea.rightRect));
    ASSERT_TRUE(utils::RectEqualToRect(EMPTY_RECT, avoidarea.topRect));
    ASSERT_TRUE(utils::RectEqualToRect(EMPTY_RECT, avoidarea.bottomRect));
}

/**
 * @tc.name: GetAvoidAreaByTypeTest02
 * @tc.desc: Add SystemBar left avoid. And Test GetAvoidArea.
 * @tc.type: FUNC
 * @tc.require: AR000GGTVD
 */
HWTEST_F(WindowImmersiveTest, GetAvoidAreaByTypeTest02, Function | MediumTest | Level3)
{
    // Add full screenwindow for call GetAvoidArea, and push_back in activeWindows_
    const sptr<Window>& win = utils::CreateTestWindow(fullScreenAppinfo_);
    activeWindows_.push_back(win);

    // Add a unexist leftAvoid
    avoidBarInfo_.rect = {0, 0, leftAvoidW_, leftAvoidH_};
    const sptr<Window>& left = utils::CreateTestWindow(avoidBarInfo_);
    activeWindows_.push_back(left);
    ASSERT_EQ(WMError::WM_OK, left->Show());
    ASSERT_EQ(WMError::WM_OK, left->Resize(leftAvoidW_, leftAvoidH_));

    // Test GetAvoidArea
    AvoidArea avoidarea;
    WMError ret = win->GetAvoidAreaByType(AvoidAreaType::TYPE_SYSTEM, avoidarea);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_TRUE(utils::RectEqualTo(left, avoidarea.leftRect));
}

/**
 * @tc.name: GetAvoidAreaByTypeTest03
 * @tc.desc: Add SystemBar top avoid. And Test GetAvoidArea.
 * @tc.type: FUNC
 * @tc.require: AR000GGTVD
 */
HWTEST_F(WindowImmersiveTest, GetAvoidAreaByTypeTest03, Function | MediumTest | Level3)
{
    // Add full screenwindow for call GetAvoidArea, and push_back in activeWindows_
    const sptr<Window>& win = utils::CreateTestWindow(fullScreenAppinfo_);
    activeWindows_.push_back(win);

    // Add a unexist topAvoid
    avoidBarInfo_.name = "TopAvoidTest";
    avoidBarInfo_.rect = {0, 0, topAvoidW_, topAvoidH_};
    const sptr<Window>& top = utils::CreateTestWindow(avoidBarInfo_);
    activeWindows_.push_back(top);
    ASSERT_EQ(WMError::WM_OK, top->Show());
    ASSERT_EQ(WMError::WM_OK, top->Resize(topAvoidW_, topAvoidH_));

    // Tesr GetAvoidArea
    AvoidArea avoidarea;
    WMError ret = win->GetAvoidAreaByType(AvoidAreaType::TYPE_SYSTEM, avoidarea);
    ASSERT_EQ(WMError::WM_OK, ret);
    ASSERT_TRUE(utils::RectEqualTo(top, avoidarea.topRect));
}

/**
 * @tc.name: OnAvoidAreaChangedTest01
 * @tc.desc: Add unexistavoid and Update this avoid. Test OnAvoidAreaChanged listener
 * @tc.type: FUNC
 * @tc.require: AR000GGTVD
 */
HWTEST_F(WindowImmersiveTest, OnAvoidAreaChangedTest01, Function | MediumTest | Level3)
{
    // Add full screenwindow for RegisterAvoidAreaChangeListener
    const sptr<Window>& window = utils::CreateTestWindow(fullScreenAppinfo_);
    sptr<IAvoidAreaChangedListener> thisListener(testAvoidAreaChangedListener_);
    window->RegisterAvoidAreaChangeListener(thisListener);
    activeWindows_.push_back(window);
    ASSERT_EQ(WMError::WM_OK, window->Show());

    // Add a unexist topAvoid
    avoidBarInfo_.name = "TopAvoidTest";
    avoidBarInfo_.rect = {0, 0, topAvoidW_, topAvoidH_};
    const sptr<Window>& top = utils::CreateTestWindow(avoidBarInfo_);
    activeWindows_.push_back(top);
    ASSERT_EQ(WMError::WM_OK, top->Show());
    ASSERT_EQ(WMError::WM_OK, top->Resize(topAvoidW_, topAvoidH_));

    // Await 100ms and get callback result in listener. Compare current avoidArea
    usleep(WAIT_ASYNC_US);
    std::vector<Rect> avoidArea = testAvoidAreaChangedListener_->avoidAreas_;
    ASSERT_EQ(4u, static_cast<uint32_t>(avoidArea.size()));      // 4: avoidAreaNum(left, top, right, bottom)
    ASSERT_TRUE(utils::RectEqualToRect(avoidBarInfo_.rect, avoidArea[1]));  // 1: left Rect

    // Update topavoid. Enlarge topAvoidH_
    uint32_t bigHeight = std::min(static_cast<uint32_t>(utils::displayRect_.height_),
        static_cast<uint32_t>(utils::displayRect_.width_ * 0.5));  // 0.5 : just use bigger height for update
    Rect bigTopRect = {0, 0, topAvoidW_, bigHeight};
    ASSERT_EQ(WMError::WM_OK, top->Resize(topAvoidW_, bigHeight));

    // Await 100ms and get callback result in listener. Compare current avoidArea
    usleep(WAIT_ASYNC_US);
    std::vector<Rect> avoidArea2 = testAvoidAreaChangedListener_->avoidAreas_;
    ASSERT_TRUE(utils::RectEqualToRect(bigTopRect, avoidArea2[1]));

    window->UnregisterAvoidAreaChangeListener();
}

/**
 * @tc.name: OnAvoidAreaChangedTest02
 * @tc.desc: Add unexistavoid and remove this avoid. Test OnAvoidAreaChanged listener
 * @tc.type: FUNC
 * @tc.require: AR000GGTVD
 */
HWTEST_F(WindowImmersiveTest, OnAvoidAreaChangedTest02, Function | MediumTest | Level3)
{
    // Add full screenwindow for call UpdateAvoidChange
    const sptr<Window>& window = utils::CreateTestWindow(fullScreenAppinfo_);
    sptr<IAvoidAreaChangedListener> thisListener(testAvoidAreaChangedListener_);
    window->RegisterAvoidAreaChangeListener(thisListener);
    activeWindows_.push_back(window);
    ASSERT_EQ(WMError::WM_OK, window->Show());

    // Add a unexist leftAvoid
    avoidBarInfo_.rect = {0, 0, leftAvoidW_, leftAvoidH_};
    const sptr<Window>& left = utils::CreateTestWindow(avoidBarInfo_);
    activeWindows_.push_back(left);
    ASSERT_EQ(WMError::WM_OK, left->Show());
    ASSERT_EQ(WMError::WM_OK, left->Resize(leftAvoidW_, leftAvoidH_));

    // Await 100ms and get callback result in listener. Compare current avoidArea
    usleep(WAIT_ASYNC_US);
    std::vector<Rect> avoidArea = testAvoidAreaChangedListener_->avoidAreas_;
    ASSERT_EQ(4u, static_cast<uint32_t>(avoidArea.size()));        // 4: avoidAreaNum(left, top, right, bottom)
    ASSERT_TRUE(utils::RectEqualToRect(avoidBarInfo_.rect, avoidArea[0]));   // 0: left Rect

    // Remove left avoid.
    ASSERT_EQ(WMError::WM_OK, left->Hide());

    // Await 100ms and get callback result in listener. Compare current avoidArea
    usleep(WAIT_ASYNC_US);
    std::vector<Rect> avoidArea2 = testAvoidAreaChangedListener_->avoidAreas_;
    ASSERT_TRUE(utils::RectEqualToRect(EMPTY_RECT, avoidArea2[0])); // 0: left Rect

    window->UnregisterAvoidAreaChangeListener();
}
}
} // namespace Rosen
} // namespace OHOS
