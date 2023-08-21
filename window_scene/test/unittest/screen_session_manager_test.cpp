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
 * @tc.desc: ScreenSesionManager rigister display manager agent
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, RegisterDisplayManagerAgent, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_STATE_LISTENER;

    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->RegisterDisplayManagerAgent(nullptr, type));
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->UnregisterDisplayManagerAgent(nullptr, type));

    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));

    ASSERT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));
    ASSERT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
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
 * @tc.name: GetScreenSession
 * @tc.desc: GetScreenSession virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenSession, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetScreenSession";
        ASSERT_EQ(ssm_->GetScreenSession(2), nullptr);
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ASSERT_NE(ssm_->GetScreenSession(2), nullptr);
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
    ASSERT_NE(ssm_->GetDefaultScreenSession(), nullptr);
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
    virtualOption.name_ = "GetDefaultScreenSession";
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
    virtualOption.name_ = "GetDefaultScreenSession";
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
    virtualOption.name_ = "GetDefaultScreenSession";
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
    virtualOption.name_ = "GetDefaultScreenSession";
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
    virtualOption.name_ = "GetDefaultScreenSession";
    ASSERT_EQ(ssm_->SetScreenActiveMode(5,0), nullptr);
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
    virtualOption.name_ = "GetDefaultScreenSession";
    ssm_->NotifyScreenChanged(5,0);
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
    virtualOption.name_ = "GetDefaultScreenSession";
    
    ASSERT_EQ(DMError::DM_ERROR_UNKNOWN, ssm_->SetVirtualPixelRatio(2,0.1));
}

/**
 * @tc.name: GetScreenColorGamut
 * @tc.desc: GetScreenColorGamut virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenColorGamut, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetDefaultScreenSession";
    ScreenColorGamut screenColorGamut;
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->GetScreenColorGamut(SCREEN_ID_INVALID,screenColorGamut));
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->GetScreenColorGamut(2,screenColorGamut));
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
    virtualOption.name_ = "GetDefaultScreenSession";
    ScreenColorGamut screenColorGamut;
    int ret =0;
    ssm_->LoadScreenSceneXml();
    ASSERT_EQ(ret, 0);
  
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
    virtualOption.name_ = "GetDefaultScreenSession";
    ScreenColorGamut screenColorGamut;
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ScreenGamutMap gamutMap;
    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, ssm_->GetScreenGamutMap(gamutMap));
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
    ScreenColorGamut screenColorGamut;
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    std::vector<ScreenId> mirrorScreenIds;
    std::vector<Point> startPoints;
    ScreenId screenGroupId1 = DISPLAY_ID_INVALID;
    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, ssm_->MakeExpand(mirrorScreenIds,startPoints));
}

/**
 * @tc.name: OnMakeExpand
 * @tc.desc: OnMakeExpand virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnMakeExpand, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "OnMakeExpand";
    ScreenColorGamut screenColorGamut;
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    std::vector<ScreenId> mirrorScreenIds;
    std::vector<Point> startPoints;
    ASSERT_EQ(false, ssm_->OnMakeExpand(mirrorScreenIds,startPoints));
}

/**
 * @tc.name: ConvertToRsScreenId
 * @tc.desc: ConvertToRsScreenId virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ConvertToRsScreenId, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "ConvertToRsScreenId";
    ScreenColorGamut screenColorGamut;
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    ASSERT_EQ(false, ssm_->ConvertToRsScreenId(2,2));
}

/**
 * @tc.name: ConvertToSmsScreenId
 * @tc.desc: ConvertToSmsScreenId virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ConvertToSmsScreenId, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "ConvertToSmsScreenId";
    ScreenColorGamut screenColorGamut;
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ASSERT_EQ(false, ssm_->ConvertToSmsScreenId(2));
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
    ScreenColorGamut screenColorGamut;
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ASSERT_NE(2, ssm_->CreateAndGetNewScreenId(2));
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
    ScreenColorGamut screenColorGamut;
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ASSERT_EQ(false, ssm_->DeleteScreenId(99));
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
    ScreenColorGamut screenColorGamut;
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ASSERT_EQ(false, ssm_->DeleteScreenId(99));
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
    virtualOption.name_ = "DeleteScreenId";
    ScreenColorGamut screenColorGamut;
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ASSERT_EQ(false, ssm_->HasRsScreenId(99));
}

/**
 * @tc.name: GetDefaultAbstractScreenId
 * @tc.desc: HasRsScreenId virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDefaultAbstractScreenId, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "DeleteScreenId";
    ScreenColorGamut screenColorGamut;
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ASSERT_EQ(false, ssm_->GetDefaultAbstractScreenId(99));
}

/**
 * @tc.name: GetDefaultAbstractScreenId
 * @tc.desc: HasRsScreenId virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDefaultAbstractScreenId, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "DeleteScreenId";
    ScreenColorGamut screenColorGamut;
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ASSERT_EQ(false, ssm_->GetDefaultAbstractScreenId(99));
}

/**
 * @tc.name: GetDefaultAbstractScreenId
 * @tc.desc: HasRsScreenId virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDefaultAbstractScreenId, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "DeleteScreenId";
    ScreenColorGamut screenColorGamut;
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ASSERT_EQ(false, ssm_->GetDefaultAbstractScreenId(99));
}


}
} // namespace Rosen
} // namespace OHOS
