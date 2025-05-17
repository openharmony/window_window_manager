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
 * @addtogroup OH_PictureInPicture
 * @file oh_window_pip.h
 *
 * @brief Declares C APIs for picture in picture window
 *
 * @kit ArkUI
 * @library libnative_window_manager.so
 * @syscap SystemCapability.Window.SessionManager
 * @since 20
 * @version 1.0
 */
#ifndef OH_WINDOW_PIP_H
#define OH_WINDOW_PIP_H

#include "napi/native_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumerates picture in picture template type.
 * @since 20
 */
typedef enum {
    VIDEO_PLAY = 0,
    VIDEO_CALL = 1,
    VIDEO_MEETING = 2,
    VIDEO_LIVE = 3,
} PictureInPicture_PiPTemplateType;

/**
 * @brief Enumerates picture in picture control group.
 * @since 20
 */
typedef enum {
    VIDEO_PLAY_VIDEO_PREVIOUS_NEXT = 101,
    VIDEO_PLAY_FAST_FORWARD_BACKWARD = 102,
    VIDEO_CALL_MICROPHONE_SWITCH = 201,
    VIDEO_CALL_HANG_UP_BUTTON = 202,
    VIDEO_CALL_CAMERA_SWITCH = 203,
    VIDEO_CALL_MUTE_SWITCH = 204,
    VIDEO_MEETING_HANG_UP_BUTTON = 301,
    VIDEO_MEETING_CAMERA_SWITCH = 302,
    VIDEO_MEETING_MUTE_SWITCH = 303,
    VIDEO_MEETING_MICROPHONE_SWITCH = 304,
    VIDEO_LIVE_VIDEO_PLAY_PAUSE = 401,
    VIDEO_LIVE_MUTE_SWITCH = 402,
} PictureInPicture_PiPControlGroup;

/**
 * @brief Enumerates picture in picture control type.
 * @since 20
 */
typedef enum {
    VIDEO_PLAY_PAUSE = 0,
    VIDEO_PREVIOUS = 1,
    VIDEO_NEXT = 2,
    FAST_FORWARD = 3,
    FAST_BACKWARD = 4,
    HANG_UP_BUTTON = 5,
    MICROPHONE_SWITCH = 6,
    CAMERA_SWITCH = 7,
    MUTE_SWITCH = 8,
    END,
} PictureInPicture_PiPControlType;

/**
 * @brief Enumerates picture in picture control status.
 * @since 20
 */
typedef enum {
    PLAY = 1,
    PAUSE = 0,
    OPEN = 1,
    CLOSE = 0,
} PictureInPicture_PiPControlStatus;

/**
 * @brief Enumerates picture in picture state.
 * @since 20
 */
typedef enum {
    ABOUT_TO_START = 1,
    STARTED = 2,
    ABOUT_TO_STOP = 3,
    STOPPED = 4,
    ABOUT_TO_RESTORE = 5,
    ERROR = 6,
} PictureInPicture_PiPState;

/**
 * @brief Defines the PiP config structure.
 *
 * @since 20
 */
typedef struct {
    /** WindowId of Corresponding mainWindow. */
    uint32_t mainWindowId;
    /** The picture-in-picture template type */
    PictureInPicture_PiPTemplateType pipTemplateType;
    /** The picture-in-picture content width */
    uint32_t width;
    /** The picture-in-picture content height */
    uint32_t height;
    /** The picture-in-picture control group */
    PictureInPicture_PiPControlGroup* controlGroup;
    /** The length of picture-in-picture control group */
    uint8_t controlGroupLength;
    /** The application environment */
    napi_env env;
} PictureInPicture_PiPConfig;

/**
 * @brief Start the picture-in-picture callback
 * @param requestId The picture-in-picture requestId
 * @param surfaceId The picture-in-picture surfaceId
 * @since 20
 */
typedef void (*WebPipStartPipCallback)(uint32_t controllerId, uint8_t requestId, uint64_t surfaceId);

/**
 * @brief The picture-in-picture lifecycle callback
 * @param state The picture-in-picture state
 * @since 20
 */
typedef void (*WebPipLifecycleCallback)(uint32_t controllerId, PictureInPicture_PiPState state, int32_t errcode);

/**
 * @brief The picture-in-picture control event callback
 * @param controlType The picture-in-picture control type
 * @param status The picture-in-picture control status
 * @since 20
 */
typedef void (*WebPipControlEventCallback)(uint32_t controllerId, PictureInPicture_PiPControlType controlType,
    PictureInPicture_PiPControlStatus status);

/**
 * @brief The picture-in-picture size change callback
 * @param width The picture-in-picture window width
 * @param height The picture-in-picture window height
 * @param scale The picture-in-picture window scale
 * @since 20
 */
typedef void (*WebPipResizeCallback)(uint32_t controllerId, uint32_t width, uint32_t height, double scale);

/**
 * @brief Create picture-in-picture controller.
 *
 * @param pipConfig The picture-in-picture configuration
 * @param controllerId The picture-in-picture controller ID
 * @return Return the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM} parameter error.
 *         {@link WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORTED} capability not supported.
 *         {@link WINDOW_MANAGER_ERRORCODE_PIP_INTERNAL_ERROR} pip internal error.
 * @since 20
 */
int32_t OH_PictureInPicture_CreatePip(PictureInPicture_PiPConfig* pipConfig, uint32_t* controllerId);

/**
 * @brief Delete picture-in-picture controller.
 *
 * @param controllerId The picture-in-picture controller ID
 * @return Return the result code.
 *         {@link OK} The function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM} parameter error.
 * @since 20
 */
int32_t OH_PictureInPicture_DeletePip(uint32_t controllerId);

/**
 * @brief Start picture-in-picture.
 *
 * @param controllerId The picture-in-picture controller ID
 * @return Return the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_PIP_STATE_ABNORMAL} the PiP window state is abnormal.
 *         {@link WINDOW_MANAGER_ERRORCODE_PIP_CREATE_FAILED} failed to create the PiP window.
 *         {@link WINDOW_MANAGER_ERRORCODE_ERRORCODE_PIP_INTERNAL_ERROR} pip internal error.
 *         {@link WINDOW_MANAGER_ERRORCODE_PIP_REPEAT_OPERATION} repeated PiP operation.
 * @since 20
 */
int32_t OH_PictureInPicture_StartPip(uint32_t controllerId);

/**
 * @brief Stop picture-in-picture.
 *
 * @param controllerId The picture-in-picture controller ID
 * @return Return the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_PIP_DESTROY_FAILED} failed to destroy the PiP window.
 *         {@link WINDOW_MANAGER_ERRORCODE_PIP_STATE_ABNORMAL} the PiP window state is abnormal.
 *         {@link WINDOW_MANAGER_ERRORCODE_ERRORCODE_PIP_INTERNAL_ERROR} pip internal error.
 *         {@link WINDOW_MANAGER_ERRORCODE_PIP_REPEAT_OPERATION} repeated PiP operation.
 * @since 20
 */
int32_t OH_PictureInPicture_StopPip(uint32_t controllerId);

/**
 * @brief Update picture-in-picture content size.
 *
 * @param controllerId The picture-in-picture controller ID
 * @param width The picture-in-picture content width
 * @param height The picture-in-picture content height
 * @since 20
 */
void OH_PictureInPicture_UpdatePipContentSize(uint32_t controllerId, uint32_t width, uint32_t height);

/**
 * @brief Update picture-in-picture control status.
 *
 * @param controllerId The picture-in-picture controller ID
 * @param controlType The picture-in-picture control type.
 * @param status The picture-in-picture control status.
  * @since 20
 */
void OH_PictureInPicture_UpdatePipControlStatus(uint32_t controllerId, PictureInPicture_PiPControlType controlType,
    PictureInPicture_PiPControlStatus status);

/**
 * @brief Set picture-in-picture controll enable status.
 *
 * @param controllerId The picture-in-picture controller ID
 * @param controlType The picture-in-picture control type.
 * @param enabled Indicate the picture-in-picture control is enabled.
 * @since 20
 */
void OH_PictureInPicture_SetPiPControlEnabled(uint32_t controllerId, PictureInPicture_PiPControlType controlType,
    bool enabled);

/**
 * @brief Register picture-in-picture controller start callback.
 *
 * @param controllerId The picture-in-picture controller ID
 * @param callback Start the picture-in-picture callback
 * @return Return the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM} parameter error.
 *         {@link WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORTED} capability not supported.
 *         {@link WINDOW_MANAGER_ERRORCODE_PIP_INTERNAL_ERROR} pip internal error.
 * @since 20
 */
int32_t OH_PictureInPicture_RegisterStartPipCallback(uint32_t controllerId, WebPipStartPipCallback callback);

/**
 * @brief Unregister picture-in-picture controller start callback.
 *
 * @param controllerId The picture-in-picture controller ID
 * @param callback Start the picture-in-picture callback
 * @return Return the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM} parameter error.
 *         {@link WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORTED} capability not supported.
 *         {@link WINDOW_MANAGER_ERRORCODE_PIP_INTERNAL_ERROR} pip internal error.
 * @since 20
 */
int32_t OH_PictureInPicture_UnregisterStartPipCallback(uint32_t controllerId, WebPipStartPipCallback callback);

/**
 * @brief Unregister all picture-in-picture controller start callbacks.
 *
 * @param controllerId The picture-in-picture controller ID
 * @return Return the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM} parameter error.
 *         {@link WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORTED} capability not supported.
 *         {@link WINDOW_MANAGER_ERRORCODE_PIP_INTERNAL_ERROR} pip internal error.
 * @since 20
 */
int32_t OH_PictureInPicture_UnregisterAllStartPipCallbacks(uint32_t controllerId);

/**
 * @brief Register picture-in-picture life cycle listener callback.
 *
 * @param controllerId The picture-in-picture controller ID
 * @param callback The picture-in-picture lifecycle callback.
 * @return Return the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM} parameter error.
 *         {@link WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORTED} capability not supported.
 *         {@link WINDOW_MANAGER_ERRORCODE_PIP_INTERNAL_ERROR} pip internal error.
 * @since 20
 */
int32_t OH_PictureInPicture_RegisterLifecycleListener(uint32_t controllerId, WebPipLifecycleCallback callback);

/**
 * @brief Unregister picture-in-picture life cycle listener callback.
 *
 * @param controllerId The picture-in-picture controller ID
 * @param callback The picture-in-picture lifecycle callback.
 * @return Return the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM} parameter error.
 *         {@link WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORTED} capability not supported.
 *         {@link WINDOW_MANAGER_ERRORCODE_PIP_INTERNAL_ERROR} pip internal error.
 * @since 20
 */
int32_t OH_PictureInPicture_UnregisterLifecycleListener(uint32_t controllerId, WebPipLifecycleCallback callback);

/**
 * @brief Unregister all picture-in-picture life cycle listener callbacks.
 *
 * @param controllerId The picture-in-picture controller ID
 * @return Return the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM} parameter error.
 *         {@link WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORTED} capability not supported.
 *         {@link WINDOW_MANAGER_ERRORCODE_PIP_INTERNAL_ERROR} pip internal error.
 * @since 20
 */
int32_t OH_PictureInPicture_UnregisterAllLifecycleListeners(uint32_t controllerId);

/**
 * @brief Register picture-in-picture control event listener callback.
 *
 * @param controllerId The picture-in-picture controller ID
 * @param callback The picture-in-picture control event callback.
 * @return Return the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM} parameter error.
 *         {@link WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORTED} capability not supported.
 *         {@link WINDOW_MANAGER_ERRORCODE_PIP_INTERNAL_ERROR} pip internal error.
 * @since 20
 */
int32_t OH_PictureInPicture_RegisterControlEventListener(uint32_t controllerId, WebPipControlEventCallback callback);

/**
 * @brief Unregister picture-in-picture control event listener callback.
 *
 * @param controllerId The picture-in-picture controller ID
 * @param callback The picture-in-picture control event callback.
 * @return Return the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM} parameter error.
 *         {@link WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORTED} capability not supported.
 *         {@link WINDOW_MANAGER_ERRORCODE_PIP_INTERNAL_ERROR} pip internal error.
 * @since 20
 */
int32_t OH_PictureInPicture_UnregisterControlEventListener(uint32_t controllerId, WebPipControlEventCallback callback);

/**
 * @brief Unregister all picture-in-picture control event listener callbacks.
 *
 * @param controllerId The picture-in-picture controller ID
 * @return Return the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM} parameter error.
 *         {@link WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORTED} capability not supported.
 *         {@link WINDOW_MANAGER_ERRORCODE_PIP_INTERNAL_ERROR} pip internal error.
 * @since 20
 */
int32_t OH_PictureInPicture_UnregisterAllControlEventListeners(uint32_t controllerId);

/**
 * @brief Register picture-in-picture resize listener callback.
 *
 * @param controllerId The picture-in-picture controller ID
 * @param callback The picture-in-picture size change callback.
 * @return Return the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM} parameter error.
 *         {@link WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORTED} capability not supported.
 *         {@link WINDOW_MANAGER_ERRORCODE_PIP_INTERNAL_ERROR} pip internal error.
 * @since 20
 */
int32_t OH_PictureInPicture_RegisterResizeListener(uint32_t controllerId, WebPipResizeCallback callback);

/**
 * @brief Unregister picture-in-picture resize listener callback.
 *
 * @param controllerId The picture-in-picture controller ID
 * @param callback The picture-in-picture size change callback.
 * @return Return the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM} parameter error.
 *         {@link WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORTED} capability not supported.
 *         {@link WINDOW_MANAGER_ERRORCODE_PIP_INTERNAL_ERROR} pip internal error.
 * @since 20
 */
int32_t OH_PictureInPicture_UnregisterResizeListener(uint32_t controllerId, WebPipResizeCallback callback);

/**
 * @brief Unregister all picture-in-picture resize listener callbacks.
 *
 * @param controllerId The picture-in-picture controller ID
 * @return Return the result code.
 *         {@link OK} the function call is successful.
 *         {@link WINDOW_MANAGER_ERRORCODE_INCORRECT_PARAM} parameter error.
 *         {@link WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORTED} capability not supported.
 *         {@link WINDOW_MANAGER_ERRORCODE_PIP_INTERNAL_ERROR} pip internal error.
 * @since 20
 */
int32_t OH_PictureInPicture_UnregisterAllResizeListeners(uint32_t controllerId);

#ifdef __cplusplus
}
#endif

#endif // OH_WINDOW_PIP_H