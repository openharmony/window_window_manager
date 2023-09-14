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

#include <gtest/gtest.h>

#include "abstract_screen.h"
#include "abstract_screen_controller.h"
#include "scene_board_judgement.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class AbstractScreenTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<AbstractScreen> absScreen_;
    static sptr<AbstractScreenGroup> absScreenGroup_;
    static std::recursive_mutex mutex_;
    static std::string name_;
};

sptr<AbstractScreen> AbstractScreenTest::absScreen_ = nullptr;
sptr<AbstractScreenGroup> AbstractScreenTest::absScreenGroup_ = nullptr;
std::recursive_mutex AbstractScreenTest::mutex_;
std::string AbstractScreenTest::name_ = "AbstractScreenTest";

void AbstractScreenTest::SetUpTestCase()
{
    sptr<AbstractScreenController> absScreenController = new AbstractScreenController(mutex_);
    absScreen_ = new AbstractScreen(absScreenController, name_, 0, 0);
    absScreenGroup_ = new AbstractScreenGroup(absScreenController,
        0, 0, name_, ScreenCombination::SCREEN_ALONE);
    absScreen_->modes_.clear();
    absScreen_->activeIdx_ = 0;
}

void AbstractScreenTest::TearDownTestCase()
{
}

void AbstractScreenTest::SetUp()
{
}

void AbstractScreenTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetScreenMode
 * @tc.desc: Get screen mode
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, GetScreenMode, Function | SmallTest | Level3)
{
    sptr<SupportedScreenModes> mode0 = new SupportedScreenModes();
    sptr<SupportedScreenModes> mode1 = new SupportedScreenModes();
    absScreen_->modes_ = {mode0, mode1};

    absScreen_->activeIdx_ = -1;
    ASSERT_EQ(nullptr, absScreen_->GetActiveScreenMode());
    absScreen_->activeIdx_ = static_cast<int32_t>(absScreen_->modes_.size());
    ASSERT_EQ(nullptr, absScreen_->GetActiveScreenMode());
    absScreen_->activeIdx_ = 0;
    ASSERT_EQ(mode0, absScreen_->GetActiveScreenMode());
    absScreen_->activeIdx_ = 1;
    ASSERT_EQ(mode1, absScreen_->GetActiveScreenMode());

    ASSERT_EQ(mode0, (absScreen_->GetAbstractScreenModes())[0]);
    ASSERT_EQ(mode1, (absScreen_->GetAbstractScreenModes())[1]);
}
/**
 * @tc.name: ConvertToScreenInfo
 * @tc.desc: Convert to screen info
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, ConvertToScreenInfo, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, absScreen_->ConvertToScreenInfo());
}
/**
 * @tc.name: RSTree
 * @tc.desc: RS tree
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, RSTree, Function | SmallTest | Level3)
{
    std::shared_ptr<RSSurfaceNode> surfaceNode;
    absScreen_->rsDisplayNode_ = nullptr;
    absScreen_->UpdateRSTree(surfaceNode, true);
    absScreen_->UpdateDisplayGroupRSTree(surfaceNode, 0, true);

    struct RSDisplayNodeConfig config;
    absScreen_->rsDisplayNode_ = std::make_shared<RSDisplayNode>(config);
    ASSERT_NE(nullptr, absScreen_->rsDisplayNode_);
    absScreen_->UpdateRSTree(surfaceNode, true);
    absScreen_->UpdateDisplayGroupRSTree(surfaceNode, 0, true);

    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig, RSSurfaceNodeType::DEFAULT);
    absScreen_->UpdateDisplayGroupRSTree(surfaceNode, 0, true);
    absScreen_->UpdateDisplayGroupRSTree(surfaceNode, 0, false);
    ASSERT_NE(nullptr, absScreen_->rsDisplayNode_);
    absScreen_->rsDisplayNode_ = nullptr;
}
/**
 * @tc.name: ColorGamut
 * @tc.desc: Screen color gamut
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, ColorGamut, Function | SmallTest | Level3)
{
    sptr<AbstractScreenController> absScreenController0 = new AbstractScreenController(mutex_);
    sptr<AbstractScreen> absScreen0 = new AbstractScreen(absScreenController0, name_, 0, -1ULL);
    std::vector<ScreenColorGamut> colorGamuts;
    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, absScreen0->GetScreenSupportedColorGamuts(colorGamuts));
    ScreenColorGamut colorGamut;
    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, absScreen0->GetScreenColorGamut(colorGamut));

    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, absScreen0->SetScreenColorGamut(0));

    ScreenGamutMap gamutMap;
    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, absScreen0->GetScreenGamutMap(gamutMap));

    gamutMap = ScreenGamutMap::GAMUT_MAP_HDR_EXTENSION;
    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, absScreen0->GetScreenGamutMap(gamutMap));
}
/**
 * @tc.name: FillScreenInfo
 * @tc.desc: Fill screen info
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, FillScreenInfo, Function | SmallTest | Level3)
{
    absScreen_->FillScreenInfo(nullptr);
    sptr<ScreenInfo> info = new ScreenInfo();
    ASSERT_NE(nullptr, info);

    absScreen_->virtualPixelRatio_ = 0.f;
    absScreen_->FillScreenInfo(info);
    ASSERT_EQ(1.f, info->virtualPixelRatio_);

    absScreen_->virtualPixelRatio_ = 2.f;
    absScreen_->FillScreenInfo(info);
    ASSERT_EQ(2.f, info->virtualPixelRatio_);
}
/**
 * @tc.name: CalcRotation
 * @tc.desc: Calc rotation
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, CalcRotation, Function | SmallTest | Level3)
{
    absScreen_->modes_.clear();
    absScreen_->activeIdx_ = 0;

    ASSERT_EQ(Rotation::ROTATION_0, absScreen_->CalcRotation(Orientation::UNSPECIFIED));

    sptr<SupportedScreenModes> mode = new SupportedScreenModes();
    mode->width_ = 1;
    mode->height_ = 1;
    absScreen_->modes_ = {mode};

    ASSERT_EQ(Rotation::ROTATION_0, absScreen_->CalcRotation(Orientation::UNSPECIFIED));
    ASSERT_EQ(Rotation::ROTATION_90, absScreen_->CalcRotation(Orientation::VERTICAL));
    ASSERT_EQ(Rotation::ROTATION_0, absScreen_->CalcRotation(Orientation::HORIZONTAL));
    ASSERT_EQ(Rotation::ROTATION_270, absScreen_->CalcRotation(Orientation::REVERSE_VERTICAL));
    ASSERT_EQ(Rotation::ROTATION_180, absScreen_->CalcRotation(Orientation::REVERSE_HORIZONTAL));
    ASSERT_EQ(Rotation::ROTATION_0, absScreen_->CalcRotation(Orientation::LOCKED));
}

/**
 * @tc.name: GetScreenGroupId
 * @tc.desc: get screen groupId
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, GetScreenGroupId, Function | SmallTest | Level3)
{
    ScreenId result = absScreen_->GetScreenGroupId();
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(result, SCREEN_ID_INVALID);
    } else {
        EXPECT_NE(result, SCREEN_ID_INVALID);
    }
    absScreen_->groupDmsId_ = 10086;
    result = absScreen_->GetScreenGroupId();
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(result, 10086);
    } else {
        EXPECT_NE(result, 10086);
    }
}

/**
 * @tc.name: SetScreenGamutMap
 * @tc.desc: set screen gamut map
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, SetScreenGamutMap, Function | SmallTest | Level3)
{
    ScreenGamutMap gamutMap = ScreenGamutMap::GAMUT_MAP_HDR_CONSTANT;
    DMError result = absScreen_->SetScreenGamutMap(gamutMap);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(result, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    } else {
        EXPECT_NE(result, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    }
}

/**
 * @tc.name: SetScreenColorTransform
 * @tc.desc: set screen color transform
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, SetScreenColorTransform, Function | SmallTest | Level3)
{
    DMError result = absScreen_->SetScreenColorTransform();
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(result, DMError::DM_OK);
    } else {
        EXPECT_NE(result, DMError::DM_OK);
    }
}

/**
 * @tc.name: GetVirtualPixelRatio
 * @tc.desc: get virtual pixel ratio
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, GetVirtualPixelRatio, Function | SmallTest | Level3)
{
    float result = absScreen_->GetVirtualPixelRatio();
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(result, 2.0f);
    } else {
        EXPECT_NE(result, 2.0f);
    }
}

/**
 * @tc.name: GetSourceMode
 * @tc.desc: get source mode
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, GetSourceMode, Function | SmallTest | Level3)
{
    absScreenGroup_->combination_ = ScreenCombination::SCREEN_MIRROR;
    absScreen_->screenController_->dmsScreenGroupMap_.insert({10086, absScreenGroup_});
    absScreen_->groupDmsId_ = 10086;
    ScreenSourceMode result = absScreen_->GetSourceMode();
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(result, ScreenSourceMode::SCREEN_MAIN);
    } else {
        EXPECT_NE(result, ScreenSourceMode::SCREEN_MAIN);
    }

    absScreen_->screenController_->defaultRsScreenId_ = 144;
    result = absScreen_->GetSourceMode();
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(result, ScreenSourceMode::SCREEN_MIRROR);
    } else {
        EXPECT_NE(result, ScreenSourceMode::SCREEN_MIRROR);
    }
    absScreenGroup_->combination_ = ScreenCombination::SCREEN_EXPAND;
    result = absScreen_->GetSourceMode();
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(result, ScreenSourceMode::SCREEN_EXTEND);
    } else {
        EXPECT_NE(result, ScreenSourceMode::SCREEN_EXTEND);
    }
    absScreenGroup_->combination_ = ScreenCombination::SCREEN_ALONE;
    result = absScreen_->GetSourceMode();
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(result, ScreenSourceMode::SCREEN_ALONE);
    } else {
        EXPECT_NE(result, ScreenSourceMode::SCREEN_ALONE);
    }

    sptr<AbstractScreenController> absScreenController = new AbstractScreenController(mutex_);
    sptr<AbstractScreen> absScreenTest = new AbstractScreen(absScreenController, name_, 0, 0);
    result = absScreenTest->GetSourceMode();
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(result, ScreenSourceMode::SCREEN_ALONE);
    } else {
        EXPECT_NE(result, ScreenSourceMode::SCREEN_ALONE);
    }
}

/**
 * @tc.name: ConvertToScreenGroupInfo
 * @tc.desc: convert to screen group info
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, ConvertToScreenGroupInfo, Function | SmallTest | Level3)
{
    Point point_(159, 357);
    std::pair<sptr<AbstractScreen>, Point> pair_ = std::make_pair(absScreen_, point_);
    (absScreenGroup_->abstractScreenMap_).insert({10086, pair_});
    sptr<ScreenGroupInfo> result = absScreenGroup_->ConvertToScreenGroupInfo();
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(result->children_[0], 10086);
    } else {
        EXPECT_NE(result->children_[0], 10086);
    }
}

/**
 * @tc.name: GetRSDisplayNodeConfig
 * @tc.desc: Get RSDisplay node config
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, GetRSDisplayNodeConfig, Function | SmallTest | Level3)
{
    sptr<AbstractScreen> absTest = nullptr;
    RSDisplayNodeConfig config_;
    bool result = absScreenGroup_->GetRSDisplayNodeConfig(absTest, config_);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_FALSE(result);
    } else {
        EXPECT_TRUE(result);
    }

    absScreenGroup_->combination_ = ScreenCombination::SCREEN_ALONE;
    result = absScreenGroup_->GetRSDisplayNodeConfig(absTest, config_);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_FALSE(result);
    } else {
        EXPECT_TRUE(result);
    }

    absScreenGroup_->combination_ = ScreenCombination::SCREEN_MIRROR;
    absScreenGroup_->mirrorScreenId_ = 0;
    result = absScreenGroup_->GetRSDisplayNodeConfig(absTest, config_);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_FALSE(result);
    } else {
        EXPECT_TRUE(result);
    }
    Point point_(159, 357);
    std::pair<sptr<AbstractScreen>, Point> pair_ = std::make_pair(absScreen_, point_);
    (absScreenGroup_->abstractScreenMap_).insert({10086, pair_});
    absScreenGroup_->mirrorScreenId_ = 10086;
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_FALSE(result);
    } else {
        EXPECT_TRUE(result);
    }
}

/**
 * @tc.name: AddChild01
 * @tc.desc: Add child
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, AddChild01, Function | SmallTest | Level3)
{
    absScreenGroup_->abstractScreenMap_.clear();
    sptr<AbstractScreen> absTest = nullptr;
    Point point_(159, 357);
    bool result = absScreenGroup_->AddChild(absTest, point_);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_FALSE(result);
    } else {
        EXPECT_TRUE(result);
    }

    absScreenGroup_->mirrorScreenId_ = 10086;
    absScreenGroup_->combination_ = ScreenCombination::SCREEN_MIRROR;
    result = absScreenGroup_->AddChild(absScreen_, point_);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_TRUE(result);
    } else {
        ASSERT_FALSE(result);
    }
    absScreenGroup_->combination_ = ScreenCombination::SCREEN_EXPAND;
    absScreen_->rsDisplayNode_ = nullptr;
    result = absScreenGroup_->AddChild(absScreen_, point_);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_FALSE(result);
    } else {
        EXPECT_TRUE(result);
    }

    struct RSDisplayNodeConfig config;
    absScreen_->rsDisplayNode_ = std::make_shared<RSDisplayNode>(config);
    absScreen_->type_ = ScreenType::REAL;
    absScreenGroup_->defaultScreenId_ = 0;
    result = absScreenGroup_->AddChild(absScreen_, point_);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_TRUE(result);
    } else {
        ASSERT_FALSE(result);
    }
}

/**
 * @tc.name: AddChild02
 * @tc.desc: Add child
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, AddChild02, Function | SmallTest | Level3)
{
    absScreenGroup_->abstractScreenMap_.clear();
    Point point_(159, 357);
    absScreenGroup_->mirrorScreenId_ = 10086;
    absScreenGroup_->combination_ = ScreenCombination::SCREEN_MIRROR;
    std::pair<sptr<AbstractScreen>, Point> pair_ = std::make_pair(absScreen_, point_);
    (absScreenGroup_->abstractScreenMap_).insert({0, pair_});
    absScreen_->rsDisplayNode_ = nullptr;
    bool result = absScreenGroup_->AddChild(absScreen_, point_);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_FALSE(result);
    } else {
        EXPECT_TRUE(result);
    }

    struct RSDisplayNodeConfig config;
    absScreen_->rsDisplayNode_ = std::make_shared<RSDisplayNode>(config);
    absScreen_->type_ = ScreenType::REAL;
    absScreenGroup_->defaultScreenId_ = 0;
    result = absScreenGroup_->AddChild(absScreen_, point_);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_TRUE(result);
    } else {
        ASSERT_FALSE(result);
    }
}

/**
 * @tc.name: AddChildren
 * @tc.desc: Add children
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, AddChildren, Function | SmallTest | Level3)
{
    Point point_(159, 357);
    std::vector<Point> vecPoint({point_});
    std::vector<sptr<AbstractScreen>> vecSptr;
    bool result = absScreenGroup_->AddChildren(vecSptr, vecPoint);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_FALSE(result);
    } else {
        EXPECT_TRUE(result);
    }

    vecSptr.push_back(absScreen_);
    result = absScreenGroup_->AddChildren(vecSptr, vecPoint);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_TRUE(result);
    } else {
        ASSERT_FALSE(result);
    }
}

/**
 * @tc.name: RemoveChild01
 * @tc.desc: Remove child
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, RemoveChild01, Function | SmallTest | Level3)
{
    sptr<AbstractScreen> absTest = nullptr;
    bool result = absScreenGroup_->RemoveChild(absTest);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_FALSE(result);
    } else {
        EXPECT_TRUE(result);
    }

    struct RSDisplayNodeConfig config;
    absScreen_->rsDisplayNode_ = std::make_shared<RSDisplayNode>(config);
    result = absScreenGroup_->RemoveChild(absScreen_);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_TRUE(result);
    } else {
        EXPECT_FALSE(result);
    }
}

/**
 * @tc.name: RemoveChild02
 * @tc.desc: Remove child
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, RemoveChild02, Function | SmallTest | Level3)
{
    Point point_(159, 357);
    struct RSDisplayNodeConfig config;
    absScreen_->rsDisplayNode_ = std::make_shared<RSDisplayNode>(config);
    std::pair<sptr<AbstractScreen>, Point> pair_ = std::make_pair(absScreen_, point_);
    (absScreenGroup_->abstractScreenMap_).insert({0, pair_});
    bool result = absScreenGroup_->RemoveChild(absScreen_);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_TRUE(result);
    } else {
        ASSERT_FALSE(result);
    }
}

/**
 * @tc.name: RemoveDefaultScreen
 * @tc.desc: Remove default screen
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, RemoveDefaultScreen, Function | SmallTest | Level3)
{
    sptr<AbstractScreen> absTest = nullptr;
    bool result = absScreenGroup_->RemoveDefaultScreen(absTest);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_FALSE(result);
    } else {
        EXPECT_TRUE(result);
    }

    absScreen_->rsDisplayNode_ = nullptr;
    result = absScreenGroup_->RemoveDefaultScreen(absScreen_);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_TRUE(result);
    } else {
        EXPECT_FALSE(result);
    }

    struct RSDisplayNodeConfig config;
    absScreen_->rsDisplayNode_ = std::make_shared<RSDisplayNode>(config);
    result = absScreenGroup_->RemoveDefaultScreen(absScreen_);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_TRUE(result);
    } else {
        ASSERT_FALSE(result);
    }
}

/**
 * @tc.name: GetChildren
 * @tc.desc: Get children
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, GetChildren, Function | SmallTest | Level3)
{
    Point point_(159, 357);
    std::pair<sptr<AbstractScreen>, Point> pair_ = std::make_pair(absScreen_, point_);
    (absScreenGroup_->abstractScreenMap_).insert({10086, pair_});
    std::vector<sptr<AbstractScreen>> result = absScreenGroup_->GetChildren();
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(result[0], absScreen_);
    } else {
        ASSERT_NE(result[0], absScreen_);
    }
}

/**
 * @tc.name: GetChildrenPosition
 * @tc.desc: Get children position
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, GetChildrenPosition, Function | SmallTest | Level3)
{
    Point point_(159, 357);
    std::pair<sptr<AbstractScreen>, Point> pair_ = std::make_pair(absScreen_, point_);
    (absScreenGroup_->abstractScreenMap_).insert({10086, pair_});
    std::vector<Point> result = absScreenGroup_->GetChildrenPosition();
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(result[0].posX_, 159);
        ASSERT_EQ(result[0].posY_, 357);
    } else {
        EXPECT_NE(result[0].posX_, 159);
        ASSERT_NE(result[0].posY_, 357);
    }
}
}
} // namespace Rosen
} // namespace OHOS
