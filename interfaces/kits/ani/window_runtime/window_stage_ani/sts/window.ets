/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
export class BusinessError extends Error {
  code: number;
}
export type AsyncCallback = (err: BusinessError) => void;
export abstract class BaseContext {
  stageMode: boolean;
}

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

export enum window_ColorSpace {
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
    statusBarColor?: string;
    isStatusBarLightIcon?: boolean;
    statusBarContentColor?: string;
    navigationBarColor?: string;
    isNavigationBarLightIcon?: boolean;
    navigationBarContentColor?: string;
    enableStatusBarAnimation?: boolean;
    enableNavigationBarAnimation?: boolean;
}

export interface WindowProperties {
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
    displayId?: number;
}

export interface MoveConfiguration {
    displayId?: number;
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

export interface WindowLimits {
    maxWidth?: number;
    maxHeight?: number;
    minWidth?: number;
    minHeight?: number;
}

export class WindowStageInternal implements WindowStage {
    static { loadLibrary('windowstageani_kit.z') }
    setNativeObj(nativeObj: long):void {
        this.nativeObj = nativeObj;
        console.println(nativeObj);
    }

    private nativeObj : long;

    public native loadContent(nativeObj:long, path: String): int;
    public native disableWindowDecor(nativeObj:long): int;
    public native setShowOnLockScreen(nativeObj:long, showOnLockScreen: boolean): int;

    public loadContent(path: string, callback: AsyncCallback): void {
        let ret = this.loadContent(this.nativeObj, String(path));
        console.println(ret)
    }
    public loadContentByName(name: string, callback: AsyncCallback): void {
        let ret = this.loadContent(this.nativeObj, name);
    }
    public disableWindowDecor(): void {
        let ret = this.disableWindowDecor(this.nativeObj);
    }
    public setShowOnLockScreen(showOnLockScreen: boolean): void {
        let ret = this.setShowOnLockScreen(this.nativeObj, showOnLockScreen);
    }
}

export interface WindowStage {
//    loadContent(path: string, storage: LocalStorage, callback: AsyncCallback): void;
//    loadContent(path: string, storage?: LocalStorage): Promise<void>;
    loadContent(path: string, callback: AsyncCallback): void;
//    loadContentByName(name: string, storage: LocalStorage, callback: AsyncCallback): void;
//    loadContentByName(name: string, storage?: LocalStorage): Promise<void>;
    loadContentByName(name: string, callback: AsyncCallback): void;
//    disableWindowDecor(): void;
//    setShowOnLockScreen(showOnLockScreen: boolean): void;
}

export class WindowInternal implements Window {
    static { loadLibrary('windowstageani_kit.z') }
    private nativeObj: long;

    public setNativeObj(nativeObj: long): void {
        this.nativeObj = nativeObj;
        console.println(nativeObj);
    }

    private native getWindowDecorHeight(nativeObj:long): int;
    public native setWindowBackgroundColorSync(nativeObj: long, color: string): int;
    private native getWindowAvoidAreaInternal(nativeObj: long, type: AvoidAreaType): AvoidArea;
    private native getLastWindowInternal(nativeObj: long, ctx: BaseContext): Window;
    private native setPreferredOrientationInternal(nativeObj: long, orientation: Orientation): void;
    private native setWindowPrivacyModeInternal(isPrivacyMode: boolean): void;

    public getWindowDecorHeight(): int {
       let ret = this.getWindowDecorHeight(this.nativeObj);
       console.println(ret);
       return ret;
    }

    public getWindowAvoidArea(type: AvoidAreaType): AvoidArea {
        return this.getWindowAvoidAreaInternal(this.nativeObj, type);
    }

    public getLastWindow(ctx: BaseContext): Promise<Window> {
        return new Promise<Window>((resolve: (window: Window) => void, reject: (error: BusinessError) => void ) => {
            // taskpool.excute(this.getLastWindowInternal).then((result: Result<Window, BusinessError>) => {
            //     if (result.error.code != 0) {
            //         reject(result.error);
            //     } else {
            //         resolve(result.result);
            //     }
            // });
        });
    }

    public getLastWindow(ctx: BaseContext, callback: AsyncCallback): void {
        // taskpool.excute(this.getLastWindowInternal).then((result: Result<Window, BusinessError>) => {
        //     callback(result.error, result.result);
        // });
    }

    // public setPreferredOrientation(orientation: Orientation): Promise<void> {
    //     return new Promise<void>((resolve: () => void, reject: (error: BusinessError) => void ) => {
    //         taskpool.excute(this.setPreferredOrientationInternal).then((result: Result<void, BusinessError>) => {
    //             if (result.error.code != 0) {
    //                 reject(result.error);
    //             } else {
    //                 resolve(result.result);
    //             }
    //         });
    //     });
    // }

    public setPreferredOrientation(orientation: Orientation, callback: AsyncCallback): void {
        // taskpool.excute(this.setPreferredOrientationInternal).then((result: Result<void, BusinessError>) => {
        //     callback(result.error, result.result);
        // });
    }

    // public setWindowPrivacyMode(isPrivacyMode: boolean): Promise<void> {
    // }

    public setWindowPrivacyMode(isPrivacyMode: boolean, callback: AsyncCallback): void {
        // taskpool.excute(this.setWindowPrivacyModeInternal).then((result: Result<void, BusinessError>) => {
        //     callback(result.error, result.result);
        // });
    }

    public setWindowBackgroundColorSync(color: string): int {
       let ret = this.setWindowBackgroundColorSync(this.nativeObj, color);
       console.println(ret);
       return ret;
    }
}

export interface Window {
    getWindowDecorHeight(): int;
    setWindowBackgroundColorSync(color: string): int;
    getWindowAvoidArea(type: AvoidAreaType): AvoidArea;
    getLastWindow(ctx: BaseContext): Promise<Window>;
    getLastWindow(ctx: BaseContext, callback: AsyncCallback): void;
    // setPreferredOrientation(orientation: Orientation): Promise<void>;
    setPreferredOrientation(orientation: Orientation, callback: AsyncCallback): void;
    // setWindowPrivacyMode(isPrivacyMode: boolean): Promise<void>;
    setWindowPrivacyMode(isPrivacyMode: boolean, callback: AsyncCallback): void;
}

export native function CreateWindowStage(scene: long): WindowStageInternal;
// export native function CreateSystemBarProperties(scene: long): SystemBarPropertiesInternal;
export function CreateWindowStageApi(scene: long): WindowStage { return CreateWindowStage(scene); }
export native function CreateWindow(window: long): WindowInternal;
}

export default window;