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
    static void *g_handle = nullptr;
    using IsInAodFunc = bool (*)();
    using StopAodFunc = bool (*)(int32_t);
    IsInAodFunc g_isInAodFunc = nullptr;
    StopAodFunc g_stopAodFunc = nullptr;
class IsInAodTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
};

void IsInAodTest::SetUp() {}

void IsInAodTest::TearDown() {}

namespace {

/**
 * @tc.name: ATC_IsInAodAndStopAod01
 * @tc.desc: ATC_IsInAodAndStopAod01
 * @tc.type: FUNC
 */
HWTEST_F(IsInAodTest, ATC_IsInAodAndStopAod01, TestSize.Level0)
{
    g_handle = nullptr;
    bool result = IsInAod();
    EXPECT_FALSE(result);
    result = StopAod(1);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: ATC_IsInAodAndStopAod02
 * @tc.desc: ATC_IsInAodAndStopAod02
 * @tc.type: FUNC
 */
HWTEST_F(IsInAodTest, ATC_IsInAodAndStopAod02, TestSize.Level0)
{
    g_handle = dlopen(PLUGIN_AOD_SO_PATH.c_str(), RTLD_LAZY);
    g_isInAodFunc = []() {return true;};
    g_stopAodFunc = []() {return true;};
    bool result = IsInAod();
    EXPECT_TRUE(result);
    result = StopAod(1);
    EXPECT_TRUE(result);
    dlclose(g_handle);
    g_handle = nullptr;
    g_isInAodFunc = nullptr;
    g_stopAodFunc = nullptr;
}
}
} // namespace Rosen
} // namespace OHOS