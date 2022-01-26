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
    width_ = info->width_;
    height_ = info->height_;
    virtualWidth_ = info->virtualWidth_;
    virtualHeight_ = info->virtualHeight_;
    virtualPixelRatio_ = info->virtualPixelRatio_;
    parent_ = info->parent_;
    hasChild_ = info->hasChild_;
}

bool ScreenInfo::Marshalling(Parcel &parcel) const
{
    return parcel.WriteUint64(id_) &&
        parcel.WriteUint32(width_) && parcel.WriteUint32(height_) &&
        parcel.WriteUint32(virtualWidth_) && parcel.WriteUint32(virtualHeight_) &&
        parcel.WriteFloat(virtualPixelRatio_) && parcel.WriteUint64(parent_) &&
        parcel.WriteBool(hasChild_);
}

sptr<ScreenInfo> ScreenInfo::Unmarshalling(Parcel &parcel)
{
    sptr<ScreenInfo> info = new ScreenInfo();
    return info->InnerUnmarshalling(parcel);
}

sptr<ScreenInfo> ScreenInfo::InnerUnmarshalling(Parcel& parcel)
{
    bool res = parcel.ReadUint64(id_) &&
        parcel.ReadUint32(width_) && parcel.ReadUint32(height_) &&
        parcel.ReadUint32(virtualWidth_) && parcel.ReadUint32(virtualHeight_) &&
        parcel.ReadFloat(virtualPixelRatio_) && parcel.ReadUint64(parent_) &&
        parcel.ReadBool(hasChild_);
    if (!res) {
        return nullptr;
    }
    return this;
}
} // namespace OHOS::Rosen