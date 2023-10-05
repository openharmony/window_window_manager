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

#include <hitrace_meter.h>
#include <pointer_event.h>
#include "input_manager.h"

#include "session/host/include/scene_persistent_storage.h"
#include "session/host/include/session_utils.h"
#include "session_manager/include/screen_session_manager.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "wm_common_inner.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "MoveDragController" };
}

MoveDragController::MoveDragController(int32_t persistentId)
{
    persistentId_ = persistentId;
}

void MoveDragController::RegisterMoveDragCallback(const MoveDragCallback& callBack)
{
    moveDragCallback_ = callBack;
}

void MoveDragController::SetStartMoveFlag(bool flag)
{
    if (flag && (!hasPointDown_ || isStartDrag_)) {
        WLOGFD("StartMove, but has not pointed down or is dragging, hasPointDown_: %{public}d, isStartFlag: %{public}d",
            hasPointDown_, isStartDrag_);
        return;
    }
    isStartMove_ = flag;
    WLOGFD("SetStartMoveFlag, isStartMove_: %{public}d", isStartMove_);
}

bool MoveDragController::GetStartMoveFlag() const
{
    WLOGFD("GetStartMoveFlag, isStartMove_: %{public}d", isStartMove_);
    return isStartMove_;
}

bool MoveDragController::GetStartDragFlag() const
{
    return isStartDrag_;
}

WSRect MoveDragController::GetTargetRect() const
{
    return moveDragProperty_.targetRect_;
}

void MoveDragController::InitMoveDragProperty()
{
    moveDragProperty_ = { -1, -1, -1, -1, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
}

void MoveDragController::SetOriginalValue(int32_t pointerId, int32_t pointerType,
    int32_t pointerPosX, int32_t pointerPosY, const WSRect& winRect)
{
    moveDragProperty_.pointerId_ = pointerId;
    moveDragProperty_.pointerType_ = pointerType;
    moveDragProperty_.originalPointerPosX_ = pointerPosX;
    moveDragProperty_.originalPointerPosY_ = pointerPosY;
    moveDragProperty_.originalRect_ = winRect;
}

void MoveDragController::SetAspectRatio(float ratio)
{
    aspectRatio_ = ratio;
}

bool MoveDragController::ConsumeMoveEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
    const WSRect& originalRect)
{
    if (pointerEvent == nullptr) {
        WLOGE("ConsumeMoveEvent stop because of nullptr");
        return false;
    }
    if (GetStartDragFlag()) {
        WLOGFI("the window is being resized");
        return false;
    }
    int32_t pointerId = pointerEvent->GetPointerId();
    int32_t startPointerId = moveDragProperty_.pointerId_;
    int32_t startPointerType = moveDragProperty_.pointerType_;
    if ((startPointerId != -1 && startPointerId != pointerId) ||
        (startPointerType != -1 && pointerEvent->GetSourceType() != startPointerType)) {
        WLOGFI("block unnecessary pointer event inside the window");
        return false;
    }
    MMI::PointerEvent::PointerItem pointerItem;
    int32_t sourceType = pointerEvent->GetSourceType();
    if (!pointerEvent->GetPointerItem(pointerId, pointerItem) ||
        (sourceType == MMI::PointerEvent::SOURCE_TYPE_MOUSE &&
        pointerEvent->GetButtonId() != MMI::PointerEvent::MOUSE_BUTTON_LEFT)) {
        WLOGFW("invalid pointerEvent");
        return false;
    }

    UpdateMoveTempProperty(pointerEvent);

    int32_t action = pointerEvent->GetPointerAction();
    if (!GetStartMoveFlag()) {
        if (action == MMI::PointerEvent::POINTER_ACTION_UP ||
            action == MMI::PointerEvent::POINTER_ACTION_BUTTON_UP ||
            action == MMI::PointerEvent::POINTER_ACTION_CANCEL) {
            WLOGFD("Reset hasPointDown_ when point up or cancel");
            hasPointDown_ = false;
        }
        WLOGFD("No need to move");
        return false;
    }
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    switch (action) {
        case MMI::PointerEvent::POINTER_ACTION_MOVE: {
            reason = SizeChangeReason::MOVE;
            break;
        }
        case MMI::PointerEvent::POINTER_ACTION_UP:
        case MMI::PointerEvent::POINTER_ACTION_BUTTON_UP:
        case MMI::PointerEvent::POINTER_ACTION_CANCEL: {
            reason = SizeChangeReason::DRAG_END;
            SetStartMoveFlag(false);
            hasPointDown_ = false;
            break;
        }
        default:
            break;
    }
    if (CalcMoveTargetRect(pointerEvent, originalRect)) {
        ProcessSessionRectChange(reason);
    }
    return true;
}

bool MoveDragController::ConsumeDragEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
    const WSRect& originalRect, const sptr<WindowSessionProperty> property, const SystemSessionConfig& sysConfig)
{
    if (!CheckDragEventLegal(pointerEvent, property)) {
        return false;
    }

    int32_t pointerId = pointerEvent->GetPointerId();
    MMI::PointerEvent::PointerItem pointerItem;
    if (!pointerEvent->GetPointerItem(pointerId, pointerItem)) {
        WLOGE("Get PointerItem failed");
        return false;
    }

    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    switch (pointerEvent->GetPointerAction()) {
        case MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN:
        case MMI::PointerEvent::POINTER_ACTION_DOWN: {
            if (!EventDownInit(pointerEvent, originalRect, property, sysConfig)) {
                return false;
            }
            reason = SizeChangeReason::DRAG_START;
            break;
        }
        case MMI::PointerEvent::POINTER_ACTION_MOVE: {
            reason = SizeChangeReason::DRAG;
            break;
        }
        case MMI::PointerEvent::POINTER_ACTION_UP:
        case MMI::PointerEvent::POINTER_ACTION_BUTTON_UP:
        case MMI::PointerEvent::POINTER_ACTION_CANCEL: {
            reason = SizeChangeReason::DRAG_END;
            isStartDrag_ = false;
            hasPointDown_ = false;
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
    ProcessSessionRectChange(reason);
    return true;
}

bool MoveDragController::CalcMoveTargetRect(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
    const WSRect& originalRect)
{
    MMI::PointerEvent::PointerItem pointerItem;
    int32_t pointerId = pointerEvent->GetPointerId();
    pointerEvent->GetPointerItem(pointerId, pointerItem);
    int32_t pointerDisplayX = pointerItem.GetDisplayX();
    int32_t pointerDisplayY = pointerItem.GetDisplayY();
    if (moveDragProperty_.isEmpty()) {
        moveDragProperty_.pointerId_ = pointerId;
        moveDragProperty_.pointerType_ = pointerEvent->GetSourceType();
        moveDragProperty_.originalPointerPosX_ = pointerDisplayX;
        moveDragProperty_.originalPointerPosY_ = pointerDisplayY;
        int32_t pointerWindowX = pointerItem.GetWindowX();
        int32_t pointerWindowY = pointerItem.GetWindowY();
        moveDragProperty_.originalRect_ = originalRect;
        moveDragProperty_.originalRect_.posX_ = pointerDisplayX - pointerWindowX;
        moveDragProperty_.originalRect_.posY_ = pointerDisplayY - pointerWindowY;
        return false;
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
        return true;
    }
}

bool MoveDragController::EventDownInit(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
    const WSRect& originalRect, const sptr<WindowSessionProperty> property, const SystemSessionConfig& sysConfig)
{
    if (pointerEvent->GetSourceType() == MMI::PointerEvent::SOURCE_TYPE_MOUSE &&
        pointerEvent->GetButtonId() != MMI::PointerEvent::MOUSE_BUTTON_LEFT) {
        return false;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "MoveDragController::EventDownInit");
    int32_t pointerId = pointerEvent->GetPointerId();
    MMI::PointerEvent::PointerItem pointerItem;
    pointerEvent->GetPointerItem(pointerId, pointerItem);
    InitMoveDragProperty();
    hasPointDown_ = true;
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
    moveDragProperty_.pointerType_ = pointerEvent->GetSourceType();
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
        targetRect.width_ -= static_cast<uint32_t>(tranX);
    } else if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::RIGHT)) {
        targetRect.width_ += static_cast<uint32_t>(tranX);
    }
    if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::TOP)) {
        targetRect.posY_ += tranY;
        targetRect.height_ -= static_cast<uint32_t>(tranY);
    } else if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::BOTTOM)) {
        targetRect.height_ += static_cast<uint32_t>(tranY);
    }
    // check current ratio limits
    if (targetRect.height_ == 0) {
        return targetRect;
    }
    float curRatio = static_cast<float>(targetRect.width_) / static_cast<float>(targetRect.height_);
    if (!MathHelper::GreatNotEqual(limits_.minRatio_, curRatio) &&
        !MathHelper::GreatNotEqual(curRatio, limits_.maxRatio_)) {
        return targetRect;
    }
    float newRatio = MathHelper::LessNotEqual(curRatio, limits_.minRatio_) ? limits_.minRatio_ : limits_.maxRatio_;
    if (MathHelper::NearZero(newRatio)) {
        return targetRect;
    }
    if ((static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::LEFT)) ||
        (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::RIGHT))) {
        targetRect.height_ = static_cast<uint32_t>(static_cast<float>(targetRect.width_) / newRatio);
    } else {
        targetRect.width_ = static_cast<uint32_t>(static_cast<float>(targetRect.height_) * newRatio);
    }
    return targetRect;
}

WSRect MoveDragController::CalcFixedAspectRatioTargetRect(AreaType type, int32_t tranX, int32_t tranY,
    float aspectRatio, WSRect originalRect)
{
    int32_t posX = originalRect.posX_;
    int32_t posY = originalRect.posY_;
    int32_t width = static_cast<int32_t>(originalRect.width_);
    int32_t height = static_cast<int32_t>(originalRect.height_);
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
    if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::LEFT)) {
        minTranX_ = static_cast<int32_t>(moveDragProperty_.originalRect_.width_ - limits_.maxWidth_);
        maxTranX_ = static_cast<int32_t>(moveDragProperty_.originalRect_.width_ - limits_.minWidth_);
    } else if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::RIGHT)) {
        minTranX_ = static_cast<int32_t>(limits_.minWidth_ - moveDragProperty_.originalRect_.width_);
        maxTranX_ = static_cast<int32_t>(limits_.maxWidth_ - moveDragProperty_.originalRect_.width_);
    }
    if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::TOP)) {
        minTranY_ = static_cast<int32_t>(moveDragProperty_.originalRect_.height_ - limits_.maxHeight_);
        maxTranY_ = static_cast<int32_t>(moveDragProperty_.originalRect_.height_ - limits_.minHeight_);
    } else if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::BOTTOM)) {
        minTranY_ = static_cast<int32_t>(limits_.minHeight_ - moveDragProperty_.originalRect_.height_);
        maxTranY_ = static_cast<int32_t>(limits_.maxHeight_ - moveDragProperty_.originalRect_.height_);
    }
}

void MoveDragController::CalcFixedAspectRatioTranslateLimits(AreaType type, AxisType axis)
{
    int32_t minW = static_cast<int32_t>(limits_.minWidth_);
    int32_t maxW = static_cast<int32_t>(limits_.maxWidth_);
    int32_t minH = static_cast<int32_t>(limits_.minHeight_);
    int32_t maxH = static_cast<int32_t>(limits_.maxHeight_);
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
            minTranX_ = static_cast<int32_t>(moveDragProperty_.originalRect_.width_) - maxW;
            maxTranX_ = static_cast<int32_t>(moveDragProperty_.originalRect_.width_) - minW;
        } else if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::RIGHT)) {
            minTranX_ = minW - static_cast<int32_t>(moveDragProperty_.originalRect_.width_);
            maxTranX_ = maxW - static_cast<int32_t>(moveDragProperty_.originalRect_.width_);
        }
    } else if (axis == AxisType::Y_AXIS) {
        if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::TOP)) {
            minTranY_ = static_cast<int32_t>(moveDragProperty_.originalRect_.height_) - maxH;
            maxTranY_ = static_cast<int32_t>(moveDragProperty_.originalRect_.height_) - minH;
        } else if (static_cast<uint32_t>(type) & static_cast<uint32_t>(AreaType::BOTTOM)) {
            minTranY_ = minH - static_cast<int32_t>(moveDragProperty_.originalRect_.height_);
            maxTranY_ = maxH - static_cast<int32_t>(moveDragProperty_.originalRect_.height_);
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

void MoveDragController::ProcessSessionRectChange(const SizeChangeReason& reason)
{
    if (moveDragCallback_) {
        moveDragCallback_(reason);
    }
}

float MoveDragController::GetVirtualPixelRatio() const
{
    float vpr = 1.5;
    auto displayInfo = ScreenSessionManager::GetInstance().GetDefaultDisplayInfo();
    if (displayInfo != nullptr) {
        vpr = displayInfo->GetVirtualPixelRatio();
    }
    WLOGFD("vpr: %{public}f", vpr);
    return vpr;
}

void MoveDragController::UpdateDragType(int32_t startPointPosX, int32_t startPointPosY)
{
    if (startPointPosX > rectExceptCorner_.posX_ &&
        (startPointPosX < rectExceptCorner_.posX_ +
        static_cast<int32_t>(rectExceptCorner_.width_))) {
        dragType_ = DragType::DRAG_BOTTOM_OR_TOP;
    } else if (startPointPosY > rectExceptCorner_.posY_ &&
        (startPointPosY < rectExceptCorner_.posY_ +
        static_cast<int32_t>(rectExceptCorner_.height_))) {
        dragType_ = DragType::DRAG_LEFT_OR_RIGHT;
    } else if ((startPointPosX <= rectExceptCorner_.posX_ && startPointPosY <= rectExceptCorner_.posY_) ||
        (startPointPosX >= rectExceptCorner_.posX_ + static_cast<int32_t>(rectExceptCorner_.width_) &&
         startPointPosY >= rectExceptCorner_.posY_ + static_cast<int32_t>(rectExceptCorner_.height_))) {
        dragType_ = DragType::DRAG_LEFT_TOP_CORNER;
    } else {
        dragType_ = DragType::DRAG_RIGHT_TOP_CORNER;
    }
}

bool MoveDragController::IsPointInDragHotZone(int32_t startPointPosX, int32_t startPointPosY,
    int32_t sourceType, const WSRect& winRect)
{
    // calculate rect with hotzone
    Rect rectWithHotzone;
    rectWithHotzone.posX_ = winRect.posX_ - static_cast<int32_t>(HOTZONE_POINTER);
    rectWithHotzone.posY_ = winRect.posY_ - static_cast<int32_t>(HOTZONE_POINTER);
    rectWithHotzone.width_ = winRect.width_ + HOTZONE_POINTER * 2u; // double hotZone
    rectWithHotzone.height_ = winRect.height_ + HOTZONE_POINTER * 2u; // double hotZone

    if (sourceType == MMI::PointerEvent::SOURCE_TYPE_MOUSE &&
        !WindowHelper::IsPointInTargetRectWithBound(startPointPosX, startPointPosY, rectWithHotzone)) {
        return false;
    } else if ((!WindowHelper::IsPointInTargetRect(startPointPosX,
        startPointPosY, rectExceptFrame_)) ||
        (!WindowHelper::IsPointInWindowExceptCorner(startPointPosX,
        startPointPosY, rectExceptCorner_))) {
        return true;
    }
    return false;
}

void MoveDragController::CalculateStartRectExceptHotZone(float vpr, const WSRect& winRect)
{
    rectExceptFrame_.posX_ = winRect.posX_ +
        static_cast<int32_t>(WINDOW_FRAME_WIDTH * vpr);
    rectExceptFrame_.posY_ = winRect.posY_ +
        static_cast<int32_t>(WINDOW_FRAME_WIDTH * vpr);
    rectExceptFrame_.width_ = winRect.width_ -
        static_cast<uint32_t>((WINDOW_FRAME_WIDTH + WINDOW_FRAME_WIDTH) * vpr);
    rectExceptFrame_.height_ = winRect.height_ -
        static_cast<uint32_t>((WINDOW_FRAME_WIDTH + WINDOW_FRAME_WIDTH) * vpr);

    rectExceptCorner_.posX_ = winRect.posX_ +
        static_cast<int32_t>(WINDOW_FRAME_CORNER_WIDTH * vpr);
    rectExceptCorner_.posY_ = winRect.posY_ +
        static_cast<int32_t>(WINDOW_FRAME_CORNER_WIDTH * vpr);
    rectExceptCorner_.width_ = winRect.width_ -
        static_cast<uint32_t>((WINDOW_FRAME_CORNER_WIDTH + WINDOW_FRAME_CORNER_WIDTH) * vpr);
    rectExceptCorner_.height_ = winRect.height_ -
        static_cast<uint32_t>((WINDOW_FRAME_CORNER_WIDTH + WINDOW_FRAME_CORNER_WIDTH) * vpr);
}

void MoveDragController::HandleMouseStyle(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, const WSRect& winRect)
{
    if (pointerEvent == nullptr) {
        WLOGFE("pointerEvent is nullptr");
        return;
    }
    int32_t action = pointerEvent->GetPointerAction();
    int32_t sourceType = pointerEvent->GetSourceType();
    if (!(sourceType == MMI::PointerEvent::SOURCE_TYPE_MOUSE &&
        (action == MMI::PointerEvent::POINTER_ACTION_MOVE ||
         action == MMI::PointerEvent::POINTER_ACTION_BUTTON_UP ||
         action == MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN))) {
        WLOGFD("Not mouse type or not dowm/move/up event");
        return;
    }

    MMI::PointerEvent::PointerItem pointerItem;
    if (!pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem)) {
        WLOGFE("Get pointeritem failed, PointerId:%{public}d", pointerEvent->GetPointerId());
        pointerEvent->MarkProcessed();
        return;
    }

    int32_t mousePointX = pointerItem.GetDisplayX();
    int32_t mousePointY = pointerItem.GetDisplayY();
    uint32_t oriStyleID = mouseStyleID_;
    uint32_t newStyleID = 0;

    float vpr = GetVirtualPixelRatio();
    CalculateStartRectExceptHotZone(vpr, winRect);
    if (IsPointInDragHotZone(mousePointX, mousePointY, sourceType, winRect)) {
        UpdateDragType(mousePointX, mousePointY);
        newStyleID = STYLEID_MAP.at(dragType_);
    } else if (action == MMI::PointerEvent::POINTER_ACTION_BUTTON_UP) {
        newStyleID = MMI::MOUSE_ICON::DEFAULT;
    }

    WLOGFI("Id: %{public}d, Mouse posX : %{public}u, posY %{public}u, Pointer action : %{public}u, "
        "winRect posX : %{public}u, posY : %{public}u, W : %{public}u, H : %{public}u, "
        "newStyle : %{public}u, oldStyle : %{public}u",
        persistentId_, mousePointX, mousePointY, action, winRect.posX_,
        winRect.posY_, winRect.width_, winRect.height_, newStyleID, oriStyleID);
    if (oriStyleID != newStyleID) {
        MMI::PointerStyle pointerStyle;
        pointerStyle.id = static_cast<int32_t>(newStyleID);
        int32_t res = MMI::InputManager::GetInstance()->SetPointerStyle(0, pointerStyle);
        if (res != 0) {
            WLOGFE("set pointer style failed, res is %{public}u", res);
            return;
        }
        mouseStyleID_ = newStyleID;
    }
}


WSError MoveDragController::UpdateMoveTempProperty(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    int32_t pointerId = pointerEvent->GetPointerId();
    int32_t startPointerId = moveTempProperty_.pointerId_;
    int32_t pointerType = pointerEvent->GetSourceType();
    int32_t startPointerType = moveDragProperty_.pointerType_;
    MMI::PointerEvent::PointerItem pointerItem;
    int32_t sourceType = pointerEvent->GetSourceType();
    if (!pointerEvent->GetPointerItem(pointerId, pointerItem) ||
        (sourceType == MMI::PointerEvent::SOURCE_TYPE_MOUSE &&
        pointerEvent->GetButtonId() != MMI::PointerEvent::MOUSE_BUTTON_LEFT)) {
        WLOGFW("invalid pointerEvent");
        return WSError::WS_ERROR_NULLPTR;
    }

    int32_t pointerDisplayX = pointerItem.GetDisplayX();
    int32_t pointerDisplayY = pointerItem.GetDisplayY();
    switch (pointerEvent->GetPointerAction()) {
        case MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN:
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
            moveTempProperty_.pointerId_ = pointerId;
            moveTempProperty_.pointerType_ = pointerType;
            moveTempProperty_.lastDownPointerPosX_ = pointerDisplayX;
            moveTempProperty_.lastDownPointerPosY_ = pointerDisplayY;
            moveTempProperty_.lastDownPointerWindowX_ = pointerItem.GetWindowX();
            moveTempProperty_.lastDownPointerWindowY_ = pointerItem.GetWindowY();
            break;
        case MMI::PointerEvent::POINTER_ACTION_MOVE:
            if ((startPointerId != -1 && startPointerId != pointerId) ||
                (startPointerType != -1 && pointerType != startPointerType)) {
                WLOGFI("block unnecessary pointer event inside the window");
                return WSError::WS_DO_NOTHING;
            }
            moveTempProperty_.lastMovePointerPosX_ = pointerDisplayX;
            moveTempProperty_.lastMovePointerPosY_ = pointerDisplayY;
            break;
        case MMI::PointerEvent::POINTER_ACTION_UP:
        case MMI::PointerEvent::POINTER_ACTION_BUTTON_UP:
        case MMI::PointerEvent::POINTER_ACTION_CANCEL: {
            moveTempProperty_ = { -1, -1, -1, -1, -1, -1, -1, -1 };
            break;
        }
        default:
            break;
    }
    return WSError::WS_OK;
}

void MoveDragController::ClacFirstMoveTargetRect(const WSRect& windowRect)
{
    if (!GetStartMoveFlag() || moveTempProperty_.isEmpty()) {
        return;
    }

    WSRect originalRect = {
        moveTempProperty_.lastDownPointerPosX_ - moveTempProperty_.lastDownPointerWindowX_,
        moveTempProperty_.lastDownPointerPosY_ - moveTempProperty_.lastDownPointerWindowY_,
        windowRect.width_,
        windowRect.height_
    };
    SetOriginalValue(moveTempProperty_.pointerId_, moveTempProperty_.pointerType_,
        moveTempProperty_.lastDownPointerPosX_, moveTempProperty_.lastDownPointerPosY_, originalRect);

    int32_t offsetX = moveTempProperty_.lastMovePointerPosX_ - moveTempProperty_.lastDownPointerPosX_;
    int32_t offsetY = moveTempProperty_.lastMovePointerPosY_ - moveTempProperty_.lastDownPointerPosY_;
    WSRect targetRect = {
        originalRect.posX_ + offsetX,
        originalRect.posY_ + offsetY,
        originalRect.width_,
        originalRect.height_
    };
    WLOGFD("first move rect: [%{public}d, %{public}d, %{public}u, %{public}u]", targetRect.posX_, targetRect.posY_,
        targetRect.width_, targetRect.height_);
    moveDragProperty_.targetRect_ = targetRect;
    ProcessSessionRectChange(SizeChangeReason::MOVE);
}

bool MoveDragController::CheckDragEventLegal(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
    const sptr<WindowSessionProperty> property)
{
    if (pointerEvent == nullptr || property == nullptr) {
        WLOGE("ConsumeDragEvent stop because of nullptr");
        return false;
    }
    if (GetStartMoveFlag()) {
        WLOGFI("the window is being moved");
        return false;
    }
    if (!GetStartDragFlag() && pointerEvent->GetPointerAction() != MMI::PointerEvent::POINTER_ACTION_DOWN &&
        pointerEvent->GetPointerAction() != MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN) {
        return false;
    }
    int32_t pointerId = pointerEvent->GetPointerId();
    int32_t startPointerId = moveDragProperty_.pointerId_;
    if (GetStartDragFlag() && startPointerId != -1 && startPointerId != pointerId) {
        WLOGFI("block unnecessary pointer event inside the window");
        return false;
    }
    return true;
}
} // namespace OHOS::Rosen
