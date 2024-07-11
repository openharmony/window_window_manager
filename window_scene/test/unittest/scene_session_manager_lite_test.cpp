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

#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/scene_session_manager_lite.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SceneSessionManagerLiteTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SceneSessionManagerLiteTest::SetUpTestCase()
{
}

void SceneSessionManagerLiteTest::TearDownTestCase()
{
}

void SceneSessionManagerLiteTest::SetUp()
{
}

void SceneSessionManagerLiteTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetInstance
 * @tc.desc: test function : GetInstance
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteTest, GetInstance, Function | SmallTest | Level1)
{
    SceneSessionManagerLite& instance1 = SceneSessionManagerLite::GetInstance();
    EXPECT_NE(nullptr, &instance1);
    SceneSessionManagerLite& instance2 = SceneSessionManagerLite::GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
 * @tc.name: GetSessionVerificationInfo
 * @tc.desc: GetSessionVerificationInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteTest, GetSessionVerificationInfo, Function | SmallTest | Level1)
{
    SceneSessionManagerLite& sceneSessionManagerLite = SceneSessionManagerLite::GetInstance();
    ASSERT_NE(nullptr, &sceneSessionManagerLite);
    SessionVerificationInfo sessionVerificationInfo;
    auto ret = sceneSessionManagerLite.GetSessionVerificationInfo(0, sessionVerificationInfo);
    ASSERT_EQ(ret, WSError::WS_ERROR_INVALID_PERMISSION);
}
}
}
}