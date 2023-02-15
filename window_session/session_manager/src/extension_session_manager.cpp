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

#include "extension_session_manager.h"

#include <ability_manager_client.h>
#include <want.h>
#include <start_options.h>

#include "extension_session.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "ExtensionSessionManager"};
}

WM_IMPLEMENT_SINGLE_INSTANCE(ExtensionSessionManager)

sptr<ExtensionSession> ExtensionSessionManager::RequestExtensionSession(const WindowSession::AbilityInfo& abilityInfo)
{
    WLOGFI("abilityInfo: bundleName: %{public}s, abilityName: %{public}s", abilityInfo.bundleName_.c_str(),
        abilityInfo.abilityName_.c_str());
    sptr<ExtensionSession> extensionSession = new (std::nothrow) ExtensionSession(abilityInfo);
    ++sessionId_;
    uint32_t persistentId = pid_ + sessionId_;
    extensionSession->SetPersistentId(persistentId);
    WLOGFI("create session persistentId: %{public}u", persistentId);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    abilityExtensionMap_.insert({persistentId, std::make_pair(extensionSession, nullptr)});
    return extensionSession;
}

WSError ExtensionSessionManager::RequestExtensionSessionActivation(const sptr<ExtensionSession>& extensionSession)
{
    if(extensionSession == nullptr) {
        WLOGFE("session is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    auto persistentId = extensionSession->GetPersistentId();
    WLOGFI("active persistentId: %{public}u", persistentId);
    if (abilityExtensionMap_.count(persistentId) == 0) {
        WLOGFE("session is invalid with %{public}u", persistentId);
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    AAFwk::Want want;
    auto abilityInfo = extensionSession->GetAbilityInfo();
    want.SetElementName(abilityInfo.bundleName_, abilityInfo.abilityName_);
    AAFwk::StartOptions startOptions;
    // to start uiExtension ability with (want, callerToken, extensionSession, surfaceNode, extensionType)
    // TODO AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want, startOptions, abilityInfo->callerToken_);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    sptr<IRemoteObject> newAbilityToken = nullptr;
    // replace with real token after start ability
    abilityExtensionMap_[persistentId].second = newAbilityToken;
    return WSError::WS_OK;
}

WSError ExtensionSessionManager::RequestExtensionSessionBackground(const sptr<ExtensionSession>& extensionSession)
{
    if(extensionSession == nullptr) {
        WLOGFE("session is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    auto persistentId = extensionSession->GetPersistentId();
    WLOGFI("background session persistentId: %{public}u", persistentId);
    extensionSession->Background();
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (abilityExtensionMap_.count(persistentId) == 0) {
        WLOGFE("session is invalid with %{public}u", persistentId);
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    auto abilityToken = abilityExtensionMap_[persistentId].second;
    AAFwk::AbilityManagerClient::GetInstance()->MinimizeAbility(abilityToken);
    return WSError::WS_OK;
}

WSError ExtensionSessionManager::RequestExtensionSessionDestruction(const sptr<ExtensionSession>& extensionSession)
{
    if(extensionSession == nullptr) {
        WLOGFE("session is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    auto persistentId = extensionSession->GetPersistentId();
    WLOGFI("destroy session persistentId: %{public}u", persistentId);
    extensionSession->Disconnect();
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (abilityExtensionMap_.count(persistentId) == 0) {
        WLOGFE("session is invalid with %{public}u", persistentId);
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    auto abilityToken = abilityExtensionMap_[persistentId].second;
    AAFwk::Want resultWant;
    AAFwk::AbilityManagerClient::GetInstance()->TerminateAbility(abilityToken, -1, &resultWant);
    abilityExtensionMap_.erase(persistentId);
    return WSError::WS_OK;
}
} // namespace OHOS::Rosen
