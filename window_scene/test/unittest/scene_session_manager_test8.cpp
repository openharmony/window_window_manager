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
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "session_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class SceneSessionManagerTest8 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

void SceneSessionManagerTest8::SetUpTestCase()
{
}

void SceneSessionManagerTest8::TearDownTestCase()
{
}

void SceneSessionManagerTest8::SetUp()
{
    SceneSessionManager::GetInstance().sceneSessionMap_.clear();
}

void SceneSessionManagerTest8::TearDown()
{
    SceneSessionManager::GetInstance().sceneSessionMap_.clear();
    usleep(WAIT_SYNC_IN_NS);
}

namespace {
/**
 * @tc.name: GetTotalUITreeInfo
 * @tc.desc: GetTotalUITreeInfo set gesture navigation enabled
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, GetTotalUITreeInfo, Function | SmallTest | Level3)
{
    std::string strId = "1234";
    std::string dumpInfo = "dumpInfo";
    SceneSessionManager::GetInstance().SetDumpUITreeFunc(nullptr);
    EXPECT_EQ(WSError::WS_OK, SceneSessionManager::GetInstance()
        .GetTotalUITreeInfo(strId, dumpInfo));
    DumpUITreeFunc func = [](uint64_t, std::string& dumpInfo) {
        return;
    };
    SceneSessionManager::GetInstance().SetDumpUITreeFunc(func);
    EXPECT_EQ(WSError::WS_OK, SceneSessionManager::GetInstance()
        .GetTotalUITreeInfo(strId, dumpInfo));
}

/**
 * @tc.name: RequestFocusStatusBySCB
 * @tc.desc: RequestFocusStatusBySCB set gesture navigation enabled
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, RequestFocusStatusBySCB, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "RequestFocusStatusBySCB";
    sessionInfo.abilityName_ = "RequestFocusStatusBySCB";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    EXPECT_NE(nullptr, sceneSession);
    sceneSession->SetFocusable(true);

    FocusChangeReason reason = FocusChangeReason::DEFAULT;
    WMError res = SceneSessionManager::GetInstance()
        .RequestFocusStatusBySCB(8, false, false, reason);
    EXPECT_EQ(WMError::WM_OK, res);

    reason = FocusChangeReason::FOREGROUND;
    res = SceneSessionManager::GetInstance()
        .RequestFocusStatusBySCB(8, true, false, reason);
    EXPECT_EQ(WMError::WM_OK, res);

    reason = FocusChangeReason::MOVE_UP;
    SceneSessionManager::GetInstance().sceneSessionMap_.insert({ 5, sceneSession });
    res = SceneSessionManager::GetInstance()
        .RequestFocusStatusBySCB(5, true, false, reason);
    EXPECT_EQ(WMError::WM_OK, res);

    res = SceneSessionManager::GetInstance()
        .RequestFocusStatusBySCB(8, true, false, reason);
    EXPECT_EQ(WMError::WM_OK, res);

    reason = FocusChangeReason::DEFAULT;
    res = SceneSessionManager::GetInstance()
        .RequestFocusStatusBySCB(8, true, true, reason);
    EXPECT_EQ(WMError::WM_OK, res);

    res = SceneSessionManager::GetInstance()
        .RequestFocusStatusBySCB(5, true, true, reason);
    EXPECT_EQ(WMError::WM_OK, res);
}

/**
 * @tc.name: GetRemoteSessionSnapshotInfo
 * @tc.desc: GetRemoteSessionSnapshotInfo set gesture navigation enabled
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest8, GetRemoteSessionSnapshotInfo, Function | SmallTest | Level3)
{
    AAFwk::MissionSnapshot sessionSnapshot;
    std::string deviceId = "deviceId";
    int res = SceneSessionManager::GetInstance().GetRemoteSessionSnapshotInfo(
        deviceId, 8, sessionSnapshot);
    EXPECT_EQ(ERR_NULL_OBJECT, res);
}
}
}
}