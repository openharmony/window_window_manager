/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cj_display_manager.h"
#include <securec.h>
#include "cj_display_impl.h"
#include "display_manager.h"
#include "dm_common.h"
#include "singleton_container.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
static void SetDisplayObject(sptr<Display> &obj, RetStruct &ret)
{
    auto result = DisplayImpl::CreateDisplayImpl(obj);
    if (result == nullptr || ret.data == nullptr) {
        TLOGE(WmsLogTag::DMS, "[GetDefaultDisplaySync] ERROR Failed to create DisplayImpl.");
        ret.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL);
        return;
    }
    int64_t *dataPtr = static_cast<int64_t*>(ret.data);
    if (dataPtr == nullptr) {
        TLOGE(WmsLogTag::DMS, "[SetDisplayObject] ERROR Failed to create dataPtr.");
        ret.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL);
        return;
    }
    dataPtr[ret.len] = result->GetID();
    ret.len += 1;
    ret.code = static_cast<int32_t>(DmErrorCode::DM_OK);
}

static void SetDisplaysArrayObject(std::vector<sptr<Display>> &list, RetStruct &ret)
{
    int64_t *displayImplIdList = static_cast<int64_t*>(malloc(sizeof(int64_t) * list.size()));
    if (displayImplIdList == nullptr) {
        TLOGE(WmsLogTag::DMS, "[SetDisplaysArrayObject] ERROR Failed to create displayImplIdList.");
        ret.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL);
        return;
    }
    ret.data = displayImplIdList;
    ret.len = 0;
    for (auto& display : list) {
        if (display == nullptr) {
            continue;
        }
        SetDisplayObject(display, ret);
        if (ret.code != 0) {
            TLOGE(WmsLogTag::DMS,
                "[SetDisplaysArrayObject] ERROR Create display failed in %{public}" PRId64" obj", ret.len);
            free(displayImplIdList);
            ret.data = nullptr;
            return;
        }
    }
}

static void SetCRect(const DMRect &row, CRect *ptr)
{
    ptr->left = row.posX_;
    ptr->top = row.posY_;
    ptr->width = row.width_;
    ptr->height = row.height_;
}

static CRect* CreateCreaseRects(std::vector<DMRect> &list)
{
    int32_t number = static_cast<int32_t>(list.size());
    CRect *result = static_cast<CRect*>(malloc(sizeof(CRect) * number));
    if (result == nullptr) {
        return nullptr;
    }
    for (int i = 0; i < number; i++) {
        SetCRect(list[i], (result + i));
    }
    return result;
}

static CFoldCreaseRegion* CreateCFoldCreaseRegionObject(sptr<FoldCreaseRegion> &foldCreaseRegion)
{
    CFoldCreaseRegion *region = static_cast<CFoldCreaseRegion*>(malloc(sizeof(CFoldCreaseRegion)));
    if (region == nullptr) {
        return nullptr;
    }
    int ret = memset_s(region, sizeof(CFoldCreaseRegion), 0, sizeof(CFoldCreaseRegion));
    if (ret != 0) {
        free(region);
        return nullptr;
    }
    region->displayId = static_cast<uint32_t>(foldCreaseRegion->GetDisplayId());
    std::vector<DMRect> creaseRects = foldCreaseRegion->GetCreaseRects();

    region->number = static_cast<int64_t>(creaseRects.size());
    region->creaseRects = CreateCreaseRects(creaseRects);
    if (region->creaseRects == nullptr) {
        TLOGE(WmsLogTag::DMS, "[CreateCreaseRects] ERROR Failed to create creaseRects.");
        free(region);
        return nullptr;
    }
    return region;
}

RetStruct CJDisplayManager::GetDefaultDisplaySync()
{
    RetStruct ret = {.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL), .len = 0, .data = nullptr};
    sptr<Display> display = SingletonContainer::Get<DisplayManager>().GetDefaultDisplaySync();
    if (display == nullptr) {
        TLOGE(WmsLogTag::DMS, "[DisplayManager] Get default display is nullptr");
        ret.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN);
        return ret;
    }
    int64_t *displayImplId = static_cast<int64_t*>(malloc(sizeof(int64_t)));
    if (displayImplId == nullptr) {
        TLOGE(WmsLogTag::DMS, "[GetDefaultDisplaySync] ERROR Failed to create displayImplId.");
        ret.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN);
        return ret;
    }
    ret.data = displayImplId;
    ret.len = 0;
    SetDisplayObject(display, ret);
    return ret;
}

RetStruct CJDisplayManager::GetAllDisplays()
{
    RetStruct ret = {.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL), .len = 0, .data = nullptr};
    std::vector<sptr<Display>> displays = SingletonContainer::Get<DisplayManager>().GetAllDisplays();
    if (displays.empty()) {
        ret.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN);
        TLOGE(WmsLogTag::DMS, "[GetAllDisplays] ERROR Failed to get all displays.");
        return ret;
    }
    SetDisplaysArrayObject(displays, ret);
    return ret;
}

RetStruct CJDisplayManager::HasPrivateWindow(uint32_t displayId)
{
    RetStruct ret = {.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL), .len = 0, .data = nullptr};
    int64_t displayId_ = static_cast<int64_t>(displayId);
    bool *hasPrivateWindow = static_cast<bool*>(malloc(sizeof(bool)));
    if (hasPrivateWindow == nullptr) {
        ret.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL);
        return ret;
    }
    *hasPrivateWindow = false;
    DmErrorCode errorCode = DM_JS_TO_ERROR_CODE_MAP.at(
        SingletonContainer::Get<DisplayManager>().HasPrivateWindow(displayId_, *hasPrivateWindow));
    TLOGI(WmsLogTag::DMS,
        "[DisplayManager] Display id = %{public}" PRIu64", hasPrivateWindow = %{public}u err = %{public}d",
         static_cast<uint64_t>(displayId_), *hasPrivateWindow, errorCode);
    if (errorCode != DmErrorCode::DM_OK) {
        ret.code = static_cast<int32_t>(errorCode);
        free(hasPrivateWindow);
        return ret;
    }
    ret.code = static_cast<int32_t>(DmErrorCode::DM_OK);
    ret.data = static_cast<void*>(hasPrivateWindow);
    return ret;
}

bool CJDisplayManager::IsFoldable()
{
    return SingletonContainer::Get<DisplayManager>().IsFoldable();
}

void CJDisplayManager::SetFoldDisplayMode(uint32_t mode)
{
    FoldDisplayMode innerMode = static_cast<FoldDisplayMode>(mode);
    SingletonContainer::Get<DisplayManager>().SetFoldDisplayMode(innerMode);
}

uint32_t CJDisplayManager::GetFoldStatus()
{
    return static_cast<uint32_t>(SingletonContainer::Get<DisplayManager>().GetFoldStatus());
}

uint32_t CJDisplayManager::GetFoldDisplayMode()
{
    return static_cast<uint32_t>(SingletonContainer::Get<DisplayManager>().GetFoldDisplayMode());
}

RetStruct CJDisplayManager::GetCurrentFoldCreaseRegion()
{
    RetStruct result = {.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL), .len = 0, .data = nullptr};
    sptr<FoldCreaseRegion> region = SingletonContainer::Get<DisplayManager>().GetCurrentFoldCreaseRegion();
    if (region == nullptr) {
        result.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL);
        return result;
    }
    result.data = CreateCFoldCreaseRegionObject(region);
    result.code = static_cast<int32_t>(DmErrorCode::DM_OK);
    if (result.data == nullptr) {
        result.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL);
    }
    return result;
}
}
}
