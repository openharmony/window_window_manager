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
import colorSpaceManager from '@ohos.graphics.colorSpaceManager';
import hdrCapability from '@ohos.graphics.hdrCapability';

export default namespace display {

loadLibrary('displayani_kit.z');

const DISPLAY_ARRAY_LENGTH = 15;  
const CUTO_ARRAY_LENGTH = 5;

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

export interface FoldCreaseRegion {
    readonly displayId: number;

    readonly creaseRects: Array<Rect>;
}

export class FoldCreaseRegionImpl implements FoldCreaseRegion {
    constructor() {
        this.creaseRects = new Array<Rect>(10);
        for (let i = 0; i < this.creaseRects.length; i++) {
            this.creaseRects[i] = new RectImpl();
        }
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
    FOLD_STATUS_UNKNOWN = 0,

    FOLD_STATUS_EXPANDED,

    FOLD_STATUS_FOLDED,

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

    colorSpaces: Array<colorSpaceManager.ColorSpace>;

    hdrFormats: Array<hdrCapability.HDRFormat>;

    getCutoutInfo(callback: AsyncCallback<CutoutInfo>): void;

    getCutoutInfo(): Promise<CutoutInfo>;
}

export class DisplayImpl implements Display {
    static { loadLibrary("displayani_kit.z"); }

    constructor() {
        this.colorSpaces = new Array<colorSpaceManager.ColorSpace>();
        this.hdrFormats = new Array<hdrCapability.HDRFormat>();
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

    colorSpaces: Array<colorSpaceManager.ColorSpace>;

    hdrFormats: Array<hdrCapability.HDRFormat>;

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
    let foldCreaseRegion: FoldCreaseRegion = new FoldCreaseRegionImpl();
    getCurrentFoldCreaseRegionNative(foldCreaseRegion as object, displayMgrRef);
    minusCreaseRects(foldCreaseRegion);
    return foldCreaseRegion;
}

function minusCreaseRects(foldCreaseRegionObj: FoldCreaseRegion): void {
    let foldCreaseRegion: FoldCreaseRegion = foldCreaseRegionObj as FoldCreaseRegion;
    let creaseRects: Array<Rect> = foldCreaseRegion.creaseRects;
    let i = 0;
    for (; i < creaseRects.length; i++) {
        if (creaseRects[i].left == 0 && creaseRects[i].top == 0 && 
            creaseRects[i].width == 0 && creaseRects[i].height == 0) {
                break;
        }
    }
    creaseRects.splice(i);
}

export native function getCurrentFoldCreaseRegionNative(foldCreaseRegion: object, nativeObj: long): void;

// display sync interfaces
export function getDisplayByIdSync(displayId: number): Display {
    let display:Display = new DisplayImpl();
    getDisplayByIdSyncNative(display as object, displayId);
    return display;
}

export native function getDisplayByIdSyncNative(obj: object, displayId: number): void;

export function getDefaultDisplaySync(): Display {
    let display:Display = new DisplayImpl();
    getDefaultDisplaySyncNative(display as object);
    return display;
}

export native function getDefaultDisplaySyncNative(obj: object): void;

export native function getAllDisplaysSyncNative(displayArray: Array<Display>): void;

export function getAllDisplays(callback: AsyncCallback<Array<Display>>): void {
    let res: Array<Display> = new Array<Display>(DISPLAY_ARRAY_LENGTH);
    for (let i = 0; i < res.length; i++) {
        res[i] = new DisplayImpl();
    }
    try {
        getAllDisplaysSyncNative(res);
        minusDisplayArray(res);
        callback(new BusinessError(), res);
    } catch (err: BusinessError) {
        callback(err, res);
    }
    return;
}

export function getAllDisplays(): Promise<Array<Display>> {
    return new Promise<Array<Display>>((resolve: (value: Array<Display>) => void, reject: (error: BusinessError) => void ) => {
        taskpool.execute((): Array<Display> => {
            let res: Array<Display> = new Array<Display>(DISPLAY_ARRAY_LENGTH);
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

export function on<T>(type: string, callback: Callback<T>): void {
    syncOn(type, callback, displayMgrRef);
}
 
export native function syncOn(type: string, callback: object, nativeObj: long): void;

export function off<T>(type: string, callback?: Callback<T>): void {
    syncOff(type, displayMgrRef, callback);
}

export native function syncOff(type: string, nativeObj: long, callback?: object) : void;

export function displayEventCallBack(cb: object, cbArg: double): void {
    const func = cb as (cbArg: double) => void;
    func(cbArg as double);
}

export function foldStatusCallback(cb: object, cbArg: int): void {
    const func = cb as (cbArg: FoldStatus) => void;
    func(cbArg as FoldStatus);
}

export function foldDisplayModeCallback(cb: object, cbArg: int): void {
    const func = cb as (cbArg: FoldDisplayMode) => void;
    func(cbArg as FoldDisplayMode);
}

export let displayMgrRef: long;

export function setDisplayMgrRef(nativeObj : long) : void{
    displayMgrRef = nativeObj;
}
}