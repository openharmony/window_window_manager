/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "common_test_utils.h"
#include "display_manager.h"
#include "display_manager_proxy.h"
#include "screen_manager.h"
#include "window.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayPowerTest"};
    constexpr uint32_t MAX_TIME_WAITING_FOR_CALLBACK = 40;
    constexpr uint32_t SLEEP_TIME_IN_US = 50000;
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

    static void CheckDisplayStateCallback(bool valueExpected);
    static void CheckDisplayPowerEventCallback(bool valueExpected);

    static inline DisplayId defaultId_;
    static inline uint32_t brightnessLevel_ = 80;
    static inline uint32_t invalidBrightnessLevel_ = 1000000000;
    static inline uint32_t times_ = 0;
    static inline bool isDisplayStateCallbackCalled_ = false;
    static sptr<DisplayPowerEventListener> listener_;

    DisplayState state_ { DisplayState::ON };
    DisplayStateCallback callback_ = [this](DisplayState state) {
        isDisplayStateCallbackCalled_ = true;
        state_ = state;
    };
};

sptr<DisplayPowerEventListener> DisplayPowerTest::listener_ = new DisplayPowerEventListener();

void DisplayPowerTest::SetUpTestCase()
{
    CommonTestUtils::SetAceessTokenPermission("SetDisplayState");
    defaultId_ = DisplayManager::GetInstance().GetDefaultDisplayId();
    if (defaultId_ == DISPLAY_ID_INVALID) {
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
    times_ = 0;

    isDisplayStateCallbackCalled_ = false;
    state_ = DisplayState::UNKNOWN;

    listener_->isCallbackCalled_ = false;
    listener_->event_ = static_cast<DisplayPowerEvent>(-1);
    listener_->status_ = static_cast<EventStatus>(-1);
}

void DisplayPowerTest::TearDown()
{
}

void DisplayPowerTest::CheckDisplayStateCallback(bool valueExpected)
{
    do {
        if (isDisplayStateCallbackCalled_ == valueExpected) {
            return;
        }
        usleep(SLEEP_TIME_IN_US);
        ++times_;
    } while (times_ <= MAX_TIME_WAITING_FOR_CALLBACK);
}

void DisplayPowerTest::CheckDisplayPowerEventCallback(bool valueExpected)
{
    do {
        if (listener_->isCallbackCalled_ == valueExpected) {
            return;
        }
        usleep(SLEEP_TIME_IN_US);
        ++times_;
    } while (times_ <= MAX_TIME_WAITING_FOR_CALLBACK);
}

namespace {
} // namespace
} // namespace Rosen
} // namespace OHOS