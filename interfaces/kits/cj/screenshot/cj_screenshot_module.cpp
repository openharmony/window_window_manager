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

#include "cj_screenshot_module.h"

#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <image_type.h>
#include <iosfwd>
#include <memory>
#include <string>
#include <type_traits>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

#include "display_manager.h"
#include "pixel_map.h"
#include "pixel_map_impl.h"
#include "window_manager_hilog.h"
#include "dm_common.h"

using namespace OHOS::FFI;
using namespace OHOS::Media;

namespace OHOS {
namespace Rosen {
struct Option {
    Media::Rect rect;
    Media::Size size;
    int rotation = 0;
    DisplayId displayId = 0;
    bool isNeedNotify = true;
    bool isNeedPointer = true;
};

struct Param {
    DmErrorCode wret;
    Option option;
    std::string errMessage;
    bool useInputOption;
    bool validInputParam;
    std::shared_ptr<Media::PixelMap> image;
    Media::Rect imageRect;
    bool isPick;
};

static CRect CreateRectObject(const Media::Rect& imageRect)
{
    CRect rect;
    rect.left = imageRect.left;
    rect.top = imageRect.top;
    rect.width = imageRect.width;
    rect.height = imageRect.height;
    return rect;
}

static std::unique_ptr<CPickInfo> CreatePickerObject(const std::unique_ptr<Param>& param)
{
    if (param == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "param nullptr.");
        return nullptr;
    }
    auto pickInfo = std::make_unique<CPickInfo>();
    pickInfo->pickRect = CreateRectObject(param->imageRect);

    auto nativeImage = FFIData::Create<PixelMapImpl>(move(param->image));
    if (nativeImage == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "create image failed");
        return pickInfo;
    }
    TLOGI(WmsLogTag::WMS_SCB, "pick end");
    pickInfo->pixelMap = nativeImage->GetID();
    return pickInfo;
}

static bool CheckWretParam(std::unique_ptr<Param>& param, RetStruct ret)
{
    bool isThrowError = true;
    switch (param->wret) {
        case DmErrorCode::DM_ERROR_NO_PERMISSION:
            TLOGE(WmsLogTag::WMS_SCB, "[CJScreenshotModule] Get default DmErrorCode is DM_ERROR_NO_PERMISSION");
            ret.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_NO_PERMISSION);
            break;
        case DmErrorCode::DM_ERROR_INVALID_PARAM:
            TLOGE(WmsLogTag::WMS_SCB, "[CJScreenshotModule] Get default DmErrorCode is DM_ERROR_INVALID_PARAM");
            ret.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM);
            break;
        case DmErrorCode::DM_ERROR_DEVICE_NOT_SUPPORT:
            TLOGE(WmsLogTag::WMS_SCB, "[CJScreenshotModule] Get default DmErrorCode is DM_ERROR_DEVICE_NOT_SUPPORT");
            ret.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_DEVICE_NOT_SUPPORT);
            break;
        case DmErrorCode::DM_ERROR_SYSTEM_INNORMAL:
            ret.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL);
            break;
        default:
            isThrowError = false;
            ret.code = static_cast<int32_t>(DmErrorCode::DM_OK);
            break;
    }
    return isThrowError;
}

static RetStruct Resolve(std::unique_ptr<Param>& param, RetStruct ret)
{
    TLOGI(WmsLogTag::WMS_SCB, "screen shot ret=%{public}d.", param->wret);
    bool isThrowError = CheckWretParam(param, ret);
    if (isThrowError) {
        return ret;
    }
    if (param->wret != DmErrorCode::DM_OK) {
        ret.code = static_cast<int32_t>(param->wret);
        TLOGE(WmsLogTag::WMS_SCB, "ret is not DmErrorCode DM_OK, errorCode:%{public}d", param->wret);
        return ret;
    }
    if (param->isPick) {
        TLOGI(WmsLogTag::WMS_SCB, "Resolve Screenshot by picker");
        auto pickInfo = CreatePickerObject(param);
        if (pickInfo == nullptr) {
            ret.code = static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM);
            return ret;
        }
        ret.data = pickInfo.release();
        return ret;
    }
    TLOGI(WmsLogTag::WMS_SCB, "Screenshot image Width %{public}d, Height %{public}d",
        param->image->GetWidth(), param->image->GetHeight());
    auto nativeImage = FFIData::Create<PixelMapImpl>(move(param->image));
    if (nativeImage == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "create image failed");
        return ret;
    }
    int64_t* pixelId = new int64_t(nativeImage->GetID());
    ret.data = pixelId;
    return ret;
}

static void GetScreenshot(std::unique_ptr<Param>& param)
{
    if (!param->validInputParam) {
        TLOGE(WmsLogTag::WMS_SCB, "Invalid Input Param!");
        param->image = nullptr;
        param->wret = DmErrorCode::DM_ERROR_INVALID_PARAM;
        param->errMessage = "Get Screenshot Failed: Invalid input param";
        return;
    }
    CaptureOption option = { param->option.displayId, param->option.isNeedNotify, param->option.isNeedPointer };
    if (!param->isPick && (!option.isNeedNotify_ || !option.isNeedPointer_)) {
        if (param->useInputOption) {
            param->image = DisplayManager::GetInstance().GetScreenshotWithOption(option,
                param->option.rect, param->option.size, param->option.rotation, &param->wret);
        } else {
            param->image = DisplayManager::GetInstance().GetScreenshotWithOption(option, &param->wret);
        }
    } else {
        if (param->useInputOption) {
            TLOGI(WmsLogTag::WMS_SCB, "Get Screenshot by input option");
            SnapShotConfig snapConfig;
            snapConfig.displayId_ = param->option.displayId;
            snapConfig.imageRect_ = param->option.rect;
            snapConfig.imageSize_ = param->option.size;
            snapConfig.rotation_ = param->option.rotation;
            param->image = DisplayManager::GetInstance().GetScreenshotwithConfig(snapConfig, &param->wret, true);
        } else if (param->isPick) {
            TLOGI(WmsLogTag::WMS_SCB, "Get Screenshot by picker");
            param->image = DisplayManager::GetInstance().GetSnapshotByPicker(param->imageRect, &param->wret);
        } else {
            TLOGI(WmsLogTag::WMS_SCB, "Get Screenshot by default option");
            param->image = DisplayManager::GetInstance().GetScreenshot(param->option.displayId, &param->wret, true);
        }
    }
    if (param->image == nullptr && param->wret == DmErrorCode::DM_OK) {
        TLOGE(WmsLogTag::WMS_SCB, "Get Screenshot failed!");
        param->wret = DmErrorCode::DM_ERROR_INVALID_SCREEN;
        param->errMessage = "Get Screenshot failed: Screenshot image is nullptr";
    }
}

static void GetScreenCapture(std::unique_ptr<Param>& param)
{
    CaptureOption captureOption;
    captureOption.displayId_ = param->option.displayId;
    captureOption.isNeedNotify_ = param->option.isNeedNotify;
    captureOption.isNeedPointer_ = param->option.isNeedPointer;
    TLOGI(WmsLogTag::WMS_SCB, "capture option isNeedNotify=%{public}d isNeedPointer=%{public}d",
        captureOption.isNeedNotify_, captureOption.isNeedPointer_);
    param->image = DisplayManager::GetInstance().GetScreenCapture(captureOption, &param->wret);
    if (param->image == nullptr && param->wret == DmErrorCode::DM_OK) {
        TLOGE(WmsLogTag::WMS_SCB, "screen capture failed!");
        param->wret = DmErrorCode::DM_ERROR_SYSTEM_INNORMAL;
        param->errMessage = "ScreenCapture failed: image is null.";
    }
}

RetStruct CJScreenshotModule::PickFunc()
{
    TLOGI(WmsLogTag::WMS_SCB, "%{public}s called", __PRETTY_FUNCTION__);

    RetStruct ret = { .code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL), .len = 0, .data = nullptr };
    auto param = std::make_unique<Param>();
    if (param == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "Create param failed.");
        return ret;
    }
    param->validInputParam = true;
    param->isPick = true;
    GetScreenshot(param);
    Resolve(param, ret);
    return ret;
}

RetStruct CJScreenshotModule::CaptureFunc(uint32_t displayId)
{
    TLOGI(WmsLogTag::WMS_SCB, "%{public}s called", __PRETTY_FUNCTION__);

    RetStruct ret = { .code = static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL), .len = 0, .data = nullptr };
    auto param = std::make_unique<Param>();
    if (param == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "Create param failed.");
        return ret;
    }
    param->option.displayId = displayId;
    GetScreenCapture(param);
    Resolve(param, ret);
    return ret;
}
} // namespace Rosen
} // namespace OHOS
