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

#include "display_manager_agent_controller.h"
#include "../../dm/include/display_manager_agent_default.h"
#include "scene_board_judgement.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DisplayManagerAgentControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DisplayManagerAgentControllerTest::SetUpTestCase()
{
}

void DisplayManagerAgentControllerTest::TearDownTestCase()
{
}

void DisplayManagerAgentControllerTest::SetUp()
{
}

void DisplayManagerAgentControllerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: NotifyDisplayStateChanged
 * @tc.desc: NotifyDisplayStateChanged test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentControllerTest, NotifyDisplayStateChanged, Function | SmallTest | Level3)
{
    DisplayManagerAgentController displayManagerAgentController;
    DisplayId id = 3;
    DisplayState state = DisplayState::ON;
    bool result = displayManagerAgentController.NotifyDisplayStateChanged(id, state);
    EXPECT_FALSE(result);

    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    displayManagerAgentController.dmAgentContainer_.agentMap_.insert(
        {DisplayManagerAgentType::DISPLAY_STATE_LISTENER, setIDisplay});
    result = displayManagerAgentController.NotifyDisplayStateChanged(id, state);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: OnScreenConnect
 * @tc.desc: OnScreenConnect test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentControllerTest, OnScreenConnect, Function | SmallTest | Level3)
{
    sptr<ScreenInfo> screenInfo = nullptr;
    DisplayManagerAgentController::GetInstance().OnScreenConnect(screenInfo);
    ASSERT_EQ(0, DisplayManagerAgentController::GetInstance().
                dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size());

    DisplayManagerAgentController displayManagerAgentController;
    screenInfo = new ScreenInfo();
    displayManagerAgentController.OnScreenConnect(screenInfo);
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    displayManagerAgentController.dmAgentContainer_.agentMap_.insert(
        {DisplayManagerAgentType::SCREEN_EVENT_LISTENER, setIDisplay});
    displayManagerAgentController.OnScreenConnect(screenInfo);
    int result = displayManagerAgentController.dmAgentContainer_.
        GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size();
    ASSERT_EQ(result, 1);
}
/**
 * @tc.name: OnScreenChange
 * @tc.desc: OnScreenChange test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentControllerTest, OnScreenChange, Function | SmallTest | Level3)
{
    sptr<ScreenInfo> screenInfo = nullptr;
    DisplayManagerAgentController::GetInstance().OnScreenChange(screenInfo, ScreenChangeEvent::UPDATE_ROTATION);
    ASSERT_EQ(0, DisplayManagerAgentController::GetInstance().
                dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size());

    DisplayManagerAgentController displayManagerAgentController;
    screenInfo = new ScreenInfo();
    ScreenChangeEvent screenChangeEvent = ScreenChangeEvent::CHANGE_MODE;
    displayManagerAgentController.OnScreenChange(screenInfo, screenChangeEvent);
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    displayManagerAgentController.dmAgentContainer_.agentMap_.insert(
        {DisplayManagerAgentType::SCREEN_EVENT_LISTENER, setIDisplay});
    displayManagerAgentController.OnScreenChange(screenInfo, screenChangeEvent);
    int result = displayManagerAgentController.dmAgentContainer_.
        GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size();
    ASSERT_EQ(result, 1);
}
/**
 * @tc.name: OnScreenGroupChange
 * @tc.desc: OnScreenChange test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentControllerTest, OnScreenGroupChange, Function | SmallTest | Level3)
{
    sptr<ScreenInfo> screenInfo = nullptr;
    std::string trigger;
    DisplayManagerAgentController::GetInstance().OnScreenGroupChange(trigger, screenInfo,
        ScreenGroupChangeEvent::ADD_TO_GROUP);
    ASSERT_EQ(0, DisplayManagerAgentController::GetInstance().
                dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size());
    std::vector<sptr<ScreenInfo>> screenInfos;
    screenInfos.push_back(screenInfo);
    DisplayManagerAgentController::GetInstance().OnScreenGroupChange(trigger, screenInfos,
        ScreenGroupChangeEvent::ADD_TO_GROUP);

    DisplayManagerAgentController displayManagerAgentController;
    ScreenGroupChangeEvent groupEvent = ScreenGroupChangeEvent::ADD_TO_GROUP;
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    displayManagerAgentController.dmAgentContainer_.agentMap_.insert(
        {DisplayManagerAgentType::SCREEN_EVENT_LISTENER, setIDisplay});
    displayManagerAgentController.OnScreenGroupChange(trigger, screenInfos, groupEvent);
    int result = displayManagerAgentController.dmAgentContainer_.
        GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size();
    ASSERT_EQ(result, 1);
    delete screenInfo;
}
/**
 * @tc.name: OnDisplayCreate
 * @tc.desc: OnDisplayCreate test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentControllerTest, OnDisplayCreate, Function | SmallTest | Level3)
{
    sptr<DisplayInfo> displayInfo;
    DisplayManagerAgentController::GetInstance().OnDisplayCreate(displayInfo);
    ASSERT_EQ(0, DisplayManagerAgentController::GetInstance().
                dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size());

    DisplayManagerAgentController displayManagerAgentController;
    displayInfo = new DisplayInfo();
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    displayManagerAgentController.dmAgentContainer_.agentMap_.insert(
        {DisplayManagerAgentType::DISPLAY_EVENT_LISTENER, setIDisplay});
    displayManagerAgentController.OnDisplayCreate(displayInfo);
    int result = displayManagerAgentController.dmAgentContainer_.
        GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size();
    ASSERT_EQ(result, 0);
    delete displayInfo;
}
/**
 * @tc.name: OnDisplayDestroy
 * @tc.desc: OnDisplayDestroy test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentControllerTest, OnDisplayDestroy, Function | SmallTest | Level3)
{
    DisplayId displayId = 0;
    DisplayManagerAgentController::GetInstance().OnDisplayDestroy(displayId);
    ASSERT_EQ(0, DisplayManagerAgentController::GetInstance().
                dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size());

    DisplayManagerAgentController displayManagerAgentController;
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    displayManagerAgentController.dmAgentContainer_.agentMap_.insert(
        {DisplayManagerAgentType::DISPLAY_EVENT_LISTENER, setIDisplay});
    displayManagerAgentController.OnDisplayDestroy(displayId);
    int result = displayManagerAgentController.dmAgentContainer_.
        GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size();
    ASSERT_EQ(result, 0);
}
/**
 * @tc.name: OnDisplayChange
 * @tc.desc: OnDisplayChange test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentControllerTest, OnDisplayChange, Function | SmallTest | Level3)
{
    sptr<DisplayInfo> displayInfo = nullptr;
    DisplayManagerAgentController::GetInstance().OnDisplayChange(displayInfo, DisplayChangeEvent::UNKNOWN);
    ASSERT_EQ(0, DisplayManagerAgentController::GetInstance().
                dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size());
    displayInfo = new DisplayInfo();
    DisplayManagerAgentController::GetInstance().OnDisplayChange(displayInfo, DisplayChangeEvent::UNKNOWN);
    ASSERT_EQ(0, DisplayManagerAgentController::GetInstance().
                dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size());

    DisplayChangeEvent displayChangeEvent = DisplayChangeEvent::UNKNOWN;
    DisplayManagerAgentController displayManagerAgentController;
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    displayManagerAgentController.dmAgentContainer_.agentMap_.insert(
        {DisplayManagerAgentType::DISPLAY_EVENT_LISTENER, setIDisplay});
    displayManagerAgentController.OnDisplayChange(displayInfo, displayChangeEvent);
    int result = displayManagerAgentController.dmAgentContainer_.
        GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size();
    ASSERT_EQ(result, 0);
}
/**
 * @tc.name: OnScreenshot
 * @tc.desc: OnScreenshot test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentControllerTest, OnScreenshot, Function | SmallTest | Level3)
{
    sptr<ScreenshotInfo> info = nullptr;
    DisplayManagerAgentController::GetInstance().OnScreenshot(info);
    ASSERT_EQ(0, DisplayManagerAgentController::GetInstance().
                dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size());

    info = new ScreenshotInfo();
    DisplayManagerAgentController displayManagerAgentController;
    displayManagerAgentController.OnScreenshot(info);
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    displayManagerAgentController.dmAgentContainer_.agentMap_.insert(
        {DisplayManagerAgentType::SCREENSHOT_EVENT_LISTENER, setIDisplay});
    displayManagerAgentController.OnScreenshot(info);
    int result = displayManagerAgentController.dmAgentContainer_.
        GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size();
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: NotifyPrivateWindowStateChanged
 * @tc.desc: NotifyPrivateWindowStateChanged test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentControllerTest, NotifyPrivateWindowStateChanged, Function | SmallTest | Level3)
{
    bool hasPrivate = true;
    DisplayManagerAgentController displayManagerAgentController;
    displayManagerAgentController.NotifyPrivateWindowStateChanged(hasPrivate);
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    displayManagerAgentController.dmAgentContainer_.agentMap_.insert(
        {DisplayManagerAgentType::PRIVATE_WINDOW_LISTENER, setIDisplay});
    displayManagerAgentController.NotifyPrivateWindowStateChanged(hasPrivate);
    int result = displayManagerAgentController.dmAgentContainer_.
        GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size();
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: NotifyFoldStatusChanged
 * @tc.desc: NotifyFoldStatusChanged test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentControllerTest, NotifyFoldStatusChanged, Function | SmallTest | Level3)
{
    FoldStatus foldStatus = FoldStatus::UNKNOWN;
    DisplayManagerAgentController displayManagerAgentController;
    displayManagerAgentController.NotifyFoldStatusChanged(foldStatus);
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    displayManagerAgentController.dmAgentContainer_.agentMap_.insert(
        {DisplayManagerAgentType::FOLD_STATUS_CHANGED_LISTENER, setIDisplay});
    displayManagerAgentController.NotifyFoldStatusChanged(foldStatus);
    int result = displayManagerAgentController.dmAgentContainer_.
        GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size();
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: NotifyDisplayModeChanged
 * @tc.desc: NotifyDisplayModeChanged test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentControllerTest, NotifyDisplayModeChanged, Function | SmallTest | Level3)
{
    FoldDisplayMode displayMode = FoldDisplayMode::UNKNOWN;
    DisplayManagerAgentController displayManagerAgentController;
    displayManagerAgentController.NotifyDisplayModeChanged(displayMode);
    sptr<IDisplayManagerAgent> idma_ = new DisplayManagerAgentDefault();
    std::set<sptr<IDisplayManagerAgent>> setIDisplay;
    setIDisplay.insert(idma_);
    displayManagerAgentController.dmAgentContainer_.agentMap_.insert(
        {DisplayManagerAgentType::DISPLAY_MODE_CHANGED_LISTENER, setIDisplay});
    displayManagerAgentController.NotifyDisplayModeChanged(displayMode);
    int result = displayManagerAgentController.dmAgentContainer_.
        GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size();
    ASSERT_EQ(result, 0);
}
}
} // namespace Rosen
} // namespace OHOS
