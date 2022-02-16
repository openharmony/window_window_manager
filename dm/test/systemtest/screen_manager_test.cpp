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
    static sptr<Display> defaultDisplay_;
    static DisplayId defaultDisplayId_;
    static ScreenId defaultScreenId_;
    static std::string defaultName_;
    static uint32_t defaultWidth_;
    static uint32_t defaultHeight_;
    static float defaultDensity_;
    static int32_t defaultFlags_;
    static VirtualScreenOption defaultoption_;
    static uint32_t waitCount_;
    const uint32_t sleepUs_ = 10 * 1000;
    const uint32_t maxWaitCount_ = 2000;
    const uint32_t execTimes_ = 10;
    const uint32_t acquireFrames_ = 1;
};

sptr<Display> ScreenManagerTest::defaultDisplay_ = nullptr;
DisplayId ScreenManagerTest::defaultDisplayId_ = DISPLAY_ID_INVALD;
ScreenId ScreenManagerTest::defaultScreenId_ = INVALID_SCREEN_ID;
std::string ScreenManagerTest::defaultName_ = "virtualScreen01";
uint32_t ScreenManagerTest::defaultWidth_ = 480;
uint32_t ScreenManagerTest::defaultHeight_ = 320;
float ScreenManagerTest::defaultDensity_ = 2.0;
int32_t ScreenManagerTest::defaultFlags_ = 0;
VirtualScreenOption ScreenManagerTest::defaultoption_ = {
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
    defaultoption_.width_ = defaultWidth_;
    defaultoption_.height_ = defaultHeight_;
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

namespace {
/**
 * @tc.name: ScreenManager01
 * @tc.desc: Create a virtual screen and destroy it
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager01, Function | MediumTest | Level1)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultoption_.surface_ = utils.psurface_;
    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultoption_);
    ASSERT_NE(SCREEN_ID_INVALID, virtualScreenId);
    ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
}

/**
 * @tc.name: ScreenManager02
 * @tc.desc: Create a virtual screen as mirror of default screen, and destroy virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager02, Function | MediumTest | Level1)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultoption_.surface_ = utils.psurface_;
    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultoption_);
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
HWTEST_F(ScreenManagerTest, ScreenManager03, Function | MediumTest | Level1)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultoption_.surface_ = utils.psurface_;
    for (uint32_t i = 0; i < execTimes_; i++) {
        ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultoption_);
        ASSERT_NE(SCREEN_ID_INVALID, virtualScreenId);
        ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
    }
}

/**
 * @tc.name: ScreenManager04
 * @tc.desc: Create a virtual screen as mirror of default screen, and destroy virtual screen for 10 times
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager04, Function | MediumTest | Level1)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultoption_.surface_ = utils.psurface_;
    for (uint32_t i = 0; i < execTimes_; i++) {
        ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultoption_);
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
HWTEST_F(ScreenManagerTest, ScreenManager05, Function | MediumTest | Level1)
{
    DisplayTestUtils utils;
    utils.SetDefaultWH(defaultDisplay_);
    ASSERT_TRUE(utils.CreateSurface());
    defaultoption_.surface_ = utils.psurface_;
    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultoption_);

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
}

/**
 * @tc.name: ScreenManager06
 * @tc.desc: Get and set screenMode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager06, Function | MediumTest | Level1)
{
    sptr<Screen> screen = ScreenManager::GetInstance().GetScreenById(defaultScreenId_);
    auto modes = screen->GetSupportedModes();
    ASSERT_GT(modes.size(), 0);
    for (uint32_t modeIdx = 0; modeIdx < modes.size(); modeIdx++) {
        ASSERT_EQ(true, screen->SetScreenActiveMode(modeIdx));
        ASSERT_EQ(modeIdx, screen->GetModeId());
    }
}

/**
 * @tc.name: ScreenManager09
 * @tc.desc: Create a virtual screen and destroy it for 10 times, it's not for shot.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager09, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultoption_.surface_ = utils.psurface_;
    defaultoption_.isForShot_ = false;
    for (uint32_t i = 0; i < execTimes_; i++) {
        ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultoption_);
        ASSERT_NE(SCREEN_ID_INVALID, virtualScreenId);
        auto screen = ScreenManager::GetInstance().GetScreenById(virtualScreenId);
        ASSERT_EQ(virtualScreenId, screen->GetId());
        ASSERT_EQ(SCREEN_ID_INVALID, screen->GetParentId());
        ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
    }
}

/**
 * @tc.name: ScreenManager10
 * @tc.desc: Create a virtual screen , mirror and destroy it for 10 times, it's not for shot.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager10, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultoption_.surface_ = utils.psurface_;
    defaultoption_.isForShot_ = false;
    for (uint32_t i = 0; i < 10; i++) {
        ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultoption_);
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
}
} // namespace Rosen
} // namespace OHOS
