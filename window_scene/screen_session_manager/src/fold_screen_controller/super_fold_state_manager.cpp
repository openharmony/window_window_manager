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

void SuperFoldStateManager::DoAngleChangeFolded()
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoAngleChangeFolded()");
}

void SuperFoldStateManager::DoAngleChangeHalfFolded()
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoAngleChangeHalfFolded())");
}

void SuperFoldStateManager::DoAngleChangeExpanded()
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoAngleChangeExpanded()");
}

void SuperFoldStateManager::DoKeyboardOn()
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoKeyboardOn()");
}

void SuperFoldStateManager::DoKeyboardOff()
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoKeyboardOff()");
}

void SuperFoldStateManager::DoSoftKeyboardOn()
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoSoftKeyboardOn()");
}

void SuperFoldStateManager::DoSoftKeyboardOff()
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoSoftKeyboardOff()");
}

void SuperFoldStateManager::DoKeyboardToExpanded()
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoKeyboardToExpanded()");
}

void SuperFoldStateManager::DoExpandedToKeyboard()
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoExpandedToKeyboard()");
}

SuperFoldStateManager::SuperFoldStateManager()
{
    initStateManagerMap(SuperFoldStatus::HALF_FOLDED,
        SuperFoldStatusChangeEvents::ANGLE_CHANGE_EXPANDED,
        SuperFoldStatus::EXPANDED,
        &SuperFoldStateManager::DoAngleChangeExpanded);

    initStateManagerMap(SuperFoldStatus::EXPANDED,
        SuperFoldStatusChangeEvents::ANGLE_CHANGE_HALF_FOLDED,
        SuperFoldStatus::HALF_FOLDED,
        &SuperFoldStateManager::DoAngleChangeHalfFolded);

    initStateManagerMap(SuperFoldStatus::HALF_FOLDED,
        SuperFoldStatusChangeEvents::ANGLE_CHANGE_FOLDED,
        SuperFoldStatus::FOLDED,
        &SuperFoldStateManager::DoAngleChangeFolded);

    initStateManagerMap(SuperFoldStatus::HALF_FOLDED,
        SuperFoldStatusChangeEvents::KEYBOARD_ON,
        SuperFoldStatus::KEYBOARD,
        &SuperFoldStateManager::DoKeyboardOn);

    initStateManagerMap(SuperFoldStatus::EXPANDED,
        SuperFoldStatusChangeEvents::KEYBOARD_ON,
        SuperFoldStatus::KEYBOARD,
        &SuperFoldStateManager::DoExpandedToKeyboard);

    initStateManagerMap(SuperFoldStatus::KEYBOARD,
        SuperFoldStatusChangeEvents::KEYBOARD_OFF,
        SuperFoldStatus::HALF_FOLDED,
        &SuperFoldStateManager::DoKeyboardOff);

    initStateManagerMap(SuperFoldStatus::KEYBOARD,
        SuperFoldStatusChangeEvents::KEYBOARD_OFF,
        SuperFoldStatus::EXPANDED,
        &SuperFoldStateManager::DoKeyboardToExpanded);

    initStateManagerMap(SuperFoldStatus::HALF_FOLDED,
        SuperFoldStatusChangeEvents::SOFT_KEYBOARD_ON,
        SuperFoldStatus::SOFT_KEYBOARD,
        &SuperFoldStateManager::DoSoftKeyboardOn);

    initStateManagerMap(SuperFoldStatus::SOFT_KEYBOARD,
        SuperFoldStatusChangeEvents::SOFT_KEYBOARD_OFF,
        SuperFoldStatus::HALF_FOLDED,
        &SuperFoldStateManager::DoSoftKeyboardOff);
}

SuperFoldStateManager::~SuperFoldStateManager() = default;

void SuperFoldStateManager::initStateManagerMap(SuperFoldStatus curState,
    SuperFoldStatusChangeEvents event,
    SuperFoldStatus nextState,
    std::function<void ()> action)
{
    stateManagerMap_[{curState, event}] = {nextState, action};
}

void SuperFoldStateManager::transferState(SuperFoldStatus nextState)
{
    TLOGI(WmsLogTag::DMS, "transferState from %{public}d to %{public}d", curState_, nextState);
    curState_ = nextState;
}

void SuperFoldStateManager::HandleSuperFoldStatusChange(SuperFoldStatusChangeEvents event)
{
    SuperFoldStatus curState = curState_;
    SuperFoldStatus nextState = SuperFoldStatus::UNKNOWN;
    bool isTransfer = false;
    std::function<void ()> action;

    auto item = stateManagerMap_.find({curState, event});
    if (item != stateManagerMap_.end()) {
        nextState = item->second.nextState;
        action = item->second.action;
        isTransfer = true;
    }

    if (isTransfer && action) {
        action();
        transferState(nextState);
        // notify
    }
}

SuperFoldStatus SuperFoldStateManager::GetCurrentStatus()
{
    return curState_;
}

void SuperFoldStateManager::SetCurrentStatus(SuperFoldStatus curState)
{
    curState_ = curState;
}

} // Rosen
} // OHOS