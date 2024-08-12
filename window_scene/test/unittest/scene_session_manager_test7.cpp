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
namespace {
const std::string EMPTY_DEVICE_ID = "";
using ConfigItem = WindowSceneConfig::ConfigItem;
}
class SceneSessionManagerTest7 : public testing::Test {
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

sptr<SceneSessionManager> SceneSessionManagerTest7::ssm_ = nullptr;

bool SceneSessionManagerTest7::gestureNavigationEnabled_ = true;
ProcessGestureNavigationEnabledChangeFunc SceneSessionManagerTest7::callbackFunc_ = [](bool enable) {
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

void SceneSessionManagerTest7::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void SceneSessionManagerTest7::TearDownTestCase()
{
    ssm_ = nullptr;
}

void SceneSessionManagerTest7::SetUp()
{
    ssm_->sceneSessionMap_.clear();
}

void SceneSessionManagerTest7::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    ssm_->sceneSessionMap_.clear();
}

namespace {
/**
 * @tc.name: UpdateSessionWindowVisibilityListener
 * @tc.desc: UpdateSessionWindowVisibilityListener
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, UpdateSessionWindowVisibilityListener, Function | SmallTest | Level3)
{
    int32_t persistentId = 1;
    bool haveListener = true;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.clear();
    auto ret = ssm_->UpdateSessionWindowVisibilityListener(persistentId, haveListener);
    EXPECT_EQ(ret, WSError::WS_DO_NOTHING);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "UpdateSessionWindowVisibilityListener";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->callingPid_ = 65535;
    ssm_->sceneSessionMap_.insert(std::make_pair(persistentId, sceneSession));
    ret = ssm_->UpdateSessionWindowVisibilityListener(persistentId, haveListener);
    EXPECT_EQ(ret, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: ProcessVirtualPixelRatioChange
 * @tc.desc: ProcessVirtualPixelRatioChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, ProcessVirtualPixelRatioChange, Function | SmallTest | Level3)
{
    DisplayId defaultDisplayId = 0;
    sptr<DisplayInfo> displayInfo = nullptr;
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap;
    DisplayStateChangeType type = DisplayStateChangeType::BEFORE_SUSPEND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    displayInfo = sptr<DisplayInfo>::MakeSptr();
    ASSERT_NE(nullptr, displayInfo);
    ssm_->sceneSessionMap_.clear();
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest6";
    sessionInfo.abilityName_ = "UpdateAvoidArea";
    sessionInfo.isSystem_ = true;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    sessionInfo.isSystem_ = false;
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    sceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    sceneSession->SetSessionState(SessionState::STATE_INACTIVE);
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    sceneSession = nullptr;
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
}

/**
 * @tc.name: ProcessVirtualPixelRatioChange01
 * @tc.desc: ProcessVirtualPixelRatioChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, ProcessVirtualPixelRatioChange01, Function | SmallTest | Level3)
{
    DisplayId defaultDisplayId = 0;
    sptr<DisplayInfo> displayInfo = sptr<DisplayInfo>::MakeSptr();
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap;
    DisplayStateChangeType type = DisplayStateChangeType::BEFORE_SUSPEND;
    ASSERT_NE(nullptr, displayInfo);
    ASSERT_NE(nullptr, ssm_);
    ssm_->processVirtualPixelRatioChangeFunc_ = nullptr;
    displayInfo->SetVirtualPixelRatio(0.1f);
    displayInfo->SetDensityInCurResolution(0.1f);
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    ProcessVirtualPixelRatioChangeFunc func = [](float ratio, const OHOS::Rosen::Rect& rect) {};
    ssm_->SetVirtualPixelRatioChangeListener(func);
    ASSERT_NE(nullptr, ssm_->processVirtualPixelRatioChangeFunc_);
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    displayInfo->SetDensityInCurResolution(0.2f);
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    ssm_->processVirtualPixelRatioChangeFunc_ = nullptr;
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
}

/**
 * @tc.name: ProcessUpdateRotationChange
 * @tc.desc: ProcessUpdateRotationChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, ProcessUpdateRotationChange, Function | SmallTest | Level3)
{
    DisplayId defaultDisplayId = 0;
    sptr<DisplayInfo> displayInfo = nullptr;
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap;
    DisplayStateChangeType type = DisplayStateChangeType::BEFORE_SUSPEND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->ProcessUpdateRotationChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    displayInfo = sptr<DisplayInfo>::MakeSptr();
    ASSERT_NE(nullptr, displayInfo);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest6";
    sessionInfo.abilityName_ = "UpdateAvoidArea";
    sessionInfo.isSystem_ = true;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    sceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    ssm_->ProcessUpdateRotationChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);
    ssm_->ProcessUpdateRotationChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    sceneSession->SetSessionState(SessionState::STATE_INACTIVE);
    ssm_->ProcessUpdateRotationChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    WSRectF bounds = { 0, 0, 0, 0 };
    sceneSession->SetBounds(bounds);
    displayInfo->width_ = 0;
    displayInfo->height_ = 0;
    Rotation rotation = Rotation::ROTATION_0;
    sceneSession->SetRotation(rotation);
    displayInfo->SetRotation(rotation);
    ssm_->ProcessUpdateRotationChange(defaultDisplayId, displayInfo, displayInfoMap, type);
}

/**
 * @tc.name: FlushUIParams
 * @tc.desc: FlushUIParams
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, FlushUIParams, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "FlushUIParams";
    sessionInfo.screenId_ = 1;
    ScreenId screenId = 2;
    std::unordered_map<int32_t, SessionUIParam> uiParams;
    uiParams.clear();
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->FlushUIParams(screenId, std::move(uiParams));
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ssm_->FlushUIParams(screenId, std::move(uiParams));
}

/**
 * @tc.name: FlushUIParams01
 * @tc.desc: FlushUIParams
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, FlushUIParams01, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "FlushUIParams01";
    sessionInfo.screenId_ = 2;
    ScreenId screenId = 2;
    std::unordered_map<int32_t, SessionUIParam> uiParams;
    uiParams.clear();
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->persistentId_ = 1;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    SessionUIParam sessionUIParam;
    uiParams.insert(std::make_pair(1, sessionUIParam));
    ssm_->FlushUIParams(screenId, std::move(uiParams));
    sessionInfo.screenId_ = -1ULL;
    ssm_->FlushUIParams(screenId, std::move(uiParams));
}

/**
 * @tc.name: RegisterIAbilityManagerCollaborator
 * @tc.desc: RegisterIAbilityManagerCollaborator
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, RegisterIAbilityManagerCollaborator, Function | SmallTest | Level3)
{
    int32_t type = 0;
    sptr<AAFwk::IAbilityManagerCollaborator> impl = nullptr;
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->RegisterIAbilityManagerCollaborator(type, impl);
    EXPECT_EQ(ret, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: ProcessBackEvent
 * @tc.desc: ProcessBackEvent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, ProcessBackEvent, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "FlushUIParams01";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->focusedSessionId_ = 1;
    ssm_->needBlockNotifyFocusStatusUntilForeground_ = true;
    auto ret = ssm_->ProcessBackEvent();
    EXPECT_EQ(ret, WSError::WS_OK);
    ssm_->needBlockNotifyFocusStatusUntilForeground_ = false;
    sessionInfo.isSystem_ = true;
    ssm_->rootSceneProcessBackEventFunc_ = nullptr;
    ret = ssm_->ProcessBackEvent();
    EXPECT_EQ(ret, WSError::WS_OK);
    RootSceneProcessBackEventFunc func = [](){};
    ssm_->rootSceneProcessBackEventFunc_ = func;
    ASSERT_NE(nullptr, ssm_->rootSceneProcessBackEventFunc_);
    ret = ssm_->ProcessBackEvent();
    EXPECT_EQ(ret, WSError::WS_OK);
    sessionInfo.isSystem_ = false;
    ret = ssm_->ProcessBackEvent();
    EXPECT_EQ(ret, WSError::WS_OK);
    ssm_->rootSceneProcessBackEventFunc_ = nullptr;
    ret = ssm_->ProcessBackEvent();
    EXPECT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: DestroySpecificSession
 * @tc.desc: DestroySpecificSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, DestroySpecificSession, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->remoteObjectMap_.clear();
    sptr<IRemoteObject> remoteObject = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(nullptr, remoteObject);
    ssm_->DestroySpecificSession(remoteObject);
    ssm_->remoteObjectMap_.insert(std::make_pair(remoteObject, 1));
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "DestroySpecificSession";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->DestroySpecificSession(remoteObject);
    sceneSession = nullptr;
    ssm_->DestroySpecificSession(remoteObject);
}

/**
 * @tc.name: DestroyAndDisconnectSpecificSessionWithDetachCallback
 * @tc.desc: DestroyAndDisconnectSpecificSessionWithDetachCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, DestroyAndDisconnectSpecificSessionWithDetachCallback,
    Function | SmallTest | Level3)
{
    int32_t persistentId = 1;
    sptr<IRemoteObject> callback = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(nullptr, callback);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "DestroyAndDisconnectSpecificSessionWithDetachCallback";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetCallingUid(1);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    auto ret = ssm_->DestroyAndDisconnectSpecificSessionWithDetachCallback(persistentId, callback);
    EXPECT_EQ(ret, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: DestroyAndDisconnectSpecificSessionInner
 * @tc.desc: DestroyAndDisconnectSpecificSessionInner
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, DestroyAndDisconnectSpecificSessionInner, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "DestroyAndDisconnectSpecificSessionInner";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    sceneSession->SetParentPersistentId(2);
    SessionInfo sessionInfo1;
    sessionInfo1.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo1.abilityName_ = "ParentSceneSession";
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo1, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession1));
    auto ret = ssm_->DestroyAndDisconnectSpecificSessionInner(1);
    EXPECT_EQ(ret, WSError::WS_OK);
    sceneSession1 = nullptr;
    ret = ssm_->DestroyAndDisconnectSpecificSessionInner(2);
    EXPECT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: DestroyAndDisconnectSpecificSessionInner01
 * @tc.desc: DestroyAndDisconnectSpecificSessionInner
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, DestroyAndDisconnectSpecificSessionInner01, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "DestroyAndDisconnectSpecificSessionInner";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_TOAST);
    sceneSession->SetParentPersistentId(2);
    SessionInfo sessionInfo1;
    sessionInfo1.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo1.abilityName_ = "ParentSceneSession";
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo1, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession1));
    auto ret = ssm_->DestroyAndDisconnectSpecificSessionInner(1);
    EXPECT_EQ(ret, WSError::WS_OK);
    sceneSession1 = nullptr;
    ret = ssm_->DestroyAndDisconnectSpecificSessionInner(2);
    EXPECT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: DestroyAndDisconnectSpecificSessionInner02
 * @tc.desc: DestroyAndDisconnectSpecificSessionInner
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, DestroyAndDisconnectSpecificSessionInner02, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "DestroyAndDisconnectSpecificSessionInner";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    sceneSession->SetParentPersistentId(2);
    SessionInfo sessionInfo1;
    sessionInfo1.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo1.abilityName_ = "ParentSceneSession";
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo1, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession1));
    auto ret = ssm_->DestroyAndDisconnectSpecificSessionInner(1);
    EXPECT_EQ(ret, WSError::WS_OK);
    sceneSession1 = nullptr;
    ret = ssm_->DestroyAndDisconnectSpecificSessionInner(2);
    EXPECT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: CheckPiPPriority
 * @tc.desc: CheckPiPPriority
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, CheckPiPPriority, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "CheckPiPPriority";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    PiPTemplateInfo pipTemplateInfo;
    pipTemplateInfo.priority = 0;
    sceneSession->pipTemplateInfo_.priority = 1;
    sceneSession->isVisible_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    auto ret = ssm_->CheckPiPPriority(pipTemplateInfo);
    EXPECT_EQ(ret, false);
    sceneSession->isVisible_ = false;
    ret = ssm_->CheckPiPPriority(pipTemplateInfo);
    EXPECT_EQ(ret, true);
    pipTemplateInfo.priority = 1;
    ret = ssm_->CheckPiPPriority(pipTemplateInfo);
    EXPECT_EQ(ret, true);
    sceneSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    ret = ssm_->CheckPiPPriority(pipTemplateInfo);
    EXPECT_EQ(ret, true);
    sceneSession = nullptr;
    ret = ssm_->CheckPiPPriority(pipTemplateInfo);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: IsSessionVisibleForeground
 * @tc.desc: IsSessionVisibleForeground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, IsSessionVisibleForeground, Function | SmallTest | Level3)
{
    sptr<SceneSession> session = nullptr;
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->IsSessionVisibleForeground(session);
    EXPECT_EQ(ret, false);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "IsSessionVisibleForeground";
    session = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, session);
    session->isVisible_ = false;
    ret = ssm_->IsSessionVisibleForeground(session);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: GetAllSessionDumpInfo
 * @tc.desc: GetAllSessionDumpInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, GetAllSessionDumpInfo, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "GetAllSessionDumpInfo";
    sessionInfo.isSystem_ = false;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->state_ = SessionState::STATE_DISCONNECT;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    std::string dumpInfo = "";
    auto ret = ssm_->GetAllSessionDumpInfo(dumpInfo);
    EXPECT_EQ(ret, WSError::WS_OK);
    sceneSession->state_ = SessionState::STATE_END;
    ret = ssm_->GetAllSessionDumpInfo(dumpInfo);
    EXPECT_EQ(ret, WSError::WS_OK);
    sceneSession->state_ = SessionState::STATE_ACTIVE;
    ret = ssm_->GetAllSessionDumpInfo(dumpInfo);
    EXPECT_EQ(ret, WSError::WS_OK);
    sessionInfo.isSystem_ = true;
    ret = ssm_->GetAllSessionDumpInfo(dumpInfo);
    EXPECT_EQ(ret, WSError::WS_OK);
    sceneSession = nullptr;
    ret = ssm_->GetAllSessionDumpInfo(dumpInfo);
    EXPECT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: GetAllSessionDumpInfo01
 * @tc.desc: GetAllSessionDumpInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, GetAllSessionDumpInfo01, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "GetAllSessionDumpInfo01";
    sessionInfo.isSystem_ = true;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->isVisible_ = true;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    std::string dumpInfo = "";
    auto ret = ssm_->GetAllSessionDumpInfo(dumpInfo);
    EXPECT_EQ(ret, WSError::WS_OK);
    sceneSession->isVisible_ = false;
    ret = ssm_->GetAllSessionDumpInfo(dumpInfo);
    EXPECT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: UpdateNormalSessionAvoidArea
 * @tc.desc: UpdateNormalSessionAvoidArea
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, UpdateNormalSessionAvoidArea, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "UpdateNormalSessionAvoidArea";
    sessionInfo.isSystem_ = true;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->isVisible_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    int32_t persistentId = 1;
    bool needUpdate = true;
    ASSERT_NE(nullptr, ssm_);
    ssm_->avoidAreaListenerSessionSet_.clear();
    ssm_->UpdateNormalSessionAvoidArea(persistentId, sceneSession, needUpdate);
    sceneSession->isVisible_ = false;
    ssm_->UpdateNormalSessionAvoidArea(persistentId, sceneSession, needUpdate);
}

/**
 * @tc.name: UpdateNormalSessionAvoidArea01
 * @tc.desc: UpdateNormalSessionAvoidArea
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, UpdateNormalSessionAvoidArea01, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "UpdateNormalSessionAvoidArea01";
    sessionInfo.isSystem_ = true;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->isVisible_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    int32_t persistentId = 1;
    bool needUpdate = true;
    ASSERT_NE(nullptr, ssm_);
    ssm_->avoidAreaListenerSessionSet_.clear();
    ssm_->avoidAreaListenerSessionSet_.insert(persistentId);
    ssm_->UpdateNormalSessionAvoidArea(persistentId, sceneSession, needUpdate);
    sceneSession = nullptr;
    ssm_->UpdateNormalSessionAvoidArea(persistentId, sceneSession, needUpdate);
}

/**
 * @tc.name: UnregisterWindowManagerAgent
 * @tc.desc: UnregisterWindowManagerAgent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, UnregisterWindowManagerAgent, Function | SmallTest | Level3)
{
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_SYSTEM_BAR;
    sptr<IWindowManagerAgent> windowManagerAgent = nullptr;
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->UnregisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(ret, WMError::WM_ERROR_NULLPTR);
    type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_GESTURE_NAVIGATION_ENABLED;
    ret = ssm_->UnregisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(ret, WMError::WM_ERROR_NULLPTR);
    type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WATER_MARK_FLAG;
    ret = ssm_->UnregisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(ret, WMError::WM_ERROR_NULLPTR);
    type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_VISIBILITY;
    ret = ssm_->UnregisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(ret, WMError::WM_ERROR_INVALID_PERMISSION);
    type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_DRAWING_STATE;
    ret = ssm_->UnregisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(ret, WMError::WM_ERROR_INVALID_PERMISSION);
    type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_VISIBLE_WINDOW_NUM;
    ret = ssm_->UnregisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(ret, WMError::WM_ERROR_INVALID_PERMISSION);
    type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    ret = ssm_->UnregisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(ret, WMError::WM_ERROR_INVALID_PERMISSION);
    type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_MODE;
    ret = ssm_->UnregisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(ret, WMError::WM_ERROR_INVALID_PERMISSION);
    type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_UPDATE;
    ret = ssm_->UnregisterWindowManagerAgent(type, windowManagerAgent);
    EXPECT_EQ(ret, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: FindSessionByAffinity
 * @tc.desc: FindSessionByAffinity
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, FindSessionByAffinity, Function | SmallTest | Level3)
{
    std::string affinity = "";
    ASSERT_NE(nullptr, ssm_);
    auto ret = ssm_->FindSessionByAffinity(affinity);
    EXPECT_EQ(ret, nullptr);
    affinity = "Test";
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "FindSessionByAffinity";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetCollaboratorType(CollaboratorType::DEFAULT_TYPE);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ret = ssm_->FindSessionByAffinity(affinity);
    EXPECT_EQ(ret, nullptr);
    sceneSession->SetCollaboratorType(CollaboratorType::OTHERS_TYPE);
    sceneSession->sessionInfo_.sessionAffinity = "Test";
    ret = ssm_->FindSessionByAffinity(affinity);
    EXPECT_EQ(ret, sceneSession);
}

/**
 * @tc.name: FindSessionByAffinity01
 * @tc.desc: FindSessionByAffinity
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, FindSessionByAffinity01, Function | SmallTest | Level3)
{
    std::string affinity = "Test";
    sptr<SceneSession> sceneSession = nullptr;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    auto ret = ssm_->FindSessionByAffinity(affinity);
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name: ProcessUpdateRotationChange01
 * @tc.desc: ProcessUpdateRotationChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, ProcessUpdateRotationChange01, Function | SmallTest | Level3)
{
    DisplayId defaultDisplayId = 0;
    sptr<DisplayInfo> displayInfo = sptr<DisplayInfo>::MakeSptr();
    ASSERT_NE(nullptr, displayInfo);
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap;
    DisplayStateChangeType type = DisplayStateChangeType::BEFORE_SUSPEND;
    sptr<SceneSession> sceneSession = nullptr;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->ProcessUpdateRotationChange(defaultDisplayId, displayInfo, displayInfoMap, type);
}

/**
 * @tc.name: ProcessUpdateRotationChange02
 * @tc.desc: ProcessUpdateRotationChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, ProcessUpdateRotationChange02, Function | SmallTest | Level3)
{
    DisplayId defaultDisplayId = 0;
    sptr<DisplayInfo> displayInfo = sptr<DisplayInfo>::MakeSptr();
    ASSERT_NE(nullptr, displayInfo);
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap;
    DisplayStateChangeType type = DisplayStateChangeType::BEFORE_SUSPEND;
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "UpdateAvoidArea";
    sessionInfo.isSystem_ = true;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    sceneSession->SetSessionState(SessionState::STATE_INACTIVE);
    WSRectF bounds = { 0, 0, 0, 0 };
    sceneSession->SetBounds(bounds);
    displayInfo->width_ = 0;
    displayInfo->height_ = 0;
    sceneSession->SetRotation(Rotation::ROTATION_0);
    displayInfo->SetRotation(Rotation::ROTATION_90);
    ssm_->ProcessUpdateRotationChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    displayInfo->height_ = 1;
    ssm_->ProcessUpdateRotationChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    displayInfo->width_ = 1;
    ssm_->ProcessUpdateRotationChange(defaultDisplayId, displayInfo, displayInfoMap, type);
}

/**
 * @tc.name: SetSkipSelfWhenShowOnVirtualScreen
 * @tc.desc: SetSkipSelfWhenShowOnVirtualScreen
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, SetSkipSelfWhenShowOnVirtualScreen, Function | SmallTest | Level3)
{
    uint64_t surfaceNodeId = 0;
    bool isSkip = true;
    ASSERT_NE(nullptr, ssm_);
    ssm_->skipSurfaceNodeIds_.clear();
    ssm_->SetSkipSelfWhenShowOnVirtualScreen(surfaceNodeId, isSkip);
    ssm_->skipSurfaceNodeIds_.push_back(surfaceNodeId);
    ssm_->SetSkipSelfWhenShowOnVirtualScreen(surfaceNodeId, isSkip);
}

/**
 * @tc.name: SetSkipSelfWhenShowOnVirtualScreen01
 * @tc.desc: SetSkipSelfWhenShowOnVirtualScreen
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, SetSkipSelfWhenShowOnVirtualScreen01, Function | SmallTest | Level3)
{
    uint64_t surfaceNodeId = 0;
    bool isSkip = false;
    ASSERT_NE(nullptr, ssm_);
    ssm_->skipSurfaceNodeIds_.clear();
    ssm_->SetSkipSelfWhenShowOnVirtualScreen(surfaceNodeId, isSkip);
    ssm_->skipSurfaceNodeIds_.push_back(surfaceNodeId);
    ssm_->SetSkipSelfWhenShowOnVirtualScreen(surfaceNodeId, isSkip);
}

/**
 * @tc.name: GetMainWindowInfos
 * @tc.desc: GetMainWindowInfos
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, GetMainWindowInfos, Function | SmallTest | Level3)
{
    int32_t topNum = 1;
    std::vector<MainWindowInfo> topNInfo;
    topNInfo.clear();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "GetMainWindowInfos";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    sceneSession->isVisible_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    auto ret = ssm_->GetMainWindowInfos(topNum, topNInfo);
    EXPECT_EQ(ret, WMError::WM_OK);
    sceneSession->isVisible_ = false;
    ret = ssm_->GetMainWindowInfos(topNum, topNInfo);
    EXPECT_EQ(ret, WMError::WM_OK);
    sceneSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ret = ssm_->GetMainWindowInfos(topNum, topNInfo);
    EXPECT_EQ(ret, WMError::WM_OK);
    sceneSession->isVisible_ = true;
    ret = ssm_->GetMainWindowInfos(topNum, topNInfo);
    EXPECT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: WindowLayerInfoChangeCallback
 * @tc.desc: WindowLayerInfoChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, WindowLayerInfoChangeCallback, Function | SmallTest | Level3)
{
    std::shared_ptr<RSOcclusionData> occlusiontionData = nullptr;
    ASSERT_NE(nullptr, ssm_);
    ssm_->WindowLayerInfoChangeCallback(occlusiontionData);
    VisibleData visibleData;
    visibleData.push_back(std::make_pair(0, WINDOW_LAYER_INFO_TYPE::ALL_VISIBLE));
    visibleData.push_back(std::make_pair(1, WINDOW_LAYER_INFO_TYPE::SEMI_VISIBLE));
    visibleData.push_back(std::make_pair(2, WINDOW_LAYER_INFO_TYPE::INVISIBLE));
    visibleData.push_back(std::make_pair(3, WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_DYNAMIC_STATUS));
    visibleData.push_back(std::make_pair(4, WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_STATIC_STATUS));
    visibleData.push_back(std::make_pair(5, WINDOW_LAYER_INFO_TYPE::WINDOW_LAYER_UNKNOWN_TYPE));
    occlusiontionData = std::make_shared<RSOcclusionData>(visibleData);
    ASSERT_NE(nullptr, occlusiontionData);
    ssm_->WindowLayerInfoChangeCallback(occlusiontionData);
}

/**
 * @tc.name: NotifySessionMovedToFront
 * @tc.desc: NotifySessionMovedToFront
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, NotifySessionMovedToFront, Function | SmallTest | Level3)
{
    int32_t persistentId = 1;
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "GetMainWindowInfos";
    sessionInfo.isSystem_ = false;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, ssm_);
    ssm_->listenerController_ = std::make_shared<SessionListenerController>();
    ASSERT_NE(nullptr, ssm_->listenerController_);
    sceneSession->sessionInfo_.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    ASSERT_NE(nullptr, sceneSession->sessionInfo_.abilityInfo);
    sceneSession->sessionInfo_.abilityInfo->excludeFromMissions = false;
    ssm_->sceneSessionMap_.insert(std::make_pair(persistentId, sceneSession));
    ssm_->NotifySessionMovedToFront(persistentId);
    sceneSession->sessionInfo_.abilityInfo->excludeFromMissions = true;
    ssm_->NotifySessionMovedToFront(persistentId);
    sceneSession->sessionInfo_.abilityInfo = nullptr;
    ssm_->NotifySessionMovedToFront(persistentId);
    sceneSession->sessionInfo_.isSystem_ = true;
    ssm_->NotifySessionMovedToFront(persistentId);
    ssm_->listenerController_ = nullptr;
    ssm_->NotifySessionMovedToFront(persistentId);
}

/**
 * @tc.name: ProcessVirtualPixelRatioChange02
 * @tc.desc: ProcessVirtualPixelRatioChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, ProcessVirtualPixelRatioChange02, Function | SmallTest | Level3)
{
    DisplayId defaultDisplayId = 0;
    sptr<DisplayInfo> displayInfo = sptr<DisplayInfo>::MakeSptr();
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap;
    DisplayStateChangeType type = DisplayStateChangeType::BEFORE_SUSPEND;
    ASSERT_NE(nullptr, displayInfo);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "ProcessVirtualPixelRatioChange02";
    sessionInfo.isSystem_ = true;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    sptr<SceneSession> sceneSession1 = nullptr;
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession1));
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
}

/**
 * @tc.name: ProcessVirtualPixelRatioChange03
 * @tc.desc: ProcessVirtualPixelRatioChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, ProcessVirtualPixelRatioChange03, Function | SmallTest | Level3)
{
    DisplayId defaultDisplayId = 0;
    sptr<DisplayInfo> displayInfo = sptr<DisplayInfo>::MakeSptr();
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap;
    DisplayStateChangeType type = DisplayStateChangeType::BEFORE_SUSPEND;
    ASSERT_NE(nullptr, displayInfo);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "ProcessVirtualPixelRatioChange03";
    sessionInfo.isSystem_ = false;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    ASSERT_NE(nullptr, ssm_);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    sceneSession1->SetSessionState(SessionState::STATE_ACTIVE);
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession1));
    sptr<SceneSession> sceneSession2 = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession2);
    sceneSession2->SetSessionState(SessionState::STATE_INACTIVE);
    ssm_->sceneSessionMap_.insert(std::make_pair(3, sceneSession2));
    ssm_->ProcessVirtualPixelRatioChange(defaultDisplayId, displayInfo, displayInfoMap, type);
}

/**
 * @tc.name: ProcessBackEvent01
 * @tc.desc: ProcessBackEvent01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest7, ProcessBackEvent01, Function | SmallTest | Level3)
{
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest7";
    sessionInfo.abilityName_ = "ProcessBackEvent01";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ASSERT_NE(nullptr, ssm_);
    ssm_->focusedSessionId_ = 1;
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->needBlockNotifyFocusStatusUntilForeground_ = true;
    auto ret = ssm_->ProcessBackEvent();
    EXPECT_EQ(ret, WSError::WS_OK);
}
}
} // namespace Rosen
} // namespace OHOS