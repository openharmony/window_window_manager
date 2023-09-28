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

#include <transaction/rs_interfaces.h>
#include "fold_screen_controller/dual_display_device_policy.h"
#include "session/screen/include/screen_session.h"
#include "screen_session_manager.h"

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "DualDisplayDevicePolicy"};
} // namespace

DualDisplayDevicePolicy::DualDisplayDevicePolicy()
{
    WLOGI("DualDisplayDevicePolicy created");
}

void DualDisplayDevicePolicy::ChangeScreenDisplayMode(FoldDisplayMode displayMode)
{
    WLOGI("DualDisplayDevicePolicy ChangeScreenDisplayMode displayMode = %{public}d", displayMode);
    ScreenId screenIdFull = 0;
    ScreenId screenIdMain = 5;
    #ifdef TP_FEATURE_ENABLE
    int32_t tpType = 12;
    std::string fullTpChange = "0";
    std::string mainTpChange = "1";
    #endif
    sptr<ScreenSession> screenSession = ScreenSessionManager::GetInstance().GetScreenSession(screenIdFull);
    if (screenSession == nullptr) {
        WLOGE("ChangeScreenDisplayMode default screenSession is null");
        return;
    }
    {
        std::lock_guard<std::recursive_mutex> lock_mode(displayModeMutex_);
        switch (displayMode) {
            case FoldDisplayMode::MAIN: {
                #ifdef TP_FEATURE_ENABLE
                RSInterfaces::GetInstance().SetTpFeatureConfig(tpType, mainTpChange.c_str());
                #endif
                //off full screen
                RSInterfaces::GetInstance().SetScreenPowerStatus(screenIdFull, ScreenPowerStatus::POWER_STATUS_OFF);
                //on main screen
                RSInterfaces::GetInstance().SetScreenPowerStatus(screenIdMain, ScreenPowerStatus::POWER_STATUS_ON);
                WLOGFI("ChangeScreenDisplayMode screenIdFull OFF and screenIdMain ON");

                screenSession->SetDisplayNodeScreenId(screenIdMain);
                screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(screenIdMain);
                screenSession->SetScreenProperty(screenProperty_);
                screenSession->PropertyChange(screenProperty_, ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND);
                screenId_ = screenIdMain;
                break;
            }
            case FoldDisplayMode::FULL: {
                #ifdef TP_FEATURE_ENABLE
                RSInterfaces::GetInstance().SetTpFeatureConfig(tpType, fullTpChange.c_str());
                #endif
                //off main screen
                RSInterfaces::GetInstance().SetScreenPowerStatus(screenIdMain, ScreenPowerStatus::POWER_STATUS_OFF);
                //on full screen
                RSInterfaces::GetInstance().SetScreenPowerStatus(screenIdFull, ScreenPowerStatus::POWER_STATUS_ON);
                WLOGFI("ChangeScreenDisplayMode screenIdMain OFF and screenIdFull ON");

                screenSession->SetDisplayNodeScreenId(screenIdFull);
                screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(screenIdFull);
                screenSession->SetScreenProperty(screenProperty_);
                screenSession->PropertyChange(screenProperty_, ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND);
                screenId_ = screenIdFull;
                break;
            }
            case FoldDisplayMode::UNKNOWN: {
                WLOGFI("DualDisplayDevicePolicy ChangeScreenDisplayMode displayMode is unknown");
                break;
            }
            default: {
                WLOGFI("DualDisplayDevicePolicy ChangeScreenDisplayMode displayMode is invalid");
                break;
            }
        }
        if (currentDisplayMode_ != displayMode) {
            WLOGFI("DualDisplayDevicePolicy NotifyDisplayModeChanged displayMode %{pubilc}d", displayMode);
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
    WLOGI("DualDisplayDevicePolicy SendSensorResult FoldStatus: %{public}d", foldStatus);
    FoldDisplayMode tempDisplayMode = FoldDisplayMode::UNKNOWN;
    switch (foldStatus) {
        case FoldStatus::EXPAND: {
            tempDisplayMode = FoldDisplayMode::FULL;
            break;
        }
        case FoldStatus::FOLDED: {
            tempDisplayMode = FoldDisplayMode::MAIN;
            break;
        }
        case FoldStatus::HALF_FOLD: {
            tempDisplayMode = FoldDisplayMode::FULL;
            break;
        }
        default: {
            WLOGI("DualDisplayDevicePolicy SendSensorResult FoldStatus is invalid");
        }
    }

    currentFoldStatus_ = foldStatus;

    if (tempDisplayMode != currentDisplayMode_) {
        ChangeScreenDisplayMode(tempDisplayMode);
    }
}
} // namespace OHOS::Rosen