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
#include "cstdint"
#include "functional"
#include "oh_window_comm.h"
#include "oh_window_event_filter.h"
#include "key_event.h"
#include "window.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

using namespace OHOS::Rosen;

KeyEventFilterFunc convert2Func(OH_NativeWindowManager_KeyEventFilter filter)
{
    std::function<bool(OHOS::MMI::KeyEvent&)> func = [filter](OHOS::MMI::KeyEvent& keyEvent) {
        Input_KeyEvent *input = OH_Input_CreateKeyEvent();
        OH_Input_SetKeyEventKeyCode(input, keyEvent.GetKeyCode());
        OH_Input_SetKeyEventAction(input, keyEvent.GetKeyAction());
        OH_Input_SetKeyEventActionTime(input, keyEvent.GetActionTime());
        return filter(input);
    };
    return func ;
}

WindowManager_ErrorCode OH_NativeWindowManager_RegisterKeyEventFilter(int32_t windowId,
    OH_NativeWindowManager_KeyEventFilter filter)
{
    TLOGI(WmsLogTag::WMS_EVENT, "register keyEventCallback, windowId:%{public}d", windowId);
    auto mainWindow = OHOS::Rosen::Window::GetWindowWithId(windowId);
    if (mainWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "window is null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::INVAILD_WINDOW_ID;
    }
    auto res = mainWindow->SetKeyEventFilter(convert2Func(filter));

    return res == WMError::WM_OK ? WindowManager_ErrorCode::OK : WindowManager_ErrorCode::SERVICE_ERROR;
}

WindowManager_ErrorCode OH_NativeWindowManager_UnregisterKeyEventFilter(int32_t windowId)
{
    TLOGI(WmsLogTag::WMS_EVENT, "clear keyEventCallback, windowId:%{public}d", windowId);
    auto mainWindow = OHOS::Rosen::Window::GetWindowWithId(windowId);
    if (mainWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "window is null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::INVAILD_WINDOW_ID;
    }
    auto res = mainWindow->ClearKeyEventFilter();
    return res == WMError::WM_OK ? WindowManager_ErrorCode::OK : WindowManager_ErrorCode::SERVICE_ERROR;
}
