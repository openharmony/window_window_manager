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

import { SerializerBase, Tags, RuntimeType, runtimeType, isResource, isInstanceOf, MaterializedBase, nullptr, KPointer } from "@koalaui/interop"
import { int32, float32, unsafeCast, int64 } from "@koalaui/common"
import { TypeChecker } from "#components"
import { KUint8ArrayPtr, NativeBuffer, InteropNativeModule } from "@koalaui/interop"
import { display, display_FoldStatus, display_FoldDisplayMode } from "./../OHDisplayNamespace"
export class Serializer extends SerializerBase {
    private static pool?: Array<Serializer> | undefined = undefined
    private static poolTop: int32 = -1
    static hold(): Serializer {
        if (!(Serializer.pool != undefined))
        {
            Serializer.pool = new Array<Serializer>(8)
            const pool : Array<Serializer> = (Serializer.pool)!
            for (let idx = 0; idx < 8; idx++) {
                pool[idx] = new Serializer()
            }
        }
        const pool : Array<Serializer> = (Serializer.pool)!
        if (Serializer.poolTop >= pool.length - 1)
        {
            throw new Error("Serializer pool is full. Check if you had released serializers before")
        }
        Serializer.poolTop = Serializer.poolTop + 1
        let serializer  = pool[Serializer.poolTop]
        return serializer
    }
    public release(): void {
        if (Serializer.poolTop == -1)
        {
            throw new Error("Serializer pool is empty. Check if you had hold serializers before")
        }
        const pool : Array<Serializer> = (Serializer.pool)!
        if ((this) == (pool[Serializer.poolTop]))
        {
            Serializer.poolTop = Serializer.poolTop - 1
            super.release()
            return
        }
        throw new Error("Only last serializer should be released")
    }
     constructor() {
        super()
    }
    writeDisplay(value: display.Display): void {
    }
    writeFoldCreaseRegion(value: display.FoldCreaseRegion): void {
        let valueSerializer : Serializer = this
        const value_displayId  = value.displayId
        valueSerializer.writeNumber(value_displayId)
        const value_creaseRects  = value.creaseRects
        valueSerializer.writeInt32(value_creaseRects.length as int32)
        for (let i = 0; i < value_creaseRects.length; i++) {
            const value_creaseRects_element : display.Rect = value_creaseRects[i]
            valueSerializer.writeRect(value_creaseRects_element)
        }
    }
    writeRect(value: display.Rect): void {
        let valueSerializer : Serializer = this
        const value_left  = value.left
        valueSerializer.writeNumber(value_left)
        const value_top  = value.top
        valueSerializer.writeNumber(value_top)
        const value_width  = value.width
        valueSerializer.writeNumber(value_width)
        const value_height  = value.height
        valueSerializer.writeNumber(value_height)
    }
}