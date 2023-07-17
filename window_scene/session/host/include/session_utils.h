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

namespace OHOS::Rosen {
namespace SessionUtils {

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

inline void CalcFloatWindowRectLimits(const WindowLimits& limits, int32_t maxFloatingWindowSize, float vpr,
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

} // namespace SessionUtils
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_UTILS_H
