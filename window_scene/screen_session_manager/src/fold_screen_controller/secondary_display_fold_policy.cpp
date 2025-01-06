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
#include "session/screen/include/screen_session.h"
#include "screen_session_manager.h"
#include "screen_scene_config.h"

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
    TLOGI(WmsLogTag::DMS, "SecondaryDisplayFoldPolicy created");
    ScreenId screenIdFull = 0;
    int32_t foldCreaseRegionABPosX = 0;
    int32_t foldCreaseRegionABPosY = 1008;
    int32_t foldCreaseRegionABPosWidth = 2232;
    int32_t foldCreaseRegionABPosHeight = 128;
    int32_t foldCreaseRegionBCPosX = 0;
    int32_t foldCreaseRegionBCPosY = 2224;
    int32_t foldCreaseRegionBCPosWidth = 2232;
    int32_t foldCreaseRegionBCPosHeight = 128;
    std::vector<DMRect> rect = {
        {
            foldCreaseRegionABPosX, foldCreaseRegionABPosY,
            foldCreaseRegionABPosWidth, foldCreaseRegionABPosHeight
        },
        {
            foldCreaseRegionBCPosX, foldCreaseRegionBCPosY,
            foldCreaseRegionBCPosWidth, foldCreaseRegionBCPosHeight
        }
    };
    currentFoldCreaseRegion_ = new FoldCreaseRegion(screenIdFull, rect);
    InitScreenParams();
}

void SecondaryDisplayFoldPolicy::ChangeScreenDisplayMode(FoldDisplayMode displayMode, DisplayModeChangeReason reason)
{
    sptr<ScreenSession> screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_FULL);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "default screenSession is null");
        return;
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
        lastDisplayMode_ = displayMode;
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
    {
        std::lock_guard<std::recursive_mutex> lock_mode(displayModeMutex_);
        if (currentDisplayMode_ == displayMode) {
            TLOGW(WmsLogTag::DMS, "already in displayMode %{public}d", displayMode);
            return;
        }
    }
    SendPropertyChangeResult(screenSession, SCREEN_ID_FULL, ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND,
        displayMode);
    if (currentDisplayMode_ != displayMode) {
        if((currentDisplayMode_ == FoldDisplayMode::GLOBAL_FULL && displayMode == FoldDisplayMode::FULL)
            || (currentDisplayMode_ == FoldDisplayMode::FULL && displayMode == FoldDisplayMode::MAIN)
            || (currentDisplayMode_ == FoldDisplayMode::GLOBAL_FULL && displayMode == FoldDisplayMode::MAIN)) {
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
    } else if (displayMode == FoldDisplayMode::MAIN) {
        reason = ScreenPropertyChangeReason::FOLD_SCREEN_FOLDING;
        SetStatusMainActiveRectAndTpFeature(screenProperty_);
    } else if (displayMode == FoldDisplayMode::GLOBAL_FULL) {
        SetStatusGlobalFullActiveRectAndTpFeature(screenProperty_);
    } else {
        TLOGW(WmsLogTag::DMS, "unKnown displayMode");
    }
    screenSession->UpdatePropertyByFoldControl(screenProperty_, displayMode);
    screenSession->PropertyChange(screenSession->GetScreenProperty(), reason);
    if (displayMode == FoldDisplayMode::MAIN) {
        screenSession->SetRotationAndScreenRotationOnly(Rotation::ROTATION_0);
    }
    TLOGI(WmsLogTag::DMS, "screenBounds : width_= %{public}f, height_= %{public}f",
          screenSession->GetScreenProperty().GetBounds().rect_.width_,
          screenSession->GetScreenProperty().GetBounds().rect_.height_);
    ScreenSessionManager::GetInstance().NotifyDisplayChanged(screenSession->ConvertToDisplayInfo(),
        DisplayChangeEvent::DISPLAY_SIZE_CHANGED);
}

void SecondaryDisplayFoldPolicy::SetStatusFullActiveRectAndTpFeature(ScreenProperty &screenProperty)
{
    auto fullStatusScreenBounds = RRect({0, screenParams_[FULL_STATUS_OFFSET_X],
        screenParams_[FULL_STATUS_WIDTH], screenParams_[SCREEN_HEIGHT]}, 0.0f, 0.0f);
        screenProperty.SetBounds(fullStatusScreenBounds);
    OHOS::Rect rectCur{
        .x = 0,
        .y = screenParams_[FULL_STATUS_OFFSET_X],
        .w = screenParams_[SCREEN_HEIGHT],
        .h = screenParams_[FULL_STATUS_WIDTH],
    };
    RSInterfaces::GetInstance().SetScreenActiveRect(0, rectCur);
#ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, STATUS_FULL, TpFeatureConfigType::AFT_TP_FEATURE);
#endif
}

void SecondaryDisplayFoldPolicy::SetStatusMainActiveRectAndTpFeature(ScreenProperty &screenProperty)
{
    auto mianStatusScreenBounds =
        RRect({0, 0, screenParams_[MAIN_STATUS_WIDTH], screenParams_[SCREEN_HEIGHT]}, 0.0f, 0.0f);
    screenProperty.SetBounds(mianStatusScreenBounds);
    OHOS::Rect rectCur{
        .x = 0,
        .y = 0,
        .w = screenParams_[SCREEN_HEIGHT],
        .h = screenParams_[MAIN_STATUS_WIDTH],
    };
    RSInterfaces::GetInstance().SetScreenActiveRect(0, rectCur);
#ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, STATUS_MAIN, TpFeatureConfigType::AFT_TP_FEATURE);
#endif
}

void SecondaryDisplayFoldPolicy::SetStatusGlobalFullActiveRectAndTpFeature(ScreenProperty &screenProperty)
{
    auto globalFullStatusScreenBounds =
        RRect({0, 0, screenParams_[GLOBAL_FULL_STATUS_WIDTH], screenParams_[SCREEN_HEIGHT]}, 0.0f, 0.0f);
    screenProperty.SetBounds(globalFullStatusScreenBounds);
    OHOS::Rect rectCur{
        .x = 0,
        .y = 0,
        .w = screenParams_[SCREEN_HEIGHT],
        .h = screenParams_[GLOBAL_FULL_STATUS_WIDTH],
    };
    RSInterfaces::GetInstance().SetScreenActiveRect(0, rectCur);
#ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, STATUS_GLOBAL_FULL, TpFeatureConfigType::AFT_TP_FEATURE);
#endif
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
} // namespace OHOS::Rosen