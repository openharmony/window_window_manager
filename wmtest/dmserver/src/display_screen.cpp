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

#include "display_screen.h"

namespace OHOS::Rosen {
DisplayScreen::DisplayScreen(const DisplayInfo& info)
    : id_(info.id_),
      width_(info.width_),
      height_(info.height_),
      freshRate_(info.freshRate_)
{
}

DisplayId DisplayScreen::GetId() const
{
    return id_;
}

int32_t DisplayScreen::GetWidth() const
{
    return width_;
}

int32_t DisplayScreen::GetHeight() const
{
    return height_;
}

uint32_t DisplayScreen::GetFreshRate() const
{
    return freshRate_;
}

void DisplayScreen::SetWidth(int32_t width)
{
    width_ = width;
}

void DisplayScreen::SetHeight(int32_t height)
{
    height_ = height;
}

void DisplayScreen::SetFreshRate(uint32_t freshRate)
{
    freshRate_ = freshRate;
}

void DisplayScreen::SetId(DisplayId id)
{
    id_ = id;
}
} // namespace OHOS::Rosen