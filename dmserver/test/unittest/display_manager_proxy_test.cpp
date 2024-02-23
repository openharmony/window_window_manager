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

#include <iremote_broker.h>
#include <iremote_object.h>
#include "display_manager_agent_default.h"
#include "display_manager_proxy.h"
#include "iremote_object_mocker.h"

#include "iconsumer_surface.h"
#include <surface.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using RemoteMocker = MockIRemoteObject;
class DisplayManagerProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DisplayManagerProxyTest::SetUpTestCase()
{
}

void DisplayManagerProxyTest::TearDownTestCase()
{
}

void DisplayManagerProxyTest::SetUp()
{
}

void DisplayManagerProxyTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetDefaultDisplayInfo
 * @tc.desc: test DisplayManagerProxy::GetDefaultDisplayInfo
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetDefaultDisplayInfo01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    ASSERT_EQ(nullptr, proxy1.remoteObject_);
    auto displayInfo1 = proxy1.GetDefaultDisplayInfo();
    ASSERT_EQ(nullptr, displayInfo1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    ASSERT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    auto displayInfo2 = proxy2.GetDefaultDisplayInfo();
    ASSERT_EQ(nullptr, displayInfo2);

    remoteMocker->sendRequestResult_ = 1;
    auto displayInfo3 = proxy2.GetDefaultDisplayInfo();
    ASSERT_EQ(nullptr, displayInfo3);
}
/**
 * @tc.name: GetDisplayInfoById01
 * @tc.desc: test DisplayManagerProxy::GetDisplayInfoById
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetDisplayInfoById01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    ASSERT_EQ(nullptr, proxy1.remoteObject_);
    auto displayInfo1 = proxy1.GetDisplayInfoById(0);
    ASSERT_EQ(nullptr, displayInfo1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    ASSERT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);

    auto displayInfo2 = proxy2.GetDisplayInfoById(0);
    ASSERT_EQ(nullptr, displayInfo2);

    remoteMocker->sendRequestResult_ = 1;
    auto displayInfo3 = proxy2.GetDisplayInfoById(0);
    ASSERT_EQ(nullptr, displayInfo3);
}
/**
 * @tc.name: GetDisplayInfoByScreen01
 * @tc.desc: test DisplayManagerProxy::GetDisplayInfoByScreen
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetDisplayInfoByScreen01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    ASSERT_EQ(nullptr, proxy1.remoteObject_);
    auto displayInfo1 = proxy1.GetDisplayInfoByScreen(0);
    ASSERT_EQ(nullptr, displayInfo1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    ASSERT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);

    auto displayInfo2 = proxy2.GetDisplayInfoByScreen(0);
    ASSERT_EQ(nullptr, displayInfo2);

    remoteMocker->sendRequestResult_ = 1;
    auto displayInfo3 = proxy2.GetDisplayInfoByScreen(0);
    ASSERT_EQ(nullptr, displayInfo3);
}
/**
 * @tc.name: CreateVirtualScreen01
 * @tc.desc: test DisplayManagerProxy::CreateVirtualScreen
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, CreateVirtualScreen01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    ASSERT_EQ(nullptr, proxy1.remoteObject_);
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "testVirtualOption";
    sptr<IRemoteObject> displayManagerAgent1 = new RemoteMocker();
    auto screenId1 = proxy1.CreateVirtualScreen(virtualOption1, displayManagerAgent1);
    ASSERT_EQ(SCREEN_ID_INVALID, screenId1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    ASSERT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);

    VirtualScreenOption virtualOption2;
    virtualOption2.name_ = "testVirtualOption";
    sptr<IRemoteObject> displayManagerAgent2 = new RemoteMocker();
    auto screenId2 = proxy2.CreateVirtualScreen(virtualOption2, displayManagerAgent2);
    ASSERT_EQ(0, screenId2);

    remoteMocker->sendRequestResult_ = 1;
    auto screenId3 = proxy2.CreateVirtualScreen(virtualOption2, displayManagerAgent2);
    ASSERT_EQ(SCREEN_ID_INVALID, screenId3);
}
/**
 * @tc.name: DestroyVirtualScreen01
 * @tc.desc: test DisplayManagerProxy::DestroyVirtualScreen
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, DestroyVirtualScreen01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    ASSERT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.DestroyVirtualScreen(0);
    ASSERT_EQ(DMError::DM_ERROR_REMOTE_CREATE_FAILED, result1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    ASSERT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);

    auto result2 = proxy2.DestroyVirtualScreen(0);
    ASSERT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.DestroyVirtualScreen(0);
    ASSERT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}
/**
 * @tc.name: SetVirtualScreenSurface01
 * @tc.desc: test DisplayManagerProxy::SetVirtualScreenSurface
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetVirtualScreenSurface01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    ASSERT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.SetVirtualScreenSurface(0, nullptr);
    ASSERT_EQ(DMError::DM_ERROR_REMOTE_CREATE_FAILED, result1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    ASSERT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);

    auto result2 = proxy2.SetVirtualScreenSurface(0, nullptr);
    ASSERT_EQ(DMError::DM_OK, result2);
    sptr<IConsumerSurface> surface = OHOS::IConsumerSurface::Create();
    auto result3 = proxy2.SetVirtualScreenSurface(0, surface->GetProducer());
    ASSERT_EQ(DMError::DM_OK, result3);

    remoteMocker->sendRequestResult_ = 1;
    auto result4 = proxy2.SetVirtualScreenSurface(0, surface->GetProducer());
    ASSERT_EQ(DMError::DM_ERROR_IPC_FAILED, result4);
}
/**
 * @tc.name: SetOrientation01
 * @tc.desc: test DisplayManagerProxy::SetOrientation
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetOrientation01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    ASSERT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.SetOrientation(0, Orientation::VERTICAL);
    ASSERT_TRUE(DMError::DM_OK != result1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    ASSERT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);

    auto result2 = proxy2.SetOrientation(0, Orientation::VERTICAL);
    ASSERT_TRUE(DMError::DM_OK == result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.SetOrientation(0, Orientation::VERTICAL);
    ASSERT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}
/**
 * @tc.name: GetDisplaySnapshot01
 * @tc.desc: test DisplayManagerProxy::GetDisplaySnapshot
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetDisplaySnapshot01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    ASSERT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.GetDisplaySnapshot(0);
    ASSERT_EQ(nullptr, result1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    ASSERT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);

    auto result2 = proxy2.GetDisplaySnapshot(0);
    ASSERT_EQ(nullptr, result2);
    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.GetDisplaySnapshot(0);
    ASSERT_EQ(nullptr, result3);
}
/**
 * @tc.name: GetScreenSupportedColorGamuts01
 * @tc.desc: test DisplayManagerProxy::GetScreenSupportedColorGamuts
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetScreenSupportedColorGamuts01, Function | SmallTest | Level1)
{
    std::vector<ScreenColorGamut> gamutVector;
    DisplayManagerProxy proxy1(nullptr);
    ASSERT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.GetScreenSupportedColorGamuts(0, gamutVector);
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    gamutVector.clear();

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    ASSERT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    auto result2 = proxy2.GetScreenSupportedColorGamuts(0, gamutVector);
    ASSERT_EQ(DMError::DM_OK, result2);
    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.GetScreenSupportedColorGamuts(0, gamutVector);
    ASSERT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}
/**
 * @tc.name: GetScreenColorGamut01
 * @tc.desc: test DisplayManagerProxy::GetScreenColorGamut
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetScreenColorGamut01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    ASSERT_EQ(nullptr, proxy1.remoteObject_);
    ScreenColorGamut screenColorGamut;
    auto result1 = proxy1.GetScreenColorGamut(0, screenColorGamut);
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, result1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    ASSERT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    screenColorGamut = ScreenColorGamut::COLOR_GAMUT_ADOBE_RGB;
    auto result2 = proxy2.GetScreenColorGamut(0, screenColorGamut);
    ASSERT_EQ(DMError::DM_OK, result2);
    ASSERT_EQ(ScreenColorGamut::COLOR_GAMUT_NATIVE, screenColorGamut);

    screenColorGamut = ScreenColorGamut::COLOR_GAMUT_ADOBE_RGB;
    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.GetScreenColorGamut(0, screenColorGamut);
    ASSERT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
    ASSERT_EQ(ScreenColorGamut::COLOR_GAMUT_ADOBE_RGB, screenColorGamut);
}

/**
 * @tc.name: SetScreenColorGamut01
 * @tc.desc: test DisplayManagerProxy::SetScreenColorGamut
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetScreenColorGamut01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.SetScreenColorGamut(0, 3);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    auto result2 = proxy2.SetScreenColorGamut(0, 3);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.SetScreenColorGamut(0, 3);
    ASSERT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: GetScreenGamutMap01
 * @tc.desc: test DisplayManagerProxy::GetScreenGamutMap
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetScreenGamutMap01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);
    ScreenGamutMap gamutMap = ScreenGamutMap::GAMUT_MAP_HDR_EXTENSION;
    auto result1 = proxy1.GetScreenGamutMap(0, gamutMap);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    gamutMap = ScreenGamutMap::GAMUT_MAP_HDR_EXTENSION;
    auto result2 = proxy2.GetScreenGamutMap(0, gamutMap);
    EXPECT_EQ(DMError::DM_OK, result2);
    EXPECT_EQ(ScreenGamutMap::GAMUT_MAP_CONSTANT, gamutMap);

    gamutMap = ScreenGamutMap::GAMUT_MAP_HDR_EXTENSION;
    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.GetScreenGamutMap(0, gamutMap);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
    EXPECT_EQ(ScreenGamutMap::GAMUT_MAP_HDR_EXTENSION, gamutMap);
}

/**
 * @tc.name: SetScreenGamutMap01
 * @tc.desc: test DisplayManagerProxy::SetScreenGamutMap
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetScreenGamutMap01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);
    ScreenGamutMap gamutMap = ScreenGamutMap::GAMUT_MAP_HDR_EXTENSION;
    auto result1 = proxy1.SetScreenGamutMap(0, gamutMap);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    gamutMap = ScreenGamutMap::GAMUT_MAP_HDR_EXTENSION;
    auto result2 = proxy2.SetScreenGamutMap(0, gamutMap);
    EXPECT_EQ(DMError::DM_OK, result2);
    EXPECT_EQ(ScreenGamutMap::GAMUT_MAP_HDR_EXTENSION, gamutMap);

    gamutMap = ScreenGamutMap::GAMUT_MAP_HDR_EXTENSION;
    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.SetScreenGamutMap(0, gamutMap);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
    EXPECT_EQ(ScreenGamutMap::GAMUT_MAP_HDR_EXTENSION, gamutMap);
}

/**
 * @tc.name: SetScreenColorTransform01
 * @tc.desc: test DisplayManagerProxy::SetScreenColorTransform
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetScreenColorTransform01, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    auto result2 = proxy2.SetScreenColorTransform(0);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.SetScreenColorTransform(0);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: RegisterDisplayManagerAgent01
 * @tc.desc: test DisplayManagerProxy::RegisterDisplayManagerAgent
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, RegisterDisplayManagerAgent01, Function | SmallTest | Level1)
{
    sptr<IRemoteObject> iRemoteObject = new IRemoteObjectMocker();
    DisplayManagerProxy proxy1(iRemoteObject);
    EXPECT_NE(nullptr, proxy1.remoteObject_);
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    DisplayManagerAgentType type = DisplayManagerAgentType::SCREENSHOT_EVENT_LISTENER;
    DMError result01 = proxy1.RegisterDisplayManagerAgent(displayManagerAgent, type);
    EXPECT_EQ(result01, DMError::DM_OK);
}

/**
 * @tc.name: UnregisterDisplayManagerAgent01
 * @tc.desc: test DisplayManagerProxy::UnregisterDisplayManagerAgent
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, UnregisterDisplayManagerAgent01, Function | SmallTest | Level1)
{
    sptr<IRemoteObject> iRemoteObject = new IRemoteObjectMocker();
    DisplayManagerProxy proxy1(iRemoteObject);
    EXPECT_NE(nullptr, proxy1.remoteObject_);
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    DisplayManagerAgentType type = DisplayManagerAgentType::SCREENSHOT_EVENT_LISTENER;
    DMError result01 = proxy1.UnregisterDisplayManagerAgent(displayManagerAgent, type);
    EXPECT_EQ(result01, DMError::DM_OK);
}

/**
 * @tc.name: WakeUpBegin01
 * @tc.desc: test DisplayManagerProxy::WakeUpBegin
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, WakeUpBegin01, Function | SmallTest | Level1)
{
    sptr<IRemoteObject> iRemoteObject = new IRemoteObjectMocker();
    DisplayManagerProxy proxy1(iRemoteObject);
    EXPECT_NE(nullptr, proxy1.remoteObject_);
    PowerStateChangeReason reason = PowerStateChangeReason::POWER_BUTTON;
    bool result01 = proxy1.WakeUpBegin(reason);
    EXPECT_EQ(result01, false);
}

/**
 * @tc.name: WakeUpEnd01
 * @tc.desc: test DisplayManagerProxy::WakeUpEnd
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, WakeUpEnd01, Function | SmallTest | Level1)
{
    sptr<IRemoteObject> iRemoteObject = new IRemoteObjectMocker();
    DisplayManagerProxy proxy1(iRemoteObject);
    EXPECT_NE(nullptr, proxy1.remoteObject_);
    bool result01 = proxy1.WakeUpEnd();
    EXPECT_EQ(result01, false);
}

/**
 * @tc.name: GetPixelFormat
 * @tc.desc: test DisplayManagerProxy::GetPixelFormat
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetPixelFormat, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy.remoteObject_);
    GraphicPixelFormat pixelFormat = GraphicPixelFormat{GRAPHIC_PIXEL_FMT_CLUT1};
    auto result = proxy.GetPixelFormat(0, pixelFormat);
    EXPECT_EQ(DMError::DM_OK, result);
}

/**
 * @tc.name: SetPixelFormat
 * @tc.desc: test DisplayManagerProxy::SetPixelFormat
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetPixelFormat, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy.remoteObject_);
    GraphicPixelFormat pixelFormat = GraphicPixelFormat{GRAPHIC_PIXEL_FMT_CLUT1};
    auto result = proxy.SetPixelFormat(0, pixelFormat);
    EXPECT_EQ(DMError::DM_OK, result);
}

/**
 * @tc.name: GetSupportedHDRFormats
 * @tc.desc: test DisplayManagerProxy::GetSupportedHDRFormats
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetSupportedHDRFormats, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy.remoteObject_);
    std::vector<ScreenHDRFormat> hdrFormats;
    auto result = proxy.GetSupportedHDRFormats(0, hdrFormats);
    EXPECT_EQ(DMError::DM_OK, result);
}

/**
 * @tc.name: SetScreenHDRFormat
 * @tc.desc: test DisplayManagerProxy::SetScreenHDRFormat
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetScreenHDRFormat, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy.remoteObject_);
    auto result = proxy.SetScreenHDRFormat(0, 0);
    EXPECT_EQ(DMError::DM_OK, result);
}

/**
 * @tc.name: GetScreenHDRFormat
 * @tc.desc: test DisplayManagerProxy::GetScreenHDRFormat
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetScreenHDRFormat, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy.remoteObject_);
    ScreenHDRFormat hdrFormats = ScreenHDRFormat{VIDEO_HLG};
    auto result = proxy.GetScreenHDRFormat(0, hdrFormats);
    EXPECT_EQ(DMError::DM_OK, result);
}

/**
 * @tc.name: GetSupportedColorSpaces
 * @tc.desc: test DisplayManagerProxy::GetSupportedColorSpaces
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetSupportedColorSpaces, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy.remoteObject_);
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    auto result = proxy.GetSupportedColorSpaces(0, colorSpaces);
    EXPECT_EQ(DMError::DM_OK, result);
}

/**
 * @tc.name: GetScreenColorSpace
 * @tc.desc: test DisplayManagerProxy::GetScreenColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetScreenColorSpace, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy.remoteObject_);
    GraphicCM_ColorSpaceType colorSpaces;
    auto result = proxy.GetScreenColorSpace(0, colorSpaces);
    EXPECT_EQ(DMError::DM_OK, result);
}

/**
 * @tc.name: SetScreenColorSpace
 * @tc.desc: test DisplayManagerProxy::SetScreenColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetScreenColorSpace, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy.remoteObject_);
    GraphicCM_ColorSpaceType colorSpaces = GraphicCM_ColorSpaceType{GRAPHIC_CM_BT601_EBU_FULL};
    auto result = proxy.SetScreenColorSpace(0, colorSpaces);
    EXPECT_EQ(DMError::DM_OK, result);
}

/**
 * @tc.name: SuspendBegin
 * @tc.desc: test DisplayManagerProxy::SuspendBegin
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SuspendBegin, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    PowerStateChangeReason reason = PowerStateChangeReason{0};
    auto result = proxy.SuspendBegin(reason);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: SuspendEnd
 * @tc.desc: test DisplayManagerProxy::SuspendEnd
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SuspendEnd, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    auto result = proxy.SuspendEnd();
    ASSERT_FALSE(result);
}

/**
 * @tc.name: SetScreenPowerForAll
 * @tc.desc: test DisplayManagerProxy::SetScreenPowerForAll
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetScreenPowerForAll, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    ScreenPowerState state = ScreenPowerState{0};
    PowerStateChangeReason reason = PowerStateChangeReason{0};
    auto result = proxy.SetScreenPowerForAll(state, reason);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: SetSpecifiedScreenPower
 * @tc.desc: test DisplayManagerProxy::SetSpecifiedScreenPower
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetSpecifiedScreenPower, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    ScreenPowerState state = ScreenPowerState{0};
    PowerStateChangeReason reason = PowerStateChangeReason{0};
    auto result = proxy.SetSpecifiedScreenPower(0, state, reason);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: SetDisplayState
 * @tc.desc: test DisplayManagerProxy::SetDisplayState
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetDisplayState, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    DisplayState state = DisplayState{0};
    auto result = proxy.SetDisplayState(state);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: AddSurfaceNodeToDisplay
 * @tc.desc: test DisplayManagerProxy::AddSurfaceNodeToDisplay
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, AddSurfaceNodeToDisplay, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy.remoteObject_);
    std::shared_ptr<class RSSurfaceNode> surfaceNode;
    auto result = proxy.AddSurfaceNodeToDisplay(0, surfaceNode, true);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result);
}

/**
 * @tc.name: RemoveSurfaceNodeFromDisplay
 * @tc.desc: test DisplayManagerProxy::RemoveSurfaceNodeFromDisplay
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, RemoveSurfaceNodeFromDisplay, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy.remoteObject_);
    std::shared_ptr<class RSSurfaceNode> surfaceNode;
    auto result = proxy.RemoveSurfaceNodeFromDisplay(0, surfaceNode);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result);
}

/**
 * @tc.name: HasPrivateWindow
 * @tc.desc: test DisplayManagerProxy::HasPrivateWindow
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, HasPrivateWindow, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy.remoteObject_);
    bool hasPrivateWindow = true;
    auto result = proxy.HasPrivateWindow(0, hasPrivateWindow);
    EXPECT_EQ(DMError::DM_OK, result);
}

/**
 * @tc.name: SetFreeze
 * @tc.desc: test DisplayManagerProxy::SetFreeze
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetFreeze, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    DisplayEvent event = DisplayEvent{0};
    proxy.NotifyDisplayEvent(event);

    std::vector<DisplayId> displayIds;
    auto result = proxy.SetFreeze(displayIds, true);
    ASSERT_TRUE(result);
}
}
} // namespace Rosen
} // namespace OHOS