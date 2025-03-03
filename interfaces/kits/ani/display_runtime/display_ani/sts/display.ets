export default namespace display {

loadLibrary('displayani_kit.z');

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

interface CutoutInfo {
    readonly boundingRects: Array<Rect>;

    readonly waterfallDisplayAreaRects: WaterfallDisplayAreaRects;
}

export class CutoutInfoImpl implements CutoutInfo {
    constructor() {
        this.boundingRects = new Array<Rect>();
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

    getCutoutInfo(): CutoutInfo;
}

// export interface Callback<T> {
  // (data: T): void;
// }

// export class CallbackImpl<T> implements Callback<T>{
  // (data: T): void;
// }

type Callback = (data : number) => void;

export class DisplayImpl implements Display {
    static { loadLibrary("displayani_kit.z"); }

    constructor() {
        this.colorSpaces = new Array<ColorSpace>();
        this.hdrFormats = new Array<HDRFormat>();
    }

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

    public native getCutoutInfo(): CutoutInfo;

}

export native function isFoldable(): boolean;

export native function getFoldDisplayMode(): FoldDisplayMode;

export native function getFoldStatus(): FoldStatus;

export native function getCurrentFoldCreaseRegion(): FoldCreaseRegion;

// display sync interfaces
export native function getDisplayByIdSync(displayId: number): Display;

export native function getDefaultDisplaySync(): Display;

// encapsulate to async callback function!
export native function getAllDisplays(): Array<Display>;

export function on(type: 'add' | 'remove' | 'change', callback: Callback): void {
    syncOn(type, callback, displayMgrRef);
}

export native function syncOn(type: 'add' | 'remove' | 'change', callback: Callback, nativeObj: long): void;

export function off(type: 'foldDisplayModeChange' | 'foldStatusChange', callback?: Callback): void
{
    syncOff(type, callback, displayMgrRef);
}
export native function syncOff(type: 'foldDisplayModeChange' | 'foldStatusChange', callback?: Callback, nativeObj: long) : void;

export let displayMgrRef: long;

export function setDisplayMgrRef(nativeObj : long) : void{
    displayMgrRef = nativeObj;
}
}