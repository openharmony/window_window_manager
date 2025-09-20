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
#include <regex>
#include "screen_session_manager_tablet_extension.h"
#include "screen_session_manager.h"
#include "screen_session.h"
#include "display_manager_agent_default.h"
#include "iconsumer_surface.h"
#include "connection/screen_cast_connection.h"
#include "screen_scene_config.h"
#include <surface.h>
#include "scene_board_judgement.h"
#include "fold_screen_state_internel.h"
#include "common_test_utils.h"
#include "iremote_object_mocker.h"
#include "os_account_manager.h"
#include "screen_session_manager_client.h"
#include "../mock/mock_accesstoken_kit.h"
#include <parameter.h>
#include <parameters.h>
#include <functional>
#include "screen_setting_helper.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
#include "window_manager_hilog.h"
#include "test_client.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TabletExtension {
namespace {
const ScreenId SCREENID = 1000;
constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
}
namespace {
std::string g_errLog;
void MyLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char* tag,
    const char* msg)
{
    g_errLog += msg;
}
}
class ScreenSessionManagerExtensionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static sptr<ScreenSessionManagerExt> ssm_;
    void SetAceessTokenPermission(const std::string processName);
};

sptr<ScreenSessionManagerExt> ScreenSessionManagerExtensionTest::ssm_ = nullptr;

void ScreenSessionManagerExtensionTest::SetUpTestCase()
{
    ssm_ = new ScreenSessionManagerExt();
    CommonTestUtils::InjectTokenInfoByHapName(0, "com.ohos.systemui", 0);
    const char** perms = new const char *[1];
    perms[0] = "ohos.permission.CAPTURE_SCREEN";
    CommonTestUtils::SetAceessTokenPermission("foundation", perms, 1);
    delete[] perms;
}

void ScreenSessionManagerExtensionTest::TearDownTestCase()
{
    ssm_ = nullptr;
}

void ScreenSessionManagerExtensionTest::SetUp()
{
}

void ScreenSessionManagerExtensionTest::TearDown()
{
    usleep(SLEEP_TIME_IN_US);
}

namespace {
/**
 * @tc.name: GetPhysicalScreenSession
 * @tc.desc: GetPhysicalScreenSession
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerExtensionTest, GetPhysicalScreenSession, TestSize.Level1)
{
    ScreenSessionManagerExt ssm;

    ScreenId screenId = 123;
    ScreenId defScreenId = 321;
    ScreenProperty property;

    EXPECT_EQ(ssm.GetPhysicalScreenSession(screenId, defScreenId, property), nullptr);
}

/**
 * @tc.name: NotifyCaptureStatusChangedGlobal
 * @tc.desc: NotifyCaptureStatusChangedGlobal
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerExtensionTest, NotifyCaptureStatusChangedGlobal, TestSize.Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new (std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";

    ssm_->NotifyCaptureStatusChangedGlobal();
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    EXPECT_EQ(ssm_->GetScreenSession(screenId)->GetDisplayNode(), nullptr);
}

/**
 * @tc.name: OnScreenChangeDefault
 * @tc.desc: OnScreenChangeDefault
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerExtensionTest, OnScreenChangeDefault, TestSize.Level1)
{
    ScreenSessionManagerExt* ssm = new ScreenSessionManagerExt();
    ssm->connectScreenNumber_ = 1;
    ssm->OnScreenChangeDefault(200, ScreenEvent::DISCONNECTED, ScreenChangeReason::DEFAULT);
    EXPECT_EQ(ssm->connectScreenNumber_, 0);
    delete ssm;
}

/**
 * @tc.name: CheckphyScreenPropMapMutex
 * @tc.desc: CheckphyScreenPropMapMutex
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerExtensionTest, CheckphyScreenPropMapMutex, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    ssm_->CheckphyScreenPropMapMutex();
    EXPECT_TRUE(g_errLog.find("CheckphyScreenPropMapMutex") != std::string::npos);
}

/**
 * @tc.name: CheckNeedSetMultiScreenFrameControl
 * @tc.desc: CheckNeedSetMultiScreenFrameControl
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerExtensionTest, CheckNeedSetMultiScreenFrameControl, TestSize.Level1)
{
    ssm_->CheckNeedSetMultiScreenFrameControl();
    EXPECT_NE(ssm_, nullptr);
}

/**
 * @tc.name: StartSwitchSubscriberInit
 * @tc.desc: StartSwitchSubscriberInit
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerExtensionTest, StartSwitchSubscriberInit, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    ssm_->StartSwitchSubscriberInit();
    EXPECT_TRUE(g_errLog.find("current device") != std::string::npos);
}

/**
 * @tc.name: ScreenConnectionChanged
 * @tc.desc: ScreenConnectionChanged
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerExtensionTest, ScreenConnectionChanged, TestSize.Level1)
{
    ScreenSessionManagerExt ScreenSessionManagerExttest;
    ScreenId screenId = SCREENID;
    sptr<ScreenSession> screenSession = new ScreenSession();
    screenSession->SetScreenId(screenId);
    ScreenEvent screenEvent = ScreenEvent::CONNECTED;

    sptr<ScreenSessionManagerClient> clientProxy = new ScreenSessionManagerClient();
    ScreenSessionManagerExttest.clientProxy_ = clientProxy;

    ScreenSessionManagerExttest.ScreenConnectionChanged(screenSession, screenId, screenEvent);
    EXPECT_NE(ScreenSessionManagerExttest.clientProxy_, nullptr);

    screenEvent = ScreenEvent::DISCONNECTED;
    ScreenSessionManagerExttest.ScreenConnectionChanged(screenSession, screenId, screenEvent);
    EXPECT_NE(ScreenSessionManagerExttest.clientProxy_, nullptr);

    ScreenSessionManagerExttest.clientProxy_ = nullptr;
    ScreenSessionManagerExttest.ScreenConnectionChanged(screenSession, screenId, screenEvent);
    EXPECT_EQ(ScreenSessionManagerExttest.clientProxy_, nullptr);
}

/**
 * @tc.name: GetAndMergeEdidInfo
 * @tc.desc: GetAndMergeEdidInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerExtensionTest, GetAndMergeEdidInfo, TestSize.Level1)
{
    sptr<ScreenSession> screenSession = new ScreenSession();
    screenSession->SetScreenId(SCREENID);

    std::string originalSerialNumber = screenSession->GetSerialNumber();
    std::string originalName = screenSession->GetName();

    ssm_->GetAndMergeEdidInfo(screenSession);
    EXPECT_NE(screenSession->GetSerialNumber(), originalSerialNumber);
    EXPECT_NE(screenSession->GetName(), originalName);
}
} // namespace
} // namespace TabletExtension
} // namespace Rosen
} // namespace OHOS