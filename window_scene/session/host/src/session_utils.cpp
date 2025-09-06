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

#include "session_utils.h"

namespace OHOS::Rosen {
namespace SessionUtils {
bool AdjustRectByAspectRatio(WSRect& rect,
                             float aspectRatio,
                             WindowLimits limits,
                             const WindowDecoration& decoration,
                             const int tolerancePx)
{
    if (MathHelper::NearZero(aspectRatio)) {
        // Aspect ratio is zero, skip adjustment
        return false;
    }

    const uint32_t decorW = decoration.Horizontal();
    const uint32_t decorH = decoration.Vertical();

    // Trim limits by decoration
    limits.Trim(decorW, decorH);
    if (!limits.IsValid()) {
        return false;
    }

    // Width-prioritized aspect ratio adjustment
    uint32_t minW = std::max(limits.minWidth_, static_cast<uint32_t>(std::ceil(limits.minHeight_ * aspectRatio)));
    uint32_t maxW = std::min(limits.maxWidth_, static_cast<uint32_t>(std::floor(limits.maxHeight_ * aspectRatio)));
    if (minW > maxW) {
        // Aspect ratio conflicts with limits
        return false;
    }

    // Available rect (excluding decoration)
    uint32_t availW = static_cast<uint32_t>(rect.width_) - decorW;
    uint32_t targetW = std::clamp(availW, minW, maxW);
    uint32_t targetH = static_cast<uint32_t>(std::round(static_cast<float>(targetW) / aspectRatio));
    targetH = std::clamp(targetH, limits.minHeight_, limits.maxHeight_);

    WSRect adjustedRect = rect;
    // Add back decorations
    adjustedRect.width_ = static_cast<int32_t>(targetW + decorW);
    adjustedRect.height_ = static_cast<int32_t>(targetH + decorH);

    // Tolerance: <= tolerancePx pixels is considered no adjustment
    if (std::abs(rect.width_ - adjustedRect.width_) <= tolerancePx &&
        std::abs(rect.height_ - adjustedRect.height_) <= tolerancePx) {
        return false;
    }
    rect = adjustedRect;
    return true;
}
} // namespace SessionUtils
} // namespace OHOS::Rosen
