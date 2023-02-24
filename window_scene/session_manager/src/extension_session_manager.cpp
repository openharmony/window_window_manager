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

#include "session_manager/include/extension_session_manager.h"

#include <ability_manager_client.h>
#include <start_options.h>
#include <want.h>

#include "session/host/include/extension_session.h"
#include "session_info.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ExtensionSessionManager" };
const std::string EXTENSION_SESSION_MANAGER_THREAD = "ExtensionSessionManager";
}

WM_IMPLEMENT_SINGLE_INSTANCE(ExtensionSessionManager)

ExtensionSessionManager::ExtensionSessionManager()
{
    Init();
}

void ExtensionSessionManager::Init()
{
    WLOGFI("extension session manager init.");
    mmsScheduler_ = std::make_shared<MessageScheduler>(EXTENSION_SESSION_MANAGER_THREAD);
}

sptr<ExtensionSession> ExtensionSessionManager::RequestExtensionSession(const SessionInfo& sessionInfo)
{
    auto task = [this, sessionInfo]() {
        WLOGFI("sessionInfo: bundleName: %{public}s, abilityName: %{public}s", sessionInfo.bundleName_.c_str(),
            sessionInfo.abilityName_.c_str());
        sptr<ExtensionSession> extensionSession = new (std::nothrow) ExtensionSession(sessionInfo);
        uint64_t persistentId = GeneratePersistentId();
        extensionSession->SetPersistentId(persistentId);
        WLOGFI("create session persistentId: %{public}" PRIu64 "", persistentId);
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        abilityExtensionMap_.insert({ persistentId, std::make_pair(extensionSession, nullptr) });
        return extensionSession;
    };
    WS_CHECK_NULL_SCHE_RETURN(mmsScheduler_, task);
    return mmsScheduler_->PostSyncTask(task);
}

WSError ExtensionSessionManager::RequestExtensionSessionActivation(const sptr<ExtensionSession>& extensionSession)
{
    wptr<ExtensionSession> weakExtSession(extensionSession);
    auto task = [this, weakExtSession]() {
        auto extSession = weakExtSession.promote();
        if(extSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = extSession->GetPersistentId();
        WLOGFI("active persistentId: %{public}" PRIu64 "", persistentId);
        if (abilityExtensionMap_.count(persistentId) == 0) {
            WLOGFE("session is invalid with %{public}" PRIu64 "", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        AAFwk::Want want;
        auto sessionInfo = extSession->GetSessionInfo();
        want.SetElementName(sessionInfo.bundleName_, sessionInfo.abilityName_);
        AAFwk::StartOptions startOptions;
        sptr<AAFwk::SessionInfo> abilitySessionInfo = new (std::nothrow) AAFwk::SessionInfo();
        abilitySessionInfo->sessionToken = extSession;
        abilitySessionInfo->surfaceNode = extSession->GetSurfaceNode();
        abilitySessionInfo->callerToken = sessionInfo.callerToken_;
        abilitySessionInfo->persistentId = extSession->GetPersistentId();
        AAFwk::AbilityManagerClient::GetInstance()->StartUIExtensionAbility(want, abilitySessionInfo,
            AAFwk::DEFAULT_INVAL_VALUE,
            AppExecFwk::ExtensionAbilityType::UIEXTENSION);
        auto newAbilityToken = AAFwk::AbilityManagerClient::GetInstance()->GetTokenBySceneSession(
            abilitySessionInfo->persistentId);
        // replace with real token after start ability
        abilityExtensionMap_[persistentId].second = newAbilityToken;
        return WSError::WS_OK;
    };
    WS_CHECK_NULL_SCHE_RETURN(mmsScheduler_, task);
    mmsScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}

WSError ExtensionSessionManager::RequestExtensionSessionBackground(const sptr<ExtensionSession>& extensionSession)
{
    wptr<ExtensionSession> weakExtSession(extensionSession);
    auto task = [this, weakExtSession]() {
        auto extSession = weakExtSession.promote();
        if(extSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = extSession->GetPersistentId();
        WLOGFI("background session persistentId: %{public}" PRIu64 "", persistentId);
        extSession->Background();
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (abilityExtensionMap_.count(persistentId) == 0) {
            WLOGFE("session is invalid with %{public}" PRIu64 "", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        AAFwk::AbilityManagerClient::GetInstance()->MinimizeUIExtensionAbility(persistentId);
        return WSError::WS_OK;
    };
    WS_CHECK_NULL_SCHE_RETURN(mmsScheduler_, task);
    mmsScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}

WSError ExtensionSessionManager::RequestExtensionSessionDestruction(const sptr<ExtensionSession>& extensionSession)
{
    wptr<ExtensionSession> weakExtSession(extensionSession);
    auto task = [this, weakExtSession]() {
        auto extSession = weakExtSession.promote();
        if(extSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = extSession->GetPersistentId();
        WLOGFI("destroy session persistentId: %{public}" PRIu64 "", persistentId);
        extSession->Disconnect();
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (abilityExtensionMap_.count(persistentId) == 0) {
            WLOGFE("session is invalid with %{public}" PRIu64 "", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        AAFwk::Want resultWant;
        AAFwk::AbilityManagerClient::GetInstance()->TerminateUIExtensionAbility(persistentId);
        abilityExtensionMap_.erase(persistentId);
        return WSError::WS_OK;
    };
    WS_CHECK_NULL_SCHE_RETURN(mmsScheduler_, task);
    mmsScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}
} // namespace OHOS::Rosen
