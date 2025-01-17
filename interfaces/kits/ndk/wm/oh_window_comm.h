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

/**
 * @addtogroup WindowManager
 * @{
 *
 *
 * @brief Provides abilities of windowManager on the native side, such as key event
 * filtration.
 *
 * @since 12
 */

/**
 * @file oh_window_comm.h
 *
 * @brief Provides the comm type definitions of windowManager on the native side.
 *
 * @syscap SystemCapability.Window.SessionManager
 * @library libnative_window_manager.so
 * @kit ArkUI
 * @since 12
 */
#ifndef OH_WINDOW_COMM_H
#define OH_WINDOW_COMM_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumerates the result types of the wm interface
 *
 * @since 12
 */
typedef enum {
    /** succ. */
    OK = 0,
    INVALID_PARAM = 401,
    /** device not support. */
    DEVICE_NOT_SUPPORT = 801,
    /** window id is invaild. */
    INVAILD_WINDOW_ID = 1000,
    /** failed. */
    SERVICE_ERROR = 2000,
} WindowManager_ErrorCode;

/**
 * @brief Enumerates the system bar types.
 *
 * @since 16
 */
typedef enum {
    STATUS_BAR = 0,
    AI_NAVIGATION_INDICATOR = 1,
} WindowManager_SystemBarType;

/**
 * @brief Enumerates the avoid area types.
 *
 * @since 16
 */
typedef enum {
    TYPE_SYSTEM = 0,
    TYPE_CUTOUT = 1,
    TYPE_SYSTEM_GESTURE = 2,
    TYPE_KEYBOARD = 3,
    TYPE_NAVIGATION_INDICATOR = 4,
} WindowManager_AvoidAreaType;

/**
 * @brief The type of a window
 *
 * @since 16
 */
typedef enum {
    /** App */
    TYPE_APP = 0,
    /** System alert */
    TYPE_SYSTEM_ALERT = 1,
    /** Float */
    TYPE_FLOAT = 8,
    /** Dialog */
    TYPE_DIALOG = 16,
} WindowManager_WindowType;

/**
 * @brief Defines the window rect data structure.
 *
 * @since 16
 */
typedef struct {
    int32_t posX;
    int32_t posY;
    uint32_t width;
    uint32_t height;
} WindowManager_Rect;

/**
 * @brief Properties of window
 *
 * @since 16
*/
typedef struct {
    /** The position and size of the window */
    WindowManager_Rect windowRect;
    /** The position relative to the window and size of drawable area */
    WindowManager_Rect drawableRect;
    /** Window type */
    WindowManager_WindowType type;
    /** Whether the window is displayed in full screen mode. The default value is false */
    bool isFullScreen;
    /** Whether the window layout is in full screen mode(whether the window is immersive). The default value is false */
    bool isLayoutFullScreen;
    /** Whether the window can gain focus. The default value is true */
    bool focusable;
    /** Whether the window is touchable. The default value is false */
    bool touchable;
    /** Brightness value of window */
    float brightness;
    /** Whether keep screen on */
    bool isKeepScreenOn;
    /** Whether make window in privacy mode or not */
    bool isPrivacyMode;
    /** Whether is transparent or not */
    bool isTransparent;
    /** Window id */
    uint32_t id;
    /** display id */
    uint32_t displayId;
} WindowManager_WindowProperties;

/**
 * @brief The native pixel map information defined by Image Kit.
 *
 * @since 16
 */
struct OH_PixelmapNative;

/**
 * @brief Defines the system bar property data structure.
 *
 * @since 16
 */
typedef struct {
    bool enable;
    bool enableAnimation;
    int32_t backgroundColor;
    int32_t contentColor;
} WindowManager_SystemBarProperty;

/**
 * @brief Defines the avoid area data structure.
 *
 * @since 16
 */
typedef struct {
    WindowManager_Rect topRect;
    WindowManager_Rect leftRect;
    WindowManager_Rect rightRect;
    WindowManager_Rect bottomRect;
} WindowManager_AvoidArea;

#ifdef __cplusplus
}
#endif

#endif // OH_WINDOW_COMM_H
/** @} */