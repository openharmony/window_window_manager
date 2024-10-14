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

#include "connection/screen_snapshot_picker_connection.h"
#include "screen_scene_config.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
}
class ScreenSnapshotPickerConnectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ScreenSnapshotPickerConnectionTest::SetUpTestCase()
{
}

void ScreenSnapshotPickerConnectionTest::TearDownTestCase()
{
    usleep(SLEEP_TIME_IN_US);
}

void ScreenSnapshotPickerConnectionTest::SetUp()
{
}

void ScreenSnapshotPickerConnectionTest::TearDown()
{
}

namespace {

/**
 * @tc.name: SnapshotPickerConnectExtension01
 * @tc.desc: SnapshotPickerConnectExtension func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSnapshotPickerConnectionTest, SnapshotPickerConnectExtension01, Function | SmallTest | Level1)
{
    ScreenSceneConfig::LoadConfigXml();
    auto strConfig = ScreenSceneConfig::GetStringConfig();
    std::string bundleName = strConfig["screenSnapshotBundleName"];
    std::string abilityName = strConfig["screenSnapshotAbilityName"];
    ScreenSnapshotPickerConnection::GetInstance().SetBundleName(bundleName);
    ScreenSnapshotPickerConnection::GetInstance().SetAbilityName(abilityName);
    auto ret = ScreenSnapshotPickerConnection::GetInstance().SnapshotPickerConnectExtension();
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: SnapshotPickerConnectExtension02
 * @tc.desc: SnapshotPickerConnectExtension func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSnapshotPickerConnectionTest, SnapshotPickerConnectExtension02, Function | SmallTest | Level1)
{
    ScreenSceneConfig::LoadConfigXml();
    auto strConfig = ScreenSceneConfig::GetStringConfig();
    std::string bundleName = strConfig[""];
    std::string abilityName = strConfig["screenSnapshotAbilityName"];
    ScreenSnapshotPickerConnection::GetInstance().SetBundleName(bundleName);
    ScreenSnapshotPickerConnection::GetInstance().SetAbilityName(abilityName);
    auto ret = ScreenSnapshotPickerConnection::GetInstance().SnapshotPickerConnectExtension();
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: SnapshotPickerConnectExtension03
 * @tc.desc: SnapshotPickerConnectExtension func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSnapshotPickerConnectionTest, SnapshotPickerConnectExtension03, Function | SmallTest | Level1)
{
    ScreenSceneConfig::LoadConfigXml();
    auto strConfig = ScreenSceneConfig::GetStringConfig();
    std::string bundleName = strConfig["screenSnapshotBundleName"];
    std::string abilityName = strConfig[""];
    ScreenSnapshotPickerConnection::GetInstance().SetBundleName(bundleName);
    ScreenSnapshotPickerConnection::GetInstance().SetAbilityName(abilityName);
    auto ret = ScreenSnapshotPickerConnection::GetInstance().SnapshotPickerConnectExtension();
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: SnapshotPickerConnectExtension04
 * @tc.desc: SnapshotPickerConnectExtension func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSnapshotPickerConnectionTest, SnapshotPickerConnectExtension04, Function | SmallTest | Level1)
{
    ScreenSceneConfig::LoadConfigXml();
    auto strConfig = ScreenSceneConfig::GetStringConfig();
    std::string bundleName = strConfig[""];
    std::string abilityName = strConfig[""];
    ScreenSnapshotPickerConnection::GetInstance().SetBundleName(bundleName);
    ScreenSnapshotPickerConnection::GetInstance().SetAbilityName(abilityName);
    auto ret = ScreenSnapshotPickerConnection::GetInstance().SnapshotPickerConnectExtension();
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: SnapshotPickerConnectExtension05
 * @tc.desc: SnapshotPickerConnectExtension func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSnapshotPickerConnectionTest, SnapshotPickerConnectExtension05, Function | SmallTest | Level1)
{
    ScreenSceneConfig::LoadConfigXml();
    auto strConfig = ScreenSceneConfig::GetStringConfig();
    std::string bundleName = strConfig[""];
    std::string abilityName = strConfig[""];
    std::unique_ptr<ScreenSessionAbilityConnection> abilityConnection_ =
        std::make_unique<ScreenSessionAbilityConnection>();
    ScreenSnapshotPickerConnection::GetInstance().SetBundleName(bundleName);
    ScreenSnapshotPickerConnection::GetInstance().SetAbilityName(abilityName);
    auto ret = ScreenSnapshotPickerConnection::GetInstance().SnapshotPickerConnectExtension();
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: GetScreenSnapshotInfo01
 * @tc.desc: GetScreenSnapshotInfo func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSnapshotPickerConnectionTest, GetScreenSnapshotInfo01, Function | SmallTest | Level1)
{
    ScreenSnapshotPickerConnection::GetInstance().abilityConnection_ = nullptr;
    Media::Rect rect{};
    ScreenId screenId = SCREEN_ID_INVALID;
    auto ret = ScreenSnapshotPickerConnection::GetInstance().GetScreenSnapshotInfo(rect, screenId);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: GetScreenSnapshotInfo02
 * @tc.desc: GetScreenSnapshotInfo func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSnapshotPickerConnectionTest, GetScreenSnapshotInfo02, Function | SmallTest | Level1)
{
    ScreenSnapshotPickerConnection::GetInstance().abilityConnection_ =
        std::make_unique<ScreenSessionAbilityConnection>();
    ScreenSnapshotPickerConnection::GetInstance().abilityConnection_->
        GetScreenSessionAbilityConnectionStub() = nullptr;
    Media::Rect rect{};
    ScreenId screenId;
    auto ret = ScreenSnapshotPickerConnection::GetInstance().GetScreenSnapshotInfo(rect, screenId);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: SnapshotPickerDisconnectExtension01
 * @tc.desc: SnapshotPickerDisconnectExtension func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSnapshotPickerConnectionTest, SnapshotPickerDisconnectExtension01, Function | SmallTest | Level1)
{
    std::unique_ptr<ScreenSessionAbilityConnection> abilityConnection_ = nullptr;
    ScreenSnapshotPickerConnection::GetInstance().SnapshotPickerDisconnectExtension();
    EXPECT_EQ(abilityConnection_, nullptr);
}

/**
 * @tc.name: SnapshotPickerDisconnectExtension02
 * @tc.desc: SnapshotPickerDisconnectExtension func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSnapshotPickerConnectionTest, SnapshotPickerDisconnectExtension02, Function | SmallTest | Level1)
{
    std::unique_ptr<ScreenSessionAbilityConnection> abilityConnection_ =
        std::make_unique<ScreenSessionAbilityConnection>();
    ScreenSnapshotPickerConnection::GetInstance().SnapshotPickerDisconnectExtension();
    EXPECT_NE(abilityConnection_, nullptr);
}

/**
 * @tc.name: GetScreenSnapshotRect
 * @tc.desc: GetScreenSnapshotRect func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSnapshotPickerConnectionTest, GetScreenSnapshotRect, Function | SmallTest | Level1)
{
    Media::Rect rect{};
    ScreenId screenId = SCREEN_ID_INVALID;
    ScreenSceneConfig::LoadConfigXml();
    auto strConfig = ScreenSceneConfig::GetStringConfig();
    std::string bundleName = strConfig["screenSnapshotBundleName"];
    std::string abilityName = strConfig["screenSnapshotAbilityName"];
    ScreenSnapshotPickerConnection::GetInstance().SetBundleName(bundleName);
    ScreenSnapshotPickerConnection::GetInstance().SetAbilityName(abilityName);
    auto ret = ScreenSnapshotPickerConnection::GetInstance().SnapshotPickerConnectExtension();
    EXPECT_EQ(ret, false);
    ScreenSnapshotPickerConnection::GetInstance().GetScreenSnapshotInfo(rect, screenId);
    EXPECT_EQ(rect.top, 0);
    EXPECT_EQ(rect.left, 0);
    EXPECT_EQ(rect.width, 0);
    EXPECT_EQ(rect.height, 0);
    ScreenSnapshotPickerConnection::GetInstance().SnapshotPickerDisconnectExtension();
}

}
} // namespace Rosen
} // namespace OHOS