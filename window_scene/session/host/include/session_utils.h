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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_UTILS_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_UTILS_H

#include "wm_common_inner.h"
#include "ws_common.h"

namespace OHOS::Rosen {
namespace SessionUtils {
constexpr const char* SESSION_NAME_MARK_HEAD = "#";
constexpr const char* SESSION_NAME_SEPARATOR = ":";

inline float ToLayoutWidth(const int32_t winWidth, float vpr)
{
    return winWidth - 2 * WINDOW_FRAME_WIDTH * vpr; // 2: left and right edge
}

inline float ToLayoutHeight(const int32_t winHeight, float vpr)
{
    return winHeight - (WINDOW_FRAME_WIDTH + WINDOW_TITLE_BAR_HEIGHT) * vpr;
}

inline float ToWinWidth(const int32_t layoutWidth, float vpr)
{
    return layoutWidth + 2 * WINDOW_FRAME_WIDTH * vpr; // 2: left and right edge
}

inline float ToWinHeight(const int32_t layoutHeight, float vpr)
{
    return layoutHeight + (WINDOW_FRAME_WIDTH + WINDOW_TITLE_BAR_HEIGHT) * vpr;
}

inline void CalcFloatWindowRectLimits(const WindowLimits& limits, uint32_t maxFloatingWindowSize, float vpr,
    int32_t& minWidth, int32_t& maxWidth, int32_t& minHeight, int32_t& maxHeight)
{
    minWidth = limits.minWidth_;
    maxWidth = (limits.maxWidth_ == 0 || limits.maxWidth_ >= INT32_MAX) ? INT32_MAX : limits.maxWidth_;
    minHeight = limits.minHeight_;
    maxHeight = (limits.maxHeight_ == 0 || limits.maxHeight_ >= INT32_MAX) ? INT32_MAX : limits.maxHeight_;
    minWidth = std::max(minWidth, static_cast<int32_t>(MIN_FLOATING_WIDTH * vpr));
    maxWidth = std::min(maxWidth, static_cast<int32_t>(maxFloatingWindowSize * vpr));
    minHeight = std::max(minHeight, static_cast<int32_t>(MIN_FLOATING_HEIGHT * vpr));
    maxHeight = std::min(maxHeight, static_cast<int32_t>(maxFloatingWindowSize * vpr));
}

inline std::string ConvertSessionName(const std::string& bundleName, const std::string& name,
    const std::string& moduleName, const int32_t appIndex = 0)
{
    std::string strName;
    if (appIndex == 0) {
        strName = SESSION_NAME_MARK_HEAD + bundleName + SESSION_NAME_SEPARATOR + moduleName +
            SESSION_NAME_SEPARATOR + name;
    } else {
        strName = SESSION_NAME_MARK_HEAD + bundleName + SESSION_NAME_SEPARATOR + std::to_string(appIndex) +
            SESSION_NAME_SEPARATOR + moduleName + SESSION_NAME_SEPARATOR + name;
    }

    return strName;
}

inline std::string GetBundleNameBySessionName(const std::string& name)
{
    const size_t len = std::char_traits<char>::length(SESSION_NAME_MARK_HEAD);
    if (name.empty() || name.size() < len || name.compare(0, len, SESSION_NAME_MARK_HEAD) != 0) {
        return "";
    }
    size_t pos = name.find(SESSION_NAME_SEPARATOR);
    return pos != std::string::npos? name.substr(len, pos - 1) : name.substr(len);
}

inline std::string GetAppLockKey(const std::string& bundleName, const int32_t appIndex)
{
    return bundleName + "#" + std::to_string(appIndex);
}

/**
 * Adjust the given rect to respect aspect ratio and window limits.
 * Width is prioritized when maintaining aspect ratio.
 *
 * Note: If aspectRatio is zero, the function returns immediately without modifying rect.
 *
 * @param rect        [in/out] Rectangle to adjust; updated if adjustment occurs
 * @param aspectRatio Desired width/height ratio
 * @param limits      Minimum and maximum width/height constraints
 * @param decoration  Window decoration (borders, title bar, etc.)
 * @param tolerancePx Pixel tolerance to ignore minor adjustments (default 2 pixels)
 * @return true if rect was adjusted, false if no adjustment was needed
 */
bool AdjustRectByAspectRatio(WSRect& rect,
                             float aspectRatio,
                             WindowLimits limits,
                             const WindowDecoration& decoration,
                             const int tolerancePx = 2)
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

    WSRect originalRect = rect;
    // Available rect (excluding decoration)
    uint32_t availW = static_cast<uint32_t>(rect.width_) - decorW;
    uint32_t targetW = std::clamp(availW, minW, maxW);
    uint32_t targetH = static_cast<uint32_t>(std::round(static_cast<float>(targetW) / aspectRatio));
    targetH = std::clamp(targetH, limits.minHeight_, limits.maxHeight_);

    // Add back decorations
    rect.width_ = static_cast<int32_t>(targetW + decorW);
    rect.height_ = static_cast<int32_t>(targetH + decorH);

    // Tolerance: <= tolerancePx pixels is considered no adjustment
    if (std::abs(originalRect.width_ - rect.width_) <= tolerancePx &&
        std::abs(originalRect.height_ - rect.height_) <= tolerancePx) {
        rect = originalRect;
        return false;
    }
    return true;
}
} // namespace SessionUtils
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_UTILS_H
