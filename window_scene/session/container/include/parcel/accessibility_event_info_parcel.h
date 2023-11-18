/*
* Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef ACCESSIBILITY_EVENT_INFO_PARCEL_H
#define ACCESSIBILITY_EVENT_INFO_PARCEL_H

#include "accessibility_event_info.h"
#include "parcel.h"

namespace OHOS {
namespace Rosen {
class AccessibilityEventInfoParcel : public Accessibility::AccessibilityEventInfo, public Parcelable {
public:
    /**
     * @brief Construct
     * @since 3
     */
    AccessibilityEventInfoParcel() {}

    /**
     * @brief Construct
     * @param eventInfo The object of AccessibilityEventInfo.
     * @since 3
     */
    AccessibilityEventInfoParcel(const Accessibility::AccessibilityEventInfo& eventInfo);

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return true: Read parcel data successfully; otherwise is not.
     * @since 3
     */
    bool ReadFromParcel(Parcel& parcel);

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return true: Write parcel data successfully; otherwise is not.
     * @since 3
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return Read AccessibilityEventInfoParcel from parcel data
     * @since 3
     */
    static sptr<AccessibilityEventInfoParcel> Unmarshalling(Parcel& parcel);

private:
    /**
     * @brief Used for IPC communication first part
     * @param parcel
     * @return true: Read parcel data successfully; otherwise is not.
     */
    bool ReadFromParcelFirstPart(Parcel& parcel);

    /**
     * @brief Used for IPC communication second part
     * @param parcel
     * @return true: Read parcel data successfully; otherwise is not.
     */
    bool ReadFromParcelSecondPart(Parcel& parcel);
};
} // namespace Rosen
} // namespace OHOS
#endif // ACCESSIBILITY_EVENT_INFO_PARCEL_H
