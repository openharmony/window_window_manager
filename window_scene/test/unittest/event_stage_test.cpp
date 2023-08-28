/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "event_stage.h"
#include <gtest/gtest.h>
#include "proto.h"
#include "util.h"
#include "window_manager_hilog.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class EventStageTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    EventStage eventStage_;
};

void EventStageTest::SetUpTestCase()
{
}

void EventStageTest::TearDownTestCase()
{
}

void EventStageTest::SetUp()
{
}

void EventStageTest::TearDown()
{
}

namespace {
/**
 * @tc.name: SetAnrStatus
 * @tc.desc: test function : SetAnrStatus
 * @tc.type: FUNC
 */
HWTEST_F(EventStageTest, SetAnrStatus, Function | SmallTest | Level1)
{
    int32_t persistentId = 0;
    bool status = false;
    eventStage_.SetAnrStatus(persistentId, status);
    status = true;
    eventStage_.SetAnrStatus(persistentId, status);
}

/**
 * @tc.name: CheckAnrStatus
 * @tc.desc: test function : CheckAnrStatus
 * @tc.type: FUNC
 */
HWTEST_F(EventStageTest, CheckAnrStatus, Function | SmallTest | Level1)
{
    int32_t persistentId = 0;
    eventStage_.CheckAnrStatus(persistentId);
}

/**
 * @tc.name: SaveANREvent
 * @tc.desc: test function : SaveANREvent
 * @tc.type: FUNC
 */
HWTEST_F(EventStageTest, SaveANREvent, Function | SmallTest | Level1)
{
    int32_t persistentId = 0;
    int32_t eventId = 0;
    int32_t timerId = 0;
    eventStage_.SaveANREvent(persistentId, eventId, timerId);
}

/**
 * @tc.name: GetTimerIds
 * @tc.desc: test function : GetTimerIds
 * @tc.type: FUNC
 */
HWTEST_F(EventStageTest, GetTimerIds, Function | SmallTest | Level1)
{
    int32_t persistentId = 0;
    eventStage_.GetTimerIds(persistentId);
}

/**
 * @tc.name: DelEvents
 * @tc.desc: test function : DelEvents
 * @tc.type: FUNC
 */
HWTEST_F(EventStageTest, DelEvents, Function | SmallTest | Level1)
{
    int32_t persistentId = 0;
    int32_t eventId = 0;
    eventStage_.DelEvents(persistentId, eventId);
}
}
}
}