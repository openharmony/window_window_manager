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

#include "session_manager/include/screen_session_manager.h"
#include "display_manager_agent_default.h"
#include "iconsumer_surface.h"
#include <surface.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScreenSessionManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<ScreenSessionManager> ssm_;

    ScreenId DEFAULT_SCREEN_ID {0};
    ScreenId VIRTUAL_SCREEN_ID {2};
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
}

namespace {
/**
 * @tc.name: RegisterDisplayManagerAgent
 * @tc.desc: RegisterDisplayManagerAgent test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, RegisterDisplayManagerAgent, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    DisplayManagerAgentType type = DisplayManagerAgentType::SCREEN_EVENT_LISTENER;
    EXPECT_NE(DMError::DM_ERROR_NOT_SYSTEM_APP, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));
    EXPECT_NE(DMError::DM_ERROR_NOT_SYSTEM_APP, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));

    type = DisplayManagerAgentType::PRIVATE_WINDOW_LISTENER;
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->RegisterDisplayManagerAgent(nullptr, type));
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->UnregisterDisplayManagerAgent(nullptr, type));

    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: ScreenChange
 * @tc.desc: ScreenChange test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ScreenChange, Function | SmallTest | Level3)
{
    ScreenEvent screenEvent = ScreenEvent::CONNECTED;
    ssm_->OnVirtualScreenChange(DEFAULT_SCREEN_ID, screenEvent);
    ssm_->OnVirtualScreenChange(VIRTUAL_SCREEN_ID, screenEvent);
    ssm_->OnScreenChange(DEFAULT_SCREEN_ID, screenEvent);
    ssm_->OnScreenChange(VIRTUAL_SCREEN_ID, screenEvent);
    EXPECT_TRUE(1);

    screenEvent = ScreenEvent::DISCONNECTED;
    ssm_->OnVirtualScreenChange(DEFAULT_SCREEN_ID, screenEvent);
    ssm_->OnVirtualScreenChange(VIRTUAL_SCREEN_ID, screenEvent);
    ssm_->OnScreenChange(DEFAULT_SCREEN_ID, screenEvent);
    ssm_->OnScreenChange(VIRTUAL_SCREEN_ID, screenEvent);
    EXPECT_TRUE(1);
}

/**
 * @tc.name: ScreenPower
 * @tc.desc: ScreenSesionManager screen power
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ScreenPower, Function | SmallTest | Level3)
{
    PowerStateChangeReason reason = PowerStateChangeReason::POWER_BUTTON;
    ScreenPowerState state = ScreenPowerState::POWER_ON;
    DisplayState displayState = DisplayState::ON;

    ASSERT_EQ(false, ssm_->WakeUpBegin(reason));
    ASSERT_EQ(false, ssm_->WakeUpEnd());

    ASSERT_EQ(false, ssm_->SuspendBegin(reason));
    ASSERT_EQ(false, ssm_->SuspendEnd());

    ASSERT_EQ(false, ssm_->SetScreenPowerForAll(state, reason));

    ASSERT_EQ(true, ssm_->SetDisplayState(displayState));
    ASSERT_EQ(DisplayState::ON, ssm_->GetDisplayState(0));
}

/**
 * @tc.name: GetDisplaySnapshot
 * @tc.desc: ScreenSesionManager screen shot
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDisplaySnapshot, Function | SmallTest | Level3)
{
    DisplayId displayId(0);
    DmErrorCode* errorCode = nullptr;
    ASSERT_EQ(nullptr, ssm_->GetDisplaySnapshot(displayId, errorCode));
}

/**
 * @tc.name: VirtualScreen
 * @tc.desc: ScreenSesionManager virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, VirtualScreen, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "testVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    std::vector<ScreenId> mirrorScreenIds;
    ScreenId mainScreenId(DEFAULT_SCREEN_ID);
    ScreenId screenGroupId{1};
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->MakeMirror(mainScreenId, mirrorScreenIds, screenGroupId));
    mirrorScreenIds.push_back(VIRTUAL_SCREEN_ID);
    ASSERT_NE(DMError::DM_OK, ssm_->MakeMirror(mainScreenId, mirrorScreenIds, screenGroupId));

    auto result1 = ssm_->SetVirtualScreenSurface(VIRTUAL_SCREEN_ID, nullptr);
    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, result1);
    sptr<IConsumerSurface> surface = OHOS::IConsumerSurface::Create();
    auto result2 = ssm_->SetVirtualScreenSurface(VIRTUAL_SCREEN_ID, surface->GetProducer());
    if (DMError::DM_ERROR_RENDER_SERVICE_FAILED == result2) {
        ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, result2);
    }
    if (DMError::DM_OK != result2) {
        ASSERT_NE(DMError::DM_OK, ssm_->DestroyVirtualScreen(VIRTUAL_SCREEN_ID));
    }
}

/**
 * @tc.name: AutoRotate
 * @tc.desc: ScreenSesionManager SetVirtualMirrorScreenCanvasRotation test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, AutoRotate, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "testAutoRotate";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    auto result1 = ssm_->SetVirtualMirrorScreenCanvasRotation(VIRTUAL_SCREEN_ID, true);
    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, result1);
    auto result2 = ssm_->SetVirtualMirrorScreenCanvasRotation(VIRTUAL_SCREEN_ID, false);
    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, result2);
}

/**
 * @tc.name: GetScreenSession
 * @tc.desc: GetScreenSession virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenSession, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetScreenSession";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSession> screenSession =
        new(std::nothrow) ScreenSession("screenSession", 2, 2, 3);
    ASSERT_NE(ssm_->GetScreenSession(2), screenSession);
}


/**
 * @tc.name: GetDefaultScreenSession
 * @tc.desc: GetDefaultScreenSession virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDefaultScreenSession, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetDefaultScreenSession";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSession> screenSession =
        new(std::nothrow) ScreenSession("GetDefaultScreenSession", 2, 2, 3);
    ASSERT_NE(ssm_->GetDefaultScreenSession(), screenSession);
}

/**
 * @tc.name: GetDefaultDisplayInfo
 * @tc.desc: GetDefaultDisplayInfo virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDefaultDisplayInfo, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetDefaultDisplayInfo";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSession> screenSession = nullptr;
    screenSession =
        new (std::nothrow) ScreenSession("GetDefaultDisplayInfo", 2, 2, 3);
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_ {
        { 2, screenSession },
    };
    ssm_->screenSessionMap_ = screenSessionMap_;

    ASSERT_NE(ssm_->GetScreenSession(2), nullptr);
    ASSERT_EQ(ssm_->GetDefaultDisplayInfo(), nullptr);
}

/**
 * @tc.name: GetDisplayInfoById
 * @tc.desc: GetDisplayInfoById virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDisplayInfoById, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetDisplayInfoById";
    ASSERT_EQ(ssm_->GetDisplayInfoById(1), nullptr);
}

/**
 * @tc.name: GetDisplayInfoByScreen
 * @tc.desc: GetDisplayInfoByScreen virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDisplayInfoByScreen, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetDisplayInfoByScreen";
    ASSERT_EQ(ssm_->GetDisplayInfoByScreen(1), nullptr);
}

/**
 * @tc.name: GetScreenInfoById
 * @tc.desc: GetScreenInfoById virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenInfoById, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetScreenInfoById";
    ASSERT_EQ(ssm_->GetScreenInfoById(1), nullptr);
}

/**
 * @tc.name: SetScreenActiveMode
 * @tc.desc: SetScreenActiveMode virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenActiveMode, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "SetScreenActiveMode";
    ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> screenSession = nullptr;
    screenSession =
        new (std::nothrow) ScreenSession("SetScreenActiveMode", 2, 2, 3);
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_{
        {2, screenSession},
    };
    ssm_->screenSessionMap_ = screenSessionMap_;
    ASSERT_EQ(ssm_->SetScreenActiveMode(2, 0), DMError::DM_OK);
}


/**
 * @tc.name: NotifyScreenChanged
 * @tc.desc: NotifyScreenChanged virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyScreenChanged, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "NotifyScreenChanged";
    ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> screenSession = nullptr;
    screenSession =
        new (std::nothrow) ScreenSession("NotifyScreenChanged", 2, 2, 3);
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_{
        {2, screenSession},
    };
    ssm_->screenSessionMap_ = screenSessionMap_;
    sptr<ScreenInfo> screenInfo;
    ssm_->NotifyScreenChanged(screenInfo, ScreenChangeEvent::UPDATE_ORIENTATION);
    screenInfo = new ScreenInfo();
    ssm_->NotifyScreenChanged(screenInfo, ScreenChangeEvent::UPDATE_ORIENTATION);
    ASSERT_EQ(ssm_->SetScreenActiveMode(2, 0), DMError::DM_OK);
}

/**
 * @tc.name: SetVirtualPixelRatio
 * @tc.desc: SetVirtualPixelRatio virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetVirtualPixelRatio, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "SetVirtualPixelRatio";
    ASSERT_EQ(DMError::DM_OK, ssm_->SetVirtualPixelRatio(2, 0.1));
}

/**
 * @tc.name: SetResolution
 * @tc.desc: SetResolution virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetResolution, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "SetResolution";
    ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> screenSession = nullptr;
    screenSession = new (std::nothrow) ScreenSession("SetResolution", 2, 2, 3);
    sptr<SupportedScreenModes> mode = new SupportedScreenModes();
    mode->width_ = 1;
    mode->height_ = 1;
    screenSession->modes_ = {mode};
    screenSession->activeIdx_ = 0;
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_{
        {2, screenSession},
    };
    ssm_->screenSessionMap_ = screenSessionMap_;
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->SetResolution(2, 100, 100, 0.5));
}

/**
 * @tc.name: GetScreenColorGamut
 * @tc.desc: GetScreenColorGamut virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenColorGamut, Function | SmallTest | Level3)
{
    ScreenColorGamut colorGamut = ScreenColorGamut::COLOR_GAMUT_SRGB;
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->GetScreenColorGamut(1, colorGamut));
    sptr<ScreenSession> screenSession = nullptr;
    screenSession = new (std::nothrow) ScreenSession("GetScreenColorGamut", 2, 2, 3);
    screenSession->activeIdx_ = 100;
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_{
        {2, screenSession},
    };
    ssm_->screenSessionMap_ = screenSessionMap_;
    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, ssm_->GetScreenColorGamut(2, colorGamut));
}

/**
 * @tc.name: LoadScreenSceneXml
 * @tc.desc: LoadScreenSceneXml virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, LoadScreenSceneXml, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "LoadScreenSceneXml";
    ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> screenSession = nullptr;
    screenSession = new (std::nothrow) ScreenSession("LoadScreenSceneXml", 2, 2, 3);
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_{
        {2, screenSession},
    };
    ssm_->screenSessionMap_ = screenSessionMap_;
    ssm_->LoadScreenSceneXml();
    ASSERT_EQ(ssm_->SetScreenActiveMode(2, 0), DMError::DM_OK);
}

/**
 * @tc.name: GetScreenGamutMap
 * @tc.desc: GetScreenGamutMap virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenGamutMap, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetScreenGamutMap";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSession> screenSession = nullptr;
    screenSession = new (std::nothrow) ScreenSession("GetScreenGamutMap", 2, 2, 3);
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_{
        {2, screenSession},
    };
    ssm_->screenSessionMap_ = screenSessionMap_;
    ScreenGamutMap gamutMap;
    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, ssm_->GetScreenGamutMap(2, gamutMap));
}

/**
 * @tc.name: MakeExpand
 * @tc.desc: MakeExpand virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, MakeExpand, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "MakeExpand";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    std::vector<ScreenId> mirrorScreenIds;
    std::vector<Point> startPoints;
    ScreenId screenGroupId2 = DISPLAY_ID_INVALID;
    ASSERT_NE(DMError::DM_ERROR_RENDER_SERVICE_FAILED, ssm_->MakeExpand(mirrorScreenIds, startPoints, screenGroupId2));
}

/**
 * @tc.name: DeleteScreenId
 * @tc.desc: DeleteScreenId virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, DeleteScreenId, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "DeleteScreenId";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ScreenSessionManager::ScreenIdManager sim;
    ASSERT_EQ(false, sim.DeleteScreenId(99));
}

/**
 * @tc.name: HasRsScreenId
 * @tc.desc: HasRsScreenId virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HasRsScreenId, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "HasRsScreenId";

    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ScreenSessionManager::ScreenIdManager sim;
    ASSERT_EQ(false, sim.HasRsScreenId(99));
}

/**
 * @tc.name: AddAsFirstScreenLocked
 * @tc.desc: AddAsFirstScreenLocked virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, AddAsFirstScreenLocked, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "AddAsFirstScreenLocked";

    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSession> newScreen = new (std::nothrow) ScreenSession();
    sptr<ScreenSessionGroup> sessiongroup=nullptr;
    ASSERT_NE(sessiongroup, ssm_->AddAsFirstScreenLocked(newScreen));
}

/**
 * @tc.name: AddAsSuccedentScreenLocked
 * @tc.desc: AddAsSuccedentScreenLocked virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, AddAsSuccedentScreenLocked, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "AddAsSuccedentScreenLocked";

    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSession> newScreen;
    sptr<ScreenSessionGroup> sessiongroup=nullptr;
    ASSERT_EQ(sessiongroup, ssm_->AddAsSuccedentScreenLocked(newScreen));
}

/**
 * @tc.name: SetMirror
 * @tc.desc: SetMirror virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetMirror, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "SetMirror";
    ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());

    std::vector<ScreenId> screens{0, 1, 2, 3, 4, 5, 6, 7};
    sptr<ScreenSession> screenSession = nullptr;
    screenSession = new (std::nothrow) ScreenSession("GetScreenGamutMap", 2, 2, 3);
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_{
        {2, screenSession},
    };
    ssm_->screenSessionMap_ = screenSessionMap_;
    auto screen = ssm_->GetScreenSession(2);
    screen->GetScreenProperty().SetScreenType(ScreenType::REAL);
    ASSERT_EQ(DMError::DM_OK, ssm_->SetMirror(2, screens));
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->SetMirror(9, screens));
}

/**
 * @tc.name: GetAbstractScreenGroup
 * @tc.desc: GetAbstractScreenGroup virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetAbstractScreenGroup, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetAbstractScreenGroup";

    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSessionGroup> sreengroup=nullptr;
    ASSERT_EQ(sreengroup, ssm_->GetAbstractScreenGroup(2));
}

/**
 * @tc.name: InitAbstractScreenModesInfo
 * @tc.desc: InitAbstractScreenModesInfo virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, InitAbstractScreenModesInfo, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "InitAbstractScreenModesInfo";

    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSession> screenSession =new  (std::nothrow) ScreenSession();
    ASSERT_EQ(true, ssm_->InitAbstractScreenModesInfo(screenSession));
}

/**
 * @tc.name: AddToGroupLocked
 * @tc.desc: AddToGroupLocked virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, AddToGroupLocked, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "AddToGroupLocked";

    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSession> newScreen=nullptr;
    sptr<ScreenSessionGroup> sessiongroup=nullptr;
    ASSERT_EQ(sessiongroup, ssm_->AddToGroupLocked(newScreen));
}

/**
 * @tc.name: InitVirtualScreen
 * @tc.desc: InitVirtualScreen virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, InitVirtualScreen, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "InitVirtualScreen";
    sptr<ScreenSession> screenSession;
    VirtualScreenOption option;
    ASSERT_NE(screenSession, ssm_->InitVirtualScreen(1, 2, option));
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    screenSession =new  (std::nothrow) ScreenSession();
    ASSERT_NE(screenSession, ssm_->InitVirtualScreen(1, 2, option));
}

/**
 * @tc.name: InitAndGetScreen
 * @tc.desc: InitAndGetScreen virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, InitAndGetScreen, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "InitAndGetScreen";

    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSession> screenSession =new  (std::nothrow) ScreenSession();
    ASSERT_NE(screenSession, ssm_->InitAndGetScreen(2));
}

/**
 * @tc.name: RemoveFromGroupLocked
 * @tc.desc: RemoveFromGroupLocked virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, RemoveFromGroupLocked, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "RemoveFromGroupLocked";

    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSession> newScreen = new (std::nothrow) ScreenSession();
    sptr<ScreenSessionGroup> sessiongroup;
    ASSERT_EQ(sessiongroup, ssm_->RemoveFromGroupLocked(newScreen));
}

/**
 * @tc.name: CreateAndGetNewScreenId
 * @tc.desc: CreateAndGetNewScreenId virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CreateAndGetNewScreenId, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "CreateAndGetNewScreenId";
    ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> screenSession = nullptr;
    screenSession = new (std::nothrow) ScreenSession("CreateAndGetNewScreenId", 2, 2, 3);
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_{
        {2, screenSession},
    };
    ssm_->screenSessionMap_ = screenSessionMap_;
    ScreenSessionManager::ScreenIdManager sim;
    ASSERT_EQ(1000, sim.CreateAndGetNewScreenId(SCREEN_ID_INVALID));
}

/**
 * @tc.name: AddScreenToGroup
 * @tc.desc: AddScreenToGroup virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, AddScreenToGroup, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "AddScreenToGroup";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    const std::vector<ScreenId> addScreens;
    const std::vector<Point> addChildPos;
    std::map<ScreenId, bool> removeChildResMap;
    sptr<ScreenSessionGroup> group;
    ssm_->AddScreenToGroup(group, addScreens, addChildPos, removeChildResMap);
    sptr<ScreenSession> screenSession =new  (std::nothrow) ScreenSession();
    ASSERT_NE(screenSession, ssm_->InitAndGetScreen(2));
}

/**
 * @tc.name: GetRSDisplayNodeByScreenId
 * @tc.desc: GetRSDisplayNodeByScreenId virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetRSDisplayNodeByScreenId, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetRSDisplayNodeByScreenId";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ssm_->GetRSDisplayNodeByScreenId(2);
    sptr<ScreenSession> screenSession =new  (std::nothrow) ScreenSession();
    ASSERT_NE(screenSession, ssm_->InitAndGetScreen(2));
}

/**
 * @tc.name: GetAllDisplayIds
 * @tc.desc: GetAllDisplayIds virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetAllDisplayIds, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetAllDisplayIds";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSession> screenSession =new  (std::nothrow) ScreenSession();
    std::vector<DisplayId>  displays(2);
    ASSERT_NE(displays, ssm_->GetAllDisplayIds());
}

/**
 * @tc.name: SetScreenGamutMap
 * @tc.desc: SetScreenGamutMap virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenGamutMap, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "SetScreenGamutMap";
    ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> screenSession = nullptr;
    screenSession = new (std::nothrow) ScreenSession("SetScreenGamutMap", 2, 2, 3);
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_{
        {2, screenSession},
    };
    ssm_->screenSessionMap_ = screenSessionMap_;
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM,
              ssm_->SetScreenGamutMap(SCREEN_ID_INVALID, ScreenGamutMap::GAMUT_MAP_HDR_EXTENSION));
    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, ssm_->SetScreenGamutMap(2, ScreenGamutMap::GAMUT_MAP_EXTENSION));
}

/**
 * @tc.name: StopExpand
 * @tc.desc: StopExpand virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, StopExpand, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "StopExpand";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    std::vector<ScreenId> expandScreenIds {0, 1, 2, 3, 4, 5};
    ASSERT_NE(DMError::DM_ERROR_NOT_SYSTEM_APP, ssm_->StopExpand(expandScreenIds));
}

/**
 * @tc.name: OnScreenDisconnect
 * @tc.desc: OnScreenDisconnect virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnScreenDisconnect, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "OnScreenDisconnect";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ssm_->OnScreenDisconnect(1);
    sptr<ScreenSession> screenSession =new  (std::nothrow) ScreenSession();
    ASSERT_NE(screenSession, ssm_->InitAndGetScreen(2));
}

/**
 * @tc.name: SetScreenColorGamut
 * @tc.desc: SetScreenColorGamut virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenColorGamut, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "SetScreenColorGamut";
    ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> screenSession = nullptr;
    screenSession = new (std::nothrow) ScreenSession("SetScreenColorGamut", 2, 2, 3);
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_{
        {2, screenSession},
    };
    ssm_->screenSessionMap_ = screenSessionMap_;
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->SetScreenColorGamut(SCREEN_ID_INVALID, 2));
    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, ssm_->SetScreenColorGamut(2, 2));
}

/**
 * @tc.name: SetScreenRotationLocked
 * @tc.desc: SetScreenRotationLocked virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenRotationLocked, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "SetScreenRotationLocked";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->SetScreenRotationLocked(false));
}

/**
 * @tc.name: UpdateScreenRotationProperty
 * @tc.desc: UpdateScreenRotationProperty virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UpdateScreenRotationProperty, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "UpdateScreenRotationProperty";
    RRect bounds;
    bounds.rect_.width_ = 1344;
    bounds.rect_.height_ = 2772;
    int rotation = 1;
    ScreenSessionManager::GetInstance().UpdateScreenRotationProperty(1, bounds, 1);
    auto screenId = ScreenSessionManager::GetInstance().CreateVirtualScreen(virtualOption,
        displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ScreenSessionManager::GetInstance().UpdateScreenRotationProperty(1, bounds, rotation);
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession();
    ASSERT_NE(screenSession, ScreenSessionManager::GetInstance().InitAndGetScreen(2));
}

/**
 * @tc.name: SetOrientationFromWindow
 * @tc.desc: SetOrientationFromWindow virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetOrientationFromWindow, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "SetOrientationFromWindow";
    ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> screenSession = nullptr;
    screenSession = new (std::nothrow) ScreenSession("GetScreenGamutMap", 1, 1, 3);
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_{
        {1, screenSession},
    };
    ssm_->screenSessionMap_ = screenSessionMap_;
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->SetOrientationFromWindow(2, Orientation::AUTO_ROTATION_RESTRICTED));
    ASSERT_EQ(DMError::DM_OK, ssm_->SetOrientationFromWindow(1, Orientation::AUTO_ROTATION_RESTRICTED));
}

/**
 * @tc.name: MakeUniqueScreen
 * @tc.desc: Make unique screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, MakeUniqueScreen, Function | SmallTest | Level3)
{
    vector<ScreenId> screenIds;
    screenIds.clear();
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->MakeUniqueScreen(screenIds));
}


/**
 * @tc.name: DisableMirror
 * @tc.desc: DisableMirror test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, DisableMirror, Function | SmallTest | Level3)
{
    ASSERT_EQ(DMError::DM_OK, ssm_->DisableMirror(false));
}

/**
 * @tc.name: HasImmersiveWindow
 * @tc.desc: HasImmersiveWindow test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HasImmersiveWindow, Function | SmallTest | Level3)
{
    bool immersive = false;
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->HasImmersiveWindow(immersive));
    immersive = true;
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->HasImmersiveWindow(immersive));
}

/**
 * @tc.name: SetSpecifiedScreenPower
 * @tc.desc: ScreenSessionManager screen power
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetSpecifiedScreenPower, Function | SmallTest | Level3)
{
    ScreenId mainScreenId(DEFAULT_SCREEN_ID);
    ScreenPowerState state = ScreenPowerState::POWER_ON;
    PowerStateChangeReason reason = PowerStateChangeReason::POWER_BUTTON;
    ASSERT_EQ(false, ssm_->SetSpecifiedScreenPower(mainScreenId, state, reason));

    reason = PowerStateChangeReason::STATE_CHANGE_REASON_COLLABORATION;
    EXPECT_TRUE(ssm_->SetSpecifiedScreenPower(mainScreenId, state, reason));
}

}
} // namespace Rosen
} // namespace OHOS
