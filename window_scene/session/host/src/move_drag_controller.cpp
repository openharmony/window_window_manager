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

#include "session/host/include/move_drag_controller.h"

#include <pointer_event.h>

#include "session/host/include/scene_persistent_storage.h"
#include "session/host/include/session_utils.h"
#include "session/host/include/session_vsync_station.h"
#include "session_manager/include/screen_session_manager.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "wm_common_inner.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "MoveDragController" };
}

MoveDragController::MoveDragController()
{
    vsyncCallback_->onCallback = std::bind(&MoveDragController::OnReceiveVsync, this, std::placeholders::_1);
}

MoveDragController::~MoveDragController()
{
    RemoveVsync();
}

void MoveDragController::SetVsyncHandleListenser(const NotifyVsyncHandleFunc& func)
{
    vsyncHandleFunc_ = func;
}

void MoveDragController::SetStartMoveFlag(bool flag)
{
    isStartMove_ = flag;
}

bool MoveDragController::GetStartMoveFlag() const
{
    return isStartMove_;
}

WSRect MoveDragController::GetTargetRect() const
{
    return moveDragProperty_.targetRect_;
}

void MoveDragController::InitMoveDragProperty()
{
    moveDragProperty_ = { -1, -1, -1, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
}

void MoveDragController::SetAspectRatio(float ratio)
{
    aspectRatio_ = ratio;
}

WSError MoveDragController::ConsumeMoveEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
    const WSRect& originalRect)
{
    if (pointerEvent == nullptr) {
        WLOGE("ConsumeMoveEvent stop because of nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    int32_t pointerId = pointerEvent->GetPointerId();
    int32_t startPointerId = moveDragProperty_.pointerId_;
    if (startPointerId != -1 && startPointerId != pointerId) {
        WLOGFI("block unnecessary pointer event inside the window");
        return WSError::WS_DO_NOTHING;
    }

    MMI::PointerEvent::PointerItem pointerItem;
    int32_t sourceType = pointerEvent->GetSourceType();
    if (!pointerEvent->GetPointerItem(pointerId, pointerItem) ||
        (sourceType == MMI::PointerEvent::SOURCE_TYPE_MOUSE &&
        pointerEvent->GetButtonId() != MMI::PointerEvent::MOUSE_BUTTON_LEFT)) {
        WLOGFW("invalid pointerEvent");
        return WSError::WS_ERROR_NULLPTR;
    }

    int32_t action = pointerEvent->GetPointerAction();
    switch (action) {
        case MMI::PointerEvent::POINTER_ACTION_MOVE: {
            break;
        }
        case MMI::PointerEvent::POINTER_ACTION_UP:
        case MMI::PointerEvent::POINTER_ACTION_BUTTON_UP:
        case MMI::PointerEvent::POINTER_ACTION_CANCEL: {
            SetStartMoveFlag(false);
            break;
        }
        default:
            break;
    }
    CalcMoveTargetRect(pointerEvent, originalRect);
    return WSError::WS_OK;
}

bool MoveDragController::ConsumeDragEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
    const WSRect& originalRect, const sptr<WindowSessionProperty> property, const SystemSessionConfig& sysConfig)
{
    if (pointerEvent == nullptr || property == nullptr) {
        WLOGE("ConsumeDragEvent stop because of nullptr");
        return false;
    }
    if (!isStartDrag_ && pointerEvent->GetPointerAction() != MMI::PointerEvent::POINTER_ACTION_DOWN &&
        pointerEvent->GetPointerAction() != MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN) {
        return false;
    }
    int32_t pointerId = pointerEvent->GetPointerId();
    int32_t startPointerId = moveDragProperty_.pointerId_;
    if (isStartDrag_ && startPointerId != -1 && startPointerId != pointerId) {
        return false;
    }
    MMI::PointerEvent::PointerItem pointerItem;
    if (!pointerEvent->GetPointerItem(pointerId, pointerItem)) {
        WLOGE("Get PointerItem failed");
        return false;
    }

    switch (pointerEvent->GetPointerAction()) {
        case MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN:
        case MMI::PointerEvent::POINTER_ACTION_DOWN: {
            if (!EventDownInit(pointerEvent, originalRect, property, sysConfig)) {
                return false;
            }
            break;
        }
        case MMI::PointerEvent::POINTER_ACTION_MOVE: {
            break;
        }
        case MMI::PointerEvent::POINTER_ACTION_UP:
        case MMI::PointerEvent::POINTER_ACTION_BUTTON_UP:
        case MMI::PointerEvent::POINTER_ACTION_CANCEL: {
            isStartDrag_ = false;
            break;
        }
        default:
            return false;
    }
    int32_t tranX = pointerItem.GetDisplayX() - moveDragProperty_.originalPointerPosX_;
    int32_t tranY = pointerItem.GetDisplayY() - moveDragProperty_.originalPointerPosY_;

    if (aspectRatio_ > NEAR_ZERO) {
        moveDragProperty_.targetRect_ = CalcFixedAspectRatioTargetRect(type_, tranX, tranY, aspectRatio_,
            moveDragProperty_.originalRect_);
    } else {
        moveDragProperty_.targetRect_ = CalcFreeformTargetRect(type_, tranX, tranY, moveDragProperty_.originalRect_);
    }
    RequestVsync();
    return true;
}

void MoveDragController::CalcMoveTargetRect(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
    const WSRect& originalRect)
{
    MMI::PointerEvent::PointerItem pointerItem;
    int32_t pointerId = pointerEvent->GetPointerId();
    pointerEvent->GetPointerItem(pointerId, pointerItem);
    int32_t pointerDisplayX = pointerItem.GetDisplayX();
    int32_t pointerDisplayY = pointerItem.GetDisplayY();
    if (moveDragProperty_.isEmpty()) {
        moveDragProperty_.pointerId_ = pointerId;
        moveDragProperty_.originalPointerPosX_ = pointerDisplayX;
        moveDragProperty_.originalPointerPosY_ = pointerDisplayY;
        int32_t pointerWindowX = pointerItem.GetWindowX();
        int32_t pointerWindowY = pointerItem.GetWindowY();
        moveDragProperty_.originalRect_ = originalRect;
        moveDragProperty_.originalRect_.posX_ = pointerDisplayX - pointerWindowX;
        moveDragProperty_.originalRect_.posY_ = pointerDisplayY - pointerWindowY;
    } else {
        int32_t offsetX = pointerDisplayX - moveDragProperty_.originalPointerPosX_;
        int32_t offsetY = pointerDisplayY - moveDragProperty_.originalPointerPosY_;
        moveDragProperty_.targetRect_ = {
            moveDragProperty_.originalRect_.posX_ + offsetX,
            moveDragProperty_.originalRect_.posY_ + offsetY,
            moveDragProperty_.originalRect_.width_,
            moveDragProperty_.originalRect_.height_ };
        WLOGFD("move rect: [%{public}d, %{public}d, %{public}u, %{public}u]",
            moveDragProperty_.targetRect_.posX_, moveDragProperty_.targetRect_.posY_,
            moveDragProperty_.targetRect_.width_, moveDragProperty_.targetRect_.height_);
        RequestVsync();
    }
}

bool MoveDragController::EventDownInit(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
    const WSRect& originalRect, const sptr<WindowSessionProperty> property, const SystemSessionConfig& sysConfig)
{
    if (pointerEvent->GetSourceType() == MMI::PointerEvent::SOURCE_TYPE_MOUSE &&
        pointerEvent->GetButtonId() != MMI::PointerEvent::MOUSE_BUTTON_LEFT) {
        return false;
    }
    maxFloatingWindowSize_ = sysConfig.maxFloatingWindowSize_;
    int32_t pointerId = pointerEvent->GetPointerId();
    MMI::PointerEvent::PointerItem pointerItem;
    pointerEvent->GetPointerItem(pointerId, pointerItem);
    InitMoveDragProperty();
    moveDragProperty_.originalRect_ = originalRect;
    auto display = ScreenSessionManager::GetInstance().GetDisplayInfoById(pointerEvent->GetTargetDisplayId());
    if (display) {
        vpr_ = display->GetVirtualPixelRatio();
    } else {
        vpr_ = 1.5f; // 1.5f: default virtual pixel ratio
    }
    type_ = GetAreaType(pointerItem.GetWindowX(), pointerItem.GetWindowY(), pointerEvent->GetSourceType(),
        moveDragProperty_.originalRect_);
    if (type_ == AreaType::UNDEFINED) {
        return false;
    }
    InitDecorValue(property, sysConfig);
    limits_ = property->GetWindowLimits();
    moveDragProperty_.pointerId_ = pointerEvent->GetPointerId();
    moveDragProperty_.originalPointerPosX_ = pointerItem.GetDisplayX();
    moveDragProperty_.originalPointerPosY_ = pointerItem.GetDisplayY();
    if (aspectRatio_ <= NEAR_ZERO) {
        CalcFreeformTranslateLimits(type_);
    }
    moveDragProperty_.originalRect_.posX_ = pointerItem.GetDisplayX() - pointerItem.GetWindowX();
    moveDragProperty_.originalRect_.posY_ = pointerItem.GetDisplayY() - pointerItem.GetWindowY();
    mainMoveAxis_ = AxisType::UNDEFINED;
    isStartDrag_ = true;
    return true;
}

AreaType MoveDragController::GetAreaType(int32_t pointWinX, int32_t pointWinY, int32_t sourceType, const WSRect& rect)
{
    int32_t insideCorner = WINDOW_FRAME_CORNER_WIDTH * vpr_;
    int32_t insideEdge = WINDOW_FRAME_WIDTH * vpr_;
    int32_t outside;
    if (sourceType == MMI::PointerEvent::SOURCE_TYPE_MOUSE) {
        outside = HOTZONE_POINTER * vpr_;
    } else {
        outside = HOTZONE_TOUCH * vpr_;
    }

    int32_t leftOut = -outside;
    int32_t leftIn = insideEdge;
    int32_t leftCorner = insideCorner;
    int32_t rightCorner = rect.width_ - insideCorner;
    int32_t rightIn = rect.width_ - insideEdge;
    int32_t rightOut = rect.width_ + outside;
    int32_t topOut = -outside;
    int32_t topIn = insideEdge;
    int32_t topCorner = insideCorner;
    int32_t bottomCorner = rect.height_ - insideCorner;
    int32_t bottomIn = rect.height_ - insideEdge;
    int32_t bottomOut = rect.height_ + outside;

    auto isInRange = [](int32_t min, int32_t max, int32_t value) { return min <= value && value <= max; };

    AreaType type;
    if (isInRange(leftOut, leftCorner, pointWinX) && isInRange(topOut, topCorner, pointWinY)) {
        type = AreaType::LEFT_TOP;
    } else if (isInRange(rightCorner, rightOut, pointWinX) && isInRange(topOut, topCorner, pointWinY)) {
        type = AreaType::RIGHT_TOP;
    } else if (isInRange(rightCorner, rightOut, pointWinX) && isInRange(bottomCorner, bottomOut, pointWinY)) {
        type = AreaType::RIGHT_BOTTOM;
    } else if (isInRange(leftOut, leftCorner, pointWinX) && isInRange(bottomCorner, bottomOut, pointWinY)) {
        type = AreaType::LEFT_BOTTOM;
    } else if (isInRange(leftOut, leftIn, pointWinX)) {
        type = AreaType::LEFT;
    } else if (isInRange(topOut, topIn, pointWinY)) {
        type = AreaType::TOP;
    } else if (isInRange(rightIn, rightOut, pointWinX)) {
        type = AreaType::RIGHT;
    } else if (isInRange(bottomIn, bottomOut, pointWinY)) {
        type = AreaType::BOTTOM;
    } else {
        type = AreaType::UNDEFINED;
    }
    WLOGD("HotAreaType :%{public}d", type);
    return type;
}

WSRect MoveDragController::CalcFreeformTargetRect(AreaType type, int32_t tranX, int32_t tranY, WSRect originalRect)
{
    WSRect targetRect = originalRect;
    FixTranslateByLimits(tranX, tranY);
    if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::LEFT)) {
        targetRect.posX_ += tranX;
        targetRect.width_ -= tranX;
    } else if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::RIGHT)) {
        targetRect.width_ += tranX;
    }
    if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::TOP)) {
        targetRect.posY_ += tranY;
        targetRect.height_ -= tranY;
    } else if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::BOTTOM)) {
        targetRect.height_ += tranY;
    }
    return targetRect;
}

WSRect MoveDragController::CalcFixedAspectRatioTargetRect(AreaType type, int32_t tranX, int32_t tranY,
    float aspectRatio, WSRect originalRect)
{
    int32_t posX = originalRect.posX_;
    int32_t posY = originalRect.posY_;
    int32_t width = originalRect.width_;
    int32_t height = originalRect.height_;
    FixTranslateByLimits(tranX, tranY);
    if (mainMoveAxis_ == AxisType::UNDEFINED) {
        if (!InitMainAxis(type, tranX, tranY)) {
            return originalRect;
        }
    }

    ConvertXYByAspectRatio(tranX, tranY, aspectRatio);
    switch (type) {
        case AreaType::LEFT_TOP: {
            return { posX + tranX, posY + tranY, width - tranX, height - tranY };
        }
        case AreaType::RIGHT_TOP: {
            return { posX, posY + (mainMoveAxis_ == AxisType::X_AXIS ? (-tranY) : (tranY)),
                     width + (mainMoveAxis_ == AxisType::X_AXIS ? (tranX) : (-tranX)),
                     height + (mainMoveAxis_ == AxisType::X_AXIS ? (tranY) : (-tranY)) };
        }
        case AreaType::RIGHT_BOTTOM: {
            return { posX, posY, width + tranX, height + tranY };
        }
        case AreaType::LEFT_BOTTOM: {
            return { posX + (mainMoveAxis_ == AxisType::X_AXIS ? (tranX) : (-tranX)), posY,
                     width - (mainMoveAxis_ == AxisType::X_AXIS ? (tranX) : (-tranX)),
                     height - (mainMoveAxis_ == AxisType::X_AXIS ? (tranY) : (-tranY)) };
        }
        case AreaType::LEFT: {
            return { posX + tranX, posY, width - tranX, height - tranY };
        }
        case AreaType::TOP: {
            return { posX, posY + tranY, width - tranX, height - tranY };
        }
        case AreaType::RIGHT: {
            return { posX, posY, width + tranX, height + tranY };
        }
        case AreaType::BOTTOM: {
            return { posX, posY, width + tranX, height + tranY };
        }
        default:
            break;
    }
    return originalRect;
}

void MoveDragController::CalcFreeformTranslateLimits(AreaType type)
{
    int32_t minW;
    int32_t maxW;
    int32_t minH;
    int32_t maxH;
    SessionUtils::CalcFloatWindowRectLimits(limits_, maxFloatingWindowSize_, vpr_, minW, maxW, minH, maxH);
    if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::LEFT)) {
        minTranX_ = moveDragProperty_.originalRect_.width_ - maxW;
        maxTranX_ = moveDragProperty_.originalRect_.width_ - minW;
    } else if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::RIGHT)) {
        minTranX_ = minW - moveDragProperty_.originalRect_.width_;
        maxTranX_ = maxW - moveDragProperty_.originalRect_.width_;
    }
    if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::TOP)) {
        minTranY_ = moveDragProperty_.originalRect_.height_ - maxH;
        maxTranY_ = moveDragProperty_.originalRect_.height_ - minH;
    } else if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::BOTTOM)) {
        minTranY_ = minH - moveDragProperty_.originalRect_.height_;
        maxTranY_ = maxH - moveDragProperty_.originalRect_.height_;
    }
}

void MoveDragController::CalcFixedAspectRatioTranslateLimits(AreaType type, AxisType axis)
{
    int32_t minW;
    int32_t maxW;
    int32_t minH;
    int32_t maxH;
    SessionUtils::CalcFloatWindowRectLimits(limits_, maxFloatingWindowSize_, vpr_, minW, maxW, minH, maxH);
    if (isDecorEnable_) {
        if (SessionUtils::ToLayoutWidth(minW, vpr_) < SessionUtils::ToLayoutHeight(minH, vpr_) * aspectRatio_) {
            minW = SessionUtils::ToWinWidth(SessionUtils::ToLayoutHeight(minH, vpr_) * aspectRatio_, vpr_);
        } else {
            minH = SessionUtils::ToWinHeight(SessionUtils::ToLayoutWidth(minW, vpr_) / aspectRatio_, vpr_);
        }
        if (SessionUtils::ToLayoutWidth(maxW, vpr_) < SessionUtils::ToLayoutHeight(maxH, vpr_) * aspectRatio_) {
            maxH = SessionUtils::ToWinHeight(SessionUtils::ToLayoutWidth(maxW, vpr_) * aspectRatio_, vpr_);
        } else {
            maxW = SessionUtils::ToWinWidth(SessionUtils::ToLayoutHeight(maxH, vpr_) / aspectRatio_, vpr_);
        }
    } else {
        if (minW < minH * aspectRatio_) {
            minW = minH * aspectRatio_;
        } else {
            minH = minW / aspectRatio_;
        }
        if (maxW < maxH * aspectRatio_) {
            maxH = maxW * aspectRatio_;
        } else {
            maxW = maxH / aspectRatio_;
        }
    }

    if (axis == AxisType::X_AXIS) {
        if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::LEFT)) {
            minTranX_ = moveDragProperty_.originalRect_.width_ - maxW;
            maxTranX_ = moveDragProperty_.originalRect_.width_ - minW;
        } else if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::RIGHT)) {
            minTranX_ = minW - moveDragProperty_.originalRect_.width_;
            maxTranX_ = maxW - moveDragProperty_.originalRect_.width_;
        }
    } else if (axis == AxisType::Y_AXIS) {
        if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::TOP)) {
            minTranY_ = moveDragProperty_.originalRect_.height_ - maxH;
            maxTranY_ = moveDragProperty_.originalRect_.height_ - minH;
        } else if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::BOTTOM)) {
            minTranY_ = minH - moveDragProperty_.originalRect_.height_;
            maxTranY_ = maxH - moveDragProperty_.originalRect_.height_;
        }
    }
}

void MoveDragController::FixTranslateByLimits(int32_t& tranX, int32_t& tranY)
{
    if (tranX < minTranX_) {
        tranX = minTranX_;
    } else if (tranX > maxTranX_) {
        tranX = maxTranX_;
    }
    if (tranY < minTranY_) {
        tranY = minTranY_;
    } else if (tranY > maxTranY_) {
        tranY = maxTranY_;
    }
}

bool MoveDragController::InitMainAxis(AreaType type, int32_t tranX, int32_t tranY)
{
    if (type == AreaType::LEFT || type == AreaType::RIGHT) {
        mainMoveAxis_ = AxisType::X_AXIS;
    } else if (type == AreaType::TOP || type == AreaType::BOTTOM) {
        mainMoveAxis_ = AxisType::Y_AXIS;
    } else if (tranX == 0 && tranY == 0) {
        return false;
    } else {
        mainMoveAxis_ = (std::abs(tranX) > std::abs(tranY)) ? AxisType::X_AXIS : AxisType::Y_AXIS;
    }
    CalcFixedAspectRatioTranslateLimits(type, mainMoveAxis_);
    return true;
}

void MoveDragController::ConvertXYByAspectRatio(int32_t& tx, int32_t& ty, float aspectRatio)
{
    if (mainMoveAxis_ == AxisType::X_AXIS) {
        ty = tx / aspectRatio;
    } else if (mainMoveAxis_ == AxisType::Y_AXIS) {
        tx = ty * aspectRatio;
    }
    return;
}

void MoveDragController::InitDecorValue(const sptr<WindowSessionProperty> property,
    const SystemSessionConfig& sysConfig)
{
    isDecorEnable_ = WindowHelper::IsMainWindow(property->GetWindowType()) &&
        sysConfig.isSystemDecorEnable_ &&
        WindowHelper::IsWindowModeSupported(sysConfig.decorModeSupportInfo_, property->GetWindowMode());
}

void MoveDragController::RequestVsync()
{
    VsyncStation::GetInstance().RequestVsync(vsyncCallback_);
}

void MoveDragController::RemoveVsync()
{
    VsyncStation::GetInstance().RemoveCallback();
}

void MoveDragController::OnReceiveVsync(int64_t timeStamp)
{
    WLOGFD("[OnReceiveVsync] receive event, time: %{public}" PRId64"", timeStamp);
    if (vsyncHandleFunc_) {
        vsyncHandleFunc_();
    }
}
} // namespace OHOS::Rosen
