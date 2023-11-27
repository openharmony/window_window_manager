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

#include "display_change_info.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "DisplayChangeInfo"};
}
bool DisplayChangeInfo::Marshalling(Parcel& parcel) const
{
    return parcel.WriteUint32(static_cast<uint32_t>(action_)) && parcel.WriteUint64(toScreenId_) &&
        parcel.WriteUint64(fromScreenId_) && parcel.WriteString(abilityName_) && parcel.WriteString(bundleName_);
}

DisplayChangeInfo* DisplayChangeInfo::Unmarshalling(Parcel& parcel)
{
    auto info = new (std::nothrow) DisplayChangeInfo();
    if (!info) {
        WLOGFE("new info failed");
        return nullptr;
    }
    info->action_ = static_cast<ActionType>(parcel.ReadUint32());
    bool res = parcel.ReadUint64(info->toScreenId_) && parcel.ReadUint64(info->fromScreenId_) &&
        parcel.ReadString(info->abilityName_) && parcel.ReadString(info->bundleName_);
    if (!res) {
        delete info;
        WLOGFE("read info failed");
        return nullptr;
    }
    return info;
}
} // namespace Rosen
} // namepsace OHOS