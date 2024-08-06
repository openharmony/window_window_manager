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

#include "intention_event/framework/anr_handler/include/anr_handler.h"
#include <gtest/gtest.h>
#include "proto.h"
#include "util.h"
#include "window_manager_hilog.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr int64_t MAX_MARK_PROCESS_DELAY_TIME_US { 3000000 };
constexpr int32_t INVALID_PERSISTENT_ID { -1 };
const std::string ANR_HANDLER_RUNNER { "ANR_HANDLER" };
class AnrHandlerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    ANRHandler anrHandler_;
    static constexpr uint32_t WAIT_SYNC_IN_NS = 1000000;
};

void AnrHandlerTest::SetUpTestCase()
{
}

void AnrHandlerTest::TearDownTestCase()
{
}

void AnrHandlerTest::SetUp()
{
}

void AnrHandlerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: HandleEventConsumed
 * @tc.desc: test function : HandleEventConsumed
 * @tc.type: FUNC
 */
HWTEST_F(AnrHandlerTest, HandleEventConsumed, Function | SmallTest | Level1)
{
    ANRHandler::SessionInfo sessionInfo;
    anrHandler_.sessionStageMap_.clear();
    anrHandler_.sessionStageMap_.emplace(0, sessionInfo);
    anrHandler_.sessionStageMap_.emplace(1, sessionInfo);
    anrHandler_.anrHandlerState_.sendStatus.emplace(0, true);
    anrHandler_.HandleEventConsumed(0, 0);
    ASSERT_EQ(anrHandler_.sessionStageMap_.size(), 2);

    anrHandler_.anrHandlerState_.sendStatus[0] = false;
    anrHandler_.HandleEventConsumed(0, MAX_MARK_PROCESS_DELAY_TIME_US);
    anrHandler_.HandleEventConsumed(0, 0);
}

/**
 * @tc.name: OnWindowDestroyed
 * @tc.desc: test function : OnWindowDestroyed
 * @tc.type: FUNC
 */
HWTEST_F(AnrHandlerTest, OnWindowDestroyed, Function | SmallTest | Level1)
{
    ANRHandler::SessionInfo sessionInfo;
    anrHandler_.sessionStageMap_.clear();
    anrHandler_.anrHandlerState_.sendStatus.emplace(0, true);
    anrHandler_.OnWindowDestroyed(0);
    usleep(WAIT_SYNC_IN_NS);
    ASSERT_EQ(anrHandler_.anrHandlerState_.sendStatus.size(), 0);

    anrHandler_.anrHandlerState_.sendStatus.emplace(0, true);
    anrHandler_.sessionStageMap_.emplace(0, sessionInfo);
    anrHandler_.OnWindowDestroyed(0);
    usleep(WAIT_SYNC_IN_NS);
    ASSERT_EQ(anrHandler_.anrHandlerState_.sendStatus.size(), 0);
    ASSERT_EQ(anrHandler_.sessionStageMap_.size(), 0);

    anrHandler_.anrHandlerState_.sendStatus.emplace(0, true);
    anrHandler_.sessionStageMap_.clear();
    anrHandler_.sessionStageMap_.emplace(1, sessionInfo);
    anrHandler_.OnWindowDestroyed(0);
}

/**
 * @tc.name: PostTask
 * @tc.desc: test function : PostTask
 * @tc.type: FUNC
 */
HWTEST_F(AnrHandlerTest, PostTask, Function | SmallTest | Level1)
{
    auto eventHandler = anrHandler_.eventHandler_;
    anrHandler_.eventHandler_ = nullptr;
    auto task = []() {
        return;
    };
    bool ret = anrHandler_.PostTask(task, "testTask", 0);
    anrHandler_.eventHandler_ = eventHandler;
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: SetAnrHandleState
 * @tc.desc: test function : SetAnrHandleState
 * @tc.type: FUNC
 */
HWTEST_F(AnrHandlerTest, SetAnrHandleState, Function | SmallTest | Level1)
{
    anrHandler_.anrHandlerState_.eventsToReceipt.clear();
    anrHandler_.SetAnrHandleState(0, true);
    ASSERT_EQ(anrHandler_.anrHandlerState_.eventsToReceipt.size(), 1);

    anrHandler_.anrHandlerState_.eventsToReceipt.clear();
    anrHandler_.SetAnrHandleState(0, false);
    ASSERT_EQ(anrHandler_.anrHandlerState_.eventsToReceipt.size(), 0);

    anrHandler_.anrHandlerState_.eventsToReceipt.clear();
    anrHandler_.anrHandlerState_.eventsToReceipt.push_back(0);
    anrHandler_.anrHandlerState_.eventsToReceipt.push_back(1);
    anrHandler_.SetAnrHandleState(0, false);
    ASSERT_EQ(anrHandler_.anrHandlerState_.eventsToReceipt.size(), 1);
}

/**
 * @tc.name: MarkProcessed
 * @tc.desc: test function : MarkProcessed
 * @tc.type: FUNC
 */
HWTEST_F(AnrHandlerTest, MarkProcessed, Function | SmallTest | Level1)
{
    anrHandler_.anrHandlerState_.eventsToReceipt.clear();
    anrHandler_.anrHandlerState_.sendStatus.clear();
    anrHandler_.MarkProcessed();
    ASSERT_EQ(anrHandler_.anrHandlerState_.sendStatus.size(), 1);

    anrHandler_.sessionStageMap_.clear();
    anrHandler_.anrHandlerState_.eventsToReceipt.clear();
    anrHandler_.anrHandlerState_.eventsToReceipt.push_back(0);
    anrHandler_.MarkProcessed();
    ASSERT_EQ(anrHandler_.anrHandlerState_.sendStatus.size(), 1);

    ANRHandler::SessionInfo sessionInfo;
    anrHandler_.sessionStageMap_.clear();
    anrHandler_.sessionStageMap_.emplace(1, sessionInfo);
    anrHandler_.sessionStageMap_.emplace(0, sessionInfo);
    anrHandler_.MarkProcessed();
    ASSERT_EQ(anrHandler_.sessionStageMap_.size(), 2);
}

/**
 * @tc.name: SendEvent
 * @tc.desc: test function : SendEvent
 * @tc.type: FUNC
 */
HWTEST_F(AnrHandlerTest, SendEvent, Function | SmallTest | Level1)
{
    anrHandler_.sessionStageMap_.clear();
    anrHandler_.anrHandlerState_.eventsToReceipt.clear();
    anrHandler_.anrHandlerState_.sendStatus.clear();
    anrHandler_.SendEvent(0, 0);
    ASSERT_EQ(anrHandler_.anrHandlerState_.sendStatus.size(), 1);

    auto eventHandler = anrHandler_.eventHandler_;
    anrHandler_.eventHandler_ = nullptr;
    anrHandler_.SendEvent(0, 0);
    anrHandler_.eventHandler_ = eventHandler;
    ASSERT_EQ(anrHandler_.anrHandlerState_.sendStatus.size(), 1);
}

/**
 * @tc.name: ClearExpiredEvents
 * @tc.desc: test function : ClearExpiredEvents
 * @tc.type: FUNC
 */
HWTEST_F(AnrHandlerTest, ClearExpiredEvents, Function | SmallTest | Level1)
{
    anrHandler_.sessionStageMap_.clear();
    anrHandler_.anrHandlerState_.eventsToReceipt.clear();
    anrHandler_.anrHandlerState_.sendStatus.clear();
    anrHandler_.ClearExpiredEvents(0);
    ASSERT_EQ(anrHandler_.sessionStageMap_.size(), 0);

    ANRHandler::SessionInfo sessionInfo;
    sessionInfo.persistentId = 1;
    anrHandler_.sessionStageMap_.clear();
    anrHandler_.sessionStageMap_.emplace(2, sessionInfo);
    anrHandler_.sessionStageMap_.emplace(3, sessionInfo);
    anrHandler_.ClearExpiredEvents(3);
    ASSERT_EQ(anrHandler_.sessionStageMap_.size(), 1);

    anrHandler_.sessionStageMap_.clear();
    anrHandler_.sessionStageMap_.emplace(0, sessionInfo);
    anrHandler_.sessionStageMap_.emplace(1, sessionInfo);
    anrHandler_.ClearExpiredEvents(1);
    ASSERT_EQ(anrHandler_.sessionStageMap_.size(), 1);

    anrHandler_.sessionStageMap_.clear();
    anrHandler_.sessionStageMap_.emplace(0, sessionInfo);
    anrHandler_.sessionStageMap_.emplace(1, sessionInfo);
    anrHandler_.sessionStageMap_.emplace(2, sessionInfo);
    anrHandler_.ClearExpiredEvents(2);
    ASSERT_EQ(anrHandler_.sessionStageMap_.size(), 1);
}

/**
 * @tc.name: GetPersistentIdOfEvent
 * @tc.desc: test function : GetPersistentIdOfEvent
 * @tc.type: FUNC
 */
HWTEST_F(AnrHandlerTest, GetPersistentIdOfEvent, Function | SmallTest | Level1)
{
    anrHandler_.sessionStageMap_.clear();
    int32_t ret = anrHandler_.GetPersistentIdOfEvent(0);
    ASSERT_EQ(ret, INVALID_PERSISTENT_ID);

    ANRHandler::SessionInfo sessionInfo;
    anrHandler_.sessionStageMap_.emplace(1, sessionInfo);
    ret = anrHandler_.GetPersistentIdOfEvent(0);
    ASSERT_EQ(ret, INVALID_PERSISTENT_ID);

    sessionInfo.persistentId = 123;
    anrHandler_.sessionStageMap_[1] = sessionInfo;
    ret = anrHandler_.GetPersistentIdOfEvent(1);
    ASSERT_EQ(ret, 123);
}

/**
 * @tc.name: IsOnEventHandler
 * @tc.desc: test function : IsOnEventHandler
 * @tc.type: FUNC
 */
HWTEST_F(AnrHandlerTest, IsOnEventHandler, Function | SmallTest | Level1)
{
    anrHandler_.anrHandlerState_.sendStatus.clear();
    bool ret = anrHandler_.IsOnEventHandler(0);
    ASSERT_EQ(ret, false);

    anrHandler_.anrHandlerState_.sendStatus.emplace(0, true);
    ret = anrHandler_.IsOnEventHandler(0);
    ASSERT_EQ(ret, true);

    anrHandler_.anrHandlerState_.sendStatus.emplace(1, false);
    ret = anrHandler_.IsOnEventHandler(1);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: UpdateLatestEventId
 * @tc.desc: test function : UpdateLatestEventId
 * @tc.type: FUNC
 */
HWTEST_F(AnrHandlerTest, UpdateLatestEventId, Function | SmallTest | Level1)
{
    ANRHandler::SessionInfo sessionInfo;
    sessionInfo.persistentId = 0;
    anrHandler_.anrHandlerState_.eventsToReceipt.clear();
    anrHandler_.sessionStageMap_.clear();
    anrHandler_.UpdateLatestEventId(0);
    ASSERT_EQ(anrHandler_.anrHandlerState_.eventsToReceipt.size(), 0);

    anrHandler_.sessionStageMap_.emplace(0, sessionInfo);
    anrHandler_.anrHandlerState_.eventsToReceipt.push_back(0);
    anrHandler_.UpdateLatestEventId(0);

    anrHandler_.sessionStageMap_.clear();
    sessionInfo.persistentId = 1;
    anrHandler_.sessionStageMap_.emplace(2, sessionInfo);
    anrHandler_.anrHandlerState_.eventsToReceipt.push_back(1);
    anrHandler_.UpdateLatestEventId(2);

    anrHandler_.sessionStageMap_.clear();
    sessionInfo.persistentId = 101;
    anrHandler_.sessionStageMap_.emplace(2, sessionInfo);
    anrHandler_.anrHandlerState_.eventsToReceipt.push_back(1);
    anrHandler_.UpdateLatestEventId(2);
}
}
}
}