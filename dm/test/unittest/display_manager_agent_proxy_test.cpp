/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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
#include <iremote_object_mocker.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "display_manager.h"
#include "display_manager_adapter.h"
#include "display_manager_proxy.h"
#include "scene_board_judgement.h"
#include "window_scene.h"
#include "zidl/display_manager_agent_proxy.h"
#include "../../../window_scene/test/mock/mock_message_parcel.h"

using namespace testing;
using namespace testing::ext;

namespace {
    std::string logMsg;
    void MyLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char* tag,
        const char* msg)
    {
        logMsg = msg;
    }
}

namespace OHOS::Rosen {
class DisplayManagerAgentProxyTest : public testing::Test {
public:
    static void SetUpTestSuite();
    void SetUp() override;
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy;
};

void DisplayManagerAgentProxyTest::SetUpTestSuite()
{

}

void DisplayManagerAgentProxyTest::SetUp()
{
    if (displayManagerAgentProxy) {
        return;
    }

    sptr<IRemoteObject> impl;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(sptr<IRemoteObjectMocker>::MakeSptr(), nullptr);
        impl = sptr<IRemoteObjectMocker>::MakeSptr();
    } else {
        ASSERT_NE(sptr<IRemoteObjectMocker>::MakeSptr(), nullptr);
        impl = sptr<IRemoteObjectMocker>::MakeSptr();
    }

    displayManagerAgentProxy = new (std::nothrow) DisplayManagerAgentProxy(impl);
    ASSERT_NE(displayManagerAgentProxy, nullptr);
}

/**
 * @tc.name: NotifyDisplayPowerEvent
 * @tc.desc: NotifyDisplayPowerEvent
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyDisplayPowerEvent, TestSize.Level1)
{
    DisplayPowerEvent event = DisplayPowerEvent::DESKTOP_READY;
    EventStatus status = EventStatus::BEGIN;
    GTEST_LOG_(INFO) << "WindowSessionImplTest:1";
    displayManagerAgentProxy->NotifyDisplayPowerEvent(event, status);

    int resultValue = 0;
    std::function<void()> func = [&]() {
        displayManagerAgentProxy->NotifyDisplayPowerEvent(event, status);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyDisplayPowerEvent02
 * @tc.desc: NotifyDisplayPowerEvent
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyDisplayPowerEvent02, TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    DisplayPowerEvent event = DisplayPowerEvent::DESKTOP_READY;
    EventStatus status = EventStatus::BEGIN;

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    displayManagerAgentProxy->NotifyDisplayPowerEvent(event, status);
    EXPECT_TRUE(logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: NotifyDisplayPowerEvent03
 * @tc.desc: NotifyDisplayPowerEvent
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyDisplayPowerEvent03, TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    DisplayPowerEvent event = DisplayPowerEvent::DESKTOP_READY;
    EventStatus status = EventStatus::BEGIN;

    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteUint32ErrorFlag(true);
    displayManagerAgentProxy->NotifyDisplayPowerEvent(event, status);
    EXPECT_TRUE(logMsg.find("Write event failed") != std::string::npos);
}

/**
 * @tc.name: NotifyDisplayStateChanged
 * @tc.desc: NotifyDisplayStateChanged
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyDisplayStateChanged, TestSize.Level1)
{
    DisplayId displayId = 0;
    DisplayState state = DisplayState::OFF;

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->NotifyDisplayStateChanged(displayId, state);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyCaptureStatusChanged
 * @tc.desc: NotifyCaptureStatusChanged
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyCaptureStatusChanged, TestSize.Level1)
{
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->NotifyCaptureStatusChanged(false);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyCaptureStatusChanged01
 * @tc.desc: NotifyCaptureStatusChanged01
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyCaptureStatusChanged01, TestSize.Level1)
{
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->NotifyCaptureStatusChanged(true);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyDisplayChangeInfoChanged
 * @tc.desc: NotifyDisplayChangeInfoChanged
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyDisplayChangeInfoChanged, TestSize.Level1)
{
    sptr<DisplayChangeInfo> display_change_info = new DisplayChangeInfo();
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->NotifyDisplayChangeInfoChanged(display_change_info);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyDisplayChangeInfoChanged01
 * @tc.desc: NotifyDisplayChangeInfoChanged01
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyDisplayChangeInfoChanged01, TestSize.Level1)
{
    sptr<DisplayChangeInfo> display_change_info = new DisplayChangeInfo();
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->NotifyDisplayChangeInfoChanged(display_change_info);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyDisplayModeChanged
 * @tc.desc: NotifyDisplayModeChanged
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyDisplayModeChanged, TestSize.Level1)
{
    FoldDisplayMode mode = FoldDisplayMode::UNKNOWN;
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->NotifyDisplayModeChanged(mode);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyAvailableAreaChanged
 * @tc.desc: NotifyAvailableAreaChanged
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyAvailableAreaChanged, TestSize.Level1)
{    DisplayId displayId = 0;

    DMRect rect = {2, 2, 2, 2};
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->NotifyAvailableAreaChanged(rect, displayId);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyAvailableAreaChanged01
 * @tc.desc: NotifyAvailableAreaChanged01
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyAvailableAreaChanged01, TestSize.Level1)
{    DisplayId displayId = 0;

    DMRect rect = {1, 1, 1, 1};
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->NotifyAvailableAreaChanged(rect, displayId);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyAvailableAreaChanged02
 * @tc.desc: NotifyAvailableAreaChanged02
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyAvailableAreaChanged02, TestSize.Level1)
{    DisplayId displayId = 0;

    DMRect rect = {3, 3, 3, 3};
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->NotifyAvailableAreaChanged(rect, displayId);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: OnScreenConnect
 * @tc.desc: OnScreenConnect
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenConnect, TestSize.Level1)
{
    sptr<ScreenInfo> screenInfo = nullptr;
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->OnScreenConnect(screenInfo);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: OnScreenConnect01
 * @tc.desc: OnScreenConnect01
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenConnect01, TestSize.Level1)
{
    sptr<ScreenInfo> screenInfo = new ScreenInfo();
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->OnScreenConnect(screenInfo);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: OnScreenConnect02
 * @tc.desc: OnScreenConnect02
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenConnect02, TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<ScreenInfo> screenInfo = new ScreenInfo();
    
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteUint32ErrorFlag(true);
    displayManagerAgentProxy->OnScreenConnect(screenInfo);
    EXPECT_TRUE(logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: OnScreenConnect03
 * @tc.desc: OnScreenConnect03
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenConnect03, TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<ScreenInfo> screenInfo = new ScreenInfo();
    
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteParcelableErrorFlag(true);
    displayManagerAgentProxy->OnScreenConnect(screenInfo);
    EXPECT_TRUE(logMsg.find("Write ScreenInfo failed") != std::string::npos);
}

/**
 * @tc.name: OnScreenDisconnect
 * @tc.desc: OnScreenDisconnect
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenDisconnect, TestSize.Level1)
{
    ScreenId screenId = 0;
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->OnScreenDisconnect(screenId);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: OnScreenDisconnect01
 * @tc.desc: OnScreenDisconnect
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenDisconnect01, TestSize.Level1)
{
    ScreenId screenId = 0;
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<ScreenInfo> screenInfo = new ScreenInfo();
    
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    displayManagerAgentProxy->OnScreenDisconnect(screenId);
    EXPECT_TRUE(logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}
/**
 * @tc.name: OnScreenChange
 * @tc.desc: OnScreenChange
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenChange, TestSize.Level1)
{
    sptr<ScreenInfo> screenInfo = new ScreenInfo();
    ScreenChangeEvent event = ScreenChangeEvent::CHANGE_MODE;

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->OnScreenChange(screenInfo, event);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: OnScreenChange01
 * @tc.desc: OnScreenChange01
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenChange01, TestSize.Level1)
{
    sptr<ScreenInfo> screenInfo = nullptr;
    ScreenChangeEvent event = ScreenChangeEvent::CHANGE_MODE;

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->OnScreenChange(screenInfo, event);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: OnScreenChange02
 * @tc.desc: OnScreenChange02
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenChange02, TestSize.Level1)
{
    sptr<ScreenInfo> screenInfo = nullptr;
    ScreenChangeEvent event = ScreenChangeEvent::CHANGE_MODE;
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<ScreenInfo> screenInfo = new ScreenInfo();
    
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    displayManagerAgentProxy->OnScreenChange(screenInfo, event);
    EXPECT_TRUE(logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: OnScreenChange03
 * @tc.desc: OnScreenChange03
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenChange03, TestSize.Level1)
{
    sptr<ScreenInfo> screenInfo = nullptr;
    ScreenChangeEvent event = ScreenChangeEvent::CHANGE_MODE;
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<ScreenInfo> screenInfo = new ScreenInfo();
    
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteParcelableErrorFlag(true);
    displayManagerAgentProxy->OnScreenChange(screenInfo, event);
    EXPECT_TRUE(logMsg.find("Write screenInfo failed") != std::string::npos);
}

/**
 * @tc.name: OnScreenChange04
 * @tc.desc: OnScreenChange04
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenChange04, TestSize.Level1)
{
    sptr<ScreenInfo> screenInfo = nullptr;
    ScreenChangeEvent event = ScreenChangeEvent::CHANGE_MODE;
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<ScreenInfo> screenInfo = new ScreenInfo();
    
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteUint32ErrorFlag(true);
    displayManagerAgentProxy->OnScreenChange(screenInfo, event);
    EXPECT_TRUE(logMsg.find("Write ScreenChangeEvent failed") != std::string::npos);
}
/**
 * @tc.name: OnScreenGroupChange
 * @tc.desc: OnScreenGroupChange
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenGroupChange, TestSize.Level1)
{
    std::string trigger = " ";
    std::vector<sptr<ScreenInfo>> screenInfos = {};
    ScreenGroupChangeEvent event = ScreenGroupChangeEvent::CHANGE_GROUP;

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->OnScreenGroupChange(trigger, screenInfos, event);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: OnScreenGroupChange02
 * @tc.desc: OnScreenGroupChange
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenGroupChange02, TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    std::string trigger = " ";
    std::vector<sptr<ScreenInfo>> screenInfos = {};
    ScreenGroupChangeEvent event = ScreenGroupChangeEvent::CHANGE_GROUP;
    
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    displayManagerAgentProxy->OnScreenGroupChange(trigger, screenInfos, event);
    EXPECT_TRUE(logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: OnScreenGroupChange03
 * @tc.desc: OnScreenGroupChange
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenGroupChange03, TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    std::string trigger = " ";
    std::vector<sptr<ScreenInfo>> screenInfos = {};
    ScreenGroupChangeEvent event = ScreenGroupChangeEvent::CHANGE_GROUP;
    
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteUint32ErrorFlag(true);
    displayManagerAgentProxy->OnScreenGroupChange(trigger, screenInfos, event);
    EXPECT_TRUE(logMsg.find("Write ScreenGroupChangeEvent failed") != std::string::npos);
}
/**
 * @tc.name: OnDisplayCreate
 * @tc.desc: OnDisplayCreate
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnDisplayCreate, TestSize.Level1)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->OnDisplayCreate(displayInfo);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: OnDisplayCreate01
 * @tc.desc: OnDisplayCreate01
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnDisplayCreate01, TestSize.Level1)
{
    sptr<DisplayInfo> displayInfo = nullptr;
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->OnDisplayCreate(displayInfo);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: OnDisplayCreate02
 * @tc.desc: OnDisplayCreate02
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnDisplayCreate02, TestSize.Level1)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    displayManagerAgentProxy->OnDisplayCreate(displayInfo);
    EXPECT_TRUE(logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: OnDisplayCreate03
 * @tc.desc: OnDisplayCreate03
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnDisplayCreate03, TestSize.Level1)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteParcelableErrorFlag(true);
    displayManagerAgentProxy->OnDisplayCreate(displayInfo);
    EXPECT_TRUE(logMsg.find("Write DisplayInfo failed") != std::string::npos);
}

/**
 * @tc.name: OnDisplayDestroy
 * @tc.desc: OnDisplayDestroy
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnDisplayDestroy, TestSize.Level1)
{
    DisplayId displayId = 0;
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->OnDisplayDestroy(displayId);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: OnDisplayDestroy01
 * @tc.desc: OnDisplayDestroy
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnDisplayDestroy01, TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    DisplayId displayId = 0;
    
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    displayManagerAgentProxy->OnDisplayDestroy(displayId);
    EXPECT_TRUE(logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: OnDisplayChange
 * @tc.desc: OnDisplayChange
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnDisplayChange, TestSize.Level1)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    DisplayChangeEvent event = DisplayChangeEvent::DISPLAY_FREEZED;
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->OnDisplayChange(displayInfo, event);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: OnDisplayChange01
 * @tc.desc: OnDisplayChange01
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnDisplayChange01, TestSize.Level1)
{
    sptr<DisplayInfo> displayInfo = nullptr;
    DisplayChangeEvent event = DisplayChangeEvent::DISPLAY_FREEZED;
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->OnDisplayChange(displayInfo, event);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: OnDisplayChange02
 * @tc.desc: OnDisplayChange02
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnDisplayChange02, TestSize.Level1)
{
    
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    DisplayChangeEvent event = DisplayChangeEvent::DISPLAY_FREEZED;
    
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    displayManagerAgentProxy->OnDisplayChange(displayInfo, event);
    EXPECT_TRUE(logMsg.find("WriteInterfaceToken failed") != std::string::npos);
}

/**
 * @tc.name: OnDisplayChange03
 * @tc.desc: OnDisplayChange03
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnDisplayChange03, TestSize.Level1)
{
    
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    DisplayChangeEvent event = DisplayChangeEvent::DISPLAY_FREEZED;
    
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteParcelableErrorFlag(true);
    displayManagerAgentProxy->OnDisplayChange(displayInfo, event);
    EXPECT_TRUE(logMsg.find("Write DisplayInfo failed") != std::string::npos);
}

/**
 * @tc.name: OnDisplayChange03
 * @tc.desc: OnDisplayChange03
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnDisplayChange03, TestSize.Level1)
{
    
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    DisplayChangeEvent event = DisplayChangeEvent::DISPLAY_FREEZED;
    
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteUint32ErrorFlag(true);
    displayManagerAgentProxy->OnDisplayChange(displayInfo, event);
    EXPECT_TRUE(logMsg.find("Write DisplayChangeEvent failed") != std::string::npos);
}
/**
 * @tc.name: OnScreenshot
 * @tc.desc: OnScreenshot
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenshot, TestSize.Level1)
{
    sptr<ScreenshotInfo> snapshotInfo = new ScreenshotInfo();
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->OnScreenshot(snapshotInfo);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: OnScreenshot01
 * @tc.desc: OnScreenshot01
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenshot01, TestSize.Level1)
{
    sptr<ScreenshotInfo> snapshotInfo = nullptr;
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->OnScreenshot(snapshotInfo);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyPrivateWindowStateChanged
 * @tc.desc: NotifyPrivateWindowStateChanged
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyPrivateWindowStateChanged, TestSize.Level1)
{
    bool hasPrivate = false;
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->NotifyPrivateWindowStateChanged(hasPrivate);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyPrivateWindowStateChanged01
 * @tc.desc: NotifyPrivateWindowStateChanged01
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyPrivateWindowStateChanged01, TestSize.Level1)
{
    bool hasPrivate = true;
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->NotifyPrivateWindowStateChanged(hasPrivate);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyPrivateStateWindowListChanged
 * @tc.desc: NotifyPrivateStateWindowListChanged
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyPrivateStateWindowListChanged, TestSize.Level1)
{
    DisplayId id = 0;
    std::vector<std::string> privacyWindowList{"win0", "win1"};
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->NotifyPrivateStateWindowListChanged(id, privacyWindowList);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyFoldStatusChanged
 * @tc.desc: NotifyFoldStatusChanged
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyFoldStatusChanged, TestSize.Level1)
{
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->NotifyFoldStatusChanged(FoldStatus::EXPAND);
        resultValue = 1;
    };
    func();

    ASSERT_EQ(resultValue, 1);
}
}
