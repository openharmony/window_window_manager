/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "permission.h"

#include <accesstoken_kit.h>
#include <bundle_constants.h>
#include <ipc_skeleton.h>
#include <bundle_mgr_proxy.h>
#include <bundle_mgr_interface.h>
#include <system_ability_definition.h>
#include <iservice_registry.h>
#include <tokenid_kit.h>

#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class PermissionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PermissionTest::SetUpTestCase()
{
}

void PermissionTest::TearDownTestCase()
{
}

void PermissionTest::SetUp()
{
}

void PermissionTest::TearDown()
{
}

namespace {
/**
 * @tc.name: IsSystemServiceCalling
 * @tc.desc: test fuction : IsSystemServiceCalling
 * @tc.type: FUNC
 */
HWTEST_F(PermissionTest, IsSystemServiceCalling, Function | SmallTest | Level2)
{
    bool result = Permission::IsSystemServiceCalling(true, true);
    ASSERT_EQ(true, result);
}
/**
 * @tc.name: IsSystemCallingOrStartByHdcd
 * @tc.desc: test fuction : IsSystemCallingOrStartByHdcd
 * @tc.type: FUNC
 */
HWTEST_F(PermissionTest, IsSystemCallingOrStartByHdcd, Function | SmallTest | Level2)
{
    bool result1 = Permission::IsSystemCallingOrStartByHdcd(false);
    ASSERT_EQ(true, result1);
    bool result2 = Permission::IsSystemCallingOrStartByHdcd(true);
    ASSERT_EQ(true, result2);
}
/**
 * @tc.name: IsStartByInputMethod
 * @tc.desc: test fuction : IsStartByInputMethod
 * @tc.type: FUNC
 */
HWTEST_F(PermissionTest, IsStartByInputMethod, Function | SmallTest | Level2)
{
    bool result = Permission::IsStartByInputMethod();
    ASSERT_EQ(false, result);
}
/**
 * @tc.name: CheckIsCallingBundleName
 * @tc.desc: test fuction : CheckIsCallingBundleName
 * @tc.type: FUNC
 */
HWTEST_F(PermissionTest, CheckIsCallingBundleName, Function | SmallTest | Level2)
{
    bool result = Permission::CheckIsCallingBundleName("err");
    ASSERT_EQ(false, result);
}
}
}
}