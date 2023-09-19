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

#include <cstdint>
#include <gtest/gtest.h>
#include "display_manager.h"
#include "window_agent.h"
#include "window_group_mgr.h"
#include "window_impl.h"
#include "window_property.h"
#include "window_root.h"
#include "wm_common.h"
#include "iremote_object_mocker.h"
#include "mock_rs_iwindow_animation_controller.h"
#include "remote_animation.h"
#include "starting_window.h"
#include "window_common_event.h"
#include "scene_board_judgement.h"
#include "vector"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowCommonEventTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WindowCommonEventTest::SetUpTestCase()
{
}
void WindowCommonEventTest::TearDownTestCase()
{
}

void WindowCommonEventTest::SetUp()
{
}

void WindowCommonEventTest::TearDown()
{
}

namespace {
/**
 * @tc.name: PreProcessWindowNode05
 * @tc.desc:UpdateSplitRatioPoints
 * @tc.type: FUNC
 */
HWTEST_F(WindowCommonEventTest, SubscriberEventInner, Function | SmallTest | Level2)
{
    sptr<WindowCommonEvent> windowCommonEvent_ = new WindowCommonEvent();
    windowCommonEvent_->SubscriberEventInner(0);
    std::vector<uint32_t> windowIds;
    ASSERT_EQ(0, windowIds.size());
}

/**
 * @tc.name: UnSubscriberEvent
 * @tc.desc: UnSubscriberEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowCommonEventTest, UnSubscriberEvent, Function | SmallTest | Level2)
{
    sptr<WindowCommonEvent> windowCommonEvent_ = new WindowCommonEvent();
    windowCommonEvent_->UnSubscriberEvent();
    std::vector<uint32_t> windowIdsa;
    ASSERT_EQ(0, windowIdsa.size());
}

/**
 * @tc.name: OnReceiveEvent
 * @tc.desc: OnReceiveEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowCommonEventTest, OnReceiveEvent, Function | SmallTest | Level2)
{
    sptr<WindowCommonEvent> windowCommonEvent_ = new WindowCommonEvent();
    EventFwk::CommonEventData data;
    windowCommonEvent_->OnReceiveEvent(data);
    std::vector<uint32_t> windowIds;
    ASSERT_EQ(0, windowIds.size());
}

/**
 * @tc.name: HandleAccountSwitched
 * @tc.desc: HandleAccountSwitched
 * @tc.type: FUNC
 */
HWTEST_F(WindowCommonEventTest, HandleAccountSwitched, Function | SmallTest | Level2)
{
    sptr<WindowCommonEvent> windowCommonEvent_ = new WindowCommonEvent();
    EventFwk::CommonEventData data;
    windowCommonEvent_->HandleAccountSwitched(data);
    std::vector<uint32_t> windowIds;
    ASSERT_EQ(0, windowIds.size());
}
}
}
}