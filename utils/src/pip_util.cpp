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

#include "pip_util.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr float DEFAULT_PROPORTION = 0.3;
    constexpr int32_t NUMBER_TWO = 2;
    constexpr int32_t NUMBER_THREE = 3;
    constexpr int32_t NUMBER_FOUR = 4;
    constexpr int32_t NUMBER_SEVEN = 7;
    float g_vpr = 1.0f;
}

void PiPUtil::SetDisplayVpr(const float displayVpr)
{
    g_vpr = displayVpr;
}

void PiPUtil::UpdateRectPivot(const int32_t start, const uint32_t len, const uint32_t totalLen, PiPScalePivot& pivot)
{
    int32_t end = static_cast<int32_t>(totalLen) - start - static_cast<int32_t>(len);
    if (start > end) {
        pivot = PiPScalePivot::END;
    } else if (start < end) {
        pivot = PiPScalePivot::START;
    } else {
        pivot = PiPScalePivot::MIDDLE;
    }
}

void PiPUtil::GetRectByPivot(int32_t& start, const uint32_t oldLen, const uint32_t len, const uint32_t totalLen,
    const PiPScalePivot& pivot)
{
    switch (pivot) {
        default:
        case PiPScalePivot::START:
            break;
        case PiPScalePivot::MIDDLE:
            start = (static_cast<int32_t>(totalLen) - static_cast<int32_t>(len)) / NUMBER_TWO;
            break;
        case PiPScalePivot::END:
            start = start - static_cast<int32_t>(len) + static_cast<int32_t>(oldLen);
            break;
    }
}

void PiPUtil::CalcWinRectLand(Rect& rect, const uint32_t width, const uint32_t height, const uint32_t winWidth,
    const uint32_t winHeight)
{
    int32_t heightTmp = static_cast<int32_t>(height) - SAFE_PADDING_VERTICAL_TOP - SAFE_PADDING_VERTICAL_BOTTOM;
    int32_t safePaddingHorizontal = static_cast<int32_t>(SAFE_PADDING_HORIZONTAL_VP * g_vpr);
    if (winWidth <= winHeight) {
        int32_t widthTmp = (NUMBER_THREE * static_cast<int32_t>(width)
            - NUMBER_SEVEN * safePaddingHorizontal) / NUMBER_FOUR;
        rect.width_ = static_cast<uint32_t>(widthTmp);
        rect.height_ = rect.width_ * winHeight / winWidth;
        if (rect.height_ > static_cast<uint32_t>(heightTmp)) {
            rect.height_ = static_cast<uint32_t>(heightTmp);
            rect.width_ = rect.height_ * winWidth / winHeight;
        }
    } else {
        rect.height_ = static_cast<uint32_t>(heightTmp);
        rect.width_ = rect.height_ * winWidth / winHeight;
    }
}

void PiPUtil::GetRectByScale(const uint32_t width, const uint32_t height, const PiPScaleLevel& scaleLevel,
    Rect& rect, bool isLandscape)
{
    uint32_t winWidth = rect.width_;
    uint32_t winHeight = rect.height_;
    if (winWidth == 0 || winHeight == 0) {
        return;
    }
    int32_t safePaddingHorizontal = static_cast<int32_t>(SAFE_PADDING_HORIZONTAL_VP * g_vpr);
    switch (scaleLevel) {
        default:
        case PiPScaleLevel::PIP_SCALE_LEVEL_SMALLEST: {
            float shortBorder = static_cast<float>(width < height ? width : height) * DEFAULT_PROPORTION;
            if (winWidth < winHeight) {
                rect.width_ = static_cast<uint32_t>(shortBorder);
                rect.height_ = rect.width_ * winHeight / winWidth;
            } else {
                rect.height_ = static_cast<uint32_t>(shortBorder);
                rect.width_ = rect.height_ * winWidth / winHeight;
            }
            break;
        }
        case PiPScaleLevel::PIP_SCALE_LEVEL_BIGGEST: {
            if (isLandscape) {
                CalcWinRectLand(rect, width, height, winWidth, winHeight);
            } else {
                int32_t widthTmp = 0;
                if (winWidth < winHeight) {
                    widthTmp = (NUMBER_THREE * static_cast<int32_t>(width) -
                        NUMBER_SEVEN * safePaddingHorizontal) / NUMBER_FOUR;
                } else {
                    widthTmp = static_cast<int32_t>(width) - NUMBER_TWO * safePaddingHorizontal;
                }
                rect.width_ = static_cast<uint32_t>(widthTmp);
                rect.height_ = rect.width_ * winHeight / winWidth;
            }
            break;
        }
    }
}

bool PiPUtil::GetValidRect(const uint32_t width, const uint32_t height, Rect& rect)
{
    int32_t safePaddingHorizontal = static_cast<int32_t>(SAFE_PADDING_HORIZONTAL_VP * g_vpr);
    bool hasChanged = false;
    if (rect.posX_ < safePaddingHorizontal) {
        rect.posX_ = safePaddingHorizontal;
        hasChanged = true;
    } else if ((rect.posX_ + rect.width_) > (width - safePaddingHorizontal)) {
        rect.posX_ = width - safePaddingHorizontal - rect.width_;
        hasChanged = true;
    }
    if (rect.posY_ < SAFE_PADDING_VERTICAL_TOP) {
        rect.posY_ = SAFE_PADDING_VERTICAL_TOP;
        hasChanged = true;
    } else if ((rect.posY_ + rect.height_) > (height - SAFE_PADDING_VERTICAL_BOTTOM)) {
        rect.posY_ = height - SAFE_PADDING_VERTICAL_BOTTOM - rect.height_;
        hasChanged = true;
    }
    return hasChanged;
}
} // namespace Rosen
} // namespace OHOS