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

#include "window_pid_visibility_info.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {

bool WindowPidVisibilityInfo::Marshalling(Parcel& parcel) const
{
    return parcel.WriteInt32(pid_) && parcel.WriteUint32(static_cast<uint32_t>(visibilityState_));
}

WindowPidVisibilityInfo* WindowPidVisibilityInfo::Unmarshalling(Parcel& parcel)
{
    auto windowPidVisibilityInfo = new (std::nothrow) WindowPidVisibilityInfo();
    if (windowPidVisibilityInfo == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "window pid visibility info is nullptr.");
        return nullptr;
    }

    uint32_t visibilityState = 0;
    bool res = parcel.ReadInt32(windowPidVisibilityInfo->pid_) && parcel.ReadUint32(visibilityState);
    if (!res) {
        delete windowPidVisibilityInfo;
        return nullptr;
    }
    windowPidVisibilityInfo->visibilityState_ = static_cast<WindowPidVisibilityState>(visibilityState);
    return windowPidVisibilityInfo;
}
} // namespace OHOS::Rosen
