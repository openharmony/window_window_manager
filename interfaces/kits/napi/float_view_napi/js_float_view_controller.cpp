/*
 * Copyright (c) 2026-2026 Huawei Device Co., Ltd.
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
#include "js_float_view_controller.h"

#include "js_float_view_utils.h"

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
napi_value CreateJsFloatViewControllerObject(napi_env env, const sptr<FloatViewController>& floatViewController)
{
    napi_value objValue = nullptr;
    napi_status status = napi_create_object(env, &objValue);
    if (status != napi_ok || objValue == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "failed to create js obj, error:%{public}d", status);
        return NapiGetUndefined(env);
    }

    TLOGI(WmsLogTag::WMS_SYSTEM, "CreateJsFloatViewControllerObject");
    std::unique_ptr<JsFloatViewController> jsFloatViewController =
        std::make_unique<JsFloatViewController>(floatViewController);
    napi_wrap(env, objValue, jsFloatViewController.release(), JsFloatViewController::Finalizer, nullptr, nullptr);

    // BindFunction
    return objValue;
}

JsFloatViewController::JsFloatViewController(const sptr<FloatViewController>& floatViewController)
    : fvController_(floatViewController)
{
}

JsFloatViewController::~JsFloatViewController()
{
    TLOGI(WmsLogTag::WMS_SYSTEM, "JsFloatViewController release");
}

void JsFloatViewController::Finalizer(napi_env env, void* data, void* hint)
{
    TLOGD(WmsLogTag::WMS_SYSTEM, "Finalizer is called");
    std::unique_ptr<JsFloatViewController>(static_cast<JsFloatViewController*>(data));
}
} // namespace Rosen
} // namespace OHOS
