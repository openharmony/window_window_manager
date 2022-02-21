/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "screen_group_info.h"

namespace OHOS::Rosen {
void ScreenGroupInfo::Update(sptr<ScreenGroupInfo> info)
{
    ScreenInfo::Update(info);
    children_.clear();
    children_.insert(children_.begin(), info->children_.begin(), info->children_.end());
    position_.clear();
    position_.insert(position_.begin(), info->position_.begin(), info->position_.end());
    combination_ = info->combination_;
}

bool ScreenGroupInfo::Marshalling(Parcel &parcel) const
{
    bool res = ScreenInfo::Marshalling(parcel) && parcel.WriteUint32((uint32_t)combination_) &&
        parcel.WriteUInt64Vector(children_);
    if (!res) {
        return false;
    }
    size_t size = position_.size();
    if (!parcel.WriteUint32(size)) {
        return false;
    }
    for (size_t i = 0; i < size; i++) {
        if (!parcel.WriteInt32(position_[i].posX_) || !parcel.WriteInt32(position_[i].posY_)) {
            return false;
        }
    }
    return true;
}

ScreenGroupInfo* ScreenGroupInfo::Unmarshalling(Parcel &parcel)
{
    ScreenGroupInfo* screenGroupInfo = new ScreenGroupInfo();
    return screenGroupInfo->InnerUnmarshalling(parcel);
}

ScreenGroupInfo* ScreenGroupInfo::InnerUnmarshalling(Parcel& parcel)
{
    uint32_t combination;
    if (!ScreenInfo::InnerUnmarshalling(parcel) || !parcel.ReadUint32(combination) ||
        !parcel.ReadUInt64Vector(&children_)) {
        return nullptr;
    }
    combination_ = (ScreenCombination) combination;
    uint32_t size;
    if (!parcel.ReadUint32(size)) {
        return nullptr;
    }
    for (size_t i = 0; i < size; i++) {
        Point point;
        if (parcel.ReadInt32(point.posX_) && parcel.ReadInt32(point.posY_)) {
            position_.push_back(point);
        } else {
            return nullptr;
        }
    }
    return this;
}
} // namespace OHOS::Rosen