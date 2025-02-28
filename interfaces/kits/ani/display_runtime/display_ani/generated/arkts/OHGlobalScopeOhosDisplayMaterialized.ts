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

import { display_FoldDisplayMode, display, display_FoldStatus } from "./OHDisplayNamespace"
import { TypeChecker } from "#components"
import { Finalizable, isResource, isInstanceOf, runtimeType, RuntimeType, SerializerBase, registerCallback, wrapCallback, KPointer, MaterializedBase, NativeBuffer } from "@koalaui/interop"
import { unsafeCast, int32, float32 } from "@koalaui/common"
import { Serializer } from "./peers/Serializer"
import { CallbackKind } from "./peers/CallbackKind"
import { Deserializer } from "./peers/Deserializer"
import { DISPLAYNativeModule } from "./DISPLAYNativeModule"
export class GlobalScope_ohos_display {
    public static getFoldDisplayMode(): display_FoldDisplayMode {
        return GlobalScope_ohos_display.getFoldDisplayMode_serialize()
    }
    public static getDefaultDisplaySync(): display.Display {
        return GlobalScope_ohos_display.getDefaultDisplaySync_serialize()
    }
    public static getFoldStatus(): display_FoldStatus {
        return GlobalScope_ohos_display.getFoldStatus_serialize()
    }
    public static getCurrentFoldCreaseRegion(): display.FoldCreaseRegion {
        return GlobalScope_ohos_display.getCurrentFoldCreaseRegion_serialize()
    }
    public static getDisplayByIdSync(displayId: number): display.Display {
        const displayId_casted = displayId as (number)
        return GlobalScope_ohos_display.getDisplayByIdSync_serialize(displayId_casted)
    }
    public static isFoldable(): boolean {
        return GlobalScope_ohos_display.isFoldable_serialize()
    }
    public static on(type: string): number {
        const type_casted = type as (string)
        return GlobalScope_ohos_display.on_serialize(type_casted)
    }
    public static getAllDisplays(): Array<display.Display> {
        return GlobalScope_ohos_display.getAllDisplays_serialize()
    }
    private static getFoldDisplayMode_serialize(): display_FoldDisplayMode {
        const retval  = DISPLAYNativeModule._GlobalScope_ohos_display_getFoldDisplayMode()
        throw new Error("Object deserialization is not implemented.")
    }
    private static getDefaultDisplaySync_serialize(): display.Display {
        const retval  = DISPLAYNativeModule._GlobalScope_ohos_display_getDefaultDisplaySync()
        throw new Error("Object deserialization is not implemented.")
    }
    private static getFoldStatus_serialize(): display_FoldStatus {
        const retval  = DISPLAYNativeModule._GlobalScope_ohos_display_getFoldStatus()
        throw new Error("Object deserialization is not implemented.")
    }
    private static getCurrentFoldCreaseRegion_serialize(): display.FoldCreaseRegion {
        const retval  = DISPLAYNativeModule._GlobalScope_ohos_display_getCurrentFoldCreaseRegion()
        return new Deserializer(retval, retval.length).readFoldCreaseRegion()
    }
    private static getDisplayByIdSync_serialize(displayId: number): display.Display {
        const retval  = DISPLAYNativeModule._GlobalScope_ohos_display_getDisplayByIdSync(displayId)
        throw new Error("Object deserialization is not implemented.")
    }
    private static isFoldable_serialize(): boolean {
        const retval  = DISPLAYNativeModule._GlobalScope_ohos_display_isFoldable()
        return retval
    }
    private static on_serialize(type: string): number {
        const retval  = DISPLAYNativeModule._GlobalScope_ohos_display_on(type)
        return retval
    }
    private static getAllDisplays_serialize(): Array<display.Display> {
        const retval  = DISPLAYNativeModule._GlobalScope_ohos_display_getAllDisplays()
        throw new Error("Object deserialization is not implemented.")
    }
}
