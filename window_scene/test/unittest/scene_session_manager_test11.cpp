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
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->SetSessionState(sessionState);
    sceneSession->SetRSVisible(isVisible);
    sceneSession->SetForegroundInteractiveStatus(isForegroundInteractive);
    sceneSession->GetSessionProperty()->SetIsPcAppInPad(isPcOrPadEnableActivation);
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
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert({1, sceneSession});
    ssm_->UpdateOccupiedAreaIfNeed(persistentId);

    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
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
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(info1, nullptr);
    ASSERT_NE(sceneSession1, nullptr);
    sceneSession1->UpdateNativeVisibility(true);

    SessionInfo info2;
    info2.abilityName_ = "GetAllSessionDumpDetailInfo2";
    info2.bundleName_ = "GetAllSessionDumpDetailInfo2";
    info2.persistentId_ = 2;
    sptr<SceneSession> sceneSession2 = sptr<SceneSession>::MakeSptr(info2, nullptr);
    ASSERT_NE(sceneSession2, nullptr);
    sceneSession2->UpdateNativeVisibility(false);

    ssm_->sceneSessionMap_.insert({0, nullptr});
    ssm_->sceneSessionMap_.insert({1, sceneSession1});
    ssm_->sceneSessionMap_.insert({2, sceneSession2});
    std::string dumpInfo;
    ASSERT_EQ(ssm_->GetAllSessionDumpDetailInfo(dumpInfo), WSError::WS_OK);
}

/**
 * @tc.name: GetAbilityInfo
 * @tc.desc: SceneSesionManager test GetAbilityInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, GetAbilityInfo, Function | SmallTest | Level1)
{
    ssm_->bundleMgr_ = nullptr;
    std::string bundleName = "bundleName";
    std::string moduleName = "moduleName";
    std::string abilityName = "abilityName";
    int32_t userId = 100;
    SCBAbilityInfo scbAbilityInfo;
    WSError ret = ssm_->GetAbilityInfo(bundleName, moduleName, abilityName, userId, scbAbilityInfo);
    ASSERT_EQ(ret, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: GetAbilityInfo02
 * @tc.desc: SceneSesionManager test GetAbilityInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, GetAbilityInfo02, Function | SmallTest | Level1)
{
    sptr<IBundleMgrMocker> bundleMgrMocker = sptr<IBundleMgrMocker>::MakeSptr();
    EXPECT_CALL(*bundleMgrMocker, GetBundleInfoV9(_, _, _, _)).WillOnce(Return(1));
    ssm_->bundleMgr_ = bundleMgrMocker;
    std::string bundleName = "bundleName";
    std::string moduleName = "moduleName";
    std::string abilityName = "abilityName";
    int32_t userId = 100;
    SCBAbilityInfo scbAbilityInfo;
    WSError ret = ssm_->GetAbilityInfo(bundleName, moduleName, abilityName, userId, scbAbilityInfo);
    ASSERT_EQ(ret, WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: GetAbilityInfo03
 * @tc.desc: SceneSesionManager test GetAbilityInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, GetAbilityInfo03, Function | SmallTest | Level1)
{
    sptr<IBundleMgrMocker> bundleMgrMocker = sptr<IBundleMgrMocker>::MakeSptr();
    EXPECT_CALL(*bundleMgrMocker, GetBundleInfoV9(_, _, _, _)).WillOnce([](
        const std::string& bundleName, int32_t flags, AppExecFwk::BundleInfo& bundleInfo, int32_t userId) {
        bundleInfo.hapModuleInfos = {};
        return 0;
    });
    ssm_->bundleMgr_ = bundleMgrMocker;
    std::string bundleName = "bundleName";
    std::string moduleName = "moduleName";
    std::string abilityName = "abilityName";
    int32_t userId = 100;
    SCBAbilityInfo scbAbilityInfo;
    WSError ret = ssm_->GetAbilityInfo(bundleName, moduleName, abilityName, userId, scbAbilityInfo);
    ASSERT_EQ(ret, WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: GetAbilityInfo04
 * @tc.desc: SceneSesionManager test GetAbilityInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, GetAbilityInfo04, Function | SmallTest | Level1)
{
    sptr<IBundleMgrMocker> bundleMgrMocker = sptr<IBundleMgrMocker>::MakeSptr();
    EXPECT_CALL(*bundleMgrMocker, GetBundleInfoV9(_, _, _, _)).WillOnce([](
        const std::string& bundleName, int32_t flags, AppExecFwk::BundleInfo& bundleInfo, int32_t userId) {
        AppExecFwk::AbilityInfo abilityInfo;
        abilityInfo.moduleName = "moduleName";
        abilityInfo.name = "abilityName";
        AppExecFwk::HapModuleInfo hapModuleInfo;
        hapModuleInfo.abilityInfos = { abilityInfo };
        bundleInfo.hapModuleInfos = { hapModuleInfo };
        bundleInfo.applicationInfo.codePath = "testCodePath";
        return 0;
    });
    ssm_->bundleMgr_ = bundleMgrMocker;
    std::string bundleName = "bundleName";
    std::string moduleName = "moduleName";
    std::string abilityName = "abilityName";
    int32_t userId = 100;
    SCBAbilityInfo scbAbilityInfo;
    WSError ret = ssm_->GetAbilityInfo(bundleName, moduleName, abilityName, userId, scbAbilityInfo);
    ASSERT_EQ(ret, WSError::WS_OK);
    ASSERT_EQ(scbAbilityInfo.codePath_, "testCodePath");
}

/**
 * @tc.name: GetAbilityInfo05
 * @tc.desc: SceneSesionManager test GetAbilityInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, GetAbilityInfo05, Function | SmallTest | Level1)
{
    sptr<IBundleMgrMocker> bundleMgrMocker = sptr<IBundleMgrMocker>::MakeSptr();
    EXPECT_CALL(*bundleMgrMocker, GetBundleInfoV9(_, _, _, _)).WillOnce([](
        const std::string& bundleName, int32_t flags, AppExecFwk::BundleInfo& bundleInfo, int32_t userId) {
        AppExecFwk::AbilityInfo abilityInfo;
        abilityInfo.moduleName = "moduleName2";
        abilityInfo.name = "abilityName2";
        AppExecFwk::HapModuleInfo hapModuleInfo;
        hapModuleInfo.abilityInfos = { abilityInfo };
        bundleInfo.hapModuleInfos = { hapModuleInfo };
        return 0;
    });
    ssm_->bundleMgr_ = bundleMgrMocker;
    std::string bundleName = "bundleName";
    std::string moduleName = "moduleName";
    std::string abilityName = "abilityName";
    int32_t userId = 100;
    SCBAbilityInfo scbAbilityInfo;
    WSError ret = ssm_->GetAbilityInfo(bundleName, moduleName, abilityName, userId, scbAbilityInfo);
    ASSERT_EQ(ret, WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: LockSessionByAbilityInfo
 * @tc.desc: SceneSesionManager test LockSessionByAbilityInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, LockSessionByAbilityInfo, Function | SmallTest | Level1)
{
    ASSERT_NE(ssm_, nullptr);
    AbilityInfoBase abilityInfo;
    abilityInfo.bundleName = "LockSessionByAbilityInfoBundle";
    abilityInfo.moduleName = "LockSessionByAbilityInfoModule";
    abilityInfo.abilityName = "LockSessionByAbilityInfoAbility";
    abilityInfo.appIndex = 0;

    auto result = ssm_->LockSessionByAbilityInfo(abilityInfo, true);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, result);
}

/**
 * @tc.name: NotifyWatchGestureConsumeResult
 * @tc.desc: SceneSesionManager test NotifyWatchGestureConsumeResult
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, NotifyWatchGestureConsumeResult, Function | SmallTest | Level1)
{
    ASSERT_NE(ssm_, nullptr);
    int32_t keyCode = 0;
    bool isConsumed = true;
    ssm_->onWatchGestureConsumeResultFunc_ = [](int32_t keyCode, bool isConsumed) {};
    auto ret = ssm_->NotifyWatchGestureConsumeResult(keyCode, isConsumed);
    ASSERT_EQ(ret, WMError::WM_OK);

    ssm_->onWatchGestureConsumeResultFunc_ = nullptr;
    ret = ssm_->NotifyWatchGestureConsumeResult(keyCode, isConsumed);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: NotifyWatchFocusActiveChange
 * @tc.desc: SceneSesionManager test NotifyWatchFocusActiveChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, NotifyWatchFocusActiveChange, Function | SmallTest | Level1)
{
    ASSERT_NE(ssm_, nullptr);
    bool isActive = true;
    ssm_->onWatchFocusActiveChangeFunc_ = [](bool isActive) {};
    auto ret = ssm_->NotifyWatchFocusActiveChange(isActive);
    ASSERT_EQ(ret, WMError::WM_OK);

    ssm_->onWatchFocusActiveChangeFunc_ = nullptr;
    ret = ssm_->NotifyWatchFocusActiveChange(isActive);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: NotifyNextAvoidRectInfo
 * @tc.desc: SceneSesionManager test NotifyNextAvoidRectInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, NotifyNextAvoidRectInfo, Function | SmallTest | Level1)
{
    ASSERT_NE(ssm_, nullptr);
    WSRect portraitRect = { 0, 0, 1260, 123 };
    WSRect landspaceRect = { 0, 0, 2720, 123 };
    auto ret = ssm_->NotifyNextAvoidRectInfo(AvoidAreaType::TYPE_SYSTEM, portraitRect, landspaceRect, 0);
    ASSERT_EQ(ret, WSError::WS_OK);
    std::pair<WSRect, WSRect> nextSystemBarAvoidAreaRectInfo;
    ret = ssm_->GetNextAvoidRectInfo(0, AvoidAreaType::TYPE_SYSTEM, nextSystemBarAvoidAreaRectInfo);
    ASSERT_EQ(ret, WSError::WS_OK);
    ret = ssm_->GetNextAvoidRectInfo(0, AvoidAreaType::TYPE_NAVIGATION_INDICATOR, nextSystemBarAvoidAreaRectInfo);
    ASSERT_EQ(ret, WSError::WS_DO_NOTHING);
}

/**
 * @tc.name: NotifyNextAvoidRectInfo_01
 * @tc.desc: SceneSesionManager test NotifyNextAvoidRectInfo_01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, NotifyNextAvoidRectInfo_01, Function | SmallTest | Level1)
{
    ASSERT_NE(ssm_, nullptr);
    WSRect portraitRect = { 0, 0, 1260, 123 };
    WSRect landspaceRect = { 0, 0, 2720, 123 };
    auto ret = ssm_->NotifyNextAvoidRectInfo(AvoidAreaType::TYPE_SYSTEM, portraitRect, landspaceRect, 0);
    ASSERT_EQ(ret, WSError::WS_OK);
    SessionInfo info;
    info.abilityName_ = "NotifyNextAvoidRectInfo";
    info.bundleName_ = "NotifyNextAvoidRectInfo";
    info.screenId_ = 0;
    auto specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    specificCb->onGetNextAvoidAreaRectInfo_ = [](
        DisplayId displayId, AvoidAreaType type, std::pair<WSRect, WSRect>& nextSystemBarAvoidAreaRectInfo) {
        return ssm_->GetNextAvoidRectInfo(displayId, type, nextSystemBarAvoidAreaRectInfo);
    };
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCb);
    sceneSession->property_->SetPersistentId(1);
    sceneSession->winRect_ = { 0, 0, 1260, 2720 };
    ssm_->sceneSessionMap_.insert({ 1, sceneSession });
    std::pair<WSRect, WSRect> nextSystemBarAvoidAreaRectInfo;
    ret = sceneSession->specificCallback_->onGetNextAvoidAreaRectInfo_(
        0, AvoidAreaType::TYPE_SYSTEM, nextSystemBarAvoidAreaRectInfo);
    ASSERT_EQ(ret, WSError::WS_OK);
    ASSERT_EQ(nextSystemBarAvoidAreaRectInfo.first, portraitRect);
    ASSERT_EQ(nextSystemBarAvoidAreaRectInfo.second, landspaceRect);
    ssm_->sceneSessionMap_.clear();
}

/**
 * @tc.name: NotifyNextAvoidRectInfo_statusBar
 * @tc.desc: SceneSesionManager test NotifyNextAvoidRectInfo_statusBar
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, NotifyNextAvoidRectInfo_statusBar, Function | SmallTest | Level1)
{
    ASSERT_NE(ssm_, nullptr);
    WSRect portraitRect = { 0, 0, 1260, 123 };
    WSRect landspaceRect = { 0, 0, 2720, 123 };
    auto ret = ssm_->NotifyNextAvoidRectInfo(AvoidAreaType::TYPE_SYSTEM, portraitRect, landspaceRect, 0);
    ASSERT_EQ(ret, WSError::WS_OK);
    SessionInfo info;
    info.abilityName_ = "NotifyNextAvoidRectInfo_statusBar";
    info.bundleName_ = "NotifyNextAvoidRectInfo_statusBar";
    info.screenId_ = 0;
    auto specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    specificCb->onGetNextAvoidAreaRectInfo_ = [](
        DisplayId displayId, AvoidAreaType type, std::pair<WSRect, WSRect>& nextSystemBarAvoidAreaRectInfo) {
        return ssm_->GetNextAvoidRectInfo(displayId, type, nextSystemBarAvoidAreaRectInfo);
    };
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCb);
    sceneSession->property_->SetPersistentId(1);
    sceneSession->winRect_ = { 0, 0, 1260, 2720 };
    ssm_->sceneSessionMap_.insert({ 1, sceneSession });
    std::map<WindowType, SystemBarProperty> properties;
    properties[WindowType::WINDOW_TYPE_STATUS_BAR] = SystemBarProperty();
    properties[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR] = SystemBarProperty();
    std::map<AvoidAreaType, AvoidArea> avoidAreas;
    sceneSession->GetAvoidAreasByRotation(0, { 0, 0, 1260, 2720 }, properties, avoidAreas);
    Rect rect = { 0, 0, 1260, 123 };
    ASSERT_EQ(avoidAreas[AvoidAreaType::TYPE_SYSTEM].topRect_, rect);
    properties[WindowType::WINDOW_TYPE_STATUS_BAR].enable_ = false;
    rect = { 0, 0, 0, 0 };
    sceneSession->GetAvoidAreasByRotation(0, { 0, 0, 1260, 123 }, properties, avoidAreas);
    ASSERT_EQ(avoidAreas[AvoidAreaType::TYPE_SYSTEM].topRect_, rect);
    ssm_->sceneSessionMap_.clear();
}

/**
 * @tc.name: NotifyNextAvoidRectInfo_statusBar_01
 * @tc.desc: SceneSesionManager test NotifyNextAvoidRectInfo_statusBar_01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, NotifyNextAvoidRectInfo_statusBar_01, Function | SmallTest | Level1)
{
    ASSERT_NE(ssm_, nullptr);
    WSRect portraitRect = { 0, 0, 1260, 123 };
    WSRect landspaceRect = { 0, 0, 2720, 123 };
    auto ret = ssm_->NotifyNextAvoidRectInfo(AvoidAreaType::TYPE_SYSTEM, portraitRect, landspaceRect, 0);
    ASSERT_EQ(ret, WSError::WS_OK);
    SessionInfo info;
    info.abilityName_ = "NotifyNextAvoidRectInfo_statusBar_01";
    info.bundleName_ = "NotifyNextAvoidRectInfo_statusBar_01";
    info.screenId_ = 0;
    auto specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    specificCb->onGetNextAvoidAreaRectInfo_ = [](
        DisplayId displayId, AvoidAreaType type, std::pair<WSRect, WSRect>& nextSystemBarAvoidAreaRectInfo) {
        return ssm_->GetNextAvoidRectInfo(displayId, type, nextSystemBarAvoidAreaRectInfo);
    };
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCb);
    sceneSession->property_->SetPersistentId(1);
    sceneSession->winRect_ = { 0, 0, 1260, 2720 };
    ssm_->sceneSessionMap_.insert({ 1, sceneSession });
    std::map<WindowType, SystemBarProperty> properties;
    properties[WindowType::WINDOW_TYPE_STATUS_BAR] = SystemBarProperty();
    properties[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR] = SystemBarProperty();
    std::map<AvoidAreaType, AvoidArea> avoidAreas;
    sceneSession->GetAvoidAreasByRotation(90, { 0, 0, 2720, 1260 }, properties, avoidAreas);
    Rect rect = { 0, 0, 2720, 123 };
    ASSERT_EQ(avoidAreas[AvoidAreaType::TYPE_SYSTEM].topRect_, rect);
    properties[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR].enable_ = false;
    rect = { 0, 0, 0, 0 };
    sceneSession->GetAvoidAreasByRotation(90, { 0, 0, 2720, 1260 }, properties, avoidAreas);
    ASSERT_EQ(avoidAreas[AvoidAreaType::TYPE_NAVIGATION_INDICATOR].bottomRect_, rect);
    ssm_->sceneSessionMap_.clear();
}

/**
 * @tc.name: NotifyNextAvoidRectInfo_keyboard
 * @tc.desc: SceneSesionManager test NotifyNextAvoidRectInfo_keyboard
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, NotifyNextAvoidRectInfo_keyboard, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "NotifyNextAvoidRectInfo_keyboard";
    info.bundleName_ = "NotifyNextAvoidRectInfo_keyboard";
    info.screenId_ = 0;
    auto specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    specificCb->onKeyboardRotationChange_ = [](int32_t persistentId, uint32_t rotation,
        std::vector<std::pair<bool, WSRect>>& avoidAreas) {
        ssm_->GetKeyboardOccupiedAreaWithRotation(persistentId, rotation, avoidAreas);
    };
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCb);
    sceneSession->property_->SetPersistentId(1);
    ssm_->sceneSessionMap_.insert({ 1, sceneSession });
    std::map<WindowType, SystemBarProperty> properties;
    properties[WindowType::WINDOW_TYPE_STATUS_BAR] = SystemBarProperty();
    properties[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR] = SystemBarProperty();
    AvoidArea avoidArea;
    sceneSession->GetKeyboardAvoidAreaByRotation(0, { 0, 0, 1260, 2720 }, avoidArea);
    Rect rect = { 0, 0, 0, 0 };
    ASSERT_EQ(avoidArea.bottomRect_, rect);
    ssm_->sceneSessionMap_.clear();
}

/**
 * @tc.name: NotifyNextAvoidRectInfo_keyboard_01
 * @tc.desc: SceneSesionManager test NotifyNextAvoidRectInfo_keyboard_01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, NotifyNextAvoidRectInfo_keyboard_01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "NotifyNextAvoidRectInfo_keyboard_01";
    info.bundleName_ = "NotifyNextAvoidRectInfo_keyboard_01";
    info.screenId_ = 0;
    auto specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    specificCb->onKeyboardRotationChange_ = [](int32_t persistentId, uint32_t rotation,
        std::vector<std::pair<bool, WSRect>>& avoidAreas) {
        ssm_->GetKeyboardOccupiedAreaWithRotation(persistentId, rotation, avoidAreas);
    };
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCb);
    sceneSession->property_->SetPersistentId(1);
    SessionInfo keyboardSessionInfo;
    info.abilityName_ = "keyboard";
    info.bundleName_ = "keyboard";
    info.screenId_ = 0;
    sptr<SceneSession> keyboardSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    keyboardSession->state_ = SessionState::STATE_FOREGROUND;
    keyboardSession->property_->type_ = WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT;
    keyboardSession->property_->keyboardLayoutParams_.PortraitPanelRect_ = { 0, 1700, 1260, 1020 };
    keyboardSession->property_->keyboardLayoutParams_.LandscapePanelRect_ = { 0, 538, 2720, 722 };
    keyboardSession->property_->SetPersistentId(2);
    ssm_->sceneSessionMap_.insert({ 1, sceneSession });
    ssm_->sceneSessionMap_.insert({ 2, keyboardSession });
    AvoidArea avoidArea;
    sceneSession->GetKeyboardAvoidAreaByRotation(0, { 0, 0, 1260, 2720 }, avoidArea);
    Rect rect = { 0, 1700, 1260, 1020 };
    ASSERT_EQ(avoidArea.bottomRect_, rect);
    sceneSession->GetKeyboardAvoidAreaByRotation(90, { 0, 0, 2720, 1260 }, avoidArea);
    rect = { 0, 538, 2720, 722 };
    ASSERT_EQ(avoidArea.bottomRect_, rect);
    keyboardSession->state_ = SessionState::STATE_BACKGROUND;
    sceneSession->GetKeyboardAvoidAreaByRotation(180, { 0, 0, 1260, 2720 }, avoidArea);
    rect = { 0, 0, 0, 0 };
    ASSERT_EQ(avoidArea.bottomRect_, rect);
    ssm_->sceneSessionMap_.clear();
}

/**
 * @tc.name: NotifyNextAvoidRectInfo_cutOut
 * @tc.desc: SceneSesionManager test NotifyNextAvoidRectInfo_cutOut
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, NotifyNextAvoidRectInfo_cutOut, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "NotifyNextAvoidRectInfo_cutOut";
    info.bundleName_ = "NotifyNextAvoidRectInfo_cutOut";
    info.screenId_ = 0;
    auto specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    specificCb->onKeyboardRotationChange_ = [](int32_t persistentId, uint32_t rotation,
        std::vector<std::pair<bool, WSRect>>& avoidAreas) {
        ssm_->GetKeyboardOccupiedAreaWithRotation(persistentId, rotation, avoidAreas);
    };
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCb);
    sceneSession->property_->SetPersistentId(1);
    ssm_->sceneSessionMap_.insert({ 1, sceneSession });
    std::map<WindowType, SystemBarProperty> properties;
    properties[WindowType::WINDOW_TYPE_STATUS_BAR] = SystemBarProperty();
    properties[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR] = SystemBarProperty();
    AvoidArea avoidArea;
    sceneSession->GetCutoutAvoidAreaByRotation(0, { 0, 0, 1260, 2720 }, avoidArea);
    Rect rect = { 494, 36, 273, 72 };
    ASSERT_EQ(avoidArea.topRect_, rect);
    sceneSession->GetCutoutAvoidAreaByRotation(90, { 0, 0, 2720, 1260 }, avoidArea);
    rect = { 2612, 494, 72, 273 };
    ASSERT_EQ(avoidArea.rightRect_, rect);
    sceneSession->GetCutoutAvoidAreaByRotation(270, { 0, 0, 2720, 1260 }, avoidArea);
    rect = { 36, 493, 72, 273 };
    ASSERT_EQ(avoidArea.leftRect_, rect);
    ssm_->sceneSessionMap_.clear();
}

/**
 * @tc.name: NotifyNextAvoidRectInfo_AIBar
 * @tc.desc: SceneSesionManager test NotifyNextAvoidRectInfo_AIBar
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, NotifyNextAvoidRectInfo_AIBar, Function | SmallTest | Level1)
{
    ASSERT_NE(ssm_, nullptr);
    WSRect portraitRect = { 409, 2629, 442, 91 };
    WSRect landspaceRect = { 884, 1169, 952, 91 };
    auto ret = ssm_->NotifyNextAvoidRectInfo(AvoidAreaType::TYPE_NAVIGATION_INDICATOR, portraitRect, landspaceRect, 0);
    ASSERT_EQ(ret, WSError::WS_OK);
    SessionInfo info;
    info.abilityName_ = "NotifyNextAvoidRectInfo_AIBar";
    info.bundleName_ = "NotifyNextAvoidRectInfo_AIBar";
    info.screenId_ = 0;
    auto specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    specificCb->onGetNextAvoidAreaRectInfo_ = [](
        DisplayId displayId, AvoidAreaType type, std::pair<WSRect, WSRect>& nextSystemBarAvoidAreaRectInfo) {
        return ssm_->GetNextAvoidRectInfo(displayId, type, nextSystemBarAvoidAreaRectInfo);
    };
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCb);
    sceneSession->property_->SetPersistentId(1);
    sceneSession->winRect_ = { 0, 0, 1260, 2720 };
    ssm_->sceneSessionMap_.insert({ 1, sceneSession });
    std::map<WindowType, SystemBarProperty> properties;
    properties[WindowType::WINDOW_TYPE_STATUS_BAR] = SystemBarProperty();
    properties[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR] = SystemBarProperty();
    std::map<AvoidAreaType, AvoidArea> avoidAreas;
    sceneSession->GetAvoidAreasByRotation(0, { 0, 0, 1260, 2720 }, properties, avoidAreas);
    Rect rect = { 409, 2629, 442, 91 };
    ASSERT_EQ(avoidAreas[AvoidAreaType::TYPE_NAVIGATION_INDICATOR].bottomRect_, rect);
    sceneSession->GetAvoidAreasByRotation(90, { 0, 0, 2720, 1260 }, properties, avoidAreas);
    rect = { 884, 1169, 952, 91 };
    ASSERT_EQ(avoidAreas[AvoidAreaType::TYPE_NAVIGATION_INDICATOR].bottomRect_, rect);
    sceneSession->GetAvoidAreasByRotation(180, { 0, 0, 1260, 2720 }, properties, avoidAreas);
    rect = { 409, 2629, 442, 91 };
    ASSERT_EQ(avoidAreas[AvoidAreaType::TYPE_NAVIGATION_INDICATOR].bottomRect_, rect);
    sceneSession->GetAvoidAreasByRotation(270, { 0, 0, 2720, 1260 }, properties, avoidAreas);
    rect = { 884, 1169, 952, 91 };
    ASSERT_EQ(avoidAreas[AvoidAreaType::TYPE_NAVIGATION_INDICATOR].bottomRect_, rect);
    properties[WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR].enable_ = false;
    rect = { 0, 0, 0, 0 };
    sceneSession->GetAvoidAreasByRotation(0, { 0, 0, 2720, 1260 }, properties, avoidAreas);
    ASSERT_EQ(avoidAreas[AvoidAreaType::TYPE_NAVIGATION_INDICATOR].topRect_, rect);
    ssm_->sceneSessionMap_.clear();
}

/**
 * @tc.name: DestroyUIServiceExtensionSubWindow
 * @tc.desc: SceneSesionManager test DestroyUIServiceExtensionSubWindow
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, DestroyUIServiceExtensionSubWindow, Function | SmallTest | Level1)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<SceneSession> sceneSession = nullptr;
    ssm_->DestroyUIServiceExtensionSubWindow(sceneSession);
    SessionInfo sessionInfo = { "bundleName", "moduleName", "abilityName" };
    sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ssm_->DestroyUIServiceExtensionSubWindow(sceneSession);

    sptr<WindowSessionProperty> property_ = sptr<WindowSessionProperty>::MakeSptr();
    property_->isUIExtFirstSubWindow_ = true;
    ASSERT_EQ(property_->isUIExtAnySubWindow_, false);
    ssm_->DestroyUIServiceExtensionSubWindow(sceneSession);
}

/**
 * @tc.name: FilterForGetAllWindowLayoutInfo
 * @tc.desc: SceneSesionManager test FilterForGetAllWindowLayoutInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, FilterForGetAllWindowLayoutInfo, Function | SmallTest | Level1)
{
    ASSERT_NE(ssm_, nullptr);
    DisplayId displayId = 0;
    bool isVirtualDisplay = true;
    std::vector<sptr<SceneSession>> filteredSessions{};
    SessionInfo info;
    info.bundleName_ = BUNDLE_NAME;
    info.appInstanceKey_ = "instanceKey";
    info.isNewAppInstance_ = true;
    sptr<SceneSession::SpecificSessionCallback> specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCb);
    ssm_->sceneSessionMap_.clear();
    auto ret = ssm_->sceneSessionMap_.size();
    ASSERT_EQ(ret, 0);
    ssm_->FilterForGetAllWindowLayoutInfo(displayId, isVirtualDisplay, filteredSessions);
    ssm_->sceneSessionMap_.insert({ 1, sceneSession });
    ssm_->FilterForGetAllWindowLayoutInfo(displayId, isVirtualDisplay, filteredSessions);
}

/**
 * @tc.name: ShiftAppWindowPointerEvent
 * @tc.desc: SceneSesionManager test ShiftAppWindowPointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, ShiftAppWindowPointerEvent, Function | SmallTest | Level1)
{
    ASSERT_NE(ssm_, nullptr);
    int32_t sourcePersistentId = 0;
    int32_t targetPersistentId = 0;
    ssm_->systemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    auto ret = ssm_->systemConfig_.IsPcWindow();
    ASSERT_EQ(ret, false);

    auto res = ssm_->ShiftAppWindowPointerEvent(sourcePersistentId, targetPersistentId);
    ASSERT_EQ(res, WMError::WM_ERROR_DEVICE_NOT_SUPPORT);

    ssm_->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ssm_->systemConfig_.freeMultiWindowEnable_ = true;
    ssm_->systemConfig_.freeMultiWindowSupport_ = true;
    res = ssm_->ShiftAppWindowPointerEvent(sourcePersistentId, targetPersistentId);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_CALLING);

    sourcePersistentId = 1;
    ssm_->sceneSessionMap_.clear();
    res = ssm_->ShiftAppWindowPointerEvent(sourcePersistentId, targetPersistentId);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_SESSION);
}

/**
 * @tc.name: HasFloatingWindowForeground
 * @tc.desc: SceneSesionManager test HasFloatingWindowForeground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, HasFloatingWindowForeground, Function | SmallTest | Level1)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<IRemoteObject> abilityToken = nullptr;
    bool hasOrNot = true;
    auto ret = ssm_->HasFloatingWindowForeground(abilityToken, hasOrNot);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: SetParentWindow
 * @tc.desc: SceneSesionManager test SetParentWindow
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, SetParentWindow, Function | SmallTest | Level1)
{
    ASSERT_NE(ssm_, nullptr);
    int32_t subWindowId = 1;
    int32_t newParentWindowId = 3;
    auto res = ssm_->SetParentWindow(subWindowId, newParentWindowId);
    EXPECT_EQ(res, WMError::WM_ERROR_INVALID_WINDOW);
    SessionInfo info;
    info.abilityName_ = "SetParentWindow";
    info.bundleName_ = "SetParentWindow";
    sptr<SceneSession> subSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    subSession->property_->SetPersistentId(1);
    subSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    ssm_->sceneSessionMap_.insert({ subSession->property_->GetPersistentId(), subSession });
    res = ssm_->SetParentWindow(subWindowId, newParentWindowId);
    EXPECT_EQ(res, WMError::WM_ERROR_INVALID_PARENT);

    sptr<SceneSession> oldParentSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    oldParentSession->property_->SetPersistentId(2);
    oldParentSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    ssm_->sceneSessionMap_.insert({ oldParentSession->property_->GetPersistentId(), oldParentSession });
    subSession->property_->SetParentPersistentId(2);
    subSession->SetParentSession(oldParentSession);
    res = ssm_->SetParentWindow(subWindowId, newParentWindowId);
    EXPECT_EQ(res, WMError::WM_ERROR_INVALID_PARENT);

    sptr<SceneSession> newParentSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    newParentSession->property_->SetPersistentId(3);
    newParentSession->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ssm_->sceneSessionMap_.insert({ newParentSession->property_->GetPersistentId(), newParentSession });
    res = ssm_->SetParentWindow(subWindowId, newParentWindowId);
    EXPECT_EQ(res, WMError::WM_ERROR_INVALID_PARENT);

    newParentSession->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    res = ssm_->SetParentWindow(subWindowId, newParentWindowId);
    EXPECT_EQ(res, WMError::WM_OK);
}
}  // namespace
}
}