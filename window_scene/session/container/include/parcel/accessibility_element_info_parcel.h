/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef ACCESSIBILITY_ELEMENT_INFO_PARCEL_H
#define ACCESSIBILITY_ELEMENT_INFO_PARCEL_H

#include "accessibility_element_info.h"
#include "parcel.h"

namespace OHOS {
namespace Rosen {
/*
* class define the action on Rosen info
*/
class AccessibleActionParcel : public Accessibility::AccessibleAction, public Parcelable {
public:
    /**
     * @brief Construct
     */
    AccessibleActionParcel() = default;

    /**
     * @brief Construct
     * @param action The object of AccessibleAction
     */
    AccessibleActionParcel(const Accessibility::AccessibleAction &action);

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return true: Read parcel date successfully; otherwise is not
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Used for IPC communication
     * @param parcel
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return Read AccessibleActionParcel from parcel data
     */
    static sptr<AccessibleActionParcel> Unmarshalling(Parcel &parcel);
};

/**
 * @brief  Define the RangInfo for progress bar
 * @note
 * @retval None
 */
class RangeInfoParcel : public Accessibility::RangeInfo, public Parcelable {
public:
    /**
     * @brief Construct
     */
    RangeInfoParcel() = default;

    /**
     * @brief Construct
     * @param rangeInfo The object of RangeInfo.
     */
    RangeInfoParcel(const Accessibility::RangeInfo &rangeInfo);

    /**
     * @brief Used for IPC communication
     * @param parcel
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Used for IPC communication
     * @param parcel
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Used for IPC communication
     * @param parcel
     */
    static sptr<RangeInfoParcel> Unmarshalling(Parcel &parcel);
};

/**
 * @brief  Define the list/grid component
 * @note
 * @retval None
 */
class GridInfoParcel : public Accessibility::GridInfo, public Parcelable {
public:
    /**
     * @brief Construct
     */
    GridInfoParcel() = default;

    /**
     * @brief Construct
     * @param gridInfo The object of GridInfo.
     */
    GridInfoParcel(const Accessibility::GridInfo &gridInfo);

    /**
     * @brief Used for IPC communication
     * @param parcel
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Used for IPC communication
     * @param parcel
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Used for IPC communication
     * @param parcel
     */
    static sptr<GridInfoParcel> Unmarshalling(Parcel &parcel);
};

class GridItemInfoParcel : public Accessibility::GridItemInfo, public Parcelable {
public:
    /**
     * @brief Construct
     */
    GridItemInfoParcel() = default;

    /**
     * @brief Construct
     * @param itemInfo The object of GridItemInfo.
     */
    GridItemInfoParcel(const Accessibility::GridItemInfo &itemInfo);

    /**
     * @brief Used for IPC communication
     * @param parcel
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Used for IPC communication
     * @param parcel
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Used for IPC communication
     * @param parcel
     */
    static sptr<GridItemInfoParcel> Unmarshalling(Parcel &parcel);
};

class RectParcel : public Accessibility::Rect, public Parcelable {
public:
    /**
     * @brief Construct
     */
    RectParcel() = default;

    /**
     * @brief Construct
     * @param rect The object of Rect.
     */
    RectParcel(const Accessibility::Rect &rect);

    /**
     * @brief Used for IPC communication
     * @param parcel
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Used for IPC communication
     * @param parcel
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Used for IPC communication
     * @param parcel
     */
    static sptr<RectParcel> Unmarshalling(Parcel &parcel);
};

/*
* The class supply the api to set/get ui component property
*/
class AccessibilityElementInfoParcel : public Accessibility::AccessibilityElementInfo, public Parcelable {
public:
    /**
     * @brief Construct
     */
    AccessibilityElementInfoParcel() = default;

    /**
     * @brief Construct
     * @param elementInfo The object of AccessibilityElementInfo.
     */
    AccessibilityElementInfoParcel(const Accessibility::AccessibilityElementInfo &elementInfo);

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return true: Read parcel data successfully; ohterwise is not.
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return true: Write parcel data successfully; ohterwise is not.
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return Read AccessibilityElementInfoParcel from parcel data
     */
    static sptr<AccessibilityElementInfoParcel> Unmarshalling(Parcel &parcel);

private:
     /**
     * @brief Used for IPC communication first part
     * @param parcel
     */
    bool ReadFromParcelFirstPart(Parcel &parcel);

     /**
     * @brief Used for IPC communication second part
     * @param parcel
     */
    bool ReadFromParcelSecondPart(Parcel &parcel);

     /**
     * @brief Used for IPC communication third part
     * @param parcel
     */
    bool ReadFromParcelThirdPart(Parcel &parcel);

    /**
     * @brief Used for IPC communication first part
     * @param parcel
     */
    bool MarshallingFirstPart(Parcel &parcel) const;

    /**
     * @brief Used for IPC communication second part
     * @param parcel
     */
    bool MarshallingSecondPart(Parcel &parcel) const;
};
} // namespace Rosen
} // namespace OHOS
#endif