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

#include "window_drawing_content_info.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowDrawingContentInfo"};
}

bool WindowDrawingContentInfo::Marshalling(Parcel &parcel) const
{
    return parcel.WriteUint32(windowId_) && parcel.WriteInt32(pid_) &&
           parcel.WriteInt32(uid_) && parcel.WriteUint32(static_cast<uint32_t>(drawingContentState_)) &&
           parcel.WriteUint32(static_cast<uint32_t>(windowType_));
}

WindowDrawingContentInfo* WindowDrawingContentInfo::Unmarshalling(Parcel &parcel)
{
    auto windowDrawingContentInfo = new (std::nothrow) WindowDrawingContentInfo();
    if (windowDrawingContentInfo == nullptr) {
        WLOGFE("window visibility info is nullptr.");
        return nullptr;
    }

    uint32_t drawingContentState = 0;
    bool res = parcel.ReadUint32(windowDrawingContentInfo->windowId_) &&
        parcel.ReadInt32(windowDrawingContentInfo->pid_) && parcel.ReadInt32(windowDrawingContentInfo->uid_) &&
        parcel.ReadUint32(drawingContentState);
    if (!res) {
        delete windowDrawingContentInfo;
        return nullptr;
    }
    windowDrawingContentInfo->drawingContentState_ = static_cast<bool>(drawingContentState);
    windowDrawingContentInfo->windowType_ = static_cast<WindowType>(parcel.ReadUint32());
    return windowDrawingContentInfo;
}
} // namespace OHOS::Rosen
