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
#include "ws_common_inner.h"
#include "wm_common.h"
#include "wm_common_inner.h"

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

    static inline bool IsBelowSystemWindow(WindowType type)
    {
        return (type >= WindowType::BELOW_APP_SYSTEM_WINDOW_BASE && type < WindowType::BELOW_APP_SYSTEM_WINDOW_END);
    }

    static inline bool IsAboveSystemWindow(WindowType type)
    {
        return (type >= WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE && type < WindowType::ABOVE_APP_SYSTEM_WINDOW_END);
    }

    static inline bool IsSystemSubWindow(WindowType type)
    {
        return (type >= WindowType::SYSTEM_SUB_WINDOW_BASE && type < WindowType::SYSTEM_SUB_WINDOW_END);
    }

    static inline bool IsSystemWindow(WindowType type)
    {
        return (IsBelowSystemWindow(type) || IsAboveSystemWindow(type) || IsSystemSubWindow(type));
    }

    static inline bool IsMainWindow(WindowType type)
    {
        return (type >= WindowType::APP_MAIN_WINDOW_BASE && type < WindowType::APP_MAIN_WINDOW_END);
    }

    static inline bool IsSubWindow(WindowType type)
    {
        return (type >= WindowType::APP_SUB_WINDOW_BASE && type < WindowType::APP_SUB_WINDOW_END);
    }

    static AreaType GetAreaType(int32_t pointWinX, int32_t pointWinY,
        int32_t sourceType, int outside, float vpr, const WSRect& rect)
    {
        int32_t insideCorner = WINDOW_FRAME_CORNER_WIDTH * vpr;
        int32_t insideEdge = WINDOW_FRAME_WIDTH * vpr;
        int32_t leftOut = -outside;
        int32_t leftIn = insideEdge;
        int32_t leftCorner = insideCorner;
        int32_t rightCorner = rect.width_ - insideCorner;
        int32_t rightIn = rect.width_ - insideEdge;
        int32_t rightOut = rect.width_ + outside;
        int32_t topOut = -outside;
        int32_t topIn = insideEdge;
        int32_t topCorner = insideCorner;
        int32_t bottomCorner = rect.height_ - insideCorner;
        int32_t bottomIn = rect.height_ - insideEdge;
        int32_t bottomOut = rect.height_ + outside;

        auto isInRange = [](int32_t min, int32_t max, int32_t value) { return min <= value && value <= max; };

        AreaType type;
        if (isInRange(leftOut, leftCorner, pointWinX) && isInRange(topOut, topCorner, pointWinY)) {
            type = AreaType::LEFT_TOP;
        } else if (isInRange(rightCorner, rightOut, pointWinX) && isInRange(topOut, topCorner, pointWinY)) {
            type = AreaType::RIGHT_TOP;
        } else if (isInRange(rightCorner, rightOut, pointWinX) && isInRange(bottomCorner, bottomOut, pointWinY)) {
            type = AreaType::RIGHT_BOTTOM;
        } else if (isInRange(leftOut, leftCorner, pointWinX) && isInRange(bottomCorner, bottomOut, pointWinY)) {
            type = AreaType::LEFT_BOTTOM;
        } else if (isInRange(leftOut, leftIn, pointWinX)) {
            type = AreaType::LEFT;
        } else if (isInRange(topOut, topIn, pointWinY)) {
            type = AreaType::TOP;
        } else if (isInRange(rightIn, rightOut, pointWinX)) {
            type = AreaType::RIGHT;
        } else if (isInRange(bottomIn, bottomOut, pointWinY)) {
            type = AreaType::BOTTOM;
        } else {
            type = AreaType::UNDEFINED;
        }
        return type;
    }
};
} // Rosen
} // OHOS
#endif // OHOS_ROSEN_SESSION_HELPER_H