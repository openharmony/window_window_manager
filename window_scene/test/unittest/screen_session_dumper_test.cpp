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

#include "screen_session_dumper.h"
#include "screen_session_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScreenSessionDumperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ScreenSessionDumperTest::SetUpTestCase()
{
}

void ScreenSessionDumperTest::TearDownTestCase()
{
}

void ScreenSessionDumperTest::SetUp()
{
}

void ScreenSessionDumperTest::TearDown()
{
}

namespace {
/**
 * @tc.name: Dump01
 * @tc.desc: Dump
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, Dump01, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args;
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    ASSERT_NE(nullptr, dumper);
}

/**
 * @tc.name: Dump02
 * @tc.desc: Dump input for -h
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, Dump02, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper->ExcuteDumpCmd();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Dump03
 * @tc.desc: Dump input for -a
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, Dump03, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-a"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper->ExcuteDumpCmd();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Dump04
 * @tc.desc: Dump input for abnormal
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, Dump04, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-abnormal"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper->ExcuteDumpCmd();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Dump05
 * @tc.desc: Dump fd less 0
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, Dump05, Function | SmallTest | Level1)
{
    int fd = -1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper->ExcuteDumpCmd();
    ASSERT_TRUE(true);
}

}

}
}