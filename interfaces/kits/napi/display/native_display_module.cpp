/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "display_manager.h"

#include <cinttypes>

#include "native_display_module.h"
#include "wm_common.h"
#include "wm_napi_common.h"

namespace OHOS {
namespace Rosen {
namespace getDefaultDisplay {
struct Param {
    WMError wret;
    sptr<Display> display;
};

void Async(napi_env env, std::unique_ptr<Param> &param)
{
    param->display = DisplayManager::GetInstance().GetDefaultDisplay();
    if (param->display == nullptr) {
        GNAPI_LOG("Get display failed!");
        param->wret = WMError::WM_ERROR_NULLPTR;
        return;
    }
    GNAPI_LOG("GetDefaultDisplay: id %{public}" PRIu64", w %{public}d, h %{public}d",
        param->display->GetId(), param->display->GetWidth(), param->display->GetHeight());
    param->wret = WMError::WM_OK;
}

napi_value Resolve(napi_env env, std::unique_ptr<Param> &param)
{
    napi_value result;
    if (param->wret != WMError::WM_OK) {
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }

    DisplayId id = param->display->GetId();
    int32_t width = param->display->GetWidth();
    int32_t height = param->display->GetHeight();
    GNAPI_LOG("id        : %{public}" PRIu64"", id);
    GNAPI_LOG("width     : %{public}d", width);
    GNAPI_LOG("height    : %{public}d", height);

    NAPI_CALL(env, napi_create_object(env, &result));
    NAPI_CALL(env, SetMemberInt32(env, result, "id", id));
    NAPI_CALL(env, SetMemberUndefined(env, result, "name"));
    NAPI_CALL(env, SetMemberUndefined(env, result, "alive"));
    NAPI_CALL(env, SetMemberUndefined(env, result, "state"));
    NAPI_CALL(env, SetMemberUndefined(env, result, "refreshRate"));
    NAPI_CALL(env, SetMemberUndefined(env, result, "rotation"));
    NAPI_CALL(env, SetMemberUint32(env, result, "width", width));
    NAPI_CALL(env, SetMemberUint32(env, result, "height", height));
    NAPI_CALL(env, SetMemberUndefined(env, result, "densityDPI"));
    NAPI_CALL(env, SetMemberUndefined(env, result, "densityPixels"));
    NAPI_CALL(env, SetMemberUndefined(env, result, "scaledDensity"));
    NAPI_CALL(env, SetMemberUndefined(env, result, "xDPI"));
    NAPI_CALL(env, SetMemberUndefined(env, result, "yDPI"));

    return result;
}

napi_value MainFunc(napi_env env, napi_callback_info info)
{
    GNAPI_LOG("Display Interface: getDefaultDisplay()");
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    auto param = std::make_unique<Param>();
    return CreatePromise<Param>(env, __PRETTY_FUNCTION__, Async, Resolve, param);
}
} // namespace getDefaultDisplay

napi_value DisplayModuleInit(napi_env env, napi_value exports)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getDefaultDisplay", getDefaultDisplay::MainFunc),
    };

    NAPI_CALL(env, napi_define_properties(env,
        exports, sizeof(properties) / sizeof(properties[0]), properties));
    return exports;
}
} // namespace Rosen
} // namespace OHOS

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module displayModule = {
        .nm_version = 1, // NAPI v1
        .nm_flags = 0, // normal
        .nm_filename = nullptr,
        .nm_register_func = OHOS::Rosen::DisplayModuleInit,
        .nm_modname = "display",
        .nm_priv = nullptr,
    };
    napi_module_register(&displayModule);
}
