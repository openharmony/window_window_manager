/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_EXTENSION_BUSINESS_INFO_H
#define OHOS_ROSEN_EXTENSION_BUSINESS_INFO_H

#include <cstdint>

namespace OHOS::Rosen::Extension {

constexpr const char *const WINDOW_MODE_FIELD = "windowMode";

enum class Businesscode : uint8_t {
    SYNC_HOST_WINDOW_MODE = 0,  // Synchronize the host's window mode
};
}  // namespace OHOS::Rosen::Extension

#endif  // OHOS_ROSEN_EXTENSION_BUSINESS_INFO_H