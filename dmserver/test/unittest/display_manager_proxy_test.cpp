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
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.SetScreenColorTransform(0);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
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
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy1(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy1.remoteObject_);

    PowerStateChangeReason reason = PowerStateChangeReason::POWER_BUTTON;
    auto result1 = proxy1.WakeUpBegin(reason);
    EXPECT_EQ(result1, false);

    remoteMocker->sendRequestResult_ = 1;
    auto result2 = proxy1.WakeUpBegin(reason);
    EXPECT_EQ(result2, false);
}

/**
 * @tc.name: WakeUpEnd01
 * @tc.desc: test DisplayManagerProxy::WakeUpEnd
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, WakeUpEnd01, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy1(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy1.remoteObject_);

    auto result1 = proxy1.WakeUpEnd();
    EXPECT_EQ(result1, false);

    remoteMocker->sendRequestResult_ = 1;
    auto result2 = proxy1.WakeUpEnd();
    EXPECT_EQ(result2, false);
}

/**
 * @tc.name: GetPixelFormat
 * @tc.desc: test DisplayManagerProxy::GetPixelFormat
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetPixelFormat, Function | SmallTest | Level1)
{
    GraphicPixelFormat pixelFormat = GraphicPixelFormat{GRAPHIC_PIXEL_FMT_CLUT1};
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.GetPixelFormat(0, pixelFormat);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.GetPixelFormat(0, pixelFormat);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.GetPixelFormat(0, pixelFormat);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: SetPixelFormat
 * @tc.desc: test DisplayManagerProxy::SetPixelFormat
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetPixelFormat, Function | SmallTest | Level1)
{
    GraphicPixelFormat pixelFormat = GraphicPixelFormat{GRAPHIC_PIXEL_FMT_CLUT1};
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.SetPixelFormat(0, pixelFormat);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.SetPixelFormat(0, pixelFormat);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.SetPixelFormat(0, pixelFormat);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: GetSupportedHDRFormats
 * @tc.desc: test DisplayManagerProxy::GetSupportedHDRFormats
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetSupportedHDRFormats, Function | SmallTest | Level1)
{
    std::vector<ScreenHDRFormat> hdrFormats;
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.GetSupportedHDRFormats(0, hdrFormats);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.GetSupportedHDRFormats(0, hdrFormats);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.GetSupportedHDRFormats(0, hdrFormats);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: SetScreenHDRFormat
 * @tc.desc: test DisplayManagerProxy::SetScreenHDRFormat
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetScreenHDRFormat, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.SetScreenHDRFormat(0, 0);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.SetScreenHDRFormat(0, 0);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.SetScreenHDRFormat(0, 0);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: GetScreenHDRFormat
 * @tc.desc: test DisplayManagerProxy::GetScreenHDRFormat
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetScreenHDRFormat, Function | SmallTest | Level1)
{
    ScreenHDRFormat hdrFormats = ScreenHDRFormat{VIDEO_HLG};
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.GetScreenHDRFormat(0, hdrFormats);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.GetScreenHDRFormat(0, hdrFormats);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.GetScreenHDRFormat(0, hdrFormats);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: GetSupportedColorSpaces
 * @tc.desc: test DisplayManagerProxy::GetSupportedColorSpaces
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetSupportedColorSpaces, Function | SmallTest | Level1)
{
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.GetSupportedColorSpaces(0, colorSpaces);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.GetSupportedColorSpaces(0, colorSpaces);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.GetSupportedColorSpaces(0, colorSpaces);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: GetScreenColorSpace
 * @tc.desc: test DisplayManagerProxy::GetScreenColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetScreenColorSpace, Function | SmallTest | Level1)
{
    GraphicCM_ColorSpaceType colorSpaces;
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.GetScreenColorSpace(0, colorSpaces);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.GetScreenColorSpace(0, colorSpaces);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.GetScreenColorSpace(0, colorSpaces);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: SetScreenColorSpace
 * @tc.desc: test DisplayManagerProxy::SetScreenColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetScreenColorSpace, Function | SmallTest | Level1)
{
    GraphicCM_ColorSpaceType colorSpaces = GraphicCM_ColorSpaceType{GRAPHIC_CM_BT601_EBU_FULL};
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.SetScreenColorSpace(0, colorSpaces);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.SetScreenColorSpace(0, colorSpaces);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.SetScreenColorSpace(0, colorSpaces);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: SuspendBegin
 * @tc.desc: test DisplayManagerProxy::SuspendBegin
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SuspendBegin, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy1(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy1.remoteObject_);

    PowerStateChangeReason reason = PowerStateChangeReason{0};
    auto result1 = proxy1.SuspendBegin(reason);
    EXPECT_EQ(result1, false);

    remoteMocker->sendRequestResult_ = 1;
    auto result2 = proxy1.SuspendBegin(reason);
    EXPECT_EQ(result2, false);
}

/**
 * @tc.name: SuspendEnd
 * @tc.desc: test DisplayManagerProxy::SuspendEnd
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SuspendEnd, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy1(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy1.remoteObject_);

    auto result1 = proxy1.SuspendEnd();
    EXPECT_EQ(result1, false);

    remoteMocker->sendRequestResult_ = 1;
    auto result2 = proxy1.SuspendEnd();
    EXPECT_EQ(result2, false);
}

/**
 * @tc.name: SetScreenPowerForAll
 * @tc.desc: test DisplayManagerProxy::SetScreenPowerForAll
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetScreenPowerForAll, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy1(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy1.remoteObject_);

    ScreenPowerState state = ScreenPowerState{0};
    PowerStateChangeReason reason = PowerStateChangeReason{0};
    auto result1 = proxy1.SetScreenPowerForAll(state, reason);
    EXPECT_EQ(result1, false);

    remoteMocker->sendRequestResult_ = 1;
    auto result2 = proxy1.SetScreenPowerForAll(state, reason);
    EXPECT_EQ(result2, false);
}

/**
 * @tc.name: SetSpecifiedScreenPower
 * @tc.desc: test DisplayManagerProxy::SetSpecifiedScreenPower
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetSpecifiedScreenPower, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy1(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy1.remoteObject_);

    ScreenPowerState state = ScreenPowerState{0};
    PowerStateChangeReason reason = PowerStateChangeReason{0};
    auto result1 = proxy1.SetSpecifiedScreenPower(0, state, reason);
    EXPECT_EQ(result1, false);

    remoteMocker->sendRequestResult_ = 1;
    auto result2 = proxy1.SetSpecifiedScreenPower(0, state, reason);
    EXPECT_EQ(result2, false);
}

/**
 * @tc.name: SetDisplayState
 * @tc.desc: test DisplayManagerProxy::SetDisplayState
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetDisplayState, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy1(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy1.remoteObject_);

    DisplayState state = DisplayState{0};
    auto result1 = proxy1.SetDisplayState(state);
    EXPECT_EQ(result1, false);

    remoteMocker->sendRequestResult_ = 1;
    auto result2 = proxy1.SetDisplayState(state);
    EXPECT_EQ(result2, false);
}

/**
 * @tc.name: AddSurfaceNodeToDisplay
 * @tc.desc: test DisplayManagerProxy::AddSurfaceNodeToDisplay
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, AddSurfaceNodeToDisplay, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy1(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy1.remoteObject_);

    std::shared_ptr<class RSSurfaceNode> surfaceNode;
    auto result1 = proxy1.AddSurfaceNodeToDisplay(0, surfaceNode, true);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result1);

    remoteMocker->sendRequestResult_ = 1;
    auto result2 = proxy1.AddSurfaceNodeToDisplay(0, surfaceNode, true);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result2);
}

/**
 * @tc.name: RemoveSurfaceNodeFromDisplay
 * @tc.desc: test DisplayManagerProxy::RemoveSurfaceNodeFromDisplay
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, RemoveSurfaceNodeFromDisplay, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy1(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy1.remoteObject_);

    std::shared_ptr<class RSSurfaceNode> surfaceNode;
    auto result1 = proxy1.RemoveSurfaceNodeFromDisplay(0, surfaceNode);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result1);

    remoteMocker->sendRequestResult_ = 1;
    auto result2 = proxy1.RemoveSurfaceNodeFromDisplay(0, surfaceNode);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result2);
}

/**
 * @tc.name: HasPrivateWindow
 * @tc.desc: test DisplayManagerProxy::HasPrivateWindow
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, HasPrivateWindow, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy1(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy1.remoteObject_);

    bool hasPrivateWindow = true;
    auto result1 = proxy1.HasPrivateWindow(0, hasPrivateWindow);
    EXPECT_EQ(DMError::DM_OK, result1);

    remoteMocker->sendRequestResult_ = 1;
    auto result2 = proxy1.HasPrivateWindow(0, hasPrivateWindow);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result2);
}

/**
 * @tc.name: SetFreeze
 * @tc.desc: test DisplayManagerProxy::SetFreeze
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetFreeze, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy1(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy1.remoteObject_);

    std::vector<DisplayId> displayIds;
    auto result1 = proxy1.SetFreeze(displayIds, true);
    EXPECT_TRUE(result1);

    remoteMocker->sendRequestResult_ = 1;
    auto result2 = proxy1.SetFreeze(displayIds, true);
    EXPECT_FALSE(result2);
}

/**
 * @tc.name: GetDisplayState
 * @tc.desc: test DisplayManagerProxy::GetDisplayState
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetDisplayState, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy1(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy1.remoteObject_);

    DisplayId displayId = 0;
    auto result1 = proxy1.GetDisplayState(displayId);
    EXPECT_EQ(result1, DisplayState::UNKNOWN);

    remoteMocker->sendRequestResult_ = 1;
    auto result2 = proxy1.GetDisplayState(displayId);
    EXPECT_EQ(result2, DisplayState::UNKNOWN);
}

/**
 * @tc.name: GetScreenPower
 * @tc.desc: test DisplayManagerProxy::GetScreenPower
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetScreenPower, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy1(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy1.remoteObject_);

    auto result1 = proxy1.GetScreenPower(0);
    EXPECT_EQ(result1, ScreenPowerState::POWER_ON);

    remoteMocker->sendRequestResult_ = 1;
    auto result2 = proxy1.GetScreenPower(0);
    EXPECT_EQ(result2, ScreenPowerState::INVALID_STATE);
}

/**
 * @tc.name: GetAllDisplayIds
 * @tc.desc: test DisplayManagerProxy::GetAllDisplayIds
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetAllDisplayIds, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy1(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy1.remoteObject_);

    auto result1 = proxy1.GetAllDisplayIds();
    EXPECT_TRUE(result1.empty());

    remoteMocker->sendRequestResult_ = 1;
    auto result2 = proxy1.GetAllDisplayIds();
    EXPECT_TRUE(result2.empty());
}

/**
 * @tc.name: GetCutoutInfo
 * @tc.desc: test DisplayManagerProxy::GetCutoutInfo
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetCutoutInfo, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.GetCutoutInfo(0);
    EXPECT_EQ(nullptr, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.GetCutoutInfo(0);
    EXPECT_EQ(nullptr, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.GetCutoutInfo(0);
    EXPECT_EQ(nullptr, result3);
}

/**
 * @tc.name: NotifyDisplayEvent
 * @tc.desc: test DisplayManagerProxy::NotifyDisplayEvent
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, NotifyDisplayEvent, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy1(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy1.remoteObject_);

    DisplayEvent event = DisplayEvent::UNLOCK;
    proxy1.NotifyDisplayEvent(event);
    EXPECT_TRUE(true);

    remoteMocker->sendRequestResult_ = 1;
    proxy1.NotifyDisplayEvent(event);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: MakeMirror
 * @tc.desc: test DisplayManagerProxy::MakeMirror
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, MakeMirror, Function | SmallTest | Level1)
{
    ScreenId mainScreenId = static_cast<ScreenId>(0);
    std::vector<ScreenId> mirrorScreenId;
    mirrorScreenId.emplace_back(1001);
    ScreenId screenGroupId{0};
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);

    auto result1 = proxy1.MakeMirror(mainScreenId, mirrorScreenId, screenGroupId);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.MakeMirror(mainScreenId, mirrorScreenId, screenGroupId);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.MakeMirror(mainScreenId, mirrorScreenId, screenGroupId);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: StopMirror
 * @tc.desc: test DisplayManagerProxy::StopMirror
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, StopMirror, Function | SmallTest | Level1)
{
    std::vector<ScreenId> mirrorScreenId;
    mirrorScreenId.emplace_back(1001);
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);

    auto result1 = proxy1.StopMirror(mirrorScreenId);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.StopMirror(mirrorScreenId);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.StopMirror(mirrorScreenId);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: GetScreenInfoById
 * @tc.desc: test DisplayManagerProxy::GetScreenInfoById
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetScreenInfoById, Function | SmallTest | Level1)
{
    ScreenId screenId = static_cast<ScreenId>(0);
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);

    auto result1 = proxy1.GetScreenInfoById(screenId);
    EXPECT_EQ(nullptr, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.GetScreenInfoById(screenId);
    EXPECT_EQ(nullptr, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.GetScreenInfoById(screenId);
    EXPECT_EQ(nullptr, result3);
}

/**
 * @tc.name: GetScreenGroupInfoById
 * @tc.desc: test DisplayManagerProxy::GetScreenGroupInfoById
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetScreenGroupInfoById, Function | SmallTest | Level1)
{
    ScreenId screenId = static_cast<ScreenId>(0);
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);

    auto result1 = proxy1.GetScreenGroupInfoById(screenId);
    EXPECT_EQ(nullptr, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.GetScreenGroupInfoById(screenId);
    EXPECT_EQ(nullptr, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.GetScreenGroupInfoById(screenId);
    EXPECT_EQ(nullptr, result3);
}

/**
 * @tc.name: GetAllScreenInfos
 * @tc.desc: test DisplayManagerProxy::GetAllScreenInfos
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetAllScreenInfos, Function | SmallTest | Level1)
{
    std::vector<sptr<ScreenInfo>> screenInfos{nullptr};
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);

    auto result1 = proxy1.GetAllScreenInfos(screenInfos);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.GetAllScreenInfos(screenInfos);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.GetAllScreenInfos(screenInfos);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: MakeExpand
 * @tc.desc: test DisplayManagerProxy::MakeExpand
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, MakeExpand, Function | SmallTest | Level1)
{
    ScreenId screenId_ = static_cast<ScreenId>(0);
    std::vector<ScreenId> screenId;
    screenId.push_back(screenId_);
    std::vector<Point> startPoint;
    Point point{0, 0};
    startPoint.push_back(point);
    ScreenId screenGroupId{0};
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);

    auto result1 = proxy1.MakeExpand(screenId, startPoint, screenGroupId);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.MakeExpand(screenId, startPoint, screenGroupId);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.MakeExpand(screenId, startPoint, screenGroupId);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: StopExpand
 * @tc.desc: test DisplayManagerProxy::StopExpand
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, StopExpand, Function | SmallTest | Level1)
{
    ScreenId screenId_ = static_cast<ScreenId>(0);
    std::vector<ScreenId> screenId;
    screenId.push_back(screenId_);
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);

    auto result1 = proxy1.StopExpand(screenId);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.StopExpand(screenId);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.StopExpand(screenId);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: RemoveVirtualScreenFromGroup
 * @tc.desc: test DisplayManagerProxy::RemoveVirtualScreenFromGroup
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, RemoveVirtualScreenFromGroup, Function | SmallTest | Level1)
{
    ScreenId screenId_ = static_cast<ScreenId>(0);
    std::vector<ScreenId> screenId;
    screenId.push_back(screenId_);
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);

    proxy1.RemoveVirtualScreenFromGroup(screenId);
    EXPECT_TRUE(true);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    proxy2.RemoveVirtualScreenFromGroup(screenId);
    EXPECT_TRUE(true);

    remoteMocker->sendRequestResult_ = 1;
    proxy2.RemoveVirtualScreenFromGroup(screenId);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetScreenActiveMode
 * @tc.desc: test DisplayManagerProxy::SetScreenActiveMode
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetScreenActiveMode, Function | SmallTest | Level1)
{
    ScreenId screenId = static_cast<ScreenId>(0);
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);

    auto result1 = proxy1.SetScreenActiveMode(screenId, 0);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.SetScreenActiveMode(screenId, 0);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.SetScreenActiveMode(screenId, 0);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: SetVirtualPixelRatio
 * @tc.desc: test DisplayManagerProxy::SetVirtualPixelRatio
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetVirtualPixelRatio, Function | SmallTest | Level1)
{
    ScreenId screenId = static_cast<ScreenId>(0);
    float virtualPixelRatio = 0;
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);

    auto result1 = proxy1.SetVirtualPixelRatio(screenId, virtualPixelRatio);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.SetVirtualPixelRatio(screenId, virtualPixelRatio);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.SetVirtualPixelRatio(screenId, virtualPixelRatio);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: SetResolution
 * @tc.desc: test DisplayManagerProxy::SetResolution
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetResolution, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy.remoteObject_);
    ScreenId screenId = 0;
    auto result = proxy.SetResolution(screenId, 50, 100, 1.00);
    EXPECT_EQ(DMError::DM_OK, result);
}

/**
 * @tc.name: GetDensityInCurResolution
 * @tc.desc: test DisplayManagerProxy::GetDensityInCurResolution
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetDensityInCurResolution, Function | SmallTest | Level1)
{
    ScreenId screenId = static_cast<ScreenId>(0);
    float virtualPixelRatio = 0;
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);

    auto result1 = proxy1.GetDensityInCurResolution(screenId, virtualPixelRatio);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.GetDensityInCurResolution(screenId, virtualPixelRatio);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.GetDensityInCurResolution(screenId, virtualPixelRatio);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: IsScreenRotationLocked
 * @tc.desc: test DisplayManagerProxy::IsScreenRotationLocked
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, IsScreenRotationLocked, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy.remoteObject_);
    bool isLocked = true;
    auto result = proxy.IsScreenRotationLocked(isLocked);
    EXPECT_EQ(DMError::DM_OK, result);
}

/**
 * @tc.name: SetScreenRotationLocked
 * @tc.desc: test DisplayManagerProxy::SetScreenRotationLocked
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetScreenRotationLocked, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);

    auto result1 = proxy1.SetScreenRotationLocked(true);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.SetScreenRotationLocked(true);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.SetScreenRotationLocked(true);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: SetScreenRotationLockedFromJs
 * @tc.desc: test DisplayManagerProxy::SetScreenRotationLockedFromJs
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetScreenRotationLockedFromJs, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);

    auto result1 = proxy1.SetScreenRotationLockedFromJs(true);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.SetScreenRotationLockedFromJs(true);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.SetScreenRotationLockedFromJs(true);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: ResizeVirtualScreen
 * @tc.desc: test DisplayManagerProxy::ResizeVirtualScreen
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, ResizeVirtualScreen, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy.remoteObject_);
    ScreenId screenId = 0;
    auto result = proxy.ResizeVirtualScreen(screenId, 50, 100);
    EXPECT_EQ(DMError::DM_OK, result);
}

/**
 * @tc.name: MakeUniqueScreen
 * @tc.desc: test DisplayManagerProxy::MakeUniqueScreen
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, MakeUniqueScreen, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy.remoteObject_);
    std::vector<ScreenId> screenIds;
    auto result = proxy.MakeUniqueScreen(screenIds);
    EXPECT_EQ(DMError::DM_OK, result);
}

/**
 * @tc.name: RemoveVirtualScreenFromGroup02
 * @tc.desc: test DisplayManagerProxy::RemoveVirtualScreenFromGroup02
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, RemoveVirtualScreenFromGroup02, Function | SmallTest | Level1)
{
    ScreenId screenId_ = static_cast<ScreenId>(0);
    std::vector<ScreenId> screenId;
    screenId.push_back(screenId_);
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);

    proxy1.RemoveVirtualScreenFromGroup(screenId);
    EXPECT_TRUE(true);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    proxy2.RemoveVirtualScreenFromGroup(screenId);
    EXPECT_TRUE(true);

    remoteMocker->sendRequestResult_ = 1;
    proxy2.RemoveVirtualScreenFromGroup(screenId);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetScreenActiveMode02
 * @tc.desc: test DisplayManagerProxy::SetScreenActiveMode02
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetScreenActiveMode02, Function | SmallTest | Level1)
{
    ScreenId screenId = static_cast<ScreenId>(0);
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);

    auto result1 = proxy1.SetScreenActiveMode(screenId, 0);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.SetScreenActiveMode(screenId, 0);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.SetScreenActiveMode(screenId, 0);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: SetVirtualPixelRatio02
 * @tc.desc: test DisplayManagerProxy::SetVirtualPixelRatio02
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetVirtualPixelRatio02, Function | SmallTest | Level1)
{
    ScreenId screenId = static_cast<ScreenId>(0);
    float virtualPixelRatio = 0;
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);

    auto result1 = proxy1.SetVirtualPixelRatio(screenId, virtualPixelRatio);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.SetVirtualPixelRatio(screenId, virtualPixelRatio);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.SetVirtualPixelRatio(screenId, virtualPixelRatio);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: SetResolution02
 * @tc.desc: test DisplayManagerProxy::SetResolution02
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetResolution02, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy.remoteObject_);
    ScreenId screenId = 0;
    auto result = proxy.SetResolution(screenId, 50, 100, 1.00);
    EXPECT_EQ(DMError::DM_OK, result);
}

/**
 * @tc.name: GetDensityInCurResolution02
 * @tc.desc: test DisplayManagerProxy::GetDensityInCurResolution02
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetDensityInCurResolution02, Function | SmallTest | Level1)
{
    ScreenId screenId = static_cast<ScreenId>(0);
    float virtualPixelRatio = 0;
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);

    auto result1 = proxy1.GetDensityInCurResolution(screenId, virtualPixelRatio);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.GetDensityInCurResolution(screenId, virtualPixelRatio);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.GetDensityInCurResolution(screenId, virtualPixelRatio);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: IsScreenRotationLocked02
 * @tc.desc: test DisplayManagerProxy::IsScreenRotationLocked02
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, IsScreenRotationLocked02, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy.remoteObject_);
    bool isLocked = true;
    auto result = proxy.IsScreenRotationLocked(isLocked);
    EXPECT_EQ(DMError::DM_OK, result);
}

/**
 * @tc.name: SetScreenRotationLocked02
 * @tc.desc: test DisplayManagerProxy::SetScreenRotationLocked02
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetScreenRotationLocked02, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);

    auto result1 = proxy1.SetScreenRotationLocked(true);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.SetScreenRotationLocked(true);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.SetScreenRotationLocked(true);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: SetScreenRotationLockedFromJs02
 * @tc.desc: test DisplayManagerProxy::SetScreenRotationLockedFromJs02
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetScreenRotationLockedFromJs02, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    EXPECT_EQ(nullptr, proxy1.remoteObject_);

    auto result1 = proxy1.SetScreenRotationLockedFromJs(true);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    
    auto result2 = proxy2.SetScreenRotationLockedFromJs(true);
    EXPECT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.SetScreenRotationLockedFromJs(true);
    EXPECT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}

/**
 * @tc.name: ResizeVirtualScreen02
 * @tc.desc: test DisplayManagerProxy::ResizeVirtualScreen02
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, ResizeVirtualScreen02, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy.remoteObject_);
    ScreenId screenId = 0;
    auto result = proxy.ResizeVirtualScreen(screenId, 50, 100);
    EXPECT_EQ(DMError::DM_OK, result);
}

/**
 * @tc.name: MakeUniqueScreen02
 * @tc.desc: test DisplayManagerProxy::MakeUniqueScreen02
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, MakeUniqueScreen02, Function | SmallTest | Level1)
{
    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy(remoteMocker);
    EXPECT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy.remoteObject_);
    std::vector<ScreenId> screenIds;
    auto result = proxy.MakeUniqueScreen(screenIds);
    EXPECT_EQ(DMError::DM_OK, result);
}
}
} // namespace Rosen
} // namespace OHOS