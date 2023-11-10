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

#include "window_visibility_info.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowVisibilityInfo"};
}

bool WindowVisibilityInfo::Marshalling(Parcel &parcel) const
{
    return parcel.WriteUint32(windowId_) && parcel.WriteInt32(pid_) &&
           parcel.WriteInt32(uid_) && parcel.WriteUint32(static_cast<uint32_t>(visibilityState_)) &&
           parcel.WriteUint32(static_cast<uint32_t>(windowType_));
}

WindowVisibilityInfo* WindowVisibilityInfo::Unmarshalling(Parcel &parcel)
{
    auto windowVisibilityInfo = new (std::nothrow) WindowVisibilityInfo();
    if (windowVisibilityInfo == nullptr) {
        WLOGFE("window visibility info is nullptr.");
        return nullptr;
    }

    uint32_t visibilityState = 0;
    bool res = parcel.ReadUint32(windowVisibilityInfo->windowId_) && parcel.ReadInt32(windowVisibilityInfo->pid_) &&
        parcel.ReadInt32(windowVisibilityInfo->uid_) && parcel.ReadUint32(visibilityState);
    if (!res) {
        delete windowVisibilityInfo;
        return nullptr;
    }
    windowVisibilityInfo->visibilityState_ = static_cast<WindowVisibilityState>(visibilityState);
    windowVisibilityInfo->windowType_ = static_cast<WindowType>(parcel.ReadUint32());
    return windowVisibilityInfo;
}
} // namespace OHOS::Rosen
