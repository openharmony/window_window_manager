/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#include <fcntl.h>
#include <gtest/gtest.h>
#include "common_test_utils.h"
#include "iremote_object_mocker.h"
#include "mock_IWindow.h"
#include "mock_RSIWindowAnimationController.h"
#include "window_manager_service.h"

#include <thread>

#include <ability_manager_client.h>
#include <cinttypes>
#include <chrono>
#include <hisysevent.h>
#include <hitrace_meter.h>
#include <ipc_skeleton.h>
#include <parameters.h>
#include <rs_iwindow_animation_controller.h>
#include <system_ability_definition.h>
#include <sstream>
#include "xcollie/watchdog.h"

#include "color_parser.h"
#include "display_manager_service_inner.h"
#include "dm_common.h"
#include "drag_controller.h"
#include "minimize_app.h"
#include "permission.h"
#include "remote_animation.h"
#include "singleton_container.h"
#include "ui/rs_ui_director.h"
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_manager_agent_controller.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "wm_math.h"


using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class MockSessionManagerServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void SetAceessTokenPermission(const std::string processName);
    sptr<WindowManagerService> wms = new WindowManagerService();
};

void WindowManagerServiceTest::SetUpTestCase()
{
}

void WindowManagerServiceTest::TearDownTestCase()
{
}

void WindowManagerServiceTest::SetUp()
{
    CommonTestUtils::SetAceessTokenPermission("WindowManagerServiceTest");
}

void WindowManagerServiceTest::TearDown()
{
}

namespace {

/*
 * @tc.name: SetSessionManagerService01
 * @tc.desc: SetSessionManagerService
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, SetSessionManagerService01, Function | SmallTest | Level2)
{
    sptr<MockSessionManagerService> mms = new MockSessionManagerService();
    ASSERT_FALSE(mms->SetSessionManagerService(nullptr));
}

/*
 * @tc.name: GetSessionManagerService01
 * @tc.desc: GetSessionManagerService
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, GetSessionManagerService01, Function | SmallTest | Level2)
{
    sptr<MockSessionManagerService> mms = new MockSessionManagerService();
    ASSERT_EQ(nullptr,mms->GetSessionManagerService());
}

/*
 * @tc.name: GetSceneSessionManager01
 * @tc.desc: GetSceneSessionManager
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, GetSceneSessionManager01, Function | SmallTest | Level2)
{
    sptr<MockSessionManagerService> mms = new MockSessionManagerService();
    ASSERT_EQ(nullptr,mms->GetSceneSessionManager());
}

/*
 * @tc.name: OnStart01
 * @tc.desc: OnStart
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, OnStart01, Function | SmallTest | Level2)
{
    auto ret=1;
    sptr<MockSessionManagerService> mms = new MockSessionManagerService();
    mms->OnStart()
    ASSERT_EQ(1,ret);
}

/*
 * @tc.name: OnRemoteDied01
 * @tc.desc: OnRemoteDie
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, OnRemoteDied01, Function | SmallTest | Level2)
{
    auto ret = 1;
    sptr<MockSessionManagerService::SMSDeathRecipient> smsDeathRecipient_ = new MockSessionManagerService::SMSDeathRecipient();
    smsDeathRecipient_->OnRemoteDie();
    ASSERT_EQ(1, ret);
}
}
}
}
