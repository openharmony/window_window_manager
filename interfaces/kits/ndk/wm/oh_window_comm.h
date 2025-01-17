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
    /** param is invaild. */
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