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
class Display::Impl : public RefBase {
friend class Display;
private:
    std::string name_;
    DisplayId id_ { DISPLAY_ID_INVALD };
    int32_t width_ { 0 };
    int32_t height_ { 0 };
    uint32_t freshRate_ { 0 };
};

Display::Display(const std::string& name, DisplayInfo* info)
    : pImpl_(new Impl())
{
    pImpl_->name_ = name;
    pImpl_->id_ = info->id_;
    pImpl_->width_ = info->width_;
    pImpl_->height_ = info->height_;
    pImpl_->freshRate_ = info->freshRate_;
}

DisplayId Display::GetId() const
{
    return pImpl_->id_;
}

int32_t Display::GetWidth() const
{
    return pImpl_->width_;
}

int32_t Display::GetHeight() const
{
    return pImpl_->height_;
}

uint32_t Display::GetFreshRate() const
{
    return pImpl_->freshRate_;
}

void Display::SetWidth(int32_t width)
{
    pImpl_->width_ = width;
}

void Display::SetHeight(int32_t height)
{
    pImpl_->height_ = height;
}

void Display::SetFreshRate(uint32_t freshRate)
{
    pImpl_->freshRate_ = freshRate;
}

float Display::GetVirtualPixelRatio() const
{
    // TODO: Should get from DMS
#ifdef PRODUCT_RK
    return 1.0f;
#else
    return 2.0f;
#endif
}

void Display::SetId(DisplayId id)
{
    pImpl_->id_ = id;
}
} // namespace OHOS::Rosen