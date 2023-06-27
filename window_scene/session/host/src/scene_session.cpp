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

#include <iterator>
#include <pointer_event.h>

#include "interfaces/include/ws_common.h"
#include "session/host/include/scene_persistent_storage.h"
#include "window_helper.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSession" };
}
MaximizeMode SceneSession::maximizeMode_ = MaximizeMode::MODE_RECOVER;
SceneSession::SceneSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback)
    : Session(info)
{
    GeneratePersistentId(false, info);
    scenePersistence_ = new ScenePersistence(info, GetPersistentId());
    specificCallback_ = specificCallback;
    moveDragController_ = new (std::nothrow) MoveDragController();
    ProcessVsyncHandleRegister();
    if (!info.bundleName_.empty() && !info.abilityName_.empty()) {
        std::string key = info.bundleName_ + "_" + info.abilityName_;
        if (ScenePersistentStorage::HasKey(key, ScenePersistentStorageType::ASPECT_RATIO)) {
            ScenePersistentStorage::Get(key, aspectRatio_, ScenePersistentStorageType::ASPECT_RATIO);
            WLOGD("SceneSession init aspectRatio , key %{public}s, value: %{public}f", key.c_str(), aspectRatio_);
            if (moveDragController_) {
                moveDragController_->SetAspectRatio(aspectRatio_);
            }
        }
    }
    if (info.isSystem_ && info.windowType_ != 0) {
        property_ = new(std::nothrow) WindowSessionProperty();
        if (property_) {
            property_->SetWindowType(static_cast<WindowType>(info.windowType_));
        }
    }
}

WSError SceneSession::Foreground()
{
    WSError ret = Session::Foreground();
    if (ret != WSError::WS_OK) {
        return ret;
    }
    UpdateCameraFloatWindowStatus(true);
    return WSError::WS_OK;
}

WSError SceneSession::Background()
{
    WSError ret = Session::Background();
    if (ret != WSError::WS_OK) {
        return ret;
    }
    if (scenePersistence_ != nullptr && GetSnapshot() != nullptr) {
        scenePersistence_->SaveSnapshot(GetSnapshot());
    }
    UpdateCameraFloatWindowStatus(false);
    return WSError::WS_OK;
}

WSError SceneSession::OnSessionEvent(SessionEvent event)
{
    WLOGFD("SceneSession OnSessionEvent event: %{public}d", static_cast<int32_t>(event));
    if (event == SessionEvent::EVENT_START_MOVE && moveDragController_) {
        moveDragController_->InitMoveDragProperty();
        moveDragController_->SetStartMoveFlag(true);
    }
    if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->OnSessionEvent_) {
        sessionChangeCallback_->OnSessionEvent_(static_cast<uint32_t>(event));
    }
    return WSError::WS_OK;
}

void SceneSession::RegisterSessionChangeCallback(const sptr<SceneSession::SessionChangeCallback>&
    sessionChangeCallback)
{
    sessionChangeCallback_ = sessionChangeCallback;
}

WSError SceneSession::SetGlobalMaximizeMode(MaximizeMode mode)
{
    WLOGFI("SceneSession SetGlobalMaximizeMode mode: %{public}u", static_cast<uint32_t>(mode));
    maximizeMode_ = mode;
    ScenePersistentStorage::Insert("maximize_state", static_cast<int32_t>(maximizeMode_),
        ScenePersistentStorageType::MAXIMIZE_STATE);
    return WSError::WS_OK;
}

WSError SceneSession::GetGlobalMaximizeMode(MaximizeMode &mode)
{
    WLOGFD("SceneSession GetGlobalMaximizeMode");
    mode = maximizeMode_;
    return WSError::WS_OK;
}

WSError SceneSession::SetAspectRatio(float ratio)
{
    aspectRatio_ = ratio;
    if (moveDragController_) {
        moveDragController_->SetAspectRatio(ratio);
    }
    if (!sessionInfo_.bundleName_.empty() && !sessionInfo_.abilityName_.empty()) {
        std::string key = sessionInfo_.bundleName_ + "_" + sessionInfo_.abilityName_;
        ScenePersistentStorage::Insert(key, aspectRatio_, ScenePersistentStorageType::ASPECT_RATIO);
        WLOGD("SceneSession save aspectRatio , key %{public}s, value: %{public}f", key.c_str(), aspectRatio_);
    }
    return WSError::WS_OK;
}

WSError SceneSession::UpdateSessionRect(const WSRect &rect, const SizeChangeReason &reason)
{
    WLOGFI("UpdateSessionRect [%{public}d, %{public}d, %{public}u, %{public}u]", rect.posX_, rect.posY_,
        rect.width_, rect.height_);
    SetSessionRect(rect);
    NotifySessionRectChange(rect);
    UpdateRect(rect, reason);
    return WSError::WS_OK;
}

WSError SceneSession::RaiseToAppTop()
{
    if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->onRaiseToTop_) {
        sessionChangeCallback_->onRaiseToTop_();
    }
    return WSError::WS_OK;
}

WSError SceneSession::CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<WindowSessionProperty> property, uint64_t& persistentId, sptr<ISession>& session)
{
    WLOGFI("CreateAndConnectSpecificSession id: %{public}" PRIu64 "", GetPersistentId());
    sptr<SceneSession> sceneSession;
    if (specificCallback_ != nullptr) {
        sceneSession = specificCallback_->onCreate_(sessionInfo_, property);
    }
    if (sceneSession == nullptr) {
        return WSError::WS_ERROR_NULLPTR;
    }
    // connect specific session and sessionStage
    WSError errCode = sceneSession->Connect(sessionStage, eventChannel, surfaceNode, systemConfig_, property);
    if (property) {
        persistentId = property->GetPersistentId();
    }
    if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->onCreateSpecificSession_) {
        sessionChangeCallback_->onCreateSpecificSession_(sceneSession);
    }
    session = sceneSession;
    return errCode;
}

WSError SceneSession::DestroyAndDisconnectSpecificSession(const uint64_t& persistentId)
{
    WSError ret = WSError::WS_OK;
    if (specificCallback_ != nullptr) {
        ret = specificCallback_->onDestroy_(persistentId);
    }
    return ret;
}

void SceneSession::UpdateCameraFloatWindowStatus(bool isShowing)
{
    if (GetWindowType() == WindowType::WINDOW_TYPE_FLOAT_CAMERA && specificCallback_ != nullptr) {
        specificCallback_->onCameraFloatSessionChange_(property_->GetAccessTokenId(), isShowing);
    }
}

WSError SceneSession::SetSystemBarProperty(WindowType type, SystemBarProperty systemBarProperty)
{
    property_->SetSystemBarProperty(type, systemBarProperty);
    WLOGFD("SceneSession SetSystemBarProperty status:%{public}d", static_cast<int32_t>(type));
    if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->OnSystemBarPropertyChange_) {
        sessionChangeCallback_->OnSystemBarPropertyChange_(systemBarProperty);
    }
    return WSError::WS_OK;
}

WSError SceneSession::OnNeedAvoid(bool status)
{
    WLOGFD("SceneSession OnNeedAvoid status:%{public}d", static_cast<int32_t>(status));
    if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->OnNeedAvoid_) {
        sessionChangeCallback_->OnNeedAvoid_(status);
    }
    return WSError::WS_OK;
}

AvoidArea SceneSession::GetAvoidAreaByType(AvoidAreaType type)
{
    AvoidArea avoidArea;
    WLOGFD("GetAvoidAreaByType avoidAreaType:%{public}u", type);
    switch (type) {
        case AvoidAreaType::TYPE_SYSTEM : {
            return avoidArea;
        }
        case AvoidAreaType::TYPE_KEYBOARD : {
            return avoidArea;
        }
        case AvoidAreaType::TYPE_CUTOUT : {
            return avoidArea;
        }
        default : {
            WLOGFD("cannot find avoidAreaType: %{public}u", type);
            return avoidArea;
        }
    }
}

WSError SceneSession::TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    WLOGFD("SceneSession TransferPointEvent");
    if (property_->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING &&
        WindowHelper::IsMainWindow(property_->GetWindowType()) &&
        property_->GetMaximizeMode() != MaximizeMode::MODE_AVOID_SYSTEM_BAR) {
        if (!moveDragController_) {
            WLOGE("moveDragController_ is null");
            return Session::TransferPointerEvent(pointerEvent);
        }
        if (moveDragController_->ConsumeDragEvent(pointerEvent, winRect_, property_, systemConfig_)) {
            return  WSError::WS_OK;
        }
        if (moveDragController_->GetStartMoveFlag()) {
            return moveDragController_->ConsumeMoveEvent(pointerEvent, winRect_);
        }
    }
    return Session::TransferPointerEvent(pointerEvent);
}

void SceneSession::NotifySessionRectChange(const WSRect& rect)
{
    if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->onRectChange_) {
        sessionChangeCallback_->onRectChange_(rect);
    }
}

void SceneSession::ProcessVsyncHandleRegister()
{
    if (moveDragController_) {
        NotifyVsyncHandleFunc func = [this](void) {
            this->OnVsyncHandle();
        };
        moveDragController_->SetVsyncHandleListenser(func);
    }
}

void SceneSession::OnVsyncHandle()
{
    WSRect rect = moveDragController_->GetTargetRect();
    WLOGFD("rect: [%{public}d, %{public}d, %{public}u, %{public}u]", rect.posX_, rect.posY_, rect.width_, rect.height_);
    NotifySessionRectChange(rect);
}
} // namespace OHOS::Rosen
