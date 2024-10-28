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
#include "window_manager_hilog.h"

namespace OHOS {

namespace Rosen {

WM_IMPLEMENT_SINGLE_INSTANCE(SuperFoldStateManager)

void SuperFoldStateManager::DoFoldToHalfFold()
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoFoldToHalfFold()");
}

void SuperFoldStateManager::DoHalfFoldToFold()
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoHalfFoldToFold()");
}

void SuperFoldStateManager::DoHalFoldToExpand()
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoHalFoldToExpand()");
}

void SuperFoldStateManager::DoExpandToHalfFold()
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoExpandToHalfFold()");
}

void SuperFoldStateManager::DoHalfFoldToKeyboard()
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoHalfFoldToKeyboard()");
}

void SuperFoldStateManager::DoKeyboardToHalfFold()
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoKeyboardToHalfFold()");
}

void SuperFoldStateManager::DoHalfFoldToSoftKeyboard()
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoHalfFoldToSoftKeyboard()");
}

void SuperFoldStateManager::DoSoftKeyboardToHalfFold()
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoSoftKeyboardToHalfFold()");
}

void SuperFoldStateManager::DoSoftKeyboardToKeyboard()
{
    TLOGI(WmsLogTag::DMS, "SuperFoldStateManager::DoSoftKeyboardToKeyboard()");
}

SuperFoldStateManager::SuperFoldStateManager()
{
    initStateManagerMap(SuperFoldStatus::FOLDED,
        SuperFoldStatusChangeEvents::FOLDED_TO_HALF_FOLDED,
        SuperFoldStatus::HALF_FOLDED,
        &SuperFoldStateManager::DoFoldToHalfFold);

    initStateManagerMap(SuperFoldStatus::HALF_FOLDED,
        SuperFoldStatusChangeEvents::HALF_FOLDED_TO_FOLDED,
        SuperFoldStatus::FOLDED,
        &SuperFoldStateManager::DoHalfFoldToFold);

    initStateManagerMap(SuperFoldStatus::HALF_FOLDED,
        SuperFoldStatusChangeEvents::HALF_FOLDED_TO_EXPANDED,
        SuperFoldStatus::EXPANDED,
        &SuperFoldStateManager::DoHalFoldToExpand);

    initStateManagerMap(SuperFoldStatus::EXPANDED,
        SuperFoldStatusChangeEvents::EXPANDED_TO_HALF_FOLDED,
        SuperFoldStatus::HALF_FOLDED,
        &SuperFoldStateManager::DoExpandToHalfFold);

    initStateManagerMap(SuperFoldStatus::HALF_FOLDED,
        SuperFoldStatusChangeEvents::HALF_FOLDED_TO_KEYBOARD,
        SuperFoldStatus::KEYBOARD,
        &SuperFoldStateManager::DoHalfFoldToKeyboard);

    initStateManagerMap(SuperFoldStatus::KEYBOARD,
        SuperFoldStatusChangeEvents::KEYBOARD_TO_HALF_FOLDED,
        SuperFoldStatus::HALF_FOLDED,
        &SuperFoldStateManager::DoKeyboardToHalfFold);

    initStateManagerMap(SuperFoldStatus::HALF_FOLDED,
        SuperFoldStatusChangeEvents::HALF_FOLDED_TO_SOFT_KEYBOARD,
        SuperFoldStatus::SOFT_KEYBOARD,
        &SuperFoldStateManager::DoHalfFoldToSoftKeyboard);

    initStateManagerMap(SuperFoldStatus::SOFT_KEYBOARD,
        SuperFoldStatusChangeEvents::SOFT_KEYBOARD_TO_HALF_FOLDED,
        SuperFoldStatus::HALF_FOLDED,
        &SuperFoldStateManager::DoSoftKeyboardToKeyboard);

    initStateManagerMap(SuperFoldStatus::SOFT_KEYBOARD,
        SuperFoldStatusChangeEvents::SOFT_KEYBOARD_TO_KEYBOARD,
        SuperFoldStatus::KEYBOARD,
        &SuperFoldStateManager::DoSoftKeyboardToKeyboard);
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

} // Rosen
} // OHOS