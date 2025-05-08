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
#include "interfaces/include/ws_common.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "window_manager_agent.h"
#include "session_manager.h"
#include "scene_session_converter.h"
#include "zidl/window_manager_agent_interface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SceneSessionConverterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SceneSessionConverterTest::SetUpTestCase() {}

void SceneSessionConverterTest::TearDownTestCase() {}

void SceneSessionConverterTest::SetUp() {}

void SceneSessionConverterTest::TearDown() {}

namespace {

/**
 * @tc.name: ConvertToMissionInfos
 * @tc.desc: ConvertToMissionInfos func
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionConverterTest, ConvertToMissionInfos, TestSize.Level1)
{
    std::vector<sptr<SceneSession>> sceneSessionInfos;
    std::vector<AAFwk::MissionInfo> missionInfos;
    auto result = SceneSessionConverter::ConvertToMissionInfos(sceneSessionInfos, missionInfos);
    EXPECT_EQ(WSError::WS_OK, result);

    SessionInfo info1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info1, nullptr);
    sceneSessionInfos.push_back(sceneSession);

    SessionInfo info2;
    AppExecFwk::AbilityInfo abilityInfo;
    abilityInfo.excludeFromMissions = true;
    info2.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>(abilityInfo);
    std::shared_ptr<AAFwk::Want> want = std::make_shared<AAFwk::Want>();
    info2.want = want;
    sptr<SceneSession> sceneSession2 = sptr<SceneSession>::MakeSptr(info2, nullptr);
    sceneSessionInfos.push_back(sceneSession2);

    auto result2 = SceneSessionConverter::ConvertToMissionInfos(sceneSessionInfos, missionInfos);
    EXPECT_EQ(WSError::WS_OK, result2);
}

/**
 * @tc.name: ConvertToMissionInfo
 * @tc.desc: ConvertToMissionInfo func
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionConverterTest, ConvertToMissionInfo, TestSize.Level1)
{
    sptr<SceneSession> sceneSession = nullptr;
    AAFwk::MissionInfo missionInfo;
    auto result = SceneSessionConverter::ConvertToMissionInfo(sceneSession, missionInfo);
    EXPECT_EQ(WSError::WS_OK, result);

    SessionInfo info2;
    sptr<SceneSession> sceneSession2 = sptr<SceneSession>::MakeSptr(info2, nullptr);
    auto result2 = SceneSessionConverter::ConvertToMissionInfo(sceneSession2, missionInfo);
    EXPECT_EQ(WSError::WS_OK, result2);

    SessionInfo info3;
    AppExecFwk::AbilityInfo abilityInfo;
    abilityInfo.excludeFromMissions = true;
    info3.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>(abilityInfo);
    std::shared_ptr<AAFwk::Want> want = std::make_shared<AAFwk::Want>();
    info3.want = want;
    sptr<SceneSession> sceneSession3 = sptr<SceneSession>::MakeSptr(info3, nullptr);
    auto result3 = SceneSessionConverter::ConvertToMissionInfo(sceneSession3, missionInfo);
    EXPECT_EQ(WSError::WS_OK, result3);
}

} // namespace
} // namespace Rosen
} // namespace OHOS
