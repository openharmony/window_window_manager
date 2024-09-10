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
namespace {
constexpr uint32_t SLEEP_TIME_US = 100000;
}
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
    usleep(SLEEP_TIME_US);
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

/**
 * @tc.name: OutputDumpInfo
 * @tc.desc: test function : OutputDumpInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, OutputDumpInfo, Function | SmallTest | Level1)
{
    int fd = -1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper->OutputDumpInfo();
    ASSERT_EQ(dumper->fd_, -1);
}

/**
 * @tc.name: ExcuteDumpCmd
 * @tc.desc: test function : ExcuteDumpCmd
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, ExcuteDumpCmd, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args;
    sptr<ScreenSessionDumper> dumper1 = new ScreenSessionDumper(fd, args);
    dumper1->ExcuteDumpCmd();
    ASSERT_EQ(dumper1->fd_, 1);

    fd = 1;
    args = {u"-h"};
    sptr<ScreenSessionDumper> dumper2 = new ScreenSessionDumper(fd, args);
    dumper2->ExcuteDumpCmd();
    ASSERT_EQ(dumper2->fd_, 1);

    fd = 1;
    args = {u"-a"};
    sptr<ScreenSessionDumper> dumper3 = new ScreenSessionDumper(fd, args);
    dumper3->ExcuteDumpCmd();
    ASSERT_EQ(dumper3->fd_, 1);

    fd = 1;
    args = {u"-f"};
    sptr<ScreenSessionDumper> dumper4 = new ScreenSessionDumper(fd, args);
    dumper4->ExcuteDumpCmd();
    ASSERT_EQ(dumper4->fd_, 1);

    fd = 1;
    args = {u"-z"};
    sptr<ScreenSessionDumper> dumper5 = new ScreenSessionDumper(fd, args);
    dumper5->ExcuteDumpCmd();
    ASSERT_EQ(dumper5->fd_, 1);

    fd = 1;
    args = {u"-y"};
    sptr<ScreenSessionDumper> dumper6 = new ScreenSessionDumper(fd, args);
    dumper6->ExcuteDumpCmd();
    ASSERT_EQ(dumper6->fd_, 1);

    fd = 1;
    args = {u"-p"};
    sptr<ScreenSessionDumper> dumper7 = new ScreenSessionDumper(fd, args);
    dumper7->ExcuteDumpCmd();
    ASSERT_EQ(dumper7->fd_, 1);

    fd = 1;
    args = {u"-g"};
    sptr<ScreenSessionDumper> dumper8 = new ScreenSessionDumper(fd, args);
    dumper8->ExcuteDumpCmd();
    ASSERT_EQ(dumper8->fd_, 1);
}

/**
 * @tc.name: DumpEventTracker
 * @tc.desc: test function : DumpEventTracker
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, DumpEventTracker, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    EventTracker tracker;
    dumper->DumpEventTracker(tracker);
    ASSERT_EQ(dumper->fd_, 1);
}

/**
 * @tc.name: DumpFreezedPidList
 * @tc.desc: test function : DumpFreezedPidList
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, DumpFreezedPidList, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    std::set<int32_t> pidList = {1, 2, 3};
    dumper->DumpFreezedPidList(pidList);
    ASSERT_EQ(dumper->fd_, 1);
}

/**
 * @tc.name: ShowHelpInfo
 * @tc.desc: test function : ShowHelpInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, ShowHelpInfo, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper->ShowHelpInfo();
    ASSERT_EQ(dumper->fd_, 1);
}

/**
 * @tc.name: ShowAllScreenInfo
 * @tc.desc: test function : ShowAllScreenInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, ShowAllScreenInfo, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper->ShowAllScreenInfo();
    ASSERT_EQ(dumper->fd_, 1);
}

/**
 * @tc.name: DumpFoldStatus
 * @tc.desc: test function : DumpFoldStatus
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, DumpFoldStatus, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper->DumpFoldStatus();
    ASSERT_EQ(dumper->fd_, 1);
}

/**
 * @tc.name: DumpScreenSessionById
 * @tc.desc: test function : DumpScreenSessionById
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, DumpScreenSessionById, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    ScreenId id = 0;
    dumper->DumpScreenSessionById(id);
    ASSERT_EQ(dumper->fd_, 1);

    id = 5;
    dumper->DumpScreenSessionById(id);
    ASSERT_EQ(dumper->fd_, 1);
}

/**
 * @tc.name: DumpRsInfoById
 * @tc.desc: test function : DumpRsInfoById
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, DumpRsInfoById, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    ScreenId id = 0;
    dumper->DumpRsInfoById(id);
    ASSERT_EQ(dumper->fd_, 1);

    id = 5;
    dumper->DumpRsInfoById(id);
    ASSERT_EQ(dumper->fd_, 1);
}

/**
 * @tc.name: DumpCutoutInfoById
 * @tc.desc: test function : DumpCutoutInfoById
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, DumpCutoutInfoById, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    ScreenId id = 0;
    dumper->DumpCutoutInfoById(id);
    ASSERT_EQ(dumper->fd_, 1);

    id = 5;
    dumper->DumpCutoutInfoById(id);
    ASSERT_EQ(dumper->fd_, 1);
}

/**
 * @tc.name: DumpScreenInfoById
 * @tc.desc: test function : DumpScreenInfoById
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, DumpScreenInfoById, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    ScreenId id = 0;
    dumper->DumpScreenInfoById(id);
    ASSERT_EQ(dumper->fd_, 1);

    id = 5;
    dumper->DumpScreenInfoById(id);
    ASSERT_EQ(dumper->fd_, 1);
}

/**
 * @tc.name: DumpScreenPropertyById
 * @tc.desc: test function : DumpScreenPropertyById
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, DumpScreenPropertyById, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    ScreenId id = 0;
    dumper->DumpScreenPropertyById(id);
    ASSERT_EQ(dumper->fd_, 1);

    id = 5;
    dumper->DumpScreenPropertyById(id);
    ASSERT_EQ(dumper->fd_, 1);
}

/**
 * @tc.name: ShowNotifyFoldStatusChangedInfo
 * @tc.desc: test function : ShowNotifyFoldStatusChangedInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, ShowNotifyFoldStatusChangedInfo, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper->params_[0] = "0";
    dumper->ShowNotifyFoldStatusChangedInfo();
    ASSERT_EQ(dumper->fd_, 1);

    dumper->params_[0] = "1";
    dumper->ShowNotifyFoldStatusChangedInfo();
    ASSERT_EQ(dumper->fd_, 1);

    dumper->params_[0] = "5";
    dumper->ShowNotifyFoldStatusChangedInfo();
    ASSERT_EQ(dumper->fd_, 1);
}

/**
 * @tc.name: ShowIllegalArgsInfo
 * @tc.desc: test function : ShowIllegalArgsInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, ShowIllegalArgsInfo, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper->params_[0] = "0";
    dumper->ShowIllegalArgsInfo();
    ASSERT_EQ(dumper->fd_, 1);
}

/**
 * @tc.name: SetMotionSensorvalue
 * @tc.desc: test function : SetMotionSensorvalue
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, SetMotionSensorvalue01, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper ->SetMotionSensorvalue("-motion,1");
    ASSERT_EQ(true, true);
}

/**
 * @tc.name: SetMotionSensorvalue
 * @tc.desc: test function : SetMotionSensorvalue
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, SetMotionSensorvalue02, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper ->SetMotionSensorvalue("-motion,2");
    ASSERT_EQ(true, true);
}

/**
 * @tc.name: SetMotionSensorvalue
 * @tc.desc: test function : SetMotionSensorvalue
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, SetMotionSensorvalue03, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper ->SetMotionSensorvalue("-motion,3");
    ASSERT_EQ(true, true);
}

/**
 * @tc.name: SetMotionSensorvalue
 * @tc.desc: test function : SetMotionSensorvalue
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, SetMotionSensorvalue04, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper ->SetMotionSensorvalue("-motion,4");
    ASSERT_EQ(true, true);
}

/**
 * @tc.name: SetMotionSensorvalue
 * @tc.desc: test function : SetMotionSensorvalue
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, SetMotionSensorvalue05, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper ->SetMotionSensorvalue("-motion,5");
    ASSERT_EQ(true, true);
}

/**
 * @tc.name: SetMotionSensorvalue
 * @tc.desc: test function : SetMotionSensorvalue
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, SetMotionSensorvalue06, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper ->SetMotionSensorvalue("-motion,9999");
    ASSERT_EQ(true, true);
}

/**
 * @tc.name: SetMotionSensorvalue
 * @tc.desc: test function : SetMotionSensorvalue
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, SetMotionSensorvalue07, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper ->SetMotionSensorvalue("-motion,xxxxx");
    ASSERT_EQ(true, true);
}

/**
 * @tc.name: SetMotionSensorvalue
 * @tc.desc: test function : SetMotionSensorvalue
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, SetMotionSensorvalue07, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper ->SetMotionSensorvalue("-motion,,,,,,");
    ASSERT_EQ(true, true);
}

/**
 * @tc.name: SetRotationLockedvalue
 * @tc.desc: test function : SetRotationLockedvalue
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, SetMotionSensorvalue01, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper ->SetRotationLockedvalue("-rotationlock,0");
    ASSERT_EQ(true, true);
}

/**
 * @tc.name: SetRotationLockedvalue
 * @tc.desc: test function : SetRotationLockedvalue
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, SetMotionSensorvalue02, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper ->SetRotationLockedvalue("-rotationlock,1");
    ASSERT_EQ(true, true);
}


/**
 * @tc.name: SetRotationLockedvalue
 * @tc.desc: test function : SetRotationLockedvalue
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, SetMotionSensorvalue03, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper ->SetRotationLockedvalue("-rotationlock,,,,,");
    ASSERT_EQ(true, true);
}

/**
 * @tc.name: SetRotationLockedvalue
 * @tc.desc: test function : SetRotationLockedvalue
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, SetMotionSensorvalue04, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper ->SetRotationLockedvalue("-rotationlock,1-");
    ASSERT_EQ(true, true);
}

/**
 * @tc.name: MockSendCastPublishEvent
 * @tc.desc: test function : MockSendCastPublishEvent
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, MockSendCastPublishEvent01, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper ->MockSendCastPublishEvent("-publishcastevent,0");
    ASSERT_EQ(true, true);
}

/**
 * @tc.name: MockSendCastPublishEvent
 * @tc.desc: test function : MockSendCastPublishEvent
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, MockSendCastPublishEvent02, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper ->MockSendCastPublishEvent("-publishcastevent,1");
    ASSERT_EQ(true, true);
}


/**
 * @tc.name: MockSendCastPublishEvent
 * @tc.desc: test function : MockSendCastPublishEvent
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, MockSendCastPublishEvent03, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper ->MockSendCastPublishEvent("-publishcastevent,,,,,");
    ASSERT_EQ(true, true);
}

/**
 * @tc.name: MockSendCastPublishEvent
 * @tc.desc: test function : MockSendCastPublishEvent
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, MockSendCastPublishEvent04, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper ->MockSendCastPublishEvent("-publishcastevent,1-");
    ASSERT_EQ(true, true);
}

/**
 * @tc.name: MockSendCastPublishEvent
 * @tc.desc: test function : MockSendCastPublishEvent
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, MockSendCastPublishEvent05, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper ->MockSendCastPublishEvent("-publishcastevent,a");
    ASSERT_EQ(true, true);
}

/**
 * @tc.name: MockSendCastPublishEvent
 * @tc.desc: test function : MockSendCastPublishEvent
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, MockSendCastPublishEvent06, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper ->MockSendCastPublishEvent("-publishcastevent=1");
    ASSERT_EQ(true, true);
}

/**
 * @tc.name: MockSendCastPublishEvent
 * @tc.desc: test function : MockSendCastPublishEvent
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionDumperTest, MockSendCastPublishEvent07, Function | SmallTest | Level1)
{
    int fd = 1;
    std::vector<std::u16string> args = {u"-h"};
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    dumper ->MockSendCastPublishEvent("-publishcastevent,7");
    ASSERT_EQ(true, true);
}
}
}
}