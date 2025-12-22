/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "screen_aod_plugin.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class IsInAodTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    static void* g_handle;
};

void* IsInAodTest::g_handle = nullptr;

void IsInAodTest::SetUp()
{
    g_handle = dlopen(nullptr, RTLD_LAZY);
}

void IsInAodTest::TearDown()
{
    if (g_handle) {
        dlclose(g_handle);
    }
}

namespace {

/**
 * @tc.name: NotifyTest01
 * @tc.desc: NotifyTest01
 * @tc.type: FUNC
 */
HWTEST_F(IsInAodTest, ATC_IsInAodAndStopAod01, TestSize.Level0)
{
    bool result = IsInAod();
    EXPECT_FALSE(result);
    result = StopAod(1);
    EXPECT_FALSE(result);
}
}
} // namespace Rosen
} // namespace OHOS