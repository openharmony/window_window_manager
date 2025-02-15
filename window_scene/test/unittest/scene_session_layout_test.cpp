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
#include <pointer_event.h>
#include <ui/rs_surface_node.h>

#include "display_manager.h"
#include "input_event.h"
#include "key_event.h"
#include "mock/mock_session_stage.h"
#include "pointer_event.h"
#include "session/host/include/main_session.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/sub_session.h"
#include "session/host/include/system_session.h"
#include "window_helper.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SceneSessionLayoutTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    sptr<SessionStageMocker> mockSessionStage_ = nullptr;
};

void SceneSessionLayoutTest::SetUpTestCase()
{
}

void SceneSessionLayoutTest::TearDownTestCase()
{
}

void SceneSessionLayoutTest::SetUp()
{
    mockSessionStage_ = sptr<SessionStageMocker>::MakeSptr();
}

void SceneSessionLayoutTest::TearDown()
{
}

namespace {
/**
 * @tc.name: NotifySingleHandTransformChange
 * @tc.desc: NotifySingleHandTransformChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, NotifySingleHandTransformChange, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifySingleHandTransformChange";
    info.bundleName_ = "NotifySingleHandTransformChange";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    SingleHandTransform testTransform;
    sceneSession->state_ = SessionState::STATE_BACKGROUND;
    sceneSession->NotifySingleHandTransformChange(testTransform);

    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    mockSessionStage_ = nullptr;
    sceneSession->sessionStage_ = mockSessionStage_;
    sceneSession->NotifySingleHandTransformChange(testTransform);

    mockSessionStage_ = sptr<SessionStageMocker>::MakeSptr();
    sceneSession->sessionStage_ = mockSessionStage_;
    sceneSession->NotifySingleHandTransformChange(testTransform);
    ASSERT_NE(100, testTransform.posX);
}

/**
 * @tc.name: GetSessionGlobalRectWithSingleHandScale
 * @tc.desc: GetSessionGlobalRectWithSingleHandScale
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, GetSessionGlobalRectWithSingleHandScale, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "GetSessionGlobalRectWithSingleHandScale";
    info.bundleName_ = "GetSessionGlobalRectWithSingleHandScale";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    SingleHandTransform testTransform;
    WSRect globalRect = { 100, 100, 800, 800 };
    sceneSession->SetSessionGlobalRect(globalRect);
    sceneSession->SetSingleHandTransform(testTransform);
    ASSERT_EQ(sceneSession->GetSessionGlobalRect().posX_,
              sceneSession->GetSessionGlobalRectWithSingleHandScale().posX_);

    testTransform.posX = 10;
    testTransform.posY = 10;
    testTransform.scaleX = 0.75f;
    testTransform.scaleY = 0.75f;
    sceneSession->SetSingleHandTransform(testTransform);
    ASSERT_NE(sceneSession->GetSessionGlobalRect().posX_,
              sceneSession->GetSessionGlobalRectWithSingleHandScale().posX_);
}
} // namespace
} // Rosen
} // OHOS