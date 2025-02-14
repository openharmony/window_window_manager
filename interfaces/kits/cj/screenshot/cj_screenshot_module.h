/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CJ_SCREENSHOT_MODULE_H
#define CJ_SCREENSHOT_MODULE_H

#include <map>

#include "screenshot_utils.h"
#include "ffi_remote_data.h"

namespace OHOS {
namespace Rosen {

class CJScreenshotModule {
public:
    static RetStruct PickFunc();
    static RetStruct CaptureFunc(uint32_t displayId);
};

} // namespace Rosen
} // namespace OHOS

#endif // CJ_SCREENSHOT_MODULE_H
