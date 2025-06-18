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
declare namespace display {

export class BusinessError extends Error {
  public code: number;
  public message: string;
}

export interface Rect {
    left: number;
 
    top: number;
 
    width: number;
 
    height: number;
}

export interface WaterfallDisplayAreaRects {
    readonly left: Rect;
 
    readonly right: Rect;
 
    readonly top: Rect;
 
    readonly bottom: Rect;
}

export interface CutoutInfo {
    readonly boundingRects: Array<Rect>;
 
    readonly waterfallDisplayAreaRects: WaterfallDisplayAreaRects;
}
 
export interface FoldCreaseRegion {
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
 
export enum DisplayState {
 
    STATE_UNKNOWN = 0,
 
    STATE_OFF,
 
    STATE_ON,
 
    STATE_DOZE,
 
    STATE_DOZE_SUSPEND,
 
    STATE_VR,
 
    STATE_ON_SUSPEND
}
 
export enum ColorSpace {
 
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
 
export enum HDRFormat {
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
    id: number;
 
    name: string;
 
    alive: boolean;
 
    state: DisplayState;
 
    refreshRate: number;
 
    rotation: number;
 
    width: number;
 
    height: number;
 
    availableHeight: number;
 
    availableWidth: number;
 
    densityDPI: number;
 
    orientation: Orientation;
 
    densityPixels: number;
 
    scaledDensity: number;
 
    xDPI: number;
 
    yDPI: number;
 
    colorSpaces: Array<ColorSpace>;
 
    hdrFormats: Array<HDRFormat>;
 
    getCutoutInfo(callback: AsyncCallback<CutoutInfo>): void;

    getCutoutInfo(): Promise<CutoutInfo>;
}
 
export type Callback<T> = (data: T) => void;
 
export type AsyncCallback<T> = (err: BusinessError, data: T) => void; 
 
export function isFoldable(): boolean;

export function getFoldDisplayMode(): FoldDisplayMode;

export function getFoldStatus(): FoldStatus;
 
export function getCurrentFoldCreaseRegion(): FoldCreaseRegion;

export function getDisplayByIdSync(displayId: int): Display;
 
export function getDefaultDisplaySync(): Display;
 
export function getAllDisplaysSyncNative(): Array<Display>;
 
export function getAllDisplays(callback: AsyncCallback<Array<Display>>): void;

export function getAllDisplays(): Promise<Array<Display>>

export function on(type: 'add' | 'remove' | 'change', callback: Callback<number>): void;
 
export function off(type: 'foldDisplayModeChange' | 'foldStatusChange', callback?: Callback<int>): void;

}

export default display;