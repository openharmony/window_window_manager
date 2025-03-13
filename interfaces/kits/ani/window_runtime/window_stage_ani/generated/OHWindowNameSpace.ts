/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


// WARNING! THIS FILE IS AUTO-GENERATED, DO NOT MAKE CHANGES, THEY WILL BE LOST ON NEXT GENERATION!

import { TypeChecker } from "#components"
import { Finalizable, isResource, isInstanceOf, runtimeType, RuntimeType, SerializerBase, registerCallback, wrapCallback, KPointer, NativeBuffer } from "@koalaui/interop"
import { MaterializedBase } from "./MaterializedBase"
import { Serializer } from "./peers/Serializer"
import { unsafeCast } from "./shared/generated-utils"
import { CallbackKind } from "./peers/CallbackKind"
import { int32, float32 } from "@koalaui/common"
import { WINDOWNativeModule } from "./WINDOWNativeModule"
export namespace window {
    export class WindowInternal implements MaterializedBase,Window {
        peer?: Finalizable | undefined
        public getPeer(): Finalizable | undefined {
            return this.peer
        }
        static ctor(): KPointer {
            const retval  = WINDOWNativeModule._Window_ctor()
            return retval
        }
         constructor() {
            const ctorPtr : KPointer = WindowInternal.ctor()
            this.peer = new Finalizable(ctorPtr, WindowInternal.getFinalizer())
        }
        static getFinalizer(): KPointer {
            return WINDOWNativeModule._Window_getFinalizer()
        }
        public getWindowDecorHeight(): number {
            return this.getWindowDecorHeight_serialize()
        }
        public setWindowBackgroundColor(color: string): void {
            const color_casted = color as (string)
            this.setWindowBackgroundColor_serialize(color_casted)
            return
        }
        public setImmersiveModeEnabledState(enabled: boolean): void {
            const enabled_casted = enabled as (boolean)
            this.setImmersiveModeEnabledState_serialize(enabled_casted)
            return
        }
        public getWindowProperties(): window.WindowProperties {
            return this.getWindowProperties_serialize()
        }
        public setWindowDecorVisible(isVisible: boolean): void {
            const isVisible_casted = isVisible as (boolean)
            this.setWindowDecorVisible_serialize(isVisible_casted)
            return
        }
        public setWindowDecorHeight(height: number): void {
            const height_casted = height as (number)
            this.setWindowDecorHeight_serialize(height_casted)
            return
        }
        public getWindowAvoidArea(type: window_AvoidAreaType): window.AvoidArea {
            const type_casted = type as (window_AvoidAreaType)
            return this.getWindowAvoidArea_serialize(type_casted)
        }
        public onWindowStatusChange(callback_: ((parameter: window_WindowStatusType) => void)): void {
            const callback__casted = callback_ as (((parameter: window_WindowStatusType) => void))
            this.onWindowStatusChange_serialize(callback__casted)
            return
        }
        public offWindowStatusChange(callback_?: ((parameter: window_WindowStatusType) => void)): void {
            const callback__casted = callback_ as (((parameter: window_WindowStatusType) => void) | undefined)
            this.offWindowStatusChange_serialize(callback__casted)
            return
        }
        on(type: string, callback_: ((parameter: window_WindowStatusType) => void)): void {
            throw new Error("TBD")
        }
        off(type: string, callback_: ((parameter: window_WindowStatusType) => void)): void {
            throw new Error("TBD")
        }
        private getWindowDecorHeight_serialize(): number {
            const retval  = WINDOWNativeModule._Window_getWindowDecorHeight(this.peer!.ptr)
            return retval
        }
        private setWindowBackgroundColor_serialize(color: string): void {
            WINDOWNativeModule._Window_setWindowBackgroundColor(this.peer!.ptr, color)
        }
        private setImmersiveModeEnabledState_serialize(enabled: boolean): void {
            WINDOWNativeModule._Window_setImmersiveModeEnabledState(this.peer!.ptr, enabled ? 1 : 0)
        }
        private getWindowProperties_serialize(): window.WindowProperties {
            const retval  = WINDOWNativeModule._Window_getWindowProperties(this.peer!.ptr)
            throw new Error("Object deserialization is not implemented.")
        }
        private setWindowDecorVisible_serialize(isVisible: boolean): void {
            WINDOWNativeModule._Window_setWindowDecorVisible(this.peer!.ptr, isVisible ? 1 : 0)
        }
        private setWindowDecorHeight_serialize(height: number): void {
            WINDOWNativeModule._Window_setWindowDecorHeight(this.peer!.ptr, height)
        }
        private getWindowAvoidArea_serialize(type: window_AvoidAreaType): window.AvoidArea {
            const retval  = WINDOWNativeModule._Window_getWindowAvoidArea(this.peer!.ptr, (type.valueOf() as int32))
            throw new Error("Object deserialization is not implemented.")
        }
        private onWindowStatusChange_serialize(callback_: ((parameter: window_WindowStatusType) => void)): void {
            const thisSerializer : Serializer = Serializer.hold()
            thisSerializer.holdAndWriteCallback(callback_)
            WINDOWNativeModule._Window_onWindowStatusChange(this.peer!.ptr, thisSerializer.asArray(), thisSerializer.length())
            thisSerializer.release()
        }
        private offWindowStatusChange_serialize(callback_?: ((parameter: window_WindowStatusType) => void)): void {
            const thisSerializer : Serializer = Serializer.hold()
            let callback__type : int32 = RuntimeType.UNDEFINED
            callback__type = runtimeType(callback_)
            thisSerializer.writeInt8(callback__type as int32)
            if ((RuntimeType.UNDEFINED) != (callback__type)) {
                const callback__value  = callback_!
                thisSerializer.holdAndWriteCallback(callback__value)
            }
            WINDOWNativeModule._Window_offWindowStatusChange(this.peer!.ptr, thisSerializer.asArray(), thisSerializer.length())
            thisSerializer.release()
        }
        public static fromPtr(ptr: KPointer): WindowInternal {
            const obj : WindowInternal = new WindowInternal()
            obj.peer = new Finalizable(ptr, WindowInternal.getFinalizer())
            return obj
        }
    }
}
export enum window_WindowStatusType {
    UNDEFINED = 0,
    FULL_SCREEN = 1,
    MAXIMIZE = 2,
    MINIMIZE = 3,
    FLOATING = 4,
    SPLIT_SCREEN = 5
}
export enum window_WindowEventType {
    WINDOW_SHOWN = 1,
    WINDOW_ACTIVE = 2,
    WINDOW_INACTIVE = 3,
    WINDOW_HIDDEN = 4,
    WINDOW_DESTROYED = 7
}
export namespace window {
    export interface Callback {
    }
}
export namespace window {
    export interface AvoidArea {
        visible: boolean
        leftRect: window.Rect
        topRect: window.Rect
        rightRect: window.Rect
        bottomRect: window.Rect
    }
}
export enum window_AvoidAreaType {
    TYPE_SYSTEM = 0,
    TYPE_CUTOUT = 1,
    TYPE_SYSTEM_GESTURE = 2,
    TYPE_KEYBOARD = 3,
    TYPE_NAVIGATION_INDICATOR = 4
}
export enum window_WindowType {
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
export namespace window {
    export interface Rect {
        left: number
        top: number
        width: number
        height: number
    }
}
export namespace window {
    export interface WindowProperties {
        windowRect: window.Rect
        drawableRect: window.Rect
        type: window_WindowType
        isFullScreen: boolean
        isLayoutFullScreen: boolean
        focusable: boolean
        touchable: boolean
        brightness: number
        dimBehindValue: number
        isKeepScreenOn: boolean
        isPrivacyMode: boolean
        isRoundCorner: boolean
        isTransparent: boolean
        id: number
        displayId?: number
    }
}
export namespace window {
    export interface Window {
        getWindowDecorHeight(): number 
        setWindowBackgroundColor(color: string): void 
        setImmersiveModeEnabledState(enabled: boolean): void 
        getWindowProperties(): window.WindowProperties 
        setWindowDecorVisible(isVisible: boolean): void 
        setWindowDecorHeight(height: number): void 
        getWindowAvoidArea(type: window_AvoidAreaType): window.AvoidArea 
        onWindowStatusChange(callback_: ((parameter: window_WindowStatusType) => void)): void 
        offWindowStatusChange(callback_?: ((parameter: window_WindowStatusType) => void)): void 
    }
}
