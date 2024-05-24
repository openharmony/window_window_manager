/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_CJ_DISPLAY_MANAGER_H
#define OHOS_CJ_DISPLAY_MANAGER_H

#include "display_utils.h"
#include "ffi_remote_data.h"

namespace OHOS {
namespace Rosen {

class CJDisplayManager {
public:
    static RetStruct GetDefaultDisplaySync();
    static RetStruct GetAllDisplays();
    static RetStruct HasPrivateWindow(uint32_t displayId);
    static bool IsFoldable();
    static void SetFoldDisplayMode(uint32_t mode);
    static uint32_t GetFoldStatus();
    static uint32_t GetFoldDisplayMode();
    static RetStruct GetCurrentFoldCreaseRegion();
};

}
}

#endif // DISPLAY_MANAGER_H
