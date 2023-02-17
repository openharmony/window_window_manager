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
#include <want.h>
#include <start_options.h>
#include <ui_window.h>
#include "scene_session_info.h"
#include "session/host/include/scene_session.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionManager"};
}

WM_IMPLEMENT_SINGLE_INSTANCE(SceneSessionManager)

sptr<RootSceneSession> SceneSessionManager::GetRootSceneSession()
{
    if (rootSceneSession_ != nullptr) {
        return rootSceneSession_;
    }

    rootSceneSession_ = new RootSceneSession();
    rootScene_ = Ace::NG::UIWindow::CreateRootScene();
    rootSceneSession_->SetLoadContentFunc([rootScene = rootScene_](const std::string& contentUrl,
        NativeEngine* engine, NativeValue* storage, AbilityRuntime::Context* context) {
        rootScene->LoadContent(contentUrl, engine, storage, context);
    });
    return rootSceneSession_;
}

sptr<SceneSession> SceneSessionManager::RequestSceneSession(const SessionInfo& sessionInfo)
{
    WLOGFI("sessionInfo: bundleName: %{public}s, abilityName: %{public}s", sessionInfo.bundleName_.c_str(),
        sessionInfo.abilityName_.c_str());
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(sessionInfo);
    ++sessionId_;
    uint32_t persistentId = pid_ + sessionId_;
    sceneSession->SetPersistentId(persistentId);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    abilitySceneMap_.insert({persistentId, std::make_pair(sceneSession, nullptr)});
    WLOGFI("create session persistentId: %{public}u", persistentId);
    return sceneSession;
}

WSError SceneSessionManager::RequestSceneSessionActivation(const sptr<SceneSession>& sceneSession)
{
    if(sceneSession == nullptr) {
        WLOGFE("session is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    auto persistentId = sceneSession->GetPersistentId();
    WLOGFI("active persistentId: %{public}u", persistentId);
    if (abilitySceneMap_.count(persistentId) == 0) {
        WLOGFE("session is invalid with %{public}u", persistentId);
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    AAFwk::Want want;
    auto sessionInfo = sceneSession->GetSessionInfo();
    want.SetElementName(sessionInfo.bundleName_, sessionInfo.abilityName_);
    AAFwk::StartOptions startOptions;
    sptr<AAFwk::SceneSessionInfo> sceneSessionInfo = new (std::nothrow) AAFwk::SceneSessionInfo();
    sceneSessionInfo->sceneSessionToken_ = sceneSession;
    sceneSessionInfo->surfaceNode_ = sceneSession->GetSurfaceNode();
    sceneSessionInfo->persistentId_ = sceneSession->GetPersistentId();
    AAFwk::AbilityManagerClient::GetInstance()->StartAbilityByLauncher(want, startOptions, nullptr, sceneSessionInfo);
    auto newAbilityToken = AAFwk::AbilityManagerClient::GetInstance()->GetTokenBySceneSession(
        sceneSessionInfo->persistentId_);
    if (newAbilityToken) {
        WLOGFW("newAbilityToken is not null");
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    // replace with real token after start ability
    abilitySceneMap_[persistentId].second = newAbilityToken;
    return WSError::WS_OK;
}

WSError SceneSessionManager::RequestSceneSessionBackground(const sptr<SceneSession>& sceneSession)
{
    if(sceneSession == nullptr) {
        WLOGFE("session is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    auto persistentId = sceneSession->GetPersistentId();
    WLOGFI("background session persistentId: %{public}u", persistentId);
    sceneSession->Background();
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (abilitySceneMap_.count(persistentId) == 0) {
        WLOGFE("session is invalid with %{public}u", persistentId);
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    auto abilityToken = abilitySceneMap_[persistentId].second;
    AAFwk::AbilityManagerClient::GetInstance()->MinimizeAbility(abilityToken);
    return WSError::WS_OK;
}

WSError SceneSessionManager::RequestSceneSessionDestruction(const sptr<SceneSession>& sceneSession)
{
    if(sceneSession == nullptr) {
        WLOGFE("session is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    auto persistentId = sceneSession->GetPersistentId();
    WLOGFI("destroy session persistentId: %{public}u", persistentId);
    sceneSession->Disconnect();
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (abilitySceneMap_.count(persistentId) == 0) {
        WLOGFE("session is invalid with %{public}u", persistentId);
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    auto abilityToken = abilitySceneMap_[persistentId].second;
    AAFwk::Want resultWant;
    AAFwk::AbilityManagerClient::GetInstance()->TerminateAbility(abilityToken, -1, &resultWant);
    abilitySceneMap_.erase(persistentId);
    return WSError::WS_OK;
}
} // namespace OHOS::Rosen
