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

#include "session/host/include/pc_fold_screen_controller.h"
#include "session/host/include/scene_session.h"
#include "wm_common_inner.h"
#include "window_manager_hilog.h"
#include "wm_math.h"
#include "display_manager.h"

namespace OHOS::Rosen {

namespace {
// moving
constexpr int32_t MOVING_RECORDS_SIZE_LIMIT = 5;
constexpr int32_t MOVING_RECORDS_TIME_LIMIT = 100;
constexpr int32_t MOVING_RESPONSE = 50;
constexpr float MOVING_DAMPING_RATIO = 0.98f;
const RSAnimationTimingProtocol MOVING_TIMING_PROTOCOL(MOVING_RESPONSE); // animation time
const RSAnimationTimingCurve MOVING_CURVE =
    RSAnimationTimingCurve::CreateSpring(static_cast<float>(MOVING_RESPONSE / 1000.0f), MOVING_DAMPING_RATIO, 0.0f);

// throw-slip
constexpr float TAN_25_DEG = 0.4663; // throw slip angle = 25 deg
constexpr float VEL_THRESHOLD = 0.13445; // 134.45 dp/s
constexpr float THROW_SLIP_TIME = 416.0f;
constexpr float THROW_SLIP_DAMPING_RATIO = 0.9934f; // stiffness = 228, damping = 30
const RSAnimationTimingProtocol THROW_SLIP_TIMING_PROTOCOL(std::round(THROW_SLIP_TIME)); // animation time
const RSAnimationTimingCurve THROW_SLIP_CURVE =
    RSAnimationTimingCurve::CreateSpring(THROW_SLIP_TIME / 1000.0f, THROW_SLIP_DAMPING_RATIO, 0.0f);

// arrange rule
constexpr int32_t RULE_TRANS_X = 48; // dp
constexpr int32_t MIN_DECOR_HEIGHT = 37;
constexpr int32_t MAX_DECOR_HEIGHT = 112;
const WSRect RECT_ZERO = { 0, 0, 0, 0 };
} // namespace

std::atomic<DisplayId> PcFoldScreenController::displayId_ = DEFAULT_SCREEN_ID;
std::atomic<float> PcFoldScreenController::vpr_ = 1.5f;
std::atomic<ScreenFoldStatus> PcFoldScreenController::screenFoldStatus_ = ScreenFoldStatus::UNKNOWN;
std::atomic<WSRect> PcFoldScreenController::defaultDisplayRect_ = RECT_ZERO;
std::atomic<WSRect> PcFoldScreenController::virtualDisplayRect_ = RECT_ZERO;
std::atomic<WSRect> PcFoldScreenController::foldCreaseRect_ = RECT_ZERO;
std::atomic<WSRect> PcFoldScreenController::defaultArrangedRect_ = RECT_ZERO;
std::atomic<WSRect> PcFoldScreenController::virtualArrangedRect_ = RECT_ZERO;

PcFoldScreenController::PcFoldScreenController(wptr<SceneSession> weak) : weakSceneSession_(weak) {}

WSError PcFoldScreenController::UpdateFoldScreenStatus(DisplayId displayId, ScreenFoldStatus status)
{
    if (displayId_.load() == displayId && screenFoldStatus_.load() == status) {
        return WSError::WS_DO_NOTHING;
    }
    TLOGI(WmsLogTag::WMS_MAIN, "display: %{public}" PRIu64", fold status: %{public}d",
        displayId, static_cast<int32_t>(status));
    screenFoldStatus_.store(status);
    ResetArrangeRule();
    displayId_.store(displayId);
    auto display = DisplayManager::GetInstance().GetDisplayById(displayId);
    if (display == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Failed to get display");
        return WSError::WS_OK;
    }
    vpr_.store(display->GetVirtualPixelRatio());
    TLOGI(WmsLogTag::WMS_MAIN, "vpr: %{public}f", vpr_.load());
    return WSError::WS_OK;
}

RSAnimationTimingProtocol PcFoldScreenController::GetMovingTimingProtocol()
{
    return MOVING_TIMING_PROTOCOL;
}

RSAnimationTimingCurve PcFoldScreenController::GetMovingTimingCurve()
{
    return MOVING_CURVE;
}

RSAnimationTimingProtocol PcFoldScreenController::GetThrowSlipTimingProtocol()
{
    return THROW_SLIP_TIMING_PROTOCOL;
}

RSAnimationTimingCurve PcFoldScreenController::GetThrowSlipTimingCurve()
{
    return THROW_SLIP_CURVE;
}

void PcFoldScreenController::RecordStartRect(const WSRect& rect, bool isStartFullScreen)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "rect: %{public}s", rect.ToString().c_str());
    startRect_ = rect;
    isStartFullScreen_ = isStartFullScreen;
}

bool PcFoldScreenController::IsStartFullScreen() const
{
    return isStartFullScreen_;
}

void PcFoldScreenController::RecordMoveRects(const WSRect& rect)
{
    auto time = std::chrono::high_resolution_clock::now();
    movingRectRecords_.push_back(std::make_pair(time, rect));
    TLOGD(WmsLogTag::WMS_LAYOUT, "rect: %{public}s", rect.ToString().c_str());
    // pop useless record
    while (movingRectRecords_.size() > MOVING_RECORDS_SIZE_LIMIT ||
        TimeHelper::GetDuration(movingRectRecords_[0].first, time) > MOVING_RECORDS_TIME_LIMIT) {
        movingRectRecords_.erase(movingRectRecords_.begin());
    }
}

bool PcFoldScreenController::ThrowSlip(DisplayId displayId, WSRect& rect,
    int32_t topAvoidHeight, int32_t botAvoidHeight)
{
    if (!IsHalfFolded() || displayId != displayId_.load()) {
        ResetArrangeRule();
        return false;
    }
    ResetArrangeRule(CalculateScreenSide(startRect_));
    WSRect titleRect = { rect.posX_, rect.posY_, rect.width_, GetTitleHeight() };
    ScreenSide startSide = CalculateScreenSide(titleRect);
    WSRectF velocity = CalculateMovingVelocity();
    if (!NeedDoThrowSlip(startSide, velocity)) {
        ResetArrangeRule(startSide);
        TLOGI(WmsLogTag::WMS_LAYOUT, "no throw rect: %{public}s", rect.ToString().c_str());
        return false;
    }

    ThrowSlipToOppositeSide(startSide, rect, topAvoidHeight, botAvoidHeight);
    ResetArrangeRule(startSide);
    TLOGI(WmsLogTag::WMS_LAYOUT, "throw to rect: %{public}s", rect.ToString().c_str());
    return true;
}

int32_t PcFoldScreenController::GetPersistentId() const
{
    auto sceneSession = weakSceneSession_.promote();
    if (sceneSession == nullptr) {
        return INVALID_SESSION_ID;
    }
    return sceneSession->GetPersistentId();
}

/*
 * get height of title bar
 * @return: vp
 */
int32_t PcFoldScreenController::GetTitleHeight() const
{
    auto sceneSession = weakSceneSession_.promote();
    if (sceneSession == nullptr) {
        return MIN_DECOR_HEIGHT;
    }
    return std::clamp(sceneSession->GetCustomDecorHeight(), MIN_DECOR_HEIGHT, MAX_DECOR_HEIGHT);
}

WSRectF PcFoldScreenController::CalculateMovingVelocity()
{
    WSRectF velocity = { 0.0f, 0.0f, 0.0f, 0.0f };
    int32_t records_size = movingRectRecords_.size();
    if (records_size <= 1) {
        return velocity;
    }

    if (records_size >= 2) { // temp use 2 points
        auto rect0 = movingRectRecords_[0].second;
        auto rect1 = movingRectRecords_[records_size - 1].second;
        int32_t duration = TimeHelper::GetDuration(movingRectRecords_[0].first,
            movingRectRecords_[records_size - 1].first);
        if (duration <= 1) {
            return velocity;
        }
        velocity.posX_ = static_cast<float>((rect1.posX_ - rect0.posX_) / static_cast<float>(duration));
        velocity.posY_ = static_cast<float>((rect1.posY_ - rect0.posY_) / static_cast<float>(duration));
        velocity.width_ = static_cast<float>((rect1.width_ - rect0.width_) / static_cast<float>(duration));
        velocity.height_ = static_cast<float>((rect1.height_ - rect0.height_) / static_cast<float>(duration));
        return velocity;
    }
    return velocity;
}

void PcFoldScreenController::RemoveMoveRects()
{
    RectRecordsVector vec;
    movingRectRecords_.swap(vec);
}

ScreenSide PcFoldScreenController::CalculateScreenSide(const WSRect& rect)
{
    int32_t midPosY = std::round(0.5 * rect.height_ + rect.posY_);
    WSRect defaultDisplayRect = PcFoldScreenController::GetDefaultDisplayRect();
    WSRect virtualDisplayRect = PcFoldScreenController::GetVirtualDisplayRect();
    return midPosY <= (defaultDisplayRect.posY_ + defaultDisplayRect.height_) ?
        ScreenSide::FOLD_B : ScreenSide::FOLD_C;
}

bool PcFoldScreenController::NeedDoThrowSlip(ScreenSide startSide, WSRectF velocity)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "side: %{public}d, velocity: %{public}s", startSide, velocity.ToString().c_str());
    if (startSide == ScreenSide::FOLD_B && velocity.posY_ > VEL_THRESHOLD * vpr_.load() &&
        std::abs(velocity.posX_ / MathHelper::NonZero(velocity.posY_)) < TAN_25_DEG) {
        return true;
    }
    if (startSide == ScreenSide::FOLD_C && velocity.posY_ < -VEL_THRESHOLD * vpr_.load() &&
        std::abs(velocity.posX_ / MathHelper::NonZero(velocity.posY_)) < TAN_25_DEG) {
        return true;
    }
    return false;
}

bool PcFoldScreenController::ThrowSlipToOppositeSide(ScreenSide startSide, WSRect& rect,
    int32_t topAvoidHeight, int32_t botAvoidHeight)
{
    if (startSide != ScreenSide::FOLD_B && startSide != ScreenSide::FOLD_C) {
        return false;
    }
    ScreenSide endSide = (startSide == ScreenSide::FOLD_B) ? ScreenSide::FOLD_C : ScreenSide::FOLD_B;
    MappingRectInScreenSideWithArrangeRule(endSide, rect, topAvoidHeight, botAvoidHeight);
    return true;
}

void PcFoldScreenController::MappingRectInScreenSide(ScreenSide side, WSRect& rect,
    int32_t topAvoidHeight, int32_t botAvoidHeight)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "side: %{public}d, rect: %{public}s, avoid heights: [%{public}d,%{public}d]",
        static_cast<int32_t>(side), rect.ToString().c_str(), topAvoidHeight, botAvoidHeight);
    WSRect topLeftLimit = RECT_ZERO;
    WSRect botRightLimit = RECT_ZERO;
    WSRect defaultDisplayRect = PcFoldScreenController::GetDefaultDisplayRect();
    WSRect virtualDisplayRect = PcFoldScreenController::GetVirtualDisplayRect();
    WSRect foldCreaseRect = PcFoldScreenController::GetFoldCreaseRect();
    switch (side) {
        case ScreenSide::FOLD_B:
            topLeftLimit.posX_ = MathHelper::Ceil(RULE_TRANS_X * vpr_.load()) - rect.width_;
            topLeftLimit.posY_ = topAvoidHeight;
            botRightLimit.posX_ = std::max(0,
                MathHelper::Floor(defaultDisplayRect.width_ - RULE_TRANS_X * vpr_.load()));
            botRightLimit.posY_ = std::max(0, foldCreaseRect.posY_ - rect.height_);
            botRightLimit.width_ = defaultDisplayRect.width_;
            botRightLimit.height_ = std::max(0, foldCreaseRect.posY_ - topLeftLimit.posY_);
            break;
        case ScreenSide::FOLD_C:
            topLeftLimit.posX_ = MathHelper::Ceil(RULE_TRANS_X * vpr_.load()) - rect.width_;
            topLeftLimit.posY_ = foldCreaseRect.posY_ + foldCreaseRect.height_;
            botRightLimit.posX_ = std::max(0,
                MathHelper::Floor(virtualDisplayRect.width_ - RULE_TRANS_X * vpr_.load()));
            botRightLimit.posY_ = std::max(foldCreaseRect.posY_ + foldCreaseRect.height_,
                MathHelper::Floor(virtualDisplayRect.posY_ + virtualDisplayRect.height_ -
                                    botAvoidHeight - MIN_DECOR_HEIGHT * vpr_.load()));
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

void PcFoldScreenController::ResizeToFullScreen(WSRect& rect, int32_t topAvoidHeight, int32_t botAvoidHeight)
{
    ScreenSide side = CalculateScreenSide(rect);
    TLOGD(WmsLogTag::WMS_LAYOUT, "side: %{public}d, rect: %{public}s",
        static_cast<int32_t>(side), rect.ToString().c_str());
    if (side != ScreenSide::FOLD_B && side != ScreenSide::FOLD_C) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "rule not avaliable, side %{public}d", static_cast<int32_t>(side));
        return;
    }

    // calculate limit rect
    WSRect defaultDisplayRect = PcFoldScreenController::GetDefaultDisplayRect();
    WSRect virtualDisplayRect = PcFoldScreenController::GetVirtualDisplayRect();
    WSRect foldCreaseRect = PcFoldScreenController::GetFoldCreaseRect();
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

void PcFoldScreenController::MappingRectInScreenSideWithArrangeRule(ScreenSide side, WSRect& rect,
    int32_t topAvoidHeight, int32_t botAvoidHeight)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "side: %{public}d, rect: %{public}s",
        static_cast<int32_t>(side), rect.ToString().c_str());
    if (side != ScreenSide::FOLD_B && side != ScreenSide::FOLD_C) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "rule not avaliable, side %{public}d", static_cast<int32_t>(side));
        return;
    }

    // calculate limit rect
    WSRect defaultDisplayRect = PcFoldScreenController::GetDefaultDisplayRect();
    WSRect virtualDisplayRect = PcFoldScreenController::GetVirtualDisplayRect();
    WSRect foldCreaseRect = PcFoldScreenController::GetFoldCreaseRect();
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

    std::atomic<WSRect>& lastArrangedRect = (side == ScreenSide::FOLD_B) ?
        defaultArrangedRect_ : virtualArrangedRect_;
    if (lastArrangedRect.load().IsEmpty()) {
        ApplyInitArrangeRule(rect, lastArrangedRect, limitRect, GetTitleHeight());
        TLOGD(WmsLogTag::WMS_LAYOUT, "init rule, limit: %{public}s, arranged: %{public}s, rect: %{public}s",
            limitRect.ToString().c_str(), lastArrangedRect.load().ToString().c_str(), rect.ToString().c_str());
        return;
    }

    ApplyArrangeRule(rect, lastArrangedRect, limitRect, GetTitleHeight());
    TLOGD(WmsLogTag::WMS_LAYOUT, "apply rule, limit: %{public}s, arranged: %{public}s, rect: %{public}s",
        limitRect.ToString().c_str(), lastArrangedRect.load().ToString().c_str(), rect.ToString().c_str());
}

/*
 * init rule: move rect to center of display
 * @param titleHeight: in vp
 */
void PcFoldScreenController::ApplyInitArrangeRule(WSRect& rect, std::atomic<WSRect>& lastArrangedRect,
    const WSRect& limitRect, int32_t titleHeight)
{
    rect.posX_ = std::max(limitRect.posX_, limitRect.posX_ + (limitRect.width_ - rect.width_) / 2); // center align
    rect.posY_ = std::max(limitRect.posY_, limitRect.posY_ + (limitRect.height_ - rect.height_) / 2); // center align
    lastArrangedRect.store({ rect.posX_, rect.posY_,
        RULE_TRANS_X * vpr_.load(), titleHeight * vpr_.load()});
}

/*
 * init rule: move rect to bottom-right of last arranged position
 * @param titleHeight: in vp
 */
void PcFoldScreenController::ApplyArrangeRule(WSRect& rect, std::atomic<WSRect>& lastArrangedRect,
    const WSRect& limitRect, int32_t titleHeight)
{
    rect.posX_ = lastArrangedRect.load().posX_ + lastArrangedRect.load().width_;
    rect.posY_ = lastArrangedRect.load().posY_ + lastArrangedRect.load().height_;
    // new column
    if (rect.posY_ + rect.height_ > limitRect.posY_ + limitRect.height_) {
        rect.posY_ = limitRect.posY_;
    }
    // reset to top-left
    if (rect.posX_ + rect.width_ > limitRect.posX_ + limitRect.width_) {
        rect.posX_ = limitRect.posX_;
        rect.posY_ = limitRect.posY_;
    }
    lastArrangedRect.store({ rect.posX_, rect.posY_,
        RULE_TRANS_X * vpr_.load(), titleHeight * vpr_.load()});
}

void PcFoldScreenController::ResetArrangeRule()
{
    defaultArrangedRect_.store(RECT_ZERO);
    virtualArrangedRect_.store(RECT_ZERO);
}

void PcFoldScreenController::ResetArrangeRule(const WSRect& rect)
{
    ResetArrangeRule(CalculateScreenSide(rect));
}

void PcFoldScreenController::ResetArrangeRule(ScreenSide side)
{
    if (side != ScreenSide::FOLD_B && side != ScreenSide::FOLD_C) {
        TLOGD(WmsLogTag::WMS_LAYOUT, "invalid side: %{public}d", static_cast<int32_t>(side));
        return;
    }
    if (side == ScreenSide::FOLD_B) {
        defaultArrangedRect_.store(RECT_ZERO);
    } else { // FOLD_C
        virtualArrangedRect_.store(RECT_ZERO);
    }
}
} // namespace OHOS::Rosen
