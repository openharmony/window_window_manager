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

import { GlobalScope_ohos_display } from "./OHGlobalScopeOhosDisplayMaterialized"
import { display_FoldDisplayMode, display, display_FoldStatus } from "./OHDisplayNamespace"
export function getFoldDisplayMode(): display_FoldDisplayMode {
    return GlobalScope_ohos_display.getFoldDisplayMode()
}
export function getDefaultDisplaySync(): display.Display {
    return GlobalScope_ohos_display.getDefaultDisplaySync()
}
export function getFoldStatus(): display_FoldStatus {
    return GlobalScope_ohos_display.getFoldStatus()
}
export function getCurrentFoldCreaseRegion(): display.FoldCreaseRegion {
    return GlobalScope_ohos_display.getCurrentFoldCreaseRegion()
}
export function getDisplayByIdSync(displayId: number): display.Display {
    return GlobalScope_ohos_display.getDisplayByIdSync(displayId)
}
export function isFoldable(): boolean {
    return GlobalScope_ohos_display.isFoldable()
}
