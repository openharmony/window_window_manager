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

import { AsyncCallback, Callback } from '@ohos.base';
import { BusinessError } from '@ohos.base';

export default namespace screen {

loadLibrary('screenani_kit.z');

export function on<T>(eventType: string, callback: Callback<T>): void{
    syncOn(eventType, callback, screenMgrRef);
}
native function syncOn(eventType: string, callback: object, nativeObj: long): void;

export function off<T>(eventType: string, callback?: Callback<T>): void{
    syncOff(eventType, screenMgrRef, callback);
}
native function syncOff(eventType: string, nativeObj: long, callback?: object): void;

export function makeMirror(mainScreen: number, mirrorScreen: Array<number>, callback: AsyncCallback<number>): void {
    taskpool.execute((): number => {
        let res: number = makeMirrorInternal(mainScreen, mirrorScreen);
        return res;
    }).then((ret: NullishType) => {
        callback(new BusinessError(), ret as number);
    }).catch((err: NullishType) => {
        callback(err as BusinessError, -1);
    });
}

export function makeMirror(mainScreen: number, mirrorScreen: Array<number>): Promise<number> {
    return new Promise<number>((resolve: (value: number) => void, reject: (error: BusinessError) => void ) => {
        taskpool.execute((): number => {
            let res: number = makeMirrorInternal(mainScreen, mirrorScreen);
            return res;
        }).then((ret: NullishType) => {
            resolve(ret as number);
        }).catch((err: NullishType) => {
            reject(err as BusinessError);
        });
    })
}

export native function makeMirrorInternal(mainScreen: number, mirrorScreen: Array<number>): number;

export function getAllScreens(callback: AsyncCallback<Array<Screen>>): void {
    taskpool.execute((): Array<Screen> => {
        let screens: Array<Screen> = new Array<Screen>(10);
        for (let i = 0; i < 10; i++) {
            screens[i] = new ScreenImpl();
        }
        getAllScreensInternal(screens);
        minusScreens(screens);
        return screens;
    }).then((ret: NullishType) => {
        callback(new BusinessError(), ret as Array<Screen>);
    }).catch((err: NullishType) => {
        callback(err as BusinessError, new Array<Screen>());
    });
}

export function getAllScreens(): Promise<Array<Screen>> {
    return new Promise<Array<Screen>>((resolve: (value: Array<Screen>) => void,
        reject: (error: BusinessError) => void ) => {
            taskpool.execute((): Array<Screen> => {
                let screens: Array<Screen> = new Array<Screen>(10);
                for (let i = 0; i < 10; i++) {
                    screens[i] = new ScreenImpl();
                }
                getAllScreensInternal(screens);
                minusScreens(screens);
                return screens;
            }).then((ret: NullishType) => {
                resolve(ret as Array<Screen>);
            }).catch((err: NullishType) => {
                reject(err as BusinessError);
            });
        }
    );
}

function minusScreens(screens: Array<Screen>) {
    let i = 0;
    for (; i < screens.length; i++) {
        let currentScreen: ScreenImpl = screens[i] as ScreenImpl;
        if (currentScreen.id < 0) {
            break;
        }
        minusScreenModeInfo(currentScreen.supportedModeInfo_);
        let current: Screen = new ScreenImpl(screens[i] as ScreenImpl);
        screens[i] = current;
    }
    screens.splice(i);
}

function minusScreenModeInfo(screenModeInfo: Array<ScreenModeInfo>) {
    let i = 0;
    for (; i < screenModeInfo.length; i++) {
        let currentScreenMode: ScreenModeInfoImpl = screenModeInfo[i] as ScreenModeInfoImpl;
        if (currentScreenMode.width <= 0) {
            break;
        }
    }
    screenModeInfo.splice(i);
}

export native function getAllScreensInternal(screens: Array<Screen>): void;

export interface Screen {

    readonly id: number;

    readonly parent: number;

    readonly supportedModeInfo: Array<ScreenModeInfo>;

    readonly activeModeIndex: number;

    readonly orientation: Orientation;

    readonly sourceMode: ScreenSourceMode;

    setDensityDpi(densityDpi: number, callback: AsyncCallback<void>): void;

    setDensityDpi(densityDpi: number): Promise<void>;
}

class ScreenImpl implements Screen {

    static { loadLibrary("screenani_kit.z"); }

    constructor() {
        this.id = -1;
        this.supportedModeInfo = new Array<ScreenModeInfo>(10);
        for (let i = 0; i < this.supportedModeInfo.length;i++) {
            this.supportedModeInfo[i] = new ScreenModeInfoImpl();
        }
    }

    constructor(screen: ScreenImpl) {
        this.id = screen.id;
        this.parent = screen.parent;
        this.activeModeIndex = screen.activeModeIndex;
        this.orientation = screen.orientation_ as Orientation;
        this.supportedModeInfo = screen.supportedModeInfo_;
        this.sourceMode = screen.sourceMode_ as ScreenSourceMode;
        this.screenNativeObj = screen.screenNativeObj;
    }

    readonly id: number;

    readonly parent: number;

    readonly supportedModeInfo: Array<ScreenModeInfo>;
    supportedModeInfo_: Array<ScreenModeInfo>;

    readonly activeModeIndex: number;

    readonly orientation: Orientation;
    orientation_: int;

    readonly sourceMode: ScreenSourceMode;
    sourceMode_: int;
    sourceModeLen: int;

    screenNativeObj: long;

    setDensityDpi(densityDpi: number, callback: AsyncCallback<void>): void {
        taskpool.execute((): void => {
            this.setDensityDpiInternal(densityDpi);
        }).then(() => {
            callback(new BusinessError(), undefined);
        }).catch((err: NullishType) => {
            callback(err as BusinessError, undefined);
        });
    }

    setDensityDpi(densityDpi: number): Promise<void> {
        return new Promise<void>((resolve: (value: undefined) => void, reject: (error: BusinessError) => void ): void => {
            taskpool.execute((): void => {
                this.setDensityDpiInternal(densityDpi);
            }).then(() => {
                resolve(undefined);
            }).catch((err: NullishType) => {
                reject(err as BusinessError);
            });
        });
    }

    native setDensityDpiInternal(densityDpi: number): void;
}

export interface ScreenModeInfo {
    id: number;

    width: number;

    height: number;

    refreshRate: number;
}

class ScreenModeInfoImpl implements ScreenModeInfo{
    id: number;

    width: number;

    height: number;

    refreshRate: number;
}

export enum ScreenSourceMode {
    SCREEN_MAIN = 0,

    SCREEN_MIRROR = 1,

    SCREEN_EXTEND = 2,

    SCREEN_ALONE = 3
}

export enum Orientation {
    UNSPECIFIED = 0,

    VERTICAL = 1,

    HORIZONTAL = 2,

    REVERSE_VERTICAL = 3,

    REVERSE_HORIZONTAL = 4
}

export let screenMgrRef: long;
export function setScreenMgrRef(nativeObj: long): void{
    screenMgrRef = nativeObj;
}

export function screenEventCallBack(cb: object, cbArg: double): void {
    const func = cb as (cbArg: double) => void;
    func(cbArg as double);
}
}
