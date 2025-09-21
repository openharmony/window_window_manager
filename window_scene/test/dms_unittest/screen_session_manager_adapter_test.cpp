/*

Copyright (c) 2025 Huawei Device Co., Ltd.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include <gtest/gtest.h>
#include <parameter.h>
#include <parameters.h>

#include "../mock/mock_accesstoken_kit.h"
#include "../mock/mock_session_permission.h"
#include "common_test_utils.h"
#include "display_manager_agent_default.h"
#include "fold_screen_state_internel.h"
#include "screen_scene_config.h"
#include "screen_session_manager/include/screen_session_manager.h"
#include "screen_session_manager_adapter.h"
#include "screen_setting_helper.h"
#include "test_client.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScreenSessionManagerAgentTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ScreenSessionManagerAgentTest::SetUpTestCase() {}

void ScreenSessionManagerAgentTest::TearDownTestCase() {}

void ScreenSessionManagerAgentTest::SetUp() {}

void ScreenSessionManagerAgentTest::TearDown() {}

namespace {
/**

@tc.name: NotifyDisplayStateChanged

@tc.desc: NotifyDisplayStateChanged test

@tc.type: FUNC
*/
HWTEST_F(ScreenSessionManagerAgentTest, NotifyDisplayStateChanged, TestSize.Level1)
{
    ScreenSessionManagerAdapter ScreenSessionManagerAdapter;
    DisplayId id = 3;
    DisplayState state = DisplayState::ON;
    bool result = ScreenSessionManagerAdapter.NotifyDisplayStateChanged(id, state);
    EXPECT_FALSE(result);

    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    ScreenSessionManagerAdapter.dmAgentContainer_.agentMap_.insert(
        { DisplayManagerAgentType::DISPLAY_STATE_LISTENER, setIDisplay });
    result = ScreenSessionManagerAdapter.NotifyDisplayStateChanged(id, state);
    ASSERT_TRUE(result);
}

/**

@tc.name: OnScreenConnect

@tc.desc: OnScreenConnect test

@tc.type: FUNC
*/
HWTEST_F(ScreenSessionManagerAgentTest, OnScreenConnect, TestSize.Level1)
{
    sptr<ScreenInfo> screenInfo = nullptr;
    ScreenSessionManagerAdapter::GetInstance().OnScreenConnect(screenInfo);
    ASSERT_EQ(0,
              ScreenSessionManagerAdapter::GetInstance()
                  .dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER)
                  .size());

    ScreenSessionManagerAdapter ScreenSessionManagerAdapter;
    screenInfo = new ScreenInfo();
    ScreenSessionManagerAdapter.OnScreenConnect(screenInfo);
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    ScreenSessionManagerAdapter.dmAgentContainer_.agentMap_.insert(
        { DisplayManagerAgentType::SCREEN_EVENT_LISTENER, setIDisplay });
    ScreenSessionManagerAdapter.OnScreenConnect(screenInfo);
    int result =
        ScreenSessionManagerAdapter.dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER)
            .size();
    ASSERT_EQ(result, 1);
}

/**

@tc.name: OnScreenChange

@tc.desc: OnScreenChange test

@tc.type: FUNC
*/
HWTEST_F(ScreenSessionManagerAgentTest, OnScreenChange, TestSize.Level1)
{
    sptr<ScreenInfo> screenInfo = nullptr;
    ScreenSessionManagerAdapter::GetInstance().OnScreenChange(screenInfo, ScreenChangeEvent::UPDATE_ROTATION);
    ASSERT_EQ(0,
              ScreenSessionManagerAdapter::GetInstance()
                  .dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER)
                  .size());

    ScreenSessionManagerAdapter ScreenSessionManagerAdapter;
    screenInfo = new ScreenInfo();
    ScreenChangeEvent screenChangeEvent = ScreenChangeEvent::CHANGE_MODE;
    ScreenSessionManagerAdapter.OnScreenChange(screenInfo, screenChangeEvent);
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    ScreenSessionManagerAdapter.dmAgentContainer_.agentMap_.insert(
        { DisplayManagerAgentType::SCREEN_EVENT_LISTENER, setIDisplay });
    ScreenSessionManagerAdapter.OnScreenChange(screenInfo, screenChangeEvent);
    int result =
        ScreenSessionManagerAdapter.dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER)
            .size();
    ASSERT_EQ(result, 1);
}

/**

@tc.name: OnDisplayCreate

@tc.desc: OnDisplayCreate test

@tc.type: FUNC
*/
HWTEST_F(ScreenSessionManagerAgentTest, OnDisplayCreate, TestSize.Level1)
{
    sptr<DisplayInfo> displayInfo;
    ScreenSessionManagerAdapter::GetInstance().OnDisplayCreate(displayInfo);
    ASSERT_EQ(0,
              ScreenSessionManagerAdapter::GetInstance()
                  .dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER)
                  .size());

    ScreenSessionManagerAdapter ScreenSessionManagerAdapter;
    displayInfo = new DisplayInfo();
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    ScreenSessionManagerAdapter.dmAgentContainer_.agentMap_.insert(
        { DisplayManagerAgentType::DISPLAY_EVENT_LISTENER, setIDisplay });
    ScreenSessionManagerAdapter.OnDisplayCreate(displayInfo);
    int result =
        ScreenSessionManagerAdapter.dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER)
            .size();
    ASSERT_EQ(result, 0);
}

/**

@tc.name: OnDisplayDestroy

@tc.desc: OnDisplayDestroy test

@tc.type: FUNC
*/
HWTEST_F(ScreenSessionManagerAgentTest, OnDisplayDestroy, TestSize.Level1)
{
    DisplayId displayId = 0;
    ScreenSessionManagerAdapter::GetInstance().OnDisplayDestroy(displayId);
    ASSERT_EQ(0,
              ScreenSessionManagerAdapter::GetInstance()
                  .dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER)
                  .size());

    ScreenSessionManagerAdapter ScreenSessionManagerAdapter;
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    ScreenSessionManagerAdapter.dmAgentContainer_.agentMap_.insert(
        { DisplayManagerAgentType::DISPLAY_EVENT_LISTENER, setIDisplay });
    ScreenSessionManagerAdapter.OnDisplayDestroy(displayId);
    int result =
        ScreenSessionManagerAdapter.dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER)
            .size();
    ASSERT_EQ(result, 0);
}

/**

@tc.name: OnDisplayChange

@tc.desc: OnDisplayChange test

@tc.type: FUNC
*/
HWTEST_F(ScreenSessionManagerAgentTest, OnDisplayChange, TestSize.Level1)
{
    sptr<DisplayInfo> displayInfo = nullptr;
    ScreenSessionManagerAdapter::GetInstance().OnDisplayChange(displayInfo, DisplayChangeEvent::UNKNOWN);
    ASSERT_EQ(0,
              ScreenSessionManagerAdapter::GetInstance()
                  .dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER)
                  .size());
    displayInfo = new DisplayInfo();
    ScreenSessionManagerAdapter::GetInstance().OnDisplayChange(displayInfo, DisplayChangeEvent::UNKNOWN);
    ASSERT_EQ(0,
              ScreenSessionManagerAdapter::GetInstance()
                  .dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER)
                  .size());

    DisplayChangeEvent displayChangeEvent = DisplayChangeEvent::UNKNOWN;
    ScreenSessionManagerAdapter ScreenSessionManagerAdapter;
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    ScreenSessionManagerAdapter.dmAgentContainer_.agentMap_.insert(
        { DisplayManagerAgentType::DISPLAY_EVENT_LISTENER, setIDisplay });
    ScreenSessionManagerAdapter.OnDisplayChange(displayInfo, displayChangeEvent);
    int result =
        ScreenSessionManagerAdapter.dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER)
            .size();
    ASSERT_EQ(result, 0);
}

/**

@tc.name: OnScreenshot

@tc.desc: OnScreenshot test

@tc.type: FUNC
*/
HWTEST_F(ScreenSessionManagerAgentTest, OnScreenshot, TestSize.Level1)
{
    sptr <ScreenshotInfo> info = nullptr;
    ScreenSessionManagerAdapter::GetInstance().OnScreenshot(info);
    ASSERT_EQ(0,
              ScreenSessionManagerAdapter::GetInstance()
                  .dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER)
                  .size());

    info = new ScreenshotInfo();
    ScreenSessionManagerAdapter ScreenSessionManagerAdapter;
    ScreenSessionManagerAdapter.OnScreenshot(info);
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    ScreenSessionManagerAdapter.dmAgentContainer_.agentMap_.insert(
        { DisplayManagerAgentType::SCREENSHOT_EVENT_LISTENER, setIDisplay });
    ScreenSessionManagerAdapter.OnScreenshot(info);
    int result =
        ScreenSessionManagerAdapter.dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER)
            .size();
    ASSERT_EQ(result, 0);
}

/**

@tc.name: NotifyPrivateWindowStateChanged
@tc.desc: NotifyPrivateWindowStateChanged test
@tc.type: FUNC
*/
HWTEST_F(ScreenSessionManagerAgentTest, NotifyPrivateWindowStateChanged, TestSize.Level1)
{
    bool hasPrivate = true;
    ScreenSessionManagerAdapter ScreenSessionManagerAdapter;
    ScreenSessionManagerAdapter.NotifyPrivateWindowStateChanged(hasPrivate);
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    ScreenSessionManagerAdapter.dmAgentContainer_.agentMap_.insert(
        { DisplayManagerAgentType::PRIVATE_WINDOW_LISTENER, setIDisplay });
    ScreenSessionManagerAdapter.NotifyPrivateWindowStateChanged(hasPrivate);
    int result =
        ScreenSessionManagerAdapter.dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER)
            .size();
    ASSERT_EQ(result, 0);
}
/**

@tc.name: NotifyFoldStatusChanged
@tc.desc: NotifyFoldStatusChanged test
@tc.type: FUNC
*/
HWTEST_F(ScreenSessionManagerAgentTest, NotifyFoldStatusChanged, TestSize.Level1)
{
    FoldStatus foldStatus = FoldStatus::UNKNOWN;
    ScreenSessionManagerAdapter ScreenSessionManagerAdapter;
    ScreenSessionManagerAdapter.NotifyFoldStatusChanged(foldStatus);
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    ScreenSessionManagerAdapter.dmAgentContainer_.agentMap_.insert(
        { DisplayManagerAgentType::FOLD_STATUS_CHANGED_LISTENER, setIDisplay });
    ScreenSessionManagerAdapter.NotifyFoldStatusChanged(foldStatus);
    int result =
        ScreenSessionManagerAdapter.dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER)
            .size();
    ASSERT_EQ(result, 0);
}
/**

@tc.name: NotifyDisplayModeChanged
@tc.desc: NotifyDisplayModeChanged test
@tc.type: FUNC
*/
HWTEST_F(ScreenSessionManagerAgentTest, NotifyDisplayModeChanged, TestSize.Level1)
{
    FoldDisplayMode displayMode = FoldDisplayMode::UNKNOWN;
    ScreenSessionManagerAdapter ScreenSessionManagerAdapter;
    ScreenSessionManagerAdapter.NotifyDisplayModeChanged(displayMode);
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    ScreenSessionManagerAdapter.dmAgentContainer_.agentMap_.insert(
        { DisplayManagerAgentType::DISPLAY_MODE_CHANGED_LISTENER, setIDisplay });
    ScreenSessionManagerAdapter.NotifyDisplayModeChanged(displayMode);
    int result =
        ScreenSessionManagerAdapter.dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER)
            .size();
    ASSERT_EQ(result, 0);
}
/**

@tc.name: OnDisplayChangeWithUid
@tc.desc: OnDisplayChange with uid test
@tc.type: FUNC
*/
HWTEST_F(ScreenSessionManagerAgentTest, OnDisplayChangeWithUid, TestSize.Level1)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    DisplayChangeEvent event = DisplayChangeEvent::UNKNOWN;
    int32_t uid = 1001;
    ScreenSessionManagerAdapter ScreenSessionManagerAdapter;
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    ScreenSessionManagerAdapter.dmAgentContainer_.agentMap_.insert(
        { DisplayManagerAgentType::DISPLAY_EVENT_LISTENER, setIDisplay });
    ScreenSessionManagerAdapter.OnDisplayChange(displayInfo, event, uid);
    int result =
        ScreenSessionManagerAdapter.dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_EVENT_LISTENER)
            .size();
    ASSERT_EQ(result, 1);
}
/**

@tc.name: OnDisplayChangeWithoutUid
@tc.desc: OnDisplayChange without uid test
@tc.type: FUNC
*/
HWTEST_F(ScreenSessionManagerAgentTest, OnDisplayChangeWithoutUid, TestSize.Level1)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    DisplayChangeEvent event = DisplayChangeEvent::UNKNOWN;
    ScreenSessionManagerAdapter ScreenSessionManagerAdapter;
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    ScreenSessionManagerAdapter.dmAgentContainer_.agentMap_.insert(
        { DisplayManagerAgentType::DISPLAY_EVENT_LISTENER, setIDisplay });
    ScreenSessionManagerAdapter.OnDisplayChange(displayInfo, event);
    int result =
        ScreenSessionManagerAdapter.dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_EVENT_LISTENER)
            .size();
    ASSERT_EQ(result, 1);
}
/**

@tc.name: NotifyScreenModeChange
@tc.desc: NotifyScreenModeChange test
@tc.type: FUNC
*/
HWTEST_F(ScreenSessionManagerAgentTest, NotifyScreenModeChange, TestSize.Level1)
{
    std::vector<sptr<ScreenInfo>> screenInfos;
    screenInfos.push_back(new ScreenInfo());
    ScreenSessionManagerAdapter ScreenSessionManagerAdapter;
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    ScreenSessionManagerAdapter.dmAgentContainer_.agentMap_.insert(
        { DisplayManagerAgentType::SCREEN_EVENT_LISTENER, setIDisplay });
    ScreenSessionManagerAdapter.NotifyScreenModeChange(screenInfos);
    int result =
        ScreenSessionManagerAdapter.dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER)
            .size();
    ASSERT_EQ(result, 1);
}
} // namespace
} // namespace Rosen
} // namespace OHOS