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

#include "cj_display_impl.h"
#include <map>
#include <securec.h>
#include "cutout_info.h"
#include "display.h"
#include "display_info.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
const std::map<DisplayState,      DisplayStateMode> NATIVE_TO_CJ_DISPLAY_STATE_MAP {
    { DisplayState::UNKNOWN,      DisplayStateMode::STATE_UNKNOWN      },
    { DisplayState::OFF,          DisplayStateMode::STATE_OFF          },
    { DisplayState::ON,           DisplayStateMode::STATE_ON           },
    { DisplayState::DOZE,         DisplayStateMode::STATE_DOZE         },
    { DisplayState::DOZE_SUSPEND, DisplayStateMode::STATE_DOZE_SUSPEND },
    { DisplayState::VR,           DisplayStateMode::STATE_VR           },
    { DisplayState::ON_SUSPEND,   DisplayStateMode::STATE_ON_SUSPEND   },
};
}
static thread_local std::map<uint64_t, sptr<DisplayImpl>> g_cjDisplayMap;
std::recursive_mutex g_mutex;

void SetCRect(const DMRect &row, CRect *ptr)
{
    ptr->left = row.posX_;
    ptr->top = row.posY_;
    ptr->width = row.width_;
    ptr->height = row.height_;
}

CRect* CreateCBoundingRects(std::vector<DMRect> &bound)
{
    int32_t number = static_cast<int32_t>(bound.size());
    CRect *result = static_cast<CRect*>(malloc(sizeof(CRect) * number));
    if (result == nullptr) {
        TLOGE(WmsLogTag::DMS, "[CreateCBoundingRects] memory failed.");
        return nullptr;
    }
    for (int i = 0; i < number; i++) {
        SetCRect(bound[i], (result + i));
    }
    return result;
}

void SetCWaterfallDisplayAreaRects(const WaterfallDisplayAreaRects &area, CCutoutInfo * info)
{
    if (info == nullptr || info->boundingRects == nullptr) {
        return;
    }
    SetCRect(area.left, &(info->waterfallDisplayAreaRects.left));
    SetCRect(area.top, &(info->waterfallDisplayAreaRects.top));
    SetCRect(area.right, &(info->waterfallDisplayAreaRects.right));
    SetCRect(area.bottom, &(info->waterfallDisplayAreaRects.bottom));
}

CCutoutInfo* CreateCCutoutInfoObject(sptr<CutoutInfo> &cutoutInfo)
{
    CCutoutInfo *info = static_cast<CCutoutInfo*>(malloc(sizeof(CCutoutInfo)));
    if (info == nullptr) {
        return nullptr;
    }
    std::vector<DMRect> boundingRects = cutoutInfo->GetBoundingRects();
    WaterfallDisplayAreaRects waterfallDisplayAreaRects = cutoutInfo->GetWaterfallDisplayAreaRects();
    info->number = static_cast<int64_t>(boundingRects.size());
    info->boundingRects = CreateCBoundingRects(boundingRects);
    if (info->boundingRects == nullptr) {
        TLOGE(WmsLogTag::DMS, "[CreateCCutoutInfoObject] memory failed.");
        free(info);
        return nullptr;
    }
    SetCWaterfallDisplayAreaRects(waterfallDisplayAreaRects, info);
    return info;
}

sptr<DisplayImpl> DisplayImpl::FindDisplayObject(uint64_t displayId)
{
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if (g_cjDisplayMap.find(displayId) == g_cjDisplayMap.end()) {
        TLOGI(WmsLogTag::DMS,
            "[FindDisplayObject] Can not find display %{public}" PRIu64" in display map", displayId);
        return nullptr;
    }
    return g_cjDisplayMap[displayId];
}

DisplayImpl::DisplayImpl(const sptr<Display>& display) : display_(display)
{}

DisplayImpl::~DisplayImpl()
{
    uint64_t displayId = display_->GetId();
    sptr<DisplayImpl> cjDisplayPtr = DisplayImpl::FindDisplayObject(displayId);
    if (cjDisplayPtr == nullptr) {
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    g_cjDisplayMap.erase(displayId);
    return;
}

sptr<DisplayImpl> DisplayImpl::CreateDisplayImpl(sptr<Display> &display)
{
    uint64_t displayId = display->GetId();
    sptr<DisplayImpl> cjDisplayPtr = DisplayImpl::FindDisplayObject(displayId);
    auto info = display->GetDisplayInfo();
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "[CreateDisplayImpl] Failed to get display info");
        return nullptr;
    }
    if (cjDisplayPtr == nullptr) {
        cjDisplayPtr = FFIData::Create<DisplayImpl>(display);
        if (cjDisplayPtr == nullptr) {
            TLOGE(WmsLogTag::DMS, "[CreateDisplayImpl] Failed to create display");
            return nullptr;
        }
        std::lock_guard<std::recursive_mutex> lock(g_mutex);
        g_cjDisplayMap[displayId] = cjDisplayPtr;
    }
    return cjDisplayPtr;
}

uint32_t DisplayImpl::GetInfoId()
{
    auto info = display_->GetDisplayInfo();
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "[GetInfoId] Failed to get display info");
        return static_cast<uint32_t>(DISPLAY_ID_INVALID);
    }
    return static_cast<uint32_t>(info->GetDisplayId());
}

char* DisplayImpl::GetName()
{
    auto info = display_->GetDisplayInfo();
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "[GetName] Failed to get display info");
        return nullptr;
    }
    auto name = info->GetName();
    int len = static_cast<int>(name.length());
    char *retData = static_cast<char*>(malloc(len + 1));
    if (retData == nullptr) {
        return nullptr;
    }
    int ret = memcpy_s(retData, len + 1, name.c_str(), len + 1);
    if (ret != 0) {
        TLOGE(WmsLogTag::DMS, "[DisplayImpl] Failed to get name");
        free(retData);
        retData = nullptr;
    }
    return retData;
}

bool DisplayImpl::GetAlive()
{
    auto info = display_->GetDisplayInfo();
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "[GetAlive] Failed to get display info");
        return false;
    }
    return info->GetAliveStatus();
}

uint32_t DisplayImpl::GetState()
{
    auto info = display_->GetDisplayInfo();
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "[GetState] Failed to get display info");
        return static_cast<uint32_t>(DisplayStateMode::STATE_UNKNOWN);
    }
    auto state = info->GetDisplayState();
    if (NATIVE_TO_CJ_DISPLAY_STATE_MAP.count(state) != 0) {
        return static_cast<uint32_t>(NATIVE_TO_CJ_DISPLAY_STATE_MAP.at(state));
    } else {
        return static_cast<uint32_t>(DisplayStateMode::STATE_UNKNOWN);
    }
}

uint32_t DisplayImpl::GetRefreshRate()
{
    auto info = display_->GetDisplayInfo();
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "[GetRefreshRate] Failed to get display info");
        return 0;
    }
    return info->GetRefreshRate();
}

uint32_t DisplayImpl::GetRotation()
{
    auto info = display_->GetDisplayInfo();
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "[GetRotation] Failed to get display info");
        return 0;
    }
    return static_cast<uint32_t>(info->GetRotation());
}

uint32_t DisplayImpl::GetOrientation()
{
    auto info = display_->GetDisplayInfo();
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "[GetOrientation] Failed to get display info");
        return 0;
    }
    return static_cast<uint32_t>(info->GetDisplayOrientation());
}

int32_t DisplayImpl::GetWidth()
{
    auto info = display_->GetDisplayInfo();
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "[GetWidth] Failed to get display info");
        return 0;
    }
    return info->GetWidth();
}

int32_t DisplayImpl::GetHeight()
{
    auto info = display_->GetDisplayInfo();
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "[GetHeight] Failed to get display info");
        return 0;
    }
    return info->GetHeight();
}

float DisplayImpl::GetDensityDPI()
{
    auto info = display_->GetDisplayInfo();
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "[GetDensityDPI] Failed to get display info");
        return 1.0 * DOT_PER_INCH;
    }
    return info->GetVirtualPixelRatio() * DOT_PER_INCH;
}

float DisplayImpl::GetVirtualPixelRatio()
{
    auto info = display_->GetDisplayInfo();
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "[GetVirtualPixelRatio] Failed to get display info");
        return 1.0;
    }
    return info->GetVirtualPixelRatio();
}

float DisplayImpl::GetXDPI()
{
    auto info = display_->GetDisplayInfo();
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "[GetXDPI] Failed to get display info");
        return 0.0;
    }
    return info->GetXDpi();
}

float DisplayImpl::GetYDPI()
{
    auto info = display_->GetDisplayInfo();
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "[GetYDPI] Failed to get display info");
        return 0.0;
    }
    return info->GetYDpi();
}

RetStruct DisplayImpl::GetCutoutInfo()
{
    RetStruct result = {.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL), .len = 0, .data = nullptr};
    sptr<CutoutInfo> cutoutInfo = display_->GetCutoutInfo();
    if (cutoutInfo == nullptr) {
        result.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN);
        return result;
    }
    result.data = CreateCCutoutInfoObject(cutoutInfo);
    result.code = static_cast<int32_t>(DmErrorCode::DM_OK);
    if (result.data == nullptr) {
        result.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL);
    }
    return result;
}
}
}
