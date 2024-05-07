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

#include <hisysevent.h>
#include <hitrace_meter.h>
#include <transaction/rs_interfaces.h>
#include "dm_common.h"
#include "fold_screen_controller/dual_display_policy.h"
#include "session/screen/include/screen_session.h"
#include "screen_session_manager.h"

#include "window_manager_hilog.h"

#ifdef POWER_MANAGER_ENABLE
#include <power_mgr_client.h>
#endif

namespace OHOS::Rosen {
namespace {
    const ScreenId SCREEN_ID_MAIN = 0;
    const ScreenId SCREEN_ID_SUB = 5;
    #ifdef TP_FEATURE_ENABLE
    const int32_t TP_TYPE = 12;
    #endif
    const std::string MAIN_TP = "0";
    const std::string SUB_TP = "1";
} // namespace

DualDisplayPolicy::DualDisplayPolicy(std::recursive_mutex& displayInfoMutex,
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler)
    : displayInfoMutex_(displayInfoMutex), screenPowerTaskScheduler_(screenPowerTaskScheduler)
{
    TLOGI(WmsLogTag::DMS, "DualDisplayPolicy created");
}

void DualDisplayPolicy::ChangeScreenDisplayMode(FoldDisplayMode displayMode)
{
    TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayMode displayMode = %{public}d", displayMode);
    sptr<ScreenSession> screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_MAIN);
    if (displayMode == FoldDisplayMode::SUB) {
        screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_SUB);
    }
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
            case FoldDisplayMode::SUB: {
                ChangeScreenDisplayModeInner(screenSession, SCREEN_ID_MAIN, SCREEN_ID_SUB);
                break;
            }
            case FoldDisplayMode::MAIN: {
                ChangeScreenDisplayModeInner(screenSession, SCREEN_ID_SUB, SCREEN_ID_MAIN);
                break;
            }
            case FoldDisplayMode::COORDINATION: {
                ChangeScreenDisplayModeToCoordination();
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

void DualDisplayPolicy::SendSensorResult(FoldStatus foldStatus)
{
    TLOGI(WmsLogTag::DMS, "SendSensorResult FoldStatus: %{public}d", foldStatus);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    if (displayMode != currentDisplayMode_) {
        ChangeScreenDisplayMode(displayMode);
    }
}

sptr<FoldCreaseRegion> DualDisplayPolicy::GetCurrentFoldCreaseRegion()
{
    TLOGI(WmsLogTag::DMS, "GetCurrentFoldCreaseRegion");
    return currentFoldCreaseRegion_;
}

void DualDisplayPolicy::LockDisplayStatus(bool locked)
{
    TLOGI(WmsLogTag::DMS, "LockDisplayStatus locked: %{public}d", locked);
    lockDisplayStatus_ = locked;
}

void DualDisplayPolicy::SetOnBootAnimation(bool onBootAnimation)
{
    TLOGI(WmsLogTag::DMS, "SetOnBootAnimation onBootAnimation: %{public}d", onBootAnimation);
    onBootAnimation_ = onBootAnimation;
    if (!onBootAnimation_) {
        TLOGI(WmsLogTag::DMS, "SetOnBootAnimation when boot animation finished, change display mode");
        RecoverWhenBootAnimationExit();
    }
}

void DualDisplayPolicy::RecoverWhenBootAnimationExit()
{
    TLOGI(WmsLogTag::DMS, "RecoverWhenBootAnimationExit currentScreen(%{public}" PRIu64 ")", screenId_);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    if (currentDisplayMode_ != displayMode) {
        ChangeScreenDisplayMode(displayMode);
    } else {
        TriggerScreenDisplayModeUpdate(displayMode);
    }
}

void DualDisplayPolicy::TriggerScreenDisplayModeUpdate(FoldDisplayMode displayMode)
{
    TLOGI(WmsLogTag::DMS, "TriggerScreenDisplayModeUpdate displayMode = %{public}d", displayMode);
    sptr<ScreenSession> screenSession = nullptr;
    if (displayMode == FoldDisplayMode::SUB) {
        screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_SUB);
    } else {
        screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_MAIN);
    }
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "TriggerScreenDisplayModeUpdate default screenSession is null");
        return;
    }
    switch (displayMode) {
        case FoldDisplayMode::SUB: {
            ChangeScreenDisplayModeInner(screenSession, SCREEN_ID_MAIN, SCREEN_ID_SUB);
            break;
        }
        case FoldDisplayMode::MAIN: {
            ChangeScreenDisplayModeInner(screenSession, SCREEN_ID_SUB, SCREEN_ID_MAIN);
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

void DualDisplayPolicy::UpdateForPhyScreenPropertyChange()
{
    TLOGI(WmsLogTag::DMS, "UpdateForPhyScreenPropertyChange currentScreen(%{public}" PRIu64 ")", screenId_);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    if (currentDisplayMode_ != displayMode) {
        ChangeScreenDisplayMode(displayMode);
    }
}

FoldDisplayMode DualDisplayPolicy::GetModeMatchStatus()
{
    FoldDisplayMode displayMode = FoldDisplayMode::UNKNOWN;
    switch (currentFoldStatus_) {
        case FoldStatus::EXPAND: {
            displayMode = FoldDisplayMode::MAIN;
            break;
        }
        case FoldStatus::FOLDED: {
            displayMode = FoldDisplayMode::SUB;
            break;
        }
        case FoldStatus::HALF_FOLD: {
            displayMode = FoldDisplayMode::MAIN;
            break;
        }
        default: {
            TLOGI(WmsLogTag::DMS, "GetModeMatchStatus FoldStatus is invalid");
        }
    }
    return displayMode;
}

void DualDisplayPolicy::ReportFoldDisplayModeChange(FoldDisplayMode displayMode)
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

void DualDisplayPolicy::ReportFoldStatusChangeBegin(int32_t offScreen, int32_t onScreen)
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

void DualDisplayPolicy::ChangeScreenDisplayModeInner(sptr<ScreenSession> screenSession, ScreenId offScreenId,
    ScreenId onScreenId)
{
    if (onBootAnimation_) {
        ChangeScreenDisplayModeOnBootAnimation(screenSession, onScreenId);
        return;
    }
    ScreenPropertyChangeReason reason = ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND;
    std::string tp = MAIN_TP;
    if (onScreenId == SCREEN_ID_SUB) {
        reason = ScreenPropertyChangeReason::FOLD_SCREEN_FOLDING;
        tp = SUB_TP;
    }
    #ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, tp.c_str());
    #endif
    ReportFoldStatusChangeBegin((int32_t)SCREEN_ID_MAIN, (int32_t)SCREEN_ID_SUB);
    bool isScreenOn = PowerMgr::PowerMgrClient::GetInstance().IsScreenOn();
    auto taskScreenOff = [=] {
        TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayMode: off screenId: %{public}" PRIu64 "", offScreenId);
        screenId_ = offScreenId;
        ScreenSessionManager::GetInstance().SetNotifyLockOrNot(false);
        PowerMgr::PowerMgrClient::GetInstance().SuspendDevice();
        ScreenSessionManager::GetInstance().SetNotifyLockOrNot(true);
    };
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOff, "screenOffTask");

    auto taskScreenOn = [=] {
        TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayMode: on screenId: %{public}" PRIu64 "", onScreenId);
        screenId_ = onScreenId;
        if (isScreenOn) {
            ScreenSessionManager::GetInstance().SetNotifyLockOrNot(false);
            PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
            ScreenSessionManager::GetInstance().SetNotifyLockOrNot(true);
        } else {
            PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
        }
    };
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOn, "screenOnTask");
    SendPropertyChangeResult(screenSession, onScreenId, reason);
}

void DualDisplayPolicy::ChangeScreenDisplayModeToCoordination()
{
    TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToCoordination");
    #ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, MAIN_TP.c_str());
    #endif
    bool isScreenOn = PowerMgr::PowerMgrClient::GetInstance().IsScreenOn();
    // on main screen
    auto taskScreenOnMain = [=] {
        TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayMode: on screenId: 0");
        screenId_ = SCREEN_ID_MAIN;
        if (isScreenOn) {
            ScreenSessionManager::GetInstance().SetNotifyLockOrNot(false);
            PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
            ScreenSessionManager::GetInstance().SetNotifyLockOrNot(true);
        } else {
            PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
        }
    };
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnMain, "taskScreenOnMain");

    auto taskScreenOnSub = [=] {
        TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayMode: on screenId: 1");
        screenId_ = SCREEN_ID_SUB;
        PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
    };
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnSub, "taskScreenOnSub");
}

void DualDisplayPolicy::SendPropertyChangeResult(sptr<ScreenSession> screenSession, ScreenId screenId,
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

void DualDisplayPolicy::ChangeScreenDisplayModeOnBootAnimation(sptr<ScreenSession> screenSession, ScreenId screenId)
{
    TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToFullOnBootAnimation");
    screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(screenId);
    ScreenPropertyChangeReason reason = ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND;
    if (screenId == SCREEN_ID_SUB) {
        reason = ScreenPropertyChangeReason::FOLD_SCREEN_FOLDING;
    }
    screenSession->UpdatePropertyByFoldControl(screenProperty_.GetBounds(), screenProperty_.GetPhyBounds());
    screenSession->PropertyChange(screenSession->GetScreenProperty(), reason);
    TLOGI(WmsLogTag::DMS, "screenBounds : width_= %{public}f, height_= %{public}f",
        screenSession->GetScreenProperty().GetBounds().rect_.width_,
        screenSession->GetScreenProperty().GetBounds().rect_.height_);
    screenId_ = screenId;
}
} // namespace OHOS::Rosen