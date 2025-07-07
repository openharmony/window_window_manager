/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include <common_event_manager.h>

#include "screen_session_manager/include/publish/screen_session_publish.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_US = 100000;
const std::string CAST_PLUG_IN_FLAG_DATA = "1";
const std::string CAST_PLUG_OUT_FLAG_DATA = "0";
const std::string COMMON_EVENT_DISPLAY_ROTATION_CHANGED = "custom.event.display_rotation_changed";
const std::string COMMON_EVENT_CAST_PLUGGED_CHANGED = "custom.event.cast_plugged_changed";
const std::string COMMON_EVENT_SMART_NOTIFICATION = "hicare.event.SMART_NOTIFICATION";
const std::string COMMON_EVENT_LOW_TEMP_WARNING = "usual.event.thermal.LOW_TEMP_WARNING";
constexpr int32_t PUBLISH_FAILURE = -1;
const std::string FAULT_DESCRIPTION = "842003014";
const std::string FAULT_SUGGESTION = "542003014";
}

class ScreenSessionPublishTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ScreenSessionPublishTest::SetUpTestCase()
{
}

void ScreenSessionPublishTest::TearDownTestCase()
{
}

void ScreenSessionPublishTest::SetUp()
{
}

void ScreenSessionPublishTest::TearDown()
{
    usleep(SLEEP_TIME_US);
}

namespace {

/**
 * @tc.name: InitPublishEvents
 * @tc.desc: InitPublishEvents
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionPublishTest, InitPublishEvents, TestSize.Level1)
{
    auto screenSessionPublish = ScreenSessionPublish::GetInstance();
    screenSessionPublish.InitPublishEvents();
    ASSERT_NE(screenSessionPublish.publishInfo_, nullptr);

    screenSessionPublish.publishInfo_ = nullptr;
    screenSessionPublish.cesWantMap_.clear();
    std::string event = "test";
    sptr<EventFwk::Want> want = nullptr;
    screenSessionPublish.cesWantMap_.insert(std::make_pair(event, want));
    screenSessionPublish.InitPublishEvents();
    ASSERT_NE(screenSessionPublish.publishInfo_, nullptr);
    ASSERT_EQ(screenSessionPublish.cesWantMap_.size(), 1);
}

/**
 * @tc.name: PublishEvents
 * @tc.desc: PublishEvents
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionPublishTest, PublishEvents, TestSize.Level1)
{
    auto screenSessionPublish = ScreenSessionPublish::GetInstance();
    EventFwk::CommonEventData eventData;
    std::string bundleName = "test";
    int32_t ret = screenSessionPublish.PublishEvents(eventData, bundleName);
    ASSERT_EQ(ret, PUBLISH_FAILURE);

    screenSessionPublish.InitPublishEvents();
    ret = screenSessionPublish.PublishEvents(eventData, bundleName);
    ASSERT_EQ(ret, PUBLISH_FAILURE);

    bundleName = "";
    ret = screenSessionPublish.PublishEvents(eventData, bundleName);
    ASSERT_EQ(ret, PUBLISH_FAILURE);
}

/**
 * @tc.name: PublishCastPluggedEvent
 * @tc.desc: PublishCastPluggedEvent
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionPublishTest, PublishCastPluggedEvent, TestSize.Level1)
{
    auto screenSessionPublish = ScreenSessionPublish::GetInstance();
    bool isEnable = false;
    screenSessionPublish.PublishCastPluggedEvent(isEnable);
    ASSERT_FALSE(isEnable);

    isEnable = true;
    sptr<EventFwk::Want> want = new (std::nothrow) EventFwk::Want();
    screenSessionPublish.cesWantMap_.insert(std::make_pair(COMMON_EVENT_CAST_PLUGGED_CHANGED, want));
    screenSessionPublish.PublishCastPluggedEvent(isEnable);
    ASSERT_TRUE(isEnable);
}

/**
 * @tc.name: PublishDisplayRotationEvent
 * @tc.desc: PublishDisplayRotationEvent
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionPublishTest, PublishDisplayRotationEvent, TestSize.Level1)
{
    auto screenSessionPublish = ScreenSessionPublish::GetInstance();
    screenSessionPublish.cesWantMap_.clear();
    ScreenId screenId = 1;
    Rotation displayRotation = Rotation::ROTATION_0;
    screenSessionPublish.PublishDisplayRotationEvent(screenId, displayRotation);
    ASSERT_EQ(screenSessionPublish.cesWantMap_.size(), 1);

    screenSessionPublish.cesWantMap_.clear();
    sptr<EventFwk::Want> want = new (std::nothrow) EventFwk::Want();
    screenSessionPublish.cesWantMap_.insert(std::make_pair(COMMON_EVENT_DISPLAY_ROTATION_CHANGED, want));
    screenSessionPublish.PublishDisplayRotationEvent(screenId, displayRotation);
    ASSERT_EQ(screenSessionPublish.cesWantMap_.size(), 2);
}

/**
 * @tc.name: PublishSmartNotificationEvent
 * @tc.desc: PublishSmartNotificationEvent
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionPublishTest, PublishSmartNotificationEvent, TestSize.Level1)
{
    auto screenSessionPublish = ScreenSessionPublish::GetInstance();
    screenSessionPublish.cesWantMap_.clear();
    screenSessionPublish.PublishSmartNotificationEvent(FAULT_DESCRIPTION, FAULT_SUGGESTION);
    ASSERT_EQ(screenSessionPublish.cesWantMap_.size(), 1);

    screenSessionPublish.cesWantMap_.clear();
    sptr<EventFwk::Want> want = new (std::nothrow) EventFwk::Want();
    screenSessionPublish.cesWantMap_.insert(std::make_pair(COMMON_EVENT_SMART_NOTIFICATION, want));
    screenSessionPublish.PublishSmartNotificationEvent(FAULT_DESCRIPTION, FAULT_SUGGESTION);
    ASSERT_EQ(screenSessionPublish.cesWantMap_.size(), 1);
}

/**
 * @tc.name: RegisterLowTempSubscriber
 * @tc.desc: RegisterLowTempSubscriber
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionPublishTest, RegisterLowTempSubscriber, TestSize.Level1)
{
    auto screenSessionPublish = ScreenSessionPublish::GetInstance();
    screenSessionPublish.subscriber_ = nullptr;
    ASSERT_EQ(screenSessionPublish.RegisterLowTempSubscriber(), true);
    ASSERT_EQ(screenSessionPublish.RegisterLowTempSubscriber(), false);
}

/**
 * @tc.name: UnRegisterLowTempSubscriber
 * @tc.desc: UnRegisterLowTempSubscriber
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionPublishTest, UnRegisterLowTempSubscriber, TestSize.Level1)
{
    auto screenSessionPublish = ScreenSessionPublish::GetInstance();
    screenSessionPublish.RegisterLowTempSubscriber();
    ASSERT_EQ(screenSessionPublish.UnRegisterLowTempSubscriber(), true);
    screenSessionPublish.subscriber_ = nullptr;
    ASSERT_EQ(screenSessionPublish.UnRegisterLowTempSubscriber(), false);
}

/**
 * @tc.name: RegisterUserSwitchedSubscriber
 * @tc.desc: RegisterUserSwitchedSubscriber
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionPublishTest, RegisterUserSwitchedSubscriber, TestSize.Level1)
{
    auto screenSessionPublish = ScreenSessionPublish::GetInstance();
    screenSessionPublish.userSwitchedSubscriber_ = nullptr;
    ASSERT_EQ(screenSessionPublish.RegisterUserSwitchedSubscriber(), true);
    ASSERT_EQ(screenSessionPublish.RegisterUserSwitchedSubscriber(), false);
}

/**
 * @tc.name: UnRegisterUserSwitchedSubscriber
 * @tc.desc: UnRegisterUserSwitchedSubscriber
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionPublishTest, UnRegisterUserSwitchedSubscriber, TestSize.Level1)
{
    auto screenSessionPublish = ScreenSessionPublish::GetInstance();
    screenSessionPublish.RegisterUserSwitchedSubscriber();
    ASSERT_EQ(screenSessionPublish.UnRegisterUserSwitchedSubscriber(), true);
    screenSessionPublish.userSwitchedSubscriber_ = nullptr;
    ASSERT_EQ(screenSessionPublish.UnRegisterUserSwitchedSubscriber(), false);
}
}
} // namespace Rosen
} // namespace OHOS