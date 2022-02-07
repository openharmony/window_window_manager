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

#include "screen_info.h"

namespace OHOS::Rosen {
void ScreenInfo::Update(sptr<ScreenInfo> info)
{
    id_ = info->id_;
    virtualWidth_ = info->virtualWidth_;
    virtualHeight_ = info->virtualHeight_;
    virtualPixelRatio_ = info->virtualPixelRatio_;
    parent_ = info->parent_;
    hasChild_ = info->hasChild_;
    modeId_ = info->modeId_;
    modes_ = info->modes_;
}

bool ScreenInfo::Marshalling(Parcel &parcel) const
{
    bool res1 = parcel.WriteUint64(id_) &&
        parcel.WriteUint32(virtualWidth_) && parcel.WriteUint32(virtualHeight_) &&
        parcel.WriteFloat(virtualPixelRatio_) && parcel.WriteUint64(parent_) &&
        parcel.WriteBool(hasChild_) && parcel.WriteUint32(modeId_) &&
        parcel.WriteUint32(static_cast<uint32_t>(modes_.size()));
    bool res2 = true;
    for (uint32_t modeIndex = 0; modeIndex < modes_.size(); modeIndex++) {
        res2 = res2 && parcel.WriteUint32(modes_[modeIndex]->height_) &&
            parcel.WriteUint32(modes_[modeIndex]->width_) &&
            parcel.WriteUint32(modes_[modeIndex]->freshRate_);
    }
    return res1 && res2;
}

ScreenInfo* ScreenInfo::Unmarshalling(Parcel &parcel)
{
    ScreenInfo* info = new ScreenInfo();
    return info->InnerUnmarshalling(parcel);
}

ScreenInfo* ScreenInfo::InnerUnmarshalling(Parcel& parcel)
{
    uint32_t size = 0;
    bool res1 = parcel.ReadUint64(id_) &&
        parcel.ReadUint32(virtualWidth_) && parcel.ReadUint32(virtualHeight_) &&
        parcel.ReadFloat(virtualPixelRatio_) && parcel.ReadUint64(parent_) &&
        parcel.ReadBool(hasChild_) && parcel.ReadUint32(modeId_) &&
        parcel.ReadUint32(size);
    if (!res1) {
        return nullptr;
    }
    bool res2 = true;
    modes_.clear();
    for (uint32_t modeIndex = 0; modeIndex < size; modeIndex++) {
        sptr<SupportedScreenModes> mode = new SupportedScreenModes();
        res2 = res2 && parcel.ReadUint32(mode->height_) &&
            parcel.ReadUint32(mode->width_) &&
            parcel.ReadUint32(mode->freshRate_);
        modes_.push_back(mode);
    }
    if (!res2) {
        return nullptr;
    }
    return this;
}
} // namespace OHOS::Rosen