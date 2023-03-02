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
}

WSError ExtensionSessionManager::Init()
{
    WLOGFI("extension session manager init.");
    if (mmsSchedulerInit_) {
        WLOGFW("mmsScheduler_ already init!");
        return WSError::WS_DO_NOTHING;
    }
    mmsScheduler_ = std::make_shared<MessageScheduler>(EXTENSION_SESSION_MANAGER_THREAD);
    if (!mmsScheduler_) {
        WLOGFE("new mmsScheduler_ failed!");
        return WSError::WS_ERROR_NULLPTR;
    }
    mmsSchedulerInit_ = true;
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
    abilitySessionInfo->sessionToken = extSession;
    abilitySessionInfo->surfaceNode = extSession->GetSurfaceNode();
    abilitySessionInfo->callerToken = sessionInfo.callerToken_;
    abilitySessionInfo->persistentId = extSession->GetPersistentId();
    return abilitySessionInfo;
}

sptr<ExtensionSession> ExtensionSessionManager::RequestExtensionSession(const SessionInfo& sessionInfo)
{
    if (!mmsSchedulerInit_) {
        WLOGFE("mmsScheduler_ not init!");
        return nullptr;
    }
    auto task = [this, sessionInfo]() {
        sptr<ExtensionSession> extensionSession = new (std::nothrow) ExtensionSession(sessionInfo);
        if (extensionSession == nullptr) {
            WLOGFE("extensionSession is nullptr!");
            return extensionSession;
        }
        uint64_t persistentId = GeneratePersistentId();
        extensionSession->SetPersistentId(persistentId);
        WLOGFI("create session persistentId: %{public}" PRIu64 ", bundleName: %{public}s, abilityName: %{public}s",
            persistentId, sessionInfo.bundleName_.c_str(), sessionInfo.abilityName_.c_str());
        extensionMap_.insert({ persistentId, extensionSession });
        return extensionSession;
    };
    // once init but mmsScheduler_ is nullptr
    WS_CHECK_NULL_SCHE_RETURN(mmsScheduler_, task);
    return mmsScheduler_->PostSyncTask(task);
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
        WLOGFI("active persistentId: %{public}" PRIu64 "", persistentId);
        if (extensionMap_.count(persistentId) == 0) {
            WLOGFE("session is invalid with %{public}" PRIu64 "", persistentId);
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
    WS_CHECK_NULL_SCHE_RETURN(mmsScheduler_, task);
    mmsScheduler_->PostAsyncTask(task);
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
        WLOGFI("background session persistentId: %{public}" PRIu64 "", persistentId);
        extSession->SetActive(false);
        extSession->Background();
        if (extensionMap_.count(persistentId) == 0) {
            WLOGFE("session is invalid with %{public}" PRIu64 "", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto extSessionInfo = SetAbilitySessionInfo(extSession);
        if (!extSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
        AAFwk::AbilityManagerClient::GetInstance()->MinimizeUIExtensionAbility(extSessionInfo);
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
        if (extSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = extSession->GetPersistentId();
        WLOGFI("destroy session persistentId: %{public}" PRIu64 "", persistentId);
        extSession->Disconnect();
        if (extensionMap_.count(persistentId) == 0) {
            WLOGFE("session is invalid with %{public}" PRIu64 "", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto extSessionInfo = SetAbilitySessionInfo(extSession);
        if (!extSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
        AAFwk::AbilityManagerClient::GetInstance()->TerminateUIExtensionAbility(extSessionInfo);
        extensionMap_.erase(persistentId);
        return WSError::WS_OK;
    };
    WS_CHECK_NULL_SCHE_RETURN(mmsScheduler_, task);
    mmsScheduler_->PostAsyncTask(task);
    return WSError::WS_OK;
}
} // namespace OHOS::Rosen
