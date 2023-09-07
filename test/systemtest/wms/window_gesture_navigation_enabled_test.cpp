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

// gtest
#include <gtest/gtest.h>

#include "future.h"
#include "window_manager.h"
#include "wm_common.h"
#include "scene_board_judgement.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr int WAIT_FUTURE_RESULT = 20000; // 20s
    constexpr int WAIT_SLEEP_TIME = 1; // 1s
}

class TestGestureNavigationEnabledChangedListener : public IGestureNavigationEnabledChangedListener {
public:
    void OnGestureNavigationEnabledUpdate(bool enable) override;

    RunnableFuture<bool> future_;
};

void TestGestureNavigationEnabledChangedListener::OnGestureNavigationEnabledUpdate(bool enable)
{
    future_.SetValue(enable);
}

class GestureNavigationEnabledTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<TestGestureNavigationEnabledChangedListener> lisenter_;
};

sptr<TestGestureNavigationEnabledChangedListener> GestureNavigationEnabledTest::lisenter_ = nullptr;
void GestureNavigationEnabledTest::SetUpTestCase()
{
    lisenter_= new (std::nothrow)TestGestureNavigationEnabledChangedListener();
    if (lisenter_ == nullptr) {
        return;
    }
}

void GestureNavigationEnabledTest::TearDownTestCase()
{
}

void GestureNavigationEnabledTest::SetUp()
{
}

void GestureNavigationEnabledTest::TearDown()
{
}

namespace {
/**
* @tc.name: SetGestureNavigationEnabled
* @tc.desc: Check gesture navigation enabled
* @tc.type: FUNC
*/
HWTEST_F(GestureNavigationEnabledTest, SetGestureNavigationEnabled, Function | MediumTest | Level1)
{
    ASSERT_NE(lisenter_, nullptr);

    auto& windowManager =  WindowManager::GetInstance();
    windowManager.SetGestureNavigaionEnabled(false);
    sleep(WAIT_SLEEP_TIME);

    windowManager.RegisterGestureNavigationEnabledChangedListener(lisenter_);
    sleep(WAIT_SLEEP_TIME);
    windowManager.SetGestureNavigaionEnabled(true);
    auto result = lisenter_->future_.GetResult(WAIT_FUTURE_RESULT);
   
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(result, true);
    } else {
        ASSERT_EQ(result, false);
    }
    lisenter_->future_.Reset(true);
    windowManager.SetGestureNavigaionEnabled(false);
    result = lisenter_->future_.GetResult(WAIT_FUTURE_RESULT);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(result, false);
    } else {
        ASSERT_EQ(result, true);
    }
    lisenter_->future_.Reset(false);
    windowManager.UnregisterGestureNavigationEnabledChangedListener(lisenter_);
    sleep(WAIT_SLEEP_TIME);
}
}
} // namespace Rosen
} // namespace OHOS

