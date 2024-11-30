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

#include "window_manager_hilog.h"

#ifdef POWER_MANAGER_ENABLE
#include <power_mgr_client.h>
#endif

namespace OHOS::Rosen {
namespace {
const ScreenId SCREEN_ID_FULL = 0;
const int32_t FULL_STATUS_WIDTH = 2048;
const int32_t GLOBAL_FULL_STATUS_WIDTH = 3184;
const int32_t MAIN_STATUS_WIDTH = 1008;
const int32_t FULL_STATUS_OFFSET_X = 1136;
const int32_t SCREEN_HEIGHT = 2232;
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
}

void SecondaryDisplayFoldPolicy::ChangeScreenDisplayMode(FoldDisplayMode displayMode, DisplayModeChangeReason reason)
{
    sptr<ScreenSession> screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_FULL);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "default screenSession is null");
        return;
    }
    switch (displayMode) {
        case FoldDisplayMode::MAIN:
        case FoldDisplayMode::FULL:
        case FoldDisplayMode::GLOBAL_FULL: {
            ChangeSuperScreenDisplayMode(screenSession, displayMode);
            break;
        }
        case FoldDisplayMode::UNKNOWN: {
            TLOGW(WmsLogTag::DMS, "displayMode is unknown");
            break;
        }
        default: {
            TLOGW(WmsLogTag::DMS, "displayMode is invalid");
            break;
        }
    }
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
    if (currentDisplayMode_ != displayMode) {
        ChangeScreenDisplayMode(displayMode);
    }
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
        case FoldStatus::FOLD_STATE_HALF_FOLDEDE_WITH_SECOND_HALF_FOLDED: {
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
    if (onBootAnimation_) {
        return;
    }
    {
        std::lock_guard<std::recursive_mutex> lock_mode(displayModeMutex_);
        if (currentDisplayMode_ == displayMode) {
            TLOGW(WmsLogTag::DMS, "already in displayMode %{public}d", displayMode);
            return;
        }
    }
    SendPropertyChangeResult(screenSession, SCREEN_ID_FULL, ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND,
        displayMode);
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
        SetStatusMainActiveRectAndTpFeature(screenProperty_);
    } else if (displayMode == FoldDisplayMode::GLOBAL_FULL) {
        SetStatusGlobalFullActiveRectAndTpFeature(screenProperty_);
    } else {
        TLOGW(WmsLogTag::DMS, "unKnown displayMode");
    }
    screenSession->UpdatePropertyByFoldControl(screenProperty_);
    screenSession->PropertyChange(screenSession->GetScreenProperty(), reason);
    screenSession->SetRotationAndScreenRotationOnly(Rotation::ROTATION_0);
    TLOGI(WmsLogTag::DMS, "screenBounds : width_= %{public}f, height_= %{public}f",
          screenSession->GetScreenProperty().GetBounds().rect_.width_,
          screenSession->GetScreenProperty().GetBounds().rect_.height_);
    ScreenSessionManager::GetInstance().NotifyDisplayChanged(screenSession->ConvertToDisplayInfo(),
        DisplayChangeEvent::DISPLAY_SIZE_CHANGED);
}

void SecondaryDisplayFoldPolicy::SetStatusFullActiveRectAndTpFeature(ScreenProperty &screenProperty_)
{
    auto fullStatusScreenBounds = RRect({0, FULL_STATUS_OFFSET_X, FULL_STATUS_WIDTH, SCREEN_HEIGHT}, 0.0f, 0.0f);
    screenProperty_.SetBounds(fullStatusScreenBounds);
    OHOS::Rect rectCur{
        .x = 0,
        .y = FULL_STATUS_OFFSET_X,
        .w = SCREEN_HEIGHT,
        .h = FULL_STATUS_WIDTH,
    };
    RSInterfaces::GetInstance().SetScreenActiveRect(0, rectCur);
#ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, STATUS_FULL, TpFeatureConfigType::AFT_TP_FEATURE);
#endif
}

void SecondaryDisplayFoldPolicy::SetStatusMainActiveRectAndTpFeature(ScreenProperty &screenProperty_)
{
    auto mianStatusScreenBounds = RRect({0, 0, MAIN_STATUS_WIDTH, SCREEN_HEIGHT}, 0.0f, 0.0f);
    screenProperty_.SetBounds(mianStatusScreenBounds);
    OHOS::Rect rectCur{
        .x = 0,
        .y = 0,
        .w = SCREEN_HEIGHT,
        .h = MAIN_STATUS_WIDTH,
    };
    RSInterfaces::GetInstance().SetScreenActiveRect(0, rectCur);
#ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, STATUS_MAIN, TpFeatureConfigType::AFT_TP_FEATURE);
#endif
}

void SecondaryDisplayFoldPolicy::SetStatusGlobalFullActiveRectAndTpFeature(ScreenProperty &screenProperty_)
{
    auto globalFullStatusScreenBounds = RRect({0, 0, GLOBAL_FULL_STATUS_WIDTH, SCREEN_HEIGHT}, 0.0f, 0.0f);
    screenProperty_.SetBounds(globalFullStatusScreenBounds);
    OHOS::Rect rectCur{
        .x = 0,
        .y = 0,
        .w = SCREEN_HEIGHT,
        .h = GLOBAL_FULL_STATUS_WIDTH,
    };
    RSInterfaces::GetInstance().SetScreenActiveRect(0, rectCur);
#ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, STATUS_GLOBAL_FULL, TpFeatureConfigType::AFT_TP_FEATURE);
#endif
}
} // namespace OHOS::Rosen