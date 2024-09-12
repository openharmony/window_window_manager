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
#include <regex>
#include <bundle_mgr_interface.h>
#include <bundlemgr/launcher_service.h>
#include "interfaces/include/ws_common.h"
#include "screen_fold_data.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/main_session.h"
#include "window_manager_agent.h"
#include "session_manager.h"
#include "zidl/window_manager_agent_interface.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
#include "application_info.h"
#include "context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
const std::string EMPTY_DEVICE_ID = "";
}
class SceneSessionManagerTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp() override;

    void TearDown() override;

    static void SetVisibleForAccessibility(sptr<SceneSession>& sceneSession);
    int32_t GetTaskCount(sptr<SceneSession>& session);
    static sptr<SceneSessionManager> ssm_;
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

sptr<SceneSessionManager> SceneSessionManagerTest::ssm_ = nullptr;

void WindowChangedFuncTest(int32_t persistentId, WindowUpdateType type)
{
}

void ProcessStatusBarEnabledChangeFuncTest(bool enable)
{
}

void DumpRootSceneElementInfoFuncTest(const std::vector<std::string>& params, std::vector<std::string>& infos)
{
}

void SceneSessionManagerTest::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerTest::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerTest::SetUp()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerTest::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerTest::SetVisibleForAccessibility(sptr<SceneSession>& sceneSession)
{
    sceneSession->SetTouchable(true);
    sceneSession->forceTouchable_ = true;
    sceneSession->systemTouchable_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    sceneSession->foregroundInteractiveStatus_.store(true);
    sceneSession->isVisible_ = true;
}

int32_t SceneSessionManagerTest::GetTaskCount(sptr<SceneSession>& session)
{
    std::string dumpInfo = session->handler_->GetEventRunner()->GetEventQueue()->DumpCurrentQueueSize();
    std::regex pattern("\\d+");
    std::smatch matches;
    int32_t taskNum = 0;
    while (std::regex_search(dumpInfo, matches, pattern)) {
        taskNum += std::stoi(matches.str());
        dumpInfo = matches.suffix();
    }
    return taskNum;
}

namespace {
/**
 * @tc.name: SetBrightness
 * @tc.desc: ScreenSesionManager set session brightness
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, SetBrightness, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "SetBrightness";
    info.bundleName_ = "SetBrightness1";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    WSError result = ssm_->SetBrightness(sceneSession, 0.5);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: GerPrivacyBundleListTwoWindow
 * @tc.desc: get privacy bundle list when two windows exist.
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GerPrivacyBundleListTwoWindow, Function | SmallTest | Level3)
{
    SessionInfo sessionInfoFirst;
    sessionInfoFirst.bundleName_ = "privacy.test.first";
    sessionInfoFirst.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSessionFirst = ssm_->CreateSceneSession(sessionInfoFirst, nullptr);
    ASSERT_NE(sceneSessionFirst, nullptr);
    ssm_->sceneSessionMap_.insert({sceneSessionFirst->GetPersistentId(), sceneSessionFirst});

    SessionInfo sessionInfoSecond;
    sessionInfoSecond.bundleName_ = "privacy.test.second";
    sessionInfoSecond.abilityName_ = "privacyAbilityName";
    sptr<SceneSession> sceneSessionSecond = ssm_->CreateSceneSession(sessionInfoSecond, nullptr);
    ASSERT_NE(sceneSessionSecond, nullptr);
    ssm_->sceneSessionMap_.insert({sceneSessionSecond->GetPersistentId(), sceneSessionSecond});

    sceneSessionFirst->GetSessionProperty()->displayId_ = 0;
    sceneSessionFirst->GetSessionProperty()->isPrivacyMode_ = true;
    sceneSessionFirst->state_ = SessionState::STATE_FOREGROUND;

    sceneSessionSecond->GetSessionProperty()->displayId_ = 0;
    sceneSessionSecond->GetSessionProperty()->isPrivacyMode_ = true;
    sceneSessionSecond->state_ = SessionState::STATE_FOREGROUND;

    std::unordered_set<std::string> privacyBundleList;
    ssm_->GetSceneSessionPrivacyModeBundles(0, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 2);

    sceneSessionSecond->GetSessionProperty()->displayId_ = 1;
    privacyBundleList.clear();
    ssm_->GetSceneSessionPrivacyModeBundles(0, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 1);

    privacyBundleList.clear();
    ssm_->GetSceneSessionPrivacyModeBundles(1, privacyBundleList);
    EXPECT_EQ(privacyBundleList.size(), 1);
}

/**
 * @tc.name: SetWindowFlags
 * @tc.desc: SceneSesionManager set window flags
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetWindowFlags, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.bundleName_ = "bundleName";
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED);
    property->SetWindowFlags(flags);
    sptr<SceneSession> scensession = nullptr;
    WSError result01 = ssm_->SetWindowFlags(scensession, property);
    EXPECT_EQ(result01, WSError::WS_ERROR_NULLPTR);
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    WSError result02 = ssm_->SetWindowFlags(scensession, property);
    EXPECT_EQ(result02, WSError::WS_ERROR_NOT_SYSTEM_APP);
    property->SetSystemCalling(true);
    WSError result03 = ssm_->SetWindowFlags(scensession, property);
    ASSERT_EQ(result03, WSError::WS_OK);
}

/**
 * @tc.name: NotifyWaterMarkFlagChangedResult
 * @tc.desc: SceneSesionManager notify water mark flag changed result
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, NotifyWaterMarkFlagChangedResult, Function | SmallTest | Level3)
{
    int32_t persistentId = 10086;
    ssm_->NotifyCompleteFirstFrameDrawing(persistentId);
    bool hasWaterMark = true;
    AppExecFwk::AbilityInfo abilityInfo;
    WSError result01 = ssm_->NotifyWaterMarkFlagChangedResult(hasWaterMark);
    EXPECT_EQ(result01, WSError::WS_OK);
    ssm_->CheckAndNotifyWaterMarkChangedResult();
    ssm_->ProcessPreload(abilityInfo);
}

/**
 * @tc.name: IsValidSessionIds
 * @tc.desc: SceneSesionManager is valid session id
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, IsValidSessionIds, Function | SmallTest | Level3)
{
    std::vector<int32_t> sessionIds = {0, 1, 2, 3, 4, 5, 24, 10086};
    std::vector<bool> results = {};
    WSError result = ssm_->IsValidSessionIds(sessionIds, results);
    EXPECT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UnRegisterSessionListener
 * @tc.desc: SceneSesionManager un register session listener
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UnRegisterSessionListener, Function | SmallTest | Level3)
{
    OHOS::MessageParcel data;
    sptr<ISessionListener> listener = iface_cast<ISessionListener>(data.ReadRemoteObject());
    WSError result = ssm_->UnRegisterSessionListener(listener);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: GetSessionInfos
 * @tc.desc: SceneSesionManager get session infos
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetSessionInfos, Function | SmallTest | Level3)
{
    std::string deviceId = "1245";
    int32_t numMax = 1024;
    AAFwk::MissionInfo infoFrist;
    infoFrist.label = "fristBundleName";
    AAFwk::MissionInfo infoSecond;
    infoSecond.label = "secondBundleName";
    std::vector<SessionInfoBean> sessionInfos = {infoFrist, infoSecond};
    WSError result = ssm_->GetSessionInfos(deviceId, numMax, sessionInfos);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
    int32_t persistentId = 24;
    SessionInfoBean sessionInfo;
    int result01 = ssm_->GetRemoteSessionInfo(deviceId, persistentId, sessionInfo);
    ASSERT_NE(result01, ERR_OK);
}

/**
 * @tc.name: GetMainWindowStatesByPid
 * @tc.desc: SceneSesionManager get main window states
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, GetMainWindowStatesByPid, Function | SmallTest | Level3)
{
    int32_t pid = 100;
    std::vector<MainWindowState> windowStates;
    WSError result = ssm_->GetMainWindowStatesByPid(pid, windowStates);
    EXPECT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: CheckIsRemote
 * @tc.desc: SceneSesionManager check is remote
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, CheckIsRemote, Function | SmallTest | Level3)
{
    std::string deviceId;
    bool result = ssm_->CheckIsRemote(deviceId);
    EXPECT_FALSE(result);
    deviceId.assign("deviceId");
    result = ssm_->CheckIsRemote(deviceId);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: AnonymizeDeviceId
 * @tc.desc: SceneSesionManager anonymize deviceId
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AnonymizeDeviceId, Function | SmallTest | Level3)
{
    std::string deviceId;
    std::string result(ssm_->AnonymizeDeviceId(deviceId));
    EXPECT_EQ(result, EMPTY_DEVICE_ID);
    deviceId.assign("100964857");
    std::string result01 = "100964******";
    ASSERT_EQ(ssm_->AnonymizeDeviceId(deviceId), result01);
}

/**
 * @tc.name: TerminateSessionNew
 * @tc.desc: SceneSesionManager terminate session new
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, TerminateSessionNew, Function | SmallTest | Level3)
{
    sptr<AAFwk::SessionInfo> info = nullptr;
    bool needStartCaller = true;
    WSError result01 = ssm_->TerminateSessionNew(info, needStartCaller);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_PARAM, result01);
    info = new (std::nothrow) AAFwk::SessionInfo();
    WSError result02 = ssm_->TerminateSessionNew(info, needStartCaller);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_PARAM, result02);
}

/**
 * @tc.name: RegisterSessionListener01
 * @tc.desc: SceneSesionManager register session listener
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RegisterSessionListener01, Function | SmallTest | Level3)
{
    OHOS::MessageParcel data;
    sptr<ISessionListener> listener = iface_cast<ISessionListener>(data.ReadRemoteObject());
    WSError result = ssm_->RegisterSessionListener(listener);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: ClearDisplayStatusBarTemporarilyFlags
 * @tc.desc: check ClearDisplayStatusBarTemporarilyFlags
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, ClearDisplayStatusBarTemporarilyFlags, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "ClearDisplayStatusBarTemporarilyFlags";
    sessionInfo.abilityName_ = "ClearDisplayStatusBarTemporarilyFlags";
    sessionInfo.windowType_ = static_cast<uint32_t>(WindowType::APP_MAIN_WINDOW_BASE);
    sptr<SceneSession> sceneSession = ssm_->RequestSceneSession(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetIsDisplayStatusBarTemporarily(true);
    ASSERT_EQ(true, sceneSession->GetIsDisplayStatusBarTemporarily());
    ssm_->ClearDisplayStatusBarTemporarilyFlags();
    ASSERT_EQ(true, sceneSession->GetIsDisplayStatusBarTemporarily());
}

/**
 * @tc.name: RequestSceneSessionByCall
 * @tc.desc: SceneSesionManager request scene session by call
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, RequestSceneSessionByCall, Function | SmallTest | Level3)
{
    sptr<SceneSession> scensession = nullptr;
    WSError result01 = ssm_->RequestSceneSessionByCall(nullptr);
    EXPECT_EQ(result01, WSError::WS_OK);
    SessionInfo info;
    info.bundleName_ = "bundleName";
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    WSError result02 = ssm_->RequestSceneSessionByCall(scensession);
    ASSERT_EQ(result02, WSError::WS_OK);
}

/**
 * @tc.name: StartAbilityBySpecified
 * @tc.desc: SceneSesionManager start ability by specified
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, StartAbilityBySpecified, Function | SmallTest | Level3)
{
    int ret = 0;
    SessionInfo info;
    ssm_->StartAbilityBySpecified(info);

    std::shared_ptr<AAFwk::Want> want = std::make_shared<AAFwk::Want>();
    AAFwk::WantParams wantParams;
    want->SetParams(wantParams);
    info.want = want;
    ssm_->StartAbilityBySpecified(info);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: FindMainWindowWithToken
 * @tc.desc: SceneSesionManager find main window with token
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, FindMainWindowWithToken, Function | SmallTest | Level3)
{
    sptr<IRemoteObject> targetToken = nullptr;
    sptr<SceneSession> result = ssm_->FindMainWindowWithToken(targetToken);
    EXPECT_EQ(result, nullptr);
    uint64_t persistentId = 1423;
    WSError result01 = ssm_->BindDialogSessionTarget(persistentId, targetToken);
    EXPECT_EQ(result01, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: UpdateParentSessionForDialog001
 * @tc.desc: SceneSesionManager update parent session for dialog
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateParentSessionForDialog001, Function | SmallTest | Level3)
{
    SessionInfo dialogInfo;
    dialogInfo.abilityName_ = "DialogWindows";
    dialogInfo.bundleName_ = "DialogWindows";
    SessionInfo parentInfo;
    parentInfo.abilityName_ = "ParentWindows";
    parentInfo.bundleName_ = "ParentWindows";

    int32_t persistentId = 1005;
    sptr<SceneSession> parentSession = new (std::nothrow) MainSession(parentInfo, nullptr);
    EXPECT_NE(parentSession, nullptr);
    ssm_->sceneSessionMap_.insert({ persistentId, parentSession });

    sptr<SceneSession> dialogSession = new (std::nothrow) SystemSession(dialogInfo, nullptr);
    EXPECT_NE(dialogSession, nullptr);

    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetParentPersistentId(persistentId);
    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);

    WSError result = ssm_->UpdateParentSessionForDialog(dialogSession, property);
    EXPECT_EQ(dialogSession->GetParentPersistentId(), persistentId);
    EXPECT_NE(dialogSession->GetParentSession(), nullptr);
    EXPECT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: MoveSessionsToBackground
 * @tc.desc: SceneSesionManager move sessions to background
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, MoveSessionsToBackground, Function | SmallTest | Level3)
{
    int32_t type = CollaboratorType::RESERVE_TYPE;
    WSError result01 = ssm_->UnregisterIAbilityManagerCollaborator(type);
    EXPECT_EQ(result01, WSError::WS_ERROR_INVALID_PERMISSION);
    std::vector<std::int32_t> sessionIds = {1, 2, 3, 15, 1423};
    std::vector<int32_t> res = {1, 2, 3, 15, 1423};
    WSError result03 = ssm_->MoveSessionsToBackground(sessionIds, res);
    ASSERT_EQ(result03, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: MoveSessionsToForeground
 * @tc.desc: SceneSesionManager move sessions to foreground
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, MoveSessionsToForeground, Function | SmallTest | Level3)
{
    std::vector<std::int32_t> sessionIds = {1, 2, 3, 15, 1423};
    int32_t topSessionId = 1;
    WSError result = ssm_->MoveSessionsToForeground(sessionIds, topSessionId);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: UnlockSession
 * @tc.desc: SceneSesionManager unlock session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UnlockSession, Function | SmallTest | Level3)
{
    int32_t sessionId = 1;
    WSError result = ssm_->UnlockSession(sessionId);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
    result = ssm_->LockSession(sessionId);
    EXPECT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: GetImmersiveState
 * @tc.desc: test GetImmersiveState
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetImmersiveState, Function | SmallTest | Level3)
{
    int ret = 0;
    ssm_->GetImmersiveState();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: NotifyAINavigationBarShowStatus
 * @tc.desc: test NotifyAINavigationBarShowStatus
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, NotifyAINavigationBarShowStatus, Function | SmallTest | Level3)
{
    bool isVisible = false;
    WSRect barArea = { 0, 0, 320, 240}; // width: 320, height: 240
    uint64_t displayId = 0;
    WSError result = ssm_->NotifyAINavigationBarShowStatus(isVisible, barArea, displayId);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: NotifyWindowExtensionVisibilityChange
 * @tc.desc: test NotifyWindowExtensionVisibilityChange
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, NotifyWindowExtensionVisibilityChange, Function | SmallTest | Level3)
{
    int32_t pid = getprocpid();
    int32_t uid = getuid();
    bool isVisible = false;
    WSError result = ssm_->NotifyWindowExtensionVisibilityChange(pid, uid, isVisible);
    ASSERT_EQ(result, WSError::WS_OK);

    pid = INVALID_PID;
    uid = INVALID_USER_ID;
    result = ssm_->NotifyWindowExtensionVisibilityChange(pid, uid, isVisible);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: UpdateTopmostProperty
 * @tc.desc: test UpdateTopmostProperty
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateTopmostProperty, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "UpdateTopmostProperty";
    info.bundleName_ = "UpdateTopmostProperty";
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetTopmost(true);
    property->SetSystemCalling(true);
    sptr<SceneSession> scenesession = new (std::nothrow) MainSession(info, nullptr);
    scenesession->SetSessionProperty(property);
    WMError result = ssm_->UpdateTopmostProperty(property, scenesession);
    ASSERT_EQ(WMError::WM_OK, result);
}

/**
 * @tc.name: UpdateSessionWindowVisibilityListener
 * @tc.desc: SceneSesionManager update window visibility listener
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateSessionWindowVisibilityListener, Function | SmallTest | Level3)
{
    int32_t persistentId = 10086;
    bool haveListener = true;
    WSError result = ssm_->UpdateSessionWindowVisibilityListener(persistentId, haveListener);
    ASSERT_EQ(result, WSError::WS_DO_NOTHING);
}

/**
 * @tc.name: GetSessionSnapshotPixelMap
 * @tc.desc: SceneSesionManager get session snapshot pixelmap
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetSessionSnapshotPixelMap, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "GetPixelMap";
    info.bundleName_ = "GetPixelMap1";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);

    int32_t persistentId = 65535;
    float scaleValue = 0.5f;
    auto pixelMap = ssm_->GetSessionSnapshotPixelMap(persistentId, scaleValue);
    EXPECT_EQ(pixelMap, nullptr);

    persistentId = 1;
    pixelMap = ssm_->GetSessionSnapshotPixelMap(persistentId, scaleValue);
    EXPECT_EQ(pixelMap, nullptr);
}

/**
 * @tc.name: GetSessionSnapshotById
 * @tc.desc: test GetSessionSnapshotById
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetSessionSnapshotById, Function | SmallTest | Level3)
{
    int32_t persistentId = -1;
    SessionSnapshot snapshot;
    WMError ret = ssm_->GetSessionSnapshotById(persistentId, snapshot);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
}

/**
 * @tc.name: GetUIContentRemoteObj
 * @tc.desc: SceneSesionManager GetUIContentRemoteObj
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetUIContentRemoteObj, Function | SmallTest | Level3)
{
    sptr<IRemoteObject> remoteObj;
    EXPECT_EQ(ssm_->GetUIContentRemoteObj(65535, remoteObj), WSError::WS_ERROR_INVALID_PERMISSION);
    SessionInfo info;
    info.abilityName_ = "GetUIContentRemoteObj";
    info.bundleName_ = "GetUIContentRemoteObj";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->sceneSessionMap_.insert({65535, sceneSession});
    EXPECT_EQ(ssm_->GetUIContentRemoteObj(65535, remoteObj), WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: CalculateCombinedExtWindowFlags
 * @tc.desc: SceneSesionManager calculate combined extension window flags
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, CalculateCombinedExtWindowFlags, Function | SmallTest | Level3)
{
    EXPECT_EQ(ssm_->combinedExtWindowFlags_.bitData, 0);
    ssm_->UpdateSpecialExtWindowFlags(1234, ExtensionWindowFlags(3), ExtensionWindowFlags(3));
    ssm_->UpdateSpecialExtWindowFlags(5678, ExtensionWindowFlags(4), ExtensionWindowFlags(4));
    ssm_->CalculateCombinedExtWindowFlags();
    EXPECT_EQ(ssm_->combinedExtWindowFlags_.bitData, 7);
    ssm_->extWindowFlagsMap_.clear();
}

/**
 * @tc.name: UpdateSpecialExtWindowFlags
 * @tc.desc: SceneSesionManager update special extension window flags
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateSpecialExtWindowFlags, Function | SmallTest | Level3)
{
    int32_t persistentId = 12345;
    EXPECT_TRUE(ssm_->extWindowFlagsMap_.empty());
    ssm_->UpdateSpecialExtWindowFlags(persistentId, 3, 3);
    EXPECT_EQ(ssm_->extWindowFlagsMap_.size(), 1);
    EXPECT_EQ(ssm_->extWindowFlagsMap_.begin()->first, persistentId);
    EXPECT_EQ(ssm_->extWindowFlagsMap_.begin()->second.bitData, 3);
    ssm_->UpdateSpecialExtWindowFlags(persistentId, 0, 3);
    EXPECT_TRUE(ssm_->extWindowFlagsMap_.empty());
    ssm_->extWindowFlagsMap_.clear();
}

/**
 * @tc.name: HideNonSecureFloatingWindows
 * @tc.desc: SceneSesionManager hide non-secure floating windows
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, HideNonSecureFloatingWindows, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "HideNonSecureFloatingWindows";
    info.bundleName_ = "HideNonSecureFloatingWindows";

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    ssm_->sceneSessionMap_.insert(std::make_pair(sceneSession->GetPersistentId(), sceneSession));

    sptr<SceneSession> floatSession;
    floatSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(floatSession, nullptr);
    floatSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    ssm_->nonSystemFloatSceneSessionMap_.insert(std::make_pair(floatSession->GetPersistentId(), floatSession));

    EXPECT_FALSE(ssm_->shouldHideNonSecureFloatingWindows_.load());
    EXPECT_FALSE(floatSession->GetSessionProperty()->GetForceHide());
    sceneSession->combinedExtWindowFlags_.hideNonSecureWindowsFlag = true;
    ssm_->HideNonSecureFloatingWindows();
    EXPECT_TRUE(floatSession->GetSessionProperty()->GetForceHide());
    sceneSession->combinedExtWindowFlags_.hideNonSecureWindowsFlag = false;
    ssm_->combinedExtWindowFlags_.hideNonSecureWindowsFlag = true;
    ssm_->HideNonSecureFloatingWindows();
    EXPECT_TRUE(floatSession->GetSessionProperty()->GetForceHide());
    ssm_->shouldHideNonSecureFloatingWindows_.store(false);
    ssm_->sceneSessionMap_.clear();
    ssm_->nonSystemFloatSceneSessionMap_.clear();
}

/**
 * @tc.name: HideNonSecureSubWindows
 * @tc.desc: SceneSesionManager hide non-secure sub windows
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, HideNonSecureSubWindows, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "HideNonSecureSubWindows";
    info.bundleName_ = "HideNonSecureSubWindows";

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    sceneSession->combinedExtWindowFlags_.hideNonSecureWindowsFlag = true;

    sptr<SceneSession> subSession;
    subSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(subSession, nullptr);
    ASSERT_NE(subSession->GetSessionProperty(), nullptr);
    sceneSession->AddSubSession(subSession);
    subSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    subSession->GetSessionProperty()->SetParentPersistentId(sceneSession->GetPersistentId());
    ssm_->sceneSessionMap_.insert(std::make_pair(subSession->GetPersistentId(), subSession));

    EXPECT_FALSE(subSession->GetSessionProperty()->GetForceHide());
    sceneSession->combinedExtWindowFlags_.hideNonSecureWindowsFlag = true;
    ssm_->HideNonSecureSubWindows(sceneSession);
    EXPECT_TRUE(subSession->GetSessionProperty()->GetForceHide());
    ssm_->sceneSessionMap_.clear();
}

/**
 * @tc.name: HandleSecureSessionShouldHide
 * @tc.desc: SceneSesionManager handle secure session should hide
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, HandleSecureSessionShouldHide, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "HandleSecureSessionShouldHide";
    info.bundleName_ = "HandleSecureSessionShouldHide";

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    sceneSession->combinedExtWindowFlags_.hideNonSecureWindowsFlag = true;

    sptr<SceneSession> subSession;
    subSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(subSession, nullptr);
    ASSERT_NE(subSession->GetSessionProperty(), nullptr);
    sceneSession->AddSubSession(subSession);
    subSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    subSession->GetSessionProperty()->SetParentPersistentId(sceneSession->GetPersistentId());
    ssm_->sceneSessionMap_.insert(std::make_pair(subSession->GetPersistentId(), subSession));

    sptr<SceneSession> floatSession;
    floatSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(floatSession, nullptr);
    ASSERT_NE(floatSession->GetSessionProperty(), nullptr);
    floatSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    ssm_->nonSystemFloatSceneSessionMap_.insert(std::make_pair(floatSession->GetPersistentId(), floatSession));

    sceneSession->SetShouldHideNonSecureWindows(true);
    auto ret = ssm_->HandleSecureSessionShouldHide(sceneSession);
    EXPECT_EQ(ret, WSError::WS_OK);
    EXPECT_TRUE(subSession->GetSessionProperty()->GetForceHide());
    EXPECT_TRUE(floatSession->GetSessionProperty()->GetForceHide());
    EXPECT_TRUE(ssm_->shouldHideNonSecureFloatingWindows_.load());
    ssm_->sceneSessionMap_.clear();
    ssm_->nonSystemFloatSceneSessionMap_.clear();
}

/**
 * @tc.name: HandleSpecialExtWindowFlagsChange
 * @tc.desc: SceneSesionManager handle special uiextension window flags change
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, HandleSpecialExtWindowFlagsChange, Function | SmallTest | Level3)
{
    int32_t persistentId = 12345;
    EXPECT_TRUE(ssm_->extWindowFlagsMap_.empty());
    ssm_->HandleSpecialExtWindowFlagsChange(persistentId, 3, 3);
    EXPECT_EQ(ssm_->extWindowFlagsMap_.size(), 1);
    EXPECT_EQ(ssm_->extWindowFlagsMap_.begin()->first, persistentId);
    EXPECT_EQ(ssm_->extWindowFlagsMap_.begin()->second.bitData, 3);
    ssm_->HandleSpecialExtWindowFlagsChange(persistentId, 0, 3);
    EXPECT_TRUE(ssm_->extWindowFlagsMap_.empty());
    ssm_->extWindowFlagsMap_.clear();
}

/**
 * @tc.name: UpdateModalExtensionRect
 * @tc.desc: SceneSesionManager update modal extension rect
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateModalExtensionRect, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "UpdateModalExtensionRect";
    info.bundleName_ = "UpdateModalExtensionRect";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    Rect rect { 1, 2, 3, 4 };
    ssm_->UpdateModalExtensionRect(nullptr, rect);
    EXPECT_FALSE(sceneSession->HasModalUIExtension());
}

/**
 * @tc.name: ProcessModalExtensionPointDown
 * @tc.desc: SceneSesionManager process modal extension point down
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, ProcessModalExtensionPointDown, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "ProcessModalExtensionPointDown";
    info.bundleName_ = "ProcessModalExtensionPointDown";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    ssm_->ProcessModalExtensionPointDown(nullptr, 0, 0);
    EXPECT_FALSE(sceneSession->HasModalUIExtension());
}

/**
 * @tc.name: GetExtensionWindowIds
 * @tc.desc: SceneSesionManager get extension window ids
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetExtensionWindowIds, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "GetExtensionWindowIds";
    info.bundleName_ = "GetExtensionWindowIds";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    int32_t persistentId = 0;
    int32_t parentId = 0;
    EXPECT_FALSE(ssm_->GetExtensionWindowIds(nullptr, persistentId, parentId));
}

/**
 * @tc.name: AddOrRemoveSecureSession
 * @tc.desc: SceneSesionManager hide non-secure windows by scene session
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AddOrRemoveSecureSession, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "AddOrRemoveSecureSession";
    info.bundleName_ = "AddOrRemoveSecureSession1";

    int32_t persistentId = 12345;
    auto ret = ssm_->AddOrRemoveSecureSession(persistentId, true);
    EXPECT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: UpdateExtWindowFlags
 * @tc.desc: SceneSesionManager update uiextension window flags
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, UpdateExtWindowFlags, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "UpdateExtWindowFlags";
    info.bundleName_ = "UpdateExtWindowFlags";

    auto ret = ssm_->UpdateExtWindowFlags(nullptr, 7, 7);
    EXPECT_EQ(ret, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: SetScreenLocked001
 * @tc.desc: SetScreenLocked001
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetScreenLocked001, Function | SmallTest | Level3)
{
    sptr<SceneSession> sceneSession = nullptr;
    SessionInfo info;
    info.bundleName_ = "bundleName";
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetEventHandler(ssm_->taskScheduler_->GetEventHandler(), ssm_->eventHandler_);
    ssm_->sceneSessionMap_.insert(std::make_pair(sceneSession->GetPersistentId(), sceneSession));
    DetectTaskInfo detectTaskInfo;
    detectTaskInfo.taskState = DetectTaskState::ATTACH_TASK;
    detectTaskInfo.taskWindowMode = WindowMode::WINDOW_MODE_UNDEFINED;
    sceneSession->SetDetectTaskInfo(detectTaskInfo);
    std::string taskName = "wms:WindowStateDetect" + std::to_string(sceneSession->persistentId_);
    auto task = [](){};
    int64_t delayTime = 3000;
    sceneSession->handler_->PostTask(task, taskName, delayTime);
    int32_t beforeTaskNum = GetTaskCount(sceneSession);
    ssm_->SetScreenLocked(true);
    ASSERT_EQ(beforeTaskNum - 1, GetTaskCount(sceneSession));
    ASSERT_EQ(DetectTaskState::NO_TASK, sceneSession->detectTaskInfo_.taskState);
    ASSERT_EQ(WindowMode::WINDOW_MODE_UNDEFINED, sceneSession->detectTaskInfo_.taskWindowMode);
}

/**
 * @tc.name: AccessibilityFillEmptySceneSessionListToNotifyList
 * @tc.desc: SceneSesionManager fill empty scene session list to accessibilityList;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillEmptySceneSessionListToNotifyList, Function | SmallTest | Level3)
{
    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;

    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    EXPECT_EQ(accessibilityInfo.size(), 0);
}

/**
 * @tc.name: AccessibilityFillOneSceneSessionListToNotifyList
 * @tc.desc: SceneSesionManager fill one sceneSession to accessibilityList;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillOneSceneSessionListToNotifyList, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ASSERT_EQ(sceneSessionList.size(), 1);

    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);
}

/**
 * @tc.name: AccessibilityFillTwoSceneSessionListToNotifyList
 * @tc.desc: SceneSesionManager fill two sceneSessions to accessibilityList;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillTwoSceneSessionListToNotifyList, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSessionFirst = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSessionFirst, nullptr);
    SetVisibleForAccessibility(sceneSessionFirst);

    sptr<SceneSession> sceneSessionSecond = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSessionSecond, nullptr);
    SetVisibleForAccessibility(sceneSessionSecond);

    ssm_->sceneSessionMap_.insert({sceneSessionFirst->GetPersistentId(), sceneSessionFirst});
    ssm_->sceneSessionMap_.insert({sceneSessionSecond->GetPersistentId(), sceneSessionSecond});

    std::vector<sptr<SceneSession>> sceneSessionList;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ASSERT_EQ(sceneSessionList.size(), 2);

    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 2);
}

/**
 * @tc.name: AccessibilityFillEmptyBundleName
 * @tc.desc: SceneSesionManager fill empty bundle name to accessibilityInfo;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillEmptyBundleName, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ASSERT_EQ(sceneSessionList.size(), 1);

    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);

    ASSERT_EQ(accessibilityInfo.at(0)->bundleName_, "");
    ASSERT_EQ(sceneSessionList.at(0)->GetSessionInfo().bundleName_, "");
    ASSERT_EQ(accessibilityInfo.at(0)->bundleName_, sceneSessionList.at(0)->GetSessionInfo().bundleName_);
}

/**
 * @tc.name: AccessibilityFillBundleName
 * @tc.desc: SceneSesionManager fill bundle name to accessibilityInfo;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillBundleName, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ASSERT_EQ(sceneSessionList.size(), 1);

    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);

    ASSERT_EQ(accessibilityInfo.at(0)->bundleName_, "accessibilityNotifyTesterBundleName");
    ASSERT_EQ(sceneSessionList.at(0)->GetSessionInfo().bundleName_, "accessibilityNotifyTesterBundleName");
    ASSERT_EQ(accessibilityInfo.at(0)->bundleName_, sceneSessionList.at(0)->GetSessionInfo().bundleName_);
}

/**
 * @tc.name: AccessibilityFillFilterBundleName
 * @tc.desc: SceneSesionManager fill filter bundle name to accessibilityInfo;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillFilterBundleName, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SCBGestureTopBar";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ASSERT_EQ(sceneSessionList.size(), 0);

    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 0);
}

/**
 * @tc.name: AccessibilityFillEmptyHotAreas
 * @tc.desc: SceneSesionManager fill empty hot areas to accessibilityInfo;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillEmptyHotAreas, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;

    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);

    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.size(), sceneSessionList.at(0)->GetTouchHotAreas().size());
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.size(), 0);
}

/**
 * @tc.name: AccessibilityFillOneHotAreas
 * @tc.desc: SceneSesionManager fill one hot areas to accessibilityInfo;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillOneHotAreas, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    Rect rect = {100, 200, 100, 200};
    std::vector<Rect> hotAreas;
    hotAreas.push_back(rect);
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->SetTouchHotAreas(hotAreas);
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;

    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);

    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.size(), sceneSessionList.at(0)->GetTouchHotAreas().size());
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.size(), 1);

    ASSERT_EQ(rect.posX_, sceneSessionList.at(0)->GetTouchHotAreas().at(0).posX_);
    ASSERT_EQ(rect.posY_, sceneSessionList.at(0)->GetTouchHotAreas().at(0).posY_);
    ASSERT_EQ(rect.width_, sceneSessionList.at(0)->GetTouchHotAreas().at(0).width_);
    ASSERT_EQ(rect.height_, sceneSessionList.at(0)->GetTouchHotAreas().at(0).height_);

    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).posX_, rect.posX_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).posY_, rect.posY_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).width_, rect.width_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).height_, rect.height_);
}

/**
 * @tc.name: AccessibilityFillTwoHotAreas
 * @tc.desc: SceneSesionManager fill two hot areas to accessibilityInfo;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFillTwoHotAreas, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    std::vector<Rect> hotAreas;
    Rect rectFitst = {100, 200, 100, 200};
    Rect rectSecond = {50, 50, 20, 30};
    hotAreas.push_back(rectFitst);
    hotAreas.push_back(rectSecond);
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->SetTouchHotAreas(hotAreas);
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;

    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);

    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.size(), sceneSessionList.at(0)->GetTouchHotAreas().size());
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.size(), 2);

    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).posX_, rectFitst.posX_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).posY_, rectFitst.posY_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).width_, rectFitst.width_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(0).height_, rectFitst.height_);

    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(1).posX_, rectSecond.posX_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(1).posY_, rectSecond.posY_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(1).width_, rectSecond.width_);
    ASSERT_EQ(accessibilityInfo.at(0)->touchHotAreas_.at(1).height_, rectSecond.height_);
}

/**
 * @tc.name: AccessibilityFilterEmptySceneSessionList
 * @tc.desc: SceneSesionManager filter empty scene session list;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFilterEmptySceneSessionList, Function | SmallTest | Level3)
{
    std::vector<sptr<SceneSession>> sceneSessionList;

    ssm_->FilterSceneSessionCovered(sceneSessionList);
    ASSERT_EQ(sceneSessionList.size(), 0);
}

/**
 * @tc.name: AccessibilityFilterOneWindow
 * @tc.desc: SceneSesionManager filter one window;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFilterOneWindow, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->SetSessionRect({100, 100, 200, 200});
    SetVisibleForAccessibility(sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ssm_->FilterSceneSessionCovered(sceneSessionList);
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);
}

/**
 * @tc.name: AccessibilityFilterTwoWindowNotCovered
 * @tc.desc: SceneSesionManager filter two windows that not covered each other;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFilterTwoWindowNotCovered, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSessionFirst = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSessionFirst, nullptr);
    sceneSessionFirst->SetSessionRect({0, 0, 200, 200});
    SetVisibleForAccessibility(sceneSessionFirst);
    ssm_->sceneSessionMap_.insert({sceneSessionFirst->GetPersistentId(), sceneSessionFirst});

    sptr<SceneSession> sceneSessionSecond = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSessionSecond, nullptr);
    sceneSessionSecond->SetSessionRect({300, 300, 200, 200});
    SetVisibleForAccessibility(sceneSessionSecond);
    ssm_->sceneSessionMap_.insert({sceneSessionSecond->GetPersistentId(), sceneSessionSecond});

    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ssm_->FilterSceneSessionCovered(sceneSessionList);
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 2);
}

/**
 * @tc.name: AccessibilityFilterTwoWindowCovered
 * @tc.desc: SceneSesionManager filter two windows that covered each other;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, AccessibilityFilterTwoWindowCovered, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "accessibilityNotifyTesterBundleName";
    sessionInfo.abilityName_ = "accessibilityNotifyTesterAbilityName";

    sptr<SceneSession> sceneSessionFirst = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSessionFirst, nullptr);
    sceneSessionFirst->SetSessionRect({0, 0, 200, 200});
    SetVisibleForAccessibility(sceneSessionFirst);
    sceneSessionFirst->SetZOrder(20);
    ssm_->sceneSessionMap_.insert({sceneSessionFirst->GetPersistentId(), sceneSessionFirst});

    sptr<SceneSession> sceneSessionSecond = ssm_->CreateSceneSession(sessionInfo, nullptr);
    ASSERT_NE(sceneSessionSecond, nullptr);
    sceneSessionSecond->SetSessionRect({50, 50, 50, 50});
    SetVisibleForAccessibility(sceneSessionSecond);
    sceneSessionSecond->SetZOrder(10);
    ssm_->sceneSessionMap_.insert({sceneSessionSecond->GetPersistentId(), sceneSessionSecond});

    std::vector<sptr<SceneSession>> sceneSessionList;
    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    ssm_->GetAllSceneSessionForAccessibility(sceneSessionList);
    ssm_->FilterSceneSessionCovered(sceneSessionList);
    ssm_->FillAccessibilityInfo(sceneSessionList, accessibilityInfo);
    ASSERT_EQ(accessibilityInfo.size(), 1);
}

/**
 * @tc.name: GetMainWindowInfos
 * @tc.desc: SceneSesionManager get topN main window infos;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetMainWindowInfos, Function | SmallTest | Level3)
{
    int32_t topNum = 1024;
    std::vector<MainWindowInfo> topNInfos;
    auto result = ssm_->GetMainWindowInfos(topNum, topNInfos);
    EXPECT_EQ(result, WMError::WM_OK);

    topNum = 0;
    ssm_->GetMainWindowInfos(topNum, topNInfos);

    topNum = 1000;
    MainWindowInfo info;
    topNInfos.push_back(info);
    ssm_->GetMainWindowInfos(topNum, topNInfos);
}

/**
 * @tc.name: GetAllWindowVisibilityInfos
 * @tc.desc: SceneSesionManager get all window visibility infos;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetAllWindowVisibilityInfos, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->sceneSessionMap_.clear();
    SessionInfo info;
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});
    std::vector<std::pair<int32_t, uint32_t>> windowVisibilityInfos;
    ssm_->GetAllWindowVisibilityInfos(windowVisibilityInfos);
    EXPECT_NE(windowVisibilityInfos.size(), 0);
}

/**
 * @tc.name: TestNotifyEnterRecentTask
 * @tc.desc: Test whether the enterRecent_ is set correctly;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, TestNotifyEnterRecentTask, Function | SmallTest | Level3)
{
    GTEST_LOG_(INFO) << "SceneSessionManagerTest: TestNotifyEnterRecentTask start";
    sptr<SceneSessionManager> sceneSessionManager = new SceneSessionManager();
    ASSERT_NE(nullptr, sceneSessionManager);

    ASSERT_EQ(sceneSessionManager->NotifyEnterRecentTask(true), WSError::WS_OK);
    ASSERT_EQ(sceneSessionManager->enterRecent_.load(), true);
}

/**
 * @tc.name: TestIsEnablePiPCreate
 * @tc.desc: Test if pip window can be created;
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, TestIsEnablePiPCreate, Function | SmallTest | Level3)
{
    GTEST_LOG_(INFO) << "SceneSessionManagerTest: TestIsEnablePiPCreate start";
    ssm_->isScreenLocked_ = true;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_TRUE(!ssm_->isEnablePiPCreate(property));

    ssm_->isScreenLocked_ = false;
    Rect reqRect = { 0, 0, 0, 0 };
    property->SetRequestRect(reqRect);
    ASSERT_TRUE(!ssm_->isEnablePiPCreate(property));

    reqRect = { 0, 0, 10, 0 };
    property->SetRequestRect(reqRect);
    ASSERT_TRUE(!ssm_->isEnablePiPCreate(property));

    reqRect = { 0, 0, 10, 10 };
    property->SetRequestRect(reqRect);
    PiPTemplateInfo info = {0, 0, {}};
    property->SetPiPTemplateInfo(info);
    SessionInfo info1;
    info1.abilityName_ = "test1";
    info1.bundleName_ = "test2";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info1, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    property->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    sceneSession->pipTemplateInfo_ = {0, 100, {}};
    ssm_->sceneSessionMap_.insert({0, sceneSession});
    ASSERT_TRUE(!ssm_->isEnablePiPCreate(property));
    ssm_->sceneSessionMap_.clear();
    ASSERT_TRUE(!ssm_->isEnablePiPCreate(property));

    property->SetParentPersistentId(100);
    ASSERT_TRUE(!ssm_->isEnablePiPCreate(property));

    ssm_->sceneSessionMap_.insert({100, sceneSession});
    ASSERT_TRUE(!ssm_->isEnablePiPCreate(property));

    ssm_->sceneSessionMap_.clear();
    sceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    ssm_->sceneSessionMap_.insert({100, sceneSession});
    ASSERT_TRUE(ssm_->isEnablePiPCreate(property));
}

/**
 * @tc.name: GetAllMainWindowInfos001
 * @tc.desc: SceneSessionManager get all main window infos.
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetAllMainWindowInfos001, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test1";
    info.windowType_ = static_cast<uint32_t>(WindowType::APP_WINDOW_BASE);
    info.persistentId_ = 1;
    std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    AppExecFwk::ApplicationInfo applicationInfo;
    applicationInfo.bundleType = AppExecFwk::BundleType::ATOMIC_SERVICE;
    abilityInfo->applicationInfo = applicationInfo;
    info.abilityInfo = abilityInfo;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    if (sceneSession == nullptr) {
        return;
    }
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});
    std::vector<MainWindowInfo> infos;
    WMError result = ssm_->GetAllMainWindowInfos(infos);
    EXPECT_EQ(result, WMError::WM_OK);
    ssm_->sceneSessionMap_.erase(sceneSession->GetPersistentId());
}

/**
 * @tc.name: GetAllMainWindowInfos002
 * @tc.desc: SceneSessionManager get all main window infos, input params are not empty.
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetAllMainWindowInfos002, Function | SmallTest | Level3)
{
    std::vector<MainWindowInfo> infos;
    MainWindowInfo info;
    info.pid_ = 1000;
    info.bundleName_ = "test";
    infos.push_back(info);
    WMError result = ssm_->GetAllMainWindowInfos(infos);
    EXPECT_EQ(result, WMError::WM_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: GetUnreliableWindowInfo01
 * @tc.desc: SceneSesionManager get unreliable window info, windowId correct
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetUnreliableWindowInfo01, Function | SmallTest | Level3)
{
    ssm_->sceneSessionMap_.clear();
    SessionInfo info;
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    int32_t windowId = sceneSession->GetPersistentId();
    std::vector<sptr<UnreliableWindowInfo>> infos;
    WMError result = ssm_->GetUnreliableWindowInfo(windowId, infos);
    EXPECT_EQ(WMError::WM_OK, result);
    EXPECT_EQ(1, infos.size());
}

/**
 * @tc.name: GetUnreliableWindowInfo02
 * @tc.desc: SceneSesionManager get unreliable window info, toast window
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetUnreliableWindowInfo02, Function | SmallTest | Level3)
{
    ssm_->sceneSessionMap_.clear();
    SessionInfo info;
    info.windowType_ = 2107;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    property->SetWindowType(WindowType::WINDOW_TYPE_TOAST);
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(info, property);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetRSVisible(true);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    int32_t windowId = 0;
    std::vector<sptr<UnreliableWindowInfo>> infos;
    WMError result = ssm_->GetUnreliableWindowInfo(windowId, infos);
    EXPECT_EQ(WMError::WM_OK, result);
    EXPECT_EQ(1, infos.size());
}

/**
 * @tc.name: GetUnreliableWindowInfo03
 * @tc.desc: SceneSesionManager get unreliable window info, app sub window
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetUnreliableWindowInfo03, Function | SmallTest | Level3)
{
    ssm_->sceneSessionMap_.clear();
    SessionInfo info;
    info.windowType_ = 1000;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    property->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(info, property);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    SessionInfo info2;
    info2.windowType_ = 1001;
    sptr<WindowSessionProperty> property2 = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property2);
    property2->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    property2->SetParentId(sceneSession->GetPersistentId());
    sptr<SceneSession> sceneSession2 = ssm_->CreateSceneSession(info2, property2);
    ASSERT_NE(nullptr, sceneSession2);
    sceneSession2->SetRSVisible(true);
    ssm_->sceneSessionMap_.insert({sceneSession2->GetPersistentId(), sceneSession2});

    int32_t windowId = 0;
    std::vector<sptr<UnreliableWindowInfo>> infos;
    WMError result = ssm_->GetUnreliableWindowInfo(windowId, infos);
    EXPECT_EQ(WMError::WM_OK, result);
    EXPECT_EQ(1, infos.size());
}

/**
 * @tc.name: GetUnreliableWindowInfo04
 * @tc.desc: SceneSesionManager get unreliable window info, input method window
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetUnreliableWindowInfo04, Function | SmallTest | Level3)
{
    ssm_->sceneSessionMap_.clear();
    SessionInfo info;
    info.windowType_ = 2105;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(info, property);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetRSVisible(true);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});

    int32_t windowId = 0;
    std::vector<sptr<UnreliableWindowInfo>> infos;
    WMError result = ssm_->GetUnreliableWindowInfo(windowId, infos);
    EXPECT_EQ(WMError::WM_OK, result);
    EXPECT_EQ(1, infos.size());
}

/**
 * @tc.name: GetUnreliableWindowInfo05
 * @tc.desc: SceneSesionManager get unreliable window info, not correct window type, not visible
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetUnreliableWindowInfo05, Function | SmallTest | Level3)
{
    ssm_->sceneSessionMap_.clear();
    SessionInfo info;
    info.windowType_ = 2122;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(info, property);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetRSVisible(true);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});
    ssm_->sceneSessionMap_.insert({0, nullptr});

    int32_t windowId = 0;
    std::vector<sptr<UnreliableWindowInfo>> infos;
    WMError result = ssm_->GetUnreliableWindowInfo(windowId, infos);
    EXPECT_EQ(WMError::WM_OK, result);
    sceneSession->SetRSVisible(false);
    result = ssm_->GetUnreliableWindowInfo(windowId, infos);
    EXPECT_EQ(WMError::WM_OK, result);
    EXPECT_EQ(0, infos.size());
}

/**
 * @tc.name: GetUnreliableWindowInfo06
 * @tc.desc: SceneSesionManager satisfy FillUnreliableWindowInfo branches coverage
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, GetUnreliableWindowInfo06, Function | SmallTest | Level3)
{
    ssm_->sceneSessionMap_.clear();
    SessionInfo info1;
    info1.bundleName_ = "SCBGestureBack";
    sptr<SceneSession> sceneSession1 = ssm_->CreateSceneSession(info1, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    ssm_->sceneSessionMap_.insert({sceneSession1->GetPersistentId(), sceneSession1});

    SessionInfo info2;
    info2.bundleName_ = "SCBGestureNavBar";
    sptr<SceneSession> sceneSession2 = ssm_->CreateSceneSession(info2, nullptr);
    ASSERT_NE(nullptr, sceneSession2);
    ssm_->sceneSessionMap_.insert({sceneSession2->GetPersistentId(), sceneSession2});

    SessionInfo info3;
    info3.bundleName_ = "SCBGestureTopBar";
    sptr<SceneSession> sceneSession3 = ssm_->CreateSceneSession(info3, nullptr);
    ASSERT_NE(nullptr, sceneSession3);
    ssm_->sceneSessionMap_.insert({sceneSession3->GetPersistentId(), sceneSession3});

    std::vector<sptr<UnreliableWindowInfo>> infos;
    ssm_->GetUnreliableWindowInfo(sceneSession1->GetPersistentId(), infos);
    ssm_->GetUnreliableWindowInfo(sceneSession2->GetPersistentId(), infos);
    ssm_->GetUnreliableWindowInfo(sceneSession3->GetPersistentId(), infos);
    EXPECT_EQ(0, infos.size());
}

/**
 * @tc.name: SkipSnapshotForAppProcess
 * @tc.desc: add or cancel snapshot skip for app process
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, SkipSnapshotForAppProcess, Function | SmallTest | Level3)
{
    int32_t pid = 1000;
    bool skip = true;
    auto result = ssm_->SkipSnapshotForAppProcess(pid, skip);
    ASSERT_EQ(result, WMError::WM_OK);
    ASSERT_NE(ssm_->snapshotSkipPidSet_.find(pid), ssm_->snapshotSkipPidSet_.end());
    skip = false;
    result = ssm_->SkipSnapshotForAppProcess(pid, skip);
    ASSERT_EQ(result, WMError::WM_OK);
    ASSERT_EQ(ssm_->snapshotSkipPidSet_.find(pid), ssm_->snapshotSkipPidSet_.end());

    SessionInfo info;
    sptr<SceneSession> sceneSession1 = ssm_->CreateSceneSession(info, nullptr);
    sptr<SceneSession> sceneSession2 = ssm_->CreateSceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    ASSERT_NE(nullptr, sceneSession2);
    sceneSession1->SetCallingPid(1000);
    sceneSession2->SetCallingPid(1001);
    ssm_->sceneSessionMap_.insert({sceneSession1->GetPersistentId(), sceneSession1});
    ssm_->sceneSessionMap_.insert({sceneSession2->GetPersistentId(), sceneSession2});
    ssm_->sceneSessionMap_.insert({-1, nullptr});
    skip = true;
    result = ssm_->SkipSnapshotForAppProcess(pid, skip);
    ASSERT_EQ(result, WMError::WM_OK);
    skip = false;
    result = ssm_->SkipSnapshotForAppProcess(pid, skip);
    ASSERT_EQ(result, WMError::WM_OK);
    ssm_->sceneSessionMap_.erase(sceneSession1->GetPersistentId());
    ssm_->sceneSessionMap_.erase(sceneSession2->GetPersistentId());
    ssm_->sceneSessionMap_.erase(-1);
}

/**
 * @tc.name: RemoveProcessSnapshotSkip
 * @tc.desc: SceneSesionManager RemoveProcessSnapshotSkip
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, RemoveProcessSnapshotSkip, Function | SmallTest | Level3)
{
    ssm_->snapshotSkipPidSet_.insert(1);
    ssm_->RemoveProcessSnapshotSkip(1);
    ASSERT_EQ(ssm_->snapshotSkipPidSet_.find(1), ssm_->snapshotSkipPidSet_.end());
}

/**
 * @tc.name: TestReportCorrectScreenFoldStatusChangeEvent
 * @tc.desc: Test whether report the correct screen fold status events
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, TestReportCorrectScreenFoldStatusChangeEvent, Function | SmallTest | Level3)
{
    GTEST_LOG_(INFO) << "SceneSessionManagerTest: TestReportCorrectScreenFoldStatusChangeEvent start";
    ScreenFoldData screenFoldData1;
    screenFoldData1.currentScreenFoldStatus_ = 1; // 1: current screen fold status
    screenFoldData1.nextScreenFoldStatus_ = 3; // 3: next screen fold status
    screenFoldData1.currentScreenFoldStatusDuration_ = 18; // 18: current duration
    screenFoldData1.postureAngle_ = 47.1f; // 47.1: posture angle (type: float)
    screenFoldData1.screenRotation_ = 1; // 1: screen rotation
    screenFoldData1.typeCThermal_ = 3000; // 3000: typec port thermal
    screenFoldData1.focusedPackageName_ = "Developer Test: (1, 3, 18, 47.1, 1, 3000)";
    WMError result = ssm_->CheckAndReportScreenFoldStatus(screenFoldData1);
    ASSERT_EQ(result, WMError::WM_DO_NOTHING); // not report half-fold event until next change

    ScreenFoldData screenFoldData2;
    screenFoldData2.currentScreenFoldStatus_ = 3; // 3: current screen fold status
    screenFoldData2.nextScreenFoldStatus_ = 2; // 2: next screen fold status
    screenFoldData2.currentScreenFoldStatusDuration_ = 20; // 20: current duration
    screenFoldData2.postureAngle_ = 143.7f; // 143.7: posture angle (type: float)
    screenFoldData2.screenRotation_ = 2; // 2: screen rotation
    screenFoldData2.typeCThermal_ = 3005; // 3005: typec port thermal
    screenFoldData2.focusedPackageName_ = "Developer Test: (3, 2, 20, 143.7, 2, 3005)";
    result = ssm_->CheckAndReportScreenFoldStatus(screenFoldData2);
    ASSERT_EQ(result, WMError::WM_OK);
}

/**
 * @tc.name: TestReportIncompleteScreenFoldStatusChangeEvent
 * @tc.desc: Test whether block the incomplete screen fold status events
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, TestReportIncompleteScreenFoldStatusChangeEvent, Function | SmallTest | Level3)
{
    GTEST_LOG_(INFO) << "SceneSessionManagerTest: TestReportIncompleteScreenFoldStatusChangeEvent start";
    // screen fold status changes from -1: invalid to 3: half_fold, duration = 0, angle = 67.0, rotation = 0
    std::vector<std::string> screenFoldInfo {"-1", "3", "0", "67.0", "0"};
    WMError result = ssm_->ReportScreenFoldStatusChange(screenFoldInfo);
    ASSERT_EQ(result, WMError::WM_DO_NOTHING);

    screenFoldInfo.clear();
    result = ssm_->ReportScreenFoldStatusChange(screenFoldInfo);
    ASSERT_EQ(result, WMError::WM_DO_NOTHING);

    // screen fold status changes from 2: folded to 3: half_fold, duration = 0, angle = 67.0, rotation = 0
    screenFoldInfo = {"2", "3", "0", "67.0", "0"};
    result = ssm_->ReportScreenFoldStatusChange(screenFoldInfo);
    ASSERT_EQ(result, WMError::WM_DO_NOTHING);

    // screen fold status changes from 3: half_fold to 1: expand, duration = 18, angle = 147.3, rotation = 2
    screenFoldInfo = {"3", "1", "18", "147.3", "2"};
    result = ssm_->ReportScreenFoldStatusChange(screenFoldInfo);
    ASSERT_EQ(result, WMError::WM_DO_NOTHING);
}

/**
 * @tc.name: SetAppForceLandscapeConfig
 * @tc.desc: SceneSesionManager SetAppForceLandscapeConfig
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, SetAppForceLandscapeConfig, Function | SmallTest | Level3)
{
    std::string bundleName = "SetAppForceLandscapeConfig";
    AppForceLandscapeConfig config = { 0, "MainPage" };
    WSError result = ssm_->SetAppForceLandscapeConfig(bundleName, config);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: GetAppForceLandscapeConfig
 * @tc.desc: SceneSesionManager GetAppForceLandscapeConfig
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, GetAppForceLandscapeConfig, Function | SmallTest | Level3)
{
    std::string bundleName = "GetAppForceLandscapeConfig";
    AppForceLandscapeConfig config = ssm_->GetAppForceLandscapeConfig(bundleName);
    ASSERT_EQ(config.mode_, 0);
    ASSERT_EQ(config.homePage_, "");
}

/**
 * @tc.name: SetProcessWatermark
 * @tc.desc: add or cancel process watermark by pid
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, SetProcessWatermark, Function | SmallTest | Level3)
{
    auto result = ssm_->SetProcessWatermark(100, "", true);
    ASSERT_EQ(result, WMError::WM_ERROR_INVALID_PARAM);

    int32_t pid = 1000;
    const std::string pictureName = "SetProcessWatermarkPictureName";
    bool isEnabled = true;
    result = ssm_->SetProcessWatermark(pid, pictureName, isEnabled);
    ASSERT_EQ(result, WMError::WM_OK);
    ASSERT_NE(ssm_->processWatermarkPidMap_.find(pid), ssm_->processWatermarkPidMap_.end());
    isEnabled = false;
    result = ssm_->SetProcessWatermark(pid, pictureName, isEnabled);
    ASSERT_EQ(result, WMError::WM_OK);
    ASSERT_EQ(ssm_->processWatermarkPidMap_.find(pid), ssm_->processWatermarkPidMap_.end());

    SessionInfo info;
    sptr<SceneSession> sceneSession1 = ssm_->CreateSceneSession(info, nullptr);
    sptr<SceneSession> sceneSession2 = ssm_->CreateSceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    ASSERT_NE(nullptr, sceneSession2);
    sceneSession1->SetCallingPid(1000);
    sceneSession2->SetCallingPid(1001);
    ssm_->sceneSessionMap_.insert({sceneSession1->GetPersistentId(), sceneSession1});
    ssm_->sceneSessionMap_.insert({sceneSession2->GetPersistentId(), sceneSession2});
    ssm_->sceneSessionMap_.insert({-1, nullptr});
    isEnabled = true;
    result = ssm_->SetProcessWatermark(pid, pictureName, isEnabled);
    ASSERT_EQ(result, WMError::WM_OK);
    isEnabled = false;
    result = ssm_->SetProcessWatermark(pid, pictureName, isEnabled);
    ASSERT_EQ(result, WMError::WM_OK);
    ssm_->sceneSessionMap_.erase(sceneSession1->GetPersistentId());
    ssm_->sceneSessionMap_.erase(sceneSession2->GetPersistentId());
    ssm_->sceneSessionMap_.erase(-1);
}

/**
 * @tc.name: DeleteProcessWatermarkPid
 * @tc.desc: SceneSesionManager DeleteProcessWatermarkPid
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, DeleteProcessWatermarkPid, Function | SmallTest | Level3)
{
    ssm_->processWatermarkPidMap_.insert({1, "test"});
    ssm_->DeleteProcessWatermarkPid(1);
    ASSERT_EQ(ssm_->processWatermarkPidMap_.find(1), ssm_->processWatermarkPidMap_.end());
}

/**
 * @tc.name: DoAddProcessWatermarkForSession
 * @tc.desc: SceneSesionManager DoAddProcessWatermarkForSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, DoAddProcessWatermarkForSession, Function | SmallTest | Level3)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession1 = ssm_->CreateSceneSession(info, nullptr);
    sceneSession1->SetCallingPid(1);
    ssm_->DoAddProcessWatermarkForSession(sceneSession1->GetPersistentId());

    ssm_->sceneSessionMap_.insert({sceneSession1->GetPersistentId(), sceneSession1});
    ssm_->DoAddProcessWatermarkForSession(sceneSession1->GetPersistentId());

    ssm_->processWatermarkPidMap_.insert({1, "test"});
    ssm_->DoAddProcessWatermarkForSession(sceneSession1->GetPersistentId());

    ssm_->sceneSessionMap_.erase(sceneSession1->GetPersistentId());
    ssm_->processWatermarkPidMap_.erase(1);
}

/**
 * @tc.name: CloseTargetFloatWindow
 * @tc.desc: SceneSesionManager CloseTargetFloatWindow
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, CloseTargetFloatWindow, Function | SmallTest | Level3)
{
    std::string bundleName = "testClose";
    auto result = ssm_->CloseTargetFloatWindow(bundleName);
    ASSERT_EQ(result, WMError::WM_OK);
}

/**
 * @tc.name: CloseTargetPiPWindow
 * @tc.desc: SceneSesionManager CloseTargetPiPWindow
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, CloseTargetPiPWindow, Function | SmallTest | Level3)
{
    std::string bundleName = "CloseTargetPiPWindow";
    auto result = ssm_->CloseTargetPiPWindow(bundleName);
    ASSERT_EQ(result, WMError::WM_OK);
}

/**
 * @tc.name: GetCurrentPiPWindowInfo01
 * @tc.desc: SceneSesionManager GetCurrentPiPWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, GetCurrentPiPWindowInfo01, Function | SmallTest | Level3)
{
    std::string bundleName;
    auto result = ssm_->GetCurrentPiPWindowInfo(bundleName);
    ASSERT_EQ(result, WMError::WM_OK);
    ASSERT_EQ("", bundleName);
}

/**
 * @tc.name: GetCurrentPiPWindowInfo02
 * @tc.desc: SceneSesionManager GetCurrentPiPWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest, GetCurrentPiPWindowInfo02, Function | SmallTest | Level3)
{
    SessionInfo info1;
    info1.abilityName_ = "test1";
    info1.bundleName_ = "test1";
    info1.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_PIP);
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(info1, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    SessionInfo info2;
    info2.abilityName_ = "test2";
    info2.bundleName_ = "test2";
    info2.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_DIALOG);
    sptr<SceneSession> sceneSession2 = sptr<SceneSession>::MakeSptr(info2, nullptr);
    ASSERT_NE(nullptr, sceneSession2);

    ssm_->sceneSessionMap_.insert({sceneSession1->GetPersistentId(), sceneSession1});
    ssm_->sceneSessionMap_.insert({sceneSession2->GetPersistentId(), sceneSession2});
    std::string bundleName;
    auto result = ssm_->GetCurrentPiPWindowInfo(bundleName);
    ASSERT_EQ(result, WMError::WM_OK);
    ASSERT_EQ(info1.abilityName_, bundleName);
}
}
} // namespace Rosen
} // namespace OHOS
