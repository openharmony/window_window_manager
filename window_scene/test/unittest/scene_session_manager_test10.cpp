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

#include "interfaces/include/ws_common.h"
#include "iremote_object_mocker.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "session_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class SceneSessionManagerTest10 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<SceneSessionManager> ssm_;
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

sptr<SceneSessionManager> SceneSessionManagerTest10::ssm_ = nullptr;

void NotifyRecoverSceneSessionFuncTest(const sptr<SceneSession>& session, const SessionInfo& sessionInfo)
{
}

bool TraverseFuncTest(const sptr<SceneSession>& session)
{
    return true;
}

void WindowChangedFuncTest(int32_t persistentId, WindowUpdateType type)
{
}

void ProcessStatusBarEnabledChangeFuncTest(bool enable)
{
}

void DumpRootSceneElementInfoFuncTest(const std::vector<std::string>& params, std::vector<std::string>& infos)
{
}

void SceneSessionManagerTest10::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerTest10::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerTest10::SetUp()
{
}

void SceneSessionManagerTest10::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
}

namespace {
/**
 * @tc.name: UpdateRotateAnimationConfig
 * @tc.desc: UpdateRotateAnimationConfig
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, UpdateRotateAnimationConfig, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    RotateAnimationConfig config = { 400 };
    ssm_->UpdateRotateAnimationConfig(config);
    usleep(WAIT_SYNC_IN_NS);
    ASSERT_EQ(ssm_->rotateAnimationConfig_.duration_, 400);

    config.duration_ = 600;
    ssm_->UpdateRotateAnimationConfig(config);
    usleep(WAIT_SYNC_IN_NS);
    ASSERT_EQ(ssm_->rotateAnimationConfig_.duration_, 600);
}

/**
 * @tc.name: RegisterAcquireRotateAnimationConfigFunc
 * @tc.desc: RegisterAcquireRotateAnimationConfigFunc
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, RegisterAcquireRotateAnimationConfigFunc, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "bundleName";
    sessionInfo.persistentId_ = 1;
    sessionInfo.isSystem_ = false;
    sessionInfo.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    ASSERT_NE(sessionInfo.abilityInfo, nullptr);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->scenePersistence_ = sptr<ScenePersistence>::MakeSptr("bundleName", 1);
    ASSERT_NE(sceneSession->scenePersistence_, nullptr);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->RegisterAcquireRotateAnimationConfigFunc(sceneSession);
    WSRect rect({1, 1, 1, 1});
    SizeChangeReason reason = SizeChangeReason::ROTATION;
    WSError result = scensession->UpdateRect(rect, reason, "SceneSessionManagerTest10");
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: CheckLastFocusedAppSessionFocus
 * @tc.desc: CheckLastFocusedAppSessionFocus
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest10, CheckLastFocusedAppSessionFocus, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    SessionInfo info1;
    info1.abilityName_ = "focusedSession";
    info1.bundleName_ = "focusedSession";
    info1.windowType_ = 1;
    sptr<SceneSession> focusedSession = new (std::nothrow) SceneSession(info1, nullptr);
    ASSERT_NE(focusedSession, nullptr);
    
    SessionInfo info2;
    info2.abilityName_ = "nextSession";
    info2.bundleName_ = "nextSession";
    info2.windowType_ = 1;
    sptr<SceneSession> nextSession = new (std::nothrow) SceneSession(info2, nullptr);
    ASSERT_NE(nextSession, nullptr);
    
    ssm_->lastFocusedAppSessionId_ = nextSession->GetPersistentId();
    ASSERT_EQ(false, ssm_->CheckLastFocusedAppSessionFocus(focusedSession, nextSession));

    ssm_->lastFocusedAppSessionId_ = 124;
    focusedSession->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ASSERT_EQ(false, ssm_->CheckLastFocusedAppSessionFocus(focusedSession, nextSession));

    nextSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    ssm_->CheckLastFocusedAppSessionFocus(focusedSession, nextSession);
    ASSERT_EQ(0, ssm_->lastFocusedAppSessionId_);
}
}  // namespace
}
}