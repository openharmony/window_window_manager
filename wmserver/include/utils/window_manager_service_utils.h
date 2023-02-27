/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_WINDOW_MANAGER_SERVICE_UTILS_H
#define OHOS_WINDOW_MANAGER_SERVICE_UTILS_H

#include <map>
#include <refbase.h>
#include <parameters.h>

#include "dm_common.h"
#include "wm_common.h"
#include "window_node.h"
#include "display_info.h"


namespace OHOS {
namespace Rosen {
namespace {
    const std::map<Orientation, DisplayOrientation> WINDOW_TO_DISPLAY_ORIENTATION_MAP {
        {Orientation::VERTICAL,            DisplayOrientation::PORTRAIT   },
        {Orientation::HORIZONTAL,          DisplayOrientation::LANDSCAPE   },
        {Orientation::REVERSE_VERTICAL,    DisplayOrientation::PORTRAIT_INVERTED   },
        {Orientation::REVERSE_HORIZONTAL,  DisplayOrientation::LANDSCAPE_INVERTED   },
    };
    const bool FIX_ORIENTATION_ENABLE = system::GetBoolParameter("persist.window.fixoriention.enable", true);
}

class WmsUtils {
public:
    static bool IsFixedOrientation(Orientation orientation, WindowMode mode, uint32_t flags);
    static bool IsExpectedRotateLandscapeWindow(Orientation requestOrientation,
        DisplayOrientation currentOrientation, uint32_t flags);
    static bool IsExpectedRotatableWindow(Orientation requestOrientation,
        DisplayOrientation currentOrientation, WindowMode mode, uint32_t flags);
    static bool IsExpectedRotatableWindow(Orientation requestOrientation,
        DisplayOrientation currentOrientation, uint32_t flags);
};
}
}
#endif // OHOS_WINDOW_MANAGER_SERVICE_UTILS_H