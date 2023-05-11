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

#include "session_manager/include/scene_session_manager.h"

#include <ability_manager_client.h>
#include <start_options.h>
#include <ui_window.h>
#include <want.h>

#include "common/include/message_scheduler.h"
#include "session/host/include/scene_session.h"
#include "session_info.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionManager" };
const std::string SCENE_SESSION_MANAGER_THREAD = "SceneSessionManager";
}

WM_IMPLEMENT_SINGLE_INSTANCE(SceneSessionManager)
SceneSessionManager::SceneSessionManager()
{
    Init();
}

void SceneSessionManager::Init()
{
    WLOGFI("scene session manager init");
    msgScheduler_ = std::make_shared<MessageScheduler>(SCENE_SESSION_MANAGER_THREAD);
}

sptr<RootSceneSession> SceneSessionManager::GetRootSceneSession()
{
    auto task = [this]() {
        if (rootSceneSession_ != nullptr) {
            return rootSceneSession_;
        }

        rootSceneSession_ = new (std::nothrow) RootSceneSession();
        rootScene_ = Ace::NG::UIWindow::CreateRootScene();
        if (!rootSceneSession_ || !rootScene_) {
            WLOGFE("rootSceneSession or rootScene is nullptr");
            return sptr<RootSceneSession>(nullptr);
        }
        rootSceneSession_->SetLoadContentFunc(
            [rootScene = rootScene_](const std::string& contentUrl, NativeEngine* engine, NativeValue* storage,
                AbilityRuntime::Context* context) { rootScene->LoadContent(contentUrl, engine, storage, context); });
        return rootSceneSession_;
    };

    WS_CHECK_NULL_SCHE_RETURN(msgScheduler_, task);
    return msgScheduler_->PostSyncTask(task);
}

sptr<SceneSession> SceneSessionManager::GetSceneSession(uint64_t persistentId)
{
    auto iter = abilitySceneMap_.find(persistentId);
    if (iter == abilitySceneMap_.end()) {
        WLOGFE("Error found scene session with id: %{public}" PRIu64, persistentId);
        return nullptr;
    }
    return iter->second;
}

sptr<SceneSession> SceneSessionManager::RequestSceneSession(const SessionInfo& sessionInfo)
{
    auto task = [this, sessionInfo]() {
        WLOGFI("sessionInfo: bundleName: %{public}s, abilityName: %{public}s", sessionInfo.bundleName_.c_str(),
            sessionInfo.abilityName_.c_str());
        sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(sessionInfo);
        if (sceneSession == nullptr) {
            WLOGFE("sceneSession is nullptr!");
            return sceneSession;
        }
        uint64_t persistentId = GeneratePersistentId();
        sceneSession->SetPersistentId(persistentId);
        abilitySceneMap_.insert({ persistentId, sceneSession });
        WLOGFI("create session persistentId: %{public}" PRIu64 "", persistentId);
        return sceneSession;
    };
    WS_CHECK_NULL_SCHE_RETURN(msgScheduler_, task);
    return msgScheduler_->PostSyncTask(task);
}

sptr<AAFwk::SessionInfo> SceneSessionManager::SetAbilitySessionInfo(const sptr<SceneSession>& scnSession)
{
    sptr<AAFwk::SessionInfo> abilitySessionInfo = new (std::nothrow) AAFwk::SessionInfo();
    if (!abilitySessionInfo) {
        WLOGFE("abilitySessionInfo is nullptr");
        return nullptr;
    }
    auto sessionInfo = scnSession->GetSessionInfo();
    abilitySessionInfo->sessionToken = scnSession;
    abilitySessionInfo->callerToken = sessionInfo.callerToken_;
    abilitySessionInfo->persistentId = scnSession->GetPersistentId();
    return abilitySessionInfo;
}

WSError SceneSessionManager::RequestSceneSessionActivation(const sptr<SceneSession>& sceneSession)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = scnSession->GetPersistentId();
        WLOGFI("active persistentId: %{public}" PRIu64 "", persistentId);
        if (abilitySceneMap_.count(persistentId) == 0) {
            WLOGFE("session is invalid with %{public}" PRIu64 "", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        AAFwk::Want want;
        auto sessionInfo = scnSession->GetSessionInfo();
        want.SetElementName(sessionInfo.bundleName_, sessionInfo.abilityName_);
        AAFwk::StartOptions startOptions;
        auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
        if (!scnSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
        // to add StartAbility
        AAFwk::AbilityManagerClient::GetInstance()->StartUIAbilityBySCB(want, startOptions, scnSessionInfo);
        return WSError::WS_OK;
    };
    WS_CHECK_NULL_SCHE_RETURN(msgScheduler_, task);
    msgScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}

WSError SceneSessionManager::RequestSceneSessionBackground(const sptr<SceneSession>& sceneSession)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = scnSession->GetPersistentId();
        WLOGFI("background session persistentId: %{public}" PRIu64 "", persistentId);
        scnSession->SetActive(false);
        scnSession->Background();
        if (abilitySceneMap_.count(persistentId) == 0) {
            WLOGFE("session is invalid with %{public}" PRIu64 "", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
        if (!scnSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
        // to add MinimizeAbility
        AAFwk::AbilityManagerClient::GetInstance()->MinimizeUIAbilityBySCB(scnSessionInfo);
        return WSError::WS_OK;
    };

    WS_CHECK_NULL_SCHE_RETURN(msgScheduler_, task);
    msgScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}

WSError SceneSessionManager::RequestSceneSessionDestruction(const sptr<SceneSession>& sceneSession)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = scnSession->GetPersistentId();
        WLOGFI("destroy session persistentId: %{public}" PRIu64 "", persistentId);
        scnSession->Disconnect();
        if (abilitySceneMap_.count(persistentId) == 0) {
            WLOGFE("session is invalid with %{public}" PRIu64 "", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        abilitySceneMap_.erase(persistentId);
        auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
        if (!scnSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
        // to add TerminateAbility
        AAFwk::AbilityManagerClient::GetInstance()->CloseUIAbilityBySCB(scnSessionInfo);
        return WSError::WS_OK;
    };

    WS_CHECK_NULL_SCHE_RETURN(msgScheduler_, task);
    msgScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}
} // namespace OHOS::Rosen
