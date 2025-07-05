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
#include "rs_adapter.h"
#include "session/screen/include/screen_session.h"
#include "screen_session_manager.h"

#include "window_manager_hilog.h"
#include "parameters.h"
#include "fold_screen_state_internel.h"

#ifdef POWER_MANAGER_ENABLE
#include <power_mgr_client.h>
#endif

namespace OHOS::Rosen {
namespace {
const ScreenId SCREEN_ID_MAIN = 0;
const ScreenId SCREEN_ID_SUB = 5;
const bool IS_COORDINATION_SUPPORT =
    OHOS::system::GetBoolParameter("const.window.foldabledevice.is_coordination_support", false);
const std::string g_FoldScreenRect = system::GetParameter("const.display.foldscreen.crease_region", "");
const int32_t FOLD_CREASE_RECT_SIZE = 4;
#ifdef TP_FEATURE_ENABLE
const int32_t TP_TYPE = 12;
#endif
const std::string MAIN_TP = "0";
const std::string SUB_TP = "1";
const int32_t REMOVE_DISPLAY_NODE = 0;
const int32_t ADD_DISPLAY_NODE = 1;
const uint32_t CHANGE_MODE_TASK_NUM = 3;
constexpr float VERTICAL_ROTATION = 0.0F;
constexpr float VERTICAL_ROTATION_REVERSE = 180.0F;
} // namespace

DualDisplayFoldPolicy::DualDisplayFoldPolicy(std::recursive_mutex& displayInfoMutex,
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler): screenPowerTaskScheduler_(screenPowerTaskScheduler)
{
    TLOGI(WmsLogTag::DMS, "DualDisplayFoldPolicy created");

    ScreenId screenIdMain = 0;
    int32_t foldCreaseRegionPosX = 0;
    int32_t foldCreaseRegionPosY = 1256;
    int32_t foldCreaseRegionPosWidth = 1136;
    int32_t foldCreaseRegionPosHeight = 184;

    std::vector<DMRect> rect = {
        {
            foldCreaseRegionPosX, foldCreaseRegionPosY,
            foldCreaseRegionPosWidth, foldCreaseRegionPosHeight
        }
    };
    currentFoldCreaseRegion_ = new FoldCreaseRegion(screenIdMain, rect);
    liveCreaseRegion_ = GetVerticalFoldCreaseRect();
}

sptr<FoldCreaseRegion> DualDisplayFoldPolicy::GetHorizontalFoldCreaseRect()
{
    std::vector<int32_t> foldRect = FoldScreenStateInternel::StringFoldRectSplitToInt(g_FoldScreenRect, ",;");
    if (foldRect.size() != FOLD_CREASE_RECT_SIZE) {
        // ccm numbers of parameter on the current device is 4
        TLOGE(WmsLogTag::DMS, "foldRect is invalid");
        return nullptr;
    }

    ScreenId screenIdMain = 0;
    int32_t liveCreaseRegionPosX = foldRect[0]; // ccm PosY
    int32_t liveCreaseRegionPosY = foldRect[1]; // ccm PosY
    int32_t liveCreaseRegionPosWidth = foldRect[2]; // ccm PosWidth
    int32_t liveCreaseRegionPosHeight = foldRect[3]; // ccm PosHeight

    std::vector<DMRect> foldCreaseRect = {
        {
            liveCreaseRegionPosX, liveCreaseRegionPosY,
            liveCreaseRegionPosWidth, liveCreaseRegionPosHeight
        }
    };
    return new FoldCreaseRegion(screenIdMain, foldCreaseRect);
}

sptr<FoldCreaseRegion> DualDisplayFoldPolicy::GetVerticalFoldCreaseRect()
{
    std::vector<int32_t> foldRect = FoldScreenStateInternel::StringFoldRectSplitToInt(g_FoldScreenRect, ",;");
    if (foldRect.size() != FOLD_CREASE_RECT_SIZE) {
        // ccm numbers of parameter on the current device is 4
        TLOGE(WmsLogTag::DMS, "foldRect is invalid");
        return nullptr;
    }

    ScreenId screenIdMain = 0;
    int32_t liveCreaseRegionPosX = foldRect[1]; // ccm PosY
    int32_t liveCreaseRegionPosY = foldRect[0]; // ccm PosY
    int32_t liveCreaseRegionPosWidth = foldRect[3]; // ccm PosWidth
    int32_t liveCreaseRegionPosHeight = foldRect[2]; // ccm PosHeight

    std::vector<DMRect> foldCreaseRect = {
        {
            liveCreaseRegionPosX, liveCreaseRegionPosY,
            liveCreaseRegionPosWidth, liveCreaseRegionPosHeight
        }
    };
    return new FoldCreaseRegion(screenIdMain, foldCreaseRect);
}

void DualDisplayFoldPolicy::SetdisplayModeChangeStatus(bool status, bool isOnBootAnimation)
{
    if (status) {
        pengdingTask_ = CHANGE_MODE_TASK_NUM;
        startTimePoint_ = std::chrono::steady_clock::now();
        displayModeChangeRunning_ = status;
    } else {
        pengdingTask_ --;
        if (pengdingTask_ != 0) {
            return;
        }
        displayModeChangeRunning_ = false;
        endTimePoint_ = std::chrono::steady_clock::now();
        if (lastCachedisplayMode_.load() != GetScreenDisplayMode()) {
            TLOGI(WmsLogTag::DMS, "start change displaymode to lastest mode");
            ChangeScreenDisplayMode(lastCachedisplayMode_.load());
        }
    }
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
    return true;
}

ScreenId DualDisplayFoldPolicy::GetScreenIdByDisplayMode(FoldDisplayMode displayMode)
{
    ScreenId screenId = SCREEN_ID_MAIN;
    if (displayMode == FoldDisplayMode::SUB) {
        screenId = SCREEN_ID_SUB;
    }
    return screenId;
}

void DualDisplayFoldPolicy::ChangeScreenDisplayMode(FoldDisplayMode displayMode, DisplayModeChangeReason reason)
{
    SetLastCacheDisplayMode(displayMode);
    if (GetModeChangeRunningStatus()) {
        TLOGW(WmsLogTag::DMS, "last process not complete, skip mode: %{public}d", displayMode);
        return;
    }
    TLOGI(WmsLogTag::DMS, "start change displaymode: %{public}d, lastElapsedMs: %{public}" PRId64 "ms",
        displayMode, getFoldingElapsedMs());
    ScreenId screenId = GetScreenIdByDisplayMode(displayMode);
    sptr<ScreenSession> screenSession = ScreenSessionManager::GetInstance().GetScreenSession(screenId);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "default screenSession is null");
        return;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:ChangeScreenDisplayMode(displayMode= %" PRIu64")", displayMode);
    {
        std::lock_guard<std::recursive_mutex> lock_mode(displayModeMutex_);
        if (!CheckDisplayMode(displayMode)) {
            return;
        }
    }
    SetdisplayModeChangeStatus(true);
    {
        std::lock_guard<std::recursive_mutex> lock_mode(displayModeMutex_);
        lastDisplayMode_ = displayMode;
    }
    ReportFoldDisplayModeChange(displayMode);
    ScreenSessionManager::GetInstance().SwitchScrollParam(displayMode);
    ChangeScreenDisplayModeProc(screenSession, displayMode);
    {
        std::lock_guard<std::recursive_mutex> lock_mode(displayModeMutex_);
        currentDisplayMode_ = displayMode;
    }
    ScreenSessionManager::GetInstance().NotifyDisplayModeChanged(displayMode);
    SetdisplayModeChangeStatus(false);
}

void DualDisplayFoldPolicy::ChangeScreenDisplayModeProc(sptr<ScreenSession> screenSession,
    FoldDisplayMode displayMode)
{
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
        default: {
            break;
        }
    }
}

void DualDisplayFoldPolicy::SendSensorResult(FoldStatus foldStatus)
{
    TLOGI(WmsLogTag::DMS, "FoldStatus: %{public}d", foldStatus);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    bool isScreenOn = PowerMgr::PowerMgrClient::GetInstance().IsFoldScreenOn();
    if (currentDisplayMode_ == FoldDisplayMode::COORDINATION && isScreenOn &&
        displayMode == FoldDisplayMode::MAIN) {
        TLOGI(WmsLogTag::DMS, "CurrentDisplayMode is coordination, HalfFold no need to change displaympde");
        return;
    }
    ChangeScreenDisplayMode(displayMode);
}

sptr<FoldCreaseRegion> DualDisplayFoldPolicy::GetCurrentFoldCreaseRegion()
{
    TLOGI(WmsLogTag::DMS, "GetCurrentFoldCreaseRegion");
    return currentFoldCreaseRegion_;
}

sptr<FoldCreaseRegion> DualDisplayFoldPolicy::GetLiveCreaseRegion()
{
    TLOGI(WmsLogTag::DMS, "enter");
    FoldDisplayMode displayMode = GetScreenDisplayMode();
    if (displayMode == FoldDisplayMode::UNKNOWN || displayMode == FoldDisplayMode::SUB) {
        return nullptr;
    }
    sptr<ScreenSession> screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_MAIN);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "default screenSession is null");
        return nullptr;
    }
    DisplayOrientation displayOrientation = screenSession->GetScreenProperty().GetDisplayOrientation();
    if (displayMode == FoldDisplayMode::MAIN || displayMode == FoldDisplayMode::COORDINATION) {
        switch (displayOrientation) {
            case DisplayOrientation::PORTRAIT:
            case DisplayOrientation::PORTRAIT_INVERTED: {
                liveCreaseRegion_ = GetHorizontalFoldCreaseRect();
                break;
            }
            case DisplayOrientation::LANDSCAPE:
            case DisplayOrientation::LANDSCAPE_INVERTED: {
                liveCreaseRegion_ = GetVerticalFoldCreaseRect();
                break;
            }
            default: {
                TLOGE(WmsLogTag::DMS, "displayOrientation is invalid");
            }
        }
    }
    return liveCreaseRegion_;
}

void DualDisplayFoldPolicy::LockDisplayStatus(bool locked)
{
    TLOGI(WmsLogTag::DMS, "locked: %{public}d", locked);
    lockDisplayStatus_ = locked;
}

void DualDisplayFoldPolicy::SetOnBootAnimation(bool onBootAnimation)
{
    TLOGI(WmsLogTag::DMS, "onBootAnimation: %{public}d", onBootAnimation);
    onBootAnimation_ = onBootAnimation;
    if (!onBootAnimation_) {
        TLOGI(WmsLogTag::DMS, "when boot animation finished, change display mode");
        RecoverWhenBootAnimationExit();
    }
}

void DualDisplayFoldPolicy::RecoverWhenBootAnimationExit()
{
    TLOGI(WmsLogTag::DMS, "CurrentScreen(%{public}" PRIu64 ")", screenId_);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    if (currentDisplayMode_ != displayMode) {
        ChangeScreenDisplayMode(displayMode);
    } else {
        TriggerScreenDisplayModeUpdate(displayMode);
    }
}

void DualDisplayFoldPolicy::TriggerScreenDisplayModeUpdate(FoldDisplayMode displayMode)
{
    TLOGI(WmsLogTag::DMS, "displayMode = %{public}d", displayMode);
    sptr<ScreenSession> screenSession = nullptr;
    if (displayMode == FoldDisplayMode::SUB) {
        screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_SUB);
    } else {
        screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_MAIN);
    }
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "default screenSession is null");
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
            TLOGI(WmsLogTag::DMS, "displayMode is unknown");
            break;
        }
        default: {
            TLOGI(WmsLogTag::DMS, "displayMode is invalid");
            break;
        }
    }
}

void DualDisplayFoldPolicy::UpdateForPhyScreenPropertyChange()
{
    TLOGI(WmsLogTag::DMS, "currentScreen(%{public}" PRIu64 ")", screenId_);
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
            TLOGI(WmsLogTag::DMS, "FoldStatus is invalid");
        }
    }
    return displayMode;
}

void DualDisplayFoldPolicy::ReportFoldDisplayModeChange(FoldDisplayMode displayMode)
{
    int32_t mode = static_cast<int32_t>(displayMode);
    TLOGI(WmsLogTag::DMS, "displayMode: %{public}d", mode);
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
        "DISPLAY_MODE",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "FOLD_DISPLAY_MODE", mode);
    if (ret != 0) {
        TLOGE(WmsLogTag::DMS, "Write HiSysEvent error, ret: %{public}d", ret);
    }
}

void DualDisplayFoldPolicy::ReportFoldStatusChangeBegin(int32_t offScreen, int32_t onScreen)
{
    TLOGI(WmsLogTag::DMS, "offScreen: %{public}d, onScreen: %{public}d",
        offScreen, onScreen);
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
        "FOLD_STATE_CHANGE_BEGIN",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "POWER_OFF_SCREEN", offScreen,
        "POWER_ON_SCREEN", onScreen);
    if (ret != 0) {
        TLOGE(WmsLogTag::DMS, "Write HiSysEvent error, ret: %{public}d", ret);
    }
}

void DualDisplayFoldPolicy::ChangeScreenDisplayModeInner(sptr<ScreenSession> screenSession, ScreenId offScreenId,
    ScreenId onScreenId)
{
    if (onBootAnimation_) {
        ChangeScreenDisplayModeOnBootAnimation(screenSession, onScreenId);
        return;
    }
    std::string tp = MAIN_TP;
    if (onScreenId == SCREEN_ID_SUB) {
        tp = SUB_TP;
    }
#ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, tp.c_str());
#endif
    ReportFoldStatusChangeBegin((int32_t)SCREEN_ID_MAIN, (int32_t)SCREEN_ID_SUB);
    bool isScreenOn = PowerMgr::PowerMgrClient::GetInstance().IsFoldScreenOn();
    TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToCoordination, isScreenOn= %{public}d", isScreenOn);
    auto taskScreenOff = [=] {
        TLOGNI(WmsLogTag::DMS, "ChangeScreenDisplayMode: off screenId: %{public}" PRIu64 "", offScreenId);
        screenId_ = offScreenId;
        ScreenSessionManager::GetInstance().SetKeyguardDrawnDoneFlag(false);
        ScreenSessionManager::GetInstance().SetScreenPowerForFold(ScreenPowerStatus::POWER_STATUS_OFF);
        SetdisplayModeChangeStatus(false);
    };
    if (screenPowerTaskScheduler_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "screenPowerTaskScheduler_ is nullpter");
        return;
    }
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOff, "screenOffTask");
    AddOrRemoveDisplayNodeToTree(offScreenId, REMOVE_DISPLAY_NODE);

    auto taskScreenOn = [=] {
        TLOGNI(WmsLogTag::DMS, "ChangeScreenDisplayMode: on screenId: %{public}" PRIu64 "", onScreenId);
        screenId_ = onScreenId;
        if (isScreenOn) {
            ScreenSessionManager::GetInstance().SetKeyguardDrawnDoneFlag(false);
            ScreenSessionManager::GetInstance().SetScreenPowerForFold(ScreenPowerStatus::POWER_STATUS_ON);
        } else {
            PowerMgr::PowerMgrClient::GetInstance().WakeupDeviceAsync();
        }
        SetdisplayModeChangeStatus(false);
    };
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOn, "screenOnTask");
    AddOrRemoveDisplayNodeToTree(onScreenId, ADD_DISPLAY_NODE);
    TriggerSensorInSub(screenSession);
}

void DualDisplayFoldPolicy::ChangeScreenDisplayModeToCoordination()
{
    bool isScreenOn = PowerMgr::PowerMgrClient::GetInstance().IsFoldScreenOn();
    TLOGI(WmsLogTag::DMS, "isScreenOn= %{public}d", isScreenOn);
#ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, MAIN_TP.c_str());
#endif
    // on main screen
    auto taskScreenOnMain = [=] {
        TLOGNI(WmsLogTag::DMS, "ChangeScreenDisplayMode: on main screenId");
        screenId_ = SCREEN_ID_MAIN;
        if (isScreenOn) {
            ScreenSessionManager::GetInstance().SetKeyguardDrawnDoneFlag(false);
            ScreenSessionManager::GetInstance().SetScreenPower(ScreenPowerStatus::POWER_STATUS_ON,
                PowerStateChangeReason::STATE_CHANGE_REASON_DISPLAY_SWITCH);
        } else {
            PowerMgr::PowerMgrClient::GetInstance().WakeupDeviceAsync();
        }
        SetdisplayModeChangeStatus(false);
    };
    if (screenPowerTaskScheduler_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "screenPowerTaskScheduler_ is nullpter");
        return;
    }
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnMain, "taskScreenOnMain");
    // on sub screen
    auto taskScreenOnSub = [=] {
        TLOGNI(WmsLogTag::DMS, "ChangeScreenDisplayMode: on sub screenId");
        if (isScreenOn) {
            ScreenSessionManager::GetInstance().SetKeyguardDrawnDoneFlag(false);
            ScreenSessionManager::GetInstance().SetScreenPowerForFold(SCREEN_ID_SUB,
                ScreenPowerStatus::POWER_STATUS_ON);
        }
        SetdisplayModeChangeStatus(false);
    };
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnSub, "taskScreenOnSub");
    AddOrRemoveDisplayNodeToTree(SCREEN_ID_SUB, ADD_DISPLAY_NODE);
}

void DualDisplayFoldPolicy::ChangeScreenDisplayModeOnBootAnimation(sptr<ScreenSession> screenSession, ScreenId screenId)
{
    TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToFullOnBootAnimation");
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "ScreenSession is nullpter");
        return;
    }
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
    TLOGI(WmsLogTag::DMS, "screenId: %{public}" PRIu64 ", command: %{public}d",
        screenId, command);
    sptr<ScreenSession> screenSession = ScreenSessionManager::GetInstance().GetScreenSession(screenId);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "screenSession is null");
        return;
    }
    std::shared_ptr<RSDisplayNode> displayNode = screenSession->GetDisplayNode();
    if (displayNode == nullptr) {
        TLOGE(WmsLogTag::DMS, "displayNode is null");
        return;
    }
    if (command == ADD_DISPLAY_NODE) {
        displayNode->AddDisplayNodeToTree();
    } else if (command == REMOVE_DISPLAY_NODE) {
        displayNode->RemoveDisplayNodeFromTree();
    }
    TLOGI(WmsLogTag::DMS, "add or remove displayNode");
    RSTransactionAdapter::FlushImplicitTransaction(screenSession->GetRSUIContext());
}

void DualDisplayFoldPolicy::ExitCoordination()
{
    ScreenSessionManager::GetInstance().SetScreenPowerForFold(SCREEN_ID_SUB,
        ScreenPowerStatus::POWER_STATUS_OFF);
    AddOrRemoveDisplayNodeToTree(SCREEN_ID_SUB, REMOVE_DISPLAY_NODE);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    currentDisplayMode_ = displayMode;
    lastDisplayMode_ = displayMode;
    TLOGI(WmsLogTag::DMS, "CurrentDisplayMode:%{public}d", displayMode);
    ScreenSessionManager::GetInstance().NotifyDisplayModeChanged(displayMode);
}

void DualDisplayFoldPolicy::ChangeOnTentMode(FoldStatus currentState)
{
    TLOGI(WmsLogTag::DMS, "Enter tent mode, current state:%{public}d, change display mode to MAIN", currentState);
    if (currentState == FoldStatus::EXPAND || currentState == FoldStatus::HALF_FOLD) {
        ChangeScreenDisplayMode(FoldDisplayMode::SUB);
    } else if (currentState == FoldStatus::FOLDED) {
        ChangeScreenDisplayMode(FoldDisplayMode::SUB);
        PowerMgr::PowerMgrClient::GetInstance().WakeupDeviceAsync();
    } else {
        TLOGE(WmsLogTag::DMS, "current state:%{public}d invalid", currentState);
    }
}
 
void DualDisplayFoldPolicy::ChangeOffTentMode()
{
    PowerMgr::PowerMgrClient::GetInstance().WakeupDeviceAsync();
    FoldDisplayMode displayMode = GetModeMatchStatus();
    TLOGW(WmsLogTag::DMS, "CurrentDisplayMode:%{public}d, CurrentFoldStatus:%{public}d",
        currentDisplayMode_, currentFoldStatus_);
    ChangeScreenDisplayMode(displayMode);
}

void DualDisplayFoldPolicy::TriggerSensorInSub(const sptr<ScreenSession>& screenSession)
{
    if (screenSession->GetScreenId() != SCREEN_ID_SUB) {
        return;
    }
    sptr<ScreenSession> screenSessionMain = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_MAIN);
    if (screenSessionMain == nullptr) {
        TLOGE(WmsLogTag::DMS, "screenSessionMain is null");
        return;
    }
    float sensorRotation = screenSessionMain->GetSensorRotation();
    TLOGI(WmsLogTag::DMS, "sensorRotation = %{public}f", sensorRotation);
    if (FoldScreenStateInternel::FloatEqualAbs(sensorRotation, VERTICAL_ROTATION) ||
        FoldScreenStateInternel::FloatEqualAbs(sensorRotation, VERTICAL_ROTATION_REVERSE)) {
        screenSession->HandleSensorRotation(sensorRotation);
    }
}
} // namespace OHOS::Rosen