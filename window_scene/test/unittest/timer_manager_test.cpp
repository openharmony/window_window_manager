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
    int32_t res = timermanager->CalcNextDelayInternal();
    ASSERT_EQ(res, 5000);
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
    timermanager->ProcessTimersInternal();
    int32_t res = timermanager->RemoveTimerInternal(0);
    ASSERT_EQ(res, -1);
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
    auto res = anrManager->GetBundleName(0, 0);
    anrManager->GetBundleName(0, 1);
    anrManager->GetBundleName(1, 0);
    anrManager->GetBundleName(1, 1);
    ASSERT_EQ(res, "unknow");
    delete anrManager;
    GTEST_LOG_(INFO) << "ANRManager::GetBundleName end";
}
}
}
}
