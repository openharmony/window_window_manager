/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <refbase.h>
#include <iremote_object.h>


#include "window_manager.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
bool AccessibilityWindowInfo::Marshalling(Parcel& parcel) const
{
    return parcel.WriteInt32(wid_) && parcel.WriteInt32(innerWid_) && parcel.WriteInt32(uiNodeId_) &&
        parcel.WriteUint32(windowRect_.width_) &&
        parcel.WriteUint32(windowRect_.height_) && parcel.WriteInt32(windowRect_.posX_) &&
        parcel.WriteInt32(windowRect_.posY_) && parcel.WriteBool(focused_) && parcel.WriteBool(isDecorEnable_) &&
        parcel.WriteUint64(displayId_)  && parcel.WriteUint32(layer_) && parcel.WriteFloat(scaleVal_) &&
        parcel.WriteUint32(static_cast<uint32_t>(mode_)) && parcel.WriteUint32(static_cast<uint32_t>(type_));
}

AccessibilityWindowInfo* AccessibilityWindowInfo::Unmarshalling(Parcel& parcel)
{
    auto info = new (std::nothrow) AccessibilityWindowInfo();
    if (info == nullptr) {
        return nullptr;
    }
    bool res = parcel.ReadInt32(info->wid_) && parcel.ReadInt32(info->innerWid_) && parcel.ReadInt32(info->uiNodeId_) &&
        parcel.ReadUint32(info->windowRect_.width_) &&
        parcel.ReadUint32(info->windowRect_.height_) && parcel.ReadInt32(info->windowRect_.posX_) &&
        parcel.ReadInt32(info->windowRect_.posY_) && parcel.ReadBool(info->focused_) &&
        parcel.ReadBool(info->isDecorEnable_) && parcel.ReadUint64(info->displayId_) &&
        parcel.ReadUint32(info->layer_) && parcel.ReadFloat(info->scaleVal_);
    if (!res) {
        delete info;
        return nullptr;
    }
    info->mode_ = static_cast<WindowMode>(parcel.ReadUint32());
    info->type_ = static_cast<WindowType>(parcel.ReadUint32());
    return info;
}
}
}