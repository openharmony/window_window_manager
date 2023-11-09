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

#include "session/host/include/extension_session.h"

#include "ipc_skeleton.h"

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ExtensionSession" };
} // namespace

ExtensionSession::ExtensionSession(const SessionInfo& info) : Session(info)
{
    WLOGFD("Create extension session, bundleName: %{public}s, moduleName: %{public}s, abilityName: %{public}s.",
        info.bundleName_.c_str(), info.moduleName_.c_str(), info.abilityName_.c_str());
    GeneratePersistentId(true, info.persistentId_);
}

WSError ExtensionSession::Connect(
    const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
    sptr<WindowSessionProperty> property, sptr<IRemoteObject> token, int32_t pid, int32_t uid)
{
    // Get pid and uid before posting task.
    pid = pid == -1 ? IPCSkeleton::GetCallingPid() : pid;
    uid = uid == -1 ? IPCSkeleton::GetCallingUid() : uid;
    return PostSyncTask(
        [weakThis = wptr(this), sessionStage, eventChannel, surfaceNode, &systemConfig, property, token, pid, uid]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        return session->Session::Connect(
            sessionStage, eventChannel, surfaceNode, systemConfig, property, token, pid, uid);
    });
}

WSError ExtensionSession::TransferAbilityResult(uint32_t resultCode, const AAFwk::Want& want)
{
    if (extSessionEventCallback_ != nullptr &&
        extSessionEventCallback_->transferAbilityResultFunc_ != nullptr) {
        extSessionEventCallback_->transferAbilityResultFunc_(resultCode, want);
    }
    return WSError::WS_OK;
}

WSError ExtensionSession::TransferExtensionData(const AAFwk::WantParams& wantParams)
{
    if (extSessionEventCallback_ != nullptr &&
        extSessionEventCallback_->transferExtensionDataFunc_ != nullptr) {
        extSessionEventCallback_->transferExtensionDataFunc_(wantParams);
    }
    return WSError::WS_OK;
}

WSError ExtensionSession::TransferComponentData(const AAFwk::WantParams& wantParams)
{
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    sessionStage_->NotifyTransferComponentData(wantParams);
    return WSError::WS_OK;
}

WSErrorCode ExtensionSession::TransferComponentDataSync(const AAFwk::WantParams& wantParams,
                                                        AAFwk::WantParams& reWantParams)
{
    if (!IsSessionValid()) {
        return WSErrorCode::WS_ERROR_TRANSFER_DATA_FAILED;
    }
    return sessionStage_->NotifyTransferComponentDataSync(wantParams, reWantParams);
}

void ExtensionSession::NotifyRemoteReady()
{
    if (extSessionEventCallback_ != nullptr &&
        extSessionEventCallback_->notifyRemoteReadyFunc_ != nullptr) {
        extSessionEventCallback_->notifyRemoteReadyFunc_();
    }
}

void ExtensionSession::NotifySyncOn()
{
    if (extSessionEventCallback_ != nullptr &&
        extSessionEventCallback_->notifySyncOnFunc_ != nullptr) {
        extSessionEventCallback_->notifySyncOnFunc_();
    }
}

void ExtensionSession::NotifyAsyncOn()
{
    if (extSessionEventCallback_ != nullptr &&
        extSessionEventCallback_->notifyAsyncOnFunc_ != nullptr) {
        extSessionEventCallback_->notifyAsyncOnFunc_();
    }
}

void ExtensionSession::RegisterExtensionSessionEventCallback(
    const sptr<ExtensionSessionEventCallback>& extSessionEventCallback)
{
    extSessionEventCallback_ = extSessionEventCallback;
}

sptr<ExtensionSession::ExtensionSessionEventCallback> ExtensionSession::GetExtensionSessionEventCallback()
{
    if (extSessionEventCallback_ == nullptr) {
        extSessionEventCallback_ = new(std::nothrow) ExtensionSessionEventCallback();
    }

    return extSessionEventCallback_;
}
} // namespace OHOS::Rosen
