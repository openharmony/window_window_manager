/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "screenshot_ffi.h"

#include "cj_screenshot_module.h"
#include "ffi_remote_data.h"
#include "window_manager_hilog.h"

using namespace OHOS::FFI;

namespace OHOS {
namespace Rosen {
extern "C" {
RetStruct FfiOHOSScreenshotPick()
{
    TLOGI(WmsLogTag::WMS_SCB, "FfiOHOSScreenshotPick start");
    return CJScreenshotModule::PickFunc();
}

RetStruct FfiOHOSScreenshotCapture(uint32_t displayId)
{
    TLOGI(WmsLogTag::WMS_SCB, "FfiOHOSScreenshotCapture start");
    return CJScreenshotModule::CaptureFunc(displayId);
}
}
} // namespace Rosen
} // namespace OHOS
