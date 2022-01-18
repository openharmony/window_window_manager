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
#include "display_manager.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "ScreenshotCmdTest"};
}

class DisplayPowerEventListener : public IDisplayPowerEventListener {
public:
    virtual void OnDisplayPowerEvent(DisplayPowerEvent event, EventStatus status)
    {
        isCallbackCalled_ = true;
        event_ = event;
        status_ = status;
    }
    DisplayPowerEvent event_;
    EventStatus status_;
    bool isCallbackCalled_ { false };
};

class DisplayPowerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    static inline DisplayId defaultId_;
    static sptr<DisplayPowerEventListener> listener_;
    DisplayState state_ { DisplayState::ON };
    bool isDisplayStateCallbackCalled_ { false };
    DisplayStateCallback callback_ = [this](DisplayState state) {
        isDisplayStateCallbackCalled_ = true;
        state_ = state;
    };
};

sptr<DisplayPowerEventListener> DisplayPowerTest::listener_ = new DisplayPowerEventListener();

void DisplayPowerTest::SetUpTestCase()
{
    defaultId_ = DisplayManager::GetInstance().GetDefaultDisplayId();
    if (defaultId_ == INVALID_DISPLAY_ID) {
        WLOGFE("GetDefaultDisplayId failed!");
    }
    DisplayManager::GetInstance().RegisterDisplayPowerEventListener(listener_);
}

void DisplayPowerTest::TearDownTestCase()
{
    DisplayManager::GetInstance().UnregisterDisplayPowerEventListener(listener_);
}

void DisplayPowerTest::SetUp()
{
    isDisplayStateCallbackCalled_ = false;
    state_ = DisplayState::UNKNOWN;

    listener_->isCallbackCalled_ = false;
    listener_->event_ = static_cast<DisplayPowerEvent>(-1);
    listener_->status_ = static_cast<EventStatus>(-1);
}

void DisplayPowerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: SetDisplayStateValid
 * @tc.desc: Call SetDisplayState and check if it the state set is the same as calling GetDisplayState
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerTest, SetDisplayStateValid01, Function | MediumTest | Level2)
{
    DisplayState initialState = DisplayManager::GetInstance().GetDisplayState(defaultId_);
    DisplayState stateToSet = (initialState == DisplayState::OFF ? DisplayState::ON : DisplayState::OFF);
    bool ret = DisplayManager::GetInstance().SetDisplayState(stateToSet, callback_);
    ASSERT_EQ(true, ret);
    DisplayState stateGet = DisplayManager::GetInstance().GetDisplayState(defaultId_);
    ASSERT_EQ(stateGet, stateToSet);
}

/**
 * @tc.name: SetDisplayStateValid
 * @tc.desc: Call SetDisplayState to set a value already set and check the return value
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerTest, SetDisplayStateValid02, Function | MediumTest | Level2)
{
    DisplayState initialState = DisplayManager::GetInstance().GetDisplayState(defaultId_);
    bool ret = DisplayManager::GetInstance().SetDisplayState(initialState, callback_);
    ASSERT_EQ(false, ret);
    DisplayState stateGet = DisplayManager::GetInstance().GetDisplayState(defaultId_);
    ASSERT_EQ(stateGet, initialState);
    ASSERT_EQ(false, isDisplayStateCallbackCalled_);
}

/**
 * @tc.name: SetDisplayStateCallbackValid
 * @tc.desc: Call SetDisplayState and check if callback state is correct
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerTest, SetDisplayStateCallbackValid01, Function | MediumTest | Level2)
{
    DisplayState initialState = DisplayManager::GetInstance().GetDisplayState(defaultId_);
    DisplayState stateToSet = (initialState == DisplayState::OFF ? DisplayState::ON : DisplayState::OFF);
    DisplayManager::GetInstance().SetDisplayState(stateToSet, callback_);
    sleep(1);
    ASSERT_EQ(true, isDisplayStateCallbackCalled_);
    ASSERT_EQ(state_, stateToSet);
}

/**
 * @tc.name: SetDisplayStateCallbackValid
 * @tc.desc: Call SetDisplayState to set a value already set and check the DisplayStateCallback
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerTest, SetDisplayStateCallbackValid02, Function | MediumTest | Level2)
{
    DisplayState initialState = DisplayManager::GetInstance().GetDisplayState(defaultId_);
    DisplayManager::GetInstance().SetDisplayState(initialState, callback_);
    ASSERT_EQ(false, isDisplayStateCallbackCalled_);
}

/**
 * @tc.name: WakeUpBeginCallbackValid
 * @tc.desc: Call WakeUpBegin and check the OnDisplayPowerEvent callback is called
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerTest, WakeUpBeginCallbackValid01, Function | MediumTest | Level2)
{
    bool ret = DisplayManager::GetInstance().WakeUpBegin(PowerStateChangeReason::POWER_BUTTON);
    ASSERT_EQ(true, ret);
    sleep(1);
    ASSERT_EQ(true, listener_->isCallbackCalled_);
    ASSERT_EQ(DisplayPowerEvent::WAKE_UP, listener_->event_);
    ASSERT_EQ(EventStatus::BEGIN, listener_->status_);
}

/**
 * @tc.name: WakeUpEndCallbackValid
 * @tc.desc: Call WakeUpBegin and check the OnDisplayPowerEvent callback is called
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerTest, WakeUpEndCallbackValid01, Function | MediumTest | Level2)
{
    bool ret = DisplayManager::GetInstance().WakeUpEnd();
    ASSERT_EQ(true, ret);
    sleep(1);
    ASSERT_EQ(true, listener_->isCallbackCalled_);
    ASSERT_EQ(DisplayPowerEvent::WAKE_UP, listener_->event_);
    ASSERT_EQ(EventStatus::END, listener_->status_);
}

/**
 * @tc.name: SuspendBeginCallbackValid
 * @tc.desc: Call SuspendBegin and check the OnDisplayPowerEvent callback is called
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerTest, SuspendBeginCallbackValid01, Function | MediumTest | Level2)
{
    bool ret = DisplayManager::GetInstance().SuspendBegin(PowerStateChangeReason::POWER_BUTTON);
    ASSERT_EQ(true, ret);
    sleep(1);
    ASSERT_EQ(true, listener_->isCallbackCalled_);
    ASSERT_EQ(DisplayPowerEvent::SLEEP, listener_->event_);
    ASSERT_EQ(EventStatus::BEGIN, listener_->status_);
}

/**
* @tc.name: SuspendEndCallbackValid
* @tc.desc: Call SuspendEnd and check the OnDisplayPowerEvent callback is called
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerTest, SuspendEndCallbackValid01, Function | MediumTest | Level2)
{
    bool ret = DisplayManager::GetInstance().SuspendEnd();
    ASSERT_EQ(true, ret);
    sleep(1);
    ASSERT_EQ(true, listener_->isCallbackCalled_);
    ASSERT_EQ(DisplayPowerEvent::SLEEP, listener_->event_);
    ASSERT_EQ(EventStatus::END, listener_->status_);
}

/**
* @tc.name: SetScreenPowerForAllCallbackValid
* @tc.desc: Call SetScreenPowerForAll OFF and check the OnDisplayPowerEvent callback is called
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerTest, SetScreenPowerForAllCallbackValid01, Function | MediumTest | Level2)
{
    bool ret = DisplayManager::GetInstance().SetScreenPowerForAll(DisplayPowerState::POWER_OFF,
        PowerStateChangeReason::POWER_BUTTON);
    ASSERT_EQ(true, ret);
    sleep(1);
    ASSERT_EQ(true, listener_->isCallbackCalled_);
    ASSERT_EQ(DisplayPowerEvent::DISPLAY_OFF, listener_->event_);
    ASSERT_EQ(EventStatus::END, listener_->status_);
}

/**
* @tc.name: SetScreenPowerForAllCallbackValid
* @tc.desc: Call SetScreenPowerForAll ON and check the OnDisplayPowerEvent callback is called
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerTest, SetScreenPowerForAllCallbackValid02, Function | MediumTest | Level2)
{
    bool ret = DisplayManager::GetInstance().SetScreenPowerForAll(DisplayPowerState::POWER_ON,
        PowerStateChangeReason::POWER_BUTTON);
    ASSERT_EQ(true, ret);
    sleep(1);
    ASSERT_EQ(true, listener_->isCallbackCalled_);
    ASSERT_EQ(DisplayPowerEvent::DISPLAY_ON, listener_->event_);
    ASSERT_EQ(EventStatus::END, listener_->status_);
}

/**
* @tc.name: SetDisplayStatePowerCallbackValid
* @tc.desc: Call SetScreenPowerForAll OFF and check the OnDisplayPowerEvent callback is called
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerTest, SetDisplayStatePowerCallbackValid01, Function | MediumTest | Level2)
{
    bool ret = DisplayManager::GetInstance().SetDisplayState(DisplayState::OFF, callback_);
    ASSERT_EQ(true, ret);
    sleep(1);
    ASSERT_EQ(true, listener_->isCallbackCalled_);
    ASSERT_EQ(DisplayPowerEvent::DISPLAY_OFF, listener_->event_);
    ASSERT_EQ(EventStatus::BEGIN, listener_->status_);
}

/**
* @tc.name: SetDisplayStatePowerCallbackValid
* @tc.desc: Call SetScreenPowerForAll ON and check the OnDisplayPowerEvent callback is called
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerTest, SetDisplayStatePowerCallbackValid02, Function | MediumTest | Level2)
{
    bool ret = DisplayManager::GetInstance().SetDisplayState(DisplayState::ON, callback_);
    ASSERT_EQ(true, ret);
    sleep(1);
    ASSERT_EQ(true, listener_->isCallbackCalled_);
    ASSERT_EQ(DisplayPowerEvent::DISPLAY_ON, listener_->event_);
    ASSERT_EQ(EventStatus::BEGIN, listener_->status_);
}

/**
* @tc.name: SetScreenPowerForAllValid
* @tc.desc: Call SetScreenPowerForAll OFF and check the GetScreenPower return value
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerTest, SetScreenPowerForAllValid01, Function | MediumTest | Level2)
{
    DisplayPowerState stateToSet = DisplayPowerState::POWER_OFF;
    bool ret = DisplayManager::GetInstance().SetScreenPowerForAll(stateToSet, PowerStateChangeReason::POWER_BUTTON);
    ASSERT_EQ(true, ret);
    DisplayPowerState stateGet = DisplayManager::GetInstance().GetScreenPower(defaultId_);
    ASSERT_EQ(stateGet, stateToSet);
}

/**
* @tc.name: SetScreenPowerForAllValid
* @tc.desc: Call SetScreenPowerForAll ON and check the GetScreenPower return value
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerTest, SetScreenPowerForAllValid02, Function | MediumTest | Level2)
{
    DisplayPowerState stateToSet = DisplayPowerState::POWER_ON;
    bool ret = DisplayManager::GetInstance().SetScreenPowerForAll(stateToSet, PowerStateChangeReason::POWER_BUTTON);
    ASSERT_EQ(true, ret);
    DisplayPowerState stateGet = DisplayManager::GetInstance().GetScreenPower(defaultId_);
    ASSERT_EQ(stateGet, stateToSet);
}
} // namespace
} // namespace Rosen
} // namespace OHOS