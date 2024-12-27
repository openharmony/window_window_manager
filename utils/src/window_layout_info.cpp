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

#include "window_layout_info.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {

bool WindowLayoutInfo::Marshalling(Parcel& parcel) const
{
    return parcel.WriteInt32(rect_.posX_) && parcel.WriteInt32(rect_.posY_) &&
        parcel.WriteUint32(rect_.width_) && parcel.WriteUint32(rect_.height_);

WindowLayoutInfo* WindowLayoutInfo::Unmarshalling(Parcel& parcel)
{
    auto WindowLayoutInfo = new (std::nothrow) (class WindowLayoutInfo)();
    if (WindowLayoutInfo == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "Display window info is nullptr.");
        return nullptr;
    }

    windowPidVisibilityInfo->rect_ = { parcel.ReadInt32(), parcel.ReadInt32(), parcel.ReadUint32(), parcel.ReadUint32() };

    return windowPidVisibilityInfo;
}
} // namespace OHOS::Rosen