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

#include "cj_display_listener.h"
#include "cutout_info.h"
#include "display.h"
#include "display_info.h"
#include "display_utils.h"
#include "singleton_container.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
const std::map<DisplayState, DisplayStateMode> NATIVE_TO_CJ_DISPLAY_STATE_MAP {
    { DisplayState::UNKNOWN, DisplayStateMode::STATE_UNKNOWN },
    { DisplayState::OFF, DisplayStateMode::STATE_OFF },
    { DisplayState::ON, DisplayStateMode::STATE_ON },
    { DisplayState::DOZE, DisplayStateMode::STATE_DOZE },
    { DisplayState::DOZE_SUSPEND, DisplayStateMode::STATE_DOZE_SUSPEND },
    { DisplayState::VR, DisplayStateMode::STATE_VR },
    { DisplayState::ON_SUSPEND, DisplayStateMode::STATE_ON_SUSPEND },
};
}
static thread_local std::map<uint64_t, sptr<DisplayImpl>> g_cjDisplayMap;
std::map<std::string, std::map<int64_t, sptr<CJDisplayListener>>> g_cjCbMap;
std::mutex g_mtx;
std::recursive_mutex g_mutex;

void SetCRect(const DMRect& row, CRect* ptr)
{
    ptr->left = row.posX_;
    ptr->top = row.posY_;
    ptr->width = row.width_;
    ptr->height = row.height_;
}

uint32_t* CreateColorSpacesObject(std::vector<uint32_t>& colorSpaces)
{
    uint32_t* colorSpacesPtr = static_cast<uint32_t*>(malloc(colorSpaces.size() * sizeof(uint32_t)));
    if (!colorSpacesPtr) {
        return nullptr;
    }
    std::copy(colorSpaces.begin(), colorSpaces.end(), colorSpacesPtr);
    return colorSpacesPtr;
}

uint32_t* CreateHdrFormatsObject(std::vector<uint32_t>& hdrFormats)
{
    uint32_t* hdrFormatsPtr = static_cast<uint32_t*>(malloc(hdrFormats.size() * sizeof(uint32_t)));
    if (!hdrFormatsPtr) {
        return nullptr;
    }
    std::copy(hdrFormats.begin(), hdrFormats.end(), hdrFormatsPtr);
    return hdrFormatsPtr;
}

CRect* CreateCBoundingRects(std::vector<DMRect>& bound)
{
    int32_t number = static_cast<int32_t>(bound.size());
    CRect* result = static_cast<CRect*>(malloc(sizeof(CRect) * number));
    if (result == nullptr) {
        TLOGE(WmsLogTag::DMS, "[CreateCBoundingRects] memory failed.");
        return nullptr;
    }
    for (int i = 0; i < number; i++) {
        SetCRect(bound[i], (result + i));
    }
    return result;
}

void SetCWaterfallDisplayAreaRects(const WaterfallDisplayAreaRects& area, CCutoutInfo* info)
{
    if (info == nullptr || info->boundingRects == nullptr) {
        return;
    }
    SetCRect(area.left, &(info->waterfallDisplayAreaRects.left));
    SetCRect(area.top, &(info->waterfallDisplayAreaRects.top));
    SetCRect(area.right, &(info->waterfallDisplayAreaRects.right));
    SetCRect(area.bottom, &(info->waterfallDisplayAreaRects.bottom));
}

CCutoutInfo* CreateCCutoutInfoObject(sptr<CutoutInfo>& cutoutInfo)
{
    CCutoutInfo* info = static_cast<CCutoutInfo*>(malloc(sizeof(CCutoutInfo)));
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
        TLOGI(WmsLogTag::DMS, "[FindDisplayObject] Can not find display %{public}" PRIu64 " in display map", displayId);
        return nullptr;
    }
    return g_cjDisplayMap[displayId];
}

DisplayImpl::DisplayImpl(const sptr<Display>& display) : display_(display) {}

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

sptr<DisplayImpl> DisplayImpl::CreateDisplayImpl(sptr<Display>& display)
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
    char* retData = static_cast<char*>(malloc(len + 1));
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

RetStruct DisplayImpl::GetColorSpaces()
{
    auto info = display_->GetDisplayInfo();
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "[GetColorSpaces] Failed to get display info");
        return {};
    }
    auto colorSpaces = info->GetColorSpaces();
    RetStruct result = {
        .code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL), .len = 0, .data = nullptr
    };
    result.data = CreateColorSpacesObject(colorSpaces);
    result.code = static_cast<int32_t>(DmErrorCode::DM_OK);
    if (result.data == nullptr) {
        result.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL);
    }
    result.len = colorSpaces.size();
    return result;
}

RetStruct DisplayImpl::GetHdrFormats()
{
    auto info = display_->GetDisplayInfo();
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "[GetHdrFormats] Failed to get display info");
        return {};
    }
    auto hdrFormats = info->GetHdrFormats();
    RetStruct result = {
        .code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL), .len = 0, .data = nullptr
    };
    result.data = CreateHdrFormatsObject(hdrFormats);
    result.code = static_cast<int32_t>(DmErrorCode::DM_OK);
    if (result.data == nullptr) {
        result.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL);
    }
    result.len = hdrFormats.size();
    return result;
}

uint32_t DisplayImpl::GetAvailableWidth()
{
    auto info = display_->GetDisplayInfo();
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "[GetAvailableWidth] Failed to get display info");
        return 0;
    }
    return info->GetAvailableWidth();
}

uint32_t DisplayImpl::GetAvailableHeight()
{
    auto info = display_->GetDisplayInfo();
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "[GetAvailableHeight] Failed to get display info");
        return 0;
    }
    return info->GetAvailableHeight();
}

RetStruct DisplayImpl::GetCutoutInfo()
{
    RetStruct result = {
        .code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL), .len = 0, .data = nullptr
    };
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

RetStruct DisplayImpl::GetAvailableArea()
{
    RetStruct result = {
        .code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL), .len = 0, .data = nullptr
    };
    DMRect area;
    DmErrorCode errorCode = DM_JS_TO_ERROR_CODE_MAP.at(display_->GetAvailableArea(area));
    if (errorCode != DmErrorCode::DM_OK) {
        result.code = static_cast<int32_t>(errorCode);
        TLOGE(WmsLogTag::DMS, "GetAvailableArea failed!");
        return result;
    }
    result.data = static_cast<CRect*>(malloc(sizeof(CRect)));
    if (result.data == nullptr) {
        result.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL);
        TLOGE(WmsLogTag::DMS, "GetAvailableArea failed!");
        return result;
    }
    SetCRect(area, static_cast<CRect*>(result.data));
    result.code = static_cast<int32_t>(DmErrorCode::DM_OK);
    return result;
}

bool IfCallbackRegistered(const std::string& type, int64_t funcId)
{
    if (g_cjCbMap.empty() || g_cjCbMap.find(type) == g_cjCbMap.end()) {
        TLOGI(WmsLogTag::DMS, "IfCallbackRegistered methodName %{public}s not registered!", type.c_str());
        return false;
    }

    for (auto& iter : g_cjCbMap[type]) {
        if (iter.first == funcId) {
            TLOGE(WmsLogTag::DMS, "IfCallbackRegistered callback already registered!");
            return true;
        }
    }
    return false;
}

int32_t DisplayImpl::OnUnRegisterAllDisplayManagerCallback(const std::string& type)
{
    std::lock_guard<std::mutex> lock(g_mtx);
    TLOGD(WmsLogTag::DMS, "DisplayImpl::OnUnRegisterDisplayManagerCallbackWithType is called");
    DmErrorCode ret = DM_JS_TO_ERROR_CODE_MAP.at(UnRegisterAllDisplayListenerWithType(type));
    if (ret != DmErrorCode::DM_OK) {
        TLOGE(WmsLogTag::DMS, "Failed to unregister display listener with type %{public}s.", type.c_str());
        ret = DmErrorCode::DM_ERROR_INVALID_PARAM;
    }
    return static_cast<int32_t>(ret);
}

DMError DisplayImpl::UnRegisterAllDisplayListenerWithType(const std::string& type)
{
    TLOGD(WmsLogTag::DMS, "DisplayImpl::UnRegisterDisplayManagerListenerWithType is called");
    if (g_cjCbMap.empty() || g_cjCbMap.find(type) == g_cjCbMap.end()) {
        TLOGI(WmsLogTag::DMS, "UnRegisterDisplayManagerListenerWithType methodName %{public}s not registered",
            type.c_str());
        return DMError::DM_OK;
    }
    DMError ret = DMError::DM_OK;
    for (auto it = g_cjCbMap[type].begin(); it != g_cjCbMap[type].end();) {
        it->second->RemoveAllCallback();
        if (type == EVENT_AVAILABLE_AREA_CHANGED) {
            sptr<DisplayManager::IAvailableAreaListener> thisListener(it->second);
            ret = SingletonContainer::Get<DisplayManager>().UnregisterAvailableAreaListener(thisListener);
        } else {
            ret = DMError::DM_ERROR_INVALID_PARAM;
        }
        g_cjCbMap[type].erase(it++);
    }
    g_cjCbMap.erase(type);
    return ret;
}

int32_t DisplayImpl::OnRegisterDisplayManagerCallback(const std::string& type, int64_t funcId)
{
    std::lock_guard<std::mutex> lock(g_mtx);
    TLOGD(WmsLogTag::DMS, "DisplayImpl::OnRegisterDisplayManagerCallback is called");
    DmErrorCode ret = DM_JS_TO_ERROR_CODE_MAP.at(RegisterDisplayListenerWithType(type, funcId));
    if (ret != DmErrorCode::DM_OK) {
        TLOGE(WmsLogTag::DMS, "Failed to register display listener with type %{public}s", type.c_str());
        ret = DmErrorCode::DM_ERROR_INVALID_PARAM;
    }
    return static_cast<int32_t>(ret);
}

DMError DisplayImpl::RegisterDisplayListenerWithType(const std::string& type, int64_t funcId)
{
    TLOGD(WmsLogTag::DMS, "DisplayImpl::RegisterDisplayListenerWithType is called");
    if (IfCallbackRegistered(type, funcId)) {
        TLOGI(WmsLogTag::DMS, "RegisterDisplayListenerWithType callback already registered!");
        return DMError::DM_OK;
    }
    sptr<CJDisplayListener> displayListener = new (std::nothrow) CJDisplayListener();
    DMError ret = DMError::DM_OK;
    if (displayListener == nullptr) {
        TLOGE(WmsLogTag::DMS, "displayListener is nullptr");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    if (type == EVENT_AVAILABLE_AREA_CHANGED) {
        ret = SingletonContainer::Get<DisplayManager>().RegisterAvailableAreaListener(displayListener);
    } else {
        TLOGI(WmsLogTag::DMS, "RegisterDisplayListenerWithType failed, %{public}s not support", type.c_str());
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    if (ret != DMError::DM_OK) {
        TLOGE(WmsLogTag::DMS, "RegisterDisplayListenerWithType failed, ret: %{public}u", ret);
        return ret;
    }
    displayListener->AddCallback(type, funcId);
    g_cjCbMap[type][funcId] = displayListener;
    return DMError::DM_OK;
}

int32_t DisplayImpl::OnUnRegisterDisplayManagerCallback(const std::string& type, int64_t funcId)
{
    std::lock_guard<std::mutex> lock(g_mtx);
    TLOGD(WmsLogTag::DMS, "DisplayImpl::OnUnRegisterDisplayManagerCallback is called");
    DmErrorCode ret = DM_JS_TO_ERROR_CODE_MAP.at(UnRegisterDisplayListenerWithType(type, funcId));
    if (ret != DmErrorCode::DM_OK) {
        TLOGE(WmsLogTag::DMS, "Failed to unregister display listener with type %{public}s", type.c_str());
        ret = DmErrorCode::DM_ERROR_INVALID_PARAM;
    }
    return static_cast<int32_t>(ret);
}

DMError DisplayImpl::UnRegisterDisplayListenerWithType(const std::string& type, int64_t funcId)
{
    TLOGD(WmsLogTag::DMS, "DisplayImpl::UnRegisterDisplayListenerWithType is called");
    if (g_cjCbMap.empty() || g_cjCbMap.find(type) == g_cjCbMap.end()) {
        TLOGI(WmsLogTag::DMS, "UnRegisterDisplayListenerWithType methodName %{public}s not registered", type.c_str());
        return DMError::DM_OK;
    }
    DMError ret = DMError::DM_OK;
    for (auto it = g_cjCbMap[type].begin(); it != g_cjCbMap[type].end();) {
        if (it->first == funcId) {
            it->second->RemoveAllCallback();
            if (type == EVENT_AVAILABLE_AREA_CHANGED) {
                sptr<DisplayManager::IAvailableAreaListener> thisListener(it->second);
                ret = SingletonContainer::Get<DisplayManager>().UnregisterAvailableAreaListener(thisListener);
            } else {
                ret = DMError::DM_ERROR_INVALID_PARAM;
            }
            g_cjCbMap[type].erase(it++);
            break;
        } else {
            it++;
        }
    }
    if (g_cjCbMap[type].empty()) {
        g_cjCbMap.erase(type);
    }
    return ret;
}
} // namespace Rosen
} // namespace OHOS
