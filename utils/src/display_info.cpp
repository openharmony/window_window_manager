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

#include "display_info.h"

namespace OHOS::Rosen {
void DisplayInfo::Update(DisplayInfo* info)
{
    id_ = info->id_;
    type_ = info->type_;
    width_ = info->width_;
    height_ = info->height_;
    freshRate_ = info->freshRate_;
    xDpi_ = info->xDpi_;
    yDpi_ = info->yDpi_;
    rotation_ = info->rotation_;
}

bool DisplayInfo::Marshalling(Parcel &parcel) const
{
    return parcel.WriteUint64(id_) && parcel.WriteUint32(type_) &&
        parcel.WriteInt32(width_) && parcel.WriteInt32(height_) &&
        parcel.WriteUint32(freshRate_) &&
        parcel.WriteFloat(xDpi_) && parcel.WriteFloat(yDpi_) &&
        parcel.WriteUint32(static_cast<uint32_t>(rotation_));
}

DisplayInfo *DisplayInfo::Unmarshalling(Parcel &parcel)
{
    DisplayInfo *displayInfo = new DisplayInfo();
    if (displayInfo == nullptr) {
        return nullptr;
    }
    uint32_t type = (uint32_t)DisplayType::DEFAULT;
    uint32_t rotation;
    bool res = parcel.ReadUint64(displayInfo->id_) && parcel.ReadUint32(type) &&
        parcel.ReadInt32(displayInfo->width_) && parcel.ReadInt32(displayInfo->height_) &&
        parcel.ReadUint32(displayInfo->freshRate_) &&
        parcel.ReadFloat(displayInfo->xDpi_) && parcel.ReadFloat(displayInfo->yDpi_) &&
        parcel.ReadUint32(rotation);
    if (!res) {
        displayInfo = nullptr;
    } else {
        displayInfo->type_ = (DisplayType)type;
        displayInfo->rotation_ = static_cast<Rotation>(rotation);
    }
    return displayInfo;
}
} // namespace OHOS::Rosen