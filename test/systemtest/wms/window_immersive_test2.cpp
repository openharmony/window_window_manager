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
#include "window_option.h"
#include <gtest/gtest.h>
#include "window_manager.h"
#include "window_test_utils.h"
#include "wm_common.h"
#include "window_session_impl.h"
#include "window_scene_session_impl.h"
#include "mock_session.h"
#include "js_window.h"
#include "js_window_utils.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowImmersiveTest1"};

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

class WindowImmersiveTest1 : public testing::Test {
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

vector<Rect> WindowImmersiveTest1::fullScreenExpecteds_;
sptr<TestSystemBarChangedListener> WindowImmersiveTest1::testSystemBarChangedListener_ =
    new TestSystemBarChangedListener();
sptr<TestAvoidAreaChangedListener> WindowImmersiveTest1::testAvoidAreaChangedListener_ =
    new TestAvoidAreaChangedListener();

void WindowImmersiveTest1::SetWindowSystemProps(const sptr<Window>& window, const SystemBarRegionTints& tints)
{
    for (auto tint : tints) {
        window->SetSystemBarProperty(tint.type_, tint.prop_);
    }
}

void WindowImmersiveTest1::DumpFailedInfo(const SystemBarRegionTints& expect)
{
    auto act = testSystemBarChangedListener_->tints_;
    WLOGI("WindowImmersiveTest1 Expected:");
    for (auto tint : expect) {
        WLOGI("WindowType: %{public}4d, Enable: %{public}4d, Color: %{public}x | %{public}x",
            static_cast<uint32_t>(tint.type_), tint.prop_.enable_,
            tint.prop_.backgroundColor_, tint.prop_.contentColor_);
    }
    WLOGI("WindowImmersiveTest1 Act: ");
    for (auto tint : act) {
        WLOGI("WindowType: %{public}4d, Enable: %{public}4d, Color: %{public}x | %{public}x",
            static_cast<uint32_t>(tint.type_), tint.prop_.enable_,
            tint.prop_.backgroundColor_, tint.prop_.contentColor_);
    }
}

void WindowImmersiveTest1::DumpFailedInfo(bool expectStatus, bool expectNav)
{
    auto act = testSystemBarChangedListener_->tints_;
    WLOGI("WindowImmersiveTest1 Expected:");
    WLOGI("expectStatus: %{public}4d, expectNav: %{public}4d", expectStatus, expectNav);
    WLOGI("WindowImmersiveTest1 Act: ");
    for (auto tint : act) {
        WLOGI("WindowType: %{public}4d, Enable: %{public}4d, Color: %{public}x | %{public}x",
            static_cast<uint32_t>(tint.type_), tint.prop_.enable_,
            tint.prop_.backgroundColor_, tint.prop_.contentColor_);
    }
}

bool WindowImmersiveTest1::SystemBarPropsEqualsTo(const SystemBarRegionTints& expect)
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

bool WindowImmersiveTest1::SystemBarEnableState(bool expectStatus, bool expectNav)
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
        for (uint32_t i = 0; i < tints_.size(); i) {
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

void WindowImmersiveTest1::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    ASSERT_TRUE((display != nullptr));
    WLOGI("GetDefaultDisplay: id %{public}" PRIu64", w %{public}d, h %{public}d, fps %{public}u",
        display->GetId(), display->GetWidth(), display->GetHeight(), display->GetRefreshRate());
    Rect displayRect = {0, 0, display->GetWidth(), display->GetHeight()};
    Utils::InitByDisplayRect(displayRect);
}

void WindowImmersiveTest1::TearDownTestCase()
{

}

void WindowImmersiveTest1::SetUp()
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

void WindowImmersiveTest1::TearDown()
{
    while (!activeWindows_.empty()) {
        EXPECT_EQ(WMError::WM_OK, activeWindows_.back()->Destroy());
        activeWindows_.pop_back();
    }
    WindowManager::GetInstance().UnregisterSystemBarChangedListener(testSystemBarChangedListener_);
    sleep(1);
}

namespace {

static void UpdateSystemBarProperties(std::map<WindowType, SystemBarProperty>& systemBarProperties,
    const std::map<WindowType, SystemBarPropertyFlag>& systemBarPropertyFlags, sptr<WindowSceneSessionImpl> windowToken)
{
    for (auto it : systemBarPropertyFlags) {
        WindowType type = it.first;
        SystemBarPropertyFlag flag = it.second;
        auto property = windowToken->GetSystemBarPropertyByType(type);
        if (flag.enableFlag == false) {
            systemBarProperties[type].enable_ = property.enable_;
        }
        if (flag.backgroundColorFlag == false) {
            systemBarProperties[type].backgroundColor_ = property.backgroundColor_;
        }
        if (flag.contentColorFlag == false) {
            systemBarProperties[type].contentColor_ = property.contentColor_;
        }
        if (flag.enableAnimationFlag == false) {
            systemBarProperties[type].enableAnimation_ = property.enableAnimation_;
        }
        if (flag.enableFlag == true) {
            systemBarProperties[type].settingFlag_ =
                static_cast<SystemBarSettingFlag>(static_cast<uint32_t>(property.settingFlag_) |
                static_cast<uint32_t>(SystemBarSettingFlag::ENABLE_SETTING));
        }
        if (flag.backgroundColorFlag == true || flag.contentColorFlag == true) {
            systemBarProperties[type].settingFlag_ =
                static_cast<SystemBarSettingFlag>(static_cast<uint32_t>(property.settingFlag_) |
                static_cast<uint32_t>(SystemBarSettingFlag::COLOR_SETTING));
        }
    }
    return;
}

static WMError SetSystemBarPropertiesByFlags(std::map<WindowType, SystemBarPropertyFlag>& systemBarPropertyFlags,
    std::map<WindowType, SystemBarProperty>& systemBarProperties, sptr<WindowSceneSessionImpl> windowToken)
{
    WMError ret = WMError::WM_OK;
    WMError err = WMError::WM_OK;

    for (auto it : systemBarPropertyFlags) {
        WindowType type = it.first;
        SystemBarPropertyFlag flag = it.second;
        if (flag.enableFlag || flag.backgroundColorFlag || flag.contentColorFlag || flag.enableAnimationFlag) {
            err = windowToken->SetSystemBarProperty(type, systemBarProperties.at(type));
            if (err != WMError::WM_OK) {
                TLOGE(WmsLogTag::WMS_IMMS, "SetSystemBarProperty failed, ret = %{public}d", err);
                ret = err;
            }
        }
    }
    return ret;
}

void GetSystemBarStatus(std::map<WindowType, SystemBarProperty>& systemBarProperties, 
                        std::map<WindowType, SystemBarPropertyFlag>& systemBarPropertyFlags, WindowType type)
{
    systemBarProperties[WindowType::WINDOW_TYPE_STATUS_BAR].enable_ = false;
    systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR].enable_ = false;
    systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].enable_ = false;
    systemBarPropertyFlags[WindowType::WINDOW_TYPE_STATUS_BAR] = {true, true, true, true};
    systemBarPropertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR] = {true, true, true, true};
    systemBarPropertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_BAR] = {true, true, true, true};

    if (type == WindowType::WINDOW_TYPE_STATUS_BAR) {
        systemBarProperties[WindowType::WINDOW_TYPE_STATUS_BAR].enable_ = true;
        systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR].enable_ = true;
    } else if (type == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
        systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].enable_ = true;
    }
}

void GetSystemBarStatus(std::map<WindowType, SystemBarProperty>& systemBarProperties, SystemBarProperty pro,
                        std::map<WindowType, SystemBarPropertyFlag>& systemBarPropertyFlags, WindowType type)
{
    systemBarPropertyFlags[WindowType::WINDOW_TYPE_STATUS_BAR] = {true, true, true, true};
    systemBarPropertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR] = {true, true, true, true};
    systemBarPropertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_BAR] = {true, true, true, true};

    if (type == WindowType::WINDOW_TYPE_STATUS_BAR) {
        systemBarProperties[type] = pro;
        systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR] = pro;
    } else if (type == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
        systemBarProperties[type] = pro;
    }
}

void GetSpecificBarStatus(std::map<WindowType, SystemBarProperty>& systemBarProperties, 
                        std::map<WindowType, SystemBarPropertyFlag>& systemBarPropertyFlags, WindowType type)
{
    systemBarProperties[WindowType::WINDOW_TYPE_STATUS_BAR].enable_ = false;
    systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR].enable_ = false;
    systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].enable_ = false;
    systemBarPropertyFlags[WindowType::WINDOW_TYPE_STATUS_BAR] = {true, true, true, true};
    systemBarPropertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR] = {true, true, true, true};
    systemBarPropertyFlags[WindowType::WINDOW_TYPE_NAVIGATION_BAR] = {true, true, true, true};
    if (type == WindowType::WINDOW_TYPE_STATUS_BAR) {
        systemBarProperties[WindowType::WINDOW_TYPE_STATUS_BAR].enable_ = true;
    } else if (type == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
        systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].enable_ = true;
    } else if (type == WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR) {
        systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR].enable_ = true;
    }
}

/**
 * @tc.name: SetFullScreen
 * @tc.desc: SetFullScreen
 * @tc.type: FUNC
 */
HWTEST_F(WindowImmersiveTest1, setFullScreen, Function | MediumTest | Level3)
{
    const vector<WindowMode>windowMode{WindowMode::WINDOW_MODE_FULLSCREEN, WindowMode::WINDOW_MODE_SPLIT_PRIMARY,
         WindowMode::WINDOW_MODE_SPLIT_SECONDARY, WindowMode::WINDOW_MODE_FLOATING};

    for (auto mode : windowMode) {
        sptr<WindowOption> option = new (std::nothrow) WindowOption();
        ASSERT_NE(nullptr, option);
        option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
        option->SetWindowMode(mode);

        sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
        EXPECT_FALSE(window == nullptr);

        SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
        sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
        ASSERT_NE(nullptr, session);
        window->property_->SetPersistentId(1);
        window->hostSession_ = session;
        window->state_ = WindowState::STATE_SHOWN;
        activeWindows_.push_back(window);
        EXPECT_EQ(WMError::WM_OK, window->SetFullScreen(true));
    
        if (mode != WindowMode::WINDOW_MODE_FULLSCREEN) {
            EXPECT_EQ(WMError::WM_OK, window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
        }
        EXPECT_EQ(true, window->IsFullScreen());

        window->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);

        if (mode == WindowMode::WINDOW_MODE_FULLSCREEN) {
            EXPECT_EQ(WMError::WM_OK, window->SetFullScreen(false));
            EXPECT_EQ(false, window->IsFullScreen());
        } else {
            EXPECT_EQ(WMError::WM_OK, window->SetFullScreen(false));
            EXPECT_EQ(WMError::WM_OK, window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
            EXPECT_EQ(false, window->IsFullScreen());
        }

        window->SetWindowType(WindowType::WINDOW_TYPE_DESKTOP);
        window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
        EXPECT_EQ(WMError::WM_OK, window->SetFullScreen(false));
        EXPECT_EQ(false, window->IsFullScreen());
    }
}

/**
 * @tc.name: SetLayoutFullScreen
 * @tc.desc: SetLayoutFullScreen 
 * @tc.type: FUNC
 */
HWTEST_F(WindowImmersiveTest1, setLayoutFullScreen, Function | MediumTest | Level3)
{
    const vector<WindowMode>windowMode{WindowMode::WINDOW_MODE_FULLSCREEN, WindowMode::WINDOW_MODE_SPLIT_PRIMARY,
         WindowMode::WINDOW_MODE_SPLIT_SECONDARY, WindowMode::WINDOW_MODE_FLOATING};

    for (auto mode : windowMode) {
        sptr<WindowOption> option = new (std::nothrow) WindowOption();
        ASSERT_NE(nullptr, option);
        option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
        option->SetWindowMode(mode);

        sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
        EXPECT_FALSE(window == nullptr);

        SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
        sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
        ASSERT_NE(nullptr, session);
        window->property_->SetPersistentId(1);
        window->hostSession_ = session;
        window->state_ = WindowState::STATE_SHOWN;
        activeWindows_.push_back(window);
        EXPECT_EQ(WMError::WM_OK, window->SetLayoutFullScreen(true));
    
        if (mode != WindowMode::WINDOW_MODE_FULLSCREEN) {
            EXPECT_EQ(WMError::WM_OK, window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
        }
        EXPECT_EQ(true, window->IsLayoutFullScreen());

        window->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
        if (mode == WindowMode::WINDOW_MODE_FULLSCREEN) {
            EXPECT_EQ(WMError::WM_OK, window->SetLayoutFullScreen(false));
            EXPECT_EQ(false, window->IsLayoutFullScreen());
        } else {
            EXPECT_EQ(WMError::WM_OK, window->SetLayoutFullScreen(false));
            EXPECT_EQ(WMError::WM_OK, window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));
            EXPECT_EQ(false, window->IsLayoutFullScreen());
        }

        window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
        window->SetWindowType(WindowType::WINDOW_TYPE_DESKTOP);
        EXPECT_EQ(WMError::WM_OK, window->SetLayoutFullScreen(false));
        EXPECT_EQ(false, window->IsLayoutFullScreen());
    }
}

/**
 * @tc.name: SetImmersiveModeEnabledState
 * @tc.desc: SetImmersiveModeEnabledState
 * @tc.type: FUNC
 */
HWTEST_F(WindowImmersiveTest1, setImmersiveModeEnabledState, Function | MediumTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->SetImmersiveModeEnabledState(false));

    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->property_->SetWindowName("SetImmersiveModeEnabledState");
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    window->state_ = WindowState::STATE_CREATED;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetImmersiveModeEnabledState(false));

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    ASSERT_EQ(WMError::WM_OK, window->SetImmersiveModeEnabledState(true));
    ASSERT_EQ(true, window->GetImmersiveModeEnabledState());
    ASSERT_EQ(WMError::WM_OK, window->SetImmersiveModeEnabledState(false));
    ASSERT_EQ(false, window->GetImmersiveModeEnabledState());

    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    ASSERT_EQ(WMError::WM_OK, window->SetLayoutFullScreen(true));
    ASSERT_EQ(true, window->IsLayoutFullScreen());
    ASSERT_EQ(WMError::WM_OK, window->SetLayoutFullScreen(false));
    ASSERT_EQ(false, window->IsLayoutFullScreen());
    ASSERT_EQ(WMError::WM_OK, window->SetImmersiveModeEnabledState(true));
    ASSERT_EQ(true, window->IsLayoutFullScreen());
    ASSERT_EQ(WMError::WM_OK, window->SetImmersiveModeEnabledState(false));
    ASSERT_EQ(false, window->IsLayoutFullScreen());

    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    ASSERT_EQ(WMError::WM_OK, window->SetImmersiveModeEnabledState(true));
    ASSERT_EQ(true, window->IsLayoutFullScreen());
    ASSERT_EQ(true, window->GetImmersiveModeEnabledState());

    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    ASSERT_EQ(WMError::WM_OK, window->SetImmersiveModeEnabledState(true));
    ASSERT_EQ(true, window->IsLayoutFullScreen());
    ASSERT_EQ(true, window->GetImmersiveModeEnabledState());

    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    ASSERT_EQ(WMError::WM_OK, window->SetImmersiveModeEnabledState(true));
    ASSERT_EQ(true, window->IsLayoutFullScreen());
    ASSERT_EQ(true, window->GetImmersiveModeEnabledState());
    ASSERT_EQ(true, window->IsLayoutFullScreen());

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_DESKTOP);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetImmersiveModeEnabledState(true));
    ASSERT_EQ(true, window->GetImmersiveModeEnabledState());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetImmersiveModeEnabledState(false));
    ASSERT_EQ(true, window->GetImmersiveModeEnabledState());
}

/**
 * @tc.name: SetWindowSystemBarEnable
 * @tc.desc: SetWindowSystemBarEnable WINDOW_TYPE_APP_MAIN_WINDOW
 * @tc.type: FUNC
 */
HWTEST_F(WindowImmersiveTest1, setWindowSystemBarEnable, Function | MediumTest | Level3)
{
    const vector<WindowMode>windowMode{WindowMode::WINDOW_MODE_FULLSCREEN, WindowMode::WINDOW_MODE_SPLIT_PRIMARY,
         WindowMode::WINDOW_MODE_SPLIT_SECONDARY, WindowMode::WINDOW_MODE_FLOATING};
    const vector<WindowType>windowType{WindowType::WINDOW_TYPE_STATUS_BAR, WindowType::WINDOW_TYPE_NAVIGATION_BAR};

    for (auto type : windowType) {
        for (auto mode : windowMode) {
            sptr<WindowOption> option = new (std::nothrow) WindowOption();
            ASSERT_NE(nullptr, option);
            option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
            option->SetWindowMode(mode);

            sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
            EXPECT_FALSE(window == nullptr);

            SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
            sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
            ASSERT_NE(nullptr, session);
            window->property_->SetPersistentId(1);
            window->hostSession_ = session;
            window->state_ = WindowState::STATE_SHOWN;
            activeWindows_.push_back(window);

            std::map<WindowType, SystemBarProperty> systemBarProperties;
            std::map<WindowType, SystemBarPropertyFlag> systemBarPropertyFlags;

            GetSystemBarStatus(systemBarProperties, systemBarPropertyFlags, type);
            UpdateSystemBarProperties(systemBarProperties, systemBarPropertyFlags, window);
            WMError ret = SetSystemBarPropertiesByFlags(systemBarPropertyFlags, systemBarProperties, window);
            EXPECT_EQ(WMError::WM_OK, ret);
            sleep(1);
            
            if (type == WindowType::WINDOW_TYPE_STATUS_BAR) {
                auto sta = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
                auto nav = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR);
                EXPECT_EQ(true, sta.enable_);
                EXPECT_EQ(true, nav.enable_);
            } else if (type == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
                auto nav = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_BAR);
                EXPECT_EQ(true, nav.enable_);
            }

            GetSystemBarStatus(systemBarProperties, systemBarPropertyFlags, WindowType::APP_WINDOW_BASE);
            UpdateSystemBarProperties(systemBarProperties, systemBarPropertyFlags, window);
            ret = SetSystemBarPropertiesByFlags(systemBarPropertyFlags, systemBarProperties, window);
            EXPECT_EQ(WMError::WM_OK, ret);
            sleep(1);
            auto sta = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
            auto nav = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR);
            EXPECT_EQ(false, sta.enable_);
            EXPECT_EQ(false, nav.enable_);

            window->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);

            GetSystemBarStatus(systemBarProperties, systemBarPropertyFlags, type);
            UpdateSystemBarProperties(systemBarProperties, systemBarPropertyFlags, window);
            ret = SetSystemBarPropertiesByFlags(systemBarPropertyFlags, systemBarProperties, window);
            EXPECT_EQ(WMError::WM_OK, ret);
            sleep(1);
            sta = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
            nav = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR);
            EXPECT_EQ(false, sta.enable_);
            EXPECT_EQ(false, nav.enable_);
        }
    }
}

/**
 * @tc.name: SetSpecificBarProperty
 * @tc.desc: SetSpecificBarProperty
 * @tc.type: FUNC
 */
HWTEST_F(WindowImmersiveTest1, setSpecificBarProperty, Function | MediumTest | Level3)
{
    const vector<WindowMode>windowMode{WindowMode::WINDOW_MODE_FULLSCREEN, WindowMode::WINDOW_MODE_SPLIT_PRIMARY,
            WindowMode::WINDOW_MODE_SPLIT_SECONDARY, WindowMode::WINDOW_MODE_FLOATING};
    const vector<WindowType>windowType{WindowType::WINDOW_TYPE_STATUS_BAR, WindowType::WINDOW_TYPE_NAVIGATION_BAR,
            WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR};
    for (auto mode : windowMode) {
        for (auto type : windowType) {
            sptr<WindowOption> option = new (std::nothrow) WindowOption();
            ASSERT_NE(nullptr, option);
            option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
            option->SetWindowMode(mode);

            sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
            EXPECT_FALSE(window == nullptr);

            SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
            sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
            ASSERT_NE(nullptr, session);
            window->property_->SetPersistentId(1);
            window->hostSession_ = session;
            window->state_ = WindowState::STATE_SHOWN;
            activeWindows_.push_back(window);

            std::map<WindowType, SystemBarProperty> systemBarProperties;
            std::map<WindowType, SystemBarPropertyFlag> systemBarPropertyFlags;
            GetSpecificBarStatus(systemBarProperties, systemBarPropertyFlags, type);
            WMError ret = window->SetSpecificBarProperty(type, systemBarProperties[type]);
            EXPECT_EQ(WMError::WM_OK, ret);
            auto status = window->GetSystemBarPropertyByType(type);
            EXPECT_EQ(true, status.enable_);

            GetSpecificBarStatus(systemBarProperties, systemBarPropertyFlags, WindowType::APP_WINDOW_BASE);
            ret = window->SetSpecificBarProperty(type, systemBarProperties[type]);
            EXPECT_EQ(WMError::WM_OK, ret);
            status = window->GetSystemBarPropertyByType(type);
            EXPECT_EQ(false, status.enable_);

            window->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);

            GetSpecificBarStatus(systemBarProperties, systemBarPropertyFlags, WindowType::APP_WINDOW_BASE);
            ret = window->SetSpecificBarProperty(type, systemBarProperties[type]);
            EXPECT_EQ(WMError::WM_OK, ret);
            status = window->GetSystemBarPropertyByType(type);
            EXPECT_EQ(false, status.enable_);
        }
    }
}

/**
 * @tc.name: SetWindowSystemBarProperties
 * @tc.desc: SetWindowSystemBarProperties 
 * @tc.type: FUNC
 */
HWTEST_F(WindowImmersiveTest1, setWindowSystemBarProperties, Function | MediumTest | Level3)
{
    const vector<WindowMode>windowMode{WindowMode::WINDOW_MODE_FULLSCREEN, WindowMode::WINDOW_MODE_SPLIT_PRIMARY,
         WindowMode::WINDOW_MODE_SPLIT_SECONDARY, WindowMode::WINDOW_MODE_FLOATING};
    const vector<WindowType>windowType{WindowType::WINDOW_TYPE_STATUS_BAR, WindowType::WINDOW_TYPE_NAVIGATION_BAR};

    for (auto type : windowType) {
        for (auto mode : windowMode) {
            sptr<WindowOption> option = new (std::nothrow) WindowOption();
            ASSERT_NE(nullptr, option);
            option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
            option->SetWindowMode(mode);

            sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
            EXPECT_FALSE(window == nullptr);

            SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
            sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
            ASSERT_NE(nullptr, session);
            window->property_->SetPersistentId(1);
            window->hostSession_ = session;
            window->state_ = WindowState::STATE_SHOWN;
            activeWindows_.push_back(window);

            std::map<WindowType, SystemBarProperty> systemBarProperties;
            std::map<WindowType, SystemBarPropertyFlag> systemBarPropertyFlags;
            GetSystemBarStatus(systemBarProperties, SYS_BAR_PROP_1, systemBarPropertyFlags, type);
            UpdateSystemBarProperties(systemBarProperties, systemBarPropertyFlags, window);
            WMError ret = SetSystemBarPropertiesByFlags(systemBarPropertyFlags, systemBarProperties, window);
            EXPECT_EQ(WMError::WM_OK, ret);
            sleep(1);
            auto sta = window->GetSystemBarPropertyByType(type);
            EXPECT_EQ(SYS_BAR_PROP_1, sta);

            GetSystemBarStatus(systemBarProperties, SYS_BAR_PROP_2, systemBarPropertyFlags, type);
            UpdateSystemBarProperties(systemBarProperties, systemBarPropertyFlags, window);
            ret = SetSystemBarPropertiesByFlags(systemBarPropertyFlags, systemBarProperties, window);
            EXPECT_EQ(WMError::WM_OK, ret);
            sleep(1);
            sta = window->GetSystemBarPropertyByType(type);
            EXPECT_EQ(SYS_BAR_PROP_2, sta);

            window->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);

            GetSystemBarStatus(systemBarProperties, SYS_BAR_PROP_1, systemBarPropertyFlags, type);
            UpdateSystemBarProperties(systemBarProperties, systemBarPropertyFlags, window);
            ret = SetSystemBarPropertiesByFlags(systemBarPropertyFlags, systemBarProperties, window);
            EXPECT_EQ(WMError::WM_OK, ret);
            sleep(1);
            sta = window->GetSystemBarPropertyByType(type);
            EXPECT_EQ(SYS_BAR_PROP_2, sta);
        }
    }
}
}
} // namespace Rosen
} // namespace OHOS