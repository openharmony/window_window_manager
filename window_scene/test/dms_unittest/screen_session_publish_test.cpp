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

#include "session_manager/include/publish/screen_session_publish.h"
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
    constexpr int32_t PUBLISH_FAILURE = -1;
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
    HWTEST_F(ScreenSessionPublishTest, InitPublishEvents, Function | SmallTest | Level3)
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
    HWTEST_F(ScreenSessionPublishTest, PublishEvents, Function | SmallTest | Level3)
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
    HWTEST_F(ScreenSessionPublishTest, PublishCastPluggedEvent, Function | SmallTest | Level3)
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
    HWTEST_F(ScreenSessionPublishTest, PublishDisplayRotationEvent, Function | SmallTest | Level3)
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
        ASSERT_EQ(screenSessionPublish.cesWantMap_.size(), 1);
    }
}
} // namespace Rosen
} // namespace OHOS