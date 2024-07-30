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
#include "fold_screen_controller/dual_display_fold_policy.h"
#include "session/screen/include/screen_session.h"
#include "screen_session_manager.h"

#include "window_manager_hilog.h"
#include "parameters.h"

#ifdef POWER_MANAGER_ENABLE
#include <power_mgr_client.h>
#endif

namespace OHOS::Rosen {
namespace {
const ScreenId SCREEN_ID_MAIN = 0;
const ScreenId SCREEN_ID_SUB = 5;
const bool IS_COORDINATION_SUPPORT =
    OHOS::system::GetBoolParameter("const.window.foldabledevice.is_coordination_support", false);
#ifdef TP_FEATURE_ENABLE
const int32_t TP_TYPE = 12;
#endif
const std::string MAIN_TP = "0";
const std::string SUB_TP = "1";
const int32_t REMOVE_DISPLAY_NODE = 0;
const int32_t ADD_DISPLAY_NODE = 0;
} // namespace

DualDisplayFoldPolicy::DualDisplayFoldPolicy(std::recursive_mutex& displayInfoMutex,
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler)
    : displayInfoMutex_(displayInfoMutex), screenPowerTaskScheduler_(screenPowerTaskScheduler)
{
    TLOGI(WmsLogTag::DMS, "DualDisplayFoldPolicy created");
}

bool DualDisplayFoldPolicy::CheckDisplayMode(FoldDisplayMode displayMode)
{
    if (displayMode == FoldDisplayMode::COORDINATION && !IS_COORDINATION_SUPPORT) {
        TLOGI(WmsLogTag::DMS, "Current device is not support coordination");
        return false;
    }
    if (currentDisplayMode_ == displayMode) {
        TLOGW(WmsLogTag::DMS, "ChangeScreenDisplayMode already in displayMode %{public}d", displayMode);
        return false;
    }
    bool isScreenOn = PowerMgr::PowerMgrClient::GetInstance().IsFoldScreenOn();
    if (currentDisplayMode_ == FoldDisplayMode::COORDINATION && isScreenOn &&
        displayMode == FoldDisplayMode::MAIN) {
        TLOGI(WmsLogTag::DMS, "CurrentDisplayMode is coordination, HalfFold no need to change displaympde");
        return false;
    }
    return true;
}

void DualDisplayFoldPolicy::ChangeScreenDisplayMode(FoldDisplayMode displayMode)
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
        if (!CheckDisplayMode(displayMode)) {
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

void DualDisplayFoldPolicy::SendSensorResult(FoldStatus foldStatus)
{
    TLOGI(WmsLogTag::DMS, "SendSensorResult FoldStatus: %{public}d", foldStatus);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    if (displayMode != currentDisplayMode_) {
        ChangeScreenDisplayMode(displayMode);
    }
}

sptr<FoldCreaseRegion> DualDisplayFoldPolicy::GetCurrentFoldCreaseRegion()
{
    TLOGI(WmsLogTag::DMS, "GetCurrentFoldCreaseRegion");
    return currentFoldCreaseRegion_;
}

void DualDisplayFoldPolicy::LockDisplayStatus(bool locked)
{
    TLOGI(WmsLogTag::DMS, "LockDisplayStatus locked: %{public}d", locked);
    lockDisplayStatus_ = locked;
}

void DualDisplayFoldPolicy::SetOnBootAnimation(bool onBootAnimation)
{
    TLOGI(WmsLogTag::DMS, "SetOnBootAnimation onBootAnimation: %{public}d", onBootAnimation);
    onBootAnimation_ = onBootAnimation;
    if (!onBootAnimation_) {
        TLOGI(WmsLogTag::DMS, "SetOnBootAnimation when boot animation finished, change display mode");
        RecoverWhenBootAnimationExit();
    }
}

void DualDisplayFoldPolicy::RecoverWhenBootAnimationExit()
{
    TLOGI(WmsLogTag::DMS, "RecoverWhenBootAnimationExit currentScreen(%{public}" PRIu64 ")", screenId_);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    if (currentDisplayMode_ != displayMode) {
        ChangeScreenDisplayMode(displayMode);
    } else {
        TriggerScreenDisplayModeUpdate(displayMode);
    }
}

void DualDisplayFoldPolicy::TriggerScreenDisplayModeUpdate(FoldDisplayMode displayMode)
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

void DualDisplayFoldPolicy::UpdateForPhyScreenPropertyChange()
{
    TLOGI(WmsLogTag::DMS, "UpdateForPhyScreenPropertyChange currentScreen(%{public}" PRIu64 ")", screenId_);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    if (currentDisplayMode_ != displayMode) {
        ChangeScreenDisplayMode(displayMode);
    }
}

FoldDisplayMode DualDisplayFoldPolicy::GetModeMatchStatus()
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

void DualDisplayFoldPolicy::ReportFoldDisplayModeChange(FoldDisplayMode displayMode)
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

void DualDisplayFoldPolicy::ReportFoldStatusChangeBegin(int32_t offScreen, int32_t onScreen)
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

void DualDisplayFoldPolicy::ChangeScreenDisplayModeInner(sptr<ScreenSession> screenSession, ScreenId offScreenId,
    ScreenId onScreenId)
{
    if (onBootAnimation_) {
        ChangeScreenDisplayModeOnBootAnimation(screenSession, onScreenId);
        AddOrRemoveDisplayNodeToTree(offScreenId, REMOVE_DISPLAY_NODE);
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
    bool isScreenOn = PowerMgr::PowerMgrClient::GetInstance().IsFoldScreenOn();
    TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToCoordination, isScreenOn= %{public}d", isScreenOn);
    auto taskScreenOff = [=] {
        TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayMode: off screenId: %{public}" PRIu64 "", offScreenId);
        screenId_ = offScreenId;
        ScreenSessionManager::GetInstance().SetKeyguardDrawnDoneFlag(false);
        ScreenSessionManager::GetInstance().SetScreenPowerForFold(ScreenPowerStatus::POWER_STATUS_OFF);
    };
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOff, "screenOffTask");
    AddOrRemoveDisplayNodeToTree(offScreenId, REMOVE_DISPLAY_NODE);

    auto taskScreenOn = [=] {
        TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayMode: on screenId: %{public}" PRIu64 "", onScreenId);
        screenId_ = onScreenId;
        if (isScreenOn) {
            ScreenSessionManager::GetInstance().SetKeyguardDrawnDoneFlag(false);
            ScreenSessionManager::GetInstance().SetScreenPowerForFold(ScreenPowerStatus::POWER_STATUS_ON);
            PowerMgr::PowerMgrClient::GetInstance().RefreshActivity();
        } else {
            PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
        }
    };
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOn, "screenOnTask");
    AddOrRemoveDisplayNodeToTree(onScreenId, ADD_DISPLAY_NODE);
    SendPropertyChangeResult(screenSession, onScreenId, reason);
}

void DualDisplayFoldPolicy::ChangeScreenDisplayModeToCoordination()
{
    bool isScreenOn = PowerMgr::PowerMgrClient::GetInstance().IsFoldScreenOn();
    TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToCoordination, isScreenOn= %{public}d", isScreenOn);
#ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, MAIN_TP.c_str());
#endif
    // on main screen
    auto taskScreenOnMain = [=] {
        TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayMode: on main screenId");
        screenId_ = SCREEN_ID_MAIN;
        if (isScreenOn) {
            ScreenSessionManager::GetInstance().SetKeyguardDrawnDoneFlag(false);
            ScreenSessionManager::GetInstance().SetScreenPower(ScreenPowerStatus::POWER_STATUS_ON,
                PowerStateChangeReason::STATE_CHANGE_REASON_DISPLAY_SWITCH);
            PowerMgr::PowerMgrClient::GetInstance().RefreshActivity();
        } else {
            PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
        }
    };
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnMain, "taskScreenOnMain");
    // on sub screen
    auto taskScreenOnSub = [=] {
        TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayMode: on sub screenId");
        if (isScreenOn) {
            ScreenSessionManager::GetInstance().SetKeyguardDrawnDoneFlag(false);
            ScreenSessionManager::GetInstance().SetScreenPowerForFold(SCREEN_ID_SUB,
                ScreenPowerStatus::POWER_STATUS_ON);
        }
    };
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnSub, "taskScreenOnSub");
    AddOrRemoveDisplayNodeToTree(SCREEN_ID_SUB, ADD_DISPLAY_NODE);
}

void DualDisplayFoldPolicy::SendPropertyChangeResult(sptr<ScreenSession> screenSession, ScreenId screenId,
    ScreenPropertyChangeReason reason)
{
    std::lock_guard<std::recursive_mutex> lock_info(displayInfoMutex_);
    screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(screenId);
    screenSession->UpdatePropertyByFoldControl(screenProperty_);
    screenSession->PropertyChange(screenSession->GetScreenProperty(), reason);
    TLOGI(WmsLogTag::DMS, "screenBounds : width_= %{public}f, height_= %{public}f",
        screenSession->GetScreenProperty().GetBounds().rect_.width_,
        screenSession->GetScreenProperty().GetBounds().rect_.height_);
    ScreenSessionManager::GetInstance().NotifyDisplayChanged(screenSession->ConvertToDisplayInfo(),
        DisplayChangeEvent::DISPLAY_SIZE_CHANGED);
}

void DualDisplayFoldPolicy::ChangeScreenDisplayModeOnBootAnimation(sptr<ScreenSession> screenSession, ScreenId screenId)
{
    TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToFullOnBootAnimation");
    screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(screenId);
    ScreenPropertyChangeReason reason = ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND;
    if (screenId == SCREEN_ID_SUB) {
        reason = ScreenPropertyChangeReason::FOLD_SCREEN_FOLDING;
    }
    screenSession->UpdatePropertyByFoldControl(screenProperty_);
    screenSession->PropertyChange(screenSession->GetScreenProperty(), reason);
    TLOGI(WmsLogTag::DMS, "screenBounds : width_= %{public}f, height_= %{public}f",
        screenSession->GetScreenProperty().GetBounds().rect_.width_,
        screenSession->GetScreenProperty().GetBounds().rect_.height_);
    screenId_ = screenId;
}

void DualDisplayFoldPolicy::AddOrRemoveDisplayNodeToTree(ScreenId screenId, int32_t command)
{
    TLOGI(WmsLogTag::DMS, "AddOrRemoveDisplayNodeToTree, screenId: %{public}" PRIu64 ", command: %{public}d",
        screenId, command);
    sptr<ScreenSession> screenSession = ScreenSessionManager::GetInstance().GetScreenSession(screenId);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "AddOrRemoveDisplayNodeToTree, screenSession is null");
        return;
    }
    std::shared_ptr<RSDisplayNode> displayNode = screenSession->GetDisplayNode();
    if (displayNode == nullptr) {
        TLOGE(WmsLogTag::DMS, "AddOrRemoveDisplayNodeToTree, displayNode is null");
        return;
    }
    if (command == ADD_DISPLAY_NODE) {
        displayNode->AddDisplayNodeToTree();
    } else if (command == REMOVE_DISPLAY_NODE) {
        displayNode->RemoveDisplayNodeFromTree();
    }
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        TLOGI(WmsLogTag::DMS, "add or remove displayNode");
        transactionProxy->FlushImplicitTransaction();
    }
}

void DualDisplayFoldPolicy::ExitCoordination()
{
    ScreenSessionManager::GetInstance().SetScreenPowerForFold(SCREEN_ID_SUB,
        ScreenPowerStatus::POWER_STATUS_OFF);
    AddOrRemoveDisplayNodeToTree(SCREEN_ID_SUB, REMOVE_DISPLAY_NODE);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    currentDisplayMode_ = displayMode;
    globalDisplayMode_ = displayMode;
    TLOGI(WmsLogTag::DMS, "CurrentDisplayMode:%{public}d", displayMode);
}

} // namespace OHOS::Rosen