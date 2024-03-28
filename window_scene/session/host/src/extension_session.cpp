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
#include "anr_manager.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ExtensionSession" };
} // namespace

void WindowEventChannelListener::SetTransferKeyEventForConsumedParams(
    const std::shared_ptr<std::promise<bool>>& isConsumedPromise, const std::shared_ptr<WSError>& retCode)
{
    std::lock_guard<std::mutex> lock(transferKeyEventForConsumedMutex_);
    isConsumedPromise_ = isConsumedPromise;
    retCode_ = retCode;
}

void WindowEventChannelListener::ResetTransferKeyEventForConsumedParams()
{
    std::lock_guard<std::mutex> lock(transferKeyEventForConsumedMutex_);
    isConsumedPromise_ = nullptr;
    retCode_ = nullptr;
}

void WindowEventChannelListener::OnTransferKeyEventForConsumed(bool isConsumed, WSError retCode)
{
    std::lock_guard<std::mutex> lock(transferKeyEventForConsumedMutex_);
    if (isConsumedPromise_ == nullptr || retCode_ == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "Promise or ret is null in WindowEventChannelListener.");
        return;
    }
    isConsumedPromise_->set_value(isConsumed);
    *retCode_ = retCode;
}

int32_t WindowEventChannelListener::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        TLOGE(WmsLogTag::WMS_EVENT, "InterfaceToken check failed");
        return -1;
    }

    auto msgId = static_cast<WindowEventChannelListenerMessage>(code);
    switch (msgId) {
        case WindowEventChannelListenerMessage::TRANS_ID_ON_TRANSFER_KEY_EVENT_FOR_CONSUMED_ASYNC: {
            bool isConsumed = data.ReadBool();
            WSError retCode = static_cast<WSError>(data.ReadInt32());
            OnTransferKeyEventForConsumed(isConsumed, retCode);
            break;
        }
        default:
            TLOGE(WmsLogTag::WMS_EVENT, "unknown transaction code %{public}d", code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return 0;
}

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
    pid = pid == -1 ? IPCSkeleton::GetCallingRealPid() : pid;
    uid = uid == -1 ? IPCSkeleton::GetCallingUid() : uid;
    auto task = [weakThis = wptr(this), sessionStage, eventChannel, surfaceNode, &systemConfig, property, token, pid, uid]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        return session->Session::Connect(
            sessionStage, eventChannel, surfaceNode, systemConfig, property, token, pid, uid);
    };
    return PostSyncTask(task, "Connect");
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

void ExtensionSession::TriggerBindModalUIExtension()
{
    if (isFirstTriggerBindModal_ && extSessionEventCallback_ != nullptr &&
        extSessionEventCallback_->notifyBindModalFunc_ != nullptr) {
        WLOGFD("Start calling bind modal func.");
        extSessionEventCallback_->notifyBindModalFunc_();
        isFirstTriggerBindModal_ = false;
    }
}

void ExtensionSession::RegisterExtensionSessionEventCallback(
    const sptr<ExtensionSessionEventCallback>& extSessionEventCallback)
{
    extSessionEventCallback_ = extSessionEventCallback;
}

WSError ExtensionSession::TransferKeyEventForConsumed(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed,
    bool& isTimeout, bool isPreImeEvent)
{
    if (!windowEventChannel_) {
        TLOGE(WmsLogTag::WMS_EVENT, "windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (keyEvent == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "KeyEvent is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    TLOGD(WmsLogTag::WMS_EVENT, "TransferKeyEventForConsumed in with isConsumed(%{public}d) isTimeout(%{public}d) "
        "isPreImeEvent(%{public}d), id:%{public}d", isConsumed, isTimeout, isPreImeEvent, keyEvent->GetId());

    sptr<WindowEventChannelListener> listener = new WindowEventChannelListener();
    auto isConsumedPromise = std::make_shared<std::promise<bool>>();
    std::shared_ptr<WSError> retCode = std::make_shared<WSError>(WSError::WS_OK);
    bool isAllocedNullptr = (listener == nullptr) || (isConsumedPromise == nullptr) || (retCode == nullptr);
    if (isAllocedNullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "Created WindowEventChannelListener is nullptr.");
        return WSError::WS_ERROR_NULLPTR;
    }
    listener->SetTransferKeyEventForConsumedParams(isConsumedPromise, retCode);
    auto ret = windowEventChannel_->TransferKeyEventForConsumedAsync(keyEvent, isPreImeEvent, listener);

    // Timeout cannot exceed APP_INPUT_BLOCK
    constexpr int64_t TRANSFER_KEY_EVENT_TIMEOUT_TIME_MS = 4000;
    auto isConsumedFuture = isConsumedPromise->get_future().share();
    if (isConsumedFuture.wait_for(std::chrono::milliseconds(TRANSFER_KEY_EVENT_TIMEOUT_TIME_MS)) ==
            std::future_status::timeout) {
        // Prevent external variables from being used after the lifecycle ends.
        listener->ResetTransferKeyEventForConsumedParams();
        isTimeout = true;
    } else {
        isTimeout = false;
        isConsumed = isConsumedFuture.get();
        ret = *retCode;
    }
    TLOGD(WmsLogTag::WMS_EVENT, "isConsumed is %{public}d, Timeout is %{public}d, ret is %{public}d in id:%{public}d.",
        isConsumed, isTimeout, ret, keyEvent->GetId());
    return ret;
}

sptr<ExtensionSession::ExtensionSessionEventCallback> ExtensionSession::GetExtensionSessionEventCallback()
{
    if (extSessionEventCallback_ == nullptr) {
        extSessionEventCallback_ = new(std::nothrow) ExtensionSessionEventCallback();
    }

    return extSessionEventCallback_;
}

WSError ExtensionSession::TransferAccessibilityEvent(const Accessibility::AccessibilityEventInfo& info,
    int64_t uiExtensionIdLevel)
{
    NotifyTransferAccessibilityEvent(info, uiExtensionIdLevel);
    return WSError::WS_OK;
}

WSError ExtensionSession::UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type)
{
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    return sessionStage_->UpdateAvoidArea(avoidArea, type);
}

AvoidArea ExtensionSession::GetAvoidAreaByType(AvoidAreaType type)
{
    Rosen::AvoidArea avoidArea;
    if (extSessionEventCallback_ != nullptr && extSessionEventCallback_->notifyGetAvoidAreaByTypeFunc_ != nullptr) {
        avoidArea = extSessionEventCallback_->notifyGetAvoidAreaByTypeFunc_(type);
    }
    return avoidArea;
}

WSError ExtensionSession::Background()
{
    SessionState state = GetSessionState();
    TLOGI(WmsLogTag::WMS_LIFE, "Background ExtensionSession, id: %{public}d, state: %{public}" PRIu32"",
        GetPersistentId(), static_cast<uint32_t>(state));
    if (state == SessionState::STATE_ACTIVE && GetWindowType() == WindowType::WINDOW_TYPE_UI_EXTENSION) {
        UpdateSessionState(SessionState::STATE_INACTIVE);
        state = SessionState::STATE_INACTIVE;
        isActive_ = false;
    }
    if (state != SessionState::STATE_INACTIVE) {
        WLOGFW("[WMSLife] Background state invalid! state:%{public}u", state);
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    UpdateSessionState(SessionState::STATE_BACKGROUND);
    NotifyBackground();
    DelayedSingleton<ANRManager>::GetInstance()->OnBackground(persistentId_);
    return WSError::WS_OK;
}
} // namespace OHOS::Rosen
