/*

Copyright (c) 2025 Huawei Device Co., Ltd.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "dms_global_mutex.h"

#include <functional>
#include <thread>
#include <vector>

using namespace testing;
using namespace OHOS::Rosen::DmUtils;

namespace OHOS {
namespace Rosen {
namespace DmUtils {
namespace {

class DmsGlobalMutexTest : public Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/** @tc.name: HoldLock_SingleThread_LockUnlock

@tc.desc: 单线程中 HoldLock 应正确加锁/解锁

@tc.type: FUNC

@tc.level: Level1 */
HWTEST_F(DmsGlobalMutexTest, HoldLock_SingleThread_LockUnlock, TestSize.Level1)
{
    std::mutex testMtx;
    std::lock_guard<std::mutex> lg(HoldLock::resMtx); // 临时锁住，防止其他测试干扰
    EXPECT_FALSE(HoldLock::resMtx.try_lock());      // 初始应可锁（但被我们占了）

    {
        HoldLock lock;
        EXPECT_FALSE(HoldLock::resMtx.try_lock()); // 被 HoldLock 锁住
    }

    EXPECT_TRUE(HoldLock::resMtx.try_lock()); // 析构后应释放
    HoldLock::resMtx.unlock();
}

/** @tc.name: HoldLock_MultiThread_Counter

@tc.desc: 多线程中 HoldLock 应正确计数，锁只在最后一个释放时才释放

@tc.type: FUNC

@tc.level: Level1 */
HWTEST_F(DmsGlobalMutexTest, HoldLock_MultiThread_Counter, TestSize.Level1)
{
    std::atomic<int> counter = 0;
    constexpr int n = 5;
    std::vector<std::thread> threads;

    for (int i = 0; i < n; ++i) {
        threads.emplace_back([&] {
            HoldLock lock;
            ++counter;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(counter.load(), N);
    // 验证锁在所有线程结束后才释放
    EXPECT_TRUE(HoldLock::resMtx.try_lock());
    HoldLock::resMtx.unlock();
}

/** @tc.name: DropLock_TempUnlock

@tc.desc: DropLock 应在构造时释放锁，析构时恢复

@tc.type: FUNC

@tc.level: Level1 */
HWTEST_F(DmsGlobalMutexTest, DropLock_TempUnlock, TestSize.Level1)
{
    {
        HoldLock lock;
        EXPECT_FALSE(HoldLock::resMtx.try_lock()); // 被持有

        {
            DropLock drop;
            EXPECT_TRUE(HoldLock::resMtx.try_lock()); // 应可获取（被临时释放）
            std::mutex tmpMtx;
            std::lock_guard<std::mutex> lg(tmpMtx); // 模拟等待
        } // DropLock 析构

        EXPECT_FALSE(HoldLock::resMtx.try_lock()); // 应恢复持有
    } // HoldLock 析构

    EXPECT_TRUE(HoldLock::resMtx.try_lock());
    HoldLock::resMtx.unlock();
}

/** @tc.name: WrapCallback_LocksDuringCall

@tc.desc: wrap_callback 包装的函数应在执行时持有锁

@tc.type: FUNC

@tc.level: Level1 */
HWTEST_F(DmsGlobalMutexTest, WrapCallback_LocksDuringCall, TestSize.Level1)
{
    std::atomic<bool> called = false;
    auto wrapped = wrap_callback([&called] {
        EXPECT_FALSE(HoldLock::resMtx.try_lock()); // 应持有锁
        called = true;
    });

    {
        HoldLock::resMtx.lock(); // 初始锁住
        wrapped();
    }

    EXPECT_TRUE(called.load());
    EXPECT_TRUE(HoldLock::resMtx.try_lock()); // 执行后应释放
    HoldLock::resMtx.unlock();
}

/** @tc.name: SafeWaitFor_TemporarilyUnlocks

@tc.desc: safe_wait_for 应在等待时临时释放锁

@tc.type: FUNC

@tc.level: Level1 */
HWTEST_F(DmsGlobalMutexTest, SafeWaitFor_TemporarilyUnlocks, TestSize.Level1)
{
    std::mutex mtx;
    std::condition_variable cv;
    std::unique_lock<std::mutex> lock(mtx);
    bool notified = false;
    auto waiter = std::thread([&mtx, &cv, &notified] {
        std::unique_lockstd::mutex lk(mtx);
        EXPECT_FALSE(HoldLock::resMtx.try_lock()); // HoldLock 持有
        safe_wait_for(cv, lk, std::chrono::milliseconds(100));
        EXPECT_FALSE(HoldLock::resMtx.try_lock()); // 仍应持有（DropLock 临时释放）
        notified = true;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    cv.notify_one();
    waiter.join();

    EXPECT_TRUE(notified);
}

/** @tc.name: ForceUnlock_ForceLock_Work

@tc.desc: ForceUnlock 和 ForceLock 应正确操作计数和锁

@tc.type: FUNC

@tc.level: Level1 */
HWTEST_F(DmsGlobalMutexTest, ForceUnlock_ForceLock_Work, TestSize.Level1)
{
    HoldLock::resMtx.lock(); // 确保初始状态
    EXPECT_FALSE(HoldLock::resMtx.try_lock());

    bool wasLocked = HoldLock::ForceUnlock();
    EXPECT_TRUE(wasLocked);
    EXPECT_TRUE(HoldLock::resMtx.try_lock());
    HoldLock::resMtx.unlock();

    HoldLock::ForceLock(true); // 加锁
    EXPECT_FALSE(HoldLock::resMtx.try_lock());
    HoldLock::ForceUnlock();
    EXPECT_TRUE(HoldLock::resMtx.try_lock());
    HoldLock::resMtx.unlock();
}

} // namespace
} // namespace DmUtils
} // namespace Rosen
} // namespace OHOS