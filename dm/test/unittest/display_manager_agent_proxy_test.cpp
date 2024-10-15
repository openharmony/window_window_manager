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
#include "display_manager_proxy.h"
#include "display_manager_adapter.h"
#include "display_manager.h"
#include <gtest/gtest.h>
#include "hilog/log.h"
#include <iremote_broker.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include "scene_board_judgement.h"
#include "screen_manager.h"
#include "window_manager_hilog.h"
#include "window_scene.h"
#include "zidl/display_manager_agent_proxy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class DisplayManagerAgentProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DisplayManagerAgentProxyTest::SetUpTestCase()
{
}

void DisplayManagerAgentProxyTest::TearDownTestCase()
{
}

void DisplayManagerAgentProxyTest::SetUp()
{
}

void DisplayManagerAgentProxyTest::TearDown()
{
}

namespace {
/**
 * @tc.name: NotifyDisplayPowerEvent
 * @tc.desc: NotifyDisplayPowerEvent
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyDisplayPowerEvent, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();

    DisplayPowerEvent event = DisplayPowerEvent::DESKTOP_READY;
    EventStatus status = EventStatus::BEGIN;
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);
    GTEST_LOG_(INFO) << "WindowSessionImplTest:1";
    displayManagerAgentProxy->NotifyDisplayPowerEvent(event, status);

    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->NotifyDisplayPowerEvent(event, status);
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: NotifyDisplayStateChanged
 * @tc.desc: NotifyDisplayStateChanged
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, NotifyDisplayStateChanged, Function | SmallTest | Level1)
{
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

    DisplayId displayId = 0;
    DisplayState state = DisplayState{1};

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
HWTEST_F(DisplayManagerAgentProxyTest, NotifyCaptureStatusChanged, Function | SmallTest | Level1)
{
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

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
HWTEST_F(DisplayManagerAgentProxyTest, NotifyCaptureStatusChanged01, Function | SmallTest | Level1)
{
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

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
HWTEST_F(DisplayManagerAgentProxyTest, NotifyDisplayChangeInfoChanged, Function | SmallTest | Level1)
{
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

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
HWTEST_F(DisplayManagerAgentProxyTest, NotifyDisplayChangeInfoChanged01, Function | SmallTest | Level1)
{
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

    sptr<DisplayChangeInfo> display_change_info = nullptr;
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
HWTEST_F(DisplayManagerAgentProxyTest, NotifyDisplayModeChanged, Function | SmallTest | Level1)
{
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

    FoldDisplayMode mode = FoldDisplayMode{0};
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
HWTEST_F(DisplayManagerAgentProxyTest, NotifyAvailableAreaChanged, Function | SmallTest | Level1)
{
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

    DMRect rect = {2, 2, 2, 2};
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->NotifyAvailableAreaChanged(rect);
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
HWTEST_F(DisplayManagerAgentProxyTest, NotifyAvailableAreaChanged01, Function | SmallTest | Level1)
{
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

    DMRect rect = {1, 1, 1, 1};
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->NotifyAvailableAreaChanged(rect);
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
HWTEST_F(DisplayManagerAgentProxyTest, NotifyAvailableAreaChanged02, Function | SmallTest | Level1)
{
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

    DMRect rect = {3, 3, 3, 3};
    int resultValue = 0;
    std::function<void()> func = [&]()
    {
        displayManagerAgentProxy->NotifyAvailableAreaChanged(rect);
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
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenConnect, Function | SmallTest | Level1)
{
    sptr<ScreenInfo> screenInfo = nullptr;

    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

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
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenConnect01, Function | SmallTest | Level1)
{
    sptr<ScreenInfo> screenInfo = new ScreenInfo();

    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

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
 * @tc.name: OnScreenDisconnect
 * @tc.desc: OnScreenDisconnect
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenDisconnect, Function | SmallTest | Level1)
{
    ScreenId screenId = 0;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

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
 * @tc.name: OnScreenChange
 * @tc.desc: OnScreenChange
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenChange, Function | SmallTest | Level1)
{
    sptr<ScreenInfo> screenInfo = new ScreenInfo();
    ScreenChangeEvent event = ScreenChangeEvent::CHANGE_MODE;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

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
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenChange01, Function | SmallTest | Level1)
{
    sptr<ScreenInfo> screenInfo = nullptr;
    ScreenChangeEvent event = ScreenChangeEvent::CHANGE_MODE;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

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
 * @tc.name: OnScreenGroupChange
 * @tc.desc: OnScreenGroupChange
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenGroupChange, Function | SmallTest | Level1)
{
    std::string trigger = " ";
    std::vector<sptr<ScreenInfo>> screenInfos = {};
    ScreenGroupChangeEvent event = ScreenGroupChangeEvent::CHANGE_GROUP;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

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
 * @tc.name: OnDisplayCreate
 * @tc.desc: OnDisplayCreate
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnDisplayCreate, Function | SmallTest | Level1)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

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
HWTEST_F(DisplayManagerAgentProxyTest, OnDisplayCreate01, Function | SmallTest | Level1)
{
    sptr<DisplayInfo> displayInfo = nullptr;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

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
 * @tc.name: OnDisplayDestroy
 * @tc.desc: OnDisplayDestroy
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnDisplayDestroy, Function | SmallTest | Level1)
{
    DisplayId displayId = 0;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

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
 * @tc.name: OnDisplayChange
 * @tc.desc: OnDisplayChange
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnDisplayChange, Function | SmallTest | Level1)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    DisplayChangeEvent event = DisplayChangeEvent::DISPLAY_FREEZED;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

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
HWTEST_F(DisplayManagerAgentProxyTest, OnDisplayChange01, Function | SmallTest | Level1)
{
    sptr<DisplayInfo> displayInfo = nullptr;
    DisplayChangeEvent event = DisplayChangeEvent::DISPLAY_FREEZED;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

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
 * @tc.name: OnScreenshot
 * @tc.desc: OnScreenshot
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenshot, Function | SmallTest | Level1)
{
    sptr<ScreenshotInfo> snapshotInfo = new ScreenshotInfo();

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

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
HWTEST_F(DisplayManagerAgentProxyTest, OnScreenshot01, Function | SmallTest | Level1)
{
    sptr<ScreenshotInfo> snapshotInfo = nullptr;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

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
HWTEST_F(DisplayManagerAgentProxyTest, NotifyPrivateWindowStateChanged, Function | SmallTest | Level1)
{
    bool hasPrivate = false;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

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
HWTEST_F(DisplayManagerAgentProxyTest, NotifyPrivateWindowStateChanged01, Function | SmallTest | Level1)
{
    bool hasPrivate = true;

    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

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
HWTEST_F(DisplayManagerAgentProxyTest, NotifyPrivateStateWindowListChanged, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    DisplayId id = 0;
    std::vector<std::string> privacyWindowList{"win0", "win1"};
    
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

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
HWTEST_F(DisplayManagerAgentProxyTest, NotifyFoldStatusChanged, Function | SmallTest | Level1)
{
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

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
}
}