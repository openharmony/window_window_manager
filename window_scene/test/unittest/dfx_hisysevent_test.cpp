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

#include "intention_event/dfx/include/dfx_hisysevent.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class DfxHisyseventTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DfxHisyseventTest::SetUpTestCase()
{
}

void DfxHisyseventTest::TearDownTestCase()
{
}

void DfxHisyseventTest::SetUp()
{
}

void DfxHisyseventTest::TearDown()
{
}

namespace {
/**
 * @tc.name: ApplicationBlockInput
 * @tc.desc: ApplicationBlockInput test Succ
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */

TEST_F(DfxHisyseventTest, ApplicationBlockInput_Success)
{
    DfxHisysevent dfxHisysevent;
    int32_t eventId = 1;
    int32_t pid = 2;
    std::string bundleName = "TestBundleName";
    int32_t persistentId = 4;
    int result = 0;
    std::function<void()> func = [&]() {
        dfxHisysevent.ApplicationBlockInput(eventId, pid, bundleName, persistentId);
        result = 1;
    };
    func();
    ASSERT_EQ(result, 1);
}

/**
 * @tc.name: ApplicationBlockInput
 * @tc.desc: ApplicationBlockInput test Fail1
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */

TEST_F(DfxHisyseventTest, ApplicationBlockInput_Fail1)
{
    DfxHisysevent dfxHisysevent;
    int32_t eventId = 1;
    int32_t pid = -5;
    std::string bundleName = "TestBundleName";
    int32_t persistentId = 2;
    int result = 0;
    std::function<void()> func = [&]() {
        dfxHisysevent.ApplicationBlockInput(eventId, pid, bundleName, persistentId);
        result = 1;
    };
    func();
    ASSERT_EQ(result, 1);
}

/**
 * @tc.name: ApplicationBlockInput
 * @tc.desc: ApplicationBlockInput test Fail2
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */

TEST_F(DfxHisyseventTest, ApplicationBlockInput_Fail2)
{
    DfxHisysevent dfxHisysevent;
    int32_t eventId = 1;
    int32_t pid = 5;
    std::string bundleName = "TestBundleName";
    int32_t persistentId = -2;
    int result = 0;
    std::function<void()> func = [&]() {
        dfxHisysevent.ApplicationBlockInput(eventId, pid, bundleName, persistentId);
        result = 1;
    };
    func();
    ASSERT_EQ(result, 1);
}

/**
 * @tc.name: ApplicationBlockInput
 * @tc.desc: ApplicationBlockInput test Fail3
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */

TEST_F(DfxHisyseventTest, ApplicationBlockInput_Fail3)
{
    DfxHisysevent dfxHisysevent;
    int32_t eventId = 1;
    int32_t pid = -5;
    std::string bundleName = "TestBundleName";
    int32_t persistentId = -2;
    int result = 0;
    std::function<void()> func = [&]() {
        dfxHisysevent.ApplicationBlockInput(eventId, pid, bundleName, persistentId);
        result = 1;
    };
    func();
    ASSERT_EQ(result, 1);
}

} // namespace
} // namespace OHOS
} // namespace Rosen
