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

#ifndef OHOS_ROSEN_WINDOW_COORDINATE_HELPER_H
#define OHOS_ROSEN_WINDOW_COORDINATE_HELPER_H

#include "wm_common.h"

namespace OHOS {
namespace Rosen {
/**
 * @class WindowCoordinateHelper
 *
 * @brief Utility class for coordinate transformations between window-local and external coordinate spaces.
 *
 * This helper class provides a set of static methods to convert positions and rectangles between:
 * - Window-local coordinates: positions relative to the window's local origin (top-left corner)
 * - External coordinates: positions relative to an external reference space (e.g. screen space, parent space)
 *
 * The class supports both:
 * - Direct translation (without transformation)
 * - Scaling transformation (via the Transform object)
 *
 * The Transform object defines:
 * - pivotX / pivotY: relative scaling center within the rectangle ([0.0 ~ 1.0])
 * - scaleX / scaleY: scaling factors along X and Y axes
 */
class WindowCoordinateHelper {
public:
    /**
     * Compute the scaled rectangle after applying the Transform.
     *
     * This function applies scaling to the given rectangle based on the specified pivot point.
     * The pivot defines the relative scaling center within the rectangle that remains stationary during scaling.
     *
     * @param rect The original rectangle in external coordinates.
     * @param transform The transformation parameters, including scaling factors and pivot (scaling center).
     * @return The scaled rectangle (absolute coordinates after transformation).
     */
    static Rect ComputeScaledRect(const Rect& rect, const Transform& transform);

    /**
     * Convert a position from window-local coordinates to external coordinates without transformation.
     *
     * This function transforms a position defined relative to the window's local origin (top-left corner)
     * directly into external coordinates, assuming no window scaling transformation.
     *
     * @param window The window rectangle in external coordinates.
     * @param localPos The position relative to the window's local origin.
     * @return The position in external coordinates.
     */
    static Position LocalToExternal(const Rect& window, const Position& localPos);

    /**
     * Convert a position from window-local coordinates to external coordinates.
     *
     * This function transforms a position defined relative to the window's local origin (top-left corner)
     * into external coordinates, based on the applied window transformation (scaling).
     *
     * @param window The window rectangle in external coordinates.
     * @param transform The transformation applied to the window, including scaling and pivot.
     * @param localPos The position relative to the window's local origin.
     * @return The position in external coordinates.
     */
    static Position LocalToExternal(const Rect& window, const Transform& transform, const Position& localPos);

    /**
     * Convert a position from external coordinates to window-local coordinates without transformation.
     *
     * This function converts a position defined in external coordinates into the window's local
     * coordinates, assuming no window scaling transformation.
     *
     * @param window The window rectangle in external coordinates.
     * @param externalPos The position in external coordinates.
     * @return The position in window-local coordinates.
     */
    static Position ExternalToLocal(const Rect& window, const Position& externalPos);

    /**
     * Convert a position from external coordinates to window-local coordinates.
     *
     * This function transforms a position defined in external coordinates into the window's local
     * coordinates. The transformation is performed based on the window's applied Transform, which
     * includes scaling and pivot (scaling center).
     *
     * @param window The window rectangle in external coordinates.
     * @param transform The transformation applied to the window, including scaling and pivot.
     * @param externalPos The position in external coordinates.
     * @return The position in window-local coordinates.
     */
    static Position ExternalToLocal(const Rect& window, const Transform& transform, const Position& externalPos);
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_WINDOW_COORDINATE_HELPER_H
