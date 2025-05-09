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

#include "display_ffi.h"

#include "cj_display_impl.h"
#include "cj_display_manager.h"
#include "ffi_remote_data.h"
#include "window_manager_hilog.h"

using namespace OHOS::FFI;

namespace OHOS {
namespace Rosen {
extern "C" {
const int32_t DM_ERROR_SYSTEM_INNORMAL = 1400003;

RetStruct FfiOHOSGetDefaultDisplaySync()
{
    TLOGI(WmsLogTag::DMS, "start");
    return CJDisplayManager::GetDefaultDisplaySync();
}

RetStruct FfiOHOSGetAllDisplays()
{
    TLOGI(WmsLogTag::DMS, "start");
    return CJDisplayManager::GetAllDisplays();
}

RetStruct FfiOHOSHasPrivateWindow(uint32_t displayId)
{
    TLOGI(WmsLogTag::DMS, "start");
    return CJDisplayManager::HasPrivateWindow(displayId);
}

bool FfiOHOSIsFoldable()
{
    TLOGI(WmsLogTag::DMS, "start");
    return CJDisplayManager::IsFoldable();
}

void FfiOHOSSetFoldDisplayMode(uint32_t mode)
{
    TLOGI(WmsLogTag::DMS, "start");
    CJDisplayManager::SetFoldDisplayMode(mode);
}

uint32_t FfiOHOSGetFoldStatus()
{
    TLOGI(WmsLogTag::DMS, "start");
    return CJDisplayManager::GetFoldStatus();
}

uint32_t FfiOHOSGetFoldDisplayMode()
{
    TLOGI(WmsLogTag::DMS, "start");
    return CJDisplayManager::GetFoldDisplayMode();
}

RetStruct FfiOHOSGetCurrentFoldCreaseRegion()
{
    TLOGI(WmsLogTag::DMS, "start");
    return CJDisplayManager::GetCurrentFoldCreaseRegion();
}

bool FfiOHOSIsCaptured()
{
    TLOGD(WmsLogTag::DMS, "start");
    return CJDisplayManager::IsCaptured();
}

RetStruct FfiOHOSGetAllDisplayPhysicalResolution()
{
    TLOGD(WmsLogTag::DMS, "start");
    return CJDisplayManager::GetAllDisplayPhysicalResolution();
}

int32_t FfiOHOSUnRegisterAllDisplayManagerCallback(const char* type)
{
    TLOGD(WmsLogTag::DMS, "start");
    return CJDisplayManager::OnUnregisterAllDisplayListenerWithType(type);
}

int32_t FfiOHOSRegisterDisplayManagerCallback(const char* type, int64_t callbackId)
{
    TLOGD(WmsLogTag::DMS, "start");
    return CJDisplayManager::OnRegisterDisplayListenerWithType(type, callbackId);
}

int32_t FfiOHOSUnRegisterDisplayManagerCallback(const char* type, int64_t callbackId)
{
    TLOGD(WmsLogTag::DMS, "start");
    return CJDisplayManager::OnUnRegisterDisplayListenerWithType(type, callbackId);
}

uint32_t FfiOHOSDisplayGetId(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return 0;
    }
    auto ret = instance->GetInfoId();
    return ret;
}

char* FfiOHOSGetDisplayName(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return nullptr;
    }
    auto ret = instance->GetName();
    return ret;
}

bool FfiOHOSDisplayGetAlive(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return false;
    }
    auto ret = instance->GetAlive();
    return ret;
}

uint32_t FfiOHOSDisplayGetState(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return 0;
    }
    auto ret = instance->GetState();
    return ret;
}

uint32_t FfiOHOSDisplayGetRefreshRate(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return 0;
    }
    auto ret = instance->GetRefreshRate();
    return ret;
}

uint32_t FfiOHOSDisplayGetRotation(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return 0;
    }
    auto ret = instance->GetRotation();
    return ret;
}

uint32_t FfiOHOSDisplayGetOrientation(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return 0;
    }
    auto ret = instance->GetOrientation();
    return ret;
}

int32_t FfiOHOSDisplayGetWidth(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return 0;
    }
    auto ret = instance->GetWidth();
    return ret;
}

int32_t FfiOHOSDisplayGetHeight(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return 0;
    }
    auto ret = instance->GetHeight();
    return ret;
}

float FfiOHOSDisplayGetDensityDPI(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return 0.0;
    }
    auto ret = instance->GetDensityDPI();
    return ret;
}

float FfiOHOSDisplayGetVirtualPixelRatio(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return 0.0;
    }
    auto ret = instance->GetVirtualPixelRatio();
    return ret;
}

float FfiOHOSDisplayGetXDPI(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return 0.0;
    }
    auto ret = instance->GetXDPI();
    return ret;
}

float FfiOHOSDisplayGetYDPI(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return 0.0;
    }
    auto ret = instance->GetYDPI();
    return ret;
}

RetStruct FfiOHOSDisplayGetColorSpaces(int64_t id)
{
    TLOGD(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return { DM_ERROR_SYSTEM_INNORMAL, 0, nullptr };
    }
    auto ret = instance->GetColorSpaces();
    return ret;
}

RetStruct FfiOHOSDisplayGetHdrFormats(int64_t id)
{
    TLOGD(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return { DM_ERROR_SYSTEM_INNORMAL, 0, nullptr };
    }
    auto ret = instance->GetHdrFormats();
    return ret;
}

uint32_t FfiOHOSDisplayGetAvailableWidth(int64_t id)
{
    TLOGD(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return 0.0;
    }
    auto ret = instance->GetAvailableWidth();
    return ret;
}

uint32_t FfiOHOSDisplayGetAvailableHeight(int64_t id)
{
    TLOGD(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return 0.0;
    }
    auto ret = instance->GetAvailableHeight();
    return ret;
}

RetStruct FfiOHOSDisplayGetCutoutInfo(int64_t id)
{
    TLOGD(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return { DM_ERROR_SYSTEM_INNORMAL, 0, nullptr };
    }
    auto ret = instance->GetCutoutInfo();
    return ret;
}

RetStruct FfiOHOSDisplayGetAvailableArea(int64_t id)
{
    TLOGD(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return { DM_ERROR_SYSTEM_INNORMAL, 0, nullptr };
    }
    auto ret = instance->GetAvailableArea();
    return ret;
}

int32_t FfiOHOSDisplayUnRegisterAllDisplayManagerCallback(const char* type, int64_t id)
{
    TLOGD(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return DM_ERROR_SYSTEM_INNORMAL;
    }
    auto ret = instance->OnUnRegisterAllDisplayManagerCallback(type);
    return ret;
}

int32_t FfiOHOSDisplayRegisterDisplayManagerCallback(const char* type, int64_t id, int64_t callbackId)
{
    TLOGD(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return DM_ERROR_SYSTEM_INNORMAL;
    }
    auto ret = instance->OnRegisterDisplayManagerCallback(type, callbackId);
    return ret;
}

int32_t FfiOHOSDisplayUnRegisterDisplayManagerCallback(const char* type, int64_t id, int64_t callbackId)
{
    TLOGD(WmsLogTag::DMS, "start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}" PRId64, id);
        return DM_ERROR_SYSTEM_INNORMAL;
    }
    auto ret = instance->OnUnRegisterDisplayManagerCallback(type, callbackId);
    return ret;
}
}
} // namespace Rosen
} // namespace OHOS
