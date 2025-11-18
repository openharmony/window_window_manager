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
import ConfigurationConstant from '@ohos.app.ability.ConfigurationConstant';
import { AsyncCallback, BusinessError, Callback } from '@ohos.base';
import { LocalStorage } from '@ohos.arkui.stateManagement';
import { UIContext } from '@ohos.arkui.UIContext';
import image from '@ohos.multimedia.image';
import BaseContext from 'application.BaseContext';
import hilog from '@ohos.hilog';
import { ColorMetrics } from '@ohos.arkui.node';
import rpc from '@ohos.rpc';
import dialogRequest from '@ohos.app.ability.dialogRequest';
import { ImageFit} from '@ohos.arkui.component';
export type AsyncCallbackVoid = (err: BusinessError) => void;
export type WindowEventCallback = (data: window.WindowEventType) => void;
export type WindowRectCallback = (data: window.Rect) => void;
export type WindowSizeCallback = (data: window.Size) => void;
export type WindowStatusCallback = (data: window.WindowStatusType) => void;
export type KeyboardHeightChangeCallback = (data: int) => void;
export type KeyboardDidShowOrHideCallback = (data: window.KeyboardInfo) => void;
export type WindowVoidCallback = () => void;
export type RotationChangeCallback<T, U> = (info: T) => U;
const DOMAIN = 0x4200;
const TAG = '[ANI]';
namespace window {
  export interface Size {
    /**
     * The width of the window.
     *
     * @type { int }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * The width of the window.
     *
     * @type { int }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @since 10
     */
    /**
     * The width of the window.
     *
     * @type { int }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 11
     */
    width: int;

    /**
     * The height of the window.
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 7
     */
    /**
     * The height of the window.
     *
     * @type { int }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @since 10
     */
    /**
     * The height of the window.
     *
     * @type { int }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 11
     */
    height: int;
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

  /**
   * System bar tint of region
   *
   * @interface SystemBarRegionTint
   * @syscap SystemCapability.WindowManager.WindowManager.Core
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface SystemBarRegionTint {
    /**
     * System bar type
     *
     * @type { WindowType }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @systemapi Hide this for inner system use.
     * @since 8
     */
    type: number;

    /**
     * The visibility of system bar
     *
     * @type { ?boolean }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @systemapi Hide this for inner system use.
     * @since 8
     */
    isEnable: boolean;

    /**
     * The region of system bar
     *
     * @type { ?Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @systemapi Hide this for inner system use.
     * @since 8
     */
    region: Rect;

    /**
     * The background color of the system bar.
     *
     * @type { ?string }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @systemapi Hide this for inner system use.
     * @since 8
     */
    backgroundColor: string;

    /**
     * The content color of the system bar.
     *
     * @type { ?string }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @systemapi Hide this for inner system use.
     * @since 8
     */
    contentColor: string;
  }

  export class SystemBarRegionTintInternal implements SystemBarRegionTint {
    type: number;
    isEnable: boolean;
    region: Rect;
    backgroundColor: string;
    contentColor: string;
  }

  /**
   * System bar tint state for systemui
   *
   * @interface SystemBarTintState
   * @syscap SystemCapability.WindowManager.WindowManager.Core
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface SystemBarTintState {
    /**
     * Id of display
     *
     * @type { long }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @systemapi Hide this for inner system use.
     * @since 8
     */
    displayId: long;
    /**
     * Region tint of systembar
     *
     * @type { Array<SystemBarRegionTint> }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @systemapi Hide this for inner system use.
     * @since 8
     */
    regionTint: Array<SystemBarRegionTint>;
  }

  export class SystemBarTintStateInternal implements SystemBarTintState {
    displayId: long;
    regionTint: Array<SystemBarRegionTint>;
  }

  export enum RectChangeReason {
    UNDEFINED = 0,
    MAXIMIZE = 1,
    RECOVER = 2,
    MOVE = 3,
    DRAG = 4,
    DRAG_START = 5,
    DRAG_END = 6,
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
     * @type { int }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 11
     */
    left: int;

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
     * @type { int }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 11
     */
    top: int;

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
     * @type { int }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 11
     */
    width: int;

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
     * @type { int }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 11
     */
    height: int;
  }

  export class RectInternal implements Rect {
    left: int;
    top: int;
    width: int;
    height: int;
    constructor() {}
    constructor(left: int, top: int, width: int, height: int) {
      this.left = left;
      this.top = top;
      this.width = width;
      this.height = height;
    }
  }

  export interface RectChangeOptions {
    rect: Rect;
    reason: RectChangeReason;
  }

  export class SizeInternal implements Size {
    width: int;
    height: int;
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
    TYPE_HANDWRITE = 21,
    TYPE_WALLET_SWIPE_CARD = 22,
    TYPE_SCREEN_CONTROL = 23,
    TYPE_FLOAT_NAVIGATION = 24,
    TYPE_MAIN = 32
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

export enum RotationChangeType {
  WINDOW_WILL_ROTATE = 0,
  WINDOW_DID_ROTATE = 1
}

export enum RectType {
  RELATIVE_TO_SCREEN = 0,
  RELATIVE_TO_PARENT_WINDOW = 1
}

export interface SubWindowOptions {
    title: string;
    decorEnabled: boolean;
    isModal?: boolean;
    isTopmost?: boolean;
    modalityType?: ModalityType;
    windowRect?: Rect;
    maximizeSupported?: boolean;
    zLevel?: int;
    outlineEnabled?: boolean;
}

export class SubWindowOptionsInternal implements SubWindowOptions {
    title: string;
    decorEnabled: boolean;
    isModal?: boolean;
    isTopmost?: boolean;
    modalityType?: ModalityType;
    windowRect?: Rect;
    maximizeSupported?: boolean;
    zLevel?: int;
    outlineEnabled?: boolean;
}

export interface SystemWindowOptions {
    windowType: WindowType;
}

export interface StatusBarProperty {
  contentColor: string;
}

export class StatusBarPropertyInternal implements StatusBarProperty {
  contentColor: string;
}

export interface WindowLayoutInfo {
  windowRect: Rect;
}

export class WindowLayoutInfoInternal implements WindowLayoutInfo {
  windowRect: Rect;
}

export interface WindowInfo {
  rect: Rect;
  globalDisplayRect?: Rect;
  bundleName: string;
  abilityName: string;
  windowId: int;
  windowStatusType: WindowStatusType;
  isFocused?: boolean;
}

export class WindowInfoInternal implements WindowInfo {
  rect: Rect;
  globalDisplayRect?: Rect;
  bundleName: string;
  abilityName: string;
  windowId: int;
  windowStatusType: WindowStatusType;
  isFocused?: boolean;
  windowStatusTypeInternal: int;
  isFocusedInternal: boolean;
}

export enum BlurStyle {
  OFF = 0,
  THIN = 1,
  REGULAR = 2,
  THICK = 3
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
    statusBarColor?: string;

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
    isStatusBarLightIcon?: boolean;

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
    statusBarContentColor?: string;

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
    navigationBarColor?: string;

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
    isNavigationBarLightIcon?: boolean;

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
    navigationBarContentColor?: string;

    /**
     * Enable the animation of the status bar.
     *
     * @type { ?boolean }
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    enableStatusBarAnimation?: boolean;

    /**
     * Enable the animation of the navigation bar.
     *
     * @type { ?boolean }
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    enableNavigationBarAnimation?: boolean;
}

export class SystemBarPropertiesInternal implements SystemBarProperties {
    statusBarColor?: string;
    isStatusBarLightIcon?: boolean;
    statusBarContentColor?: string;
    navigationBarColor?: string;
    isNavigationBarLightIcon?: boolean;
    navigationBarContentColor?: string;
    enableStatusBarAnimation?: boolean;
    enableNavigationBarAnimation?: boolean;
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
     * @type { double }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @since 10
     */
    /**
     * Brightness value of window.
     *
     * @type { double }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @crossplatform
     * @atomicservice
     * @since 11
     */
    brightness: double;

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
     * @type { int }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 9
     */
    /**
     * Window id.
     *
     * @type { int }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    id: int;

    /**
     * display id.
     *
     * @type { ?long }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    displayId?: long;

    /**
     * window name.
     *
     * @type { ?string }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 18
     */
    name?: string;
}

export class WindowPropertiesInternal implements WindowProperties {
    windowRect: Rect;
    drawableRect: Rect;
    isFullScreen: boolean;
    isLayoutFullScreen: boolean;
    focusable: boolean;
    touchable: boolean;
    brightness: double;
    dimBehindValue: number;
    isKeepScreenOn: boolean;
    isPrivacyMode: boolean;
    isRoundCorner: boolean;
    isTransparent: boolean;
    type: WindowType;
    id: int;
    displayId?: long;
    name?: string;

    constructor(windowRect: Rect, drawableRect: Rect, isFullScreen: boolean, isLayoutFullScreen: boolean,
      focusable: boolean, touchable: boolean, brightness: double, dimBehindValue: number, isKeepScreenOn: boolean,
      isPrivacyMode: boolean, isRoundCorner: boolean, isTransparent: boolean, typeInternal: int, id: int,
      displayId: long, name: string)
    {
      this.windowRect = windowRect;
      this.drawableRect = drawableRect;
      this.isFullScreen = isFullScreen;
      this.isLayoutFullScreen = isLayoutFullScreen;
      this.focusable = focusable;
      this.touchable = touchable;
      this.brightness = brightness;
      this.dimBehindValue = dimBehindValue;
      this.isKeepScreenOn = isKeepScreenOn;
      this.isPrivacyMode = isPrivacyMode;
      this.isRoundCorner = isRoundCorner;
      this.isTransparent = isTransparent;
      this.type = typeInternal as WindowType;
      this.id = id;
      this.displayId = displayId;
      this.name = name;
    }

    constructor() {}
}

/**
 * Configuration parameters for window creation.
 *
 * @interface Configuration
 * @syscap SystemCapability.WindowManager.WindowManager.Core
 * @since 9
 */
/**
 * Configuration parameters for window creation.
 *
 * @interface Configuration
 * @syscap SystemCapability.WindowManager.WindowManager.Core
 * @atomicservice
 * @since 12
 */
export interface Configuration {
    /**
     * Indicates window id.
     *
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 9
     */
    /**
     * Indicates window id.
     *
     * @type { string }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    name: string;

    /**
     * Indicates window type
     *
     * @type { WindowType }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 9
     */
    /**
     * Indicates window type
     *
     * @type { WindowType }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    windowType: WindowType;

    /**
     * Indicates window context.
     *
     * @type { ?BaseContext }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 9
     */
    /**
     * Indicates window context.
     *
     * @type { ?BaseContext }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    ctx?: BaseContext;

    /**
     * Indicates display ID.
     *
     * @type { ?number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 9
     */
    /**
     * Indicates display ID.
     *
     * @type { ?long }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    displayId?: long;

    /**
     * Indicates Parent window id
     *
     * @type { ?int }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 9
     */
    /**
     * Indicates Parent window id
     *
     * @type { ?int }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    parentId?: int;

    /**
     * Indicates whether enable window decor, only support dialog, The default value is false.
     *
     * @type { ?boolean }
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    decorEnabled?: boolean;

    /**
     * Indicates dialog window title when decor enabled.
     *
     * @type { ?string }
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    title?: string;
}

export class ConfigurationInternal implements Configuration {
    name: string;
    windowType: WindowType;
    ctx?: BaseContext;
    displayId?: long;
    parentId?: int;
    decorEnabled?: boolean;
    title?: string;
}

export interface MoveConfiguration {
    displayId: long;
}

export interface WindowDensityInfo {
    systemDensity: double;
    defaultDensity: double;
    customDensity: double;
}

export class WindowDensityInfoInternal implements WindowDensityInfo {
    systemDensity: double;
    defaultDensity: double;
    customDensity: double;
}

export interface SystemBarStyle {
  statusBarContentColor?: string;
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
    animationForShown: (context: window.TransitionContext) => void;
    animationForHidden: (context: window.TransitionContext) => void;
}

export native function transContextFinalizerCallback(nativeObj: long): void;
let transContextFinalizerRegister = new FinalizationRegistry<long>(transContextFinalizerCallback);
export class TransitionContextInternal implements TransitionContext {
    toWindow: Window;
    private nativeObj: long = 0;

    setNativeObj(nativeObj: long): void {
      if (this.nativeObj) {
        transContextFinalizerRegister.unregister(this);
      }
      transContextFinalizerRegister.register(this, nativeObj, this);
      this.nativeObj = nativeObj;
    }

    private native completeTransitionSync(nativeObj: long, shouldHide: boolean): void;

    public completeTransition(isCompleted: boolean): void {
        this.completeTransitionSync(this.nativeObj, isCompleted);
    }
}

export native function transControllerFinalizerCallback(nativeObj: long): void;
let transControllerFinalizerRegister = new FinalizationRegistry<long>(transControllerFinalizerCallback);
export class TransitionControllerInternal implements TransitionController {
    static { loadLibrary('windowstageani_module.z') }
    private nativeObj: long = 0;

    setNativeObj(nativeObj: long): void {
      if (this.nativeObj) {
        transControllerFinalizerRegister.unregister(this);
      }
      transControllerFinalizerRegister.register(this, nativeObj, this);
      this.nativeObj = nativeObj;
    }

    animationForShown: (context: window.TransitionContext) => void;
    animationForHidden: (context: window.TransitionContext) => void;
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
     * @type { ?int }
     * @syscap SystemCapability.Window.SessionManager
     * @since 11
     */
    /**
     * The maximum width of the window.
     *
     * @type { ?int }
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    maxWidth: int;

    /**
     * The maximum height of the window.
     *
     * @type { ?int }
     * @syscap SystemCapability.Window.SessionManager
     * @since 11
     */
    /**
     * The maximum height of the window.
     *
     * @type { ?int }
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    maxHeight: int;

    /**
     * The minimum width of the window.
     *
     * @type { ?int }
     * @syscap SystemCapability.Window.SessionManager
     * @since 11
     */
    /**
     * The minimum width of the window.
     *
     * @type { ?int }
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    minWidth: int;

    /**
     * The minimum height of the window.
     *
     * @type { ?int }
     * @syscap SystemCapability.Window.SessionManager
     * @since 11
     */
    /**
     * The minimum height of the window.
     *
     * @type { ?int }
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    minHeight: int;
}

/**
   * Rectangular area of the title buttons relative to the upper right corner of the window.
   *
   * @interface TitleButtonRect
   * @syscap SystemCapability.Window.SessionManager
   * @since 11
   */
  /**
   * Rectangular area of the title buttons relative to the upper right corner of the window.
   *
   * @interface TitleButtonRect
   * @syscap SystemCapability.Window.SessionManager
   * @atomicservice
   * @since 12
   */
  export interface TitleButtonRect {

    /**
     * The right of the Rect.
     *
     * @type { int }
     * @syscap SystemCapability.Window.SessionManager
     * @since 11
     */
    /**
     * The right of the Rect.
     *
     * @type { int }
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    right: int;

    /**
     * The top of the Rect.
     *
     * @type { int }
     * @syscap SystemCapability.Window.SessionManager
     * @since 11
     */
    /**
     * The top of the Rect.
     *
     * @type { int }
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    top: int;

    /**
     * The width of the Rect.
     *
     * @type { int }
     * @syscap SystemCapability.Window.SessionManager
     * @since 11
     */
    /**
     * The width of the Rect.
     *
     * @type { int }
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    width: int;

    /**
     * The height of the Rect.
     *
     * @type { int }
     * @syscap SystemCapability.Window.SessionManager
     * @since 11
     */
    /**
     * The height of the Rect.
     *
     * @type { int }
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    height: int;
  }

/**
 * The decor button style of the window.
 * 
 * @interface DecorButtonStyle
 * @syscap SystemCapability.Window.SessionManager
 * @atomicservice
 * @since 14
 * @arkts 1.1&1.2
 */
export interface DecorButtonStyle {
  /**
   * color mode.
   * 
   * @type { ?colorMode }
   * @syscap SystemCapability.Window.SessionManager
   * @atomicservice
   * @since 14
   */
  colorMode?: ConfigurationConstant.ColorMode;

  /**
   * button background size when hover.
   * 
   * @type { ?int }
   * @syscap SystemCapability.Window.SessionManager
   * @atomicservice
   * @since 14
   */
  buttonBackgroundSize?: int;

  /**
   * button spacing.
   * 
   * @type { ?int }
   * @syscap SystemCapability.Window.SessionManager
   * @atomicservice
   * @since 14
   */
  spacingBetweenButtons?: int;

  /**
   * close button right margin.
   * 
   * @type { ?int }
   * @syscap SystemCapability.Window.SessionManager
   * @atomicservice
   * @since 14
   */
  closeButtonRightMargin?: int;

  /**
   * button icon size.
   *
   * @type { ?int }
   * @syscap SystemCapability.Window.SessionManager
   * @atomicservice
   * @since 20
   */
  buttonIconSize?: int;

  /**
   * corner radius of button background when hover.
   *
   * @type { ?int }
   * @syscap SystemCapability.Window.SessionManager
   * @atomicservice
   * @since 20
   */
  buttonBackgroundCornerRadius?: int;
}

export class DecorButtonStyleInternal implements DecorButtonStyle {
  colorMode?: ConfigurationConstant.ColorMode;
  buttonBackgroundSize?: int;
  spacingBetweenButtons?: int;
  closeButtonRightMargin?: int;
  buttonIconSize?: int;
  buttonBackgroundCornerRadius?: int;
}

export interface WindowTitleButtonVisibleParam {
  isMaximizeButtonVisible: boolean;
  isMinimizeButtonVisible: boolean;
  isCloseButtonVisible: boolean;
}

export interface TitleButtonVisibleParam {
  isMaximizeVisible: boolean;
  isMinimizeVisible: boolean;
  isSplitVisible: boolean;
  isCloseVisible: boolean;
}

export interface GetWindowsByCoordinateParam {
  displayId: long;
  windowNumber: int;
  x: int;
  y: int;
}

export class GetWindowsByCoordinateParamInternal implements GetWindowsByCoordinateParam {
  displayId: long;
  windowNumber: int;
  x: int;
  y: int;
}

/**
 * The information of keyboard
 *
 * @interface KeyboardInfo
 * @syscap SystemCapability.Window.SessionManager
 * @atomicservice
 * @since 18
 */
export interface KeyboardInfo {
  /**
   * The position and size of keyboard before animation.
   *
   * @type { Rect }
   * @syscap SystemCapability.Window.SessionManager
   * @atomicservice
   * @since 18
   */
  beginRect: Rect;

  /**
   * The position and size of keyboard after animation completed.
   *
   * @type { Rect }
   * @syscap SystemCapability.Window.SessionManager
   * @atomicservice
   * @since 18
   */
  endRect: Rect;
}

export class KeyboardInfoInternal implements KeyboardInfo {
  beginRect: Rect;
  endRect: Rect;
}

/**
  * Describes the scale Transition Options of window
  *
  * @interface ScaleOptions
  * @syscap SystemCapability.WindowManager.WindowManager.Core
  * @systemapi
  * @since 9
  */
export interface ScaleOptions {
  /**
    * The scale param of x direction. Default is 1.f
    *
    * @type { ?double }
    * @syscap SystemCapability.WindowManager.WindowManager.Core
    * @systemapi
    * @since 9
    */
  x?: double;

  /**
    * The scale param of y direction. Default is 1.f
    *
    * @type { ?double }
    * @syscap SystemCapability.WindowManager.WindowManager.Core
    * @systemapi
    * @since 9
    */
  y?: double;

  /**
    * The scale param of pivot point of x. Default is 0.5f, Interval is 0.f - 1.f
    *
    * @type { ?double }
    * @syscap SystemCapability.WindowManager.WindowManager.Core
    * @systemapi
    * @since 9
    */
  pivotX?: double;

  /**
    * The scale param of pivot point of y. Default is 0.5f, Interval is 0.f - 1.f
    *
    * @type { ?double }
    * @syscap SystemCapability.WindowManager.WindowManager.Core
    * @systemapi
    * @since 9
    */
  pivotY?: double;
}

export class ScaleOptionsInternal implements ScaleOptions {
  x?: double;
  y?: double;
  pivotX?: double;
  pivotY?: double;
}

/**
  * Describes the rotate Transition Options of window
  *
  * @interface RotateOptions
  * @syscap SystemCapability.WindowManager.WindowManager.Core
  * @systemapi
  * @since 9
  */
export interface RotateOptions {
  /**
    * The rotate degree of x direction. Default value is 0.f
    *
    * @type { ?double }
    * @syscap SystemCapability.WindowManager.WindowManager.Core
    * @systemapi
    * @since 9
    */
  x?: double;

  /**
    * The rotate degree of y direction. Default value is 0.f
    *
    * @type { ?double }
    * @syscap SystemCapability.WindowManager.WindowManager.Core
    * @systemapi
    * @since 9
    */
  y?: double;

  /**
    * The rotate degree of z direction. Default value is 0.f
    *
    * @type { ?double }
    * @syscap SystemCapability.WindowManager.WindowManager.Core
    * @systemapi
    * @since 9
    */
  z?: double;

  /**
    * The param of pivot point of x. Default is 0.5f, Interval is 0.f - 1.f
    *
    * @type { ?double }
    * @syscap SystemCapability.WindowManager.WindowManager.Core
    * @systemapi
    * @since 9
    */
  pivotX?: double;

  /**
    * The param of pivot point of y. Default is 0.5f, Interval is 0.f - 1.f
    *
    * @type { ?double }
    * @syscap SystemCapability.WindowManager.WindowManager.Core
    * @systemapi
    * @since 9
    */
  pivotY?: double;
}

export class RotateOptionsInternal implements RotateOptions {
  x?: double;
  y?: double;
  z?: double;
  pivotX?: double;
  pivotY?: double;
}

/**
  * Describes the translate Transition Options of window
  *
  * @interface TranslateOptions
  * @syscap SystemCapability.WindowManager.WindowManager.Core
  * @systemapi
  * @since 9
  */
export interface TranslateOptions {
  /**
    * The translate pixel param of x direction. Default is 0.f
    *
    * @type { ?double }
    * @syscap SystemCapability.WindowManager.WindowManager.Core
    * @systemapi
    * @since 9
    */
  x?: double;

  /**
    * The translate pixel param of y direction. Default is 0.f
    *
    * @type { ?double }
    * @syscap SystemCapability.WindowManager.WindowManager.Core
    * @systemapi
    * @since 9
    */
  y?: double;

  /**
    * The translate pixel param of z direction. Default is 0.f
    *
    * @type { ?double }
    * @syscap SystemCapability.WindowManager.WindowManager.Core
    * @systemapi
    * @since 9
    */
  z?: double;
}

export class TranslateOptionsInternal implements TranslateOptions {
  x?: double;
  y?: double;
  z?: double;
}

/**
 * Rotation change info
 *
 * @interface RotationChangeInfo
 * @syscap SystemCapability.Window.SessionManager
 * @atomicservice
 * @since 19
 */
export interface RotationChangeInfo {
  /**
   * Rotation change type
   *
   * @type { RotationChangeType }
   * @syscap SystemCapability.Window.SessionManager
   * @atomicservice
   * @since 19
   */
  type: RotationChangeType;
  /**
   * window orientation
   *
   * @type { int }
   * @syscap SystemCapability.Window.SessionManager
   * @atomicservice
   * @since 19
   */
  orientation: int;
  /**
   * Display id
   *
   * @type { long }
   * @syscap SystemCapability.Window.SessionManager
   * @atomicservice
   * @since 19
   */
  displayId: long;
  /**
   * Display rect
   *
   * @type { Rect }
   * @syscap SystemCapability.Window.SessionManager
   * @atomicservice
   * @since 19
   */
  displayRect: Rect;
}

export class RotationChangeInfoInternal implements RotationChangeInfo {
  type: RotationChangeType;
  orientation: int;
  displayId: long;
  displayRect: Rect;
}

/**
 * Rotation change result
 *
 * @interface RotationChangeResult
 * @syscap SystemCapability.Window.SessionManager
 * @atomicservice
 * @since 19
 */
export interface RotationChangeResult {
  /**
   * Rect type
   *
   * @type { RectType }
   * @syscap SystemCapability.Window.SessionManager
   * @atomicservice
   * @since 19
   */
  rectType: RectType;
  /**
   * Window Rect
   *
   * @type { Rect }
   * @syscap SystemCapability.Window.SessionManager
   * @atomicservice
   * @since 19
   */
  windowRect: Rect;
}

export class WindowStageInternal implements WindowStage {
    static { loadLibrary('windowstageani_module.z') }

    private nativeObj: long = 0;
    setNativeObj(nativeObj: long): void {
        this.nativeObj = nativeObj;
    }

    private static native nativeTransferStatic(input: ESValue): Object;
    private static native nativeTransferDynamic(nativeObj: long): ESValue;
    public native setWindowRectAutoSave(nativeObj: long, enabled: boolean, isSaveBySpecifiedFlag: boolean): void;
    public native isWindowRectAutoSave(nativeObj: long): boolean;
    public native removeStartingWindow(nativeObj: long): void;
    public native loadContentSync(nativeObj: long, path: string, storage?: LocalStorage): void;
    public native disableWindowDecorSync(nativeObj:long): void;
    public native setShowOnLockScreenSync(nativeObj: long, showOnLockScreen: boolean): void;
    public native setWindowModal(nativeObj: long, isModal: boolean): void;
    public native getMainWindowSync(nativeObj: long): Window;
    public native createSubWindowSync(nativeObj: long, name: String): Window;
    public native setCustomDensitySync(nativeObj: long, density: double): void;
    public native setDefaultDensityEnabledSync(nativeObj: long, enabled: boolean): void;
    public native setImageForRecentSync(nativeObj: long, imgResourceId: long, value: int): void;
    public native getSubWindowSync(nativeObj: long): Array<Window>;
    public native createSubWindowWithOptionsSync(nativeObj: long, name: string, options: SubWindowOptions): Window;
    private native onSync(nativeObj: long, type: string, callback: object): void;
    private native offSync(nativeObj: long, type: string, callback?: object): void;

    public static transferStatic(input: Any): Object {
      hilog.info(DOMAIN, TAG, 'transfer static, input:' + input);
      let windowStage: ESValue = ESValue.wrap(input).getPropertySafe('windowStage_');
      if (windowStage !== ESValue.Undefined) {
        return WindowStageInternal.nativeTransferStatic(windowStage);
      } else {
        return WindowStageInternal.nativeTransferStatic(ESValue.wrap(input));
      }
    }

    public static transferDynamic(input: Object): Any {
      hilog.info(DOMAIN, TAG, 'transfer dynamic, input:' + input);
      if (input instanceof WindowStageInternal) {
        return WindowStageInternal.nativeTransferDynamic((input as WindowStageInternal).nativeObj).unwrap();
      } else {
        return undefined;
      }
    }

    public setWindowRectAutoSave(enabled: boolean, isSaveBySpecifiedFlag: boolean): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setWindowRectAutoSave(this.nativeObj, enabled, isSaveBySpecifiedFlag);
        }).then((ret: Any): void => {
          resolve(undefined);
        }).catch((err: Any): void => {
          reject(err as BusinessError);
        });
      });
    }

    public setWindowRectAutoSave(enabled: boolean): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setWindowRectAutoSave(this.nativeObj, enabled, false);
        }).then((ret: Any): void => {
          resolve(undefined);
        }).catch((err: Any): void => {
          reject(err as BusinessError);
        });
      });
    }

    public isWindowRectAutoSave(): Promise<boolean> {
      return new Promise<boolean>((resolve: (value: boolean) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): boolean => {
          return this.isWindowRectAutoSave(this.nativeObj);
        }).then((ret: Any): void => {
          resolve(ret as boolean);
        }).catch((err: Any): void => {
          reject(err as BusinessError);
        });
      });
    }

    public removeStartingWindow(): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.removeStartingWindow(this.nativeObj);
        }).then((ret: Any): void => {
          resolve(undefined);
        }).catch((err: Any): void => {
          reject(err as BusinessError);
        });
      });
    }

    public loadContent(path: string, storage: LocalStorage, callback: AsyncCallback<void>): void {
      try {
        this.loadContentSync(this.nativeObj, path, storage);
        callback(new BusinessError(), undefined);
      } catch (err) {
        callback(err as BusinessError, undefined);
      }
    }

    public loadContent(path: string, storage?: LocalStorage): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        try {
          this.loadContentSync(this.nativeObj, path, storage);
          resolve(undefined);
        } catch (err) {
          reject(err as BusinessError);
        }
      });
    }

    public loadContent(path: string, callback: AsyncCallback<void>): void {
        try {
        this.loadContentSync(this.nativeObj, path, undefined);
        callback(new BusinessError(), undefined);
      } catch (err) {
        callback(err as BusinessError, undefined);
      }
    }

    public disableWindowDecor(): void {
        this.disableWindowDecorSync(this.nativeObj);
    }

    public setShowOnLockScreen(showOnLockScreen: boolean): void {
        this.setShowOnLockScreenSync(this.nativeObj, showOnLockScreen);
    }

    public setWindowModal(isModal: boolean): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
            taskpool.execute((): void => {
                this.setWindowModal(this.nativeObj, isModal);
            }).then((ret: Any): void => {
                resolve(undefined);
            }).catch((err: Any): void => {
                reject(err as BusinessError);
            });
        });
    }
    public setCustomDensity(density: double): void {
        this.setCustomDensitySync(this.nativeObj, density);
    }

    public setDefaultDensityEnabled(enabled: boolean): void {
        this.setDefaultDensityEnabledSync(this.nativeObj, enabled);
    }
    
    public getMainWindowSync(): Window {
        let ret = this.getMainWindowSync(this.nativeObj);
        return ret;
    }

    public getMainWindow(): Promise<Window> {
        return new Promise<Window>((resolve: (value: Window) => void, reject: (error: BusinessError) => void) => {
            taskpool.execute((): Window => {
                let window = this.getMainWindowSync(this.nativeObj);
                return window;
            }).then((ret: Any) => {
                resolve(ret as Window);
            }).catch((err: Any) => {
                reject(err as BusinessError);
            });
        });
    }

    public getMainWindow(callback: AsyncCallback<Window>): void {
        taskpool.execute((): Window => {
            let res = this.getMainWindowSync(this.nativeObj);
            return res;
        }).then((ret: Any) => {
            callback(new BusinessError(), ret as Window);
        }).catch((err: Any) => {
        });
    }

    public createSubWindow(name: string): Promise<Window> {
        return new Promise<Window>((resolve: (value: Window) => void, reject: (error: BusinessError) => void) => {
            taskpool.execute((): Window => {
                let window = this.createSubWindowSync(this.nativeObj, name);
                return window;
            }).then((ret: Any) => {
                resolve(ret as Window);
            }).catch((err: Any) => {
                reject(err as BusinessError);
            });
        });
    }

    public createSubWindow(name: string, callback: AsyncCallback<Window>): void {
        taskpool.execute((): Window => {
            let res = this.createSubWindowSync(this.nativeObj, name);
            return res;
        }).then((ret: Any) => {
            callback(new BusinessError(), ret as Window);
        }).catch((err: Any) => {
        });
    }

    public getSubWindow(): Promise<Array<Window>> {
      return new Promise<Array<Window>>((resolve: (value: Array<Window>) => void,
        reject: (error: BusinessError) => void) => {
          taskpool.execute((): Array<Window> => {
              return this.getSubWindowSync(this.nativeObj);
          }).then((ret: Any) => {
              resolve(ret as Array<Window>);
          }).catch((err: Any) => {
              reject(err as BusinessError);
          });
      });
    }

    public getSubWindow(callback: AsyncCallback<Array<Window>>): void {
      taskpool.execute((): Array<Window> => {
          return this.getSubWindowSync(this.nativeObj);
      }).then((ret: Any) => {
          callback(new BusinessError(), ret as Array<Window>);
      }).catch((err: Any) => {
          callback(err as BusinessError, undefined);
      });
    }

    public createSubWindowWithOptions(name: string, options: SubWindowOptions): Promise<Window> {
      return new Promise<Window>((resolve: (value: Window) => void, reject: (error: BusinessError) => void) => {
          taskpool.execute((): Window => {
              return this.createSubWindowWithOptionsSync(this.nativeObj, name, options);
          }).then((ret: Any) => {
              resolve(ret as Window);
          }).catch((err: Any) => {
              reject(err as BusinessError);
          });
      });
    }

    public setImageForRecent(imgResourceId: long, value: ImageFit): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void) => {
            taskpool.execute((): void => {
                this.setImageForRecentSync(this.nativeObj, imgResourceId, value as int);
            }).then((ret: Any): void => {
                resolve(undefined);
            }).catch((err: Any): void => {
                reject(err as BusinessError);
            });
        });
    }

    public on(eventType: 'windowStageEvent', callback: Callback<WindowStageEventType>): void {
      this.onSync(this.nativeObj, eventType, callback);
    }

    public off(eventType: 'windowStageEvent', callback?: Callback<WindowStageEventType>): void {
      this.offSync(this.nativeObj, eventType, callback);
    }

    public onWindowStageEvent(callback: Callback<void>): void {
      this.onSync(this.nativeObj, 'windowStageEvent', callback);
    }

    public offWindowStageEvent(callback?: Callback<void>): void {
      this.offSync(this.nativeObj, 'windowStageEvent', callback);
    }

    public onWindowStageClose(callback: Callback<void>): void {
      this.onSync(this.nativeObj, 'windowStageClose', callback);
    }

    public offWindowStageClose(callback?: Callback<void>): void {
      this.offSync(this.nativeObj, 'windowStageClose', callback);
    }
}

export interface WindowStage {
    setWindowRectAutoSave(enabled: boolean): Promise<void>;
    isWindowRectAutoSave(): Promise<boolean>;
    setWindowRectAutoSave(enabled: boolean, isSaveBySpecifiedFlag: boolean): Promise<void>;
    removeStartingWindow(): Promise<void>;
    loadContent(path: string, storage: LocalStorage, callback: AsyncCallback<void>): void;
    loadContent(path: string, storage?: LocalStorage): Promise<void>;
    loadContent(path: string, callback: AsyncCallback<void>): void;
    getMainWindowSync(): Window;
    getMainWindow(): Promise<Window>;
    getMainWindow(callback: AsyncCallback<Window>): void;
    createSubWindow(name: string): Promise<Window>;
    createSubWindow(name: string, callback: AsyncCallback<Window>): void;
    disableWindowDecor(): void;
    setShowOnLockScreen(showOnLockScreen: boolean): void;
    getSubWindow(): Promise<Array<Window>>;
    getSubWindow(callback: AsyncCallback<Array<Window>>): void;
    createSubWindowWithOptions(name: string, options: SubWindowOptions): Promise<Window>;
    setWindowModal(isModal: boolean): Promise<void>;
    setCustomDensity(density: double): void;
    setDefaultDensityEnabled(enabled: boolean): void;
    setImageForRecent(imgResourceId: long, value: ImageFit): Promise<void>;
    on(eventType: 'windowStageEvent', callback: Callback<WindowStageEventType>): void;
    off(eventType: 'windowStageEvent', callback?: Callback<WindowStageEventType>): void;
    onWindowStageEvent(callback: Callback<void>): void;
    offWindowStageEvent(callback?: Callback<void>): void;
    onWindowStageClose(callback: Callback<void>): void;
    offWindowStageClose(callback?: Callback<void>): void;
}

export native function windowDestroyCallback(nativeObj: long): void;
let windowDestroyRegister = new FinalizationRegistry<long>(windowDestroyCallback);

export class WindowInternal implements Window {
    static readonly DEFAULT_RET_VAL = 1;
    static { loadLibrary('windowstageani_module.z') }
    private nativeObj: long;

    public setNativeObj(nativeObject: long): void {
        if (!this.nativeObj) {
            windowDestroyRegister.register(this, nativeObject);
        }
        this.nativeObj = nativeObject;
    }

    private static native nativeTransferStatic(input: ESValue): Object;
    private static native nativeTransferDynamic(nativeObj: long): ESValue;

    private native throwIfWindowInvalid(nativeObj: long): void;
    private native setFollowParentWindowLayoutEnabled(nativeObj: long, enabled: boolean): void;
    private native setWindowDelayRaiseOnDrag(nativeObj: long, isEnabled: boolean): void;
    private native getParentWindow(nativeObj: long): Window;
    private native getWindowDecorVisible(nativeObj: long): boolean;
    private native stopMoving(nativeObj: long): void;
    private native setParentWindow(nativeObj: long, windowId: double): void;
    private native setWindowTitle(nativeObj: long, titleName: string): void;
    private native getDecorButtonStyle(nativeObj: long): DecorButtonStyle;
    private native getTitleButtonRect(nativeObj: long): TitleButtonRect;
    private native setTitleButtonVisible(nativeObj: long, visibleParam: TitleButtonVisibleParam): void;
    private native setWindowTitleMoveEnabled(nativeObj: long, enabled: boolean): void;
    private native setWindowTopmost(nativeObj: long, isWindowTopmost: boolean): void;
    private native setTitleAndDockHoverShown(nativeObj: long, isTitleHoverShown: boolean, isDockHoverShown: boolean): void;
    private native restore(nativeObj: long): void;

    private native startMoving(nativeObj: long): void;
    private native startMoveWindowWithCoordinate(nativeObj: long, offsetX: int, offsetY: int): void;
    private native setWindowTitleButtonVisible(nativeObj: long, visibleParam: WindowTitleButtonVisibleParam): void;
    private native setDecorButtonStyle(nativeObj: long, decorStyle: DecorButtonStyle): void;
    private native getWindowStatus(nativeObj: long): int;
    private native minimize(nativeObj: long): void;
    private native maximize(nativeObj: long, presentation: int): void;
    private native resize(nativeObj: long, width: int, height: int): void;
    private native moveWindowTo(nativeObj: long, x: int, y: int): void;
    private native getGlobalRect(nativeObj:long): Rect;
    private native setHandwritingFlag(nativeObj: long, enable: boolean): void;
    private native getWindowDecorHeight(nativeObj:long): int;
    private native setWindowBackgroundColor(nativeObj: long, color: string): int;
    private native setImmersiveModeEnabledState(nativeObj: long, enable: boolean): int;
    private native setWindowDecorVisible(nativeObj: long, isVisible: boolean): int;
    private native setWindowDecorHeight(nativeObj: long, height: int): int;
    private native getWindowProperties(nativeObj: long): WindowProperties;
    private native getProperties(nativeObj: long): WindowProperties;
    private native isWindowSupportWideGamut(nativeObj: long): boolean;
    private native setWindowLayoutFullScreen(nativeObj: long, isLayoutFullScreen: boolean): int;
    private native setRaiseByClickEnabled(nativeObj: long, enable: boolean): int;
    private native setWindowSystemBarProperties(nativeObj: long, systemBarProperties: SystemBarProperties): int;
    private native setExclusivelyHighlighted(nativeObj: long, exclusivelyHighlighted: boolean): int;
    private native setSpecificSystemBarEnabled(nativeObj: long, name: String, enable: boolean, enableAnimation: boolean): int;
    private native snapshot(nativeObj: long): image.PixelMap;
    private native hideNonSystemFloatingWindows(nativeObj: long, shouldHide: boolean): void;
    private native opacity(nativeObj: long, opacity: double): void;
    private native scale(nativeObj: long, scaleOptions: ScaleOptions): void;
    private native translate(nativeObj: long, translateOptions: TranslateOptions): void;
    private native rotate(nativeObj: long, rotateOptions: RotateOptions): void;
    private native setShadow(nativeObj: long, radius: double, color?: string,
      offsetX?: double, offsetY?: double): void;
    private native setBlur(nativeObj: long, radius: double): void;
    private native setBackdropBlurStyle(nativeObj: long, blurStyle: int): void;
    private native setBackdropBlur(nativeObj: long, radius: double): void;
    private native getWindowCornerRadius(nativeObj: long): double;
    private native setWindowCornerRadiusSync(nativeObj: long, cornerRadius: double): void;
    private native setWindowShadowRadius(nativeObj: long, radius: double): void;
    private native setTouchableAreas(nativeObj: long, rects: Array<Rect>): void;
    private native setCornerRadiusSync(nativeObj: long, cornerRadius: double): void;
    private native getTransitionControllerSync(nativeObj: long): TransitionController;

    private native setWindowColorSpaceSync(nativeObj: long, colorSpace: int): void;
    private native setPreferredOrientationSync(nativeObj: long, orientation: int): void;
    private native getPreferredOrientation(nativeObj: long): int;
    private native setWindowPrivacyModeSync(nativeObj: long, isPrivacyMode: boolean): void;
    private native recoverSync(nativeObj: long): void;
    private native setUIContentSync(nativeObj: long, path: string): void;
    private native loadContentSync(nativeObj: long, path: string, storage?: LocalStorage): void;
    private native setWindowKeepScreenOnSync(nativeObj: long, isKeepScreenOn: boolean): void;
    private native setWindowSystemBarEnableSync(nativeObj: long, names: Array<'status' | 'navigation'>): void;
    private native getUIContextSync(nativeObj: long): UIContext;
    private native getWindowAvoidAreaSync(nativeObj: long, type: int): AvoidArea;
    private native setWaterMarkFlagSync(nativeObj: long, enable: boolean): void;
    private native showWindowSync(nativeObj: long): void;
    private native destroyWindowSync(nativeObj: long): void;
    private native isWindowShowingSync(nativeObj: long): boolean;
    private native isWindowHighlightedSync(nativeObj: long): boolean;
    private native hideWithAnimationSync(nativeObj: long): void;
    private native showWithAnimationSync(nativeObj: long): void;
    private native setWindowFocusableSync(nativeObj: long, isFocusable: boolean): void;
    private native getSubWindowZLevelSync(nativeObj: long): int;
    private native raiseAboveTargetSync(nativeObj: long, windowId: int): void;
    private native raiseToAppTopSync(nativeObj: long): void;
    private native setTopmostSync(nativeObj: long, isTopmost: boolean): void;
    private native requestFocusSync(nativeObj: long, isFocused: boolean): void;
    private native setSubWindowModal(nativeObj: long, isModal: boolean): void;
    private native setSubWindowModalType(nativeObj: long, isModal: boolean, modalityType: int): void;
    private native isFocusedSync(nativeObj: long): boolean;
    private native setSubWindowZLevelSync(nativeObj: long, zLevel: int): void;
    private native setWindowBrightness(nativeObj: long, brightness: double): void;
    private native getWindowColorSpace(nativeObj: long): int;
    private native setWakeUpScreen(nativeObj: long, wakeUp: boolean): void;
    private native setSnapshotSkip(nativeObj: long, isSkip: boolean): void;
    private native snapshotIgnorePrivacy(nativeObj: long): image.PixelMap;
    private native setWindowContainerColor(nativeObj: long, activeColor: string, inactiveColor: string): void;
    private native getStatusBarProperty(nativeObj: long): StatusBarProperty;
    private native setStatusBarColor(nativeObj: long, color: long): void;
    private native isSystemAvoidAreaEnabled(nativeObj: long): boolean;
    private native setSystemAvoidAreaEnabled(nativeObj: long, enabled: boolean): void;
    private native getWindowDensityInfo(nativeObj: long): WindowDensityInfo;
    private native getImmersiveModeEnabledState(nativeObj: long): boolean;
    private native setWindowGrayScale(nativeObj: long, grayScale: double): void;
    private native getWindowSystemBarProperties(nativeObj: long): SystemBarProperties;
    private native isGestureBackEnabled(nativeObj: long): boolean;
    private native setGestureBackEnabled(nativeObj: long, enabled: boolean): void;
    private native setSingleFrameComposerEnabled(nativeObj: long, enabled: boolean): void;
    private native setWindowTouchableSync(nativeObj: long, isTouchable: boolean): void;
    private native setDialogBackGestureEnabledSync(nativeObj: long, enabled: boolean): void;
    private native setWindowMaskSync(nativeObj: long, windowMask: Array<Array<long>>): void;
    private native onNoInteractionDetected(nativeObj: long, type: string, timeout: long, callback: Object): void;
    private native keepKeyboardOnFocusSync(nativeObj: long, enable: boolean): void;
    private native createSubWindowWithOptionsSync(nativeObj: long, name: string, options: SubWindowOptions): Window;
    private native hideSync(nativeObj: long): void;
    private native onSync(nativeObj: long, type: string, callback: object): void;
    private native offSync(nativeObj: long, type: string, callback?: object): void;

    public static transferStatic(input: Any): Object {
      hilog.info(DOMAIN, TAG, 'window transfer static, input:' + input);
      return WindowInternal.nativeTransferStatic(ESValue.wrap(input));
    }

    public static transferDynamic(input: Object): Any {
      hilog.info(DOMAIN, TAG, 'window transfer dynamic, input:' + input);
      return WindowInternal.nativeTransferDynamic((input as WindowInternal).nativeObj).unwrap();
    }

    public setFollowParentWindowLayoutEnabled(enabled: boolean): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void ): void => {
        taskpool.execute((): void => {
          this.setFollowParentWindowLayoutEnabled(this.nativeObj, enabled);
        }).then((ret: Any): void => {
          resolve(undefined);
        }).catch((err: Any): void => {
          reject(err as BusinessError);
        });
      });
    }

    public setWindowDelayRaiseOnDrag(isEnabled: boolean): void {
      this.setWindowDelayRaiseOnDrag(this.nativeObj, isEnabled);
    }

    public getParentWindow(): Window {
      return this.getParentWindow(this.nativeObj);
    }

    public getWindowDecorVisible(): boolean {
      return this.getWindowDecorVisible(this.nativeObj);
    }

    public stopMoving(): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void ): void => {
        taskpool.execute((): void => {
          this.stopMoving(this.nativeObj);
        }).then((ret: Any): void => {
          resolve(undefined);
        }).catch((err: Any): void => {
          reject(err as BusinessError);
        });
      });
    }

    public setParentWindow(windowId: number): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void ): void => {
        taskpool.execute((): void => {
          this.setParentWindow(this.nativeObj, windowId);
        }).then((ret: Any): void => {
          resolve(undefined);
        }).catch((err: Any): void => {
          reject(err as BusinessError);
        });
      });
    }

    public setWindowTitle(titleName: string): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void ): void => {
        taskpool.execute((): void => {
          this.setWindowTitle(this.nativeObj, titleName);
        }).then((ret: Any): void => {
          resolve(undefined);
        }).catch((err: Any): void => {
          reject(err as BusinessError);
        });
      });
    }

    public getDecorButtonStyle(): DecorButtonStyle {
      return this.getDecorButtonStyle(this.nativeObj);
    }

    public getTitleButtonRect(): TitleButtonRect {
      return this.getTitleButtonRect(this.nativeObj);
    }

    public setTitleButtonVisible(isMaximizeVisible: boolean, isMinimizeVisible: boolean,
                                 isSplitVisible: boolean): void {
      let visibleParam: window.TitleButtonVisibleParam = {
        isMaximizeVisible: isMaximizeVisible,
        isMinimizeVisible: isMinimizeVisible,
        isSplitVisible: isSplitVisible,
        isCloseVisible: true,
      };
      this.setTitleButtonVisible(this.nativeObj, visibleParam);
    }

    public setWindowTitleMoveEnabled(enabled: boolean): void {
      this.setWindowTitleMoveEnabled(this.nativeObj, enabled);
    }

    public setWindowTopmost(isWindowTopmost: boolean): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setWindowTopmost(this.nativeObj, isWindowTopmost);
        }).then((ret: Any): void => {
          resolve(undefined);
        }).catch((err: Any): void => {
          reject(err as BusinessError);
        });
      });
    }

    public setTitleAndDockHoverShown(isTitleHoverShown?: boolean, isDockHoverShown?: boolean): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void ): void => {
        taskpool.execute((): void => {
          let titleShow: boolean = true;
          if (isTitleHoverShown != undefined) {
            titleShow = isTitleHoverShown;
          }
          let dockShow: boolean = true;
          if (isDockHoverShown != undefined) {
            dockShow = isDockHoverShown;
          }
          this.setTitleAndDockHoverShown(this.nativeObj, titleShow, dockShow);
        }).then((ret: Any): void => {
          resolve(undefined);
        }).catch((err: Any): void => {
          reject(err as BusinessError);
        });
      });
    }

    public restore(): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void ): void => {
        taskpool.execute((): void => {
          this.restore(this.nativeObj);
        }).then((ret: Any): void => {
          resolve(undefined);
        }).catch((err: Any): void => {
          reject(err as BusinessError);
        });
      });
    }

    public startMoving(): Promise<void> {
      this.throwIfWindowInvalid(this.nativeObj);
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void ): void => {
        taskpool.execute((): void => {
          this.startMoving(this.nativeObj);
        }).then((ret: Any): void => {
            resolve(undefined);
        }).catch((err: Any): void => {
            reject(err as BusinessError);
        });
      });
    }

    public startMoving(offsetX: int, offsetY: int): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void ): void => {
        taskpool.execute((): void => {
          this.startMoveWindowWithCoordinate(this.nativeObj, offsetX, offsetY);
        }).then((ret: Any): void => {
            resolve(undefined);
        }).catch((err: Any): void => {
            reject(err as BusinessError);
        });
      });
    }

    public setWindowTitleButtonVisible(isMaximizeButtonVisible: boolean, isMinimizeButtonVisible: boolean,
                                       isCloseButtonVisible?: boolean): void {
      let visibleParam: window.WindowTitleButtonVisibleParam = {
        isMaximizeButtonVisible: isMaximizeButtonVisible,
        isMinimizeButtonVisible: isMinimizeButtonVisible,
        isCloseButtonVisible: true
      };
      if (isCloseButtonVisible != undefined) {
        visibleParam.isCloseButtonVisible = isCloseButtonVisible;
      }
      this.setWindowTitleButtonVisible(this.nativeObj, visibleParam);
    }

    public setDecorButtonStyle(decorStyle: DecorButtonStyle): void {
      this.setDecorButtonStyle(this.nativeObj, decorStyle);
    }

    public getWindowStatus(): WindowStatusType {
      return this.getWindowStatus(this.nativeObj) as WindowStatusType;
    }

    public minimize(): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void ): void => {
        taskpool.execute((): void => {
          this.minimize(this.nativeObj);
        }).then((ret: Any): void => {
            resolve(undefined);
        }).catch((err: Any): void => {
            reject(err as BusinessError);
        });
      });
    }

    public minimize(callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
          this.minimize(this.nativeObj);
      }).then((ret: Any) => {
          callback(new BusinessError(), undefined);
      }).catch((err: Any) => {
          callback(err as BusinessError, undefined);
      });
    }

    public maximize(presentation?: MaximizePresentation): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void ): void => {
        taskpool.execute((): void => {
          if (presentation == undefined) {
            this.maximize(this.nativeObj, MaximizePresentation.ENTER_IMMERSIVE as int);
          }
          else {
            this.maximize(this.nativeObj, presentation as int);
          }
        }).then((ret: Any): void => {
            resolve(undefined);
        }).catch((err: Any): void => {
            reject(err as BusinessError);
        });
      });
    }
    
    public resize(width: int, height: int): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.resize(this.nativeObj, width, height);
        }).then((ret: Any): void => {
            resolve(undefined);
        }).catch((err: Any): void => {
            reject(err as BusinessError);
        });
      });
    }

    public moveWindowTo(x: int, y: int): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.moveWindowTo(this.nativeObj, x, y);
        }).then((ret: Any): void => {
            resolve(undefined);
        }).catch((err: Any): void => {
            reject(err as BusinessError);
        });
      });
    }

    public resize(width: int, height: int, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.resize(this.nativeObj, width, height);
        }).then((ret: Any) => {
            callback(new BusinessError(), undefined);
        }).catch((err: Any) => {
            callback(err as BusinessError, undefined);
        });
    }

    public moveWindowTo(x: int, y: int, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.moveWindowTo(this.nativeObj, x, y);
        }).then((ret: Any) => {
            callback(new BusinessError(), undefined);
        }).catch((err: Any) => {
            callback(err as BusinessError, undefined);
        });
    }

    public getGlobalRect(): Rect {
       let ret = this.getGlobalRect(this.nativeObj);
       return ret;
    }

    public setHandwritingFlag(enable: boolean): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
            taskpool.execute((): void => {
                this.setHandwritingFlag(this.nativeObj, enable);
            }).then((ret: Any): void => {
                resolve(undefined);
            }).catch((err: Any): void => {
                reject(err as BusinessError);
            });
        });
    }

    public getWindowDecorHeight(): int {
       let ret = this.getWindowDecorHeight(this.nativeObj);
       return ret;
    }

    public setWindowBackgroundColor(color: string | ColorMetrics): void {
       if (color instanceof string) {
         this.setWindowBackgroundColor(this.nativeObj, color as string);
       }
    }

    public setBackgroundColor(color: string): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
            taskpool.execute((): void => {
                this.setWindowBackgroundColor(this.nativeObj, color);
            }).then((ret: Any): void => {
                resolve(undefined);
            }).catch((err: Any): void => {
                reject(err as BusinessError);
            });
        });
    }

    public setBackgroundColor(color: string, callback: AsyncCallback<void>): void {
        taskpool.execute((): void => {
            this.setWindowBackgroundColor(this.nativeObj, color);
        }).then((ret: Any) => {
            callback(new BusinessError(), undefined);
        }).catch((err: Any) => {
            callback(err as BusinessError, undefined);
        });
    }

    public setImmersiveModeEnabledState(enable: boolean): void {
        this.setImmersiveModeEnabledState(this.nativeObj, enable);
    }

    public setWindowDecorVisible(isVisible: boolean): void {
        this.setWindowDecorVisible(this.nativeObj, isVisible);
    }

    public setWindowDecorHeight(height: int): void {
        this.setWindowDecorHeight(this.nativeObj, height);
    }

    public getWindowProperties(): WindowProperties {
        let windowProperties = this.getWindowProperties(this.nativeObj) as WindowPropertiesInternal;
        return windowProperties;
    }

    public getProperties(): Promise<WindowProperties> {
        return new Promise<WindowProperties>((resolve: (value: WindowProperties) => void,
            reject: (error: BusinessError) => void) => {
            taskpool.execute((): WindowProperties => {
                let res = this.getProperties(this.nativeObj) as WindowPropertiesInternal;
                return res;
            }).then((ret: Any) => {
                resolve(ret as WindowProperties);
            }).catch((err: Any) => {
                reject(err as BusinessError);
            });
        });
    }

    public getProperties(callback: AsyncCallback<WindowProperties>): void {
        taskpool.execute((): WindowProperties => {
            let res = this.getProperties(this.nativeObj) as WindowPropertiesInternal;
            return res;
        }).then((ret: Any) => {
            callback(new BusinessError(), ret as WindowProperties);
        }).catch((err: Any) => {
            callback(err as BusinessError, new WindowPropertiesInternal());
        });
    }

    public isWindowSupportWideGamut(): Promise<boolean> {
        return new Promise<boolean>((resolve: (value: boolean) => void, reject: (error: BusinessError) => void) => {
            taskpool.execute((): boolean => {
                let res = this.isWindowSupportWideGamut(this.nativeObj);
                return res;
            }).then((ret: Any) => {
                resolve(ret as boolean);
            }).catch((err: Any) => {
                reject(err as BusinessError);
            });
        });
    }

    public isWindowSupportWideGamut(callback: AsyncCallback<boolean>): void {
        taskpool.execute((): boolean => {
            let res = this.isWindowSupportWideGamut(this.nativeObj);
            return res;
        }).then((ret: Any) => {
            callback(new BusinessError(), ret as boolean);
        }).catch((err: Any) => {
            callback(err as BusinessError, false);
        });
    }

    public setWindowLayoutFullScreen(isLayoutFullScreen: boolean): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
            taskpool.execute((): void => {
                this.setWindowLayoutFullScreen(this.nativeObj, isLayoutFullScreen);
            }).then((ret: Any): void => {
                resolve(undefined);
            }).catch((err: Any): void => {
                reject(err as BusinessError);
            });
        });
    }

    public setWindowLayoutFullScreen(isLayoutFullScreen: boolean, callback: AsyncCallback<void>): void {
        taskpool.execute((): void => {
            this.setWindowLayoutFullScreen(this.nativeObj, isLayoutFullScreen);
        }).then((ret: Any) => {
            callback(new BusinessError(), undefined);
        }).catch((err: Any) => {
            callback(err as BusinessError, undefined);
        });
    }

    public setRaiseByClickEnabled(enable: boolean): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
            taskpool.execute((): void => {
                this.setRaiseByClickEnabled(this.nativeObj, enable);
            }).then((ret: Any): void => {
                resolve(undefined);
            }).catch((err: Any): void => {
                reject(err as BusinessError);
            });
        });
    }

    public setRaiseByClickEnabled(enable: boolean, callback: AsyncCallback<void>): void {
        taskpool.execute((): void => {
            this.setRaiseByClickEnabled(this.nativeObj, enable);
        }).then((ret: Any) => {
            callback(new BusinessError(), undefined);
        }).catch((err: Any) => {
            callback(err as BusinessError, undefined);
        });
    }

    public setExclusivelyHighlighted(exclusivelyHighlighted: boolean): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
            taskpool.execute((): void => {
                this.setExclusivelyHighlighted(this.nativeObj, exclusivelyHighlighted);
            }).then((ret: Any): void => {
                resolve(undefined);
            }).catch((err: Any): void => {
                reject(err as BusinessError);
            });
        });
    }

    public setWindowSystemBarProperties(systemBarProperties: SystemBarProperties): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
            taskpool.execute((): void => {
                this.setWindowSystemBarProperties(this.nativeObj, systemBarProperties);
            }).then((ret: Any): void => {
                resolve(undefined);
            }).catch((err: Any): void => {
                reject(err as BusinessError);
            });
        });
    }

    public setWindowSystemBarProperties(systemBarProperties: SystemBarProperties, callback: AsyncCallback<void>): void {
        taskpool.execute((): void => {
            this.setWindowSystemBarProperties(this.nativeObj, systemBarProperties);
        }).then((ret: Any) => {
            callback(new BusinessError(), undefined);
        }).catch((err: Any) => {
            callback(err as BusinessError, undefined);
        });
    }

    public setSpecificSystemBarEnabled(name: String, enable: boolean, enableAnimation?: boolean): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
            taskpool.execute((): void => {
                if (enableAnimation == undefined) {
                    this.setSpecificSystemBarEnabled(this.nativeObj, name, enable, false);
                } else {
                    this.setSpecificSystemBarEnabled(this.nativeObj, name, enable, enableAnimation);
                }
            }).then((ret: Any): void => {
                resolve(undefined);
            }).catch((err: Any): void => {
                reject(err as BusinessError);
            });
        });
    }

    public snapshot(): Promise<image.PixelMap> {
      return new Promise<image.PixelMap>((resolve: (value: image.PixelMap) => void, reject: (error: BusinessError) => void) => {
        taskpool.execute(() => {
          return this.snapshot(this.nativeObj);
        }).then((ret: Any) => {
          resolve(ret as image.PixelMap);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public snapshot(callback: AsyncCallback<image.PixelMap | undefined>): void {
      taskpool.execute(() => {
        return this.snapshot(this.nativeObj);
      }).then((ret: Any) => {
        callback(new BusinessError(), ret as image.PixelMap);
      }).catch((err: Any) => {
        callback(err as BusinessError, undefined);
      });
    }

    public hideNonSystemFloatingWindows(shouldHide: boolean): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute(() => {
          this.hideNonSystemFloatingWindows(this.nativeObj, shouldHide);
        }).then(() => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public hideNonSystemFloatingWindows(shouldHide: boolean, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.hideNonSystemFloatingWindows(this.nativeObj, shouldHide);
      }).then(() => {
        callback(new BusinessError(), undefined);
      }).catch((err: Any) => {
        callback(err as BusinessError, undefined);
      });
    }

    public setWindowColorSpace(colorSpace: ColorSpace): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setWindowColorSpaceSync(this.nativeObj, colorSpace as int);
        }).then((ret: Any) => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public setWindowColorSpace(colorSpace: ColorSpace, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.setWindowColorSpaceSync(this.nativeObj, colorSpace as int);
      }).then((ret: Any) => {
        callback(new BusinessError(), undefined);
      }).catch((err: Any) => {
        callback(err as BusinessError, undefined);
      });
    }

    public setPreferredOrientation(orientation: Orientation): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setPreferredOrientationSync(this.nativeObj, orientation as int);
        }).then((ret: Any) => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public setPreferredOrientation(orientation: Orientation, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.setPreferredOrientationSync(this.nativeObj, orientation as int);
      }).then((ret: Any) => {
        callback(new BusinessError(), undefined);
      }).catch((err: Any) => {
        callback(err as BusinessError, undefined);
      });
    }

    public getPreferredOrientation(): Orientation {
      return this.getPreferredOrientation(this.nativeObj) as Orientation;
    }

    public setWindowPrivacyMode(isPrivacyMode: boolean): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setWindowPrivacyModeSync(this.nativeObj, isPrivacyMode);
        }).then((ret: Any) => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public setWindowPrivacyMode(isPrivacyMode: boolean, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.setWindowPrivacyModeSync(this.nativeObj, isPrivacyMode);
      }).then((ret: Any) => {
        callback(new BusinessError(), undefined);
      }).catch((err: Any) => {
        callback(err as BusinessError, undefined);
      });
    }

    public recover(): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.recoverSync(this.nativeObj);
        }).then((ret: Any) => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public setUIContent(path: string): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        try {
          this.setUIContentSync(this.nativeObj, path);
          resolve(undefined);
        } catch (err) {
          reject(err as BusinessError);
        }
      });
    }

    public setUIContent(path: string, callback: AsyncCallback<void>): void {
      try {
        this.setUIContentSync(this.nativeObj, path);
        callback(new BusinessError(), undefined);
      } catch (err) {
        callback(err as BusinessError, undefined);
      }
    }

    public loadContent(path: string, storage: LocalStorage): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        try {
          this.loadContentSync(this.nativeObj, path, storage);
          resolve(undefined);
        } catch (err) {
          reject(err as BusinessError);
        }
      });
    }

    public loadContent(path: string, storage: LocalStorage, callback: AsyncCallback<void>): void {
      try {
        this.loadContentSync(this.nativeObj, path, storage);
        callback(new BusinessError(), undefined);
      } catch (err) {
        callback(err as BusinessError, undefined);
      }
    }

    public loadContent(path: string): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        try {
          this.loadContentSync(this.nativeObj, path);
          resolve(undefined);
        } catch (err) {
          reject(err as BusinessError);
        }
      });
    }

    public loadContent(path: string, callback: AsyncCallback<void>): void {
      try {
        this.loadContentSync(this.nativeObj, path);
        callback(new BusinessError(), undefined);
      } catch (err) {
        callback(err as BusinessError, undefined);
      }
    }

    public onAvoidAreaChange(callback: Callback<AvoidAreaOptions>): void {
      this.onSync(this.nativeObj, 'avoidAreaChange', callback);
    }

    public offAvoidAreaChange(callback?: Callback<AvoidAreaOptions>): void {
      this.offSync(this.nativeObj, 'avoidAreaChange', callback);
    }

    public onDisplayIdChange(callback: Callback<long>): void {
      this.onSync(this.nativeObj, 'displayIdChange', callback);
    }

    public offDisplayIdChange(callback?: Callback<long>): void {
      this.offSync(this.nativeObj, 'displayIdChange', callback);
    }

    public onWindowVisibilityChange(callback: Callback<boolean>) {
      this.onSync(this.nativeObj, 'windowVisibilityChange', callback);
    }

    public offWindowVisibilityChange(callback?: Callback<boolean>): void {
      this.offSync(this.nativeObj, 'windowVisibilityChange', callback);
    }

    public onSystemDensityChange(callback: Callback<double>): void {
      this.onSync(this.nativeObj, 'systemDensityChange', callback);
    }

    public offSystemDensityChange(callback?: Callback<double>): void {
      this.offSync(this.nativeObj, 'systemDensityChange', callback);
    }

    public onScreenshot(callback: Callback<void>): void {
      this.onSync(this.nativeObj, 'screenshot', callback);
    }

    public offScreenshot(callback?: Callback<void>): void {
      this.offSync(this.nativeObj, 'screenshot', callback);
    }

    public setWindowKeepScreenOn(isKeepScreenOn: boolean): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
            taskpool.execute((): void => {
                this.setWindowKeepScreenOnSync(this.nativeObj, isKeepScreenOn);
            }).then((ret: Any): void => {
                resolve(ret as undefined);
            }).catch((err: Any): void => {
                reject(err as BusinessError);
            });
        });
    }

    public setWindowKeepScreenOn(isKeepScreenOn: boolean, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.setWindowKeepScreenOnSync(this.nativeObj, isKeepScreenOn);
      }).then((ret: Any) => {
        callback(new BusinessError(), undefined);
      }).catch((err: Any) => {
        callback(err as BusinessError, undefined);
      });
    }

    public setWindowSystemBarEnable(names: Array<'status' | 'navigation'>): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
            taskpool.execute((): void => {
                this.setWindowSystemBarEnableSync(this.nativeObj, names);
            }).then((ret: Any) => {
                resolve(undefined);
            }).catch((err: Any) => {
                reject(err as BusinessError);
            });
        });
    }

    public setWindowSystemBarEnable(names: Array<'status' | 'navigation'>, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.setWindowSystemBarEnableSync(this.nativeObj, names);
      }).then((ret: Any) => {
        callback(new BusinessError(), undefined);
      }).catch((err: Any) => {
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
        }).then((ret: Any) => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public setWaterMarkFlag(enable: boolean, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.setWaterMarkFlagSync(this.nativeObj, enable);
      }).then((ret: Any) => {
        callback(new BusinessError(), undefined);
      }).catch((err: Any) => {
        callback(err as BusinessError, undefined);
      });
    }

    public setWindowFocusable(isFocusable: boolean): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setWindowFocusableSync(this.nativeObj, isFocusable);
        }).then((ret: Any) => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public setWindowFocusable(isFocusable: boolean, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.setWindowFocusableSync(this.nativeObj, isFocusable);
      }).then((ret: Any) => {
        callback(new BusinessError(), undefined);
      }).catch((err: Any) => {
        callback(err as BusinessError, undefined);
      });
    }

    public getSubWindowZLevel(): int {
      let ret = this.getSubWindowZLevelSync(this.nativeObj);
      return ret;
    };

    public isFocused(): boolean {
      return this.isFocusedSync(this.nativeObj);
    };

    public setSubWindowZLevel(zLevel: int): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setSubWindowZLevelSync(this.nativeObj, zLevel);
        }).then((ret: Any) => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public raiseAboveTarget(windowId: int): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.raiseAboveTargetSync(this.nativeObj, windowId);
        }).then((ret: Any): void => {
          resolve(undefined);
        }).catch((err: Any): void => {
          reject(err as BusinessError);
        });
      });
    }

    public raiseAboveTarget(windowId: int, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.raiseAboveTargetSync(this.nativeObj, windowId);
      }).then((ret: Any) => {
        callback(new BusinessError(), undefined);
      }).catch((err: Any) => {
        callback(err as BusinessError, undefined);
      });
    }

    public raiseToAppTop(): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.raiseToAppTopSync(this.nativeObj);
        }).then((ret: Any): void => {
          resolve(undefined);
        }).catch((err: Any): void => {
          reject(err as BusinessError);
        });
      });
    }

    public raiseToAppTop(callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.raiseToAppTopSync(this.nativeObj);
      }).then((ret: Any) => {
        callback(new BusinessError(), undefined);
      }).catch((err: Any) => {
        callback(err as BusinessError, undefined);
      });
    }

    public setTopmost(isTopmost: boolean): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setTopmostSync(this.nativeObj, isTopmost);
        }).then((ret: Any): void => {
          resolve(undefined);
        }).catch((err: Any): void => {
          reject(err as BusinessError);
        });
      });
    }

    public requestFocus(isFocused: boolean): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.requestFocusSync(this.nativeObj, isFocused);
        }).then((ret: Any): void => {
          resolve(undefined);
        }).catch((err: Any): void => {
          reject(err as BusinessError);
        });
      });
    }

    public setSubWindowModal(isModal: boolean): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
            taskpool.execute((): void => {
                this.setSubWindowModal(this.nativeObj, isModal);
            }).then((ret: Any): void => {
                resolve(undefined);
            }).catch((err: Any): void => {
                reject(err as BusinessError);
            });
        });
    }

    public setSubWindowModal(isModal: boolean, modalityType: ModalityType): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
            taskpool.execute((): void => {
                this.setSubWindowModalType(this.nativeObj, isModal, modalityType);
            }).then((ret: Any): void => {
                resolve(undefined);
            }).catch((err: Any): void => {
                reject(err as BusinessError);
            });
        });
    }

    public keepKeyboardOnFocus(keepKeyboardFlag: boolean): void {
      this.keepKeyboardOnFocusSync(this.nativeObj, keepKeyboardFlag);
    }

    public setWindowTouchable(isTouchable: boolean): Promise<void> {
       return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
         taskpool.execute((): void => {
           this.setWindowTouchableSync(this.nativeObj, isTouchable);
         }).then((ret: Any) => {
           resolve(undefined);
         }).catch((err: Any) => {
           reject(err as BusinessError);
         });
       });
     }

    public setWindowTouchable(isTouchable: boolean, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.setWindowTouchableSync(this.nativeObj, isTouchable);
      }).then((ret: Any) => {
        callback(new BusinessError(), undefined);
      }).catch((err: Any) => {
        callback(err as BusinessError, undefined);
      });
    }

    public setDialogBackGestureEnabled(enabled: boolean): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
         taskpool.execute((): void => {
           this.setDialogBackGestureEnabledSync(this.nativeObj, enabled);
         }).then((ret: Any) => {
           resolve(undefined);
         }).catch((err: Any) => {
           reject(err as BusinessError);
         });
       });
    }

    public setWindowMask(windowMask: Array<Array<long>>): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setWindowMaskSync(this.nativeObj, windowMask);
        }).then((ret: Any) => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public setTouchableAreas(rects: Array<Rect>): void {
      this.setTouchableAreas(this.nativeObj, rects);
    }

    public opacity(opacity: double): void {
      this.opacity(this.nativeObj, opacity);
    }

    public scale(scaleOptions: ScaleOptions): void {
      this.scale(this.nativeObj, scaleOptions);
    }

    public translate(translateOptions: TranslateOptions): void {
      this.translate(this.nativeObj, translateOptions);
    }

    public rotate(rotateOptions: RotateOptions): void {
      this.rotate(this.nativeObj, rotateOptions);
    }

    public setShadow(radius: double, color?: string, offsetX?: double, offsetY?: double): void {
      this.setShadow(this.nativeObj, radius, color, offsetX, offsetY);
    }

    public setCornerRadius(cornerRadius: double): void {
      this.setCornerRadiusSync(this.nativeObj, cornerRadius);
    }

    public setBlur(radius: double): void {
      this.setBlur(this.nativeObj, radius);
    }

    public setBackdropBlurStyle(blurStyle: BlurStyle): void {
      this.setBackdropBlurStyle(this.nativeObj, blurStyle);
    }

    public setBackdropBlur(radius: double): void {
      this.setBackdropBlur(this.nativeObj, radius);
    }

    public getWindowCornerRadius(): double {
      return this.getWindowCornerRadius(this.nativeObj);
    }

    public setWindowCornerRadius(cornerRadius: double): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setWindowCornerRadiusSync(this.nativeObj, cornerRadius);
        }).then((ret: Any) => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public setWindowShadowRadius(radius: double): void {
      this.setWindowShadowRadius(this.nativeObj, radius);
    }
    public getTransitionController(): TransitionController {
      return this.getTransitionControllerSync(this.nativeObj);
    }

    public onWindowHighlightChange(callback: Callback<boolean>): void {
      this.onSync(this.nativeObj, "windowHighlightChange", callback);
    }

    public offWindowHighlightChange(callback?: Callback<boolean>): void {
      this.offSync(this.nativeObj, "windowHighlightChange", callback);
    }

    public on<T>(type: string, callback: Callback<T>): void {
      this.onSync(this.nativeObj, type, callback);
    }

    public on<T>(type: 'noInteractionDetected', timeout: long, callback: Callback<T>): void {
      this.onNoInteractionDetected(this.nativeObj, type, timeout, callback as WindowVoidCallback);
    }

    public onTouchOutside(callback: Callback<void>): void {
      this.onSync(this.nativeObj, 'touchOutside', callback);
    }

    public onDialogTargetTouch(callback: Callback<void>): void {
      this.onSync(this.nativeObj, 'dialogTargetTouch', callback);
    }

    public onNoInteractionDetected(timeout: long, callback: Callback<void>): void {
      this.onNoInteractionDetected(this.nativeObj, 'noInteractionDetected', timeout, callback as WindowVoidCallback);
    }

    public onKeyboardHeightChange(callback: Callback<int>): void {
      this.onSync(this.nativeObj, "keyboardHeightChange", callback);
    }

    public onKeyboardDidShow(callback: Callback<KeyboardInfo>): void {
      this.onSync(this.nativeObj, "keyboardDidShow", callback);
    }

    public onKeyboardDidHide(callback: Callback<KeyboardInfo>): void {
      this.onSync(this.nativeObj, "keyboardDidHide", callback);
    }

    public onRotationChange(callback: RotationChangeCallback<RotationChangeInfo,
      RotationChangeResult | undefined>): void {
      this.onSync(this.nativeObj, "rotationChange", callback);
    }

    public off<T>(type: string, callback?: Callback<T>): void {
      this.offSync(this.nativeObj, type, callback);
    }

    public offKeyboardHeightChange(callback?: Callback<int>): void {
      this.offSync(this.nativeObj, "keyboardHeightChange", callback);
    }

    public offKeyboardDidShow(callback?: Callback<KeyboardInfo>): void {
      this.offSync(this.nativeObj, "keyboardDidShow", callback);
    }

    public offKeyboardDidHide(callback?: Callback<KeyboardInfo>): void {
      this.offSync(this.nativeObj, "keyboardDidHide", callback);
    }

    public offRotationChange(callback?: RotationChangeCallback<RotationChangeInfo,
      RotationChangeResult | undefined>): void {
      this.offSync(this.nativeObj, "rotationChange", callback);
    }

    public offTouchOutside(callback?: Callback<void>): void {
      this.offSync(this.nativeObj, 'touchOutside', callback);
    }

    public offDialogTargetTouch(callback?: Callback<void>): void {
      this.offSync(this.nativeObj, 'dialogTargetTouch', callback);
    }

    public offNoInteractionDetected(callback?: Callback<void>): void {
      this.offSync(this.nativeObj, 'noInteractionDetected', callback);
    }

    public showWindow(callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.showWindowSync(this.nativeObj);
      }).then((ret: Any) => {
        callback(new BusinessError(), undefined);
      }).catch((err: Any) => {
        callback(err as BusinessError, undefined);
      });
    }

    public showWindow(): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.showWindowSync(this.nativeObj);
        }).then((ret: Any) => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public destroyWindow(callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.destroyWindowSync(this.nativeObj);
      }).then((ret: Any) => {
        callback(new BusinessError(), undefined);
      }).catch((err: Any) => {
        callback(err as BusinessError, undefined);
      });
    }

    public destroyWindow(): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.destroyWindowSync(this.nativeObj);
        }).then((ret: Any) => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public isWindowShowing(): boolean {
      return this.isWindowShowingSync(this.nativeObj);
    }

    public isWindowHighlighted(): boolean {
      return this.isWindowHighlightedSync(this.nativeObj);
    }
    
    public hideWithAnimation(callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.hideWithAnimationSync(this.nativeObj);
      }).then((ret: Any) => {
        callback(new BusinessError(), undefined);
      }).catch((err: Any) => {
        callback(err as BusinessError, undefined);
      });
    }

    public hideWithAnimation(): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.hideWithAnimationSync(this.nativeObj);
        }).then((ret: Any) => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public showWithAnimation(callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.showWithAnimationSync(this.nativeObj);
      }).then((ret: Any) => {
        callback(new BusinessError(), undefined);
      }).catch((err: Any) => {
        callback(err as BusinessError, undefined);
      });
    }

    public showWithAnimation(): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.showWithAnimationSync(this.nativeObj);
        }).then((ret: Any) => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public createSubWindowWithOptions(name: string, options: SubWindowOptions): Promise<Window> {
      return new Promise<Window>((resolve: (value: Window) => void, reject: (error: BusinessError) => void) => {
        taskpool.execute((): Window => {
          return this.createSubWindowWithOptionsSync(this.nativeObj, name, options); 
        }).then((ret: Any) => {
          resolve(ret as Window);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public hide(): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void,
        reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.hideSync(this.nativeObj);
        }).then((ret: Any) => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public hide(callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.hideSync(this.nativeObj);
      }).then((ret: Any) => {
        callback(new BusinessError(), undefined);
      }).catch((err: Any) => {
        callback(err as BusinessError, undefined);
      });
    }

    public setWindowBrightness(brightness: double): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setWindowBrightness(this.nativeObj, brightness);
        }).then((ret: Any) => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public setWindowBrightness(brightness: double, callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
        this.setWindowBrightness(this.nativeObj, brightness);
      }).then((ret: Any) => {
        callback(new BusinessError(), undefined);
      }).catch((err: Any) => {
        callback(err as BusinessError, undefined);
      });
    }

    public getWindowColorSpace(): ColorSpace {
      return this.getWindowColorSpace(this.nativeObj) as ColorSpace;
    }

    public setWakeUpScreen(wakeUp: boolean): void {
      return this.setWakeUpScreen(this.nativeObj, wakeUp);
    }

    public setSnapshotSkip(isSkip: boolean): void {
      return this.setSnapshotSkip(this.nativeObj, isSkip);
    }

    public snapshotIgnorePrivacy(): Promise<image.PixelMap> {
      return new Promise<image.PixelMap>((resolve: (value: image.PixelMap) => void,
        reject: (error: BusinessError) => void): void => {
        taskpool.execute(() => {
          return this.snapshotIgnorePrivacy(this.nativeObj);
        }).then((ret: Any) => {
          resolve(ret as image.PixelMap);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public setWindowContainerColor(activeColor: string, inactiveColor: string): void {
      return this.setWindowContainerColor(this.nativeObj, activeColor, inactiveColor);
    }

    public getStatusBarProperty(): StatusBarProperty {
      return this.getStatusBarProperty(this.nativeObj);
    }

    public setStatusBarColor(color: ColorMetrics): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setStatusBarColor(
            this.nativeObj, (color.alpha << 24) | (color.red << 16) | (color.green << 8) | color.blue);
        }).then((ret: Any) => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public isSystemAvoidAreaEnabled(): boolean {
      return this.isSystemAvoidAreaEnabled(this.nativeObj);
    }

    public setSystemAvoidAreaEnabled(enabled: boolean): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setSystemAvoidAreaEnabled(this.nativeObj, enabled);
        }).then((ret: Any) => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public getWindowDensityInfo(): WindowDensityInfo {
      return this.getWindowDensityInfo(this.nativeObj) as WindowDensityInfoInternal;
    }

    public getImmersiveModeEnabledState(): boolean {
      return this.getImmersiveModeEnabledState(this.nativeObj);
    }

    public setWindowGrayScale(grayScale: double): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setWindowGrayScale(this.nativeObj, grayScale);
        }).then((ret: Any) => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public getWindowSystemBarProperties(): SystemBarProperties {
      return this.getWindowSystemBarProperties(this.nativeObj);
    }

    public isGestureBackEnabled(): boolean {
      return this.isGestureBackEnabled(this.nativeObj);
    }

    public setGestureBackEnabled(enabled: boolean): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setGestureBackEnabled(this.nativeObj, enabled);
        }).then((ret: Any) => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public setSingleFrameComposerEnabled(enabled: boolean): Promise<void> {
      return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
        taskpool.execute((): void => {
          this.setSingleFrameComposerEnabled(this.nativeObj, enabled);
        }).then((ret: Any) => {
          resolve(undefined);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public onWindowSizeChange(callback: Callback<Size>): void {
      this.onSync(this.nativeObj, 'windowSizeChange', callback);
    }

    public offWindowSizeChange(callback?: Callback<Size>): void {
      this.offSync(this.nativeObj, 'windowSizeChange', callback);
    }

    public onWindowStatusChange(callback: Callback<WindowStatusType>): void {
      this.onSync(this.nativeObj, 'windowStatusChange', callback);
    }

    public offWindowStatusChange(callback?: Callback<WindowStatusType>): void {
      this.offSync(this.nativeObj, 'windowStatusChange', callback);
    }

    public onWindowRectChange(callback: Callback<RectChangeOptions>): void {
      this.onSync(this.nativeObj, 'windowRectChange', callback);
    }

    public offWindowRectChange(callback?: Callback<RectChangeOptions>): void {
      this.offSync(this.nativeObj, 'windowRectChange', callback);
    }

    public onWindowWillClose(callback: Callback<Promise<boolean>>): void {
      this.onSync(this.nativeObj, 'windowWillClose', callback);
    }

    public offWindowWillClose(callback?: Callback<Promise<boolean>>): void {
      this.offSync(this.nativeObj, 'windowWillClose', callback);
    }

    public onSubWindowClose(callback: Callback<void>): void {
      this.onSync(this.nativeObj, 'subWindowClose', callback);
    }

    public offSubWindowClose(callback?: Callback<void>): void {
      this.offSync(this.nativeObj, 'subWindowClose', callback);
    }

    public onWindowTitleButtonRectChange(callback: Callback<TitleButtonRect>): void {
      this.onSync(this.nativeObj, 'windowTitleButtonRectChange', callback);
    }

    public offWindowTitleButtonRectChange(callback?: Callback<TitleButtonRect>): void {
      this.offSync(this.nativeObj, 'windowTitleButtonRectChange', callback);
    }

    public onWindowEvent(callback: Callback<WindowEventType>): void {
      this.onSync(this.nativeObj, 'windowEvent', callback);
    }

    public offWindowEvent(callback?: Callback<WindowEventType>): void {
      this.offSync(this.nativeObj, 'windowEvent', callback);
    }
}

export interface Window {
    setFollowParentWindowLayoutEnabled(enabled: boolean): Promise<void>;
    setWindowDelayRaiseOnDrag(isEnabled: boolean): void;
    getParentWindow(): Window;
    getWindowDecorVisible(): boolean;
    stopMoving(): Promise<void>;
    setParentWindow(windowId: number): Promise<void>;
    setWindowTitle(titleName: string): Promise<void>;
    getDecorButtonStyle(): DecorButtonStyle;
    getTitleButtonRect(): TitleButtonRect;
    setTitleButtonVisible(isMaximizeVisible: boolean, isMinimizeVisible: boolean, isSplitVisible: boolean): void;
    setWindowTitleMoveEnabled(enabled: boolean): void;
    setWindowTopmost(isWindowTopmost: boolean): Promise<void>;
    setTitleAndDockHoverShown(isTitleHoverShown?: boolean, isDockHoverShown?: boolean): Promise<void>;
    restore(): Promise<void>;
    hide(): Promise<void>;
    hide(callback: AsyncCallback<void>): void;

    startMoving(): Promise<void>;
    startMoving(offsetX: int, offsetY: int): Promise<void>;
    setWindowTitleButtonVisible(isMaximizeButtonVisible: boolean, isMinimizeButtonVisible: boolean,
        isCloseButtonVisible?: boolean): void;
    setDecorButtonStyle(dectorStyle: DecorButtonStyle): void;
    getWindowStatus(): WindowStatusType;
    minimize(callback: AsyncCallback<void>): void;
    minimize(): Promise<void>;
    maximize(presentation?: MaximizePresentation): Promise<void>;
    resize(width: int, height: int): Promise<void>;
    moveWindowTo(x: int, y: int): Promise<void>;
    resize(width: int, height: int, callback: AsyncCallback<void>): void;
    moveWindowTo(x: int, y: int, callback: AsyncCallback<void>): void;
    getGlobalRect(): Rect;
    setHandwritingFlag(enable: boolean): Promise<void>;
    getWindowDecorHeight(): int;
    setWindowBackgroundColor(color: string | ColorMetrics): void;
    setBackgroundColor(color: string): Promise<void>;
    setBackgroundColor(color: string, callback: AsyncCallback<void>): void;
    setImmersiveModeEnabledState(enable: boolean): void;
    setWindowDecorVisible(isVisible: boolean): void;
    setWindowDecorHeight(height: int): void;
    getWindowProperties(): WindowProperties;
    getProperties(): Promise<WindowProperties>;
    getProperties(callback: AsyncCallback<WindowProperties>): void;
    isWindowSupportWideGamut(): Promise<boolean>;
    isWindowSupportWideGamut(callback: AsyncCallback<boolean>): void;
    setWindowLayoutFullScreen(isLayoutFullScreen: boolean): Promise<void>;
    setWindowLayoutFullScreen(isLayoutFullScreen: boolean, callback: AsyncCallback<void>): void;
    setExclusivelyHighlighted(exclusivelyHighlighted: boolean): Promise<void>;
    setRaiseByClickEnabled(enable: boolean): Promise<void>;
    setRaiseByClickEnabled(enable: boolean, callback: AsyncCallback<void>): void;
    setWindowSystemBarProperties(systemBarProperties: SystemBarProperties): Promise<void>;
    setWindowSystemBarProperties(systemBarProperties: SystemBarProperties, callback: AsyncCallback<void>): void;
    setSpecificSystemBarEnabled(name: String, enable: boolean, enableAnimation?: boolean): Promise<void>;
    snapshot(): Promise<image.PixelMap>;
    snapshot(callback: AsyncCallback<image.PixelMap | undefined>): void;
    hideNonSystemFloatingWindows(shouldHide: boolean): Promise<void>;
    hideNonSystemFloatingWindows(shouldHide: boolean, callback: AsyncCallback<void>): void;
    setWindowColorSpace(colorSpace: ColorSpace): Promise<void>;
    setWindowColorSpace(colorSpace: ColorSpace, callback: AsyncCallback<void>): void;
    setPreferredOrientation(orientation: Orientation): Promise<void>;
    setPreferredOrientation(orientation: Orientation, callback: AsyncCallback<void>): void;
    getPreferredOrientation(): Orientation;
    setWindowPrivacyMode(isPrivacyMode: boolean): Promise<void>;
    setWindowPrivacyMode(isPrivacyMode: boolean, callback: AsyncCallback<void>): void;
    recover(): Promise<void>;
    setUIContent(path: string): Promise<void>;
    setUIContent(path: string, callback: AsyncCallback<void>): void;
    loadContent(path: string, storage: LocalStorage): Promise<void>;
    loadContent(path: string, storage: LocalStorage, callback: AsyncCallback<void>): void;
    loadContent(path: string): Promise<void>;
    loadContent(path: string, callback: AsyncCallback<void>): void;
    onAvoidAreaChange(callback: Callback<AvoidAreaOptions>): void;
    offAvoidAreaChange(callback?: Callback<AvoidAreaOptions>): void;
    onDisplayIdChange(callback: Callback<long>): void;
    offDisplayIdChange(callback?: Callback<long>): void;
    onWindowVisibilityChange(callback: Callback<boolean>): void;
    offWindowVisibilityChange(callback?: Callback<boolean>): void;
    onSystemDensityChange(callback: Callback<double>): void;
    offSystemDensityChange(callback?: Callback<double>): void;
    onScreenshot(callback: Callback<void>): void;
    offScreenshot(callback?: Callback<void>): void;
    setWindowKeepScreenOn(isKeepScreenOn: boolean): Promise<void>;
    setWindowKeepScreenOn(isKeepScreenOn: boolean, callback: AsyncCallback<void>): void;
    setWindowSystemBarEnable(names: Array<'status' | 'navigation'>): Promise<void>;
    setWindowSystemBarEnable(names: Array<'status' | 'navigation'>, callback: AsyncCallback<void>): void;
    getUIContext(): UIContext;
    getWindowAvoidArea(type: AvoidAreaType): AvoidArea;
    setWaterMarkFlag(enable: boolean): Promise<void>;
    setWaterMarkFlag(enable: boolean, callback: AsyncCallback<void>): void;
    showWindow(callback: AsyncCallback<void>): void;
    showWindow(): Promise<void>;
    destroyWindow(callback: AsyncCallback<void>): void;
    destroyWindow(): Promise<void>;
    isWindowShowing(): boolean;
    isWindowHighlighted(): boolean;
    hideWithAnimation(callback: AsyncCallback<void>): void;
    hideWithAnimation(): Promise<void>;
    showWithAnimation(callback: AsyncCallback<void>): void;
    showWithAnimation(): Promise<void>;
    setWindowFocusable(isFocusable: boolean): Promise<void>;
    setWindowFocusable(isFocusable: boolean, callback: AsyncCallback<void>): void;
    getSubWindowZLevel(): int;
    isFocused(): boolean;
    setSubWindowZLevel(zLevel: int): Promise<void>;
    raiseAboveTarget(windowId: int): Promise<void>;
    raiseAboveTarget(windowId: int, callback: AsyncCallback<void>): void;
    raiseToAppTop(callback: AsyncCallback<void>): void;
    raiseToAppTop(): Promise<void>;
    setTopmost(isTopmost: boolean): Promise<void>;
    requestFocus(isFocused: boolean): Promise<void>;
    setSubWindowModal(isModal: boolean): Promise<void>;
    setSubWindowModal(isModal: boolean, modalityType: ModalityType): Promise<void>;
    keepKeyboardOnFocus(enable: boolean): void;
    setWindowTouchable(isTouchable: boolean): Promise<void>
    setWindowTouchable(isTouchable: boolean, callback: AsyncCallback<void>): void
    setDialogBackGestureEnabled(enabled: boolean): Promise<void>;
    setWindowMask(windowMask: Array<Array<long>>): Promise<void>;
    setTouchableAreas(rects: Array<Rect>): void;
    opacity(opacity: double): void;
    scale(scaleOptions: ScaleOptions): void;
    translate(translateOptions: TranslateOptions): void;
    rotate(rotateOptions: RotateOptions): void;
    setShadow(radius: double, color?: string, offsetX?: double, offsetY?: double): void;
    setCornerRadius(cornerRadius: double): void;
    setBlur(radius: double): void;
    setBackdropBlurStyle(blurStyle: BlurStyle): void;
    setBackdropBlur(radius: double): void;
    getWindowCornerRadius(): double;
    setWindowCornerRadius(cornerRadius: double): Promise<void>;
    setWindowShadowRadius(radius: double): void;
    setWindowBrightness(brightness: double): Promise<void>;
    setWindowBrightness(brightness: double, callback: AsyncCallback<void>): void;
    getWindowColorSpace(): ColorSpace;
    setWakeUpScreen(wakeUp: boolean): void;
    setSnapshotSkip(isSkip: boolean): void;
    snapshotIgnorePrivacy(): Promise<image.PixelMap>;
    setWindowContainerColor(activeColor: string, inactiveColor: string): void;
    getStatusBarProperty(): StatusBarProperty;
    setStatusBarColor(color: ColorMetrics): Promise<void>;
    isSystemAvoidAreaEnabled(): boolean;
    setSystemAvoidAreaEnabled(enabled: boolean): Promise<void>;
    getWindowDensityInfo(): WindowDensityInfo;
    getImmersiveModeEnabledState(): boolean;
    setWindowGrayScale(grayScale: double): Promise<void>;
    getWindowSystemBarProperties(): SystemBarProperties;
    isGestureBackEnabled(): boolean;
    setGestureBackEnabled(enabled: boolean): Promise<void>;
    setSingleFrameComposerEnabled(enabled: boolean): Promise<void>;
    getTransitionController(): TransitionController;
    onWindowHighlightChange(callback: Callback<boolean>): void;
    offWindowHighlightChange(callback?: Callback<boolean>): void;
    createSubWindowWithOptions(name: string, options: SubWindowOptions): Promise<Window>;
    on<T>(type: string, callback: Callback<T>): void;
    on<T>(type: 'noInteractionDetected', timeout: long, callback: Callback<T>): void;
    onTouchOutside(callback: Callback<void>): void;
    onDialogTargetTouch(callback: Callback<void>): void;
    onNoInteractionDetected(timeout: long, callback: Callback<void>): void;
    onKeyboardHeightChange(callback: Callback<int>): void;
    onKeyboardDidShow(callback: Callback<KeyboardInfo>): void;
    onKeyboardDidHide(callback: Callback<KeyboardInfo>): void;
    onRotationChange(callback: RotationChangeCallback<RotationChangeInfo, RotationChangeResult |
      undefined>): void;
    off<T>(type: string, callback?: Callback<T>): void;
    offKeyboardHeightChange(callback?: Callback<int>): void;
    offKeyboardDidShow(callback?: Callback<KeyboardInfo>): void;
    offKeyboardDidHide(callback?: Callback<KeyboardInfo>): void;
    offRotationChange(callback?: RotationChangeCallback<RotationChangeInfo, RotationChangeResult |
      undefined>): void;
    offTouchOutside(callback?: Callback<void>): void;
    offDialogTargetTouch(callback?: Callback<void>): void;
    offNoInteractionDetected(callback?: Callback<void>): void;
    onWindowSizeChange(callback: Callback<Size>): void;
    offWindowSizeChange(callback?: Callback<Size>): void;
    onWindowStatusChange(callback: Callback<WindowStatusType>): void;
    offWindowStatusChange(callback?: Callback<WindowStatusType>): void;
    onWindowRectChange(callback: Callback<RectChangeOptions>): void;
    offWindowRectChange(callback?: Callback<RectChangeOptions>): void;
    onWindowWillClose(callback: Callback<Promise<boolean>>): void;
    offWindowWillClose(callback?: Callback<Promise<boolean>>): void;
    onSubWindowClose(callback: Callback<void>): void;
    offSubWindowClose(callback?: Callback<void>): void;
    onWindowTitleButtonRectChange(callback: Callback<TitleButtonRect>): void;
    offWindowTitleButtonRectChange(callback?: Callback<TitleButtonRect>): void;
    onWindowEvent(callback: Callback<WindowEventType>): void;
    offWindowEvent(callback?: Callback<WindowEventType>): void;
}

export native function CreateWindowStage(scene: long): WindowStageInternal;
export function CreateWindowStageApi(scene: long): WindowStage { return CreateWindowStage(scene); }
export native function CreateWindow(window: long): WindowInternal;

let nativeObj: long;
export function setNativeObj(nativeObject: long): void {
    hilog.info(DOMAIN, TAG, 'setNativeObj, nativeObject:' + nativeObject);
    nativeObj = nativeObject;
}

native function getWindowsByCoordinate(nativeObj: long, param: window.GetWindowsByCoordinateParam): Array<Window>;
native function getLastWindowSync(nativeObj: long, ctx: BaseContext): Window;
native function minimizeAllSync(nativeObj: long, id: long): void;
native function findWindowSync(nativeObj: long, name: string): Window;
native function onSync(nativeObj: long, type: string, callback: object): void;
native function offSync(nativeObj: long, type: string, callback?: object): void;
native function shiftAppWindowFocusSync(nativeObj: long, sourceWindowId: int, targetWindowId: int): void;
native function shiftAppWindowPointerEventSync(nativeObj: long, sourceWindowId: int, targetWindowId: int): void;
native function createWindowSync(nativeObj: long, config: Configuration): Window;
native function getAllWindowLayoutInfo(nativeObj: long, displayId: long): Array<WindowLayoutInfo>;
native function getSnapshot(nativeObj: long, windowId: int): image.PixelMap;
native function getVisibleWindowInfo(nativeObj: long): Array<WindowInfoInternal>;
native function setGestureNavigationEnabled(nativeObj: long, enabled: boolean): void;
native function setWaterMarkImage(nativeObj: long, pixelMap: image.PixelMap, enabled: boolean): void;
native function toggleShownStateForAllAppWindowsSync(nativeObj: long): void;

export function getLastWindow(ctx: BaseContext): Promise<Window> {
  return new Promise<Window>((resolve: (value: Window) => void, reject: (error: BusinessError) => void) => {
      taskpool.execute((): Window => {
          hilog.info(DOMAIN, TAG, 'getLastWindow, ctx:' + ctx);
          return getLastWindowSync(nativeObj, ctx);
      }).then((ret: Any) => {
          resolve(ret as Window);
      }).catch((err: Any) => {
          reject(err as BusinessError);
      });
  }); 
}

export function getLastWindow(ctx: BaseContext, callback: AsyncCallback<Window>): void {
    taskpool.execute((): Window => {
        hilog.info(DOMAIN, TAG, 'getLastWindow, ctx:' + ctx);
        return getLastWindowSync(nativeObj, ctx);
    }).then((ret: Any) => {
        callback(new BusinessError(), ret as Window);
    }).catch((err: Any) => {
        callback(err as BusinessError, new WindowInternal());
    });
}

export function createWindow(config: Configuration): Promise<Window> {
  return new Promise<Window>((resolve: (value: Window) => void, reject: (error: BusinessError) => void ) => {
      taskpool.execute((): Window => {
          return createWindowSync(nativeObj, config);
      }).then((ret: Any) => {
          resolve(ret as Window);
      }).catch((err: Any) => {
          reject(err as BusinessError);
      });
  });
}

export function createWindow(config: Configuration, callback: AsyncCallback<Window>): void {
    taskpool.execute((): Window => {
        return createWindowSync(nativeObj, config);
    }).then((ret: Any) => {
        callback(new BusinessError(), ret as Window);
    }).catch((err: Any) => {
        callback(err as BusinessError, new WindowInternal());
    });
}

export function minimizeAll(id: long): Promise<void> {
  return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
      taskpool.execute((): void => {
          minimizeAllSync(nativeObj, id);
      }).then((ret: Any) => {
          resolve(undefined);
      }).catch((err: Any) => {
          reject(err as BusinessError);
      });
  }); 
}

export function minimizeAll(id: long, callback: AsyncCallback<void>): void {
    taskpool.execute((): void => {
        minimizeAllSync(nativeObj, id);
    }).then((ret: Any) => {
        callback(new BusinessError(), undefined);
    }).catch((err: Any) => {
        callback(err as BusinessError, undefined);
    });
}

export function getWindowsByCoordinate(displayId: long, windowNumber?: int,
                                       x?: int, y?: int): Promise<Array<Window>> {
  return new Promise<Array<Window>>((resolve: (value: Array<Window>) => void,
                                     reject: (error: BusinessError) => void): void => {
    taskpool.execute((): Array<Window> => {
      let param: window.GetWindowsByCoordinateParam = {
        displayId: displayId,
        windowNumber: 0,
        x: -1,
        y: -1
      } as GetWindowsByCoordinateParamInternal;
      if (windowNumber != undefined) {
        param.windowNumber = windowNumber;
      }
      if (x != undefined) {
        param.x = x;
      }
      if (y != undefined) {
        param.y = y;
      }
      return getWindowsByCoordinate(nativeObj, param);
    }).then((ret: Any) => {
        resolve(ret as Array<Window>);
    }).catch((err: Any) => {
        reject(err as BusinessError);
    });
  });
}

export function getAllWindowLayoutInfo(displayId: long): Promise<Array<WindowLayoutInfo>> {
  return new Promise<Array<WindowLayoutInfo>>((
    resolve: (value: Array<WindowLayoutInfo>) => void, reject: (error: BusinessError) => void): void => {
      taskpool.execute(() => {
          return getAllWindowLayoutInfo(nativeObj, displayId);
      }).then((ret: Any) => {
          resolve(ret as Array<WindowLayoutInfo>);
      }).catch((err: Any) => {
          reject(err as BusinessError);
      });
  });
}

export function getSnapshot(windowId: int): Promise<image.PixelMap> {
  return new Promise<image.PixelMap>((
    resolve: (value: image.PixelMap) => void, reject: (error: BusinessError) => void): void => {
      taskpool.execute(() => {
          return getSnapshot(nativeObj, windowId);
      }).then((ret: Any) => {
          resolve(ret as image.PixelMap);
      }).catch((err: Any) => {
          reject(err as BusinessError);
      });
  });
}

export function getVisibleWindowInfo(): Promise<Array<WindowInfo>> {
  return new Promise<Array<WindowInfo>>((
    resolve: (value: Array<WindowInfo>) => void, reject: (error: BusinessError) => void): void => {
      taskpool.execute(() => {
          let visibleWindowInfos = getVisibleWindowInfo(nativeObj);
          for (let i = 0; i < visibleWindowInfos.length; i++) {
            visibleWindowInfos[i].windowStatusType =
              visibleWindowInfos[i].windowStatusTypeInternal as WindowStatusType;
            visibleWindowInfos[i].isFocused = visibleWindowInfos[i].isFocusedInternal;
          }
          return visibleWindowInfos;
      }).then((ret: Any) => {
          resolve(ret as Array<WindowInfo>);
      }).catch((err: Any) => {
          reject(err as BusinessError);
      });
  });
}

export function setGestureNavigationEnabled(enabled: boolean): Promise<void> {
  return new Promise<void>((
    resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
      taskpool.execute((): void => {
          return setGestureNavigationEnabled(nativeObj, enabled);
      }).then((ret: Any) => {
          resolve(undefined);
      }).catch((err: Any) => {
          reject(err as BusinessError);
      });
  });
}

export function setGestureNavigationEnabled(enabled: boolean, callback: AsyncCallback<void>): void {
    taskpool.execute((): void => {
        setGestureNavigationEnabled(nativeObj, enabled);
    }).then((ret: Any) => {
        callback(new BusinessError(), undefined);
    }).catch((err: Any) => {
        callback(err as BusinessError, undefined);
    });
}

export function setWaterMarkImage(pixelMap: image.PixelMap, enabled: boolean): Promise<void> {
  return new Promise<void>((
    resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
      taskpool.execute((): void => {
          return setWaterMarkImage(nativeObj, pixelMap, enabled);
      }).then((ret: Any) => {
          resolve(undefined);
      }).catch((err: Any) => {
          reject(err as BusinessError);
      });
  });
}

export function setWaterMarkImage(pixelMap: image.PixelMap, enabled: boolean, callback: AsyncCallback<void>): void {
    taskpool.execute((): void => {
        setWaterMarkImage(nativeObj, pixelMap, enabled);
    }).then((ret: Any) => {
        callback(new BusinessError(), undefined);
    }).catch((err: Any) => {
        callback(err as BusinessError, undefined);
    });
}

export function findWindow(name: string): Window {
  return findWindowSync(nativeObj, name);
}

export function on<T>(type: string, callback: Callback<T>): void {
  onSync(nativeObj, type, callback);
}

export function off<T>(type: string, callback?: Callback<T>): void {
  offSync(nativeObj, type, callback);
}

export function onSystemBarTintChange(callback: Callback<SystemBarTintState>): void {
  onSync(nativeObj, 'systemBarTintChange', callback);
}

export function offSystemBarTintChange(callback?: Callback<SystemBarTintState>): void {
  offSync(nativeObj, 'systemBarTintChange', callback);
}

export function onGestureNavigationEnabledChange(callback: Callback<boolean>): void {
  onSync(nativeObj, 'gestureNavigationEnabledChange', callback);
}

export function offGestureNavigationEnabledChange(callback?: Callback<boolean>): void {
  offSync(nativeObj, 'gestureNavigationEnabledChange', callback);
}

export function onWaterMarkFlagChange(callback: Callback<boolean>): void {
  onSync(nativeObj, 'waterMarkFlagChange', callback);
}

export function offWaterMarkFlagChange(callback?: Callback<boolean>): void {
  offSync(nativeObj, 'waterMarkFlagChange', callback);
}

function runWindowListenerVoidArgCallback(cb: object): void {
  const func = cb as () => void;
  func();
}

function runWindowListenerBooleanArgCallback(cb: object, cbArg: boolean): void {
  const func = cb as (cbArg: boolean) => void;
  func(cbArg);
}

function runSystemDensityChangeCallback(cb: object, density: double): void {
  const func = cb as (density: double) => void;
  func(density as double);
}

function runDisplayIdChangeCallback(cb: object, displayId: long): void {
  const func = cb as (displayId: long) => void;
  func(displayId as long);
}

function runSystemBarTintChangeCallback(cb: object, tint: object): void {
  const func = cb as (tint: SystemBarTintState) => void;
  func(tint as SystemBarTintState);
}

function runAvoidAreaChangeCallback(cb: object, area: object, type: int): void {
  const func = cb as (data: AvoidAreaOptions) => void;
  func({type: type as AvoidAreaType, area: area as AvoidArea});
}

export function shiftAppWindowFocus(sourceWindowId: int, targetWindowId: int): Promise<void> {
  return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
      taskpool.execute((): void => {
          return shiftAppWindowFocusSync(nativeObj, sourceWindowId, targetWindowId);
      }).then((ret: Any) => {
          resolve(undefined);
      }).catch((err: Any) => {
          reject(err as BusinessError);
      });
  });
}

export function toggleShownStateForAllAppWindows(callback: AsyncCallback<void>): void {
      taskpool.execute((): void => {
          toggleShownStateForAllAppWindowsSync(nativeObj);
      }).then((ret: Any) => {
          callback(new BusinessError(), undefined);
      }).catch((err: Any) => {
          callback(err as BusinessError, undefined);
      });
}

export function toggleShownStateForAllAppWindows(): Promise<void> {
  return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
    taskpool.execute((): void => {
        return toggleShownStateForAllAppWindowsSync(nativeObj);
    }).then((ret: Any) => {
        resolve(undefined);
    }).catch((err: Any) => {
        reject(err as BusinessError);
    });
  });
}

export function shiftAppWindowPointerEvent(sourceWindowId: int, targetWindowId: int): Promise<void> {
  return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void): void => {
    taskpool.execute((): void => {
      return shiftAppWindowPointerEventSync(nativeObj, sourceWindowId, targetWindowId);
    }).then((ret: Any) => {
      resolve(undefined);
    }).catch((err: Any) => {
      reject(err as BusinessError);
    });
  });
}

function runWindowDialogTargetCallback(cb: object): void {
    const func = cb as () => void;
    func();
}

function runWindowEventCallback(cb: object, cbArg: int): void {
    const func = cb as (cbArg: WindowEventType) => void;
    func(cbArg as WindowEventType);
}

function runWindowStageEventCallback(cb: object, cbArg: int): void {
    const func = cb as (cbArg: WindowStageEventType) => void;
    func(cbArg as WindowStageEventType);
}

function runWindowTouchOutCallback(cb: object): void {
    const func = cb as () => void;
    func();
}

function runWindowSizeCallback(cb: object, cbArg: object): void {
    const func = cb as (cbArg: Size) => void;
    func(cbArg as Size);
}

function runWindowRectChangeCallback(cb: object, rect: object, reason: int): void {
    const func = cb as (data: RectChangeOptions) => void;
    func({rect: rect as Rect, reason: reason as RectChangeReason});
}

function runKeyboardHeightChangeCallback(cb: object, cbArg: int): void {
    const func = cb as (cbArg: int) => void;
    func(cbArg as int);
}

function runKeyboardDidShowCallback(cb: object, cbArg: object): void {
    const func = cb as (cbArg: KeyboardInfo) => void;
    func(cbArg as KeyboardInfo);
}

function runKeyboardDidHideCallback(cb: object, cbArg: object): void {
    const func = cb as (cbArg: KeyboardInfo) => void;
    func(cbArg as KeyboardInfo);
}

function runWindowStatusCallback(cb: object, cbArg: int): void {
    const func = cb as (cbArg: WindowStatusType) => void;
    func(cbArg as WindowStatusType);
}

function runWindowNoInteractionCallback(cb: object): void {
    const func = cb as () => void;
    func();
}
}

export default window;