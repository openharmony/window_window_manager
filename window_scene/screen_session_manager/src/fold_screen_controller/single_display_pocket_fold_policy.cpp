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
#include "fold_screen_controller/single_display_pocket_fold_policy.h"
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

const int32_t REMOVE_DISPLAY_NODE = 0;
const int32_t ADD_DISPLAY_NODE = 1;

#ifdef TP_FEATURE_ENABLE
const int32_t TP_TYPE = 12;
const int32_t TP_TYPE_POWER_CTRL = 18;
const std::string MAIN_TP = "0";
const std::string SUB_TP = "1";
const std::string SUB_TP_OFF = "1,1";
#endif
} // namespace

SingleDisplayPocketFoldPolicy::SingleDisplayPocketFoldPolicy(std::recursive_mutex& displayInfoMutex,
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler)
    : displayInfoMutex_(displayInfoMutex), screenPowerTaskScheduler_(screenPowerTaskScheduler)
{
    TLOGI(WmsLogTag::DMS, "SingleDisplayPocketFoldPolicy created");

    ScreenId screenIdMain = 0;
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
    currentFoldCreaseRegion_ = new FoldCreaseRegion(screenIdMain, rect);
}

void SingleDisplayPocketFoldPolicy::SetdisplayModeChangeStatus(bool status)
{
    if (status) {
        pengdingTask_ = FOLD_TO_EXPAND_TASK_NUM;
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
            ScreenSessionManager::GetInstance().TriggerDisplayModeUpdate(lastCachedisplayMode_.load());
        }
    }
}

void SingleDisplayPocketFoldPolicy::ChangeScreenDisplayMode(FoldDisplayMode displayMode,
    DisplayModeChangeReason reason)
{
    SetLastCacheDisplayMode(displayMode);
    if (GetModeChangeRunningStatus()) {
        TLOGW(WmsLogTag::DMS, "last process not complete, skip mode: %{public}d", displayMode);
        return;
    }
    TLOGI(WmsLogTag::DMS, "start change displaymode: %{public}d, lastElapsedMs: %{public}" PRId64 "ms",
        displayMode, getFoldingElapsedMs());
    
    sptr<ScreenSession> screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_MAIN);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "default screenSession is null");
        return;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:ChangeScreenDisplayMode(displayMode = %" PRIu64")", displayMode);
    {
        std::lock_guard<std::recursive_mutex> lock_mode(displayModeMutex_);
        if (currentDisplayMode_ == displayMode) {
            TLOGW(WmsLogTag::DMS, "ChangeScreenDisplayMode already in displayMode %{public}d", displayMode);
            return;
        }
    }
    ReportFoldDisplayModeChange(displayMode);
    ScreenSessionManager::GetInstance().SwitchScrollParam(displayMode);
    ChangeScreenDisplayModeProc(screenSession, displayMode);
    {
        std::lock_guard<std::recursive_mutex> lock_mode(displayModeMutex_);
        currentDisplayMode_ = displayMode;
        lastDisplayMode_ = displayMode;
    }
    ScreenSessionManager::GetInstance().NotifyDisplayModeChanged(displayMode);
}

void SingleDisplayPocketFoldPolicy::ChangeScreenDisplayModeProc(sptr<ScreenSession> screenSession,
    FoldDisplayMode displayMode)
{
    switch (displayMode) {
        case FoldDisplayMode::SUB: {
            if (currentDisplayMode_ == FoldDisplayMode::COORDINATION) {
                CloseCoordinationScreen();
            }
            ChangeScreenDisplayModeToSub(screenSession);
            break;
        }
        case FoldDisplayMode::MAIN: {
            if (currentDisplayMode_ == FoldDisplayMode::COORDINATION) {
                CloseCoordinationScreen();
            } else {
                ChangeScreenDisplayModeToMain(screenSession);
            }
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
}

void SingleDisplayPocketFoldPolicy::SendSensorResult(FoldStatus foldStatus)
{
    TLOGI(WmsLogTag::DMS, "SendSensorResult FoldStatus: %{public}d", foldStatus);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    if (!(currentDisplayMode_ == FoldDisplayMode::COORDINATION &&
        displayMode == FoldDisplayMode::MAIN)) {
        ChangeScreenDisplayMode(displayMode);
    }
}

sptr<FoldCreaseRegion> SingleDisplayPocketFoldPolicy::GetCurrentFoldCreaseRegion()
{
    TLOGI(WmsLogTag::DMS, "GetCurrentFoldCreaseRegion");
    return currentFoldCreaseRegion_;
}

void SingleDisplayPocketFoldPolicy::LockDisplayStatus(bool locked)
{
    TLOGI(WmsLogTag::DMS, "LockDisplayStatus locked: %{public}d", locked);
    lockDisplayStatus_ = locked;
}

void SingleDisplayPocketFoldPolicy::SetOnBootAnimation(bool onBootAnimation)
{
    TLOGI(WmsLogTag::DMS, "SetOnBootAnimation onBootAnimation: %{public}d", onBootAnimation);
    onBootAnimation_ = onBootAnimation;
    if (!onBootAnimation_) {
        TLOGI(WmsLogTag::DMS, "SetOnBootAnimation when boot animation finished, change display mode");
        RecoverWhenBootAnimationExit();
        NotifyRefreshRateEvent(false);
    } else {
        NotifyRefreshRateEvent(true);
    }
}

void SingleDisplayPocketFoldPolicy::RecoverWhenBootAnimationExit()
{
    TLOGI(WmsLogTag::DMS, "CurrentScreen(%{public}" PRIu64 ")", screenId_);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    if (currentDisplayMode_ != displayMode) {
        ChangeScreenDisplayMode(displayMode);
    }
}

void SingleDisplayPocketFoldPolicy::UpdateForPhyScreenPropertyChange()
{
    TLOGI(WmsLogTag::DMS, "CurrentScreen(%{public}" PRIu64 ")", screenId_);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    if (currentDisplayMode_ != displayMode) {
        ChangeScreenDisplayMode(displayMode);
    }
}

FoldDisplayMode SingleDisplayPocketFoldPolicy::GetModeMatchStatus()
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

void SingleDisplayPocketFoldPolicy::ReportFoldDisplayModeChange(FoldDisplayMode displayMode)
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

void SingleDisplayPocketFoldPolicy::ReportFoldStatusChangeBegin(int32_t offScreen, int32_t onScreen)
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

void SingleDisplayPocketFoldPolicy::ChangeScreenDisplayModeToSubWhenFoldScreenOn(sptr<ScreenSession> screenSession)
{
    TLOGI(WmsLogTag::DMS, "IsFoldScreenOn is true, begin.");
    ReportFoldStatusChangeBegin(static_cast<int32_t>(SCREEN_ID_MAIN),
        static_cast<int32_t>(SCREEN_ID_SUB));
    auto taskScreenOnSub = [=] {
        // off main screen
        TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToSub: IsFoldScreenOn is true, screenIdMain OFF.");
        screenId_ = SCREEN_ID_MAIN;
        ChangeScreenDisplayModePower(SCREEN_ID_MAIN, ScreenPowerStatus::POWER_STATUS_OFF);
        SetdisplayModeChangeStatus(false);

        // on sub screen
        TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToSub: IsFoldScreenOn is true, screenIdSub ON.");
        screenId_ = SCREEN_ID_SUB;
        ChangeScreenDisplayModePower(SCREEN_ID_SUB, ScreenPowerStatus::POWER_STATUS_ON);
        SetdisplayModeChangeStatus(false);
    };
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnSub, "screenOnSubTask");
    SendPropertyChangeResult(screenSession, SCREEN_ID_SUB, ScreenPropertyChangeReason::FOLD_SCREEN_FOLDING);
}

void SingleDisplayPocketFoldPolicy::ChangeScreenDisplayModeToSubWhenFoldScreenOff(sptr<ScreenSession> screenSession)
{
    TLOGI(WmsLogTag::DMS, "IsFoldScreenOn is false, begin.");
    // off main screen
    auto taskScreenOffSubOff = [=] {
        TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToSub: IsFoldScreenOn is false, screenIdMain OFF.");
        screenId_ = SCREEN_ID_MAIN;
        ChangeScreenDisplayModePower(SCREEN_ID_MAIN, ScreenPowerStatus::POWER_STATUS_OFF);
        SetdisplayModeChangeStatus(false);
    };
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOffSubOff, "screenOffSubOffTask");
    SendPropertyChangeResult(screenSession, SCREEN_ID_SUB, ScreenPropertyChangeReason::FOLD_SCREEN_FOLDING);
    bool ifTentMode = ScreenSessionManager::GetInstance().GetTentMode();
    auto taskScreenOnSubChangeScreenId = [=] {
        TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToSub: IsFoldScreenOn is false, Change ScreenId to Sub.");
        screenId_ = SCREEN_ID_SUB;
#ifdef TP_FEATURE_ENABLE
        RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE_POWER_CTRL, SUB_TP_OFF.c_str());
#endif
        if (ifTentMode) {
            PowerMgr::PowerMgrClient::GetInstance().WakeupDeviceAsync();
        }
        SetdisplayModeChangeStatus(false);
    };
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnSubChangeScreenId, "taskScreenOnSubChangeScreenId");
}


void SingleDisplayPocketFoldPolicy::ChangeScreenDisplayModeToSub(sptr<ScreenSession> screenSession,
    DisplayModeChangeReason reason)
{
    SetdisplayModeChangeStatus(true);
    if (onBootAnimation_) {
        ChangeScreenDisplayModeToSubOnBootAnimation(screenSession);
        return;
    }
    RSInterfaces::GetInstance().SetScreenSwitching(true);
#ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, SUB_TP.c_str());
#endif
    if (PowerMgr::PowerMgrClient::GetInstance().IsFoldScreenOn()) {
        ChangeScreenDisplayModeToSubWhenFoldScreenOn(screenSession);
    } else { // When the screen is off and folded, it is not powered on
        ChangeScreenDisplayModeToSubWhenFoldScreenOff(screenSession);
    }
}

void SingleDisplayPocketFoldPolicy::ChangeScreenDisplayModeToMain(sptr<ScreenSession> screenSession,
    DisplayModeChangeReason reason)
{
    SetdisplayModeChangeStatus(true);
    if (onBootAnimation_) {
        ChangeScreenDisplayModeToMainOnBootAnimation(screenSession);
        return;
    }
    RSInterfaces::GetInstance().SetScreenSwitching(true);
    ReportFoldStatusChangeBegin((int32_t)SCREEN_ID_SUB, (int32_t)SCREEN_ID_MAIN);
#ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE, MAIN_TP.c_str());
#endif
    if (PowerMgr::PowerMgrClient::GetInstance().IsFoldScreenOn()) {
        TLOGI(WmsLogTag::DMS, "IsFoldScreenOn is true, begin.");
        auto taskScreenOnMain = [=] {
            // off sub screen
            TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToMain: IsFoldScreenOn is true, screenIdSub OFF.");
            screenId_ = SCREEN_ID_SUB;
            ChangeScreenDisplayModePower(SCREEN_ID_SUB, ScreenPowerStatus::POWER_STATUS_OFF);
            SetdisplayModeChangeStatus(false);

            // on main screen
            TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToMain: IsFoldScreenOn is true, screenIdMain ON.");
            screenId_ = SCREEN_ID_MAIN;
            ChangeScreenDisplayModePower(SCREEN_ID_MAIN, ScreenPowerStatus::POWER_STATUS_ON);
            SetdisplayModeChangeStatus(false);
        };
        screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnMain, "screenOnMainTask");
        SendPropertyChangeResult(screenSession, SCREEN_ID_MAIN, ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND);
    } else { //AOD scene
        TLOGI(WmsLogTag::DMS, "IsFoldScreenOn is false, begin.");
        // off sub screen
        auto taskScreenOffMainOff = [=] {
            TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToMain: IsFoldScreenOn is false, screenIdSub OFF.");
            screenId_ = SCREEN_ID_SUB;
            ChangeScreenDisplayModePower(SCREEN_ID_SUB, ScreenPowerStatus::POWER_STATUS_OFF);
            SetdisplayModeChangeStatus(false);
        };
        screenPowerTaskScheduler_->PostAsyncTask(taskScreenOffMainOff, "screenOffMainOffTask");
        SendPropertyChangeResult(screenSession, SCREEN_ID_MAIN, ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND);
        // on main screen
        auto taskScreenOnMainOn = [=] {
            TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToMain: IsFoldScreenOn is false, screenIdMain ON.");
            screenId_ = SCREEN_ID_MAIN;
            PowerMgr::PowerMgrClient::GetInstance().WakeupDeviceAsync();
            SetdisplayModeChangeStatus(false);
        };
        screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnMainOn, "screenOnMainOnTask");
    }
}

void SingleDisplayPocketFoldPolicy::ChangeScreenDisplayModePower(ScreenId screenId, ScreenPowerStatus screenPowerStatus)
{
    ScreenSessionManager::GetInstance().SetKeyguardDrawnDoneFlag(false);
    ScreenSessionManager::GetInstance().SetScreenPowerForFold(screenId, screenPowerStatus);
}

void SingleDisplayPocketFoldPolicy::SendPropertyChangeResult(sptr<ScreenSession> screenSession, ScreenId screenId,
    ScreenPropertyChangeReason reason)
{
    std::lock_guard<std::recursive_mutex> lock_info(displayInfoMutex_);
    screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(screenId);
    ScreenProperty property = screenSession->UpdatePropertyByFoldControl(screenProperty_);
    screenSession->PropertyChange(property, reason);
    screenSession->SetRotationAndScreenRotationOnly(Rotation::ROTATION_0);
    TLOGI(WmsLogTag::DMS, "screenBounds : width_= %{public}f, height_= %{public}f",
        screenSession->GetScreenProperty().GetBounds().rect_.width_,
        screenSession->GetScreenProperty().GetBounds().rect_.height_);
    ScreenSessionManager::GetInstance().NotifyDisplayChanged(screenSession->ConvertToDisplayInfo(),
        DisplayChangeEvent::DISPLAY_SIZE_CHANGED);
}

void SingleDisplayPocketFoldPolicy::ChangeScreenDisplayModeToSubOnBootAnimation(sptr<ScreenSession> screenSession)
{
    TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToSubOnBootAnimation");
    screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(SCREEN_ID_SUB);
    screenSession->UpdatePropertyByFoldControl(screenProperty_);
    screenSession->PropertyChange(screenSession->GetScreenProperty(),
        ScreenPropertyChangeReason::FOLD_SCREEN_FOLDING);
    TLOGI(WmsLogTag::DMS, "screenBounds : width_= %{public}f, height_= %{public}f",
        screenSession->GetScreenProperty().GetBounds().rect_.width_,
        screenSession->GetScreenProperty().GetBounds().rect_.height_);
    screenId_ = SCREEN_ID_SUB;
}

void SingleDisplayPocketFoldPolicy::ChangeScreenDisplayModeToMainOnBootAnimation(sptr<ScreenSession> screenSession)
{
    TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToMainOnBootAnimation");
    screenProperty_ = ScreenSessionManager::GetInstance().GetPhyScreenProperty(SCREEN_ID_MAIN);
    screenSession->UpdatePropertyByFoldControl(screenProperty_);
    screenSession->PropertyChange(screenSession->GetScreenProperty(),
        ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND);
    TLOGI(WmsLogTag::DMS, "screenBounds : width_= %{public}f, height_= %{public}f",
        screenSession->GetScreenProperty().GetBounds().rect_.width_,
        screenSession->GetScreenProperty().GetBounds().rect_.height_);
    screenId_ = SCREEN_ID_MAIN;
}

void SingleDisplayPocketFoldPolicy::BootAnimationFinishPowerInit()
{
    int64_t timeStamp = 50;
    if (RSInterfaces::GetInstance().GetActiveScreenId() == SCREEN_ID_MAIN) {
        // 同显切内屏：外屏下电
        TLOGI(WmsLogTag::DMS, "Fold Screen Power sub screen off.");
        RSInterfaces::GetInstance().SetScreenPowerStatus(SCREEN_ID_SUB, ScreenPowerStatus::POWER_STATUS_OFF);
    } else if (RSInterfaces::GetInstance().GetActiveScreenId() == SCREEN_ID_SUB) {
        // 同显切外屏：双屏都灭再外屏上电
        TLOGI(WmsLogTag::DMS, "Fold Screen Power all screen off.");
        RSInterfaces::GetInstance().SetScreenPowerStatus(SCREEN_ID_SUB, ScreenPowerStatus::POWER_STATUS_OFF);
        RSInterfaces::GetInstance().SetScreenPowerStatus(SCREEN_ID_MAIN, ScreenPowerStatus::POWER_STATUS_OFF);
        std::this_thread::sleep_for(std::chrono::milliseconds(timeStamp));
        TLOGI(WmsLogTag::DMS, "Fold Screen Power sub screen on.");
        RSInterfaces::GetInstance().SetScreenPowerStatus(SCREEN_ID_SUB, ScreenPowerStatus::POWER_STATUS_ON);
    } else {
        TLOGI(WmsLogTag::DMS, "Fold Screen Power Init, invalid active screen id");
    }
}

void SingleDisplayPocketFoldPolicy::ChangeOnTentMode(FoldStatus currentState)
{
    TLOGI(WmsLogTag::DMS, "Enter tent mode, current state:%{public}d, change display mode to SUB", currentState);
    if (currentState == FoldStatus::EXPAND || currentState == FoldStatus::HALF_FOLD) {
        ChangeScreenDisplayMode(FoldDisplayMode::SUB);
    } else if (currentState == FoldStatus::FOLDED) {
        RSInterfaces::GetInstance().SetScreenPowerStatus(SCREEN_ID_MAIN, ScreenPowerStatus::POWER_STATUS_OFF);
        ChangeScreenDisplayMode(FoldDisplayMode::SUB);
        RSInterfaces::GetInstance().SetScreenPowerStatus(SCREEN_ID_SUB, ScreenPowerStatus::POWER_STATUS_ON);
    } else {
        TLOGE(WmsLogTag::DMS, "current state:%{public}d invalid", currentState);
    }
}

void SingleDisplayPocketFoldPolicy::ChangeOffTentMode()
{
    FoldDisplayMode displayMode = GetModeMatchStatus();
    ChangeScreenDisplayMode(displayMode);
}

void SingleDisplayPocketFoldPolicy::AddOrRemoveDisplayNodeToTree(ScreenId screenId, int32_t command)
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
    displayNode = nullptr;
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        TLOGI(WmsLogTag::DMS, "add or remove displayNode");
        transactionProxy->FlushImplicitTransaction();
    }
}

void SingleDisplayPocketFoldPolicy::ChangeScreenDisplayModeToCoordination()
{
    TLOGI(WmsLogTag::DMS, "change displaymode to coordination current mode=%{public}d", currentDisplayMode_);
    
    ScreenSessionManager::GetInstance().SetCoordinationFlag(true);
    ScreenSessionManager::GetInstance().OnScreenChange(SCREEN_ID_SUB, ScreenEvent::CONNECTED);

    // on sub screen
    auto taskScreenOnSubOn = [=] {
        TLOGI(WmsLogTag::DMS, "ChangeScreenDisplayModeToCoordination: screenIdSub ON.");
        NotifyRefreshRateEvent(true);
        ChangeScreenDisplayModePower(SCREEN_ID_SUB, ScreenPowerStatus::POWER_STATUS_ON);
        PowerMgr::PowerMgrClient::GetInstance().RefreshActivity();
    };
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnSubOn, "ScreenToCoordinationTask");
    AddOrRemoveDisplayNodeToTree(SCREEN_ID_SUB, ADD_DISPLAY_NODE);
}

void SingleDisplayPocketFoldPolicy::CloseCoordinationScreen()
{
    TLOGI(WmsLogTag::DMS, "Close Coordination Screen current mode=%{public}d", currentDisplayMode_);
    
    // on sub screen
    auto taskScreenOnSubOFF = [=] {
        TLOGI(WmsLogTag::DMS, "CloseCoordinationScreen: screenIdSub OFF.");
        ChangeScreenDisplayModePower(SCREEN_ID_SUB, ScreenPowerStatus::POWER_STATUS_OFF);
        NotifyRefreshRateEvent(false);
    };
    screenPowerTaskScheduler_->PostAsyncTask(taskScreenOnSubOFF, "CloseCoordinationScreenTask");
    AddOrRemoveDisplayNodeToTree(SCREEN_ID_SUB, REMOVE_DISPLAY_NODE);

    ScreenSessionManager::GetInstance().OnScreenChange(SCREEN_ID_SUB, ScreenEvent::DISCONNECTED);
    ScreenSessionManager::GetInstance().SetCoordinationFlag(false);
}

void SingleDisplayPocketFoldPolicy::ExitCoordination()
{
    ChangeScreenDisplayModePower(SCREEN_ID_SUB, ScreenPowerStatus::POWER_STATUS_OFF);
    AddOrRemoveDisplayNodeToTree(SCREEN_ID_SUB, REMOVE_DISPLAY_NODE);
    ScreenSessionManager::GetInstance().OnScreenChange(SCREEN_ID_SUB, ScreenEvent::DISCONNECTED);
    ScreenSessionManager::GetInstance().SetCoordinationFlag(false);
    NotifyRefreshRateEvent(false);
    FoldDisplayMode displayMode = GetModeMatchStatus();
    currentDisplayMode_ = displayMode;
    lastDisplayMode_ = displayMode;
    TLOGI(WmsLogTag::DMS, "Exit coordination, current display mode:%{public}d", displayMode);
    ScreenSessionManager::GetInstance().NotifyDisplayModeChanged(displayMode);
}

void SingleDisplayPocketFoldPolicy::NotifyRefreshRateEvent(bool isEventStatus)
{
    EventInfo eventInfo = {
        .eventName = "VOTER_MULTISELFOWNEDSCREEN",
        .eventStatus = isEventStatus,
        .minRefreshRate = 60,
        .maxRefreshRate = 60,
    };
    RSInterfaces::GetInstance().NotifyRefreshRateEvent(eventInfo);
}
} // namespace OHOS::Rosen