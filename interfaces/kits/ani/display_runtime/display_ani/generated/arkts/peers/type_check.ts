import { KBoolean, KStringPtr, NativeBuffer, MaterializedBase } from "@koalaui/interop"
import { display_FoldDisplayMode, display_FoldStatus, display } from "./../OHDisplayNamespace"
export class TypeChecker {
    static typeInstanceOf<T>(value: Object, prop: string): boolean {
        return value instanceof T
    }
    static typeCast<T>(value: Object): T {
        return value as T
    }
    static isdisplay_FoldDisplayMode(value: object|string|number|undefined|null): boolean {
        return value instanceof display_FoldDisplayMode
    }
    static isdisplay_FoldStatus(value: object|string|number|undefined|null): boolean {
        return value instanceof display_FoldStatus
    }
    static isdisplayDisplay(value: object|string|number|undefined|null): boolean {
        return value instanceof display.Display
    }
    static isdisplayFoldCreaseRegion(value: object|string|number|undefined|null, arg0: boolean, arg1: boolean): boolean {
        return value instanceof display.FoldCreaseRegion
    }
    static isdisplayRect(value: object|string|number|undefined|null, arg0: boolean, arg1: boolean, arg2: boolean, arg3: boolean): boolean {
        return value instanceof display.Rect
    }
    static isArray_Rect(value: object|string|number|undefined|null): boolean {
        return value instanceof Array<display.Rect>
    }
}