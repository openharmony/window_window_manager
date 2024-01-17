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
#include "display_manager_proxy.h"
#include "window_scene.h"
#include "scene_board_judgement.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DisplayManagerAdapterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DisplayManagerAdapterTest::SetUpTestCase()
{
}

void DisplayManagerAdapterTest::TearDownTestCase()
{
}

void DisplayManagerAdapterTest::SetUp()
{
}

void DisplayManagerAdapterTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetDisplayInfo
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, GetDisplayInfo, Function | SmallTest | Level2)
{
    sptr<DisplayInfo> info = SingletonContainer::Get<DisplayManagerAdapter>().GetDisplayInfo(DISPLAY_ID_INVALID);
    ASSERT_EQ(info, nullptr);
}

/**
 * @tc.name: GetCutoutInfo
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, GetCutoutInfo, Function | SmallTest | Level2)
{
    sptr<CutoutInfo> info = SingletonContainer::Get<DisplayManagerAdapter>().GetCutoutInfo(DISPLAY_ID_INVALID);
    ASSERT_EQ(info, nullptr);
}

/**
 * @tc.name: GetScreenSupportedColorGamuts
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, GetScreenSupportedColorGamuts, Function | SmallTest | Level2)
{
    std::vector<ScreenColorGamut> colorGamuts;
    SingletonContainer::Get<ScreenManagerAdapter>().GetScreenSupportedColorGamuts(0, colorGamuts);
}

/**
 * @tc.name: SetScreenColorGamut
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, SetScreenColorGamut, Function | SmallTest | Level2)
{
    std::vector<ScreenColorGamut> colorGamuts;
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetScreenColorGamut(0, -1);
    ASSERT_EQ(err, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
}

/**
 * @tc.name: GetScreenColorGamut
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, GetScreenColorGamut, Function | SmallTest | Level2)
{
    ScreenColorGamut colorGamut;
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().GetScreenColorGamut(0, colorGamut);
    ASSERT_EQ(err, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
}

/**
 * @tc.name: GetScreenGamutMap
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, GetScreenGamutMap, Function | SmallTest | Level2)
{
    ScreenGamutMap gamutMap;
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().GetScreenGamutMap(0, gamutMap);
    ASSERT_EQ(err, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
}

/**
 * @tc.name: SetScreenGamutMap
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, SetScreenGamutMap, Function | SmallTest | Level2)
{
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetScreenGamutMap(0, GAMUT_MAP_CONSTANT);
    ASSERT_EQ(err, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
}

/**
 * @tc.name: SetScreenColorTransform
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, SetScreenColorTransform, Function | SmallTest | Level2)
{
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetScreenColorTransform(0);
    ASSERT_EQ(err, DMError::DM_OK);
}

/**
 * @tc.name: SetFreeze
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, SetFreeze, Function | SmallTest | Level2)
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
HWTEST_F(DisplayManagerAdapterTest, GetScreenGroupInfoById, Function | SmallTest | Level2)
{
    auto info = SingletonContainer::Get<ScreenManagerAdapter>().GetScreenGroupInfoById(SCREEN_ID_INVALID);
    ASSERT_EQ(info, nullptr);
}

/**
 * @tc.name: GetScreenInfo
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, GetScreenInfo, Function | SmallTest | Level2)
{
    sptr<ScreenInfo> info = SingletonContainer::Get<ScreenManagerAdapter>().GetScreenInfo(SCREEN_ID_INVALID);
    ASSERT_EQ(info, nullptr);
}

/**
 * @tc.name: OnRemoteDied
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, OnRemoteDied, Function | SmallTest | Level2)
{
    sptr<IRemoteObject::DeathRecipient> dmsDeath_ = nullptr;
    dmsDeath_ = new(std::nothrow) DMSDeathRecipient(SingletonContainer::Get<ScreenManagerAdapter>());
    dmsDeath_->OnRemoteDied(nullptr);
}

/**
 * @tc.name: OnRemoteDied01
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, OnRemoteDied01, Function | SmallTest | Level2)
{
    sptr<IRemoteObject::DeathRecipient> dmsDeath_ = nullptr;
    dmsDeath_ = new(std::nothrow) DMSDeathRecipient(SingletonContainer::Get<ScreenManagerAdapter>());
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> obj = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    wptr<IRemoteObject> wptrDeath = obj;
    dmsDeath_->OnRemoteDied(wptrDeath);
}

/**
 * @tc.name: Clear
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, Clear, Function | SmallTest | Level2)
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
HWTEST_F(DisplayManagerAdapterTest, Clear01, Function | SmallTest | Level2)
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
HWTEST_F(DisplayManagerAdapterTest, DisableMirror, Function | SmallTest | Level2)
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
HWTEST_F(DisplayManagerAdapterTest, HasImmersiveWindow, Function | SmallTest | Level2)
{
    bool immersive = false;
    DMError ret = SingletonContainer::Get<DisplayManagerAdapter>().HasImmersiveWindow(immersive);
    ASSERT_EQ(ret, DMError::DM_ERROR_DEVICE_NOT_SUPPORT);
}

/**
 * @tc.name: GetPixelFormat
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, GetPixelFormat, Function | SmallTest | Level2)
{
    GraphicPixelFormat pixelFormat = GraphicPixelFormat{GRAPHIC_PIXEL_FMT_CLUT8};
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().GetPixelFormat(0, pixelFormat);
    ASSERT_EQ(err, DMError::DM_ERROR_IPC_FAILED);
}

/**
 * @tc.name: SetPixelFormat
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, SetPixelFormat, Function | SmallTest | Level2)
{
    GraphicPixelFormat pixelFormat = GraphicPixelFormat{GRAPHIC_PIXEL_FMT_CLUT8};
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetPixelFormat(0, pixelFormat);
    ASSERT_EQ(err, DMError::DM_ERROR_IPC_FAILED);
}

/**
 * @tc.name: GetSupportedHDRFormats
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, GetSupportedHDRFormats, Function | SmallTest | Level2)
{
    std::vector<ScreenHDRFormat> hdrFormats;
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().GetSupportedHDRFormats(0, hdrFormats);
    ASSERT_EQ(err, DMError::DM_ERROR_IPC_FAILED);
}

/**
 * @tc.name: GetScreenHDRFormat
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, GetScreenHDRFormat, Function | SmallTest | Level2)
{
    ScreenHDRFormat hdrFormat = ScreenHDRFormat{NOT_SUPPORT_HDR};
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().GetScreenHDRFormat(0, hdrFormat);
    ASSERT_EQ(err, DMError::DM_ERROR_IPC_FAILED);
}

/**
 * @tc.name: SetScreenHDRFormat
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, SetScreenHDRFormat, Function | SmallTest | Level2)
{
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetScreenHDRFormat(0, 0);
    ASSERT_EQ(err, DMError::DM_ERROR_IPC_FAILED);
}

/**
 * @tc.name: GetSupportedColorSpaces
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, GetSupportedColorSpaces, Function | SmallTest | Level2)
{
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().GetSupportedColorSpaces(0, colorSpaces);
    ASSERT_EQ(err, DMError::DM_ERROR_IPC_FAILED);
}

/**
 * @tc.name: GetScreenColorSpace
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, GetScreenColorSpace, Function | SmallTest | Level2)
{
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType{GRAPHIC_CM_COLORSPACE_NONE};
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().GetScreenColorSpace(0, colorSpace);
    ASSERT_EQ(err, DMError::DM_ERROR_IPC_FAILED);
}

/**
 * @tc.name: SetScreenColorSpace
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, SetScreenColorSpace, Function | SmallTest | Level2)
{
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType{GRAPHIC_CM_COLORSPACE_NONE};
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetScreenColorSpace(0, colorSpace);
    ASSERT_EQ(err, DMError::DM_ERROR_IPC_FAILED);
}

/**
 * @tc.name: DestroyVirtualScreen
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, DestroyVirtualScreen, Function | SmallTest | Level2)
{
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().DestroyVirtualScreen(0);
    ASSERT_EQ(err, DMError::DM_ERROR_INVALID_CALLING);
}

/**
 * @tc.name: SetVirtualMirrorScreenCanvasRotation
 * @tc.desc: test SetVirtualMirrorScreenCanvasRotation
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, SetVirtualMirrorScreenCanvasRotation, Function | SmallTest | Level2)
{
    bool canvasRotation = false;
    DMError ret = SingletonContainer::Get<ScreenManagerAdapter>().
                    SetVirtualMirrorScreenCanvasRotation(0, canvasRotation);
    ASSERT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: SetScreenRotationLocked
 * @tc.desc: test SetScreenRotationLocked
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, SetScreenRotationLocked, Function | SmallTest | Level2)
{
    bool isLocked = false;
    DMError ret = SingletonContainer::Get<ScreenManagerAdapter>().SetScreenRotationLocked(isLocked);
    ASSERT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: IsScreenRotationLocked
 * @tc.desc: test IsScreenRotationLocked
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, IsScreenRotationLocked, Function | SmallTest | Level2)
{
    bool isLocked = false;
    DMError ret = SingletonContainer::Get<ScreenManagerAdapter>().IsScreenRotationLocked(isLocked);
    ASSERT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: SetSpecifiedScreenPower
 * @tc.desc: test SetSpecifiedScreenPower
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, SetSpecifiedScreenPower, Function | SmallTest | Level2)
{
    ScreenPowerState state = ScreenPowerState{0};
    PowerStateChangeReason reason = PowerStateChangeReason{0};
    bool ret = SingletonContainer::Get<ScreenManagerAdapter>().SetSpecifiedScreenPower(0, state, reason);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: SetOrientation
 * @tc.desc: SetOrientation success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, SetOrientation, Function | SmallTest | Level2)
{
    Orientation orientation = Orientation{0};
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetOrientation(0, orientation);
    ASSERT_EQ(err, DMError::DM_OK);
}

/**
 * @tc.name: WakeUpBegin
 * @tc.desc: test WakeUpBegin
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, WakeUpBegin, Function | SmallTest | Level2)
{
    PowerStateChangeReason reason = PowerStateChangeReason{0};
    bool ret = SingletonContainer::Get<DisplayManagerAdapter>().WakeUpBegin(reason);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: WakeUpEnd
 * @tc.desc: test WakeUpEnd
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, WakeUpEnd, Function | SmallTest | Level2)
{
    bool ret = SingletonContainer::Get<DisplayManagerAdapter>().WakeUpEnd();
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: SuspendBegin
 * @tc.desc: test SuspendBegin
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, SuspendBegin, Function | SmallTest | Level2)
{
    PowerStateChangeReason reason = PowerStateChangeReason{0};
    bool ret = SingletonContainer::Get<DisplayManagerAdapter>().SuspendBegin(reason);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: SuspendEnd
 * @tc.desc: test SuspendEnd
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, SuspendEnd, Function | SmallTest | Level2)
{
    bool ret = SingletonContainer::Get<DisplayManagerAdapter>().SuspendEnd();
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: SetDisplayState
 * @tc.desc: test SetDisplayState
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, SetDisplayState, Function | SmallTest | Level2)
{
    DisplayState state = DisplayState{1};
    bool ret = SingletonContainer::Get<DisplayManagerAdapter>().SetDisplayState(state);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: MakeMirror
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, MakeMirror, Function | SmallTest | Level2)
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
HWTEST_F(DisplayManagerAdapterTest, StopMirror, Function | SmallTest | Level2)
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
HWTEST_F(DisplayManagerAdapterTest, HasPrivateWindow, Function | SmallTest | Level2)
{
    bool hasPrivateWindow = false;
    DMError err = SingletonContainer::Get<DisplayManagerAdapter>().HasPrivateWindow(0,
        hasPrivateWindow);
    ASSERT_EQ(err, DMError::DM_OK);
}

/**
 * @tc.name: AddSurfaceNodeToDisplay
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, AddSurfaceNodeToDisplay, Function | SmallTest | Level2)
{
    std::shared_ptr<class RSSurfaceNode> surfaceNode;
    DMError err = SingletonContainer::Get<DisplayManagerAdapter>().AddSurfaceNodeToDisplay(0,
        surfaceNode);
    ASSERT_EQ(err, DMError::DM_ERROR_IPC_FAILED);
}

/**
 * @tc.name: RemoveSurfaceNodeFromDisplay
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, RemoveSurfaceNodeFromDisplay, Function | SmallTest | Level2)
{
    std::shared_ptr<class RSSurfaceNode> surfaceNode;
    DMError err = SingletonContainer::Get<DisplayManagerAdapter>().RemoveSurfaceNodeFromDisplay(0,
        surfaceNode);
    ASSERT_EQ(err, DMError::DM_ERROR_IPC_FAILED);
}

/**
 * @tc.name: MakeExpand
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, MakeExpand, Function | SmallTest | Level2)
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
HWTEST_F(DisplayManagerAdapterTest, StopExpand, Function | SmallTest | Level2)
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
HWTEST_F(DisplayManagerAdapterTest, RemoveVirtualScreenFromGroup, Function | SmallTest | Level2)
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
HWTEST_F(DisplayManagerAdapterTest, SetScreenActiveMode, Function | SmallTest | Level2)
{
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetScreenActiveMode(0, 100);
    ASSERT_EQ(err, DMError::DM_OK);
}

/**
 * @tc.name: SetVirtualPixelRatio
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, SetVirtualPixelRatio, Function | SmallTest | Level2)
{
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetVirtualPixelRatio(0, 0);
    ASSERT_EQ(err, DMError::DM_OK);
}

/**
 * @tc.name: SetResolution
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, SetResolution, Function | SmallTest | Level2)
{
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetResolution(0, 70, 100, 50);
    ASSERT_EQ(err, DMError::DM_ERROR_IPC_FAILED);
}

/**
 * @tc.name: ResizeVirtualScreen
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, ResizeVirtualScreen, Function | SmallTest | Level2)
{
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().ResizeVirtualScreen(0, 70, 100);
    ASSERT_EQ(err, DMError::DM_OK);
}

/**
 * @tc.name: MakeUniqueScreen
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, MakeUniqueScreen, Function | SmallTest | Level2)
{
    std::vector<ScreenId> screenIds;
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().MakeUniqueScreen(screenIds);
    ASSERT_EQ(err, DMError::DM_OK);
}

/**
 * @tc.name: GetAvailableArea
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, GetAvailableArea, Function | SmallTest | Level2)
{
    DMRect area;
    DMError err = SingletonContainer::Get<DisplayManagerAdapter>().GetAvailableArea(0, area);
    ASSERT_EQ(err, DMError::DM_ERROR_DEVICE_NOT_SUPPORT);
}
}
}
}
