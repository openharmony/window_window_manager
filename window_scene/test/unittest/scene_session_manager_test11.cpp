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

#include <bundlemgr/launcher_service.h>
#include "interfaces/include/ws_common.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "session_manager.h"
#include "session/host/include/scene_session.h"
#include "mock/mock_ibundle_mgr.h"
#include "common/include/task_scheduler.h"
#include "session/host/include/multi_instance_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    const std::string BUNDLE_NAME = "bundleName";
    const int32_t USER_ID { 100 };
    const int32_t SLEEP_TIME { 10000 };
}
class SceneSessionManagerTest11 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<SceneSessionManager> ssm_;
private:
    sptr<SceneSession> GetSceneSession(const std::string& instanceKey = "");
    void Init(AppExecFwk::MultiAppModeType modeType, uint32_t maxCount);
    std::shared_ptr<TaskScheduler> GetTaskScheduler();
};

sptr<SceneSessionManager> SceneSessionManagerTest11::ssm_ = nullptr;

void SceneSessionManagerTest11::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
    ssm_->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
}

void SceneSessionManagerTest11::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerTest11::SetUp()
{
}

void SceneSessionManagerTest11::TearDown()
{
    sptr<IBundleMgrMocker> bundleMgrMocker = sptr<IBundleMgrMocker>::MakeSptr();
    EXPECT_CALL(*bundleMgrMocker, GetApplicationInfo(_, _, _, _)).WillOnce(Return(false));
    MultiInstanceManager::GetInstance().Init(bundleMgrMocker, GetTaskScheduler());
    ssm_->RefreshAppInfo(BUNDLE_NAME);
    usleep(SLEEP_TIME);
}

sptr<SceneSession> SceneSessionManagerTest11::GetSceneSession(const std::string& instanceKey)
{
    SessionInfo info;
    info.bundleName_ = BUNDLE_NAME;
    info.appInstanceKey_ = instanceKey;
    info.isNewAppInstance_ = true;
    sptr<SceneSession::SpecificSessionCallback> specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCb);
    return sceneSession;
}

void SceneSessionManagerTest11::Init(AppExecFwk::MultiAppModeType modeType, uint32_t maxCount)
{
    sptr<IBundleMgrMocker> bundleMgrMocker = sptr<IBundleMgrMocker>::MakeSptr();
    EXPECT_CALL(*bundleMgrMocker, GetApplicationInfos(_, _, _)).WillOnce([modeType, maxCount](
        const AppExecFwk::ApplicationFlag flag, const int32_t userId,
        std::vector<AppExecFwk::ApplicationInfo>& appInfos) {
        AppExecFwk::ApplicationInfo appInfo;
        appInfo.bundleName = BUNDLE_NAME;
        appInfo.multiAppMode.multiAppModeType = modeType;
        appInfo.multiAppMode.maxCount = maxCount;
        appInfos.push_back(appInfo);
        return true;
    });
    MultiInstanceManager::GetInstance().Init(bundleMgrMocker, GetTaskScheduler());
    MultiInstanceManager::GetInstance().SetCurrentUserId(USER_ID);
    usleep(SLEEP_TIME);
}

std::shared_ptr<TaskScheduler> SceneSessionManagerTest11::GetTaskScheduler()
{
    std::string threadName = "threadName";
    std::shared_ptr<TaskScheduler> taskScheduler = std::make_shared<TaskScheduler>(threadName);
    return taskScheduler;
}

namespace {
/**
 * @tc.name: GetMainWindowStatesByPid
 * @tc.desc: SceneSesionManager get main window states by pid
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, GetMainWindowStatesByPid, Function | SmallTest | Level3)
{
    int32_t pid = 100;
    std::vector<MainWindowState> windowStates;
    WSError result = ssm_->GetMainWindowStatesByPid(pid, windowStates);
    EXPECT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: GetMainWindowStatesByPid02
 * @tc.desc: SceneSesionManager get main window states by pid
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, GetMainWindowStatesByPid02, Function | SmallTest | Level3)
{
    int32_t invalidPid = -1;
    std::vector<MainWindowState> windowStates;
    WSError result = ssm_->GetMainWindowStatesByPid(invalidPid, windowStates);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: GetMainWindowStatesByPid03
 * @tc.desc: SceneSesionManager get main window states by pid
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, GetMainWindowStatesByPid03, Function | SmallTest | Level3)
{
    SessionState sessionState = SessionState::STATE_FOREGROUND;
    bool isVisible = true;
    bool isForegroundInteractive = true;
    bool isPcOrPadEnableActivation = true;
    int32_t callingPid = 1001;
    SessionInfo sessionInfo;
    int32_t persistentId = 1005;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(sessionInfo, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->SetSessionState(sessionState);
    sceneSession->SetRSVisible(isVisible);
    sceneSession->SetForegroundInteractiveStatus(isForegroundInteractive);
    sceneSession->SetCallingPid(callingPid);
    ssm_->sceneSessionMap_.insert({ persistentId, sceneSession });
    std::vector<MainWindowState> windowStates;
    WSError result = ssm_->GetMainWindowStatesByPid(callingPid, windowStates);
    EXPECT_EQ(result, WSError::WS_OK);
    EXPECT_EQ(windowStates.size(), 1);
    EXPECT_EQ(windowStates[0].state_, static_cast<int32_t>(sessionState));
    EXPECT_EQ(windowStates[0].isVisible_, isVisible);
    EXPECT_EQ(windowStates[0].isForegroundInteractive_, isForegroundInteractive);
    EXPECT_EQ(windowStates[0].isPcOrPadEnableActivation_, isPcOrPadEnableActivation);
}

/**
 * @tc.name: GetMaxInstanceCount
 * @tc.desc: test function : GetMaxInstanceCount
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, GetMaxInstanceCount, Function | SmallTest | Level1)
{
    AppExecFwk::MultiAppModeType modeType = AppExecFwk::MultiAppModeType::MULTI_INSTANCE;
    uint32_t maxCount = 5;
    Init(modeType, maxCount);
    ASSERT_EQ(ssm_->GetMaxInstanceCount(BUNDLE_NAME), maxCount);
}

/**
 * @tc.name: GetInstanceCount
 * @tc.desc: test function : GetInstanceCount
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, GetInstanceCount, Function | SmallTest | Level1)
{
    AppExecFwk::MultiAppModeType modeType = AppExecFwk::MultiAppModeType::MULTI_INSTANCE;
    uint32_t maxCount = 5;
    Init(modeType, maxCount);
    ASSERT_EQ(ssm_->GetInstanceCount(BUNDLE_NAME), 0);
    std::string instanceKey0 = "app_instance_0";
    sptr<SceneSession> sceneSession = GetSceneSession(instanceKey0);
    ASSERT_EQ(MultiInstanceManager::GetInstance().CreateNewInstanceKey(BUNDLE_NAME), instanceKey0);
    MultiInstanceManager::GetInstance().IncreaseInstanceKeyRefCount(sceneSession);
    ASSERT_EQ(ssm_->GetInstanceCount(BUNDLE_NAME), 1);
    MultiInstanceManager::GetInstance().DecreaseInstanceKeyRefCount(sceneSession);
    ASSERT_EQ(ssm_->GetInstanceCount(BUNDLE_NAME), 0);
}

/**
 * @tc.name: GetLastInstanceKey
 * @tc.desc: test function : GetLastInstanceKey
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, GetLastInstanceKey, Function | SmallTest | Level1)
{
    AppExecFwk::MultiAppModeType modeType = AppExecFwk::MultiAppModeType::MULTI_INSTANCE;
    uint32_t maxCount = 5;
    Init(modeType, maxCount);
    ASSERT_EQ(ssm_->GetLastInstanceKey(BUNDLE_NAME), "");
    std::string instanceKey0 = "app_instance_0";
    sptr<SceneSession> sceneSession = GetSceneSession(instanceKey0);
    ASSERT_EQ(MultiInstanceManager::GetInstance().CreateNewInstanceKey(BUNDLE_NAME), instanceKey0);
    MultiInstanceManager::GetInstance().IncreaseInstanceKeyRefCount(sceneSession);
    ASSERT_EQ(ssm_->GetLastInstanceKey(BUNDLE_NAME), instanceKey0);
    MultiInstanceManager::GetInstance().DecreaseInstanceKeyRefCount(sceneSession);
    ASSERT_EQ(ssm_->GetLastInstanceKey(BUNDLE_NAME), "");
}

/**
 * @tc.name: UpdateOccupiedAreaIfNeed
 * @tc.desc: SceneSesionManager update occupiedArea
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, UpdateOccupiedAreaIfNeed, Function | SmallTest | Level1)
{
    int ret = 0;
    int32_t persistentId = 0;
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    info.moduleName_ = "test3";
    info.persistentId_ = 1;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->UpdateOccupiedAreaIfNeed(persistentId);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    sceneSession->SetSessionProperty(property);
    ssm_->UpdateOccupiedAreaIfNeed(persistentId);

    persistentId = 1;
    ssm_->UpdateOccupiedAreaIfNeed(persistentId);

    ssm_->sceneSessionMap_.erase(1);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: GetAllSessionDumpDetailInfo
 * @tc.desc: SceneSesionManager test GetAllSessionDumpDetailInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, GetAllSessionDumpDetailInfo, Function | SmallTest | Level1)
{
    SessionInfo info1;
    info1.abilityName_ = "GetAllSessionDumpDetailInfo1";
    info1.bundleName_ = "GetAllSessionDumpDetailInfo1";
    info1.persistentId_ = 1;
    sptr<SceneSession> sceneSession1 = new (std::nothrow) SceneSession(info1, nullptr);
    ASSERT_NE(sceneSession1, nullptr);
    sceneSession1->UpdateNativeVisibility(true);

    SessionInfo info2;
    info2.abilityName_ = "GetAllSessionDumpDetailInfo2";
    info2.bundleName_ = "GetAllSessionDumpDetailInfo2";
    info2.persistentId_ = 2;
    sptr<SceneSession> sceneSession2 = new (std::nothrow) SceneSession(info2, nullptr);
    ASSERT_NE(sceneSession2, nullptr);
    sceneSession2->UpdateNativeVisibility(false);

    ssm_->sceneSessionMap_.insert({0, nullptr});
    ssm_->sceneSessionMap_.insert({1, sceneSession1});
    ssm_->sceneSessionMap_.insert({2, sceneSession2});
    std::string dumpInfo;
    ASSERT_EQ(ssm_->GetAllSessionDumpDetailInfo(dumpInfo), WSError::WS_OK);
}

/**
 * @tc.name: GetWindowDisplayIds
 * @tc.desc: test function : GetWindowDisplayIds
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, GetWindowDisplayIds, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "test";
    info.bundleName_ = "test";
    sptr<SceneSession> sceneSession1 = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->sceneSessionMap_.insert({sceneSession1->GetPersistentId(), sceneSession1});
    sptr<SceneSession> sceneSession2 = new (std::nothrow) SceneSession(info, nullptr);
    ssm_->sceneSessionMap_.insert({sceneSession2->GetPersistentId(), sceneSession2});

    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    DisplayId displayId = 0;
    property->SetDisplayId(displayId);
    sceneSession1->SetSessionProperty(property);

    std::vector<uint64_t> windowIds = {1001, sceneSession1->GetPersistentId(), sceneSession2->GetPersistentId()};
    std::unordered_map<uint64_t, DisplayId> windowDisplayMap;
    ASSERT_EQ(ssm_->GetWindowDisplayIds(windowIds, windowDisplayMap), WMError::WM_OK);
}
}  // namespace
}
}