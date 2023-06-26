/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "screen_cutout_controller.h"

#include "screen_scene_config.h"
#include "screen_session_manager.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr std::vector<int>::size_type LEFT = 0;
constexpr std::vector<int>::size_type TOP = 1;
constexpr std::vector<int>::size_type RIGHT = 2;
constexpr std::vector<int>::size_type BOTTOM = 3;
constexpr char CURVED_SCREEN_BOUNDARY[] = "curvedScreenBoundary";
constexpr uint32_t HALF_SCREEN = 2;
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ScreenCutoutController" };
}

sptr<CutoutInfo> ScreenCutoutController::GetScreenCutoutInfo()
{
    WLOGFD("get screen cutout info.");
    std::vector<DMRect> boundaryRects;
    if (!ScreenSceneConfig::GetCutoutBoundaryRect().empty()) {
        ConvertBoundaryRectsByRotation(boundaryRects);
    }

    CalcWaterfallRects();
    sptr<CutoutInfo> cutoutInfo = new CutoutInfo(boundaryRects, waterfallDisplayAreaRects_);
    return cutoutInfo;
}

void ScreenCutoutController::ConvertBoundaryRectsByRotation(std::vector<DMRect>& boundaryRects)
{
    std::vector<DMRect> finalVector;
    sptr<DisplayInfo> displayInfo = ScreenSessionManager::GetInstance().GetDefaultDisplayInfo();
    if (!displayInfo) {
        WLOGFE("displayInfo invaild");
        boundaryRects = finalVector;
        return;
    }

    Rotation currentRotation = displayInfo->GetRotation();
    std::vector<DMRect> displayBoundaryRects = ScreenSceneConfig::GetCutoutBoundaryRect();
    CheckBoundaryRects(displayBoundaryRects, displayInfo);
    if (currentRotation == Rotation::ROTATION_0) {
        boundaryRects = displayBoundaryRects;
        return;
    }

    uint32_t displayWidth = displayInfo->GetWidth();
    uint32_t displayHeight = displayInfo->GetHeight();
    switch (currentRotation) {
        case Rotation::ROTATION_90: {
            for (DMRect rect : displayBoundaryRects) {
                finalVector.emplace_back(DMRect{
                    .posX_ = displayHeight - rect.posY_ - rect.height_,
                    .posY_ = rect.posX_,
                    .width_ = rect.height_,
                    .height_ = rect.width_ });
            }
            break;
        }
        case Rotation::ROTATION_180: {
            for (DMRect rect : displayBoundaryRects) {
                finalVector.emplace_back(DMRect {displayWidth - rect.posX_ - rect.width_,
                    displayHeight - rect.posY_ - rect.height_, rect.width_, rect.height_});
            }
            break;
        }
        case Rotation::ROTATION_270: {
            for (DMRect rect : displayBoundaryRects) {
                finalVector.emplace_back(
                    DMRect{ rect.posY_, displayWidth - rect.posX_ - rect.width_, rect.height_, rect.width_});
            }
            break;
        }
        default:
            break;
    }
    boundaryRects = finalVector;
}

void ScreenCutoutController::CheckBoundaryRects(std::vector<DMRect>& boundaryRects, sptr<DisplayInfo> displayInfo)
{
    if (!displayInfo) {
        WLOGFE("displayInfo invaild");
        return;
    }

    uint32_t displayWidth = displayInfo->GetWidth();
    uint32_t displayHeight = displayInfo->GetHeight();
    for (auto iter = boundaryRects.begin(); iter != boundaryRects.end();) {
        DMRect boundaryRect = *iter;
        if (boundaryRect.posX_ < 0 || boundaryRect.posY_ < 0 ||
            static_cast<int32_t>(boundaryRect.width_) + boundaryRect.posX_ > static_cast<int32_t>(displayWidth) ||
            static_cast<int32_t>(boundaryRect.height_) + boundaryRect.posY_ > static_cast<int32_t>(displayHeight) ||
            boundaryRect.width_ > displayWidth || boundaryRect.height_ > displayHeight ||
            boundaryRect.IsUninitializedRect()) {
            WLOGFE("boundaryRect boundary is invalid");
            iter = boundaryRects.erase(iter);
        } else {
            iter++;
        }
    }
}

void ScreenCutoutController::CalcWaterfallRects()
{
    WaterfallDisplayAreaRects emptyRects = {};
    if (!ScreenSceneConfig::IsWaterfallDisplay()) {
        WLOGFE("not waterfall display");
        waterfallDisplayAreaRects_ = emptyRects;
        return;
    }

    auto initConfig = ScreenSceneConfig::GetIntNumbersConfig();
    std::vector<int> numberVec = initConfig[CURVED_SCREEN_BOUNDARY];
    if (numberVec.empty()) {
        WLOGFI("curved screen boundary is empty");
        waterfallDisplayAreaRects_ = emptyRects;
        return;
    }

    std::vector<uint32_t> realNumVec = { 0, 0, 0, 0 };
    for (int i = LEFT; i <= BOTTOM; i++) {
        if (numberVec.size() > i) {
            realNumVec[i] = numberVec[i];
        }
    }
    if (std::all_of(realNumVec.begin(), realNumVec.end(), [](uint32_t result) { return result == 0; })) {
        waterfallDisplayAreaRects_ = emptyRects;
        return;
    }

    sptr<DisplayInfo> displayInfo = ScreenSessionManager::GetInstance().GetDefaultDisplayInfo();
    if (!displayInfo) {
        WLOGFE("displayInfo invaild");
        return;
    }

    uint32_t displayWidth = displayInfo->GetWidth();
    uint32_t displayHeight = displayInfo->GetHeight();
    if ((realNumVec[LEFT] > displayWidth / HALF_SCREEN) || (realNumVec[RIGHT] > displayWidth / HALF_SCREEN) ||
        (realNumVec[TOP] > displayHeight / HALF_SCREEN) || (realNumVec[BOTTOM] > displayHeight / HALF_SCREEN)) {
        WLOGFE("curved screen boundary data is not correct");
        waterfallDisplayAreaRects_ = emptyRects;
        return;
    }

    CalcWaterfallRectsByRotation(
        ScreenSessionManager::GetInstance().GetDefaultDisplayInfo()->GetRotation(), displayHeight,
        displayWidth, realNumVec);
}

void ScreenCutoutController::CalcWaterfallRectsByRotation(Rotation rotation, uint32_t displayHeight,
    uint32_t displayWidth, std::vector<uint32_t> realNumVec)
{
    switch (rotation) {
        case Rotation::ROTATION_0: {
            DMRect leftRect = CreateWaterfallRect(0, 0, realNumVec[LEFT], displayHeight);
            DMRect topRect = CreateWaterfallRect(0, 0, displayWidth, realNumVec[TOP]);
            DMRect rightRect =
                CreateWaterfallRect(displayWidth - realNumVec[RIGHT], 0, realNumVec[RIGHT], displayHeight);
            DMRect bottomRect =
                CreateWaterfallRect(0, displayHeight - realNumVec[BOTTOM], displayWidth, realNumVec[BOTTOM]);
            waterfallDisplayAreaRects_ = WaterfallDisplayAreaRects {leftRect, topRect, rightRect, bottomRect};
            return;
        }
        case Rotation::ROTATION_90: {
            DMRect leftRect = CreateWaterfallRect(0, 0, realNumVec[BOTTOM], displayWidth);
            DMRect topRect = CreateWaterfallRect(0, 0, displayHeight, realNumVec[LEFT]);
            DMRect rightRect = CreateWaterfallRect(displayHeight - realNumVec[TOP], 0, realNumVec[TOP], displayWidth);
            DMRect bottomRect =
                CreateWaterfallRect(0, displayWidth - realNumVec[RIGHT], displayHeight, realNumVec[RIGHT]);
            waterfallDisplayAreaRects_ = WaterfallDisplayAreaRects {leftRect, topRect, rightRect, bottomRect};
            return;
        }
        case Rotation::ROTATION_180: {
            DMRect leftRect = CreateWaterfallRect(0, 0, realNumVec[RIGHT], displayHeight);
            DMRect topRect = CreateWaterfallRect(0, 0, realNumVec[BOTTOM], displayWidth);
            DMRect rightRect = CreateWaterfallRect(displayWidth - realNumVec[LEFT], 0, realNumVec[LEFT], displayHeight);
            DMRect bottomRect = CreateWaterfallRect(0, displayHeight - realNumVec[TOP], displayWidth, realNumVec[TOP]);
            waterfallDisplayAreaRects_ = WaterfallDisplayAreaRects {leftRect, topRect, rightRect, bottomRect};
            return;
        }
        case Rotation::ROTATION_270: {
            DMRect leftRect = CreateWaterfallRect(0, 0, realNumVec[TOP], displayWidth);
            DMRect topRect = CreateWaterfallRect(0, 0, displayHeight, realNumVec[RIGHT]);
            DMRect rightRect =
                CreateWaterfallRect(displayHeight - realNumVec[BOTTOM], 0, realNumVec[BOTTOM], displayWidth);
            DMRect bottomRect =
                CreateWaterfallRect(0, displayWidth - realNumVec[LEFT], displayHeight, realNumVec[LEFT]);
            waterfallDisplayAreaRects_ = WaterfallDisplayAreaRects {leftRect, topRect, rightRect, bottomRect};
            return;
        }
        default: {
        }
    }
}

DMRect ScreenCutoutController::CreateWaterfallRect(uint32_t left, uint32_t top, uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0) {
        return DMRect {0, 0, 0, 0};
    }
    return DMRect {left, top, width, height};
}
} // namespace OHOS::Rosen
