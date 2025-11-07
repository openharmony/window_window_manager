/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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
import { AsyncCallback, BusinessError, Callback } from '@ohos.base';
import { LocalStorage } from '@ohos.arkui.stateManagement';
import { UIContext } from '@ohos.arkui.UIContext';
import BaseContext from 'application.BaseContext';
import { ColorMetrics } from '@ohos.arkui.node';

export type AsyncCallbackVoid = (err: BusinessError) => void;
export type WindowEventCallback = (data: window.WindowEventType) => void;
export type WindowSizeCallback = (data: window.Size) => void;
export type WindowStatusCallback = (data: window.WindowStatusType) => void;

namespace window {
  export interface Size {
    /**
     * The width of the window.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * The width of the window.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @since 10
     */
    /**
     * The width of the window.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 11
     */
    width: number;

    /**
     * The height of the window.
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * The height of the window.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @since 10
     */
    /**
     * The height of the window.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 11
     */
    height: number;
  }
  export enum AvoidAreaType {
    /**
     * Default area of the system
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * Default area of the system
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 11
     */
    /**
     * Default area of the system
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 12
     */
    TYPE_SYSTEM,

    /**
     * Notch
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * Notch
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 11
     */
    /**
     * Notch
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 12
     */
    TYPE_CUTOUT,

    /**
     * Area for system gesture
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 9
     */
    /**
     * Area for system gesture
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 11
     */
    /**
     * Area for system gesture
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 12
     */
    TYPE_SYSTEM_GESTURE,

    /**
     * Area for keyboard
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 9
     */
    /**
     * Area for keyboard
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 11
     */
    /**
     * Area for keyboard
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 12
     */
    TYPE_KEYBOARD,

    /**
     * Area for navigation indicator
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 11
     */
    /**
     * Area for navigation indicator
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 12
     */
    TYPE_NAVIGATION_INDICATOR
  }
  export interface AvoidArea {
    /**
     * Whether avoidArea is visible on screen
     *
     * @type { boolean }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 9
     */
    /**
     * Whether avoidArea is visible on screen
     *
     * @type { boolean }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 11
     */
    visible: boolean;

    /**
     * Rectangle on the left of the screen
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * Rectangle on the left of the screen
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 11
     */
    /**
     * Rectangle on the left of the screen
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 12
     */
    leftRect: Rect;

    /**
     * Rectangle on the top of the screen
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * Rectangle on the top of the screen
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 11
     */
    /**
     * Rectangle on the top of the screen
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 12
     */
    topRect: Rect;

    /**
     * Rectangle on the right of the screen
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * Rectangle on the right of the screen
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 11
     */
    /**
     * Rectangle on the right of the screen
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 12
     */
    rightRect: Rect;

    /**
     * Rectangle on the bottom of the screen
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * Rectangle on the bottom of the screen
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 11
     */
    /**
     * Rectangle on the bottom of the screen
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 12
     */
    bottomRect: Rect;
  }

  export class AvoidAreaInternal implements AvoidArea {
    visible: boolean;
    leftRect: Rect;
    topRect: Rect;
    rightRect: Rect;
    bottomRect: Rect;
  }

  export interface Rect {
    /**
     * The left of the Rect.
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * The left of the Rect.
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @since 10
     */
    /**
     * The left of the Rect.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 11
     */
    left: number;

    /**
     * The top of the Rect.
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * The top of the Rect.
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @since 10
     */
    /**
     * The top of the Rect.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 11
     */
    top: number;

    /**
     * The width of the Rect.
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * The width of the Rect.
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @since 10
     */
    /**
     * The width of the Rect.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 11
     */
    width: number;

    /**
     * The height of the Rect.
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * The height of the Rect.
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @since 10
     */
    /**
     * The height of the Rect.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 11
     */
    height: number;
  }

  export class RectInternal implements Rect {
    left: double;
    top: double;
    width: double;
    height: double;
  }

  export class SizeInternal implements Size {
    width: double;
    height: double;
  }

//enum WMError {
    const WMError_WM_OK = 0
    const WMError_WM_DO_NOTHING = 1
    const WMError_WM_ERROR_NO_MEM = 2
    const WMError_WM_ERROR_DESTROYED_OBJECT = 3
    const WMError_WM_ERROR_INVALID_WINDOW = 4
    const WMError_WM_ERROR_INVALID_WINDOW_MODE_OR_SIZE = 5
    const WMError_WM_ERROR_INVALID_OPERATION = 6
    const WMError_WM_ERROR_INVALID_PERMISSION = 7
    const WMError_WM_ERROR_NOT_SYSTEM_APP = 8
    const WMError_WM_ERROR_NO_REMOTE_ANIMATION = 9
    const WMError_WM_ERROR_INVALID_DISPLAY = 10
    const WMError_WM_ERROR_INVALID_PARENT = 11
    const WMError_WM_ERROR_OPER_FULLSCREEN_FAILED = 12
    const WMError_WM_ERROR_REPEAT_OPERATION = 13
    const WMError_WM_ERROR_INVALID_SESSION = 14
    const WMError_WM_ERROR_INVALID_CALLING = 15
    const WMError_WM_ERROR_SYSTEM_ABNORMALLY = 16

    const WMError_WM_ERROR_DEVICE_NOT_SUPPORT = 801 // the value do not change.It is defined on all system

    const WMError_WM_ERROR_NEED_REPORT_BASE = 1000 // error code > 1000 means need report
    const WMError_WM_ERROR_NULLPTR = 1001
    const WMError_WM_ERROR_INVALID_TYPE = 1002
    const WMError_WM_ERROR_INVALID_PARAM = 1003
    const WMError_WM_ERROR_SAMGR = 1004
    const WMError_WM_ERROR_IPC_FAILED = 1005
    const WMError_WM_ERROR_NEED_REPORT_END = 1006
    const WMError_WM_ERROR_START_ABILITY_FAILED = 1007
    const WMError_WM_ERROR_PIP_DESTROY_FAILED = 1008
    const WMError_WM_ERROR_PIP_STATE_ABNORMALLY = 1009
    const WMError_WM_ERROR_PIP_CREATE_FAILED = 1010
    const WMError_WM_ERROR_PIP_INTERNAL_ERROR = 1011
    const WMError_WM_ERROR_PIP_REPEAT_OPERATION = 1012
//}

//enum WmErrorCode {
    const WmErrorCode_WM_OK = 0
    const WmErrorCode_WM_ERROR_NO_PERMISSION = 201
    const WmErrorCode_WM_ERROR_NOT_SYSTEM_APP = 202
    const WmErrorCode_WM_ERROR_INVALID_PARAM = 401
    const WmErrorCode_WM_ERROR_DEVICE_NOT_SUPPORT = 801
    const WmErrorCode_WM_ERROR_REPEAT_OPERATION = 1300001
    const WmErrorCode_WM_ERROR_STATE_ABNORMALLY = 1300002
    const WmErrorCode_WM_ERROR_SYSTEM_ABNORMALLY = 1300003
    const WmErrorCode_WM_ERROR_INVALID_CALLING = 1300004
    const WmErrorCode_WM_ERROR_STAGE_ABNORMALLY = 1300005
    const WmErrorCode_WM_ERROR_CONTEXT_ABNORMALLY = 1300006
    const WmErrorCode_WM_ERROR_START_ABILITY_FAILED = 1300007
    const WmErrorCode_WM_ERROR_INVALID_DISPLAY = 1300008
    const WmErrorCode_WM_ERROR_INVALID_PARENT = 1300009
    const WmErrorCode_WM_ERROR_OPER_FULLSCREEN_FAILED = 1300010
    const WmErrorCode_WM_ERROR_PIP_DESTROY_FAILED = 1300011
    const WmErrorCode_WM_ERROR_PIP_STATE_ABNORMALLY = 1300012
    const WmErrorCode_WM_ERROR_PIP_CREATE_FAILED = 1300013
    const WmErrorCode_WM_ERROR_PIP_INTERNAL_ERROR = 1300014
    const WmErrorCode_WM_ERROR_PIP_REPEAT_OPERATION = 1300015
//}

export enum ColorSpace {
    DEFAULT = 0,
    WIDE_GAMUT = 1
}

export enum MaximizePresentation {
    FOLLOW_APP_IMMERSIVE_SETTING = 0,
    EXIT_IMMERSIVE = 1,
    ENTER_IMMERSIVE = 2,
    ENTER_IMMERSIVE_DISABLE_TITLE_AND_DOCK_HOVER = 3
}

export enum WindowStatusType {
    UNDEFINED = 0,
    FULL_SCREEN = 1,
    MAXIMIZE = 2,
    MINIMIZE = 3,
    FLOATING = 4,
    SPLIT_SCREEN = 5
}

export enum WindowStageEventType {
    SHOWN = 1,
    ACTIVE = 2,
    INACTIVE = 3,
    HIDDEN = 4,
    RESUMED = 5,
    PAUSED = 6
}

export enum WindowType {
    TYPE_APP = 0,
    TYPE_SYSTEM_ALERT = 1,
    TYPE_INPUT_METHOD = 2,
    TYPE_STATUS_BAR = 3,
    TYPE_PANEL = 4,
    TYPE_KEYGUARD = 5,
    TYPE_VOLUME_OVERLAY = 6,
    TYPE_NAVIGATION_BAR = 7,
    TYPE_FLOAT = 8,
    TYPE_WALLPAPER = 9,
    TYPE_DESKTOP = 10,
    TYPE_LAUNCHER_RECENT = 11,
    TYPE_LAUNCHER_DOCK = 12,
    TYPE_VOICE_INTERACTION = 13,
    TYPE_POINTER = 14,
    TYPE_FLOAT_CAMERA = 15,
    TYPE_DIALOG = 16,
    TYPE_SCREENSHOT = 17,
    TYPE_SYSTEM_TOAST = 18,
    TYPE_DIVIDER = 19,
    TYPE_GLOBAL_SEARCH = 20,
    TYPE_HANDWRITE = 21
}

export enum Orientation {
    UNSPECIFIED = 0,
    PORTRAIT = 1,
    LANDSCAPE = 2,
    PORTRAIT_INVERTED = 3,
    LANDSCAPE_INVERTED = 4,
    AUTO_ROTATION = 5,
    AUTO_ROTATION_PORTRAIT = 6,
    AUTO_ROTATION_LANDSCAPE = 7,
    AUTO_ROTATION_RESTRICTED = 8,
    AUTO_ROTATION_PORTRAIT_RESTRICTED = 9,
    AUTO_ROTATION_LANDSCAPE_RESTRICTED = 10,
    LOCKED = 11,
    AUTO_ROTATION_UNSPECIFIED = 12,
    USER_ROTATION_PORTRAIT = 13,
    USER_ROTATION_LANDSCAPE = 14,
    USER_ROTATION_PORTRAIT_INVERTED = 15,
    USER_ROTATION_LANDSCAPE_INVERTED = 16,
    FOLLOW_DESKTOP = 17
}

export enum WindowEventType {
    WINDOW_SHOWN = 1,
    WINDOW_ACTIVE = 2,
    WINDOW_INACTIVE = 3,
    WINDOW_HIDDEN = 4,
    WINDOW_DESTROYED = 7
}

export enum WindowStageLifeCycleEventType {
    FOREGROUND = 1,
    RESUMED = 2,
    PAUSED = 3,
    BACKGROUND = 4,
}

export enum WindowMode {
    UNDEFINED = 1,
    FULLSCREEN = 2,
    PRIMARY = 3,
    SECONDARY = 4,
    FLOATING = 5
}

export enum ModalityType {
    WINDOW_MODALITY = 0,
    APPLICATION_MODALITY = 1
}

export interface SystemBarProperties {
    /**
     * The color of the status bar.
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 6
     */
    /**
     * The color of the status bar.
     *
     * @type { ?string }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    statusBarColor: string;

    /**
     * The light icon of the status bar.
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * The light icon of the status bar.
     *
     * @type { ?boolean }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    isStatusBarLightIcon: boolean;

    /**
     * The content color of the status bar
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 8
     */
    /**
     * The content color of the status bar
     *
     * @type { ?string }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    statusBarContentColor: string;

    /**
     * The color of the navigation bar.
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 6
     */
    /**
     * The color of the navigation bar.
     *
     * @type { ?string }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    navigationBarColor: string;

    /**
     * The light icon of the navigation bar.
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * The light icon of the navigation bar.
     *
     * @type { ?boolean }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    isNavigationBarLightIcon: boolean;

    /**
     * The content color of the navigation bar
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 8
     */
    /**
     * The content color of the navigation bar
     *
     * @type { ?string }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    navigationBarContentColor: string;

    /**
     * Enable the animation of the status bar.
     *
     * @type { ?boolean }
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    enableStatusBarAnimation: boolean;

    /**
     * Enable the animation of the navigation bar.
     *
     * @type { ?boolean }
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    enableNavigationBarAnimation: boolean;
}

export class SystemBarPropertiesInternal implements SystemBarProperties {
    statusBarColor: string = "";
    isStatusBarLightIcon: boolean;
    statusBarContentColor: string = "";
    navigationBarColor: string = "";
    isNavigationBarLightIcon: boolean;
    navigationBarContentColor: string = "";
    enableStatusBarAnimation: boolean;
    enableNavigationBarAnimation: boolean;
}

/**
 * Properties of window, it couldn't update automatically
 *
 * @interface WindowProperties
 * @syscap SystemCapability.WindowManager.WindowManager.Core
 * @since 6
 */
/**
 * Properties of window, it couldn't update automatically
 *
 * @interface WindowProperties
 * @syscap SystemCapability.WindowManager.WindowManager.Core
 * @crossplatform
 * @since 10
 */
/**
 * Properties of window, it couldn't update automatically
 *
 * @interface WindowProperties
 * @syscap SystemCapability.WindowManager.WindowManager.Core
 * @crossplatform
 * @atomicservice
 * @since 11
 */
export interface WindowProperties {
    /**
     * The position and size of the window
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * The position and size of the window
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @since 10
     */
    /**
     * The position and size of the window
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 11
     */
    windowRect: Rect;

    /**
     * The position relative to the window and size of drawable area
     *
     * @type { Rect } 
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 11
     */
    /**
     * The position relative to the window and size of drawable area
     *
     * @type { Rect } 
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    drawableRect: Rect;

    /**
     * Window type
     *
     * @type { WindowType }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * Window type
     *
     * @type { WindowType }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    type: WindowType;

    /**
     * Whether the window is displayed in full screen mode. The default value is false.
     *
     * @type { boolean }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 6
     */
    /**
     * Whether the window is displayed in full screen mode. The default value is false.
     *
     * @type { boolean }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    isFullScreen: boolean;

    /**
     * Whether the window layout is in full screen mode(whether the window is immersive). The default value is false.
     *
     * @type { boolean }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * Whether the window layout is in full screen mode(whether the window is immersive). The default value is false.
     *
     * @type { boolean }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    isLayoutFullScreen: boolean;

    /**
     * Whether the window can gain focus. The default value is true
     *
     * @type { boolean }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * Whether the window can gain focus. The default value is true
     *
     * @type { boolean }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    focusable: boolean;

    /**
     * Whether the window is touchable. The default value is false
     *
     * @type { boolean }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * Whether the window is touchable. The default value is false
     *
     * @type { boolean }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    touchable: boolean;

    /**
     * Brightness value of window.
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 6
     */
    /**
     * Brightness value of window.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @since 10
     */
    /**
     * Brightness value of window.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 11
     */
    brightness: number;

    /**
     * The dimbehind value of window.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     * @deprecated since 9
     */
    dimBehindValue: number;

    /**
     * Whether keep screen on.
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 6
     */
    /**
     * Whether keep screen on.
     *
     * @type { boolean }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @since 10
     */
    /**
     * Whether keep screen on.
     *
     * @type { boolean }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 11
     */
    isKeepScreenOn: boolean;

    /**
     * Whether make window in privacy mode or not.
     *
     * @type { boolean }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * Whether make window in privacy mode or not.
     *
     * @type { boolean }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    isPrivacyMode: boolean;

    /**
     * Whether is round corner or not.
     *
     * @type { boolean }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     * @deprecated since 9
     */
    isRoundCorner: boolean;

    /**
     * Whether is transparent or not.
     *
     * @type { boolean }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * Whether is transparent or not.
     *
     * @type { boolean }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    isTransparent: boolean;

    /**
     * Window id.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 9
     */
    /**
     * Window id.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    id: number;

    /**
     * display id.
     *
     * @type { ?number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    displayId: number;
}

export class WindowPropertiesInternal implements WindowProperties {
    windowRect: Rect;
    drawableRect: Rect;
    type: WindowType;
    isFullScreen: boolean;
    isLayoutFullScreen: boolean;
    focusable: boolean;
    touchable: boolean;
    brightness: number;
    dimBehindValue: number;
    isKeepScreenOn: boolean;
    isPrivacyMode: boolean;
    isRoundCorner: boolean;
    isTransparent: boolean;
    id: number;
    displayId: number;
    typeInternal: int;
}

export interface MoveConfiguration {
    displayId: number;
}

export interface WindowDensityInfo {
    systemDensity: number;
    defaultDensity: number;
    customDensity: number;
}

export interface AvoidAreaOptions {
    type: AvoidAreaType;
    area: AvoidArea;
}

export interface TransitionContext {
    toWindow: Window;
    completeTransition(isCompleted: boolean): void;
}

export interface TransitionController {
    animationForShown(context: window.TransitionContext): void;
    animationForHidden(context: window.TransitionContext): void;
}

/**
 * Limits of window.
 *
 * @interface WindowLimits
 * @syscap SystemCapability.Window.SessionManager
 * @since 11
 */
/**
 * Limits of window.
 *
 * @interface WindowLimits
 * @syscap SystemCapability.Window.SessionManager
 * @atomicservice
 * @since 12
 */
export interface WindowLimits {

    /**
     * The maximum width of the window.
     *
     * @type { ?number }
     * @syscap SystemCapability.Window.SessionManager
     * @since 11
     */
    /**
     * The maximum width of the window.
     *
     * @type { ?number }
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    maxWidth: number;

    /**
     * The maximum height of the window.
     *
     * @type { ?number }
     * @syscap SystemCapability.Window.SessionManager
     * @since 11
     */
    /**
     * The maximum height of the window.
     *
     * @type { ?number }
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    maxHeight: number;

    /**
     * The minimum width of the window.
     *
     * @type { ?number }
     * @syscap SystemCapability.Window.SessionManager
     * @since 11
     */
    /**
     * The minimum width of the window.
     *
     * @type { ?number }
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    minWidth: number;

    /**
     * The minimum height of the window.
     *
     * @type { ?number }
     * @syscap SystemCapability.Window.SessionManager
     * @since 11
     */
    /**
     * The minimum height of the window.
     *
     * @type { ?number }
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    minHeight: number;
}

export class WindowStageInternal implements WindowStage {
    static { loadLibrary('windowstageani_kit.z') }

    private nativeObj : long;
    setNativeObj(nativeObj: long):void {
        this.nativeObj = nativeObj;
    }

    public native loadContent(nativeObj:long, path: String): int;
    public native disableWindowDecorSync(nativeObj:long): void;
    public native setShowOnLockScreenSync(nativeObj:long, showOnLockScreen: boolean): void;
    public native getMainWindowSync(nativeObj: long): Window;

    public loadContent(path: string, callback: AsyncCallback<void>): void {
        // uicontent not support taskpool, must use ui thread
        let ret = this.loadContent(this.nativeObj, String(path));
        if (ret == WMError_WM_DO_NOTHING) {
            throw {code: WmErrorCode_WM_ERROR_STAGE_ABNORMALLY, message: 
                "This window state is abnormal."} as BusinessError;
        } else if (ret == WMError_WM_ERROR_INVALID_PARAM) {
            throw {code: WmErrorCode_WM_ERROR_INVALID_PARAM, message: 
                "Parameter error. Possible cause: 1. Mandatory parameters are left unspecified; " +
                "2. Incorrect parameter types."} as BusinessError;
        }
        callback({code: WmErrorCode_WM_OK, message: ""} as BusinessError, undefined);
    }

    public loadContentByName(name: string, callback: AsyncCallbackVoid): void {
        let ret = this.loadContent(this.nativeObj, name);
    }
  
    public disableWindowDecor(): void {
        this.disableWindowDecorSync(this.nativeObj);
    }

    public setShowOnLockScreen(showOnLockScreen: boolean): void {
        this.setShowOnLockScreenSync(this.nativeObj, showOnLockScreen);
    }

    public getMainWindowSync(): Window {
        let ret = this.getMainWindowSync(this.nativeObj);
        return ret;
    }

    public getMainWindow(): Promise<Window> {
        return new Promise<Window>((resolve: (value: Window) => void, reject: (error: BusinessError) => void ) => {
            taskpool.execute((): Window => {
                let window = this.getMainWindowSync(this.nativeObj);
                return window;
            }).then((ret: NullishType) => {
                resolve(ret as Window);
            }).catch((err: NullishType) => {
                reject(err as BusinessError);
            });
        });
    }

    public getMainWindow(callback: AsyncCallback<Window>): void {
        taskpool.execute((): Window => {
            let res = this.getMainWindowSync(this.nativeObj);
            return res;
        }).then((ret: NullishType) => {
            callback(new BusinessError(), ret as Window);
        }).catch((err: NullishType) => {
        });
    }
}

export interface WindowStage {
    loadContent(path: string, callback: AsyncCallback<void>): void;
    loadContentByName(name: string, callback: AsyncCallbackVoid): void;
    getMainWindowSync(): Window;
    getMainWindow(): Promise<Window>;
    getMainWindow(callback: AsyncCallback<Window>): void;
    disableWindowDecor(): void;
    setShowOnLockScreen(showOnLockScreen: boolean): void;
}

export class WindowInternal implements Window {
    static readonly DEFAULT_RET_VAL = 1;
    static { loadLibrary('windowstageani_kit.z') }
    private nativeObj: long;

    public setNativeObj(nativeObj: long): void {
        this.nativeObj = nativeObj;
    }

    private native resize(nativeObj: long, width: number, height: number): void;
    private native moveWindowTo(nativeObj: long, x: number, y: number): void;
    private native getGlobalRect(nativeObj:long): Rect;

    private native getWindowDecorHeight(nativeObj:long): double;
    private native setWindowBackgroundColor(nativeObj: long, color: string): int;
    private native setImmersiveModeEnabledState(nativeObj: long, enable: boolean): int;
    private native setWindowDecorVisible(nativeObj: long, isVisible: boolean): int;
    private native setWindowDecorHeight(nativeObj: long, height: double): int;
    private native getWindowProperties(nativeObj: long): WindowProperties;
    private native getProperties(nativeObj: long): WindowProperties;
    private native isWindowSupportWideGamut(nativeObj: long): boolean;
    private native setWindowLayoutFullScreen(nativeObj: long, isLayoutFullScreen: boolean): int;
    private native setWindowSystemBarProperties(nativeObj: long, systemBarProperties: SystemBarProperties): int;
    private native setSpecificSystemBarEnabled(
      nativeObj: long, name: String, enable: boolean, enableAnimation?: boolean): int;

    private native setWindowColorSpaceSync(nativeObj: long, colorSpace: int): void;
    private native setPreferredOrientationSync(nativeObj: long, orientation: int): void;
    private native setWindowPrivacyModeSync(nativeObj: long, isPrivacyMode: boolean): void;
    private native recoverSync(nativeObj: long): void;
    private native setUIContentSync(nativeObj: long, path: string): void;
    private native loadContentSync(nativeObj: long, path: string, storage: LocalStorage): void;
    private native loadContentSync(nativeObj: long, path: string): void;
    private native setWindowKeepScreenOnSync(nativeObj: long, isKeepScreenOn: boolean): void;
    private native setWindowSystemBarEnableSync(nativeObj: long, names: Array<'status' | 'navigation'>): void;
    private native getUIContextSync(nativeObj: long): UIContext;
    private native getWindowAvoidAreaSync(nativeObj: long, type: int): AvoidArea;
    private native setWaterMarkFlagSync(nativeObj: long, enable: boolean): void;
    private native onSync(nativeObj: long, type: string, callback: object): void;
    private native offSync(nativeObj: long, type: string, callback?: object): void;

    public resize(width: number, height: number): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void ) :void => {
        taskpool.execute((): void => {
          this.resize(this.nativeObj, width, height);
        }).then((ret: NullishType) :void => {
            resolve(undefined);
        }).catch((err: NullishType) :void => {
            reject(err as BusinessError);
        });
      });
    }

    public moveWindowTo(x: number, y: number): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void ) :void => {
        taskpool.execute((): void => {
          this.moveWindowTo(this.nativeObj, x, y);
        }).then((ret: NullishType) :void => {
            resolve(undefined);
        }).catch((err: NullishType) :void => {
            reject(err as BusinessError);
        });
      });
    }

    public resize(width: number, height: number, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.resize(this.nativeObj, width, height);
        }).then((ret: NullishType) => {
            callback(new BusinessError(), undefined);
        }).catch((err: NullishType) => {
            callback(err as BusinessError, undefined);
        });
    }

    public moveWindowTo(x: number, y: number, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.moveWindowTo(this.nativeObj, x, y);
        }).then((ret: NullishType) => {
            callback(new BusinessError(), undefined);
        }).catch((err: NullishType) => {
            callback(err as BusinessError, undefined);
        });
    }

    public getGlobalRect(): Rect {
      return this.getGlobalRect(this.nativeObj);
    }

    public getWindowDecorHeight(): number {
       let ret = this.getWindowDecorHeight(this.nativeObj);
       return ret;
    }

    public setWindowBackgroundColor(color: string | ColorMetrics): void {
       if (color instanceof string) {
         this.setWindowBackgroundColor(this.nativeObj, color as string);
       }
    }

    public setBackgroundColor(color: string): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void ) :void => {
            taskpool.execute((): void => {
                this.setWindowBackgroundColor(this.nativeObj, color);
            }).then((ret: NullishType) :void => {
                resolve(undefined);
            }).catch((err: NullishType) :void => {
                reject(err as BusinessError);
            });
        });
    }

    public setBackgroundColor(color: string, callback: AsyncCallback<void>): void {
        taskpool.execute((): void => {
            this.setWindowBackgroundColor(this.nativeObj, color);
        }).then((ret: NullishType) => {
            callback(new BusinessError(), undefined);
        }).catch((err: NullishType) => {
            callback(err as BusinessError, undefined);
        });
    }

    public setImmersiveModeEnabledState(enable: boolean): void {
        this.setImmersiveModeEnabledState(this.nativeObj, enable);
    }

    public setWindowDecorVisible(isVisible: boolean): void {
        this.setWindowDecorVisible(this.nativeObj, isVisible);
    }

    public setWindowDecorHeight(height: number): void {
        this.setWindowDecorHeight(this.nativeObj, height);
    }

    public getWindowProperties(): WindowProperties {
        let windowProperties = this.getWindowProperties(this.nativeObj) as WindowPropertiesInternal;
        windowProperties.type = windowProperties.typeInternal as WindowType;
        return windowProperties;
    }

    public getProperties(): Promise<WindowProperties> {
        return new Promise<WindowProperties>((resolve: (value: WindowProperties) => void,
            reject: (error: BusinessError) => void ) => {
            taskpool.execute((): WindowProperties => {
                let res = this.getProperties(this.nativeObj) as WindowPropertiesInternal;
                res.type = res.typeInternal as WindowType;
                return res;
            }).then((ret: NullishType) => {
                resolve(ret as WindowProperties);
            }).catch((err: NullishType) => {
                reject(err as BusinessError);
            });
        });
    }

    public getProperties(callback: AsyncCallback<WindowProperties>): void {
        taskpool.execute((): WindowProperties => {
            let res = this.getProperties(this.nativeObj);
            return res;
        }).then((ret: NullishType) => {
            callback(new BusinessError(), ret as WindowProperties);
        }).catch((err: NullishType) => {
            callback(err as BusinessError, new WindowPropertiesInternal());
        });
    }

    public isWindowSupportWideGamut(): Promise<boolean> {
        return new Promise<boolean>((resolve: (value: boolean) => void, reject: (error: BusinessError) => void ) => {
            taskpool.execute((): boolean => {
                let res = this.isWindowSupportWideGamut(this.nativeObj);
                return res;
            }).then((ret: NullishType) => {
                resolve(ret as boolean);
            }).catch((err: NullishType) => {
                reject(err as BusinessError);
            });
        });
    }

    public isWindowSupportWideGamut(callback: AsyncCallback<boolean>): void {
        taskpool.execute((): boolean => {
            let res = this.isWindowSupportWideGamut(this.nativeObj);
            return res;
        }).then((ret: NullishType) => {
            callback(new BusinessError(), ret as boolean);
        }).catch((err: NullishType) => {
            callback(err as BusinessError, false);
        });
    }

    public setWindowLayoutFullScreen(isLayoutFullScreen: boolean): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void) :void => {
            taskpool.execute((): void => {
                this.setWindowLayoutFullScreen(this.nativeObj, isLayoutFullScreen);
            }).then((ret: NullishType) :void => {
                resolve(undefined);
            }).catch((err: NullishType) :void => {
                reject(err as BusinessError);
            });
        });
    }

    public setWindowLayoutFullScreen(isLayoutFullScreen: boolean, callback: AsyncCallback<void>): void {
        taskpool.execute((): void => {
            this.setWindowLayoutFullScreen(this.nativeObj, isLayoutFullScreen);
        }).then((ret: NullishType) => {
            callback(new BusinessError(), undefined);
        }).catch((err: NullishType) => {
            callback(err as BusinessError, undefined);
        });
    }

    public setWindowSystemBarProperties(systemBarProperties: SystemBarProperties): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void) :void => {
            taskpool.execute((): void => {
                this.setWindowSystemBarProperties(this.nativeObj, systemBarProperties);
            }).then((ret: NullishType) :void => {
                resolve(undefined);
            }).catch((err: NullishType) :void => {
                reject(err as BusinessError);
            });
        });
    }

    public setWindowSystemBarProperties(systemBarProperties: SystemBarProperties, callback: AsyncCallback<void>): void {
        taskpool.execute((): void => {
            this.setWindowSystemBarProperties(this.nativeObj, systemBarProperties);
        }).then((ret: NullishType) => {
            callback(new BusinessError(), undefined);
        }).catch((err: NullishType) => {
            callback(err as BusinessError, undefined);
        });
    }

    public setSpecificSystemBarEnabled(name: String, enable: boolean, enableAnimation?: boolean): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void) :void => {
            taskpool.execute((): void => {
              this.setSpecificSystemBarEnabled(this.nativeObj, name, enable, enableAnimation);
            }).then((ret: NullishType) :void => {
                resolve(undefined);
            }).catch((err: NullishType) :void => {
                reject(err as BusinessError);
            });
        });
    }

    public setWindowColorSpace(colorSpace: ColorSpace): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setWindowColorSpaceSync(this.nativeObj, colorSpace as int);
        }).then((ret: NullishType) => {
          resolve(undefined);
        }).catch((err: NullishType) => {
          reject(err as BusinessError);
        });
      });
    }

    public setWindowColorSpace(colorSpace: ColorSpace, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.setWindowColorSpaceSync(this.nativeObj, colorSpace as int);
      }).then((ret: NullishType) => {
        callback(new BusinessError(), undefined);
      }).catch((err: NullishType) => {
        callback(err as BusinessError, undefined);
      });
    }

    public setPreferredOrientation(orientation: Orientation): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setPreferredOrientationSync(this.nativeObj, orientation as int);
        }).then((ret: NullishType) => {
          resolve(undefined);
        }).catch((err: NullishType) => {
          reject(err as BusinessError);
        });
      });
    }

    public setPreferredOrientation(orientation: Orientation, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.setPreferredOrientationSync(this.nativeObj, orientation as int);
      }).then((ret: NullishType) => {
        callback(new BusinessError(), undefined);
      }).catch((err: NullishType) => {
        callback(err as BusinessError, undefined);
      });
    }

    public setWindowPrivacyMode(isPrivacyMode: boolean): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setWindowPrivacyModeSync(this.nativeObj, isPrivacyMode);
        }).then((ret: NullishType) => {
          resolve(undefined);
        }).catch((err: NullishType) => {
          reject(err as BusinessError);
        });
      });
    }

    public setWindowPrivacyMode(isPrivacyMode: boolean, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.setWindowPrivacyModeSync(this.nativeObj, isPrivacyMode);
      }).then((ret: NullishType) => {
        callback(new BusinessError(), undefined);
      }).catch((err: NullishType) => {
        callback(err as BusinessError, undefined);
      });
    }

    public recover(): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.recoverSync(this.nativeObj);
        }).then((ret: NullishType) => {
          resolve(undefined);
        }).catch((err: NullishType) => {
          reject(err as BusinessError);
        });
      });
    }

    public setUIContent(path: string): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        try {
          this.setUIContentSync(this.nativeObj, path);
          resolve(undefined);
        } catch (err: Error) {
          reject(err as BusinessError);
        }
      });
    }

    public setUIContent(path: string, callback: AsyncCallback<void>): void {
      try {
        this.setUIContentSync(this.nativeObj, path);
        callback(new BusinessError(), undefined);
      } catch (err: Error) {
        callback(err as BusinessError, undefined);
      }
    }

    public loadContent(path: string, storage: LocalStorage): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        try {
          this.loadContentSync(this.nativeObj, path, storage);
          resolve(undefined);
        } catch (err: Error) {
          reject(err as BusinessError);
        }
      });
    }

    public loadContent(path: string, storage: LocalStorage, callback: AsyncCallback<void>): void {
      try {
        this.loadContentSync(this.nativeObj, path, storage);
        callback(new BusinessError(), undefined);
      } catch (err: Error) {
        callback(err as BusinessError, undefined);
      }
    }

    public loadContent(path: string): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        try {
          this.loadContentSync(this.nativeObj, path);
          resolve(undefined);
        } catch (err: Error) {
          reject(err as BusinessError);
        }
      });
    }

    public loadContent(path: string, callback: AsyncCallback<void>): void {
      try {
        this.loadContentSync(this.nativeObj, path);
        callback(new BusinessError(), undefined);
      } catch (err: Error) {
        callback(err as BusinessError, undefined);
      }
    }

    public setWindowKeepScreenOn(isKeepScreenOn: boolean): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void) :void => {
            taskpool.execute((): void => {
                this.setWindowKeepScreenOnSync(this.nativeObj, isKeepScreenOn);
            }).then((ret: NullishType): void => {
                resolve(ret as undefined);
            }).catch((err: NullishType): void => {
                reject(err as BusinessError);
            });
        });
    }

    public setWindowKeepScreenOn(isKeepScreenOn: boolean, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.setWindowKeepScreenOnSync(this.nativeObj, isKeepScreenOn);
      }).then((ret: NullishType) => {
        callback(new BusinessError(), undefined);
      }).catch((err: NullishType) => {
        callback(err as BusinessError, undefined);
      });
    }

    public setWindowSystemBarEnable(names: Array<'status' | 'navigation'>): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void ) :void => {
            taskpool.execute((): void => {
                this.setWindowSystemBarEnableSync(this.nativeObj, names);
            }).then((ret: NullishType) => {
                resolve(undefined);
            }).catch((err: NullishType) => {
                reject(err as BusinessError);
            });
        });
    }

    public setWindowSystemBarEnable(names: Array<'status' | 'navigation'>, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.setWindowSystemBarEnableSync(this.nativeObj, names);
      }).then((ret: NullishType) => {
        callback(new BusinessError(), undefined);
      }).catch((err: NullishType) => {
        callback(err as BusinessError, undefined);
      });
    }

    public getUIContext(): UIContext {
      return this.getUIContextSync(this.nativeObj);
    }

    public getWindowAvoidArea(type: AvoidAreaType): AvoidArea {
      return this.getWindowAvoidAreaSync(this.nativeObj, type as AvoidAreaType);
    }

    public setWaterMarkFlag(enable: boolean): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setWaterMarkFlagSync(this.nativeObj, enable);
        }).then((ret: NullishType) => {
          resolve(undefined);
        }).catch((err: NullishType) => {
          reject(err as BusinessError);
        });
      });
    }

    public setWaterMarkFlag(enable: boolean, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.setWaterMarkFlagSync(this.nativeObj, enable);
      }).then((ret: NullishType) => {
        callback(new BusinessError(), undefined);
      }).catch((err: NullishType) => {
        callback(err as BusinessError, undefined);
      });
    }

    public on<T>(type: string, callback: Callback<T>): void {
      this.onSync(this.nativeObj, type, callback);
    }

    public off<T>(type: string, callback?: Callback<T>): void {
      this.offSync(this.nativeObj, type, callback);
    }
}

export interface Window {
    resize(width: number, height: number): Promise<void>;
    moveWindowTo(x: number, y: number): Promise<void>;
    resize(width: number, height: number, callback: AsyncCallback<void>): void;
    moveWindowTo(x: number, y: number, callback: AsyncCallback<void>): void;
    getGlobalRect(): Rect;
    getWindowDecorHeight(): double;
    setWindowBackgroundColor(color: string | ColorMetrics): void;
    setBackgroundColor(color: string): Promise<void>;
    setBackgroundColor(color: string, callback: AsyncCallback<void>): void;
    setImmersiveModeEnabledState(enable: boolean): void;
    setWindowDecorVisible(isVisible: boolean): void;
    setWindowDecorHeight(height: number): void;
    getWindowProperties(): WindowProperties;
    getProperties(): Promise<WindowProperties>;
    getProperties(callback: AsyncCallback<WindowProperties>): void;
    isWindowSupportWideGamut(): Promise<boolean>;
    isWindowSupportWideGamut(callback: AsyncCallback<boolean>): void;
    setWindowLayoutFullScreen(isLayoutFullScreen: boolean): Promise<void>;
    setWindowLayoutFullScreen(isLayoutFullScreen: boolean, callback: AsyncCallback<void>): void;
    setWindowSystemBarProperties(systemBarProperties: SystemBarProperties): Promise<void>;
    setWindowSystemBarProperties(systemBarProperties: SystemBarProperties, callback: AsyncCallback<void>): void;
    setSpecificSystemBarEnabled(name: String, enable: boolean, enableAnimation?: boolean): Promise<void>;
    setWindowColorSpace(colorSpace: ColorSpace): Promise<void>;
    setWindowColorSpace(colorSpace: ColorSpace, callback: AsyncCallback<void>): void;
    setPreferredOrientation(orientation: Orientation): Promise<void>;
    setPreferredOrientation(orientation: Orientation, callback: AsyncCallback<void>): void;
    setWindowPrivacyMode(isPrivacyMode: boolean): Promise<void>;
    setWindowPrivacyMode(isPrivacyMode: boolean, callback: AsyncCallback<void>): void;
    recover(): Promise<void>;
    setUIContent(path: string): Promise<void>;
    setUIContent(path: string, callback: AsyncCallback<void>): void;
    loadContent(path: string, storage: LocalStorage): Promise<void>;
    loadContent(path: string, storage: LocalStorage, callback: AsyncCallback<void>): void;
    loadContent(path: string): Promise<void>;
    loadContent(path: string, callback: AsyncCallback<void>): void;
    setWindowKeepScreenOn(isKeepScreenOn: boolean): Promise<void>;
    setWindowKeepScreenOn(isKeepScreenOn: boolean, callback: AsyncCallback<void>): void;
    setWindowSystemBarEnable(names: Array<'status' | 'navigation'>): Promise<void>;
    setWindowSystemBarEnable(names: Array<'status' | 'navigation'>, callback: AsyncCallback<void>): void;
    getUIContext(): UIContext;
    getWindowAvoidArea(type: AvoidAreaType): AvoidArea;
    setWaterMarkFlag(enable: boolean): Promise<void>;
    setWaterMarkFlag(enable: boolean, callback: AsyncCallback<void>): void;
    on<T>(type: string, callback: Callback<T>): void;
    off<T>(type: string, callback?: Callback<T>): void;
}

export native function CreateWindowStage(scene: long): WindowStageInternal;
export function CreateWindowStageApi(scene: long): WindowStage { return CreateWindowStage(scene); }
export native function CreateWindow(window: long): WindowInternal;

let nativeObj: long;
function setNativeObj(nativeObj: long):void {
    nativeObj = nativeObj;
}

native function getLastWindowSync(nativeObj: long, ctx: BaseContext): Window;

export function getLastWindow(ctx: BaseContext): Promise<Window> {
  return new Promise<Window>((resolve: (value: Window) => void, reject: (error: BusinessError) => void ) => {
      taskpool.execute((): Window => {
          return getLastWindowSync(nativeObj, ctx);
      }).then((ret: NullishType) => {
          resolve(ret as Window);
      }).catch((err: NullishType) => {
          reject(err as BusinessError);
      });
  }); 
}

export function getLastWindow(ctx: BaseContext, callback: AsyncCallback<Window>): void {
    taskpool.execute((): Window => {
        return getLastWindowSync(nativeObj, ctx);
    }).then((ret: NullishType) => {
        callback(new BusinessError(), ret as Window);
    }).catch((err: NullishType) => {
        callback(err as BusinessError, new WindowInternal());
    });
}

function runWindowStageLifecycleEventCallBack(cb: object, cbArg: int): void {
    const func = cb as (cbArg: WindowStageLifeCycleEventType) => void;
    func(cbArg as WindowStageLifeCycleEventType);
}

function runWindowEventCallBack(cb: object, cbArg: int): void {
    const func = cb as (cbArg: WindowEventType) => void;
    func(cbArg as WindowEventType);
}

function runWindowSizeCallBack(cb: object, cbArg: object): void {
    const func = cb as (cbArg: Size) => void;
    func(cbArg as Size);
}

function runWindowStatusCallBack(cb: object, cbArg: int): void {
    const func = cb as (cbArg: WindowStatusType) => void;
    func(cbArg as WindowStatusType);
}
}

export default window;