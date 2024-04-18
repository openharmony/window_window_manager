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
    TLOGI(WmsLogTag::DMS, "FfiOHOSGetDefaultDisplaySync start");
    return CJDisplayManager::GetDefaultDisplaySync();
}

RetStruct FfiOHOSGetAllDisplays()
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSGetAllDisplays start");
    return CJDisplayManager::GetAllDisplays();
}

RetStruct FfiOHOSHasPrivateWindow(uint32_t displayId)
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSHasPrivateWindow start");
    return CJDisplayManager::HasPrivateWindow(displayId);
}

bool FfiOHOSIsFoldable()
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSIsFoldable start");
    return CJDisplayManager::IsFoldable();
}

void FfiOHOSSetFoldDisplayMode(uint32_t mode)
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSSetFoldDisplayMode start");
    CJDisplayManager::SetFoldDisplayMode(mode);
}

uint32_t FfiOHOSGetFoldStatus()
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSGetFoldStatus start");
    return CJDisplayManager::GetFoldStatus();
}

uint32_t FfiOHOSGetFoldDisplayMode()
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSGetFoldDisplayMode start");
    return CJDisplayManager::GetFoldDisplayMode();
}

RetStruct FfiOHOSGetCurrentFoldCreaseRegion()
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSGetCurrentFoldCreaseRegion start");
    return CJDisplayManager::GetCurrentFoldCreaseRegion();
}

uint32_t FfiOHOSDisplayGetId(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSDisplayGetId start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}lld", id);
        return 0;
    }
    auto ret = instance->GetInfoId();
    return ret;
}

char* FfiOHOSGetDisplayName(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSGetDisplayName");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}lld", id);
        return nullptr;
    }
    auto ret = instance->GetName();
    return ret;
}

bool FfiOHOSDisplayGetAlive(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSDisplayGetAlive start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}lld", id);
        return false;
    }
    auto ret = instance->GetAlive();
    return ret;
}

uint32_t FfiOHOSDisplayGetState(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSDisplayGetState start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}lld", id);
        return 0;
    }
    auto ret = instance->GetState();
    return ret;
}

uint32_t FfiOHOSDisplayGetRefreshRate(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSDisplayGetRefreshRate start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}lld", id);
        return 0;
    }
    auto ret = instance->GetRefreshRate();
    return ret;
}

uint32_t FfiOHOSDisplayGetRotation(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSDisplayGetRotation start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}lld", id);
        return 0;
    }
    auto ret = instance->GetRotation();
    return ret;
}

uint32_t FfiOHOSDisplayGetOrientation(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSDisplayGetOrientation start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}lld", id);
        return 0;
    }
    auto ret = instance->GetOrientation();
    return ret;
}

int32_t FfiOHOSDisplayGetWidth(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSDisplayGetWidth start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}lld", id);
        return 0;
    }
    auto ret = instance->GetWidth();
    return ret;
}

int32_t FfiOHOSDisplayGetHeight(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSDisplayGetHeight start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}lld", id);
        return 0;
    }
    auto ret = instance->GetHeight();
    return ret;
}

float FfiOHOSDisplayGetDensityDPI(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSDisplayGetVirtualPixelRatio start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}lld", id);
        return 0.0;
    }
    auto ret = instance->GetDensityDPI();
    return ret;
}

float FfiOHOSDisplayGetVirtualPixelRatio(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSDisplayGetVirtualPixelRatio start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}lld", id);
        return 0.0;
    }
    auto ret = instance->GetVirtualPixelRatio();
    return ret;
}

float FfiOHOSDisplayGetXDPI(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSDisplayGetXDPI start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}lld", id);
        return 0.0;
    }
    auto ret = instance->GetXDPI();
    return ret;
}

float FfiOHOSDisplayGetYDPI(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSDisplayGetYDPI start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}lld", id);
        return 0.0;
    }
    auto ret = instance->GetYDPI();
    return ret;
}

RetStruct FfiOHOSDisplayGetCutoutInfo(int64_t id)
{
    TLOGI(WmsLogTag::DMS, "FfiOHOSDisplayGetCutoutInfo start");
    auto instance = FFIData::GetData<DisplayImpl>(id);
    if (!instance) {
        TLOGE(WmsLogTag::DMS, "[Display] instance not exist %{public}lld", id);
        return {DM_ERROR_SYSTEM_INNORMAL, 0, nullptr};
    }
    auto ret = instance->GetCutoutInfo();
    return ret;
}
}
}
}
