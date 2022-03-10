/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "abstract_display.h"

#include "abstract_screen_controller.h"
#include "display_manager_service.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "AbstractDisplay"};
    constexpr int32_t LARGE_SCREEN_WIDTH = 2160;
}

AbstractDisplay::AbstractDisplay(DisplayId id, ScreenId screenId, int32_t width, int32_t height, uint32_t refreshRate)
    : id_(id),
      screenId_(screenId),
      width_(width),
      height_(height),
      refreshRate_(refreshRate)
{
    if ((width_ >= LARGE_SCREEN_WIDTH) || (height_ >= LARGE_SCREEN_WIDTH)) {
        virtualPixelRatio_ = 2.0f;
    } else {
        virtualPixelRatio_ = 1.0f;
    }
}

DisplayId AbstractDisplay::GetId() const
{
    return id_;
}

int32_t AbstractDisplay::GetWidth() const
{
    return width_;
}

int32_t AbstractDisplay::GetHeight() const
{
    return height_;
}

uint32_t AbstractDisplay::GetRefreshRate() const
{
    return refreshRate_;
}

float AbstractDisplay::GetVirtualPixelRatio() const
{
    return virtualPixelRatio_;
}

void AbstractDisplay::SetWidth(int32_t width)
{
    width_ = width;
}

void AbstractDisplay::SetHeight(int32_t height)
{
    height_ = height;
}

void AbstractDisplay::SetRefreshRate(uint32_t refreshRate)
{
    refreshRate_ = refreshRate;
}

void AbstractDisplay::SetVirtualPixelRatio(float virtualPixelRatio)
{
    virtualPixelRatio_ = virtualPixelRatio;
}

void AbstractDisplay::SetId(DisplayId id)
{
    id_ = id;
}

void AbstractDisplay::SetOrientation(Orientation orientation)
{
    orientation_ = orientation;
}

bool AbstractDisplay::RequestRotation(Rotation rotation)
{
    WLOGD("request rotation from %{public}u to %{public}u, display %{public}" PRIu64"", rotation_, rotation, id_);
    if (rotation_ == rotation) {
        WLOGFE("rotation not change %{public}u", rotation);
        return false;
    }
    if (IsVertical(rotation) != IsVertical(rotation_)) {
        std::swap(width_, height_);
    }
    rotation_ = rotation;
    return true;
}

Rotation AbstractDisplay::GetRotation() const
{
    return rotation_;
}

void AbstractDisplay::SetFreezeFlag(FreezeFlag freezeFlag)
{
    freezeFlag_ = freezeFlag;
}

FreezeFlag AbstractDisplay::GetFreezeFlag() const
{
    return freezeFlag_;
}

bool AbstractDisplay::BindAbstractScreen(ScreenId dmsScreenId)
{
    sptr<AbstractScreenController> screenController
        = DisplayManagerService::GetInstance().GetAbstractScreenController();
    return BindAbstractScreen(screenController->GetAbstractScreen(dmsScreenId));
}

bool AbstractDisplay::BindAbstractScreen(sptr<AbstractScreen> abstractScreen)
{
    if (abstractScreen == nullptr) {
        WLOGE("display bind screen error, cannot get screen. display:%{public}" PRIu64"", id_);
        return false;
    }
    ScreenId dmsScreenId = abstractScreen->dmsId_;
    sptr<SupportedScreenModes> info = abstractScreen->GetActiveScreenMode();
    if (info == nullptr) {
        WLOGE("display bind screen error, cannot get info. display:%{public}" PRIu64", screen:%{public}" PRIu64"",
            id_, dmsScreenId);
        return false;
    }
    width_ = static_cast<int32_t>(info->width_);
    height_ = static_cast<int32_t>(info->height_);
    refreshRate_ = info->refreshRate_;
    screenId_ = dmsScreenId;
    WLOGD("display bound to screen. display:%{public}" PRIu64", screen:%{public}" PRIu64"", id_, dmsScreenId);
    return true;
}

ScreenId AbstractDisplay::GetAbstractScreenId() const
{
    return screenId_;
}

sptr<DisplayInfo> AbstractDisplay::ConvertToDisplayInfo() const
{
    sptr<DisplayInfo> displayInfo = new(std::nothrow) DisplayInfo();
    if (displayInfo == nullptr) {
        return displayInfo;
    }
    displayInfo->width_ = width_;
    displayInfo->height_ = height_;
    displayInfo->id_ = id_;
    displayInfo->refreshRate_ = refreshRate_;
    displayInfo->screenId_ = screenId_;
    displayInfo->virtualPixelRatio_ = virtualPixelRatio_;
    displayInfo->rotation_ = rotation_;
    displayInfo->orientation_ = orientation_;
    return displayInfo;
}
} // namespace OHOS::Rosen