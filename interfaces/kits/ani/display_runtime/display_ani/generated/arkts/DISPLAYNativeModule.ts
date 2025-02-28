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

import { KInt, KBoolean, KFloat, KUInt, KStringPtr, KPointer, KNativePointer, KInt32ArrayPtr, KUint8ArrayPtr, KFloat32ArrayPtr, pointer, KInteropReturnBuffer, NativeBuffer, loadNativeModuleLibrary } from "@koalaui/interop"
import { int32, float32 } from "@koalaui/common"

export class DISPLAYNativeModule {
    static {
        loadNativeModuleLibrary("DISPLAYNativeModule")
    }
    native static _AllocateNativeBuffer(len: int32, data: KUint8ArrayPtr, init: KUint8ArrayPtr): NativeBuffer 
    native static _Display_ctor(): KPointer 
    native static _Display_getFinalizer(): KPointer 
    native static _Display_getCutoutInfo(ptr: KPointer): KInteropReturnBuffer 
    native static _GlobalScope_ohos_display_getFoldDisplayMode(): KPointer 
    native static _GlobalScope_ohos_display_getDefaultDisplaySync(): KPointer 
    native static _GlobalScope_ohos_display_getFoldStatus(): KPointer 
    native static _GlobalScope_ohos_display_getCurrentFoldCreaseRegion(): KInteropReturnBuffer 
    native static _GlobalScope_ohos_display_getDisplayByIdSync(displayId: number): KPointer 
    native static _GlobalScope_ohos_display_isFoldable(): boolean 
    native static _GlobalScope_ohos_display_on(type: KStringPtr): number 
    native static _GlobalScope_ohos_display_getAllDisplays(): KPointer 
}