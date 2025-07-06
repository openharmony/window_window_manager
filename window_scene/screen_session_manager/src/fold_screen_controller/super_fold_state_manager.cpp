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

namespace OHOS {

namespace Rosen {

WM_IMPLEMENT_SINGLE_INSTANCE(SuperFoldStateManager)

namespace {
const std::string g_FoldScreenRect = system::GetParameter("const.window.foldscreen.config_rect", "");
const int32_t PARAM_NUMBER_MIN = 10;
const int32_t HEIGHT_HALF = 2;
#ifdef TP_FEATURE_ENABLE
const int32_t TP_TYPE = 12;
const char* KEYBOARD_ON_CONFIG = "version:3+main";
const char* KEYBOARD_OFF_CONFIG = "version:3+whole";
#endif
static bool isKeyboardOn_ = false;
static bool isSystemKeyboardOn_ = false;
constexpr int32_t FOLD_CREASE_RECT_SIZE = 4; //numbers of parameter on the current device is 4
const std::string g_LiveCreaseRegion = system::GetParameter("const.display.foldscreen.crease_region", "");
const std::string FOLD_CREASE_DELIMITER = ",;";
constexpr ScreenId SCREEN_ID_FULL = 0;
}

void SuperFoldStateManager::DoAngleChangeFolded(SuperFoldStatusChangeEvents event)
{
    TLOGI(WmsLogTag::DMS, "enter %{public}d", event);
}

void SuperFoldStateManager::DoAngleChangeHalfFolded(SuperFoldStatusChangeEvents event)
{
    TLOGI(WmsLogTag::DMS, "enter %{public}d", event);
}

void SuperFoldStateManager::DoAngleChangeExpanded(SuperFoldStatusChangeEvents event)
{
    TLOGI(WmsLogTag::DMS, "enter %{public}d", event);
}

void SuperFoldStateManager::DoKeyboardOn(SuperFoldStatusChangeEvents event)
{
    TLOGI(WmsLogTag::DMS, "enter %{public}d", event);
    isKeyboardOn_ = true;
    if (!ChangeScreenState(true)) {
        TLOGI(WmsLogTag::DMS, "change to half screen fail!");
    }
}

void SuperFoldStateManager::DoKeyboardOff(SuperFoldStatusChangeEvents event)
{
    TLOGI(WmsLogTag::DMS, "enter %{public}d", event);
    isKeyboardOn_ = false;
    if (!ChangeScreenState(false)) {
        TLOGI(WmsLogTag::DMS, "recover from half screen fail!");
    }
}

void SuperFoldStateManager::DoFoldedToHalfFolded(SuperFoldStatusChangeEvents event)
{
    TLOGI(WmsLogTag::DMS, "enter %{public}d", event);
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

    AddStateManagerMap(SuperFoldStatus::FOLDED,
        SuperFoldStatusChangeEvents::ANGLE_CHANGE_EXPANDED,
        SuperFoldStatus::EXPANDED,
        &SuperFoldStateManager::DoAngleChangeExpanded);

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

FoldCreaseRegion SuperFoldStateManager::GetFoldCreaseRegion(bool isVertical)
{
    std::vector<int32_t> foldRect = FoldScreenStateInternel::StringFoldRectSplitToInt(g_FoldScreenRect,
        FOLD_CREASE_DELIMITER);
    if (foldRect.size() != FOLD_CREASE_RECT_SIZE) {
        TLOGE(WmsLogTag::DMS, "foldRect is invalid");
        return FoldCreaseRegion(0, {});
    }

    ScreenId screenIdFull = 0;
    std::vector<DMRect> foldCreaseRect;
    GetFoldCreaseRect(isVertical, foldCreaseRect, foldRect);
    return FoldCreaseRegion(screenIdFull, foldCreaseRect);
}

void SuperFoldStateManager::GetFoldCreaseRect(bool isVertical,
    std::vector<DMRect>& foldCreaseRect, const std::vector<int32_t>& foldRect)
{
    if (isVertical) {
        TLOGI(WmsLogTag::DMS, "the current FoldCreaseRect is vertical");
        int32_t liveCreaseRegionPosX = foldRect[1]; // live Crease Region PosX
        int32_t liveCreaseRegionPosY = foldRect[0]; // live Crease Region PosY
        int32_t liveCreaseRegionPosWidth = foldRect[3]; // live Crease Region PosWidth
        int32_t liveCreaseRegionPosHeight = foldRect[2]; // live Crease Region PosHeight
    } else {
        TLOGI(WmsLogTag::DMS, "the current FoldCreaseRect is horizontal");
        int32_t liveCreaseRegionPosX = foldRect[0]; // live Crease Region PosX
        int32_t liveCreaseRegionPosY = foldRect[1]; // live Crease Region PosY
        int32_t liveCreaseRegionPosWidth = foldRect[2]; // live Crease Region PosWidth
        int32_t liveCreaseRegionPosHeight = foldRect[3]; // live Crease Region PosHeight
    }
    foldCreaseRect = {
        {
            liveCreaseRegionPosX, liveCreaseRegionPosY,
            liveCreaseRegionPosWidth, liveCreaseRegionPosHeight
        }
    };
    return;
}

SuperFoldStateManager::SuperFoldStateManager()
{
    InitSuperFoldStateManagerMap();
    InitSuperFoldCreaseRegionParams();
}

SuperFoldStateManager::~SuperFoldStateManager()
{
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

void SuperFoldStateManager::HandleScreenConnectChange()
{
    std::unique_lock<std::mutex> lock(superStatusMutex_);
    SuperFoldStatus curFoldState = curState_.load();
    lock.unlock();
    if (curFoldState == SuperFoldStatus::KEYBOARD) {
        HandleSuperFoldStatusChange(SuperFoldStatusChangeEvents::KEYBOARD_OFF);
        HandleSuperFoldStatusChange(SuperFoldStatusChangeEvents::ANGLE_CHANGE_EXPANDED);
    } else {
        HandleSuperFoldStatusChange(SuperFoldStatusChangeEvents::ANGLE_CHANGE_EXPANDED);
    }
}

void SuperFoldStateManager::HandleSuperFoldStatusChange(SuperFoldStatusChangeEvents event)
{
    std::unique_lock<std::mutex> lock(superStatusMutex_);
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
        action(event);
        TransferState(nextState);
        lock.unlock();
        ReportNotifySuperFoldStatusChange(static_cast<int32_t>(curState), static_cast<int32_t>(nextState), curAngle);
        HandleDisplayNotify(event);
        // notify
        auto screenSession = ScreenSessionManager::GetInstance().GetDefaultScreenSession();
        if (screenSession == nullptr) {
            TLOGE(WmsLogTag::DMS, "screen session is null!");
            return;
        }
        ScreenId screenId = screenSession->GetScreenId();
        ScreenSessionManager::GetInstance().OnSuperFoldStatusChange(screenId, curState_.load());
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
    std::unique_lock<std::mutex> lock(superStatusMutex_);
    curState_.store(curState);
}

sptr<FoldCreaseRegion> SuperFoldStateManager::GetCurrentFoldCreaseRegion()
{
    TLOGI(WmsLogTag::DMS, "GetCurrentFoldCreaseRegion()");
    return currentSuperFoldCreaseRegion_;
}

FoldCreaseRegion SuperFoldStateManager::GetLiveCreaseRegion()
{
    TLOGI(WmsLogTag::DMS, "enter");
    SuperFoldStatus curFoldState = ScreenSessionManager::GetInstance().GetSuperFoldStatus();
    if (curFoldState == SuperFoldStatus::UNKNOWN || curFoldState == SuperFoldStatus::FOLDED) {
        return FoldCreaseRegion(0, {});
    }
    sptr<ScreenSession> screenSession = ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_FULL);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "default screenSession is null");
        return FoldCreaseRegion(0, {});
    }
    DisplayOrientation displayOrientation = screenSession->GetScreenProperty().GetDisplayOrientation();
    switch (displayOrientation) {
        case DisplayOrientation::PORTRAIT:
        case DisplayOrientation::PORTRAIT_INVERTED: {
            liveCreaseRegion_ = GetFoldCreaseRegion(false);
            break;
        }
        case DisplayOrientation::LANDSCAPE:
        case DisplayOrientation::LANDSCAPE_INVERTED: {
            liveCreaseRegion_ = GetFoldCreaseRegion(true);
            break;
        }
        default: {
            TLOGE(WmsLogTag::DMS, "displayOrientation is invalid");
        }
    }
    return liveCreaseRegion_;
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
        case SuperFoldStatusChangeEvents::SYSTEM_KEYBOARD_ON:
            HandleSystemKeyboardStatusDisplayNotify(screenSession, true);
            TLOGI(WmsLogTag::DMS, "handle system keyboard on");
            break;
        case SuperFoldStatusChangeEvents::SYSTEM_KEYBOARD_OFF:
            HandleSystemKeyboardStatusDisplayNotify(screenSession, false);
            TLOGI(WmsLogTag::DMS, "handle system keyboard off");
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
    auto screeBounds = screenSession->GetScreenProperty().GetBounds();
    bool currFakeInUse = screenSession->GetScreenProperty().GetIsFakeInUse();
    screenSession->UpdatePropertyByFakeInUse(false);
    screenSession->SetIsBScreenHalf(true);
    int32_t validheight = GetCurrentValidHeight(screenSession);
    if (screeBounds.rect_.GetWidth() < screeBounds.rect_.GetHeight()) {
        screenSession->SetValidHeight(validheight);
        screenSession->SetValidWidth(screeBounds.rect_.GetWidth());
    } else {
        screenSession->SetValidWidth(screeBounds.rect_.GetHeight());
        screenSession->SetValidHeight(validheight);
    }
    sptr<ScreenSession> fakeScreenSession = screenSession->GetFakeScreenSession();
    sptr<DisplayInfo> fakeDisplayInfo = fakeScreenSession->ConvertToDisplayInfo();
    if (fakeDisplayInfo == nullptr) {
        TLOGE(WmsLogTag::DMS, "get fake display failed");
        return;
    }
    DisplayId fakeDisplayId = fakeDisplayInfo->GetDisplayId();
    if (currFakeInUse) {
        ScreenSessionManager::GetInstance().NotifyDisplayDestroy(fakeDisplayId);
    }
    screenSession->PropertyChange(screenSession->GetScreenProperty(),
        ScreenPropertyChangeReason::SUPER_FOLD_STATUS_CHANGE);
    ScreenSessionManager::GetInstance().UpdateValidArea(
        screenSession->GetScreenId(),
        screenSession->GetValidWidth(),
        screenSession->GetValidHeight());
}

void SuperFoldStateManager::HandleKeyboardOffDisplayNotify(sptr<ScreenSession> screenSession)
{
    auto screeBounds = screenSession->GetScreenProperty().GetBounds();
    screenSession->UpdatePropertyByFakeInUse(true);
    screenSession->SetIsBScreenHalf(true);
    screenSession->SetValidWidth(screeBounds.rect_.GetWidth());
    screenSession->SetValidHeight(screeBounds.rect_.GetHeight());
    sptr<ScreenSession> fakeScreenSession = screenSession->GetFakeScreenSession();
    ScreenSessionManager::GetInstance().NotifyDisplayCreate(
        fakeScreenSession->ConvertToDisplayInfo());
    screenSession->PropertyChange(screenSession->GetScreenProperty(),
        ScreenPropertyChangeReason::SUPER_FOLD_STATUS_CHANGE);
    ScreenSessionManager::GetInstance().UpdateValidArea(
        screenSession->GetScreenId(),
        screenSession->GetValidWidth(),
        screenSession->GetValidHeight());
}

void SuperFoldStateManager::SetSystemKeyboardStatus(bool isTpKeyboardOn)
{
    SuperFoldStatusChangeEvents events = isTpKeyboardOn ? SuperFoldStatusChangeEvents::SYSTEM_KEYBOARD_ON
        : SuperFoldStatusChangeEvents::SYSTEM_KEYBOARD_OFF;
    isSystemKeyboardOn_ = isTpKeyboardOn;
    HandleDisplayNotify(events);
}

bool SuperFoldStateManager::GetSystemKeyboardStatus()
{
    return isSystemKeyboardOn_;
}

void SuperFoldStateManager::HandleSystemKeyboardStatusDisplayNotify(
    sptr<ScreenSession> screenSession, bool isTpKeyboardOn)
{
    SuperFoldStatus curFoldState = curState_.load();
    TLOGD(WmsLogTag::DMS, "curFoldState: %{public}u", curFoldState);
    if (!isKeyboardOn_ && curFoldState == SuperFoldStatus::HALF_FOLDED) {
        screenSession->UpdatePropertyByFakeInUse(!isTpKeyboardOn);
        screenSession->SetIsFakeInUse(!isTpKeyboardOn);
        screenSession->SetIsBScreenHalf(true);
    }
    sptr<ScreenSession> fakeScreenSession = screenSession->GetFakeScreenSession();
    sptr<DisplayInfo> fakeDisplayInfo = fakeScreenSession->ConvertToDisplayInfo();
    if (fakeDisplayInfo == nullptr) {
        TLOGE(WmsLogTag::DMS, "get fake display failed");
        return;
    }
    if (isTpKeyboardOn) {
        DisplayId fakeDisplayId = fakeDisplayInfo->GetDisplayId();
        ScreenSessionManager::GetInstance().NotifyDisplayDestroy(fakeDisplayId);

        auto screeBounds = screenSession->GetScreenProperty().GetBounds();
        int32_t validheight = GetCurrentValidHeight(screenSession);
        if (screeBounds.rect_.GetWidth() < screeBounds.rect_.GetHeight()) {
            screenSession->SetPointerActiveWidth(static_cast<uint32_t>(screeBounds.rect_.GetWidth()));
            screenSession->SetPointerActiveHeight(static_cast<uint32_t>(validheight));
        } else {
            screenSession->SetPointerActiveWidth(static_cast<uint32_t>(screeBounds.rect_.GetHeight()));
            screenSession->SetPointerActiveHeight(static_cast<uint32_t>(validheight));
        }
        TLOGD(WmsLogTag::DMS, "vh: %{public}d, paw: %{public}u, pah: %{public}u", validheight,
            screenSession->GetPointerActiveWidth(), screenSession->GetPointerActiveHeight());
    } else {
        if (curFoldState == SuperFoldStatus::HALF_FOLDED) {
            ScreenSessionManager::GetInstance().NotifyDisplayCreate(fakeDisplayInfo);
        }

        screenSession->SetPointerActiveWidth(0);
        screenSession->SetPointerActiveHeight(0);
        TLOGD(WmsLogTag::DMS, "paw: %{public}u, pah: %{public}u",
            screenSession->GetPointerActiveWidth(), screenSession->GetPointerActiveHeight());
    }
    screenSession->PropertyChange(screenSession->GetScreenProperty(),
        ScreenPropertyChangeReason::SUPER_FOLD_STATUS_CHANGE);
}

int32_t SuperFoldStateManager::GetCurrentValidHeight(sptr<ScreenSession> screenSession)
{
    if (currentSuperFoldCreaseRegion_ != nullptr) {
        std::vector<DMRect> creaseRects = currentSuperFoldCreaseRegion_->GetCreaseRects();
        if (!creaseRects.empty()) {
            TLOGI(WmsLogTag::DMS, "validheight: %{public}d", creaseRects[0].posY_);
            return creaseRects[0].posY_;
        }
    }
    TLOGE(WmsLogTag::DMS, "Get CreaseRects failed, validheight is half of height");
    auto screeBounds = screenSession->GetScreenProperty().GetBounds();
    if (screeBounds.rect_.GetWidth() < screeBounds.rect_.GetHeight()) {
        return screeBounds.rect_.GetHeight() / HEIGHT_HALF;
    } else {
        return screeBounds.rect_.GetWidth() / HEIGHT_HALF;
    }
}

bool SuperFoldStateManager::ChangeScreenState(bool toHalf)
{
    ScreenSessionManager::GetInstance().NotifyScreenMagneticStateChanged(toHalf);
    sptr<ScreenSession> screenSession = ScreenSessionManager::GetInstance().
        GetDefaultScreenSession();
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "screen session is null!");
        return false;
    }
    auto screenProperty = ScreenSessionManager::GetInstance().
        GetPhyScreenProperty(screenSession->GetScreenId());
    auto screenWidth = screenProperty.GetPhyBounds().rect_.GetWidth();
    auto screenHeight = screenProperty.GetPhyBounds().rect_.GetHeight();
    if (toHalf) {
        screenWidth = screenProperty.GetPhyBounds().rect_.GetWidth();
        screenHeight = screenProperty.GetPhyBounds().rect_.GetHeight() / HEIGHT_HALF;
    }
    OHOS::Rect rectCur{
        .x = 0,
        .y = 0,
        .w = static_cast<int>(screenWidth),
        .h = static_cast<int>(screenHeight),
    };
    // SCREEN_ID_FULL = 0
    auto response = RSInterfaces::GetInstance().SetScreenActiveRect(0, rectCur);
#ifdef TP_FEATURE_ENABLE
    RSInterfaces::GetInstance().SetTpFeatureConfig(TP_TYPE,
        toHalf ? KEYBOARD_ON_CONFIG : KEYBOARD_OFF_CONFIG, TpFeatureConfigType::AFT_TP_FEATURE);
#endif
    TLOGI(WmsLogTag::DMS, "rect [%{public}f , %{public}f], rs response is %{public}ld",
        screenWidth, screenHeight, static_cast<long>(response));
    return true;
}

bool SuperFoldStateManager::GetKeyboardState()
{
    TLOGI(WmsLogTag::DMS, "GetKeyboardState isKeyboardOn_ : %{public}d", isKeyboardOn_);
    return isKeyboardOn_;
}
} // Rosen
} // OHOS