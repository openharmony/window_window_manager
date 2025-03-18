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
import type { AsyncCallback, Callback} from '@ohos.base';
import { BusinessError } from '@ohos.base';

export default namespace display {

loadLibrary('displayani_kit.z');

// export class BusinessError extends Error {
  // code: number;
  // message: string;
// }

export interface Rect {
    left: number;

    top: number;

    width: number;

    height: number;
}

export class RectImpl implements Rect {
    left: number;

    top: number;

    width: number;

    height: number;
}

interface WaterfallDisplayAreaRects {
    readonly left: Rect;

    readonly right: Rect;

    readonly top: Rect;

    readonly bottom: Rect;
}

export class WaterfallDisplayAreaRectsImpl implements WaterfallDisplayAreaRects {
    constructor() {
        this.left = new RectImpl()
        this.right = new RectImpl();
        this.top = new RectImpl()
        this.bottom = new RectImpl();
    }
    readonly left: Rect;

    readonly right: Rect;

    readonly top: Rect;

    readonly bottom: Rect;
}

export interface CutoutInfo {
    readonly boundingRects: Array<Rect>;

    readonly waterfallDisplayAreaRects: WaterfallDisplayAreaRects;
}

export class CutoutInfoImpl implements CutoutInfo {
    constructor() {
        this.boundingRects = new Array<Rect>(5);
        for (let i = 0; i < this.boundingRects.length; i++) {
            this.boundingRects[i] = new RectImpl();
        }
        this.waterfallDisplayAreaRects = new WaterfallDisplayAreaRectsImpl();
    }

    readonly boundingRects: Array<Rect>;

    readonly waterfallDisplayAreaRects: WaterfallDisplayAreaRects;
}

interface FoldCreaseRegion {
    readonly displayId: number;

    readonly creaseRects: Array<Rect>;
}

export class FoldCreaseRegionImpl implements FoldCreaseRegion {
    constructor() {
        this.creaseRects = new Array<Rect>();
    }

    readonly displayId: number;

    readonly creaseRects: Array<Rect>;
}

export enum FoldDisplayMode {

    FOLD_DISPLAY_MODE_UNKNOWN = 0,

    FOLD_DISPLAY_MODE_FULL,

    FOLD_DISPLAY_MODE_MAIN,

    FOLD_DISPLAY_MODE_SUB,

    FOLD_DISPLAY_MODE_COORDINATION
}

export enum Orientation {

    PORTRAIT = 0,

    LANDSCAPE = 1,

    PORTRAIT_INVERTED = 2,

    LANDSCAPE_INVERTED = 3
  }

export enum FoldStatus {
    /**
     * Fold Status Unknown.
     *
     * @syscap SystemCapability.Window.SessionManager
     * @since 10
     */
    /**
     * Fold Status Unknown.
     *
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    FOLD_STATUS_UNKNOWN = 0,
    /**
     * Fold Status Expanded.
     *
     * @syscap SystemCapability.Window.SessionManager
     * @since 10
     */
    /**
     * Fold Status Expanded.
     *
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    FOLD_STATUS_EXPANDED,
    /**
     * Fold Status Folded.
     *
     * @syscap SystemCapability.Window.SessionManager
     * @since 10
     */
    /**
     * Fold Status Folded.
     *
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    FOLD_STATUS_FOLDED,
    /**
     * Fold Status Half Folded.
     *
     * @syscap SystemCapability.Window.SessionManager
     * @since 10
     */
    /**
     * Fold Status Half Folded.
     *
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    FOLD_STATUS_HALF_FOLDED
  }

enum DisplayState {

    STATE_UNKNOWN = 0,

    STATE_OFF,

    STATE_ON,

    STATE_DOZE,

    STATE_DOZE_SUSPEND,

    STATE_VR,

    STATE_ON_SUSPEND
}

enum ColorSpace {

    UNKNOWN = 0,

    ADOBE_RGB_1998 = 1,

    DCI_P3 = 2,

    DISPLAY_P3 = 3,

    SRGB = 4,

    BT709 = 6,

    BT601_EBU = 7,

    BT601_SMPTE_C = 8,

    BT2020_HLG = 9,

    BT2020_PQ = 10,

    P3_HLG = 11,

    P3_PQ = 12,

    ADOBE_RGB_1998_LIMIT = 13,

    DISPLAY_P3_LIMIT = 14,

    SRGB_LIMIT = 15,

    BT709_LIMIT = 16,

    BT601_EBU_LIMIT = 17,

    BT601_SMPTE_C_LIMIT = 18,

    BT2020_HLG_LIMIT = 19,

    BT2020_PQ_LIMIT = 20,

    P3_HLG_LIMIT = 21,

    P3_PQ_LIMIT = 22,

    LINEAR_P3 = 23,

    LINEAR_SRGB = 24,

    LINEAR_BT709 = 24,

    LINEAR_BT2020 = 25,

    DISPLAY_SRGB = 4,

    DISPLAY_P3_SRGB = 3,

    DISPLAY_P3_HLG = 11,

    DISPLAY_P3_PQ = 12,

    CUSTOM = 5,
}

enum HDRFormat {
    NONE = 0,

    VIDEO_HLG = 1,

    VIDEO_HDR10 = 2,

    VIDEO_HDR_VIVID = 3,

    IMAGE_HDR_VIVID_DUAL = 4,

    IMAGE_HDR_VIVID_SINGLE = 5,

    IMAGE_HDR_ISO_DUAL = 6,

    IMAGE_HDR_ISO_SINGLE = 7,
}

export interface Display {
    id: double;

    name: string;

    alive: boolean;

    state: DisplayState;

    refreshRate: double;

    rotation: double;

    width: double;
 
    height: double;

    availableHeight: double;

    availableWidth: double;

    densityDPI: double;

    orientation: Orientation;

    densityPixels: double;

    scaledDensity: double;

    xDPI: double;

    yDPI: double;

    colorSpaces: Array<ColorSpace>;

    hdrFormats: Array<HDRFormat>;

    getCutoutInfo(callback: AsyncCallback<CutoutInfo>): void;

    getCutoutInfo(): Promise<CutoutInfo>;
}

// export type Callback<T> = (data: T) => void;

// export type AsyncCallback<T> = (err: BusinessError, data: T) => void;

export class DisplayImpl implements Display {
    static { loadLibrary("displayani_kit.z"); }

    constructor() {
        this.colorSpaces = new Array<ColorSpace>();
        this.hdrFormats = new Array<HDRFormat>();
    }

    id: double;

    name: string;

    alive: boolean;

    state: DisplayState;

    refreshRate: double;

    rotation: double;

    width: double;
 
    height: double;

    availableHeight: double;

    availableWidth: double;

    densityDPI: double;

    orientation: Orientation;

    densityPixels: double;

    scaledDensity: double;

    xDPI: double;

    yDPI: double;

    colorSpaces: Array<ColorSpace>;

    hdrFormats: Array<HDRFormat>;

    getCutoutInfo(callback: AsyncCallback<CutoutInfo>): void {
        taskpool.execute((): CutoutInfo => {
            let res: CutoutInfo = new CutoutInfoImpl()
            this.getCutoutInfoInternal(res);
            minusRectArray(res.boundingRects);
            return res;
        }).then((ret: NullishType) => {
            callback(new BusinessError(), ret as CutoutInfo);
        }).catch((err: NullishType) => {
            callback(err as BusinessError, new CutoutInfoImpl());
        });
    }

    getCutoutInfo(): Promise<CutoutInfo> {
        return new Promise<CutoutInfo>((resolve: (value: CutoutInfo) => void, reject: (error: BusinessError) => void ) => {
            taskpool.execute((): CutoutInfo => {
                let res: CutoutInfo = new CutoutInfoImpl()
                this.getCutoutInfoInternal(res);
                minusRectArray(res.boundingRects);
                return res;
            }).then((ret: NullishType) => {
                resolve(ret as CutoutInfo);
            }).catch((err: NullishType) => {
                reject(err as BusinessError);
            });
        });
    }

    public native getCutoutInfoInternal(cutoutInfo: CutoutInfo): void;

}

function minusRectArray(rectArr: Array<Rect>): void {
    let i = 0;
    for (; i < rectArr.length; i++) {
        if (rectArr[i].width == 0) {
            break;
        }
    }
    rectArr.splice(i);
}

export native function isFoldable(): boolean;

export function getFoldDisplayMode(): FoldDisplayMode {
    let res = getFoldDisplayModeNative();
    return res as FoldDisplayMode;
}

export native function getFoldDisplayModeNative(): int;

export function getFoldStatus(): FoldStatus{
    let res = getFoldStatusNative();
    return res as FoldStatus;
}

export native function getFoldStatusNative(): int;

export function getCurrentFoldCreaseRegion(): FoldCreaseRegion {
    let foldCreaseRegion:FoldCreaseRegion = new FoldCreaseRegionImpl();
    // getCurrentFoldCreaseRegionNative(foldCreaseRegion);
    return foldCreaseRegion;
}

export native function getCurrentFoldCreaseRegionNative(foldCreaseRegion : FoldCreaseRegion): int;

// display sync interfaces
export function getDisplayByIdSync(displayId: number): Display {
    let display:Display = new DisplayImpl();
    getDisplayByIdSyncNative(display as object, displayId);
    return display;
}

export native function getDisplayByIdSyncNative(obj: object, displayId: number): int;

export function getDefaultDisplaySync(): Display {
    let display:Display = new DisplayImpl();
    getDefaultDisplaySyncNative(display as object);
    return display;
}

export native function getDefaultDisplaySyncNative(obj: object): int;

export native function getAllDisplaysSyncNative(displayArray: Array<Display>): void;

export function getAllDisplays(callback?: AsyncCallback<Array<Display>>): undefined | Promise<Array<Display>> {
    if (callback != undefined) {
        let AsyncCallback: AsyncCallback<Array<Display>> = callback as AsyncCallback<Array<Display>>;
        taskpool.execute((): Array<Display> => {
            let res: Array<Display> = new Array<Display>(5);
            for (let i = 0; i < res.length; i++) {
                res[i] = new DisplayImpl();
            }
            getAllDisplaysSyncNative(res);
            minusDisplayArray(res);
            return res;
        }).then((ret: NullishType) => {
            AsyncCallback(new BusinessError(), ret as Array<Display>);
        }).catch((err: NullishType) => {
            AsyncCallback(err as BusinessError, new Array<Display>());
        });
        return undefined;
    } else {
        return new Promise<Array<Display>>((resolve: (value: Array<Display>) => void, reject: (error: BusinessError) => void ) => {
        taskpool.execute((): Array<Display> => {
            let res: Array<Display> = new Array<Display>(5);
            for (let i = 0; i < res.length; i++) {
                res[i] = new DisplayImpl();
            }
            getAllDisplaysSyncNative(res);
            minusDisplayArray(res)
            return res;
        }).then((ret: NullishType) => {
            resolve(ret as Array<Display>);
        }).catch((err: NullishType) => {
            reject(err as BusinessError);
        });
    });
    }
}

function minusDisplayArray(displayArr: Array<Display>): void {
    let i = 0;
    for (; i < displayArr.length; i++) {
        if (displayArr[i].width == 0) {
            break;
        }
    }
    displayArr.splice(i);
}

export function getNewDisplay(): DisplayImpl {
    return new DisplayImpl();
}

export function on(type: 'add' | 'remove' | 'change' | 'privateModeChange' | 'foldStatusChange' |
    'foldAngleChange' | 'captureStatusChange' | 'foldDisplayModeChange',
    callback: Callback<number> | Callback<boolean> | Callback<FoldStatus> | Callback<Array<number>> |
    Callback<boolean> | Callback<FoldDisplayMode>): void
{
    syncOn(type, callback as object, displayMgrRef);
}

export native function syncOn(type: 'add' | 'remove' | 'change' | 'privateModeChange' | 'foldStatusChange' |
    'foldAngleChange' | 'captureStatusChange' | 'foldDisplayModeChange',
    callback: object, nativeObj: long): void;

export function off(type: 'add' | 'remove' | 'change' | 'foldDisplayModeChange' | 'foldStatusChange',
    callback?: Callback<number>): void
{
    syncOff(type, callback as object, displayMgrRef);
}

export native function syncOff(type: 'add' | 'remove' | 'change' | 'foldDisplayModeChange' | 'foldStatusChange',
    callback: object, nativeObj: long) : void;

export function displayEventCallBack(cb: object, cbArg: double): void {
    const func = cb as (cbArg: double) => void;
    func(cbArg as double);
}

export function foldStatusCallback(cb: object, cbArg: int): void {
    const func = cb as (cbArg: FoldStatus) => void;
    func(cbArg as FoldStatus);
}

export let displayMgrRef: long;

export function setDisplayMgrRef(nativeObj : long) : void{
    displayMgrRef = nativeObj;
}
}