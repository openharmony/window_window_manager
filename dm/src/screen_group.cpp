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

#include "screen_group.h"

#include "wm_common.h"

namespace OHOS::Rosen {
class ScreenGroup::Impl : public RefBase {
friend class ScreenGroup;
private:
    Impl() = default;
    ~Impl() = default;

    std::vector<sptr<Screen>> children_;
    std::vector<Point> position_;
    ScreenType type_ { ScreenType::SCREEN_ALONE };
};

ScreenGroup::ScreenGroup()
{
    pImpl_ = new Impl();
}

ScreenGroup::~ScreenGroup()
{
}

ScreenType ScreenGroup::GetType() const
{
    return pImpl_->type_;
}

std::vector<sptr<Screen>> ScreenGroup::GetChildren() const
{
    return pImpl_->children_;
}

std::vector<Point> ScreenGroup::GetChildrenPosition() const
{
    return pImpl_->position_;
}
} // namespace OHOS::Rosen