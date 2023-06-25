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
#include <want.h>

#include "session/host/include/extension_session.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ExtensionSessionManager" };
const std::string EXTENSION_SESSION_MANAGER_THREAD = "ExtensionSessionManager";
}

WM_IMPLEMENT_SINGLE_INSTANCE(ExtensionSessionManager)

ExtensionSessionManager::ExtensionSessionManager()
{
}

WSError ExtensionSessionManager::Init()
{
    if (taskScheduler_) {
        return WSError::WS_DO_NOTHING;
    }
    WLOGFI("Initialize extension session manager.");
    taskScheduler_ = std::make_shared<TaskScheduler>(EXTENSION_SESSION_MANAGER_THREAD);
    return WSError::WS_OK;
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
        WLOGFI("create session persistentId: %{public}" PRIu64 ", bundleName: %{public}s, abilityName: %{public}s",
            persistentId, sessionInfo.bundleName_.c_str(), sessionInfo.abilityName_.c_str());
        extensionSessionMap_.insert({ persistentId, extensionSession });
        return extensionSession;
    };

    return taskScheduler_->PostSyncTask(task);
}

WSError ExtensionSessionManager::RequestExtensionSessionActivation(const sptr<ExtensionSession>& extensionSession)
{
    wptr<ExtensionSession> weakExtSession(extensionSession);
    auto task = [this, weakExtSession]() {
        auto extSession = weakExtSession.promote();
        if (extSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = extSession->GetPersistentId();
        WLOGFI("Activate session with persistentId: %{public}" PRIu64, persistentId);
        if (extensionSessionMap_.count(persistentId) == 0) {
            WLOGFE("Session is invalid!");
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        AAFwk::Want want;
        auto sessionInfo = extSession->GetSessionInfo();
        want.SetElementName(sessionInfo.bundleName_, sessionInfo.abilityName_);
        AAFwk::StartOptions startOptions;
        auto extSessionInfo = SetAbilitySessionInfo(extSession);
        if (!extSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
        AAFwk::AbilityManagerClient::GetInstance()->StartUIExtensionAbility(want, extSessionInfo,
            AAFwk::DEFAULT_INVAL_VALUE,
            AppExecFwk::ExtensionAbilityType::UI);
        return WSError::WS_OK;
    };

    taskScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}

WSError ExtensionSessionManager::RequestExtensionSessionBackground(const sptr<ExtensionSession>& extensionSession)
{
    wptr<ExtensionSession> weakExtSession(extensionSession);
    auto task = [this, weakExtSession]() {
        auto extSession = weakExtSession.promote();
        if (extSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = extSession->GetPersistentId();
        WLOGFI("Background session with persistentId: %{public}" PRIu64, persistentId);
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
        AAFwk::AbilityManagerClient::GetInstance()->MinimizeUIExtensionAbility(extSessionInfo);
        return WSError::WS_OK;
    };

    taskScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}

WSError ExtensionSessionManager::RequestExtensionSessionDestruction(const sptr<ExtensionSession>& extensionSession)
{
    wptr<ExtensionSession> weakExtSession(extensionSession);
    auto task = [this, weakExtSession]() {
        auto extSession = weakExtSession.promote();
        if (extSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = extSession->GetPersistentId();
        WLOGFI("Destroy session with persistentId: %{public}" PRIu64, persistentId);
        extSession->Disconnect();
        if (extensionSessionMap_.count(persistentId) == 0) {
            WLOGFE("Session is invalid!");
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto extSessionInfo = SetAbilitySessionInfo(extSession);
        if (!extSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
        AAFwk::AbilityManagerClient::GetInstance()->TerminateUIExtensionAbility(extSessionInfo);
        extensionSessionMap_.erase(persistentId);
        return WSError::WS_OK;
    };

    taskScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}
} // namespace OHOS::Rosen
