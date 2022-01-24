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

#include "screen.h"

namespace OHOS::Rosen {
class Screen::Impl : public RefBase {
friend class Screen;
public:
    Impl() = default;
    ~Impl() = default;

    ScreenId id_ { SCREEN_ID_INVALID };
    uint32_t width_ { 0 };
    uint32_t height_ { 0 };
    uint32_t virtualWidth_ { 0 };
    uint32_t virtualHeight_ { 0 };
    float virtualPixelRatio_ { 0.0 };
    sptr<Screen> parent_ { nullptr };
    bool hasChild_ { false };
};

Screen::Screen()
{
    pImpl_ = new Impl();
}

Screen::~Screen()
{
}

bool Screen::IsGroup() const
{
    return pImpl_->hasChild_;
}

ScreenId Screen::GetId() const
{
    return pImpl_->id_;
}

uint32_t Screen::GetWidth() const
{
    return pImpl_->width_;
}

uint32_t Screen::GetHeight() const
{
    return pImpl_->height_;
}

uint32_t Screen::GetVirtualWidth() const
{
    return pImpl_->virtualWidth_;
}

uint32_t Screen::GetVirtualHeight() const
{
    return pImpl_->virtualHeight_;
}

float Screen::GetVirtualPixelRatio() const
{
    return pImpl_->virtualPixelRatio_;
}

sptr<Screen> Screen::GetParent() const
{
    return pImpl_->parent_;
}
} // namespace OHOS::Rosen