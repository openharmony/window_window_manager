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
#include <fstream>
#include <string>
#include "interfaces/innerkits/include/scene_board_judgement.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class SceneBoardJudgementTest : public testing::Test {
protected:
    SceneBoardJudgement sceneBoardJudgement;
    std::ifstream configFile;
    std::string line;
};

HWTEST_F(SceneBoardJudgementTest, SafeGetLine_WhenLineEndsWithCR, Function | SmallTest | Level1)
{
    std::istringstream input("Test\r");
    std::string line;
    std::ifstream configFile;
    configFile.basic_ios<char>::rdbuf(input.rdbuf());

    SceneBoardJudgement sceneBoardJudgement;
    sceneBoardJudgement.SafeGetLine(configFile, line);
    EXPECT_EQ(line, "Test");
}

HWTEST_F(SceneBoardJudgementTest, SafeGetLine_WhenLineDoesNotEndWithCR, Function | SmallTest | Level1)
{
    std::istringstream input("Test");
    std::string line;
    std::ifstream configFile;
    configFile.basic_ios<char>::rdbuf(input.rdbuf());

    SceneBoardJudgement sceneBoardJudgement;
    sceneBoardJudgement.SafeGetLine(configFile, line);
    EXPECT_EQ(line, "Test");
}

HWTEST_F(SceneBoardJudgementTest, SafeGetLine_WhenLineIsEmpty, Function | SmallTest | Level1)
{
    std::istringstream input("");
    std::string line;
    std::ifstream configFile;
    configFile.basic_ios<char>::rdbuf(input.rdbuf());

    SceneBoardJudgement sceneBoardJudgement;
    sceneBoardJudgement.SafeGetLine(configFile, line);
    EXPECT_EQ(line, "");
}

} // Rosen
} // OHOS