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

#ifndef OHOS_ROSEN_WINDOW_PIP_UTIL_H
#define OHOS_ROSEN_WINDOW_PIP_UTIL_H

#include "wm_common.h"

namespace OHOS::Rosen {
class PiPUtil {
public:
    static void SetDisplayVpr(const float displayVpr);
    static void UpdateRectPivot(const int32_t start, const uint32_t len, const uint32_t totalLen,
        PiPScalePivot& pivot);
    static void GetRectByPivot(int32_t& start, const uint32_t oldLen, const uint32_t len, const uint32_t totalLen,
        const PiPScalePivot& pivot);
    static void GetRectByScale(const uint32_t width, const uint32_t height, const PiPScaleLevel& scaleLevel,
        Rect& rect, bool isLandscape);
    static bool GetValidRect(const int32_t width, const int32_t height, Rect& rect);
    static void CalcWinRectLand(Rect& rect, const uint32_t width, const uint32_t height, const uint32_t winWidth,
        const uint32_t winHeight);
    static constexpr int32_t SAFE_PADDING_HORIZONTAL_VP = 12;
    static constexpr int32_t SAFE_PADDING_VERTICAL_TOP = 150;
    static constexpr int32_t SAFE_PADDING_VERTICAL_BOTTOM = 150;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_PIP_UTIL_H