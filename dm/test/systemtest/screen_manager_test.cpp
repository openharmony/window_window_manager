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

#include "display_test_utils.h"
#include "screen.h"
#include "window.h"
#include "window_option.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScreenManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    sptr<Window> CreateWindowByDisplayId(DisplayId displayId);
    static sptr<Display> defaultDisplay_;
    static DisplayId defaultDisplayId_;
    static ScreenId defaultScreenId_;
    static std::string defaultName_;
    static uint32_t defaultWidth_;
    static uint32_t defaultHeight_;
    static float defaultDensity_;
    static int32_t defaultFlags_;
    static VirtualScreenOption defaultOption_;
    static uint32_t waitCount_;
    const uint32_t sleepUs_ = 10 * 1000;
    const uint32_t maxWaitCount_ = 2000;
    const uint32_t execTimes_ = 10;
    const uint32_t acquireFrames_ = 1;
    static constexpr uint32_t TEST_SPEEP_S = 1; // test spleep time
};

sptr<Display> ScreenManagerTest::defaultDisplay_ = nullptr;
DisplayId ScreenManagerTest::defaultDisplayId_ = DISPLAY_ID_INVALD;
ScreenId ScreenManagerTest::defaultScreenId_ = INVALID_SCREEN_ID;
std::string ScreenManagerTest::defaultName_ = "virtualScreen01";
uint32_t ScreenManagerTest::defaultWidth_ = 480;
uint32_t ScreenManagerTest::defaultHeight_ = 320;
float ScreenManagerTest::defaultDensity_ = 2.0;
int32_t ScreenManagerTest::defaultFlags_ = 0;
VirtualScreenOption ScreenManagerTest::defaultOption_ = {
    defaultName_, defaultWidth_, defaultHeight_, defaultDensity_, nullptr, defaultFlags_
};
uint32_t ScreenManagerTest::waitCount_ = 0;

void ScreenManagerTest::SetUpTestCase()
{
    defaultDisplay_ = DisplayManager::GetInstance().GetDefaultDisplay();
    defaultDisplayId_ = defaultDisplay_->GetId();
    defaultScreenId_ = defaultDisplay_->GetScreenId();
    defaultWidth_ = defaultDisplay_->GetWidth();
    defaultHeight_ = defaultDisplay_->GetHeight();
    defaultOption_.width_ = defaultWidth_;
    defaultOption_.height_ = defaultHeight_;
}

void ScreenManagerTest::TearDownTestCase()
{
}

void ScreenManagerTest::SetUp()
{
}

void ScreenManagerTest::TearDown()
{
}

sptr<Window> ScreenManagerTest::CreateWindowByDisplayId(DisplayId displayId)
{
    sptr<WindowOption> option = new WindowOption();
    if (option == nullptr) {
        return nullptr;
    }
    Rect displayRect = {0, 0, 200, 400};
    option->SetDisplayId(displayId);
    option->SetWindowRect(displayRect);
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    option->SetWindowName("CreateVirtualWindow01");
    option->AddWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
    option->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    sptr<Window> window = Window::Create(option->GetWindowName(), option);
    window->Show();
    return window;
}

namespace {
/**
 * @tc.name: ScreenManager01
 * @tc.desc: Create a virtual screen and destroy it
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager01, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
    ASSERT_NE(SCREEN_ID_INVALID, virtualScreenId);
    ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
}

/**
 * @tc.name: ScreenManager02
 * @tc.desc: Create a virtual screen as mirror of default screen, and destroy virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager02, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
    std::vector<ScreenId> mirrorIds;
    mirrorIds.push_back(virtualScreenId);
    ScreenManager::GetInstance().MakeMirror(defaultScreenId_, mirrorIds);
    ASSERT_NE(SCREEN_ID_INVALID, virtualScreenId);
    ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
}

/**
 * @tc.name: ScreenManager03
 * @tc.desc: Create a virtual screen and destroy it for 10 times
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager03, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    for (uint32_t i = 0; i < execTimes_; i++) {
        ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
        ASSERT_NE(SCREEN_ID_INVALID, virtualScreenId);
        ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
    }
}

/**
 * @tc.name: ScreenManager04
 * @tc.desc: Create a virtual screen as mirror of default screen, and destroy virtual screen for 10 times
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager04, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    for (uint32_t i = 0; i < execTimes_; i++) {
        ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
        std::vector<ScreenId> mirrorIds;
        mirrorIds.push_back(virtualScreenId);
        ScreenManager::GetInstance().MakeMirror(defaultScreenId_, mirrorIds);
        ASSERT_NE(SCREEN_ID_INVALID, virtualScreenId);
        ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
    }
}

/**
 * @tc.name: ScreenManager05
 * @tc.desc: Compare the length and width for recording screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager05, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    utils.SetDefaultWH(defaultDisplay_);
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);

    ASSERT_NE(SCREEN_ID_INVALID, virtualScreenId);
    uint32_t lastCount = -1u;
    std::vector<ScreenId> mirrorIds;
    mirrorIds.push_back(virtualScreenId);
    ScreenManager::GetInstance().MakeMirror(defaultScreenId_, mirrorIds);

    while (utils.successCount_ < acquireFrames_ && waitCount_ <=  maxWaitCount_) {
        if (lastCount != utils.successCount_) {
            lastCount = utils.successCount_;
        }
        ASSERT_EQ(0, utils.failCount_);
        waitCount_++;
        usleep(sleepUs_);
    }
    ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
    ASSERT_GT(utils.successCount_, 0);
    ASSERT_GT(maxWaitCount_, waitCount_);
    waitCount_ = 0;
}

/**
 * @tc.name: ScreenManager06
 * @tc.desc: Compare the length and width for recording screen, set VirtualScreen Surface before make mirror.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager06, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    utils.SetDefaultWH(defaultDisplay_);
    defaultOption_.surface_ = nullptr;
    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
    ASSERT_NE(SCREEN_ID_INVALID, virtualScreenId);

    ASSERT_TRUE(utils.CreateSurface());
    ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().SetVirtualScreenSurface(virtualScreenId, utils.psurface_));

    uint32_t lastCount = -1u;
    std::vector<ScreenId> mirrorIds;
    mirrorIds.push_back(virtualScreenId);
    ScreenManager::GetInstance().MakeMirror(defaultScreenId_, mirrorIds);

    while (utils.successCount_ < acquireFrames_ && waitCount_ <=  maxWaitCount_) {
        if (lastCount != utils.successCount_) {
            lastCount = utils.successCount_;
        }
        ASSERT_EQ(0, utils.failCount_);
        waitCount_++;
        usleep(sleepUs_);
    }
    DMError res = ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId);
    ASSERT_EQ(DMError::DM_OK, res);
    ASSERT_GT(utils.successCount_, 0);
    ASSERT_GT(maxWaitCount_, waitCount_);
    waitCount_ = 0;
}

/**
 * @tc.name: ScreenManager07
 * @tc.desc: Get and set screenMode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager07, Function | MediumTest | Level2)
{
    sptr<Screen> screen = ScreenManager::GetInstance().GetScreenById(defaultScreenId_);
    auto modes = screen->GetSupportedModes();
    auto defaultModeId = screen->GetModeId();
    ASSERT_GT(modes.size(), 0);
    for (uint32_t modeIdx = 0; modeIdx < modes.size(); modeIdx++) {
        ASSERT_EQ(true, screen->SetScreenActiveMode(modeIdx));
        sleep(TEST_SPEEP_S);
        ASSERT_EQ(modeIdx, screen->GetModeId());
    }
    ASSERT_EQ(true, screen->SetScreenActiveMode(defaultModeId));
    sleep(TEST_SPEEP_S);
}

/**
 * @tc.name: ScreenManager08
 * @tc.desc: Create a virtual screen as expansion of default screen, and destroy virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager08, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    defaultOption_.isForShot_ = false;
    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
    std::vector<sptr<Screen>> screens = ScreenManager::GetInstance().GetAllScreens();
    sptr<Screen> DefaultScreen = screens.front();
    std::vector<ExpandOption> options = {{DefaultScreen->GetId(), 0, 0}, {virtualScreenId, defaultWidth_, 0}};
    ScreenId expansionId = ScreenManager::GetInstance().MakeExpand(options);
    ASSERT_NE(SCREEN_ID_INVALID, expansionId);
    ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
}

/**
 * @tc.name: ScreenManager09
 * @tc.desc: Create a virtual screen as expansion of default screen, create windowNode on virtual screen,
 *           and destroy virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager09, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    defaultOption_.isForShot_ = false;
    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
    std::vector<sptr<Screen>> screens = ScreenManager::GetInstance().GetAllScreens();
    sptr<Screen> DefaultScreen = screens.front();
    DisplayId virtualDisplayId = DISPLAY_ID_INVALD;
    std::vector<DisplayId> displayIds = DisplayManager::GetInstance().GetAllDisplayIds();
    for (auto& id : displayIds) {
        if (id != defaultDisplayId_) {
            virtualDisplayId = id; // find the display id of virtual screen
        }
    }
    sptr<Window> window = CreateWindowByDisplayId(virtualDisplayId);
    ASSERT_NE(nullptr, window);
    sleep(TEST_SPEEP_S);
    std::vector<ExpandOption> options = {{DefaultScreen->GetId(), 0, 0}, {virtualScreenId, defaultWidth_, 0}};
    ScreenId expansionId = ScreenManager::GetInstance().MakeExpand(options);
    sleep(TEST_SPEEP_S);
    ASSERT_NE(SCREEN_ID_INVALID, expansionId);
    ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
    sleep(TEST_SPEEP_S);
    window->Destroy();
    // will add NotifyExpandDisconnect check logic.
}

/**
 * @tc.name: ScreenManager10
 * @tc.desc: Create a virtual screen and destroy it for 10 times, it's not for shot.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager10, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    defaultOption_.isForShot_ = false;
    for (uint32_t i = 0; i < execTimes_; i++) {
        ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
        ASSERT_NE(SCREEN_ID_INVALID, virtualScreenId);
        auto screen = ScreenManager::GetInstance().GetScreenById(virtualScreenId);
        ASSERT_EQ(virtualScreenId, screen->GetId());
        ASSERT_EQ(SCREEN_ID_INVALID, screen->GetParentId());
        ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
    }
}

/**
 * @tc.name: ScreenManager11
 * @tc.desc: Create a virtual screen , mirror and destroy it for 10 times, it's not for shot.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager11, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    defaultOption_.isForShot_ = false;
    for (uint32_t i = 0; i < 10; i++) {
        ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
        std::vector<ScreenId> mirrorIds;
        mirrorIds.push_back(virtualScreenId);
        ScreenManager::GetInstance().MakeMirror(defaultScreenId_, mirrorIds);
        ASSERT_NE(SCREEN_ID_INVALID, virtualScreenId);
        auto screen = ScreenManager::GetInstance().GetScreenById(virtualScreenId);
        ASSERT_EQ(virtualScreenId, screen->GetId());
        ASSERT_NE(SCREEN_ID_INVALID, screen->GetParentId());
        ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
    }
}

/**
 * @tc.name: ScreenManager11
 * @tc.desc: Screen orientatin.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager12, Function | MediumTest | Level2)
{
    auto screens = ScreenManager::GetInstance().GetAllScreens();
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    uint32_t orientation = static_cast<uint32_t>(Orientation::VERTICAL);
    uint32_t end = static_cast<uint32_t>(Orientation::REVERSE_HORIZONTAL);
    for (; orientation <= end; ++orientation) {
        screens[0]->SetOrientation(static_cast<Orientation>(orientation));
        usleep(1E6);
        ASSERT_EQ(static_cast<uint32_t>(screens[0]->GetOrientation()), orientation);
        ASSERT_EQ(static_cast<uint32_t>(display->GetOrientation()), orientation);
    }
    screens[0]->SetOrientation(Orientation::UNSPECIFIED);
    ASSERT_EQ(static_cast<uint32_t>(screens[0]->GetOrientation()), static_cast<uint32_t>(Orientation::UNSPECIFIED));
    ASSERT_EQ(static_cast<uint32_t>(display->GetOrientation()), static_cast<uint32_t>(Orientation::UNSPECIFIED));
}
}
} // namespace Rosen
} // namespace OHOS
