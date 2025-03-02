export default namespace display {

interface Rect {
    left: number;

    top: number;

    width: number;

    height: number;
}

export class RectImpl implements Rect {

}

interface WaterfallDisplayAreaRects {
    readonly left: Rect;

    readonly right: Rect;

    readonly top: Rect;

    readonly bottom: Rect;
}

export class WaterfallDisplayAreaRectsImpl implements WaterfallDisplayAreaRects {

}

interface CutoutInfo {
    readonly boundingRects: Array<Rect>;

    readonly waterfallDisplayAreaRects: WaterfallDisplayAreaRects;
}

export class CutoutInfoImpl implements CutoutInfo {

}

interface FoldCreaseRegion {
    readonly displayId: number;

    readonly creaseRects: Array<Rect>;
}

export class FoldCreaseRegionImpl implements FoldCreaseRegion {

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

export interface Callback<T> {
  (data: T): void;
}

export class DisplayImpl implements Display {
    static { loadLibrary("displayani_kit.z"); }

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

export native function on(type: 'add' | 'remove' | 'change', callback: Callback<number>): void;
}