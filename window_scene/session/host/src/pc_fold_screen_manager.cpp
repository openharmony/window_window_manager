/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
constexpr float TAN_25_DEG = 0.4663; // throw slip angle = 25 deg
constexpr float VEL_B_THRESHOLD = 1.732; // 1732 dp/s
constexpr float VEL_C_THRESHOLD = 1.345; // 1345 dp/s
constexpr float THROW_BACKTRACING_DURATION = 100.0f;
constexpr int32_t THROW_BACKTRACING_THRESHOLD = 200;
constexpr float THROW_SLIP_TIME = 416.0f;
constexpr float THROW_SLIP_DAMPING_RATIO = 0.9934f; // stiffness = 228, damping = 30
constexpr float THROW_SLIP_DECELERATION_RATE = 0.002;
const RSAnimationTimingProtocol THROW_SLIP_TIMING_PROTOCOL(std::round(THROW_SLIP_TIME)); // animation time
const RSAnimationTimingCurve THROW_SLIP_CURVE =
    RSAnimationTimingCurve::CreateSpring(THROW_SLIP_TIME / 1000.0f, THROW_SLIP_DAMPING_RATIO, 0.0f);

// arrange rule
constexpr int32_t RULE_TRANS_X = 48; // dp
constexpr int32_t MIN_DECOR_HEIGHT = 37;
constexpr WSRect RECT_ZERO = { 0, 0, 0, 0 };
} // namespace

WM_IMPLEMENT_SINGLE_INSTANCE(PcFoldScreenManager);

void PcFoldScreenManager::UpdateFoldScreenStatus(DisplayId displayId, SuperFoldStatus status,
    const WSRect& defaultDisplayRect, const WSRect& virtualDisplayRect, const WSRect& foldCreaseRect)
{
    SetDisplayInfo(displayId, status);
    SetDisplayRects(defaultDisplayRect, virtualDisplayRect, foldCreaseRect);
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
    ExecuteFoldScreenStatusChangeCallbacks(displayId_, screenFoldStatus_, prevScreenFoldStatus_);
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
    TLOGI(WmsLogTag::WMS_LAYOUT_PC, "%{public}s, %{public}s, %{public}s",
        defaultDisplayRect.ToString().c_str(), virtualDisplayRect.ToString().c_str(),
        foldCreaseRect.ToString().c_str());
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

bool PcFoldScreenManager::IsHalfFolded(DisplayId displayId) const
{
    std::shared_lock<std::shared_mutex> lock(displayInfoMutex_);
    return screenFoldStatus_ == SuperFoldStatus::HALF_FOLDED && displayId_ == displayId;
}

void PcFoldScreenManager::UpdateSystemKeyboardStatus(bool hasSystemKeyboard)
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "status: %{public}d", hasSystemKeyboard);
    std::unique_lock<std::shared_mutex> lock(displayInfoMutex_);
    hasSystemKeyboard_ = hasSystemKeyboard;
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
    return midPosY <= (defaultDisplayRect.posY_ + defaultDisplayRect.height_) ?
        ScreenSide::FOLD_B : ScreenSide::FOLD_C;
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
        TLOGD(WmsLogTag::WMS_LAYOUT, "invalid side: %{public}d", static_cast<int32_t>(side));
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
    TLOGD(WmsLogTag::WMS_LAYOUT, "side: %{public}d, rect: %{public}s",
        static_cast<int32_t>(side), rect.ToString().c_str());
    if (side != ScreenSide::FOLD_B && side != ScreenSide::FOLD_C) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "rule not avaliable, side %{public}d", static_cast<int32_t>(side));
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
    if (!CheckVelocityOrientation(backtracingRect, velocity)) {
        return false;
    }

    const ScreenSide startSide = CalculateScreenSide(backtracingRect);
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
    if (startSide == ScreenSide::FOLD_B) {
        if (rect.posY_ > virtualDisplayRect.posY_ + virtualDisplayRect.height_ / 2) { // 2: center
            return false;
        }
        easyThrowRect.posY_ = foldCreaseRect.posY_;
    } else {
        if (rect.posY_ < defaultDisplayRect.posY_ + defaultDisplayRect.height_ / 2) { // 2: center
            return false;
        }
        easyThrowRect.posY_ = foldCreaseRect.posY_ + foldCreaseRect.height_;
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
            aimX = virtualDisplayRect.posX_ + virtualDisplayRect.height_;
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
    if ((midPosY < midScreenY - THROW_BACKTRACING_THRESHOLD && MathHelper::LessNotEqual(velocity.posY_, 0.0f)) ||
        (midPosY > midScreenY + THROW_BACKTRACING_THRESHOLD && MathHelper::GreatNotEqual(velocity.posY_, 0.0f))) {
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
    if (startSide == ScreenSide::FOLD_B) {
        // C side
        rect.posY_ = rect.posY_ + virtualDisplayRect.posY_ - defaultDisplayRect.posY_;
        topLimit = virtualDisplayRect.posY_;
        botLimit = virtualDisplayRect.posY_ + virtualDisplayRect.height_ - rect.height_ - botAvoidHeight;
    } else {
        // B side
        rect.posY_ = rect.posY_ + defaultDisplayRect.posY_ - virtualDisplayRect.posY_;
        topLimit = topAvoidHeight;
        botLimit = foldCreaseRect.posY_ - rect.height_;
    }
    // top limit first
    rect.posY_ = std::max(std::min(rect.posY_, botLimit), topLimit);
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "end posY: %{public}d", rect.posY_);
    return true;
}

void PcFoldScreenManager::MappingRectInScreenSide(ScreenSide side, WSRect& rect,
    int32_t topAvoidHeight, int32_t botAvoidHeight)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "side: %{public}d, rect: %{public}s, avoid heights: [%{public}d,%{public}d]",
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
            TLOGW(WmsLogTag::WMS_LAYOUT, "invalid side: %{public}d", static_cast<int32_t>(side));
            return;
    }
    rect.posX_ = std::max(rect.posX_, topLeftLimit.posX_);
    rect.posY_ = std::max(rect.posY_, topLeftLimit.posY_);
    rect.posX_ = std::min(rect.posX_, botRightLimit.posX_);
    rect.posY_ = std::min(rect.posY_, botRightLimit.posY_);
    rect.width_ = std::min(rect.width_, botRightLimit.width_);
    rect.height_ = std::min(rect.height_, botRightLimit.height_);
    TLOGD(WmsLogTag::WMS_LAYOUT, "limit rects: [%{public}s,%{public}s], mapped rect: %{public}s",
        topLeftLimit.ToString().c_str(), botRightLimit.ToString().c_str(), rect.ToString().c_str());
}

void PcFoldScreenManager::MappingRectInScreenSideWithArrangeRule(ScreenSide side, WSRect& rect,
    int32_t topAvoidHeight, int32_t botAvoidHeight, int32_t titleHeight)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "side: %{public}d, rect: %{public}s",
        static_cast<int32_t>(side), rect.ToString().c_str());
    if (side != ScreenSide::FOLD_B && side != ScreenSide::FOLD_C) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "rule not avaliable, side %{public}d", static_cast<int32_t>(side));
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
            TLOGD(WmsLogTag::WMS_LAYOUT, "init rule, limit: %{public}s, arranged: %{public}s, rect: %{public}s",
                limitRect.ToString().c_str(), lastArrangedRect.ToString().c_str(), rect.ToString().c_str());
            return;
        }

        ApplyArrangeRule(rect, lastArrangedRect, limitRect, titleHeight);
        TLOGD(WmsLogTag::WMS_LAYOUT, "apply rule, limit: %{public}s, arranged: %{public}s, rect: %{public}s",
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
    rect.posX_ = std::max(limitRect.posX_, limitRect.posX_ + (limitRect.width_ - rect.width_) / 2); // 2: center align
    rect.posY_ = std::max(limitRect.posY_, limitRect.posY_ + (limitRect.height_ - rect.height_) / 2); // 2: center align
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
    TLOGI(WmsLogTag::WMS_LAYOUT, "id: %{public}d", persistentId);
    std::unique_lock<std::mutex> lock(callbackMutex_);
    auto [_, result] = foldScreenStatusChangeCallbacks_.insert_or_assign(persistentId, func);
    if (result) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "callback has registered");
    }
}

void PcFoldScreenManager::UnregisterFoldScreenStatusChangeCallback(int32_t persistentId)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "id: %{public}d", persistentId);
    std::unique_lock<std::mutex> lock(callbackMutex_);
    auto iter = foldScreenStatusChangeCallbacks_.find(persistentId);
    if (iter == foldScreenStatusChangeCallbacks_.end()) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "callback not registered");
        return;
    }
    foldScreenStatusChangeCallbacks_.erase(iter);
}

void PcFoldScreenManager::ExecuteFoldScreenStatusChangeCallbacks(DisplayId displayId,
    SuperFoldStatus status, SuperFoldStatus prevStatus)
{
    std::unique_lock<std::mutex> lock(callbackMutex_);
    for (auto iter = foldScreenStatusChangeCallbacks_.begin(); iter != foldScreenStatusChangeCallbacks_.end();) {
        auto callback = iter->second.lock();
        if (callback == nullptr) {
            TLOGW(WmsLogTag::WMS_LAYOUT, "callback invalid, id: %{public}d", iter->first);
            iter = foldScreenStatusChangeCallbacks_.erase(iter);
            continue;
        }
        (*callback)(displayId, status, prevStatus);
        iter++;
    }
}
} // namespace OHOS::Rosen
