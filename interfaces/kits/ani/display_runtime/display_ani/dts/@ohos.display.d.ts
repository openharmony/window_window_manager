declare namespace display {

  enum FoldDisplayMode {
    /**
     * Unknown Display.
     *
     * @syscap SystemCapability.Window.SessionManager
     * @since 10
     */
    /**
     * Unknown Display.
     *
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    FOLD_DISPLAY_MODE_UNKNOWN = 0,
    /**
     * Full Display.
     *
     * @syscap SystemCapability.Window.SessionManager
     * @since 10
     */
    /**
     * Full Display.
     *
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    FOLD_DISPLAY_MODE_FULL,
    /**
     * Main Display.
     *
     * @syscap SystemCapability.Window.SessionManager
     * @since 10
     */
    /**
     * Main Display.
     *
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    FOLD_DISPLAY_MODE_MAIN,
    /**
     * Sub Display.
     *
     * @syscap SystemCapability.Window.SessionManager
     * @since 10
     */
    /**
     * Sub Display.
     *
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    FOLD_DISPLAY_MODE_SUB,
    /**
     * Coordination Display.
     *
     * @syscap SystemCapability.Window.SessionManager
     * @since 10
     */
    /**
     * Coordination Display.
     *
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    FOLD_DISPLAY_MODE_COORDINATION
  }

  enum FoldStatus {
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

  interface FoldCreaseRegion {
    /**
     * The display ID is used to identify the screen where the crease is located.
     *
     * @type { number }
     * @readonly
     * @syscap SystemCapability.Window.SessionManager
     * @since 10
     */
    /**
     * The display ID is used to identify the screen where the crease is located.
     *
     * @type { number }
     * @readonly
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    readonly displayId: number;

    /**
     * Crease Region.
     *
     * @type { Array<Rect> }
     * @readonly
     * @syscap SystemCapability.Window.SessionManager
     * @since 10
     */
    /**
     * Crease Region.
     *
     * @type { Array<Rect> }
     * @readonly
     * @syscap SystemCapability.Window.SessionManager
     * @atomicservice
     * @since 12
     */
    readonly creaseRects: Array<Rect>;
  }

  /**
   * Rectangle
   *
   * @interface Rect
   * @syscap SystemCapability.WindowManager.WindowManager.Core
   * @since 9
   */
  /**
   * Rectangle
   *
   * @interface Rect
   * @syscap SystemCapability.WindowManager.WindowManager.Core
   * @atomicservice
   * @since 12
   */
   interface Rect {
    /**
     * The X-axis coordinate of the upper left vertex of the rectangle, in pixels.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 9
     */
    /**
     * The X-axis coordinate of the upper left vertex of the rectangle, in pixels.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    left: number;

    /**
     * The Y-axis coordinate of the upper left vertex of the rectangle, in pixels.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 9
     */
    /**
     * The Y-axis coordinate of the upper left vertex of the rectangle, in pixels.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    top: number;

    /**
     * Width of the rectangle, in pixels.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 9
     */
    /**
     * Width of the rectangle, in pixels.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    width: number;

    /**
     * Height of the rectangle, in pixels.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 9
     */
    /**
     * Height of the rectangle, in pixels.
     *
     * @type { number }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    height: number;
  }

  function getFoldDisplayMode(): FoldDisplayMode;

  function getDefaultDisplaySync(): Display;

  function getFoldStatus(): FoldStatus;
  
  function getCurrentFoldCreaseRegion(): FoldCreaseRegion;

  function getDisplayByIdSync(displayId: number): Display;

  function isFoldable(): boolean;

  function on(type: 'add' | 'remove' | 'change'): number;

  function getAllDisplays(): Array<Display>;

  interface Display{

    getCutoutInfo(): CutoutInfo;

  }

  interface CutoutInfo {
    /**
     * Bounding rectangles of the cutout areas of the display.
     *
     * @type { Array<Rect> }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 9
     */
    /**
     * Bounding rectangles of the cutout areas of the display.
     *
     * @type { Array<Rect> }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    readonly boundingRects: Array<Rect>;

    /**
     * Rectangles of curved parts on each side of a waterfall display.
     *
     * @type { WaterfallDisplayAreaRects }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 9
     */
    /**
     * Rectangles of curved parts on each side of a waterfall display.
     *
     * @type { WaterfallDisplayAreaRects }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    readonly waterfallDisplayAreaRects: WaterfallDisplayAreaRects;
  }

  interface WaterfallDisplayAreaRects {
    /**
     * Indicates the size of left side curved area of the waterfall screen.
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 9
     */
    /**
     * Indicates the size of left side curved area of the waterfall screen.
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    readonly left: Rect;

    /**
     * Indicates the size of right side curved area of the waterfall screen.
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 9
     */
    /**
     * Indicates the size of right side curved area of the waterfall screen.
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    readonly right: Rect;

    /**
     * Indicates the size of top side curved area of the waterfall screen.
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 9
     */
    /**
     * Indicates the size of top side curved area of the waterfall screen.
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    readonly top: Rect;

    /**
     * Indicates the size of bottom side curved area of the waterfall screen.
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @since 9
     */
    /**
     * Indicates the size of bottom side curved area of the waterfall screen.
     *
     * @type { Rect }
     * @syscap SystemCapability.WindowManager.WindowManager.Core
     * @atomicservice
     * @since 12
     */
    readonly bottom: Rect;
  }

}

export default display;