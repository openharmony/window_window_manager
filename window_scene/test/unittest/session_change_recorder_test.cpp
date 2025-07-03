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

#include "session/host/include/session_change_recorder.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr uint32_t MAX_RECORD_TYPE_SIZE = 10;

class SessionChangeRecorderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 500000;
};

void SessionChangeRecorderTest::SetUpTestCase() {}

void SessionChangeRecorderTest::TearDownTestCase() {}

void SessionChangeRecorderTest::SetUp() {}

void SessionChangeRecorderTest::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
}

namespace {
/**
 * @tc.name: RecordSceneSessionChange
 * @tc.desc: RecordSceneSessionChange
 * @tc.type: FUNC
 */
HWTEST_F(SessionChangeRecorderTest, RecordSceneSessionChange, TestSize.Level1)
{
    SceneSessionChangeInfo changeInfo;
    auto result1 = SessionChangeRecorder::GetInstance().RecordSceneSessionChange(
        RecordType::RECORD_TYPE_BEGIN, changeInfo);
    EXPECT_EQ(result1, WSError::WS_ERROR_INVALID_PARAM);

    changeInfo.logTag_ = WmsLogTag::END;
    auto result2 = SessionChangeRecorder::GetInstance().RecordSceneSessionChange(
        RecordType::RECORD_TYPE_BEGIN, changeInfo);
    EXPECT_EQ(result2, WSError::WS_ERROR_INVALID_PARAM);

    changeInfo.logTag_ = WmsLogTag::WMS_MAIN;
    auto result3 = SessionChangeRecorder::GetInstance().RecordSceneSessionChange(
        RecordType::RECORD_TYPE_BEGIN, changeInfo);
    EXPECT_EQ(result3, WSError::WS_ERROR_INVALID_PARAM);

    changeInfo.changeInfo_ = "Record Scene Session Change Test1";
    auto result4 = SessionChangeRecorder::GetInstance().RecordSceneSessionChange(
        RecordType::RECORD_TYPE_BEGIN, changeInfo);
    EXPECT_EQ(result4, WSError::WS_OK);

    changeInfo.changeInfo_ = "Record Scene Session Change Test2";
    changeInfo.time_ = "04-28 12:27:00.791";
    auto result5 = SessionChangeRecorder::GetInstance().RecordSceneSessionChange(
        RecordType::RECORD_TYPE_BEGIN, changeInfo);
    EXPECT_EQ(result5, WSError::WS_OK);
}

/**
 * @tc.name: SetRecordSize
 * @tc.desc: SetRecordSize
 * @tc.type: FUNC
 */
HWTEST_F(SessionChangeRecorderTest, SetRecordSize, TestSize.Level1)
{
    auto result1 = SessionChangeRecorder::GetInstance().SetRecordSize(
        RecordType::RECORD_TYPE_BEGIN, 0);
    EXPECT_EQ(result1, WSError::WS_ERROR_INVALID_PARAM);

    auto result2 = SessionChangeRecorder::GetInstance().SetRecordSize(
        RecordType::RECORD_TYPE_BEGIN, MAX_RECORD_TYPE_SIZE + 1);
    EXPECT_EQ(result2, WSError::WS_ERROR_INVALID_PARAM);

    auto result3 = SessionChangeRecorder::GetInstance().SetRecordSize(
        RecordType::RECORD_TYPE_BEGIN, MAX_RECORD_TYPE_SIZE);
    EXPECT_EQ(result3, WSError::WS_OK);
}

/**
 * @tc.name: GetSceneSessionNeedDumpInfo
 * @tc.desc: GetSceneSessionNeedDumpInfo
 * @tc.type: FUNC
 */
HWTEST_F(SessionChangeRecorderTest, GetSceneSessionNeedDumpInfo, TestSize.Level1)
{
    SessionChangeRecorder::GetInstance().sceneSessionChangeNeedLogMap_.clear();
    SessionChangeRecorder::GetInstance().sceneSessionChangeNeedDumpMap_.clear();

    std::vector<std::string> params;
    std::string dumpInfo;
    SessionChangeRecorder::GetInstance().GetSceneSessionNeedDumpInfo(params, dumpInfo);
    auto result1 = dumpInfo.find("Available args") != std::string::npos;
    EXPECT_TRUE(result1);

    SceneSessionChangeInfo changeInfo1 {
        .persistentId_ = 123,
        .changeInfo_ = "changeInfo1",
        .logTag_ = WmsLogTag::WMS_MAIN,
    };

    SceneSessionChangeInfo changeInfo2 {
        .persistentId_ = 124,
        .changeInfo_ = "changeInfo2",
        .logTag_ = WmsLogTag::WMS_MAIN,
    };

    SceneSessionChangeInfo changeInfo3 {
        .persistentId_ = 125,
        .changeInfo_ = "changeInfo3",
        .logTag_ = WmsLogTag::WMS_MAIN,
    };

    SessionChangeRecorder::GetInstance().RecordSceneSessionChange(RecordType::SESSION_STATE_RECORD, changeInfo1);
    SessionChangeRecorder::GetInstance().RecordSceneSessionChange(RecordType::SESSION_STATE_RECORD, changeInfo2);
    SessionChangeRecorder::GetInstance().RecordSceneSessionChange(RecordType::RECORD_TYPE_END, changeInfo3);

    params.push_back("all");
    SessionChangeRecorder::GetInstance().GetSceneSessionNeedDumpInfo(params, dumpInfo);
    auto result2 = (dumpInfo.find("123") != std::string::npos) && (dumpInfo.find("124") != std::string::npos) &&
        (dumpInfo.find("125") != std::string::npos);
    dumpInfo.clear();
    EXPECT_TRUE(result2);

    params.push_back("1");
    SessionChangeRecorder::GetInstance().GetSceneSessionNeedDumpInfo(params, dumpInfo);
    auto result3 = (dumpInfo.find("123") != std::string::npos) && (dumpInfo.find("124") != std::string::npos) &&
        (dumpInfo.find("125") == std::string::npos);
    dumpInfo.clear();
    EXPECT_TRUE(result3);

    params.pop_back();
    params[0] = "123";
    SessionChangeRecorder::GetInstance().GetSceneSessionNeedDumpInfo(params, dumpInfo);
    auto result4 = (dumpInfo.find("123") != std::string::npos) && (dumpInfo.find("124") == std::string::npos) &&
        (dumpInfo.find("125") == std::string::npos);
    dumpInfo.clear();
    EXPECT_TRUE(result4);
}

/**
 * @tc.name: RecordDump
 * @tc.desc: RecordDump
 * @tc.type: FUNC
 */
HWTEST_F(SessionChangeRecorderTest, RecordDump, TestSize.Level1)
{
    SessionChangeRecorder::GetInstance().sceneSessionChangeNeedDumpMap_.clear();
    SceneSessionChangeInfo changeInfo1 {
        .persistentId_ = 123,
        .changeInfo_ = "changeInfo1",
        .logTag_ = WmsLogTag::WMS_MAIN,
    };
    SessionChangeRecorder::GetInstance().RecordDump(RecordType::RECORD_TYPE_BEGIN, changeInfo1);
    auto result1 = SessionChangeRecorder::GetInstance().sceneSessionChangeNeedDumpMap_;
    EXPECT_EQ(result1[RecordType::RECORD_TYPE_BEGIN].size(), 1);

    SceneSessionChangeInfo changeInfo2 {
        .persistentId_ = 123,
        .changeInfo_ = "changeInfo2",
        .logTag_ = WmsLogTag::WMS_MAIN,
    };
    SessionChangeRecorder::GetInstance().RecordDump(RecordType::RECORD_TYPE_BEGIN, changeInfo2);
    auto result2 = SessionChangeRecorder::GetInstance().sceneSessionChangeNeedDumpMap_;
    EXPECT_EQ(result2[RecordType::RECORD_TYPE_BEGIN].size(), 2);

    SceneSessionChangeInfo changeInfo3 {
        .persistentId_ = 123,
        .changeInfo_ = "changeInfo3",
        .logTag_ = WmsLogTag::WMS_MAIN,
    };
    SessionChangeRecorder::GetInstance().SetRecordSize(RecordType::RECORD_TYPE_BEGIN, 2);
    SessionChangeRecorder::GetInstance().RecordDump(RecordType::RECORD_TYPE_BEGIN, changeInfo3);
    auto result3 = SessionChangeRecorder::GetInstance().sceneSessionChangeNeedDumpMap_;
    EXPECT_EQ(result3[RecordType::RECORD_TYPE_BEGIN].size(), 2);
    SessionChangeRecorder::GetInstance().SetRecordSize(RecordType::RECORD_TYPE_BEGIN, MAX_RECORD_TYPE_SIZE);
}

/**
 * @tc.name: RecordLog
 * @tc.desc: RecordLog
 * @tc.type: FUNC
 */
HWTEST_F(SessionChangeRecorderTest, RecordLog, TestSize.Level1)
{
    SessionChangeRecorder::GetInstance().sceneSessionChangeNeedLogMap_.clear();
    SceneSessionChangeInfo changeInfo1 {
        .persistentId_ = 123,
        .changeInfo_ = "changeInfo1",
        .logTag_ = WmsLogTag::WMS_MAIN,
    };
    SessionChangeRecorder::GetInstance().RecordLog(RecordType::RECORD_TYPE_BEGIN, changeInfo1);
    auto result1 = SessionChangeRecorder::GetInstance().sceneSessionChangeNeedLogMap_;
    EXPECT_EQ(result1[RecordType::RECORD_TYPE_BEGIN].size(), 1);

    SceneSessionChangeInfo changeInfo2 {
        .persistentId_ = 123,
        .changeInfo_ = "changeInfo2",
        .logTag_ = WmsLogTag::WMS_MAIN,
    };
    SessionChangeRecorder::GetInstance().RecordLog(RecordType::RECORD_TYPE_BEGIN, changeInfo2);
    auto result2 = SessionChangeRecorder::GetInstance().sceneSessionChangeNeedLogMap_;
    EXPECT_EQ(result2[RecordType::RECORD_TYPE_BEGIN].size(), 2);

    SceneSessionChangeInfo changeInfo3 {
        .persistentId_ = 123,
        .changeInfo_ = "changeInfo3",
        .logTag_ = WmsLogTag::WMS_MAIN,
    };
    SessionChangeRecorder::GetInstance().SetRecordSize(RecordType::RECORD_TYPE_BEGIN, 2);
    SessionChangeRecorder::GetInstance().RecordLog(RecordType::RECORD_TYPE_BEGIN, changeInfo3);
    auto result3 = SessionChangeRecorder::GetInstance().sceneSessionChangeNeedLogMap_;
    EXPECT_EQ(result3[RecordType::RECORD_TYPE_BEGIN].size(), 0);
    SessionChangeRecorder::GetInstance().SetRecordSize(RecordType::RECORD_TYPE_BEGIN, MAX_RECORD_TYPE_SIZE);
}

/**
 * @tc.name: SimplifyDumpInfo
 * @tc.desc: SimplifyDumpInfo
 * @tc.type: FUNC
 */
HWTEST_F(SessionChangeRecorderTest, SimplifyDumpInfo, TestSize.Level1)
{
    std::string dumpInfo = "";
    SessionChangeRecorder::GetInstance().SimplifyDumpInfo(dumpInfo, "TestSimplifyDumpInfo");
    EXPECT_NE(dumpInfo.size(), 0);
}
}
} // namespace OHOS::Rosen