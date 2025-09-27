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

#ifndef OHOS_ROSEN_FOCUS_NOTIFY_INFO_H
#define OHOS_ROSEN_FOCUS_NOTIFY_INFO_H

#include <cstdint>
#include <parcel.h>
#include <iremote_object.h>

#include "wm_common.h"

namespace OHOS::Rosen {
class FocusNotifyInfo : public Parcelable {
public:
    /**
     * @brief Default construct of FocusNotifyInfo
     */
    FocusNotifyInfo() = default;

    /**
     * @brief Construct of FocusNotifyInfo
     */
    FocusNotifyInfo(int64_t timeStamp, int32_t unfocusWindowId, int32_t focusWindowId, bool isSyncNotify)
        : timeStamp_(timeStamp),
          unfocusWindowId_(unfocusWindowId),
          focusWindowId_(focusWindowId),
          isSyncNotify_(isSyncNotify) {};

    /**
     * @brief Deconstruct of FocusNotifyInfo
     */
    ~FocusNotifyInfo() = default;

    /**
     * @brief Marshalling FocusNotifyInfo.
     *
     * @param parcel Package of FocusNotifyInfo.
     * @return True means marshall FocusNotifyInfo success, false means marshall failed.
     */
    virtual bool Marshalling(Parcel& parcel) const
    {
        return parcel.WriteInt64(timeStamp_) && parcel.WriteInt32(unfocusWindowId_) &&
            parcel.WriteInt32(focusWindowId_) && parcel.WriteBool(isSyncNotify_);
    }

    static FocusNotifyInfo* Unmarshalling(Parcel& parcel)
    {
        auto focusNotifyInfo = new FocusNotifyInfo();
        bool res = parcel.ReadInt64(focusNotifyInfo->timeStamp_) &&
            parcel.ReadInt32(focusNotifyInfo->unfocusWindowId_) && parcel.ReadInt32(focusNotifyInfo->focusWindowId_) &&
            parcel.ReadBool(focusNotifyInfo->isSyncNotify_);
        if (!res) {
            delete focusNotifyInfo;
            return nullptr;
        }
        return focusNotifyInfo;
    }

    int64_t timeStamp_ = 0;
    int32_t unfocusWindowId_ = INVALID_WINDOW_ID;
    int32_t focusWindowId_ = INVALID_WINDOW_ID;
    bool isSyncNotify_ = false;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_FOCUS_CHANGE_INFO_H