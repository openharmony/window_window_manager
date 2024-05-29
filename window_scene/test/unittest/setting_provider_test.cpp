/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <system_ability_definition.h>

#include "setting_provider.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SettingProviderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SettingProviderTest::SetUpTestCase()
{
}

void SettingProviderTest::TearDownTestCase()
{
}

void SettingProviderTest::SetUp()
{
}

void SettingProviderTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetIntValue
 * @tc.desc: test function : GetIntValue
 * @tc.type: FUNC
 */
HWTEST_F(SettingProviderTest, GetIntValue, Function | SmallTest | Level1)
{
    std::string key = "";
    int32_t value = 0;
    EXPECT_NE(SettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID).GetIntValue(key, value), ERR_OK);
}

/**
 * @tc.name: GetLongValue
 * @tc.desc: test function : GetLongValue
 * @tc.type: FUNC
 */
HWTEST_F(SettingProviderTest, GetLongValue, Function | SmallTest | Level1)
{
    std::string key = "";
    int64_t value = 0;
    EXPECT_NE(SettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID).GetLongValue(key, value), ERR_OK);
}

/**
 * @tc.name: GetBoolValue
 * @tc.desc: test function : GetBoolValue
 * @tc.type: FUNC
 */
HWTEST_F(SettingProviderTest, GetBoolValue, Function | SmallTest | Level1)
{
    std::string key = "";
    bool value = false;
    EXPECT_NE(SettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID).GetBoolValue(key, value), ERR_OK);
}

/**
 * @tc.name: ExecRegisterCb01
 * @tc.desc: test function : ExecRegisterCb
 * @tc.type: FUNC
 */
HWTEST_F(SettingProviderTest, ExecRegisterCb01, Function | SmallTest | Level1)
{
    sptr<SettingObserver> observer = nullptr;
    SettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID).ExecRegisterCb(observer);
    EXPECT_EQ(observer, nullptr);
}

/**
 * @tc.name: ExecRegisterCb02
 * @tc.desc: test function : ExecRegisterCb
 * @tc.type: FUNC
 */
HWTEST_F(SettingProviderTest, ExecRegisterCb02, Function | SmallTest | Level1)
{
    SettingObserver::UpdateFunc updateFunc = [&](const std::string&) {};
    auto observer = SettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID).CreateObserver(
        "settings.power.suspend_sources", updateFunc);
    SettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID).ExecRegisterCb(observer);
    EXPECT_NE(observer, nullptr);
}

/**
 * @tc.name: UnregisterObserver
 * @tc.desc: test function : UnregisterObserver
 * @tc.type: FUNC
 */
HWTEST_F(SettingProviderTest, UnregisterObserver, Function | SmallTest | Level1)
{
    SettingObserver::UpdateFunc updateFunc = [&](const std::string&) {};
    auto observer = SettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID).CreateObserver(
        "settings.power.suspend_sources", updateFunc);
    EXPECT_EQ(SettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID).UnregisterObserver(observer), ERR_OK);
}

/**
 * @tc.name: PutStringValue01
 * @tc.desc: test function : PutStringValue
 * @tc.type: FUNC
 */
HWTEST_F(SettingProviderTest, PutStringValue01, Function | SmallTest | Level1)
{
    SettingObserver::UpdateFunc updateFunc = [&](const std::string&) {};
    std::string key = "key";
    std::string value = "value";
    bool needNotify = true;
    auto observer = SettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID).CreateObserver(
        "settings.power.suspend_sources", updateFunc);
    EXPECT_EQ(SettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID).PutStringValue(key, value, needNotify), ERR_OK);
}

/**
 * @tc.name: PutStringValue02
 * @tc.desc: test function : PutStringValue
 * @tc.type: FUNC
 */
HWTEST_F(SettingProviderTest, PutStringValue02, Function | SmallTest | Level1)
{
    SettingObserver::UpdateFunc updateFunc = [&](const std::string&) {};
    std::string key = "key";
    std::string value = "value";
    bool needNotify = false;
    auto observer = SettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID).CreateObserver(
        "settings.power.suspend_sources", updateFunc);
    EXPECT_EQ(SettingProvider::GetInstance(POWER_MANAGER_SERVICE_ID).PutStringValue(key, value, needNotify), ERR_OK);
}
}
}
}