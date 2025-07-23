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

#ifndef OHOS_ROSEN_WINDOW_SESSION_COORDINATE_HELPER_H
#define OHOS_ROSEN_WINDOW_SESSION_COORDINATE_HELPER_H

#include "interfaces/include/ws_common.h"

namespace OHOS::Rosen {

/**
 * @class SessionCoordinateHelper
 *
 * @brief Helper class for converting between global and screen-relative coordinates.
 *
 * Provides utility methods to convert window rectangles between:
 * - Global coordinates: a unified coordinate space where (0, 0) is the top-left of the primary screen.
 * - Screen-relative coordinates: coordinates relative to the top-left corner of a specific screen.
 */
class SessionCoordinateHelper {
public:
    /**
     * @brief Convert screen-relative coordinates to global coordinates.
     *
     * This function converts a rectangle defined relative to a specific screen's origin
     * into global coordinates based on the screen’s global position.
     *
     * @param screenId The screen ID associated with the relative coordinates.
     * @param relativeRect The rectangle relative to the given screen.
     * @return The rectangle in global coordinates.
     */
    static WSRect RelativeToGlobalDisplayRect(ScreenId screenId, const WSRect& relativeRect);

    /**
     * @brief Convert global coordinates to screen-relative coordinates.
     *
     * This function maps a global rectangle into the coordinate space of a specific screen.
     * It selects the screen that has the largest intersection area with the given global rectangle.
     * If no screen overlaps, the original screen is used as a fallback.
     *
     * @param originalScreenId The original screen ID where the window was originally located.
     * @param globalRect The rectangle in global coordinates.
     * @return The rectangle relative to the selected screen, along with its screen ID.
     */
    static WSScreenRelativeRect GlobalToScreenRelativeRect(ScreenId originalScreenId, const WSRect& globalRect);
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SESSION_COORDINATE_HELPER_H
