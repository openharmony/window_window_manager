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

class SceneSessionManagerTest9 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<SceneSessionManager> ssm_;
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

sptr<SceneSessionManager> SceneSessionManagerTest9::ssm_ = nullptr;

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

void SceneSessionManagerTest9::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerTest9::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerTest9::SetUp()
{
}

void SceneSessionManagerTest9::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
}

namespace {
/**
 * @tc.name: RequestSceneSessionDestructionInner
 * @tc.desc: RequestSceneSessionDestructionInner
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest6, RequestSceneSessionDestructionInner, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<SceneSession> scnSession;
    sptr<AAFwk::SessionInfo> scnSessionInfo;
    bool needRemoveSession = true;
    bool isForceClean = true;

    SessionInfo sessionInfo;
    sessionInfo.collaboratorType_ = CollaboratorType::RESERVE_TYPE;
    ssm_->RequestSceneSessionDestructionInner(scnSession, scnSessionInfo, needRemoveSession, isForceClean);

    needRemoveSession = false;
    isForceClean = false;
    sessionInfo.collaboratorType_ = CollaboratorType::DEFAULT_TYPE;
    sessionInfo.want = std::make_shared<AAFwk::Want>();
    ssm_->listenerController_ = std::make_shared<SessionListenerController>();
    ssm_->RequestSceneSessionDestructionInner(scnSession, scnSessionInfo, needRemoveSession, isForceClean);
}
}
}
}