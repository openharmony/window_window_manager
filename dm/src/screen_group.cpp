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
#include "screen.h"
#include "screen_group_info.h"

namespace OHOS::Rosen {
class ScreenGroup::Impl : public RefBase {
friend class ScreenGroup;
private:
    Impl() = default;
    ~Impl() = default;

    std::vector<ScreenId> children_;
    std::vector<Point> position_;
    ScreenCombination combination_ { ScreenCombination::SCREEN_ALONE };
};

ScreenGroup::ScreenGroup(const ScreenGroupInfo* info)
    : Screen(info), pImpl_(new Impl())
{
    pImpl_->children_ = info->children_;
    pImpl_->position_ = info->position_;
    pImpl_->combination_ = info->combination_;
}

ScreenGroup::~ScreenGroup()
{
}

ScreenCombination ScreenGroup::GetCombination() const
{
    return pImpl_->combination_;
}

std::vector<ScreenId> ScreenGroup::GetChildrenIds() const
{
    return pImpl_->children_;
}
} // namespace OHOS::Rosen