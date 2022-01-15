/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "native_screenshot_module.h"
#include <memory>
#include "display_manager.h"
#include "display_property.h"
#include "pixel_map.h"
#include "pixel_map_napi.h"
#include "wm_napi_common.h"

namespace OHOS {
namespace Rosen {
namespace save {
struct Rect {
    int32_t left = 0;
    int32_t top = 0;
    int32_t width = 0;
    int32_t height = 0;
};

struct Size {
    int32_t width = 0;
    int32_t height = 0;
};

struct Option {
    Rect rect;
    Size size;
    int rotation = 0;
    DisplayId displayId = 0;
};

struct Param {
    WMError wret;
    Option screenshotOptions;
    std::shared_ptr<Media::PixelMap> image;
};

void Async(napi_env env, std::unique_ptr<Param> &param)
{
    param->screenshotOptions.displayId = DisplayManager::GetInstance().GetDefaultDisplayId();
    param->image = DisplayManager::GetInstance().GetScreenshot(param->screenshotOptions.displayId);
    if (param->image == nullptr) {
        GNAPI_LOG("Get Screenshot failed!");
        param->wret = WMError::WM_ERROR_NULLPTR;
        return;
    }
    param->wret = WMError::WM_OK;
}

napi_value Resolve(napi_env env, std::unique_ptr<Param> &param)
{
    napi_value result;
    if (param->wret != WMError::WM_OK) {
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    GNAPI_LOG("Screenshot image Width %{public}d, Height %{public}d",
        param->image->GetWidth(), param->image->GetHeight());
    napi_value jsImage = OHOS::Media::PixelMapNapi::CreatePixelMap(env, param->image);
    return jsImage;
}

napi_value MainFunc(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);

    auto param = std::make_unique<Param>();
    return CreatePromise<Param>(env, __PRETTY_FUNCTION__, Async, Resolve, param);
}
} // namespace save

napi_value ScreenshotModuleInit(napi_env env, napi_value exports)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("save", save::MainFunc),
    };

    NAPI_CALL(env, napi_define_properties(env,
        exports, sizeof(properties) / sizeof(properties[0]), properties));
    return exports;
}
} // namespace Rosen
} // namespace OHOS

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module screenshotModule = {
        .nm_version = 1, // NAPI v1
        .nm_flags = 0, // normal
        .nm_filename = nullptr,
        .nm_register_func = OHOS::Rosen::ScreenshotModuleInit,
        .nm_modname = "screenshot",
        .nm_priv = nullptr,
    };
    napi_module_register(&screenshotModule);
}
