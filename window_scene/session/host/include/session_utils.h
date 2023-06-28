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

inline int32_t ToLayoutWidth(const int32_t winWidth, float vpr)
{
    return winWidth - 2 * WINDOW_FRAME_WIDTH * vpr; // 2: left and right edge
}

inline int32_t ToLayoutHeight(const int32_t winHeight, float vpr)
{
    return winHeight - (WINDOW_FRAME_WIDTH + WINDOW_TITLE_BAR_HEIGHT) * vpr;
}

inline int32_t ToWinWidth(const int32_t layoutWidth, float vpr)
{
    return layoutWidth + 2 * WINDOW_FRAME_WIDTH * vpr; // 2: left and right edge
}

inline int32_t ToWinHeight(const int32_t layoutHeight, float vpr)
{
    return layoutHeight + (WINDOW_FRAME_WIDTH + WINDOW_TITLE_BAR_HEIGHT) * vpr;
}

} // namespace SessionUtils
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_UTILS_H
