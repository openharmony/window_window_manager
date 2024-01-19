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
#include <thread>

#include "perform_reporter.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "PerformReporterTest"};
}
class PerformReporterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    void SimuReportProcess(PerformReporter& pr, const std::vector<uint32_t>& durations);
    bool PerformDataCmp(const PerformReporter& pr, const uint32_t totalCount, const std::vector<uint32_t>& splitCount);
};

void PerformReporterTest::SetUpTestCase()
{
}

void PerformReporterTest::TearDownTestCase()
{
}

void PerformReporterTest::SetUp()
{
}

void PerformReporterTest::TearDown()
{
}

void PerformReporterTest::SimuReportProcess(PerformReporter& pr, const std::vector<uint32_t>& durations)
{
    for (auto duration : durations) {
        pr.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(duration));
        pr.end();
    }
}

bool PerformReporterTest::PerformDataCmp(const PerformReporter& pr,
    const uint32_t totalCount, const std::vector<uint32_t>& splitCount)
{
    if (pr.totalCount_ != totalCount) {
        WLOGFE("pr.totalCount_=%{public}u, expect=%{public}u", pr.totalCount_.load(), totalCount);
        return false;
    }

    size_t i = 0;
    for (auto& iter: pr.timeSplitCount_) {
        if (iter.second != splitCount[i]) {
            std::ostringstream oss;
            oss << "pr.timeSplitCount_[" << iter.first << "]=" << iter.second << ", but expect=" << splitCount[i];
            WLOGI("%{public}s", oss.str().c_str());
            return false;
        }
        i++;
    }

    return true;
}

namespace {
/**
 * @tc.name: StartEnd
 * @tc.desc: StartEnd test
 * @tc.type: FUNC
 */
HWTEST_F(PerformReporterTest, StartEnd, Function | SmallTest | Level2)
{
    PerformReporter pr = PerformReporter("TestTag", {100, 200, 300}, 10);
    SimuReportProcess(pr, {50, 150, 250, 350, 450});
    ASSERT_EQ(true, PerformDataCmp(pr, 5, {1, 1, 1, 2}));
}

/**
 * @tc.name: StartEndClear
 * @tc.desc: StartEndClear test
 * @tc.type: FUNC
 */
HWTEST_F(PerformReporterTest, StartEndClear, Function | SmallTest | Level2)
{
    PerformReporter pr = PerformReporter("TestTag", {100, 200, 300}, 3);
    SimuReportProcess(pr, {50, 150, 250});
    ASSERT_EQ(true, PerformDataCmp(pr, 0, {0, 0, 0, 0}));
}

/**
 * @tc.name: StartEndInvSeq
 * @tc.desc: StartEndInvSeq test
 * @tc.type: FUNC
 */
HWTEST_F(PerformReporterTest, StartEndInvSeq, Function | SmallTest | Level2)
{
    PerformReporter pr = PerformReporter("TestTag", {100, 200, 300}, 4);
    SimuReportProcess(pr, {250, 150, 50});
    ASSERT_EQ(true, PerformDataCmp(pr, 3, {1, 1, 1, 0}));
}

/**
 * @tc.name: PrivateClear
 * @tc.desc: PrivateClear test
 * @tc.type: FUNC
 */
HWTEST_F(PerformReporterTest, PrivateClear, Function | SmallTest | Level2)
{
    PerformReporter pr = PerformReporter("TestTag", {100, 200, 300}, 10);
    SimuReportProcess(pr, {50, 150, 250, 350, 450});
    ASSERT_EQ(true, PerformDataCmp(pr, 5, {1, 1, 1, 2}));

    pr.clear();
    ASSERT_EQ(true, PerformDataCmp(pr, 0, {0, 0, 0, 0}));
}

/**
 * @tc.name: GetMsgString001
 * @tc.desc: GetMsgString test
 * @tc.type: FUNC
 */
HWTEST_F(PerformReporterTest, GetMsgString001, Function | SmallTest | Level2)
{
    WindowInfoReporter windowInfoReporter;
    FullInfoMap infoMap;
    std::string res = windowInfoReporter.GetMsgString(infoMap);
    ASSERT_EQ(res, "");
}

/**
 * @tc.name: GetMsgString002
 * @tc.desc: GetMsgString test
 * @tc.type: FUNC
 */
HWTEST_F(PerformReporterTest, GetMsgString002, Function | SmallTest | Level2)
{
    WindowInfoReporter windowInfoReporter;
    FullInfoMap infoMap;
    infoMap["bundleName"]["packageName"] = 0;
    std::string res = windowInfoReporter.GetMsgString(infoMap);
    std::ostringstream oss;
    oss << "{";
    for (auto& bundleInfos : infoMap) {
        if (bundleInfos.second.empty()) {
            continue;
        }
        oss << "{";
        for (auto& packageInfo : bundleInfos.second) {
            oss << "BUNDLE_NAME:" << bundleInfos.first << ",";
            oss << "ABILITY_NAME:" << packageInfo.first << ",";
            oss << "COUNT:" << packageInfo.second;
        }
        oss << "},";
    }
    oss << "};";
    ASSERT_EQ(res, oss.str());
}

/**
 * @tc.name: GetMsgString003
 * @tc.desc: GetMsgString test
 * @tc.type: FUNC
 */
HWTEST_F(PerformReporterTest, GetMsgString003, Function | SmallTest | Level2)
{
    WindowInfoReporter windowInfoReporter;
    BundleNameMap infoMap;
    std::string res = windowInfoReporter.GetMsgString(infoMap);
    ASSERT_EQ(res, "");
}

/**
 * @tc.name: GetMsgString004
 * @tc.desc: GetMsgString test
 * @tc.type: FUNC
 */
HWTEST_F(PerformReporterTest, GetMsgString004, Function | SmallTest | Level2)
{
    WindowInfoReporter windowInfoReporter;
    BundleNameMap infoMap;
    infoMap["bundleName"] = 0;
    std::string res = windowInfoReporter.GetMsgString(infoMap);
    std::ostringstream oss;
    oss << "{";
    for (auto& bundleInfo : infoMap) {
        oss << "{";
        oss << "BUNDLE_NAME:" << bundleInfo.first << ",";
        oss << "COUNT:" << bundleInfo.second;
        oss << "},";
    }
    oss << "};";
    ASSERT_EQ(res, oss.str());
}

/**
 * @tc.name: InsertCreateReportInfo005
 * @tc.desc: InsertCreateReportInfo test
 * @tc.type: FUNC
 */
HWTEST_F(PerformReporterTest, InsertCreateReportInfo005, Function | SmallTest | Level2)
{
    int res = 0;
    WindowInfoReporter windowInfoReporter;
    std::string bundleName = "bundleName";
    std::string packageName = "packageName";
    windowInfoReporter.InsertCreateReportInfo(bundleName);
    windowInfoReporter.InsertShowReportInfo(bundleName);
    windowInfoReporter.InsertHideReportInfo(bundleName);
    windowInfoReporter.InsertDestroyReportInfo(bundleName);
    windowInfoReporter.InsertNavigationBarReportInfo(bundleName, packageName);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: UpdateReportInfo006
 * @tc.desc: UpdateReportInfo test
 * @tc.type: FUNC
 */
HWTEST_F(PerformReporterTest, UpdateReportInfo006, Function | SmallTest | Level2)
{
    WindowInfoReporter windowInfoReporter;
    FullInfoMap infoMap;
    std::string bundleName;
    std::string packageName;
    windowInfoReporter.UpdateReportInfo(infoMap, bundleName, packageName);
    std::string res = windowInfoReporter.GetMsgString(infoMap);
    ASSERT_EQ(res, "");
}

/**
 * @tc.name: UpdateReportInfo007
 * @tc.desc: UpdateReportInfo test
 * @tc.type: FUNC
 */
HWTEST_F(PerformReporterTest, UpdateReportInfo007, Function | SmallTest | Level2)
{
    int res = 0;
    WindowInfoReporter windowInfoReporter;
    FullInfoMap infoMap_1;
    std::string bundleName = "bundleName";
    std::string packageName = "packageName";
    infoMap_1["bundleName"]["packageName"] = 0;
    windowInfoReporter.UpdateReportInfo(infoMap_1, bundleName, packageName);
    FullInfoMap infoMap_2;
    infoMap_2["Name"]["packageName"] = 0;
    windowInfoReporter.UpdateReportInfo(infoMap_2, bundleName, packageName);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: UpdateReportInfo008
 * @tc.desc: UpdateReportInfo test
 * @tc.type: FUNC
 */
HWTEST_F(PerformReporterTest, UpdateReportInfo008, Function | SmallTest | Level2)
{
    WindowInfoReporter windowInfoReporter;
    BundleNameMap infoMap;
    std::string bundleName;
    windowInfoReporter.UpdateReportInfo(infoMap, bundleName);
    std::string res = windowInfoReporter.GetMsgString(infoMap);
    ASSERT_EQ(res, "");
}

/**
 * @tc.name: UpdateReportInfo009
 * @tc.desc: UpdateReportInfo test
 * @tc.type: FUNC
 */
HWTEST_F(PerformReporterTest, UpdateReportInfo009, Function | SmallTest | Level2)
{
    int res = 0;
    WindowInfoReporter windowInfoReporter;
    BundleNameMap infoMap_1;
    std::string bundleName = "bundleName";
    infoMap_1["bundleName"] = 0;
    windowInfoReporter.UpdateReportInfo(infoMap_1, bundleName);
    BundleNameMap infoMap_2;
    infoMap_2["Name"] = 0;
    windowInfoReporter.UpdateReportInfo(infoMap_2, bundleName);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: ReportBackButtonInfoImmediately010
 * @tc.desc: ReportBackButtonInfoImmediately test
 * @tc.type: FUNC
 */
HWTEST_F(PerformReporterTest, ReportBackButtonInfoImmediately010, Function | SmallTest | Level2)
{
    int res = 0;
    WindowInfoReporter windowInfoReporter;
    windowInfoReporter.ReportBackButtonInfoImmediately();
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: ReportZeroOpacityInfoImmediately011
 * @tc.desc: ReportZeroOpacityInfoImmediately test
 * @tc.type: FUNC
 */
HWTEST_F(PerformReporterTest, ReportZeroOpacityInfoImmediately011, Function | SmallTest | Level2)
{
    int res = 0;
    std::string bundleName;
    std::string packageName = "packageName";
    WindowInfoReporter windowInfoReporter;
    windowInfoReporter.ReportZeroOpacityInfoImmediately(bundleName, packageName);
    bundleName = "bundleName";
    windowInfoReporter.ReportZeroOpacityInfoImmediately(bundleName, packageName);
    ASSERT_EQ(res, 0);
}

/**
 * @tc.name: ReportStartWindow012
 * @tc.desc: ReportStartWindow test
 * @tc.type: FUNC
 */
HWTEST_F(PerformReporterTest, ReportStartWindow012, Function | SmallTest | Level2)
{
    int res = 0;
    std::string bundleName = "bundleName";
    std::string windowName = "windowName";
    WindowInfoReporter windowInfoReporter;
    windowInfoReporter.ReportStartWindow(bundleName, windowName);
    ASSERT_EQ(res, 0);
}
}
} // namespace Rosen
} // namespace OHOS