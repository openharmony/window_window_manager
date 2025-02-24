
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

import { KPointer, runtimeType, RuntimeType, CallbackResource, DeserializerBase } from "@koalaui/interop"
import { int32, float32, int64 } from "@koalaui/common"
import { Serializer } from "./Serializer"
import { DISPLAYNativeModule } from "#components"
import { CallbackKind } from "./CallbackKind"
import { TypeChecker } from "#components"
import { KUint8ArrayPtr, NativeBuffer, InteropNativeModule } from "@koalaui/interop"
import { display, display_FoldStatus, display_FoldDisplayMode } from "./../OHDisplayNamespace"

export class Deserializer extends DeserializerBase {
     constructor(data: KUint8ArrayPtr, length: int32) {
        super(data, length)
    }
    readDisplay(): display.Display {
        let value : display.Display = ({} as display.Display)
        return value
    }
    readFoldCreaseRegion(): display.FoldCreaseRegion {
        let valueDeserializer : Deserializer = this
        const displayId_result : number = (valueDeserializer.readNumber() as number)
        const creaseRects_buf_length : int32 = valueDeserializer.readInt32()
        let creaseRects_buf : Array<display.Rect> = new Array<display.Rect>()
        for (let creaseRects_buf_i = 0; creaseRects_buf_i < creaseRects_buf_length; creaseRects_buf_i++) {
            creaseRects_buf[creaseRects_buf_i] = valueDeserializer.readRect()
        }
        const creaseRects_result : Array<display.Rect> = creaseRects_buf
        let value : display.FoldCreaseRegion = ({displayId: displayId_result,creaseRects: creaseRects_result} as display.FoldCreaseRegion)
        return value
    }
    readRect(): display.Rect {
        let valueDeserializer : Deserializer = this
        const left_result : number = (valueDeserializer.readNumber() as number)
        const top_result : number = (valueDeserializer.readNumber() as number)
        const width_result : number = (valueDeserializer.readNumber() as number)
        const height_result : number = (valueDeserializer.readNumber() as number)
        let value : display.Rect = ({left: left_result,top: top_result,width: width_result,height: height_result} as display.Rect)
        return value
    }
}
