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
#include "session/host/include/scene_session.h"
#include "window_manager_hilog.h"
#include "wm_common_inner.h"
#include "wm_math.h"

namespace OHOS::Rosen {

namespace {
// moving
constexpr int32_t MOVING_RECORDS_SIZE_LIMIT = 5;
constexpr int32_t MOVING_RECORDS_TIME_LIMIT = 100;

// arrange rule
constexpr int32_t MIN_DECOR_HEIGHT = 37;
constexpr int32_t MAX_DECOR_HEIGHT = 112;
} // namespace

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