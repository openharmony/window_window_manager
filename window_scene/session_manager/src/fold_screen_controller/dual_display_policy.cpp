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
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DualDisplayPolicy"};
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
    WLOGI("DualDisplayPolicy created");
}

void DualDisplayPolicy::ChangeScreenDisplayMode(FoldDisplayMode displayMode)
{
    WLOGI("ChangeScreenDisplayMode displayMode = %{public}d", displayMode);
    sptr<ScreenSession> screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_MAIN);
    if (displayMode == FoldDisplayMode::SUB) {
        screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_SUB);
    }
    if (screenSession == nullptr) {
        WLOGE("ChangeScreenDisplayMode default screenSession is null");
        return;
    }
    {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER,
            "ssm:ChangeScreenDisplayMode(displayMode = %" PRIu64")", displayMode);
        std::lock_guard<std::recursive_mutex> lock_mode(displayModeMutex_);
        if (currentDisplayMode_ == displayMode) {
            WLOGFW("ChangeScreenDisplayMode already in displayMode %{public}d", displayMode);
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
                WLOGFI("ChangeScreenDisplayMode displayMode is unknown");
                break;
            }
            default: {
                WLOGFI("ChangeScreenDisplayMode displayMode is invalid");
                break;
            }
        }
        if (currentDisplayMode_ != displayMode) {
            WLOGI("ChangeScreenDisplayMode NotifyDisplayModeChanged displayMode = %{public}d", displayMode);
            ScreenSessionManager::GetInstance().NotifyDisplayModeChanged(displayMode);
        }
        currentDisplayMode_ = displayMode;
        globalDisplayMode_ = displayMode;
    }
}

void DualDisplayPolicy::SendSensorResult(FoldStatus foldStatus)
{
    WLOGI("SendSensorResult FoldStatus: %{public}d", foldStatus);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    if (displayMode != currentDisplayMode_) {
        ChangeScreenDisplayMode(displayMode);
    }
}

sptr<FoldCreaseRegion> DualDisplayPolicy::GetCurrentFoldCreaseRegion()
{
    WLOGI("GetCurrentFoldCreaseRegion");
    return currentFoldCreaseRegion_;
}

void DualDisplayPolicy::LockDisplayStatus(bool locked)
{
    WLOGI("LockDisplayStatus locked: %{public}d", locked);
    lockDisplayStatus_ = locked;
}

void DualDisplayPolicy::SetOnBootAnimation(bool onBootAnimation)
{
    WLOGI("SetOnBootAnimation onBootAnimation: %{public}d", onBootAnimation);
    onBootAnimation_ = onBootAnimation;
    if (!onBootAnimation_) {
        WLOGI("SetOnBootAnimation when boot animation finished, change display mode");
        RecoverWhenBootAnimationExit();
    }
}

void DualDisplayPolicy::RecoverWhenBootAnimationExit()
{
    WLOGI("RecoverWhenBootAnimationExit currentScreen(%{public}" PRIu64 ")", screenId_);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    if (currentDisplayMode_ != displayMode) {
        ChangeScreenDisplayMode(displayMode);
    } else {
        TriggerScreenDisplayModeUpdate(displayMode);
    }
}

void DualDisplayPolicy::TriggerScreenDisplayModeUpdate(FoldDisplayMode displayMode)
{
    WLOGI("TriggerScreenDisplayModeUpdate displayMode = %{public}d", displayMode);
    sptr<ScreenSession> screenSession = nullptr;
    if (displayMode == FoldDisplayMode::SUB) {
        screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_SUB);
    } else {
        screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_MAIN);
    }
    if (screenSession == nullptr) {
        WLOGE("TriggerScreenDisplayModeUpdate default screenSession is null");
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
            WLOGFI("TriggerScreenDisplayModeUpdate displayMode is unknown");
            break;
        }
        default: {
            WLOGFI("TriggerScreenDisplayModeUpdate displayMode is invalid");
            break;
        }
    }
}

void DualDisplayPolicy::UpdateForPhyScreenPropertyChange()
{
    WLOGI("UpdateForPhyScreenPropertyChange currentScreen(%{public}" PRIu64 ")", screenId_);
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
            WLOGI("GetModeMatchStatus FoldStatus is invalid");
        }
    }
    return displayMode;
}

void DualDisplayPolicy::ReportFoldDisplayModeChange(FoldDisplayMode displayMode)
{
    int32_t mode = static_cast<int32_t>(displayMode);
    WLOGI("ReportFoldDisplayModeChange displayMode: %{public}d", mode);
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
        "DISPLAY_MODE",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "FOLD_DISPLAY_MODE", mode);
    if (ret != 0) {
        WLOGE("ReportFoldDisplayModeChange Write HiSysEvent error, ret: %{public}d", ret);
    }
}

void DualDisplayPolicy::ReportFoldStatusChangeBegin(int32_t offScreen, int32_t onScreen)
{
    WLOGI("ReportFoldStatusChangeBegin offScreen: %{public}d, onScreen: %{public}d", offScreen, onScreen);
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
        "FOLD_STATE_CHANGE_BEGIN",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "POWER_OFF_SCREEN", offScreen,
        "POWER_ON_SCREEN", onScreen);
    if (ret != 0) {
        WLOGE("ReportFoldStatusChangeBegin Write HiSysEvent error, ret: %{public}d", ret);
    }
}

void DualDisplayPolicy::ChangeScreenDisplayModeInner(sptr<ScreenSession> screenSession, int32_t offScreenId,
    int32_t onScreenId)
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
    auto taskScreenOff = [=] {
        WLOGFI("ChangeScreenDisplayMode: off screenId: %{public}d", offScreenId);
        screenId_ = offScreenId;
        ScreenSessionManager::GetInstance().SetKeyguardDrawnDoneFlag(false);
        ScreenSessionManager::GetInstance().SetScreenPower(ScreenPowerStatus::POWER_STATUS_OFF,
            PowerStateChangeReason::STATE_CHANGE_REASON_DISPLAY_SWITCH);
    };
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOff, "screenOffTask");

    auto taskScreenOn = [=] {
        WLOGFI("ChangeScreenDisplayMode: on screenId: %{public}d", onScreenId);
        screenId_ = onScreenId;
        if (PowerMgr::PowerMgrClient::GetInstance().IsScreenOn()) {
            ScreenSessionManager::GetInstance().SetKeyguardDrawnDoneFlag(false);
            ScreenSessionManager::GetInstance().SetScreenPower(ScreenPowerStatus::POWER_STATUS_ON,
                PowerStateChangeReason::STATE_CHANGE_REASON_DISPLAY_SWITCH);
            PowerMgr::PowerMgrClient::GetInstance().RefreshActivity();
        } else {
            PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
        }
    };
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOn, "screenOnTask");
    SendPropertyChangeResult(screenSession, onScreenId, reason);
}

void DualDisplayPolicy::ChangeScreenDisplayModeToCoordination()
{
    WLOGFI("ChangeScreenDisplayModeToCoordination");
    #ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, MAIN_TP.c_str());
    #endif
    // on main screen
    auto taskScreenOnMain = [=] {
        WLOGFI("ChangeScreenDisplayMode: on screenId: 0");
        screenId_ = SCREEN_ID_MAIN;
        if (PowerMgr::PowerMgrClient::GetInstance().IsScreenOn()) {
            ScreenSessionManager::GetInstance().SetKeyguardDrawnDoneFlag(false);
            ScreenSessionManager::GetInstance().SetScreenPower(ScreenPowerStatus::POWER_STATUS_ON,
                PowerStateChangeReason::STATE_CHANGE_REASON_DISPLAY_SWITCH);
            PowerMgr::PowerMgrClient::GetInstance().RefreshActivity();
        } else {
            PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
        }
    };
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnMain, "taskScreenOnMain");

    auto taskScreenOnSub = [=] {
        WLOGFI("ChangeScreenDisplayMode: on screenId: 1");
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
    WLOGFI("screenBounds : width_= %{public}f, height_= %{public}f",
        screenSession->GetScreenProperty().GetBounds().rect_.width_,
        screenSession->GetScreenProperty().GetBounds().rect_.height_);
    ScreenSessionManager::GetInstance().NotifyDisplayChanged(screenSession->ConvertToDisplayInfo(),
        DisplayChangeEvent::DISPLAY_SIZE_CHANGED);
}

void DualDisplayPolicy::ChangeScreenDisplayModeOnBootAnimation(sptr<ScreenSession> screenSession, int32_t screenId)
{
    WLOGFI("ChangeScreenDisplayModeToFullOnBootAnimation");
    screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(screenId);
    ScreenPropertyChangeReason reason = ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND;
    if (screenId == SCREEN_ID_SUB) {
        reason = ScreenPropertyChangeReason::FOLD_SCREEN_FOLDING;
    }
    screenSession->UpdatePropertyByFoldControl(screenProperty_.GetBounds(), screenProperty_.GetPhyBounds());
    screenSession->PropertyChange(screenSession->GetScreenProperty(), reason);
    WLOGFI("screenBounds : width_= %{public}f, height_= %{public}f",
        screenSession->GetScreenProperty().GetBounds().rect_.width_,
        screenSession->GetScreenProperty().GetBounds().rect_.height_);
    screenId_ = screenId;
}
} // namespace OHOS::Rosen