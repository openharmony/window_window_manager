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
#include "fold_screen_controller/secondary_display_fold_policy.h"
#include <hisysevent.h>
#include <hitrace_meter.h>
#include <transaction/rs_interfaces.h>
#include <parameters.h>
#include "session/screen/include/screen_session.h"
#include "screen_session_manager.h"
#include "screen_scene_config.h"
#include "sensor_agent.h"
#include "sensor_agent_type.h"

#include "window_manager_hilog.h"

#ifdef POWER_MANAGER_ENABLE
#include <power_mgr_client.h>
#endif

namespace OHOS::Rosen {
namespace {
const ScreenId SCREEN_ID_FULL = 0;
const int32_t MAIN_STATUS_WIDTH = 0;
const int32_t FULL_STATUS_WIDTH = 1;
const int32_t GLOBAL_FULL_STATUS_WIDTH = 2;
const int32_t SCREEN_HEIGHT = 3;
const int32_t FULL_STATUS_OFFSET_X = 4;
constexpr uint32_t HALF_DIVIDER = 2;
#ifdef TP_FEATURE_ENABLE
const int32_t TP_TYPE = 12;
const char* STATUS_MAIN = "version:3+main";
const char* STATUS_GLOBAL_FULL = "version:3+whole";
const char* STATUS_FULL = "version:3+minor";
#endif
} // namespace

SecondaryDisplayFoldPolicy::SecondaryDisplayFoldPolicy(std::recursive_mutex& displayInfoMutex,
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler)
    : displayInfoMutex_(displayInfoMutex), screenPowerTaskScheduler_(screenPowerTaskScheduler)
{
    InitScreenParams();
    std::vector<int32_t> screenParamsSigned;
    for (uint32_t ele : screenParams_) {
        screenParamsSigned.emplace_back(static_cast<int32_t>(ele));
    }
    ScreenId screenIdFull = 0;
    int32_t foldCreaseRegionABPosX = 0;
    int32_t foldCreaseRegionABPosY = screenParamsSigned[MAIN_STATUS_WIDTH]; // 1008
    int32_t foldCreaseRegionABPosHeight = screenParamsSigned[GLOBAL_FULL_STATUS_WIDTH] -
        screenParamsSigned[FULL_STATUS_WIDTH] - screenParamsSigned[MAIN_STATUS_WIDTH]; // 128
    int32_t foldCreaseRegionABPosWidth = screenParamsSigned[SCREEN_HEIGHT]; // 2232
    int32_t foldCreaseRegionBCPosX = 0;
    // 2224
    int32_t foldCreaseRegionBCPosY =
        screenParamsSigned[GLOBAL_FULL_STATUS_WIDTH] -
        ((screenParamsSigned[FULL_STATUS_WIDTH] / HALF_DIVIDER) - (foldCreaseRegionABPosHeight / HALF_DIVIDER));
    int32_t foldCreaseRegionBCPosWidth = screenParamsSigned[SCREEN_HEIGHT]; // 2232
    int32_t foldCreaseRegionBCPosHeight = foldCreaseRegionABPosHeight; // 128
    TLOGW(WmsLogTag::DMS, "created, screenIdFull = %{public}" PRIu64", foldCreaseRegionABPosX = %{public}d,\
        foldCreaseRegionABPosY = %{public}d, foldCreaseRegionABPosHeight = %{public}d,\
        foldCreaseRegionABPosWidth = %{public}d, foldCreaseRegionBCPosX = %{public}d,\
        foldCreaseRegionBCPosY = %{public}d, foldCreaseRegionBCPosWidth = %{public}d,\
        foldCreaseRegionBCPosHeight = %{public}d", screenIdFull, foldCreaseRegionABPosX, foldCreaseRegionABPosY,
        foldCreaseRegionABPosHeight, foldCreaseRegionABPosWidth, foldCreaseRegionBCPosX, foldCreaseRegionBCPosY,
        foldCreaseRegionBCPosWidth, foldCreaseRegionBCPosHeight);
    std::vector<DMRect> rect = {
        {
            foldCreaseRegionABPosX, foldCreaseRegionABPosY,
            static_cast<uint32_t>(foldCreaseRegionABPosWidth), static_cast<uint32_t>(foldCreaseRegionABPosHeight)
        },
        {
            foldCreaseRegionBCPosX, foldCreaseRegionBCPosY,
            static_cast<uint32_t>(foldCreaseRegionBCPosWidth), static_cast<uint32_t>(foldCreaseRegionBCPosHeight)
        }
    };
    currentFoldCreaseRegion_ = new FoldCreaseRegion(screenIdFull, rect);
}

void SecondaryDisplayFoldPolicy::ChangeScreenDisplayMode(FoldDisplayMode displayMode, DisplayModeChangeReason reason)
{
    SetLastCacheDisplayMode(displayMode);
    if (GetModeChangeRunningStatus()) {
        TLOGW(WmsLogTag::DMS, "last process not complete, skip mode: %{public}d", displayMode);
        return;
    }
    sptr<ScreenSession> screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_FULL);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "default screenSession is null");
        return;
    }
    {
        std::lock_guard<std::recursive_mutex> lock_mode(displayModeMutex_);
        lastDisplayMode_ = displayMode;
    }
    if (displayMode == FoldDisplayMode::UNKNOWN) {
        TLOGW(WmsLogTag::DMS, "displayMode is unknown");
    } else {
        ChangeSuperScreenDisplayMode(screenSession, displayMode);
    }
    ReportFoldDisplayModeChange(displayMode);
    {
        std::lock_guard<std::recursive_mutex> lock_mode(displayModeMutex_);
        currentDisplayMode_ = displayMode;
    }
    if (displayMode == FoldDisplayMode::GLOBAL_FULL) {
        TLOGW(WmsLogTag::DMS, "Set device status to STATUS_GLOBAL_FULL");
        SetDeviceStatus(static_cast<uint32_t>(DMDeviceStatus::STATUS_GLOBAL_FULL));
        system::SetParameter("persist.dms.device.status",
            std::to_string(static_cast<uint32_t>(DMDeviceStatus::STATUS_GLOBAL_FULL)));
    } else {
        TLOGW(WmsLogTag::DMS, "Set device status to UNKNOWN");
        SetDeviceStatus(static_cast<uint32_t>(DMDeviceStatus::UNKNOWN));
        system::SetParameter("persist.dms.device.status",
            std::to_string(static_cast<uint32_t>(DMDeviceStatus::UNKNOWN)));
    }
    ScreenSessionManager::GetInstance().NotifyDisplayModeChanged(displayMode);
}

void SecondaryDisplayFoldPolicy::SendSensorResult(FoldStatus foldStatus)
{
    TLOGW(WmsLogTag::DMS, "FoldStatus: %{public}d", foldStatus);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    ChangeScreenDisplayMode(displayMode);
}

void SecondaryDisplayFoldPolicy::SetOnBootAnimation(bool onBootAnimation)
{
    TLOGW(WmsLogTag::DMS, "onBootAnimation: %{public}d", onBootAnimation);
    onBootAnimation_ = onBootAnimation;
    if (!onBootAnimation_) {
        TLOGW(WmsLogTag::DMS, "when boot animation finished, change display mode");
        isChangeScreenWhenBootCompleted = true;
        RecoverWhenBootAnimationExit();
    }
}

void SecondaryDisplayFoldPolicy::RecoverWhenBootAnimationExit()
{
    TLOGW(WmsLogTag::DMS, "CurrentScreen(%{public}" PRIu64 ")", screenId_);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    ChangeScreenDisplayMode(displayMode);
}

FoldDisplayMode SecondaryDisplayFoldPolicy::GetModeMatchStatus()
{
    FoldDisplayMode displayMode = FoldDisplayMode::MAIN;
    switch (currentFoldStatus_) {
        case FoldStatus::EXPAND: {
            displayMode = FoldDisplayMode::FULL;
            break;
        }
        case FoldStatus::FOLDED: {
            displayMode = FoldDisplayMode::MAIN;
            break;
        }
        case FoldStatus::HALF_FOLD: {
            displayMode = FoldDisplayMode::FULL;
            break;
        }
        case FoldStatus::FOLD_STATE_EXPAND_WITH_SECOND_EXPAND: {
            displayMode = FoldDisplayMode::GLOBAL_FULL;
            break;
        }
        case FoldStatus::FOLD_STATE_EXPAND_WITH_SECOND_HALF_FOLDED: {
            displayMode = FoldDisplayMode::GLOBAL_FULL;
            break;
        }
        case FoldStatus::FOLD_STATE_FOLDED_WITH_SECOND_EXPAND: {
            displayMode = FoldDisplayMode::MAIN;
            break;
        }
        case FoldStatus::FOLD_STATE_FOLDED_WITH_SECOND_HALF_FOLDED: {
            displayMode = FoldDisplayMode::MAIN;
            break;
        }
        case FoldStatus::FOLD_STATE_HALF_FOLDED_WITH_SECOND_EXPAND: {
            displayMode = FoldDisplayMode::GLOBAL_FULL;
            break;
        }
        case FoldStatus::FOLD_STATE_HALF_FOLDED_WITH_SECOND_HALF_FOLDED: {
            displayMode = FoldDisplayMode::GLOBAL_FULL;
            break;
        }
        default: {
            TLOGW(WmsLogTag::DMS, "FoldStatus is invalid");
        }
    }
    TLOGW(WmsLogTag::DMS, "displayMode: %{public}d", displayMode);
    return displayMode;
}

void SecondaryDisplayFoldPolicy::ChangeSuperScreenDisplayMode(sptr<ScreenSession> screenSession,
    FoldDisplayMode displayMode)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:ChangeScreenDisplayMode(displayMode = %" PRIu64")", displayMode);
    {
        std::lock_guard<std::recursive_mutex> lock_mode(displayModeMutex_);
        if (currentDisplayMode_ == displayMode && !isChangeScreenWhenBootCompleted) {
            TLOGW(WmsLogTag::DMS, "already in displayMode %{public}d", displayMode);
            return;
        }
        if (isChangeScreenWhenBootCompleted) {
            isChangeScreenWhenBootCompleted = false;
        }
    }
    SetSecondaryDisplayModeChangeStatus(true);
    SendPropertyChangeResult(screenSession, SCREEN_ID_FULL, ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND,
        displayMode);
    if (currentDisplayMode_ != displayMode) {
        if ((currentDisplayMode_ == FoldDisplayMode::GLOBAL_FULL && displayMode == FoldDisplayMode::FULL) ||
            (currentDisplayMode_ == FoldDisplayMode::FULL && displayMode == FoldDisplayMode::MAIN) ||
            (currentDisplayMode_ == FoldDisplayMode::GLOBAL_FULL && displayMode == FoldDisplayMode::MAIN)) {
                return;
        }
    }
    auto taskScreenOnFullOn = [=] {
        screenId_ = SCREEN_ID_FULL;
        PowerMgr::PowerMgrClient::GetInstance().WakeupDeviceAsync();
        SetdisplayModeChangeStatus(false);
    };
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnFullOn, "screenOnFullOnTask");
}

void SecondaryDisplayFoldPolicy::SendPropertyChangeResult(sptr<ScreenSession> screenSession, ScreenId screenId,
    ScreenPropertyChangeReason reason, FoldDisplayMode displayMode)
{
    std::lock_guard<std::recursive_mutex> lock_info(displayInfoMutex_);
    screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(screenId);
    if (displayMode == FoldDisplayMode::FULL) {
        SetStatusFullActiveRectAndTpFeature(screenProperty_);
        screenSession->SetScreenAreaOffsetY(screenParams_[FULL_STATUS_OFFSET_X]);
        screenSession->SetScreenAreaHeight(screenParams_[FULL_STATUS_WIDTH]);
    } else if (displayMode == FoldDisplayMode::MAIN) {
        reason = ScreenPropertyChangeReason::FOLD_SCREEN_FOLDING;
        SetStatusMainActiveRectAndTpFeature(screenProperty_);
        screenSession->SetScreenAreaOffsetY(0);
        screenSession->SetScreenAreaHeight(screenParams_[MAIN_STATUS_WIDTH]);
    } else if (displayMode == FoldDisplayMode::GLOBAL_FULL) {
        SetStatusGlobalFullActiveRectAndTpFeature(screenProperty_);
        screenSession->SetScreenAreaOffsetY(0);
        screenSession->SetScreenAreaHeight(screenParams_[GLOBAL_FULL_STATUS_WIDTH]);
    } else {
        TLOGW(WmsLogTag::DMS, "unKnown displayMode");
    }
    if (displayMode == FoldDisplayMode::MAIN) {
        reason = ScreenPropertyChangeReason::FOLD_SCREEN_FOLDING;
    }
    auto taskScreenOnFullOn = [this, displayMode] {
        if (displayMode == FoldDisplayMode::FULL) {
            SetStatusFullActiveRectAndTpFeature();
        } else if (displayMode == FoldDisplayMode::MAIN) {
            SetStatusMainActiveRectAndTpFeature();
        } else if (displayMode == FoldDisplayMode::GLOBAL_FULL) {
            SetStatusGlobalFullActiveRectAndTpFeature();
        } else {
            TLOGW(WmsLogTag::DMS, "unKnown displayMode");
        }
    };
    screenPowerTaskScheduler_->PostAsyncTask(std::move(taskScreenOnFullOn), __func__);
    {
        std::lock_guard<std::recursive_mutex> lock_info(displayInfoMutex_);
        screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(screenId);
        if (displayMode == FoldDisplayMode::FULL) {
            auto fullStatusScreenBounds = RRect({ 0, screenParams_[FULL_STATUS_OFFSET_X],
                screenParams_[FULL_STATUS_WIDTH], screenParams_[SCREEN_HEIGHT] }, 0.0f, 0.0f);
            screenProperty_.SetBounds(fullStatusScreenBounds);
        } else if (displayMode == FoldDisplayMode::MAIN) {
            auto mainStatusScreenBounds =
                RRect({ 0, 0, screenParams_[MAIN_STATUS_WIDTH], screenParams_[SCREEN_HEIGHT] }, 0.0f, 0.0f);
            screenProperty_.SetBounds(mainStatusScreenBounds);
        } else if (displayMode == FoldDisplayMode::GLOBAL_FULL) {
            auto globalFullStatusScreenBounds =
                RRect({ 0, 0, screenParams_[GLOBAL_FULL_STATUS_WIDTH], screenParams_[SCREEN_HEIGHT] }, 0.0f, 0.0f);
            screenProperty_.SetBounds(globalFullStatusScreenBounds);
        } else {
            TLOGW(WmsLogTag::DMS, "unKnown displayMode");
        }
        screenSession->UpdatePropertyByFoldControl(screenProperty_, displayMode);
        screenSession->PropertyChange(screenSession->GetScreenProperty(), reason);
        if (displayMode == FoldDisplayMode::MAIN) {
            screenSession->SetRotationAndScreenRotationOnly(Rotation::ROTATION_0);
        }
        TLOGI(WmsLogTag::DMS, "screenBounds : width= %{public}f, height= %{public}f",
            screenSession->GetScreenProperty().GetBounds().rect_.width_,
            screenSession->GetScreenProperty().GetBounds().rect_.height_);
    }
}

void SecondaryDisplayFoldPolicy::SetStatusFullActiveRectAndTpFeature()
{
    OHOS::Rect rectCur{
        .x = 0,
        .y = screenParams_[FULL_STATUS_OFFSET_X],
        .w = screenParams_[SCREEN_HEIGHT],
        .h = screenParams_[FULL_STATUS_WIDTH],
    };
    if (!onBootAnimation_) {
        RSInterfaces::GetInstance().NotifyScreenSwitched();
        auto response = RSInterfaces::GetInstance().SetScreenActiveRect(0, rectCur);
        TLOGI(WmsLogTag::DMS, "rs response is %{public}ld", static_cast<long>(response));
    }
#ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, STATUS_FULL, TpFeatureConfigType::AFT_TP_FEATURE);
#endif
    SetSecondaryDisplayModeChangeStatus(false);
}

void SecondaryDisplayFoldPolicy::SetStatusMainActiveRectAndTpFeature()
{
    OHOS::Rect rectCur{
        .x = 0,
        .y = 0,
        .w = screenParams_[SCREEN_HEIGHT],
        .h = screenParams_[MAIN_STATUS_WIDTH],
    };
    if (!onBootAnimation_) {
        RSInterfaces::GetInstance().NotifyScreenSwitched();
        auto response = RSInterfaces::GetInstance().SetScreenActiveRect(0, rectCur);
        TLOGI(WmsLogTag::DMS, "rs response is %{public}ld", static_cast<long>(response));
    }
#ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, STATUS_MAIN, TpFeatureConfigType::AFT_TP_FEATURE);
#endif
    SetSecondaryDisplayModeChangeStatus(false);
}

void SecondaryDisplayFoldPolicy::SetStatusGlobalFullActiveRectAndTpFeature()
{
    OHOS::Rect rectCur{
        .x = 0,
        .y = 0,
        .w = screenParams_[SCREEN_HEIGHT],
        .h = screenParams_[GLOBAL_FULL_STATUS_WIDTH],
    };
    if (!onBootAnimation_) {
        RSInterfaces::GetInstance().NotifyScreenSwitched();
        auto response = RSInterfaces::GetInstance().SetScreenActiveRect(0, rectCur);
        TLOGI(WmsLogTag::DMS, "rs response is %{public}ld", static_cast<long>(response));
    }
#ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, STATUS_GLOBAL_FULL, TpFeatureConfigType::AFT_TP_FEATURE);
#endif
    SetSecondaryDisplayModeChangeStatus(false);
}

void SecondaryDisplayFoldPolicy::ReportFoldDisplayModeChange(FoldDisplayMode displayMode)
{
    int32_t mode = static_cast<int32_t>(displayMode);
    TLOGW(WmsLogTag::DMS, "displayMode: %{public}d", mode);
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
        "DISPLAY_MODE",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "FOLD_DISPLAY_MODE", mode);
    if (ret != 0) {
        TLOGE(WmsLogTag::DMS, "write HiSysEvent error, ret: %{public}d", ret);
    }
}

sptr<FoldCreaseRegion> SecondaryDisplayFoldPolicy::GetCurrentFoldCreaseRegion()
{
    return currentFoldCreaseRegion_;
}

void SecondaryDisplayFoldPolicy::InitScreenParams()
{
    std::vector<DisplayPhysicalResolution> resolutions = ScreenSceneConfig::GetAllDisplayPhysicalConfig();
    for (auto &resolution : resolutions) {
        if (FoldDisplayMode::MAIN == resolution.foldDisplayMode_) {
            screenParams_.push_back(resolution.physicalWidth_);
        } else if (FoldDisplayMode::FULL == resolution.foldDisplayMode_) {
            screenParams_.push_back(resolution.physicalWidth_);
        } else if (FoldDisplayMode::GLOBAL_FULL == resolution.foldDisplayMode_) {
            screenParams_.push_back(resolution.physicalWidth_);
            screenParams_.push_back(resolution.physicalHeight_);
        } else {
            TLOGW(WmsLogTag::DMS, "unKnown displayMode");
        }
    }
    screenParams_.push_back(screenParams_[GLOBAL_FULL_STATUS_WIDTH] - screenParams_[FULL_STATUS_WIDTH]);
    TLOGI(WmsLogTag::DMS,
        "PhysicalResolution : mainStatusWidth_= %{public}d, fullStatusWidth_= %{public}d, gloablFullStatusWidth_="
        "%{public}d, screenHeight_= %{public}d, fullStatusOffsetX_= %{public}d",
          screenParams_[MAIN_STATUS_WIDTH], screenParams_[FULL_STATUS_WIDTH],
          screenParams_[GLOBAL_FULL_STATUS_WIDTH], screenParams_[SCREEN_HEIGHT],
          screenParams_[FULL_STATUS_OFFSET_X]);
}

std::vector<uint32_t> SecondaryDisplayFoldPolicy::GetScreenParams()
{
    return screenParams_;
}

Drawing::Rect SecondaryDisplayFoldPolicy::GetScreenSnapshotRect()
{
    Drawing::Rect snapshotRect = {0, 0, 0, 0};
    if (currentDisplayMode_ == FoldDisplayMode::MAIN) {
        snapshotRect.left_ = 0;
        snapshotRect.top_ = 0;
        snapshotRect.right_ = screenParams_[SCREEN_HEIGHT];
        snapshotRect.bottom_ = screenParams_[MAIN_STATUS_WIDTH];
    } else if (currentDisplayMode_ == FoldDisplayMode::FULL) {
        snapshotRect.left_ = 0;
        snapshotRect.top_ = screenParams_[FULL_STATUS_OFFSET_X];
        snapshotRect.right_ = screenParams_[SCREEN_HEIGHT];
        snapshotRect.bottom_ = screenParams_[GLOBAL_FULL_STATUS_WIDTH];
    } else if (currentDisplayMode_ == FoldDisplayMode::GLOBAL_FULL) {
        snapshotRect.left_ = 0;
        snapshotRect.top_ = 0;
        snapshotRect.right_ = screenParams_[SCREEN_HEIGHT];
        snapshotRect.bottom_ = screenParams_[GLOBAL_FULL_STATUS_WIDTH];
    }
    return snapshotRect;
}

void SecondaryDisplayFoldPolicy::SetMainScreenRegion(DMRect& mainScreenRegion)
{
    if (currentDisplayMode_ == FoldDisplayMode::MAIN) {
        mainScreenRegion.posX_ = 0;
        mainScreenRegion.posY_ = 0;
        mainScreenRegion.width_ = screenParams_[SCREEN_HEIGHT];
        mainScreenRegion.height_ = screenParams_[MAIN_STATUS_WIDTH];
    } else if (currentDisplayMode_ == FoldDisplayMode::FULL) {
        mainScreenRegion.posX_ = 0;
        mainScreenRegion.posY_ = static_cast<int32_t>(screenParams_[FULL_STATUS_OFFSET_X]);
        mainScreenRegion.width_ = screenParams_[SCREEN_HEIGHT];
        mainScreenRegion.height_ = screenParams_[FULL_STATUS_WIDTH];
    } else if (currentDisplayMode_ == FoldDisplayMode::GLOBAL_FULL) {
        mainScreenRegion.posX_ = 0;
        mainScreenRegion.posY_ = 0;
        mainScreenRegion.width_ = screenParams_[SCREEN_HEIGHT];
        mainScreenRegion.height_ = screenParams_[GLOBAL_FULL_STATUS_WIDTH];
    }
}

void SecondaryDisplayFoldPolicy::SetSecondaryDisplayModeChangeStatus(bool status)
{
    if (status) {
        secondaryPengdingTask_ = SECONDARY_FOLD_TO_EXPAND_TASK_NUM;
        startTimePoint_ = std::chrono::steady_clock::now();
        displayModeChangeRunning_ = status;
    } else {
        secondaryPengdingTask_ --;
        if (secondaryPengdingTask_ > 0) {
            return;
        }
        displayModeChangeRunning_ = false;
        endTimePoint_ = std::chrono::steady_clock::now();
        if (lastCachedisplayMode_.load() != GetScreenDisplayMode()) {
            ScreenSessionManager::GetInstance().TriggerDisplayModeUpdate(lastCachedisplayMode_.load());
        }
    }
}

} // namespace OHOS::Rosen