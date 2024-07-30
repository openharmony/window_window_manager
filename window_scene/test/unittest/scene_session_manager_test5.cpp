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
#include <regex>
#include <bundle_mgr_interface.h>
#include <bundlemgr/launcher_service.h>
#include "iremote_object_mocker.h"
#include "interfaces/include/ws_common.h"
#include "iremote_object_mocker.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/main_session.h"
#include "window_manager_agent.h"
#include "session_manager.h"
#include "zidl/window_manager_agent_interface.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
#include "context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class SceneSessionManagerTest5 : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp() override;

    void TearDown() override;

    static bool gestureNavigationEnabled_;

    static ProcessGestureNavigationEnabledChangeFunc callbackFunc_;
    static sptr<SceneSessionManager> ssm_;

private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

sptr<SceneSessionManager> SceneSessionManagerTest5::ssm_ = nullptr;
bool SceneSessionManagerTest5::gestureNavigationEnabled_ = true;

ProcessGestureNavigationEnabledChangeFunc SceneSessionManagerTest5::callbackFunc_ = [](bool enable) {
    gestureNavigationEnabled_ = enable;
};


void WindowChangedFuncTest(int32_t persistentId, WindowUpdateType type)
{
}

void ProcessStatusBarEnabledChangeFuncTest(bool enable)
{
}

void DumpRootSceneElementInfoFuncTest(const std::vector<std::string>& params, std::vector<std::string>& infos)
{
}

void SceneSessionManagerTest5::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerTest5::TearDownTestCase()
{
    ssm_->sceneSessionMap_.clear();
    ssm_ = nullptr;
}

void SceneSessionManagerTest5::SetUp()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerTest5::TearDown()
{
    ssm_->sceneSessionMap_.clear();
    usleep(WAIT_SYNC_IN_NS);
}

namespace {
/**
 * @tc.name: NotifySessionTouchOutside
 * @tc.desc: SceneSesionManager notify session touch outside
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, NotifySessionTouchOutside01, Function | SmallTest | Level3)
{
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(ssm_, nullptr);
    ssm_->recoveringFinished_ = false;
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(info, property);
    property->SetWindowType(WindowType::APP_WINDOW_BASE);
    ssm_->NotifySessionTouchOutside(0);
    property->SetPersistentId(1);
    ssm_->NotifySessionTouchOutside(1);
}

/**
 * @tc.name: DestroyAndDisconnectSpecificSessionInner
 * @tc.desc: check func DestroyAndDisconnectSpecificSessionInner
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest5, DestroyAndDisconnectSpecificSessionInner, Function | SmallTest | Level2)
{
    sptr<ISession> session;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    std::vector<int32_t> recoveredPersistentIds = {0, 1, 2};
    ssm_->SetAlivePersistentIds(recoveredPersistentIds);
    ProcessShiftFocusFunc shiftFocusFunc_;
    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ssm_->DestroyAndDisconnectSpecificSessionInner(1);
    property->SetPersistentId(1);
    ssm_->DestroyAndDisconnectSpecificSessionInner(1);

    property->SetWindowType(WindowType::WINDOW_TYPE_TOAST);
    ssm_->DestroyAndDisconnectSpecificSessionInner(1);
}


/**
 * @tc.name: DestroyAndDisconnectSpecificSessionWithDetachCallback
 * @tc.desc: SceneSesionManager destroy and disconnect specific session with detach callback
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest, DestroyAndDetachCallback, Function | SmallTest | Level3)
{
    int32_t persistentId = 0;
    ASSERT_NE(ssm_, nullptr);
    sptr<IRemoteObject> callback = new (std::nothrow) IRemoteObjectMocker();
    ASSERT_NE(callback, nullptr);
    ssm_->DestroyAndDisconnectSpecificSessionWithDetachCallback(persistentId, callback);
    sptr<WindowSessionProperty> property;
    ssm_->recoveringFinished_ = false;
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(info, property);
    ssm_->DestroyAndDisconnectSpecificSessionWithDetachCallback(persistentId, callback);
}

/**
 * @tc.name: GetStartupPageFromResource
 * @tc.desc: GetStartupPageFromResource
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, GetStartupPageFromResource, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    AppExecFwk::AbilityInfo info;
    info.startWindowBackgroundId = 1;
    std::string path = "path";
    uint32_t bgColor = 1;
    ASSERT_EQ(false, ssm_->GetStartupPageFromResource(info, path, bgColor));
    info.startWindowIconId = 0;
    ASSERT_EQ(false, ssm_->GetStartupPageFromResource(info, path, bgColor));
    info.hapPath = "hapPath";
    ASSERT_EQ(false, ssm_->GetStartupPageFromResource(info, path, bgColor));
}

/**
 * @tc.name: GetStartupPage
 * @tc.desc: GetStartupPage
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, GetStartupPage, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<AppExecFwk::IBundleMgr> bundleMgr_ = nullptr;
    std::string path = "path";
    uint32_t bgColor = 1;
    ssm_->GetStartupPage(info, path, bgColor);
}

/**
 * @tc.name: OnSCBSystemSessionBufferAvailable
 * @tc.desc: OnSCBSystemSessionBufferAvailable
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest5, OnSCBSystemSessionBufferAvailable02, Function | SmallTest | Level3)
{
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    ASSERT_NE(sceneSessionManager, nullptr);
    sceneSessionManager->OnSCBSystemSessionBufferAvailable(WindowType::WINDOW_TYPE_FLOAT);
    delete sceneSessionManager;
}

/**
 * @tc.name: CreateKeyboardPanelSession
 * @tc.desc: CreateKeyboardPanelSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest5, CreateKeyboardPanelSession, Function | SmallTest | Level3)
{
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    ASSERT_NE(sceneSessionManager, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    sceneSessionManager->CreateKeyboardPanelSession(sceneSession);
    delete sceneSessionManager;
}

/**
 * @tc.name: PrepareTerminate
 * @tc.desc: SceneSesionManager prepare terminate
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, PrepareTerminate, Function | SmallTest | Level3)
{
    int32_t persistentId = 1;
    bool isPrepareTerminate = true;
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    ASSERT_NE(sceneSessionManager, nullptr);
    ASSERT_EQ(WSError::WS_OK, sceneSessionManager->PrepareTerminate(persistentId, isPrepareTerminate));
    delete sceneSessionManager;
}

/**
 * @tc.name: IsKeyboardForeground
 * @tc.desc: IsKeyboardForeground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest5, IsKeyboardForeground, Function | SmallTest | Level3)
{
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    ASSERT_NE(sceneSessionManager, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    sceneSessionManager->IsKeyboardForeground();
    property->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW);
    sceneSessionManager->IsKeyboardForeground();
    delete sceneSessionManager;
}

/**
 * @tc.name: RequestInputMethodCloseKeyboard
 * @tc.desc: RequestInputMethodCloseKeyboard
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, RequestInputMethodCloseKeyboard02, Function | SmallTest | Level3)
{
    int32_t persistentId = -1;
    bool isPrepareTerminate = true;
    SceneSessionManager* sceneSessionManager = new SceneSessionManager();
    ASSERT_NE(sceneSessionManager, nullptr);
    sceneSessionManager->PrepareTerminate(persistentId, isPrepareTerminate);
    delete sceneSessionManager;
}

/**
 * @tc.name: UpdatePropertyRaiseEnabled
 * @tc.desc: UpdatePropertyRaiseEnabled
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, UpdatePropertyRaiseEnabled, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(info, property);
    ASSERT_NE(property, nullptr);
    auto result = ssm_->UpdatePropertyRaiseEnabled(property, sceneSession);
    ssm_->UpdatePropertyDragEnabled(property, sceneSession);
    ASSERT_EQ(result, WMError::WM_ERROR_NOT_SYSTEM_APP);
}

/**
 * @tc.name: HandleSpecificSystemBarProperty
 * @tc.desc: HandleSpecificSystemBarProperty
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, HandleSpecificSystemBarProperty, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    WindowType type = WindowType::WINDOW_TYPE_STATUS_BAR;
    ssm_->HandleSpecificSystemBarProperty(type, property, sceneSession);
}

/**
 * @tc.name: UpdateBrightness
 * @tc.desc: UpdateBrightness
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, UpdateBrightness, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    info.isSystem_ = false;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    sptr<SceneSession> sceneSession = ssm_->CreateSceneSession(info, property);
    ASSERT_NE(property, nullptr);
    ssm_->UpdateBrightness(1);
    FocusChangeInfo focusInfo;
    ssm_->GetCurrentUserId();
    ssm_->GetFocusWindowInfo(focusInfo);
}

/**
 * @tc.name: UpdateSecSurfaceInfo
 * @tc.desc: UpdateSecSurfaceInfo
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, UpdateSecSurfaceInfo, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    std::map<NodeId, std::vector<SecSurfaceInfo>> callbackData;
    std::shared_ptr<RSUIExtensionData> secExtData = std::make_shared<RSUIExtensionData>(callbackData);
    ssm_->currentUserId_ = 101;
    ssm_->UpdateSecSurfaceInfo(secExtData, 100);

    ssm_->currentUserId_ = 100;
    ssm_->UpdateSecSurfaceInfo(secExtData, 100);
}

/**
 * @tc.name: RegisterSessionSnapshotFunc
 * @tc.desc: RegisterSessionSnapshotFunc
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, RegisterSessionSnapshotFunc, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> scensession = nullptr;
    ssm_->RegisterSessionSnapshotFunc(scensession);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->RegisterSessionSnapshotFunc(scensession);
    info.isSystem_ = false;
    ssm_->RegisterSessionSnapshotFunc(scensession);
    std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo;
    ssm_->RegisterSessionSnapshotFunc(scensession);
}

/**
 * @tc.name: RequestAllAppSessionUnfocus
 * @tc.desc: RequestAllAppSessionUnfocus
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, RequestAllAppSessionUnfocus, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> scensession = nullptr;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(scensession, nullptr);
    ssm_->HandleHideNonSystemFloatingWindows(property, scensession);
    ssm_->RequestAllAppSessionUnfocus();
}

/**
 * @tc.name: RequestSessionFocus
 * @tc.desc: RequestSessionFocus
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, RequestSessionFocus, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> scensession = nullptr;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    property->SetFocusable(false);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    FocusChangeReason reason = FocusChangeReason::DEFAULT;
    ssm_->RequestSessionFocus(0, true, reason);
    ssm_->RequestSessionFocus(100, true, reason);
}

/**
 * @tc.name: SetShiftFocusListener
 * @tc.desc: SetShiftFocusListener
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, SetShiftFocusListener, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    FocusChangeReason reason = FocusChangeReason::SPLIT_SCREEN;
    sptr<SceneSession> scensession = nullptr;
    ssm_->ShiftFocus(scensession, reason);
    info.isSystem_ = true;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ProcessShiftFocusFunc fun;
    NotifySCBAfterUpdateFocusFunc func;
    ssm_->SetShiftFocusListener(fun);
    ssm_->SetSCBFocusedListener(func);
    ssm_->SetSCBUnfocusedListener(func);
    ProcessCallingSessionIdChangeFunc func1;
    ssm_->SetCallingSessionIdSessionListenser(func1);
    ProcessStartUIAbilityErrorFunc func2;
    ssm_->SetStartUIAbilityErrorListener(func2);
    ssm_->ShiftFocus(sceneSession, reason);
}

/**
 * @tc.name: UpdateFocusStatus
 * @tc.desc: UpdateFocusStatus
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, UpdateFocusStatus, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> scensession = nullptr;
    ssm_->UpdateFocusStatus(scensession, false);
    ssm_->UpdateFocusStatus(scensession, true);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->UpdateFocusStatus(sceneSession, true);
    ssm_->UpdateFocusStatus(sceneSession, false);
}

/**
 * @tc.name: RequestSessionUnfocus
 * @tc.desc: RequestSessionUnfocus
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, RequestSessionUnfocus, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> scensession = nullptr;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    FocusChangeReason reason = FocusChangeReason::MOVE_UP;
    ssm_->RequestSessionUnfocus(0, reason);
}

/**
 * @tc.name: RequestFocusSpecificCheck
 * @tc.desc: RequestFocusSpecificCheck
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, RequestFocusSpecificCheck, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> scensession = nullptr;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    FocusChangeReason reason = FocusChangeReason::MOVE_UP;
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ssm_->RequestFocusSpecificCheck(sceneSession, true, reason);

    FocusChangeReason reason1 = FocusChangeReason::SPLIT_SCREEN;
    property->SetWindowType(WindowType::APP_WINDOW_BASE);
    ssm_->RequestFocusSpecificCheck(sceneSession, true, reason1);
}

/**
 * @tc.name: NotifyFocusStatus
 * @tc.desc: NotifyFocusStatus
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, NotifyFocusStatus, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> scensession = nullptr;
    ssm_->NotifyFocusStatus(scensession, true);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->NotifyFocusStatus(sceneSession, false);
    info.isSystem_ = true;
    ssm_->NotifyFocusStatus(sceneSession, true);
}

/**
 * @tc.name: NotifyFocusStatusByMission
 * @tc.desc: NotifyFocusStatusByMission
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, NotifyFocusStatusByMission, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> scensession = nullptr;
    sptr<SceneSession> currSession = nullptr;
    ssm_->NotifyFocusStatusByMission(scensession, currSession);
    ASSERT_EQ(false, ssm_->MissionChanged(scensession, currSession));
    std::shared_ptr<SessionListenerController> listenerController =
        std::make_shared<SessionListenerController>();
    ssm_->listenerController_ = listenerController;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(scensession, nullptr);
    ssm_->NotifyFocusStatusByMission(scensession, currSession);
    ssm_->MissionChanged(scensession, currSession);
    currSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(currSession, nullptr);
    ssm_->NotifyFocusStatusByMission(scensession, currSession);
    ssm_->MissionChanged(scensession, currSession);
    info.isSystem_ = true;
    ssm_->NotifyFocusStatusByMission(scensession, currSession);
    ssm_->MissionChanged(scensession, currSession);
}

/**
 * @tc.name: UpdateFocus
 * @tc.desc: UpdateFocus
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, UpdateFocus, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> scensession = nullptr;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->UpdateFocus(1, true);
    ssm_->UpdateFocus(0, false);
}

/**
 * @tc.name: RequestSessionFocusImmediately
 * @tc.desc: RequestSessionFocusImmediately
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, RequestSessionFocusImmediately, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> scensession = nullptr;
    ssm_->RequestSessionFocusImmediately(0);
    ssm_->RequestFocusBasicCheck(0);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sptr<Session> session = new (std::nothrow) Session(info);
    session->SetSessionProperty(property);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->RequestSessionFocusImmediately(1);
    ssm_->RequestFocusBasicCheck(1);
}
/**
 * @tc.name: CheckFocusIsDownThroughBlockingType
 * @tc.desc: CheckFocusIsDownThroughBlockingType
 * @tc.type: FUNC
*/
HWTEST_F(SceneSessionManagerTest5, CheckFocusIsDownThroughBlockingType, Function | SmallTest | Level3)
{
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test1";
    SessionInfo info2;
    info2.abilityName_ = "test2";
    info2.bundleName_ = "test2";
    sptr<SceneSession> scensession = nullptr;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info2, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    sptr<SceneSession> sceneSession2 = new (std::nothrow) SceneSession(info2, nullptr);
    ASSERT_NE(sceneSession2, nullptr);
    ssm_->CheckFocusIsDownThroughBlockingType(sceneSession, sceneSession2, true);
}


/**
 * @tc.name: CheckFocusIsDownThroughBlockingType
 * @tc.desc: CheckFocusIsDownThroughBlockingType
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest5, CheckFocusIsDownThroughBlockingType01, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> requestSceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(requestSceneSession, nullptr);
    sptr<SceneSession> focusedSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(focusedSession, nullptr);
    bool includingAppSession = true;
    ssm_->CheckFocusIsDownThroughBlockingType(requestSceneSession, focusedSession, includingAppSession);

    requestSceneSession->SetZOrder(0);
    focusedSession->SetZOrder(1);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    Session session = Session(info);
    session.property_ = nullptr;
    session.SetZOrder(2);

    session.property_ = new WindowSessionProperty();
    session.isVisible_ = true;
    ssm_->CheckFocusIsDownThroughBlockingType(requestSceneSession, focusedSession, includingAppSession);
}

/**
 * @tc.name: CheckTopmostWindowFocus
 * @tc.desc: CheckTopmostWindowFocus
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest5, CheckTopmostWindowFocus, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";

    sptr<SceneSession> focusedSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(focusedSession, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    Session session = Session(info);
    session.property_ = nullptr;

    session.persistentId_ = 1;
    focusedSession->GetMissionId();
    ssm_->CheckTopmostWindowFocus(focusedSession, sceneSession);
}

/**
 * @tc.name: CheckRequestFocusImmdediately
 * @tc.desc: CheckRequestFocusImmdediately
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest5, CheckRequestFocusImmdediately, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    Session session = Session(info);
    session.property_ = nullptr;
    ssm_->CheckRequestFocusImmdediately(sceneSession);
}

/**
 * @tc.name: GetNextFocusableSession
 * @tc.desc: GetNextFocusableSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest5, GetNextFocusableSession, Function | SmallTest | Level3)
{
    int32_t persistentId = 0;
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->GetNextFocusableSession(persistentId);
    sceneSession->GetForceHideState();
    Session session = Session(info);
    session.property_ = new WindowSessionProperty();
    sptr<WindowSessionProperty> windowSessionProperty = new WindowSessionProperty();
    ASSERT_NE(windowSessionProperty, nullptr);
    ssm_->GetNextFocusableSession(persistentId);
}

/**
 * @tc.name: GetTopNearestBlockingFocusSession
 * @tc.desc: GetTopNearestBlockingFocusSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest5, GetTopNearestBlockingFocusSession, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";

    ssm_->GetTopNearestBlockingFocusSession(2, true);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);
    ssm_->GetTopNearestBlockingFocusSession(0, true);

    Session session = Session(info);
    session.property_ = nullptr;
    ssm_->GetTopNearestBlockingFocusSession(0, true);
    sptr<SceneSession> session_ = nullptr;
    ssm_->GetTopNearestBlockingFocusSession(0, true);
    session_ = new (std::nothrow) SceneSession(info, nullptr);
    ASSERT_NE(session_, nullptr);
    ssm_->GetTopNearestBlockingFocusSession(0, true);
}

/**
 * @tc.name: PreloadInLakeApp、UpdateSessionAvoidAreaListener
 * @tc.desc: PreloadInLakeApp、UpdateSessionAvoidAreaListener
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest5, PreloadInLakeApp, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    int32_t persistentId = 0;

    ssm_->PreloadInLakeApp("");
    sptr<SceneSession> sceneSession = nullptr;

    ssm_->UpdateSessionAvoidAreaListener(persistentId, true);
    sceneSession = ssm_->CreateSceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});
    ssm_->UpdateSessionAvoidAreaListener(persistentId, true);
}

/**
 * @tc.name: NotifyMMIWindowPidChange
 * @tc.desc: NotifyMMIWindowPidChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest5, NotifyMMIWindowPidChange, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    SessionInfo info;
    info.abilityName_ = "test1";
    info.bundleName_ = "test2";
    sptr<SceneSession> sceneSession = nullptr;
    ssm_->NotifyMMIWindowPidChange(0, true);
    sceneSession = ssm_->CreateSceneSession(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert({sceneSession->GetPersistentId(), sceneSession});
}
}
} // namespace Rosen
} // namespace OHOS
