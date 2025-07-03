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

#include <optional>
#include <unordered_map>

#include "dm_common.h"
#include "interfaces/include/ws_common.h"
#include "screen_session_manager_client/include/screen_session_manager_client.h"

namespace OHOS::Rosen {

/**
 * @class SessionCoordinateHelper
 *
 * @brief Utility class for coordinate conversions between global coordinates and relative display coordinates.
 *
 * This class provides helper methods to convert window rectangles between:
 * - Global coordinates: a unified space where (0,0) is the top-left corner of the primary display.
 * - Relative display coordinates: coordinates relative to the top-left corner of a specific display.
 */
class SessionCoordinateHelper {
public:
    /**
     * @brief Convert relative display coordinates to global coordinates.
     *
     * This function converts a rectangle defined relative to a specific display's origin
     * into global coordinates based on the display's global position.
     *
     * @param displayId The display ID associated with the relative coordinates.
     * @param relativeRect The rectangle relative to the given display.
     * @return The rectangle represented in global coordinates.
     */
    static WSRect RelativeToGlobalDisplayRect(DisplayId displayId, const WSRect& relativeRect);

    /**
     * @brief Convert global coordinates to relative display coordinates.
     *
     * This function maps a global rectangle into a display-relative coordinate space.
     * The best-matching display is determined based on the rectangle's top-left position,
     * overlap area, or maximum intersection area.
     *
     * @param originalDisplayId The original display ID where the window was created or originally located.
     * @param globalRect The rectangle in global coordinates.
     * @return The rectangle relative to a selected display, along with its associated display ID.
     */
    static WSRelativeDisplayRect GlobalToRelativeDisplayRect(DisplayId originalDisplayId, const WSRect& globalRect);

private:
    static WSRect ToRelative(const WSRect& globalRect, const WSRect& screenRect);

    static std::unordered_map<uint64_t, WSRect> BuildScreenRectMap();

    static std::optional<WSRelativeDisplayRect> MatchBestIntersectionScreen(
        const std::unordered_map<uint64_t, WSRect>& screenRectMap, const WSRect& globalRect);

    static WSRelativeDisplayRect FallbackToOriginalOrPrimaryScreen(
        const std::unordered_map<uint64_t, WSRect>& screenRectMap, const WSRect& globalRect,
        DisplayId originalDisplayId);
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SESSION_COORDINATE_HELPER_H
