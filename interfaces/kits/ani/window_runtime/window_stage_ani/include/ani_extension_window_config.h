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

#ifndef OHOS_ANI_EXTENSION_WINDOW_CONFIG_H
#define OHOS_ANI_EXTENSION_WINDOW_CONFIG_H
#include "ani.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
#ifdef _WIN32
#define WINDOW_EXPORT __attribute__((dllexport))
#else
#define WINDOW_EXPORT __attribute__((visibility("default")))
#endif

WINDOW_EXPORT ani_object CreateAniExtensionWindowConfig(ani_env* env,
    const std::shared_ptr<ExtensionWindowConfig>& extensionWindowConfig);
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ANI_EXTENSION_WINDOW_CONFIG_H