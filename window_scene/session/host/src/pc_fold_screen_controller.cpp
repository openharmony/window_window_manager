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
#include <parameters.h>
#include "display_manager.h"
#include "session/host/include/scene_session.h"
#include "window_manager_hilog.h"
#include "wm_common_inner.h"
#include "wm_math.h"

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
    std::unique_lock<std::shared_mutex> lock(rectsMutex_);
    defaultDisplayRect_ = defaultDisplayRect;
    virtualDisplayRect_ = virtualDisplayRect;
    foldCreaseRect_ = foldCreaseRect;
}

bool PcFoldScreenManager::IsHalfFolded(DisplayId displayId)
{
    std::shared_lock<std::shared_mutex> lock(displayInfoMutex_);
    return screenFoldStatus_ == SuperFoldStatus::HALF_FOLDED && displayId_ == displayId;
}

float PcFoldScreenManager::GetVpr()
{
    std::unique_lock<std::shared_mutex> lock(displayInfoMutex_);
    return vpr_;
}

std::tuple<WSRect, WSRect, WSRect> PcFoldScreenManager::GetDisplayRects()
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

bool PcFoldScreenManager::NeedDoThrowSlip(ScreenSide startSide, const WSRectF& velocity)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "side: %{public}d, velocity: %{public}s",
        static_cast<int32_t>(startSide), velocity.ToString().c_str());
    float vpr = GetVpr();
    if (startSide == ScreenSide::FOLD_B && velocity.posY_ > VEL_THRESHOLD * vpr &&
        std::abs(velocity.posX_ / MathHelper::NonZero(velocity.posY_)) < TAN_25_DEG) {
        return true;
    }
    if (startSide == ScreenSide::FOLD_C && velocity.posY_ < -VEL_THRESHOLD * vpr &&
        std::abs(velocity.posX_ / MathHelper::NonZero(velocity.posY_)) < TAN_25_DEG) {
        return true;
    }
    return false;
}

/**
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
    ScreenSide endSide = (startSide == ScreenSide::FOLD_B) ? ScreenSide::FOLD_C : ScreenSide::FOLD_B;
    MappingRectInScreenSideWithArrangeRule(endSide, rect, topAvoidHeight, botAvoidHeight, titleHeight);
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

/**
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

/**
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

PcFoldScreenController::PcFoldScreenController(wptr<SceneSession> weakSession, int32_t persistentId)
    : weakSceneSession_(std::move(weakSession)), persistentId_(persistentId)
{
    onFoldScreenStatusChangeCallback_ = std::make_shared<FoldScreenStatusChangeCallback>(
        [weakThis = wptr(this)](DisplayId displayId, SuperFoldStatus status, SuperFoldStatus prevStatus) {
            auto controller = weakThis.promote();
            if (controller == nullptr) {
                TLOGNE(WmsLogTag::WMS_LAYOUT, "controller is nullptr");
                return;
            }
            if ((prevStatus == SuperFoldStatus::HALF_FOLDED && status == SuperFoldStatus::FOLDED) ||
                (prevStatus == SuperFoldStatus::FOLDED && status == SuperFoldStatus::HALF_FOLDED)) {
                return;
            }
            controller->UpdateFullScreenWaterfallMode(false);
        }
    );
    PcFoldScreenManager::GetInstance().RegisterFoldScreenStatusChangeCallback(GetPersistentId(),
        std::weak_ptr<FoldScreenStatusChangeCallback>(onFoldScreenStatusChangeCallback_));
}

PcFoldScreenController::~PcFoldScreenController()
{
    PcFoldScreenManager::GetInstance().UnregisterFoldScreenStatusChangeCallback(GetPersistentId());
}

bool PcFoldScreenController::IsHalfFolded(DisplayId displayId)
{
    return PcFoldScreenManager::GetInstance().IsHalfFolded(displayId);
}

bool PcFoldScreenController::NeedFollowHandAnimation()
{
    static bool needFollowHandAnimation =
        system::GetParameter("persist.window.throw_slip_follow_animation.enabled", "0") == "1";
    return needFollowHandAnimation;
}

void PcFoldScreenController::RecordStartMoveRect(const WSRect& rect, bool isStartFullScreen)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "rect: %{public}s, isStartFullScreen: %{public}d",
        rect.ToString().c_str(), isStartFullScreen);
    std::unique_lock<std::mutex> lock(moveMutex_);
    startMoveRect_ = rect;
    isStartFullScreen_ = isStartFullScreen;
}

bool PcFoldScreenController::IsStartFullScreen()
{
    std::unique_lock<std::mutex> lock(moveMutex_);
    return isStartFullScreen_;
}

void PcFoldScreenController::RecordMoveRects(const WSRect& rect)
{
    auto time = std::chrono::high_resolution_clock::now();
    std::unique_lock<std::mutex> lock(moveMutex_);
    movingRectRecords_.push_back(std::make_pair(time, rect));
    TLOGD(WmsLogTag::WMS_LAYOUT, "id: %{public}d, rect: %{public}s", GetPersistentId(), rect.ToString().c_str());
    // pop useless record
    while (movingRectRecords_.size() > MOVING_RECORDS_SIZE_LIMIT ||
           TimeHelper::GetDuration(movingRectRecords_[0].first, time) > MOVING_RECORDS_TIME_LIMIT) {
        movingRectRecords_.erase(movingRectRecords_.begin());
    }
    TLOGD(WmsLogTag::WMS_LAYOUT, "records size: %{public}zu, duration: %{public}d", movingRectRecords_.size(),
        TimeHelper::GetDuration(movingRectRecords_[0].first, movingRectRecords_[movingRectRecords_.size() - 1].first));
}

/**
 * if move fast, window can be throwed to other side
 * @param rect: current rect. if throwed, move it to other side
 * @param topAvoidHeight: avoid status bar
 * @param botAvoidHeight: avoid dock
 */
bool PcFoldScreenController::ThrowSlip(DisplayId displayId, WSRect& rect,
    int32_t topAvoidHeight, int32_t botAvoidHeight)
{
    auto& manager = PcFoldScreenManager::GetInstance();
    if (!manager.IsHalfFolded(displayId)) {
        manager.ResetArrangeRule();
        return false;
    }
    {
        std::unique_lock<std::mutex> lock(moveMutex_);
        manager.ResetArrangeRule(startMoveRect_);
    }
    WSRect titleRect = { rect.posX_, rect.posY_, rect.width_, GetTitleHeight() };
    ScreenSide startSide = manager.CalculateScreenSide(titleRect);
    WSRectF velocity = CalculateMovingVelocity();
    if (!manager.NeedDoThrowSlip(startSide, velocity)) {
        manager.ResetArrangeRule(startSide);
        TLOGI(WmsLogTag::WMS_LAYOUT, "no throw rect: %{public}s", rect.ToString().c_str());
        return false;
    }

    manager.ThrowSlipToOppositeSide(startSide, rect, topAvoidHeight, botAvoidHeight, GetTitleHeight());
    manager.ResetArrangeRule(startSide);
    TLOGI(WmsLogTag::WMS_LAYOUT, "throw to rect: %{public}s", rect.ToString().c_str());
    return true;
}

/**
 * resize to fullscreen in one side considering avoid area
 * @param rect: resize in its side
 */
void PcFoldScreenController::ResizeToFullScreen(WSRect& rect, int32_t topAvoidHeight, int32_t botAvoidHeight)
{
    PcFoldScreenManager::GetInstance().ResizeToFullScreen(rect, topAvoidHeight, botAvoidHeight);
}

RSAnimationTimingProtocol PcFoldScreenController::GetMovingTimingProtocol()
{
    return PcFoldScreenManager::GetInstance().GetMovingTimingProtocol();
}

RSAnimationTimingCurve PcFoldScreenController::GetMovingTimingCurve()
{
    return PcFoldScreenManager::GetInstance().GetMovingTimingCurve();
}

RSAnimationTimingProtocol PcFoldScreenController::GetThrowSlipTimingProtocol()
{
    return PcFoldScreenManager::GetInstance().GetThrowSlipTimingProtocol();
}

RSAnimationTimingCurve PcFoldScreenController::GetThrowSlipTimingCurve()
{
    return PcFoldScreenManager::GetInstance().GetThrowSlipTimingCurve();
}

void PcFoldScreenController::UpdateFullScreenWaterfallMode(bool isWaterfallMode)
{
    auto sceneSession = weakSceneSession_.promote();
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "session is nullptr, id: %{public}d", GetPersistentId());
        return;
    }
    sceneSession->PostTask([weakThis = wptr(this), isWaterfallMode] {
        auto controller = weakThis.promote();
        if (controller == nullptr) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "controller is nullptr");
            return;
        }
        if (controller->isFullScreenWaterfallMode_ == isWaterfallMode) {
            return;
        }
        controller->isFullScreenWaterfallMode_ = isWaterfallMode;
        controller->ExecuteFullScreenWaterfallModeChangeCallback();
    }, __func__);
}

void PcFoldScreenController::RegisterFullScreenWaterfallModeChangeCallback(
    std::function<void(bool isWaterfallMode)>&& func)
{
    fullScreenWaterfallModeChangeCallback_ = std::move(func);
    ExecuteFullScreenWaterfallModeChangeCallback();
}

void PcFoldScreenController::UnregisterFullScreenWaterfallModeChangeCallback()
{
    fullScreenWaterfallModeChangeCallback_ = nullptr;
}

void PcFoldScreenController::ExecuteFullScreenWaterfallModeChangeCallback()
{
    if (fullScreenWaterfallModeChangeCallback_ == nullptr) {
        return;
    }
    fullScreenWaterfallModeChangeCallback_(isFullScreenWaterfallMode_);
    // notify client
    auto sceneSession = weakSceneSession_.promote();
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "session is nullptr, id: %{public}d", GetPersistentId());
        return;
    }
    if (sceneSession->sessionStage_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "sessionStage is nullptr, id: %{public}d", GetPersistentId());
        return;
    }
    sceneSession->sessionStage_->SetFullScreenWaterfallMode(isFullScreenWaterfallMode_);
}

int32_t PcFoldScreenController::GetPersistentId() const
{
    return persistentId_;
}

/**
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
    std::unique_lock<std::mutex> lock(moveMutex_);
    int32_t recordsSize = movingRectRecords_.size();
    if (recordsSize <= 1) {
        return velocity;
    }

    if (recordsSize >= 2) { // temp use 2 points
        auto rect0 = movingRectRecords_[0].second;
        auto rect1 = movingRectRecords_[recordsSize - 1].second;
        int32_t duration = TimeHelper::GetDuration(movingRectRecords_[0].first,
            movingRectRecords_[recordsSize - 1].first);
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
} // namespace OHOS::Rosen