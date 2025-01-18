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
    /** 
     * Param is invaild.
     * 
     * @since 16
     */
    WINDOW_MANAGER_ERRORCODE_INVALID_PARAM = 401,
    /** 
     * Device not support.
     * 
     * @since 16
     */
    WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORT = 801,
    /** window id is invaild. */
    INVAILD_WINDOW_ID = 1000,
    /** failed. */
    SERVICE_ERROR = 2000,
    /** 
     * Window state is abnormal.
     * 
     * @since 16
     */
    WINDOW_MANAGER_ERRORCODE_STATE_ABNORMALLY = 1300002,
    /** 
     * Window state is abnormal.
     * 
     * @since 16
     */
    WINDOW_MANAGER_ERRORCODE_SYSTEM_ABNORMALLY = 1300003,
} WindowManager_ErrorCode;

/**
 * @brief Enumerates the avoid area types.
 *
 * @since 16
 */
typedef enum {
    /** System. */
    WINDOW_MANAGER_AVOID_AREA_TYPE_SYSTEM = 0,
    /** Cutout. */
    WINDOW_MANAGER_AVOID_AREA_TYPE_CUTOUT = 1,
    /** System gesture. */
    WINDOW_MANAGER_AVOID_AREA_TYPE_SYSTEM_GESTURE = 2,
    /** Keyboard. */
    WINDOW_MANAGER_AVOID_AREA_TYPE_KEYBOARD = 3,
    /** Navigation indicator. */
    WINDOW_MANAGER_AVOID_AREA_TYPE_NAVIGATION_INDICATOR = 4,
} WindowManager_AvoidAreaType;

/**
 * @brief Defines the window rect data structure.
 *
 * @since 16
 */
typedef struct {
    /** X-axis of the window. */
    int32_t posX;
    /** Y-axis of the window. */
    int32_t posY;
    /** Width of the window. */
    uint32_t width;
    /** Height of the window. */
    uint32_t height;
} WindowManager_Rect;

/**
 * @brief Defines the avoid area data structure.
 *
 * @since 16
 */
typedef struct {
    /** Top rect of the avoid area. */
    WindowManager_Rect topRect;
    /** Left rect of the avoid area. */
    WindowManager_Rect leftRect;
    /** Right rect of the avoid area. */
    WindowManager_Rect rightRect;
    /** Bottom rect of the avoid area. */
    WindowManager_Rect bottomRect;
} WindowManager_AvoidArea;

#ifdef __cplusplus
}
#endif

#endif // OH_WINDOW_COMM_H