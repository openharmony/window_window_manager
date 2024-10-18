/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "event_handler.h"
#include "event_runner.h"
#include "js_window_listener.h"
#include "js_runtime_utils.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsListener"};
}

JsWindowListener::~JsWindowListener()
{
    WLOGI("[NAPI]~JsWindowListener");
}

void JsWindowListener::SetMainEventHandler()
{
    auto mainRunner = AppExecFwk::EventRunner::GetMainEventRunner();
    if (mainRunner == nullptr) {
        return;
    }
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(mainRunner);
}

void JsWindowListener::CallJsMethod(const char* methodName, napi_value const* argv, size_t argc)
{
    WLOGFD("[NAPI]methodName = %{public}s", methodName);
    if (env_ == nullptr || jsCallBack_ == nullptr) {
        WLOGFE("[NAPI]env_ nullptr or jsCallBack_ is nullptr");
        return;
    }
    napi_value method = jsCallBack_->GetNapiValue();
    if (method == nullptr) {
        WLOGFE("[NAPI]Failed to get method callback from object");
        return;
    }
    napi_value result = nullptr;
    napi_get_undefined(env_, &result);
    napi_call_function(env_, result, method, argc, argv, nullptr);
}

void JsWindowListener::OnAvoidAreaChanged(const AvoidArea avoidArea, AvoidAreaType type)
{
    WLOGFD("[NAPI]");
    // js callback should run in js thread
    auto jsCallback = [self = weakRef_, avoidArea, type, env = env_] {
        auto thisListener = self.promote();
        if (thisListener == nullptr || env == nullptr) {
            TLOGNE(WmsLogTag::WMS_IMMS, "[NAPI]this listener or env is nullptr");
            return;
        }
        HandleScope handleScope(env);
        napi_value avoidAreaValue = ConvertAvoidAreaToJsValue(env, avoidArea, type);
        if (avoidAreaValue == nullptr) {
            return;
        }
        if (thisListener->isDeprecatedInterface_) {
            napi_value argv[] = {avoidAreaValue};
            thisListener->CallJsMethod(SYSTEM_AVOID_AREA_CHANGE_CB.c_str(), argv, ArraySize(argv));
        } else {
            napi_value objValue = nullptr;
            napi_create_object(env, &objValue);
            if (objValue == nullptr) {
                TLOGNE(WmsLogTag::WMS_IMMS, "Failed to get object");
                return;
            }
            napi_set_named_property(env, objValue, "type", CreateJsValue(env, static_cast<uint32_t>(type)));
            napi_set_named_property(env, objValue, "area", avoidAreaValue);
            napi_value argv[] = {objValue};
            thisListener->CallJsMethod(AVOID_AREA_CHANGE_CB.c_str(), argv, ArraySize(argv));
        }
    };
    if (napi_status::napi_ok != napi_send_event(env_, jsCallback, napi_eprio_immediate)) {
        TLOGE(WmsLogTag::WMS_IMMS, "Failed to send event");
    }
}

} // namespace Rosen
} // namespace OHOS