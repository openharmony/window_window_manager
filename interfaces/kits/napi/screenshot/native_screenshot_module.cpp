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
struct Option {
    Media::Rect rect;
    Media::Size size;
    int rotation = 0;
    DisplayId displayId = 0;
};

struct Param {
    WMError wret;
    Option option;
    std::shared_ptr<Media::PixelMap> image;
};

static napi_valuetype GetType(napi_env env, napi_value root)
{
    napi_valuetype res = napi_undefined;
    napi_typeof(env, root, &res);
    return res;
}

static void GetDisplayId(napi_env env, std::unique_ptr<Param> &param, napi_value &argv)
{
    GNAPI_LOG("Get Screenshot Option: GetDisplayId");
    napi_value displayId;
    NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, argv, "displayId", &displayId));
    if (displayId != nullptr && GetType(env, displayId) == napi_number) {
        int64_t dispId;
        NAPI_CALL_RETURN_VOID(env, napi_get_value_int64(env, displayId, &dispId));
        param->option.displayId = static_cast<DisplayId>(dispId);
        GNAPI_LOG("GetDisplayId success, displayId = %{public}" PRIu64"", param->option.displayId);
    } else {
        GNAPI_LOG("GetDisplayId failed, invalid param, use default displayId = 0");
    }
}

static void GetRotation(napi_env env, std::unique_ptr<Param> &param, napi_value &argv)
{
    GNAPI_LOG("Get Screenshot Option: GetRotation");
    napi_value rotation;
    NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, argv, "rotation", &rotation));
    if (rotation != nullptr && GetType(env, rotation) == napi_number) {
        NAPI_CALL_RETURN_VOID(env, napi_get_value_int32(env, rotation, &param->option.rotation));
        GNAPI_LOG("GetRotation success, rotation = %{public}d", param->option.rotation);
    } else {
        GNAPI_LOG("GetRotation failed, invalid param, use default rotation = 0");
    }
}

static void GetScreenRect(napi_env env, std::unique_ptr<Param> &param, napi_value &argv)
{
    GNAPI_LOG("Get Screenshot Option: GetScreenRect");
    napi_value screenRect;
    NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, argv, "screenRect", &screenRect));
    if (screenRect != nullptr && GetType(env, screenRect) == napi_object) {
        GNAPI_LOG("get ScreenRect success");

        napi_value left;
        NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, screenRect, "left", &left));
        if (left != nullptr && GetType(env, left) == napi_number) {
            NAPI_CALL_RETURN_VOID(env, napi_get_value_int32(env, left, &param->option.rect.left));
            GNAPI_LOG("get ScreenRect.left success, left = %{public}d", param->option.rect.left);
        } else {
            GNAPI_LOG("get ScreenRect.left failed, invalid param, use default left = 0");
        }

        napi_value top;
        NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, screenRect, "top", &top));
        if (top != nullptr && GetType(env, top) == napi_number) {
            NAPI_CALL_RETURN_VOID(env, napi_get_value_int32(env, top, &param->option.rect.top));
            GNAPI_LOG("get ScreenRect.top success, top = %{public}d", param->option.rect.top);
        } else {
            GNAPI_LOG("get ScreenRect.top failed, invalid param, use default top = 0");
        }

        napi_value width;
        NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, screenRect, "width", &width));
        if (width != nullptr && GetType(env, width) == napi_number) {
            NAPI_CALL_RETURN_VOID(env, napi_get_value_int32(env, width, &param->option.rect.width));
            GNAPI_LOG("get ScreenRect.width success, width = %{public}d", param->option.rect.width);
        } else {
            GNAPI_LOG("get ScreenRect.width failed, invalid param, use default width = 0");
        }
        
        napi_value height;
        NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, screenRect, "height", &height));
        if (height != nullptr && GetType(env, height) == napi_number) {
            NAPI_CALL_RETURN_VOID(env, napi_get_value_int32(env, height, &param->option.rect.height));
            GNAPI_LOG("get ScreenRect.height success, height = %{public}d", param->option.rect.height);
        } else {
            GNAPI_LOG("get ScreenRect.height failed, invalid param, use default height = 0");
        }
    } else {
        GNAPI_LOG("get ScreenRect failed, use default ScreenRect param");
    }
}

static void GetImageSize(napi_env env, std::unique_ptr<Param> &param, napi_value &argv)
{
    GNAPI_LOG("Get Screenshot Option: ImageSize");
    napi_value imageSize;
    NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, argv, "imageSize", &imageSize));
    if (imageSize != nullptr && GetType(env, imageSize) == napi_object) {
        napi_value width;
        NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, imageSize, "width", &width));
        if (width != nullptr && GetType(env, width) == napi_number) {
            NAPI_CALL_RETURN_VOID(env, napi_get_value_int32(env, width, &param->option.size.width));
            GNAPI_LOG("get ImageSize.width success, width = %{public}d", param->option.size.width);
        } else {
            GNAPI_LOG("get ImageSize.width failed, invalid param, use default width = 0");
        }

        napi_value height;
        NAPI_CALL_RETURN_VOID(env, napi_get_named_property(env, imageSize, "height", &height));
        if (height != nullptr && GetType(env, height) == napi_number) {
            NAPI_CALL_RETURN_VOID(env, napi_get_value_int32(env, height, &param->option.size.height));
            GNAPI_LOG("get ImageSize.height success, height = %{public}d", param->option.size.height);
        } else {
            GNAPI_LOG("get ImageSize.height failed, invalid param, use default height = 0");
        }
    }
}

static void AsyncGetScreenshotByDefaultOption(napi_env env, std::unique_ptr<Param> &param)
{
    GNAPI_LOG("Get Screenshot by default option");
    param->image = DisplayManager::GetInstance().GetScreenshot(param->option.displayId);
    if (param->image == nullptr) {
        GNAPI_LOG("Get Screenshot failed!");
        param->wret = WMError::WM_ERROR_NULLPTR;
        return;
    }
    param->wret = WMError::WM_OK;
}

static void AsyncGetScreenshotByInputOption(napi_env env, std::unique_ptr<Param> &param)
{
    GNAPI_LOG("Get Screenshot by input option");
    param->image = DisplayManager::GetInstance().GetScreenshot(param->option.displayId,
        param->option.rect, param->option.size, param->option.rotation);
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
    napi_value argv[2] = {0}; // the max number of input parameters is 2
    size_t argc = 2; // the max number of input parameters is 2
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    auto param = std::make_unique<Param>();
    param->option.displayId = DisplayManager::GetInstance().GetDefaultDisplayId();
    napi_ref ref = nullptr;
    if (argc == 0) {
        GNAPI_LOG("argc == 0");
        return CreatePromise<Param>(env, __PRETTY_FUNCTION__, AsyncGetScreenshotByDefaultOption,
            Resolve, ref, param);
    } else if (argc == 1) {
        GNAPI_LOG("argc == 1");
        if (GetType(env, argv[0]) == napi_object) {
            GNAPI_LOG("argv[0]'s type is napi_object");
            GetDisplayId(env, param, argv[0]);
            GetRotation(env, param, argv[0]);
            GetScreenRect(env, param, argv[0]);
            GetImageSize(env, param, argv[0]);
            return CreatePromise<Param>(env, __PRETTY_FUNCTION__, AsyncGetScreenshotByInputOption,
                Resolve, ref, param);
        }
        if (GetType(env, argv[0]) == napi_function) {
            GNAPI_LOG("argv[0]'s type is napi_function");
            NAPI_CALL(env, napi_create_reference(env, argv[0], 1, &ref));
            return CreatePromise<Param>(env, __PRETTY_FUNCTION__, AsyncGetScreenshotByDefaultOption,
                Resolve, ref, param);
        }
        GNAPI_LOG("argv[0]'s type is not napi_object or napi_function");
        return nullptr;
    } else if (argc == 2) { // the number of input parameters is 2
        GNAPI_LOG("argc == 2");
        if (GetType(env, argv[0]) == napi_object && GetType(env, argv[1]) == napi_function) {
            GNAPI_LOG("argv[0]'s type is napi_object, argv[0]'s type is napi_function");
            GetDisplayId(env, param, argv[0]);
            GetRotation(env, param, argv[0]);
            GetScreenRect(env, param, argv[0]);
            GetImageSize(env, param, argv[0]);
            NAPI_CALL(env, napi_create_reference(env, argv[1], 1, &ref));
            return CreatePromise<Param>(env, __PRETTY_FUNCTION__, AsyncGetScreenshotByInputOption,
                Resolve, ref, param);
        }
    } else {
        GNAPI_LOG("argc number mismatch");
        return nullptr;
    }
    GNAPI_LOG("argc number mismatch");
    return nullptr;
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
