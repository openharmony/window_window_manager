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
#include "session/host/include/session_utils.h"
#include "session_manager/include/screen_session_manager.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "wm_math.h"
#include <running_lock.h>

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
    property_ = new(std::nothrow) WindowSessionProperty();
    if (property_) {
        property_->SetWindowType(static_cast<WindowType>(info.windowType_));
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
    WLOGFD("SceneSession SetGlobalMaximizeMode mode: %{public}u", static_cast<uint32_t>(mode));
    maximizeMode_ = mode;
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
    if (!property_) {
        WLOGE("SetAspectRatio failed because property is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    float vpr = 1.5f; // 1.5f: default virtual pixel ratio
    auto display = ScreenSessionManager::GetInstance().GetDefaultDisplayInfo();
    if (!display) {
        vpr = display->GetVirtualPixelRatio();
        WLOGD("vpr = %{public}f", vpr);
    }

    auto limits = property_->GetWindowLimits();
    if (IsDecorEnable()) {
        if (limits.minWidth_ && limits.maxHeight_ &&
            MathHelper::LessNotEqual(ratio, static_cast<float>(SessionUtils::ToLayoutWidth(limits.minWidth_, vpr)) /
            SessionUtils::ToLayoutHeight(limits.maxHeight_, vpr))) {
            WLOGE("Failed, because aspectRatio is smaller than minWidth/maxHeight");
            return WSError::WS_ERROR_INVALID_PARAM;
        } else if (limits.minHeight_ && limits.maxWidth_ &&
            MathHelper::GreatNotEqual(ratio, static_cast<float>(SessionUtils::ToLayoutWidth(limits.maxWidth_, vpr)) /
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

    aspectRatio_ = ratio;
    if (moveDragController_) {
        moveDragController_->SetAspectRatio(ratio);
    }
    SaveAspectRatio(aspectRatio_);
    if (FixRectByAspectRatio(winRect_)) {
        SetSessionRect(winRect_);
        NotifySessionRectChange(winRect_);
        UpdateRect(winRect_, SizeChangeReason::RESIZE);
    }
    return WSError::WS_OK;
}

WSError SceneSession::UpdateSessionRect(const WSRect &rect, const SizeChangeReason &reason)
{
    WLOGFI("UpdateSessionRect [%{public}d, %{public}d, %{public}u, %{public}u]", rect.posX_, rect.posY_,
        rect.width_, rect.height_);
    WSRect newRect = rect;
    if (isFirstStart_ && FixRectByAspectRatio(newRect)) {
        WLOGFD("FixedRect [%{public}d, %{public}d, %{public}u, %{public}u]", newRect.posX_, newRect.posY_,
            newRect.width_, newRect.height_);
    }
    isFirstStart_ = false;
    SetSessionRect(newRect);
    NotifySessionRectChange(newRect);
    UpdateRect(newRect, reason);
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
        sessionChangeCallback_->OnSystemBarPropertyChange_(property_->GetSystemBarProperty());
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
    if (property_ && property_->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING &&
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

bool SceneSession::IsDecorEnable()
{
    return WindowHelper::IsMainWindow(property_->GetWindowType()) && systemConfig_.isSystemDecorEnable_ &&
        WindowHelper::IsWindowModeSupported(systemConfig_.decorModeSupportInfo_, property_->GetWindowMode());
}

bool SceneSession::SaveAspectRatio(float ratio)
{
    if (!sessionInfo_.bundleName_.empty() && !sessionInfo_.abilityName_.empty()) {
        std::string key = sessionInfo_.bundleName_ + "_" + sessionInfo_.abilityName_;
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
} // namespace OHOS::Rosen
