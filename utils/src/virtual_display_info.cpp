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

#include "virtual_display_info.h"

namespace OHOS::Rosen {
VirtualDisplayInfo::VirtualDisplayInfo()
{
    name_ = "";
    width_ = 0;
    height_ = 0;
    displayIdToMirror_ = 0;
    flags_ = 0;
}

VirtualDisplayInfo::VirtualDisplayInfo(const std::string &name, uint32_t width, uint32_t height,
    DisplayId displayIdToMirror, int32_t flags)
{
    name_ = name;
    width_ = width;
    height_ = height;
    displayIdToMirror_ = displayIdToMirror;
    flags_ = flags;
}

bool VirtualDisplayInfo::Marshalling(Parcel &parcel) const
{
    return parcel.WriteString(name_) && parcel.WriteUint32(width_) &&
        parcel.WriteUint32(height_) && parcel.WriteUint64(displayIdToMirror_) &&
        parcel.WriteInt32(flags_);
}

VirtualDisplayInfo* VirtualDisplayInfo::Unmarshalling(Parcel &parcel)
{
    VirtualDisplayInfo *virtualDisplayInfo = new VirtualDisplayInfo();
    if (virtualDisplayInfo == nullptr) {
        return nullptr;
    }
    bool res = parcel.ReadString(virtualDisplayInfo->name_) &&
        parcel.ReadUint32(virtualDisplayInfo->width_) &&
        parcel.ReadUint32(virtualDisplayInfo->height_) &&
        parcel.ReadUint64(virtualDisplayInfo->displayIdToMirror_) &&
        parcel.ReadInt32(virtualDisplayInfo->flags_);
    if (!res) {
        return nullptr;
    }
    return virtualDisplayInfo;
}
} // namespace OHOS::Rosen