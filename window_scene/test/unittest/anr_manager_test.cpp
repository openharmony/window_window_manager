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

#include "intention_event/service/anr_manager/include/anr_manager.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr int32_t MAX_ANR_TIMER_COUNT { 64 };
class AnrManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

namespace {
/**
 * @tc.name: AddTimer
 * @tc.desc: AddTimer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AnrManagerTest, AddTimer, Function | SmallTest | Level1)
{
    int ret = 0;
    Rosen::ANRManager anrMgr;
    bool switcherTemp = anrMgr.switcher_;
    anrMgr.switcher_ = false;
    anrMgr.AddTimer(0, 1);

    anrMgr.switcher_ = true;
    anrMgr.anrTimerCount_ = MAX_ANR_TIMER_COUNT;
    anrMgr.AddTimer(0, 1);

    anrMgr.anrTimerCount_ = 0;
    anrMgr.AddTimer(0, 1);
    anrMgr.switcher_ = switcherTemp;
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: IsANRTriggered
 * @tc.desc: IsANRTriggered
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AnrManagerTest, IsANRTriggered, Function | SmallTest | Level1)
{
    Rosen::ANRManager anrMgr;
    bool ret = anrMgr.IsANRTriggered(0);
    ASSERT_EQ(ret, false);

    DelayedSingleton<EventStage>::GetInstance()->isAnrProcess_.emplace(123, true);
    ret = anrMgr.IsANRTriggered(123);
    ASSERT_EQ(ret, true);

    DelayedSingleton<EventStage>::GetInstance()->isAnrProcess_.emplace(156, false);
    ret = anrMgr.IsANRTriggered(156);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: RemoveTimers
 * @tc.desc: RemoveTimers
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AnrManagerTest, RemoveTimers, Function | SmallTest | Level1)
{
    int ret = 0;
    Rosen::ANRManager anrMgr;
    anrMgr.RemoveTimers(1);
    std::vector<EventStage::EventTime> events;
    DelayedSingleton<EventStage>::GetInstance()->events_.emplace(1, events);
    DelayedSingleton<EventStage>::GetInstance()->events_.emplace(2, events);
    anrMgr.RemoveTimers(2);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: GetAppInfoByPersistentId
 * @tc.desc: GetAppInfoByPersistentId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AnrManagerTest, GetAppInfoByPersistentId, Function | SmallTest | Level1)
{
    Rosen::ANRManager::AppInfo appInfo;
    Rosen::ANRManager anrMgr;
    auto ret = anrMgr.GetAppInfoByPersistentId(0);
    ASSERT_EQ(ret.pid, -1);

    appInfo.pid = 123;
    anrMgr.applicationMap_.emplace(0, appInfo);
    ret = anrMgr.GetAppInfoByPersistentId(0);
    ASSERT_EQ(ret.pid, 123);
}

/**
 * @tc.name: SwitchAnr
 * @tc.desc: SwitchAnr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AnrManagerTest, SwitchAnr, Function | SmallTest | Level1)
{
    Rosen::ANRManager anrMgr;
    anrMgr.SwitchAnr(true);
    ASSERT_EQ(anrMgr.switcher_, true);

    anrMgr.SwitchAnr(false);
    ASSERT_EQ(anrMgr.switcher_, false);
}

/**
 * @tc.name: GetBundleName
 * @tc.desc: GetBundleName
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AnrManagerTest, GetBundleName, Function | SmallTest | Level1)
{
    Rosen::ANRManager anrMgr;
    auto ret = anrMgr.GetBundleName(0, 0);
    ASSERT_EQ(ret, "unknown");

    anrMgr.appInfoGetter_ = [](int32_t pid, std::string& bundleName, int32_t uid) {
        bundleName = std::to_string(pid) + std::to_string(uid);
    };
    ret = anrMgr.GetBundleName(1, 2);
    ASSERT_EQ(ret, "12");
}

/**
 * @tc.name: ExecuteAnrObserver
 * @tc.desc: ExecuteAnrObserver
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(AnrManagerTest, ExecuteAnrObserver, Function | SmallTest | Level1)
{
    Rosen::ANRManager anrMgr;
    anrMgr.anrObserver_ = nullptr;
    anrMgr.ExecuteAnrObserver(1);
    ASSERT_EQ(anrMgr.anrObserver_, nullptr);
    int32_t ret = 0;
    anrMgr.anrObserver_ = [&ret](int32_t pid) {
        ret = pid;
    };
    anrMgr.ExecuteAnrObserver(123);
    ASSERT_EQ(ret, 123);
}
}
}
} // namespace OHOS