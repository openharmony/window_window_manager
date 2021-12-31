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

#include "display.h"
#include "display_info.h"

namespace OHOS::Rosen {
Display::Display(const std::string& name, DisplayInfo* info)
    : name_(name),
      id_(info->id_),
      width_(info->width_),
      height_(info->height_),
      freshRate_(info->freshRate_)
{
}

DisplayId Display::GetId() const
{
    return id_;
}

int32_t Display::GetWidth() const
{
    return width_;
}

int32_t Display::GetHeight() const
{
    return height_;
}

uint32_t Display::GetFreshRate() const
{
    return freshRate_;
}

void Display::SetWidth(int32_t width)
{
    width_ = width;
}

void Display::SetHeight(int32_t height)
{
    height_ = height;
}

void Display::SetFreshRate(uint32_t freshRate)
{
    freshRate_ = freshRate;
}

void Display::SetId(DisplayId id)
{
    id_ = id;
}
} // namespace OHOS::Rosen