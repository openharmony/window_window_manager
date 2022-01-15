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
    const SystemBarProperty SYS_BAR_PROP_DEFAULT;
    const SystemBarProperty SYS_BAR_PROP_1(true, 0xE5111111, 0xE5222222);
    const SystemBarProperty SYS_BAR_PROP_2(false, 0xE5222222, 0xE5333333);
    const SystemBarProperty SYS_BAR_PROP_3(false, 0xE5333333, 0xE5444444);
    const SystemBarProperty SYS_BAR_PROP_4(true, 0xE5444444, 0x66555555);
    const SystemBarProps TEST_PROPS_DEFAULT = {
        { WindowType::WINDOW_TYPE_STATUS_BAR, SYS_BAR_PROP_DEFAULT },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SYS_BAR_PROP_DEFAULT },
    };
    const SystemBarProps TEST_PROPS_1 = {
        { WindowType::WINDOW_TYPE_STATUS_BAR, SYS_BAR_PROP_1 },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SYS_BAR_PROP_2 },
    };
    const SystemBarProps TEST_PROPS_2 = {
        { WindowType::WINDOW_TYPE_STATUS_BAR, SYS_BAR_PROP_1 },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SYS_BAR_PROP_3 },
    };
    const SystemBarProps TEST_DIFF_PROPS_1_2 = {
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SYS_BAR_PROP_3 },
    };
    const SystemBarProps TEST_DIFF_PROPS_2_1 = {
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SYS_BAR_PROP_2 },
    };
}
using utils = WindowTestUtils;
class TestSystemBarChangedListener : public ISystemBarChangedListener {
public:
    SystemBarProps props_;
    void OnSystemBarPropertyChange(uint64_t displayId, const SystemBarProps& props) override;
};

class WindowImmersiveTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    void SetWindowSystemProps(const sptr<Window>& window, const SystemBarProps& props);
    bool SystemBarPropsEqualsTo(const SystemBarProps& expect);
    void DumpFailedInfo(const SystemBarProps& expect);
    int displayId_ = 0;
    std::vector<sptr<Window>> activeWindows_;
    static vector<Rect> fullScreenExpecteds_;
    static sptr<TestSystemBarChangedListener> testSystemBarChangedListener_;
};

vector<Rect> WindowImmersiveTest::fullScreenExpecteds_;
sptr<TestSystemBarChangedListener> WindowImmersiveTest::testSystemBarChangedListener_ =
    new TestSystemBarChangedListener();

void WindowImmersiveTest::SetWindowSystemProps(const sptr<Window>& window, const SystemBarProps& props)
{
    for (auto prop : props) {
        window->SetSystemBarProperty(prop.first, prop.second);
    }
}

void WindowImmersiveTest::DumpFailedInfo(const SystemBarProps& expect)
{
    auto act = testSystemBarChangedListener_->props_;
    printf("WindowImmersiveTest Expected: \n");
    for (auto prop : expect) {
        printf("WindowType: %4d, Enable: %4d, Color: %x | %x\n", static_cast<uint32_t>(prop.first),
            prop.second.enable_, prop.second.backgroundColor_, prop.second.contentColor_);
    }
    printf("WindowImmersiveTest Act: \n");
    for (auto prop : act) {
        printf("WindowType: %4d, Enable: %4d, Color: %x | %x\n", static_cast<uint32_t>(prop.first),
            prop.second.enable_, prop.second.backgroundColor_, prop.second.contentColor_);
    }
}

bool WindowImmersiveTest::SystemBarPropsEqualsTo(const SystemBarProps& expect)
{
    auto act = testSystemBarChangedListener_->props_;
    if (act.size() != expect.size()) {
        DumpFailedInfo(expect);
        return false;
    }
    for (auto item : expect) {
        if (std::find(act.begin(), act.end(), item) == act.end()) {
            DumpFailedInfo(expect);
            return false;
        }
    }
    return true;
}

void TestSystemBarChangedListener::OnSystemBarPropertyChange(uint64_t displayId, const SystemBarProps& props)
{
    printf("TestSystemBarChangedListener Display ID: %llu\n", displayId);
    props_ = props;
}

void WindowImmersiveTest::SetUpTestCase()
{
}

void WindowImmersiveTest::TearDownTestCase()
{
}

void WindowImmersiveTest::SetUp()
{
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
    utils::TestWindowInfo info = {
        .name = "main",
        .rect = utils::defaultAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN, // immersive setting
        .needAvoid = false, // immersive setting
        .parentLimit = false,
        .parentName = "",
    };
    const sptr<Window>& window = utils::CreateTestWindow(info);
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
    utils::TestWindowInfo info = {
        .name = "main",
        .rect = utils::defaultAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN, // immersive setting
        .needAvoid = false, // immersive setting
        .parentLimit = false,
        .parentName = "",
    };
    const sptr<Window>& window1 = utils::CreateTestWindow(info);
    activeWindows_.push_back(window1);
    SetWindowSystemProps(window1, TEST_PROPS_1);
    info.name = "main2";
    const sptr<Window>& window2 = utils::CreateTestWindow(info);
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
    utils::TestWindowInfo info = {
        .name = "main",
        .rect = utils::defaultAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN, // immersive setting
        .needAvoid = false, // immersive setting
        .parentLimit = false,
        .parentName = "",
    };
    const sptr<Window>& window1 = utils::CreateTestWindow(info);
    activeWindows_.push_back(window1);
    SetWindowSystemProps(window1, TEST_PROPS_1);
    info.name = "main2";
    info.needAvoid = true; // no immersive setting
    const sptr<Window>& window2 = utils::CreateTestWindow(info);
    activeWindows_.push_back(window2);
    SetWindowSystemProps(window2, TEST_PROPS_2);
    ASSERT_EQ(WMError::WM_OK, window1->Show());
    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_PROPS_1));
    ASSERT_EQ(WMError::WM_OK, window2->Show());
    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_PROPS_1));
    ASSERT_EQ(WMError::WM_OK, window1->Hide());
    ASSERT_TRUE(SystemBarPropsEqualsTo(TEST_PROPS_DEFAULT));
}
}
} // namespace Rosen
} // namespace OHOS
