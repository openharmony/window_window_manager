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

#include <gtest/gtest.h>
#include <parameter.h>
#include <parameters.h>

#include "screen_session_manager/include/screen_session_manager.h"
#include "screen_scene_config.h"
#include "scene_board_judgement.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
}
class ScreenSessionManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<ScreenSessionManager> ssm_;
};

sptr<ScreenSessionManager> ScreenSessionManagerTest::ssm_ = nullptr;

void ScreenSessionManagerTest::SetUpTestCase()
{
    ssm_ = new ScreenSessionManager();
}

void ScreenSessionManagerTest::TearDownTestCase()
{
    ssm_ = nullptr;
}

void ScreenSessionManagerTest::SetUp()
{
}

void ScreenSessionManagerTest::TearDown()
{
    usleep(SLEEP_TIME_IN_US);
}

namespace {
/**
 * @tc.name: SwitchScrollParam01
 * @tc.desc: SwitchScrollParam test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SwitchScrollParam01, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ScreenSceneConfig::scrollableParams_.clear();
    vector<FoldDisplayMode> displayModeALL = {
        FoldDisplayMode::SUB,
        FoldDisplayMode::MAIN,
        FoldDisplayMode::FULL,
        FoldDisplayMode::UNKNOWN,
        FoldDisplayMode::COORDINATION,
    };
    std::map<FoldDisplayMode, std::string> scrollVelocityScaleParam = {
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::SUB, "1.0"),
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::MAIN, "1.1"),
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::FULL, "1.2"),
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::UNKNOWN, "1.3"),
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::COORDINATION, "1.4")
    };
    std::map<FoldDisplayMode, std::string> scrollFrictionParam = {
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::SUB, "1.0"),
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::MAIN, "2.0"),
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::FULL, "3.0"),
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::UNKNOWN, "4.0"),
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::COORDINATION, "5.0"),
    };
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    std::string ret1, ret2;
    for (FoldDisplayMode displayMode : displayModeALL) {
        ScrollableParam scrollableParam;
        scrollableParam.velocityScale_ = scrollVelocityScaleParam.count(displayMode) ?
            scrollVelocityScaleParam[displayMode] : "";
        scrollableParam.friction_ = scrollFrictionParam.count(displayMode) ?
            scrollFrictionParam[displayMode] : "";
        ScreenSceneConfig::scrollableParams_[displayMode] = scrollableParam;
        ssm->SwitchScrollParam(displayMode);
        ret1 = system::GetParameter("persist.scrollable.velocityScale", "0");
        ret2 = system::GetParameter("persist.scrollable.friction", "0");
        EXPECT_EQ(ret1, scrollVelocityScaleParam[displayMode]);
        EXPECT_EQ(ret2, scrollFrictionParam[displayMode]);
    }
}

/**
 * @tc.name: SwitchScrollParam02
 * @tc.desc: SwitchScrollParam test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SwitchScrollParam02, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ScreenSceneConfig::scrollableParams_.clear();
    vector<FoldDisplayMode> displayModeALL = {
        FoldDisplayMode::SUB,
        FoldDisplayMode::MAIN,
        FoldDisplayMode::FULL,
        FoldDisplayMode::UNKNOWN,
        FoldDisplayMode::COORDINATION,
    };
    std::map<FoldDisplayMode, std::string> scrollVelocityScaleParam = {
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::FULL, "2.0"),
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::MAIN, "main"),
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::UNKNOWN, "!!"),
    };
    std::map<FoldDisplayMode, std::string> scrollFrictionParam;
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    std::string ret1, ret2;
    for (FoldDisplayMode displayMode : displayModeALL) {
        ScrollableParam scrollableParam;
        scrollableParam.velocityScale_ = scrollVelocityScaleParam.count(displayMode) ?
            scrollVelocityScaleParam[displayMode] : "";
        scrollableParam.friction_ = scrollFrictionParam.count(displayMode) ?
            scrollFrictionParam[displayMode] : "";
        ScreenSceneConfig::scrollableParams_[displayMode] = scrollableParam;
        ssm->SwitchScrollParam(displayMode);
        ret1 = system::GetParameter("persist.scrollable.velocityScale", "0");
        ret2 = system::GetParameter("persist.scrollable.friction", "0");
        EXPECT_EQ(ret1, scrollVelocityScaleParam[displayMode]);
        EXPECT_EQ(ret2, scrollFrictionParam[displayMode]);
    }
}

/**
 * @tc.name: WakeUpPictureFrameBlock
 * @tc.desc: WakeUpPictureFrameBlock test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, WakeUpPictureFrameBlock, Function | SmallTest | Level3)
{
    ssm_->WakeUpPictureFrameBlock(DisplayEvent::SCREEN_LOCK_OFF);
    ASSERT_EQ(ssm_->pictureFrameBreak_, false);
    ASSERT_NE(ssm_, nullptr);
    ssm_->pictureFrameReady_ = false;
    ssm_->pictureFrameBreak_ = false;
    ssm_->WakeUpPictureFrameBlock(DisplayEvent::SCREEN_LOCK_START_DREAM);
    ASSERT_EQ(ssm_->pictureFrameReady_, true);
    ssm_->WakeUpPictureFrameBlock(DisplayEvent::SCREEN_LOCK_END_DREAM);
    ASSERT_EQ(ssm_->pictureFrameBreak_, true);
}

/**
 * @tc.name: AddVirtualScreenBlockList
 * @tc.desc: AddVirtualScreenBlockList test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, AddVirtualScreenBlockList, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    std::vector<int32_t> persistentIds {0, 1, 2};
    ASSERT_EQ(DMError::DM_OK, ssm_->AddVirtualScreenBlockList(persistentIds));
}

/**
 * @tc.name: RemoveVirtualScreenBlockList
 * @tc.desc: RemoveVirtualScreenBlockList test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, RemoveVirtualScreenBlockList, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    std::vector<int32_t> persistentIds {0, 1, 2};
    ASSERT_EQ(DMError::DM_OK, ssm_->RemoveVirtualScreenBlockList(persistentIds));
}

/**
 * @tc.name: BlockScreenWaitPictureFrameByCV
 * @tc.desc: BlockScreenWaitPictureFrameByCV test
 * @tc.type: FUNC
 */
 HWTEST_F(ScreenSessionManagerTest, BlockScreenWaitPictureFrameByCV, Function | SmallTest | Level3)
 {
     ASSERT_NE(ssm_, nullptr);
     ssm_->pictureFrameReady_ = true;
     ssm_->pictureFrameBreak_ = true;
     bool result = ssm_->BlockScreenWaitPictureFrameByCV(true);
     ASSERT_EQ(result, true);
     result = ssm_->BlockScreenWaitPictureFrameByCV(false);
     ASSERT_EQ(result, true);

     ASSERT_EQ(ssm_->pictureFrameReady_, false);
     ASSERT_EQ(ssm_->pictureFrameBreak_, false);
}
}
}
}