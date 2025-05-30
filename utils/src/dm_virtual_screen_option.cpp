/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "dm_virtual_screen_option.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
DmVirtualScreenOption::DmVirtualScreenOption(VirtualScreenOption option) : option_(std::move(option))
{}

bool DmVirtualScreenOption::Marshalling(Parcel& parcel) const
{
    return parcel.WriteString(option_.name_) && parcel.WriteUint32(option_.width_) &&
           parcel.WriteUint32(option_.height_) && parcel.WriteFloat(option_.density_) &&
           parcel.WriteInt32(option_.flags_) && parcel.WriteBool(option_.isForShot_) &&
           parcel.WriteUInt64Vector(option_.missionIds_);
}

DmVirtualScreenOption* DmVirtualScreenOption::Unmarshalling(Parcel& parcel)
{
    std::string name;
    uint32_t width;
    uint32_t height;
    float density;
    int32_t flags;
    bool isForShot;
    std::vector<uint64_t> missionIds;
    if (!(parcel.ReadString(name) && parcel.ReadUint32(width) && parcel.ReadUint32(height) &&
        parcel.ReadFloat(density) && parcel.ReadInt32(flags) && parcel.ReadBool(isForShot) &&
        parcel.ReadUInt64Vector(&missionIds))) {
        TLOGE(WmsLogTag::DMS, "read from parcel failed");
        return nullptr;
    }

    auto* virScrOption = new (std::nothrow) DmVirtualScreenOption();
    if (virScrOption == nullptr) {
        TLOGE(WmsLogTag::DMS, "new DmVirtualScreenOption failed");
        return nullptr;
    }

    virScrOption->option_.name_ = name;
    virScrOption->option_.width_ = width;
    virScrOption->option_.height_ = height;
    virScrOption->option_.density_ = density;
    virScrOption->option_.flags_ = flags;
    virScrOption->option_.isForShot_ = isForShot;
    virScrOption->option_.missionIds_ = missionIds;
    return virScrOption;
}

VirtualScreenOption DmVirtualScreenOption::GetOption() const
{
    return option_;
}
} // namespace OHOS::Rosen
