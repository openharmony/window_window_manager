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
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    GTEST_LOG_(INFO) << "axxx";
    DisplayPowerEvent event = DisplayPowerEvent::DISPLAY_OFF;
    GTEST_LOG_(INFO) << "bxxx";
    EventStatus status = EventStatus::BEGIN;
    GTEST_LOG_(INFO) << "cxxx";
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);
    GTEST_LOG_(INFO) << "dxxx";
    int resultValue = 0;
    std::function<void()> func = [&]() {
        displayManagerAgentProxy->NotifyDisplayPowerEvent(event,status);
        resultValue = 1;
    };
    GTEST_LOG_(INFO) << "exxx";
    func();
    GTEST_LOG_(INFO) << "fxxx";
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

 
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

    int resultValue = 0;
    std::function<void()> func = [&]() {
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
    std::function<void()> func = [&]() {
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
    std::function<void()> func = [&]() {
        displayManagerAgentProxy->OnScreenChange(screenInfo,event);
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
    std::function<void()> func = [&]() {
        displayManagerAgentProxy->OnScreenGroupChange(trigger,screenInfos,event);
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
    std::function<void()> func = [&]() {
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
    std::function<void()> func = [&]() {
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
    std::function<void()> func = [&]() {
        displayManagerAgentProxy->OnDisplayChange(displayInfo,event);
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
    std::function<void()> func = [&]() {
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
    std::function<void()> func = [&]() {
        displayManagerAgentProxy->NotifyPrivateWindowStateChanged(hasPrivate);
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
    //FoldStatus foldStatus = FoldStatus::EXPAND;
    sptr<IRemoteObject> impl = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    sptr<DisplayManagerAgentProxy> displayManagerAgentProxy = new DisplayManagerAgentProxy(impl);

    int resultValue = 0;
    std::function<void()> func = [&]() {
        displayManagerAgentProxy->NotifyFoldStatusChanged(FoldStatus::EXPAND);
        resultValue = 1;
    };
    func();

    ASSERT_EQ(resultValue, 1);
}
}
}
}