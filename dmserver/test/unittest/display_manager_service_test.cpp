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

#include "display_manager_adapter.h"
#include "display_manager_config.h"
#include "display_manager_service.h"
#include "display_manager_agent_default.h"
#include "common_test_utils.h"
#include "mock_rs_display_node.h"
#include "scene_board_judgement.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DisplayManagerServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void SetAceessTokenPermission(const std::string processName);
    static sptr<DisplayManagerService> dms_;
    static constexpr DisplayId DEFAULT_DISPLAY = 0ULL;
    static constexpr DisplayId DEFAULT_SCREEN = 0ULL;
};

sptr<DisplayManagerService> DisplayManagerServiceTest::dms_ = nullptr;

void DisplayManagerServiceTest::SetUpTestCase()
{
    dms_ = new DisplayManagerService();

    dms_->abstractScreenController_->defaultRsScreenId_ = 0;
    dms_->abstractScreenController_->screenIdManager_.rs2DmsScreenIdMap_.clear();
    dms_->abstractScreenController_->screenIdManager_.rs2DmsScreenIdMap_ = {
        {0, 0}
    };
    dms_->abstractScreenController_->screenIdManager_.dms2RsScreenIdMap_.clear();
    dms_->abstractScreenController_->screenIdManager_.dms2RsScreenIdMap_ = {
        {0, 0}
    };
    const char** perms = new const char *[1];
    perms[0] = "ohos.permission.CAPTURE_SCREEN";
    CommonTestUtils::SetAceessTokenPermission("DisplayManagerServiceTest", perms, 1);
}

void DisplayManagerServiceTest::TearDownTestCase()
{
    dms_ = nullptr;
}

void DisplayManagerServiceTest::SetUp()
{
}

void DisplayManagerServiceTest::TearDown()
{
}

class DisplayChangeListenerTest : public IDisplayChangeListener {
public:
    void OnDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type) override {};
    void OnScreenshot(DisplayId displayId) override {};
};

class WindowInfoQueriedListenerTest : public IWindowInfoQueriedListener {
public:
    void HasPrivateWindow(DisplayId id, bool& hasPrivateWindow) override {};
};

namespace {
/**
 * @tc.name: OnStart
 * @tc.desc: DMS OnStart
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceTest, OnStart, Function | SmallTest | Level3)
{
    dms_->OnStart();
    bool result = dms_->Init();
    EXPECT_TRUE(result);
}

/**
 * @tc.name: Dump
 * @tc.desc: DMS dump
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceTest, Dump, Function | SmallTest | Level3)
{
    std::vector<std::u16string> args;
    ASSERT_EQ(static_cast<int>(DMError::DM_ERROR_INVALID_PARAM), dms_->Dump(-1, args));
}

/**
 * @tc.name: Config
 * @tc.desc: DMS config
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceTest, Config, Function | SmallTest | Level3)
{
    DisplayManagerConfig::intNumbersConfig_.clear();
    DisplayManagerConfig::enableConfig_.clear();
    DisplayManagerConfig::stringConfig_.clear();
    dms_->ConfigureDisplayManagerService();

    DisplayManagerConfig::intNumbersConfig_ = {
        {"dpi", {320}},
        {"defaultDeviceRotationOffset", {90}},
        {"curvedScreenBoundary", {20, 30, 40, 50}},
        {"buildInDefaultOrientation", {90}},
        {"waterfallAreaCompressionSizeWhenHorzontal", {90}}
    };
    DisplayManagerConfig::enableConfig_ = {
        {"isWaterfallDisplay", false},
        {"isWaterfallAreaCompressionEnableWhenHorizontal", false}
    };
    DisplayManagerConfig::stringConfig_ = {
        {"defaultDisplayCutoutPath", "/path"}
    };

    dms_->ConfigureDisplayManagerService();

    ASSERT_NE(dms_->displayCutoutController_, nullptr);
    ASSERT_FALSE(dms_->displayCutoutController_->isWaterfallDisplay_);
    ASSERT_EQ(dms_->displayCutoutController_->curvedScreenBoundary_[0],
        DisplayManagerConfig::intNumbersConfig_["curvedScreenBoundary"][0]);
}

/**
 * @tc.name: DisplayChange
 * @tc.desc: DMS display change
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceTest, DisplayChange, Function | SmallTest | Level3)
{
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap;
    sptr<DisplayInfo> displayInfo = new DisplayInfo();

    dms_->RegisterDisplayChangeListener(nullptr);
    dms_->NotifyDisplayStateChange(0, nullptr, displayInfoMap, DisplayStateChangeType::SIZE_CHANGE);
    dms_->NotifyScreenshot(0);

    sptr<DisplayChangeListenerTest> displayChangeListener = new DisplayChangeListenerTest();
    ASSERT_NE(nullptr, displayChangeListener);
    dms_->RegisterDisplayChangeListener(displayChangeListener);
    dms_->NotifyDisplayStateChange(0, displayInfo, displayInfoMap, DisplayStateChangeType::SIZE_CHANGE);
    dms_->NotifyScreenshot(0);

    dms_->NotifyDisplayEvent(DisplayEvent::UNLOCK);

    std::vector<DisplayId> displayIds;
    dms_->SetFreeze(displayIds, false);
}

/**
 * @tc.name: HasPrivateWindow
 * @tc.desc: DMS has private window
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceTest, HasPrivateWindow, Function | SmallTest | Level3)
{
    bool hasPrivateWindow = false;
    dms_->abstractDisplayController_->abstractDisplayMap_.clear();
    dms_->abstractDisplayController_->abstractDisplayMap_ = {
        {1, nullptr}
    };
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, dms_->HasPrivateWindow(0, hasPrivateWindow));

    dms_->RegisterWindowInfoQueriedListener(nullptr);
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, dms_->HasPrivateWindow(1, hasPrivateWindow));

    sptr<WindowInfoQueriedListenerTest> windowInfoQueriedListener = new WindowInfoQueriedListenerTest();
    ASSERT_NE(nullptr, windowInfoQueriedListener);
    dms_->RegisterWindowInfoQueriedListener(windowInfoQueriedListener);
    ASSERT_EQ(DMError::DM_OK, dms_->HasPrivateWindow(1, hasPrivateWindow));
}

/**
 * @tc.name: GetDisplayInfo
 * @tc.desc: DMS get display info
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceTest, GetDisplayInfo, Function | SmallTest | Level2)
{
    // build abstractDisplayController_ env
    std::string name = "testDisplay";
    sptr<SupportedScreenModes> info = new SupportedScreenModes();
    sptr<AbstractScreen> absScreen = new AbstractScreen(dms_->abstractScreenController_, name, 0, 0);
    sptr<AbstractDisplay> absDisplay = new AbstractDisplay(0, info, absScreen);

    dms_->abstractDisplayController_->abstractDisplayMap_.clear();
    ASSERT_EQ(nullptr, dms_->GetDefaultDisplayInfo());

    dms_->abstractDisplayController_->abstractDisplayMap_ = {
        {0, absDisplay}
    };
    ASSERT_EQ(absDisplay->name_, dms_->GetDefaultDisplayInfo()->name_);

    ASSERT_EQ(nullptr, dms_->GetDisplayInfoById(1));
    ASSERT_EQ(absDisplay->name_, dms_->GetDisplayInfoById(0)->name_);

    ASSERT_EQ(nullptr, dms_->GetDisplayInfoByScreen(1));
    ASSERT_EQ(absDisplay->name_, dms_->GetDisplayInfoByScreen(0)->name_);

    absDisplay->screenId_ = 0;

    ASSERT_EQ(SCREEN_ID_INVALID, dms_->GetScreenIdByDisplayId(1));
    ASSERT_EQ(0, dms_->GetScreenIdByDisplayId(0));

    ASSERT_EQ(nullptr, dms_->GetScreenInfoById(1));
    ASSERT_EQ(nullptr, dms_->GetScreenInfoById(0));

    ASSERT_EQ(nullptr, dms_->GetScreenGroupInfoById(1));
    ASSERT_EQ(nullptr, dms_->GetScreenGroupInfoById(0));

    ASSERT_EQ(SCREEN_ID_INVALID, dms_->GetScreenGroupIdByScreenId(1));
    ASSERT_EQ(SCREEN_ID_INVALID, dms_->GetScreenGroupIdByScreenId(0));

    dms_->GetAllDisplayIds();
    std::vector<sptr<ScreenInfo>> screenInfos;
    dms_->GetAllScreenInfos(screenInfos);

    dms_->abstractDisplayController_->abstractDisplayMap_.clear();
}

/**
 * @tc.name: VirtualScreen
 * @tc.desc: DMS virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceTest, VirtualScreen, Function | SmallTest | Level3)
{
    VirtualScreenOption option;
    ASSERT_EQ(-1, dms_->CreateVirtualScreen(option, nullptr));

    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, dms_->SetVirtualScreenSurface(-1, nullptr));
    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, dms_->SetVirtualScreenSurface(0, nullptr));

    std::vector<ScreenId> screens;
    dms_->RemoveVirtualScreenFromGroup(screens);

    DMError result = dms_->DestroyVirtualScreen(10086);
    EXPECT_EQ(result, DMError::DM_ERROR_INVALID_CALLING);
}

/**
 * @tc.name: GetDisplaySnapshot
 * @tc.desc: DMS get display snapshot
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceTest, GetDisplaySnapshot, Function | SmallTest | Level3)
{
    DisplayId displayId = 10086;
    DmErrorCode* errorCode = nullptr;
    std::shared_ptr<Media::PixelMap> result = dms_->GetDisplaySnapshot(displayId, errorCode);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: OrientationAndRotation
 * @tc.desc: DMS set oritation and rotation
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceTest, OrientationAndRotation, Function | SmallTest | Level3)
{
    Orientation orientation = Orientation::UNSPECIFIED;
    ASSERT_TRUE(DMError::DM_OK != dms_->SetOrientation(0, orientation));
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, dms_->SetOrientationFromWindow(0, orientation, true));
    Rotation rotation = Rotation::ROTATION_0;
    ASSERT_EQ(false, dms_->SetRotationFromWindow(0, rotation, true));
}

/**
 * @tc.name: ScreenColor
 * @tc.desc: DMS screen color
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceTest, ScreenColor, Function | SmallTest | Level3)
{
    std::vector<ScreenColorGamut> colorGamuts;
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, dms_->GetScreenSupportedColorGamuts(SCREEN_ID_INVALID, colorGamuts));
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, dms_->GetScreenSupportedColorGamuts(0, colorGamuts));

    ScreenColorGamut colorGamut = ScreenColorGamut::COLOR_GAMUT_SRGB;
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, dms_->GetScreenColorGamut(SCREEN_ID_INVALID, colorGamut));
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, dms_->GetScreenColorGamut(0, colorGamut));

    colorGamut = ScreenColorGamut::COLOR_GAMUT_SRGB;
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, dms_->SetScreenColorGamut(SCREEN_ID_INVALID, colorGamut));
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, dms_->SetScreenColorGamut(0, colorGamut));

    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, dms_->SetScreenColorGamut(SCREEN_ID_INVALID, 0));
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, dms_->SetScreenColorGamut(0, 0));

    ScreenGamutMap gamutMap;
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, dms_->GetScreenGamutMap(SCREEN_ID_INVALID, gamutMap));
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, dms_->GetScreenGamutMap(0, gamutMap));

    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, dms_->SetScreenGamutMap(SCREEN_ID_INVALID, gamutMap));
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, dms_->SetScreenGamutMap(0, gamutMap));

    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, dms_->SetScreenColorTransform(SCREEN_ID_INVALID));
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, dms_->SetScreenColorTransform(0));
}

/**
 * @tc.name: RegisterDisplayManagerAgent
 * @tc.desc: DMS rigister display manager agent
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceTest, RegisterDisplayManagerAgent, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_STATE_LISTENER;

    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, dms_->RegisterDisplayManagerAgent(nullptr, type));
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, dms_->UnregisterDisplayManagerAgent(nullptr, type));

    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, dms_->UnregisterDisplayManagerAgent(displayManagerAgent, type));

    ASSERT_EQ(DMError::DM_OK, dms_->RegisterDisplayManagerAgent(displayManagerAgent, type));
    ASSERT_EQ(DMError::DM_OK, dms_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: ScreenPower
 * @tc.desc: DMS screen power
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceTest, ScreenPower, Function | SmallTest | Level3)
{
    PowerStateChangeReason reason = PowerStateChangeReason::POWER_BUTTON;
    ScreenPowerState state = ScreenPowerState::POWER_ON;
    DisplayState displayState = DisplayState::ON;

    ASSERT_EQ(false, dms_->WakeUpBegin(reason));
    ASSERT_EQ(false, dms_->WakeUpEnd());

    ASSERT_EQ(false, dms_->SuspendBegin(reason));
    ASSERT_EQ(false, dms_->SuspendEnd());

    ASSERT_EQ(false, dms_->SetScreenPowerForAll(state, reason));

    ScreenId dmsScreenId = 2;
    ScreenPowerState result = dms_->GetScreenPower(dmsScreenId);
    EXPECT_EQ(result, ScreenPowerState::INVALID_STATE);

    ASSERT_EQ(true, dms_->SetDisplayState(displayState));
    ASSERT_EQ(DisplayState::ON, dms_->GetDisplayState(0));
}

/**
 * @tc.name: RsDisplayNode
 * @tc.desc: DMS rs display node
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceTest, RsDisplayNode, Function | SmallTest | Level3)
{
    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    dms_->UpdateRSTree(DISPLAY_ID_INVALID, DISPLAY_ID_INVALID, surfaceNode, true, false);
    dms_->UpdateRSTree(0, 0, surfaceNode, true, false);
}

/**
 * @tc.name: MirrorAndExpand
 * @tc.desc: DMS mirror
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceTest, MirrorAndExpand, Function | SmallTest | Level3)
{
    std::vector<ScreenId> mirrorScreenIds;
    ScreenId screenGroupId1 = DISPLAY_ID_INVALID;
    dms_->MakeMirror(DISPLAY_ID_INVALID, mirrorScreenIds, screenGroupId1);
    ASSERT_EQ(SCREEN_ID_INVALID, screenGroupId1);
    ASSERT_EQ(DMError::DM_OK, dms_->StopMirror(mirrorScreenIds));

    std::vector<ScreenId> expandScreenIds;
    std::vector<Point> startPoints;
    ScreenId screenGroupId2 = DISPLAY_ID_INVALID;
    dms_->MakeExpand(expandScreenIds, startPoints, screenGroupId2);
    ASSERT_EQ(SCREEN_ID_INVALID, screenGroupId2);
    ASSERT_EQ(DMError::DM_OK, dms_->StopExpand(expandScreenIds));
}

/**
 * @tc.name: ScreenActiveMode
 * @tc.desc: DMS mirror
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceTest, ScreenActiveMode, Function | SmallTest | Level3)
{
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, dms_->SetScreenActiveMode(SCREEN_ID_INVALID, 0));
}

/**
 * @tc.name: VirtualPixelRatio
 * @tc.desc: DMS mirror
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceTest, VirtualPixelRatio, Function | SmallTest | Level3)
{
    ASSERT_TRUE(DMError::DM_OK != dms_->SetVirtualPixelRatio(SCREEN_ID_INVALID, 0.f));
}

/**
 * @tc.name: ScreenRotationLock
 * @tc.desc: DMS mirror
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceTest, ScreenRotationLock, Function | SmallTest | Level3)
{
    dms_->SetScreenRotationLocked(false);

    bool isLocked = false;
    dms_->IsScreenRotationLocked(isLocked);
    ASSERT_EQ(false, isLocked);

    ASSERT_NE(nullptr, dms_->GetCutoutInfo(10));
}

/**
 * @tc.name: AddSurfaceNodeToDisplay | RemoveSurfaceNodeFromDisplay
 * @tc.desc: add/remove surfaceNode to/from display
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceTest, AddAndRemoveSurfaceNode, Function | SmallTest | Level3)
{
    sptr<DisplayManagerService> dms = new DisplayManagerService();
    std::shared_ptr<RSSurfaceNode> surfaceNode = nullptr;
    DMError result = dms_->RemoveSurfaceNodeFromDisplay(DEFAULT_DISPLAY, surfaceNode);
    EXPECT_EQ(result, DMError::DM_ERROR_NULLPTR);

    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, dms->AddSurfaceNodeToDisplay(DEFAULT_DISPLAY, surfaceNode, true));
    surfaceNode = std::make_shared<RSSurfaceNode>(RSSurfaceNodeConfig{}, true);
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, dms->AddSurfaceNodeToDisplay(DEFAULT_DISPLAY, surfaceNode, true));
    std::shared_ptr<RSDisplayNode> displayNode = std::make_shared<MockRSDisplayNode>(RSDisplayNodeConfig{});
    sptr<SupportedScreenModes> info = new SupportedScreenModes;
    sptr<AbstractScreen> absScreen =
        new AbstractScreen(nullptr, "", INVALID_SCREEN_ID, INVALID_SCREEN_ID);
    dms->abstractDisplayController_->abstractDisplayMap_[DEFAULT_DISPLAY] =
        new AbstractDisplay(DEFAULT_DISPLAY, info, absScreen);
    dms->abstractDisplayController_->abstractDisplayMap_[DEFAULT_DISPLAY]->screenId_ = DEFAULT_SCREEN;

    dms->abstractScreenController_->dmsScreenMap_[DEFAULT_SCREEN] =
        new AbstractScreen(dms->abstractScreenController_, "", INVALID_SCREEN_ID, INVALID_SCREEN_ID);
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, dms->AddSurfaceNodeToDisplay(DEFAULT_DISPLAY, surfaceNode, true));
    dms->abstractScreenController_->dmsScreenMap_[DEFAULT_SCREEN]->rsDisplayNode_ = displayNode;
 
    EXPECT_CALL(*reinterpret_cast<MockRSDisplayNode*>(displayNode.get()), AddChild(_, _));
    ASSERT_EQ(DMError::DM_OK, dms->AddSurfaceNodeToDisplay(DEFAULT_DISPLAY, surfaceNode, false));
    EXPECT_CALL(*reinterpret_cast<MockRSDisplayNode*>(displayNode.get()), RemoveChild(_));
    ASSERT_EQ(DMError::DM_OK, dms->RemoveSurfaceNodeFromDisplay(DEFAULT_DISPLAY, surfaceNode));

    testing::Mock::AllowLeak(displayNode.get());
}

/**
 * @tc.name: SetGravitySensorSubscriptionEnabled
 * @tc.desc: DMS set gravity sensor subscription enabled
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceTest, SetGravitySensorSubscriptionEnabled, Function | SmallTest | Level3)
{
    dms_->SetGravitySensorSubscriptionEnabled();
    DMError result = dms_->DestroyVirtualScreen(10086);
    EXPECT_EQ(result, DMError::DM_ERROR_INVALID_CALLING);
}

/**
 * @tc.name: OnStop
 * @tc.desc: DMS on stop
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerServiceTest, OnStop, Function | SmallTest | Level3)
{   
    dms_->OnStop();
    ASSERT_TRUE(true);
}
}
} // namespace Rosen
} // namespace OHOS
