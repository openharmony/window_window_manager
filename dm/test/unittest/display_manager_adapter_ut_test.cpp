/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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
#include "display_manager.h"
#include "screen_manager.h"
#include "display_manager_proxy.h"
#include "window_scene.h"
#include "scene_board_judgement.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DisplayManagerAdapterUtTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DisplayManagerAdapterUtTest::SetUpTestCase()
{
}

void DisplayManagerAdapterUtTest::TearDownTestCase()
{
}

void DisplayManagerAdapterUtTest::SetUp()
{
}

void DisplayManagerAdapterUtTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetDisplayInfo
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, GetDisplayInfo, TestSize.Level1)
{
    sptr<DisplayInfo> info = SingletonContainer::Get<DisplayManagerAdapter>().GetDisplayInfo(DISPLAY_ID_INVALID);
    ASSERT_EQ(info, nullptr);
}

/**
 * @tc.name: GetCutoutInfo
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, GetCutoutInfo, TestSize.Level1)
{
    sptr<CutoutInfo> info = SingletonContainer::Get<DisplayManagerAdapter>().GetCutoutInfo(DISPLAY_ID_INVALID);
    ASSERT_EQ(info, nullptr);
}

/**
 * @tc.name: GetScreenSupportedColorGamuts
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, GetScreenSupportedColorGamuts, TestSize.Level1)
{
    std::vector<ScreenColorGamut> colorGamuts;
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().GetScreenSupportedColorGamuts(0, colorGamuts);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(err, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    } else {
        ASSERT_EQ(err, DMError::DM_OK);
    }
}

/**
 * @tc.name: SetScreenColorGamut
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, SetScreenColorGamut, TestSize.Level1)
{
    std::vector<ScreenColorGamut> colorGamuts;
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetScreenColorGamut(0, -1);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(err, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    } else {
        ASSERT_EQ(err, DMError::DM_ERROR_INVALID_PARAM);
    }
}

/**
 * @tc.name: GetScreenColorGamut
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, GetScreenColorGamut, TestSize.Level1)
{
    ScreenColorGamut colorGamut;
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().GetScreenColorGamut(0, colorGamut);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(err, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    } else {
        ASSERT_EQ(err, DMError::DM_OK);
    }
}

/**
 * @tc.name: GetScreenGamutMap
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, GetScreenGamutMap, TestSize.Level1)
{
    ScreenGamutMap gamutMap;
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().GetScreenGamutMap(0, gamutMap);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(err, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    } else {
        ASSERT_EQ(err, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    }
}

/**
 * @tc.name: SetScreenGamutMap
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, SetScreenGamutMap, TestSize.Level1)
{
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetScreenGamutMap(0, GAMUT_MAP_CONSTANT);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(err, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    } else {
        ASSERT_EQ(err, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    }
}

/**
 * @tc.name: SetScreenColorTransform
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, SetScreenColorTransform, TestSize.Level1)
{
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetScreenColorTransform(0);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(err, DMError::DM_OK);
    } else {
        ASSERT_EQ(err, DMError::DM_OK);
    }
}

/**
 * @tc.name: SetFreeze
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, SetFreeze, TestSize.Level1)
{
    std::vector<DisplayId> displayIds;
    bool ret = SingletonContainer::Get<DisplayManagerAdapter>().SetFreeze(displayIds, false);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_TRUE(ret);
    } else {
        ASSERT_FALSE(ret);
    }
}

/**
 * @tc.name: GetScreenGroupInfoById
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, GetScreenGroupInfoById, TestSize.Level1)
{
    auto info = SingletonContainer::Get<ScreenManagerAdapter>().GetScreenGroupInfoById(SCREEN_ID_INVALID);
    ASSERT_EQ(info, nullptr);
}

/**
 * @tc.name: GetScreenInfo
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, GetScreenInfo, TestSize.Level1)
{
    sptr<ScreenInfo> info = SingletonContainer::Get<ScreenManagerAdapter>().GetScreenInfo(SCREEN_ID_INVALID);
    ASSERT_EQ(info, nullptr);
}

/**
 * @tc.name: OnRemoteDied
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, OnRemoteDied, TestSize.Level1)
{
    sptr<IRemoteObject::DeathRecipient> dmsDeath_ = nullptr;
    dmsDeath_ = new(std::nothrow) DMSDeathRecipient(SingletonContainer::Get<ScreenManagerAdapter>());
    dmsDeath_->OnRemoteDied(nullptr);
    EXPECT_NE(nullptr, dmsDeath_);
}

/**
 * @tc.name: OnRemoteDied01
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, OnRemoteDied01, TestSize.Level1)
{
    sptr<IRemoteObject::DeathRecipient> dmsDeath_ = nullptr;
    dmsDeath_ = new(std::nothrow) DMSDeathRecipient(SingletonContainer::Get<ScreenManagerAdapter>());
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> obj = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    wptr<IRemoteObject> wptrDeath = obj;
    dmsDeath_->OnRemoteDied(wptrDeath);
    EXPECT_NE(nullptr, dmsDeath_);
}

/**
 * @tc.name: Clear
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, Clear, TestSize.Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    SingletonContainer::Get<ScreenManagerAdapter>().Clear();
    ASSERT_FALSE(SingletonContainer::Get<ScreenManagerAdapter>().isProxyValid_);
}

/**
 * @tc.name: Clear01
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, Clear01, TestSize.Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_ = nullptr;
    SingletonContainer::Get<ScreenManagerAdapter>().Clear();
    ASSERT_FALSE(SingletonContainer::Get<ScreenManagerAdapter>().isProxyValid_);
}

/**
 * @tc.name: DisableMirror
 * @tc.desc: DisableMirror test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, DisableMirror, TestSize.Level1)
{
    DMError ret = SingletonContainer::Get<ScreenManagerAdapter>().DisableMirror(false);
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(DMError::DM_OK, ret);
    } else {
        ASSERT_NE(DMError::DM_OK, ret);
    }
}

/**
 * @tc.name: HasImmersiveWindow
 * @tc.desc: test HasImmersiveWindow
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, HasImmersiveWindow, TestSize.Level1)
{
    bool immersive = false;
    DMError ret = SingletonContainer::Get<DisplayManagerAdapter>().HasImmersiveWindow(0u, immersive);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(ret, DMError::DM_ERROR_DEVICE_NOT_SUPPORT);
    } else {
        ASSERT_EQ(ret, DMError::DM_OK);
    }
}

/**
 * @tc.name: GetPixelFormat
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, GetPixelFormat, TestSize.Level1)
{
    GraphicPixelFormat pixelFormat = GraphicPixelFormat{GRAPHIC_PIXEL_FMT_CLUT8};
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().GetPixelFormat(0, pixelFormat);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(err, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(err, DMError::DM_OK);
    }
}

/**
 * @tc.name: SetPixelFormat
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, SetPixelFormat, TestSize.Level1)
{
    GraphicPixelFormat pixelFormat = GraphicPixelFormat{GRAPHIC_PIXEL_FMT_CLUT8};
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetPixelFormat(0, pixelFormat);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(err, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(err, DMError::DM_OK);
    }
}

/**
 * @tc.name: GetSupportedHDRFormats
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, GetSupportedHDRFormats, TestSize.Level1)
{
    std::vector<ScreenHDRFormat> hdrFormats;
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().GetSupportedHDRFormats(0, hdrFormats);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(err, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(err, DMError::DM_OK);
    }
}

/**
 * @tc.name: GetScreenHDRFormat
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, GetScreenHDRFormat, TestSize.Level1)
{
    ScreenHDRFormat hdrFormat = ScreenHDRFormat{NOT_SUPPORT_HDR};
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().GetScreenHDRFormat(0, hdrFormat);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(err, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(err, DMError::DM_OK);
    }
}

/**
 * @tc.name: SetScreenHDRFormat
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, SetScreenHDRFormat, TestSize.Level1)
{
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetScreenHDRFormat(0, 0);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(err, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(err, DMError::DM_OK);
    }
}

/**
 * @tc.name: GetSupportedColorSpaces
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, GetSupportedColorSpaces, TestSize.Level1)
{
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().GetSupportedColorSpaces(0, colorSpaces);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(err, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(err, DMError::DM_OK);
    }
}

/**
 * @tc.name: GetScreenColorSpace
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, GetScreenColorSpace, TestSize.Level1)
{
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType{GRAPHIC_CM_COLORSPACE_NONE};
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().GetScreenColorSpace(0, colorSpace);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(err, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(err, DMError::DM_OK);
    }
}

/**
 * @tc.name: SetScreenColorSpace
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, SetScreenColorSpace, TestSize.Level1)
{
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType{GRAPHIC_CM_COLORSPACE_NONE};
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetScreenColorSpace(0, colorSpace);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(err, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(err, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    }
}

/**
 * @tc.name: DestroyVirtualScreen
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, DestroyVirtualScreen, TestSize.Level1)
{
    VirtualScreenOption defaultOption = {"virtualScreen01", 480, 320, 2.0, nullptr, 0};
    ScreenId id = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption);
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().DestroyVirtualScreen(id);
    ASSERT_EQ(err, DMError::DM_OK);
}

/**
 * @tc.name: SetVirtualMirrorScreenCanvasRotation
 * @tc.desc: test SetVirtualMirrorScreenCanvasRotation
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, SetVirtualMirrorScreenCanvasRotation, TestSize.Level1)
{
    bool canvasRotation = false;
    DMError ret = SingletonContainer::Get<ScreenManagerAdapter>().
                    SetVirtualMirrorScreenCanvasRotation(0, canvasRotation);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(ret, DMError::DM_OK);
    } else {
        ASSERT_EQ(ret, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
    }
}

/**
 * @tc.name: SetScreenRotationLocked
 * @tc.desc: test SetScreenRotationLocked
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, SetScreenRotationLocked, TestSize.Level1)
{
    bool isLocked = false;
    DMError ret = SingletonContainer::Get<ScreenManagerAdapter>().SetScreenRotationLocked(isLocked);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(ret, DMError::DM_OK);
    } else {
        ASSERT_EQ(ret, DMError::DM_OK);
    }
}

/**
 * @tc.name: IsScreenRotationLocked
 * @tc.desc: test IsScreenRotationLocked
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, IsScreenRotationLocked, TestSize.Level1)
{
    bool isLocked = false;
    DMError ret = SingletonContainer::Get<ScreenManagerAdapter>().IsScreenRotationLocked(isLocked);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(ret, DMError::DM_OK);
    } else {
        ASSERT_EQ(ret, DMError::DM_OK);
    }
}

/**
 * @tc.name: SetSpecifiedScreenPower
 * @tc.desc: test SetSpecifiedScreenPower
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, SetSpecifiedScreenPower, TestSize.Level1)
{
    ScreenPowerState state = ScreenPowerState{0};
    PowerStateChangeReason reason = PowerStateChangeReason{0};
    bool ret = SingletonContainer::Get<ScreenManagerAdapter>().SetSpecifiedScreenPower(0, state, reason);
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_TRUE(ret);
    } else {
        ASSERT_FALSE(ret);
    }
}

/**
 * @tc.name: SetOrientation
 * @tc.desc: SetOrientation success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, SetOrientation, TestSize.Level1)
{
    Orientation orientation = Orientation{0};
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetOrientation(0, orientation);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(err, DMError::DM_OK);
    } else {
        ASSERT_EQ(err, DMError::DM_OK);
    }
}

/**
 * @tc.name: WakeUpBegin
 * @tc.desc: test WakeUpBegin
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, WakeUpBegin, TestSize.Level1)
{
    PowerStateChangeReason reason = PowerStateChangeReason{0};
    bool ret = SingletonContainer::Get<DisplayManagerAdapter>().WakeUpBegin(reason);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_TRUE(ret);
    } else {
        ASSERT_TRUE(ret);
    }
}

/**
 * @tc.name: WakeUpEnd
 * @tc.desc: test WakeUpEnd
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, WakeUpEnd, TestSize.Level1)
{
    bool ret = SingletonContainer::Get<DisplayManagerAdapter>().WakeUpEnd();
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_TRUE(ret);
    } else {
        ASSERT_TRUE(ret);
    }
}

/**
 * @tc.name: SuspendBegin
 * @tc.desc: test SuspendBegin
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, SuspendBegin, TestSize.Level1)
{
    PowerStateChangeReason reason = PowerStateChangeReason{0};
    bool ret = SingletonContainer::Get<DisplayManagerAdapter>().SuspendBegin(reason);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_TRUE(ret);
    } else {
        ASSERT_TRUE(ret);
    }
}

/**
 * @tc.name: SuspendEnd
 * @tc.desc: test SuspendEnd
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, SuspendEnd, TestSize.Level1)
{
    bool ret = SingletonContainer::Get<DisplayManagerAdapter>().SuspendEnd();
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_TRUE(ret);
    } else {
        ASSERT_TRUE(ret);
    }
}

/**
 * @tc.name: SetDisplayState
 * @tc.desc: test SetDisplayState
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, SetDisplayState, TestSize.Level1)
{
    DisplayState state = DisplayState{1};
    bool ret = SingletonContainer::Get<DisplayManagerAdapter>().SetDisplayState(state);
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_TRUE(ret);
    } else {
        ASSERT_FALSE(ret);
    }
}

/**
 * @tc.name: MakeMirror
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, MakeMirror, TestSize.Level1)
{
    std::vector<ScreenId> mirrorScreenId;
    ScreenId screenGroupId;
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().MakeMirror(0,
        mirrorScreenId, screenGroupId);
    ASSERT_EQ(err, DMError::DM_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: StopMirror
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, StopMirror, TestSize.Level1)
{
    std::vector<ScreenId> mirrorScreenIds;
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().StopMirror(mirrorScreenIds);
    ASSERT_EQ(err, DMError::DM_OK);
}

/**
 * @tc.name: HasPrivateWindow
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, HasPrivateWindow, TestSize.Level1)
{
    bool hasPrivateWindow = false;
    DMError err = SingletonContainer::Get<DisplayManagerAdapter>().HasPrivateWindow(0,
        hasPrivateWindow);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(DMError::DM_OK, err);
    } else {
        ASSERT_EQ(DMError::DM_OK, err);
    }
}

/**
 * @tc.name: AddSurfaceNodeToDisplay
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, AddSurfaceNodeToDisplay, TestSize.Level1)
{
    std::shared_ptr<class RSSurfaceNode> surfaceNode;
    DMError err = SingletonContainer::Get<DisplayManagerAdapter>().AddSurfaceNodeToDisplay(0,
        surfaceNode);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(DMError::DM_ERROR_IPC_FAILED, err);
    } else {
        ASSERT_EQ(DMError::DM_OK, err);
    }
}

/**
 * @tc.name: RemoveSurfaceNodeFromDisplay
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, RemoveSurfaceNodeFromDisplay, TestSize.Level1)
{
    std::shared_ptr<class RSSurfaceNode> surfaceNode;
    DMError err = SingletonContainer::Get<DisplayManagerAdapter>().RemoveSurfaceNodeFromDisplay(0,
        surfaceNode);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(DMError::DM_ERROR_IPC_FAILED, err);
    } else {
        ASSERT_EQ(DMError::DM_OK, err);
    }
}

/**
 * @tc.name: MakeExpand
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, MakeExpand, TestSize.Level1)
{
    std::vector<ScreenId> screenId;
    std::vector<Point> startPoint;
    ScreenId screenGroupId;
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().MakeExpand(screenId,
        startPoint, screenGroupId);
    ASSERT_EQ(err, DMError::DM_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: StopExpand
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, StopExpand, TestSize.Level1)
{
    std::vector<ScreenId> expandScreenIds;
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().StopExpand(expandScreenIds);
    ASSERT_EQ(err, DMError::DM_OK);
}

/**
 * @tc.name: RemoveVirtualScreenFromGroup
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, RemoveVirtualScreenFromGroup, TestSize.Level1)
{
    std::vector<ScreenId> screens;
    int resultValue = 0;
    std::function<void()> func = [&]() {
        SingletonContainer::Get<ScreenManagerAdapter>().RemoveVirtualScreenFromGroup(screens);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: SetScreenActiveMode
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, SetScreenActiveMode, TestSize.Level1)
{
    VirtualScreenOption defaultOption = {"virtualScreen02", 480, 320, 2.0, nullptr, 0};
    ScreenId id = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption);
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetScreenActiveMode(id, 100);
    ASSERT_EQ(err, DMError::DM_OK);
    SingletonContainer::Get<ScreenManagerAdapter>().DestroyVirtualScreen(id);
}

/**
 * @tc.name: SetVirtualPixelRatio
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, SetVirtualPixelRatio, TestSize.Level1)
{
    VirtualScreenOption defaultOption = {"virtualScreen03", 480, 320, 2.0, nullptr, 0};
    ScreenId id = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption);
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetVirtualPixelRatio(id, 0);
    ASSERT_EQ(err, DMError::DM_OK);
    SingletonContainer::Get<ScreenManagerAdapter>().DestroyVirtualScreen(id);
}

/**
 * @tc.name: SetResolution
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, SetResolution, TestSize.Level1)
{
    VirtualScreenOption defaultOption = {"virtualScreen04", 480, 320, 2.0, nullptr, 0};
    ScreenId id = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption);
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetResolution(id, 70, 100, 1);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(err, DMError::DM_ERROR_IPC_FAILED);
    } else {
        ASSERT_EQ(err, DMError::DM_OK);
    }
    SingletonContainer::Get<ScreenManagerAdapter>().DestroyVirtualScreen(id);
}

/**
 * @tc.name: ResizeVirtualScreen
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, ResizeVirtualScreen, TestSize.Level1)
{
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().ResizeVirtualScreen(0, 70, 100);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(err, DMError::DM_OK);
    } else {
        ASSERT_EQ(err, DMError::DM_OK);
    }
}

/**
 * @tc.name: MakeUniqueScreen
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, MakeUniqueScreen, TestSize.Level1)
{
    std::vector<ScreenId> screenIds;
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().MakeUniqueScreen(screenIds);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(err, DMError::DM_OK);
    } else {
        ASSERT_EQ(err, DMError::DM_ERROR_INVALID_PARAM);
    }
}

/**
 * @tc.name: GetAvailableArea
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, GetAvailableArea, TestSize.Level1)
{
    DMRect area;
    DMError err = SingletonContainer::Get<DisplayManagerAdapter>().GetAvailableArea(0, area);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(err, DMError::DM_ERROR_DEVICE_NOT_SUPPORT);
    } else {
        ASSERT_EQ(err, DMError::DM_OK);
    }
}

/**
 * @tc.name: GetAllDisplayPhysicalResolution
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, GetAllDisplayPhysicalResolution, TestSize.Level1)
{
    std::vector<DisplayPhysicalResolution> allSize =
        SingletonContainer::Get<DisplayManagerAdapter>().GetAllDisplayPhysicalResolution();
    ASSERT_TRUE(!allSize.empty());
}

/**
 * @tc.name: SetDisplayScale
 * @tc.desc: SetDisplayScale test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, SetDisplayScale, TestSize.Level1)
{
    DisplayManagerAdapter& displayManagerAdapter = SingletonContainer::Get<DisplayManagerAdapter>();
    const float scaleX = 1.0f;
    const float scaleY = 1.0f;
    const float pivotX = 0.5f;
    const float pivotY = 0.5f;
    sptr<DisplayInfo> displayInfo = displayManagerAdapter.GetDefaultDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    ScreenId screenId = displayInfo->GetScreenId();
    displayManagerAdapter.SetDisplayScale(screenId, scaleX, scaleY, pivotX, pivotY);
}

/**
 * @tc.name: GetPrimaryDisplayInfo
 * @tc.desc: GetPrimaryDisplayInfo test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterUtTest, GetPrimaryDisplayInfo, TestSize.Level1)
{
    DisplayManagerAdapter& displayManagerAdapter = SingletonContainer::Get<DisplayManagerAdapter>();
    sptr<DisplayInfo> displayInfo = displayManagerAdapter.GetPrimaryDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
}
}
}
}
