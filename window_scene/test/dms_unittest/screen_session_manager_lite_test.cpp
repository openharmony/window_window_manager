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

#include "screen_session_manager/include/screen_session_manager_lite.h"
#include "window_manager_hilog.h"
#include "display_manager_agent_default.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_US = 100000;
}

class ScreenSessionManagerLiteTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ScreenSessionManagerLiteTest::SetUpTestCase()
{
}

void ScreenSessionManagerLiteTest::TearDownTestCase()
{
}

void ScreenSessionManagerLiteTest::SetUp()
{
}

void ScreenSessionManagerLiteTest::TearDown()
{
    usleep(SLEEP_TIME_US);
}

namespace {

/**
 * @tc.name: ConnectToServer
 * @tc.desc: ConnectToServer
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteTest, ConnectToServer, Function | SmallTest | Level3)
{
    ScreenSessionManagerLite screenSessionManagerLite = ScreenSessionManagerLite();
    screenSessionManagerLite.ConnectToServer();
    ASSERT_EQ(screenSessionManagerLite.screenSessionManager_, nullptr);
}

/**
 * @tc.name: RegisterDisplayManagerAgent
 * @tc.desc: RegisterDisplayManagerAgent
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteTest, RegisterDisplayManagerAgent, Function | SmallTest | Level3)
{
    ScreenSessionManagerLite screenSessionManagerLite = ScreenSessionManagerLite();
    sptr<IDisplayManagerAgent> displayManagerAgent= new DisplayManagerAgentDefault();
    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    DMError ret = screenSessionManagerLite.RegisterDisplayManagerAgent(displayManagerAgent, type);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: UnregisterDisplayManagerAgent
 * @tc.desc: UnregisterDisplayManagerAgent
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteTest, UnregisterDisplayManagerAgent, Function | SmallTest | Level3)
{
    ScreenSessionManagerLite screenSessionManagerLite = ScreenSessionManagerLite();
    sptr<IDisplayManagerAgent> displayManagerAgent= new DisplayManagerAgentDefault();
    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    DMError ret = screenSessionManagerLite.UnregisterDisplayManagerAgent(displayManagerAgent, type);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: GetFoldDisplayMode
 * @tc.desc: GetFoldDisplayMode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteTest, GetFoldDisplayMode, Function | SmallTest | Level3)
{
    ScreenSessionManagerLite screenSessionManagerLite = ScreenSessionManagerLite();
    FoldDisplayMode ret = screenSessionManagerLite.GetFoldDisplayMode();
    ASSERT_EQ(ret, FoldDisplayMode::UNKNOWN);
}

/**
 * @tc.name: IsFoldable
 * @tc.desc: IsFoldable
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteTest, IsFoldable, Function | SmallTest | Level3)
{
    ScreenSessionManagerLite screenSessionManagerLite = ScreenSessionManagerLite();
    bool ret = screenSessionManagerLite.IsFoldable();
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: GetFoldStatus
 * @tc.desc: GetFoldStatus
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteTest, GetFoldStatus, Function | SmallTest | Level3)
{
    ScreenSessionManagerLite screenSessionManagerLite = ScreenSessionManagerLite();
    FoldStatus ret = screenSessionManagerLite.GetFoldStatus();
    ASSERT_EQ(ret, FoldStatus::UNKNOWN);
}

/**
 * @tc.name: GetDefaultDisplayInfo
 * @tc.desc: GetDefaultDisplayInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteTest, GetDefaultDisplayInfo, Function | SmallTest | Level3)
{
    ScreenSessionManagerLite screenSessionManagerLite = ScreenSessionManagerLite();
    sptr<DisplayInfo> ret = screenSessionManagerLite.GetDefaultDisplayInfo();
    ASSERT_EQ(ret, nullptr);
}

/**
 * @tc.name: GetDisplayInfoById
 * @tc.desc: GetDisplayInfoById
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteTest, GetDisplayInfoById, Function | SmallTest | Level3)
{
    ScreenSessionManagerLite screenSessionManagerLite = ScreenSessionManagerLite();
    DisplayId displayId = 0;
    sptr<DisplayInfo> ret = screenSessionManagerLite.GetDisplayInfoById(displayId);
    ASSERT_EQ(ret, nullptr);
}

/**
 * @tc.name: GetCutoutInfo
 * @tc.desc: GetCutoutInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteTest, GetCutoutInfo, Function | SmallTest | Level3)
{
    ScreenSessionManagerLite screenSessionManagerLite = ScreenSessionManagerLite();
    DisplayId displayId = 0;
    sptr<CutoutInfo> ret = screenSessionManagerLite.GetCutoutInfo(displayId);
    ASSERT_EQ(ret, nullptr);
}

/**
 * @tc.name: Clear
 * @tc.desc: Clear
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteTest, Clear, Function | SmallTest | Level3)
{
    ScreenSessionManagerLite screenSessionManagerLite = ScreenSessionManagerLite();
    screenSessionManagerLite.Clear();
    ASSERT_EQ(screenSessionManagerLite.screenSessionManager_, nullptr);
}
}
} // namespace Rosen
} // namespace OHOS