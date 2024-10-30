/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "window_frame_trace.h"
#include <gtest/gtest.h>
#include <unistd.h>

using namespace testing;
using namespace testing::ext;
namespace FRAME_TRACE {
class WindowFrameTraceImplTest : public testing::Test {
  public:
    WindowFrameTraceImplTest() {}
    ~WindowFrameTraceImplTest() {}
};

namespace {
#ifdef FRAME_TRACE_ENABLE
/**
 * @tc.name: AccessFrameTrace01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(WindowFrameTraceImplTest, AccessFrameTrace01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowFrameTraceImplTest: AccessFrameTrace01 start";
    WindowFrameTraceImpl trace;
    bool res = trace.AccessFrameTrace();
    ASSERT_EQ(res, true);
    bool res1 = trace.AccessFrameTrace();
    ASSERT_EQ(res1, true);
    WindowFrameTraceImpl *trace_ = WindowFrameTraceImpl::GetInstance();
    bool res2 = trace_->AccessFrameTrace();
    ASSERT_EQ(res2, true);
    GTEST_LOG_(INFO) << "WindowFrameTraceImplTest: AccessFrameTrace01 end";
}

/**
 * @tc.name: VsyncStartFrameTrace01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(WindowFrameTraceImplTest, VsyncStartFrameTrace01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowFrameTraceImplTest: VsyncStartFrameTrace01 start";
    WindowFrameTraceImpl trace;
    int32_t res = 0;
    trace.VsyncStartFrameTrace();
    ASSERT_EQ(res, 0);
    WindowFrameTraceImpl *trace_ = WindowFrameTraceImpl::GetInstance();
    bool res2 = trace_->AccessFrameTrace();
    trace_->VsyncStartFrameTrace();
    ASSERT_EQ(res2, true);
    GTEST_LOG_(INFO) << "WindowFrameTraceImplTest: VsyncStartFrameTrace01 end";
}

/**
 * @tc.name: VsyncStopFrameTrace01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(WindowFrameTraceImplTest, VsyncStopFrameTrace01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowFrameTraceImplTest: VsyncStopFrameTrace01 start";
    WindowFrameTraceImpl trace;
    int32_t res = 0;
    trace.VsyncStopFrameTrace();
    ASSERT_EQ(res, 0);
    WindowFrameTraceImpl *trace_ = WindowFrameTraceImpl::GetInstance();
    bool res2 = trace_->AccessFrameTrace();
    trace_->VsyncStartFrameTrace();
    trace_->VsyncStopFrameTrace();
    ASSERT_EQ(res2, true);
    GTEST_LOG_(INFO) << "WindowFrameTraceImplTest: VsyncStopFrameTrace01 end";
}
#endif
} // namespace
} // namespace FRAME_TRACE
