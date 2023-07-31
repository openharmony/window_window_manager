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

#ifndef OHOS_ROSEN_SESSION_HELPER_H
#define OHOS_ROSEN_SESSION_HELPER_H

#include <string>
#include "ws_common.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class SessionHelper {
public:
    static WSRect GetOverlap(const WSRect& rect1, const WSRect& rect2, int offsetX, int offsetY)
    {
        int32_t x_begin = std::max(rect1.posX_, rect2.posX_);
        int32_t x_end = std::min(rect1.posX_ + static_cast<int32_t>(rect1.width_),
            rect2.posX_ + static_cast<int32_t>(rect2.width_));
        int32_t y_begin = std::max(rect1.posY_, rect2.posY_);
        int32_t y_end = std::min(rect1.posY_ + static_cast<int32_t>(rect1.height_),
            rect2.posY_ + static_cast<int32_t>(rect2.height_));
        if (y_begin >= y_end || x_begin >= x_end) {
            return { 0, 0, 0, 0 };
        }
        return { x_begin - offsetX, y_begin - offsetY,
            static_cast<uint32_t>(x_end - x_begin), static_cast<uint32_t>(y_end - y_begin) };
    }

    static inline bool IsEmptyRect(const WSRect& r)
    {
        return (r.posX_ == 0 && r.posY_ == 0 && r.width_ == 0 && r.height_ == 0);
    }

    static bool IsPointInRect(int32_t pointPosX, int32_t pointPosY, const Rect& rect)
    {
        if ((pointPosX > rect.posX_) &&
            (pointPosX < (rect.posX_ + static_cast<int32_t>(rect.width_)) - 1) &&
            (pointPosY > rect.posY_) &&
            (pointPosY < (rect.posY_ + static_cast<int32_t>(rect.height_)) - 1)) {
            return true;
        }
        return false;
    }

    static inline WSRect TransferToWSRect(const Rect& rect)
    {
        WSRect r;
        r.height_ = rect.height_;
        r.width_ = rect.width_;
        r.posX_ = rect.posX_;
        r.posY_ = rect.posY_;
        return r;
    }

    static inline Rect TransferToRect(const WSRect& rect)
    {
        Rect r;
        r.height_ = rect.height_;
        r.width_ = rect.width_;
        r.posX_ = rect.posX_;
        r.posY_ = rect.posY_;
        return r;
    }
};
} // Rosen
} // OHOS
#endif // OHOS_ROSEN_SESSION_HELPER_H