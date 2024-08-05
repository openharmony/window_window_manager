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

#include <gtest/gtest.h>
#include "intention_event/service/timer_manager/include/timer_manager.h"
#include "intention_event/service/anr_manager/include/anr_manager.h"
#include "intention_event/framework/anr_handler/include/anr_handler.h"
#include <algorithm>
#include <cinttypes>
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
constexpr int32_t MAX_INTERVAL_MS = 10000;
constexpr int32_t MIN_DELAY = 5000;
class TimerManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void TimerManagerTest::SetUpTestCase()
{
}

void TimerManagerTest::TearDownTestCase()
{
}

void TimerManagerTest::SetUp()
{
}

void TimerManagerTest::TearDown()
{
}

int64_t GetMillisTime()
{
    auto timeNow = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow.time_since_epoch());
    return tmp.count();
}

namespace {
/**
 * @tc.name: Init
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, Init, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "TimerManagerTest::Init start";
    TimerManager* timermanager = new TimerManager();
    ASSERT_NE(timermanager, nullptr);
    timermanager->Init();
    timermanager->Init();
    int32_t res = timermanager->RemoveTimer(0);
    ASSERT_EQ(-1, res);
    delete timermanager;
    GTEST_LOG_(INFO) << "TimerManagerTest::Init end";
}

/**
 * @tc.name: RemoveTimer
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, RemoveTimer, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "TimerManagerTest::RemoveTimer start";
    TimerManager* timermanager = new TimerManager();
    ASSERT_NE(timermanager, nullptr);
    int32_t res = timermanager->RemoveTimer(0);
    ASSERT_EQ(-1, res);
    delete timermanager;
    GTEST_LOG_(INFO) << "TimerManagerTest::RemoveTimer end";
}

/**
 * @tc.name: OnThread
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, OnThread, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "TimerManagerTest::OnThread start";
    TimerManager* timermanager = new TimerManager();
    ASSERT_NE(timermanager, nullptr);
    timermanager->OnThread();
    ASSERT_EQ(-1, timermanager->RemoveTimerInternal(0));
    delete timermanager;
    GTEST_LOG_(INFO) << "TimerManagerTest::OnThread start";
}

/**
 * @tc.name: TakeNextTimerId
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, TakeNextTimerId, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "TimerManagerTest::TakeNextTimerId start";
    TimerManager* timermanager = new TimerManager();
    ASSERT_NE(timermanager, nullptr);
    int32_t res = timermanager->TakeNextTimerId();
    ASSERT_EQ(res, 0);
    delete timermanager;
    GTEST_LOG_(INFO) << "TimerManagerTest::TakeNextTimerId start";
}

/**
 * @tc.name: RemoveTimerInternal
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, RemoveTimerInternal, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "TimerManagerTest::RemoveTimerInternal start";
    TimerManager* timermanager = new TimerManager();
    ASSERT_NE(timermanager, nullptr);
    int32_t res = timermanager->RemoveTimerInternal(0);
    ASSERT_EQ(res, -1);
    delete timermanager;
    GTEST_LOG_(INFO) << "TimerManagerTest::RemoveTimerInternal start";
}

/**
 * @tc.name: CalcNextDelayInternal
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, CalcNextDelayInternal, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "TimerManagerTest::CalcNextDelayInternal start";
    TimerManager* timermanager = new TimerManager();
    ASSERT_NE(timermanager, nullptr);
    int32_t res = timermanager->CalcNextDelayInternal();
    ASSERT_EQ(res, MIN_DELAY);

    std::unique_ptr<TimerManager::TimerItem> timer = std::make_unique<TimerManager::TimerItem>();
    ASSERT_NE(timer, nullptr);
    timermanager->timers_.push_back(std::move(timer));
    res = timermanager->CalcNextDelayInternal();
    ASSERT_EQ(res, 0);

    auto nowTime = GetMillisTime();
    if (!timermanager->timers_.empty()) {
        auto& firstTimer = timermanager->timers_.front();
        firstTimer->nextCallTime = nowTime + 10000;
    }
    res = timermanager->CalcNextDelayInternal();
    ASSERT_EQ(res, 10000);

    delete timermanager;
    GTEST_LOG_(INFO) << "TimerManagerTest::CalcNextDelayInternal start";
}

/**
 * @tc.name: ProcessTimersInternal
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, ProcessTimersInternal, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "TimerManagerTest::ProcessTimersInternal start";
    TimerManager* timermanager = new TimerManager();
    ASSERT_NE(timermanager, nullptr);
    timermanager->ProcessTimersInternal();
    ASSERT_EQ(timermanager->timers_.size(), 0);

    std::unique_ptr<TimerManager::TimerItem> timer = std::make_unique<TimerManager::TimerItem>();
    ASSERT_NE(timer, nullptr);
    timer->nextCallTime = GetMillisTime() + 10000;
    timermanager->timers_.push_back(std::move(timer));
    timermanager->ProcessTimersInternal();

    if (!timermanager->timers_.empty()) {
        auto& firstTimer = timermanager->timers_.front();
        ASSERT_NE(firstTimer, nullptr);
        firstTimer->nextCallTime = 0;
        ASSERT_EQ(firstTimer->callback, nullptr);
        timermanager->ProcessTimersInternal();
    }

    delete timermanager;
    GTEST_LOG_(INFO) << "TimerManagerTest::ProcessTimersInternal start";
}

/**
 * @tc.name: ANRManagerInit
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, ANRManagerInit, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ANRManager::Init start";
    ANRManager* anrManager = new ANRManager();
    ASSERT_NE(anrManager, nullptr);
    anrManager->Init();
    ASSERT_EQ(anrManager->switcher_, false);
    delete anrManager;
    GTEST_LOG_(INFO) << "ANRManager::Init end";
}

/**
 * @tc.name: ANRManagerAddTimer
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, ANRManagerAddTimer, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ANRManager::AddTimer start";
    ANRManager* anrManager = new ANRManager();
    ASSERT_NE(anrManager, nullptr);
    anrManager->AddTimer(0, 1);
    anrManager->AddTimer(1, 1);
    anrManager->AddTimer(1, 0);
    ASSERT_EQ(anrManager->switcher_, true);
    delete anrManager;
    GTEST_LOG_(INFO) << "ANRManager::AddTimer end";
}

/**
 * @tc.name: ANRManagerMarkProcessed
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, ANRManagerMarkProcessed, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ANRManager::MarkProcessed start";
    ANRManager* anrManager = new ANRManager();
    ASSERT_NE(anrManager, nullptr);
    anrManager->MarkProcessed(0, 0);
    anrManager->MarkProcessed(0, 1);
    anrManager->MarkProcessed(1, 1);
    anrManager->MarkProcessed(1, 0);
    ASSERT_NE(anrManager->anrTimerCount_, 0);
    delete anrManager;
    GTEST_LOG_(INFO) << "ANRManager::MarkProcessed end";
}

/**
 * @tc.name: ANRManagerRemoveTimers
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, ANRManagerRemoveTimers, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ANRManager::RemoveTimers start";
    ANRManager* anrManager = new ANRManager();
    ASSERT_NE(anrManager, nullptr);
    anrManager->RemoveTimers(0);
    ASSERT_EQ(anrManager->anrTimerCount_, 0);
    delete anrManager;
    GTEST_LOG_(INFO) << "ANRManager::RemoveTimers end";
}

/**
 * @tc.name: ANRManagerSwitchAnr
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, ANRManagerSwitchAnr, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ANRManager::SwitchAnr start";
    ANRManager* anrManager = new ANRManager();
    ASSERT_NE(anrManager, nullptr);
    anrManager->RemoveTimers(true);
    ASSERT_EQ(anrManager->switcher_, true);
    delete anrManager;
    GTEST_LOG_(INFO) << "ANRManager::SwitchAnr end";
}

/**
 * @tc.name: ANRManagerSetAppInfoGetter
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, ANRManagerSetAppInfoGetter, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ANRManager::SetAppInfoGetter start";
    ANRManager* anrManager = new ANRManager();
    ASSERT_NE(anrManager, nullptr);
    std::function<void(int32_t, std::string&, int32_t)> callback;
    anrManager->SetAppInfoGetter(callback);
    ASSERT_EQ(anrManager->appInfoGetter_, nullptr);
    delete anrManager;
    GTEST_LOG_(INFO) << "ANRManager::SetAppInfoGetter end";
}

/**
 * @tc.name: ANRManagerGetBundleName
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, ANRManagerGetBundleName, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ANRManager::GetBundleName start";
    ANRManager* anrManager = new ANRManager();
    ASSERT_NE(anrManager, nullptr);
    auto res = anrManager->GetBundleName(0, 0);
    anrManager->GetBundleName(0, 1);
    anrManager->GetBundleName(1, 0);
    anrManager->GetBundleName(1, 1);
    ASSERT_EQ(res, "unknow");
    delete anrManager;
    GTEST_LOG_(INFO) << "ANRManager::GetBundleName end";
}

/**
 * @tc.name: AddTimer001
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, AddTimer001, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "AddTimer001::ProcessTimersInternal start";
    TimerManager* timermanager = new TimerManager();
    ASSERT_NE(timermanager, nullptr);
    int32_t intervalMs = 1;
    std::function<void()> callback;
    int32_t res = timermanager->AddTimer(intervalMs, callback);
    ASSERT_EQ(res, -1);
    timermanager->state_ = TimerMgrState::STATE_RUNNING;
    res = timermanager->AddTimer(intervalMs, callback);
    ASSERT_EQ(res, timermanager->AddTimerInternal(intervalMs, callback));
    delete(timermanager);
    GTEST_LOG_(INFO) << "AddTimer001::ProcessTimersInternal start";
}

/**
 * @tc.name: ProcessTimers002
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, ProcessTimers002, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ProcessTimers002::ProcessTimersInternal start";
    TimerManager* timermanager = new TimerManager();
    ASSERT_NE(timermanager, nullptr);
    int res = 0;
    std::function<void()> func = [&]() {
        timermanager->ProcessTimers();
        res = 1;
    };
    func();
    ASSERT_EQ(res, 1);
    delete(timermanager);
    GTEST_LOG_(INFO) << "ProcessTimers002::ProcessTimersInternal start";
}

/**
 * @tc.name: OnStop003
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, OnStop003, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "OnStop003::ProcessTimersInternal start";
    TimerManager* timermanager = new TimerManager();
    ASSERT_NE(timermanager, nullptr);
    int res = 0;
    std::function<void()> func = [&]() {
        timermanager->OnStop();
        res = 1;
    };
    func();
    ASSERT_EQ(res, 1);
    delete(timermanager);
    GTEST_LOG_(INFO) << "OnStop003::ProcessTimersInternal start";
}

/**
 * @tc.name: AddTimerInternal004
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, AddTimerInternal004, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "AddTimerInternal004::ProcessTimersInternal start";
    TimerManager* timermanager = new TimerManager();
    ASSERT_NE(timermanager, nullptr);
    int32_t intervalMs = 1;
    std::function<void()> callback;
    int32_t res = timermanager->AddTimerInternal(intervalMs, callback);
    ASSERT_EQ(res, -1);

    res = timermanager->AddTimerInternal(MAX_INTERVAL_MS + 1, callback);
    ASSERT_EQ(res, -1);

    delete(timermanager);
    GTEST_LOG_(INFO) << "AddTimerInternal004::ProcessTimersInternal start";
}

/**
 * @tc.name: ANRHandler.HandleEventConsumed
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, HandleEventConsumed01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ANRHandler::HandleEventConsumed01 start";
    int32_t evenid = -1;
    int64_t actionTime = 10;
    ANRHandler* ANRHandler = new (class ANRHandler)();
    ANRHandler->HandleEventConsumed(evenid, actionTime);
    ASSERT_EQ(evenid, -1);
    delete ANRHandler;
    GTEST_LOG_(INFO) << "ANRHandler::HandleEventConsumed01 end";
}

/**
 * @tc.name: ANRHandler.HandleEventConsumed
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, HandleEventConsumed02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ANRHandler::HandleEventConsumed02 start";
    int32_t evenid = 0;
    int64_t actionTime = 10;
    ANRHandler* ANRHandler = new (class ANRHandler)();
    ANRHandler->HandleEventConsumed(evenid, actionTime);
    ASSERT_EQ(evenid, 0);
    delete ANRHandler;
    GTEST_LOG_(INFO) << "ANRHandler::HandleEventConsumed02 end";
}

/**
 * @tc.name: ANRHandler.HandleEventConsumed
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, HandleEventConsumed03, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ANRHandler::HandleEventConsumed03 start";
    int32_t evenid = 1;
    int64_t actionTime = 10;
    ANRHandler* ANRHandler = new (class ANRHandler)();
    ANRHandler->HandleEventConsumed(evenid, actionTime);
    ASSERT_EQ(evenid, 1);
    delete ANRHandler;
    GTEST_LOG_(INFO) << "ANRHandler::HandleEventConsumed03 end";
}

/**
 * @tc.name: ANRHandler.OnWindowDestroyed
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, OnWindowDestroyed01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ANRHandler::OnWindowDestroyed01 start";
    int32_t persistentId = 1;
    ANRHandler* ANRHandler = new (class ANRHandler)();
    ANRHandler->OnWindowDestroyed(persistentId);
    ASSERT_EQ(persistentId, 1);
    delete ANRHandler;
    GTEST_LOG_(INFO) << "ANRHandler::OnWindowDestroyed01 end";
}

/**
 * @tc.name: ANRHandler.OnWindowDestroyed
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, OnWindowDestroyed02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ANRHandler::OnWindowDestroyed02 start";
    int32_t persistentId = -1;
    ANRHandler* ANRHandler = new (class ANRHandler)();
    ANRHandler->OnWindowDestroyed(persistentId);
    ASSERT_EQ(persistentId, -1);
    delete ANRHandler;
    GTEST_LOG_(INFO) << "ANRHandler::OnWindowDestroyed02 end";
}

/**
 * @tc.name: ANRHandler.UpdateLatestEventId
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, UpdateLatestEventId, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ANRHandler::UpdateLatestEventId start";
    int32_t eventId = -1;
    ANRHandler* ANRHandler = new (class ANRHandler)();
    ANRHandler->UpdateLatestEventId(eventId);
    ASSERT_EQ(eventId, -1);
    delete ANRHandler;
    GTEST_LOG_(INFO) << "ANRHandler::UpdateLatestEventId end";
}


/**
 * @tc.name: CalcNextDelay
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, CalcNextDelay, Function | SmallTest | Level2)
{
    TimerManager* timermanager = new TimerManager();
    ASSERT_NE(timermanager, nullptr);
    timermanager->state_ = TimerMgrState::STATE_NOT_START;
    int32_t res = timermanager->CalcNextDelay();
    ASSERT_EQ(res, -1);

    timermanager->state_ = TimerMgrState::STATE_RUNNING;
    res = timermanager->CalcNextDelay();
    auto res2 = timermanager->CalcNextDelayInternal();
    ASSERT_EQ(res, res2);
    delete timermanager;
}

/**
 * @tc.name: ProcessTimers
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TimerManagerTest, ProcessTimers, Function | SmallTest | Level2)
{
    int res = 0;
    TimerManager* timermanager = new TimerManager();
    ASSERT_NE(timermanager, nullptr);
    timermanager->state_ = TimerMgrState::STATE_NOT_START;
    timermanager->ProcessTimers();
    ASSERT_EQ(res, 0);
    delete timermanager;
}
}
}
}
