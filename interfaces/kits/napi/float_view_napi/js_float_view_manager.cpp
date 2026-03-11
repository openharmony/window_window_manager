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
#include "js_float_view_manager.h"

#include "js_float_view_controller.h"
#include "js_float_view_utils.h"
#include "float_view_controller.h"
#include "float_view_manager.h"

#include "window_manager_hilog.h"
#include "js_err_utils.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
JsFloatViewManager::JsFloatViewManager()
{
}

JsFloatViewManager::~JsFloatViewManager()
{
    TLOGI(WmsLogTag::WMS_SYSTEM, "JsFloatViewManager release");
}

void JsFloatViewManager::Finalizer(napi_env env, void* data, void* hint)
{
    std::unique_ptr<JsFloatViewManager>(static_cast<JsFloatViewManager*>(data));
}

napi_value JsFloatViewManager::CreateFloatViewController(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_SYSTEM, "JsFloatViewManager create controller");
    JsFloatViewManager* me = CheckParamsAndGetThis<JsFloatViewManager>(env, info);
    return (me != nullptr) ? me->OnCreateFloatViewController(env, info) : nullptr;
}

napi_value JsFloatViewManager::OnCreateFloatViewController(napi_env env, napi_callback_info info)
{
    std::shared_ptr<WmErrorCode> errCodePtr = std::make_shared<WmErrorCode>(WmErrorCode::WM_OK);
    sptr<FloatViewController> floatViewController = sptr<FloatViewController>::MakeSptr();
    NapiAsyncTask::ExecuteCallback execute = [errCodePtr, floatViewController] {
        if (errCodePtr == nullptr) {
            return;
        }
    };
    NapiAsyncTask::CompleteCallback complete =
        [errCodePtr, floatViewController](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (*errCodePtr == WmErrorCode::WM_OK) {
                task.Resolve(env, CreateJsFloatViewControllerObject(env, floatViewController));
            } else {
                task.Reject(env, JsErrUtils::CreateJsError(env, *errCodePtr,
                    "JsFloatViewManager::OnCreateFloatViewController failed."));
            }
        };
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsFloatViewManager::OnCreateFloatViewController",
        env, CreateAsyncTaskWithLastParam(env, nullptr, std::move(execute), std::move(complete), &result));
    return result;
}

napi_value JsFloatViewManager::IsFloatViewEnabled(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_SYSTEM, "JsFloatViewManager check is enabled");
    JsFloatViewManager* me = CheckParamsAndGetThis<JsFloatViewManager>(env, info);
    return (me != nullptr) ? me->OnIsFloatViewEnabled(env, info) : nullptr;
}

napi_value JsFloatViewManager::OnIsFloatViewEnabled(napi_env env, napi_callback_info info)
{
    bool isSupportFloatView = FloatViewManager::IsSupportFloatView();
    return CreateJsValue(env, isSupportFloatView);
}

napi_value JsFloatViewWMInit(napi_env env, napi_value exportObj)
{
    TLOGI(WmsLogTag::WMS_SYSTEM, "JsFloatViewWMInit");
    if (env == nullptr || exportObj == nullptr) {
        TLOGI(WmsLogTag::WMS_SYSTEM, "JsFloatViewWMInit env or exportObj is nullptr");
        return nullptr;
    }
    std::unique_ptr<JsFloatViewManager> jsFvManager = std::make_unique<JsFloatViewManager>();
    napi_wrap(env, exportObj, jsFvManager.release(), JsFloatViewManager::Finalizer, nullptr, nullptr);
    
    const char* moduleName = "JsFloatViewManager";
    BindNativeFunction(env, exportObj, "todo_create", moduleName, JsFloatViewManager::CreateFloatViewController);
    BindNativeFunction(env, exportObj, "todo_isFloatViewEnabled", moduleName, JsFloatViewManager::IsFloatViewEnabled);
    return NapiGetUndefined(env);
}
} // namespace Rosen
} // namespace OHOS
