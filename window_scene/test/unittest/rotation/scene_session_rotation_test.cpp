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

#include "application_context.h"
#include "input_event.h"
#include "key_event.h"
#include "mock/mock_session_stage.h"
#include "pointer_event.h"

#include "session/host/include/main_session.h"
#include "session/host/include/keyboard_session.h"
#define PRIVATE public
#define PROTECTED public
#include "session/host/include/scene_session.h"
#undef PRIVATE
#undef PROTECTED
#include "session/host/include/sub_session.h"
#include "session/host/include/system_session.h"
#include "ui/rs_surface_node.h"
#include "window_helper.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SceneSessionRotationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SceneSessionRotationTest::SetUpTestCase() {}

void SceneSessionRotationTest::TearDownTestCase() {}

void SceneSessionRotationTest::SetUp() {}

void SceneSessionRotationTest::TearDown() {}

namespace {
/**
 * @tc.name: SetDefaultRequestedOrientation
 * @tc.desc: SetDefaultRequestedOrientation
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionRotationTest, SetDefaultRequestedOrientation, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetDefaultRequestedOrientation";
    info.bundleName_ = "SetDefaultRequestedOrientation";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    Orientation orientation = Orientation::AUTO_ROTATION_UNSPECIFIED;
    sceneSession->SetDefaultRequestedOrientation(orientation);
    Orientation ret = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(orientation, ret);

    orientation = Orientation::USER_ROTATION_PORTRAIT;
    sceneSession->SetDefaultRequestedOrientation(orientation);
    ret = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(orientation, ret);

    orientation = Orientation::USER_ROTATION_LANDSCAPE;
    sceneSession->SetDefaultRequestedOrientation(orientation);
    ret = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(orientation, ret);

    orientation = Orientation::USER_ROTATION_PORTRAIT_INVERTED;
    sceneSession->SetDefaultRequestedOrientation(orientation);
    ret = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(orientation, ret);

    orientation = Orientation::USER_ROTATION_LANDSCAPE_INVERTED;
    sceneSession->SetDefaultRequestedOrientation(orientation);
    ret = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(orientation, ret);

    orientation = Orientation::FOLLOW_DESKTOP;
    sceneSession->SetDefaultRequestedOrientation(orientation);
    ret = sceneSession->GetRequestedOrientation();
    ASSERT_EQ(orientation, ret);
}
} // namespace
} // namespace Rosen
} // namespace OHOS