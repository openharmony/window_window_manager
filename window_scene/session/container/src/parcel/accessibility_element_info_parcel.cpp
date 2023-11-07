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

#include "accessibility_element_info_parcel.h"
#include "parcel_util.h"
#include "window_manager_hilog.h"

using namespace OHOS::Accessibility;
namespace OHOS::Rosen {
/* AccessibilityElementInfoParcel       Parcel struct                 */
AccessibilityElementInfoParcel::AccessibilityElementInfoParcel(
    const Accessibility::AccessibilityElementInfo &elementInfo)
{
    WLOGFD();

    Accessibility::AccessibilityElementInfo *self = this;
    *self = elementInfo;
}

bool AccessibilityElementInfoParcel::ReadFromParcelFirstPart(Parcel &parcel)
{
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, pageId_);
    int32_t textMoveStep = STEP_CHARACTER;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, textMoveStep);
    textMoveStep_ = static_cast<TextMoveUnit>(textMoveStep);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, itemCounts_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, windowId_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, elementId_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, parentId_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, bundleName_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, componentType_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, text_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, hintText_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, contentDescription_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, resourceName_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32Vector, parcel, &childNodeIds_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, childCount_);
    return true;
}

bool AccessibilityElementInfoParcel::ReadFromParcelSecondPart(Parcel &parcel)
{
    int32_t operationsSize = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, operationsSize);
    ContainerSecurityVerify(parcel, operationsSize, operations_.max_size());
    for (int32_t i = 0; i < operationsSize; i++) {
        sptr<AccessibleActionParcel> accessibleOperation = parcel.ReadStrongParcelable<AccessibleActionParcel>();
        if (!accessibleOperation) {
            WLOGFE("ReadStrongParcelable<accessibleOperation> failed");
            return false;
        }
        operations_.emplace_back(*accessibleOperation);
    }

    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, textLengthLimit_);
    sptr<RectParcel> rect = parcel.ReadStrongParcelable<RectParcel>();
    if (!rect) {
        return false;
    }
    bounds_ = *rect;

    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, checkable_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, checked_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, focusable_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, focused_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, visible_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, accessibilityFocused_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, selected_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, clickable_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, longClickable_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, enable_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isPassword_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, scrollable_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, editable_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, popupSupported_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, multiLine_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, deletable_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, hint_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isEssential_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, currentIndex_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, beginIndex_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, endIndex_);
    return true;
}

bool AccessibilityElementInfoParcel::ReadFromParcelThirdPart(Parcel &parcel)
{
    sptr<RangeInfoParcel> rangeInfo = parcel.ReadStrongParcelable<RangeInfoParcel>();
    if (!rangeInfo) {
        return false;
    }
    rangeInfo_ = *rangeInfo;

    sptr<GridInfoParcel> grid = parcel.ReadStrongParcelable<GridInfoParcel>();
    if (!grid) {
        return false;
    }
    grid_ = *grid;

    sptr<GridItemInfoParcel> gridItem = parcel.ReadStrongParcelable<GridItemInfoParcel>();
    if (!gridItem) {
        return false;
    }
    gridItem_ = *gridItem;

    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, liveRegion_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, contentInvalid_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, error_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, labeled_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, beginSelected_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, endSelected_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, inputType_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, validElement_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, inspectorKey_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, pagePath_);
    return true;
}

bool AccessibilityElementInfoParcel::ReadFromParcel(Parcel &parcel)
{
    WLOGFD();
    if (!ReadFromParcelFirstPart(parcel)) {
        return false;
    }
    if (!ReadFromParcelSecondPart(parcel)) {
        return false;
    }
    if (!ReadFromParcelThirdPart(parcel)) {
        return false;
    }
    return true;
}

bool AccessibilityElementInfoParcel::MarshallingFirstPart(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, pageId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(textMoveStep_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, itemCounts_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, windowId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, elementId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, parentId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, bundleName_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, componentType_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, text_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, hintText_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, contentDescription_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, resourceName_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32Vector, parcel, childNodeIds_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, childCount_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, operations_.size());
    for (auto &operations : operations_) {
        AccessibleActionParcel action(operations);
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &action);
    }
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, textLengthLimit_);
    return true;
}

bool AccessibilityElementInfoParcel::MarshallingSecondPart(Parcel &parcel) const
{
    RectParcel boundsParcel(bounds_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &boundsParcel);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, checkable_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, checked_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, focusable_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, focused_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, visible_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, accessibilityFocused_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, selected_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, clickable_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, longClickable_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, enable_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isPassword_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, scrollable_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, editable_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, popupSupported_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, multiLine_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, deletable_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, hint_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isEssential_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, currentIndex_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, beginIndex_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, endIndex_);
    RangeInfoParcel rangeInfoParcel(rangeInfo_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &rangeInfoParcel);
    GridInfoParcel gridParcel(grid_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &gridParcel);
    GridItemInfoParcel gridItemParcel(gridItem_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &gridItemParcel);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, liveRegion_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, contentInvalid_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, error_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, labeled_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, beginSelected_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, endSelected_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, inputType_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, validElement_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, inspectorKey_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, pagePath_);
    return true;
}

bool AccessibilityElementInfoParcel::Marshalling(Parcel &parcel) const
{
    WLOGFD();
    if (!MarshallingFirstPart(parcel)) {
        return false;
    }
    if (!MarshallingSecondPart(parcel)) {
        return false;
    }
    return true;
}

sptr<AccessibilityElementInfoParcel> AccessibilityElementInfoParcel::Unmarshalling(Parcel& parcel)
{
    WLOGD();

    sptr<AccessibilityElementInfoParcel> accessibilityInfo = new(std::nothrow) AccessibilityElementInfoParcel();
    if (!accessibilityInfo) {
        WLOGFE("Failed to create accessibilityInfo.");
        return nullptr;
    }
    if (!accessibilityInfo->ReadFromParcel(parcel)) {
        WLOGFE("read from parcel failed");
        return nullptr;
    }
    return accessibilityInfo;
}

AccessibleActionParcel::AccessibleActionParcel(const AccessibleAction &action)
{
    WLOGFD();

    AccessibleAction *self = this;
    *self = action;
}

bool AccessibleActionParcel::ReadFromParcel(Parcel &parcel)
{
    WLOGFD();
    int32_t type = ActionType::ACCESSIBILITY_ACTION_INVALID;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, type);
    actionType_ = static_cast<ActionType>(type);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, description_);
    return true;
}

bool AccessibleActionParcel::Marshalling(Parcel &parcel) const
{
    WLOGFD();
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(actionType_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, description_);

    return true;
}

sptr<AccessibleActionParcel> AccessibleActionParcel::Unmarshalling(Parcel& parcel)
{
    WLOGFD();
    sptr<AccessibleActionParcel> accessibleOperation = new(std::nothrow) AccessibleActionParcel();
    if (!accessibleOperation) {
        WLOGFE("Failed to create accessibleOperation.");
        return nullptr;
    }
    if (!accessibleOperation->ReadFromParcel(parcel)) {
        WLOGFE("read from parcel failed");
        return nullptr;
    }
    return accessibleOperation;
}

RangeInfoParcel::RangeInfoParcel(const RangeInfo &rangeInfo)
{
    WLOGFD();

    RangeInfo *self = this;
    *self = rangeInfo;
}

bool RangeInfoParcel::ReadFromParcel(Parcel &parcel)
{
    WLOGFD();
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, min_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, max_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, current_);

    return true;
}

bool RangeInfoParcel::Marshalling(Parcel &parcel) const
{
    WLOGFD();
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, min_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, max_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, current_);

    return true;
}

sptr<RangeInfoParcel> RangeInfoParcel::Unmarshalling(Parcel& parcel)
{
    WLOGFD();
    sptr<RangeInfoParcel> rangeInfo = new(std::nothrow) RangeInfoParcel();
    if (!rangeInfo) {
        WLOGFE("Failed to create rangeInfo.");
        return nullptr;
    }
    if (!rangeInfo->ReadFromParcel(parcel)) {
        WLOGFE("read from parcel failed");
        return nullptr;
    }
    return rangeInfo;
}

GridInfoParcel::GridInfoParcel(const GridInfo &gridInfo)
{
    WLOGFD();

    GridInfo *self = this;
    *self = gridInfo;
}

bool GridInfoParcel::ReadFromParcel(Parcel &parcel)
{
    WLOGFD();
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rowCount_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, columnCount_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, selectionMode_);

    return true;
}

bool GridInfoParcel::Marshalling(Parcel &parcel) const
{
    WLOGFD();
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rowCount_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, columnCount_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, selectionMode_);

    return true;
}

sptr<GridInfoParcel> GridInfoParcel::Unmarshalling(Parcel& parcel)
{
    WLOGFD();
    sptr<GridInfoParcel> grid = new(std::nothrow) GridInfoParcel();
    if (!grid) {
        WLOGFE("Failed to create grid.");
        return nullptr;
    }
    if (!grid->ReadFromParcel(parcel)) {
        WLOGFE("read from parcel failed");
        return nullptr;
    }
    return grid;
}

GridItemInfoParcel::GridItemInfoParcel(const GridItemInfo &itemInfo)
{
    WLOGFD();

    GridItemInfo *self = this;
    *self = itemInfo;
}

bool GridItemInfoParcel::ReadFromParcel(Parcel &parcel)
{
    WLOGFD();
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, heading_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, columnIndex_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rowIndex_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, columnSpan_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rowSpan_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, selected_);

    return true;
}

bool GridItemInfoParcel::Marshalling(Parcel &parcel) const
{
    WLOGFD();
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, heading_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, columnIndex_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rowIndex_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, columnSpan_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rowSpan_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, selected_);

    return true;
}

sptr<GridItemInfoParcel> GridItemInfoParcel::Unmarshalling(Parcel& parcel)
{
    WLOGFD();
    sptr<GridItemInfoParcel> gridItem = new(std::nothrow) GridItemInfoParcel();
    if (!gridItem) {
        WLOGFE("Failed to create gridItem.");
        return nullptr;
    }
    if (!gridItem->ReadFromParcel(parcel)) {
        WLOGFE("read from parcel failed");
        return nullptr;
    }
    return gridItem;
}

RectParcel::RectParcel(const Rect &rect)
{
    WLOGFD();

    Rect *self = this;
    *self = rect;
}

bool RectParcel::ReadFromParcel(Parcel &parcel)
{
    WLOGFD();
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, leftTopX_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, leftTopY_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rightBottomX_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rightBottomY_);
    return true;
}

bool RectParcel::Marshalling(Parcel &parcel) const
{
    WLOGFD();
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, leftTopX_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, leftTopY_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rightBottomX_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, rightBottomY_);
    return true;
}

sptr<RectParcel> RectParcel::Unmarshalling(Parcel& parcel)
{
    WLOGFD();
    sptr<RectParcel> rect = new(std::nothrow) RectParcel();
    if (!rect) {
        WLOGFE("Failed to create rect.");
        return nullptr;
    }
    if (!rect->ReadFromParcel(parcel)) {
        WLOGFE("read from parcel failed");
        return nullptr;
    }
    return rect;
}
}  // namespace OHOS::Rosen