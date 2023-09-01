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

#include <hitrace_meter.h>
#include <ipc_skeleton.h>
#include <pointer_event.h>
#include <transaction/rs_transaction.h>
#include <ui/rs_surface_node.h>

#include "../../proxy/include/window_info.h"

#include "common/include/session_permission.h"
#include "interfaces/include/ws_common.h"
#include "pixel_map.h"
#include "session/host/include/scene_persistent_storage.h"
#include "session/host/include/session_utils.h"
#include "session_manager/include/screen_session_manager.h"
#include "session_helper.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "wm_math.h"
#include <running_lock.h>

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSession" };
const std::string DLP_INDEX = "ohos.dlp.params.index";
} // namespace

MaximizeMode SceneSession::maximizeMode_ = MaximizeMode::MODE_RECOVER;
wptr<SceneSession> SceneSession::enterSession_ = nullptr;
std::mutex SceneSession::enterSessionMutex_;

SceneSession::SceneSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback)
    : Session(info)
{
    GeneratePersistentId(false, info.persistentId_);
    if (!info.bundleName_.empty()) {
        scenePersistence_ = new ScenePersistence(info.bundleName_, GetPersistentId());
    }
    specificCallback_ = specificCallback;
    moveDragController_ = new (std::nothrow) MoveDragController(GetPersistentId());
    SetMoveDragCallback();
    std::string key = GetRatioPreferenceKey();
    if (!key.empty()) {
        if (ScenePersistentStorage::HasKey(key, ScenePersistentStorageType::ASPECT_RATIO)) {
            ScenePersistentStorage::Get(key, aspectRatio_, ScenePersistentStorageType::ASPECT_RATIO);
            WLOGD("SceneSession init aspectRatio , key %{public}s, value: %{public}f", key.c_str(), aspectRatio_);
            if (moveDragController_) {
                moveDragController_->SetAspectRatio(aspectRatio_);
            }
        }
    }
    property_ = new(std::nothrow) WindowSessionProperty();
    if (property_) {
        property_->SetWindowType(static_cast<WindowType>(info.windowType_));
    }

    auto name = sessionInfo_.bundleName_;
    auto pos = name.find_last_of('.');
    name = (pos == std::string::npos) ? name : name.substr(pos + 1); // skip '.'

    if (WindowHelper::IsMainWindow(GetWindowType())) {
        Rosen::RSSurfaceNodeConfig leashWinconfig;
        leashWinconfig.SurfaceNodeName = "WindowScene_" + name + std::to_string(GetPersistentId());
        leashWinSurfaceNode_ = Rosen::RSSurfaceNode::Create(leashWinconfig,
            Rosen::RSSurfaceNodeType::LEASH_WINDOW_NODE);
    }

    if (sessionInfo_.isSystem_) {
        Rosen::RSSurfaceNodeConfig config;
        config.SurfaceNodeName = name;
        surfaceNode_ = Rosen::RSSurfaceNode::Create(config, Rosen::RSSurfaceNodeType::APP_WINDOW_NODE);
    }
}

WSError SceneSession::Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
    sptr<WindowSessionProperty> property, sptr<IRemoteObject> token)
{
    // Get pid and uid before posting task.
    auto pid = IPCSkeleton::GetCallingPid();
    auto uid = IPCSkeleton::GetCallingUid();
    return PostSyncTask(
        [weakThis = wptr(this), sessionStage, eventChannel, surfaceNode, &systemConfig, property, token, pid, uid]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        auto ret = session->Session::Connect(
            sessionStage, eventChannel, surfaceNode, systemConfig, property, token);
        if (ret != WSError::WS_OK) {
            return ret;
        }
        session->SetCallingPid(pid);
        session->SetCallingUid(uid);
        session->NotifyPropertyWhenConnect();
        return ret;
    });
}

WSError SceneSession::Foreground(sptr<WindowSessionProperty> property)
{
    PostTask([weakThis = wptr(this), property]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        // use property from client
        if (property && property->GetAnimationFlag() == static_cast<uint32_t>(WindowAnimation::CUSTOM)) {
            session->property_->SetAnimationFlag(static_cast<uint32_t>(WindowAnimation::CUSTOM));
            session->NotifyIsCustomAnimationPlaying(true);
            if (session->setWindowScenePatternFunc_ && session->setWindowScenePatternFunc_->setOpacityFunc_) {
                session->setWindowScenePatternFunc_->setOpacityFunc_(0.0f);
            }
        }
        auto ret = session->Session::Foreground(property);
        if (ret != WSError::WS_OK) {
            return ret;
        }
        session->NotifyForeground();
        session->UpdateCameraFloatWindowStatus(true);
        if (session->specificCallback_ != nullptr) {
            session->specificCallback_->onUpdateAvoidArea_(session->GetPersistentId());
            session->specificCallback_->onWindowInfoUpdate_(
                session->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_ADDED);
        }
        return WSError::WS_OK;
    });
    return WSError::WS_OK;
}

WSError SceneSession::Background()
{
    PostTask([weakThis = wptr(this)]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        // background will remove surfaceNode, custom not execute
        // not animation playing when already background; inactive may be animation playing
        if (session->property_ &&
            session->property_->GetAnimationFlag() == static_cast<uint32_t>(WindowAnimation::CUSTOM)) {
            session->NotifyIsCustomAnimationPlaying(true);
            return WSError::WS_OK;
        }
        auto ret = session->Session::Background();
        if (ret != WSError::WS_OK) {
            return ret;
        }
        if (WindowHelper::IsMainWindow(session->GetWindowType())) {
            session->snapshot_ = session->Snapshot();
            if (session->scenePersistence_ && session->snapshot_) {
                session->scenePersistence_->SaveSnapshot(session->snapshot_);
            }
        }
        session->NotifyBackground();
        session->snapshot_.reset();
        session->UpdateCameraFloatWindowStatus(false);
        if (session->specificCallback_ != nullptr) {
            session->specificCallback_->onUpdateAvoidArea_(session->GetPersistentId());
            session->specificCallback_->onWindowInfoUpdate_(
                session->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_REMOVED);
        }
        return WSError::WS_OK;
    });
    return WSError::WS_OK;
}

WSError SceneSession::Disconnect()
{
    PostTask([weakThis = wptr(this)]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        auto state = session->GetSessionState();
        if (state == SessionState::STATE_ACTIVE && WindowHelper::IsMainWindow(session->GetWindowType())) {
            session->snapshot_ = session->Snapshot();
            if (session->scenePersistence_ && session->snapshot_) {
                session->scenePersistence_->SaveSnapshot(session->snapshot_);
            }
            session->isActive_ = false;
        }
        session->Session::Disconnect();
        session->snapshot_.reset();
        session->isTerminating = false;
        if (session->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
            session->NotifyCallingSessionBackground();
        }
        return WSError::WS_OK;
    });
    return WSError::WS_OK;
}

WSError SceneSession::UpdateActiveStatus(bool isActive)
{
    PostTask([weakThis = wptr(this), isActive]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (!session->IsSessionValid()) {
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        if (isActive == session->isActive_) {
            WLOGFD("Session active do not change: %{public}d", isActive);
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
        WLOGFD("UpdateActiveStatus, isActive: %{public}d, state: %{public}u",
            session->isActive_, session->GetSessionState());
        return ret;
    });
    return WSError::WS_OK;
}

WSError SceneSession::UpdateWindowSessionProperty(sptr<WindowSessionProperty> property)
{
    PostTask([weakThis = wptr(this), property]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        session->property_ = property;
        return WSError::WS_OK;
    });
    return WSError::WS_OK;
}

WSError SceneSession::OnSessionEvent(SessionEvent event)
{
    PostTask([weakThis = wptr(this), event]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        WLOGFD("SceneSession OnSessionEvent event: %{public}d", static_cast<int32_t>(event));
        if (event == SessionEvent::EVENT_START_MOVE && session->moveDragController_ &&
            !session->moveDragController_->GetStartDragFlag()) {
            HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSession::StartMove");
            session->moveDragController_->InitMoveDragProperty();
            session->moveDragController_->SetStartMoveFlag(true);
            session->moveDragController_->ClacFirstMoveTargetRect(session->winRect_);
        }
        if (session->sessionChangeCallback_ && session->sessionChangeCallback_->OnSessionEvent_) {
            session->sessionChangeCallback_->OnSessionEvent_(static_cast<uint32_t>(event));
        }
        return WSError::WS_OK;
    });
    return WSError::WS_OK;
}

void SceneSession::RegisterSessionChangeCallback(const sptr<SceneSession::SessionChangeCallback>&
    sessionChangeCallback)
{
    std::lock_guard<std::mutex> guard(sessionChangeCbMutex_);
    sessionChangeCallback_ = sessionChangeCallback;
}

WSError SceneSession::SetGlobalMaximizeMode(MaximizeMode mode)
{
    PostTask([weakThis = wptr(this), mode]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        WLOGFI("SceneSession SetGlobalMaximizeMode mode: %{public}u", static_cast<uint32_t>(mode));
        session->maximizeMode_ = mode;
        ScenePersistentStorage::Insert("maximize_state", static_cast<int32_t>(session->maximizeMode_),
            ScenePersistentStorageType::MAXIMIZE_STATE);
        return WSError::WS_OK;
    });
    return WSError::WS_OK;
}

WSError SceneSession::GetGlobalMaximizeMode(MaximizeMode &mode)
{
    return PostSyncTask([weakThis = wptr(this), &mode]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        WLOGFD("SceneSession GetGlobalMaximizeMode");
        mode = maximizeMode_;
        return WSError::WS_OK;
    });
}

WSError SceneSession::SetAspectRatio(float ratio)
{
    PostTask([weakThis = wptr(this), ratio]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (!session->property_) {
            WLOGE("SetAspectRatio failed because property is null");
            return WSError::WS_ERROR_NULLPTR;
        }
        float vpr = 1.5f; // 1.5f: default virtual pixel ratio
        auto display = ScreenSessionManager::GetInstance().GetDefaultDisplayInfo();
        if (display) {
            vpr = display->GetVirtualPixelRatio();
            WLOGD("vpr = %{public}f", vpr);
        }
        if (!MathHelper::NearZero(ratio)) {
            auto limits = session->property_->GetWindowLimits();
            if (session->IsDecorEnable()) {
                if (limits.minWidth_ && limits.maxHeight_ &&
                    MathHelper::LessNotEqual(ratio, SessionUtils::ToLayoutWidth(limits.minWidth_, vpr)) /
                    SessionUtils::ToLayoutHeight(limits.maxHeight_, vpr)) {
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
        }
        session->aspectRatio_ = ratio;
        if (session->moveDragController_) {
            session->moveDragController_->SetAspectRatio(ratio);
        }
        session->SaveAspectRatio(session->aspectRatio_);
        if (session->FixRectByAspectRatio(session->winRect_)) {
            session->NotifySessionRectChange(session->winRect_);
            session->UpdateRect(session->winRect_, SizeChangeReason::RESIZE);
        }
        return WSError::WS_OK;
    });
    return WSError::WS_OK;
}

WSError SceneSession::UpdateRect(const WSRect& rect, SizeChangeReason reason)
{
    WLOGFD("Id: %{public}d, reason: %{public}d, rect: [%{public}d, %{public}d, %{public}u, %{public}u]",
        GetPersistentId(), reason, rect.posX_, rect.posY_, rect.width_, rect.height_);
    bool isMoveOrDrag = moveDragController_ &&
        (moveDragController_->GetStartDragFlag() || moveDragController_->GetStartMoveFlag());
    if (isMoveOrDrag && reason == SizeChangeReason::UNDEFINED) {
        WLOGFD("skip redundant rect update!");
        return WSError::WS_ERROR_REPEAT_OPERATION;
    }
    WSError ret = Session::UpdateRect(rect, reason);
    if ((ret == WSError::WS_OK || sessionInfo_.isSystem_) && specificCallback_ != nullptr) {
        specificCallback_->onUpdateAvoidArea_(GetPersistentId());
    }
    return ret;
}

bool SceneSession::UpdateInputMethodSessionRect(const WSRect&rect, WSRect& newWinRect, WSRect& newRequestRect)
{
    SessionGravity gravity;
    uint32_t percent = 0;
    property_->GetSessionGravity(gravity, percent);
    if (GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT &&
        (gravity == SessionGravity::SESSION_GRAVITY_BOTTOM || gravity == SessionGravity::SESSION_GRAVITY_DEFAULT)) {
        auto defaultDisplayInfo = ScreenSessionManager::GetInstance().GetDefaultDisplayInfo();
        if (defaultDisplayInfo == nullptr) {
            WLOGFE("defaultDisplayInfo is nullptr");
            return false;
        }

        newWinRect.width_ = (gravity == SessionGravity::SESSION_GRAVITY_BOTTOM) ?
            static_cast<uint32_t>(defaultDisplayInfo->GetWidth()) : rect.width_;
        newRequestRect.width_ = newWinRect.width_;
        newWinRect.height_ = (gravity == SessionGravity::SESSION_GRAVITY_BOTTOM && percent != 0) ?
            static_cast<uint32_t>(defaultDisplayInfo->GetHeight()) * percent / 100u : rect.height_;
        newRequestRect.height_ = newWinRect.height_;
        newWinRect.posX_ = (gravity == SessionGravity::SESSION_GRAVITY_BOTTOM) ? 0 : rect.posX_;
        newRequestRect.posX_ = newWinRect.posX_;
        newWinRect.posY_ = defaultDisplayInfo->GetHeight() - static_cast<int32_t>(newWinRect.height_);
        newRequestRect.posY_ = newWinRect.posY_;

        return true;
    }
    return false;
}

WSError SceneSession::UpdateSessionRect(const WSRect& rect, const SizeChangeReason& reason)
{
    PostTask([weakThis = wptr(this), rect, reason]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        auto newWinRect = session->winRect_;
        auto newRequestRect = session->GetSessionRequestRect();
        if (reason == SizeChangeReason::MOVE) {
            newWinRect.posX_ = rect.posX_;
            newWinRect.posY_ = rect.posY_;
            newRequestRect.posX_ = rect.posX_;
            newRequestRect.posY_ = rect.posY_;
            session->SetSessionRect(newWinRect);
            session->SetSessionRequestRect(newRequestRect);
            session->NotifySessionRectChange(newRequestRect, reason);
        } else if (reason == SizeChangeReason::RESIZE) {
            if (!session->UpdateInputMethodSessionRect(rect, newWinRect, newRequestRect)) {
                newWinRect.width_ = rect.width_;
                newWinRect.height_ = rect.height_;
                newRequestRect.width_ = rect.width_;
                newRequestRect.height_ = rect.height_;
            }
            session->SetSessionRect(newWinRect);
            session->SetSessionRequestRect(newRequestRect);
            session->NotifySessionRectChange(newRequestRect, reason);
        } else {
            session->SetSessionRect(rect);
            session->NotifySessionRectChange(rect, reason);
        }

        WLOGFI("Id: %{public}d, reason: %{public}d, rect: %{public}s, "
            "newRequestRect: %{public}s, newWinRect: %{public}s,", session->GetPersistentId(), reason,
            rect.ToString().c_str(), newRequestRect.ToString().c_str(), newWinRect.ToString().c_str());
        return WSError::WS_OK;
    });
    return WSError::WS_OK;
}

WSError SceneSession::RaiseToAppTop()
{
    PostTask([weakThis = wptr(this)]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (!SessionPermission::IsSystemCalling()) {
            WLOGFE("raise to app top permission denied!");
            return WSError::WS_ERROR_NOT_SYSTEM_APP;
        }
        if (session->sessionChangeCallback_ && session->sessionChangeCallback_->onRaiseToTop_) {
            session->sessionChangeCallback_->onRaiseToTop_();
        }
        return WSError::WS_OK;
    });
    return WSError::WS_OK;
}

WSError SceneSession::RaiseAboveTarget(int32_t subWindowId)
{
    PostTask([weakThis = wptr(this), subWindowId]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
            WLOGFE("RaiseAboveTarget permission denied!");
            return WSError::WS_ERROR_NOT_SYSTEM_APP;
        }
        if (session->sessionChangeCallback_ && session->sessionChangeCallback_->onRaiseAboveTarget_) {
            session->sessionChangeCallback_->onRaiseAboveTarget_(subWindowId);
        }
        return WSError::WS_OK;
    });
    return WSError::WS_OK;
}

WSError SceneSession::CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session, sptr<IRemoteObject> token)
{
    // Get pid and uid before posting task.
    auto pid = IPCSkeleton::GetCallingPid();
    auto uid = IPCSkeleton::GetCallingUid();
    return PostSyncTask([weakThis = wptr(this), sessionStage, eventChannel, surfaceNode, property,
        &persistentId, &session, token, pid, uid]() {
        auto self = weakThis.promote();
        if (!self) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        WLOGFI("CreateAndConnectSpecificSession id: %{public}d", self->GetPersistentId());
        sptr<SceneSession> sceneSession;
        if (self->specificCallback_) {
            SessionInfo sessionInfo;
            if (property) {
                sessionInfo.windowType_ = static_cast<uint32_t>(property->GetWindowType());
            }
            sceneSession = self->specificCallback_->onCreate_(sessionInfo, property);
        }
        if (sceneSession == nullptr) {
            return WSError::WS_ERROR_NULLPTR;
        }
        auto errCode = sceneSession->Connect(
            sessionStage, eventChannel, surfaceNode, self->systemConfig_, property, token);
        sceneSession->SetCallingPid(pid);
        sceneSession->SetCallingUid(uid);
        if (property) {
            persistentId = property->GetPersistentId();
        }
        if (self->sessionChangeCallback_ && self->sessionChangeCallback_->onCreateSpecificSession_) {
            self->sessionChangeCallback_->onCreateSpecificSession_(sceneSession);
        }
        session = sceneSession;
        return errCode;
    });
}

WSError SceneSession::BindDialogTarget(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("dialog session is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->onBindDialogTarget_) {
        sessionChangeCallback_->onBindDialogTarget_(sceneSession);
    }
    return WSError::WS_OK;
}

WSError SceneSession::DestroyAndDisconnectSpecificSession(const int32_t& persistentId)
{
    return PostSyncTask([weakThis = wptr(this), persistentId]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (!session->specificCallback_) {
            WLOGFE("specificCallback_ is null");
            return WSError::WS_ERROR_NULLPTR;
        }
        return session->specificCallback_->onDestroy_(persistentId);
    });
}

void SceneSession::UpdateCameraFloatWindowStatus(bool isShowing)
{
    if (GetWindowType() == WindowType::WINDOW_TYPE_FLOAT_CAMERA && specificCallback_ != nullptr) {
        specificCallback_->onCameraFloatSessionChange_(property_->GetAccessTokenId(), isShowing);
    }
}

WSError SceneSession::SetSystemBarProperty(WindowType type, SystemBarProperty systemBarProperty)
{
    if (property_ == nullptr) {
        return WSError::WS_ERROR_NULLPTR;
    }
    property_->SetSystemBarProperty(type, systemBarProperty);
    WLOGFD("SceneSession SetSystemBarProperty status:%{public}d", static_cast<int32_t>(type));
    if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->OnSystemBarPropertyChange_) {
        sessionChangeCallback_->OnSystemBarPropertyChange_(property_->GetSystemBarProperty());
    }
    return WSError::WS_OK;
}

void SceneSession::NotifyPropertyWhenConnect()
{
    WLOGFI("Notify property when connect.");
    if (property_ == nullptr) {
        WLOGFD("id: %{public}d property is nullptr", persistentId_);
        return;
    }
    NotifySessionFocusableChange(property_->GetFocusable());
    NotifySessionTouchableChange(property_->GetTouchable());
    OnShowWhenLocked(IsShowWhenLocked());
}

WSError SceneSession::OnNeedAvoid(bool status)
{
    PostTask([weakThis = wptr(this), status]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        WLOGFD("SceneSession OnNeedAvoid status:%{public}d", static_cast<int32_t>(status));
        if (session->sessionChangeCallback_ && session->sessionChangeCallback_->OnNeedAvoid_) {
            session->sessionChangeCallback_->OnNeedAvoid_(status);
        }
        return WSError::WS_OK;
    });
    return WSError::WS_OK;
}

WSError SceneSession::OnShowWhenLocked(bool showWhenLocked)
{
    WLOGFD("SceneSession ShowWhenLocked status:%{public}d", static_cast<int32_t>(showWhenLocked));
    if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->OnShowWhenLocked_) {
        sessionChangeCallback_->OnShowWhenLocked_(showWhenLocked);
    }
    return WSError::WS_OK;
}

bool SceneSession::IsShowWhenLocked() const
{
    return property_->GetWindowFlags() & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED);
}

void SceneSession::CalculateAvoidAreaRect(WSRect& rect, WSRect& avoidRect, AvoidArea& avoidArea)
{
    if (SessionHelper::IsEmptyRect(rect) || SessionHelper::IsEmptyRect(avoidRect)) {
        return;
    }
    Rect avoidAreaRect = SessionHelper::TransferToRect(SessionHelper::GetOverlap(rect, avoidRect, 0, 0));
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
    if (property_->GetWindowFlags() & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID)) {
        return;
    }
    std::vector<sptr<SceneSession>> statusBarVector =
        specificCallback_->onGetSceneSessionVectorByType_(WindowType::WINDOW_TYPE_STATUS_BAR);
    for (auto& statusBar : statusBarVector) {
        if (!(statusBar->isVisible_)) {
            continue;
        }
        WSRect statusBarRect = statusBar->GetSessionRect();
        CalculateAvoidAreaRect(rect, statusBarRect, avoidArea);
    }

    return;
}

void SceneSession::GetKeyboardAvoidArea(WSRect& rect, AvoidArea& avoidArea)
{
    std::vector<sptr<SceneSession>> inputMethodVector =
        specificCallback_->onGetSceneSessionVectorByType_(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    for (auto& inputMethod : inputMethodVector) {
        if (inputMethod->GetSessionState() != SessionState::STATE_FOREGROUND &&
            inputMethod->GetSessionState() != SessionState::STATE_ACTIVE) {
            continue;
        }
        SessionGravity gravity;
        uint32_t percent = 0;
        inputMethod->GetSessionProperty()->GetSessionGravity(gravity, percent);
        if (gravity == SessionGravity::SESSION_GRAVITY_FLOAT) {
            continue;
        }
        WSRect inputMethodRect = inputMethod->GetSessionRect();
        CalculateAvoidAreaRect(rect, inputMethodRect, avoidArea);
    }

    return;
}

void SceneSession::GetCutoutAvoidArea(WSRect& rect, AvoidArea& avoidArea)
{
    sptr<CutoutInfo> cutoutInfo = ScreenSessionManager::GetInstance().
        GetCutoutInfo(property_->GetDisplayId());
    if (cutoutInfo == nullptr) {
        WLOGFI("GetCutoutAvoidArea There is no CutoutInfo");
        return;
    }
    std::vector<DMRect> cutoutAreas = cutoutInfo->GetBoundingRects();
    if (cutoutAreas.empty()) {
        WLOGFI("GetCutoutAvoidArea There is no cutoutAreas");
        return;
    }
    for (auto& cutoutArea : cutoutAreas) {
        WSRect cutoutAreaRect = {
            cutoutArea.posX_,
            cutoutArea.posY_,
            cutoutArea.width_,
            cutoutArea.height_
        };
        CalculateAvoidAreaRect(rect, cutoutAreaRect, avoidArea);
    }

    return;
}

AvoidArea SceneSession::GetAvoidAreaByType(AvoidAreaType type)
{
    return PostSyncTask([weakThis = wptr(this), type]() -> AvoidArea {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return {};
        }
        AvoidArea avoidArea;
        WSRect rect = session->GetSessionRect();
        WLOGFD("GetAvoidAreaByType avoidAreaType:%{public}u", type);
        switch (type) {
            case AvoidAreaType::TYPE_SYSTEM: {
                session->GetSystemAvoidArea(rect, avoidArea);
                return avoidArea;
            }
            case AvoidAreaType::TYPE_KEYBOARD: {
                session->GetKeyboardAvoidArea(rect, avoidArea);
                return avoidArea;
            }
            case AvoidAreaType::TYPE_CUTOUT: {
                session->GetCutoutAvoidArea(rect, avoidArea);
                return avoidArea;
            }
            default: {
                WLOGFD("cannot find avoidAreaType: %{public}u", type);
                return avoidArea;
            }
        }
    });
}

WSError SceneSession::UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type)
{
    if (!sessionStage_) {
        return WSError::WS_ERROR_NULLPTR;
    }
    return sessionStage_->UpdateAvoidArea(avoidArea, type);
}

void SceneSession::HandleStyleEvent(MMI::WindowArea area)
{
    static std::pair<int32_t, MMI::WindowArea> preWindowArea =
        std::make_pair(INVALID_WINDOW_ID, MMI::WindowArea::EXIT);
    if (preWindowArea.first == Session::GetWindowId() && preWindowArea.second == area) {
        return;
    }
    if (area != MMI::WindowArea::EXIT) {
        if (Session::SetPointerStyle(area) != WSError::WS_OK) {
            WLOGFE("Failed to set the cursor style, WSError:%{public}d", Session::SetPointerStyle(area));
        }
    }
    preWindowArea = { Session::GetWindowId(), area };
}

WSError SceneSession::HandleEnterWinwdowArea(int32_t displayX, int32_t displayY)
{
    if (displayX < 0 || displayY < 0) {
        WLOGE("Illegal parameter, displayX:%{public}d, displayY:%{public}d", displayX, displayY);
        return WSError::WS_ERROR_INVALID_PARAM;
    }

    auto iter = Session::windowAreas_.cend();
    if (!Session::IsSystemSession() &&
        Session::GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING &&
        Session::GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
        iter = Session::windowAreas_.cbegin();
        for (;iter != Session::windowAreas_.cend(); ++iter) {
            WSRectF rect = iter->second;
            if (rect.IsInRegion(displayX, displayY)) {
                break;
            }
        }
    }

    MMI::WindowArea area = MMI::WindowArea::EXIT;
    if (iter == Session::windowAreas_.cend()) {
        bool isInRegion = false;
        WSRect rect = Session::winRect_;
        if (Session::GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING &&
            Session::GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
            WSRectF rectF = Session::UpdateHotRect(rect);
            isInRegion = rectF.IsInRegion(displayX, displayY);
        } else {
            isInRegion = rect.IsInRegion(displayX, displayY);
        }
        if (!isInRegion) {
            WLOGFE("The wrong event(%{public}d, %{public}d) could not be matched to the region:" \
                "[%{public}d, %{public}d, %{public}d, %{public}d]",
                displayX, displayY, rect.posX_, rect.posY_, rect.width_, rect.height_);
            return WSError::WS_ERROR_INVALID_TYPE;
        }
        area = MMI::WindowArea::FOCUS_ON_INNER;
    } else {
        area = iter->first;
    }
    HandleStyleEvent(area);
    return WSError::WS_OK;
}

WSError SceneSession::HandlePointerStyle(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    if (pointerEvent == nullptr) {
        WLOGFE("pointerEvent is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (pointerEvent->GetSourceType() != MMI::PointerEvent::SOURCE_TYPE_MOUSE) {
        return WSError::WS_DO_NOTHING;
    }
    if (!(pointerEvent->GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_MOVE &&
         pointerEvent->GetButtonId() == MMI::PointerEvent::BUTTON_NONE)) {
        return WSError::WS_DO_NOTHING;
    }

    MMI::PointerEvent::PointerItem pointerItem;
    if (!pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem)) {
        WLOGFE("Get pointeritem failed");
        pointerEvent->MarkProcessed();
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    int32_t mousePointX = pointerItem.GetDisplayX();
    int32_t mousePointY = pointerItem.GetDisplayY();

    auto displayInfo = ScreenSessionManager::GetInstance().GetDisplayInfoById(pointerEvent->GetTargetDisplayId());
    if (displayInfo != nullptr) {
        float vpr = displayInfo->GetVirtualPixelRatio();
        if (vpr <= 0) {
            vpr = 1.5f;
        }
        Session::SetVpr(vpr);
    }
    return HandleEnterWinwdowArea(mousePointX, mousePointY);
}

WSError SceneSession::TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    WLOGFD("SceneSession TransferPointEvent");
    if (pointerEvent == nullptr) {
        WLOGFE("pointerEvent is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    {
        if (pointerEvent->GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_ENTER_WINDOW) {
            std::lock_guard<std::mutex> guard(enterSessionMutex_);
            enterSession_ = wptr<SceneSession>(this);
        }
    }

    static bool isNew = true;
    if (isNew) {
        auto ret = HandlePointerStyle(pointerEvent);
        if ((ret != WSError::WS_OK) && (ret != WSError::WS_DO_NOTHING)) {
            WLOGFE("Failed to update the mouse cursor style, ret:%{public}d", ret);
        }
    }
    if (property_ && property_->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING &&
        WindowHelper::IsMainWindow(property_->GetWindowType()) &&
        property_->GetMaximizeMode() != MaximizeMode::MODE_AVOID_SYSTEM_BAR) {
        PresentFoucusIfNeed(pointerEvent->GetPointerAction());
        if (!moveDragController_) {
            WLOGE("moveDragController_ is null");
            return Session::TransferPointerEvent(pointerEvent);
        }
        if (property_->GetDragEnabled()) {
            if (!isNew) {
                moveDragController_->HandleMouseStyle(pointerEvent, winRect_);
            }
            if (moveDragController_->ConsumeDragEvent(pointerEvent, winRect_, property_, systemConfig_)) {
                return  WSError::WS_OK;
            }
        }
        if (IsDecorEnable() && moveDragController_->ConsumeMoveEvent(pointerEvent, winRect_)) {
            return WSError::WS_OK;
        }
    }

    auto action = pointerEvent->GetPointerAction();
    bool raiseEnabled = WindowHelper::IsSubWindow(property_->GetWindowType()) && property_->GetRaiseEnabled() &&
        (action == MMI::PointerEvent::POINTER_ACTION_DOWN || action == MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    if (raiseEnabled) {
        RaiseToAppTop();
    }
    return Session::TransferPointerEvent(pointerEvent);
}

WSError SceneSession::RequestSessionBack(bool needMoveToBackground)
{
    PostTask([weakThis = wptr(this), needMoveToBackground]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (!session->backPressedFunc_) {
            WLOGFW("Session didn't register back event consumer!");
            return WSError::WS_DO_NOTHING;
        }
        session->backPressedFunc_(needMoveToBackground);
        return WSError::WS_OK;
    });
    return WSError::WS_OK;
}

const wptr<SceneSession> SceneSession::GetEnterWindow()
{
    std::lock_guard<std::mutex> guard(enterSessionMutex_);
    return enterSession_;
}

void SceneSession::ClearEnterWindow()
{
    std::lock_guard<std::mutex> guard(enterSessionMutex_);
    enterSession_ = nullptr;
}

void SceneSession::NotifySessionRectChange(const WSRect& rect, const SizeChangeReason& reason)
{
    std::lock_guard<std::mutex> guard(sessionChangeCbMutex_);
    if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->onRectChange_) {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSession::NotifySessionRectChange");
        sessionChangeCallback_->onRectChange_(rect, reason);
    }
}

bool SceneSession::IsDecorEnable()
{
    return WindowHelper::IsMainWindow(property_->GetWindowType()) && systemConfig_.isSystemDecorEnable_ &&
        WindowHelper::IsWindowModeSupported(systemConfig_.decorModeSupportInfo_, property_->GetWindowMode());
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

bool SceneSession::FixRectByAspectRatio(WSRect& rect)
{
    const int tolerancePx = 2; // 2: tolerance delta pixel value, unit: px
    WSRect originalRect = rect;
    if (!property_ || property_->GetWindowMode() != WindowMode::WINDOW_MODE_FLOATING ||
        !WindowHelper::IsMainWindow(property_->GetWindowType())) {
        return false;
    }

    if (MathHelper::NearZero(aspectRatio_)) {
        return false;
    }
    float vpr = 1.5f; // 1.5f: default virtual pixel ratio
    auto display = ScreenSessionManager::GetInstance().GetDefaultDisplayInfo();
    if (display) {
        vpr = display->GetVirtualPixelRatio();
    }
    int32_t minW;
    int32_t maxW;
    int32_t minH;
    int32_t maxH;
    SessionUtils::CalcFloatWindowRectLimits(property_->GetWindowLimits(), systemConfig_.maxFloatingWindowSize_, vpr,
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
    if (std::abs(static_cast<int32_t>(originalRect.width_) - static_cast<int32_t>(rect.width_)) <= tolerancePx &&
        std::abs(static_cast<int32_t>(originalRect.height_) - static_cast<int32_t>(rect.height_)) <= tolerancePx) {
        rect = originalRect;
        return false;
    }
    return true;
}

void SceneSession::SetMoveDragCallback()
{
    if (moveDragController_) {
        MoveDragCallback callBack = [this](const SizeChangeReason& reason) {
            this->OnMoveDragCallback(reason);
        };
        moveDragController_->RegisterMoveDragCallback(callBack);
    }
}

void SceneSession::OnMoveDragCallback(const SizeChangeReason& reason)
{
    WSRect rect = moveDragController_->GetTargetRect();
    WLOGFD("OnMoveDragCallback rect: [%{public}d, %{public}d, %{public}u, %{public}u], reason : %{public}d",
        rect.posX_, rect.posY_, rect.width_, rect.height_, reason);
    if (reason == SizeChangeReason::DRAG || reason == SizeChangeReason::DRAG_END) {
        UpdateWinRectForSystemBar(rect);
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER,
        "SceneSession::OnMoveDragCallback [%d, %d, %u, %u]", rect.posX_, rect.posY_, rect.width_, rect.height_);
    SetSurfaceBounds(rect);
    if (reason != SizeChangeReason::MOVE) {
        UpdateRect(rect, reason);
    }
    if (reason == SizeChangeReason::DRAG_END) {
        NotifySessionRectChange(rect, reason);
        OnSessionEvent(SessionEvent::EVENT_END_MOVE);
    }
}

void SceneSession::UpdateWinRectForSystemBar(WSRect& rect)
{
    if (!specificCallback_) {
        WLOGFE("specificCallback_ is null!");
        return;
    }
    std::vector<sptr<SceneSession>> statusBarVector =
        specificCallback_->onGetSceneSessionVectorByType_(WindowType::WINDOW_TYPE_STATUS_BAR);
    for (auto& statusBar : statusBarVector) {
        if (!(statusBar->isVisible_)) {
            continue;
        }
        WSRect statusBarRect = statusBar->GetSessionRect();
        if ((rect.posY_ < statusBarRect.posY_ + static_cast<int32_t>(statusBarRect.height_)) &&
            (rect.height_ != winRect_.height_ || rect.width_ != winRect_.width_)) {
            rect.posY_ = statusBarRect.posY_ + statusBarRect.height_;
            rect.height_ = winRect_.height_;
        }
    }
    WLOGFD("after UpdateWinRectForSystemBar rect: [%{public}d, %{public}d, %{public}u, %{public}u]",
        rect.posX_, rect.posY_, rect.width_, rect.height_);
}

void SceneSession::SetSurfaceBounds(const WSRect& rect)
{
    auto rsTransaction = RSTransactionProxy::GetInstance();
    if (rsTransaction) {
        rsTransaction->Begin();
    }
    if (surfaceNode_ && leashWinSurfaceNode_) {
        leashWinSurfaceNode_->SetBounds(rect.posX_, rect.posY_, rect.width_, rect.height_);
        surfaceNode_->SetBounds(0, 0, rect.width_, rect.height_);
    } else {
        WLOGE("SetSurfaceBounds surfaceNode is null!");
    }
    if (rsTransaction) {
        rsTransaction->Commit();
    }
}

void SceneSession::SetZOrder(uint32_t zOrder)
{
    if (zOrder_ != zOrder) {
        Session::SetZOrder(zOrder);
        if (specificCallback_ != nullptr) {
            specificCallback_->onWindowInfoUpdate_(GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_PROPERTY);
        }
    }
}

const std::string& SceneSession::GetWindowName() const
{
    return property_->GetWindowName();
}

WSError SceneSession::SetTurnScreenOn(bool turnScreenOn)
{
    property_->SetTurnScreenOn(turnScreenOn);
    return WSError::WS_OK;
}

bool SceneSession::IsTurnScreenOn() const
{
    return property_->IsTurnScreenOn();
}

WSError SceneSession::SetKeepScreenOn(bool keepScreenOn)
{
    property_->SetKeepScreenOn(keepScreenOn);
    return WSError::WS_OK;
}

bool SceneSession::IsKeepScreenOn() const
{
    return property_->IsKeepScreenOn();
}

std::string SceneSession::GetSessionSnapshotFilePath()
{
    WLOGFI("GetSessionSnapshotFilePath id %{public}d", GetPersistentId());
    if (Session::GetSessionState() < SessionState::STATE_BACKGROUND) {
        WLOGFI("GetSessionSnapshotFilePath UpdateSnapshot");
        auto snapshot = Snapshot();
        if (scenePersistence_ != nullptr) {
            scenePersistence_->SaveSnapshot(snapshot);
        }
    }
    if (scenePersistence_ != nullptr) {
        return scenePersistence_->GetSnapshotFilePath();
    }
    return "";
}

void SceneSession::SaveUpdatedIcon(const std::shared_ptr<Media::PixelMap> &icon)
{
    WLOGFI("run SaveUpdatedIcon");
    if (scenePersistence_ != nullptr) {
        scenePersistence_->SaveUpdatedIcon(icon);
    }
}

std::string SceneSession::GetUpdatedIconPath()
{
    WLOGFI("run GetUpdatedIconPath");
    if (scenePersistence_ != nullptr) {
        return scenePersistence_->GetUpdatedIconPath();
    }
    return "";
}

void SceneSession::UpdateNativeVisibility(bool visible)
{
    isVisible_ = visible;
    if (specificCallback_ == nullptr) {
        WLOGFW("specific callback is null.");
        return;
    }

    if (visible) {
        specificCallback_->onWindowInfoUpdate_(GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_ADDED);
    } else {
        specificCallback_->onWindowInfoUpdate_(GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_REMOVED);
    }
    specificCallback_->onUpdateAvoidArea_(GetPersistentId());
    // update private state
    if (!property_) {
        WLOGFE("UpdateNativeVisibility property_ is null");
        return;
    }
}

bool SceneSession::IsVisible() const
{
    return isVisible_;
}

void SceneSession::SetPrivacyMode(bool isPrivacy)
{
    if (!property_) {
        WLOGFE("SetPrivacyMode property_ is null");
        return;
    }
    if (!surfaceNode_) {
        WLOGFE("surfaceNode_ is null");
        return;
    }
    bool lastPrivacyMode = property_->GetPrivacyMode() || property_->GetSystemPrivacyMode();
    if (lastPrivacyMode == isPrivacy) {
        WLOGFW("privacy mode is not change, do nothing, isPrivacy:%{public}d", isPrivacy);
        return;
    }
    property_->SetPrivacyMode(isPrivacy);
    property_->SetSystemPrivacyMode(isPrivacy);
    surfaceNode_->SetSecurityLayer(isPrivacy);
    if (leashWinSurfaceNode_ != nullptr) {
        leashWinSurfaceNode_->SetSecurityLayer(isPrivacy);
    }
    RSTransaction::FlushImplicitTransaction();
}

WSError SceneSession::UpdateWindowAnimationFlag(bool needDefaultAnimationFlag)
{
    PostTask([weakThis = wptr(this), needDefaultAnimationFlag]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (session->sessionChangeCallback_ && session->sessionChangeCallback_->onWindowAnimationFlagChange_) {
            session->sessionChangeCallback_->onWindowAnimationFlagChange_(needDefaultAnimationFlag);
        }
        return WSError::WS_OK;
    });
    return WSError::WS_OK;
}

void SceneSession::NotifyIsCustomAnimationPlaying(bool isPlaying)
{
    WLOGFI("id %{public}d %{public}u", GetPersistentId(), isPlaying);
    if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->onIsCustomAnimationPlaying_) {
        sessionChangeCallback_->onIsCustomAnimationPlaying_(isPlaying);
    }
}

WSError SceneSession::UpdateWindowSceneAfterCustomAnimation(bool isAdd)
{
    PostTask([weakThis = wptr(this), isAdd]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        WLOGFI("id %{public}d", session->GetPersistentId());
        if (isAdd) {
            if (!session->setWindowScenePatternFunc_ || !session->setWindowScenePatternFunc_->setOpacityFunc_) {
                WLOGFE("SetOpacityFunc not register %{public}d", session->GetPersistentId());
                return WSError::WS_ERROR_INVALID_OPERATION;
            }
            session->setWindowScenePatternFunc_->setOpacityFunc_(1);
        } else {
            WLOGFI("background after custom animation id %{public}d", session->GetPersistentId());
            // since background will remove surfaceNode
            session->Background();
            session->NotifyIsCustomAnimationPlaying(false);
        }
        return WSError::WS_OK;
    });
    return WSError::WS_OK;
}

bool SceneSession::IsFloatingWindowAppType() const
{
    if (property_ == nullptr) {
        return false;
    }

    return property_->IsFloatingWindowAppType();
}

std::vector<Rect> SceneSession::GetTouchHotAreas() const
{
    std::vector<Rect> touchHotAreas;
    if (property_) {
        property_->GetTouchHotAreas(touchHotAreas);
    }
    return touchHotAreas;
}

void SceneSession::DumpSessionElementInfo(const std::vector<std::string>& params)
{
    if (!sessionStage_) {
        return;
    }
    return sessionStage_->DumpSessionElementInfo(params);
}

Rect SceneSession::GetHotAreaRect(int32_t action)
{
    Rect hotAreaRect;
    WSRect rect = GetSessionRect();

    float vpr = 1.5f; // 1.5f: default virtual pixel ratio
    auto display = ScreenSessionManager::GetInstance().GetDefaultDisplayInfo();
    if (display) {
        vpr = display->GetVirtualPixelRatio();
        WLOGD("vpr = %{public}f", vpr);
    }

    float hotZone = 0.0;
    if (action == MMI::PointerEvent::POINTER_ACTION_DOWN) {
        hotZone = HOTZONE_TOUCH;
    } else if (action == MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN) {
        hotZone = HOTZONE_POINTER;
    }
    hotAreaRect.posX_ = rect.posX_ - static_cast<int32_t>(vpr * hotZone);
    hotAreaRect.posY_ = rect.posY_ - static_cast<int32_t>(vpr * hotZone);
    hotAreaRect.width_ = rect.width_ + static_cast<uint32_t>(vpr * hotZone * 2); // 2: double hotZone
    hotAreaRect.height_ = rect.height_ + static_cast<uint32_t>(vpr * hotZone * 2); // 2: double hotZone

    return hotAreaRect;
}

WSError SceneSession::NotifyTouchOutside()
{
    if (!sessionStage_) {
        return WSError::WS_ERROR_NULLPTR;
    }
    return sessionStage_->NotifyTouchOutside();
}

void SceneSession::SetRequestedOrientation(Orientation orientation)
{
    WLOGFI("id: %{public}d orientation: %{public}u", GetPersistentId(), static_cast<uint32_t>(orientation));
    property_->SetRequestedOrientation(orientation);
    if (sessionChangeCallback_ && sessionChangeCallback_->OnRequestedOrientationChange_) {
        sessionChangeCallback_->OnRequestedOrientationChange_(static_cast<uint32_t>(orientation));
    }
}

void SceneSession::NotifyForceHideChange(bool hide)
{
    WLOGFI("id: %{public}d forceHide: %{public}u", persistentId_, hide);
    if (property_ == nullptr) {
        WLOGFD("id: %{public}d property is nullptr", persistentId_);
        return;
    }
    property_->SetForceHide(hide);
    if (sessionChangeCallback_ && sessionChangeCallback_->OnForceHideChange_) {
        sessionChangeCallback_->OnForceHideChange_(hide);
    }
}

Orientation SceneSession::GetRequestedOrientation() const
{
    return property_->GetRequestedOrientation();
}

int32_t SceneSession::GetCollaboratorType() const
{
    return collaboratorType_;
}

void SceneSession::SetCollaboratorType(int32_t collaboratorType)
{
    collaboratorType_ = collaboratorType;
}

void SceneSession::DumpSessionInfo(std::vector<std::string> &info) const
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

std::shared_ptr<AppExecFwk::AbilityInfo> SceneSession::GetAbilityInfo()
{
    SessionInfo& sessionInfo = GetSessionInfo();
    return sessionInfo.abilityInfo;
}

void SceneSession::SetAbilitySessionInfo(std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo)
{
    SessionInfo& sessionInfo = GetSessionInfo();
    sessionInfo.abilityInfo = abilityInfo;
}

void SceneSession::SetSelfToken(sptr<IRemoteObject> selfToken)
{
    selfToken_ = selfToken;
}

sptr<IRemoteObject> SceneSession::GetSelfToken()
{
    return selfToken_;
}

WSError SceneSession::PendingSessionActivation(const sptr<AAFwk::SessionInfo> abilitySessionInfo)
{
    PostTask([weakThis = wptr(this), abilitySessionInfo]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (abilitySessionInfo == nullptr) {
            WLOGFE("abilitySessionInfo is null");
            return WSError::WS_ERROR_NULLPTR;
        }
        session->sessionInfo_.startMethod = StartMethod::START_CALL;

        SessionInfo info;
        info.abilityName_ = abilitySessionInfo->want.GetElement().GetAbilityName();
        info.bundleName_ = abilitySessionInfo->want.GetElement().GetBundleName();
        info.moduleName_ = abilitySessionInfo->want.GetModuleName();
        info.appIndex_ = abilitySessionInfo->want.GetIntParam(DLP_INDEX, 0);
        info.persistentId_ = abilitySessionInfo->persistentId;
        info.callerPersistentId_ = session->GetPersistentId();
        info.callState_ = static_cast<uint32_t>(abilitySessionInfo->state);
        info.uiAbilityId_ = abilitySessionInfo->uiAbilityId;
        info.want = std::make_shared<AAFwk::Want>(abilitySessionInfo->want);
        info.requestCode = abilitySessionInfo->requestCode;
        info.callerToken_ = abilitySessionInfo->callerToken;
        info.startSetting = abilitySessionInfo->startSetting;
        info.callingTokenId_ = abilitySessionInfo->callingTokenId;
        info.reuse = abilitySessionInfo->reuse;
        if (info.want != nullptr) {
            info.windowMode = info.want->GetIntParam(AAFwk::Want::PARAM_RESV_WINDOW_MODE, 0);
        }
        WLOGFI("PendingSessionActivation:bundleName %{public}s, moduleName:%{public}s, abilityName:%{public}s, \
            appIndex:%{public}d", info.bundleName_.c_str(), info.moduleName_.c_str(), info.abilityName_.c_str(),
            info.appIndex_);
        WLOGFI("PendingSessionActivation callState:%{public}d, want persistentId: %{public}d, callingTokenId:%{public}d," \
            "uiAbilityId: %{public}" PRIu64 ", windowMode: %{public}d",
            info.callState_, info.persistentId_, info.callingTokenId_, info.uiAbilityId_, info.windowMode);
        if (session->pendingSessionActivationFunc_) {
            session->pendingSessionActivationFunc_(info);
        }
        return WSError::WS_OK;
    });
    return WSError::WS_OK;
}

WSError SceneSession::TerminateSession(const sptr<AAFwk::SessionInfo> abilitySessionInfo)
{
    PostTask([weakThis = wptr(this), abilitySessionInfo]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (abilitySessionInfo == nullptr) {
            WLOGFE("abilitySessionInfo is null");
            return WSError::WS_ERROR_NULLPTR;
        }
        if (session->isTerminating) {
            WLOGFE("TerminateSession isTerminating, return!");
            return WSError::WS_ERROR_INVALID_OPERATION;
        }
        session->isTerminating = true;
        SessionInfo info;
        info.abilityName_ = abilitySessionInfo->want.GetElement().GetAbilityName();
        info.bundleName_ = abilitySessionInfo->want.GetElement().GetBundleName();
        info.callerToken_ = abilitySessionInfo->callerToken;
        info.persistentId_ = static_cast<int32_t>(abilitySessionInfo->persistentId);
        session->sessionInfo_.want = std::make_shared<AAFwk::Want>(abilitySessionInfo->want);
        session->sessionInfo_.resultCode = abilitySessionInfo->resultCode;
        if (session->terminateSessionFunc_) {
            session->terminateSessionFunc_(info);
        }
        return WSError::WS_OK;
    });
    return WSError::WS_OK;
}

WSError SceneSession::NotifySessionException(const sptr<AAFwk::SessionInfo> abilitySessionInfo)
{
    PostTask([weakThis = wptr(this), abilitySessionInfo]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        if (abilitySessionInfo == nullptr) {
            WLOGFE("abilitySessionInfo is null");
            return WSError::WS_ERROR_NULLPTR;
        }
        if (session->isTerminating) {
            WLOGFE("NotifySessionException isTerminating, return!");
            return WSError::WS_ERROR_INVALID_OPERATION;
        }
        session->isTerminating = true;
        SessionInfo info;
        info.abilityName_ = abilitySessionInfo->want.GetElement().GetAbilityName();
        info.bundleName_ = abilitySessionInfo->want.GetElement().GetBundleName();
        info.callerToken_ = abilitySessionInfo->callerToken;
        info.errorCode = abilitySessionInfo->errorCode;
        info.errorReason = abilitySessionInfo->errorReason;
        info.persistentId_ = static_cast<int32_t>(abilitySessionInfo->persistentId);
        session->sessionInfo_.want = std::make_shared<AAFwk::Want>(abilitySessionInfo->want);
        session->sessionInfo_.errorCode = abilitySessionInfo->errorCode;
        session->sessionInfo_.errorReason = abilitySessionInfo->errorReason;
        if (!session->sessionExceptionFuncs_.empty()) {
            for (auto funcPtr : session->sessionExceptionFuncs_) {
                auto sessionExceptionFunc = *funcPtr;
                sessionExceptionFunc(info);
            }
        }
        return WSError::WS_OK;
    });
    return WSError::WS_OK;
}
} // namespace OHOS::Rosen
