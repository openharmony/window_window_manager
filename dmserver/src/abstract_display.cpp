/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "AbstractDisplay"};
}

AbstractDisplay::AbstractDisplay(const DisplayInfo& info)
    : id_(info.id_),
      width_(info.width_),
      height_(info.height_),
      freshRate_(info.freshRate_)
{
}

AbstractDisplay::AbstractDisplay(DisplayId id, ScreenId screenId, int32_t width, int32_t height, uint32_t freshRate)
    : id_(id),
      screenId_(screenId),
      width_(width),
      height_(height),
      freshRate_(freshRate)
{
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

uint32_t AbstractDisplay::GetFreshRate() const
{
    return freshRate_;
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

void AbstractDisplay::SetFreshRate(uint32_t freshRate)
{
    freshRate_ = freshRate;
}

void AbstractDisplay::SetVirtualPixelRatio(float virtualPixelRatio)
{
    virtualPixelRatio_ = virtualPixelRatio;
}

void AbstractDisplay::SetId(DisplayId id)
{
    id_ = id;
}

bool AbstractDisplay::BindAbstractScreenId(ScreenId dmsScreenId)
{
    sptr<AbstractScreenController> screenController
        = DisplayManagerService::GetInstance().GetAbstractScreenController();
    sptr<AbstractScreen> screen = screenController->GetAbstractScreen(dmsScreenId);
    if (screen == nullptr) {
        WLOGE("display bind screen error, cannot get screen. display:%{public}" PRIu64", screen:%{public}" PRIu64"",
            id_, dmsScreenId);
        return false;
    }
    // TODO: screen->rsDisplayNode_->SetScreenId(rsScreenId);
    sptr<AbstractScreenInfo> info = screen->GetActiveScreenInfo();
    if (info == nullptr) {
        WLOGE("display bind screen error, cannot get info. display:%{public}" PRIu64", screen:%{public}" PRIu64"",
            id_, dmsScreenId);
        return false;
    }
    width_ = info->width_;
    height_ = info->height_;
    freshRate_ = info->freshRate_;
    screenId_ = dmsScreenId;
    WLOGD("display bound to screen. display:%{public}" PRIu64", screen:%{public}" PRIu64"", id_, dmsScreenId);
    return true;
}

ScreenId AbstractDisplay::GetAbstractScreenId() const
{
    return screenId_;
}
} // namespace OHOS::Rosen