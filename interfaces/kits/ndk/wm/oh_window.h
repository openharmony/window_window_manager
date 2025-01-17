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
 * @since 16
 */
WindowManager_ErrorCode OH_Window_SetWindowStatusBarEnabled(int32_t windowId, bool enabled, bool enableAnimation);

/**
 * @brief Set status bar content color.
 *
 * @param windowId WindowId when window is created.
 * @param color The color value to set, the format is ARGB.
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
 * @since 16
 */
WindowManager_ErrorCode OH_Window_GetWindowAvoidArea(
    int32_t windowId, WindowManager_AvoidAreaType type, WindowManager_AvoidArea* avoidArea);

/**
 * @brief Checks whether the window is displayed.
 *
 * @param windowId WindowId when window is created.
 * @param isShow Whether the window is displayed. The value true means that the window is displayed, and false means the opposite.
 * @return Returns the status code of the execution.
 * @since 16
 */
WindowManager_ErrorCode OH_Window_IsWindowShowing(int32_t windowId, bool* isShow);

/**
 * @brief Show window.
 *
 * @param windowId WindowId when window is created.
 * @return Returns the status code of the execution.
 * @since 16
 */
WindowManager_ErrorCode OH_Window_ShowWindow(int32_t windowId);

/**
 * @brief Set window touchable
 *
 * @param windowId WindowId when window is created.
 * @param isTouchable Indicates whether the specified window can be touched
 * @return Returns the status code of the execution
 * @since 16
 */
WindowManager_ErrorCode OH_Window_SetWindowTouchable(int32_t windowId, bool isTouchable);

/**
 * @brief Set focusable property of window.
 *
 * @param windowId WindowId when window is created.
 * @param isFocusable Window can be focused or not.
 * @return Returns the status code of the execution
 * @since 16
 */
WindowManager_ErrorCode OH_Window_SetWindowFocusable(int32_t windowId, bool isFocusable);

/**
 * @brief Sets the background color of window.
 *
 * @param windowId WindowId when window is created.
 * @param color the specified color.
 * @return Returns the status code of the execution.
 * @since 16
 */
WindowManager_ErrorCode OH_Window_SetWindowBackgroundColor(int32_t windowId, const char* color);

/**
 * @brief Sets the brightness of window.
 *
 * @param windowId WindowId when window is created.
 * @param brightness the specified brightness value.
 * @return Returns the status code of the execution.
 * @since 16
 */
WindowManager_ErrorCode OH_Window_SetWindowBrightness(int32_t windowId, float brightness);

/**
 * @brief Sets whether keep screen on or not.
 *
 * @param windowId WindowId when window is created.
 * @param isKeepScreenOn keep screen on if true, or not if false.
 * @return Returns the status code of the execution.
 * @since 16
 */
WindowManager_ErrorCode OH_Window_SetWindowKeepScreenOn(int32_t windowId, bool isKeepScreenOn);

/**
 * @brief Sets whether is private mode or not.
 *
 * @param windowId WindowId when window is created.
 * @param isPrivacy In private mode if true, or not if false.
 * @return Returns the status code of the execution.
 * @since 16
 */
WindowManager_ErrorCode OH_Window_SetWindowPrivacyMode(int32_t windowId, bool isPrivacy);

/**
 * @brief Get the properties of current window.
 *
 * @param windowId WindowId when window is created.
 * @param windowProperties Properties of current window.
 * @return Returns the status code of the execution.
 * @since 16
 */
WindowManager_ErrorCode OH_Window_GetWindowProperties(int32_t windowId, WindowManager_WindowProperties* windowProperties);

/**
 * @brief Obtains snapshot of window.
 *
 * @param windowId windowId when window is created.
 * @param pixelMap snapshot of window.
 * @return Returns the status code of the execution.
 * @since 16
 */
WindowManager_ErrorCode OH_Window_Snapshot(int32_t windowId, OH_PixelmapNative* pixelMap);

#ifdef __cplusplus
}
#endif

#endif // OH_WINDOW_H
/** @} */
