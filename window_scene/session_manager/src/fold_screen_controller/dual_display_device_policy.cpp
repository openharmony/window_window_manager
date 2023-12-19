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
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "DualDisplayDevicePolicy"};
    const ScreenId SCREEN_ID_FULL = 0;
    const ScreenId SCREEN_ID_MAIN = 5;

    #ifdef TP_FEATURE_ENABLE
    const int32_t TP_TYPE = 12;
    const std::string FULL_TP = "0";
    const std::string MAIN_TP = "1";
    #endif
} // namespace

DualDisplayDevicePolicy::DualDisplayDevicePolicy(std::recursive_mutex& displayInfoMutex): displayInfoMutex_(displayInfoMutex)
{
    WLOGI("DualDisplayDevicePolicy created");

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
    WLOGI("ChangeScreenDisplayMode displayMode = %{public}d", displayMode);
    sptr<ScreenSession> screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_FULL);
    if (screenSession == nullptr) {
        WLOGE("ChangeScreenDisplayMode default screenSession is null");
        return;
    }
    {
        std::lock_guard<std::recursive_mutex> lock_mode(displayModeMutex_);
        if (currentDisplayMode_ == displayMode) {
            WLOGFW("ChangeScreenDisplayMode already in displayMode %{public}d", displayMode);
            return;
        }
        std::lock_guard<std::recursive_mutex> lock_info(displayInfoMutex_);
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
    }
}

FoldDisplayMode DualDisplayDevicePolicy::GetScreenDisplayMode()
{
    std::lock_guard<std::recursive_mutex> lock_mode(displayModeMutex_);
    return currentDisplayMode_;
}

FoldStatus DualDisplayDevicePolicy::GetFoldStatus()
{
    return currentFoldStatus_;
}

void DualDisplayDevicePolicy::SendSensorResult(FoldStatus foldStatus)
{
    WLOGI("SendSensorResult FoldStatus: %{public}d", foldStatus);
    currentFoldStatus_ = foldStatus;
    FoldDisplayMode displayMode = GetModeMatchStatus();
    if (displayMode != currentDisplayMode_) {
        ChangeScreenDisplayMode(displayMode);
    }
}

sptr<FoldCreaseRegion> DualDisplayDevicePolicy::GetCurrentFoldCreaseRegion()
{
    WLOGI("GetCurrentFoldCreaseRegion");
    return currentFoldCreaseRegion_;
}

void DualDisplayDevicePolicy::LockDisplayStatus(bool locked)
{
    WLOGI("LockDisplayStatus locked: %{public}d", locked);
    lockDisplayStatus_ = locked;
}

void DualDisplayDevicePolicy::SetOnBootAnimation(bool onBootAnimation)
{
    WLOGI("SetOnBootAnimation onBootAnimation: %{public}d", onBootAnimation);
    onBootAnimation_ = onBootAnimation;
    if (!onBootAnimation_) {
        WLOGI("SetOnBootAnimation when boot animation finished, change display mode");
        RecoverWhenBootAnimationExit();
    }
}

void DualDisplayDevicePolicy::RecoverWhenBootAnimationExit()
{
    WLOGI("RecoverWhenBootAnimationExit currentScreen(%{public}" PRIu64 ")", screenId_);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    if (currentDisplayMode_ != displayMode) {
        ChangeScreenDisplayMode(displayMode);
    } else {
        TriggerScreenDisplayModeUpdate(displayMode);
    }
}

void DualDisplayDevicePolicy::TriggerScreenDisplayModeUpdate(FoldDisplayMode displayMode)
{
    WLOGI("TriggerScreenDisplayModeUpdate displayMode = %{public}d", displayMode);
    sptr<ScreenSession> screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_FULL);
    if (screenSession == nullptr) {
        WLOGE("TriggerScreenDisplayModeUpdate default screenSession is null");
        return;
    }
    {
        std::lock_guard<std::recursive_mutex> lock_info(displayInfoMutex_);
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
                WLOGFI("TriggerScreenDisplayModeUpdate displayMode is unknown");
                break;
            }
            default: {
                WLOGFI("TriggerScreenDisplayModeUpdate displayMode is invalid");
                break;
            }
        }
    }
}

void DualDisplayDevicePolicy::UpdateForPhyScreenPropertyChange()
{
    WLOGI("UpdateForPhyScreenPropertyChange currentScreen(%{public}" PRIu64 ")", screenId_);
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
            WLOGI("GetModeMatchStatus FoldStatus is invalid");
        }
    }
    return displayMode;
}

void DualDisplayDevicePolicy::ReportFoldDisplayModeChange(FoldDisplayMode displayMode)
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

void DualDisplayDevicePolicy::ReportFoldStatusChangeBegin(int32_t offScreen, int32_t onScreen)
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

void DualDisplayDevicePolicy::ChangeScreenDisplayModeToMain(sptr<ScreenSession> screenSession)
{
    if (onBootAnimation_) {
        ChangeScreenDisplayModeToMainOnBootAnimation(screenSession);
        return;
    }
    ReportFoldStatusChangeBegin((int32_t)SCREEN_ID_FULL, (int32_t)SCREEN_ID_MAIN);
    #ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, MAIN_TP.c_str());
    #endif
    if (PowerMgr::PowerMgrClient::GetInstance().IsScreenOn()) {
        // off full screen
        screenId_ = SCREEN_ID_FULL;
        ScreenSessionManager::GetInstance().SetKeyguardDrawnDoneFlag(false);
        WLOGFI("ChangeScreenDisplayModeToMain keyguardDrawnDone_ is false");
        ScreenSessionManager::GetInstance().SetScreenPower(ScreenPowerStatus::POWER_STATUS_OFF,
            PowerStateChangeReason::POWER_BUTTON);
        screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(SCREEN_ID_MAIN);
        screenSession->UpdatePropertyByFoldControl(screenProperty_.GetBounds(), screenProperty_.GetPhyBounds());
        screenSession->PropertyChange(screenSession->GetScreenProperty(),
            ScreenPropertyChangeReason::FOLD_SCREEN_FOLDING);
        ScreenSessionManager::GetInstance().NotifyDisplayChanged(screenSession->ConvertToDisplayInfo(),
            DisplayChangeEvent::DISPLAY_SIZE_CHANGED);
        // on main screen
        screenId_ = SCREEN_ID_MAIN;
        ScreenSessionManager::GetInstance().SetKeyguardDrawnDoneFlag(false);
        WLOGFI("ChangeScreenDisplayModeToMain keyguardDrawnDone_ is false");
        ScreenSessionManager::GetInstance().SetScreenPower(ScreenPowerStatus::POWER_STATUS_ON,
            PowerStateChangeReason::POWER_BUTTON);
        WLOGFI("ChangeScreenDisplayMode screenIdFull OFF and screenIdMain ON");
        ScreenSessionManager::GetInstance().SetDisplayNodeScreenId(screenSession->GetScreenId(), SCREEN_ID_MAIN);
    } else { // When the screen is off and folded, it is not powered on
        // off full screen
        screenId_ = SCREEN_ID_FULL;
        ScreenSessionManager::GetInstance().SetKeyguardDrawnDoneFlag(false);
        WLOGFI("ChangeScreenDisplayModeToMain keyguardDrawnDone_ is false");
        ScreenSessionManager::GetInstance().SetScreenPower(ScreenPowerStatus::POWER_STATUS_OFF,
            PowerStateChangeReason::POWER_BUTTON);
        screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(SCREEN_ID_MAIN);
        screenSession->UpdatePropertyByFoldControl(screenProperty_.GetBounds(), screenProperty_.GetPhyBounds());
        screenSession->PropertyChange(screenSession->GetScreenProperty(),
            ScreenPropertyChangeReason::FOLD_SCREEN_FOLDING);
        ScreenSessionManager::GetInstance().NotifyDisplayChanged(screenSession->ConvertToDisplayInfo(),
            DisplayChangeEvent::DISPLAY_SIZE_CHANGED);
        // on main screen
        WLOGFI("ChangeScreenDisplayMode screenIdFull no need to OFF and screenIdMain ON");
        ScreenSessionManager::GetInstance().SetDisplayNodeScreenId(screenSession->GetScreenId(), SCREEN_ID_MAIN);
        screenId_ = SCREEN_ID_MAIN;
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
        // off main screen
        screenId_ = SCREEN_ID_MAIN;
        ScreenSessionManager::GetInstance().SetKeyguardDrawnDoneFlag(false);
        WLOGFI("ChangeScreenDisplayModeToMain keyguardDrawnDone_ is false");
        ScreenSessionManager::GetInstance().SetScreenPower(ScreenPowerStatus::POWER_STATUS_OFF,
            PowerStateChangeReason::POWER_BUTTON);
        screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(SCREEN_ID_FULL);
        screenSession->UpdatePropertyByFoldControl(screenProperty_.GetBounds(), screenProperty_.GetPhyBounds());
        screenSession->PropertyChange(screenSession->GetScreenProperty(),
            ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND);
        // on full screen
        screenId_ = SCREEN_ID_FULL;
        ScreenSessionManager::GetInstance().SetKeyguardDrawnDoneFlag(false);
        WLOGFI("ChangeScreenDisplayModeToMain keyguardDrawnDone_ is false");
        ScreenSessionManager::GetInstance().SetScreenPower(ScreenPowerStatus::POWER_STATUS_ON,
            PowerStateChangeReason::POWER_BUTTON);
        WLOGFI("ChangeScreenDisplayMode screenIdMain OFF and screenIdFull ON");
        ScreenSessionManager::GetInstance().SetDisplayNodeScreenId(screenSession->GetScreenId(), SCREEN_ID_FULL);
    } else { //AOD scene
        // off main screen
        screenId_ = SCREEN_ID_MAIN;
        ScreenSessionManager::GetInstance().SetKeyguardDrawnDoneFlag(false);
        WLOGFI("ChangeScreenDisplayModeToMain keyguardDrawnDone_ is false");
        ScreenSessionManager::GetInstance().SetScreenPower(ScreenPowerStatus::POWER_STATUS_OFF,
            PowerStateChangeReason::POWER_BUTTON);
        screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(SCREEN_ID_FULL);
        screenSession->UpdatePropertyByFoldControl(screenProperty_.GetBounds(), screenProperty_.GetPhyBounds());
        screenSession->PropertyChange(screenSession->GetScreenProperty(),
            ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND);
        // on full screen
        screenId_ = SCREEN_ID_FULL;
        WLOGFI("ChangeScreenDisplayModeToMain WakeupDevice begin");
        PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
        WLOGFI("ChangeScreenDisplayMode screenIdMain OFF and screenIdFull ON");
        ScreenSessionManager::GetInstance().SetDisplayNodeScreenId(screenSession->GetScreenId(), SCREEN_ID_FULL);
    }
}

void DualDisplayDevicePolicy::ChangeScreenDisplayModeToMainOnBootAnimation(sptr<ScreenSession> screenSession)
{
    WLOGFI("ChangeScreenDisplayModeToMainOnBootAnimation");
    screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(SCREEN_ID_MAIN);
    screenSession->UpdatePropertyByFoldControl(screenProperty_.GetBounds(), screenProperty_.GetPhyBounds());
    screenSession->PropertyChange(screenSession->GetScreenProperty(),
        ScreenPropertyChangeReason::FOLD_SCREEN_FOLDING);
    screenId_ = SCREEN_ID_MAIN;
}

void DualDisplayDevicePolicy::ChangeScreenDisplayModeToFullOnBootAnimation(sptr<ScreenSession> screenSession)
{
    WLOGFI("ChangeScreenDisplayModeToFullOnBootAnimation");
    screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(SCREEN_ID_FULL);
    screenSession->UpdatePropertyByFoldControl(screenProperty_.GetBounds(), screenProperty_.GetPhyBounds());
    screenSession->PropertyChange(screenSession->GetScreenProperty(),
        ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND);
    screenId_ = SCREEN_ID_FULL;
}
} // namespace OHOS::Rosen