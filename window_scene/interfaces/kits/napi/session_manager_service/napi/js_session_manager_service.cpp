/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "js_session_manager_service.h"

#include "js_runtime_utils.h"

#include "session_manager_service.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
using namespace OHOS::AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsSessionManagerService" };
} // namespace

napi_value NapiGetUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

class JsSessionManagerService {
public:
    JsSessionManagerService() = default;
    ~JsSessionManagerService() = default;

    static void Finalizer(napi_env env, void* data, void* hint)
    {
        WLOGI("Finalizer is called");
        std::unique_ptr<JsSessionManagerService>(static_cast<JsSessionManagerService*>(data));
    }

    static napi_value InitSessionManagerService(napi_env env, napi_callback_info info)
    {
        JsSessionManagerService* me = CheckParamsAndGetThis<JsSessionManagerService>(env, info);
        return (me != nullptr) ? me->OnInitSessionManagerService(env, info) : nullptr;
    }

    __attribute__((no_sanitize("cfi")))
    static napi_value NotifySceneBoardAvailable(napi_env env, napi_callback_info info)
    {
        JsSessionManagerService* me = CheckParamsAndGetThis<JsSessionManagerService>(env, info);
        return (me != nullptr) ? me->OnNotifySceneBoardAvailable(env, info) : nullptr;
    }

private:
    napi_value OnInitSessionManagerService(napi_env env, napi_callback_info info)
    {
        WLOGI("[WMSRecover] JsSessionManagerService: OnInitSessionManagerService is called");
        if (SessionManagerService::GetInstance() != nullptr) {
            SessionManagerService::GetInstance()->Init();
        }
        return NapiGetUndefined(env);
    }

    napi_value OnNotifySceneBoardAvailable(napi_env env, napi_callback_info info)
    {
        WLOGI("[WMSRecover] JsSessionManagerService: OnNotifySceneBoardAvailable is called");
        if (SessionManagerService::GetInstance() != nullptr) {
            SessionManagerService::GetInstance()->NotifySceneBoardAvailable();
        }
        return NapiGetUndefined(env);
    }
};

napi_value JsSessionManagerServiceInit(napi_env env, napi_value exportObj)
{
    WLOGI("JsSessionManagerServiceInit is called.");
    if (env == nullptr || exportObj == nullptr) {
        WLOGFE("JsSessionManagerServiceInit env or exportObj is nullptr");
        return nullptr;
    }

    auto jsSessionManagerService = std::make_unique<JsSessionManagerService>();
    napi_wrap(env, exportObj, jsSessionManagerService.release(), JsSessionManagerService::Finalizer, nullptr, nullptr);

    const char* moduleName = "JsSessionManagerService";
    BindNativeFunction(env, exportObj, "initSessionManagerService", moduleName,
        JsSessionManagerService::InitSessionManagerService);
    BindNativeFunction(env, exportObj, "notifySceneBoardAvailable", moduleName,
        JsSessionManagerService::NotifySceneBoardAvailable);
    return NapiGetUndefined(env);
}
} // namespace OHOS::Rosen
