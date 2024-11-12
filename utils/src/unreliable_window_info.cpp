/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "window_manager.h"

namespace OHOS {
namespace Rosen {
bool UnreliableWindowInfo::Marshalling(Parcel& parcel) const
{
    return parcel.WriteInt32(windowId_) && parcel.WriteUint32(windowRect_.width_) &&
        parcel.WriteUint32(windowRect_.height_) && parcel.WriteInt32(windowRect_.posX_) &&
        parcel.WriteInt32(windowRect_.posY_) && parcel.WriteUint32(zOrder_) &&
        parcel.WriteFloat(floatingScale_) && parcel.WriteFloat(scaleX_) && parcel.WriteFloat(scaleY_);
}

UnreliableWindowInfo* UnreliableWindowInfo::Unmarshalling(Parcel& parcel)
{
    auto info = new (std::nothrow) UnreliableWindowInfo();
    if (info == nullptr) {
        return nullptr;
    }
    bool res = parcel.ReadInt32(info->windowId_) && parcel.ReadUint32(info->windowRect_.width_) &&
        parcel.ReadUint32(info->windowRect_.height_) && parcel.ReadInt32(info->windowRect_.posX_) &&
        parcel.ReadInt32(info->windowRect_.posY_) && parcel.ReadUint32(info->zOrder_) &&
        parcel.ReadFloat(info->floatingScale_) && parcel.ReadFloat(info->scaleX_) && parcel.ReadFloat(info->scaleY_);
    if (!res) {
        delete info;
        return nullptr;
    }
    return info;
}
}
}