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

#include "window_manager_service_utils.h"

namespace OHOS {
namespace Rosen {

bool WmsUtils::IsFixedOrientation(Orientation orientation, WindowMode mode)
{
    if (mode != WindowMode::WINDOW_MODE_FULLSCREEN) {
        return false;
    }
    if (orientation > Orientation::REVERSE_HORIZONTAL) {
        return false;
    }
    if (orientation < Orientation::VERTICAL) {
        return false;
    }
    return true;
}

bool WmsUtils::IsExpectedRotateLandscapeWindow(Orientation requestOrientation,
    DisplayOrientation currentOrientation)
{
    if (requestOrientation != Orientation::HORIZONTAL && requestOrientation != Orientation::REVERSE_HORIZONTAL) {
        return false;
    }
    return IsExpectedRotatableWindow(requestOrientation, currentOrientation);
}

bool WmsUtils::IsExpectedRotatableWindow(Orientation requestOrientation,
    DisplayOrientation currentOrientation, WindowMode mode)
{
    if (mode != WindowMode::WINDOW_MODE_FULLSCREEN) {
        return false;
    }
    return IsExpectedRotatableWindow(requestOrientation, currentOrientation);
}

bool WmsUtils::IsExpectedRotatableWindow(Orientation requestOrientation,
    DisplayOrientation currentOrientation)
{
    if (WINDOW_TO_DISPLAY_ORIENTATION_MAP.count(requestOrientation) == 0) {
        return false;
    }
    DisplayOrientation disOrientation = WINDOW_TO_DISPLAY_ORIENTATION_MAP.at(requestOrientation);
    if (disOrientation != currentOrientation) {
        return true;
    }
    return false;
}
}
}