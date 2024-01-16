/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef WINDOW_WINDOW_MANAGER_EXTENSION_WINDOW_H
#define WINDOW_WINDOW_MANAGER_EXTENSION_WINDOW_H

#include <atomic>
#include <optional>
#include <shared_mutex>
#include "wm_common.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
class ExtensionWindow {
public:
    virtual ~ExtensionWindow() = default;

    virtual WMError GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea) = 0;

    virtual sptr<Window> GetWindow() = 0;

    virtual WMError HideNonSecureWindows(bool shouldHide) = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif //WINDOW_WINDOW_MANAGER_EXTENSION_WINDOW_H
