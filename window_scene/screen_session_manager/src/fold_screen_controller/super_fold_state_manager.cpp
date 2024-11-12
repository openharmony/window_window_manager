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

#include "screen_session_manager/include/screen_session_manager.h"
#include "fold_screen_controller/super_fold_state_manager.h"
#include "fold_screen_controller/super_fold_sensor_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {

namespace Rosen {

WM_IMPLEMENT_SINGLE_INSTANCE(SuperFoldStateManager)

void SuperFoldStateManager::DoAngleChangeFolded(SuperFoldStatusChangeEvents event)
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoAngleChangeFolded()");
}

void SuperFoldStateManager::DoAngleChangeHalfFolded(SuperFoldStatusChangeEvents event)
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoAngleChangeHalfFolded())");
}

void SuperFoldStateManager::DoAngleChangeExpanded(SuperFoldStatusChangeEvents event)
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoAngleChangeExpanded()");
}

void SuperFoldStateManager::DoKeyboardOn(SuperFoldStatusChangeEvents event)
{
    ScreenSessionManager::GetInstance().NotifyScreenMagneticStateChanged(true);
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoKeyboardOn()");
}

void SuperFoldStateManager::DoKeyboardOff(SuperFoldStatusChangeEvents event)
{
    ScreenSessionManager::GetInstance().NotifyScreenMagneticStateChanged(false);
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoKeyboardOff()");
}

void SuperFoldStateManager::DoFoldedToHalfFolded(SuperFoldStatusChangeEvents event)
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoFoldedToHalfFolded()");
}

void SuperFoldStateManager::DoExpandedToKeyboard(SuperFoldStatusChangeEvents event)
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoExpandedToKeyboard()");
}

SuperFoldStateManager::SuperFoldStateManager()
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

    AddStateManagerMap(SuperFoldStatus::EXPANDED,
        SuperFoldStatusChangeEvents::KEYBOARD_ON,
        SuperFoldStatus::HALF_FOLDED,
        &SuperFoldStateManager::DoExpandedToKeyboard);

    AddStateManagerMap(SuperFoldStatus::KEYBOARD,
        SuperFoldStatusChangeEvents::KEYBOARD_OFF,
        SuperFoldStatus::HALF_FOLDED,
        &SuperFoldStateManager::DoKeyboardOff);
}

SuperFoldStateManager::~SuperFoldStateManager() = default;

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

    if (isTransfer && action) {
        action(event);
        HandleDisplayNotify(event);
        TransferState(nextState);
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
    curState_.store(curState);
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
} // Rosen
} // OHOS