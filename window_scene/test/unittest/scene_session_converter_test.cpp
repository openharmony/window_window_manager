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

void SceneSessionConverterTest::SetUpTestCase()
{
}

void SceneSessionConverterTest::TearDownTestCase()
{
}

void SceneSessionConverterTest::SetUp()
{
}

void SceneSessionConverterTest::TearDown()
{
}

namespace {

/**
 * @tc.name: ConvertToMissionInfos
 * @tc.desc: ConvertToMissionInfos func
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionConverterTest, ConvertToMissionInfos_EmptyInput, Function | SmallTest | Level1)
{
    std::vector<sptr<SceneSession>> sceneSessionInfos;
    std::vector<AAFwk::MissionInfo> missionInfos;
    auto result = SceneSessionConverter::ConvertToMissionInfos(sceneSessionInfos, missionInfos);
    ASSERT_EQ(WSError::WS_OK, result);
}

}
} // namespace Rosen
} // namespace OHOS
