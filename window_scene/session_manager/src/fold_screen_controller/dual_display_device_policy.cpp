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

#include <hisysevent.h>
#include <hitrace_meter.h>
#include <transaction/rs_interfaces.h>
#include "fold_screen_controller/dual_display_device_policy.h"
#include "session/screen/include/screen_session.h"
#include "screen_session_manager.h"

#include "window_manager_hilog.h"

#ifdef POWER_MANAGER_ENABLE
#include <power_mgr_client.h>
#endif

namespace OHOS::Rosen {
namespace {
    const ScreenId SCREEN_ID_FULL = 0;
    const ScreenId SCREEN_ID_MAIN = 5;

    #ifdef TP_FEATURE_ENABLE
    const int32_t TP_TYPE = 12;
    const std::string FULL_TP = "0";
    const std::string MAIN_TP = "1";
    #endif
} // namespace

DualDisplayDevicePolicy::DualDisplayDevicePolicy(std::recursive_mutex& displayInfoMutex,
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler)
    : displayInfoMutex_(displayInfoMutex), screenPowerTaskScheduler_(screenPowerTaskScheduler)
{
    TLOGI(WmsLogTag::DMS, "DualDisplayDevicePolicy created");

    ScreenId screenIdFull = 0;
    int32_t foldCreaseRegionPosX = 0;
    int32_t foldCreaseRegionPosY = 1064;
    int32_t foldCreaseRegionPosWidth = 2496;
    int32_t foldCreaseRegionPosHeight = 171;

    std::vector<DMRect> rect = {
        {
            foldCreaseRegionPosX, foldCreaseRegionPosY,
            foldCreaseRegionPosWidth, foldCreaseRegionPosHeight
        }
    };
    currentFoldCreaseRegion_ = new FoldCreaseRegion(screenIdFull, rect);
}

void DualDisplayDevicePolicy::ChangeScreenDisplayMode(FoldDisplayMode displayMode)
{
    TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayMode displayMode = %{public}d", displayMode);
    sptr<ScreenSession> screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_FULL);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "ChangeScreenDisplayMode default screenSession is null");
        return;
    }
    {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER,
            "ssm:ChangeScreenDisplayMode(displayMode = %" PRIu64")", displayMode);
        std::lock_guard<std::recursive_mutex> lock_mode(displayModeMutex_);
        if (currentDisplayMode_ == displayMode) {
            TLOGW(WmsLogTag::DMS, "ChangeScreenDisplayMode already in displayMode %{public}d", displayMode);
            return;
        }
        ReportFoldDisplayModeChange(displayMode);
        switch (displayMode) {
            case FoldDisplayMode::MAIN: {
                ChangeScreenDisplayModeToMain(screenSession);
                break;
            }
            case FoldDisplayMode::FULL: {
                ChangeScreenDisplayModeToFull(screenSession);
                break;
            }
            case FoldDisplayMode::UNKNOWN: {
                TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayMode displayMode is unknown");
                break;
            }
            default: {
                TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayMode displayMode is invalid");
                break;
            }
        }
        if (currentDisplayMode_ != displayMode) {
            TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayMode NotifyDisplayModeChanged displayMode = %{public}d",
                displayMode);
            ScreenSessionManager::GetInstance().NotifyDisplayModeChanged(displayMode);
        }
        currentDisplayMode_ = displayMode;
        globalDisplayMode_ = displayMode;
    }
}

void DualDisplayDevicePolicy::SendSensorResult(FoldStatus foldStatus)
{
    TLOGI(WmsLogTag::DMS, "SendSensorResult FoldStatus: %{public}d", foldStatus);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    if (displayMode != currentDisplayMode_) {
        ChangeScreenDisplayMode(displayMode);
    }
}

sptr<FoldCreaseRegion> DualDisplayDevicePolicy::GetCurrentFoldCreaseRegion()
{
    TLOGI(WmsLogTag::DMS, "GetCurrentFoldCreaseRegion");
    return currentFoldCreaseRegion_;
}

void DualDisplayDevicePolicy::LockDisplayStatus(bool locked)
{
    TLOGI(WmsLogTag::DMS, "LockDisplayStatus locked: %{public}d", locked);
    lockDisplayStatus_ = locked;
}

void DualDisplayDevicePolicy::SetOnBootAnimation(bool onBootAnimation)
{
    TLOGI(WmsLogTag::DMS, "SetOnBootAnimation onBootAnimation: %{public}d", onBootAnimation);
    onBootAnimation_ = onBootAnimation;
    if (!onBootAnimation_) {
        TLOGI(WmsLogTag::DMS, "SetOnBootAnimation when boot animation finished, change display mode");
        RecoverWhenBootAnimationExit();
    }
}

void DualDisplayDevicePolicy::RecoverWhenBootAnimationExit()
{
    TLOGI(WmsLogTag::DMS, "RecoverWhenBootAnimationExit currentScreen(%{public}" PRIu64 ")", screenId_);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    if (currentDisplayMode_ != displayMode) {
        ChangeScreenDisplayMode(displayMode);
    } else {
        TriggerScreenDisplayModeUpdate(displayMode);
    }
}

void DualDisplayDevicePolicy::TriggerScreenDisplayModeUpdate(FoldDisplayMode displayMode)
{
    TLOGI(WmsLogTag::DMS, "TriggerScreenDisplayModeUpdate displayMode = %{public}d", displayMode);
    sptr<ScreenSession> screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_FULL);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "TriggerScreenDisplayModeUpdate default screenSession is null");
        return;
    }
    switch (displayMode) {
        case FoldDisplayMode::MAIN: {
            ChangeScreenDisplayModeToMain(screenSession);
            break;
        }
        case FoldDisplayMode::FULL: {
            ChangeScreenDisplayModeToFull(screenSession);
            break;
        }
        case FoldDisplayMode::UNKNOWN: {
            TLOGI(WmsLogTag::DMS, "TriggerScreenDisplayModeUpdate displayMode is unknown");
            break;
        }
        default: {
            TLOGI(WmsLogTag::DMS, "TriggerScreenDisplayModeUpdate displayMode is invalid");
            break;
        }
    }
}

void DualDisplayDevicePolicy::UpdateForPhyScreenPropertyChange()
{
    TLOGI(WmsLogTag::DMS, "UpdateForPhyScreenPropertyChange currentScreen(%{public}" PRIu64 ")", screenId_);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    if (currentDisplayMode_ != displayMode) {
        ChangeScreenDisplayMode(displayMode);
    }
}

FoldDisplayMode DualDisplayDevicePolicy::GetModeMatchStatus()
{
    FoldDisplayMode displayMode = FoldDisplayMode::UNKNOWN;
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
        default: {
            TLOGI(WmsLogTag::DMS, "GetModeMatchStatus FoldStatus is invalid");
        }
    }
    return displayMode;
}

void DualDisplayDevicePolicy::ReportFoldDisplayModeChange(FoldDisplayMode displayMode)
{
    int32_t mode = static_cast<int32_t>(displayMode);
    TLOGI(WmsLogTag::DMS, "ReportFoldDisplayModeChange displayMode: %{public}d", mode);
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
        "DISPLAY_MODE",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "FOLD_DISPLAY_MODE", mode);

    if (ret != 0) {
        TLOGE(WmsLogTag::DMS, "ReportFoldDisplayModeChange Write HiSysEvent error, ret: %{public}d", ret);
    }
}

void DualDisplayDevicePolicy::ReportFoldStatusChangeBegin(int32_t offScreen, int32_t onScreen)
{
    TLOGI(WmsLogTag::DMS, "ReportFoldStatusChangeBegin offScreen: %{public}d, onScreen: %{public}d",
        offScreen, onScreen);
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
        "FOLD_STATE_CHANGE_BEGIN",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "POWER_OFF_SCREEN", offScreen,
        "POWER_ON_SCREEN", onScreen);

    if (ret != 0) {
        TLOGE(WmsLogTag::DMS, "ReportFoldStatusChangeBegin Write HiSysEvent error, ret: %{public}d", ret);
    }
}

void DualDisplayDevicePolicy::ChangeScreenDisplayModeToMain(sptr<ScreenSession> screenSession)
{
    if (onBootAnimation_) {
        ChangeScreenDisplayModeToMainOnBootAnimation(screenSession);
        return;
    }
    #ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, MAIN_TP.c_str());
    #endif
    if (PowerMgr::PowerMgrClient::GetInstance().IsScreenOn()) {
        TLOGI(WmsLogTag::DMS, "IsScreenOn is true, begin.");
        ReportFoldStatusChangeBegin((int32_t)SCREEN_ID_FULL, (int32_t)SCREEN_ID_MAIN);
        // off full screen
        auto taskScreenOnMainOff = [=] {
            TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToMain: IsScreenOn is true, screenIdFull OFF.");
            screenId_ = SCREEN_ID_FULL;
            ScreenSessionManager::GetInstance().SetNotifyLockOrNot(false);
            PowerMgr::PowerMgrClient::GetInstance().SuspendDevice();
            ScreenSessionManager::GetInstance().SetNotifyLockOrNot(true);
        };
        screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnMainOff, "screenOnMainOffTask");
        SendPropertyChangeResult(screenSession, SCREEN_ID_MAIN, ScreenPropertyChangeReason::FOLD_SCREEN_FOLDING);
        // on main screen
        auto taskScreenOnMainOn = [=] {
            TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToMain: IsScreenOn is true, screenIdMain ON.");
            screenId_ = SCREEN_ID_MAIN;
            ScreenSessionManager::GetInstance().SetNotifyLockOrNot(false);
            PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
            ScreenSessionManager::GetInstance().SetNotifyLockOrNot(true);
        };
        screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnMainOn, "screenOnMainOnTask");
    } else { // When the screen is off and folded, it is not powered on
        TLOGI(WmsLogTag::DMS, "IsScreenOn is false, begin.");
        // off full screen
        auto taskScreenOffMainOff = [=] {
            TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToMain: IsScreenOn is false, screenIdFull OFF.");
            screenId_ = SCREEN_ID_FULL;
            ScreenSessionManager::GetInstance().SetNotifyLockOrNot(false);
            PowerMgr::PowerMgrClient::GetInstance().SuspendDevice();
            ScreenSessionManager::GetInstance().SetNotifyLockOrNot(true);
        };
        screenPowerTaskScheduler_->PostAsyncTask(taskScreenOffMainOff, "screenOffMainOffTask");
        SendPropertyChangeResult(screenSession, SCREEN_ID_MAIN, ScreenPropertyChangeReason::FOLD_SCREEN_FOLDING);
        auto taskScreenOnMainChangeScreenId = [=] {
            TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToMain: IsScreenOn is false, Change ScreenId to Main.");
            screenId_ = SCREEN_ID_MAIN;
        };
        screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnMainChangeScreenId, "taskScreenOnMainChangeScreenId");
    }
}

void DualDisplayDevicePolicy::ChangeScreenDisplayModeToFull(sptr<ScreenSession> screenSession)
{
    if (onBootAnimation_) {
        ChangeScreenDisplayModeToFullOnBootAnimation(screenSession);
        return;
    }
    ReportFoldStatusChangeBegin((int32_t)SCREEN_ID_MAIN, (int32_t)SCREEN_ID_FULL);
    #ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, FULL_TP.c_str());
    #endif
    if (PowerMgr::PowerMgrClient::GetInstance().IsScreenOn()) {
        TLOGI(WmsLogTag::DMS, "IsScreenOn is true, begin.");
        // off main screen
        auto taskScreenOnFullOff = [=] {
            TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToFull: IsScreenOn is true, screenIdMain OFF.");
            screenId_ = SCREEN_ID_MAIN;
            ScreenSessionManager::GetInstance().SetNotifyLockOrNot(false);
            PowerMgr::PowerMgrClient::GetInstance().SuspendDevice();
            ScreenSessionManager::GetInstance().SetNotifyLockOrNot(true);
        };
        screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnFullOff, "screenOnFullOffTask");
        SendPropertyChangeResult(screenSession, SCREEN_ID_FULL, ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND);
        // on full screen
        auto taskScreenOnFullOn = [=] {
            TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToFull: IsScreenOn is true, screenIdFull ON.");
            screenId_ = SCREEN_ID_FULL;
            ScreenSessionManager::GetInstance().SetNotifyLockOrNot(false);
            PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
            ScreenSessionManager::GetInstance().SetNotifyLockOrNot(true);
        };
        screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnFullOn, "screenOnFullOnTask");
    } else { //AOD scene
        TLOGI(WmsLogTag::DMS, "IsScreenOn is false, begin.");
        // off main screen
        auto taskScreenOffFullOff = [=] {
            TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToFull: IsScreenOn is false, screenIdMain OFF.");
            screenId_ = SCREEN_ID_MAIN;
            ScreenSessionManager::GetInstance().SetNotifyLockOrNot(false);
            PowerMgr::PowerMgrClient::GetInstance().SuspendDevice();
            ScreenSessionManager::GetInstance().SetNotifyLockOrNot(true);
        };
        screenPowerTaskScheduler_->PostAsyncTask(taskScreenOffFullOff, "screenOffFullOffTask");
        SendPropertyChangeResult(screenSession, SCREEN_ID_FULL, ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND);
        // on full screen
        auto taskScreenOnFullOn = [=] {
            TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToFull: IsScreenOn is false, screenIdFull ON.");
            screenId_ = SCREEN_ID_FULL;
            PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
        };
        screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnFullOn, "screenOnFullOnTask");
    }
}

void DualDisplayDevicePolicy::SendPropertyChangeResult(sptr<ScreenSession> screenSession, ScreenId screenId,
    ScreenPropertyChangeReason reason)
{
    std::lock_guard<std::recursive_mutex> lock_info(displayInfoMutex_);
    screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(screenId);
    screenSession->UpdatePropertyByFoldControl(screenProperty_.GetBounds(), screenProperty_.GetPhyBounds());
    screenSession->PropertyChange(screenSession->GetScreenProperty(), reason);
    TLOGI(WmsLogTag::DMS, "screenBounds : width_= %{public}f, height_= %{public}f",
        screenSession->GetScreenProperty().GetBounds().rect_.width_,
        screenSession->GetScreenProperty().GetBounds().rect_.height_);
    ScreenSessionManager::GetInstance().NotifyDisplayChanged(screenSession->ConvertToDisplayInfo(),
        DisplayChangeEvent::DISPLAY_SIZE_CHANGED);
}

void DualDisplayDevicePolicy::ChangeScreenDisplayModeToMainOnBootAnimation(sptr<ScreenSession> screenSession)
{
    TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToMainOnBootAnimation");
    screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(SCREEN_ID_MAIN);
    screenSession->UpdatePropertyByFoldControl(screenProperty_.GetBounds(), screenProperty_.GetPhyBounds());
    screenSession->PropertyChange(screenSession->GetScreenProperty(),
        ScreenPropertyChangeReason::FOLD_SCREEN_FOLDING);
    TLOGI(WmsLogTag::DMS, "screenBounds : width_= %{public}f, height_= %{public}f",
        screenSession->GetScreenProperty().GetBounds().rect_.width_,
        screenSession->GetScreenProperty().GetBounds().rect_.height_);
    screenId_ = SCREEN_ID_MAIN;
}

void DualDisplayDevicePolicy::ChangeScreenDisplayModeToFullOnBootAnimation(sptr<ScreenSession> screenSession)
{
    TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToFullOnBootAnimation");
    screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(SCREEN_ID_FULL);
    screenSession->UpdatePropertyByFoldControl(screenProperty_.GetBounds(), screenProperty_.GetPhyBounds());
    screenSession->PropertyChange(screenSession->GetScreenProperty(),
        ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND);
    TLOGI(WmsLogTag::DMS, "screenBounds : width_= %{public}f, height_= %{public}f",
        screenSession->GetScreenProperty().GetBounds().rect_.width_,
        screenSession->GetScreenProperty().GetBounds().rect_.height_);
    screenId_ = SCREEN_ID_FULL;
}
} // namespace OHOS::Rosen