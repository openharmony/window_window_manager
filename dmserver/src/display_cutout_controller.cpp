/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "display_cutout_controller.h"
#include <screen_manager/screen_types.h>
#include "display_manager_service_inner.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayCutoutController"};
}

void DisplayCutoutController::SetBuiltInDisplayCutoutSvgPath(const std::string& svgPath)
{
    SetCutoutSvgPath(0, svgPath);
}

void DisplayCutoutController::SetIsWaterfallDisplay(bool isWaterfallDisplay)
{
    WLOGFI("Set isWaterfallDisplay: %{public}u", isWaterfallDisplay);
    isWaterfallDisplay_ = isWaterfallDisplay;
}

void DisplayCutoutController::SetCurvedScreenBoundary(std::vector<int> curvedScreenBoundary)
{
    WLOGFI("Set curvedScreenBoundary");
    curvedScreenBoundary_ = curvedScreenBoundary;
}

void DisplayCutoutController::SetWaterfallAreaLayoutEnable(bool isEnable)
{
    WLOGFI("Set waterfall area layout enable: %{public}u", isEnable);
    isWaterfallAreaLayoutEnable_ = isEnable;
}

void DisplayCutoutController::SetCutoutSvgPath(DisplayId displayId, const std::string& svgPath)
{
    WLOGFI("Set SvgPath: %{public}s", svgPath.c_str());
    if (svgPaths_.count(displayId) == 1) {
        svgPaths_[displayId].emplace_back(svgPath);
    } else {
        std::vector<std::string> pathVec;
        pathVec.emplace_back(svgPath);
        svgPaths_[displayId] = pathVec;
    }
    Rect boundingRect = CalcCutoutBoundingRect(svgPath);
    if (boundingRects_.count(displayId) == 1) {
        boundingRects_[displayId].emplace_back(boundingRect);
    } else {
        std::vector<Rect> rectVec;
        rectVec.emplace_back(boundingRect);
        boundingRects_[displayId] = rectVec;
    }
}

sptr<CutoutInfo> DisplayCutoutController::GetCutoutInfo(DisplayId displayId)
{
    WLOGFI("Get Cutout Info");
    std::vector<Rect> boundingRects;
    WaterfallDisplayAreaRects waterfallDisplayAreaRects;
    if (boundingRects_.count(displayId) == 1) {
        TransferBoundingRectsByRotation(displayId, boundingRects);
    }
    if (displayId == DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId()) {
        CalcBuiltInDisplayWaterfallRects();
        waterfallDisplayAreaRects = waterfallDisplayAreaRects_;
    }
    CutoutInfo *cutoutInfo = new CutoutInfo(boundingRects, waterfallDisplayAreaRects);
    return cutoutInfo;
}

Rect DisplayCutoutController::CalcCutoutBoundingRect(std::string svgPath)
{
    Rect emptyRect = {0, 0, 0, 0};
    SkPath skCutoutSvgPath;
    if (!SkParsePath::FromSVGString(svgPath.c_str(), &skCutoutSvgPath)) {
        WLOGFE("Parse svg string path failed.");
        return emptyRect;
    }
    SkRect skRect = skCutoutSvgPath.computeTightBounds();
    if (skRect.isEmpty()) {
        WLOGFI("Get empty skRect");
        return emptyRect;
    }
    SkIRect skiRect = skRect.roundOut();
    if (skiRect.isEmpty()) {
        WLOGFI("Get empty skiRect");
        return emptyRect;
    }
    int32_t left = static_cast<int32_t>(skiRect.left());
    int32_t top = static_cast<int32_t>(skiRect.top());
    uint32_t width = static_cast<uint32_t>(skiRect.width());
    uint32_t height = static_cast<uint32_t>(skiRect.height());
    Rect cutoutMinOuterRect = {.posX_ = left, .posY_ = top, .width_ = width, .height_ = height};
    return cutoutMinOuterRect;
}

void DisplayCutoutController::CalcBuiltInDisplayWaterfallRects()
{
    WaterfallDisplayAreaRects emptyRects = {};
    if (!isWaterfallDisplay_) {
        WLOGFI("not waterfall display");
        waterfallDisplayAreaRects_ = emptyRects;
        return;
    }
    if (curvedScreenBoundary_.empty()) {
        WLOGFI("curved screen boundary is empty");
        waterfallDisplayAreaRects_ = emptyRects;
        return;
    }
    uint32_t left = curvedScreenBoundary_[0];
    uint32_t top = curvedScreenBoundary_[1];
    uint32_t right = curvedScreenBoundary_[2];
    uint32_t bottom = curvedScreenBoundary_[3];

    sptr<SupportedScreenModes> modes =
        DisplayManagerServiceInner::GetInstance().GetScreenModesByDisplayId(
            DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId());
    uint32_t displayHeight = modes->height_;
    uint32_t displayWidth = modes->width_;

    if ((left > displayWidth / 2) || (right > displayWidth / 2) || // invalid if more than 1/2 width
        (top > displayHeight / 2) || (bottom > displayHeight / 2)) { // invalid if more than 1/2 height
        WLOGFE("Curved screen boundary data is not valid.");
        waterfallDisplayAreaRects_ = emptyRects;
        return;
    }
    CalcBuiltInDisplayWaterfallRectsByRotation(
        DisplayManagerServiceInner::GetInstance().GetDisplayById(0)->GetRotation(),
        displayHeight, displayWidth);
}

void DisplayCutoutController::CalcBuiltInDisplayWaterfallRectsByRotation(
    Rotation rotation, uint32_t displayHeight, uint32_t displayWidth)
{
    uint32_t left = curvedScreenBoundary_[0];
    uint32_t top = curvedScreenBoundary_[1];
    uint32_t right = curvedScreenBoundary_[2];
    uint32_t bottom = curvedScreenBoundary_[3];
    switch (rotation) {
        case Rotation::ROTATION_0: {
            Rect leftRect = {0, 0, left, displayHeight};
            Rect topRect = {0, 0, displayWidth, top};
            Rect rightRect = {displayWidth - right, 0, right, displayHeight};
            Rect bottomRect = {0, displayHeight - bottom, displayWidth, bottom};
            waterfallDisplayAreaRects_ = WaterfallDisplayAreaRects {leftRect, topRect, rightRect, bottomRect};
            return;
        }
        case Rotation::ROTATION_90: {
            Rect leftRect = {0, 0, bottom, displayWidth};
            Rect topRect = {0, 0, displayHeight, left};
            Rect rightRect = {displayHeight - top, 0, top, displayWidth};
            Rect bottomRect = {0, displayWidth - right, displayHeight, right};
            waterfallDisplayAreaRects_ = WaterfallDisplayAreaRects {leftRect, topRect, rightRect, bottomRect};
            return;
        }
        case Rotation::ROTATION_180: {
            Rect leftRect = {0, 0, right, displayHeight};
            Rect topRect = {0, 0, bottom, displayWidth};
            Rect rightRect = {displayWidth - left, 0, left, displayHeight};
            Rect bottomRect = {0, displayHeight - top, displayWidth, top};
            waterfallDisplayAreaRects_ = WaterfallDisplayAreaRects {leftRect, topRect, rightRect, bottomRect};
            return;
        }
        case Rotation::ROTATION_270: {
            Rect leftRect = {0, 0, top, displayWidth};
            Rect topRect = {0, 0, displayHeight, right};
            Rect rightRect = {displayHeight - bottom, 0, bottom, displayWidth};
            Rect bottomRect = {0, displayWidth - left, displayHeight, left};
            waterfallDisplayAreaRects_ = WaterfallDisplayAreaRects {leftRect, topRect, rightRect, bottomRect};
            return;
        }
        default: {
        }
    }
}

void DisplayCutoutController::TransferBoundingRectsByRotation(DisplayId displayId, std::vector<Rect>& boudingRects)
{
    std::vector<Rect> resultVec;
    if (boundingRects_.count(displayId) == 0) {
        boudingRects = resultVec;
        return;
    }
    std::vector<Rect> displayBoundingRects = boundingRects_[displayId];
    if (displayBoundingRects.empty()) {
        boudingRects = resultVec;
        return;
    }
    Rotation currentRotation = DisplayManagerServiceInner::GetInstance().GetDisplayById(displayId)->GetRotation();
    if (currentRotation == Rotation::ROTATION_0) {
        boudingRects = displayBoundingRects;
        return;
    }
    sptr<SupportedScreenModes> modes =
        DisplayManagerServiceInner::GetInstance().GetScreenModesByDisplayId(displayId);
    uint32_t displayHeight = modes->height_;
    uint32_t displayWidth = modes->width_;
    
    switch (currentRotation) {
        case Rotation::ROTATION_90: {
            for (Rect rect : displayBoundingRects) {
                resultVec.emplace_back(Rect {.posX_ = displayHeight - rect.posY_ - rect.height_,
                    .posY_ = rect.posX_, .width_ = rect.height_, .height_ = rect.width_});
            }
            break;
        }
        case Rotation::ROTATION_180: {
            for (Rect rect : displayBoundingRects) {
                resultVec.emplace_back(Rect {displayWidth - rect.posX_ - rect.width_,
                    displayHeight - rect.posY_ - rect.height_, rect.width_, rect.height_});
            }
            break;
        }
        case Rotation::ROTATION_270: {
            for (Rect rect : displayBoundingRects) {
                resultVec.emplace_back(Rect {rect.posY_, displayWidth - rect.posX_ - rect.width_,
                    rect.height_, rect.width_});
            }
            break;
        }
        default: {
        }
    }
    boudingRects = resultVec;
}
} // Rosen
} // OHOS