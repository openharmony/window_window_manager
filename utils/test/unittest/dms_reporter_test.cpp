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
#include "dms_reporter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DmsReporterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DmsReporterTest::SetUpTestCase()
{
}

void DmsReporterTest::TearDownTestCase()
{
}

void DmsReporterTest::SetUp()
{
}

void DmsReporterTest::TearDown()
{
}

namespace {
/**
 * @tc.name: ReportRegisterSessionListener
 * @tc.desc: ReportRegisterSessionListener test
 * @tc.type: FUNC
 */
HWTEST_F(DmsReporterTest, ReportRegisterSessionListener, Function | SmallTest | Level2)
{
    int res = 0;
    DmsReporter dmsReporter;
    dmsReporter.ReportRegisterSessionListener(true, 0);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: ReportQuerySessionInfo
 * @tc.desc: ReportQuerySessionInfo test
 * @tc.type: FUNC
 */
HWTEST_F(DmsReporterTest, ReportQuerySessionInfo, Function | SmallTest | Level2)
{
    int res = 0;
    DmsReporter dmsReporter;
    dmsReporter.ReportQuerySessionInfo(true, 0);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: ReportContinueApp
 * @tc.desc: ReportContinueApp test
 * @tc.type: FUNC
 */
HWTEST_F(DmsReporterTest, ReportContinueApp, Function | SmallTest | Level2)
{
    int res = 0;
    DmsReporter dmsReporter;
    dmsReporter.ReportContinueApp(true, 0);
    ASSERT_EQ(res, 0);
}

}
} // namespace Rosen
} // namespace OHOS