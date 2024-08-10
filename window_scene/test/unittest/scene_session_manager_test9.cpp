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
 * @tc.name: TraverseSessionTreeFromTopToBottom01
 * @tc.desc: TraverseSessionTreeFromTopToBottom
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, TraverseSessionTreeFromTopToBottom01, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    TraverseFunc func;
    ssm_->TraverseSessionTreeFromTopToBottom(func);
}

/**
 * @tc.name: TraverseSessionTreeFromTopToBottom02
 * @tc.desc: TraverseSessionTreeFromTopToBottom
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, TraverseSessionTreeFromTopToBottom02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "TraverseSessionTreeFromTopToBottom";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, nullptr));
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession));
}

/**
 * @tc.name: RequestFocusStatus02
 * @tc.desc: RequestFocusStatus
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, RequestFocusStatus02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    EXPECT_EQ(ssm_->RequestFocusStatus(1, false, false, FocusChangeReason::FLOATING_SCENE), WMError::WM_ERROR_NULLPTR);

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "RequestFocusStatus02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->RequestFocusStatus(1, false, false, FocusChangeReason::FLOATING_SCENE);
}

/**
 * @tc.name: RequestSessionFocusImmediately02
 * @tc.desc: RequestSessionFocusImmediately
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, RequestSessionFocusImmediately02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    EXPECT_NE(ssm_->RequestSessionFocusImmediately(0), WSError::WS_OK);

    ssm_->RequestSessionFocusImmediately(2);

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "RequestSessionFocusImmediately02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);

    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetFocusable(false);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->RequestSessionFocusImmediately(1);

    sceneSession->SetFocusable(true);
    sceneSession->SetFocusedOnShow(false);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->RequestSessionFocusImmediately(1);

    sceneSession->SetFocusedOnShow(true);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->RequestSessionFocusImmediately(1);
}

/**
 * @tc.name: RequestSessionFocus02
 * @tc.desc: RequestSessionFocus
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, RequestSessionFocus02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "RequestSessionFocus02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->SetFocusable(true);
    sceneSession->UpdateVisibilityInner(true);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);
    sceneSession->SetFocusedOnShow(false);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->RequestSessionFocus(1, false, FocusChangeReason::DEFAULT);

    sceneSession->SetFocusedOnShow(true);
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->RequestSessionFocus(1, false, FocusChangeReason::DEFAULT);

    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sceneSession->SetForceHideState(ForceHideState::HIDDEN_WHEN_FOCUSED);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->RequestSessionFocus(1, false, FocusChangeReason::DEFAULT);

    sceneSession->SetForceHideState(ForceHideState::NOT_HIDDEN);
    sceneSession->SetTopmost(true);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->RequestSessionFocus(1, false, FocusChangeReason::DEFAULT);
}

/**
 * @tc.name: RequestSessionUnfocus02
 * @tc.desc: RequestSessionUnfocus
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, RequestSessionUnfocus02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->RequestSessionUnfocus(1, FocusChangeReason::DEFAULT);

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "RequestSessionUnfocus02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    ssm_->focusedSessionId_ = 2;
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));

    SessionInfo sessionInfo1;
    sptr<SceneSession> sceneSession1 = sptr<SceneSession>::MakeSptr(sessionInfo1, nullptr);
    ASSERT_NE(nullptr, sceneSession1);
    sceneSession1->GetSessionProperty()->SetParentPersistentId(3);
    sceneSession1->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ssm_->sceneSessionMap_.insert(std::make_pair(2, sceneSession1));
    ssm_->RequestSessionUnfocus(1, FocusChangeReason::DEFAULT);

    ssm_->lastFocusedSessionId_ = 4;
    sceneSession1->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_FLOAT);
    ssm_->RequestSessionUnfocus(1, FocusChangeReason::DEFAULT);

    SessionInfo sessionInfo2;
    sptr<SceneSession> sceneSession2 = sptr<SceneSession>::MakeSptr(sessionInfo2, nullptr);
    ASSERT_NE(nullptr, sceneSession2);
    sceneSession2->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_FLOAT);
    ssm_->sceneSessionMap_.insert(std::make_pair(4, sceneSession1));
    ssm_->RequestSessionUnfocus(1, FocusChangeReason::DEFAULT);

    sceneSession2->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_DESKTOP);
    ssm_->RequestSessionUnfocus(1, FocusChangeReason::DEFAULT);

    ssm_->focusedSessionId_ = 5;
    ssm_->RequestSessionUnfocus(1, FocusChangeReason::DEFAULT);
}

/**
 * @tc.name: RequestAllAppSessionUnfocusInner
 * @tc.desc: RequestAllAppSessionUnfocusInner
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, RequestAllAppSessionUnfocusInner, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    ssm_->RequestAllAppSessionUnfocusInner();

    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "RequestAllAppSessionUnfocusInner";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ssm_->focusedSessionId_ = 1;
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->RequestAllAppSessionUnfocusInner();

    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_FLOAT);
    ssm_->RequestAllAppSessionUnfocusInner();
}

/**
 * @tc.name: UpdateFocus04
 * @tc.desc: UpdateFocus
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, UpdateFocus04, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "UpdateFocus04";
    sessionInfo.isSystem_ = true;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, sceneSession));
    ssm_->focusedSessionId_ = 0;
    sceneSession->UpdateFocus(false);
    ssm_->UpdateFocus(1, false);

    ssm_->listenerController_ = nullptr;
    ssm_->UpdateFocus(1, true);

    std::shared_ptr<SessionListenerController> listenerController = std::make_shared<SessionListenerController>();
    ssm_->listenerController_ = listenerController;
    ssm_->UpdateFocus(1, true);

    sessionInfo.isSystem_ = false;
    ssm_->focusedSessionId_ = 1;
    sceneSession->UpdateFocus(true);
    ssm_->UpdateFocus(1, false);

    sceneSession->UpdateFocus(false);
    ssm_->UpdateFocus(1, true);
}

/**
 * @tc.name: ProcessFocusWhenForeground
 * @tc.desc: ProcessFocusWhenForeground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, ProcessFocusWhenForeground, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "ProcessFocusWhenForeground";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->persistentId_ = 1;
    ASSERT_NE(nullptr, sceneSession->property_);
    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ssm_->focusedSessionId_ = 1;
    ssm_->needBlockNotifyFocusStatusUntilForeground_ = false;
    ssm_->ProcessFocusWhenForeground(sceneSession);
    
    sceneSession->isVisible_ = true;
    sceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    ssm_->ProcessFocusWhenForeground(sceneSession);
}

/**
 * @tc.name: ProcessSubSessionForeground03
 * @tc.desc: ProcessSubSessionForeground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, ProcessSubSessionForeground03, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "ProcessSubSessionForeground03";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);

    SessionInfo subSessionInfo;
    sptr<SceneSession> subSceneSession = sptr<SceneSession>::MakeSptr(subSessionInfo, nullptr);
    ASSERT_NE(nullptr, subSceneSession);
    subSceneSession->SetTopmost(true);
    subSceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    subSceneSession->persistentId_ = 1;
    sceneSession->GetSubSession().push_back(subSceneSession);
    ssm_->sceneSessionMap_.insert(std::make_pair(1, subSceneSession));

    ssm_->focusedSessionId_ = 1;
    ssm_->needBlockNotifyFocusStatusUntilForeground_ = true;

    SessionInfo subSessionInfo1;
    sptr<SceneSession> subSceneSession1 = sptr<SceneSession>::MakeSptr(subSessionInfo1, nullptr);
    ASSERT_NE(nullptr, subSceneSession1);
    subSceneSession1->SetTopmost(true);
    subSceneSession1->SetSessionState(SessionState::STATE_ACTIVE);
    subSceneSession1->persistentId_ = 0;
    sceneSession->GetSubSession().push_back(subSceneSession1);

    SessionInfo subSessionInfo2;
    sptr<SceneSession> subSceneSession2 = sptr<SceneSession>::MakeSptr(subSessionInfo2, nullptr);
    ASSERT_NE(nullptr, subSceneSession2);
    subSceneSession2->SetTopmost(true);
    subSceneSession2->SetSessionState(SessionState::STATE_CONNECT);
    sceneSession->GetSubSession().push_back(subSceneSession2);

    SessionInfo subSessionInfo3;
    sptr<SceneSession> subSceneSession3 = sptr<SceneSession>::MakeSptr(subSessionInfo3, nullptr);
    ASSERT_NE(nullptr, subSceneSession3);
    subSceneSession3->SetTopmost(false);
    subSceneSession3->SetSessionState(SessionState::STATE_ACTIVE);
    sceneSession->GetSubSession().push_back(subSceneSession3);

    sceneSession->GetSubSession().push_back(nullptr);

    ssm_->ProcessSubSessionForeground(sceneSession);

    ssm_->needBlockNotifyFocusStatusUntilForeground_ = false;
    ssm_->ProcessSubSessionForeground(sceneSession);

    ssm_->focusedSessionId_ = 2;
    ssm_->ProcessSubSessionForeground(sceneSession);
}

/**
 * @tc.name: ProcessModalTopmostRequestFocusImmdediately02
 * @tc.desc: ProcessModalTopmostRequestFocusImmdediately
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, ProcessModalTopmostRequestFocusImmdediately02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "ProcessModalTopmostRequestFocusImmdediately02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);

    SessionInfo subSessionInfo;
    sptr<SceneSession> subSceneSession = sptr<SceneSession>::MakeSptr(subSessionInfo, nullptr);
    ASSERT_NE(nullptr, subSceneSession);
    subSceneSession->SetTopmost(true);
    subSceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    subSceneSession->persistentId_ = 1;
    subSceneSession->SetFocusable(true);
    subSceneSession->SetFocusedOnShow(true);
    sceneSession->GetSubSession().push_back(subSceneSession);

    ssm_->sceneSessionMap_.insert(std::make_pair(1, subSceneSession));
    ssm_->focusedSessionId_ = 1;
    ssm_->needBlockNotifyFocusStatusUntilForeground_ = true;

    SessionInfo subSessionInfo1;
    sptr<SceneSession> subSceneSession1 = sptr<SceneSession>::MakeSptr(subSessionInfo1, nullptr);
    ASSERT_NE(nullptr, subSceneSession1);
    subSceneSession1->SetTopmost(true);
    subSceneSession1->SetSessionState(SessionState::STATE_ACTIVE);
    subSceneSession1->persistentId_ = 0;
    sceneSession->GetSubSession().push_back(subSceneSession1);

    SessionInfo subSessionInfo2;
    sptr<SceneSession> subSceneSession2 = sptr<SceneSession>::MakeSptr(subSessionInfo2, nullptr);
    ASSERT_NE(nullptr, subSceneSession2);
    subSceneSession2->SetTopmost(true);
    subSceneSession2->SetSessionState(SessionState::STATE_CONNECT);
    sceneSession->GetSubSession().push_back(subSceneSession2);

    SessionInfo subSessionInfo3;
    sptr<SceneSession> subSceneSession3 = sptr<SceneSession>::MakeSptr(subSessionInfo3, nullptr);
    ASSERT_NE(nullptr, subSceneSession3);
    subSceneSession3->SetTopmost(false);
    subSceneSession3->SetSessionState(SessionState::STATE_ACTIVE);
    sceneSession->GetSubSession().push_back(subSceneSession3);

    sceneSession->GetSubSession().push_back(nullptr);

    ssm_->ProcessModalTopmostRequestFocusImmdediately(sceneSession);

    ssm_->needBlockNotifyFocusStatusUntilForeground_ = false;
    ssm_->ProcessSubSessionForeground(sceneSession);

    ssm_->persistentId = 0;
    ssm_->ProcessModalTopmostRequestFocusImmdediately(sceneSession);
}

/**
 * @tc.name: ProcessDialogRequestFocusImmdediately02
 * @tc.desc: ProcessDialogRequestFocusImmdediately
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerTest9, ProcessDialogRequestFocusImmdediately02, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, ssm_);
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = "SceneSessionManagerTest9";
    sessionInfo.abilityName_ = "ProcessDialogRequestFocusImmdediately02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);

    SessionInfo dialogSessionInfo;
    sptr<SceneSession> dialogSceneSession = sptr<SceneSession>::MakeSptr(dialogSessionInfo, nullptr);
    ASSERT_NE(nullptr, dialogSceneSession);
    dialogSceneSession->SetTopmost(true);
    dialogSceneSession->SetSessionState(SessionState::STATE_FOREGROUND);
    dialogSceneSession->persistentId_ = 1;
    dialogSceneSession->SetFocusable(true);
    dialogSceneSession->SetFocusedOnShow(true);
    sceneSession->GetDialogVector().push_back(dialogSceneSession);

    ssm_->sceneSessionMap_.insert(std::make_pair(1, dialogSceneSession));
    ssm_->focusedSessionId_ = 1;
    ssm_->needBlockNotifyFocusStatusUntilForeground_ = true;

    SessionInfo dialogSessionInfo1;
    sptr<SceneSession> dialogSceneSession1 = sptr<SceneSession>::MakeSptr(dialogSessionInfo1, nullptr);
    ASSERT_NE(nullptr, dialogSceneSession1);
    dialogSceneSession1->SetTopmost(true);
    dialogSceneSession1->SetSessionState(SessionState::STATE_ACTIVE);
    dialogSceneSession1->persistentId_ = 0;
    sceneSession->GetDialogVector().push_back(dialogSceneSession1);

    SessionInfo dialogSessionInfo2;
    sptr<SceneSession> dialogSceneSession2 = sptr<SceneSession>::MakeSptr(dialogSessionInfo2, nullptr);
    ASSERT_NE(nullptr, dialogSceneSession2);
    dialogSceneSession2->SetTopmost(true);
    dialogSceneSession2->SetSessionState(SessionState::STATE_CONNECT);
    sceneSession->GetDialogVector().push_back(dialogSceneSession2);

    sceneSession->GetdialogSession().push_back(nullptr);

    ssm_->ProcessDialogRequestFocusImmdediately(sceneSession);

    ssm_->persistentId = 0;
    ssm_->ProcessDialogRequestFocusImmdediately(sceneSession);
}
}
} // namespace Rosen
} // namespace OHOS