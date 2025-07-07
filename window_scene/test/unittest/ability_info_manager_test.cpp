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

#include <bundle_mgr_interface.h>
#include <bundlemgr/launcher_service.h>
#include "session/host/include/ability_info_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class AbilityInfoManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void AbilityInfoManagerTest::SetUpTestCase()
{
}

void AbilityInfoManagerTest::TearDownTestCase()
{
}

void AbilityInfoManagerTest::SetUp()
{
}

void AbilityInfoManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: FindAbilityInfo01
 * @tc.desc: FindAbilityInfo01
 * @tc.type: FUNC
 */
HWTEST_F(AbilityInfoManagerTest, FindAbilityInfo01, TestSize.Level1)
{
    std::string moduleName = "testModuleName";
    std::string abilityName = "testAbilityName";
    AppExecFwk::BundleInfo bundleInfo;
    AppExecFwk::AbilityInfo targetAbilityInfo;
    auto ret = AbilityInfoManager::FindAbilityInfo(bundleInfo, moduleName, abilityName, targetAbilityInfo);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: FindAbilityInfo02
 * @tc.desc: FindAbilityInfo02
 * @tc.type: FUNC
 */
HWTEST_F(AbilityInfoManagerTest, FindAbilityInfo02, TestSize.Level1)
{
    std::string moduleName = "testModuleName";
    std::string abilityName = "testAbilityName";
    AppExecFwk::BundleInfo bundleInfo;
    AppExecFwk::AbilityInfo targetAbilityInfo;
    AppExecFwk::AbilityInfo abilityInfo;
    abilityInfo.moduleName = moduleName;
    abilityInfo.name = abilityName;
    AppExecFwk::HapModuleInfo hapModuleInfo;
    hapModuleInfo.abilityInfos.emplace_back(abilityInfo);
    bundleInfo.hapModuleInfos.emplace_back(hapModuleInfo);
    auto ret = AbilityInfoManager::FindAbilityInfo(bundleInfo, moduleName, abilityName, targetAbilityInfo);
    EXPECT_TRUE(ret);
    EXPECT_EQ(targetAbilityInfo.moduleName, moduleName);
    EXPECT_EQ(targetAbilityInfo.name, abilityName);
}
} // namespace
} // namespace Rosen
} // namespace OHOS