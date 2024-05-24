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

#include <functional>
#include "screen_scene_config.h"
#include "screen_session_manager.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr std::vector<int>::size_type LEFT = 0;
constexpr std::vector<int>::size_type TOP = 1;
constexpr std::vector<int>::size_type RIGHT = 2;
constexpr std::vector<int>::size_type BOTTOM = 3;
constexpr uint8_t HALF_SCREEN = 2;
constexpr uint8_t QUARTER_SCREEN = 4;

DMRect TransferRectByRotation90(const DMRect& rect, uint32_t width, uint32_t height)
{
    return DMRect { .posX_ = width - rect.posY_ - rect.height_,
                    .posY_ = rect.posX_,
                    .width_ = rect.height_,
                    .height_ = rect.width_ };
}

DMRect TransferRectByRotation180(const DMRect& rect, uint32_t width, uint32_t height)
{
    return DMRect { width - rect.posX_ - rect.width_,
                    height - rect.posY_ - rect.height_, rect.width_, rect.height_ };
}

DMRect TransferRectByRotation270(const DMRect& rect, uint32_t width, uint32_t height)
{
    return DMRect { rect.posY_, height - rect.posX_ - rect.width_,
                    rect.height_, rect.width_ };
}

using TransferRectByRotationFunc = std::function<DMRect(const DMRect&, uint32_t, uint32_t)>;
TransferRectByRotationFunc SelectTransferRectByRotationFunc(Rotation currentRotation)
{
    TransferRectByRotationFunc func;
    switch (currentRotation) {
        case Rotation::ROTATION_90: {
            func = &TransferRectByRotation90;
            break;
        }
        case Rotation::ROTATION_180: {
            func = &TransferRectByRotation180;
            break;
        }
        case Rotation::ROTATION_270: {
            func = &TransferRectByRotation270;
            break;
        }
        default: {
        }
    }
    return func;
}

}

uint32_t ScreenCutoutController::defaultDeviceRotation_ = 0;
std::map<DeviceRotationValue, Rotation> ScreenCutoutController::deviceToDisplayRotationMap_;

sptr<CutoutInfo> ScreenCutoutController::GetScreenCutoutInfo(DisplayId displayId)
{
    TLOGD(WmsLogTag::DMS, "get screen cutout info.");
    std::vector<DMRect> boundaryRects;
    if (!ScreenSceneConfig::GetCutoutBoundaryRect(displayId).empty()) {
        ConvertBoundaryRectsByRotation(boundaryRects, displayId);
    }

    CalcWaterfallRects(displayId);
    sptr<CutoutInfo> cutoutInfo = new CutoutInfo(boundaryRects, waterfallDisplayAreaRects_);
    return cutoutInfo;
}

std::vector<DMRect> ScreenCutoutController::GetBoundaryRects(sptr<DisplayInfo> displayInfo, DisplayId displayId)
{
    std::vector<DMRect> displayBoundaryRects;
    if (ScreenSessionManager::GetInstance().IsFoldable() &&
        (ScreenSessionManager::GetInstance().GetFoldStatus() == FoldStatus::FOLDED)) {
        displayBoundaryRects = ScreenSceneConfig::GetSubCutoutBoundaryRect();
    } else {
        displayBoundaryRects = ScreenSceneConfig::GetCutoutBoundaryRect(displayId);
    }
    CheckBoundaryRects(displayBoundaryRects, displayInfo);

    return displayBoundaryRects;
}

void ScreenCutoutController::ConvertBoundaryRectsByRotation(std::vector<DMRect>& boundaryRects, DisplayId displayId)
{
    boundaryRects.clear();
    sptr<DisplayInfo> displayInfo = ScreenSessionManager::GetInstance().GetDisplayInfoById(displayId);
    if (!displayInfo) {
        TLOGE(WmsLogTag::DMS, "displayInfo invalid");
        return;
    }

    Rotation currentRotation = displayInfo->GetRotation();
    std::vector<DMRect> displayBoundaryRects = GetBoundaryRects(displayInfo, displayId);
    if (currentRotation == Rotation::ROTATION_0) {
        boundaryRects = std::move(displayBoundaryRects);
        return;
    }

    uint32_t displayWidth = static_cast<uint32_t>(displayInfo->GetWidth());
    uint32_t displayHeight = static_cast<uint32_t>(displayInfo->GetHeight());
    TransferRectByRotationFunc transferFunc = SelectTransferRectByRotationFunc(currentRotation);
    if (!transferFunc) {
        return;
    }

    for (const DMRect& rect : displayBoundaryRects) {
        boundaryRects.emplace_back(transferFunc(rect, displayHeight, displayWidth));
    }
}

void ScreenCutoutController::CheckBoundaryRects(std::vector<DMRect>& boundaryRects, sptr<DisplayInfo> displayInfo)
{
    if (!displayInfo) {
        TLOGE(WmsLogTag::DMS, "displayInfo invalid");
        return;
    }

    uint32_t displayWidth = static_cast<uint32_t>(displayInfo->GetWidth());
    uint32_t displayHeight = static_cast<uint32_t>(displayInfo->GetHeight());
    for (auto iter = boundaryRects.begin(); iter != boundaryRects.end();) {
        DMRect boundaryRect = *iter;
        if (boundaryRect.posX_ < 0 || boundaryRect.posY_ < 0 ||
            static_cast<int32_t>(boundaryRect.width_) + boundaryRect.posX_ > static_cast<int32_t>(displayWidth) ||
            static_cast<int32_t>(boundaryRect.height_) + boundaryRect.posY_ > static_cast<int32_t>(displayHeight) ||
            boundaryRect.width_ > displayWidth || boundaryRect.height_ > displayHeight ||
            boundaryRect.IsUninitializedRect()) {
            TLOGE(WmsLogTag::DMS, "boundaryRect boundary is invalid");
            iter = boundaryRects.erase(iter);
        } else {
            iter++;
        }
    }
}

void ScreenCutoutController::CalcWaterfallRects(DisplayId displayId)
{
    WaterfallDisplayAreaRects emptyRects = {};
    if (!ScreenSceneConfig::IsWaterfallDisplay()) {
        TLOGE(WmsLogTag::DMS, "not waterfall display");
        waterfallDisplayAreaRects_ = emptyRects;
        return;
    }

    std::vector<int> numberVec = ScreenSceneConfig::GetCurvedScreenBoundaryConfig();
    if (numberVec.empty()) {
        TLOGI(WmsLogTag::DMS, "curved screen boundary is empty");
        waterfallDisplayAreaRects_ = emptyRects;
        return;
    }

    std::vector<uint32_t> realNumVec = { 0, 0, 0, 0 };
    for (auto i = LEFT; i <= BOTTOM; i++) {
        if (numberVec.size() > i) {
            realNumVec[i] = static_cast<uint32_t>(numberVec[i]);
        }
    }
    if (std::all_of(realNumVec.begin(), realNumVec.end(), [](uint32_t result) { return result == 0; })) {
        waterfallDisplayAreaRects_ = emptyRects;
        return;
    }

    sptr<DisplayInfo> displayInfo = ScreenSessionManager::GetInstance().GetDisplayInfoById(displayId);
    if (!displayInfo) {
        TLOGE(WmsLogTag::DMS, "displayInfo invalid");
        return;
    }

    uint32_t displayWidth = static_cast<uint32_t>(displayInfo->GetWidth());
    uint32_t displayHeight = static_cast<uint32_t>(displayInfo->GetHeight());
    if ((realNumVec[LEFT] > displayWidth / HALF_SCREEN) || (realNumVec[RIGHT] > displayWidth / HALF_SCREEN) ||
        (realNumVec[TOP] > displayHeight / HALF_SCREEN) || (realNumVec[BOTTOM] > displayHeight / HALF_SCREEN)) {
        TLOGE(WmsLogTag::DMS, "curved screen boundary data is not correct");
        waterfallDisplayAreaRects_ = emptyRects;
        return;
    }

    CalcWaterfallRectsByRotation(GetCurrentDisplayRotation(displayId), displayWidth, displayHeight, realNumVec);
}

void ScreenCutoutController::CalcWaterfallRectsByRotation(Rotation rotation, uint32_t displayWidth,
    uint32_t displayHeight, std::vector<uint32_t> realNumVec)
{
    switch (rotation) {
        case Rotation::ROTATION_0: {
            DMRect leftRect = CreateWaterfallRect(0, 0, realNumVec[LEFT], displayHeight);
            DMRect topRect = CreateWaterfallRect(0, 0, displayWidth, realNumVec[TOP]);
            DMRect rightRect =
                CreateWaterfallRect(displayWidth - realNumVec[RIGHT], 0, realNumVec[RIGHT], displayHeight);
            DMRect bottomRect =
                CreateWaterfallRect(0, displayHeight - realNumVec[BOTTOM], displayWidth, realNumVec[BOTTOM]);
            waterfallDisplayAreaRects_ = WaterfallDisplayAreaRects { leftRect, topRect, rightRect, bottomRect };
            return;
        }
        case Rotation::ROTATION_90: {
            DMRect leftRect = CreateWaterfallRect(0, 0, realNumVec[BOTTOM], displayHeight);
            DMRect topRect = CreateWaterfallRect(0, 0, displayWidth, realNumVec[LEFT]);
            DMRect rightRect = CreateWaterfallRect(displayWidth - realNumVec[TOP], 0, realNumVec[TOP], displayHeight);
            DMRect bottomRect =
                CreateWaterfallRect(0, displayHeight - realNumVec[RIGHT], displayWidth, realNumVec[RIGHT]);
            waterfallDisplayAreaRects_ = WaterfallDisplayAreaRects { leftRect, topRect, rightRect, bottomRect };
            return;
        }
        case Rotation::ROTATION_180: {
            DMRect leftRect = CreateWaterfallRect(0, 0, realNumVec[RIGHT], displayHeight);
            DMRect topRect = CreateWaterfallRect(0, 0, displayWidth, realNumVec[BOTTOM]);
            DMRect rightRect = CreateWaterfallRect(displayWidth - realNumVec[LEFT], 0, realNumVec[LEFT], displayHeight);
            DMRect bottomRect = CreateWaterfallRect(0, displayHeight - realNumVec[TOP], displayWidth, realNumVec[TOP]);
            waterfallDisplayAreaRects_ = WaterfallDisplayAreaRects { leftRect, topRect, rightRect, bottomRect };
            return;
        }
        case Rotation::ROTATION_270: {
            DMRect leftRect = CreateWaterfallRect(0, 0, realNumVec[TOP], displayHeight);
            DMRect topRect = CreateWaterfallRect(0, 0, displayWidth, realNumVec[RIGHT]);
            DMRect rightRect =
                CreateWaterfallRect(displayWidth - realNumVec[BOTTOM], 0, realNumVec[BOTTOM], displayHeight);
            DMRect bottomRect =
                CreateWaterfallRect(0, displayHeight - realNumVec[LEFT], displayWidth, realNumVec[LEFT]);
            waterfallDisplayAreaRects_ = WaterfallDisplayAreaRects { leftRect, topRect, rightRect, bottomRect };
            return;
        }
        default: {
        }
    }
}

DMRect ScreenCutoutController::CreateWaterfallRect(uint32_t left, uint32_t top, uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0) {
        return DMRect { 0, 0, 0, 0 };
    }
    return DMRect { static_cast<int32_t>(left), static_cast<int32_t>(top), width, height };
}

RectF ScreenCutoutController::CalculateCurvedCompression(const ScreenProperty& screenProperty)
{
    TLOGI(WmsLogTag::DMS, "calculate curved compression");
    RectF finalRect = RectF(0, 0, 0, 0);
    sptr<DisplayInfo> displayInfo = ScreenSessionManager::GetInstance().GetDefaultDisplayInfo();
    uint32_t iCurvedSize = 0;
    if (!displayInfo || iCurvedSize == 0) {
        TLOGE(WmsLogTag::DMS, "display Info. invalid or curved area config value is zero");
        return finalRect;
    }

    uint32_t screenWidth = static_cast<uint32_t>(displayInfo->GetWidth());
    uint32_t screenHeight = static_cast<uint32_t>(displayInfo->GetHeight());
    uint32_t realWidth = static_cast<uint32_t>(iCurvedSize * screenProperty.GetVirtualPixelRatio());
    if (realWidth >= screenHeight / QUARTER_SCREEN || realWidth >= screenWidth / QUARTER_SCREEN) {
        TLOGW(WmsLogTag::DMS, "curved area is beyond the edge limit");
        return finalRect;
    }

    Rotation rotation = displayInfo->GetRotation();
    TLOGI(WmsLogTag::DMS, "realWidth : %{public}u rotation : %{public}u", realWidth, rotation);
    bool isLandscape = screenHeight < screenWidth ? true : false;
    uint32_t totalCompressedSize = realWidth * HALF_SCREEN; // *2 for both sides.
    uint32_t displayHeightAfter =
        isLandscape ? screenHeight - totalCompressedSize : screenWidth - totalCompressedSize;
    finalRect.left_ = screenProperty.GetBounds().rect_.GetLeft();
    finalRect.top_ = screenProperty.GetBounds().rect_.GetTop();
    if (!IsDisplayRotationHorizontal(rotation)) {
        finalRect.width_ = displayHeightAfter;
        finalRect.height_ = screenProperty.GetBounds().rect_.GetHeight();
        offsetY_ = realWidth;
        SetWaterfallDisplayCompressionStatus(true);
    } else {
        if (GetWaterfallDisplayCompressionStatus()) {
            displayHeightAfter =
                isLandscape ? screenHeight + totalCompressedSize : screenWidth + totalCompressedSize;
        }
        finalRect.width_ = screenProperty.GetBounds().rect_.GetWidth();
        finalRect.height_ = displayHeightAfter;
        SetWaterfallDisplayCompressionStatus(false);
    }
    return finalRect;
}

bool ScreenCutoutController::IsDisplayRotationHorizontal(Rotation rotation)
{
    return (rotation == ConvertDeviceToDisplayRotation(DeviceRotationValue::ROTATION_LANDSCAPE)) ||
        (rotation == ConvertDeviceToDisplayRotation(DeviceRotationValue::ROTATION_LANDSCAPE_INVERTED));
}

Rotation ScreenCutoutController::ConvertDeviceToDisplayRotation(DeviceRotationValue deviceRotation)
{
    if (deviceRotation == DeviceRotationValue::INVALID) {
        return Rotation::ROTATION_0;
    }
    if (deviceToDisplayRotationMap_.empty()) {
        ProcessRotationMapping();
    }
    return deviceToDisplayRotationMap_.at(deviceRotation);
}

Rotation ScreenCutoutController::GetCurrentDisplayRotation(DisplayId displayId)
{
    sptr<DisplayInfo> displayInfo = ScreenSessionManager::GetInstance().GetDisplayInfoById(displayId);
    if (!displayInfo) {
        TLOGE(WmsLogTag::DMS, "Cannot get default display info");
        return defaultDeviceRotation_ == 0 ? ConvertDeviceToDisplayRotation(DeviceRotationValue::ROTATION_PORTRAIT) :
            ConvertDeviceToDisplayRotation(DeviceRotationValue::ROTATION_LANDSCAPE);
    }
    return displayInfo->GetRotation();
}

void ScreenCutoutController::ProcessRotationMapping()
{
    sptr<DisplayInfo> displayInfo = ScreenSessionManager::GetInstance().GetDefaultDisplayInfo();
    // 0 means PORTRAIT, 1 means LANDSCAPE.
    defaultDeviceRotation_ =
        (!displayInfo || (displayInfo->GetWidth() < displayInfo->GetHeight())) ? 0 : 1;
    TLOGI(WmsLogTag::DMS, "defaultDeviceRotation: %{public}u", defaultDeviceRotation_);

    if (deviceToDisplayRotationMap_.empty()) {
        deviceToDisplayRotationMap_ = {
            { DeviceRotationValue::ROTATION_PORTRAIT,
                defaultDeviceRotation_ == 0 ? Rotation::ROTATION_0 : Rotation::ROTATION_90 },
            { DeviceRotationValue::ROTATION_LANDSCAPE,
                defaultDeviceRotation_ == 1 ? Rotation::ROTATION_0 : Rotation::ROTATION_90 },
            { DeviceRotationValue::ROTATION_PORTRAIT_INVERTED,
                defaultDeviceRotation_ == 0 ? Rotation::ROTATION_180 : Rotation::ROTATION_270 },
            { DeviceRotationValue::ROTATION_LANDSCAPE_INVERTED,
                defaultDeviceRotation_ == 1 ? Rotation::ROTATION_180 : Rotation::ROTATION_270 },
        };
    }
}

uint32_t ScreenCutoutController::GetOffsetY()
{
    return offsetY_;
}
} // namespace OHOS::Rosen
