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

#include <hitrace_meter.h>
#include <hisysevent.h>
#include "screen_session_manager/include/screen_session_manager.h"
#include "fold_screen_controller/super_fold_sensor_manager.h"
#include "window_manager_hilog.h"
#include "fold_screen_state_internel.h"
#include "fold_screen_controller/super_fold_state_manager.h"
#include "screen_setting_helper.h"
#include "parameter.h"

namespace OHOS {

namespace Rosen {

WM_IMPLEMENT_SINGLE_INSTANCE(SuperFoldStateManager)

namespace {
const std::string g_FoldScreenRect = system::GetParameter("const.window.foldscreen.config_rect", "");
const int32_t PARAM_NUMBER_MIN = 10;
#ifdef TP_FEATURE_ENABLE
const int32_t TP_TYPE = 12;
const char* KEYBOARD_ON_CONFIG = "version:3+main";
const char* KEYBOARD_OFF_CONFIG = "version:3+whole";
#endif
static bool isHalfScreen_ = false;
static bool isHalfScreenSwitchOn_ = false;
static bool isHalfFolded_ = false;
static bool isKeyboardOn_ = false;
const std::string BOOTEVENT_BOOT_COMPLETED = "bootevent.boot.completed";
}

void SuperFoldStateManager::DoAngleChangeFolded(SuperFoldStatusChangeEvents event)
{
    TLOGI(WmsLogTag::DMS, "enter DoAngleChangeFolded() func");
    isHalfFolded_ = false;
}

void SuperFoldStateManager::DoAngleChangeHalfFolded(SuperFoldStatusChangeEvents event)
{
    TLOGI(WmsLogTag::DMS, "enter DoAngleChangeHalfFolded() func");
     isHalfFolded_ = true;
     if (!isHalfScreenSwitchOn_) {
        TLOGI(WmsLogTag::DMS, "Half Screen Switch is off");
        return;
    }
    if (!ChangeScreenState(true)) {
        TLOGI(WmsLogTag::DMS, "change to half screen fail!");
    }
}

void SuperFoldStateManager::DoAngleChangeExpanded(SuperFoldStatusChangeEvents event)
{
    TLOGI(WmsLogTag::DMS, "enter DoAngleChangeExpanded() func");
    isHalfFolded_ = false;
    if (isKeyboardOn_) {
        TLOGI(WmsLogTag::DMS, "Keyboard On, no need to recover");
        return;
    }
    if (!ChangeScreenState(false)) {
        TLOGI(WmsLogTag::DMS, "recover from half screen fail!");
    }
}

void SuperFoldStateManager::DoKeyboardOn(SuperFoldStatusChangeEvents event)
{
    TLOGI(WmsLogTag::DMS, "enter DoKeyboardOn() func");
    isKeyboardOn_ = true;
    if (!ChangeScreenState(true)) {
        TLOGI(WmsLogTag::DMS, "change to half screen fail!");
    }
}

void SuperFoldStateManager::DoKeyboardOff(SuperFoldStatusChangeEvents event)
{
    TLOGI(WmsLogTag::DMS, "enter DoKeyboardOff() func");
    isKeyboardOn_ = false;
    if (isHalfFolded_ && isHalfScreenSwitchOn_) {
        TLOGI(WmsLogTag::DMS, "screen is folded and switch on, no need to recover");
        return;
    }
    if (!ChangeScreenState(false)) {
        TLOGI(WmsLogTag::DMS, "recover from half screen fail!");
    }
}

void SuperFoldStateManager::DoFoldedToHalfFolded(SuperFoldStatusChangeEvents event)
{
    TLOGI(WmsLogTag::DMS, "enter DoFoldedToHalfFolded() func");
    isHalfFolded_ = true;
    if (!isHalfScreenSwitchOn_) {
        TLOGI(WmsLogTag::DMS, "Half Screen Switch is off");
        return;
    }
    if (!ChangeScreenState(true)) {
        TLOGI(WmsLogTag::DMS, "change to half screen fail!");
    }
}

void SuperFoldStateManager::InitSuperFoldStateManagerMap()
{
    AddStateManagerMap(SuperFoldStatus::HALF_FOLDED,
        SuperFoldStatusChangeEvents::ANGLE_CHANGE_EXPANDED,
        SuperFoldStatus::EXPANDED,
        &SuperFoldStateManager::DoAngleChangeExpanded);
    
    AddStateManagerMap(SuperFoldStatus::FOLDED,
        SuperFoldStatusChangeEvents::ANGLE_CHANGE_HALF_FOLDED,
        SuperFoldStatus::HALF_FOLDED,
        &SuperFoldStateManager::DoFoldedToHalfFolded);

    AddStateManagerMap(SuperFoldStatus::EXPANDED,
        SuperFoldStatusChangeEvents::ANGLE_CHANGE_HALF_FOLDED,
        SuperFoldStatus::HALF_FOLDED,
        &SuperFoldStateManager::DoAngleChangeHalfFolded);

    AddStateManagerMap(SuperFoldStatus::HALF_FOLDED,
        SuperFoldStatusChangeEvents::ANGLE_CHANGE_FOLDED,
        SuperFoldStatus::FOLDED,
        &SuperFoldStateManager::DoAngleChangeFolded);

    AddStateManagerMap(SuperFoldStatus::HALF_FOLDED,
        SuperFoldStatusChangeEvents::KEYBOARD_ON,
        SuperFoldStatus::KEYBOARD,
        &SuperFoldStateManager::DoKeyboardOn);

    AddStateManagerMap(SuperFoldStatus::KEYBOARD,
        SuperFoldStatusChangeEvents::KEYBOARD_OFF,
        SuperFoldStatus::HALF_FOLDED,
        &SuperFoldStateManager::DoKeyboardOff);

    // 开机状态自检
    AddStateManagerMap(SuperFoldStatus::UNKNOWN,
        SuperFoldStatusChangeEvents::KEYBOARD_ON,
        SuperFoldStatus::KEYBOARD,
        [&](SuperFoldStatusChangeEvents events) {});

    AddStateManagerMap(SuperFoldStatus::UNKNOWN,
        SuperFoldStatusChangeEvents::ANGLE_CHANGE_HALF_FOLDED,
        SuperFoldStatus::HALF_FOLDED,
        [&](SuperFoldStatusChangeEvents events) {});

    AddStateManagerMap(SuperFoldStatus::UNKNOWN,
        SuperFoldStatusChangeEvents::ANGLE_CHANGE_EXPANDED,
        SuperFoldStatus::EXPANDED,
        [&](SuperFoldStatusChangeEvents events) {});

    AddStateManagerMap(SuperFoldStatus::UNKNOWN,
        SuperFoldStatusChangeEvents::ANGLE_CHANGE_FOLDED,
        SuperFoldStatus::FOLDED,
        [&](SuperFoldStatusChangeEvents events) {});
}

bool SuperFoldStateManager::isParamsValid(std::vector<std::string>& params)
{
    for (auto &param : params) {
        if (param.size() == 0) {
            return false;
        }
        for (int32_t i = 0; i < static_cast<int32_t>(param.size()); ++i) {
            if (param.at(i) < '0' || param.at(i) > '9') {
                return false;
            }
        }
    }
    return true;
}

void SuperFoldStateManager::InitSuperFoldCreaseRegionParams()
{
    std::vector<std::string> foldRect = FoldScreenStateInternel::StringSplit(g_FoldScreenRect, ',');

    if (foldRect.empty() || foldRect.size() <= PARAM_NUMBER_MIN) {
        // ccm numbers of parameter > 10
        TLOGE(WmsLogTag::DMS, "foldRect is invalid");
        return;
    }

    if (!isParamsValid(foldRect)) {
        TLOGE(WmsLogTag::DMS, "params is invalid");
        return;
    }

    ScreenId screenIdFull = 0;
    int32_t superFoldCreaseRegionPosX = 0;
    int32_t superFoldCreaseRegionPosY = std::stoi(foldRect[10]); // ccm PosY
    int32_t superFoldCreaseRegionPosWidth = std::stoi(foldRect[4]); // ccm PosWidth
    int32_t superFoldCreaseRegionPosHeight = std::stoi(foldRect[9]); // ccm PosHeight

    std::vector<DMRect> rect = {
        {
            superFoldCreaseRegionPosX, superFoldCreaseRegionPosY,
            superFoldCreaseRegionPosWidth, superFoldCreaseRegionPosHeight
        }
    };
    currentSuperFoldCreaseRegion_ = new FoldCreaseRegion(screenIdFull, rect);
}

SuperFoldStateManager::SuperFoldStateManager()
{
    InitSuperFoldStateManagerMap();
    InitSuperFoldCreaseRegionParams();
    WatchParameter(BOOTEVENT_BOOT_COMPLETED.c_str(), BootFinishedCallback, this);
}

SuperFoldStateManager::~SuperFoldStateManager()
{
    UnregisterHalfScreenSwitchesObserver();
}

void SuperFoldStateManager::AddStateManagerMap(SuperFoldStatus curState,
    SuperFoldStatusChangeEvents event,
    SuperFoldStatus nextState,
    std::function<void (SuperFoldStatusChangeEvents)> action)
{
    stateManagerMap_[{curState, event}] = {nextState, action};
}

void SuperFoldStateManager::TransferState(SuperFoldStatus nextState)
{
    TLOGI(WmsLogTag::DMS, "TransferState from %{public}d to %{public}d", curState_.load(), nextState);
    curState_ .store(nextState);
}

FoldStatus SuperFoldStateManager::MatchSuperFoldStatusToFoldStatus(SuperFoldStatus superFoldStatus)
{
    switch (superFoldStatus) {
        case SuperFoldStatus::EXPANDED:
            return FoldStatus::EXPAND;
        case SuperFoldStatus::HALF_FOLDED:
            return FoldStatus::HALF_FOLD;
        case SuperFoldStatus::FOLDED:
            return FoldStatus::FOLDED;
        case SuperFoldStatus::KEYBOARD:
            return FoldStatus::HALF_FOLD;
        default:
            return FoldStatus::UNKNOWN;
    }
}

void SuperFoldStateManager::ReportNotifySuperFoldStatusChange(int32_t currentStatus, int32_t nextStatus,
    float postureAngle)
{
    TLOGI(WmsLogTag::DMS, "currentStatus: %{public}d, nextStatus: %{public}d, postureAngle: %{public}f",
        currentStatus, nextStatus, postureAngle);
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
        "NOTIFY_FOLD_STATE_CHANGE",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "CURRENT_FOLD_STATUS", currentStatus,
        "NEXT_FOLD_STATUS", nextStatus,
        "SENSOR_POSTURE", postureAngle);
    if (ret != 0) {
        TLOGI(WmsLogTag::DMS, "Write HiSysEvent error, ret: %{public}d", ret);
    }
}

void SuperFoldStateManager::HandleSuperFoldStatusChange(SuperFoldStatusChangeEvents event)
{
    SuperFoldStatus curState = curState_.load();
    SuperFoldStatus nextState = SuperFoldStatus::UNKNOWN;
    bool isTransfer = false;
    std::function<void (SuperFoldStatusChangeEvents)> action;

    auto item = stateManagerMap_.find({curState, event});
    if (item != stateManagerMap_.end()) {
        nextState = item->second.nextState;
        action = item->second.action;
        isTransfer = true;
    }

    float curAngle = SuperFoldSensorManager::GetInstance().GetCurAngle();
    TLOGD(WmsLogTag::DMS, "curAngle: %{public}f", curAngle);
    if (isTransfer && action) {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:HandleSuperFoldStatusChange");
        ReportNotifySuperFoldStatusChange(static_cast<int32_t>(curState), static_cast<int32_t>(nextState), curAngle);
        action(event);
        TransferState(nextState);
        HandleDisplayNotify(event);
        // notify
        auto screenSession = ScreenSessionManager::GetInstance().GetDefaultScreenSession();
        if (screenSession == nullptr) {
            TLOGE(WmsLogTag::DMS, "screen session is null!");
            return;
        }
        ScreenId screenId = screenSession->GetScreenId();
        if (isHalfScreen_) {
            ScreenSessionManager::GetInstance().OnSuperFoldStatusChange(screenId, SuperFoldStatus::KEYBOARD);
        } else {
            ScreenSessionManager::GetInstance().OnSuperFoldStatusChange(screenId, curState_.load());
        }
        ScreenSessionManager::GetInstance().NotifyFoldStatusChanged(
            MatchSuperFoldStatusToFoldStatus(curState_.load()));
    }
}

SuperFoldStatus SuperFoldStateManager::GetCurrentStatus()
{
    return curState_.load();
}

void SuperFoldStateManager::SetCurrentStatus(SuperFoldStatus curState)
{
    curState_.store(curState);
}

sptr<FoldCreaseRegion> SuperFoldStateManager::GetCurrentFoldCreaseRegion()
{
    TLOGI(WmsLogTag::DMS, "GetCurrentFoldCreaseRegion()");
    return currentSuperFoldCreaseRegion_;
}

void SuperFoldStateManager::HandleDisplayNotify(SuperFoldStatusChangeEvents changeEvent)
{
    TLOGI(WmsLogTag::DMS, "changeEvent: %{public}d", static_cast<uint32_t>(changeEvent));
    sptr<ScreenSession> screenSession =
        ScreenSessionManager::GetInstance().GetDefaultScreenSession();
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "screen session is null");
        return;
    }
    if (screenSession->GetFakeScreenSession() == nullptr) {
        TLOGE(WmsLogTag::DMS, "fake screen session is null");
        return;
    }
    switch (changeEvent) {
        case SuperFoldStatusChangeEvents::ANGLE_CHANGE_HALF_FOLDED:
            HandleExtendToHalfFoldDisplayNotify(screenSession);
            TLOGI(WmsLogTag::DMS, "handle extend change to half fold");
            break;
        case SuperFoldStatusChangeEvents::ANGLE_CHANGE_EXPANDED:
            HandleHalfFoldToExtendDisplayNotify(screenSession);
            TLOGI(WmsLogTag::DMS, "handle half fold change to extend");
            break;
        case SuperFoldStatusChangeEvents::KEYBOARD_ON:
            HandleKeyboardOnDisplayNotify(screenSession);
            TLOGI(WmsLogTag::DMS, "handle keyboard on");
            break;
        case SuperFoldStatusChangeEvents::KEYBOARD_OFF:
            HandleKeyboardOffDisplayNotify(screenSession);
            TLOGI(WmsLogTag::DMS, "handle keyboard off");
            break;
        default:
            TLOGE(WmsLogTag::DMS, "nothing to handle");
            return;
    }
}

void SuperFoldStateManager::HandleExtendToHalfFoldDisplayNotify(sptr<ScreenSession> screenSession)
{
    screenSession->UpdatePropertyByFakeInUse(true);
    screenSession->SetIsBScreenHalf(true);
    ScreenSessionManager::GetInstance().NotifyDisplayChanged(
        screenSession->ConvertToDisplayInfo(), DisplayChangeEvent::SUPER_FOLD_RESOLUTION_CHANGED);
    sptr<ScreenSession> fakeScreenSession = screenSession->GetFakeScreenSession();
    ScreenSessionManager::GetInstance().NotifyDisplayCreate(
        fakeScreenSession->ConvertToDisplayInfo());
    screenSession->PropertyChange(screenSession->GetScreenProperty(),
        ScreenPropertyChangeReason::SUPER_FOLD_STATUS_CHANGE);
}

void SuperFoldStateManager::HandleHalfFoldToExtendDisplayNotify(sptr<ScreenSession> screenSession)
{
    screenSession->UpdatePropertyByFakeInUse(false);
    screenSession->SetIsBScreenHalf(false);
    sptr<ScreenSession> fakeScreenSession = screenSession->GetFakeScreenSession();
    sptr<DisplayInfo> fakeDisplayInfo = fakeScreenSession->ConvertToDisplayInfo();
    if (fakeDisplayInfo == nullptr) {
        TLOGE(WmsLogTag::DMS, "get fake display failed");
        return;
    }
    DisplayId fakeDisplayId = fakeDisplayInfo->GetDisplayId();
    ScreenSessionManager::GetInstance().NotifyDisplayDestroy(fakeDisplayId);
    ScreenSessionManager::GetInstance().NotifyDisplayChanged(
        screenSession->ConvertToDisplayInfo(),
        DisplayChangeEvent::SUPER_FOLD_RESOLUTION_CHANGED);
    screenSession->PropertyChange(screenSession->GetScreenProperty(),
        ScreenPropertyChangeReason::SUPER_FOLD_STATUS_CHANGE);
}

void SuperFoldStateManager::HandleKeyboardOnDisplayNotify(sptr<ScreenSession> screenSession)
{
    screenSession->UpdatePropertyByFakeInUse(false);
    screenSession->SetIsBScreenHalf(true);
    sptr<ScreenSession> fakeScreenSession = screenSession->GetFakeScreenSession();
    sptr<DisplayInfo> fakeDisplayInfo = fakeScreenSession->ConvertToDisplayInfo();
    if (fakeDisplayInfo == nullptr) {
        TLOGE(WmsLogTag::DMS, "get fake display failed");
        return;
    }
    DisplayId fakeDisplayId = fakeDisplayInfo->GetDisplayId();
    ScreenSessionManager::GetInstance().NotifyDisplayDestroy(fakeDisplayId);
    screenSession->PropertyChange(screenSession->GetScreenProperty(),
        ScreenPropertyChangeReason::SUPER_FOLD_STATUS_CHANGE);
}

void SuperFoldStateManager::HandleKeyboardOffDisplayNotify(sptr<ScreenSession> screenSession)
{
    screenSession->UpdatePropertyByFakeInUse(true);
    screenSession->SetIsBScreenHalf(true);
    sptr<ScreenSession> fakeScreenSession = screenSession->GetFakeScreenSession();
    ScreenSessionManager::GetInstance().NotifyDisplayCreate(
        fakeScreenSession->ConvertToDisplayInfo());
    screenSession->PropertyChange(screenSession->GetScreenProperty(),
        ScreenPropertyChangeReason::SUPER_FOLD_STATUS_CHANGE);
}

void SuperFoldStateManager::BootFinishedCallback(const char *key, const char *value, void *context)
{
    if (strcmp(key, BOOTEVENT_BOOT_COMPLETED.c_str()) == 0 && strcmp(value, "true") == 0) {
        TLOGI(WmsLogTag::DMS, "boot animation finished");
        auto &that = *reinterpret_cast<SuperFoldStateManager *>(context);
        that.RegisterHalfScreenSwitchesObserver();
        that.InitHalfScreen();
    }
}

void SuperFoldStateManager::InitHalfScreen()
{
    isHalfScreenSwitchOn_ = ScreenSettingHelper::GetHalfScreenSwitchState();
    SuperFoldStatus curFoldState = ScreenSessionManager::GetInstance().GetSuperFoldStatus();
    if (curFoldState == SuperFoldStatus::HALF_FOLDED) {
        isHalfFolded_ = true;
    }
    if (isHalfScreenSwitchOn_ && isHalfFolded_) {
        if (!ChangeScreenState(true)) {
            TLOGI(WmsLogTag::DMS, "change to half screen fail!");
        }
        auto screenSession = ScreenSessionManager::GetInstance().GetDefaultScreenSession();
        ScreenId screenId = screenSession->GetScreenId();
        ScreenSessionManager::GetInstance().OnSuperFoldStatusChange(screenId, SuperFoldStatus::KEYBOARD);
    }
}

void SuperFoldStateManager::RegisterHalfScreenSwitchesObserver()
{
    TLOGI(WmsLogTag::DMS, "half screen switch register");
    SettingObserver::UpdateFunc updateFunc =
        [&](const std::string& key) {this->OnHalfScreenSwitchesStateChanged();};
    ScreenSettingHelper::RegisterSettingHalfScreenObserver(updateFunc);
}

void SuperFoldStateManager::UnregisterHalfScreenSwitchesObserver()
{
    TLOGI(WmsLogTag::DMS, "half screen switch unregister");
    ScreenSettingHelper::UnregisterSettingHalfScreenObserver();
}

void SuperFoldStateManager::OnHalfScreenSwitchesStateChanged()
{
    TLOGI(WmsLogTag::DMS, "half screen switch state changed");
    bool curSwitchState = ScreenSettingHelper::GetHalfScreenSwitchState();
    if (curSwitchState == isHalfScreenSwitchOn_) {
        TLOGI(WmsLogTag::DMS, "half screen switch didn't change");
        return;
    }
    isHalfScreenSwitchOn_ = curSwitchState;
    auto tempState = SuperFoldStatus::KEYBOARD;
    if ((isHalfScreenSwitchOn_ && isHalfFolded_) || isKeyboardOn_)  {
        if (!ChangeScreenState(true)) {
            TLOGI(WmsLogTag::DMS, "change to half screen fail!");
        }
    } else {
        if (!ChangeScreenState(false)) {
            TLOGI(WmsLogTag::DMS, "recover from half screen fail!");
        }
        tempState = curState_;
    }
    auto screenSession = ScreenSessionManager::GetInstance().GetDefaultScreenSession();
    ScreenId screenId = screenSession->GetScreenId();
    ScreenSessionManager::GetInstance().OnSuperFoldStatusChange(screenId, tempState);
}

bool SuperFoldStateManager::ChangeScreenState(bool toHalf)
{
    TLOGD(WmsLogTag::DMS, "isHalfScreen_ = %{public}d, toHalf = %{public}d", isHalfScreen_, toHalf);
    if (isHalfScreen_ == toHalf) {
        TLOGI(WmsLogTag::DMS, "screen is already in the desired state, no need to change");
        return false;
    }
    sptr<ScreenSession> meScreenSession = ScreenSessionManager::GetInstance().
        GetDefaultScreenSession();
    if (meScreenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "screen session is null!");
        return false;
    }
    auto screenProperty = meScreenSession->GetScreenProperty();
    auto screenWidth = screenProperty.GetBounds().rect_.GetWidth();
    auto screenHeight = screenProperty.GetBounds().rect_.GetHeight();
    if (toHalf) {
        screenWidth = screenProperty.GetFakeBounds().rect_.GetWidth();
        screenHeight = screenProperty.GetFakeBounds().rect_.GetHeight();
    }
    OHOS::Rect rectCur{
        .x = 0,
        .y = 0,
        .w = static_cast<int>(screenWidth),
        .h = static_cast<int>(screenHeight),
    };
    // SCREEN_ID_FULL = 0
    auto response = RSInterfaces::GetInstance().SetScreenActiveRect(0, rectCur);
    isHalfScreen_ = toHalf;
#ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE,
        toHalf ? KEYBOARD_ON_CONFIG : KEYBOARD_OFF_CONFIG, TpFeatureConfigType::AFT_TP_FEATURE);
#endif
    ScreenSessionManager::GetInstance().NotifyScreenMagneticStateChanged(toHalf);
    TLOGI(WmsLogTag::DMS, "rect [%{public}f , %{public}f], rs response is %{public}ld",
        screenWidth, screenHeight, static_cast<long>(response));
    return true;
}
} // Rosen
} // OHOS