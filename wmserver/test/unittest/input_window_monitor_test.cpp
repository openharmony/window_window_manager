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
#include "input_window_monitor.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class InputWindowMonitorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<WindowRoot> root_;
    sptr<InputWindowMonitor> input_monitor_;
};

void InputWindowMonitorTest::SetUpTestCase()
{
}

void InputWindowMonitorTest::TearDownTestCase()
{
}

void InputWindowMonitorTest::SetUp()
{
    root_ = new WindowRoot([](Event event, const sptr<IRemoteObject>& remoteObject) {});
    input_monitor_ = new InputWindowMonitor(root_);
}

void InputWindowMonitorTest::TearDown()
{
    root_ = nullptr;
    input_monitor_->InputWindowMonitor::~InputWindowMonitor();
}

namespace {
/**
 * @tc.name: UpdateInputWindow
 * @tc.desc: Update Input Window
 * @tc.type: FUNC
 */
HWTEST_F(InputWindowMonitorTest, UpdateInputWindow01, Function | SmallTest | Level2)
{
    input_monitor_->windowRoot_ = nullptr;
    input_monitor_->UpdateInputWindow(0);
    ASSERT_EQ(nullptr, input_monitor_->windowRoot_);

    input_monitor_->windowRoot_ = new WindowRoot([](Event event, const sptr<IRemoteObject>& remoteObject) {});
    input_monitor_->windowRoot_->windowNodeMap_.insert(std::make_pair(0, nullptr));
    input_monitor_->UpdateInputWindow(0);
    ASSERT_EQ(nullptr, input_monitor_->windowRoot_->windowNodeMap_[0]);
}
}
}
}
