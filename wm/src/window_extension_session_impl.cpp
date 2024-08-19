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

#include "window_extension_session_impl.h"

#include <transaction/rs_interfaces.h>
#include <transaction/rs_transaction.h>
#ifdef IMF_ENABLE
#include <input_method_controller.h>
#endif
#include "window_manager_hilog.h"
#include "display_info.h"
#include "parameters.h"
#include "anr_handler.h"
#include "hitrace_meter.h"
#include "perform_reporter.h"
#include "session_permission.h"
#include "singleton_container.h"
#include "window_adapter.h"
#include "input_transfer_station.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowExtensionSessionImpl"};
constexpr int64_t DISPATCH_KEY_EVENT_TIMEOUT_TIME_MS = 1000;
constexpr int32_t UIEXTENTION_ROTATION_ANIMATION_TIME = 400;
}

#define CHECK_HOST_SESSION_RETURN_IF_NULL(hostSession)                         \
    do {                                                                       \
        if ((hostSession) == nullptr) {                                        \
            TLOGE(WmsLogTag::DEFAULT, "hostSession is null");                  \
            return;                                                            \
        }                                                                      \
    } while (false)

#define CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, ret)              \
    do {                                                                       \
        if ((hostSession) == nullptr) {                                        \
            TLOGE(WmsLogTag::DEFAULT, "hostSession is null");                  \
            return ret;                                                        \
        }                                                                      \
    } while (false)

std::set<sptr<WindowSessionImpl>> WindowExtensionSessionImpl::windowExtensionSessionSet_;
std::shared_mutex WindowExtensionSessionImpl::windowExtensionSessionMutex_;

WindowExtensionSessionImpl::WindowExtensionSessionImpl(const sptr<WindowOption>& option) : WindowSessionImpl(option)
{
    if (property_->GetUIExtensionUsage() == UIExtensionUsage::MODAL ||
        property_->GetUIExtensionUsage() == UIExtensionUsage::CONSTRAINED_EMBEDDED) {
        extensionWindowFlags_.hideNonSecureWindowsFlag = true;
    }
    TLOGI(WmsLogTag::WMS_UIEXT, "UIExtension usage=%{public}u, the default state of hideNonSecureWindows is %{public}d",
        property_->GetUIExtensionUsage(), extensionWindowFlags_.hideNonSecureWindowsFlag);
}

WindowExtensionSessionImpl::~WindowExtensionSessionImpl()
{
    WLOGFI("[WMSCom] %{public}d, %{public}s", GetPersistentId(), GetWindowName().c_str());
}

WMError WindowExtensionSessionImpl::Create(const std::shared_ptr<AbilityRuntime::Context>& context,
    const sptr<Rosen::ISession>& iSession, const std::string& identityToken)
{
    TLOGD(WmsLogTag::WMS_LIFE, "Called.");
    if (!context || !iSession) {
        TLOGE(WmsLogTag::WMS_LIFE, "context is nullptr: %{public}u or sessionToken is nullptr: %{public}u",
            context == nullptr, iSession == nullptr);
        return WMError::WM_ERROR_NULLPTR;
    }
    if (vsyncStation_ == nullptr || !vsyncStation_->IsVsyncReceiverCreated()) {
        return WMError::WM_ERROR_NULLPTR;
    }
    SetDefaultDisplayIdIfNeed();
    {
        std::lock_guard<std::mutex> lock(hostSessionMutex_);
        hostSession_ = iSession;
    }
    context_ = context;
    if (context_) {
        abilityToken_ = context_->GetToken();
    }
    AddExtensionWindowStageToSCB();
    WMError ret = Connect();
    if (ret == WMError::WM_OK) {
        MakeSubOrDialogWindowDragableAndMoveble();
        std::unique_lock<std::shared_mutex> lock(windowExtensionSessionMutex_);
        windowExtensionSessionSet_.insert(this);
    }
    state_ = WindowState::STATE_CREATED;
    isUIExtensionAbilityProcess_ = true;
    TLOGI(WmsLogTag::WMS_LIFE, "Created name:%{public}s %{public}d successfully.",
        property_->GetWindowName().c_str(), GetPersistentId());
    sptr<Window> self(this);
    InputTransferStation::GetInstance().AddInputWindow(self);
    needRemoveWindowInputChannel_ = true;
    AddSetUIContentTimeoutCheck();
    return WMError::WM_OK;
}

void WindowExtensionSessionImpl::AddExtensionWindowStageToSCB()
{
    if (!abilityToken_) {
        TLOGE(WmsLogTag::WMS_UIEXT, "token is nullptr");
        return;
    }
    if (surfaceNode_ == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "surfaceNode_ is nullptr");
        return;
    }

    SingletonContainer::Get<WindowAdapter>().AddExtensionWindowStageToSCB(sptr<ISessionStage>(this), abilityToken_,
        surfaceNode_->GetId());
}

void WindowExtensionSessionImpl::RemoveExtensionWindowStageFromSCB()
{
    if (!abilityToken_) {
        TLOGE(WmsLogTag::WMS_UIEXT, "token is nullptr");
        return;
    }

    SingletonContainer::Get<WindowAdapter>().RemoveExtensionWindowStageFromSCB(sptr<ISessionStage>(this),
        abilityToken_);
}

void WindowExtensionSessionImpl::UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    if (auto uiContent = GetUIContentSharedPtr()) {
        WLOGFD("notify ace winId:%{public}u", GetWindowId());
        uiContent->UpdateConfiguration(configuration);
    }
}

void WindowExtensionSessionImpl::UpdateConfigurationForAll(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    WLOGD("notify scene ace update config");
    std::unique_lock<std::shared_mutex> lock(windowExtensionSessionMutex_);
    for (const auto& window : windowExtensionSessionSet_) {
        window->UpdateConfiguration(configuration);
    }
}

WMError WindowExtensionSessionImpl::Destroy(bool needNotifyServer, bool needClearListener)
{
    TLOGI(WmsLogTag::WMS_LIFE, "id:%{public}d Destroy, state:%{public}u, needNotifyServer:%{public}d, "
        "needClearListener:%{public}d", GetPersistentId(), state_, needNotifyServer, needClearListener);
    if (needRemoveWindowInputChannel_) {
        TLOGI(WmsLogTag::WMS_LIFE, "Id:%{public}d Destroy", GetPersistentId());
        InputTransferStation::GetInstance().RemoveInputWindow(GetPersistentId());
        needRemoveWindowInputChannel_ = false;
    }
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (auto hostSession = GetHostSession()) {
        TLOGI(WmsLogTag::WMS_LIFE, "Disconnected with host session, id: %{public}d.", GetPersistentId());
        hostSession->Disconnect();
    }
    NotifyBeforeDestroy(GetWindowName());
    if (needClearListener) {
        ClearListenersById(GetPersistentId());
    }
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        state_ = WindowState::STATE_DESTROYED;
        requestState_ = WindowState::STATE_DESTROYED;
    }
    {
        TLOGI(WmsLogTag::WMS_LIFE, "Reset state, id: %{public}d.", GetPersistentId());
        std::lock_guard<std::mutex> lock(hostSessionMutex_);
        hostSession_ = nullptr;
    }
    {
        std::unique_lock<std::shared_mutex> lock(windowExtensionSessionMutex_);
        windowExtensionSessionSet_.erase(this);
    }
    TLOGI(WmsLogTag::WMS_LIFE, "Erase windowExtensionSession in set, id: %{public}d.", GetPersistentId());
    if (context_) {
        context_.reset();
    }
    ClearVsyncStation();
    SetUIContentComplete();
    RemoveExtensionWindowStageFromSCB();
    TLOGI(WmsLogTag::WMS_LIFE, "Destroyed successfully, id: %{public}d.", GetPersistentId());
    return WMError::WM_OK;
}

WMError WindowExtensionSessionImpl::MoveTo(int32_t x, int32_t y)
{
    WLOGFD("Id:%{public}d MoveTo %{public}d %{public}d", property_->GetPersistentId(), x, y);
    if (IsWindowSessionInvalid()) {
        WLOGFE("Window session invalid.");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    const auto& rect = property_->GetWindowRect();
    WSRect wsRect = { x, y, rect.width_, rect.height_ };
    WSError error = UpdateRect(wsRect, SizeChangeReason::MOVE);
    return static_cast<WMError>(error);
}

WMError WindowExtensionSessionImpl::Resize(uint32_t width, uint32_t height)
{
    WLOGFD("Id:%{public}d Resize %{public}u %{public}u", property_->GetPersistentId(), width, height);
    if (IsWindowSessionInvalid()) {
        WLOGFE("Window session invalid.");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    const auto& rect = property_->GetWindowRect();
    WSRect wsRect = { rect.posX_, rect.posY_, width, height };
    WSError error = UpdateRect(wsRect, SizeChangeReason::RESIZE);
    return static_cast<WMError>(error);
}

WMError WindowExtensionSessionImpl::TransferAbilityResult(uint32_t resultCode, const AAFwk::Want& want)
{
    if (IsWindowSessionInvalid()) {
        WLOGFE("Window session invalid.");
        return WMError::WM_ERROR_REPEAT_OPERATION;
    }
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_INVALID_WINDOW);
    return static_cast<WMError>(hostSession->TransferAbilityResult(resultCode, want));
}

WMError WindowExtensionSessionImpl::TransferExtensionData(const AAFwk::WantParams& wantParams)
{
    if (IsWindowSessionInvalid()) {
        WLOGFE("Window session invalid.");
        return WMError::WM_ERROR_REPEAT_OPERATION;
    }
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_INVALID_WINDOW);
    return static_cast<WMError>(hostSession->TransferExtensionData(wantParams));
}

void WindowExtensionSessionImpl::RegisterTransferComponentDataListener(const NotifyTransferComponentDataFunc& func)
{
    if (IsWindowSessionInvalid()) {
        WLOGFE("Window session invalid.");
        return;
    }
    notifyTransferComponentDataFunc_ = std::move(func);
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_IF_NULL(hostSession);
    hostSession->NotifyAsyncOn();
}

WSError WindowExtensionSessionImpl::NotifyTransferComponentData(const AAFwk::WantParams& wantParams)
{
    TLOGD(WmsLogTag::WMS_UIEXT, "Called.");
    if (notifyTransferComponentDataFunc_) {
        notifyTransferComponentDataFunc_(wantParams);
    }
    return WSError::WS_OK;
}

WSErrorCode WindowExtensionSessionImpl::NotifyTransferComponentDataSync(
    const AAFwk::WantParams& wantParams, AAFwk::WantParams& reWantParams)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "called");
    if (notifyTransferComponentDataForResultFunc_) {
        reWantParams = notifyTransferComponentDataForResultFunc_(wantParams);
        return WSErrorCode::WS_OK;
    }
    return WSErrorCode::WS_ERROR_NOT_REGISTER_SYNC_CALLBACK;
}

void WindowExtensionSessionImpl::RegisterTransferComponentDataForResultListener(
    const NotifyTransferComponentDataForResultFunc& func)
{
    if (IsWindowSessionInvalid()) {
        WLOGFE("session invalid.");
        return;
    }
    notifyTransferComponentDataForResultFunc_ = std::move(func);
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_IF_NULL(hostSession);
    hostSession->NotifySyncOn();
}

void WindowExtensionSessionImpl::TriggerBindModalUIExtension()
{
    WLOGFD("called");
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_IF_NULL(hostSession);
    hostSession->TriggerBindModalUIExtension();
}

WMError WindowExtensionSessionImpl::SetPrivacyMode(bool isPrivacyMode)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "persistentId: %{public}u, isPrivacyMode: %{public}u", GetPersistentId(),
        isPrivacyMode);
    if (surfaceNode_ == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "surfaceNode_ is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    surfaceNode_->SetSecurityLayer(isPrivacyMode);
    RSTransaction::FlushImplicitTransaction();

    if (state_ != WindowState::STATE_SHOWN) {
        extensionWindowFlags_.privacyModeFlag = isPrivacyMode;
        return WMError::WM_OK;
    }
    if (isPrivacyMode == extensionWindowFlags_.privacyModeFlag) {
        return WMError::WM_OK;
    }

    auto updateFlags = extensionWindowFlags_;
    updateFlags.privacyModeFlag = isPrivacyMode;
    ExtensionWindowFlags actions(0);
    actions.privacyModeFlag = true;
    auto ret = UpdateExtWindowFlags(updateFlags, actions);
    if (ret == WMError::WM_OK) {
        extensionWindowFlags_ = updateFlags;
    }
    return ret;
}

void WindowExtensionSessionImpl::NotifyFocusStateEvent(bool focusState)
{
    if (auto uiContent = GetUIContentSharedPtr()) {
        focusState ? uiContent->Focus() : uiContent->UnFocus();
    }
    if (focusState) {
        NotifyWindowAfterFocused();
    } else {
        NotifyWindowAfterUnfocused();
    }
    focusState_ = focusState;
    if (focusState_ != std::nullopt) {
        TLOGI(WmsLogTag::WMS_FOCUS, "persistentId:%{public}d focusState:%{public}d",
            GetPersistentId(), static_cast<int32_t>(focusState_.value()));
    }
}

void WindowExtensionSessionImpl::NotifyFocusActiveEvent(bool isFocusActive)
{
    if (auto uiContent = GetUIContentSharedPtr()) {
        uiContent->SetIsFocusActive(isFocusActive);
    }
}

void WindowExtensionSessionImpl::NotifyBackpressedEvent(bool& isConsumed)
{
    if (auto uiContent = GetUIContentSharedPtr()) {
        WLOGFD("Transfer backpressed event to uiContent");
        isConsumed = uiContent->ProcessBackPressed();
    }
    WLOGFD("Backpressed event is consumed %{public}d", isConsumed);
}

void WindowExtensionSessionImpl::InputMethodKeyEventResultCallback(const std::shared_ptr<MMI::KeyEvent>& keyEvent,
    bool consumed, std::shared_ptr<std::promise<bool>> isConsumedPromise, std::shared_ptr<bool> isTimeout)
{
    if (keyEvent == nullptr) {
        WLOGFW("keyEvent is null, consumed:%{public}" PRId32, consumed);
        if (isConsumedPromise != nullptr) {
            isConsumedPromise->set_value(consumed);
        }
        return;
    }

    auto id = keyEvent->GetId();
    if (isConsumedPromise == nullptr || isTimeout == nullptr) {
        WLOGFW("Shared point isConsumedPromise or isTimeout is null, id:%{public}" PRId32, id);
        keyEvent->MarkProcessed();
        return;
    }

    if (*isTimeout) {
        WLOGFW("DispatchKeyEvent timeout id:%{public}" PRId32, id);
        keyEvent->MarkProcessed();
        return;
    }

    if (consumed) {
        isConsumedPromise->set_value(consumed);
        WLOGD("Input method has processed key event, id:%{public}" PRId32, id);
        return;
    }

    bool isConsumed = false;
    DispatchKeyEventCallback(const_cast<std::shared_ptr<MMI::KeyEvent>&>(keyEvent), isConsumed);
    isConsumedPromise->set_value(isConsumed);
}

void WindowExtensionSessionImpl::NotifyKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed,
    bool notifyInputMethod)
{
    if (keyEvent == nullptr) {
        WLOGFE("keyEvent is nullptr");
        return;
    }

#ifdef IMF_ENABLE
    bool isKeyboardEvent = IsKeyboardEvent(keyEvent);
    if (isKeyboardEvent && notifyInputMethod) {
        WLOGD("Async dispatch keyEvent to input method, id:%{public}" PRId32, keyEvent->GetId());
        auto isConsumedPromise = std::make_shared<std::promise<bool>>();
        auto isConsumedFuture = isConsumedPromise->get_future().share();
        auto isTimeout = std::make_shared<bool>(false);
        auto ret = MiscServices::InputMethodController::GetInstance()->DispatchKeyEvent(keyEvent,
            [this, isConsumedPromise, isTimeout](const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool consumed) {
                this->InputMethodKeyEventResultCallback(keyEvent, consumed, isConsumedPromise, isTimeout);
            });
        if (ret != 0) {
            WLOGFW("DispatchKeyEvent failed, ret:%{public}" PRId32 ", id:%{public}" PRId32, ret, keyEvent->GetId());
            DispatchKeyEventCallback(keyEvent, isConsumed);
            return;
        }
        if (isConsumedFuture.wait_for(std::chrono::milliseconds(DISPATCH_KEY_EVENT_TIMEOUT_TIME_MS)) ==
            std::future_status::timeout) {
            *isTimeout = true;
            isConsumed = true;
            WLOGFE("DispatchKeyEvent timeout, id:%{public}" PRId32, keyEvent->GetId());
        } else {
            isConsumed = isConsumedFuture.get();
        }
        WLOGFD("Input Method DispatchKeyEvent isConsumed:%{public}" PRId32, isConsumed);
        return;
    }
#endif // IMF_ENABLE
    DispatchKeyEventCallback(keyEvent, isConsumed);
}

void WindowExtensionSessionImpl::ArkUIFrameworkSupport()
{
    uint32_t version = 0;
    if ((context_ != nullptr) && (context_->GetApplicationInfo() != nullptr)) {
        version = context_->GetApplicationInfo()->apiCompatibleVersion;
    }
    // 10 ArkUI new framework support after API10
    if (version < 10) {
        SetLayoutFullScreenByApiVersion(isIgnoreSafeArea_);
        if (!isSystembarPropertiesSet_) {
            SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, SystemBarProperty());
        }
    } else if (isIgnoreSafeAreaNeedNotify_) {
        SetLayoutFullScreenByApiVersion(isIgnoreSafeArea_);
    }
}

WMError WindowExtensionSessionImpl::NapiSetUIContent(const std::string& contentInfo, napi_env env, napi_value storage,
    BackupAndRestoreType type, sptr<IRemoteObject> token, AppExecFwk::Ability* ability)
{
    WLOGFD("%{public}s state:%{public}u", contentInfo.c_str(), state_);
    if (auto uiContent = GetUIContentSharedPtr()) {
        uiContent->Destroy();
    }
    {
        std::unique_ptr<Ace::UIContent> uiContent;
        if (ability != nullptr) {
            uiContent = Ace::UIContent::Create(ability);
        } else {
            uiContent = Ace::UIContent::Create(context_.get(), reinterpret_cast<NativeEngine*>(env));
        }
        if (uiContent == nullptr) {
            WLOGFE("fail to NapiSetUIContent id: %{public}d", GetPersistentId());
            return WMError::WM_ERROR_NULLPTR;
        }
        uiContent->SetParentToken(token);
        if (property_->GetUIExtensionUsage() == UIExtensionUsage::CONSTRAINED_EMBEDDED) {
            uiContent->SetUIContentType(Ace::UIContentType::SECURITY_UI_EXTENSION);
        }
        uiContent->Initialize(this, contentInfo, storage, property_->GetParentId());
        // make uiContent available after Initialize/Restore
        std::unique_lock<std::shared_mutex> lock(uiContentMutex_);
        uiContent_ = std::move(uiContent);
    }
    SetUIContentComplete();

    UpdateAccessibilityTreeInfo();
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (focusState_ != std::nullopt) {
        focusState_.value() ? uiContent->Focus() : uiContent->UnFocus();
    }
    ArkUIFrameworkSupport();
    UpdateDecorEnable(true);
    if (state_ == WindowState::STATE_SHOWN) {
        // UIContent may be nullptr when show window, need to notify again when window is shown
        uiContent->Foreground();
        UpdateTitleButtonVisibility();
    }
    UpdateViewportConfig(GetRect(), WindowSizeChangeReason::UNDEFINED);
    WLOGFD("notify uiContent window size change end");
    return WMError::WM_OK;
}

WSError WindowExtensionSessionImpl::UpdateRect(const WSRect& rect, SizeChangeReason reason,
    const std::shared_ptr<RSTransaction>& rsTransaction)
{
    auto wmReason = static_cast<WindowSizeChangeReason>(reason);
    Rect wmRect = {rect.posX_, rect.posY_, rect.width_, rect.height_};
    auto preRect = GetRect();
    if (rect.width_ == static_cast<int>(preRect.width_) && rect.height_ == static_cast<int>(preRect.height_)) {
        WLOGFD("Update rect [%{public}d, %{public}d, reason: %{public}d]", rect.width_,
            rect.height_, static_cast<int>(reason));
    } else {
        WLOGFI("Update rect [%{public}d, %{public}d, reason: %{public}d]", rect.width_,
            rect.height_, static_cast<int>(reason));
    }
    property_->SetWindowRect(wmRect);

    if (property_->GetUIExtensionUsage() == UIExtensionUsage::MODAL) {
        if (!abilityToken_) {
            TLOGE(WmsLogTag::WMS_UIEXT, "token is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        SingletonContainer::Get<WindowAdapter>().UpdateModalExtensionRect(abilityToken_, wmRect);
    }

    if (wmReason == WindowSizeChangeReason::ROTATION) {
        UpdateRectForRotation(wmRect, preRect, wmReason, rsTransaction);
    } else if (handler_ != nullptr) {
        UpdateRectForOtherReason(wmRect, wmReason);
    } else {
        NotifySizeChange(wmRect, wmReason);
        UpdateViewportConfig(wmRect, wmReason);
    }
    return WSError::WS_OK;
}

void WindowExtensionSessionImpl::UpdateRectForRotation(const Rect& wmRect, const Rect& preRect,
    WindowSizeChangeReason wmReason, const std::shared_ptr<RSTransaction>& rsTransaction)
{
    if (!handler_) {
        return;
    }
    auto task = [weak = wptr(this), wmReason, wmRect, preRect, rsTransaction]() mutable {
        HITRACE_METER_NAME(HITRACE_TAG_WINDOW_MANAGER, "WindowExtensionSessionImpl::UpdateRectForRotation");
        auto window = weak.promote();
        if (!window) {
            return;
        }
        int32_t duration = UIEXTENTION_ROTATION_ANIMATION_TIME;
        bool needSync = false;
        if (rsTransaction && rsTransaction->GetSyncId() > 0) {
            // extract high 32 bits of SyncId as pid
            auto SyncTransactionPid = static_cast<int32_t>(rsTransaction->GetSyncId() >> 32);
            if (rsTransaction->IsOpenSyncTransaction() || SyncTransactionPid != rsTransaction->GetParentPid()) {
                needSync = true;
            }
        }

        if (needSync) {
            duration = rsTransaction->GetDuration() ? rsTransaction->GetDuration() : duration;
            RSTransaction::FlushImplicitTransaction();
            rsTransaction->Begin();
        }
        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(duration);
        // animation curve: cubic [0.2, 0.0, 0.2, 1.0]
        auto curve = RSAnimationTimingCurve::CreateCubicCurve(0.2, 0.0, 0.2, 1.0);
        RSNode::OpenImplicitAnimation(protocol, curve);
        if (wmRect != preRect) {
            window->NotifySizeChange(wmRect, wmReason);
        }
        window->UpdateViewportConfig(wmRect, wmReason, rsTransaction);
        RSNode::CloseImplicitAnimation();
        if (needSync) {
            rsTransaction->Commit();
        } else {
            RSTransaction::FlushImplicitTransaction();
        }
    };
    handler_->PostTask(task, "WMS_WindowExtensionSessionImpl_UpdateRectForRotation");
}

void WindowExtensionSessionImpl::UpdateRectForOtherReason(const Rect& wmRect, WindowSizeChangeReason wmReason)
{
    auto task = [weak = wptr(this), wmReason, wmRect] {
        auto window = weak.promote();
        if (!window) {
            TLOGE(WmsLogTag::WMS_LAYOUT, "window is null, updateViewPortConfig failed");
            return;
        }
        window->NotifySizeChange(wmRect, wmReason);
        window->UpdateViewportConfig(wmRect, wmReason);
    };
    if (handler_) {
        handler_->PostTask(task, "WMS_WindowExtensionSessionImpl_UpdateRectForOtherReason");
    }
}

WSError WindowExtensionSessionImpl::UpdateSessionViewportConfig(const SessionViewportConfig& config)
{
    if (std::islessequal(config.density_, 0.0f)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "invalid density_: %{public}f", config.density_);
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    if (!handler_) {
        TLOGE(WmsLogTag::WMS_UIEXT, "handler_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    auto task = [weak = wptr(this), config]() {
        auto window = weak.promote();
        if (!window) {
            return;
        }
        if (window->property_ == nullptr) {
            TLOGE(WmsLogTag::WMS_UIEXT, "property_ is null");
            return;
        }
        auto viewportConfig = config;
        viewportConfig.density_ = window->UpdateExtensionDensity(config);
        TLOGI(WmsLogTag::WMS_UIEXT,
            "UpdateSessionViewportConfig: windowId:%{public}d, isDensityFollowHost_:%{public}d, displayId:%{public}lu,"
            "density:%{public}f, lastDensity:%{public}f, orientation:%{public}d, lastOrientation:%{public}d",
            window->GetPersistentId(), viewportConfig.isDensityFollowHost_, viewportConfig.displayId_,
            viewportConfig.density_, window->lastDensity_, viewportConfig.orientation_, window->lastOrientation_);
        window->NotifyDisplayInfoChange(viewportConfig);
        window->property_->SetDisplayId(viewportConfig.displayId_);

        auto ret = window->UpdateSessionViewportConfigInner(viewportConfig);
        if (ret == WSError::WS_OK) {
            window->lastDensity_ = viewportConfig.density_;
            window->lastOrientation_ = viewportConfig.orientation_;
        }
    };
    handler_->PostTask(task, "UpdateSessionViewportConfig");
    return WSError::WS_OK;
}

float WindowExtensionSessionImpl::UpdateExtensionDensity(const SessionViewportConfig& config)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "isFollowHost:%{public}d densityValue:%{public}f", config.isDensityFollowHost_,
        config.density_);
    auto density = 1.0f;
    if (config.isDensityFollowHost_) {
        isDensityFollowHost_ = config.isDensityFollowHost_;
        hostDensityValue_ = config.density_;
        return config.density_;
    }

    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(config.displayId_);
    if (display == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "display is null!");
        return density;
    }
    auto displayInfo = display->GetDisplayInfo();
    if (displayInfo == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "displayInfo is null");
        return density;
    }
    return displayInfo->GetVirtualPixelRatio();
}

void WindowExtensionSessionImpl::NotifyDisplayInfoChange(const SessionViewportConfig& config)
{
    if (property_->GetDisplayId() == config.displayId_ && NearEqual(lastDensity_, config.density_) &&
        lastOrientation_ == config.orientation_) {
        TLOGI(WmsLogTag::WMS_UIEXT, "No parameters have changed, no need to update");
        return;
    }
    if (context_ == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "get token of window:%{public}d failed because of context is null.",
            GetPersistentId());
        return;
    }
    auto token = context_->GetToken();
    if (token == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "get token of window:%{public}d failed.", GetPersistentId());
        return;
    }
    SingletonContainer::Get<WindowManager>().NotifyDisplayInfoChange(
        token, config.displayId_, config.density_, static_cast<DisplayOrientation>(config.orientation_));
}

WSError WindowExtensionSessionImpl::UpdateSessionViewportConfigInner(
    const SessionViewportConfig& config, const std::shared_ptr<RSTransaction>& rsTransaction)
{
    if (NearEqual(lastDensity_, config.density_) && lastOrientation_ == config.orientation_) {
        TLOGI(WmsLogTag::WMS_UIEXT, "No parameters have changed, no need to update");
        return WSError::WS_DO_NOTHING;
    }
    Ace::ViewportConfig viewportConfig;
    auto rect = GetRect();
    viewportConfig.SetSize(rect.width_, rect.height_);
    viewportConfig.SetPosition(rect.posX_, rect.posY_);
    viewportConfig.SetDensity(config.density_);
    viewportConfig.SetOrientation(config.orientation_);
    viewportConfig.SetTransformHint(config.transform_);

    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr) {
        TLOGW(WmsLogTag::WMS_UIEXT, "uiContent is null!");
        return WSError::WS_ERROR_NULLPTR;
    }
    uiContent->UpdateViewportConfig(viewportConfig, WindowSizeChangeReason::UNDEFINED, nullptr);
    return WSError::WS_OK;
}

WSError WindowExtensionSessionImpl::NotifyAccessibilityHoverEvent(float pointX, float pointY, int32_t sourceType,
    int32_t eventType, int64_t timeMs)
{
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr) {
        WLOGFE("error, no uiContent");
        return WSError::WS_ERROR_NO_UI_CONTENT_ERROR;
    }
    uiContent->HandleAccessibilityHoverEvent(pointX, pointY, sourceType, eventType, timeMs);
    return WSError::WS_OK;
}

WMError WindowExtensionSessionImpl::TransferAccessibilityEvent(const Accessibility::AccessibilityEventInfo& info,
    int64_t uiExtensionIdLevel)
{
    if (IsWindowSessionInvalid()) {
        WLOGFE("Window session invalid.");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_INVALID_WINDOW);
    return static_cast<WMError>(hostSession->TransferAccessibilityEvent(info, uiExtensionIdLevel));
}

void WindowExtensionSessionImpl::NotifySessionForeground(uint32_t reason, bool withAnimation)
{
}

void WindowExtensionSessionImpl::NotifySessionBackground(uint32_t reason, bool withAnimation, bool isFromInnerkits)
{
}

void WindowExtensionSessionImpl::NotifyOccupiedAreaChangeInfo(sptr<OccupiedAreaChangeInfo> info,
                                                              const std::shared_ptr<RSTransaction>& rsTransaction)
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "TextFieldPosY = %{public}f, KeyBoardHeight = %{public}d",
        info->textFieldPositionY_, info->rect_.height_);
    if (occupiedAreaChangeListener_) {
        occupiedAreaChangeListener_->OnSizeChange(info, rsTransaction);
    }
}

WMError WindowExtensionSessionImpl::RegisterOccupiedAreaChangeListener(
    const sptr<IOccupiedAreaChangeListener>& listener)
{
    occupiedAreaChangeListener_ = listener;
    return WMError::WM_OK;
}

WMError WindowExtensionSessionImpl::UnregisterOccupiedAreaChangeListener(
    const sptr<IOccupiedAreaChangeListener>& listener)
{
    occupiedAreaChangeListener_ = nullptr;
    return WMError::WM_OK;
}

WMError WindowExtensionSessionImpl::GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea)
{
    WLOGFI("type %{public}d", type);
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_NULLPTR);
    avoidArea = hostSession->GetAvoidAreaByType(type);
    return WMError::WM_OK;
}

WMError WindowExtensionSessionImpl::RegisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener)
{
    return RegisterExtensionAvoidAreaChangeListener(listener);
}

WMError WindowExtensionSessionImpl::UnregisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener)
{
    return UnregisterExtensionAvoidAreaChangeListener(listener);
}

WMError WindowExtensionSessionImpl::Show(uint32_t reason, bool withAnimation)
{
    CheckAndAddExtWindowFlags();
    return this->WindowSessionImpl::Show(reason, withAnimation);
}

WMError WindowExtensionSessionImpl::Hide(uint32_t reason, bool withAnimation, bool isFromInnerkits)
{
    TLOGI(WmsLogTag::WMS_LIFE, "id:%{public}d Hide, reason:%{public}u, state:%{public}u",
        GetPersistentId(), reason, state_);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_INVALID_WINDOW);
    CheckAndRemoveExtWindowFlags();
    if (state_ == WindowState::STATE_HIDDEN || state_ == WindowState::STATE_CREATED) {
        TLOGD(WmsLogTag::WMS_LIFE, "window extension session is already hidden "
            "[name:%{public}s, id:%{public}d, type: %{public}u]",
            property_->GetWindowName().c_str(), GetPersistentId(), property_->GetWindowType());
        NotifyBackgroundFailed(WMError::WM_DO_NOTHING);
        return WMError::WM_OK;
    }
    WSError ret = hostSession->Background();
    WMError res = static_cast<WMError>(ret);
    if (res == WMError::WM_OK) {
        state_ = WindowState::STATE_HIDDEN;
        requestState_ = WindowState::STATE_HIDDEN;
        NotifyAfterBackground();
    } else {
        TLOGD(WmsLogTag::WMS_LIFE, "window extension session Hide to Background is error");
    }
    return WMError::WM_OK;
}

WSError WindowExtensionSessionImpl::NotifyDensityFollowHost(bool isFollowHost, float densityValue)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "isFollowHost:%{public}d densityValue:%{public}f", isFollowHost, densityValue);

    if (!isFollowHost && !isDensityFollowHost_) {
        TLOGI(WmsLogTag::WMS_UIEXT, "isFollowHost is false and not change");
        return WSError::WS_OK;
    }

    if (isFollowHost) {
        if (std::islessequal(densityValue, 0.0f)) {
            TLOGE(WmsLogTag::WMS_UIEXT, "densityValue is invalid");
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        if (hostDensityValue_ != std::nullopt &&
            std::abs(hostDensityValue_->load() - densityValue) < std::numeric_limits<float>::epsilon()) {
            TLOGI(WmsLogTag::WMS_UIEXT, "densityValue not change");
            return WSError::WS_OK;
        }
        hostDensityValue_ = densityValue;
    }

    isDensityFollowHost_ = isFollowHost;

    UpdateViewportConfig(GetRect(), WindowSizeChangeReason::UNDEFINED);
    return WSError::WS_OK;
}

float WindowExtensionSessionImpl::GetVirtualPixelRatio(sptr<DisplayInfo> displayInfo)
{
    if (isDensityFollowHost_ && hostDensityValue_ != std::nullopt) {
        return hostDensityValue_->load();
    }
    float vpr = 1.0f;
    if (displayInfo == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "displayInfo is nullptr");
        return vpr;
    }
    return displayInfo->GetVirtualPixelRatio();
}

WMError WindowExtensionSessionImpl::HideNonSecureWindows(bool shouldHide)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "persistentId: %{public}u, shouldHide: %{public}u", GetPersistentId(), shouldHide);
    if (property_ == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "property_ is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    if ((property_->GetUIExtensionUsage() == UIExtensionUsage::MODAL ||
         property_->GetUIExtensionUsage() == UIExtensionUsage::CONSTRAINED_EMBEDDED) && !shouldHide) {
        extensionWindowFlags_.hideNonSecureWindowsFlag = true;
        TLOGE(WmsLogTag::WMS_UIEXT, "Setting this property to false is not allowed in %{public}s UIExtension.",
            property_->GetUIExtensionUsage() == UIExtensionUsage::MODAL ? "modal" : "constrained embedded");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    if (state_ != WindowState::STATE_SHOWN) {
        extensionWindowFlags_.hideNonSecureWindowsFlag = shouldHide;
        return WMError::WM_OK;
    }
    if (shouldHide == extensionWindowFlags_.hideNonSecureWindowsFlag) {
        return WMError::WM_OK;
    }

    auto updateFlags = extensionWindowFlags_;
    updateFlags.hideNonSecureWindowsFlag = shouldHide;
    ExtensionWindowFlags actions(0);
    actions.hideNonSecureWindowsFlag = true;
    auto ret = UpdateExtWindowFlags(updateFlags, actions);
    if (ret == WMError::WM_OK) {
        extensionWindowFlags_ = updateFlags;
    }
    return ret;
}

WMError WindowExtensionSessionImpl::SetWaterMarkFlag(bool isEnable)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "persistentId: %{public}u, isEnable: %{public}u", GetPersistentId(), isEnable);
    if (state_ != WindowState::STATE_SHOWN) {
        extensionWindowFlags_.waterMarkFlag = isEnable;
        return WMError::WM_OK;
    }
    if (isEnable == extensionWindowFlags_.waterMarkFlag) {
        return WMError::WM_OK;
    }

    auto updateFlags = extensionWindowFlags_;
    updateFlags.waterMarkFlag = isEnable;
    ExtensionWindowFlags actions(0);
    actions.waterMarkFlag = true;
    auto ret = UpdateExtWindowFlags(updateFlags, actions);
    if (ret == WMError::WM_OK) {
        extensionWindowFlags_ = updateFlags;
    }
    return ret;
}

void WindowExtensionSessionImpl::CheckAndAddExtWindowFlags()
{
    if (extensionWindowFlags_.bitData != 0) {
        // If flag is true, make it active when foreground
        UpdateExtWindowFlags(extensionWindowFlags_, extensionWindowFlags_);
    }
}

void WindowExtensionSessionImpl::CheckAndRemoveExtWindowFlags()
{
    if (extensionWindowFlags_.bitData != 0) {
        // If flag is true, make it inactive when background
        UpdateExtWindowFlags(ExtensionWindowFlags(), extensionWindowFlags_);
    }
}

WSError WindowExtensionSessionImpl::NotifyAccessibilityChildTreeRegister(
    uint32_t windowId, int32_t treeId, int64_t accessibilityId)
{
    if (!handler_) {
        return WSError::WS_ERROR_INTERNAL_ERROR;
    }
    auto uiContentSharedPtr = GetUIContentSharedPtr();
    if (uiContentSharedPtr == nullptr) {
        accessibilityChildTreeInfo_ = {
            .windowId = windowId,
            .treeId = treeId,
            .accessibilityId = accessibilityId
        };
        TLOGD(WmsLogTag::WMS_UIEXT, "uiContent is null, save the accessibility child tree info.");
        return WSError::WS_OK;
    }
    handler_->PostTask([uiContent = uiContentSharedPtr, windowId, treeId, accessibilityId]() {
        if (uiContent == nullptr) {
            TLOGE(WmsLogTag::WMS_UIEXT, "NotifyAccessibilityChildTreeRegister error, no uiContent");
            return;
        }
        TLOGI(WmsLogTag::WMS_UIEXT,
            "NotifyAccessibilityChildTreeRegister: %{public}d %{public}" PRId64, treeId, accessibilityId);
        uiContent->RegisterAccessibilityChildTree(windowId, treeId, accessibilityId);
    });
    return WSError::WS_OK;
}

WSError WindowExtensionSessionImpl::NotifyAccessibilityChildTreeUnregister()
{
    if (!handler_) {
        return WSError::WS_ERROR_INTERNAL_ERROR;
    }
    handler_->PostTask([uiContent = GetUIContentSharedPtr()]() {
        if (uiContent == nullptr) {
            TLOGE(WmsLogTag::WMS_UIEXT, "NotifyAccessibilityChildTreeUnregister error, no uiContent");
            return;
        }
        uiContent->DeregisterAccessibilityChildTree();
    });
    return WSError::WS_OK;
}

WSError WindowExtensionSessionImpl::NotifyAccessibilityDumpChildInfo(
    const std::vector<std::string>& params, std::vector<std::string>& info)
{
    if (!handler_) {
        return WSError::WS_ERROR_INTERNAL_ERROR;
    }
    handler_->PostSyncTask([uiContent = GetUIContentSharedPtr(), params, &info]() {
        if (uiContent == nullptr) {
            TLOGE(WmsLogTag::WMS_UIEXT, "NotifyAccessibilityDumpChildInfo error, no uiContent");
            return;
        }
        uiContent->AccessibilityDumpChildInfo(params, info);
    });
    return WSError::WS_OK;
}

void WindowExtensionSessionImpl::UpdateAccessibilityTreeInfo()
{
    if (accessibilityChildTreeInfo_ == std::nullopt) {
        return;
    }
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr) {
        return;
    }
    uiContent->RegisterAccessibilityChildTree(accessibilityChildTreeInfo_->windowId,
        accessibilityChildTreeInfo_->treeId, accessibilityChildTreeInfo_->accessibilityId);
    accessibilityChildTreeInfo_.reset();
}

WMError WindowExtensionSessionImpl::UpdateExtWindowFlags(const ExtensionWindowFlags& flags,
    const ExtensionWindowFlags& actions)
{
    // action is true when the corresponding flag should be updated
    if (IsWindowSessionInvalid()) {
        TLOGI(WmsLogTag::WMS_UIEXT, "session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    if (!abilityToken_) {
        TLOGE(WmsLogTag::WMS_UIEXT, "token is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }

    return SingletonContainer::Get<WindowAdapter>().UpdateExtWindowFlags(abilityToken_, flags.bitData, actions.bitData);
}

Rect WindowExtensionSessionImpl::GetHostWindowRect(int32_t hostWindowId)
{
    Rect rect;
    if (hostWindowId != property_->GetParentId()) {
        TLOGE(WmsLogTag::WMS_UIEXT, "hostWindowId is invalid");
        return rect;
    }
    SingletonContainer::Get<WindowAdapter>().GetHostWindowRect(hostWindowId, rect);
    return rect;
}

void WindowExtensionSessionImpl::ConsumePointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    if (pointerEvent == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "PointerEvent is nullptr, windowId: %{public}d", GetWindowId());
        return;
    }
    if (hostSession_ == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "hostSession is nullptr, windowId: %{public}d", GetWindowId());
        pointerEvent->MarkProcessed();
        return;
    }

    MMI::PointerEvent::PointerItem pointerItem;
    if (!pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem)) {
        TLOGW(WmsLogTag::WMS_EVENT, "invalid pointerEvent, windowId: %{public}d", GetWindowId());
        pointerEvent->MarkProcessed();
        return;
    }
    auto action = pointerEvent->GetPointerAction();
    bool isPointDown = (action == MMI::PointerEvent::POINTER_ACTION_DOWN ||
        action == MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    if ((property_->GetUIExtensionUsage() == UIExtensionUsage::MODAL) && isPointDown) {
        if (!abilityToken_) {
            TLOGE(WmsLogTag::WMS_UIEXT, "token is nullptr");
            return;
        }
        SingletonContainer::Get<WindowAdapter>().ProcessModalExtensionPointDown(abilityToken_,
            pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
    }
    if (action != MMI::PointerEvent::POINTER_ACTION_MOVE) {
        TLOGI(WmsLogTag::WMS_EVENT, "InputTracking id:%{public}d,windowId:%{public}u,"
            "pointId:%{public}d,sourceType:%{public}d", pointerEvent->GetId(), GetWindowId(),
            pointerEvent->GetPointerId(), pointerEvent->GetSourceType());
    }
    NotifyPointerEvent(pointerEvent);
}

bool WindowExtensionSessionImpl::PreNotifyKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    if (keyEvent == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "keyEvent is nullptr");
        return false;
    }
    RefreshNoInteractionTimeoutMonitor();
    if (property_->GetUIExtensionUsage() == UIExtensionUsage::CONSTRAINED_EMBEDDED) {
        if (focusState_ == std::nullopt) {
            TLOGW(WmsLogTag::WMS_EVENT, "focusState is null");
            keyEvent->MarkProcessed();
            return true;
        }
        if (!focusState_.value()) {
            keyEvent->MarkProcessed();
            return true;
        }
        TLOGI(WmsLogTag::WMS_EVENT, "InputTracking:%{public}d wid:%{public}d",
            keyEvent->GetId(), keyEvent->GetAgentWindowId());
    }
    if (auto uiContent = GetUIContentSharedPtr()) {
        return uiContent->ProcessKeyEvent(keyEvent, true);
    }
    return false;
}

bool WindowExtensionSessionImpl::GetFreeMultiWindowModeEnabledState()
{
    bool enable = false;
    SingletonContainer::Get<WindowAdapter>().GetFreeMultiWindowEnableState(enable);
    TLOGI(WmsLogTag::WMS_MULTI_WINDOW, "enable = %{public}u", enable);
    return enable;
}

void WindowExtensionSessionImpl::NotifyExtensionTimeout(int32_t errorCode)
{
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_IF_NULL(hostSession);
    hostSession->NotifyExtensionTimeout(errorCode);
}

int32_t WindowExtensionSessionImpl::GetRealParentId() const
{
    if (property_ == nullptr) {
        return static_cast<int32_t>(INVALID_WINDOW_ID);
    }
    return property_->GetRealParentId();
}
} // namespace Rosen
} // namespace OHOS
