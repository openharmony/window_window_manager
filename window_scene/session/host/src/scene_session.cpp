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

#include "session/host/include/scene_session.h"
#include <parameters.h>

#include <ability_manager_client.h>
#include <algorithm>
#include <climits>
#include <hitrace_meter.h>
#include <type_traits>
#ifdef IMF_ENABLE
#include <input_method_controller.h>
#endif // IMF_ENABLE
#include <ipc_skeleton.h>
#include <pointer_event.h>
#include <transaction/rs_sync_transaction_controller.h>
#include <transaction/rs_transaction.h>
#include <ui/rs_surface_node.h>

#include "proxy/include/window_info.h"

#include "application_context.h"
#include "common/include/session_permission.h"
#ifdef DEVICE_STATUS_ENABLE
#include "interaction_manager.h"
#endif // DEVICE_STATUS_ENABLE
#include "interfaces/include/ws_common.h"
#include "pixel_map.h"
#include "session/screen/include/screen_session.h"
#include "screen_session_manager_client/include/screen_session_manager_client.h"
#include "session/host/include/scene_persistent_storage.h"
#include "session/host/include/session_utils.h"
#include "display_manager.h"
#include "session_helper.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "wm_math.h"
#include <running_lock.h>
#include "screen_manager.h"
#include "screen.h"
#include "fold_screen_state_internel.h"
#include "fold_screen_common.h"
#include "session/host/include/ability_info_manager.h"
#include "session/host/include/multi_instance_manager.h"
#include "session/host/include/pc_fold_screen_controller.h"

#ifdef POWER_MANAGER_ENABLE
#include <power_mgr_client.h>
#endif

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSession" };
const std::string DLP_INDEX = "ohos.dlp.params.index";
const std::string ERROR_REASON_LOW_MEMORY_KILL = "LowMemoryKill";
constexpr const char* APP_CLONE_INDEX = "ohos.extra.param.key.appCloneIndex";
constexpr float MINI_FLOAT_SCALE = 0.3f;
constexpr float MOVE_DRAG_POSITION_Z = 100.5f;
constexpr DisplayId VIRTUAL_DISPLAY_ID = 999;
constexpr WSRectF VELOCITY_RELOCATION_TO_TOP = {0.0f, -10.0f, 0.0f, 0.0f};
constexpr WSRectF VELOCITY_RELOCATION_TO_BOTTOM = {0.0f, 10.0f, 0.0f, 0.0f};
constexpr int32_t API_VERSION_18 = 18;
constexpr int32_t HOOK_SYSTEM_BAR_HEIGHT = 40;
constexpr int32_t HOOK_AI_BAR_HEIGHT = 28;
constexpr int32_t MULTI_WINDOW_TITLE_BAR_DEFAULT_HEIGHT_VP = 32;

bool CheckIfRectElementIsTooLarge(const WSRect& rect)
{
    int32_t largeNumber = static_cast<int32_t>(SHRT_MAX);
    if (rect.posX_ >= largeNumber || rect.posY_ >= largeNumber ||
        rect.width_ >= largeNumber || rect.height_ >= largeNumber) {
        return true;
    }
    return false;
}
} // namespace

MaximizeMode SceneSession::maximizeMode_ = MaximizeMode::MODE_RECOVER;
std::shared_mutex SceneSession::windowDragHotAreaMutex_;
std::map<uint64_t, std::map<uint32_t, WSRect>> SceneSession::windowDragHotAreaMap_;
static bool g_enableForceUIFirst = system::GetParameter("window.forceUIFirst.enabled", "1") == "1";
GetConstrainedModalExtWindowInfoFunc SceneSession::onGetConstrainedModalExtWindowInfoFunc_;
std::unordered_map<std::string,
    std::unordered_map<ControlAppType, SceneSession::ControlInfo>> SceneSession::allAppUseControlMap_;

SceneSession::SceneSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback)
    : Session(info)
{
    GeneratePersistentId(false, info.persistentId_);
    specificCallback_ = specificCallback;
    SetCollaboratorType(info.collaboratorType_);
    TLOGI(WmsLogTag::WMS_LIFE, "Create session, id: %{public}d", GetPersistentId());
}

SceneSession::~SceneSession()
{
    TLOGI(WmsLogTag::WMS_LIFE, "id: %{public}d", GetPersistentId());
}

WSError SceneSession::ConnectInner(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
    sptr<WindowSessionProperty> property, sptr<IRemoteObject> token, int32_t pid, int32_t uid,
    const std::string& identityToken)
{
    return PostSyncTask([weakThis = wptr(this), sessionStage, eventChannel, surfaceNode, &systemConfig,
        property, token, pid, uid, identityToken, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (SessionHelper::IsMainWindow(session->GetWindowType())) {
            if (!session->CheckIdentityTokenIfMatched(identityToken)) {
                TLOGNW(WmsLogTag::WMS_LIFE, "%{public}s check failed", where);
                return WSError::WS_OK;
            }
        }
        if (property) {
            property->SetCollaboratorType(session->GetCollaboratorType());
            property->SetAppInstanceKey(session->GetAppInstanceKey());
        }
        session->RetrieveStatusBarDefaultVisibility();
        auto ret = LOCK_GUARD_EXPR(SCENE_GUARD, session->Session::ConnectInner(
            sessionStage, eventChannel, surfaceNode, systemConfig, property, token, pid, uid));
        if (ret != WSError::WS_OK) {
            return ret;
        }
        session->NotifySingleHandTransformChange(session->GetSingleHandTransform());
        session->NotifyPropertyWhenConnect();
        if (session->pcFoldScreenController_) {
            session->pcFoldScreenController_->OnConnect();
        }
        session->SaveLastDensity();
        return ret;
    }, __func__);
}

WSError SceneSession::Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
    sptr<WindowSessionProperty> property, sptr<IRemoteObject> token,
    const std::string& identityToken)
{
    // Get pid and uid before posting task.
    int32_t pid = IPCSkeleton::GetCallingRealPid();
    int32_t uid = IPCSkeleton::GetCallingUid();
    return ConnectInner(sessionStage, eventChannel, surfaceNode, systemConfig,
        property, token, pid, uid, identityToken);
}

WSError SceneSession::Reconnect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, sptr<WindowSessionProperty> property, sptr<IRemoteObject> token,
    int32_t pid, int32_t uid)
{
    return PostSyncTask([weakThis = wptr(this), sessionStage, eventChannel,
        surfaceNode, property, token, pid, uid, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        WSError ret = LOCK_GUARD_EXPR(SCENE_GUARD,
            session->Session::Reconnect(sessionStage, eventChannel, surfaceNode, property, token, pid, uid));
        if (ret != WSError::WS_OK) {
            return ret;
        }
        return LOCK_GUARD_EXPR(SCENE_GUARD, session->ReconnectInner(property));
    });
}

WSError SceneSession::ReconnectInner(sptr<WindowSessionProperty> property)
{
    if (property == nullptr) {
        TLOGE(WmsLogTag::WMS_RECOVER, "property is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    WindowState windowState = property->GetWindowState();
    TLOGI(WmsLogTag::WMS_RECOVER, "persistentId: %{public}d, windowState: %{public}d ", GetPersistentId(), windowState);
    WSError ret = WSError::WS_OK;
    switch (windowState) {
        case WindowState::STATE_INITIAL: {
            TLOGE(WmsLogTag::WMS_RECOVER, "persistentId: %{public}d, invalid window state: STATE_INITIAL",
                GetPersistentId());
            ret = WSError::WS_ERROR_INVALID_PARAM;
            break;
        }
        case WindowState::STATE_CREATED:
            break;
        case WindowState::STATE_SHOWN: {
            UpdateSessionState(SessionState::STATE_FOREGROUND);
            UpdateActiveStatus(true);
            break;
        }
        case WindowState::STATE_HIDDEN: {
            UpdateSessionState(SessionState::STATE_BACKGROUND);
            break;
        }
        default:
            TLOGE(WmsLogTag::WMS_RECOVER, "persistentId: %{public}d, invalid window state: %{public}u",
                GetPersistentId(), windowState);
            ret = WSError::WS_ERROR_INVALID_PARAM;
            break;
    }
    if (ret != WSError::WS_OK) {
        Session::Disconnect(false);
    }
    return ret;
}

bool SceneSession::IsShowOnLockScreen(uint32_t lockScreenZOrder)
{
    TLOGD(WmsLogTag::WMS_UIEXT, "UIExtOnLock: lockScreenZOrder: %{public}d, zOrder_: %{public}d", lockScreenZOrder,
        zOrder_);

    // must be default screen
    ScreenId defaultScreenId = ScreenSessionManagerClient::GetInstance().GetDefaultScreenId();
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty != nullptr && defaultScreenId != sessionProperty->GetDisplayId()) {
        TLOGD(WmsLogTag::WMS_UIEXT, "UIExtOnLock: not default display");
        return false;
    }

    // current window on lock screen jurded by zorder
    if (zOrder_ >= lockScreenZOrder) {
        TLOGI(WmsLogTag::WMS_UIEXT, "zOrder >= lockScreenZOrder");
        return true;
    }

    if (zOrder_ == 0) {
        if (auto mainSession = GetMainSession()) {
            TLOGI(WmsLogTag::WMS_UIEXT, "mainSession zOrder=%{public}d", mainSession->GetZOrder());
            return mainSession->GetZOrder() >= lockScreenZOrder;
        }
    }
    return false;
}

void SceneSession::AddExtensionTokenInfo(const UIExtensionTokenInfo& tokenInfo)
{
    extensionTokenInfos_.push_back(tokenInfo);
    TLOGI(WmsLogTag::WMS_UIEXT, "can show:%{public}u, id: %{public}d",
        tokenInfo.canShowOnLockScreen, GetPersistentId());
}

void SceneSession::RemoveExtensionTokenInfo(const sptr<IRemoteObject>& abilityToken)
{
    auto persistentId = GetPersistentId();
    auto itr = std::remove_if(
        extensionTokenInfos_.begin(), extensionTokenInfos_.end(),
        [&abilityToken, persistentId, where = __func__](const auto& tokenInfo) {
            TLOGNI(WmsLogTag::WMS_UIEXT,
                "%{public}s UIExtOnLock: need remove, token: %{public}u, persistentId: %{public}d",
                where, tokenInfo.callingTokenId, persistentId);
            return tokenInfo.abilityToken == abilityToken;
        });
    extensionTokenInfos_.erase(itr, extensionTokenInfos_.end());
}

void SceneSession::OnNotifyAboveLockScreen()
{
    CheckExtensionOnLockScreenToClose();
}

void SceneSession::CheckExtensionOnLockScreenToClose()
{
    TLOGD(WmsLogTag::WMS_UIEXT, "UIExtOnLock: %{public}d", GetPersistentId());

    // 1. check sub session
    for (auto& session : GetSubSession()) {
        if (!session) {
            TLOGE(WmsLogTag::WMS_UIEXT, "UIExtOnLock: session is null");
            continue;
        }
        session->CheckExtensionOnLockScreenToClose();
    }

    // 2. check self permission
    std::vector<UIExtensionTokenInfo> tokenInfosToClose;
    for (auto& tokenInfo : extensionTokenInfos_) {
        if (tokenInfo.canShowOnLockScreen) {
            continue;
        }
        tokenInfosToClose.push_back(tokenInfo);
    }

    // 3. close ui extension without lock screen permisson
    std::for_each(tokenInfosToClose.rbegin(), tokenInfosToClose.rend(),
        [this](const UIExtensionTokenInfo& tokenInfo) { CloseExtensionSync(tokenInfo); });
}

void SceneSession::CloseExtensionSync(const UIExtensionTokenInfo& tokenInfo)
{
    TLOGD(WmsLogTag::WMS_UIEXT, "UIExtOnLock");

    // hide sub window
    auto subSceneSessions = GetSubSession();
    for (auto& session : subSceneSessions) {
        if (!session) {
            TLOGE(WmsLogTag::WMS_UIEXT, "UIExtOnLock: session is null");
            continue;
        }
        // hide sub window of ui extension
        if (session->GetAbilityToken() == tokenInfo.abilityToken) {
            TLOGI(WmsLogTag::WMS_UIEXT, "UIExtOnLock: hide sub window %{public}u", session->GetWindowId());
            session->HideSync();
        }
    }

    TLOGI(WmsLogTag::WMS_UIEXT, "UIExtOnLock: close ui extension, callerToken: %{public}u, persistent id %{public}d",
        tokenInfo.callingTokenId, GetPersistentId());

    // kill ui extension ability
    AAFwk::AbilityManagerClient::GetInstance()->CloseUIExtensionAbilityBySCB(tokenInfo.abilityToken);
}

WSError SceneSession::Foreground(
    sptr<WindowSessionProperty> property, bool isFromClient, const std::string& identityToken)
{
    if (!CheckPermissionWithPropertyAnimation(property)) {
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }

    // return when screen is locked and show without ShowWhenLocked flag
    ScreenId defaultScreenId = ScreenSessionManagerClient::GetInstance().GetDefaultScreenId();
    auto sessionProperty = GetSessionProperty();
    if (GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW &&
        IsActivatedAfterScreenLocked() &&
        GetStateFromManager(ManagerState::MANAGER_STATE_SCREEN_LOCKED) &&
        (sessionProperty != nullptr && defaultScreenId == sessionProperty->GetDisplayId()) &&
        !IsShowWhenLocked()) {
        if (SessionPermission::VerifyCallingPermission("ohos.permission.CALLED_BELOW_LOCK_SCREEN")) {
            TLOGW(WmsLogTag::WMS_LIFE, "screen is locked, session %{public}d %{public}s permission verified",
                GetPersistentId(), sessionInfo_.bundleName_.c_str());
        } else {
            TLOGW(WmsLogTag::WMS_LIFE,
                "failed: screen is locked, session %{public}d %{public}s show without ShowWhenLocked flag",
                GetPersistentId(), sessionInfo_.bundleName_.c_str());
            return WSError::WS_ERROR_INVALID_SESSION;
        }
    }

    if (isFromClient && SessionHelper::IsMainWindow(GetWindowType())) {
        if (!CheckPidIfMatched() || !CheckIdentityTokenIfMatched(identityToken)) {
            TLOGW(WmsLogTag::WMS_LIFE, "check failed");
            return WSError::WS_OK;
        }
    }
    return ForegroundTask(property);
}

void SceneSession::SetRequestNextVsyncFunc(RequestVsyncFunc&& func)
{
    if (func == nullptr) {
        TLOGI(WmsLogTag::DEFAULT, "func is nullptr");
        return;
    }
    requestNextVsyncFunc_ = std::move(func);
}

WSError SceneSession::ForegroundTask(const sptr<WindowSessionProperty>& property)
{
    PostTask([weakThis = wptr(this), property, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        auto sessionProperty = session->GetSessionProperty();
        if (property && sessionProperty) {
            sessionProperty->SetWindowMode(property->GetWindowMode());
            sessionProperty->SetDecorEnable(property->IsDecorEnable());
            session->SetFocusableOnShow(property->GetFocusableOnShow());
        }
        int32_t persistentId = session->GetPersistentId();
        auto ret = session->Session::Foreground(property);
        if (ret != WSError::WS_OK) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session foreground failed, ret=%{public}d persistentId=%{public}d",
                where, ret, persistentId);
            return ret;
        }
        session->NotifySingleHandTransformChange(session->GetSingleHandTransform());
        auto leashWinSurfaceNode = session->GetLeashWinSurfaceNode();
        if (leashWinSurfaceNode && sessionProperty) {
            bool lastPrivacyMode = sessionProperty->GetPrivacyMode() || sessionProperty->GetSystemPrivacyMode();
            leashWinSurfaceNode->SetSecurityLayer(lastPrivacyMode);
        }
        if (session->specificCallback_ != nullptr) {
            if (Session::IsScbCoreEnabled()) {
                session->MarkAvoidAreaAsDirty();
            } else {
                session->specificCallback_->onUpdateAvoidArea_(persistentId);
            }
            session->specificCallback_->onWindowInfoUpdate_(
                persistentId, WindowUpdateType::WINDOW_UPDATE_ADDED);
            session->specificCallback_->onHandleSecureSessionShouldHide_(session);
            session->UpdateGestureBackEnabled();
        } else {
            TLOGNI(WmsLogTag::WMS_LIFE,
                "%{public}s foreground specific callback does not take effect, callback function null", where);
        }
        return WSError::WS_OK;
    }, __func__);
    return WSError::WS_OK;
}

void SceneSession::CheckAndMoveDisplayIdRecursively(uint64_t displayId)
{
    if (GetSessionProperty()->GetDisplayId() == displayId || !shouldFollowParentWhenShow_) {
        return;
    }
    TLOGI(WmsLogTag::WMS_LAYOUT, "session id: %{public}d, Move display to %{public}" PRIu64,
        GetPersistentId(), displayId);
    SetScreenId(displayId); // notify client to update display id
    GetSessionProperty()->SetDisplayId(displayId); // set session property
    NotifySessionDisplayIdChange(displayId);
    for (const auto& session : subSession_) {
        if (session) {
            session->CheckAndMoveDisplayIdRecursively(displayId);
        }
    }
}

WSError SceneSession::Background(bool isFromClient, const std::string& identityToken)
{
    if (!CheckPermissionWithPropertyAnimation(GetSessionProperty())) {
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }

    if (isFromClient && SessionHelper::IsMainWindow(GetWindowType())) {
        if (!CheckPidIfMatched() || !CheckIdentityTokenIfMatched(identityToken)) {
            TLOGW(WmsLogTag::WMS_LIFE, "check failed");
            return WSError::WS_OK;
        }
    }
    return BackgroundTask(true);
}

WSError SceneSession::NotifyFrameLayoutFinishFromApp(bool notifyListener, const WSRect& rect)
{
    TLOGI(WmsLogTag::WMS_PATTERN, "%{public}d, %{public}s", notifyListener, rect.ToString().c_str());
    PostTask([weakThis = wptr(this), notifyListener, rect, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_MULTI_WINDOW, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        session->layoutRect_ = rect;
        session->NotifyLayoutFinished();
        if (notifyListener && session->frameLayoutFinishFunc_) {
            TLOGND(WmsLogTag::WMS_MULTI_WINDOW, "%{public}s id: %{public}d", where, session->GetPersistentId());
            session->frameLayoutFinishFunc_();
        }
        return WSError::WS_OK;
    }, __func__);
    return WSError::WS_OK;
}

WSError SceneSession::BackgroundTask(const bool isSaveSnapshot)
{
    PostTask([weakThis = wptr(this), isSaveSnapshot, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        auto state = session->GetSessionState();
        if (state == SessionState::STATE_BACKGROUND) {
            return WSError::WS_OK;
        }
        auto ret = session->Session::Background();
        if (ret != WSError::WS_OK) {
            return ret;
        }
        if (WindowHelper::IsMainWindow(session->GetWindowType()) && isSaveSnapshot) {
            session->SaveSnapshot(true);
        }
        if (session->specificCallback_ != nullptr) {
            if (Session::IsScbCoreEnabled()) {
                session->MarkAvoidAreaAsDirty();
            } else {
                session->specificCallback_->onUpdateAvoidArea_(session->GetPersistentId());
            }
            session->specificCallback_->onWindowInfoUpdate_(
                session->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_REMOVED);
            session->specificCallback_->onHandleSecureSessionShouldHide_(session);
            session->UpdateGestureBackEnabled();
        }
        return WSError::WS_OK;
    }, __func__);
    return WSError::WS_OK;
}

void SceneSession::ClearSpecificSessionCbMap()
{
    PostTask([weakThis = wptr(this), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_SYSTEM, "%{public}s: session is null", where);
            return;
        }
        session->ClearJsSceneSessionCbMap(true);
    }, __func__);
}

void SceneSession::ClearJsSceneSessionCbMap(bool needRemove)
{
    if (clearCallbackMapFunc_) {
        TLOGD(WmsLogTag::WMS_LIFE, "id: %{public}d, needRemove: %{public}d", GetPersistentId(), needRemove);
        clearCallbackMapFunc_(needRemove);
    } else {
        TLOGE(WmsLogTag::WMS_LIFE, "get callback failed, id: %{public}d", GetPersistentId());
    }
}

void SceneSession::RegisterShowWhenLockedCallback(NotifyShowWhenLockedFunc&& callback)
{
    PostTask([weakThis = wptr(this), callback = std::move(callback), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is nullptr", where);
            return;
        }
        session->onShowWhenLockedFunc_ = std::move(callback);
        session->onShowWhenLockedFunc_(session->GetShowWhenLockedFlagValue());
    }, __func__);
}

void SceneSession::RegisterForceHideChangeCallback(NotifyForceHideChangeFunc&& callback)
{
    PostTask([weakThis = wptr(this), callback = std::move(callback), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is nullptr", where);
            return;
        }
        session->onForceHideChangeFunc_ = std::move(callback);
    }, __func__);
}

void SceneSession::RegisterClearCallbackMapCallback(ClearCallbackMapFunc&& callback)
{
    PostTask([weakThis = wptr(this), callback = std::move(callback), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is nullptr", where);
            return;
        }
        session->clearCallbackMapFunc_ = std::move(callback);
    }, __func__);
}

WSError SceneSession::Disconnect(bool isFromClient, const std::string& identityToken)
{
    if (isFromClient && SessionHelper::IsMainWindow(GetWindowType())) {
        if (!CheckPidIfMatched() || !CheckIdentityTokenIfMatched(identityToken)) {
            TLOGW(WmsLogTag::WMS_LIFE, "check failed");
            return WSError::WS_OK;
        }
    }
    return DisconnectTask(isFromClient, true);
}

WSError SceneSession::DisconnectTask(bool isFromClient, bool isSaveSnapshot)
{
    PostTask([weakThis = wptr(this), isFromClient, isSaveSnapshot, where = __func__]()
        THREAD_SAFETY_GUARD(SCENE_GUARD) {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (isFromClient) {
            TLOGNI(WmsLogTag::WMS_LIFE, "%{public}s Client need notify destroy session, id: %{public}d",
                where, session->GetPersistentId());
            session->SetSessionState(SessionState::STATE_DISCONNECT);
            return WSError::WS_OK;
        }
        auto state = session->GetSessionState();
        auto isMainWindow = SessionHelper::IsMainWindow(session->GetWindowType());
        if ((session->needSnapshot_ || (state == SessionState::STATE_ACTIVE && isMainWindow)) && isSaveSnapshot) {
            session->SaveSnapshot(false);
        }
        session->Session::Disconnect(isFromClient);
        session->isTerminating_ = false;
        if (session->specificCallback_ != nullptr) {
            session->specificCallback_->onHandleSecureSessionShouldHide_(session);
            session->isEnableGestureBack_ = true;
            session->UpdateGestureBackEnabled();
            session->isEnableGestureBackHadSet_ = false;
        }
        return WSError::WS_OK;
    }, __func__);
    return WSError::WS_OK;
}

WSError SceneSession::UpdateActiveStatus(bool isActive)
{
    PostTask([weakThis = wptr(this), isActive, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (!session->IsSessionValid()) {
            TLOGNW(WmsLogTag::WMS_LIFE, "%{public}s Session is invalid, id: %{public}d state: %{public}u",
                where, session->GetPersistentId(), session->GetSessionState());
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        if (isActive == session->isActive_) {
            TLOGND(WmsLogTag::WMS_LIFE, "%{public}s Session active do not change: %{public}d",
                where, isActive);
            return WSError::WS_DO_NOTHING;
        }

        WSError ret = WSError::WS_DO_NOTHING;
        if (isActive && session->GetSessionState() == SessionState::STATE_FOREGROUND) {
            session->UpdateSessionState(SessionState::STATE_ACTIVE);
            session->isActive_ = isActive;
            ret = WSError::WS_OK;
        }
        if (!isActive && session->GetSessionState() == SessionState::STATE_ACTIVE) {
            session->UpdateSessionState(SessionState::STATE_INACTIVE);
            session->isActive_ = isActive;
            ret = WSError::WS_OK;
        }
        TLOGNI(WmsLogTag::WMS_LIFE, "%{public}s isActive: %{public}d, state: %{public}u",
            where, session->isActive_, session->GetSessionState());
        return ret;
    }, std::string(__func__) + ":" + std::to_string(isActive));
    return WSError::WS_OK;
}

DMRect SceneSession::CalcRectForStatusBar()
{
    DMRect statusBarRect = {0, 0, 0, 0};
    if (specificCallback_ == nullptr || specificCallback_->onGetSceneSessionVectorByTypeAndDisplayId_ == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "specificCallback is null");
        return statusBarRect;
    }
    const auto& statusBarVector = specificCallback_->onGetSceneSessionVectorByTypeAndDisplayId_(
        WindowType::WINDOW_TYPE_STATUS_BAR, GetSessionProperty()->GetDisplayId());
    for (auto& statusBar : statusBarVector) {
        if (statusBar == nullptr) {
            continue;
        }
        if (statusBar->IsVisible() &&
            static_cast<uint32_t>(statusBar->GetSessionRect().height_) > statusBarRect.height_) {
            statusBarRect.height_ = static_cast<uint32_t>(statusBar->GetSessionRect().height_);
        }
        if (static_cast<uint32_t>(statusBar->GetSessionRect().width_) > statusBarRect.width_) {
            statusBarRect.width_ = static_cast<uint32_t>(statusBar->GetSessionRect().width_);
        }
    }
    TLOGD(
        WmsLogTag::WMS_KEYBOARD, "width: %{public}d, height: %{public}d", statusBarRect.width_, statusBarRect.height_);
    return statusBarRect;
}

WSError SceneSession::SetMoveAvailableArea(DisplayId displayId)
{
    sptr<Display> display = DisplayManager::GetInstance().GetDisplayById(displayId);
    if (display == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "fail to get display");
        return WSError::WS_ERROR_INVALID_DISPLAY;
    }

    DMRect availableArea;
    DMError ret = display->GetAvailableArea(availableArea);
    if (ret != DMError::DM_OK) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Failed to get available area, ret: %{public}d", ret);
        return WSError::WS_ERROR_INVALID_DISPLAY;
    }

    DMRect statusBarRect = CalcRectForStatusBar();
    if (systemConfig_.IsPadWindow() || systemConfig_.IsPhoneWindow()) {
        uint32_t statusBarHeight = statusBarRect.height_;
        if (statusBarHeight > availableArea.posY_) {
            availableArea.posY_ = statusBarHeight;
        }

        sptr<ScreenSession> currentScreenSession =
            ScreenSessionManagerClient::GetInstance().GetScreenSessionById(GetSessionProperty()->GetDisplayId());
        if (currentScreenSession == nullptr) {
            TLOGW(WmsLogTag::WMS_KEYBOARD, "Screen session is null");
            return WSError::WS_ERROR_INVALID_DISPLAY;
        }
        uint32_t currentScreenHeight = currentScreenSession->GetScreenProperty().GetBounds().rect_.height_;
        availableArea.height_ = currentScreenHeight - static_cast<uint32_t>(availableArea.posY_);
    }

    bool isFoldable = ScreenSessionManagerClient::GetInstance().IsFoldable();
    if (systemConfig_.IsPhoneWindow() && isFoldable && statusBarRect.width_) {
        availableArea.width_ = statusBarRect.width_;
    }
    TLOGD(WmsLogTag::WMS_KEYBOARD,
          "the available area x is: %{public}d, y is: %{public}d, width is: %{public}d, height is: %{public}d",
          availableArea.posX_, availableArea.posY_, availableArea.width_, availableArea.height_);
    moveDragController_->SetMoveAvailableArea(availableArea);
    return WSError::WS_OK;
}

WSError SceneSession::InitializeMoveInputBar()
{
    auto property = GetSessionProperty();
    WindowType windowType = property->GetWindowType();
    if (WindowHelper::IsInputWindow(windowType)) {
        TLOGD(WmsLogTag::WMS_KEYBOARD, "Start init move input bar param");
        DisplayId displayId = property->GetDisplayId();

        WSError ret = SetMoveAvailableArea(displayId);
        if (ret != WSError::WS_OK) {
            TLOGD(WmsLogTag::WMS_KEYBOARD, "set move availableArea error");
            return WSError::WS_ERROR_INVALID_OPERATION;
        }
        moveDragController_->SetMoveInputBarStartDisplayId(displayId);
    }
    return WSError::WS_OK;
}

WSError SceneSession::OnSessionEvent(SessionEvent event)
{
    PostTask([weakThis = wptr(this), event, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        TLOGNI(WmsLogTag::WMS_LIFE, "%{public}s event: %{public}d", where, static_cast<int32_t>(event));
        session->UpdateWaterfallMode(event);
        if (event == SessionEvent::EVENT_START_MOVE) {
            if (!session->IsMovable()) {
                return WSError::WS_OK;
            }
            HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSession::StartMove");
            WSError ret = session->InitializeMoveInputBar();
            if (ret != WSError::WS_OK) {
                return ret;
            }
            session->InitializeCrossMoveDrag();
            session->moveDragController_->InitMoveDragProperty();
            if (session->pcFoldScreenController_) {
                session->pcFoldScreenController_->RecordStartMoveRect(session->GetSessionRect(),
                    session->IsFullScreenMovable());
            }
            WSRect rect = session->winRect_;
            if (session->IsFullScreenMovable()) {
                session->UpdateFullScreenWaterfallMode(false);
                rect = session->moveDragController_->GetFullScreenToFloatingRect(session->winRect_,
                    session->GetSessionRequestRect());
                session->Session::UpdateRect(rect, SizeChangeReason::RECOVER, where, nullptr);
                session->moveDragController_->SetStartMoveFlag(true);
                session->moveDragController_->CalcFirstMoveTargetRect(rect, true);
            } else {
                session->moveDragController_->SetStartMoveFlag(true);
                session->moveDragController_->CalcFirstMoveTargetRect(rect, false);
            }
            session->SetSessionEventParam({session->moveDragController_->GetOriginalPointerPosX(),
                session->moveDragController_->GetOriginalPointerPosY(), rect.width_, rect.height_});
        }
        session->HandleSessionDragEvent(event);
        if (session->onSessionEvent_) {
            session->onSessionEvent_(static_cast<uint32_t>(event), session->sessionEventParam_);
        }
        return WSError::WS_OK;
    }, std::string(__func__) + ":" + std::to_string(static_cast<uint32_t>(event)));
    return WSError::WS_OK;
}

void SceneSession::HandleSessionDragEvent(SessionEvent event)
{
    if (moveDragController_ &&
        (event == SessionEvent::EVENT_DRAG || event == SessionEvent::EVENT_DRAG_START)) {
        WSRect rect = moveDragController_->GetTargetRect(
            MoveDragController::TargetRectCoordinate::RELATED_TO_START_DISPLAY);
        DragResizeType dragResizeType = DragResizeType::RESIZE_TYPE_UNDEFINED;
        if (event == SessionEvent::EVENT_DRAG_START) {
            dragResizeType = GetAppDragResizeType();
            SetDragResizeTypeDuringDrag(dragResizeType);
        }
        SetSessionEventParam({rect.posX_, rect.posY_, rect.width_, rect.height_,
            static_cast<uint32_t>(dragResizeType)});
    }
}

void SceneSession::UpdateWaterfallMode(SessionEvent event)
{
    if (pcFoldScreenController_ == nullptr) {
        return;
    }
    switch (event) {
        case SessionEvent::EVENT_MAXIMIZE_WATERFALL:
            UpdateFullScreenWaterfallMode(pcFoldScreenController_->IsHalfFolded(GetScreenId()));
            break;
        case SessionEvent::EVENT_WATERFALL_TO_MAXIMIZE:
        case SessionEvent::EVENT_RECOVER:
        case SessionEvent::EVENT_SPLIT_PRIMARY:
        case SessionEvent::EVENT_SPLIT_SECONDARY:
            UpdateFullScreenWaterfallMode(false);
            break;
        default:
            break;
    }
}

WSError SceneSession::SyncSessionEvent(SessionEvent event)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "the sync session event is: %{public}d", event);
    if (event != SessionEvent::EVENT_START_MOVE && event != SessionEvent::EVENT_END_MOVE) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "This is not start move event or end move event, "
            "eventId=%{public}u windowId=%{public}d", event, GetPersistentId());
        return WSError::WS_ERROR_NULLPTR;
    }
    return PostSyncTask([weakThis = wptr(this), event, where = __func__] {
        auto session = weakThis.promote();
        if (!session || !session->moveDragController_) {
            TLOGNW(WmsLogTag::WMS_LAYOUT, "%{public}s: session or moveDragController is null", where);
            return WSError::WS_ERROR_NULLPTR;
        }
        if (event == SessionEvent::EVENT_END_MOVE) {
            if (!session->moveDragController_->GetStartMoveFlag()) {
                TLOGNW(WmsLogTag::WMS_LAYOUT_PC, "%{public}s Repeat operation, window is not moving", where);
                return WSError::WS_OK;
            }
            session->moveDragController_->StopMoving();
            return WSError::WS_OK;
        }
        if (session->moveDragController_->GetStartMoveFlag()) {
            TLOGNW(WmsLogTag::WMS_LAYOUT, "%{public}s: Repeat operation, system window is moving", where);
            return WSError::WS_ERROR_REPEAT_OPERATION;
        }
        session->OnSessionEvent(event);
        return WSError::WS_OK;
    }, __func__);
}

WSError SceneSession::StartMovingWithCoordinate(int32_t offsetX, int32_t offsetY,
    int32_t pointerPosX, int32_t pointerPosY)
{
    return PostSyncTask([weakThis = wptr(this), offsetX, offsetY, pointerPosX, pointerPosY, where = __func__] {
        auto session = weakThis.promote();
        if (!session || !session->moveDragController_) {
            TLOGNW(WmsLogTag::WMS_LAYOUT_PC, "%{public}s: session or moveDragController is null", where);
            return WSError::WS_ERROR_NULLPTR;
        }
        if (session->moveDragController_->GetStartMoveFlag()) {
            TLOGNW(WmsLogTag::WMS_LAYOUT_PC, "%{public}s: Repeat operation, window is moving", where);
            return WSError::WS_ERROR_REPEAT_OPERATION;
        }
        TLOGND(WmsLogTag::WMS_LAYOUT_PC, "%{public}s: offsetX:%{public}d offsetY:%{public}d pointerPosX:%{public}d"
            " pointerPosY:%{public}d", where, offsetX, offsetY, pointerPosX, pointerPosY);
        WSRect winRect = {
            pointerPosX - offsetX,
            pointerPosY - offsetY,
            session->winRect_.width_,
            session->winRect_.height_
        };
        session->moveDragController_->HandleStartMovingWithCoordinate(offsetX,
            offsetY, pointerPosX, pointerPosY, winRect);
        session->OnSessionEvent(SessionEvent::EVENT_START_MOVE);
        return WSError::WS_OK;
    }, __func__);
}

uint32_t SceneSession::GetWindowDragHotAreaType(DisplayId displayId, uint32_t type, int32_t pointerX, int32_t pointerY)
{
    std::shared_lock<std::shared_mutex> lock(windowDragHotAreaMutex_);
    if (windowDragHotAreaMap_.find(displayId) == windowDragHotAreaMap_.end()) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "Display is invalid.");
        return type;
    }
    for (const auto& [key, rect] : windowDragHotAreaMap_[displayId]) {
        if (rect.IsInRegion(pointerX, pointerY)) {
            type |= key;
        }
    }
    return type;
}

void SceneSession::AddOrUpdateWindowDragHotArea(DisplayId displayId, uint32_t type, const WSRect& area)
{
    std::unique_lock<std::shared_mutex> lock(windowDragHotAreaMutex_);
    windowDragHotAreaMap_[displayId].insert_or_assign(type, area);
}

WSError SceneSession::NotifySubModalTypeChange(SubWindowModalType subWindowModalType)
{
    PostTask([weakThis = wptr(this), subWindowModalType, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return;
        }
        TLOGNI(WmsLogTag::WMS_HIERARCHY, "%{public}s subWindowModalType: %{public}u",
            where, static_cast<uint32_t>(subWindowModalType));
        if (session->onSubModalTypeChange_) {
            session->onSubModalTypeChange_(subWindowModalType);
        }
    }, __func__);
    return WSError::WS_OK;
}

void SceneSession::RegisterSubModalTypeChangeCallback(NotifySubModalTypeChangeFunc&& func)
{
    PostTask([weakThis = wptr(this), func = std::move(func), where = __func__] {
        auto session = weakThis.promote();
        if (!session || !func) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session or SessionModalTypeChangeFunc is null", where);
            return;
        }
        session->onSubModalTypeChange_ = std::move(func);
        TLOGNI(WmsLogTag::WMS_HIERARCHY, "%{public}s id: %{public}d",
            where, session->GetPersistentId());
    }, __func__);
}

void SceneSession::RegisterMainModalTypeChangeCallback(NotifyMainModalTypeChangeFunc&& func)
{
    PostTask([weakThis = wptr(this), func = std::move(func), where = __func__] {
        auto session = weakThis.promote();
        if (!session || !func) {
            TLOGNE(WmsLogTag::WMS_MAIN, "%{public}s session or func is null", where);
            return;
        }
        session->onMainModalTypeChange_ = std::move(func);
        TLOGNI(WmsLogTag::WMS_MAIN, "%{public}s id: %{public}d", where, session->GetPersistentId());
    }, __func__);
}

bool SceneSession::IsDialogWindow() const
{
    return WindowHelper::IsDialogWindow(GetSessionProperty()->GetWindowType());
}

SubWindowModalType SceneSession::GetSubWindowModalType() const
{
    SubWindowModalType modalType = SubWindowModalType::TYPE_UNDEFINED;
    auto property = GetSessionProperty();
    if (property == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "property is nullptr");
        return modalType;
    }
    auto windowType = property->GetWindowType();
    if (WindowHelper::IsToastSubWindow(windowType, property->GetWindowFlags())) {
        return SubWindowModalType::TYPE_TOAST;
    }
    if (WindowHelper::IsTextMenuSubWindow(windowType, property->GetWindowFlags())) {
        return SubWindowModalType::TYPE_TEXT_MENU;
    }
    if (WindowHelper::IsDialogWindow(windowType)) {
        modalType = SubWindowModalType::TYPE_DIALOG;
    } else if (WindowHelper::IsModalSubWindow(windowType, property->GetWindowFlags())) {
        if (WindowHelper::IsApplicationModalSubWindow(windowType, property->GetWindowFlags())) {
            modalType = SubWindowModalType::TYPE_APPLICATION_MODALITY;
        } else {
            modalType = SubWindowModalType::TYPE_WINDOW_MODALITY;
        }
    } else if (WindowHelper::IsSubWindow(windowType)) {
        modalType = SubWindowModalType::TYPE_NORMAL;
    }
    return modalType;
}

void SceneSession::SetSessionEventParam(SessionEventParam param)
{
    sessionEventParam_ = param;
}

void SceneSession::RegisterSessionEventCallback(NotifySessionEventFunc&& callback)
{
    PostTask([weakThis = wptr(this), callback = std::move(callback), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s session is null", where);
            return;
        }
        session->onSessionEvent_ = std::move(callback);
    }, __func__);
}

void SceneSession::RegisterUpdateAppUseControlCallback(UpdateAppUseControlFunc&& callback)
{
    PostTask([weakThis = wptr(this), callback = std::move(callback), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return;
        }
        session->onUpdateAppUseControlFunc_ = std::move(callback);
        std::string key =
            session->GetSessionInfo().bundleName_ + "#" + std::to_string(session->GetSessionInfo().appIndex_);
        if (allAppUseControlMap_.find(key) == allAppUseControlMap_.end()) {
            return;
        }
        for (const auto& [type, info] : allAppUseControlMap_[key]) {
            TLOGNI(WmsLogTag::WMS_LIFE,
                "notify appUseControl when register, key: %{public}s, control: %{public}d, controlRecent: %{public}d",
                key.c_str(), info.isNeedControl, info.isControlRecentOnly);
            session->onUpdateAppUseControlFunc_(type, info.isNeedControl, info.isControlRecentOnly);
        }
    }, __func__);
}

void SceneSession::NotifyUpdateAppUseControl(ControlAppType type, const ControlInfo& controlInfo)
{
    PostTask([weakThis = wptr(this), type, controlInfo, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return;
        }
        session->appUseControlMap_[type] = controlInfo;
        if (session->onUpdateAppUseControlFunc_) {
            session->onUpdateAppUseControlFunc_(type, controlInfo.isNeedControl, controlInfo.isControlRecentOnly);
        }
    }, __func__);
}

void SceneSession::RegisterDefaultAnimationFlagChangeCallback(NotifyWindowAnimationFlagChangeFunc&& callback)
{
    PostTask([weakThis = wptr(this), callback = std::move(callback), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return;
        }
        session->onWindowAnimationFlagChange_ = std::move(callback);
        session->onWindowAnimationFlagChange_(session->IsNeedDefaultAnimation());
    }, __func__);
}

void SceneSession::RegisterDefaultDensityEnabledCallback(NotifyDefaultDensityEnabledFunc&& callback)
{
    PostTask([weakThis = wptr(this), callback = std::move(callback), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return;
        }
        session->onDefaultDensityEnabledFunc_ = std::move(callback);
    }, __func__);
}

void SceneSession::RegisterNeedAvoidCallback(NotifyNeedAvoidFunc&& callback)
{
    PostTask([weakThis = wptr(this), callback = std::move(callback), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_IMMS, "%{public}s session is null", where);
            return;
        }
        session->onNeedAvoid_ = std::move(callback);
    }, __func__);
}

void SceneSession::RegisterSystemBarPropertyChangeCallback(NotifySystemBarPropertyChangeFunc&& callback)
{
    PostTask([weakThis = wptr(this), callback = std::move(callback), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return;
        }
        session->onSystemBarPropertyChange_ = std::move(callback);
    }, __func__);
}

void SceneSession::RegisterTouchOutsideCallback(NotifyTouchOutsideFunc&& callback)
{
    PostTask([weakThis = wptr(this), callback = std::move(callback), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return;
        }
        session->onTouchOutside_ = std::move(callback);
    }, __func__);
}

WSError SceneSession::SetGlobalMaximizeMode(MaximizeMode mode)
{
    return PostSyncTask([weakThis = wptr(this), mode, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        TLOGND(WmsLogTag::WMS_PC, "%{public}s mode: %{public}u", where, static_cast<uint32_t>(mode));
        session->maximizeMode_ = mode;
        ScenePersistentStorage::Insert("maximize_state", static_cast<int32_t>(session->maximizeMode_),
            ScenePersistentStorageType::MAXIMIZE_STATE);
        return WSError::WS_OK;
    }, __func__);
}

WSError SceneSession::GetGlobalMaximizeMode(MaximizeMode& mode)
{
    return PostSyncTask([weakThis = wptr(this), &mode, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_PC, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        mode = maximizeMode_;
        TLOGND(WmsLogTag::WMS_PC, "%{public}s mode: %{public}u", where, static_cast<uint32_t>(mode));
        return WSError::WS_OK;
    }, __func__);
}

static WSError CheckAspectRatioValid(const sptr<SceneSession>& session, float ratio, float vpr)
{
    if (MathHelper::NearZero(ratio)) {
        return WSError::WS_OK;
    }
    if (!session) {
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    auto sessionProperty = session->GetSessionProperty();
    if (!sessionProperty) {
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    auto limits = sessionProperty->GetWindowLimits();
    if (session->IsDecorEnable()) {
        if (limits.minWidth_ && limits.maxHeight_ &&
            MathHelper::LessNotEqual(ratio, SessionUtils::ToLayoutWidth(limits.minWidth_, vpr) /
            SessionUtils::ToLayoutHeight(limits.maxHeight_, vpr))) {
            WLOGE("Failed, because aspectRatio is smaller than minWidth/maxHeight");
            return WSError::WS_ERROR_INVALID_PARAM;
        } else if (limits.minHeight_ && limits.maxWidth_ &&
            MathHelper::GreatNotEqual(ratio, SessionUtils::ToLayoutWidth(limits.maxWidth_, vpr) /
            SessionUtils::ToLayoutHeight(limits.minHeight_, vpr))) {
            WLOGE("Failed, because aspectRatio is bigger than maxWidth/minHeight");
            return WSError::WS_ERROR_INVALID_PARAM;
        }
    } else {
        if (limits.minWidth_ && limits.maxHeight_ && MathHelper::LessNotEqual(ratio,
            static_cast<float>(limits.minWidth_) / limits.maxHeight_)) {
            WLOGE("Failed, because aspectRatio is smaller than minWidth/maxHeight");
            return WSError::WS_ERROR_INVALID_PARAM;
        } else if (limits.minHeight_ && limits.maxWidth_ && MathHelper::GreatNotEqual(ratio,
            static_cast<float>(limits.maxWidth_) / limits.minHeight_)) {
            WLOGE("Failed, because aspectRatio is bigger than maxWidth/minHeight");
            return WSError::WS_ERROR_INVALID_PARAM;
        }
    }
    return WSError::WS_OK;
}

/** @note @window.layout */
WSError SceneSession::SetAspectRatio(float ratio)
{
    return PostSyncTask([weakThis = wptr(this), ratio, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        float vpr = 1.5f; // 1.5f: default virtual pixel ratio
        auto display = DisplayManager::GetInstance().GetDefaultDisplay();
        if (display) {
            vpr = display->GetVirtualPixelRatio();
            TLOGND(WmsLogTag::WMS_LAYOUT, "%{public}s vpr=%{public}f", where, vpr);
        }
        WSError ret = CheckAspectRatioValid(session, ratio, vpr);
        if (ret != WSError::WS_OK) {
            return ret;
        }
        session->aspectRatio_ = ratio;
        if (session->moveDragController_) {
            session->moveDragController_->SetAspectRatio(ratio);
        }
        session->SaveAspectRatio(session->aspectRatio_);
        WSRect adjustedRect = session->winRect_;
        TLOGNI(WmsLogTag::WMS_LAYOUT, "%{public}s Before adjusting, the id:%{public}d, the current rect:%{public}s, "
            "ratio:%{public}f", where, session->GetPersistentId(), adjustedRect.ToString().c_str(), ratio);
        if (session->AdjustRectByAspectRatio(adjustedRect)) {
            TLOGNI(WmsLogTag::WMS_LAYOUT, "%{public}s After adjusting, the id:%{public}d, the adjusted rect:%{public}s",
                where, session->GetPersistentId(), adjustedRect.ToString().c_str());
            session->NotifySessionRectChange(adjustedRect, SizeChangeReason::RESIZE);
        }
        return WSError::WS_OK;
    }, __func__);
}

/** @note @window.layout */
WSError SceneSession::UpdateRect(const WSRect& rect, SizeChangeReason reason,
    const std::string& updateReason, const std::shared_ptr<RSTransaction>& rsTransaction)
{
    PostTask([weakThis = wptr(this), rect, reason, rsTransaction, updateReason, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s: session is null", where);
            return;
        }
        if (session->reason_ == SizeChangeReason::DRAG) {
            TLOGND(WmsLogTag::WMS_LAYOUT, "%{public}s: skip drag reason update id:%{public}d rect:%{public}s",
                where, session->GetPersistentId(), rect.ToString().c_str());
            return;
        }
        if (session->winRect_ == rect && session->reason_ != SizeChangeReason::DRAG_END &&
            (session->GetWindowType() != WindowType::WINDOW_TYPE_KEYBOARD_PANEL &&
             session->GetWindowType() != WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT)) {
            if (!session->sessionStage_) {
                TLOGND(WmsLogTag::WMS_LAYOUT, "%{public}s: skip same rect update id:%{public}d rect:%{public}s",
                    where, session->GetPersistentId(), rect.ToString().c_str());
                return;
            } else if (session->GetClientRect() == rect) {
                TLOGND(WmsLogTag::WMS_LAYOUT, "%{public}s: skip same rect update id:%{public}d rect:%{public}s "
                    "clientRect:%{public}s", where, session->GetPersistentId(), rect.ToString().c_str(),
                    session->GetClientRect().ToString().c_str());
                return;
            }
        }
        if (rect.IsInvalid()) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s: id:%{public}d rect:%{public}s is invalid",
                where, session->GetPersistentId(), rect.ToString().c_str());
            return;
        }
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSession::UpdateRect %d [%d, %d, %u, %u]",
            session->GetPersistentId(), rect.posX_, rect.posY_, rect.width_, rect.height_);
        // position change no need to notify client, since frame layout finish will notify
        if (NearEqual(rect.width_, session->winRect_.width_) && NearEqual(rect.height_, session->winRect_.height_) &&
            (session->reason_ != SizeChangeReason::DRAG_MOVE || !session->rectChangeListenerRegistered_)) {
            TLOGND(WmsLogTag::WMS_LAYOUT, "%{public}s: position change no need notify client id:%{public}d, "
                "rect:%{public}s, preRect:%{public}s", where,
                session->GetPersistentId(), rect.ToString().c_str(), session->winRect_.ToString().c_str());
            session->SetWinRectWhenUpdateRect(rect);
        } else {
            session->SetWinRectWhenUpdateRect(rect);
            session->NotifyClientToUpdateRect(updateReason, rsTransaction);
        }
        session->dirtyFlags_ |= static_cast<uint32_t>(SessionUIDirtyFlag::RECT);
        TLOGNI(WmsLogTag::WMS_LAYOUT, "%{public}s: id:%{public}d, reason:%{public}d %{public}s, "
            "rect:%{public}s, clientRect:%{public}s",
            where, session->GetPersistentId(), session->reason_, updateReason.c_str(),
            rect.ToString().c_str(), session->GetClientRect().ToString().c_str());
    }, __func__ + GetRectInfo(rect));
    return WSError::WS_OK;
}

/** @note @window.layout */
void SceneSession::SetWinRectWhenUpdateRect(const WSRect& rect)
{
    if (GetIsMidScene() && rect.posX_ == 0 && rect.posY_ == 0) {
        winRect_.width_ = rect.width_;
        winRect_.height_ = rect.height_;
    } else {
        winRect_ = rect;
    }
}

WSError SceneSession::NotifyClientToUpdateRectTask(const std::string& updateReason,
    std::shared_ptr<RSTransaction> rsTransaction)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "id:%{public}d, reason:%{public}d, rect:%{public}s",
        GetPersistentId(), reason_, winRect_.ToString().c_str());
    bool isMoveOrDrag = moveDragController_ &&
        (moveDragController_->GetStartDragFlag() || moveDragController_->GetStartMoveFlag());
    if (isMoveOrDrag && reason_ == SizeChangeReason::UNDEFINED) {
        TLOGD(WmsLogTag::WMS_LAYOUT, "skip redundant rect update!");
        return WSError::WS_ERROR_REPEAT_OPERATION;
    }
    WSError ret = WSError::WS_OK;
    if (reason_ != SizeChangeReason::DRAG_MOVE) {
        UpdateCrossAxisOfLayout(winRect_);
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER,
        "SceneSession::NotifyClientToUpdateRect%d [%d, %d, %u, %u] reason:%u",
        GetPersistentId(), winRect_.posX_, winRect_.posY_, winRect_.width_, winRect_.height_, reason_);

    std::map<AvoidAreaType, AvoidArea> avoidAreas;
    if (GetForegroundInteractiveStatus()) {
        GetAllAvoidAreas(avoidAreas);
    } else {
        TLOGD(WmsLogTag::WMS_IMMS, "win [%{public}d] avoid area update rejected by recent", GetPersistentId());
    }
    // once reason is undefined, not use rsTransaction
    // when rotation, sync cnt++ in marshalling. Although reason is undefined caused by resize
    if (reason_ == SizeChangeReason::UNDEFINED || reason_ == SizeChangeReason::RESIZE || IsMoveToOrDragMove(reason_)) {
        ret = Session::UpdateRectWithLayoutInfo(winRect_, reason_, updateReason, nullptr, avoidAreas);
    } else {
        ret = Session::UpdateRectWithLayoutInfo(winRect_, reason_, updateReason, rsTransaction, avoidAreas);
#ifdef DEVICE_STATUS_ENABLE
        // When the drag is in progress, the drag window needs to be notified to rotate.
        if (rsTransaction != nullptr) {
            RotateDragWindow(rsTransaction);
        }
#endif // DEVICE_STATUS_ENABLE
    }

    return ret;
}

WSError SceneSession::NotifyClientToUpdateRect(const std::string& updateReason,
    std::shared_ptr<RSTransaction> rsTransaction)
{
    PostTask([weakThis = wptr(this), rsTransaction, updateReason, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        WSError ret = session->NotifyClientToUpdateRectTask(updateReason, rsTransaction);
        if (ret != WSError::WS_OK) {
            return ret;
        }
        if (session->specificCallback_ && session->specificCallback_->onUpdateOccupiedAreaIfNeed_) {
            session->specificCallback_->onUpdateOccupiedAreaIfNeed_(session->GetPersistentId());
        }
        return ret;
    }, __func__);
    return WSError::WS_OK;
}

bool SceneSession::GetScreenWidthAndHeightFromServer(const sptr<WindowSessionProperty>& sessionProperty,
    uint32_t& screenWidth, uint32_t& screenHeight)
{
    const auto& screenSession = sessionProperty == nullptr ? nullptr :
        ScreenSessionManagerClient::GetInstance().GetScreenSession(sessionProperty->GetDisplayId());
    if (screenSession != nullptr) {
        screenWidth = screenSession->GetScreenProperty().GetBounds().rect_.width_;
        screenHeight = screenSession->GetScreenProperty().GetBounds().rect_.height_;
    } else {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "sessionProperty or screenSession is nullptr, use defaultDisplayInfo");
        auto defaultDisplayInfo = DisplayManager::GetInstance().GetDefaultDisplay();
        if (defaultDisplayInfo != nullptr) {
            screenWidth = static_cast<uint32_t>(defaultDisplayInfo->GetWidth());
            screenHeight = static_cast<uint32_t>(defaultDisplayInfo->GetHeight());
        } else {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "defaultDisplayInfo is null, get screenWidthAndHeight failed");
            return false;
        }
    }
    if (IsSystemKeyboard() && PcFoldScreenManager::GetInstance().IsHalfFolded(GetScreenId())) {
        const auto& [defaultDisplayRect, virtualDisplayRect, foldCreaseRect] =
            PcFoldScreenManager::GetInstance().GetDisplayRects();
        screenHeight = virtualDisplayRect.posY_ + virtualDisplayRect.height_;
        TLOGI(WmsLogTag::WMS_KEYBOARD, "id: %{public}d, virtual display posY: %{public}d, height: %{public}d",
            GetPersistentId(), virtualDisplayRect.posY_, virtualDisplayRect.height_);
    }
    TLOGI(WmsLogTag::WMS_KEYBOARD, "screenWidth: %{public}d, screenHeight: %{public}d", screenWidth, screenHeight);
    return true;
}

bool SceneSession::GetScreenWidthAndHeightFromClient(const sptr<WindowSessionProperty>& sessionProperty,
    uint32_t& screenWidth, uint32_t& screenHeight)
{
    auto defaultDisplayInfo = DisplayManager::GetInstance().GetDefaultDisplay();
    if (defaultDisplayInfo != nullptr) {
        screenWidth = static_cast<uint32_t>(defaultDisplayInfo->GetWidth());
        screenHeight = static_cast<uint32_t>(defaultDisplayInfo->GetHeight());
    } else {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "defaultDisplayInfo is null, get screenWidthAndHeight failed");
        return false;
    }
    if (IsSystemKeyboard() && PcFoldScreenManager::GetInstance().IsHalfFolded(GetScreenId())) {
        const auto& [defaultDisplayRect, virtualDisplayRect, foldCreaseRect] =
            PcFoldScreenManager::GetInstance().GetDisplayRects();
        screenHeight = virtualDisplayRect.posY_ + virtualDisplayRect.height_;
        TLOGI(WmsLogTag::WMS_KEYBOARD, "id: %{public}d, virtual display posY: %{public}d, height: %{public}d",
            GetPersistentId(), virtualDisplayRect.posY_, virtualDisplayRect.height_);
    }
    TLOGI(WmsLogTag::WMS_KEYBOARD, "screenWidth: %{public}d, screenHeight: %{public}d", screenWidth, screenHeight);
    return true;
}

void SceneSession::SetSessionRectChangeCallback(const NotifySessionRectChangeFunc& func)
{
    PostTask([weakThis = wptr(this), func, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        session->sessionRectChangeFunc_ = func;
        if (session->sessionRectChangeFunc_ && session->GetWindowType() != WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
            auto reason = SizeChangeReason::UNDEFINED;
            auto rect = session->GetSessionRequestRect();
            if (rect.width_ == 0 && rect.height_ == 0) {
                reason = SizeChangeReason::MOVE;
            }
            TLOGND(WmsLogTag::WMS_LAYOUT, "%{public}s, winName:%{public}s, reason:%{public}d, rect:%{public}s",
                where, session->GetWindowName().c_str(), reason, rect.ToString().c_str());
            auto rectAnimationConfig = session->GetRequestRectAnimationConfig();
            session->sessionRectChangeFunc_(rect, reason, DISPLAY_ID_INVALID, rectAnimationConfig);
        }
        return WSError::WS_OK;
    }, __func__);
}

void SceneSession::SetSessionDisplayIdChangeCallback(NotifySessionDisplayIdChangeFunc&& func)
{
    PostTask([weakThis = wptr(this), func = std::move(func), where = __func__] {
        auto session = weakThis.promote();
        if (!session || !func) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s session or display id is null", where);
            return;
        }
        session->sessionDisplayIdChangeFunc_ = std::move(func);
    }, __func__);
}

void SceneSession::SetMainWindowTopmostChangeCallback(NotifyMainWindowTopmostChangeFunc&& func)
{
    PostTask([weakThis = wptr(this), func = std::move(func), where = __func__] {
        auto session = weakThis.promote();
        if (!session || !func) {
            TLOGNE(WmsLogTag::WMS_HIERARCHY, "%{public}s session or func is null", where);
            return;
        }
        session->mainWindowTopmostChangeFunc_ = std::move(func);
    }, __func__);
}

void SceneSession::SetTitleAndDockHoverShowChangeCallback(NotifyTitleAndDockHoverShowChangeFunc&& func)
{
    PostTask([weakThis = wptr(this), func = std::move(func), where = __func__] {
        auto session = weakThis.promote();
        if (!session || !func) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s session or TitleAndDockHoverShowChangeFunc is null", where);
            return;
        }
        session->onTitleAndDockHoverShowChangeFunc_ = std::move(func);
        TLOGND(WmsLogTag::WMS_LAYOUT_PC, "%{public}s id: %{public}d",
            where, session->GetPersistentId());
    }, __func__);
}

void SceneSession::SetRestoreMainWindowCallback(NotifyRestoreMainWindowFunc&& func)
{
    PostTask([weakThis = wptr(this), func = std::move(func), where = __func__] {
        auto session = weakThis.promote();
        if (!session || !func) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s session or RestoreMainWindowFunc is null", where);
            return;
        }
        session->onRestoreMainWindowFunc_ = std::move(func);
        TLOGND(WmsLogTag::WMS_LAYOUT_PC, "%{public}s id: %{public}d",
            where, session->GetPersistentId());
    }, __func__);
}

void SceneSession::SetAdjustKeyboardLayoutCallback(const NotifyKeyboardLayoutAdjustFunc& func)
{
    PostTask([weakThis = wptr(this), func, where = __func__] {
        auto session = weakThis.promote();
        if (!session || !func) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "%{public}s session or keyboardLayoutFunc is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        session->adjustKeyboardLayoutFunc_ = func;
        auto property = session->GetSessionProperty();
        if (property == nullptr) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "%{public}s property is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        KeyboardLayoutParams params = property->GetKeyboardLayoutParams();
        session->adjustKeyboardLayoutFunc_(params);
        TLOGNI(WmsLogTag::WMS_KEYBOARD,
            "%{public}s Notify adjust keyboard layout when register, keyboardId: %{public}d, "
            "gravity: %{public}u, landscapeAvoidHeight: %{public}d, PortraitAvoidHeight: %{public}d, "
            "LandscapeKeyboardRect: %{public}s, PortraitKeyboardRect: %{public}s, "
            "LandscapePanelRect: %{public}s, PortraitPanelRect: %{public}s", where, session->GetPersistentId(),
            static_cast<uint32_t>(params.gravity_), params.landscapeAvoidHeight_, params.portraitAvoidHeight_,
            params.LandscapeKeyboardRect_.ToString().c_str(), params.PortraitKeyboardRect_.ToString().c_str(),
            params.LandscapePanelRect_.ToString().c_str(), params.PortraitPanelRect_.ToString().c_str());
        return WSError::WS_OK;
    }, __func__);
}

void SceneSession::SetSessionPiPControlStatusChangeCallback(const NotifySessionPiPControlStatusChangeFunc& func)
{
    PostTask([weakThis = wptr(this), func, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_PIP, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        session->sessionPiPControlStatusChangeFunc_ = func;
        return WSError::WS_OK;
    }, __func__);
}

void SceneSession::SetAutoStartPiPStatusChangeCallback(const NotifyAutoStartPiPStatusChangeFunc& func)
{
    PostTask([weakThis = wptr(this), func, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_PIP, "%{public}s session is null", where);
            return;
        }
        session->autoStartPiPStatusChangeFunc_ = func;
    }, __func__);
}

/** @note @window.layout */
void SceneSession::UpdateSessionRectInner(const WSRect& rect, SizeChangeReason reason,
    const MoveConfiguration& moveConfiguration, const RectAnimationConfig& rectAnimationConfig)
{
    auto newWinRect = winRect_;
    auto newRequestRect = GetSessionRequestRect();
    SizeChangeReason newReason = reason;
    if (reason == SizeChangeReason::MOVE || reason == SizeChangeReason::MOVE_WITH_ANIMATION) {
        newWinRect.posX_ = rect.posX_;
        newWinRect.posY_ = rect.posY_;
        newRequestRect.posX_ = rect.posX_;
        newRequestRect.posY_ = rect.posY_;
        if (!Session::IsScbCoreEnabled() && !WindowHelper::IsMainWindow(GetWindowType())) {
            SetSessionRect(newWinRect);
        }
        SetSessionRequestRect(newRequestRect);
        SetRequestRectAnimationConfig(moveConfiguration.rectAnimationConfig);
        NotifySessionRectChange(newRequestRect, reason, moveConfiguration.displayId,
            moveConfiguration.rectAnimationConfig);
    } else if (reason == SizeChangeReason::RESIZE || reason == SizeChangeReason::RESIZE_WITH_ANIMATION) {
        if (rect.width_ > 0 && rect.height_ > 0) {
            newWinRect.width_ = rect.width_;
            newWinRect.height_ = rect.height_;
            newRequestRect.width_ = rect.width_;
            newRequestRect.height_ = rect.height_;
        }
        if (!Session::IsScbCoreEnabled() && GetWindowType() != WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
            SetSessionRect(newWinRect);
        }
        SetRequestRectAnimationConfig(rectAnimationConfig);
        DisplayId displayId = GetSessionProperty() != nullptr ? GetSessionProperty()->GetDisplayId() :
            DISPLAY_ID_INVALID;
        TLOGI(WmsLogTag::WMS_LAYOUT, "Get displayId: %{public}" PRIu64, displayId);
        auto notifyRect = newRequestRect;
        if (PcFoldScreenManager::GetInstance().IsHalfFolded(GetScreenId())) {
            newReason = SizeChangeReason::UNDEFINED;
            notifyRect = rect;
        }
        SetSessionRequestRect(notifyRect);
        NotifySessionRectChange(notifyRect, newReason, displayId, rectAnimationConfig);
    } else {
        if (!Session::IsScbCoreEnabled()) {
            SetSessionRect(rect);
        }
        NotifySessionRectChange(rect, reason);
    }
    TLOGI(WmsLogTag::WMS_LAYOUT, "Id:%{public}d reason:%{public}d newReason:%{public}d moveConfiguration:%{public}s "
        "rect:%{public}s newRequestRect:%{public}s newWinRect:%{public}s", GetPersistentId(), reason, newReason,
        moveConfiguration.ToString().c_str(), rect.ToString().c_str(), newRequestRect.ToString().c_str(),
        newWinRect.ToString().c_str());
}

void SceneSession::UpdateSessionRectPosYFromClient(SizeChangeReason reason, DisplayId& configDisplayId, WSRect& rect)
{
    if (!PcFoldScreenManager::GetInstance().IsHalfFolded(GetScreenId())) {
        TLOGI(WmsLogTag::WMS_LAYOUT, "winId: %{public}d, displayId: %{public}" PRIu64,
            GetPersistentId(), GetScreenId());
        return;
    }
    TLOGI(WmsLogTag::WMS_LAYOUT, "winId: %{public}d, reason: %{public}u, lastRect: %{public}s, currRect: %{public}s",
        GetPersistentId(), reason, winRect_.ToString().c_str(), rect.ToString().c_str());
    if (reason != SizeChangeReason::RESIZE) {
        configDisplayId_ = configDisplayId;
    }
    if (configDisplayId_ != DISPLAY_ID_INVALID &&
        !PcFoldScreenManager::GetInstance().IsPcFoldScreen(configDisplayId_)) {
        TLOGI(WmsLogTag::WMS_LAYOUT, "winId: %{public}d, configDisplayId: %{public}" PRIu64,
            GetPersistentId(), configDisplayId_);
        return;
    }
    auto clientDisplayId = clientDisplayId_;
    TLOGI(WmsLogTag::WMS_LAYOUT, "winId: %{public}d, input: %{public}s, screenId: %{public}" PRIu64
        ", clientDisplayId: %{public}" PRIu64 ", configDisplayId: %{public}" PRIu64,
        GetPersistentId(), rect.ToString().c_str(), GetScreenId(), clientDisplayId, configDisplayId_);
    if (configDisplayId_ != VIRTUAL_DISPLAY_ID && clientDisplayId != VIRTUAL_DISPLAY_ID) {
        return;
    }
    const auto& [defaultDisplayRect, virtualDisplayRect, foldCreaseRect] =
        PcFoldScreenManager::GetInstance().GetDisplayRects();
    rect.posY_ += defaultDisplayRect.height_ + foldCreaseRect.height_;
    configDisplayId = DEFAULT_DISPLAY_ID;
    TLOGI(WmsLogTag::WMS_LAYOUT, "winId: %{public}d, output: %{public}s", GetPersistentId(), rect.ToString().c_str());
}

/** @note @window.layout */
WSError SceneSession::UpdateSessionRect(
    const WSRect& rect, SizeChangeReason reason, bool isGlobal, bool isFromMoveToGlobal,
    const MoveConfiguration& moveConfiguration, const RectAnimationConfig& rectAnimationConfig)
{
    bool isMoveOrResize = reason == SizeChangeReason::MOVE || reason == SizeChangeReason::RESIZE ||
        reason == SizeChangeReason::MOVE_WITH_ANIMATION || reason == SizeChangeReason::RESIZE_WITH_ANIMATION;
    if (isMoveOrResize && GetWindowType() == WindowType::WINDOW_TYPE_PIP) {
        return WSError::WS_DO_NOTHING;
    }
    WSRect newRect = rect;
    MoveConfiguration newMoveConfiguration = moveConfiguration;
    UpdateSessionRectPosYFromClient(reason, newMoveConfiguration.displayId, newRect);
    if (isGlobal && WindowHelper::IsSubWindow(Session::GetWindowType()) &&
        (systemConfig_.IsPhoneWindow() ||
         (systemConfig_.IsPadWindow() && !IsFreeMultiWindowMode()))) {
        auto parentSession = GetParentSession();
        if (parentSession) {
            auto parentRect = parentSession->GetSessionRect();
            if (!CheckIfRectElementIsTooLarge(parentRect)) {
                newRect.posX_ -= parentRect.posX_;
                newRect.posY_ -= parentRect.posY_;
            }
        }
    }
    if (isFromMoveToGlobal && WindowHelper::IsSubWindow(Session::GetWindowType()) &&
        (systemConfig_.IsPhoneWindow() ||
         (systemConfig_.IsPadWindow() && !IsFreeMultiWindowMode()))) {
        auto parentSession = GetParentSession();
        if (parentSession && parentSession->GetFloatingScale() != 0) {
            Rect parentGlobalRect;
            WMError errorCode = parentSession->GetGlobalScaledRect(parentGlobalRect);
            newRect.posX_ = (newRect.posX_ - parentGlobalRect.posX_) / parentSession->GetFloatingScale();
            newRect.posY_ = (newRect.posY_ - parentGlobalRect.posY_) / parentSession->GetFloatingScale();
        }
    }
    Session::RectCheckProcess();
    PostTask([weakThis = wptr(this), newRect, reason, newMoveConfiguration, rectAnimationConfig, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        session->UpdateSessionRectInner(newRect, reason, newMoveConfiguration, rectAnimationConfig);
        return WSError::WS_OK;
    }, __func__ + GetRectInfo(rect));
    return WSError::WS_OK;
}

void SceneSession::UpdateCrossAxisOfLayout(const WSRect& rect)
{
    const int FOLD_CREASE_TYPE = 2;
    isCrossAxisOfLayout_ = rect.IsOverlap(std::get<FOLD_CREASE_TYPE>(
        PcFoldScreenManager::GetInstance().GetDisplayRects()));
    UpdateCrossAxis();
}

void SceneSession::UpdateCrossAxis()
{
    CrossAxisState crossAxisState = CrossAxisState::STATE_INVALID;
    if (PcFoldScreenManager::GetInstance().GetDisplayId() == SCREEN_ID_INVALID ||
        PcFoldScreenManager::GetInstance().GetDisplayId() != GetSessionProperty()->GetDisplayId()) {
        return;
    }
    if (PcFoldScreenManager::GetInstance().GetScreenFoldStatus() != SuperFoldStatus::UNKNOWN) {
        if (PcFoldScreenManager::GetInstance().GetScreenFoldStatus() == SuperFoldStatus::HALF_FOLDED &&
            isCrossAxisOfLayout_) {
            crossAxisState = CrossAxisState::STATE_CROSS;
        } else {
            crossAxisState = CrossAxisState::STATE_NO_CROSS;
        }
    }
    if (crossAxisState_ != static_cast<uint32_t>(crossAxisState) && sessionStage_ != nullptr) {
        crossAxisState_ = static_cast<uint32_t>(crossAxisState);
        sessionStage_->NotifyWindowCrossAxisChange(crossAxisState);
    } else if (sessionStage_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "sessionStage_ is nullptr, id: %{public}d", GetPersistentId());
    }
}

WSError SceneSession::GetCrossAxisState(CrossAxisState& state)
{
    state = static_cast<CrossAxisState>(crossAxisState_.load());
    return WSError::WS_OK;
}

/** @note @window.layout */
WSError SceneSession::UpdateClientRect(const WSRect& rect)
{
    PostTask([weakThis = wptr(this), rect, funcName = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s: session is null", funcName);
            return;
        }
        if (rect.IsInvalid()) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s: id:%{public}d rect:%{public}s is invalid",
                funcName, session->GetPersistentId(), rect.ToString().c_str());
            return;
        }
        if (rect == session->GetClientRect()) {
            TLOGND(WmsLogTag::WMS_LAYOUT, "%{public}s: id:%{public}d skip same rect",
                funcName, session->GetPersistentId());
            return;
        }
        session->SetClientRect(rect);
    }, __func__ + GetRectInfo(rect));
    return WSError::WS_OK;
}

void SceneSession::NotifySingleHandTransformChange(const SingleHandTransform& singleHandTransform)
{
    if (!IsSessionForeground() && !IsVisible()) {
        TLOGD(WmsLogTag::WMS_LAYOUT, "id:%{public}d, session is not foreground and not visible!", GetPersistentId());
        return;
    }
    if (sessionStage_ != nullptr) {
        sessionStage_->NotifySingleHandTransformChange(singleHandTransform);
    }
}

void SceneSession::RegisterRaiseToTopCallback(NotifyRaiseToTopFunc&& callback)
{
    PostTask([weakThis = wptr(this), callback = std::move(callback), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return;
        }
        session->onRaiseToTop_ = std::move(callback);
    }, __func__);
}

void SceneSession::RegisterRaiseAboveTargetCallback(NotifyRaiseAboveTargetFunc&& callback)
{
    PostTask([weakThis = wptr(this), callback = std::move(callback), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return;
        }
        session->onRaiseAboveTarget_ = std::move(callback);
    }, __func__);
}

void SceneSession::RegisterSessionTopmostChangeCallback(NotifySessionTopmostChangeFunc&& callback)
{
    PostTask([weakThis = wptr(this), callback = std::move(callback), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return;
        }
        session->onSessionTopmostChange_ = std::move(callback);
        session->onSessionTopmostChange_(session->IsTopmost());
    }, __func__);
}

/** @note @window.hierarchy */
void SceneSession::RegisterSubSessionZLevelChangeCallback(NotifySubSessionZLevelChangeFunc&& callback)
{
    PostTask([weakThis = wptr(this), callback = std::move(callback), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return;
        }
        session->onSubSessionZLevelChange_ = std::move(callback);
        session->onSubSessionZLevelChange_(session->GetSubWindowZLevel());
    }, __func__);
}

/** @note @window.hierarchy */
WSError SceneSession::RaiseToAppTop()
{
    return PostSyncTask([weakThis = wptr(this), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_HIERARCHY, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (session->onRaiseToTop_) {
            TLOGNI(WmsLogTag::WMS_HIERARCHY, "%{public}s id: %{public}d", where, session->GetPersistentId());
            session->onRaiseToTop_();
            session->SetMainSessionUIStateDirty(true);
        }
        return WSError::WS_OK;
    }, __func__);
}

/** @note @window.hierarchy */
WSError SceneSession::RaiseAboveTarget(int32_t subWindowId)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("RaiseAboveTarget permission denied!");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    auto subSession = std::find_if(subSession_.begin(), subSession_.end(), [subWindowId](sptr<SceneSession> session) {
        bool res = (session != nullptr && session->GetWindowId() == subWindowId) ? true : false;
        return res;
    });
    int32_t callingPid = IPCSkeleton::GetCallingPid();
    if (subSession != subSession_.end() && callingPid != (*subSession)->GetCallingPid()) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "permission denied! id: %{public}d", subWindowId);
        return WSError::WS_ERROR_INVALID_CALLING;
    }
    return PostSyncTask([weakThis = wptr(this), subWindowId, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_HIERARCHY, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (session->onRaiseAboveTarget_) {
            session->onRaiseAboveTarget_(subWindowId);
        }
        return WSError::WS_OK;
    }, __func__);
}

WSError SceneSession::BindDialogSessionTarget(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "dialog session is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (onBindDialogTarget_) {
        TLOGI(WmsLogTag::WMS_DIALOG, "id: %{public}d", sceneSession->GetPersistentId());
        onBindDialogTarget_(sceneSession);
    }
    return WSError::WS_OK;
}

WSError SceneSession::SetSystemBarProperty(WindowType type, SystemBarProperty systemBarProperty)
{
    TLOGD(WmsLogTag::WMS_IMMS, "win %{public}u type %{public}u "
        "%{public}u %{public}x %{public}x %{public}u settingFlag %{public}u",
        GetPersistentId(), static_cast<uint32_t>(type),
        systemBarProperty.enable_, systemBarProperty.backgroundColor_, systemBarProperty.contentColor_,
        systemBarProperty.enableAnimation_, systemBarProperty.settingFlag_);
    auto property = GetSessionProperty();
    property->SetSystemBarProperty(type, systemBarProperty);
    if (type == WindowType::WINDOW_TYPE_STATUS_BAR && systemBarProperty.enable_) {
        SetIsDisplayStatusBarTemporarily(false);
    }
    if (onSystemBarPropertyChange_) {
        onSystemBarPropertyChange_(property->GetSystemBarProperty());
    }
    return WSError::WS_OK;
}

void SceneSession::SetIsStatusBarVisible(bool isVisible)
{
    PostTask([weakThis = wptr(this), isVisible, where = __func__] {
        sptr<SceneSession> sceneSession = weakThis.promote();
        if (sceneSession == nullptr) {
            TLOGNE(WmsLogTag::WMS_IMMS, "%{public}s session is null", where);
            return;
        }
        sceneSession->SetIsStatusBarVisibleInner(isVisible);
    }, __func__);
}

WSError SceneSession::SetIsStatusBarVisibleInner(bool isVisible)
{
    bool isNeedNotify = isStatusBarVisible_ != isVisible;
    TLOGI(WmsLogTag::WMS_IMMS, "win [%{public}d, %{public}s] visible %{public}u need notify %{public}u",
        GetPersistentId(), GetWindowName().c_str(), isVisible, isNeedNotify);
    isStatusBarVisible_ = isVisible;
    if (!isNeedNotify) {
        return WSError::WS_OK;
    }
    if (isLastFrameLayoutFinishedFunc_ == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "isLastFrameLayoutFinishedFunc is null, win %{public}d", GetPersistentId());
        return WSError::WS_ERROR_NULLPTR;
    }
    bool isLayoutFinished = false;
    WSError ret = isLastFrameLayoutFinishedFunc_(isLayoutFinished);
    if (ret != WSError::WS_OK) {
        TLOGE(WmsLogTag::WMS_IMMS, "isLastFrameLayoutFinishedFunc failed, ret %{public}d", ret);
        return ret;
    }
    if (isLayoutFinished) {
        UpdateAvoidArea(new AvoidArea(GetAvoidAreaByType(AvoidAreaType::TYPE_SYSTEM)), AvoidAreaType::TYPE_SYSTEM);
    } else {
        MarkAvoidAreaAsDirty();
    }
    return WSError::WS_OK;
}

void SceneSession::NotifyPropertyWhenConnect()
{
    WLOGFI("Notify property when connect.");
    auto property = GetSessionProperty();
    if (property == nullptr) {
        WLOGFD("id: %{public}d property is nullptr", persistentId_);
        return;
    }
    NotifySessionFocusableChange(property->GetFocusable());
    NotifySessionTouchableChange(property->GetTouchable());
    OnShowWhenLocked(GetShowWhenLockedFlagValue());
}

/** @note @window.hierarchy */
WSError SceneSession::RaiseAppMainWindowToTop()
{
    PostTask([weakThis = wptr(this), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (session->IsFocusedOnShow()) {
            FocusChangeReason reason = FocusChangeReason::MOVE_UP;
            session->NotifyRequestFocusStatusNotifyManager(true, true, reason);
            session->NotifyClick(true, false);
        } else {
            session->SetFocusedOnShow(true);
        }
        return WSError::WS_OK;
    }, __func__);
    return WSError::WS_OK;
}

WSError SceneSession::OnNeedAvoid(bool status)
{
    PostTask([weakThis = wptr(this), status, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_IMMS, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        TLOGNI(WmsLogTag::WMS_IMMS, "%{public}s win %{public}d status %{public}d",
            where, session->GetPersistentId(), static_cast<int32_t>(status));
        if (session->onNeedAvoid_) {
            session->onNeedAvoid_(status);
        }
        return WSError::WS_OK;
    }, __func__);
    return WSError::WS_OK;
}

WSError SceneSession::OnShowWhenLocked(bool showWhenLocked)
{
    WLOGFD("SceneSession ShowWhenLocked status:%{public}d", static_cast<int32_t>(showWhenLocked));
    if (onShowWhenLockedFunc_) {
        onShowWhenLockedFunc_(showWhenLocked);
    }
    return WSError::WS_OK;
}

bool SceneSession::IsShowWhenLocked() const
{
    return (GetSessionProperty()->GetWindowFlags() &
        static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED)) || IsTemporarilyShowWhenLocked();
}

bool SceneSession::GetShowWhenLockedFlagValue() const
{
    return GetSessionProperty()->GetWindowFlags() & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED);
}

void SceneSession::CalculateAvoidAreaRect(const WSRect& rect, const WSRect& avoidRect, AvoidArea& avoidArea) const
{
    if (SessionHelper::IsEmptyRect(rect) || SessionHelper::IsEmptyRect(avoidRect)) {
        return;
    }
    Rect avoidAreaRect = SessionHelper::TransferToRect(
        SessionHelper::GetOverlap(rect, avoidRect, rect.posX_, rect.posY_));
    if (WindowHelper::IsEmptyRect(avoidAreaRect)) {
        return;
    }

    uint32_t avoidAreaCenterX = static_cast<uint32_t>(avoidAreaRect.posX_) + (avoidAreaRect.width_ >> 1);
    uint32_t avoidAreaCenterY = static_cast<uint32_t>(avoidAreaRect.posY_) + (avoidAreaRect.height_ >> 1);
    float res1 = float(avoidAreaCenterY) - float(rect.height_) / float(rect.width_) *
        float(avoidAreaCenterX);
    float res2 = float(avoidAreaCenterY) + float(rect.height_) / float(rect.width_) *
        float(avoidAreaCenterX) - float(rect.height_);
    if (res1 < 0) {
        if (res2 < 0) {
            avoidArea.topRect_ = avoidAreaRect;
        } else {
            avoidArea.rightRect_ = avoidAreaRect;
        }
    } else {
        if (res2 < 0) {
            avoidArea.leftRect_ = avoidAreaRect;
        } else {
            avoidArea.bottomRect_ = avoidAreaRect;
        }
    }
}

void SceneSession::GetSystemAvoidArea(WSRect& rect, AvoidArea& avoidArea)
{
    auto sessionProperty = GetSessionProperty();
    bool isNeedAvoid = sessionProperty->GetWindowFlags() & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID);
    if (isNeedAvoid && WindowHelper::IsAppWindow(GetWindowType())) {
        TLOGI(WmsLogTag::WMS_IMMS, "win %{public}d type %{public}u flag %{public}d",
            GetPersistentId(), static_cast<uint32_t>(GetWindowType()), sessionProperty->GetWindowFlags());
        return;
    }
    if (sessionProperty->GetCompatibleModeInPc()) {
        HookAvoidAreaInCompatibleMode(rect, avoidArea, AvoidAreaType::TYPE_SYSTEM);
        return;
    }
    WindowMode windowMode = Session::GetWindowMode();
    bool isWindowFloatingOrSplit = windowMode == WindowMode::WINDOW_MODE_FLOATING ||
                                   windowMode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY ||
                                   windowMode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY;
    WindowType windowType = Session::GetWindowType();
    bool isAvailableSystemWindow = WindowHelper::IsSystemWindow(windowType) &&
        (GetSessionProperty()->GetAvoidAreaOption() & static_cast<uint32_t>(AvoidAreaOption::ENABLE_SYSTEM_WINDOW));
    bool isAvailableAppSubWindow = WindowHelper::IsSubWindow(windowType) &&
        (GetSessionProperty()->GetAvoidAreaOption() & static_cast<uint32_t>(AvoidAreaOption::ENABLE_APP_SUB_WINDOW));
    bool isAvailableWindowType = WindowHelper::IsMainWindow(windowType) || isAvailableSystemWindow ||
                                 isAvailableAppSubWindow;
    bool isAvailableDevice = (systemConfig_.IsPhoneWindow() || systemConfig_.IsPadWindow()) &&
                             !IsFreeMultiWindowMode();
    DisplayId displayId = sessionProperty->GetDisplayId();
    auto screenSession = ScreenSessionManagerClient::GetInstance().GetScreenSession(displayId);
    bool isAvailableScreen = !screenSession || (screenSession->GetName() != "HiCar");
    if (isWindowFloatingOrSplit && isAvailableWindowType && isAvailableDevice && isAvailableScreen) {
        // mini floating scene no need avoid
        if (LessOrEqual(Session::GetFloatingScale(), MINI_FLOAT_SCALE)) {
            return;
        }
        float vpr = 3.5f; // 3.5f: default pixel ratio
        auto display = DisplayManager::GetInstance().GetDefaultDisplay();
        if (display == nullptr) {
            WLOGFE("display is null");
            return;
        }
        vpr = display->GetVirtualPixelRatio();
        bool isFloat = windowMode == WindowMode::WINDOW_MODE_FLOATING && !GetIsMidScene();
        int32_t height = isFloat ? GetStatusBarHeight() : vpr * MULTI_WINDOW_TITLE_BAR_DEFAULT_HEIGHT_VP;
        avoidArea.topRect_.height_ = static_cast<uint32_t>(height);
        avoidArea.topRect_.width_ = static_cast<uint32_t>(display->GetWidth());
        return;
    }
    std::vector<sptr<SceneSession>> statusBarVector;
    if (specificCallback_ != nullptr && specificCallback_->onGetSceneSessionVectorByTypeAndDisplayId_) {
        statusBarVector = specificCallback_->onGetSceneSessionVectorByTypeAndDisplayId_(
            WindowType::WINDOW_TYPE_STATUS_BAR, sessionProperty->GetDisplayId());
    }
    for (auto& statusBar : statusBarVector) {
        if (statusBar == nullptr) {
            continue;
        }
        bool isVisible = statusBar->isVisible_;
        if (onGetStatusBarConstantlyShowFunc_) {
            onGetStatusBarConstantlyShowFunc_(displayId, isVisible);
            TLOGD(WmsLogTag::WMS_IMMS, "win %{public}d displayId %{public}" PRIu64 " constantly isVisible %{public}d",
                GetPersistentId(), displayId, isVisible);
        }
        bool isStatusBarVisible = WindowHelper::IsMainWindow(Session::GetWindowType()) ?
            isStatusBarVisible_ : isVisible;
        if (!isStatusBarVisible) {
            TLOGI(WmsLogTag::WMS_IMMS, "win %{public}d status bar not visible", GetPersistentId());
            continue;
        }
        WSRect statusBarRect = statusBar->GetSessionRect();
        if (onGetStatusBarAvoidHeightFunc_) {
            onGetStatusBarAvoidHeightFunc_(statusBarRect);
            TLOGD(WmsLogTag::WMS_IMMS, "win %{public}d status bar height %{public}d",
                GetPersistentId(), statusBarRect.height_);
        }
        TLOGI(WmsLogTag::WMS_IMMS, "win %{public}d rect %{public}s status bar %{public}s",
            GetPersistentId(), rect.ToString().c_str(), statusBarRect.ToString().c_str());
        CalculateAvoidAreaRect(rect, statusBarRect, avoidArea);
    }
    return;
}

void SceneSession::GetKeyboardAvoidArea(WSRect& rect, AvoidArea& avoidArea)
{
    if (!keyboardAvoidAreaActive_) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "id: %{public}d, isSystemKeyboard: %{public}d, state: %{public}d, "
            "gravity: %{public}d", GetPersistentId(), IsSystemKeyboard(), GetSessionState(), GetKeyboardGravity());
        return;
    }
    if (Session::CheckEmptyKeyboardAvoidAreaIfNeeded()) {
        TLOGD(WmsLogTag::WMS_IMMS, "Keyboard avoid area needs to be empty in floating mode");
        return;
    }
    std::vector<sptr<SceneSession>> inputMethodVector;
    if (specificCallback_ != nullptr && specificCallback_->onGetSceneSessionVectorByType_) {
        inputMethodVector = specificCallback_->onGetSceneSessionVectorByType_(
            WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    }
    for (auto& inputMethod : inputMethodVector) {
        if (inputMethod->GetSessionState() != SessionState::STATE_FOREGROUND &&
            inputMethod->GetSessionState() != SessionState::STATE_ACTIVE) {
            continue;
        }
        SessionGravity gravity = inputMethod->GetKeyboardGravity();
        if (gravity == SessionGravity::SESSION_GRAVITY_FLOAT || !inputMethod->IsKeyboardAvoidAreaActive()) {
            continue;
        }
        if (isKeyboardPanelEnabled_) {
            WSRect keyboardRect = {0, 0, 0, 0};
            if (inputMethod && inputMethod->GetKeyboardPanelSession()) {
                keyboardRect = inputMethod->GetKeyboardPanelSession()->GetSessionRect();
                inputMethod->RecalculatePanelRectForAvoidArea(keyboardRect);
            }
            TLOGI(WmsLogTag::WMS_IMMS, "win %{public}s keyboard %{public}s",
                  rect.ToString().c_str(), keyboardRect.ToString().c_str());
            CalculateAvoidAreaRect(rect, keyboardRect, avoidArea);
        } else {
            WSRect inputMethodRect = inputMethod->GetSessionRect();
            TLOGI(WmsLogTag::WMS_IMMS, "win %{public}s input method %{public}s",
                  rect.ToString().c_str(), inputMethodRect.ToString().c_str());
            CalculateAvoidAreaRect(rect, inputMethodRect, avoidArea);
        }
    }
    return;
}

void SceneSession::GetCutoutAvoidArea(WSRect& rect, AvoidArea& avoidArea)
{
    auto display = DisplayManager::GetInstance().GetDisplayById(GetSessionProperty()->GetDisplayId());
    if (display == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "Failed to get display");
        return;
    }
    sptr<CutoutInfo> cutoutInfo = display->GetCutoutInfo();
    if (cutoutInfo == nullptr) {
        TLOGI(WmsLogTag::WMS_IMMS, "There is no cutout info");
        return;
    }
    std::vector<DMRect> cutoutAreas = cutoutInfo->GetBoundingRects();
    if (cutoutAreas.empty()) {
        TLOGI(WmsLogTag::WMS_IMMS, "There is no cutout area");
        return;
    }
    for (auto& cutoutArea : cutoutAreas) {
        WSRect cutoutAreaRect = {
            cutoutArea.posX_,
            cutoutArea.posY_,
            cutoutArea.width_,
            cutoutArea.height_
        };
        TLOGI(WmsLogTag::WMS_IMMS, "win %{public}s cutout %{public}s",
              rect.ToString().c_str(), cutoutAreaRect.ToString().c_str());
        CalculateAvoidAreaRect(rect, cutoutAreaRect, avoidArea);
    }

    return;
}

void SceneSession::GetAINavigationBarArea(WSRect rect, AvoidArea& avoidArea) const
{
    if (isDisplayStatusBarTemporarily_.load()) {
        TLOGD(WmsLogTag::WMS_IMMS, "temporary show navigation bar, no need to avoid");
        return;
    }
    if (Session::GetWindowMode() == WindowMode::WINDOW_MODE_PIP) {
        TLOGD(WmsLogTag::WMS_IMMS, "window mode pip return");
        return;
    }
    // compatibleMode app need to hook avoidArea in pc
    if (GetSessionProperty()->GetCompatibleModeInPc()) {
        HookAvoidAreaInCompatibleMode(rect, avoidArea, AvoidAreaType::TYPE_NAVIGATION_INDICATOR);
        return;
    }
    WSRect barArea;
    if (specificCallback_ != nullptr && specificCallback_->onGetAINavigationBarArea_) {
        barArea = specificCallback_->onGetAINavigationBarArea_(GetSessionProperty()->GetDisplayId());
    }
    TLOGI(WmsLogTag::WMS_IMMS, "win %{public}s AI bar %{public}s",
          rect.ToString().c_str(), barArea.ToString().c_str());
    CalculateAvoidAreaRect(rect, barArea, avoidArea);
}

void SceneSession::HookAvoidAreaInCompatibleMode(WSRect& rect, AvoidArea& avoidArea, AvoidAreaType avoidAreaType) const
{
    WindowMode mode = GetWindowMode();
    if (!GetSessionProperty()->GetCompatibleModeInPc() || mode == WindowMode::WINDOW_MODE_FULLSCREEN) {
        TLOGE(WmsLogTag::WMS_PC, "only support compatibleMode app in pc");
        return;
    }

    float vpr = 3.5f; // 3.5f: default pixel ratio
    DMHookInfo hookInfo;
    ScreenSessionManagerClient::GetInstance().GetDisplayHookInfo(GetCallingUid(), hookInfo);
    if (hookInfo.density_) {
        vpr = hookInfo.density_;
    }
    switch (avoidAreaType) {
        case AvoidAreaType::TYPE_SYSTEM: {
            avoidArea.topRect_.posX_ = rect.posX_;
            avoidArea.topRect_.posY_ = rect.posY_;
            avoidArea.topRect_.height_ = HOOK_SYSTEM_BAR_HEIGHT * vpr;
            avoidArea.topRect_.width_ = rect.width_;
            return;
        }
        case AvoidAreaType::TYPE_NAVIGATION_INDICATOR: {
            avoidArea.bottomRect_.posX_ = rect.posX_;
            avoidArea.bottomRect_.posY_ = rect.posY_ + rect.height_ - HOOK_AI_BAR_HEIGHT * vpr;
            avoidArea.bottomRect_.width_ = rect.width_;
            avoidArea.bottomRect_.height_ = HOOK_AI_BAR_HEIGHT * vpr;
            return;
        }
        default: {
            TLOGE(WmsLogTag::WMS_IMMS, "cannot find win %{public}d type %{public}u", GetPersistentId(), avoidAreaType);
            return;
        }
    }
}

bool SceneSession::CheckGetSubWindowAvoidAreaAvailable(WindowMode winMode, AvoidAreaType type)
{
    if (GetSessionProperty()->GetAvoidAreaOption() & static_cast<uint32_t>(AvoidAreaOption::ENABLE_APP_SUB_WINDOW)) {
        return true;
    }
    if (winMode == WindowMode::WINDOW_MODE_FLOATING && IsFreeMultiWindowMode()) {
        TLOGD(WmsLogTag::WMS_IMMS, "win %{public}d type pad free multi window mode, return 0", GetPersistentId());
        return false;
    }
    auto parentSession = GetParentSession();
    if (!parentSession) {
        TLOGE(WmsLogTag::WMS_IMMS, "win %{public}d parent session is nullptr", GetPersistentId());
        return false;
    }
    if (parentSession->GetSessionRect() != GetSessionRect()) {
        TLOGE(WmsLogTag::WMS_IMMS, "rect mismatch: win %{public}d parent %{public}d",
            GetPersistentId(), parentSession->GetPersistentId());
    }
    return parentSession->CheckGetAvoidAreaAvailable(type);
}

bool SceneSession::CheckGetMainWindowAvoidAreaAvailable(WindowMode winMode, AvoidAreaType type)
{
    // compatibleMode app in pc,need use avoid Area
    if (GetSessionProperty()->GetCompatibleModeInPc()) {
        return true;
    }
    if (winMode == WindowMode::WINDOW_MODE_FLOATING && type != AvoidAreaType::TYPE_SYSTEM) {
        return false;
    }
    if (winMode == WindowMode::WINDOW_MODE_FLOATING && IsFreeMultiWindowMode()) {
        TLOGD(WmsLogTag::WMS_IMMS, "win %{public}d type pad free multi window mode, return 0", GetPersistentId());
        return false;
    }
    if (winMode != WindowMode::WINDOW_MODE_FLOATING || systemConfig_.IsPhoneWindow() || systemConfig_.IsPadWindow()) {
        return true;
    }
    return false;
}

bool SceneSession::CheckGetSystemWindowAvoidAreaAvailable()
{
    if (GetSessionProperty()->GetAvoidAreaOption() &
        static_cast<uint32_t>(AvoidAreaOption::ENABLE_SYSTEM_WINDOW)) {
        return systemConfig_.IsPhoneWindow() || systemConfig_.IsPadWindow();
    }
    return false;
}

bool SceneSession::CheckGetAvoidAreaAvailable(AvoidAreaType type)
{
    if (type == AvoidAreaType::TYPE_KEYBOARD) {
        TLOGD(WmsLogTag::WMS_IMMS, "win %{public}d type 3, return 1", GetPersistentId());
        return true;
    }
    WindowMode winMode = GetWindowMode();
    WindowType winType = GetWindowType();
    bool isAvailable = false;
    if (WindowHelper::IsSubWindow(winType)) {
        isAvailable = CheckGetSubWindowAvoidAreaAvailable(winMode, type);
    } else if (WindowHelper::IsMainWindow(winType)) {
        isAvailable = CheckGetMainWindowAvoidAreaAvailable(winMode, type);
    } else if (WindowHelper::IsSystemWindow(winType)) {
        isAvailable = CheckGetSystemWindowAvoidAreaAvailable();
    }
    TLOGD(WmsLogTag::WMS_IMMS, "win %{public}d type %{public}u avoidAreaType %{public}u "
        "windowMode %{public}u avoidAreaOption %{public}u, return %{public}d",
        GetPersistentId(), static_cast<uint32_t>(winType), static_cast<uint32_t>(type),
        static_cast<uint32_t>(winMode), GetSessionProperty()->GetAvoidAreaOption(), isAvailable);
    return isAvailable;
}

void SceneSession::AddNormalModalUIExtension(const ExtensionWindowEventInfo& extensionInfo)
{
    TLOGD(WmsLogTag::WMS_UIEXT, "parentId=%{public}d, persistentId=%{public}d, pid=%{public}d", GetPersistentId(),
        extensionInfo.persistentId, extensionInfo.pid);
    {
        std::unique_lock<std::shared_mutex> lock(modalUIExtensionInfoListMutex_);
        modalUIExtensionInfoList_.push_back(extensionInfo);
    }
    NotifySessionInfoChange();
}

void SceneSession::UpdateNormalModalUIExtension(const ExtensionWindowEventInfo& extensionInfo)
{
    TLOGD(WmsLogTag::WMS_UIEXT, "persistentId=%{public}d,pid=%{public}d,"
        "Rect:[%{public}d %{public}d %{public}d %{public}d]",
        extensionInfo.persistentId, extensionInfo.pid, extensionInfo.windowRect.posX_,
        extensionInfo.windowRect.posY_, extensionInfo.windowRect.width_, extensionInfo.windowRect.height_);
    {
        std::unique_lock<std::shared_mutex> lock(modalUIExtensionInfoListMutex_);
        auto iter = std::find_if(modalUIExtensionInfoList_.begin(), modalUIExtensionInfoList_.end(),
            [extensionInfo](const ExtensionWindowEventInfo& eventInfo) {
            return extensionInfo.persistentId == eventInfo.persistentId && extensionInfo.pid == eventInfo.pid;
        });
        if (iter == modalUIExtensionInfoList_.end()) {
            return;
        }
        iter->windowRect = extensionInfo.windowRect;
        iter->uiExtRect = extensionInfo.uiExtRect;
        iter->hasUpdatedRect = extensionInfo.hasUpdatedRect;
    }
    NotifySessionInfoChange();
}

void SceneSession::RemoveNormalModalUIExtension(int32_t persistentId)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "parentId=%{public}d, persistentId=%{public}d", GetPersistentId(), persistentId);
    {
        std::unique_lock<std::shared_mutex> lock(modalUIExtensionInfoListMutex_);
        auto iter = std::find_if(modalUIExtensionInfoList_.begin(), modalUIExtensionInfoList_.end(),
            [persistentId](const ExtensionWindowEventInfo& extensionInfo) {
            return extensionInfo.persistentId == persistentId;
        });
        if (iter == modalUIExtensionInfoList_.end()) {
            return;
        }
        modalUIExtensionInfoList_.erase(iter);
    }
    NotifySessionInfoChange();
}

void SceneSession::RegisterGetConstrainedModalExtWindowInfo(GetConstrainedModalExtWindowInfoFunc&& callback)
{
    onGetConstrainedModalExtWindowInfoFunc_ = std::move(callback);
}

std::optional<ExtensionWindowEventInfo> SceneSession::GetLastModalUIExtensionEventInfo()
{
    // Priority query constrained modal UIExt, if unavailable, then query normal modal UIExt
    if (onGetConstrainedModalExtWindowInfoFunc_) {
        if (auto constrainedExtEventInfo = onGetConstrainedModalExtWindowInfoFunc_(this)) {
            TLOGD(WmsLogTag::WMS_UIEXT, "get constrained UIExt eventInfo, id: %{public}d",
                constrainedExtEventInfo->persistentId);
            return constrainedExtEventInfo;
        }
    }
    std::shared_lock<std::shared_mutex> lock(modalUIExtensionInfoListMutex_);
    return modalUIExtensionInfoList_.empty() ? std::nullopt :
        std::make_optional<ExtensionWindowEventInfo>(modalUIExtensionInfoList_.back());
}

Vector2f SceneSession::GetSessionGlobalPosition(bool useUIExtension)
{
    WSRect windowRect = GetSessionGlobalRect();
    if (useUIExtension) {
        if (auto modalUIExtensionEventInfo = GetLastModalUIExtensionEventInfo()) {
            const auto& rect = modalUIExtensionEventInfo.value().windowRect;
            windowRect.posX_ = rect.posX_;
            windowRect.posY_ = rect.posY_;
        }
    }
    Vector2f position(windowRect.posX_, windowRect.posY_);
    return position;
}

WSRect SceneSession::GetSessionGlobalRectWithSingleHandScale()
{
    WSRect rectWithTransform = GetSessionGlobalRect();
    const SingleHandTransform& transform = GetSingleHandTransform();
    if (transform.posX == 0 && transform.posY == 0) {
        return rectWithTransform;
    }
    rectWithTransform.posX_ =
        static_cast<int32_t>(static_cast<float>(rectWithTransform.posX_) * transform.scaleX) + transform.posX;
    rectWithTransform.posY_ =
        static_cast<int32_t>(static_cast<float>(rectWithTransform.posY_) * transform.scaleY) + transform.posY;
    rectWithTransform.width_ =
        static_cast<int32_t>(static_cast<float>(rectWithTransform.width_) * transform.scaleX);
    rectWithTransform.height_ =
        static_cast<int32_t>(static_cast<float>(rectWithTransform.height_) * transform.scaleY);
    return rectWithTransform;
}

void SceneSession::AddUIExtSurfaceNodeId(uint64_t surfaceNodeId, int32_t persistentId)
{
    std::unique_lock<std::shared_mutex> lock(uiExtNodeIdToPersistentIdMapMutex_);
    TLOGI(WmsLogTag::WMS_UIEXT, "Add uiExtension pair surfaceNodeId=%{public}" PRIu64 ", persistentId=%{public}d",
        surfaceNodeId, persistentId);
    uiExtNodeIdToPersistentIdMap_.insert(std::make_pair(surfaceNodeId, persistentId));
}

void SceneSession::RemoveUIExtSurfaceNodeId(int32_t persistentId)
{
    std::unique_lock<std::shared_mutex> lock(uiExtNodeIdToPersistentIdMapMutex_);
    TLOGI(WmsLogTag::WMS_UIEXT, "Remove uiExtension by persistentId=%{public}d", persistentId);
    auto pairIter = std::find_if(uiExtNodeIdToPersistentIdMap_.begin(), uiExtNodeIdToPersistentIdMap_.end(),
        [persistentId](const auto& entry) { return entry.second == persistentId; });
    if (pairIter != uiExtNodeIdToPersistentIdMap_.end()) {
        TLOGI(WmsLogTag::WMS_UIEXT,
            "Successfully removed uiExtension pair surfaceNodeId=%{public}" PRIu64 ", persistentId=%{public}d",
            pairIter->first, persistentId);
        uiExtNodeIdToPersistentIdMap_.erase(pairIter);
        return;
    }
    TLOGE(WmsLogTag::WMS_UIEXT, "Failed to remove uiExtension by persistentId=%{public}d", persistentId);
}

int32_t SceneSession::GetUIExtPersistentIdBySurfaceNodeId(uint64_t surfaceNodeId) const
{
    std::shared_lock<std::shared_mutex> lock(uiExtNodeIdToPersistentIdMapMutex_);
    auto ret = uiExtNodeIdToPersistentIdMap_.find(surfaceNodeId);
    if (ret == uiExtNodeIdToPersistentIdMap_.end()) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Failed to find uiExtension by surfaceNodeId=%{public}" PRIu64 "", surfaceNodeId);
        return 0;
    }
    return ret->second;
}

AvoidArea SceneSession::GetAvoidAreaByTypeInner(AvoidAreaType type, const WSRect& rect)
{
    if (!CheckGetAvoidAreaAvailable(type)) {
        return {};
    }

    AvoidArea avoidArea;
    WSRect sessionRect = rect.IsEmpty() ? GetSessionRect() : rect;
    switch (type) {
        case AvoidAreaType::TYPE_SYSTEM: {
            GetSystemAvoidArea(sessionRect, avoidArea);
            return avoidArea;
        }
        case AvoidAreaType::TYPE_CUTOUT: {
            GetCutoutAvoidArea(sessionRect, avoidArea);
            return avoidArea;
        }
        case AvoidAreaType::TYPE_SYSTEM_GESTURE: {
            return avoidArea;
        }
        case AvoidAreaType::TYPE_KEYBOARD: {
            GetKeyboardAvoidArea(sessionRect, avoidArea);
            return avoidArea;
        }
        case AvoidAreaType::TYPE_NAVIGATION_INDICATOR: {
            GetAINavigationBarArea(sessionRect, avoidArea);
            return avoidArea;
        }
        default: {
            TLOGE(WmsLogTag::WMS_IMMS, "cannot find win %{public}d type %{public}u",
                GetPersistentId(), type);
            return avoidArea;
        }
    }
}

AvoidArea SceneSession::GetAvoidAreaByType(AvoidAreaType type, const WSRect& rect, int32_t apiVersion)
{
    return PostSyncTask([weakThis = wptr(this), type, rect, where = __func__]() -> AvoidArea {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_IMMS, "%{public}s session is null", where);
            return {};
        }
        return session->GetAvoidAreaByTypeInner(type, rect);
    }, __func__);
}

WSError SceneSession::GetAllAvoidAreas(std::map<AvoidAreaType, AvoidArea>& avoidAreas)
{
    return PostSyncTask([weakThis = wptr(this), &avoidAreas, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_IMMS, "%{public}s session is null", where);
            return WSError::WS_ERROR_NULLPTR;
        }

        using T = std::underlying_type_t<AvoidAreaType>;
        for (T avoidType = static_cast<T>(AvoidAreaType::TYPE_START);
            avoidType < static_cast<T>(AvoidAreaType::TYPE_END); avoidType++) {
            auto type = static_cast<AvoidAreaType>(avoidType);
            auto area = session->GetAvoidAreaByTypeInner(type);
            // code below aims to check if ai bar avoid area reaches window rect's bottom
            // it should not be removed until unexpected window rect update issues were solved
            if (type == AvoidAreaType::TYPE_NAVIGATION_INDICATOR) {
                if (session->isAINavigationBarAvoidAreaValid_ &&
                    !session->isAINavigationBarAvoidAreaValid_(area, session->GetSessionRect().height_)) {
                    continue;
                }
            }
            avoidAreas[type] = area;
        }
        return WSError::WS_OK;
    }, __func__);
}

WSError SceneSession::UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type)
{
    if (!sessionStage_) {
        return WSError::WS_ERROR_NULLPTR;
    }
    if (!GetForegroundInteractiveStatus()) {
        TLOGD(WmsLogTag::WMS_IMMS, "win [%{public}d] avoid area update rejected by recent", GetPersistentId());
        return WSError::WS_DO_NOTHING;
    }
    return sessionStage_->UpdateAvoidArea(avoidArea, type);
}

WSError SceneSession::SetPipActionEvent(const std::string& action, int32_t status)
{
    TLOGI(WmsLogTag::WMS_PIP, "action: %{public}s, status: %{public}d", action.c_str(), status);
    if (!sessionStage_) {
        return WSError::WS_ERROR_NULLPTR;
    }
    return sessionStage_->SetPipActionEvent(action, status);
}

WSError SceneSession::SetPiPControlEvent(WsPiPControlType controlType, WsPiPControlStatus status)
{
    TLOGI(WmsLogTag::WMS_PIP, "controlType: %{public}u, status: %{public}u", controlType, status);
    if (GetWindowType() != WindowType::WINDOW_TYPE_PIP || GetWindowMode() != WindowMode::WINDOW_MODE_PIP) {
        return WSError::WS_ERROR_INVALID_TYPE;
    }
    if (!sessionStage_) {
        return WSError::WS_ERROR_NULLPTR;
    }
    return sessionStage_->SetPiPControlEvent(controlType, status);
}

WSError SceneSession::NotifyPipWindowSizeChange(double width, double height, double scale)
{
    TLOGI(WmsLogTag::WMS_PIP, "width: %{public}f, height: %{public}f scale: %{public}f", width, height, scale);
    if (!sessionStage_) {
        return WSError::WS_ERROR_NULLPTR;
    }
    return sessionStage_->NotifyPipWindowSizeChange(width, height, scale);
}

void SceneSession::RegisterProcessPrepareClosePiPCallback(NotifyPrepareClosePiPSessionFunc&& callback)
{
    PostTask([weakThis = wptr(this), callback = std::move(callback), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_PIP, "%{public}s session is null", where);
            return;
        }
        session->onPrepareClosePiPSession_ = std::move(callback);
    }, __func__);
}

WSError SceneSession::ProcessPointDownSession(int32_t posX, int32_t posY)
{
    const auto& id = GetPersistentId();
    TLOGD(WmsLogTag::WMS_INPUT_KEY_FLOW, "id:%{public}d, type:%{public}d", id, GetWindowType());

    // notify touch outside
    if (specificCallback_ != nullptr && specificCallback_->onSessionTouchOutside_ &&
        sessionInfo_.bundleName_.find("SCBGestureBack") == std::string::npos) {
        specificCallback_->onSessionTouchOutside_(id);
    }

    // notify outside down event
    if (specificCallback_ != nullptr && specificCallback_->onOutsideDownEvent_) {
        specificCallback_->onOutsideDownEvent_(posX, posY);
    }
    return WSError::WS_OK;
}

WSError SceneSession::SendPointEventForMoveDrag(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
    bool isExecuteDelayRaise)
{
    NotifyOutsideDownEvent(pointerEvent);
    TransferPointerEvent(pointerEvent, false, isExecuteDelayRaise);
    return WSError::WS_OK;
}

void SceneSession::NotifyOutsideDownEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    // notify touchOutside and touchDown event
    int32_t action = pointerEvent->GetPointerAction();
    if (action != MMI::PointerEvent::POINTER_ACTION_DOWN &&
        action != MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN) {
        return;
    }

    MMI::PointerEvent::PointerItem pointerItem;
    if (!pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem)) {
        return;
    }

    // notify touch outside
    if (specificCallback_ != nullptr && specificCallback_->onSessionTouchOutside_ &&
        sessionInfo_.bundleName_.find("SCBGestureBack") == std::string::npos) {
        specificCallback_->onSessionTouchOutside_(GetPersistentId());
    }

    // notify outside down event
    if (specificCallback_ != nullptr && specificCallback_->onOutsideDownEvent_) {
        specificCallback_->onOutsideDownEvent_(pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
    }
}

WSError SceneSession::TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
    bool needNotifyClient, bool isExecuteDelayRaise)
{
    PostTask([weakThis = wptr(this), pointerEvent, needNotifyClient, isExecuteDelayRaise, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::DEFAULT, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        return session->TransferPointerEventInner(pointerEvent, needNotifyClient, isExecuteDelayRaise);
    }, __func__);
    return WSError::WS_OK;
}

WSError SceneSession::TransferPointerEventInner(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
    bool needNotifyClient, bool isExecuteDelayRaise)
{
    WLOGFD("[WMSCom] TransferPointEvent, id: %{public}d, type: %{public}d, needNotifyClient: %{public}d",
        GetPersistentId(), GetWindowType(), needNotifyClient);
    if (pointerEvent == nullptr) {
        WLOGFE("pointerEvent is null");
        return WSError::WS_ERROR_NULLPTR;
    }

    int32_t action = pointerEvent->GetPointerAction();

    if (!CheckPointerEventDispatch(pointerEvent)) {
        WLOGFI("Do not dispatch this pointer event");
        return WSError::WS_DO_NOTHING;
    }

    bool isPointDown = (action == MMI::PointerEvent::POINTER_ACTION_DOWN ||
        action == MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);

    auto property = GetSessionProperty();
    if (property == nullptr) {
        return Session::TransferPointerEvent(pointerEvent, needNotifyClient, isExecuteDelayRaise);
    }
    auto windowType = property->GetWindowType();
    bool isMovableWindowType = IsMovableWindowType();
    bool isMainWindow = WindowHelper::IsMainWindow(windowType);
    bool isSubWindow = WindowHelper::IsSubWindow(windowType);
    bool isDialog = WindowHelper::IsDialogWindow(windowType);
    bool isMaxModeAvoidSysBar = property->GetMaximizeMode() == MaximizeMode::MODE_AVOID_SYSTEM_BAR;
    bool isDragAccessibleSystemWindow = WindowHelper::IsSystemWindow(windowType) && IsDragAccessible() &&
        !isDialog;
    bool isMovableSystemWindow = WindowHelper::IsSystemWindow(windowType) && !isDialog;
    TLOGD(WmsLogTag::WMS_EVENT, "%{public}s: %{public}d && %{public}d", property->GetWindowName().c_str(),
        WindowHelper::IsSystemWindow(windowType), IsDragAccessible());
    if (isMovableWindowType && !isMaxModeAvoidSysBar &&
        (isMainWindow || isSubWindow || isDialog || isDragAccessibleSystemWindow || isMovableSystemWindow)) {
        if (CheckDialogOnForeground() && isPointDown) {
            HandlePointDownDialog();
            pointerEvent->MarkProcessed();
            TLOGI(WmsLogTag::WMS_DIALOG, "There is dialog window foreground");
            return WSError::WS_OK;
        }
        if (!moveDragController_) {
            TLOGD(WmsLogTag::WMS_LAYOUT, "moveDragController_ is null");
            return Session::TransferPointerEvent(pointerEvent, needNotifyClient, isExecuteDelayRaise);
        }
        if (isFollowParentLayout_) {
            return WSError::WS_OK;
        }
        if ((property->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING && IsDragAccessible()) ||
            isDragAccessibleSystemWindow) {
            if ((systemConfig_.IsPcWindow() || IsFreeMultiWindowMode() ||
                 (property->GetIsPcAppInPad() && !isMainWindow)) &&
                moveDragController_->ConsumeDragEvent(pointerEvent, winRect_, property, systemConfig_)) {
                auto surfaceNode = GetSurfaceNode();
                moveDragController_->UpdateGravityWhenDrag(pointerEvent, surfaceNode);
                PresentFoucusIfNeed(pointerEvent->GetPointerAction());
                pointerEvent->MarkProcessed();
                return WSError::WS_OK;
            }
        }
        if ((WindowHelper::IsMainWindow(windowType) ||
             WindowHelper::IsSubWindow(windowType) ||
             WindowHelper::IsSystemWindow(windowType)) &&
            moveDragController_->ConsumeMoveEvent(pointerEvent, winRect_)) {
            PresentFoucusIfNeed(pointerEvent->GetPointerAction());
            pointerEvent->MarkProcessed();
            Session::TransferPointerEvent(pointerEvent, needNotifyClient, isExecuteDelayRaise);
            ProcessWindowMoving(pointerEvent);
            return WSError::WS_OK;
        }
    }

    bool raiseEnabled = property->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG && property->GetRaiseEnabled() &&
        (action == MMI::PointerEvent::POINTER_ACTION_DOWN || action == MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    if (raiseEnabled) {
        RaiseToAppTopForPointDown();
    }
    // modify the window coordinates when move end
    MMI::PointerEvent::PointerItem pointerItem;
    if ((action == MMI::PointerEvent::POINTER_ACTION_BUTTON_UP || action == MMI::PointerEvent::POINTER_ACTION_MOVE) &&
        needNotifyClient && pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem) &&
        IsStartMoving()) {
        int32_t windowX = pointerItem.GetDisplayX() - winRect_.posX_;
        int32_t windowY = pointerItem.GetDisplayY() - winRect_.posY_;
        TLOGD(WmsLogTag::WMS_EVENT, "move end position: windowX:%{private}d windowY:%{private}d action:%{public}d",
            windowX, windowY, action);
        pointerItem.SetWindowX(windowX);
        pointerItem.SetWindowY(windowY);
        pointerEvent->AddPointerItem(pointerItem);
    }
    return Session::TransferPointerEvent(pointerEvent, needNotifyClient, isExecuteDelayRaise);
}

void SceneSession::NotifyUpdateGravity()
{
    std::unordered_map<int32_t, NotifySurfaceBoundsChangeFunc> funcMap;
    {
        std::lock_guard lock(registerNotifySurfaceBoundsChangeMutex_);
        funcMap = notifySurfaceBoundsChangeFuncMap_;
    }
    for (const auto& [sessionId, _] : funcMap) {
        auto subSession = GetSceneSessionById(sessionId);
        if (!subSession || !subSession->GetIsFollowParentLayout()) {
            return;
        }
        auto surfaceNode = subSession->GetSurfaceNode();
        auto subController = subSession->GetMoveDragController();
        if (subController && surfaceNode) {
            subController->UpdateSubWindowGravityWhenFollow(moveDragController_, surfaceNode);
        }
    }
}

void SceneSession::ProcessWindowMoving(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    if (pointerEvent->GetPointerAction() != MMI::PointerEvent::POINTER_ACTION_MOVE) {
        return;
    }
    MMI::PointerEvent::PointerItem pointerItem;
    if (!pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem)) {
        return;
    }
    if (notifyWindowMovingFunc_) {
        notifyWindowMovingFunc_(static_cast<DisplayId>(pointerEvent->GetTargetDisplayId()),
            pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
    }
}

void SceneSession::SetWindowMovingCallback(NotifyWindowMovingFunc&& func)
{
    PostTask([weakThis = wptr(this), where = __func__, func = std::move(func)] {
        auto session = weakThis.promote();
        if (!session || !func) {
            TLOGNE(WmsLogTag::WMS_MAIN, "%{public}s: session or func is null", where);
            return;
        }
        session->notifyWindowMovingFunc_ = std::move(func);
        TLOGNI(WmsLogTag::WMS_MAIN, "%{public}s id: %{public}d", where,
            session->GetPersistentId());
    }, __func__);
}

bool SceneSession::IsMovableWindowType()
{
    auto property = GetSessionProperty();
    if (property == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "property is null");
        return false;
    }

    return property->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING ||
        property->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_PRIMARY ||
        property->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_SECONDARY ||
        IsFullScreenMovable();
}

bool SceneSession::IsFullScreenMovable()
{
    auto property = GetSessionProperty();
    if (property == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "property is null");
        return false;
    }
    return property->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN &&
        WindowHelper::IsWindowModeSupported(property->GetWindowModeSupportType(), WindowMode::WINDOW_MODE_FLOATING);
}

bool SceneSession::IsMovable()
{
    if (!moveDragController_) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "moveDragController_ is null, id: %{public}d", GetPersistentId());
        return false;
    }

    bool windowIsMovable = !moveDragController_->GetStartDragFlag() && IsMovableWindowType() &&
                           moveDragController_->HasPointDown() && moveDragController_->GetMovable();
    auto property = GetSessionProperty();
    if (property->GetWindowType() != WindowType::WINDOW_TYPE_INPUT_METHOD_STATUS_BAR &&
        property->GetWindowType() != WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        windowIsMovable = windowIsMovable && IsFocused();
    }
    if (!windowIsMovable) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "Window is not movable, id: %{public}d, startDragFlag: %{public}d, "
            "isFocused: %{public}d, movableWindowType: %{public}d, hasPointDown: %{public}d, movable: %{public}d",
            GetPersistentId(), moveDragController_->GetStartDragFlag(), IsFocused(), IsMovableWindowType(),
            moveDragController_->HasPointDown(), moveDragController_->GetMovable());
        return false;
    }
    return true;
}

WSError SceneSession::RequestSessionBack(bool needMoveToBackground)
{
    PostTask([weakThis = wptr(this), needMoveToBackground, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (!session->backPressedFunc_) {
            TLOGNW(WmsLogTag::WMS_EVENT, "%{public}s Session didn't register back event consumer!", where);
            return WSError::WS_DO_NOTHING;
        }
        if (g_enableForceUIFirst) {
            auto rsTransaction = RSTransactionProxy::GetInstance();
            if (rsTransaction) {
                rsTransaction->Begin();
            }
            auto leashWinSurfaceNode = session->GetLeashWinSurfaceNode();
            if (leashWinSurfaceNode) {
                leashWinSurfaceNode->SetForceUIFirst(true);
                TLOGNI(WmsLogTag::WMS_EVENT, "%{public}s leashWinSurfaceNode_ SetForceUIFirst id:%{public}u!",
                    where, session->GetPersistentId());
            } else {
                TLOGNI(WmsLogTag::WMS_EVENT, "%{public}s failed, leashWinSurfaceNode_ null id:%{public}u",
                    where, session->GetPersistentId());
            }
            if (rsTransaction) {
                rsTransaction->Commit();
            }
        }
        session->backPressedFunc_(needMoveToBackground);
        return WSError::WS_OK;
    }, std::string(__func__) + ":" + std::to_string(needMoveToBackground));
    return WSError::WS_OK;
}

#ifdef DEVICE_STATUS_ENABLE
void SceneSession::RotateDragWindow(std::shared_ptr<RSTransaction> rsTransaction)
{
    Msdp::DeviceStatus::DragState state = Msdp::DeviceStatus::DragState::STOP;
    Msdp::DeviceStatus::InteractionManager::GetInstance()->GetDragState(state);
    if (state == Msdp::DeviceStatus::DragState::START) {
        Msdp::DeviceStatus::InteractionManager::GetInstance()->RotateDragWindowSync(rsTransaction);
    }
}
#endif // DEVICE_STATUS_ENABLE

/** @note @window.layout */
void SceneSession::NotifySessionRectChange(const WSRect& rect,
    SizeChangeReason reason, DisplayId displayId, const RectAnimationConfig& rectAnimationConfig)
{
    PostTask([weakThis = wptr(this), rect, reason, displayId, rectAnimationConfig, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s session is null", where);
            return;
        }
        if (session->sessionRectChangeFunc_) {
            HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSession::NotifySessionRectChange");
            session->sessionRectChangeFunc_(rect, reason, displayId, rectAnimationConfig);
        }
    }, __func__ + GetRectInfo(rect));
}

/** @note @window.layout */
void SceneSession::NotifySessionDisplayIdChange(uint64_t displayId)
{
    PostTask([weakThis = wptr(this), displayId] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "session is null");
            return;
        }
        if (session->sessionDisplayIdChangeFunc_) {
            session->sessionDisplayIdChangeFunc_(displayId);
        }
    }, __func__);
}

void SceneSession::CheckSubSessionShouldFollowParent(uint64_t displayId)
{
    for (const auto& session : subSession_) {
        if (session && session->IsSessionForeground() && session->GetSessionProperty()->GetDisplayId() != displayId) {
            session->SetShouldFollowParentWhenShow(false);
        }
    }
}

bool SceneSession::IsDecorEnable() const
{
    auto property = GetSessionProperty();
    if (property == nullptr) {
        WLOGE("property is nullptr");
        return false;
    }
    auto windowType = property->GetWindowType();
    bool isMainWindow = WindowHelper::IsMainWindow(windowType);
    bool isSubWindow = WindowHelper::IsSubWindow(windowType);
    bool isDialogWindow = WindowHelper::IsDialogWindow(windowType);
    bool isValidWindow = isMainWindow ||
        ((isSubWindow || isDialogWindow) && property->IsDecorEnable());
    bool isWindowModeSupported = WindowHelper::IsWindowModeSupported(
        systemConfig_.decorWindowModeSupportType_, property->GetWindowMode());
    bool enable = isValidWindow && systemConfig_.isSystemDecorEnable_ && isWindowModeSupported;
    return enable;
}

std::string SceneSession::GetRatioPreferenceKey()
{
    std::string key = sessionInfo_.bundleName_ + sessionInfo_.moduleName_ + sessionInfo_.abilityName_;
    if (key.length() > ScenePersistentStorage::MAX_KEY_LEN) {
        return key.substr(key.length() - ScenePersistentStorage::MAX_KEY_LEN);
    }
    return key;
}

bool SceneSession::SaveAspectRatio(float ratio)
{
    std::string key = GetRatioPreferenceKey();
    if (!key.empty()) {
        ScenePersistentStorage::Insert(key, ratio, ScenePersistentStorageType::ASPECT_RATIO);
        WLOGD("SceneSession save aspectRatio , key %{public}s, value: %{public}f", key.c_str(), aspectRatio_);
        return true;
    }
    return false;
}

void SceneSession::AdjustRectByLimits(WindowLimits limits, float ratio, bool isDecor, float vpr, WSRect& rect)
{
    if (isDecor) {
        rect.width_ = SessionUtils::ToLayoutWidth(rect.width_, vpr);
        rect.height_ = SessionUtils::ToLayoutHeight(rect.height_, vpr);
        limits.minWidth_ = SessionUtils::ToLayoutWidth(limits.minWidth_, vpr);
        limits.maxWidth_ = SessionUtils::ToLayoutWidth(limits.maxWidth_, vpr);
        limits.minHeight_ = SessionUtils::ToLayoutHeight(limits.minHeight_, vpr);
        limits.maxHeight_ = SessionUtils::ToLayoutHeight(limits.maxHeight_, vpr);
    }
    if (static_cast<uint32_t>(rect.height_) > limits.maxHeight_) {
        rect.height_ = static_cast<int32_t>(limits.maxHeight_);
        rect.width_ = floor(rect.height_ * ratio);
    } else if (static_cast<uint32_t>(rect.width_) > limits.maxWidth_) {
        rect.width_ = static_cast<int32_t>(limits.maxWidth_);
        rect.height_ = floor(rect.width_ / ratio);
    } else if (static_cast<uint32_t>(rect.width_) < limits.minWidth_) {
        rect.width_ = static_cast<int32_t>(limits.minWidth_);
        rect.height_ = ceil(rect.width_ / ratio);
    } else if (static_cast<uint32_t>(rect.height_) < limits.minHeight_) {
        rect.height_ = static_cast<int32_t>(limits.minHeight_);
        rect.width_ = ceil(rect.height_ * ratio);
    }
    if (isDecor) {
        rect.height_ = SessionUtils::ToWinHeight(rect.height_, vpr) ;
        rect.width_ = SessionUtils::ToWinWidth(rect.width_, vpr);
    }
}
bool SceneSession::AdjustRectByAspectRatio(WSRect& rect)
{
    const int tolerancePx = 2; // 2: tolerance delta pixel value, unit: px
    WSRect originalRect = rect;
    auto property = GetSessionProperty();
    if (!property || property->GetWindowMode() != WindowMode::WINDOW_MODE_FLOATING ||
        !WindowHelper::IsMainWindow(GetWindowType())) {
        return false;
    }

    if (MathHelper::NearZero(aspectRatio_)) {
        return false;
    }
    float vpr = 1.5f; // 1.5f: default virtual pixel ratio
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    if (display) {
        vpr = display->GetVirtualPixelRatio();
    }
    int32_t minW;
    int32_t maxW;
    int32_t minH;
    int32_t maxH;
    SessionUtils::CalcFloatWindowRectLimits(property->GetWindowLimits(), systemConfig_.maxFloatingWindowSize_, vpr,
        minW, maxW, minH, maxH);
    rect.width_ = std::max(minW, static_cast<int32_t>(rect.width_));
    rect.width_ = std::min(maxW, static_cast<int32_t>(rect.width_));
    rect.height_ = std::max(minH, static_cast<int32_t>(rect.height_));
    rect.height_ = std::min(maxH, static_cast<int32_t>(rect.height_));
    if (IsDecorEnable()) {
        if (SessionUtils::ToLayoutWidth(rect.width_, vpr) >
                SessionUtils::ToLayoutHeight(rect.height_, vpr) * aspectRatio_) {
            rect.width_ = SessionUtils::ToWinWidth(SessionUtils::ToLayoutHeight(rect.height_, vpr)* aspectRatio_, vpr);
        } else {
            rect.height_ = SessionUtils::ToWinHeight(SessionUtils::ToLayoutWidth(rect.width_, vpr) / aspectRatio_, vpr);
        }
    } else {
        if (rect.width_ > rect.height_ * aspectRatio_) {
            rect.width_ = rect.height_ * aspectRatio_;
        } else {
            rect.height_ = rect.width_ / aspectRatio_;
        }
    }
    AdjustRectByLimits(property->GetWindowLimits(), aspectRatio_, IsDecorEnable(), vpr, rect);
    if (std::abs(static_cast<int32_t>(originalRect.width_) - static_cast<int32_t>(rect.width_)) <= tolerancePx &&
        std::abs(static_cast<int32_t>(originalRect.height_) - static_cast<int32_t>(rect.height_)) <= tolerancePx) {
        rect = originalRect;
        return false;
    }
    return true;
}

void SceneSession::HandleCompatibleModeMoveDrag(WSRect& rect, SizeChangeReason reason)
{
    auto sessionProperty = GetSessionProperty();
    if (!sessionProperty) {
        TLOGE(WmsLogTag::WMS_SCB, "sessionProperty is null");
        return;
    }
    bool isSupportDragInPcCompatibleMode = sessionProperty->GetIsSupportDragInPcCompatibleMode();
    if (reason != SizeChangeReason::DRAG_MOVE) {
        if (reason == SizeChangeReason::DRAG_END && !moveDragController_->GetStartDragFlag()) {
            return;
        } else {
            HandleCompatibleModeDrag(rect, reason, isSupportDragInPcCompatibleMode);
        }
    }
}

void SceneSession::HandleCompatibleModeDrag(WSRect& rect, SizeChangeReason reason, bool isSupportDragInPcCompatibleMode)
{
    auto sessionProperty = GetSessionProperty();
    if (!sessionProperty) {
        TLOGE(WmsLogTag::WMS_SCB, "sessionProperty is null");
        return;
    }
    WindowLimits windowLimits = sessionProperty->GetWindowLimits();
    const int32_t compatibleInPcPortraitWidth = sessionProperty->GetCompatibleInPcPortraitWidth();
    const int32_t compatibleInPcPortraitHeight = sessionProperty->GetCompatibleInPcPortraitHeight();
    const int32_t compatibleInPcLandscapeWidth = sessionProperty->GetCompatibleInPcLandscapeWidth();
    const int32_t compatibleInPcLandscapeHeight = sessionProperty->GetCompatibleInPcLandscapeHeight();
    const int32_t compatibleInPcDragLimit = compatibleInPcLandscapeWidth - compatibleInPcPortraitWidth;
    WSRect windowRect = moveDragController_->GetOriginalRect();
    auto windowWidth = windowRect.width_;
    auto windowHeight = windowRect.height_;
    if (isSupportDragInPcCompatibleMode && windowWidth > windowHeight &&
        (rect.width_ < compatibleInPcLandscapeWidth - compatibleInPcDragLimit ||
            rect.width_ == static_cast<int32_t>(windowLimits.minWidth_))) {
        rect.width_ = compatibleInPcPortraitWidth;
        rect.height_ = compatibleInPcPortraitHeight;
    } else if (isSupportDragInPcCompatibleMode && windowWidth < windowHeight &&
        rect.width_ > compatibleInPcPortraitWidth + compatibleInPcDragLimit) {
        rect.width_ = compatibleInPcLandscapeWidth;
        rect.height_ = compatibleInPcLandscapeHeight;
    } else if (isSupportDragInPcCompatibleMode) {
        rect.width_ = (windowWidth < windowHeight) ? compatibleInPcPortraitWidth : compatibleInPcLandscapeWidth;
        rect.height_ = (windowWidth < windowHeight) ? compatibleInPcPortraitHeight : compatibleInPcLandscapeHeight;
    }
}

void SceneSession::SetMoveDragCallback()
{
    if (moveDragController_) {
        MoveDragCallback callBack = [this](SizeChangeReason reason) {
            this->OnMoveDragCallback(reason);
        };
        moveDragController_->RegisterMoveDragCallback(callBack);
    }
}

/** @note @window.drag */
void SceneSession::InitializeCrossMoveDrag()
{
    auto movedSurfaceNode = GetSurfaceNodeForMoveDrag();
    if (!movedSurfaceNode) {
        return;
    }
    auto parentNode = movedSurfaceNode->GetParent();
    if (!parentNode) {
        return;
    }
    auto property = GetSessionProperty();
    if (!property) {
        return;
    }
    auto originalPositionZ = movedSurfaceNode->GetStagingProperties().GetPositionZ();
    moveDragController_->SetOriginalPositionZ(originalPositionZ);
    moveDragController_->InitCrossDisplayProperty(property->GetDisplayId(), parentNode->GetId());
}

/** @note @window.drag */
void SceneSession::HandleMoveDragSurfaceBounds(WSRect& rect, WSRect& globalRect, SizeChangeReason reason)
{
    bool isGlobal = (reason != SizeChangeReason::DRAG_END);
    bool needFlush = (reason != SizeChangeReason::DRAG_END);
    throwSlipToFullScreenAnimCount_.store(0);
    UpdateSizeChangeReason(reason);
    if (moveDragController_ && moveDragController_->GetPointerType() == MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN &&
        pcFoldScreenController_ && pcFoldScreenController_->IsAllowThrowSlip(GetScreenId()) &&
        (reason == SizeChangeReason::DRAG_MOVE || reason == SizeChangeReason::DRAG_END)) {
        if (pcFoldScreenController_->NeedFollowHandAnimation()) {
            auto movingPair = std::make_pair(pcFoldScreenController_->GetMovingTimingProtocol(),
                pcFoldScreenController_->GetMovingTimingCurve());
            SetSurfaceBoundsWithAnimation(movingPair, globalRect, nullptr, isGlobal);
        } else if (reason != SizeChangeReason::DRAG_START) {
            SetSurfaceBounds(globalRect, isGlobal, needFlush);
        }
        pcFoldScreenController_->RecordMoveRects(rect);
    } else {
        SetSurfaceBounds(globalRect, isGlobal, needFlush);
    }
    if (reason != SizeChangeReason::DRAG_MOVE) {
        UpdateRectForDrag(rect);
        std::shared_ptr<VsyncCallback> nextVsyncDragCallback = std::make_shared<VsyncCallback>();
        nextVsyncDragCallback->onCallback = [weakThis = wptr(this), where = __func__](int64_t, int64_t) {
            auto session = weakThis.promote();
            if (!session) {
                TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s: session is null", where);
                return;
            }
            session->OnNextVsyncReceivedWhenDrag();
        };
        if (requestNextVsyncFunc_) {
            requestNextVsyncFunc_(nextVsyncDragCallback);
        } else {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "Func is null, could not request vsync");
        }
    }
}

void SceneSession::OnNextVsyncReceivedWhenDrag()
{
    PostTask([weakThis = wptr(this), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s: session is null", where);
            return;
        }
        if (session->IsDirtyDragWindow()) {
            TLOGND(WmsLogTag::WMS_LAYOUT, "%{public}s: id:%{public}u, winRect:%{public}s",
                where, session->GetPersistentId(), session->winRect_.ToString().c_str());
            session->NotifyClientToUpdateRect("OnMoveDragCallback", nullptr);
            session->ResetDirtyDragFlags();
        }
    });
}

/** @note @window.drag */
void SceneSession::HandleMoveDragEnd(WSRect& rect, SizeChangeReason reason)
{
    if (GetOriPosYBeforeRaisedByKeyboard() != 0) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Calling session is moved and reset oriPosYBeforeRaisedByKeyboard");
        SetOriPosYBeforeRaisedByKeyboard(0);
    }
    if (moveDragController_->GetPointerType() == MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN &&
        MoveUnderInteriaAndNotifyRectChange(rect, reason)) {
        TLOGI(WmsLogTag::WMS_LAYOUT_PC, "set full screen after throw slip");
    }
    if ((moveDragController_->GetMoveDragEndDisplayId() == moveDragController_->GetMoveDragStartDisplayId() ||
         WindowHelper::IsSystemWindow(GetWindowType())) && !WindowHelper::IsInputWindow(GetWindowType())) {
        NotifySessionRectChange(rect, reason);
    } else {
        displayChangedByMoveDrag_ = true;
        NotifySessionRectChange(rect, reason, moveDragController_->GetMoveDragEndDisplayId());
        CheckSubSessionShouldFollowParent(moveDragController_->GetMoveDragEndDisplayId());
    }
    moveDragController_->ResetCrossMoveDragProperty();
    OnSessionEvent(SessionEvent::EVENT_END_MOVE);
}

/**
 * move with init velocity
 * @return true: successfully throw slip
 */
bool SceneSession::MoveUnderInteriaAndNotifyRectChange(WSRect& rect, SizeChangeReason reason)
{
    if (pcFoldScreenController_ == nullptr) {
        return false;
    }
    bool ret = pcFoldScreenController_->ThrowSlip(GetScreenId(), rect, GetStatusBarHeight(), GetDockHeight());
    if (!ret) {
        TLOGD(WmsLogTag::WMS_LAYOUT_PC, "no throw slip");
        pcFoldScreenController_->ResetRecords();
        return false;
    }

    WSRect endRect = rect;
    std::function<void()> finishCallback = nullptr;
    bool needSetFullScreen = pcFoldScreenController_->IsStartFullScreen();
    if (needSetFullScreen) {
        // maximize end rect and notify last rect
        throwSlipToFullScreenAnimCount_.fetch_add(1);
        pcFoldScreenController_->ResizeToFullScreen(endRect, GetStatusBarHeight(), GetDockHeight());
        if (pcFoldScreenController_->IsThrowSlipDirectly()) {
            pcFoldScreenController_->ThrowSlipFloatingRectDirectly(
                rect, GetSessionRequestRect(), GetStatusBarHeight(), GetDockHeight());
        }
        finishCallback = [weakThis = wptr(this), rect, where = __func__] {
            auto session = weakThis.promote();
            if (session == nullptr) {
                TLOGNW(WmsLogTag::WMS_LAYOUT_PC, "%{public}s session is nullptr", where);
                return;
            }
            session->OnThrowSlipAnimationStateChange(false);
            session->NotifyFullScreenAfterThrowSlip(rect);
        };
    } else {
        finishCallback = [weakThis = wptr(this), rect, where = __func__] {
            auto session = weakThis.promote();
            if (session == nullptr) {
                TLOGNW(WmsLogTag::WMS_LAYOUT_PC, "%{public}s session is nullptr", where);
                return;
            }
            session->OnThrowSlipAnimationStateChange(false);
        };
    }
    auto throwSlipPair = std::make_pair(pcFoldScreenController_->GetThrowSlipTimingProtocol(),
        pcFoldScreenController_->GetThrowSlipTimingCurve());
    SetSurfaceBoundsWithAnimation(throwSlipPair, endRect, finishCallback);
    OnThrowSlipAnimationStateChange(true);
    rect = endRect;
    pcFoldScreenController_->ResetRecords();
    return true;
}

void SceneSession::OnThrowSlipAnimationStateChange(bool isAnimating)
{
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "status: %{public}d", isAnimating);
    auto leashWinSurfaceNode = GetLeashWinSurfaceNode();
    if (leashWinSurfaceNode) {
        leashWinSurfaceNode->SetUIFirstSwitch(isAnimating ? RSUIFirstSwitch::FORCE_DISABLE : RSUIFirstSwitch::NONE);
    }
    if (onThrowSlipAnimationStateChangeFunc_) {
        onThrowSlipAnimationStateChangeFunc_(isAnimating);
    }
}

void SceneSession::NotifyFullScreenAfterThrowSlip(const WSRect& rect)
{
    PostTask([weakThis = wptr(this), rect, where = __func__] {
        auto session = weakThis.promote();
        if (session == nullptr) {
            TLOGNW(WmsLogTag::WMS_LAYOUT, "%{public}s session is nullptr", where);
            return;
        }
        if (!session->IsVisibleForeground()) {
            TLOGNW(WmsLogTag::WMS_LAYOUT, "%{public}s session go background when throw", where);
            return;
        }
        if (session->throwSlipToFullScreenAnimCount_.load() == 0) {
            TLOGNW(WmsLogTag::WMS_LAYOUT, "%{public}s session moved when throw", where);
            return;
        }
        if (session->throwSlipToFullScreenAnimCount_.load() > 1) {
            TLOGNW(WmsLogTag::WMS_LAYOUT, "%{public}s throw-slip fullscreen animation count: %{public}u",
                where, session->throwSlipToFullScreenAnimCount_.load());
            session->throwSlipToFullScreenAnimCount_.fetch_sub(1);
            return;
        }
        session->throwSlipToFullScreenAnimCount_.fetch_sub(1);
        if (!session->onSessionEvent_) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s invalid callback", where);
            return;
        }
        TLOGNI(WmsLogTag::WMS_LAYOUT, "%{public}s rect: %{public}s", where, rect.ToString().c_str());
        session->onSessionEvent_(
            static_cast<uint32_t>(SessionEvent::EVENT_MAXIMIZE_WITHOUT_ANIMATION),
            SessionEventParam {rect.posX_, rect.posY_, rect.width_, rect.height_});
    }, __func__);
}

void SceneSession::ThrowSlipDirectly(const WSRectF& velocity)
{
    const char* const where = __func__;
    PostTask([weakThis = wptr(this), velocity, where] {
        auto session = weakThis.promote();
        if (session == nullptr) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s session is nullptr", where);
            return;
        }
        auto controller = session->pcFoldScreenController_;
        TLOGNI(WmsLogTag::WMS_LAYOUT_PC, "%{public}s session: %{public}d, velocity: %{public}s",
            where, session->GetPersistentId(), velocity.ToString().c_str());
        if (!(controller && controller->IsAllowThrowSlip(session->GetScreenId()))) {
            TLOGNW(WmsLogTag::WMS_LAYOUT_PC, "%{public}s not allow throw slip", where);
            return;
        }
        if (!session->IsMovableWindowType()) {
            TLOGNW(WmsLogTag::WMS_LAYOUT_PC, "%{public}s not movable", where);
            return;
        }
        bool isFullScreen = session->IsFullScreenMovable();
        controller->RecordStartMoveRectDirectly(session->GetSessionRect(), velocity, isFullScreen);
        const WSRect& oriGlobalRect = session->GetSessionGlobalRect();
        WSRect globalRect = oriGlobalRect;
        if (!session->MoveUnderInteriaAndNotifyRectChange(globalRect, SizeChangeReason::UNDEFINED)) {
            TLOGNW(WmsLogTag::WMS_LAYOUT_PC, "%{public}s no throw", where);
            return;
        }
        if (isFullScreen) {
            session->UpdateFullScreenWaterfallMode(false);
        }
        WSRect rect = session->GetSessionRect();
        rect.posX_ += globalRect.posX_ - oriGlobalRect.posX_;
        rect.posY_ += globalRect.posY_ - oriGlobalRect.posY_;
        session->NotifySessionRectChange(globalRect, SizeChangeReason::UNDEFINED);
    }, __func__);
}

/** @note @window.drag */
void SceneSession::OnMoveDragCallback(SizeChangeReason reason)
{
    if (!moveDragController_) {
        WLOGE("moveDragController_ is null");
        return;
    }
    auto property = GetSessionProperty();
    if (property == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "property is null");
        return;
    }
    if (reason == SizeChangeReason::DRAG_START) {
        InitializeCrossMoveDrag();
    }
    bool isCompatibleModeInPc = property->GetCompatibleModeInPc();
    bool isSupportDragInPcCompatibleMode = property->GetIsSupportDragInPcCompatibleMode();
    bool isMainWindow = WindowHelper::IsMainWindow(property->GetWindowType());
    WSRect rect = moveDragController_->GetTargetRect(
        MoveDragController::TargetRectCoordinate::RELATED_TO_START_DISPLAY);
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER,
        "SceneSession::OnMoveDragCallback [%d, %d, %u, %u]", rect.posX_, rect.posY_, rect.width_, rect.height_);
    if (reason == SizeChangeReason::DRAG || reason == SizeChangeReason::DRAG_END) {
        UpdateWinRectForSystemBar(rect);
    }
    if (isCompatibleModeInPc && !IsFreeMultiWindowMode()) {
        HandleCompatibleModeMoveDrag(rect, reason);
    }
    if (IsDragResizeWhenEnd(reason)) {
        UpdateSizeChangeReason(reason);
        OnSessionEvent(SessionEvent::EVENT_DRAG);
        return;
    }
    moveDragController_->SetTargetRect(rect);
    TLOGD(WmsLogTag::WMS_LAYOUT, "Rect: [%{public}d, %{public}d, %{public}u, %{public}u], reason: %{public}d, "
        "isCompatibleMode: %{public}d",
        rect.posX_, rect.posY_, rect.width_, rect.height_, reason, isCompatibleModeInPc);
    WSRect relativeRect = moveDragController_->GetTargetRect(reason == SizeChangeReason::DRAG_END ?
        MoveDragController::TargetRectCoordinate::RELATED_TO_END_DISPLAY :
        MoveDragController::TargetRectCoordinate::RELATED_TO_START_DISPLAY);
    WSRect globalRect = moveDragController_->GetTargetRect(reason == SizeChangeReason::DRAG_END ?
        MoveDragController::TargetRectCoordinate::RELATED_TO_END_DISPLAY :
        MoveDragController::TargetRectCoordinate::GLOBAL);
    HandleMoveDragSurfaceNode(reason);
    HandleMoveDragSurfaceBounds(relativeRect, globalRect, reason);
    if (reason == SizeChangeReason::DRAG_END) {
        HandleMoveDragEnd(relativeRect, reason);
        SetUIFirstSwitch(RSUIFirstSwitch::NONE);
    } else if (reason == SizeChangeReason::DRAG_START) {
        OnSessionEvent(SessionEvent::EVENT_DRAG_START);
        NotifyUpdateGravity();
        SetUIFirstSwitch(RSUIFirstSwitch::FORCE_DISABLE);
    }
}

bool SceneSession::IsDragResizeWhenEnd(SizeChangeReason reason)
{
    auto property = GetSessionProperty();
    if (property == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "property is null");
        return true;
    }
    bool isPcOrPcModeMainWindow = (systemConfig_.IsPcWindow() || IsFreeMultiWindowMode()) &&
        WindowHelper::IsMainWindow(property->GetWindowType());
    return reason == SizeChangeReason::DRAG && isPcOrPcModeMainWindow &&
        GetDragResizeTypeDuringDrag() == DragResizeType::RESIZE_WHEN_DRAG_END;
}

/** @note @window.drag */
void SceneSession::HandleMoveDragSurfaceNode(SizeChangeReason reason)
{
    auto movedSurfaceNode = GetSurfaceNodeForMoveDrag();
    if (movedSurfaceNode == nullptr) {
        TLOGD(WmsLogTag::WMS_LAYOUT, "SurfaceNode is null");
        return;
    }
    if (reason == SizeChangeReason::DRAG || reason == SizeChangeReason::DRAG_MOVE) {
        auto rsTransaction = RSTransactionProxy::GetInstance();
        if (rsTransaction != nullptr) {
            rsTransaction->Begin();
        }
        for (const auto displayId : moveDragController_->GetNewAddedDisplayIdsDuringMoveDrag()) {
            if (displayId == moveDragController_->GetMoveDragStartDisplayId()) {
                continue;
            }
            auto screenSession = ScreenSessionManagerClient::GetInstance().GetScreenSessionById(displayId);
            if (screenSession == nullptr) {
                TLOGD(WmsLogTag::WMS_LAYOUT, "ScreenSession is null");
                continue;
            }
            if (screenSession->GetScreenProperty().GetScreenType() == ScreenType::VIRTUAL) {
                TLOGD(WmsLogTag::WMS_LAYOUT, "virtual screen, no need to add cross parent child");
                continue;
            }
            movedSurfaceNode->SetPositionZ(MOVE_DRAG_POSITION_Z);
            screenSession->GetDisplayNode()->AddCrossScreenChild(movedSurfaceNode, -1, true);
            movedSurfaceNode->SetIsCrossNode(true);
            TLOGD(WmsLogTag::WMS_LAYOUT, "Add window to display: %{public}" PRIu64, displayId);
        }
        if (rsTransaction != nullptr) {
            rsTransaction->Commit();
        }
    } else if (reason == SizeChangeReason::DRAG_END) {
        for (const auto displayId : moveDragController_->GetDisplayIdsDuringMoveDrag()) {
            if (displayId == moveDragController_->GetMoveDragStartDisplayId()) {
                continue;
            }
            auto screenSession = ScreenSessionManagerClient::GetInstance().GetScreenSessionById(displayId);
            if (screenSession == nullptr) {
                TLOGD(WmsLogTag::WMS_LAYOUT, "ScreenSession is null");
                continue;
            }
            if (screenSession->GetScreenProperty().GetScreenType() == ScreenType::VIRTUAL) {
                TLOGD(WmsLogTag::WMS_LAYOUT, "virtual screen, no need to remove cross parent child");
                continue;
            }
            movedSurfaceNode->SetPositionZ(moveDragController_->GetOriginalPositionZ());
            screenSession->GetDisplayNode()->RemoveCrossScreenChild(movedSurfaceNode);
            movedSurfaceNode->SetIsCrossNode(false);
            TLOGD(WmsLogTag::WMS_LAYOUT, "Remove window from display: %{public}" PRIu64, displayId);
        }
    }
}

/** @note @window.drag */
WSError SceneSession::UpdateRectForDrag(const WSRect& rect)
{
    return PostSyncTask([weakThis = wptr(this), rect, where = __func__] {
        auto sceneSession = weakThis.promote();
        if (!sceneSession) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s: session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        sceneSession->winRect_ = rect;
        sceneSession->dirtyFlags_ |= static_cast<uint32_t>(SessionUIDirtyFlag::DRAG_RECT);
        sceneSession->isDragging_ = true; // isDrag only reset by Vsync, not flushuiparam
        return WSError::WS_OK;
    }, __func__);
}

/** @note @window.drag */
void SceneSession::UpdateWinRectForSystemBar(WSRect& rect)
{
    if (!specificCallback_) {
        WLOGFE("specificCallback_ is null!");
        return;
    }
    auto sessionProperty = GetSessionProperty();
    float tmpPosY = 0.0;
    std::vector<sptr<SceneSession>> statusBarVector;
    if (specificCallback_->onGetSceneSessionVectorByTypeAndDisplayId_) {
        statusBarVector = specificCallback_->onGetSceneSessionVectorByTypeAndDisplayId_(
            WindowType::WINDOW_TYPE_STATUS_BAR, sessionProperty->GetDisplayId());
    }
    for (auto& statusBar : statusBarVector) {
        if (!(statusBar->isVisible_)) {
            continue;
        }
        WSRect statusBarRect = statusBar->GetSessionRect();
        if ((rect.posY_ < statusBarRect.posY_ + static_cast<int32_t>(statusBarRect.height_)) &&
            (rect.height_ != winRect_.height_ || rect.width_ != winRect_.width_)) {
            tmpPosY = rect.posY_ + rect.height_;
            rect.posY_ = statusBarRect.posY_ + statusBarRect.height_;
            rect.height_ = tmpPosY - rect.posY_;
        }
    }
    WLOGFD("after UpdateWinRectForSystemBar rect: [%{public}d, %{public}d, %{public}u, %{public}u]",
        rect.posX_, rect.posY_, rect.width_, rect.height_);
}

void SceneSession::SetSurfaceBoundsWithAnimation(
    const std::pair<RSAnimationTimingProtocol, RSAnimationTimingCurve>& animationParam,
    const WSRect& rect, const std::function<void()>& finishCallback, bool isGlobal)
{
    RSNode::Animate(animationParam.first, animationParam.second, [weakThis = wptr(this), rect,
        isGlobal, where = __func__] {
        auto session = weakThis.promote();
        if (session == nullptr) {
            TLOGNW(WmsLogTag::WMS_LAYOUT, "%{public}s session is nullptr", where);
            return;
        }
        session->SetSurfaceBounds(rect, isGlobal, false);
        RSTransaction::FlushImplicitTransaction();
    }, finishCallback);
}

void SceneSession::SetSurfaceBounds(const WSRect& rect, bool isGlobal, bool needFlush)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "id: %{public}d, rect: %{public}s isGlobal: %{public}d needFlush: %{public}d",
        GetPersistentId(), rect.ToString().c_str(), isGlobal, needFlush);
    auto rsTransaction = RSTransactionProxy::GetInstance();
    if (rsTransaction != nullptr && needFlush) {
        rsTransaction->Begin();
    }
    auto surfaceNode = GetSurfaceNode();
    auto leashWinSurfaceNode = GetLeashWinSurfaceNode();
    NotifySubAndDialogFollowRectChange(rect, isGlobal, needFlush);
    if (surfaceNode && leashWinSurfaceNode) {
        leashWinSurfaceNode->SetGlobalPositionEnabled(isGlobal);
        leashWinSurfaceNode->SetBounds(rect.posX_, rect.posY_, rect.width_, rect.height_);
        leashWinSurfaceNode->SetFrame(rect.posX_, rect.posY_, rect.width_, rect.height_);
        surfaceNode->SetBounds(0, 0, rect.width_, rect.height_);
        surfaceNode->SetFrame(0, 0, rect.width_, rect.height_);
    } else if (WindowHelper::IsPipWindow(GetWindowType()) && surfaceNode) {
        TLOGD(WmsLogTag::WMS_PIP, "PipWindow setSurfaceBounds");
        surfaceNode->SetGlobalPositionEnabled(isGlobal);
        surfaceNode->SetBounds(rect.posX_, rect.posY_, rect.width_, rect.height_);
        surfaceNode->SetFrame(rect.posX_, rect.posY_, rect.width_, rect.height_);
    } else if (WindowHelper::IsSubWindow(GetWindowType()) && surfaceNode) {
        WLOGFD("subwindow setSurfaceBounds");
        surfaceNode->SetGlobalPositionEnabled(isGlobal);
        surfaceNode->SetBounds(rect.posX_, rect.posY_, rect.width_, rect.height_);
        surfaceNode->SetFrame(rect.posX_, rect.posY_, rect.width_, rect.height_);
    } else if (WindowHelper::IsDialogWindow(GetWindowType()) && surfaceNode) {
        TLOGD(WmsLogTag::WMS_DIALOG, "dialogWindow setSurfaceBounds");
        surfaceNode->SetGlobalPositionEnabled(isGlobal);
        surfaceNode->SetBounds(rect.posX_, rect.posY_, rect.width_, rect.height_);
        surfaceNode->SetFrame(rect.posX_, rect.posY_, rect.width_, rect.height_);
    } else if (WindowHelper::IsSystemWindow(GetWindowType()) && surfaceNode) {
        TLOGD(WmsLogTag::WMS_SYSTEM, "system window setSurfaceBounds");
        surfaceNode->SetGlobalPositionEnabled(isGlobal);
        surfaceNode->SetBounds(rect.posX_, rect.posY_, rect.width_, rect.height_);
        surfaceNode->SetFrame(rect.posX_, rect.posY_, rect.width_, rect.height_);
    } else {
        WLOGE("SetSurfaceBounds surfaceNode is null!");
    }
    if (rsTransaction != nullptr && needFlush) {
        rsTransaction->Commit();
    }
}

void SceneSession::SetZOrder(uint32_t zOrder)
{
    PostTask([weakThis = wptr(this), zOrder, where = __func__] {
        auto session = weakThis.promote();
        if (session == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return;
        }
        if (session->zOrder_ != zOrder) {
            session->Session::SetZOrder(zOrder);
            if (session->specificCallback_ != nullptr) {
                session->specificCallback_->onWindowInfoUpdate_(session->GetPersistentId(),
                    WindowUpdateType::WINDOW_UPDATE_PROPERTY);
            }
        }
    }, __func__);
}

void SceneSession::SetFloatingScale(float floatingScale)
{
    if (floatingScale_ != floatingScale) {
        Session::SetFloatingScale(floatingScale);
        if (specificCallback_ != nullptr) {
            specificCallback_->onWindowInfoUpdate_(GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_PROPERTY);
            if (Session::IsScbCoreEnabled()) {
                MarkAvoidAreaAsDirty();
            } else {
                specificCallback_->onUpdateAvoidArea_(GetPersistentId());
            }
        }
    }
}

void SceneSession::SetParentPersistentId(int32_t parentId)
{
    auto property = GetSessionProperty();
    if (property) {
        property->SetParentPersistentId(parentId);
    }
}

int32_t SceneSession::GetParentPersistentId() const
{
    auto property = GetSessionProperty();
    if (property) {
        return property->GetParentPersistentId();
    }
    return INVALID_SESSION_ID;
}

int32_t SceneSession::GetMainSessionId()
{
    const auto& mainSession = GetMainSession();
    if (mainSession) {
        return mainSession->GetPersistentId();
    }
    return INVALID_SESSION_ID;
}

std::string SceneSession::GetWindowNameAllType() const
{
    if (GetSessionInfo().isSystem_) {
        return GetSessionInfo().abilityName_;
    } else {
        return GetWindowName();
    }
}

WSError SceneSession::SetTurnScreenOn(bool turnScreenOn)
{
    GetSessionProperty()->SetTurnScreenOn(turnScreenOn);
    return WSError::WS_OK;
}

bool SceneSession::IsTurnScreenOn() const
{
    return GetSessionProperty()->IsTurnScreenOn();
}

WSError SceneSession::SetKeepScreenOn(bool keepScreenOn)
{
    GetSessionProperty()->SetKeepScreenOn(keepScreenOn);
    return WSError::WS_OK;
}

bool SceneSession::IsKeepScreenOn() const
{
    return GetSessionProperty()->IsKeepScreenOn();
}

WSError SceneSession::SetViewKeepScreenOn(bool keepScreenOn)
{
    GetSessionProperty()->SetViewKeepScreenOn(keepScreenOn);
    return WSError::WS_OK;
}

bool SceneSession::IsViewKeepScreenOn() const
{
    return GetSessionProperty()->IsViewKeepScreenOn();
}

void SceneSession::SaveUpdatedIcon(const std::shared_ptr<Media::PixelMap>& icon)
{
    WLOGFI("run SaveUpdatedIcon");
    if (scenePersistence_ != nullptr) {
        scenePersistence_->SaveUpdatedIcon(icon);
    }
}

std::string SceneSession::GetUpdatedIconPath() const
{
    WLOGFI("run GetUpdatedIconPath");
    if (scenePersistence_ != nullptr) {
        return scenePersistence_->GetUpdatedIconPath();
    }
    return "";
}

void SceneSession::UpdateNativeVisibility(bool visible)
{
    PostTask([weakThis = wptr(this), visible, where = __func__]() THREAD_SAFETY_GUARD(SCENE_GUARD) {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return;
        }
        int32_t persistentId = session->GetPersistentId();
        TLOGNI(WmsLogTag::WMS_SCB, "%{public}s name: %{public}s, id: %{public}u, visible: %{public}u",
            where, session->sessionInfo_.bundleName_.c_str(), persistentId, visible);
        bool oldVisibleState = session->isVisible_;
        session->isVisible_ = visible;
        if (session->visibilityChangedDetectFunc_) {
            session->visibilityChangedDetectFunc_(session->GetCallingPid(), oldVisibleState, visible);
        }
        if (session->specificCallback_ == nullptr) {
            TLOGNW(WmsLogTag::WMS_SCB, "%{public}s specific callback is null.", where);
            return;
        }
        if (visible) {
            session->specificCallback_->onWindowInfoUpdate_(persistentId, WindowUpdateType::WINDOW_UPDATE_ADDED);
        } else {
            session->specificCallback_->onWindowInfoUpdate_(persistentId, WindowUpdateType::WINDOW_UPDATE_REMOVED);
        }
        session->NotifyAccessibilityVisibilityChange();
        session->specificCallback_->onUpdateAvoidArea_(persistentId);
        // update private state
        if (!session->GetSessionProperty()) {
            TLOGNE(WmsLogTag::WMS_SCB, "%{public}s property is null", where);
            return;
        }
        if (session->updatePrivateStateAndNotifyFunc_ != nullptr) {
            session->updatePrivateStateAndNotifyFunc_(persistentId);
        }
    }, __func__);
}

void SceneSession::UpdateRotationAvoidArea()
{
    if (specificCallback_) {
        if (Session::IsScbCoreEnabled()) {
            MarkAvoidAreaAsDirty();
        } else {
            specificCallback_->onUpdateAvoidArea_(GetPersistentId());
        }
    }
}

void SceneSession::SetPrivacyMode(bool isPrivacy)
{
    auto property = GetSessionProperty();
    if (!property) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "property is null");
        return;
    }
    auto surfaceNode = GetSurfaceNode();
    if (!surfaceNode) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "surfaceNode_ is null");
        return;
    }
    bool lastPrivacyMode = property->GetPrivacyMode() || property->GetSystemPrivacyMode();
    if (lastPrivacyMode == isPrivacy) {
        TLOGW(WmsLogTag::WMS_ATTRIBUTE, "privacy mode does not change, isPrivacy:%{public}d", isPrivacy);
        return;
    }
    property->SetPrivacyMode(isPrivacy);
    property->SetSystemPrivacyMode(isPrivacy);
    auto rsTransaction = RSTransactionProxy::GetInstance();
    if (rsTransaction != nullptr) {
        rsTransaction->Begin();
    }
    surfaceNode->SetSecurityLayer(isPrivacy);
    auto leashWinSurfaceNode = GetLeashWinSurfaceNode();
    if (leashWinSurfaceNode != nullptr) {
        leashWinSurfaceNode->SetSecurityLayer(isPrivacy);
    }
    if (rsTransaction != nullptr) {
        rsTransaction->Commit();
    }
    NotifyPrivacyModeChange();
}

void SceneSession::NotifyPrivacyModeChange()
{
    bool isPrivacyMode = GetSessionProperty()->GetPrivacyMode();
    std::unique_lock<std::shared_mutex> lock(combinedExtWindowFlagsMutex_);
    bool currExtPrivacyMode = combinedExtWindowFlags_.privacyModeFlag;
    TLOGD(WmsLogTag::WMS_SCB, "id:%{public}d, currExtPrivacyMode:%{public}d, session property privacyMode: %{public}d, "
        "last privacyMode:%{public}d",
        GetPersistentId(), currExtPrivacyMode, isPrivacyMode, isPrivacyMode_);
    bool mixedPrivacyMode = currExtPrivacyMode || isPrivacyMode;
    if (mixedPrivacyMode != isPrivacyMode_) {
        isPrivacyMode_ = mixedPrivacyMode;
        if (privacyModeChangeNotifyFunc_) {
            privacyModeChangeNotifyFunc_(isPrivacyMode_);
        }
    }
}

WMError SceneSession::SetSnapshotSkip(bool isSkip)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "id: %{public}d, permission denied!", GetPersistentId());
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    auto property = GetSessionProperty();
    if (!property) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "property is null");
        return WMError::WM_ERROR_DESTROYED_OBJECT;
    }
    auto surfaceNode = GetSurfaceNode();
    if (!surfaceNode) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "surfaceNode_ is null");
        return WMError::WM_ERROR_DESTROYED_OBJECT;
    }
    bool lastSnapshotSkip = property->GetSnapshotSkip();
    if (lastSnapshotSkip == isSkip) {
        TLOGW(WmsLogTag::WMS_ATTRIBUTE, "Snapshot skip does not change, do nothing, isSkip: %{public}d, "
            "id: %{public}d", isSkip, GetPersistentId());
        return WMError::WM_OK;
    }
    property->SetSnapshotSkip(isSkip);
    auto rsTransaction = RSTransactionProxy::GetInstance();
    if (rsTransaction != nullptr) {
        rsTransaction->Begin();
    }
    surfaceNode->SetSkipLayer(isSkip);
    auto leashWinSurfaceNode = GetLeashWinSurfaceNode();
    if (leashWinSurfaceNode != nullptr) {
        leashWinSurfaceNode->SetSkipLayer(isSkip);
    }
    if (rsTransaction != nullptr) {
        rsTransaction->Commit();
    }
    return WMError::WM_OK;
}

void SceneSession::SetWatermarkEnabled(const std::string& watermarkName, bool isEnabled)
{
    auto surfaceNode = GetSurfaceNode();
    if (!surfaceNode) {
        TLOGE(WmsLogTag::DEFAULT, "surfaceNode is null");
        return;
    }
    TLOGI(WmsLogTag::DEFAULT, "watermarkName:%{public}s, isEnabled:%{public}d, wid:%{public}d",
        watermarkName.c_str(), isEnabled, GetPersistentId());
    auto rsTransaction = RSTransactionProxy::GetInstance();
    if (rsTransaction != nullptr) {
        rsTransaction->Begin();
    }
    surfaceNode->SetWatermarkEnabled(watermarkName, isEnabled);
    if (auto leashWinSurfaceNode = GetLeashWinSurfaceNode()) {
        leashWinSurfaceNode->SetWatermarkEnabled(watermarkName, isEnabled);
    }
    if (rsTransaction != nullptr) {
        rsTransaction->Commit();
    }
}

void SceneSession::SetPiPTemplateInfo(const PiPTemplateInfo& pipTemplateInfo)
{
    pipTemplateInfo_ = pipTemplateInfo;
}

void SceneSession::SetSystemSceneOcclusionAlpha(double alpha)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSession::SetAbilityBGAlpha");
    if (alpha < 0 || alpha > 1.0) {
        WLOGFE("OnSetSystemSceneOcclusionAlpha property is null");
        return;
    }
    auto surfaceNode = GetSurfaceNode();
    if (!surfaceNode) {
        WLOGFE("surfaceNode_ is null");
        return;
    }
    uint8_t alpha8bit = static_cast<uint8_t>(alpha * 255);
    WLOGFI("SetAbilityBGAlpha alpha8bit=%{public}u.", alpha8bit);
    auto rsTransaction = RSTransactionProxy::GetInstance();
    if (rsTransaction != nullptr) {
        rsTransaction->Begin();
    }
    surfaceNode->SetAbilityBGAlpha(alpha8bit);
    if (rsTransaction != nullptr) {
        rsTransaction->Commit();
    }
}

void SceneSession::ResetOcclusionAlpha()
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSession::ResetAbilityBGAlpha");
    auto surfaceNode = GetSurfaceNode();
    if (!surfaceNode) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "surfaceNode_ is null");
        return;
    }
    uint8_t alpha8bit = GetSessionProperty()->GetBackgroundAlpha();
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "alpha8bit=%{public}u, windowId=%{public}d", alpha8bit, GetPersistentId());
    auto rsTransaction = RSTransactionProxy::GetInstance();
    if (rsTransaction != nullptr) {
        rsTransaction->Begin();
    }
    surfaceNode->SetAbilityBGAlpha(alpha8bit);
    if (rsTransaction != nullptr) {
        rsTransaction->Commit();
    }
}

void SceneSession::SetSystemSceneForceUIFirst(bool forceUIFirst)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSession::SetForceUIFirst");
    auto leashWinSurfaceNode = GetLeashWinSurfaceNode();
    auto surfaceNode = GetSurfaceNode();
    if (leashWinSurfaceNode == nullptr && surfaceNode == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "leashWindow and surfaceNode are nullptr");
        return;
    }
    auto rsTransaction = RSTransactionProxy::GetInstance();
    if (rsTransaction != nullptr) {
        rsTransaction->Begin();
    }
    if (leashWinSurfaceNode != nullptr) {
        TLOGI(WmsLogTag::DEFAULT, "%{public}s %{public}" PRIu64 " forceUIFirst=%{public}d.",
            leashWinSurfaceNode->GetName().c_str(), leashWinSurfaceNode->GetId(), forceUIFirst);
        leashWinSurfaceNode->SetForceUIFirst(forceUIFirst);
    } else if (surfaceNode != nullptr) {
        TLOGI(WmsLogTag::DEFAULT, "%{public}s %{public}" PRIu64 " forceUIFirst=%{public}d.",
            surfaceNode->GetName().c_str(), surfaceNode->GetId(), forceUIFirst);
        surfaceNode->SetForceUIFirst(forceUIFirst);
    }
    if (rsTransaction != nullptr) {
        rsTransaction->Commit();
    }
}

void SceneSession::SetUIFirstSwitch(RSUIFirstSwitch uiFirstSwitch)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSession::SetUIFirstSwitch");
    auto rsTransaction = RSTransactionProxy::GetInstance();
    if (rsTransaction == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "rsTransaction is nullptr");
        return;
    }
    rsTransaction->Begin();
    auto leashWinSurfaceNode = GetLeashWinSurfaceNode();
    auto surfaceNode = GetSurfaceNode();
    if (leashWinSurfaceNode != nullptr) {
        TLOGI(WmsLogTag::DEFAULT, "%{public}s %{public}" PRIu64 " uiFirstSwitch=%{public}d",
            leashWinSurfaceNode->GetName().c_str(), leashWinSurfaceNode->GetId(), uiFirstSwitch);
        leashWinSurfaceNode->SetUIFirstSwitch(uiFirstSwitch);
    } else if (surfaceNode != nullptr) {
        TLOGI(WmsLogTag::DEFAULT, "%{public}s %{public}" PRIu64 " uiFirstSwitch=%{public}d",
            surfaceNode->GetName().c_str(), surfaceNode->GetId(), uiFirstSwitch);
        surfaceNode->SetUIFirstSwitch(uiFirstSwitch);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "leashWindow and surfaceNode are nullptr");
    }
    rsTransaction->Commit();
}

void SceneSession::CloneWindow(NodeId surfaceNodeId)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSession::CloneWindow");
    auto rsTransaction = RSTransactionProxy::GetInstance();
    if (rsTransaction != nullptr) {
        rsTransaction->Begin();
    }
    if (auto surfaceNode = GetSurfaceNode()) {
        TLOGI(WmsLogTag::WMS_PC, "%{public}s this: %{public}" PRIu64 " cloned: %{public}" PRIu64,
            surfaceNode->GetName().c_str(), surfaceNode->GetId(), surfaceNodeId);
        surfaceNode->SetClonedNodeId(surfaceNodeId);
    }
    if (rsTransaction != nullptr) {
        rsTransaction->Commit();
    }
}

void SceneSession::MarkSystemSceneUIFirst(bool isForced, bool isUIFirstEnabled)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSession::MarkSystemSceneUIFirst");
    auto leashWinSurfaceNode = GetLeashWinSurfaceNode();
    auto surfaceNode = GetSurfaceNode();
    if (leashWinSurfaceNode == nullptr && surfaceNode == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "leashWindow and surfaceNode are nullptr");
        return;
    }
    if (leashWinSurfaceNode != nullptr) {
        TLOGI(WmsLogTag::DEFAULT, "%{public}s %{public}" PRIu64 " isForced=%{public}d. isUIFirstEnabled=%{public}d",
            leashWinSurfaceNode->GetName().c_str(), leashWinSurfaceNode->GetId(), isForced, isUIFirstEnabled);
        leashWinSurfaceNode->MarkUifirstNode(isForced, isUIFirstEnabled);
    } else {
        TLOGI(WmsLogTag::DEFAULT, "%{public}s %{public}" PRIu64 " isForced=%{public}d. isUIFirstEnabled=%{public}d",
            surfaceNode->GetName().c_str(), surfaceNode->GetId(), isForced, isUIFirstEnabled);
        surfaceNode->MarkUifirstNode(isForced, isUIFirstEnabled);
    }
}

WSError SceneSession::UpdateWindowAnimationFlag(bool needDefaultAnimationFlag)
{
    return PostSyncTask([weakThis = wptr(this), needDefaultAnimationFlag, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        session->needDefaultAnimationFlag_ = needDefaultAnimationFlag;
        if (session->onWindowAnimationFlagChange_) {
            session->onWindowAnimationFlagChange_(needDefaultAnimationFlag);
        }
        return WSError::WS_OK;
    }, __func__);
}

void SceneSession::SetWindowAnimationFlag(bool needDefaultAnimationFlag)
{
    needDefaultAnimationFlag_ = needDefaultAnimationFlag;
    if (onWindowAnimationFlagChange_) {
        onWindowAnimationFlagChange_(needDefaultAnimationFlag);
    }
    return;
}

bool SceneSession::IsNeedDefaultAnimation() const
{
    return needDefaultAnimationFlag_;
}

bool SceneSession::IsAppSession() const
{
    if (GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
        return true;
    }
    if (GetMainSession()) {
        return true;
    }
    return false;
}

/** @note @window.focus */
bool SceneSession::IsAppOrLowerSystemSession() const
{
    WindowType windowType = GetWindowType();
    if (windowType == WindowType::WINDOW_TYPE_NEGATIVE_SCREEN ||
        windowType == WindowType::WINDOW_TYPE_GLOBAL_SEARCH ||
        windowType == WindowType::WINDOW_TYPE_DESKTOP) {
        return true;
    }
    return IsAppSession();
}

/** @note @window.focus */
bool SceneSession::IsSystemSessionAboveApp() const
{
    WindowType windowType = GetWindowType();
    if (windowType == WindowType::WINDOW_TYPE_DIALOG || windowType == WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW) {
        return true;
    }
    if (windowType == WindowType::WINDOW_TYPE_PANEL &&
        sessionInfo_.bundleName_.find("SCBDropdownPanel") != std::string::npos) {
        return true;
    }
    if (windowType == WindowType::WINDOW_TYPE_FLOAT &&
        sessionInfo_.bundleName_.find("SCBGestureDock") != std::string::npos) {
        return true;
    }
    return false;
}

/** @note @window.focus */
bool SceneSession::IsSameMainSession(const sptr<SceneSession>& prevSession)
{
    if (prevSession == nullptr) {
        TLOGE(WmsLogTag::WMS_FOCUS, "prevSession is nullptr");
        return false;
    }
    int32_t currSessionId = GetMainSessionId();
    int32_t prevSessionId = prevSession->GetMainSessionId();
    return currSessionId == prevSessionId && prevSessionId != INVALID_SESSION_ID;
}

void SceneSession::NotifyIsCustomAnimationPlaying(bool isPlaying)
{
    WLOGFI("id %{public}d %{public}u", GetPersistentId(), isPlaying);
    if (onIsCustomAnimationPlaying_) {
        onIsCustomAnimationPlaying_(isPlaying);
    }
}

WSError SceneSession::UpdateWindowSceneAfterCustomAnimation(bool isAdd)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "failed to update with id:%{public}u!", GetPersistentId());
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    PostTask([weakThis = wptr(this), isAdd, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        TLOGNI(WmsLogTag::WMS_LAYOUT, "%{public}s id %{public}d, isAdd: %{public}d",
            where, session->GetPersistentId(), isAdd);
        if (isAdd) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s SetOpacityFunc not register %{public}d",
                where, session->GetPersistentId());
            return WSError::WS_ERROR_INVALID_OPERATION;
        } else {
            TLOGNI(WmsLogTag::WMS_LAYOUT, "%{public}s background after custom animation id %{public}d",
                where, session->GetPersistentId());
            // since background will remove surfaceNode
            session->Background();
            session->NotifyIsCustomAnimationPlaying(false);
        }
        return WSError::WS_OK;
    }, std::string(__func__) + ":" + std::to_string(isAdd));
    return WSError::WS_OK;
}

bool SceneSession::IsFloatingWindowAppType() const
{
    auto property = GetSessionProperty();
    if (property == nullptr) {
        return false;
    }
    return property->IsFloatingWindowAppType();
}

std::vector<Rect> SceneSession::GetTouchHotAreas() const
{
    std::vector<Rect> touchHotAreas;
    auto property = GetSessionProperty();
    if (property) {
        property->GetTouchHotAreas(touchHotAreas);
    }
    return touchHotAreas;
}

PiPTemplateInfo SceneSession::GetPiPTemplateInfo() const
{
    return pipTemplateInfo_;
}

void SceneSession::DumpSessionElementInfo(const std::vector<std::string>& params)
{
    if (!sessionStage_) {
        return;
    }
    return sessionStage_->DumpSessionElementInfo(params);
}

void SceneSession::NotifyTouchOutside()
{
    WLOGFI("id: %{public}d, type: %{public}d", GetPersistentId(), GetWindowType());
    if (sessionStage_) {
        WLOGFD("Notify sessionStage TouchOutside");
        sessionStage_->NotifyTouchOutside();
    }
    if (onTouchOutside_) {
        WLOGFD("Notify sessionChangeCallback TouchOutside");
        onTouchOutside_();
    }
}

void SceneSession::NotifyWindowVisibility()
{
    if (sessionStage_) {
        sessionStage_->NotifyWindowVisibility(GetRSVisible());
    } else {
        WLOGFE("Notify window(id:%{public}d) visibility failed, for this session stage is nullptr", GetPersistentId());
    }
}

bool SceneSession::CheckTouchOutsideCallbackRegistered()
{
    return onTouchOutside_ != nullptr;
}

void SceneSession::SetRequestedOrientation(Orientation orientation)
{
    WLOGFI("id: %{public}d orientation: %{public}u", GetPersistentId(), static_cast<uint32_t>(orientation));
    GetSessionProperty()->SetRequestedOrientation(orientation);
    if (onRequestedOrientationChange_) {
        onRequestedOrientationChange_(static_cast<uint32_t>(orientation));
    }
}

WSError SceneSession::SetDefaultRequestedOrientation(Orientation orientation)
{
    return PostSyncTask([weakThis = wptr(this), orientation, where = __func__]() -> WSError {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::DEFAULT, "%{public}s session is null", where);
            return WSError::WS_ERROR_NULLPTR;
        }
        TLOGNI(WmsLogTag::DEFAULT, "%{public}s id: %{public}d defaultRequestedOrientation: %{public}u",
            where, session->GetPersistentId(), static_cast<uint32_t>(orientation));
        auto property = session->GetSessionProperty();
        property->SetRequestedOrientation(orientation);
        property->SetDefaultRequestedOrientation(orientation);
        return WSError::WS_OK;
    }, __func__);
}

void SceneSession::NotifyForceHideChange(bool hide)
{
    WLOGFI("id: %{public}d forceHide: %{public}u", persistentId_, hide);
    auto property = GetSessionProperty();
    if (property == nullptr) {
        WLOGFD("id: %{public}d property is nullptr", persistentId_);
        return;
    }
    property->SetForceHide(hide);
    if (onForceHideChangeFunc_) {
        onForceHideChangeFunc_(hide);
    }
    SetForceTouchable(!hide);
    if (hide) {
        if (isFocused_) {
            FocusChangeReason reason = FocusChangeReason::DEFAULT;
            NotifyRequestFocusStatusNotifyManager(false, true, reason);
            SetForceHideState(ForceHideState::HIDDEN_WHEN_FOCUSED);
        } else if (forceHideState_ == ForceHideState::NOT_HIDDEN) {
            SetForceHideState(ForceHideState::HIDDEN_WHEN_UNFOCUSED);
        }
    } else {
        if (forceHideState_ == ForceHideState::HIDDEN_WHEN_FOCUSED) {
            SetForceHideState(ForceHideState::NOT_HIDDEN);
            FocusChangeReason reason = FocusChangeReason::DEFAULT;
            NotifyRequestFocusStatusNotifyManager(true, true, reason);
        } else {
            SetForceHideState(ForceHideState::NOT_HIDDEN);
        }
    }
}

Orientation SceneSession::GetRequestedOrientation() const
{
    return GetSessionProperty()->GetRequestedOrientation();
}

bool SceneSession::IsAnco() const
{
    return collaboratorType_ == static_cast<int32_t>(CollaboratorType::RESERVE_TYPE);
}

void SceneSession::SetBlank(bool isAddBlank)
{
    isAddBlank_ = isAddBlank;
}

bool SceneSession::GetBlank() const
{
    return isAddBlank_;
}

void SceneSession::SetBufferAvailableCallbackEnable(bool enable)
{
    bufferAvailableCallbackEnable_ = enable;
}

bool SceneSession::GetBufferAvailableCallbackEnable() const
{
    return bufferAvailableCallbackEnable_;
}

int32_t SceneSession::GetCollaboratorType() const
{
    return collaboratorType_;
}

void SceneSession::SetCollaboratorType(int32_t collaboratorType)
{
    collaboratorType_ = collaboratorType;
    sessionInfo_.collaboratorType_ = collaboratorType;
}

std::string SceneSession::GetClientIdentityToken() const
{
    return clientIdentityToken_;
}

void SceneSession::SetClientIdentityToken(const std::string& clientIdentityToken)
{
    clientIdentityToken_ = clientIdentityToken;
}

void SceneSession::DumpSessionInfo(std::vector<std::string>& info) const
{
    std::string dumpInfo = "      Session ID #" + std::to_string(persistentId_);
    info.push_back(dumpInfo);
    dumpInfo = "        session name [" + SessionUtils::ConvertSessionName(sessionInfo_.bundleName_,
        sessionInfo_.abilityName_, sessionInfo_.moduleName_, sessionInfo_.appIndex_) + "]";
    info.push_back(dumpInfo);
    dumpInfo = "        runningState [" + std::string(isActive_ ? "FOREGROUND" : "BACKGROUND") + "]";
    info.push_back(dumpInfo);
    dumpInfo = "        lockedState [" + std::to_string(sessionInfo_.lockedState) + "]";
    info.push_back(dumpInfo);
    auto abilityInfo = sessionInfo_.abilityInfo;
    dumpInfo = "        continuable [" + (abilityInfo ? std::to_string(abilityInfo->continuable) : " ") + "]";
    info.push_back(dumpInfo);
    dumpInfo = "        timeStamp [" + sessionInfo_.time + "]";
    info.push_back(dumpInfo);
    dumpInfo = "        label [" + (abilityInfo ? abilityInfo->label : " ") + "]";
    info.push_back(dumpInfo);
    dumpInfo = "        iconPath [" + (abilityInfo ? abilityInfo->iconPath : " ") + "]";
    info.push_back(dumpInfo);
    dumpInfo = "        want [" + (sessionInfo_.want ? sessionInfo_.want->ToUri() : " ") + "]";
    info.push_back(dumpInfo);
}

std::shared_ptr<AppExecFwk::AbilityInfo> SceneSession::GetAbilityInfo() const
{
    return GetSessionInfo().abilityInfo;
}

void SceneSession::SetAbilitySessionInfo(std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo)
{
    SetSessionInfoAbilityInfo(abilityInfo);
}

void SceneSession::SetSessionState(SessionState state)
{
    Session::SetSessionState(state);
    NotifyAccessibilityVisibilityChange();
}

void SceneSession::UpdateSessionState(SessionState state)
{
    Session::UpdateSessionState(state);
    NotifyAccessibilityVisibilityChange();
}

bool SceneSession::IsVisibleForAccessibility() const
{
    if (Session::IsScbCoreEnabled()) {
        return GetSystemTouchable() && GetForegroundInteractiveStatus() && IsVisibleForeground();
    }
    return GetSystemTouchable() && GetForegroundInteractiveStatus() &&
        (IsVisible() || state_ == SessionState::STATE_ACTIVE || state_ == SessionState::STATE_FOREGROUND);
}

void SceneSession::SetForegroundInteractiveStatus(bool interactive)
{
    Session::SetForegroundInteractiveStatus(interactive);
    NotifyAccessibilityVisibilityChange();
    if (interactive) {
        return;
    }
    for (auto toastSession : toastSession_) {
        if (toastSession == nullptr) {
            TLOGD(WmsLogTag::WMS_TOAST, "toastSession session is nullptr");
            continue;
        }
        auto state = toastSession->GetSessionState();
        if (state != SessionState::STATE_FOREGROUND && state != SessionState::STATE_ACTIVE) {
            continue;
        }
        toastSession->SetActive(false);
        toastSession->BackgroundTask();
    }
}

void SceneSession::NotifyAccessibilityVisibilityChange()
{
    bool isVisibleForAccessibilityNew = IsVisibleForAccessibility();
    if (isVisibleForAccessibilityNew == isVisibleForAccessibility_.load()) {
        return;
    }
    WLOGFD("[WMSAccess] NotifyAccessibilityVisibilityChange id: %{public}d, access: %{public}d ",
        GetPersistentId(), isVisibleForAccessibilityNew);
    isVisibleForAccessibility_.store(isVisibleForAccessibilityNew);
    if (specificCallback_ && specificCallback_->onWindowInfoUpdate_) {
        if (isVisibleForAccessibilityNew) {
            specificCallback_->onWindowInfoUpdate_(GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_ADDED);
        } else {
            specificCallback_->onWindowInfoUpdate_(GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_REMOVED);
        }
    } else {
        WLOGFD("specificCallback_->onWindowInfoUpdate_ not exist, persistent id: %{public}d", GetPersistentId());
    }
}

void SceneSession::SetSystemTouchable(bool touchable)
{
    Session::SetSystemTouchable(touchable);
    NotifyAccessibilityVisibilityChange();
}

WSError SceneSession::ChangeSessionVisibilityWithStatusBar(
    const sptr<AAFwk::SessionInfo> abilitySessionInfo, bool visible)
{
    if (!SessionPermission::VerifyCallingPermission(PermissionConstants::PERMISSION_MANAGE_MISSION)) {
        TLOGE(WmsLogTag::WMS_LIFE, "The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    PostTask([weakThis = wptr(this), abilitySessionInfo, visible, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (abilitySessionInfo == nullptr) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s abilitySessionInfo is null", where);
            return WSError::WS_ERROR_NULLPTR;
        }

        SessionInfo info;
        info.abilityName_ = abilitySessionInfo->want.GetElement().GetAbilityName();
        info.bundleName_ = abilitySessionInfo->want.GetElement().GetBundleName();
        info.moduleName_ = abilitySessionInfo->want.GetModuleName();
        int32_t appCloneIndex = abilitySessionInfo->want.GetIntParam(APP_CLONE_INDEX, 0);
        info.appIndex_ = appCloneIndex == 0 ? abilitySessionInfo->want.GetIntParam(DLP_INDEX, 0) : appCloneIndex;
        info.persistentId_ = abilitySessionInfo->persistentId;
        info.callerPersistentId_ = session->GetPersistentId();
        info.callerBundleName_ = abilitySessionInfo->want.GetStringParam(AAFwk::Want::PARAM_RESV_CALLER_BUNDLE_NAME);
        info.callerAbilityName_ = abilitySessionInfo->want.GetStringParam(AAFwk::Want::PARAM_RESV_CALLER_ABILITY_NAME);
        info.callState_ = static_cast<uint32_t>(abilitySessionInfo->state);
        info.uiAbilityId_ = abilitySessionInfo->uiAbilityId;
        info.requestId = abilitySessionInfo->requestId;
        info.want = std::make_shared<AAFwk::Want>(abilitySessionInfo->want);
        info.requestCode = abilitySessionInfo->requestCode;
        info.callerToken_ = abilitySessionInfo->callerToken;
        info.startSetting = abilitySessionInfo->startSetting;
        info.callingTokenId_ = abilitySessionInfo->callingTokenId;
        info.reuse = abilitySessionInfo->reuse;
        info.processOptions = abilitySessionInfo->processOptions;

        if (session->changeSessionVisibilityWithStatusBarFunc_) {
            session->changeSessionVisibilityWithStatusBarFunc_(info, visible);
        }

        return WSError::WS_OK;
    }, __func__);
    return WSError::WS_OK;
}

static SessionInfo MakeSessionInfoDuringPendingActivation(const sptr<AAFwk::SessionInfo>& abilitySessionInfo,
    const sptr<SceneSession>& session, bool isFoundationCall)
{
    SessionInfo info;
    info.abilityName_ = abilitySessionInfo->want.GetElement().GetAbilityName();
    info.bundleName_ = abilitySessionInfo->want.GetElement().GetBundleName();
    info.moduleName_ = abilitySessionInfo->want.GetModuleName();
    int32_t appCloneIndex = abilitySessionInfo->want.GetIntParam(APP_CLONE_INDEX, 0);
    info.appIndex_ = appCloneIndex == 0 ? abilitySessionInfo->want.GetIntParam(DLP_INDEX, 0) : appCloneIndex;
    info.persistentId_ = abilitySessionInfo->persistentId;
    info.callerPersistentId_ = session->GetPersistentId();
    info.callerBundleName_ = abilitySessionInfo->want.GetStringParam(AAFwk::Want::PARAM_RESV_CALLER_BUNDLE_NAME);
    info.callerAbilityName_ = abilitySessionInfo->want.GetStringParam(AAFwk::Want::PARAM_RESV_CALLER_ABILITY_NAME);
    info.callState_ = static_cast<uint32_t>(abilitySessionInfo->state);
    info.uiAbilityId_ = abilitySessionInfo->uiAbilityId;
    info.requestId = abilitySessionInfo->requestId;
    info.want = std::make_shared<AAFwk::Want>(abilitySessionInfo->want);
    info.requestCode = abilitySessionInfo->requestCode;
    info.callerToken_ = abilitySessionInfo->callerToken;
    info.startSetting = abilitySessionInfo->startSetting;
    info.callingTokenId_ = abilitySessionInfo->callingTokenId;
    info.reuse = abilitySessionInfo->reuse;
    info.processOptions = abilitySessionInfo->processOptions;
    info.isAtomicService_ = abilitySessionInfo->isAtomicService;
    info.isBackTransition_ = abilitySessionInfo->isBackTransition;
    info.needClearInNotShowRecent_ = abilitySessionInfo->needClearInNotShowRecent;
    info.appInstanceKey_ = abilitySessionInfo->instanceKey;
    info.isFromIcon_ = abilitySessionInfo->isFromIcon;
    info.isPcOrPadEnableActivation_ = session->IsPcOrPadEnableActivation();
    info.canStartAbilityFromBackground_ = abilitySessionInfo->canStartAbilityFromBackground;
    info.isFoundationCall_ = isFoundationCall;
    info.specifiedFlag_ = abilitySessionInfo->specifiedFlag;
    if (session->IsPcOrPadEnableActivation()) {
        info.startWindowOption = abilitySessionInfo->startWindowOption;
        int32_t maxWindowWidth = abilitySessionInfo->want.GetIntParam(AAFwk::Want::PARAM_RESV_MAX_WINDOW_WIDTH, 0);
        info.windowSizeLimits.maxWindowWidth = static_cast<std::uint32_t>(maxWindowWidth > 0 ? maxWindowWidth : 0);
        int32_t minWindowWidth = abilitySessionInfo->want.GetIntParam(AAFwk::Want::PARAM_RESV_MIN_WINDOW_WIDTH, 0);
        info.windowSizeLimits.minWindowWidth = static_cast<std::uint32_t>(minWindowWidth > 0 ? minWindowWidth : 0);
        int32_t maxWindowHeight = abilitySessionInfo->want.GetIntParam(AAFwk::Want::PARAM_RESV_MAX_WINDOW_HEIGHT, 0);
        info.windowSizeLimits.maxWindowHeight = static_cast<std::uint32_t>(maxWindowHeight > 0 ? maxWindowHeight : 0);
        int32_t minWindowHeight = abilitySessionInfo->want.GetIntParam(AAFwk::Want::PARAM_RESV_MIN_WINDOW_HEIGHT, 0);
        info.windowSizeLimits.minWindowHeight = static_cast<std::uint32_t>(minWindowHeight > 0 ? minWindowHeight : 0);
        if (!abilitySessionInfo->supportWindowModes.empty()) {
            info.supportedWindowModes.assign(abilitySessionInfo->supportWindowModes.begin(),
                abilitySessionInfo->supportWindowModes.end());
        }
    }
    if (info.want != nullptr) {
        info.windowMode = info.want->GetIntParam(AAFwk::Want::PARAM_RESV_WINDOW_MODE, 0);
        info.sessionAffinity = info.want->GetStringParam(Rosen::PARAM_KEY::PARAM_MISSION_AFFINITY_KEY);
        info.screenId_ = static_cast<uint64_t>(info.want->GetIntParam(AAFwk::Want::PARAM_RESV_DISPLAY_ID, -1));
        TLOGI(WmsLogTag::WMS_LIFE, "want: screenId %{public}" PRIu64, info.screenId_);
    }
    if (info.windowMode == static_cast<int32_t>(WindowMode::WINDOW_MODE_FULLSCREEN)) {
        info.fullScreenStart_ = true;
    }
    TLOGI(WmsLogTag::WMS_LIFE, "bundleName:%{public}s, moduleName:%{public}s, abilityName:%{public}s, "
        "appIndex:%{public}d, affinity:%{public}s. callState:%{public}d, want persistentId:%{public}d, "
        "uiAbilityId:%{public}" PRIu64 ", windowMode:%{public}d, callerId:%{public}d, "
        "needClearInNotShowRecent:%{public}u, appInstanceKey: %{public}s, isFromIcon:%{public}d, "
        "supportedWindowModes.size:%{public}zu, requestId:%{public}d, "
        "maxWindowWidth:%{public}d, minWindowWidth:%{public}d, maxWindowHeight:%{public}d, minWindowHeight:%{public}d",
        info.bundleName_.c_str(), info.moduleName_.c_str(), info.abilityName_.c_str(), info.appIndex_,
        info.sessionAffinity.c_str(), info.callState_, info.persistentId_, info.uiAbilityId_, info.windowMode,
        info.callerPersistentId_, info.needClearInNotShowRecent_, info.appInstanceKey_.c_str(), info.isFromIcon_,
        info.supportedWindowModes.size(), info.requestId,
        info.windowSizeLimits.maxWindowWidth, info.windowSizeLimits.minWindowWidth,
        info.windowSizeLimits.maxWindowHeight, info.windowSizeLimits.minWindowHeight);
    return info;
}

WSError SceneSession::PendingSessionActivation(const sptr<AAFwk::SessionInfo> abilitySessionInfo)
{
    if (!SessionPermission::VerifyCallingPermission(PermissionConstants::PERMISSION_MANAGE_MISSION)) {
        TLOGE(WmsLogTag::WMS_LIFE, "The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    bool isFoundationCall = SessionPermission::IsFoundationCall();
    PostTask([weakThis = wptr(this), abilitySessionInfo, isFoundationCall, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (abilitySessionInfo == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s abilitySessionInfo is null", where);
            return WSError::WS_ERROR_NULLPTR;
        }
        bool isFromAncoAndToAnco = session->IsAnco() && AbilityInfoManager::GetInstance().IsAnco(
            abilitySessionInfo->want.GetElement().GetBundleName(),
            abilitySessionInfo->want.GetElement().GetAbilityName(), abilitySessionInfo->want.GetModuleName());
        if (session->DisallowActivationFromPendingBackground(session->IsPcOrPadEnableActivation(), isFoundationCall,
            abilitySessionInfo->canStartAbilityFromBackground, isFromAncoAndToAnco)) {
            return WSError::WS_ERROR_INVALID_OPERATION;
        }
        session->sessionInfo_.startMethod = StartMethod::START_CALL;
        SessionInfo info = MakeSessionInfoDuringPendingActivation(abilitySessionInfo, session, isFoundationCall);
        if (MultiInstanceManager::IsSupportMultiInstance(session->systemConfig_) &&
            MultiInstanceManager::GetInstance().IsMultiInstance(session->GetSessionInfo().bundleName_)) {
            if (!MultiInstanceManager::GetInstance().MultiInstancePendingSessionActivation(info)) {
                TLOGNE(WmsLogTag::WMS_LIFE,
                    "%{public}s multi instance start fail, id:%{public}d instanceKey:%{public}s",
                    where, session->GetPersistentId(), info.appInstanceKey_.c_str());
                return WSError::WS_ERROR_INVALID_PARAM;
            }
        }
        session->HandleCastScreenConnection(info, session);
        if (session->pendingSessionActivationFunc_) {
            session->pendingSessionActivationFunc_(info);
        }
        return WSError::WS_OK;
    }, __func__);
    return WSError::WS_OK;
}

bool SceneSession::DisallowActivationFromPendingBackground(bool isPcOrPadEnableActivation, bool isFoundationCall,
    bool canStartAbilityFromBackground, bool isFromAncoAndToAnco)
{
    if (isPcOrPadEnableActivation || !WindowHelper::IsMainWindow(GetWindowType())) {
        return false;
    }
    bool isPendingToBackgroundState = GetIsPendingToBackgroundState();
    bool foregroundInteractiveStatus = GetForegroundInteractiveStatus();
    TLOGI(WmsLogTag::WMS_LIFE, "session state:%{public}d, isFoundationCall:%{public}u, "
        "canStartAbilityFromBackground:%{public}u, foregroundInteractiveStatus:%{public}u, "
        "isPendingToBackgroundState:%{public}u, isFromAncoAndToAnco:%{public}u",
        GetSessionState(), isFoundationCall, canStartAbilityFromBackground, foregroundInteractiveStatus,
        isPendingToBackgroundState, isFromAncoAndToAnco);
    bool isSessionForeground = GetSessionState() == SessionState::STATE_FOREGROUND ||
        GetSessionState() == SessionState::STATE_ACTIVE;
    if (isSessionForeground) {
        if (isPendingToBackgroundState) {
            if (!(isFoundationCall && canStartAbilityFromBackground) && !isFromAncoAndToAnco) {
                TLOGW(WmsLogTag::WMS_LIFE, "no permission to start ability from PendingBackground, id:%{public}d",
                    GetPersistentId());
                return true;
            }
        } else if (!foregroundInteractiveStatus) {
            TLOGW(WmsLogTag::WMS_LIFE, "start ability invalid, session in a non interactive state");
            return true;
        }
    } else if (!(isFoundationCall && canStartAbilityFromBackground) && !isFromAncoAndToAnco) {
        TLOGW(WmsLogTag::WMS_LIFE, "no permission to start ability from Background, id:%{public}d",
            GetPersistentId());
        return true;
    }
    return false;
}

void SceneSession::HandleCastScreenConnection(SessionInfo& info, sptr<SceneSession> session)
{
    ScreenId defScreenId = ScreenSessionManagerClient::GetInstance().GetDefaultScreenId();
    if (defScreenId == info.screenId_) {
        return;
    }
    auto flag = Rosen::ScreenManager::GetInstance().GetVirtualScreenFlag(info.screenId_);
    if (flag != VirtualScreenFlag::CAST) {
        return;
    }
    TLOGI(WmsLogTag::WMS_LIFE, "Get exist session state :%{public}d persistentId:%{public}d",
        session->GetSessionState(), info.callerPersistentId_);
    if (session->GetSessionState() != SessionState::STATE_FOREGROUND &&
        session->GetSessionState() != SessionState::STATE_ACTIVE) {
        TLOGI(WmsLogTag::WMS_LIFE, "Get exist session state is not foreground");
        return;
    }
    info.isCastSession_ = true;
    std::vector<uint64_t> mirrorIds { info.screenId_ };
    Rosen::DMError ret = Rosen::ScreenManager::GetInstance().MakeUniqueScreen(mirrorIds);
    if (ret != Rosen::DMError::DM_OK) {
        TLOGE(WmsLogTag::WMS_LIFE, "MakeUniqueScreen failed,ret: %{public}d", ret);
        return;
    }
}

static bool IsNeedSystemPermissionByAction(WSPropertyChangeAction action,
    const sptr<WindowSessionProperty>& property, const sptr<WindowSessionProperty>& sessionProperty)
{
    switch (action) {
        case WSPropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON:
        case WSPropertyChangeAction::ACTION_UPDATE_SNAPSHOT_SKIP:
        case WSPropertyChangeAction::ACTION_UPDATE_HIDE_NON_SYSTEM_FLOATING_WINDOWS:
        case WSPropertyChangeAction::ACTION_UPDATE_TOPMOST:
        case WSPropertyChangeAction::ACTION_UPDATE_DECOR_ENABLE:
        case WSPropertyChangeAction::ACTION_UPDATE_MODE_SUPPORT_INFO:
            return true;
        case WSPropertyChangeAction::ACTION_UPDATE_ANIMATION_FLAG:
            return property->GetAnimationFlag() == static_cast<uint32_t>(WindowAnimation::CUSTOM);
        case WSPropertyChangeAction::ACTION_UPDATE_FLAGS: {
            uint32_t oldFlags = sessionProperty->GetWindowFlags();
            uint32_t flags = property->GetWindowFlags();
            if ((oldFlags ^ flags) == static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK)) {
                return true;
            }
            break;
        }
        default:
            break;
    }
    return false;
}

WMError SceneSession::UpdateSessionPropertyByAction(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    if (property == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "property is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "get session property failed");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (action == WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE) {
        if (!SessionPermission::VerifyCallingPermission("ohos.permission.PRIVACY_WINDOW")) {
            return WMError::WM_ERROR_INVALID_PERMISSION;
        }
    }
    if (action == WSPropertyChangeAction::ACTION_UPDATE_MAIN_WINDOW_TOPMOST) {
        uint32_t accessTokenId = property->GetAccessTokenId();
        if (!SessionPermission::VerifyPermissionByCallerToken(accessTokenId,
            PermissionConstants::PERMISSION_MAIN_WINDOW_TOPMOST)) {
            TLOGE(WmsLogTag::WMS_HIERARCHY, "The caller has no permission granted.");
            return WMError::WM_ERROR_INVALID_PERMISSION;
        }
    }

    bool isSystemCalling = SessionPermission::IsSystemCalling() || SessionPermission::IsStartByHdcd();
    if (!isSystemCalling && IsNeedSystemPermissionByAction(action, property, sessionProperty)) {
        TLOGE(WmsLogTag::DEFAULT, "permission denied! action: %{public}" PRIu64, action);
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    property->SetSystemCalling(isSystemCalling);
    auto task = [weak = wptr(this), property, action, where = __func__]() -> WMError {
        auto sceneSession = weak.promote();
        if (sceneSession == nullptr) {
            TLOGNE(WmsLogTag::DEFAULT, "%{public}s the session is nullptr", where);
            return WMError::WM_DO_NOTHING;
        }
        TLOGND(WmsLogTag::DEFAULT, "%{public}s Id: %{public}d, action: %{public}" PRIu64,
            where, sceneSession->GetPersistentId(), action);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSession:UpdateProperty");
        return sceneSession->HandleUpdatePropertyByAction(property, action);
    };
    if (AppExecFwk::EventRunner::IsAppMainThread()) {
        PostTask(std::move(task), __func__);
        return WMError::WM_OK;
    }
    return PostSyncTask(std::move(task), __func__);
}

WMError SceneSession::SetGestureBackEnabled(bool isEnabled)
{
    PostTask([weakThis = wptr(this), isEnabled, where = __func__] {
        auto sceneSession = weakThis.promote();
        if (!sceneSession) {
            TLOGNE(WmsLogTag::WMS_IMMS, "%{public}s session is invalid", where);
            return;
        }
        if (sceneSession->isEnableGestureBack_ == isEnabled) {
            TLOGND(WmsLogTag::WMS_IMMS, "%{public}s isEnabled equals last", where);
            return;
        }
        TLOGNI(WmsLogTag::WMS_IMMS, "%{public}s win %{public}d isEnabled %{public}d",
            where, sceneSession->GetPersistentId(), isEnabled);
        sceneSession->isEnableGestureBack_ = isEnabled;
        sceneSession->isEnableGestureBackHadSet_ = true;
        sceneSession->UpdateGestureBackEnabled();
    }, __func__);
    return WMError::WM_OK;
}

bool SceneSession::GetGestureBackEnabled()
{
    return isEnableGestureBack_;
}

bool SceneSession::GetEnableGestureBackHadSet()
{
    return isEnableGestureBackHadSet_;
}

void SceneSession::UpdateFullScreenWaterfallMode(bool isWaterfallMode)
{
    PostTask([weakThis = wptr(this), isWaterfallMode, where = __func__] {
        auto session = weakThis.promote();
        if (session == nullptr) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s session is nullptr", where);
            return;
        }
        if (session->pcFoldScreenController_ == nullptr) {
            return;
        }
        session->pcFoldScreenController_->UpdateFullScreenWaterfallMode(isWaterfallMode);
    }, __func__);
}

bool SceneSession::IsFullScreenWaterfallMode()
{
    if (pcFoldScreenController_ == nullptr) {
        return false;
    }
    return pcFoldScreenController_->IsFullScreenWaterfallMode();
}

void SceneSession::RegisterFullScreenWaterfallModeChangeCallback(std::function<void(bool isWaterfallMode)>&& func)
{
    PostTask([weakThis = wptr(this), func = std::move(func), where = __func__]() mutable {
        auto session = weakThis.promote();
        if (session == nullptr) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s session is nullptr", where);
            return;
        }
        if (session->pcFoldScreenController_ == nullptr) {
            return;
        }
        session->pcFoldScreenController_->RegisterFullScreenWaterfallModeChangeCallback(std::move(func));
    }, __func__);
}

void SceneSession::RegisterThrowSlipAnimationStateChangeCallback(std::function<void(bool isAnimating)>&& func)
{
    PostTask([weakThis = wptr(this), func = std::move(func), where = __func__]() mutable {
        auto session = weakThis.promote();
        if (session == nullptr) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s session is nullptr", where);
            return;
        }
        session->onThrowSlipAnimationStateChangeFunc_ = std::move(func);
    }, __func__);
}

bool SceneSession::IsMissionHighlighted()
{
    if (!SessionHelper::IsMainWindow(GetWindowType())) {
        return false;
    }
    if (IsFocused()) {
        return true;
    }
    return std::any_of(subSession_.begin(), subSession_.end(),
        [](const sptr<SceneSession>& sceneSession) {
            return sceneSession != nullptr && sceneSession->IsMissionHighlighted();
        });
}

void SceneSession::MaskSupportEnterWaterfallMode()
{
    if (pcFoldScreenController_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "pcFoldScreenController is null");
        return;
    }
    pcFoldScreenController_->MaskSupportEnterWaterfallMode();
}

void SceneSession::SetSupportEnterWaterfallMode(bool isSupportEnter)
{
    if (!sessionStage_) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "sessionStage is nullptr");
        return;
    }
    sessionStage_->SetSupportEnterWaterfallMode(isSupportEnter);
}

void SceneSession::SetSessionChangeByActionNotifyManagerListener(const SessionChangeByActionNotifyManagerFunc& func)
{
    TLOGD(WmsLogTag::DEFAULT, "setListener success");
    sessionChangeByActionNotifyManagerFunc_ = func;
}

WMError SceneSession::HandleUpdatePropertyByAction(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    if (property == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "property is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }

    return ProcessUpdatePropertyByAction(property, action);
}

WMError SceneSession::ProcessUpdatePropertyByAction(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    switch (static_cast<uint64_t>(action)) {
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON):
            return HandleActionUpdateTurnScreenOn(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON):
            return HandleActionUpdateKeepScreenOn(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_VIEW_KEEP_SCREEN_ON):
            return HandleActionUpdateViewKeepScreenOn(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_FOCUSABLE):
            return HandleActionUpdateFocusable(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_TOUCHABLE):
            return HandleActionUpdateTouchable(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS):
            return HandleActionUpdateSetBrightness(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_ORIENTATION):
            return HandleActionUpdateOrientation(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE):
            return HandleActionUpdatePrivacyMode(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_SYSTEM_PRIVACY_MODE):
            return HandleActionUpdatePrivacyMode(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_SNAPSHOT_SKIP):
            return HandleActionUpdateSnapshotSkip(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_MAXIMIZE_STATE):
            return HandleActionUpdateMaximizeState(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_OTHER_PROPS):
            return HandleActionUpdateOtherProps(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_STATUS_PROPS):
            return HandleActionUpdateStatusProps(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_NAVIGATION_PROPS):
            return HandleActionUpdateNavigationProps(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_NAVIGATION_INDICATOR_PROPS):
            return HandleActionUpdateNavigationIndicatorProps(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_FLAGS):
            return HandleActionUpdateFlags(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_MODE):
            return HandleActionUpdateMode(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_ANIMATION_FLAG):
            return HandleActionUpdateAnimationFlag(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_TOUCH_HOT_AREA):
            return HandleActionUpdateTouchHotArea(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_KEYBOARD_TOUCH_HOT_AREA):
            return HandleActionUpdateKeyboardTouchHotArea(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_DECOR_ENABLE):
            return HandleActionUpdateDecorEnable(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_WINDOW_LIMITS):
            return HandleActionUpdateWindowLimits(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_DRAGENABLED):
            return HandleActionUpdateDragenabled(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_RAISEENABLED):
            return HandleActionUpdateRaiseenabled(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_HIDE_NON_SYSTEM_FLOATING_WINDOWS):
            return HandleActionUpdateHideNonSystemFloatingWindows(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_TEXTFIELD_AVOID_INFO):
            return HandleActionUpdateTextfieldAvoidInfo(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_WINDOW_MASK):
            return HandleActionUpdateWindowMask(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_TOPMOST):
            return HandleActionUpdateTopmost(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_MAIN_WINDOW_TOPMOST):
            return HandleActionUpdateMainWindowTopmost(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_SUB_WINDOW_Z_LEVEL):
            return HandleActionUpdateSubWindowZLevel(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_MODE_SUPPORT_INFO):
            return HandleActionUpdateWindowModeSupportType(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_AVOID_AREA_OPTION):
            return HandleActionUpdateAvoidAreaOption(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_BACKGROUND_ALPHA):
            return HandleBackgroundAlpha(property, action);
        case static_cast<uint64_t>(WSPropertyChangeAction::ACTION_UPDATE_EXCLUSIVE_HIGHLIGHTED):
            return HandleActionUpdateExclusivelyHighlighted(property, action);
        default:
            TLOGE(WmsLogTag::DEFAULT, "Failed to find func handler!");
            return WMError::WM_DO_NOTHING;
    }
}

WMError SceneSession::HandleActionUpdateTurnScreenOn(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    SetTurnScreenOn(property->IsTurnScreenOn());
#ifdef POWER_MANAGER_ENABLE
    PostTask([weakThis = wptr(this), where = __func__] {
        auto sceneSession = weakThis.promote();
        if (!sceneSession) {
            TLOGNE(WmsLogTag::WMS_ATTRIBUTE, "%{public}s session is invalid", where);
            return;
        }
        TLOGND(WmsLogTag::WMS_ATTRIBUTE, "%{public}s Win: %{public}s, is turn on: %{public}d",
            where, sceneSession->GetWindowName().c_str(), sceneSession->IsTurnScreenOn());
        std::string identity = IPCSkeleton::ResetCallingIdentity();
        if (sceneSession->IsTurnScreenOn()) {
            TLOGNI(WmsLogTag::WMS_ATTRIBUTE, "%{public}s turn screen on", where);
            PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
        }
        // set ipc identity to raw
        IPCSkeleton::SetCallingIdentity(identity);
    }, __func__);
#else
    TLOGD(WmsLogTag::WMS_ATTRIBUTE, "Can not found the sub system of PowerMgr");
#endif
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateKeepScreenOn(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    SetKeepScreenOn(property->IsKeepScreenOn());
    NotifySessionChangeByActionNotifyManager(property, action);
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateViewKeepScreenOn(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "id: %{public}d, enabled: %{public}u",
        GetPersistentId(), property->IsViewKeepScreenOn());
    SetViewKeepScreenOn(property->IsViewKeepScreenOn());
    NotifySessionChangeByActionNotifyManager(property, action);
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateFocusable(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    SetFocusable(property->GetFocusable());
    NotifySessionChangeByActionNotifyManager(property, action);
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateTouchable(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    SetTouchable(property->GetTouchable());
    NotifySessionChangeByActionNotifyManager(property, action);
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateSetBrightness(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    if (GetWindowType() != WindowType::WINDOW_TYPE_APP_MAIN_WINDOW &&
        GetWindowType() != WindowType::WINDOW_TYPE_WALLET_SWIPE_CARD) {
        TLOGW(WmsLogTag::WMS_ATTRIBUTE, "only app main window or wallet swipe card can set brightness");
        return WMError::WM_OK;
    }
    if (!IsSessionValid()) {
        TLOGW(WmsLogTag::WMS_MAIN, "Session is invalid, id: %{public}d state: %{public}u",
            GetPersistentId(), GetSessionState());
        return WMError::WM_ERROR_INVALID_SESSION;
    }
    float brightness = property->GetBrightness();
    if (std::abs(brightness - GetBrightness()) < std::numeric_limits<float>::epsilon()) {
        TLOGD(WmsLogTag::WMS_ATTRIBUTE, "Session brightness do not change: [%{public}f]", brightness);
        return WMError::WM_OK;
    }
    SetBrightness(brightness);
    NotifySessionChangeByActionNotifyManager(property, action);
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateOrientation(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    SetRequestedOrientation(property->GetRequestedOrientation());
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdatePrivacyMode(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    bool isPrivacyMode = property->GetPrivacyMode() || property->GetSystemPrivacyMode();
    SetPrivacyMode(isPrivacyMode);
    NotifySessionChangeByActionNotifyManager(property, action);
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateSnapshotSkip(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    return SetSnapshotSkip(property->GetSnapshotSkip());
}

WMError SceneSession::HandleActionUpdateMaximizeState(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty != nullptr) {
        sessionProperty->SetMaximizeMode(property->GetMaximizeMode());
        sessionProperty->SetIsLayoutFullScreen(property->IsLayoutFullScreen());
    }
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateOtherProps(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    auto systemBarProperties = property->GetSystemBarProperty();
    for (auto iter : systemBarProperties) {
        SetSystemBarProperty(iter.first, iter.second);
    }
    NotifySessionChangeByActionNotifyManager(property, action);
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateStatusProps(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    HandleSpecificSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property);
    NotifySessionChangeByActionNotifyManager(property, action);
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateNavigationProps(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    HandleSpecificSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_BAR, property);
    NotifySessionChangeByActionNotifyManager(property, action);
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateNavigationIndicatorProps(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    HandleSpecificSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR, property);
    NotifySessionChangeByActionNotifyManager(property, action);
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateFlags(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    SetWindowFlags(property);
    NotifySessionChangeByActionNotifyManager(property, action);
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateMode(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty != nullptr) {
        sessionProperty->SetWindowMode(property->GetWindowMode());
    }
    NotifySessionChangeByActionNotifyManager(property, action);
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateAnimationFlag(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty != nullptr) {
        sessionProperty->SetAnimationFlag(property->GetAnimationFlag());
    }
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateTouchHotArea(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    std::vector<Rect> touchHotAreas;
    property->GetTouchHotAreas(touchHotAreas);
    GetSessionProperty()->SetTouchHotAreas(touchHotAreas);
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateKeyboardTouchHotArea(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    if (GetWindowType() != WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        return WMError::WM_ERROR_INVALID_TYPE;
    }
    GetSessionProperty()->SetKeyboardTouchHotAreas(property->GetKeyboardTouchHotAreas());
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateDecorEnable(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    if (property != nullptr && !property->GetSystemCalling()) {
        TLOGE(WmsLogTag::DEFAULT, "update decor enable permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty != nullptr) {
        sessionProperty->SetDecorEnable(property->IsDecorEnable());
    }
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateWindowLimits(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty != nullptr) {
        sessionProperty->SetWindowLimits(property->GetWindowLimits());
        WindowLimits windowLimits = sessionProperty->GetWindowLimits();
        TLOGI(WmsLogTag::WMS_LAYOUT, "UpdateWindowLimits minWidth:%{public}u, minHeight:%{public}u, "
            "maxWidth:%{public}u, maxHeight:%{public}u, vpRatio:%{public}f", windowLimits.minWidth_,
            windowLimits.minHeight_, windowLimits.maxWidth_, windowLimits.maxHeight_, windowLimits.vpRatio_);
    }
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateDragenabled(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty != nullptr) {
        sessionProperty->SetDragEnabled(property->GetDragEnabled());
    }
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateRaiseenabled(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty != nullptr) {
        TLOGI(WmsLogTag::WMS_HIERARCHY, "id: %{public}d, raise enabled: %{public}d", GetPersistentId(),
            property->GetRaiseEnabled());
        sessionProperty->SetRaiseEnabled(property->GetRaiseEnabled());
    }
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateHideNonSystemFloatingWindows(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "Update property hideNonSystemFloatingWindows permission denied!");
        return WMError::WM_OK;
    }
    auto currentProperty = GetSessionProperty();
    if (currentProperty != nullptr) {
        NotifySessionChangeByActionNotifyManager(property, action);
        currentProperty->SetHideNonSystemFloatingWindows(property->GetHideNonSystemFloatingWindows());
    }
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateTextfieldAvoidInfo(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty != nullptr) {
        sessionProperty->SetTextFieldPositionY(property->GetTextFieldPositionY());
        sessionProperty->SetTextFieldHeight(property->GetTextFieldHeight());
    }
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateWindowMask(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty != nullptr) {
        sessionProperty->SetWindowMask(property->GetWindowMask());
        sessionProperty->SetIsShaped(property->GetIsShaped());
        NotifySessionChangeByActionNotifyManager(property, action);
    }
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateTopmost(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "UpdateTopmostProperty permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }

    SetTopmost(property->IsTopmost());
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateMainWindowTopmost(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    SetMainWindowTopmost(property->IsMainWindowTopmost());
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateSubWindowZLevel(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    SetSubWindowZLevel(property->GetSubWindowZLevel());
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateAvoidAreaOption(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    auto sessionProperty = GetSessionProperty();
    if (!SessionHelper::IsSubWindow(sessionProperty->GetWindowType()) &&
        !SessionHelper::IsSystemWindow(sessionProperty->GetWindowType())) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    sessionProperty->SetAvoidAreaOption(property->GetAvoidAreaOption());
    return WMError::WM_OK;
}

WMError SceneSession::HandleBackgroundAlpha(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    auto sessionProperty = GetSessionProperty();
    if (!sessionProperty) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "property is null");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    sessionProperty->SetBackgroundAlpha(property->GetBackgroundAlpha());
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateExclusivelyHighlighted(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    auto sessionProperty = GetSessionProperty();
    if (!sessionProperty) {
        TLOGE(WmsLogTag::WMS_FOCUS, "property is null");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    sessionProperty->SetExclusivelyHighlighted(property->GetExclusivelyHighlighted());
    return WMError::WM_OK;
}

void SceneSession::HandleSpecificSystemBarProperty(WindowType type, const sptr<WindowSessionProperty>& property)
{
    auto systemBarProperties = property->GetSystemBarProperty();
    if (auto iter = systemBarProperties.find(type); iter != systemBarProperties.end()) {
        SetSystemBarProperty(iter->first, iter->second);
        TLOGD(WmsLogTag::WMS_IMMS, "type %{public}d enable %{public}d",
            static_cast<int32_t>(iter->first), iter->second.enable_);
    }
}

void SceneSession::SetWindowFlags(const sptr<WindowSessionProperty>& property)
{
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "get session property failed");
        return;
    }
    uint32_t flags = property->GetWindowFlags();
    uint32_t oldFlags = sessionProperty->GetWindowFlags();
    if (((oldFlags ^ flags) == static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED) ||
         (oldFlags ^ flags) == static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK)) &&
        !property->GetSystemCalling()) {
        TLOGE(WmsLogTag::DEFAULT, "Set window flags permission denied");
        return;
    }
    sessionProperty->SetWindowFlags(flags);
    if ((oldFlags ^ flags) == static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED)) {
        OnShowWhenLocked(flags & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED));
    }
    TLOGI(WmsLogTag::DEFAULT, "flags: %{public}u", flags);
}

void SceneSession::NotifySessionChangeByActionNotifyManager(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    TLOGD(WmsLogTag::DEFAULT, "id: %{public}d, action: %{public}" PRIu64, GetPersistentId(), action);
    if (sessionChangeByActionNotifyManagerFunc_ == nullptr) {
        TLOGW(WmsLogTag::DEFAULT, "func is null");
        return;
    }
    sessionChangeByActionNotifyManagerFunc_(this, property, action);
}

WSError SceneSession::TerminateSession(const sptr<AAFwk::SessionInfo> abilitySessionInfo)
{
    PostLifeCycleTask([weakThis = wptr(this), abilitySessionInfo, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (abilitySessionInfo == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s abilitySessionInfo is null", where);
            session->RemoveLifeCycleTask(LifeCycleTaskType::STOP);
            return WSError::WS_ERROR_NULLPTR;
        }
        if (session->isTerminating_) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s is terminating, return!", where);
            session->RemoveLifeCycleTask(LifeCycleTaskType::STOP);
            return WSError::WS_ERROR_INVALID_OPERATION;
        }
        session->isTerminating_ = true;
        SessionInfo info;
        info.abilityName_ = abilitySessionInfo->want.GetElement().GetAbilityName();
        info.bundleName_ = abilitySessionInfo->want.GetElement().GetBundleName();
        info.callerToken_ = abilitySessionInfo->callerToken;
        info.persistentId_ = static_cast<int32_t>(abilitySessionInfo->persistentId);
        {
            std::lock_guard<std::recursive_mutex> lock(session->sessionInfoMutex_);
            session->sessionInfo_.closeAbilityWant = std::make_shared<AAFwk::Want>(abilitySessionInfo->want);
            session->sessionInfo_.resultCode = abilitySessionInfo->resultCode;
        }
        if (session->terminateSessionFunc_) {
            session->terminateSessionFunc_(info);
        }
        return WSError::WS_OK;
    }, __func__, LifeCycleTaskType::STOP);
    return WSError::WS_OK;
}

WSError SceneSession::NotifySessionExceptionInner(const sptr<AAFwk::SessionInfo> abilitySessionInfo,
    const ExceptionInfo& exceptionInfo, bool isFromClient, bool startFail)
{
    PostLifeCycleTask([weakThis = wptr(this), abilitySessionInfo, exceptionInfo,
        isFromClient, startFail, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (abilitySessionInfo == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s abilitySessionInfo is null", where);
            session->RemoveLifeCycleTask(LifeCycleTaskType::STOP);
            return WSError::WS_ERROR_NULLPTR;
        }
        if (SessionHelper::IsMainWindow(session->GetWindowType()) && !session->clientIdentityToken_.empty() &&
            isFromClient && (abilitySessionInfo->errorReason != ERROR_REASON_LOW_MEMORY_KILL &&
            session->clientIdentityToken_ != abilitySessionInfo->identityToken)) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s client exception not matched: %{public}s, %{public}s",
                where, session->clientIdentityToken_.c_str(), abilitySessionInfo->identityToken.c_str());
            session->RemoveLifeCycleTask(LifeCycleTaskType::STOP);
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        if (session->isTerminating_) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s is terminating, return!", where);
            session->RemoveLifeCycleTask(LifeCycleTaskType::STOP);
            return WSError::WS_ERROR_INVALID_OPERATION;
        }
        session->isTerminating_ = true;
        SessionInfo info;
        info.abilityName_ = abilitySessionInfo->want.GetElement().GetAbilityName();
        info.bundleName_ = abilitySessionInfo->want.GetElement().GetBundleName();
        info.callerToken_ = abilitySessionInfo->callerToken;
        info.errorCode = abilitySessionInfo->errorCode;
        info.errorReason = abilitySessionInfo->errorReason;
        info.persistentId_ = static_cast<int32_t>(abilitySessionInfo->persistentId);
        {
            std::lock_guard<std::recursive_mutex> lock(session->sessionInfoMutex_);
            session->sessionInfo_.closeAbilityWant = std::make_shared<AAFwk::Want>(abilitySessionInfo->want);
            session->sessionInfo_.errorCode = abilitySessionInfo->errorCode;
            session->sessionInfo_.errorReason = abilitySessionInfo->errorReason;
        }
        if (session->sessionExceptionFunc_) {
            session->sessionExceptionFunc_(info, exceptionInfo, false);
        }
        if (session->jsSceneSessionExceptionFunc_) {
            session->jsSceneSessionExceptionFunc_(info, exceptionInfo, startFail);
        }
        return WSError::WS_OK;
    }, __func__, LifeCycleTaskType::STOP);
    return WSError::WS_OK;
}

WSError SceneSession::NotifySessionException(const sptr<AAFwk::SessionInfo> abilitySessionInfo,
    const ExceptionInfo& exceptionInfo)
{
    if (!SessionPermission::VerifyCallingPermission(PermissionConstants::PERMISSION_MANAGE_MISSION)) {
        TLOGE(WmsLogTag::WMS_LIFE, "permission failed.");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    return NotifySessionExceptionInner(abilitySessionInfo, exceptionInfo, true);
}

WSRect SceneSession::GetLastSafeRect() const
{
    return lastSafeRect;
}

void SceneSession::SetLastSafeRect(WSRect rect)
{
    lastSafeRect.posX_ = rect.posX_;
    lastSafeRect.posY_ = rect.posY_;
    lastSafeRect.width_ = rect.width_;
    lastSafeRect.height_ = rect.height_;
    return;
}

void SceneSession::SetMovable(bool movable)
{
    PostTask([weakThis = wptr(this), movable, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s session is null", where);
            return;
        }
        if (session->moveDragController_) {
            TLOGNI(WmsLogTag::WMS_LAYOUT, "%{public}s id: %{public}d, isMovable: %{public}d",
                where, session->GetPersistentId(), movable);
            session->moveDragController_->SetMovable(movable);
        }
    }, __func__);
}

WSError SceneSession::SetSplitButtonVisible(bool isVisible)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "isVisible: %{public}d", isVisible);
    if (!sessionStage_) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "sessionStage is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    return sessionStage_->SetSplitButtonVisible(isVisible);
}

WSError SceneSession::SendContainerModalEvent(const std::string& eventName, const std::string& eventValue)
{
    TLOGI(WmsLogTag::WMS_EVENT, "name: %{public}s, value: %{public}s", eventName.c_str(), eventValue.c_str());
    if (!sessionStage_) {
        TLOGE(WmsLogTag::WMS_EVENT, "sessionStage is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    return sessionStage_->SendContainerModalEvent(eventName, eventValue);
}

WSError SceneSession::OnContainerModalEvent(const std::string& eventName, const std::string& eventValue)
{
    TLOGI(WmsLogTag::WMS_LAYOUT_PC, "name: %{public}s, value: %{public}s", eventName.c_str(), eventValue.c_str());
    if (eventName == WINDOW_RELOCATION_EVENT) {
        if (eventValue == "true") {
            ThrowSlipDirectly(VELOCITY_RELOCATION_TO_TOP);
        } else {
            ThrowSlipDirectly(VELOCITY_RELOCATION_TO_BOTTOM);
        }
    }
    return WSError::WS_OK;
}

void SceneSession::RegisterSetLandscapeMultiWindowFunc(NotifyLandscapeMultiWindowSessionFunc&& callback)
{
    PostTask([weakThis = wptr(this), callback = std::move(callback), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_MULTI_WINDOW, "%{public}s session is null", where);
            return;
        }
        session->onSetLandscapeMultiWindowFunc_ = std::move(callback);
    }, __func__);
}

int32_t SceneSession::GetOriPosYBeforeRaisedByKeyboard() const
{
    return oriPosYBeforeRaisedByKeyboard_;
}

void SceneSession::SetOriPosYBeforeRaisedByKeyboard(int32_t posY)
{
    oriPosYBeforeRaisedByKeyboard_ = posY;
}

bool SceneSession::AddSubSession(const sptr<SceneSession>& subSession)
{
    if (subSession == nullptr) {
        TLOGE(WmsLogTag::WMS_SUB, "subSession is nullptr");
        return false;
    }
    const auto& persistentId = subSession->GetPersistentId();
    auto iter = std::find_if(subSession_.begin(), subSession_.end(),
        [persistentId](sptr<SceneSession> session) {
            bool res = (session != nullptr && session->GetPersistentId() == persistentId) ? true : false;
            return res;
        });
    if (iter != subSession_.end()) {
        TLOGE(WmsLogTag::WMS_SUB, "Sub ession is already exists, id: %{public}d, parentId: %{public}d",
            subSession->GetPersistentId(), GetPersistentId());
        return false;
    }
    TLOGD(WmsLogTag::WMS_SUB, "Success, id: %{public}d, parentId: %{public}d",
        subSession->GetPersistentId(), GetPersistentId());
    subSession_.push_back(subSession);
    return true;
}

bool SceneSession::RemoveSubSession(int32_t persistentId)
{
    auto iter = std::find_if(subSession_.begin(), subSession_.end(),
        [persistentId](sptr<SceneSession> session) {
            bool res = (session != nullptr && session->GetPersistentId() == persistentId) ? true : false;
            return res;
        });
    if (iter == subSession_.end()) {
        TLOGE(WmsLogTag::WMS_SUB, "Could not find subsession, id: %{public}d, parentId: %{public}d",
            persistentId, GetPersistentId());
        return false;
    }
    TLOGD(WmsLogTag::WMS_SUB, "Success, id: %{public}d, parentId: %{public}d", persistentId, GetPersistentId());
    subSession_.erase(iter);
    return true;
}

bool SceneSession::AddToastSession(const sptr<SceneSession>& toastSession)
{
    if (toastSession == nullptr) {
        TLOGE(WmsLogTag::WMS_TOAST, "toastSession is nullptr");
        return false;
    }
    const auto& persistentId = toastSession->GetPersistentId();
    auto iter = std::find_if(toastSession_.begin(), toastSession_.end(),
        [persistentId](sptr<SceneSession> session) {
            bool res = (session != nullptr && session->GetPersistentId() == persistentId) ? true : false;
            return res;
        });
    if (iter != toastSession_.end()) {
        TLOGE(WmsLogTag::WMS_TOAST, "Toast ession is already exists, id: %{public}d, parentId: %{public}d",
            toastSession->GetPersistentId(), GetPersistentId());
        return false;
    }
    TLOGD(WmsLogTag::WMS_TOAST, "Success, id: %{public}d, parentId: %{public}d",
        toastSession->GetPersistentId(), GetPersistentId());
    toastSession_.push_back(toastSession);
    return true;
}

bool SceneSession::RemoveToastSession(int32_t persistentId)
{
    auto iter = std::find_if(toastSession_.begin(), toastSession_.end(),
        [persistentId](sptr<SceneSession> session) {
            bool res = (session != nullptr && session->GetPersistentId() == persistentId) ? true : false;
            return res;
        });
    if (iter == toastSession_.end()) {
        TLOGE(WmsLogTag::WMS_TOAST, "Could not find toastSession, id: %{public}d, parentId: %{public}d",
            persistentId, GetPersistentId());
        return false;
    }
    TLOGD(WmsLogTag::WMS_TOAST, "Success, id: %{public}d, parentId: %{public}d", persistentId, GetPersistentId());
    toastSession_.erase(iter);
    return true;
}

void SceneSession::NotifyPiPWindowPrepareClose()
{
    TLOGD(WmsLogTag::WMS_PIP, "in");
    int32_t callingPid = IPCSkeleton::GetCallingPid();
    PostTask([weakThis = wptr(this), callingPid, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_PIP, "%{public}s session is null", where);
            return;
        }
        if (callingPid != session->GetCallingPid()) {
            TLOGNW(WmsLogTag::WMS_PIP, "%{public}s permission denied, not call by the same process", where);
            return;
        }
        if (session->onPrepareClosePiPSession_) {
            session->onPrepareClosePiPSession_();
        }
        TLOGND(WmsLogTag::WMS_PIP, "%{public}s id: %{public}d", where, session->GetPersistentId());
    }, __func__);
}

WSError SceneSession::SetLandscapeMultiWindow(bool isLandscapeMultiWindow)
{
    TLOGD(WmsLogTag::WMS_MULTI_WINDOW, "in");
    int32_t callingPid = IPCSkeleton::GetCallingPid();
    PostTask([weakThis = wptr(this), isLandscapeMultiWindow, callingPid, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_MULTI_WINDOW, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (callingPid != session->GetCallingPid()) {
            TLOGNE(WmsLogTag::WMS_MULTI_WINDOW, "%{public}s premission denied, not call by the same process", where);
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
        if (session->onSetLandscapeMultiWindowFunc_) {
            session->onSetLandscapeMultiWindowFunc_(isLandscapeMultiWindow);
        }
        TLOGND(WmsLogTag::WMS_MULTI_WINDOW, "%{public}s id: %{public}d, isLandscapeMultiWindow: %{public}u",
            where, session->GetPersistentId(), isLandscapeMultiWindow);
        return WSError::WS_OK;
    }, __func__);
    return WSError::WS_OK;
}

std::vector<sptr<SceneSession>> SceneSession::GetSubSession() const
{
    return subSession_;
}

std::vector<sptr<SceneSession>> SceneSession::GetToastSession() const
{
    return toastSession_;
}

WSRect SceneSession::GetSessionTargetRectByDisplayId(DisplayId displayId) const
{
    WSRect rect;
    if (moveDragController_) {
        rect = moveDragController_->GetTargetRectByDisplayId(displayId);
    } else {
        WLOGFI("moveDragController_ is null");
    }
    return rect;
}

void SceneSession::SetWindowDragHotAreaListener(const NotifyWindowDragHotAreaFunc& func)
{
    if (moveDragController_) {
        moveDragController_->SetWindowDragHotAreaFunc(func);
    }
}

void SceneSession::NotifySessionForeground(uint32_t reason, bool withAnimation)
{
    if (!sessionStage_) {
        return;
    }
    return sessionStage_->NotifySessionForeground(reason, withAnimation);
}

void SceneSession::NotifySessionBackground(uint32_t reason, bool withAnimation, bool isFromInnerkits)
{
    if (!sessionStage_) {
        return;
    }
    return sessionStage_->NotifySessionBackground(reason, withAnimation, isFromInnerkits);
}

void SceneSession::NotifySessionFullScreen(bool fullScreen)
{
    if (!sessionStage_) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "sessionStage is null");
        return;
    }
    sessionStage_->NotifySessionFullScreen(fullScreen);
}

WSError SceneSession::UpdatePiPRect(const Rect& rect, SizeChangeReason reason)
{
    if (!WindowHelper::IsPipWindow(GetWindowType())) {
        return WSError::WS_DO_NOTHING;
    }
    int32_t callingPid = IPCSkeleton::GetCallingPid();
    auto task = [weakThis = wptr(this), rect, reason, callingPid, where = __func__] {
        auto session = weakThis.promote();
        if (!session || session->isTerminating_) {
            TLOGNE(WmsLogTag::WMS_PIP,
                "%{public}s session is null or is terminating", where);
            return WSError::WS_ERROR_INVALID_OPERATION;
        }
        if (callingPid != session->GetCallingPid()) {
            TLOGNW(WmsLogTag::WMS_PIP, "%{public}s permission denied, not call by the same process", where);
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
        WSRect wsRect = SessionHelper::TransferToWSRect(rect);
        if (reason == SizeChangeReason::PIP_START) {
            session->SetSessionRequestRect(wsRect);
        }
        TLOGNI(WmsLogTag::WMS_PIP, "%{public}s rect:%{public}s, reason:%{public}u", where, wsRect.ToString().c_str(),
            static_cast<uint32_t>(reason));
        session->NotifySessionRectChange(wsRect, reason);
        return WSError::WS_OK;
    };
    if (mainHandler_ != nullptr) {
        mainHandler_->PostTask(std::move(task), __func__, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    } else {
        PostTask(std::move(task), __func__);
    }
    return WSError::WS_OK;
}

WSError SceneSession::UpdatePiPControlStatus(WsPiPControlType controlType, WsPiPControlStatus status)
{
    TLOGI(WmsLogTag::WMS_PIP, "controlType:%{public}u, status:%{public}d", controlType, status);
    if (!WindowHelper::IsPipWindow(GetWindowType())) {
        return WSError::WS_DO_NOTHING;
    }
    int32_t callingPid = IPCSkeleton::GetCallingPid();
    PostTask([weakThis = wptr(this), controlType, status, callingPid, where = __func__] {
        auto session = weakThis.promote();
        if (!session || session->isTerminating_) {
            TLOGNE(WmsLogTag::WMS_PIP, "%{public}s session is null or is terminating", where);
            return WSError::WS_ERROR_INVALID_OPERATION;
        }
        if (callingPid != session->GetCallingPid()) {
            TLOGNW(WmsLogTag::WMS_PIP, "%{public}s permission denied, not call by the same process", where);
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
        if (session->sessionPiPControlStatusChangeFunc_) {
            HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSession::UpdatePiPControlStatus");
            session->sessionPiPControlStatusChangeFunc_(controlType, status);
        }
        return WSError::WS_OK;
    }, __func__);
    return WSError::WS_OK;
}

WSError SceneSession::SetAutoStartPiP(bool isAutoStart, uint32_t priority, uint32_t width, uint32_t height)
{
    TLOGI(WmsLogTag::WMS_PIP, "isAutoStart:%{public}u priority:%{public}u width:%{public}u height:%{public}u",
        isAutoStart, priority, width, height);
    PostTask([weakThis = wptr(this), isAutoStart, priority, width, height, where = __func__] {
        auto session = weakThis.promote();
        if (!session || session->isTerminating_) {
            TLOGNE(WmsLogTag::WMS_PIP, "%{public}s session is null or is terminating", where);
            return;
        }
        if (session->autoStartPiPStatusChangeFunc_) {
            HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSession::SetAutoStartPiP");
            session->autoStartPiPStatusChangeFunc_(isAutoStart, priority, width, height);
        }
    }, __func__);
    return WSError::WS_OK;
}

void SceneSession::SendPointerEventToUI(std::shared_ptr<MMI::PointerEvent> pointerEvent)
{
    NotifySystemSessionPointerEventFunc systemSessionPointerEventFunc = nullptr;
    {
        std::lock_guard<std::mutex> lock(pointerEventMutex_);
        systemSessionPointerEventFunc = systemSessionPointerEventFunc_;
    }
    if (systemSessionPointerEventFunc != nullptr) {
        systemSessionPointerEventFunc(pointerEvent);
    } else {
        TLOGE(WmsLogTag::WMS_EVENT, "PointerEventFunc_ nullptr, id:%{public}d", pointerEvent->GetId());
        pointerEvent->MarkProcessed();
    }
}

bool SceneSession::SendKeyEventToUI(std::shared_ptr<MMI::KeyEvent> keyEvent, bool isPreImeEvent)
{
    NotifySystemSessionKeyEventFunc systemSessionKeyEventFunc = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(keyEventMutex_);
        systemSessionKeyEventFunc = systemSessionKeyEventFunc_;
    }
    if (systemSessionKeyEventFunc != nullptr) {
        return systemSessionKeyEventFunc(keyEvent, isPreImeEvent);
    }
    return false;
}

WSError SceneSession::UpdateSizeChangeReason(SizeChangeReason reason)
{
    PostTask([weakThis = wptr(this), reason, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        session->reason_ = reason;
        if (reason != SizeChangeReason::UNDEFINED) {
            HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER,
                "SceneSession::UpdateSizeChangeReason%d reason:%d",
                session->GetPersistentId(), static_cast<uint32_t>(reason));
            TLOGND(WmsLogTag::WMS_LAYOUT, "%{public}s Id: %{public}d, reason: %{public}d",
                where, session->GetPersistentId(), reason);
        }
        return WSError::WS_OK;
    }, __func__);
    return WSError::WS_OK;
}

void SceneSession::ResetSizeChangeReasonIfDirty()
{
    auto reason = GetSizeChangeReason();
    if (IsDirtyWindow() &&
        reason != SizeChangeReason::DRAG &&
        reason != SizeChangeReason::DRAG_END &&
        reason != SizeChangeReason::DRAG_START &&
        reason != SizeChangeReason::DRAG_MOVE) {
        UpdateSizeChangeReason(SizeChangeReason::UNDEFINED);
    }
}

bool SceneSession::IsDirtyWindow()
{
    return dirtyFlags_ & static_cast<uint32_t>(SessionUIDirtyFlag::RECT);
}

bool SceneSession::IsDirtyDragWindow()
{
    return dirtyFlags_ & static_cast<uint32_t>(SessionUIDirtyFlag::DRAG_RECT) || isDragging_;
}

void SceneSession::ResetDirtyDragFlags()
{
    dirtyFlags_ &= ~static_cast<uint32_t>(SessionUIDirtyFlag::DRAG_RECT);
    isDragging_ = false;
}

void SceneSession::NotifyUILostFocus()
{
    if (moveDragController_) {
        moveDragController_->OnLostFocus();
    }
    Session::NotifyUILostFocus();
}

void SceneSession::SetScale(float scaleX, float scaleY, float pivotX, float pivotY)
{
    if (scaleX_ != scaleX || scaleY_ != scaleY || pivotX_ != pivotX || pivotY_ != pivotY) {
        Session::SetScale(scaleX, scaleY, pivotX, pivotY);
        if (specificCallback_ != nullptr) {
            specificCallback_->onWindowInfoUpdate_(GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_PROPERTY);
        }
        if (sessionStage_ != nullptr) {
            Transform transform;
            transform.scaleX_ = scaleX;
            transform.scaleY_ = scaleY;
            transform.pivotX_ = pivotX;
            transform.pivotY_ = pivotY;
            sessionStage_->NotifyTransformChange(transform);
        } else {
            WLOGFE("sessionStage_ is nullptr");
        }
    }
}

void SceneSession::RequestHideKeyboard(bool isAppColdStart)
{
#ifdef IMF_ENABLE
    PostExportTask([weakThis = wptr(this), isAppColdStart, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD,
                "%{public}s Session is null, notify inputMethod framework hide keyboard failed!", where);
            return;
        }
        TLOGNI(WmsLogTag::WMS_KEYBOARD, "%{public}s Notify inputMethod framework hide keyboard start, id: %{public}d,"
            "isAppColdStart: %{public}d", where, session->GetPersistentId(), isAppColdStart);
        if (MiscServices::InputMethodController::GetInstance()) {
            MiscServices::InputMethodController::GetInstance()->RequestHideInput();
            TLOGNI(WmsLogTag::WMS_KEYBOARD, "%{public}s Notify InputMethod framework hide keyboard end. id: %{public}d",
                where, session->GetPersistentId());
        }
    }, __func__);
#endif
}

bool SceneSession::IsStartMoving()
{
    return isStartMoving_.load();
}

void SceneSession::SetIsStartMoving(bool startMoving)
{
    isStartMoving_.store(startMoving);
}

void SceneSession::SetShouldHideNonSecureWindows(bool shouldHide)
{
    shouldHideNonSecureWindows_.store(shouldHide);
}

void SceneSession::CalculateCombinedExtWindowFlags()
{
    {
        // Only correct when each flag is true when active, and once a uiextension is active, the host is active
        std::unique_lock<std::shared_mutex> lock(combinedExtWindowFlagsMutex_);
        combinedExtWindowFlags_.bitData = 0;
        for (const auto& iter: extWindowFlagsMap_) {
            combinedExtWindowFlags_.bitData |= iter.second.bitData;
        }
    }
    NotifyPrivacyModeChange();
}

void SceneSession::UpdateExtWindowFlags(int32_t extPersistentId, const ExtensionWindowFlags& extWindowFlags,
    const ExtensionWindowFlags& extWindowActions)
{
    auto iter = extWindowFlagsMap_.find(extPersistentId);
    // Each flag is false when inactive, 0 means all flags are inactive
    auto oldFlags = iter != extWindowFlagsMap_.end() ? iter->second : ExtensionWindowFlags();
    ExtensionWindowFlags newFlags((extWindowFlags.bitData & extWindowActions.bitData) |
        (oldFlags.bitData & ~extWindowActions.bitData));
    if (newFlags.bitData == 0) {
        extWindowFlagsMap_.erase(extPersistentId);
    } else {
        extWindowFlagsMap_[extPersistentId] = newFlags;
    }
    CalculateCombinedExtWindowFlags();
}

ExtensionWindowFlags SceneSession::GetCombinedExtWindowFlags()
{
    std::shared_lock<std::shared_mutex> lock(combinedExtWindowFlagsMutex_);
    auto combinedExtWindowFlags = combinedExtWindowFlags_;
    combinedExtWindowFlags.hideNonSecureWindowsFlag = IsSessionForeground() &&
        (combinedExtWindowFlags.hideNonSecureWindowsFlag || shouldHideNonSecureWindows_.load());
    return combinedExtWindowFlags;
}

void SceneSession::NotifyDisplayMove(DisplayId from, DisplayId to)
{
    if (sessionStage_) {
        sessionStage_->NotifyDisplayMove(from, to);
    } else {
        WLOGFE("Notify display move failed, sessionStage is null");
    }
}

void SceneSession::RemoveExtWindowFlags(int32_t extPersistentId)
{
    extWindowFlagsMap_.erase(extPersistentId);
    CalculateCombinedExtWindowFlags();
}

void SceneSession::ClearExtWindowFlags()
{
    std::unique_lock<std::shared_mutex> lock(combinedExtWindowFlagsMutex_);
    extWindowFlagsMap_.clear();
    combinedExtWindowFlags_.bitData = 0;
}

WSError SceneSession::UpdateRectChangeListenerRegistered(bool isRegister)
{
    PostTask([weakThis = wptr(this), isRegister, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        session->rectChangeListenerRegistered_ = isRegister;
        return WSError::WS_OK;
    }, __func__);
    return WSError::WS_OK;
}

void SceneSession::SetIsLayoutFullScreen(bool isLayoutFullScreen)
{
    isLayoutFullScreen_ = isLayoutFullScreen;
}

bool SceneSession::IsLayoutFullScreen() const
{
    return isLayoutFullScreen_;
}

WSError SceneSession::OnLayoutFullScreenChange(bool isLayoutFullScreen)
{
    PostTask([weakThis = wptr(this), isLayoutFullScreen, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s session is null", where);
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        TLOGNI(WmsLogTag::WMS_LAYOUT, "%{public}s isLayoutFullScreen: %{public}d", where, isLayoutFullScreen);
        if (session->onLayoutFullScreenChangeFunc_) {
            session->SetIsLayoutFullScreen(isLayoutFullScreen);
            session->onLayoutFullScreenChangeFunc_(isLayoutFullScreen);
        }
        return WSError::WS_OK;
    }, __func__);
    return WSError::WS_OK;
}

WSError SceneSession::OnDefaultDensityEnabled(bool isDefaultDensityEnabled)
{
    PostTask([weakThis = wptr(this), isDefaultDensityEnabled, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s session is null", where);
            return;
        }
        TLOGNI(WmsLogTag::WMS_LAYOUT, "%{public}s isDefaultDensityEnabled: %{public}d",
            where, isDefaultDensityEnabled);
        if (session->onDefaultDensityEnabledFunc_) {
            session->onDefaultDensityEnabledFunc_(isDefaultDensityEnabled);
        }
    }, __func__);
    return WSError::WS_OK;
}

void SceneSession::SetForceHideState(ForceHideState forceHideState)
{
    forceHideState_ = forceHideState;
}

ForceHideState SceneSession::GetForceHideState() const
{
    return forceHideState_;
}

void SceneSession::SetIsDisplayStatusBarTemporarily(bool isTemporary)
{
    isDisplayStatusBarTemporarily_.store(isTemporary);
}

bool SceneSession::GetIsDisplayStatusBarTemporarily() const
{
    return isDisplayStatusBarTemporarily_.load();
}

void SceneSession::RetrieveStatusBarDefaultVisibility()
{
    if (specificCallback_ && specificCallback_->onGetStatusBarDefaultVisibilityByDisplayId_) {
        isStatusBarVisible_ = specificCallback_->onGetStatusBarDefaultVisibilityByDisplayId_(
            GetSessionProperty()->GetDisplayId());
    }
}

void SceneSession::SetIsLastFrameLayoutFinishedFunc(IsLastFrameLayoutFinishedFunc&& func)
{
    isLastFrameLayoutFinishedFunc_ = std::move(func);
}

void SceneSession::SetIsAINavigationBarAvoidAreaValidFunc(IsAINavigationBarAvoidAreaValidFunc&& func)
{
    isAINavigationBarAvoidAreaValid_ = std::move(func);
}

void SceneSession::SetStartingWindowExitAnimationFlag(bool enable)
{
    TLOGI(WmsLogTag::WMS_PATTERN, "SetStartingWindowExitAnimationFlag %{public}d", enable);
    needStartingWindowExitAnimation_.store(enable);
}

bool SceneSession::NeedStartingWindowExitAnimation() const
{
    return needStartingWindowExitAnimation_.load();
}

bool SceneSession::IsSystemSpecificSession() const
{
    return isSystemSpecificSession_;
}

void SceneSession::SetIsSystemSpecificSession(bool isSystemSpecificSession)
{
    isSystemSpecificSession_ = isSystemSpecificSession;
}

void SceneSession::SetTemporarilyShowWhenLocked(bool isTemporarilyShowWhenLocked)
{
    if (isTemporarilyShowWhenLocked_.load() == isTemporarilyShowWhenLocked) {
        return;
    }
    isTemporarilyShowWhenLocked_.store(isTemporarilyShowWhenLocked);
    TLOGI(WmsLogTag::WMS_SCB, "SetTemporarilyShowWhenLocked successfully, target:%{public}u",
        isTemporarilyShowWhenLocked);
}

bool SceneSession::IsTemporarilyShowWhenLocked() const
{
    return isTemporarilyShowWhenLocked_.load();
}

void SceneSession::SetSkipDraw(bool skip)
{
    auto surfaceNode = GetSurfaceNode();
    if (!surfaceNode) {
        WLOGFE("surfaceNode_ is null");
        return;
    }
    auto rsTransaction = RSTransactionProxy::GetInstance();
    if (rsTransaction != nullptr) {
        rsTransaction->Begin();
    }
    surfaceNode->SetSkipDraw(skip);
    if (auto leashWinSurfaceNode = GetLeashWinSurfaceNode()) {
        leashWinSurfaceNode->SetSkipDraw(skip);
    }
    if (rsTransaction != nullptr) {
        rsTransaction->Commit();
    }
}

void SceneSession::SetSkipSelfWhenShowOnVirtualScreen(bool isSkip)
{
    TLOGW(WmsLogTag::WMS_SCB, "in sceneSession, do nothing");
    return;
}

WMError SceneSession::SetUniqueDensityDpi(bool useUnique, float dpi)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "SceneSession set unique dpi: id=%{public}d, dpi=%{public}f",
        GetPersistentId(), dpi);
    if (useUnique && (dpi > DOT_PER_INCH_MAXIMUM_VALUE || dpi < DOT_PER_INCH_MINIMUM_VALUE)) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "Invalid input dpi value, valid input range: %{public}u ~ %{public}u",
            DOT_PER_INCH_MINIMUM_VALUE, DOT_PER_INCH_MAXIMUM_VALUE);
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    float density = static_cast<float>(dpi) / 160; // 160 is the coefficient between density and dpi;
    if (!IsSessionValid()) {
        return WMError::WM_ERROR_INVALID_SESSION;
    }
    if (!sessionStage_) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "sessionStage_ is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    sessionStage_->SetUniqueVirtualPixelRatio(useUnique, density);
    return WMError::WM_OK;
}

WMError SceneSession::SetSystemWindowEnableDrag(bool enableDrag)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "id:%{public}d, permission denied!", GetPersistentId());
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }

    if (!WindowHelper::IsSystemWindow(GetWindowType())) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "id:%{public}d, this is not system window", GetPersistentId());
        return WMError::WM_ERROR_INVALID_CALLING;
    }

    PostTask([weakThis = wptr(this), enableDrag, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s: session is null", where);
            return;
        }
        TLOGNI(WmsLogTag::WMS_LAYOUT, "%{public}s: id:%{public}d, enableDrag:%{public}d",
            where, session->GetPersistentId(), enableDrag);
        session->GetSessionProperty()->SetDragEnabled(enableDrag);
        session->NotifySessionInfoChange();
    }, __func__);
    return WMError::WM_OK;
}

WMError SceneSession::SetWindowEnableDragBySystem(bool enableDrag)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "enableDrag: %{public}d", enableDrag);
    PostTask([weakThis = wptr(this), enableDrag, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s session is null", where);
            return;
        }
        session->SetClientDragEnable(enableDrag);
        TLOGNI(WmsLogTag::WMS_LAYOUT, "%{public}s id: %{public}d, enableDrag: %{public}d",
            where, session->GetPersistentId(), enableDrag);
        session->GetSessionProperty()->SetDragEnabled(enableDrag);
        if (session->sessionStage_) {
            session->sessionStage_->SetEnableDragBySystem(enableDrag);
        }
    }, __func__);
    return WMError::WM_OK;
}

WMError SceneSession::ActivateDragBySystem(bool activateDrag)
{
    PostTask([weakThis = wptr(this), activateDrag, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s session is null", where);
            return;
        }
        session->SetDragActivated(activateDrag);
        session->NotifySessionInfoChange();
        TLOGNI(WmsLogTag::WMS_LAYOUT, "%{public}s id: %{public}d, activate drag: %{public}d",
            where, session->GetPersistentId(), activateDrag);
        if (session->sessionStage_) {
            session->sessionStage_->SetDragActivated(activateDrag);
        }
    }, __func__);
    return WMError::WM_OK;
}

WMError SceneSession::HandleActionUpdateWindowModeSupportType(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    if (!property->GetSystemCalling()) {
        TLOGE(WmsLogTag::DEFAULT, "permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }

    auto sessionProperty = GetSessionProperty();
    if (sessionProperty != nullptr) {
        sessionProperty->SetWindowModeSupportType(property->GetWindowModeSupportType());
    }
    return WMError::WM_OK;
}

void SceneSession::RegisterForceSplitListener(const NotifyForceSplitFunc& func)
{
    forceSplitFunc_ = func;
}

void SceneSession::RegisterRequestedOrientationChangeCallback(NotifyReqOrientationChangeFunc&& callback)
{
    PostTask([weakThis = wptr(this), callback = std::move(callback), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return;
        }
        session->onRequestedOrientationChange_ = std::move(callback);
    }, __func__);
}

void SceneSession::RegisterBindDialogSessionCallback(const NotifyBindDialogSessionFunc& callback)
{
    onBindDialogTarget_ = callback;
}

void SceneSession::RegisterIsCustomAnimationPlayingCallback(NotifyIsCustomAnimationPlayingCallback&& callback)
{
    PostTask([weakThis = wptr(this), callback = std::move(callback), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return;
        }
        session->onIsCustomAnimationPlaying_ = std::move(callback);
    }, __func__);
}

void SceneSession::RegisterLayoutFullScreenChangeCallback(NotifyLayoutFullScreenChangeFunc&& callback)
{
    PostTask([weakThis = wptr(this), callback = std::move(callback), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "%{public}s session is null", where);
            return;
        }
        session->onLayoutFullScreenChangeFunc_ = std::move(callback);
    }, __func__);
}

void SceneSession::RegisterGetStatusBarAvoidHeightFunc(GetStatusBarAvoidHeightFunc&& callback)
{
    onGetStatusBarAvoidHeightFunc_ = std::move(callback);
}

void SceneSession::RegisterGetStatusBarConstantlyShowFunc(GetStatusBarConstantlyShowFunc&& callback)
{
    onGetStatusBarConstantlyShowFunc_ = std::move(callback);
}

WMError SceneSession::GetAppForceLandscapeConfig(AppForceLandscapeConfig& config)
{
    if (forceSplitFunc_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    config = forceSplitFunc_(sessionInfo_.bundleName_);
    return WMError::WM_OK;
}

void SceneSession::SetUpdatePrivateStateAndNotifyFunc(const UpdatePrivateStateAndNotifyFunc& func)
{
    updatePrivateStateAndNotifyFunc_ = func;
}

void SceneSession::SetNotifyVisibleChangeFunc(const NotifyVisibleChangeFunc& func)
{
    notifyVisibleChangeFunc_ = func;
}

void SceneSession::SetPrivacyModeChangeNotifyFunc(NotifyPrivacyModeChangeFunc&& func)
{
    PostTask([weakThis = wptr(this), func = std::move(func), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_SCB, "%{public}s session is null", where);
            return;
        }
        session->privacyModeChangeNotifyFunc_ = std::move(func);
    }, __func__);
}

void SceneSession::SetHighlightChangeNotifyFunc(const NotifyHighlightChangeFunc& func)
{
    std::lock_guard lock(highlightChangeFuncMutex_);
    highlightChangeFunc_ = func;
}

void SceneSession::RegisterNotifySurfaceBoundsChangeFunc(int32_t sessionId, NotifySurfaceBoundsChangeFunc&& func)
{
    if (!func) {
        TLOGE(WmsLogTag::WMS_SUB, "func is null");
        return;
    }
    std::lock_guard lock(registerNotifySurfaceBoundsChangeMutex_);
    notifySurfaceBoundsChangeFuncMap_[sessionId] = func;
}

void SceneSession::UnregisterNotifySurfaceBoundsChangeFunc(int32_t sessionId)
{
    std::lock_guard lock(registerNotifySurfaceBoundsChangeMutex_);
    notifySurfaceBoundsChangeFuncMap_.erase(sessionId);
}

sptr<SceneSession> SceneSession::GetSceneSessionById(int32_t sessionId) const
{
    if (specificCallback_ == nullptr || specificCallback_->onGetSceneSessionByIdCallback_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "specificCallback or onGetSceneSessionByIdCallback is null");
        return nullptr;
    }
    return specificCallback_->onGetSceneSessionByIdCallback_(sessionId);
}

void SceneSession::SetFollowParentRectFunc(NotifyFollowParentRectFunc&& func)
{
    if (!func) {
        TLOGW(WmsLogTag::WMS_SUB, "func is null");
        return;
    }
    PostTask([weakThis = wptr(this), func = std::move(func), where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_SUB, "%{public}s session is null", where);
            return;
        }
        func(session->isFollowParentLayout_);
        session->followParentRectFunc_ = std::move(func);
    });
}

WSError SceneSession::SetFollowParentWindowLayoutEnabled(bool isFollow)
{
    auto property = GetSessionProperty();
    if (!property || property->GetSubWindowLevel() > 1) {
        TLOGE(WmsLogTag::WMS_SUB, "property is null or not surppot more than 1 level window");
        return WSError::WS_ERROR_INVALID_OPERATION;
    }
    if (!WindowHelper::IsSubWindow(property->GetWindowType()) &&
        !WindowHelper::IsDialogWindow(property->GetWindowType())) {
        TLOGE(WmsLogTag::WMS_SUB, "only sub window and dialog is valid");
        return WSError::WS_ERROR_INVALID_OPERATION;
    }
    if (!systemConfig_.supportFollowParentWindowLayout_) {
        TLOGI(WmsLogTag::WMS_SUB, "not support device");
        return WSError::WS_ERROR_DEVICE_NOT_SUPPORT;
    }
    PostTask([weakThis = wptr(this), isFollow = isFollow,  where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_SUB, "%{public}s session is null", where);
            return;
        }
        session->isFollowParentLayout_ = isFollow;
        if (session->followParentRectFunc_) {
            session->followParentRectFunc_(isFollow);
        } else {
            TLOGI(WmsLogTag::WMS_SUB, "func is null");
        }
        // if parent is null, don't need follow move drag
        if (!session->parentSession_) {
            TLOGW(WmsLogTag::WMS_SUB, "parent is null");
            return;
        }
        if (!isFollow) {
            session->parentSession_->UnregisterNotifySurfaceBoundsChangeFunc(session->GetPersistentId());
            return;
        }
        auto task = [weak = weakThis](const WSRect& rect, bool isGlobal, bool needFlush) {
            auto session = weak.promote();
            if (!session) {
                TLOGNE(WmsLogTag::WMS_SUB, "session has been destroy");
                return;
            }
            session->SetSurfaceBounds(rect, isGlobal, needFlush);
        };
        session->parentSession_->RegisterNotifySurfaceBoundsChangeFunc(session->GetPersistentId(), std::move(task));
    });
    return WSError::WS_OK;
}

int32_t SceneSession::GetStatusBarHeight()
{
    int32_t height = 0;
    if (specificCallback_ == nullptr || specificCallback_->onGetSceneSessionVectorByTypeAndDisplayId_ == nullptr ||
        GetSessionProperty() == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "specificCallback_ or session property is null");
        return height;
    }
    const auto& statusBarVector = specificCallback_->onGetSceneSessionVectorByTypeAndDisplayId_(
        WindowType::WINDOW_TYPE_STATUS_BAR, GetSessionProperty()->GetDisplayId());
    for (auto& statusBar : statusBarVector) {
        if (statusBar == nullptr) {
            continue;
        }
        WSRect statusBarRect = statusBar->GetSessionRect();
        if (onGetStatusBarAvoidHeightFunc_) {
            onGetStatusBarAvoidHeightFunc_(statusBarRect);
            TLOGD(WmsLogTag::WMS_IMMS, "win %{public}d status bar height %{public}d",
                GetPersistentId(), statusBarRect.height_);
        }
        height = statusBarRect.height_;
    }
    TLOGI(WmsLogTag::WMS_IMMS, "win %{public}d height %{public}d", GetPersistentId(), height);
    return height;
}

int32_t SceneSession::GetDockHeight()
{
    int32_t height = 0;
    if (specificCallback_ == nullptr || specificCallback_->onGetSceneSessionVectorByTypeAndDisplayId_ == nullptr ||
        GetSessionProperty() == nullptr) {
        TLOGE(WmsLogTag::WMS_DECOR, "specificCallback_ or session property is null");
        return height;
    }
    const auto& dockVector = specificCallback_->onGetSceneSessionVectorByTypeAndDisplayId_(
        WindowType::WINDOW_TYPE_LAUNCHER_DOCK, GetSessionProperty()->GetDisplayId());
    for (auto& dock : dockVector) {
        if (dock != nullptr && dock->IsVisible() && dock->GetWindowName().find("SCBSmartDock") != std::string::npos) {
            int32_t dockHeight = dock->GetSessionRect().height_;
            if (dockHeight > height) {
                height = dockHeight;
            }
        }
    }
    return height;
}

bool SceneSession::CheckPermissionWithPropertyAnimation(const sptr<WindowSessionProperty>& property) const
{
    if (property && property->GetAnimationFlag() == static_cast<uint32_t>(WindowAnimation::CUSTOM)) {
        if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
            TLOGE(WmsLogTag::WMS_LIFE, "Not system app, no permission");
            return false;
        }
    }
    return true;
}

void SceneSession::UpdatePCZOrderAndMarkDirty(const uint32_t zOrder)
{
    dirtyFlags_ |= UpdateZOrderInner(zOrder) ? static_cast<uint32_t>(SessionUIDirtyFlag::Z_ORDER) : 0;
}

uint32_t SceneSession::UpdateUIParam(const SessionUIParam& uiParam)
{
    bool lastVisible = IsVisible();
    dirtyFlags_ |= UpdateInteractiveInner(uiParam.interactive_) ?
        static_cast<uint32_t>(SessionUIDirtyFlag::INTERACTIVE) : 0;
    if (!uiParam.interactive_) {
        // keep ui state in recent
        return dirtyFlags_;
    }
    dirtyFlags_ |= UpdateVisibilityInner(true) ? static_cast<uint32_t>(SessionUIDirtyFlag::VISIBLE) : 0;
    dirtyFlags_ |= UpdateRectInner(uiParam, reason_) ?
        static_cast<uint32_t>(SessionUIDirtyFlag::RECT) : 0;
    dirtyFlags_ |= UpdateScaleInner(uiParam.scaleX_, uiParam.scaleY_, uiParam.pivotX_, uiParam.pivotY_) ?
        static_cast<uint32_t>(SessionUIDirtyFlag::SCALE) : 0;
    if (!isPcScenePanel_) {
        dirtyFlags_ |= UpdateZOrderInner(uiParam.zOrder_) ? static_cast<uint32_t>(SessionUIDirtyFlag::Z_ORDER) : 0;
    }
    if (!lastVisible && IsVisible() && !isFocused_ && !postProcessFocusState_.enabled_ &&
        GetForegroundInteractiveStatus()) {
        postProcessFocusState_.enabled_ = true;
        postProcessFocusState_.isFocused_ = true;
        postProcessFocusState_.reason_ = isStarting_ ?
            FocusChangeReason::SCB_START_APP : FocusChangeReason::FOREGROUND;
    }
    return dirtyFlags_;
}

uint32_t SceneSession::UpdateUIParam()
{
    bool lastVisible = IsVisible();
    dirtyFlags_ |= UpdateVisibilityInner(false) ? static_cast<uint32_t>(SessionUIDirtyFlag::VISIBLE) : 0;
    if (lastVisible && !IsVisible() && isFocused_) {
        postProcessFocusState_.enabled_ = true;
        postProcessFocusState_.isFocused_ = false;
        postProcessFocusState_.reason_ = FocusChangeReason::BACKGROUND;
    }
    return dirtyFlags_;
}

bool SceneSession::UpdateVisibilityInner(bool visibility)
{
    if (isVisible_ == visibility) {
        return false;
    }
    TLOGI(WmsLogTag::WMS_PIPELINE, "id: %{public}d, visibility: %{public}u -> %{public}u",
        GetPersistentId(), isVisible_, visibility);
    if (visibilityChangedDetectFunc_) {
        visibilityChangedDetectFunc_(GetCallingPid(), isVisible_, visibility);
    }
    isVisible_ = visibility;
    if (updatePrivateStateAndNotifyFunc_ != nullptr) {
        updatePrivateStateAndNotifyFunc_(GetPersistentId());
    }
    if (notifyVisibleChangeFunc_ != nullptr) {
        notifyVisibleChangeFunc_(GetPersistentId());
    }
    return true;
}

bool SceneSession::UpdateInteractiveInner(bool interactive)
{
    if (GetForegroundInteractiveStatus() == interactive) {
        return false;
    }
    SetForegroundInteractiveStatus(interactive);
    NotifyClientToUpdateInteractive(interactive);
    return true;
}

bool SceneSession::PipelineNeedNotifyClientToUpdateRect() const
{
    return IsVisibleForeground() && GetForegroundInteractiveStatus();
}

bool SceneSession::UpdateRectInner(const SessionUIParam& uiParam, SizeChangeReason reason)
{
    if (!((NotifyServerToUpdateRect(uiParam, reason) || IsDirtyWindow()) && PipelineNeedNotifyClientToUpdateRect())) {
        return false;
    }
    std::shared_ptr<RSTransaction> rsTransaction = nullptr;
    auto transactionController = RSSyncTransactionController::GetInstance();
    if (transactionController) {
        rsTransaction = transactionController->GetRSTransaction();
    }
    NotifyClientToUpdateRect("WMSPipeline", rsTransaction);
    return true;
}

bool SceneSession::NotifyServerToUpdateRect(const SessionUIParam& uiParam, SizeChangeReason reason)
{
    if (!GetForegroundInteractiveStatus()) {
        TLOGD(WmsLogTag::WMS_PIPELINE, "skip recent, id:%{public}d", GetPersistentId());
        return false;
    }
    if (uiParam.rect_.IsInvalid()) {
        TLOGW(WmsLogTag::WMS_PIPELINE, "id:%{public}d rect:%{public}s is invalid",
            GetPersistentId(), uiParam.rect_.ToString().c_str());
        return false;
    }
    auto globalRect = GetSessionGlobalRect();
    if (globalRect != uiParam.rect_) {
        UpdateAllModalUIExtensions(uiParam.rect_);
    }
    SetSessionGlobalRect(uiParam.rect_);
    if (!uiParam.needSync_ || !isNeedSyncSessionRect_) {
        TLOGD(WmsLogTag::WMS_LAYOUT, "id:%{public}d, scenePanelNeedSync:%{public}u needSyncSessionRect:%{public}u "
            "rectAfter:%{public}s preRect:%{public}s preGlobalRect:%{public}s", GetPersistentId(), uiParam.needSync_,
            isNeedSyncSessionRect_, uiParam.rect_.ToString().c_str(), winRect_.ToString().c_str(),
            globalRect.ToString().c_str());
        return false;
    }
    WSRect rect = { uiParam.rect_.posX_ - uiParam.transX_, uiParam.rect_.posY_ - uiParam.transY_,
        uiParam.rect_.width_, uiParam.rect_.height_ };
    if (winRect_ == rect && (!sessionStage_ || clientRect_ == rect)) {
        TLOGD(WmsLogTag::WMS_PIPELINE, "skip same rect update id:%{public}d rect:%{public}s preGlobalRect:%{public}s!",
            GetPersistentId(), rect.ToString().c_str(), globalRect.ToString().c_str());
        return false;
    }
    if (rect.IsInvalid()) {
        TLOGE(WmsLogTag::WMS_PIPELINE, "id:%{public}d rect:%{public}s is invalid, preGlobalRect:%{public}s",
            GetPersistentId(), rect.ToString().c_str(), globalRect.ToString().c_str());
        return false;
    }
    TLOGI(WmsLogTag::WMS_LAYOUT, "id:%{public}d, updateRect rectAfter:%{public}s preRect:%{public}s "
        "preGlobalRect:%{public}s clientRect:%{public}s", GetPersistentId(), rect.ToString().c_str(),
        winRect_.ToString().c_str(), globalRect.ToString().c_str(), clientRect_.ToString().c_str());
    winRect_ = rect;
    RectCheckProcess();
    return true;
}

void SceneSession::PostProcessNotifyAvoidArea()
{
    if (PipelineNeedNotifyClientToUpdateAvoidArea(dirtyFlags_)) {
        NotifyClientToUpdateAvoidArea();
    }
}

bool SceneSession::PipelineNeedNotifyClientToUpdateAvoidArea(uint32_t dirty) const
{
    return ((dirty & static_cast<uint32_t>(SessionUIDirtyFlag::VISIBLE)) && IsImmersiveType()) ||
        ((dirty & static_cast<uint32_t>(SessionUIDirtyFlag::AVOID_AREA)) && isVisible_);
}

void SceneSession::NotifyClientToUpdateAvoidArea()
{
    if (specificCallback_ == nullptr) {
        return;
    }
    // flush avoid areas on (avoid area dirty && (normal session rect NOT dirty || avoid session))
    if ((IsImmersiveType() || !IsDirtyWindow()) && specificCallback_->onUpdateAvoidArea_) {
        specificCallback_->onUpdateAvoidArea_(GetPersistentId());
    }
    if (specificCallback_->onUpdateOccupiedAreaIfNeed_) {
        specificCallback_->onUpdateOccupiedAreaIfNeed_(GetPersistentId());
    }
}

bool SceneSession::IsTransformNeedChange(float scaleX, float scaleY, float pivotX, float pivotY)
{
    bool nearEqual = NearEqual(scaleX_, scaleX) && NearEqual(scaleY_, scaleY) &&
        NearEqual(pivotX_, pivotX) && NearEqual(pivotY_, pivotY) &&
        NearEqual(clientScaleX_, scaleX) && NearEqual(clientScaleY_, scaleY) &&
        NearEqual(clientPivotX_, pivotX) && NearEqual(clientPivotY_, pivotY);
    return !nearEqual;
}

bool SceneSession::UpdateScaleInner(float scaleX, float scaleY, float pivotX, float pivotY)
{
    if (!IsTransformNeedChange(scaleX, scaleY, pivotX, pivotY)) {
        return false;
    }
    Session::SetScale(scaleX, scaleY, pivotX, pivotY);
    if (!IsSessionForeground()) {
        TLOGD(WmsLogTag::WMS_LAYOUT, "id:%{public}d, session is not foreground!", GetPersistentId());
        return false;
    }
    if (sessionStage_ != nullptr) {
        Transform transform;
        transform.scaleX_ = scaleX;
        transform.scaleY_ = scaleY;
        transform.pivotX_ = pivotX;
        transform.pivotY_ = pivotY;
        sessionStage_->NotifyTransformChange(transform);
        Session::SetClientScale(scaleX, scaleY, pivotX, pivotY);
    } else {
        WLOGFE("sessionStage is nullptr");
    }
    return true;
}

bool SceneSession::UpdateZOrderInner(uint32_t zOrder)
{
    if (zOrder_ == zOrder) {
        return false;
    }
    TLOGI(WmsLogTag::WMS_PIPELINE, "id: %{public}d, zOrder: %{public}u -> %{public}u, lastZOrder: %{public}u",
          GetPersistentId(), zOrder_, zOrder, lastZOrder_);
    lastZOrder_ = zOrder_;
    zOrder_ = zOrder;
    return true;
}

void SceneSession::SetPostProcessFocusState(PostProcessFocusState state)
{
    postProcessFocusState_ = state;
}

PostProcessFocusState SceneSession::GetPostProcessFocusState() const
{
    return postProcessFocusState_;
}

void SceneSession::ResetPostProcessFocusState()
{
    postProcessFocusState_.Reset();
}

void SceneSession::SetPostProcessProperty(bool state)
{
    postProcessProperty_ = state;
}

bool SceneSession::GetPostProcessProperty() const
{
    return postProcessProperty_;
}

bool SceneSession::IsImmersiveType() const
{
    WindowType type = GetWindowType();
    return type == WindowType::WINDOW_TYPE_STATUS_BAR ||
        type == WindowType::WINDOW_TYPE_NAVIGATION_BAR ||
        type == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT;
}

bool SceneSession::IsPcOrPadEnableActivation() const
{
    auto property = GetSessionProperty();
    bool isPcAppInPad = false;
    if (property != nullptr) {
        isPcAppInPad = property->GetIsPcAppInPad();
    }
    return systemConfig_.IsPcWindow() || IsFreeMultiWindowMode() || isPcAppInPad;
}

void SceneSession::SetMinimizedFlagByUserSwitch(bool isMinimized)
{
    TLOGI(WmsLogTag::WMS_MULTI_USER, "winId: %{public}d, isMinimized: %{public}d", GetPersistentId(), isMinimized);
    isMinimizedByUserSwitch_ = isMinimized;
}

bool SceneSession::IsMinimizedByUserSwitch() const
{
    return isMinimizedByUserSwitch_;
}

void SceneSession::UnregisterSessionChangeListeners()
{
    PostTask([weakThis = wptr(this), where = __func__] {
        auto session = weakThis.promote();
        if (session == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s session is null", where);
            return;
        }
        session->Session::UnregisterSessionChangeListeners();
    }, __func__);
}

void SceneSession::SetVisibilityChangedDetectFunc(VisibilityChangedDetectFunc&& func)
{
    LOCK_GUARD_EXPR(SCENE_GUARD, visibilityChangedDetectFunc_ = std::move(func));
}

void SceneSession::SetDefaultDisplayIdIfNeed()
{
    if (sessionInfo_.screenId_ == SCREEN_ID_INVALID) {
        auto defaultDisplayId = ScreenSessionManagerClient::GetInstance().GetDefaultScreenId();
        sessionInfo_.screenId_ = defaultDisplayId;
        TLOGI(WmsLogTag::WMS_LIFE, "winId: %{public}d, update screen id %{public}" PRIu64,
            GetPersistentId(), defaultDisplayId);
        auto sessionProperty = GetSessionProperty();
        if (sessionProperty) {
            sessionProperty->SetDisplayId(defaultDisplayId);
        }
    }
}

int32_t SceneSession::GetCustomDecorHeight() const
{
    std::lock_guard lock(customDecorHeightMutex_);
    return customDecorHeight_;
}

void SceneSession::SetCustomDecorHeight(int32_t height)
{
    constexpr int32_t MIN_DECOR_HEIGHT = 37;
    constexpr int32_t MAX_DECOR_HEIGHT = 112;
    if (height < MIN_DECOR_HEIGHT || height > MAX_DECOR_HEIGHT) {
        return;
    }
    std::lock_guard lock(customDecorHeightMutex_);
    customDecorHeight_ = height;
}

void SceneSession::UpdateGestureBackEnabled()
{
    if (specificCallback_ != nullptr &&
        specificCallback_->onUpdateGestureBackEnabled_ != nullptr) {
        specificCallback_->onUpdateGestureBackEnabled_(GetPersistentId());
    }
}

bool SceneSession::CheckIdentityTokenIfMatched(const std::string& identityToken)
{
    if (!identityToken.empty() && !clientIdentityToken_.empty() && identityToken != clientIdentityToken_) {
        TLOGW(WmsLogTag::WMS_LIFE,
            "failed, clientIdentityToken: %{public}s, identityToken: %{public}s, bundleName: %{public}s",
            clientIdentityToken_.c_str(), identityToken.c_str(), GetSessionInfo().bundleName_.c_str());
        return false;
    }
    return true;
}

bool SceneSession::CheckPidIfMatched()
{
    int32_t callingPid = IPCSkeleton::GetCallingPid();
    if (callingPid != -1 && callingPid != GetCallingPid()) {
        TLOGW(WmsLogTag::WMS_LIFE,
            "failed, callingPid_: %{public}d, callingPid: %{public}d, bundleName: %{public}s",
            GetCallingPid(), callingPid, GetSessionInfo().bundleName_.c_str());
        return false;
    }
    return true;
}

void SceneSession::SetNeedSyncSessionRect(bool needSync)
{
    PostTask([weakThis = wptr(this), needSync, where = __func__] {
        auto session = weakThis.promote();
        if (session == nullptr) {
            TLOGNE(WmsLogTag::WMS_PIPELINE, "%{public}s session is null", where);
            return;
        }
        TLOGNI(WmsLogTag::WMS_PIPELINE,
            "%{public}s: change isNeedSync from %{public}d to %{public}d, id:%{public}d",
            where, session->isNeedSyncSessionRect_, needSync, session->GetPersistentId());
        session->isNeedSyncSessionRect_ = needSync;
    }, __func__);
}

bool SceneSession::SetFrameGravity(Gravity gravity)
{
    auto surfaceNode = GetSurfaceNode();
    if (surfaceNode == nullptr) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "fail id:%{public}d gravity:%{public}d", GetPersistentId(), gravity);
        return false;
    }
    TLOGI(WmsLogTag::WMS_LAYOUT, "id:%{public}d gravity:%{public}d", GetPersistentId(), gravity);
    surfaceNode->SetFrameGravity(gravity);
    return true;
}

void SceneSession::SetWindowRectAutoSaveCallback(NotifySetWindowRectAutoSaveFunc&& func)
{
    PostTask([weakThis = wptr(this), where = __func__, func = std::move(func)] {
        auto session = weakThis.promote();
        if (!session || !func) {
            TLOGNE(WmsLogTag::WMS_MAIN, "%{public}s session or onSetWindowRectAutoSaveFunc is null", where);
            return;
        }
        session->onSetWindowRectAutoSaveFunc_ = std::move(func);
        TLOGNI(WmsLogTag::WMS_MAIN, "%{public}s id: %{public}d", where,
            session->GetPersistentId());
    }, __func__);
}

void SceneSession::SetIsSystemKeyboard(bool isSystemKeyboard)
{
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "property is nullptr");
        return;
    }
    sessionProperty->SetIsSystemKeyboard(isSystemKeyboard);
}

bool SceneSession::IsSystemKeyboard() const
{
    auto sessionProperty = GetSessionProperty();
    if (sessionProperty == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "property is nullptr");
        return false;
    }
    return sessionProperty->IsSystemKeyboard();
}

void SceneSession::RegisterSupportWindowModesCallback(NotifySetSupportedWindowModesFunc&& func)
{
    PostTask([weakThis = wptr(this), func = std::move(func), where = __func__] {
        auto session = weakThis.promote();
        if (!session || !func) {
            TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s session or func is null", where);
            return;
        }
        session->onSetSupportedWindowModesFunc_ = std::move(func);
        TLOGND(WmsLogTag::WMS_LAYOUT_PC, "%{public}s id: %{public}d", where, session->GetPersistentId());
    }, __func__);
}

void SceneSession::ActivateKeyboardAvoidArea(bool active, bool recalculateAvoid)
{
    if (recalculateAvoid && !active) {
        RestoreCallingSession();
    }
    keyboardAvoidAreaActive_ = active;
    if (recalculateAvoid && active) {
        EnableCallingSessionAvoidArea();
    }
}

bool SceneSession::IsKeyboardAvoidAreaActive() const
{
    return keyboardAvoidAreaActive_;
}

void SceneSession::MarkAvoidAreaAsDirty()
{
    dirtyFlags_ |= static_cast<uint32_t>(SessionUIDirtyFlag::AVOID_AREA);
}

void SceneSession::SetMousePointerDownEventStatus(bool mousePointerDownEventStatus)
{
    isMousePointerDownEventStatus_ = mousePointerDownEventStatus;
}

bool SceneSession::GetMousePointerDownEventStatus() const
{
    return isMousePointerDownEventStatus_;
}

void SceneSession::SetFingerPointerDownStatus(int32_t fingerId)
{
    if (fingerPointerDownStatusList_.find(fingerId) != fingerPointerDownStatusList_.end()) {
        TLOGE(WmsLogTag::WMS_EVENT, "wid:%{public}d fingerId:%{public}d receive twice down event",
              GetPersistentId(), fingerId);
    } else {
        fingerPointerDownStatusList_.insert(fingerId);
    }
}

void SceneSession::RemoveFingerPointerDownStatus(int32_t fingerId)
{
    if (fingerPointerDownStatusList_.find(fingerId) == fingerPointerDownStatusList_.end()) {
        TLOGE(WmsLogTag::WMS_EVENT, "wid:%{public}d fingerId:%{public}d receive up without down event",
              GetPersistentId(), fingerId);
    } else {
        fingerPointerDownStatusList_.erase(fingerId);
    }
}

std::unordered_set<int32_t> SceneSession::GetFingerPointerDownStatusList() const
{
    return fingerPointerDownStatusList_;
}

void SceneSession::SetBehindWindowFilterEnabled(bool enabled)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "id: %{public}d, enabled: %{public}d", GetPersistentId(), enabled);
    auto surfaceNode = GetSurfaceNode();
    if (surfaceNode == nullptr) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "fail to get surfaceNode");
        return;
    }
    auto rsTransaction = RSTransactionProxy::GetInstance();
    if (rsTransaction != nullptr) {
        TLOGD(WmsLogTag::WMS_LAYOUT, "begin rsTransaction");
        rsTransaction->Begin();
    }

    if (behindWindowFilterEnabledModifier_ == nullptr) {
        auto behindWindowFilterEnabledProperty = std::make_shared<RSProperty<bool>>(enabled);
        behindWindowFilterEnabledModifier_ = std::make_shared<RSBehindWindowFilterEnabledModifier>(
            behindWindowFilterEnabledProperty);
        surfaceNode->AddModifier(behindWindowFilterEnabledModifier_);
    } else {
        auto behindWindowFilterEnabledProperty = std::static_pointer_cast<RSProperty<bool>>(
            behindWindowFilterEnabledModifier_->GetProperty());
        if (!behindWindowFilterEnabledProperty) {
            TLOGE(WmsLogTag::WMS_LAYOUT, "fail to get property");
            return;
        }
        behindWindowFilterEnabledProperty->Set(enabled);
    }

    if (rsTransaction != nullptr) {
        TLOGD(WmsLogTag::WMS_LAYOUT, "commit rsTransaction");
        rsTransaction->Commit();
    }
}

void SceneSession::UpdateAllModalUIExtensions(const WSRect& globalRect)
{
    if (modalUIExtensionInfoList_.empty()) {
        return;
    }
    PostTask([weakThis = wptr(this), where = __func__, globalRect] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_UIEXT, "%{public}s session is null", where);
            return;
        }
        auto parentTransX = globalRect.posX_ - session->GetClientRect().posX_;
        auto parentTransY = globalRect.posY_ - session->GetClientRect().posY_;
        {
            std::unique_lock<std::shared_mutex> lock(session->modalUIExtensionInfoListMutex_);
            for (auto& extensionInfo : session->modalUIExtensionInfoList_) {
                if (!extensionInfo.hasUpdatedRect) {
                    continue;
                }
                extensionInfo.windowRect = extensionInfo.uiExtRect;
                extensionInfo.windowRect.posX_ += parentTransX;
                extensionInfo.windowRect.posY_ += parentTransY;
            }
        }
        session->NotifySessionInfoChange();
        TLOGNI(WmsLogTag::WMS_UIEXT, "%{public}s: id: %{public}d, globalRect: %{public}s, parentTransX: %{public}d, "
            "parentTransY: %{public}d", where, session->GetPersistentId(), globalRect.ToString().c_str(),
            parentTransX, parentTransY);
    }, __func__);
}

void SceneSession::SetWindowCornerRadiusCallback(NotifySetWindowCornerRadiusFunc&& func)
{
    const char* const where = __func__;
    PostTask([weakThis = wptr(this), where, func = std::move(func)] {
        auto session = weakThis.promote();
        if (!session || !func) {
            TLOGNE(WmsLogTag::WMS_ATTRIBUTE, "session or onSetWindowCornerRadiusFunc is null");
            return;
        }
        session->onSetWindowCornerRadiusFunc_ = std::move(func);
        auto property = session->GetSessionProperty();
        session->onSetWindowCornerRadiusFunc_(property->GetWindowCornerRadius());
        TLOGNI(WmsLogTag::WMS_ATTRIBUTE, "%{public}s id: %{public}d", where,
            session->GetPersistentId());
    }, __func__);
}

WSError SceneSession::SetWindowCornerRadius(float cornerRadius)
{
    const char* const where = __func__;
    PostTask([weakThis = wptr(this), cornerRadius, where] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_ATTRIBUTE, "session is null");
            return;
        }
        if (session->onSetWindowCornerRadiusFunc_) {
            TLOGND(WmsLogTag::WMS_ATTRIBUTE, "%{public}s id %{public}d radius: %{public}f",
                where, session->GetPersistentId(), cornerRadius);
            session->onSetWindowCornerRadiusFunc_(cornerRadius);
            session->GetSessionProperty()->SetWindowCornerRadius(cornerRadius);
        }
    }, __func__);
    return WSError::WS_OK;
}

void SceneSession::UpdateSubWindowLevel(uint32_t subWindowLevel)
{
    GetSessionProperty()->SetSubWindowLevel(subWindowLevel);
    for (const auto& session : GetSubSession()) {
        if (session != nullptr) {
            session->UpdateSubWindowLevel(subWindowLevel + 1);
        }
    }
}

int SceneSession::GetMaxSubWindowLevel() const
{
    int maxSubWindowLevel = 1;
    for (const auto& session : GetSubSession()) {
        if (session != nullptr) {
            maxSubWindowLevel = std::max(maxSubWindowLevel, session->GetMaxSubWindowLevel() + 1);
        }
    }
    return maxSubWindowLevel;
}

void SceneSession::SetColorSpace(ColorSpace colorSpace)
{
    auto surfaceNode = GetSurfaceNode();
    if (!surfaceNode) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "surfaceNode is invalid");
        return;
    }
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "SetColorSpace value=%{public}u", colorSpace);
    auto colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    if (colorSpace == ColorSpace::COLOR_SPACE_WIDE_GAMUT) {
        colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3;
    }
    auto rsTransaction = RSTransactionProxy::GetInstance();
    if (rsTransaction != nullptr) {
        rsTransaction->Begin();
        surfaceNode->SetColorSpace(colorGamut);
    }
    if (rsTransaction != nullptr) {
        rsTransaction->Commit();
    }
}

void SceneSession::AddSidebarBlur()
{
    auto surfaceNode = GetSurfaceNode();
    if (!surfaceNode) {
        TLOGE(WmsLogTag::WMS_PC, "surfaceNode is null");
        return;
    }
    auto rsNodeTemp = Rosen::RSNodeMap::Instance().GetNode(surfaceNode->GetId());
    if (rsNodeTemp) {
        std::shared_ptr<AbilityRuntime::ApplicationContext> appContext =
        AbilityRuntime::Context::GetApplicationContext();
        if (appContext == nullptr) {
            TLOGE(WmsLogTag::WMS_PC, "app context is nullptr");
            return;
        }
        std::shared_ptr<AppExecFwk::Configuration> config = appContext->GetConfiguration();
        if (config == nullptr) {
            TLOGE(WmsLogTag::WMS_PC, "app configuration is nullptr");
            return;
        }
        std::string colorMode = config->GetItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE);
        bool isDark = (colorMode == AppExecFwk::ConfigurationInner::COLOR_MODE_DARK);
        AddRSNodeModifier(isDark, rsNodeTemp);
    }
}

void SceneSession::AddRSNodeModifier(bool isDark, const std::shared_ptr<RSBaseNode>& rsNode)
{
    if (!rsNode) {
        TLOGE(WmsLogTag::WMS_PC, "rsNode is nullptr");
        return;
    }
    TLOGI(WmsLogTag::WMS_PC, "isDark: %{public}d", isDark);
    if (isDark) {
        blurRadiusValue_ = std::make_shared<Rosen::RSAnimatableProperty<float>>(
            SIDEBAR_DEFAULT_RADIUS_DARK);
        blurSaturationValue_ = std::make_shared<Rosen::RSAnimatableProperty<float>>(
            SIDEBAR_DEFAULT_SATURATION_DARK);
        blurBrightnessValue_ = std::make_shared<Rosen::RSAnimatableProperty<float>>(
            SIDEBAR_DEFAULT_BRIGHTNESS_DARK);
        blurMaskColorValue_ = std::make_shared<RSAnimatableProperty<Rosen::RSColor>>(
            Rosen::RSColor::FromArgbInt(SIDEBAR_DEFAULT_MASKCOLOR_DARK));
    } else {
        blurRadiusValue_ = std::make_shared<Rosen::RSAnimatableProperty<float>>(
            SIDEBAR_DEFAULT_RADIUS_LIGHT);
        blurSaturationValue_ = std::make_shared<Rosen::RSAnimatableProperty<float>>(
            SIDEBAR_DEFAULT_SATURATION_LIGHT);
        blurBrightnessValue_ = std::make_shared<Rosen::RSAnimatableProperty<float>>(
            SIDEBAR_DEFAULT_BRIGHTNESS_LIGHT);
        blurMaskColorValue_ = std::make_shared<RSAnimatableProperty<Rosen::RSColor>>(
            Rosen::RSColor::FromArgbInt(SIDEBAR_DEFAULT_MASKCOLOR_LIGHT));
    }

    std::shared_ptr<Rosen::RSBehindWindowFilterRadiusModifier> radius =
        std::make_shared<Rosen::RSBehindWindowFilterRadiusModifier>(blurRadiusValue_);
    rsNode->AddModifier(radius);
    std::shared_ptr<Rosen::RSBehindWindowFilterSaturationModifier> saturation =
        std::make_shared<Rosen::RSBehindWindowFilterSaturationModifier>(blurSaturationValue_);
    rsNode->AddModifier(saturation);
    std::shared_ptr<Rosen::RSBehindWindowFilterBrightnessModifier> brightness =
        std::make_shared<Rosen::RSBehindWindowFilterBrightnessModifier>(blurBrightnessValue_);
    rsNode->AddModifier(brightness);
    std::shared_ptr<Rosen::RSBehindWindowFilterMaskColorModifier> modifier =
        std::make_shared<Rosen::RSBehindWindowFilterMaskColorModifier>(blurMaskColorValue_);
    rsNode->AddModifier(modifier);
}

void SceneSession::SetSidebarBlur(bool isDefaultSidebarBlur)
{
    TLOGI(WmsLogTag::WMS_PC, "isDefaultSidebarBlur: %{public}d", isDefaultSidebarBlur);
    auto surfaceNode = GetSurfaceNode();
    if (!surfaceNode) {
        TLOGE(WmsLogTag::WMS_PC, "surfaceNode is null");
        return;
    }
    if (!blurRadiusValue_ || !blurSaturationValue_ || !blurBrightnessValue_ || !blurMaskColorValue_) {
        TLOGE(WmsLogTag::WMS_PC, "RSAnimatableProperty is null");
        return;
    }

    std::shared_ptr<AbilityRuntime::ApplicationContext> appContext =
        AbilityRuntime::Context::GetApplicationContext();
    if (appContext == nullptr) {
        TLOGE(WmsLogTag::WMS_PC, "app context is nullptr");
        return;
    }
    std::shared_ptr<AppExecFwk::Configuration> config = appContext->GetConfiguration();
    if (config == nullptr) {
        TLOGE(WmsLogTag::WMS_PC, "app configuration is nullptr");
        return;
    }
    std::string colorMode = config->GetItem(AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE);
    bool isDark = (colorMode == AppExecFwk::ConfigurationInner::COLOR_MODE_DARK);
    ModifyRSAnimatableProperty(isDefaultSidebarBlur, isDark);
}

void SceneSession::ModifyRSAnimatableProperty(bool isDefaultSidebarBlur, bool isDark)
{
    TLOGI(WmsLogTag::WMS_PC, "isDefaultSidebarBlur: %{public}d, isDark: %{public}d",
        isDefaultSidebarBlur, isDark);
    // sidebar animation duration
    constexpr int32_t duration = 150;
    if (isDefaultSidebarBlur) {
        Rosen::RSAnimationTimingProtocol timingProtocol;
        timingProtocol.SetDuration(duration);
        timingProtocol.SetDirection(true);
        timingProtocol.SetFillMode(Rosen::FillMode::FORWARDS);
        timingProtocol.SetFinishCallbackType(Rosen::FinishCallbackType::LOGICALLY);

        Rosen::RSNode::OpenImplicitAnimation(timingProtocol, Rosen::RSAnimationTimingCurve::LINEAR, nullptr);
        if (isDark) {
            blurRadiusValue_->Set(SIDEBAR_DEFAULT_RADIUS_DARK);
            blurSaturationValue_->Set(SIDEBAR_DEFAULT_SATURATION_DARK);
            blurBrightnessValue_->Set(SIDEBAR_DEFAULT_BRIGHTNESS_DARK);
            blurMaskColorValue_->Set(Rosen::RSColor::FromArgbInt(SIDEBAR_DEFAULT_MASKCOLOR_DARK));
        } else {
            blurRadiusValue_->Set(SIDEBAR_DEFAULT_RADIUS_LIGHT);
            blurSaturationValue_->Set(SIDEBAR_DEFAULT_SATURATION_LIGHT);
            blurBrightnessValue_->Set(SIDEBAR_DEFAULT_BRIGHTNESS_LIGHT);
            blurMaskColorValue_->Set(Rosen::RSColor::FromArgbInt(SIDEBAR_DEFAULT_MASKCOLOR_LIGHT));
        }
        Rosen::RSNode::CloseImplicitAnimation();
    } else {
        blurRadiusValue_->Set(SIDEBAR_BLUR_NUMBER_ZERO);
        blurSaturationValue_->Set(SIDEBAR_BLUR_NUMBER_ZERO);
        blurBrightnessValue_->Set(SIDEBAR_BLUR_NUMBER_ZERO);
        if (isDark) {
            blurMaskColorValue_->Set(Rosen::RSColor::FromArgbInt(SIDEBAR_SNAPSHOT_MASKCOLOR_DARK));
        } else {
            blurMaskColorValue_->Set(Rosen::RSColor::FromArgbInt(SIDEBAR_SNAPSHOT_MASKCOLOR_LIGHT));
        }
    }
}

void SceneSession::NotifyWindowAttachStateListenerRegistered(bool registered)
{
    SetNeedNotifyAttachState(registered);
}

void SceneSession::NotifyKeyboardAnimationCompleted(bool isShowAnimation,
    const WSRect& beginRect, const WSRect& endRect)
{
    PostTask([weakThis = wptr(this), isShowAnimation, beginRect, endRect, where = __func__] {
        auto session = weakThis.promote();
        if (!session) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "%{public}s session is null", where);
            return;
        }
        if (session->sessionStage_ == nullptr) {
            TLOGNI(WmsLogTag::WMS_KEYBOARD, "%{public}s sessionStage_ is null, id: %{public}d",
                where, session->GetPersistentId());
            return;
        }
        if (isShowAnimation && !session->isKeyboardDidShowRegistered_.load()) {
            TLOGND(WmsLogTag::WMS_KEYBOARD, "keyboard did show listener is not registered");
            return;
        }
        if (!isShowAnimation && !session->isKeyboardDidHideRegistered_.load()) {
            TLOGND(WmsLogTag::WMS_KEYBOARD, "keyboard did hide listener is not registered");
            return;
        }
    
        KeyboardPanelInfo keyboardPanelInfo;
        keyboardPanelInfo.beginRect_ = SessionHelper::TransferToRect(beginRect);
        keyboardPanelInfo.endRect_ = SessionHelper::TransferToRect(endRect);
        keyboardPanelInfo.isShowing_ = isShowAnimation;
        session->sessionStage_->NotifyKeyboardAnimationCompleted(keyboardPanelInfo);
    }, __func__);
}

void SceneSession::NotifyKeyboardDidShowRegistered(bool registered)
{
    isKeyboardDidShowRegistered_.store(registered);
}

void SceneSession::NotifyKeyboardDidHideRegistered(bool registered)
{
    isKeyboardDidHideRegistered_.store(registered);
}

WSError SceneSession::UpdateDensity()
{
    if (systemConfig_.IsPcWindow() && WindowHelper::IsAppWindow(GetWindowType()) &&
        Session::GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING) {
        UpdateNewSizeForPCWindow();
    }
    SaveLastDensity();
    return Session::UpdateDensity();
}

void SceneSession::UpdateNewSizeForPCWindow()
{
    if (moveDragController_ && moveDragController_->IsMoveDragHotAreaCrossDisplay()) {
        TLOGW(WmsLogTag::WMS_LAYOUT_PC, "is move drag to hot area, do nothing.");
        moveDragController_->SetMoveDragHotAreaCrossDisplay(false);
        return;
    }

    if (auto property = GetSessionProperty()) {
        DisplayId displayId = property->GetDisplayId();
        auto display = DisplayManager::GetInstance().GetDisplayById(displayId);
        if (display == nullptr) {
            TLOGE(WmsLogTag::WMS_LAYOUT_PC, "get available area failed!");
            return;
        }

        DMRect availableArea = { 0, 0, 0, 0 };
        DMError ret = display->GetAvailableArea(availableArea);
        if (ret != DMError::DM_OK || availableArea.IsUninitializedRect()) {
            TLOGE(WmsLogTag::WMS_LAYOUT_PC, "get available area failed!");
            return;
        }

        float newVpr = display->GetVirtualPixelRatio();
        if (CalcNewWindowRectIfNeed(availableArea, newVpr)) {
            NotifySessionRectChange(winRect_, SizeChangeReason::UPDATE_DPI_SYNC);
            sessionStage_->UpdateRect(winRect_, SizeChangeReason::UPDATE_DPI_SYNC);
            TLOGI(WmsLogTag::WMS_LAYOUT_PC, "left: %{public}d, top: %{public}d, width: %{public}u, "
                "height: %{public}u, Id: %{public}u", winRect_.posX_, winRect_.posY_, winRect_.width_,
                winRect_.height_, GetPersistentId());
        }
    }
    displayChangedByMoveDrag_ = false;
}

bool SceneSession::CalcNewWindowRectIfNeed(DMRect& availableArea, float newVpr)
{
    float currVpr = 0.0f;
    if (auto property = GetSessionProperty()) {
        currVpr = property->GetLastLimitsVpr();
        TLOGD(WmsLogTag::WMS_LAYOUT_PC, "currVpr: %{public}f Id: %{public}u", currVpr, GetPersistentId());
    }
    if (MathHelper::NearZero(currVpr - newVpr) || MathHelper::NearZero(currVpr)) {
        TLOGW(WmsLogTag::WMS_LAYOUT_PC, "need not update new rect, currVpr: %{public}f newVpr: %{public}f "
            "Id: %{public}u", currVpr, newVpr, GetPersistentId());
        return false;
    }
    int32_t left = winRect_.posX_;
    int32_t top = winRect_.posY_;
    uint32_t width = static_cast<uint32_t>(winRect_.width_ * newVpr / currVpr);
    uint32_t height = static_cast<uint32_t>(winRect_.height_ * newVpr / currVpr);
    int32_t statusBarHeight = 0;
    if (isStatusBarVisible_ && IsPrimaryDisplay()) {
        statusBarHeight = GetStatusBarHeight();
    }
    width = MathHelper::Min(width, availableArea.width_);
    height = MathHelper::Min(height, availableArea.height_);
    bool needMove = top < statusBarHeight || left < 0 ||
        top > static_cast<int32_t>(availableArea.height_ - height) ||
        left > static_cast<int32_t>(availableArea.width_ - width) || displayChangedByMoveDrag_;
    if (displayChangedByMoveDrag_ && moveDragController_) {
        int32_t pointerPosX = moveDragController_->GetLastMovePointerPosX();
        left = pointerPosX - static_cast<int32_t>((pointerPosX -left) * newVpr / currVpr);
    } else {
        top = MathHelper::Min(top, static_cast<int32_t>(availableArea.height_ - height));
        top = MathHelper::Max(top, statusBarHeight);
        left = MathHelper::Min(left, static_cast<int32_t>(availableArea.width_ - width));
        left = MathHelper::Max(left, 0);
    }
    winRect_.width_ = width;
    winRect_.height_ = height;
    if (needMove) {
        winRect_.posX_ = left;
        winRect_.posY_ = top;
    }
    return true;
}

bool SceneSession::IsPrimaryDisplay() const
{
    if (auto property = GetSessionProperty()) {
        auto display = DisplayManager::GetInstance().GetPrimaryDisplaySync();
        if (display == nullptr) {
            TLOGE(WmsLogTag::WMS_LAYOUT_PC, "display is null.");
            return false;
        }
        if (auto displayInfo = display->GetDisplayInfo()) {
            return property->GetDisplayId() == displayInfo->GetDisplayId();
        }
    }
    return false;
}

void SceneSession::SaveLastDensity()
{
    if (auto property = GetSessionProperty()) {
        DisplayId displayId = property->GetDisplayId();
        if (auto display = DisplayManager::GetInstance().GetDisplayById(displayId)) {
            property->SetLastLimitsVpr(display->GetVirtualPixelRatio());
        }
    }
}

void SceneSession::NotifyUpdateFlagCallback(NotifyUpdateFlagFunc&& func)
{
    const char* const where = __func__;
    PostTask([weakThis = wptr(this), where, func = std::move(func)] {
        auto session = weakThis.promote();
        if (!session || !func) {
            TLOGNE(WmsLogTag::WMS_MAIN, "session or func is null");
            return;
        }
        session->onUpdateFlagFunc_ = std::move(func);
        session->onUpdateFlagFunc_(session->sessionInfo_.specifiedFlag_);
        TLOGND(WmsLogTag::WMS_MAIN, "%{public}s id: %{public}d specifiedFlag: %{public}s", where,
            session->GetPersistentId(), session->sessionInfo_.specifiedFlag_.c_str());
    }, __func__);
}

void SceneSession::SetIsAncoForFloatingWindow(bool isAncoForFloatingWindow)
{
    isAncoForFloatingWindow_ = isAncoForFloatingWindow;
}

bool SceneSession::GetIsAncoForFloatingWindow() const
{
    return isAncoForFloatingWindow_;
}

std::unordered_map<std::string, std::unordered_map<ControlAppType, SceneSession::ControlInfo>>& SceneSession::GetAllAppUseControlMap()
{
    return allAppUseControlMap_;
}
} // namespace OHOS::Rosen
