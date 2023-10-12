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
#include "js_screen_listener.h"
#include "js_runtime_utils.h"
#include "window_manager_hilog.h"
#include "js_screen.h"
namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "JsScreenListener"};
}
inline uint32_t SCREEN_DISCONNECT_TYPE = 0;
inline uint32_t SCREEN_CONNECT_TYPE = 1;

void JsScreenListener::AddCallback(const std::string& type, napi_value jsListenerObject)
{
    WLOGI("JsScreenListener::AddCallback is called");
    std::lock_guard<std::mutex> lock(mtx_);
    std::unique_ptr<NativeReference> callbackRef;
    napi_ref result = nullptr;
    napi_create_reference(env_, jsListenerObject, 1, &result);
    callbackRef.reset(reinterpret_cast<NativeReference*>(result));
    jsCallBack_[type].emplace_back(std::move(callbackRef));
    WLOGI("JsScreenListener::AddCallback success jsCallBack_ size: %{public}u!",
        static_cast<uint32_t>(jsCallBack_[type].size()));
}

void JsScreenListener::RemoveAllCallback()
{
    std::lock_guard<std::mutex> lock(mtx_);
    jsCallBack_.clear();
}

void JsScreenListener::RemoveCallback(napi_env env, const std::string& type, napi_value jsListenerObject)
{
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = jsCallBack_.find(type);
    if (it == jsCallBack_.end()) {
        WLOGE("JsScreenListener::RemoveCallback no callback to remove");
        return;
    }
    auto& listeners = it->second;
    for (auto iter = listeners.begin(); iter != listeners.end();) {
        bool isEquals = false;
        napi_strict_equals(env, jsListenerObject, (*iter)->GetNapiValue(), &isEquals);
        if (isEquals) {
            listeners.erase(iter);
        } else {
            iter++;
        }
    }
    WLOGI("JsScreenListener::RemoveCallback success jsCallBack_ size: %{public}u!",
        static_cast<uint32_t>(listeners.size()));
}

void JsScreenListener::CallJsMethod(const std::string& methodName, napi_value const* argv, size_t argc)
{
    if (methodName.empty()) {
        WLOGFE("empty method name str, call method failed");
        return;
    }
    WLOGI("CallJsMethod methodName = %{public}s", methodName.c_str());
    if (env_ == nullptr) {
        WLOGFE("env_ nullptr");
        return;
    }
    for (auto& callback : jsCallBack_[methodName]) {
        napi_value method = callback->GetNapiValue();
        if (method == nullptr) {
            WLOGFE("Failed to get method callback from object");
            continue;
        }
        napi_call_function(env_, NapiGetUndefined(env_), method, argc, argv, nullptr);
    }
}

void JsScreenListener::OnConnect(ScreenId id)
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGI("JsScreenListener::OnConnect is called");
    if (jsCallBack_.empty()) {
        WLOGFE("JsScreenListener::OnConnect not register!");
        return;
    }
    if (jsCallBack_.find(EVENT_CONNECT) == jsCallBack_.end()) {
        WLOGE("JsScreenListener::OnConnect not this event, return");
        return;
    }
    sptr<JsScreenListener> listener = this; // Avoid this be destroyed when using.
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback> (
        [this, listener, id] (napi_env env, NapiAsyncTask &task, int32_t status) {
            napi_value argv[] = {CreateJsValue(env_, static_cast<uint32_t>(id))};
            CallJsMethod(EVENT_CONNECT, argv, ArraySize(argv));
        }
    );

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsScreenListener::OnConnect", env_, std::make_unique<NapiAsyncTask>(
            callback, std::move(execute), std::move(complete)));
}

void JsScreenListener::OnDisconnect(ScreenId id)
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGI("JsScreenListener::OnDisconnect is called");
    if (jsCallBack_.empty()) {
        WLOGFE("JsScreenListener::OnDisconnect not register!");
        return;
    }
    if (jsCallBack_.find(EVENT_DISCONNECT) == jsCallBack_.end()) {
        WLOGE("JsScreenListener::OnDisconnect not this event, return");
        return;
    }
    sptr<JsScreenListener> listener = this; // Avoid this be destroyed when using.
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback> (
        [this, listener, id] (napi_env env, NapiAsyncTask &task, int32_t status) {
            napi_value argv[] = {CreateJsValue(env_, static_cast<uint32_t>(id))};
            CallJsMethod(EVENT_DISCONNECT, argv, ArraySize(argv));
        }
    );

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsScreenListener::OnDisconnect", env_, std::make_unique<NapiAsyncTask>(
            callback, std::move(execute), std::move(complete)));
}

void JsScreenListener::OnChange(ScreenId id)
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGI("JsScreenListener::OnChange is called");
    if (jsCallBack_.empty()) {
        WLOGFE("JsScreenListener::OnChange not register!");
        return;
    }
    if (jsCallBack_.find(EVENT_CHANGE) == jsCallBack_.end()) {
        WLOGE("JsScreenListener::OnChange not this event, return");
        return;
    }
    sptr<JsScreenListener> listener = this; // Avoid this be destroyed when using.
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback> (
        [this, listener, id] (napi_env env, NapiAsyncTask &task, int32_t status) {
            napi_value argv[] = {CreateJsValue(env_, static_cast<uint32_t>(id))};
            CallJsMethod(EVENT_CHANGE, argv, ArraySize(argv));
        }
    );

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsScreenListener::OnChange", env_, std::make_unique<NapiAsyncTask>(
            callback, std::move(execute), std::move(complete)));
}

napi_value JsScreenListener::CreateScreenIdArray(napi_env env, const std::vector<ScreenId>& data)
{
    napi_value arrayValue = nullptr;
    napi_create_array_with_length(env, data.size(), &arrayValue);
    if (arrayValue == nullptr) {
        WLOGFE("Failed to create screenid array");
        return NapiGetUndefined(env);
    }
    uint32_t index = 0;
    for (const auto& item : data) {
        napi_set_element(env, arrayValue, index++, CreateJsValue(env, static_cast<uint32_t>(item)));
    }
    return arrayValue;
}
} // namespace Rosen
} // namespace OHOS
