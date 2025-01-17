/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

/**
 * @addtogroup WindowManager
 * @{
 *
 * @brief Provides abilities of window on the native side.
 * @since 16
 */

/**
 * @file oh_window.h
 *
 * @brief Declares APIs for window
 *
 * @library libnative_window_manager.so
 * @kit ArkUI
 * @syscap SystemCapability.Window.SessionManager
 * @since 16
 */
#ifndef OH_WINDOW_H
#define OH_WINDOW_H

#include "stdbool.h"
#include "stdint.h"

#include "oh_window_comm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set whether to show status bar.
 *
 * @param windowId WindowId when window is created.
 * @param enabled If true, the status bar is displayed. If false, the status bar is hidden.
 * @param enableAnimation If true, the status bar is displayed and hidden with animation.
 *                        If false, the status bar is displayed and hidden with no animation.
 * @return Return the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_INVALID_PARAM} parameter error.
 *         {@link WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORT} capability not supported.
 *         {@link WINDOW_MANAGER_ERRORCODE_STATE_ABNORMALLY} this window state is abnormal.
 *         {@link WINDOW_MANAGER_ERRORCODE_SYSTEM_ABNORMALLY} the window manager service works abnormal.
 * @since 16
 */
WindowManager_ErrorCode OH_Window_SetWindowStatusBarEnabled(int32_t windowId, bool enabled, bool enableAnimation);

/**
 * @brief Set status bar content color.
 *
 * @param windowId WindowId when window is created.
 * @param color The color value to set, the format is ARGB.
 * @return Return the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_INVALID_PARAM} parameter error.
 *         {@link WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORT} capability not supported.
 *         {@link WINDOW_MANAGER_ERRORCODE_STATE_ABNORMALLY} this window state is abnormal.
 *         {@link WINDOW_MANAGER_ERRORCODE_SYSTEM_ABNORMALLY} the window manager service works abnormal.
 * @since 16
 */
WindowManager_ErrorCode OH_Window_SetWindowStatusBarColor(int32_t windowId, int32_t color);

/**
 * @brief Set whether to show navigation bar.
 *
 * @param windowId WindowId when window is created.
 * @param enabled If true, the navigation bar is displayed. If false, the navigation bar is hidden.
 * @param enableAnimation If true, the navigation bar is displayed and hidden with animation.
 *                        If false, the navigation bar is displayed and hidden with no animation.
 * @return Return the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_INVALID_PARAM} parameter error.
 *         {@link WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORT} capability not supported.
 *         {@link WINDOW_MANAGER_ERRORCODE_STATE_ABNORMALLY} this window state is abnormal.
 *         {@link WINDOW_MANAGER_ERRORCODE_SYSTEM_ABNORMALLY} the window manager service works abnormal.
 * @since 16
 */
WindowManager_ErrorCode OH_Window_SetWindowNavigationBarEnabled(int32_t windowId, bool enabled, bool enableAnimation);

/**
 * @brief Get the avoid area
 *
 * @param windowId WindowId when window is created.
 * @param type Type of the avoid area.
 * @param avoidArea Indicates the pointer to a WindowManager_AvoidArea object.
 * @return Returns the status code of the execution.
 * @return Return the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_INVALID_PARAM} parameter error.
 *         {@link WINDOW_MANAGER_ERRORCODE_STATE_ABNORMALLY} this window state is abnormal.
 *         {@link WINDOW_MANAGER_ERRORCODE_SYSTEM_ABNORMALLY} the window manager service works abnormal.
 * @since 16
 */
WindowManager_ErrorCode OH_Window_GetWindowAvoidArea(
    int32_t windowId, WindowManager_AvoidAreaType type, WindowManager_AvoidArea* avoidArea);

#ifdef __cplusplus
}
#endif

#endif // OH_WINDOW_H
