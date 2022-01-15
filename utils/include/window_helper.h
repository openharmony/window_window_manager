/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_WM_INCLUDE_WM_HELPER_H
#define OHOS_WM_INCLUDE_WM_HELPER_H

#include <wm_common.h>

namespace OHOS {
namespace Rosen {
class WindowHelper {
public:
    static inline bool IsMainWindow(WindowType type)
    {
        return (type >= WindowType::APP_MAIN_WINDOW_BASE && type <= WindowType::APP_MAIN_WINDOW_END);
    }

    static inline bool IsSubWindow(WindowType type)
    {
        return (type >= WindowType::APP_SUB_WINDOW_BASE && type <= WindowType::APP_SUB_WINDOW_END);
    }

    static inline bool IsAppWindow(WindowType type)
    {
        return (IsMainWindow(type) || IsSubWindow(type));
    }

    static inline bool IsBelowSystemWindow(WindowType type)
    {
        return (type >= WindowType::BELOW_APP_SYSTEM_WINDOW_BASE && type <= WindowType::BELOW_APP_SYSTEM_WINDOW_END);
    }

    static inline bool IsAboveSystemWindow(WindowType type)
    {
        return (type >= WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE && type <= WindowType::ABOVE_APP_SYSTEM_WINDOW_END);
    }

    static inline bool IsSystemWindow(WindowType type)
    {
        return (IsBelowSystemWindow(type) || IsAboveSystemWindow(type));
    }

    static inline bool IsSplitWindowMode(WindowMode mode)
    {
        return mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY || mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY;
    }

    static inline bool IsValidWindowMode(WindowMode mode)
    {
        return mode == WindowMode::WINDOW_MODE_FULLSCREEN || mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY ||
            mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY || mode == WindowMode::WINDOW_MODE_FLOATING ||
            mode == WindowMode::WINDOW_MODE_PIP;
    }

    WindowHelper() = default;
    ~WindowHelper() = default;
};
} // namespace OHOS
} // namespace Rosen
#endif // OHOS_WM_INCLUDE_WM_HELPER_H