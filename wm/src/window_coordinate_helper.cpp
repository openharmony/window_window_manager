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

#include "window_coordinate_helper.h"

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
Rect WindowCoordinateHelper::ComputeScaledRect(const Rect& originalRect, const Transform& transform)
{
    // Compute the absolute position of the scaling center (pivot)
    double pivotAbsX = static_cast<double>(originalRect.posX_) + originalRect.width_ * transform.pivotX_;
    double pivotAbsY = static_cast<double>(originalRect.posY_) + originalRect.height_ * transform.pivotY_;
    // Apply scaling to width and height
    double scaledWidth = originalRect.width_ * transform.scaleX_;
    double scaledHeight = originalRect.height_ * transform.scaleY_;
    // Recalculate the top-left corner after scaling
    double scaledPosX = pivotAbsX - scaledWidth * transform.pivotX_;
    double scaledPosY = pivotAbsY - scaledHeight * transform.pivotY_;
    // Prevent negative size
    scaledWidth = std::max(0.0, scaledWidth);
    scaledHeight = std::max(0.0, scaledHeight);
    return {
        static_cast<int32_t>(std::round(scaledPosX)),
        static_cast<int32_t>(std::round(scaledPosY)),
        static_cast<uint32_t>(std::round(scaledWidth)),
        static_cast<uint32_t>(std::round(scaledHeight))
    };
}

Position WindowCoordinateHelper::LocalToExternal(const Rect& windowRect, const Position& localPos)
{
    int32_t externalX = windowRect.posX_ + localPos.x;
    int32_t externalY = windowRect.posY_ + localPos.y;
    return { externalX, externalY };
}

Position WindowCoordinateHelper::LocalToExternal(
    const Rect& originalWindowRect, const Transform& transform, const Position& localPos)
{
    // scaledWindowRect is the actual rect after applying scaling, still in external coordinates
    Rect scaledWindowRect = ComputeScaledRect(originalWindowRect, transform);
    int32_t externalX = scaledWindowRect.posX_ + static_cast<int32_t>(std::round(localPos.x * transform.scaleX_));
    int32_t externalY = scaledWindowRect.posY_ + static_cast<int32_t>(std::round(localPos.y * transform.scaleY_));
    return { externalX, externalY };
}

Position WindowCoordinateHelper::ExternalToLocal(const Rect& windowRect, const Position& externalPos)
{
    int32_t localX = externalPos.x - windowRect.posX_;
    int32_t localY = externalPos.y - windowRect.posY_;
    return { localX, localY };
}

Position WindowCoordinateHelper::ExternalToLocal(
    const Rect& originalWindowRect, const Transform& transform, const Position& externalPos)
{
    constexpr float EPSILON = 1e-6f;
    auto IsZero = [](float value) {
        return std::fabs(value) < EPSILON;
    };
    if (IsZero(transform.scaleX_) || IsZero(transform.scaleY_)) {
        TLOGE(WmsLogTag::WMS_LAYOUT,
            "The scaleX (%{public}.3f) or scaleY (%{public}.3f) is near zero, cannot convert position.",
            transform.scaleX_, transform.scaleY_);
        return { 0, 0 };
    }
    // scaledWindowRect is the actual rect after applying scaling, still in external coordinates
    Rect scaledWindowRect = ComputeScaledRect(originalWindowRect, transform);
    int32_t localX = static_cast<int32_t>((externalPos.x - scaledWindowRect.posX_) / transform.scaleX_);
    int32_t localY = static_cast<int32_t>((externalPos.y - scaledWindowRect.posY_) / transform.scaleY_);
    return { localX, localY };
}
} // namespace Rosen
} // namespace OHOS
