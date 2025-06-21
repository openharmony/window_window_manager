/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "session/host/include/pc_fold_screen_manager.h"
#include "display_manager.h"
#include "window_manager_hilog.h"
#include "wm_math.h"

namespace OHOS::Rosen {
namespace {
// moving
constexpr int32_t MOVING_RESPONSE = 50;
constexpr float MOVING_DAMPING_RATIO = 0.98f;
const RSAnimationTimingProtocol MOVING_TIMING_PROTOCOL(MOVING_RESPONSE); // animation time
const RSAnimationTimingCurve MOVING_CURVE =
    RSAnimationTimingCurve::CreateSpring(static_cast<float>(MOVING_RESPONSE / 1000.0f), MOVING_DAMPING_RATIO, 0.0f);

// throw-slip
constexpr float THROW_BACKTRACING_DURATION = 100.0f;
constexpr int32_t THROW_BACKTRACING_THRESHOLD = 200;
constexpr float THROW_SLIP_TIME = 416.0f;
constexpr float THROW_SLIP_DAMPING_RATIO = 0.7947f; // stiffness = 228, damping = 24
constexpr float THROW_SLIP_DECELERATION_RATE = 0.002;
const RSAnimationTimingProtocol THROW_SLIP_TIMING_PROTOCOL(std::round(THROW_SLIP_TIME)); // animation time
const RSAnimationTimingCurve THROW_SLIP_CURVE =
    RSAnimationTimingCurve::CreateSpring(THROW_SLIP_TIME / 1000.0f, THROW_SLIP_DAMPING_RATIO, 0.0f);

// arrange rule
constexpr int32_t RULE_TRANS_X = 48; // dp
constexpr int32_t MIN_DECOR_HEIGHT = 37;
constexpr WSRect RECT_ZERO = { 0, 0, 0, 0 };

// all displayId
constexpr DisplayId DEFAULT_DISPLAY_ID = 0;
constexpr DisplayId VIRTUAL_DISPLAY_ID = 999;
} // namespace

WM_IMPLEMENT_SINGLE_INSTANCE(PcFoldScreenManager);

void PcFoldScreenManager::UpdateFoldScreenStatus(DisplayId displayId, SuperFoldStatus status,
    const WSRect& defaultDisplayRect, const WSRect& virtualDisplayRect, const WSRect& foldCreaseRect)
{
    DisplayId preDisplayId = DEFAULT_DISPLAY_ID;
    SuperFoldStatus preStatus = SuperFoldStatus::UNKNOWN;
    {
        std::unique_lock<std::shared_mutex> lock(displayInfoMutex_);
        preDisplayId = displayId_;
        preStatus = screenFoldStatus_;
    }
    SetDisplayInfo(displayId, status);
    SetDisplayRects(defaultDisplayRect, virtualDisplayRect, foldCreaseRect);
    if (preDisplayId == displayId && preStatus == status) {
        return;
    }
    ExecuteFoldScreenStatusChangeCallbacks(displayId, status, preStatus);
}

void PcFoldScreenManager::SetDisplayInfo(DisplayId displayId, SuperFoldStatus status)
{
    std::unique_lock<std::shared_mutex> lock(displayInfoMutex_);
    if (displayId_ == displayId && screenFoldStatus_ == status) {
        return;
    }
    TLOGI(WmsLogTag::WMS_MAIN, "display: %{public}" PRIu64", fold status: %{public}d",
        displayId, static_cast<int32_t>(status));
    prevScreenFoldStatus_ = screenFoldStatus_;
    screenFoldStatus_ = status;
    ResetArrangeRule();
    displayId_ = displayId;
    auto display = DisplayManager::GetInstance().GetDisplayById(displayId);
    if (display == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "Failed to get display");
        return;
    }
    vpr_ = display->GetVirtualPixelRatio();
    TLOGI(WmsLogTag::WMS_MAIN, "vpr: %{public}f", vpr_);
}

void PcFoldScreenManager::SetDisplayRects(
    const WSRect& defaultDisplayRect, const WSRect& virtualDisplayRect, const WSRect& foldCreaseRect)
{
    TLOGI(WmsLogTag::WMS_LAYOUT_PC, "%{public}s, %{public}s, %{public}s", defaultDisplayRect.ToString().c_str(),
        virtualDisplayRect.ToString().c_str(), foldCreaseRect.ToString().c_str());
    std::unique_lock<std::shared_mutex> lock(rectsMutex_);
    defaultDisplayRect_ = defaultDisplayRect;
    virtualDisplayRect_ = virtualDisplayRect;
    foldCreaseRect_ = foldCreaseRect;
}

SuperFoldStatus PcFoldScreenManager::GetScreenFoldStatus() const
{
    std::shared_lock<std::shared_mutex> lock(displayInfoMutex_);
    return screenFoldStatus_;
}

SuperFoldStatus PcFoldScreenManager::GetScreenFoldStatus(DisplayId displayId) const
{
    std::shared_lock<std::shared_mutex> lock(displayInfoMutex_);
    if (displayId_ != displayId) {
        return SuperFoldStatus::UNKNOWN;
    }
    return screenFoldStatus_;
}

bool PcFoldScreenManager::IsHalfFolded(DisplayId displayId) const
{
    std::shared_lock<std::shared_mutex> lock(displayInfoMutex_);
    return screenFoldStatus_ == SuperFoldStatus::HALF_FOLDED && displayId_ == displayId;
}

bool PcFoldScreenManager::IsHalfFoldedOnMainDisplay(DisplayId displayId) const
{
    std::shared_lock<std::shared_mutex> lock(displayInfoMutex_);
    return screenFoldStatus_ == SuperFoldStatus::HALF_FOLDED && displayId == DEFAULT_DISPLAY_ID;
}

bool PcFoldScreenManager::IsPcFoldScreen(DisplayId displayId) const
{
    return displayId == DEFAULT_DISPLAY_ID || displayId == VIRTUAL_DISPLAY_ID;
}

void PcFoldScreenManager::UpdateSystemKeyboardStatus(bool hasSystemKeyboard)
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "status: %{public}d", hasSystemKeyboard);
    std::unique_lock<std::shared_mutex> lock(displayInfoMutex_);
    if (hasSystemKeyboard_ == hasSystemKeyboard) {
        return;
    }
    hasSystemKeyboard_ = hasSystemKeyboard;
    ExecuteSystemKeyboardStatusChangeCallbacks(displayId_, hasSystemKeyboard_);
}

bool PcFoldScreenManager::HasSystemKeyboard() const
{
    std::shared_lock<std::shared_mutex> lock(displayInfoMutex_);
    return hasSystemKeyboard_;
}

float PcFoldScreenManager::GetVpr() const
{
    std::shared_lock<std::shared_mutex> lock(displayInfoMutex_);
    return vpr_;
}

int32_t PcFoldScreenManager::GetVirtualDisplayPosY() const
{
    std::shared_lock<std::shared_mutex> lock(rectsMutex_);
    return defaultDisplayRect_.height_ + foldCreaseRect_.height_;
}

std::tuple<WSRect, WSRect, WSRect> PcFoldScreenManager::GetDisplayRects() const
{
    std::shared_lock<std::shared_mutex> lock(rectsMutex_);
    return { defaultDisplayRect_, virtualDisplayRect_, foldCreaseRect_ };
}

RSAnimationTimingProtocol PcFoldScreenManager::GetMovingTimingProtocol()
{
    return MOVING_TIMING_PROTOCOL;
}

RSAnimationTimingCurve PcFoldScreenManager::GetMovingTimingCurve()
{
    return MOVING_CURVE;
}

RSAnimationTimingProtocol PcFoldScreenManager::GetThrowSlipTimingProtocol()
{
    return THROW_SLIP_TIMING_PROTOCOL;
}

RSAnimationTimingCurve PcFoldScreenManager::GetThrowSlipTimingCurve()
{
    return THROW_SLIP_CURVE;
}

ScreenSide PcFoldScreenManager::CalculateScreenSide(const WSRect& rect)
{
    int32_t midPosY = rect.height_ / 2 + rect.posY_; // 2: center
    const auto& [defaultDisplayRect, virtualDisplayRect, foldCreaseRect] = GetDisplayRects();
    return midPosY <= (foldCreaseRect.posY_ + foldCreaseRect.height_ / 2) ? // 2: center
        ScreenSide::FOLD_B : ScreenSide::FOLD_C;
}

ScreenSide PcFoldScreenManager::CalculateScreenSide(int32_t posY)
{
    const auto& [defaultDisplayRect, virtualDisplayRect, foldCreaseRect] = GetDisplayRects();
    return posY < foldCreaseRect.posY_ ? ScreenSide::FOLD_B : ScreenSide::FOLD_C;
}

bool PcFoldScreenManager::IsCrossFoldCrease(const WSRect& rect)
{
    const auto& [defaultDisplayRect, virtualDisplayRect, foldCreaseRect] = GetDisplayRects();
    const int32_t midScreenY = foldCreaseRect.posY_ + foldCreaseRect.height_ / 2; // 2: center
    return rect.posY_ < midScreenY && rect.posY_ + rect.height_ > midScreenY;
}

void PcFoldScreenManager::ResetArrangeRule()
{
    std::unique_lock<std::mutex> lock(arrangedRectsMutex_);
    defaultArrangedRect_ = RECT_ZERO;
    virtualArrangedRect_ = RECT_ZERO;
}

void PcFoldScreenManager::ResetArrangeRule(const WSRect& rect)
{
    ResetArrangeRule(CalculateScreenSide(rect));
}

void PcFoldScreenManager::ResetArrangeRule(ScreenSide side)
{
    if (side != ScreenSide::FOLD_B && side != ScreenSide::FOLD_C) {
        TLOGD(WmsLogTag::WMS_LAYOUT_PC, "invalid side: %{public}d", static_cast<int32_t>(side));
        return;
    }
    std::unique_lock<std::mutex> lock(arrangedRectsMutex_);
    if (side == ScreenSide::FOLD_B) {
        defaultArrangedRect_ = RECT_ZERO;
    } else { // FOLD_C
        virtualArrangedRect_ = RECT_ZERO;
    }
}

void PcFoldScreenManager::ResizeToFullScreen(WSRect& rect, int32_t topAvoidHeight, int32_t botAvoidHeight)
{
    ScreenSide side = CalculateScreenSide(rect);
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "side: %{public}d, rect: %{public}s",
        static_cast<int32_t>(side), rect.ToString().c_str());
    if (side != ScreenSide::FOLD_B && side != ScreenSide::FOLD_C) {
        TLOGW(WmsLogTag::WMS_LAYOUT_PC, "rule not avaliable, side %{public}d", static_cast<int32_t>(side));
        return;
    }

    ResetArrangeRule(side);
    // calculate limit rect
    const auto& [defaultDisplayRect, virtualDisplayRect, foldCreaseRect] = GetDisplayRects();
    WSRect limitRect = RECT_ZERO;
    if (side == ScreenSide::FOLD_B) {
        limitRect.posX_ = defaultDisplayRect.posX_;
        limitRect.posY_ = defaultDisplayRect.posY_ + topAvoidHeight;
        limitRect.width_ = defaultDisplayRect.width_;
        limitRect.height_ = foldCreaseRect.posY_ - limitRect.posY_;
    } else { // FOLD_C
        limitRect.posX_ = virtualDisplayRect.posX_;
        limitRect.posY_ = foldCreaseRect.posY_ + foldCreaseRect.height_;
        limitRect.width_ = virtualDisplayRect.width_;
        limitRect.height_ = virtualDisplayRect.posY_ + virtualDisplayRect.height_ - botAvoidHeight - limitRect.posY_;
    }

    rect = limitRect;
}

bool PcFoldScreenManager::NeedDoThrowSlip(const WSRect& rect, const WSRectF& velocity, ScreenSide& throwSide)
{
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "rect: %{public}s, velocity: %{public}s, throwSide: %{public}d",
        rect.ToString().c_str(), velocity.ToString().c_str(), static_cast<int32_t>(throwSide));

    // velocity check
    const WSRect& backtracingRect = CalculateThrowBacktracingRect(rect, velocity);
    if (!CheckVelocityOrientation(backtracingRect, velocity) && !IsCrossFoldCrease(rect)) {
        TLOGD(WmsLogTag::WMS_LAYOUT_PC, "orientation check failed, rect: %{public}s, velocity: %{public}s",
            backtracingRect.ToString().c_str(), velocity.ToString().c_str());
        return false;
    }

    ScreenSide startSide = CalculateScreenSide(backtracingRect);
    if (IsCrossFoldCrease(rect)) {
        startSide = MathHelper::GreatNotEqual(velocity.posY_, 0.0f) ? ScreenSide::FOLD_B : ScreenSide::FOLD_C;
    }
    const WSRect& endRect = CalculateThrowEnd(backtracingRect, velocity);
    const ScreenSide endSide = CalculateScreenSide(endRect);
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "backtracingRect: %{public}s, endRect: %{public}s",
        backtracingRect.ToString().c_str(), endRect.ToString().c_str());
    if (startSide == ScreenSide::FOLD_B && endSide == ScreenSide::FOLD_C) {
        throwSide = startSide;
        return true;
    }
    if (startSide == ScreenSide::FOLD_C && endSide == ScreenSide::FOLD_B) {
        throwSide = startSide;
        return true;
    }
    return false;
}

/*
 * only for fullscreen cross-axis throw slip
 */
bool PcFoldScreenManager::NeedDoEasyThrowSlip(const WSRect& rect, const WSRect& startRect,
    const WSRectF& velocity, ScreenSide& throwSide)
{
    TLOGD(WmsLogTag::WMS_LAYOUT_PC,
        "rect: %{public}s, startRect: %{public}s, velocity: %{public}s, throwSide: %{public}d",
        rect.ToString().c_str(), startRect.ToString().c_str(),
        velocity.ToString().c_str(), static_cast<int32_t>(throwSide));

    ScreenSide startSide = CalculateScreenSide(startRect);
    if (startSide == throwSide) {
        return NeedDoThrowSlip(rect, velocity, throwSide);
    }

    const auto& [defaultDisplayRect, virtualDisplayRect, foldCreaseRect] = GetDisplayRects();
    WSRect easyThrowRect = rect;
    int32_t midY = rect.posY_ + rect.height_ / 2; // 2: center
    if (startSide == ScreenSide::FOLD_B) {
        if (midY > virtualDisplayRect.posY_ + virtualDisplayRect.height_ / 2) { // 2: center
            return false;
        }
        easyThrowRect.posY_ = foldCreaseRect.posY_ - easyThrowRect.height_ / 2; // 2: center
    } else {
        if (midY < defaultDisplayRect.posY_ + defaultDisplayRect.height_ / 2) { // 2: center
            return false;
        }
        easyThrowRect.posY_ = foldCreaseRect.posY_ + foldCreaseRect.height_ - easyThrowRect.height_ / 2; // 2: center
    }
    return NeedDoThrowSlip(easyThrowRect, velocity, throwSide);
}

bool PcFoldScreenManager::CheckVelocityOrientation(const WSRect& rect, const WSRectF& velocity)
{
    const auto& [defaultDisplayRect, virtualDisplayRect, foldCreaseRect] = GetDisplayRects();
    const int32_t centerX = rect.posX_ + rect.width_ / 2; // 2: center
    const int32_t centerY = rect.posY_ + rect.height_ / 2; // 2: center
    ScreenSide startSide = CalculateScreenSide(rect);
    int32_t aimX = 0;
    int32_t aimY = 0;
    if (startSide == ScreenSide::FOLD_B) {
        if (MathHelper::LessNotEqual(velocity.posX_, 0.0f)) {
            aimX = defaultDisplayRect.posX_;
            aimY = defaultDisplayRect.posY_ + defaultDisplayRect.height_;
        } else {
            aimX = defaultDisplayRect.posX_ + defaultDisplayRect.width_;
            aimY = defaultDisplayRect.posY_ + defaultDisplayRect.height_;
        }
        return MathHelper::GreatNotEqual(velocity.posY_,
            std::abs(static_cast<float>(aimY - centerY) /
                     MathHelper::NonZero(static_cast<float>(aimX - centerX)) * velocity.posX_));
    }
    if (startSide == ScreenSide::FOLD_C) {
        if (MathHelper::LessNotEqual(velocity.posX_, 0.0f)) {
            aimX = virtualDisplayRect.posX_;
            aimY = virtualDisplayRect.posY_;
        } else {
            aimX = virtualDisplayRect.posX_ + virtualDisplayRect.width_;
            aimY = virtualDisplayRect.posY_;
        }
        return MathHelper::LessNotEqual(velocity.posY_,
            -std::abs(static_cast<float>(aimY - centerY) /
                      MathHelper::NonZero(static_cast<float>(aimX - centerX)) * velocity.posX_));
    }

    return false;
}

WSRect PcFoldScreenManager::CalculateThrowBacktracingRect(const WSRect& rect, const WSRectF& velocity)
{
    int32_t midPosY = rect.height_ / 2 + rect.posY_; // 2: center
    const auto& [defaultDisplayRect, virtualDisplayRect, foldCreaseRect] = GetDisplayRects();
    const int32_t midScreenY = defaultDisplayRect.posY_ + defaultDisplayRect.height_;
    bool isInUpperThreshold = midPosY < midScreenY && midPosY >= (midScreenY - THROW_BACKTRACING_THRESHOLD);
    bool isInLowerThreshold = midPosY >= midScreenY && midPosY <= (midScreenY + THROW_BACKTRACING_THRESHOLD);
    if ((!isInUpperThreshold && MathHelper::LessNotEqual(velocity.posY_, 0.0f)) ||
        (!isInLowerThreshold && MathHelper::GreatNotEqual(velocity.posY_, 0.0f))) {
        return rect;
    }
    return WSRect{static_cast<int32_t>(rect.posX_ - velocity.posX_ * THROW_BACKTRACING_DURATION),
                  static_cast<int32_t>(rect.posY_ - velocity.posY_ * THROW_BACKTRACING_DURATION),
                  rect.width_, rect.height_};
}

WSRect PcFoldScreenManager::CalculateThrowEnd(const WSRect& rect, const WSRectF& velocity)
{
    return WSRect{ static_cast<int32_t>(rect.posX_ + velocity.posX_ / THROW_SLIP_DECELERATION_RATE),
                   static_cast<int32_t>(rect.posY_ + velocity.posY_ / THROW_SLIP_DECELERATION_RATE),
                   rect.width_, rect.height_ };
}

/*
 * move rect to other side
 * @param rect: current side, moved to other side
 * @param titleHeight: used in arrange rule to avoid title bar
 */
bool PcFoldScreenManager::ThrowSlipToOppositeSide(ScreenSide startSide, WSRect& rect,
    int32_t topAvoidHeight, int32_t botAvoidHeight, int32_t titleHeight)
{
    if (startSide != ScreenSide::FOLD_B && startSide != ScreenSide::FOLD_C) {
        return false;
    }

    const auto& [defaultDisplayRect, virtualDisplayRect, foldCreaseRect] = GetDisplayRects();
    int32_t topLimit = 0;
    int32_t botLimit = 0;
    const WSRect defaultLimitRect = {
        defaultDisplayRect.posX_, defaultDisplayRect.posY_ + topAvoidHeight,
        defaultDisplayRect.width_, defaultDisplayRect.height_ - topAvoidHeight };
    const WSRect virtualLimitRect = {
        virtualDisplayRect.posX_, virtualDisplayRect.posY_,
        virtualDisplayRect.width_, virtualDisplayRect.height_ - botAvoidHeight };
    const WSRect& startLimitRect = startSide == ScreenSide::FOLD_B ? defaultLimitRect : virtualLimitRect;
    const WSRect& endLimitRect = startSide == ScreenSide::FOLD_B ? virtualLimitRect : defaultLimitRect;
    if (rect.height_ < startLimitRect.height_ && rect.height_ < endLimitRect.height_ &&
        rect.posY_ > startLimitRect.posY_) {
        float ratio = static_cast<float>(endLimitRect.height_ - rect.height_) /
            static_cast<float>(startLimitRect.height_ - rect.height_);
        // after mutiple throw-slip, posY converges
        if (MathHelper::GreatNotEqual(ratio, 1.0f)) {
            rect.posY_ = MathHelper::Floor(ratio * static_cast<float>(rect.posY_ - startLimitRect.posY_)) +
                endLimitRect.posY_;
        } else {
            rect.posY_ = MathHelper::Ceil(ratio * static_cast<float>(rect.posY_ - startLimitRect.posY_)) +
                endLimitRect.posY_;
        }
    } else {
        rect.posY_ = rect.posY_ + endLimitRect.posY_ - startLimitRect.posY_;
    }
    // top limit first
    rect.posY_ = std::max(std::min(rect.posY_, endLimitRect.posY_ + endLimitRect.height_ - rect.height_),
                          endLimitRect.posY_);
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "end posY: %{public}d", rect.posY_);
    return true;
}

void PcFoldScreenManager::MappingRectInScreenSide(ScreenSide side, WSRect& rect,
    int32_t topAvoidHeight, int32_t botAvoidHeight)
{
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "side: %{public}d, rect: %{public}s, avoid heights: [%{public}d,%{public}d]",
        static_cast<int32_t>(side), rect.ToString().c_str(), topAvoidHeight, botAvoidHeight);
    WSRect topLeftLimit = RECT_ZERO;
    WSRect botRightLimit = RECT_ZERO;
    const auto& [defaultDisplayRect, virtualDisplayRect, foldCreaseRect] = GetDisplayRects();
    float vpr = GetVpr();
    switch (side) {
        case ScreenSide::FOLD_B:
            topLeftLimit.posX_ = MathHelper::Ceil(RULE_TRANS_X * vpr) - rect.width_;
            topLeftLimit.posY_ = topAvoidHeight;
            botRightLimit.posX_ = std::max(0,
                MathHelper::Floor(defaultDisplayRect.width_ - RULE_TRANS_X * vpr));
            botRightLimit.posY_ = std::max(0, foldCreaseRect.posY_ - rect.height_);
            botRightLimit.width_ = defaultDisplayRect.width_;
            botRightLimit.height_ = std::max(0, foldCreaseRect.posY_ - topLeftLimit.posY_);
            break;
        case ScreenSide::FOLD_C:
            topLeftLimit.posX_ = MathHelper::Ceil(RULE_TRANS_X * vpr) - rect.width_;
            topLeftLimit.posY_ = foldCreaseRect.posY_ + foldCreaseRect.height_;
            botRightLimit.posX_ = std::max(0,
                MathHelper::Floor(virtualDisplayRect.width_ - RULE_TRANS_X * vpr));
            botRightLimit.posY_ = std::max(foldCreaseRect.posY_ + foldCreaseRect.height_,
                MathHelper::Floor(virtualDisplayRect.posY_ + virtualDisplayRect.height_ -
                    botAvoidHeight - MIN_DECOR_HEIGHT * vpr));
            botRightLimit.width_ = virtualDisplayRect.width_;
            botRightLimit.height_ = std::max(0,
                virtualDisplayRect.posY_ + virtualDisplayRect.height_ - topLeftLimit.posY_ - botAvoidHeight);
            break;
        default:
            TLOGW(WmsLogTag::WMS_LAYOUT_PC, "invalid side: %{public}d", static_cast<int32_t>(side));
            return;
    }
    rect.posX_ = std::max(rect.posX_, topLeftLimit.posX_);
    rect.posY_ = std::max(rect.posY_, topLeftLimit.posY_);
    rect.posX_ = std::min(rect.posX_, botRightLimit.posX_);
    rect.posY_ = std::min(rect.posY_, botRightLimit.posY_);
    rect.width_ = std::min(rect.width_, botRightLimit.width_);
    rect.height_ = std::min(rect.height_, botRightLimit.height_);
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "limit rects: [%{public}s,%{public}s], mapped rect: %{public}s",
        topLeftLimit.ToString().c_str(), botRightLimit.ToString().c_str(), rect.ToString().c_str());
}

void PcFoldScreenManager::MappingRectInScreenSideWithArrangeRule(ScreenSide side, WSRect& rect,
    int32_t topAvoidHeight, int32_t botAvoidHeight, int32_t titleHeight)
{
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "side: %{public}d, rect: %{public}s",
        static_cast<int32_t>(side), rect.ToString().c_str());
    if (side != ScreenSide::FOLD_B && side != ScreenSide::FOLD_C) {
        TLOGW(WmsLogTag::WMS_LAYOUT_PC, "rule not avaliable, side %{public}d", static_cast<int32_t>(side));
        return;
    }

    // calculate limit rect
    const auto& [defaultDisplayRect, virtualDisplayRect, foldCreaseRect] = GetDisplayRects();
    WSRect limitRect = RECT_ZERO;
    if (side == ScreenSide::FOLD_B) {
        limitRect.posX_ = defaultDisplayRect.posX_;
        limitRect.posY_ = defaultDisplayRect.posY_ + topAvoidHeight;
        limitRect.width_ = defaultDisplayRect.width_;
        limitRect.height_ = foldCreaseRect.posY_ - limitRect.posY_;
    } else { // FOLD_C
        limitRect.posX_ = virtualDisplayRect.posX_;
        limitRect.posY_ = foldCreaseRect.posY_ + foldCreaseRect.height_;
        limitRect.width_ = virtualDisplayRect.width_;
        limitRect.height_ = virtualDisplayRect.posY_ + virtualDisplayRect.height_ - botAvoidHeight - limitRect.posY_;
    }

    {
        std::unique_lock<std::mutex> lock(arrangedRectsMutex_);
        WSRect& lastArrangedRect = (side == ScreenSide::FOLD_B) ? defaultArrangedRect_ : virtualArrangedRect_;
        if (lastArrangedRect.IsEmpty()) {
            ApplyInitArrangeRule(rect, lastArrangedRect, limitRect, titleHeight);
            TLOGD(WmsLogTag::WMS_LAYOUT_PC, "init rule, limit: %{public}s, arranged: %{public}s, rect: %{public}s",
                limitRect.ToString().c_str(), lastArrangedRect.ToString().c_str(), rect.ToString().c_str());
            return;
        }

        ApplyArrangeRule(rect, lastArrangedRect, limitRect, titleHeight);
        TLOGD(WmsLogTag::WMS_LAYOUT_PC, "apply rule, limit: %{public}s, arranged: %{public}s, rect: %{public}s",
            limitRect.ToString().c_str(), lastArrangedRect.ToString().c_str(), rect.ToString().c_str());
    }
}

/*
 * init rule: move rect to center of display
 * @param titleHeight: in vp
 */
void PcFoldScreenManager::ApplyInitArrangeRule(WSRect& rect, WSRect& lastArrangedRect,
    const WSRect& limitRect, int32_t titleHeight)
{
    rect.posX_ = std::max(limitRect.posX_, limitRect.posX_ + (limitRect.width_ - rect.width_) / 2); // 2:center align
    rect.posY_ = std::max(limitRect.posY_, limitRect.posY_ + (limitRect.height_ - rect.height_) / 2); // 2:center align
    float vpr = GetVpr();
    lastArrangedRect = { rect.posX_, rect.posY_, RULE_TRANS_X * vpr, titleHeight * vpr };
}

/*
 * init rule: move rect to bottom-right of last arranged position
 * @param titleHeight: in vp
 */
void PcFoldScreenManager::ApplyArrangeRule(WSRect& rect, WSRect& lastArrangedRect,
    const WSRect& limitRect, int32_t titleHeight)
{
    rect.posX_ = lastArrangedRect.posX_ + lastArrangedRect.width_;
    rect.posY_ = lastArrangedRect.posY_ + lastArrangedRect.height_;
    // new column
    if (rect.posY_ + rect.height_ > limitRect.posY_ + limitRect.height_) {
        rect.posY_ = limitRect.posY_;
    }
    // reset to top-left
    if (rect.posX_ + rect.width_ > limitRect.posX_ + limitRect.width_) {
        rect.posX_ = limitRect.posX_;
        rect.posY_ = limitRect.posY_;
    }
    float vpr = GetVpr();
    lastArrangedRect = { rect.posX_, rect.posY_, RULE_TRANS_X * vpr, titleHeight * vpr};
}

void PcFoldScreenManager::RegisterFoldScreenStatusChangeCallback(int32_t persistentId,
    const std::weak_ptr<FoldScreenStatusChangeCallback>& func)
{
    TLOGI(WmsLogTag::WMS_LAYOUT_PC, "id: %{public}d", persistentId);
    std::unique_lock<std::mutex> lock(callbackMutex_);
    auto [_, result] = foldScreenStatusChangeCallbacks_.insert_or_assign(persistentId, func);
    if (!result) {
        TLOGW(WmsLogTag::WMS_LAYOUT_PC, "callback has registered");
    }
}

void PcFoldScreenManager::UnregisterFoldScreenStatusChangeCallback(int32_t persistentId)
{
    TLOGI(WmsLogTag::WMS_LAYOUT_PC, "id: %{public}d", persistentId);
    std::unique_lock<std::mutex> lock(callbackMutex_);
    auto iter = foldScreenStatusChangeCallbacks_.find(persistentId);
    if (iter == foldScreenStatusChangeCallbacks_.end()) {
        TLOGW(WmsLogTag::WMS_LAYOUT_PC, "callback not registered");
        return;
    }
    foldScreenStatusChangeCallbacks_.erase(iter);
}

void PcFoldScreenManager::ExecuteFoldScreenStatusChangeCallbacks(DisplayId displayId,
    SuperFoldStatus status, SuperFoldStatus prevStatus)
{
    std::unordered_map<int32_t, std::weak_ptr<FoldScreenStatusChangeCallback>> foldScreenStatusChangeCallbacksCopy;
    {
        std::unique_lock<std::mutex> lock(callbackMutex_);
        foldScreenStatusChangeCallbacksCopy = foldScreenStatusChangeCallbacks_;
    }
    for (auto iter = foldScreenStatusChangeCallbacksCopy.begin(); iter != foldScreenStatusChangeCallbacksCopy.end();) {
        auto callback = iter->second.lock();
        if (callback == nullptr) {
            TLOGW(WmsLogTag::WMS_LAYOUT_PC, "callback invalid, id: %{public}d", iter->first);
            iter = foldScreenStatusChangeCallbacksCopy.erase(iter);
            continue;
        }
        (*callback)(displayId, status, prevStatus);
        iter++;
    }
}

void PcFoldScreenManager::RegisterSystemKeyboardStatusChangeCallback(int32_t persistentId,
    const std::weak_ptr<SystemKeyboardStatusChangeCallback>& func)
{
    TLOGI(WmsLogTag::WMS_LAYOUT_PC, "id: %{public}d", persistentId);
    std::unique_lock<std::mutex> lock(callbackMutex_);
    auto [_, result] = systemKeyboardStatusChangeCallbacks_.insert_or_assign(persistentId, func);
    if (!result) {
        TLOGW(WmsLogTag::WMS_LAYOUT_PC, "callback has registered");
    }
}

void PcFoldScreenManager::UnregisterSystemKeyboardStatusChangeCallback(int32_t persistentId)
{
    TLOGI(WmsLogTag::WMS_LAYOUT_PC, "id: %{public}d", persistentId);
    std::unique_lock<std::mutex> lock(callbackMutex_);
    auto iter = systemKeyboardStatusChangeCallbacks_.find(persistentId);
    if (iter == systemKeyboardStatusChangeCallbacks_.end()) {
        TLOGW(WmsLogTag::WMS_LAYOUT_PC, "callback not registered");
        return;
    }
    systemKeyboardStatusChangeCallbacks_.erase(iter);
}

void PcFoldScreenManager::ExecuteSystemKeyboardStatusChangeCallbacks(DisplayId displayId, bool hasSystemKeyboard)
{
    std::unique_lock<std::mutex> lock(callbackMutex_);
    for (auto iter = systemKeyboardStatusChangeCallbacks_.begin();
        iter != systemKeyboardStatusChangeCallbacks_.end();) {
        auto callback = iter->second.lock();
        if (callback == nullptr) {
            TLOGW(WmsLogTag::WMS_LAYOUT_PC, "callback invalid, id: %{public}d", iter->first);
            iter = systemKeyboardStatusChangeCallbacks_.erase(iter);
            continue;
        }
        (*callback)(displayId, hasSystemKeyboard);
        iter++;
    }
}
} // namespace OHOS::Rosen
