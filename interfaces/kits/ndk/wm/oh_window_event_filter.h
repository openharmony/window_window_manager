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

#ifndef OH_WINDOW_EVENT_FILTER_H
#define OH_WINDOW_EVENT_FILTER_H

#include "oh_input_manager.h"
#include "key_event.h"
#include "oh_window_comm.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief the callback funcation type when keyEvent was filter
 * @param keyEvent multimodal keyEvent
 * @since 12
 */
typedef bool (*OH_NativeWindowManager_KeyEventFilter)(Input_KeyEvent* keyEvent);

/**
 * @brief Registers a filter callback for the window, the callback is called when the
 * window is dispatched to the event
 *
 * @param windowId windowId when window is created
 * @param keyEventFilter key event callback ,called when the window is dispatched
 * to the event
 * @return Returns the status code of the execution.
 * @since 12
 */
WindowManager_ErrorCode OH_NativeWindowManager_RegisterKeyEventFilter(int32_t windowId,
    OH_NativeWindowManager_KeyEventFilter keyEventFilter);

/**
 * @brief clear callback for the window
 *
 * @param windowId windowId when window is created
 * @return Returns the status code of the execution.
 * @since 12
 */
WindowManager_ErrorCode OH_NativeWindowManager_UnregisterKeyEventFilter(int32_t windowId);

/**
 * @brief the callback function type when mouseEvent was filter
 * @param mouseEvent multimodal mouseEvent
 * @since 15
 */
typedef bool (*OH_NativeWindowManager_MouseEventFilter)(Input_MouseEvent* mouseEvent);

/**
 * @brief Registers a filter callback for the window ,the callback is called when the
 * window is dispatched to the event
 *
 * @param windowId windowId when window is created
 * @param mouseEventFilter mouse event callback ,called when the window is dispatched
 * to the event
 * @return Returns the status code of the execution.
 * @since 15
 */
WindowManager_ErrorCode OH_NativeWindowManager_RegisterMouseEventFilter(int32_t windowId,
    OH_NativeWindowManager_MouseEventFilter mouseEventFilter);

/**
 * @brief clear callback for the window
 *
 * @param windowId windowId when window is created
 * @return Returns the status code of the execution.
 * @since 15
 */
WindowManager_ErrorCode OH_NativeWindowManager_UnregisterMouseEventFilter(int32_t windowId);

/**
 * @brief the callback function type when touchEvent was filter
 * @param touchEvent multimodal touchEvent
 * @since 15
 */
typedef bool (*OH_NativeWindowManager_TouchEventFilter)(Input_TouchEvent* touchEvent);

/**
 * @brief Registers a filter callback for the window ,the callback is called when the
 * window is dispatched to the event
 *
 * @param windowId windowId when window is created
 * @param touchEventFilter touch event callback ,called when the window is dispatched
 * to the event
 * @return Returns the status code of the execution.
 * @since 15
 */
WindowManager_ErrorCode OH_NativeWindowManager_RegisterTouchEventFilter(int32_t windowId,
    OH_NativeWindowManager_TouchEventFilter touchEventFilter);

/**
 * @brief clear callback for the window
 *
 * @param windowId windowId when window is created
 * @return Returns the status code of the execution.
 * @since 15
 */
WindowManager_ErrorCode OH_NativeWindowManager_UnregisterTouchEventFilter(int32_t windowId);

#ifdef __cplusplus
}
#endif
#endif // OH_WINDOW_EVENT_FILTER_H
