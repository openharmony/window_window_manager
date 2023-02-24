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
#include "session_info.h"
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
    WLOGFI("scene session manager init.");
    mmsScheduler_ = std::make_shared<MessageScheduler>(SCENE_SESSION_MANAGER_THREAD);
}

sptr<RootSceneSession> SceneSessionManager::GetRootSceneSession()
{
    auto task = [this]() {
        if (rootSceneSession_ != nullptr) {
            return rootSceneSession_;
        }

        rootSceneSession_ = new RootSceneSession();
        rootScene_ = Ace::NG::UIWindow::CreateRootScene();
        rootSceneSession_->SetLoadContentFunc(
            [rootScene = rootScene_](const std::string& contentUrl, NativeEngine* engine, NativeValue* storage,
                AbilityRuntime::Context* context) { rootScene->LoadContent(contentUrl, engine, storage, context); });
        return rootSceneSession_;
    };

    WS_CHECK_NULL_SCHE_RETURN(mmsScheduler_, task);
    return mmsScheduler_->PostSyncTask(task);
}

sptr<SceneSession> SceneSessionManager::RequestSceneSession(const SessionInfo& sessionInfo)
{
    auto task = [this, sessionInfo]() {
        WLOGFI("sessionInfo: bundleName: %{public}s, abilityName: %{public}s", sessionInfo.bundleName_.c_str(),
            sessionInfo.abilityName_.c_str());
        sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(sessionInfo);
        uint64_t persistentId = GeneratePersistentId();
        sceneSession->SetPersistentId(persistentId);
        abilitySceneMap_.insert({ persistentId, std::make_pair(sceneSession, nullptr) });
        WLOGFI("create session persistentId: %{public}" PRIu64 "", persistentId);
        return sceneSession;
    };
    WS_CHECK_NULL_SCHE_RETURN(mmsScheduler_, task);
    return mmsScheduler_->PostSyncTask(task);
}

WSError SceneSessionManager::RequestSceneSessionActivation(const sptr<SceneSession>& sceneSession)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession]() {
        auto scnSession = weakSceneSession.promote();
        if(scnSession == nullptr) {
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
        sptr<AAFwk::SessionInfo> abilitySessionInfo = new (std::nothrow) AAFwk::SessionInfo();
        abilitySessionInfo->sessionToken = scnSession;
        abilitySessionInfo->surfaceNode = scnSession->GetSurfaceNode();
        abilitySessionInfo->persistentId = scnSession->GetPersistentId();
        AAFwk::AbilityManagerClient::GetInstance()->StartAbilityByLauncher(want, startOptions, nullptr, abilitySessionInfo);
        auto newAbilityToken = AAFwk::AbilityManagerClient::GetInstance()->GetTokenBySceneSession(
            abilitySessionInfo->persistentId);
        if (newAbilityToken) {
            WLOGFW("newAbilityToken is not null");
        }
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        // replace with real token after start ability
        abilitySceneMap_[persistentId].second = newAbilityToken;
        return WSError::WS_OK;
    };
    WS_CHECK_NULL_SCHE_RETURN(mmsScheduler_, task);
    mmsScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}

WSError SceneSessionManager::RequestSceneSessionBackground(const sptr<SceneSession>& sceneSession)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession]() {
        auto scnSession = weakSceneSession.promote();
        if(scnSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = scnSession->GetPersistentId();
        WLOGFI("background session persistentId: %{public}" PRIu64 "", persistentId);
        scnSession->Background();
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (abilitySceneMap_.count(persistentId) == 0) {
            WLOGFE("session is invalid with %{public}" PRIu64 "", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto abilityToken = abilitySceneMap_[persistentId].second;
        AAFwk::AbilityManagerClient::GetInstance()->MinimizeAbility(abilityToken);
        return WSError::WS_OK;
    };

    WS_CHECK_NULL_SCHE_RETURN(mmsScheduler_, task);
    mmsScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}

WSError SceneSessionManager::RequestSceneSessionDestruction(const sptr<SceneSession>& sceneSession)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession]() {
        auto scnSession = weakSceneSession.promote();
        if(scnSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = scnSession->GetPersistentId();
        WLOGFI("destroy session persistentId: %{public}" PRIu64 "", persistentId);
        scnSession->Disconnect();
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (abilitySceneMap_.count(persistentId) == 0) {
            WLOGFE("session is invalid with %{public}" PRIu64 "", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto abilityToken = abilitySceneMap_[persistentId].second;
        AAFwk::Want resultWant;
        AAFwk::AbilityManagerClient::GetInstance()->TerminateAbility(abilityToken, -1, &resultWant);
        abilitySceneMap_.erase(persistentId);
        return WSError::WS_OK;
    };

    WS_CHECK_NULL_SCHE_RETURN(mmsScheduler_, task);
    mmsScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}
} // namespace OHOS::Rosen
