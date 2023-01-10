/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "scene_session_manager.h"

#include <ability_manager_client.h>
#include <want.h>
#include <start_options.h>
#include <scene_session_info.h>

#include "scene_session.h"
#include "window_scene_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionManager"};
}

WS_IMPLEMENT_SINGLE_INSTANCE(SceneSessionManager)

sptr<SceneSession> SceneSessionManager::RequestSceneSession(const SceneAbilityInfo& abilityInfo, SessionOption option)
{
    WLOGFI("abilityInfo: bundleName: %{public}s, abilityName: %{public}s", abilityInfo.bundleName_.c_str(),
        abilityInfo.abilityName_.c_str());
    sptr<SceneSession> session = new (std::nothrow) SceneSession(abilityInfo);
    uint32_t persistentId = pid_ + GenSessionId();
    session->SetPersistentId(persistentId);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    sessions_.emplace_back(session);
    return session;
}

WSError SceneSessionManager::RequestSceneSessionActivation(const sptr<SceneSession>& sceneSession)
{
    if(sceneSession == nullptr) {
        WLOGFE("session is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    AAFwk::Want want;
    auto abilityInfo = sceneSession->GetAbilityInfo();
    want.SetElementName(abilityInfo.bundleName_, abilityInfo.abilityName_);
    AAFwk::StartOptions startOptions;
    sptr<AAFwk::SceneSessionInfo> sceneSessionInfo = new (std::nothrow) AAFwk::SceneSessionInfo();
    sceneSessionInfo->sceneSessionToken_ = sceneSession;
    sceneSessionInfo->surfaceNode_ = sceneSession->GetSurfaceNode();
    sceneSessionInfo->persistentId_ = sceneSession->GetPersistentId();
    AAFwk::AbilityManagerClient::GetInstance()->StartAbilityByLauncher(want, startOptions, nullptr, sceneSessionInfo);

    auto abilityToken = AAFwk::AbilityManagerClient::GetInstance()->GetTokenBySceneSession(
        sceneSessionInfo->persistentId_);
    if (abilityToken) {
        WLOGFW("abilityToken is not null");
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    abilitySceneMap_.insert(std::make_pair(sceneSession->GetPersistentId(), abilityToken));
    return WSError::WS_OK;
}

WSError SceneSessionManager::RequestSceneSessionBackground(const sptr<SceneSession>& sceneSession)
{
    if(sceneSession == nullptr) {
        WLOGFE("session is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    sceneSession->Background();
    // TODO:AMS
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto persistentId = sceneSession->GetPersistentId();
    if (abilitySceneMap_.count(persistentId) == 0) {
        WLOGFE("persistentId:%{public}u not find ability.", persistentId);
        return WSError::WS_ERROR_NULLPTR;
    }
    auto abilityToken = abilitySceneMap_[persistentId];
    AAFwk::AbilityManagerClient::GetInstance()->MinimizeAbility(abilityToken);
    return WSError::WS_OK;
}

WSError SceneSessionManager::RequestSceneSessionDestruction(const sptr<SceneSession>& sceneSession)
{
    if(sceneSession == nullptr) {
        WLOGFE("session is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    sceneSession->Disconnect();
    // TODO:AMS
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto persistentId = sceneSession->GetPersistentId();
    if (abilitySceneMap_.count(persistentId) == 0) {
        WLOGFE("persistentId:%{public}u not find ability.", persistentId);
        return WSError::WS_ERROR_NULLPTR;
    }
    auto abilityToken = abilitySceneMap_[persistentId];
    AAFwk::AbilityManagerClient::GetInstance()->TerminateAbility(abilityToken, -1);
    abilitySceneMap_.erase(persistentId);
    auto iter = std::find(sessions_.begin(), sessions_.end(), sceneSession);
    if (iter == sessions_.end()) {
        WLOGFW("could not find session");
        return WSError::WS_DO_NOTHING;
    }
    sessions_.erase(iter);
    return WSError::WS_OK;
}

uint32_t SceneSessionManager::GenSessionId()
{
    return ++sessionId_;
}
}
