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

#include "window_scene_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionManager"};
}

WS_IMPLEMENT_SINGLE_INSTANCE(SceneSessionManager)

sptr<SceneSession> SceneSessionManager::RequestSceneSession(const AbilityInfo& abilityInfo, SessionOption option)
{
    sptr<SceneSession> session = new SceneSession(abilityInfo);
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
    // TODO:AMS
    return WSError::WS_OK;
}
WSError SceneSessionManager::RequestSceneSessionBackground(const sptr<SceneSession>& sceneSession)
{
    if(sceneSession == nullptr) {
        WLOGFE("session is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    // TODO:AMS
    return WSError::WS_OK;
}
WSError SceneSessionManager::RequestSceneSessionDestruction(const sptr<SceneSession>& sceneSession)
{
    if(sceneSession == nullptr) {
        WLOGFE("session is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    // TODO:AMS
    std::lock_guard<std::recursive_mutex> lock(mutex_);
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
