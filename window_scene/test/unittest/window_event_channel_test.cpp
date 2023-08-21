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

#include <gtest/gtest.h>
#include "session/container/include/window_event_channel.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class WindowEventChannelTest : public testing::Test {
    public:
        WindowEventChannelTest() {}
        ~WindowEventChannelTest() {}
};

/**
 * @tc.name: OnDispatchEventProcessed
 * @tc.desc: normal function OnDispatchEventProcessed
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelTest, OnDispatchEventProcessed, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowEventChannelTest: OnDispatchEventProcessed";
    int32_t eventId = 12;
    int64_t actionTime = 8888;
    uint32_t res = 0;
    WindowEventChannel::OnDispatchEventProcessed(eventId, actionTime);
    ASSERT_EQ(0, res);
    GTEST_LOG_(INFO) << "WindowEventChannelTest: OnDispatchEventProcessed";
}

}
