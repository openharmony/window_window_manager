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
#include <session_info.h>
#include <start_options.h>

#include "session/host/include/extension_session.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ExtensionSessionManager" };
const std::string EXTENSION_SESSION_MANAGER_THREAD = "ExtensionSessionManager";
} // namespace

ExtensionSessionManager& ExtensionSessionManager::GetInstance()
{
    static ExtensionSessionManager* instance = nullptr;
    if (instance == nullptr) {
        instance = new ExtensionSessionManager();
        instance->Init();
    }
    return *instance;
}

void ExtensionSessionManager::Init()
{
    taskScheduler_ = std::make_shared<TaskScheduler>(EXTENSION_SESSION_MANAGER_THREAD);
}

sptr<AAFwk::SessionInfo> ExtensionSessionManager::SetAbilitySessionInfo(const sptr<ExtensionSession>& extSession)
{
    sptr<AAFwk::SessionInfo> abilitySessionInfo = new (std::nothrow) AAFwk::SessionInfo();
    if (!abilitySessionInfo) {
        WLOGFE("abilitySessionInfo is nullptr");
        return nullptr;
    }
    auto sessionInfo = extSession->GetSessionInfo();
    sptr<ISession> iSession(extSession);
    abilitySessionInfo->sessionToken = iSession->AsObject();
    abilitySessionInfo->callerToken = sessionInfo.callerToken_;
    abilitySessionInfo->persistentId = extSession->GetPersistentId();
    if (sessionInfo.want != nullptr) {
        abilitySessionInfo->want = *sessionInfo.want;
    }
    return abilitySessionInfo;
}

sptr<ExtensionSession> ExtensionSessionManager::RequestExtensionSession(const SessionInfo& sessionInfo)
{
    auto task = [this, sessionInfo]() {
        sptr<ExtensionSession> extensionSession = new (std::nothrow) ExtensionSession(sessionInfo);
        if (extensionSession == nullptr) {
            WLOGFE("extensionSession is nullptr!");
            return extensionSession;
        }
        auto persistentId = extensionSession->GetPersistentId();
        WLOGFI("create session persistentId: %{public}d, bundleName: %{public}s, abilityName: %{public}s",
            persistentId, sessionInfo.bundleName_.c_str(), sessionInfo.abilityName_.c_str());
        extensionSessionMap_.insert({ persistentId, extensionSession });
        return extensionSession;
    };

    return taskScheduler_->PostSyncTask(task);
}

WSError ExtensionSessionManager::RequestExtensionSessionActivation(const sptr<ExtensionSession>& extensionSession)
{
    wptr<ExtensionSession> weakExtSession(extensionSession);
    WSError ret = WSError::WS_OK;
    auto task = [this, weakExtSession, &ret]() {
        auto extSession = weakExtSession.promote();
        if (extSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = extSession->GetPersistentId();
        WLOGFI("Activate session with persistentId: %{public}d", persistentId);
        if (extensionSessionMap_.count(persistentId) == 0) {
            WLOGFE("Session is invalid!");
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto extSessionInfo = SetAbilitySessionInfo(extSession);
        if (extSessionInfo == nullptr) {
            return WSError::WS_ERROR_NULLPTR;
        }
        auto errorCode = AAFwk::AbilityManagerClient::GetInstance()->StartUIExtensionAbility(extSessionInfo,
            AAFwk::DEFAULT_INVAL_VALUE);
        ret = (errorCode == ERR_OK) ? WSError::WS_OK : WSError::WS_ERROR_START_UI_EXTENSION_ABILITY_FAILED;
        return ret;
    };
    taskScheduler_->PostSyncTask(task);
    return ret;
}

WSError ExtensionSessionManager::RequestExtensionSessionBackground(const sptr<ExtensionSession>& extensionSession)
{
    wptr<ExtensionSession> weakExtSession(extensionSession);
    WSError ret = WSError::WS_OK;
    auto task = [this, weakExtSession, &ret]() {
        auto extSession = weakExtSession.promote();
        if (extSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = extSession->GetPersistentId();
        WLOGFI("Background session with persistentId: %{public}d", persistentId);
        extSession->SetActive(false);
        extSession->Background();
        if (extensionSessionMap_.count(persistentId) == 0) {
            WLOGFE("Session is invalid!");
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto extSessionInfo = SetAbilitySessionInfo(extSession);
        if (!extSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
        auto errorCode = AAFwk::AbilityManagerClient::GetInstance()->MinimizeUIExtensionAbility(extSessionInfo);
        ret = (errorCode == ERR_OK) ? WSError::WS_OK : WSError::WS_ERROR_MIN_UI_EXTENSION_ABILITY_FAILED;
        return ret;
    };
    taskScheduler_->PostSyncTask(task);
    return ret;
}

WSError ExtensionSessionManager::RequestExtensionSessionDestruction(const sptr<ExtensionSession>& extensionSession)
{
    wptr<ExtensionSession> weakExtSession(extensionSession);
    WSError ret = WSError::WS_OK;
    auto task = [this, weakExtSession, &ret]() {
        auto extSession = weakExtSession.promote();
        if (extSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = extSession->GetPersistentId();
        WLOGFI("Destroy session with persistentId: %{public}d", persistentId);
        extSession->Disconnect();
        if (extensionSessionMap_.count(persistentId) == 0) {
            WLOGFE("Session is invalid!");
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto extSessionInfo = SetAbilitySessionInfo(extSession);
        if (!extSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
        auto errorCode = AAFwk::AbilityManagerClient::GetInstance()->TerminateUIExtensionAbility(extSessionInfo);
        extensionSessionMap_.erase(persistentId);
        ret = (errorCode == ERR_OK) ? WSError::WS_OK : WSError::WS_ERROR_TERMINATE_UI_EXTENSION_ABILITY_FAILED;
        return ret;
    };
    taskScheduler_->PostSyncTask(task);
    return ret;
}
} // namespace OHOS::Rosen
