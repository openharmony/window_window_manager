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
    std::string bundleName = "LockSessionByAbilityInfoBundle";
    std::string moduleName = "LockSessionByAbilityInfoModule";
    std::string abilityName = "LockSessionByAbilityInfoAbility";
    int32_t appIndex = 0;

    auto result = ssm_->LockSessionByAbilityInfo(bundleName, moduleName, abilityName, appIndex);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, result);
}

/**
 * @tc.name: UnlockSessionByAbilityInfo
 * @tc.desc: SceneSesionManager test UnlockSessionByAbilityInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest11, UnlockSessionByAbilityInfo, Function | SmallTest | Level1)
{
    ASSERT_NE(ssm_, nullptr);
    std::string bundleName = "UnlockSessionByAbilityInfoBundle";
    std::string moduleName = "UnlockSessionByAbilityInfoModule";
    std::string abilityName = "UnlockSessionByAbilityInfoAbility";
    int32_t appIndex = 0;

    auto result = ssm_->UnlockSessionByAbilityInfo(bundleName, moduleName, abilityName, appIndex);
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
}  // namespace
}
}