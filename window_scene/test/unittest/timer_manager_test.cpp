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
#include <algorithm>
#include <cinttypes>
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
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
    timermanager->Init();
    timermanager->Init();
    int32_t res = timermanager->RemoveTimer(0);
    ASSERT_EQ(-1, res);
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
    int32_t res = timermanager->RemoveTimer(0);
    ASSERT_EQ(-1, res);
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
    timermanager->OnThread();
    ASSERT_EQ(-1, timermanager->RemoveTimerInternal(0));
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
    int32_t res = timermanager->TakeNextTimerId();
    ASSERT_EQ(res, 0);
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
    int32_t res = timermanager->RemoveTimerInternal(0);
    ASSERT_EQ(res, -1);
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
    int32_t res = timermanager->CalcNextDelayInternal();
    ASSERT_EQ(res, 5000);
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
    timermanager->ProcessTimersInternal();
    int32_t res = timermanager->RemoveTimerInternal(0);
    ASSERT_EQ(res, -1);
    GTEST_LOG_(INFO) << "TimerManagerTest::ProcessTimersInternal start";
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
    int32_t intervalMs = 1;
    std::function<void()> callback;
    int32_t res = timermanager->AddTimerInternal(intervalMs, callback);
    ASSERT_EQ(res, -1);
    delete(timermanager);
    GTEST_LOG_(INFO) << "AddTimerInternal004::ProcessTimersInternal start";
}
}
}
}
