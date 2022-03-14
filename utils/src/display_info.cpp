/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
bool DisplayInfo::Marshalling(Parcel &parcel) const
{
    return parcel.WriteUint64(id_) && parcel.WriteUint32(type_) &&
        parcel.WriteInt32(width_) && parcel.WriteInt32(height_) &&
        parcel.WriteUint32(freshRate_) && parcel.WriteUint32(refreshRate_) && parcel.WriteUint64(screenId_) &&
        parcel.WriteFloat(virtualPixelRatio_) && parcel.WriteFloat(xDpi_) && parcel.WriteFloat(yDpi_) &&
        parcel.WriteUint32(static_cast<uint32_t>(rotation_)) &&
        parcel.WriteUint32(static_cast<uint32_t>(orientation_));
}

DisplayInfo *DisplayInfo::Unmarshalling(Parcel &parcel)
{
    DisplayInfo *displayInfo = new DisplayInfo();
    uint32_t type = (uint32_t)DisplayType::DEFAULT;
    uint32_t rotation;
    uint32_t orientation;
    bool res = parcel.ReadUint64(displayInfo->id_) && parcel.ReadUint32(type) &&
        parcel.ReadInt32(displayInfo->width_) && parcel.ReadInt32(displayInfo->height_) &&
        parcel.ReadUint32(displayInfo->freshRate_) &&
        parcel.ReadUint32(displayInfo->refreshRate_) && parcel.ReadUint64(displayInfo->screenId_) &&
        parcel.ReadFloat(displayInfo->virtualPixelRatio_) &&
        parcel.ReadFloat(displayInfo->xDpi_) && parcel.ReadFloat(displayInfo->yDpi_) &&
        parcel.ReadUint32(rotation) && parcel.ReadUint32(orientation);
    if (!res) {
        delete displayInfo;
        return nullptr;
    }
    displayInfo->type_ = (DisplayType)type;
    displayInfo->rotation_ = static_cast<Rotation>(rotation);
    displayInfo->orientation_ = static_cast<Orientation>(orientation);
    return displayInfo;
}
} // namespace OHOS::Rosen