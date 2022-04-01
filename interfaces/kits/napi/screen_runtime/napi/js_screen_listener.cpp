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
namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "JsScreenListener"};
}
inline uint32_t SCREEN_DISCONNECT_TYPE = 0;
inline uint32_t SCREEN_CONNECT_TYPE = 1;

void JsScreenListener::AddCallback(NativeValue* jsListenerObject)
{
    WLOGFI("JsScreenListener::AddCallback is called");
    std::lock_guard<std::mutex> lock(mtx_);
    std::unique_ptr<NativeReference> callbackRef;
    callbackRef.reset(engine_->CreateReference(jsListenerObject, 1));
    jsCallBack_.push_back(std::move(callbackRef));
    WLOGFI("JsScreenListener::AddCallback success jsCallBack_ size: %{public}u!",
        static_cast<uint32_t>(jsCallBack_.size()));
}

void JsScreenListener::RemoveAllCallback()
{
    std::lock_guard<std::mutex> lock(mtx_);
    jsCallBack_.clear();
}

void JsScreenListener::RemoveCallback(NativeValue* jsListenerObject)
{
    std::lock_guard<std::mutex> lock(mtx_);
    for (auto iter = jsCallBack_.begin(); iter != jsCallBack_.end();) {
        if (jsListenerObject->StrictEquals((*iter)->Get())) {
            jsCallBack_.erase(iter);
        } else {
            iter++;
        }
    }
    WLOGFI("JsScreenListener::RemoveCallback success jsCallBack_ size: %{public}u!",
        static_cast<uint32_t>(jsCallBack_.size()));
}

void JsScreenListener::CallJsMethod(const std::string& methodName, NativeValue* const* argv, size_t argc)
{
    if (methodName.empty()) {
        WLOGFE("empty method name str, call method failed");
        return;
    }
    WLOGFI("CallJsMethod methodName = %{public}s", methodName.c_str());
    if (engine_ == nullptr) {
        WLOGFE("engine_ nullptr");
        return;
    }
    for (auto& callback : jsCallBack_) {
        NativeValue* method = callback->Get();
        if (method == nullptr) {
            WLOGFE("Failed to get method callback from object");
            continue;
        }
        engine_->CallFunction(engine_->CreateUndefined(), method, argv, argc);
    }
}

void JsScreenListener::OnConnect(ScreenId id)
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGFI("JsScreenListener::OnConnect is called");
    if (jsCallBack_.empty()) {
        WLOGFE("JsScreenListener::OnConnect not register!");
        return;
    }

    std::unique_ptr<AsyncTask::CompleteCallback> complete = std::make_unique<AsyncTask::CompleteCallback> (
        [=] (NativeEngine &engine, AsyncTask &task, int32_t status) {
            NativeValue* argv[] = {CreateJsValue(*engine_, static_cast<uint32_t>(id))};
            CallJsMethod(EVENT_CONNECT, argv, ArraySize(argv));
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule(*engine_, std::make_unique<AsyncTask>(
            callback, std::move(execute), std::move(complete)));
}

void JsScreenListener::OnDisconnect(ScreenId id)
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGFI("JsScreenListener::OnDisconnect is called");
    if (jsCallBack_.empty()) {
        WLOGFE("JsScreenListener::OnDisconnect not register!");
        return;
    }

    std::unique_ptr<AsyncTask::CompleteCallback> complete = std::make_unique<AsyncTask::CompleteCallback> (
        [=] (NativeEngine &engine, AsyncTask &task, int32_t status) {
            NativeValue* argv[] = {CreateJsValue(*engine_, static_cast<uint32_t>(id))};
            CallJsMethod(EVENT_DISCONNECT, argv, ArraySize(argv));
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule(*engine_, std::make_unique<AsyncTask>(
            callback, std::move(execute), std::move(complete)));
}

void JsScreenListener::OnChange(ScreenId id)
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGFI("JsScreenListener::OnChange is called");
    if (jsCallBack_.empty()) {
        WLOGFE("JsScreenListener::OnChange not register!");
        return;
    }

    std::unique_ptr<AsyncTask::CompleteCallback> complete = std::make_unique<AsyncTask::CompleteCallback> (
        [=] (NativeEngine &engine, AsyncTask &task, int32_t status) {
            NativeValue* argv[] = {CreateJsValue(*engine_, static_cast<uint32_t>(id))};
            CallJsMethod(EVENT_CHANGE, argv, ArraySize(argv));
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule(*engine_, std::make_unique<AsyncTask>(
            callback, std::move(execute), std::move(complete)));
}

NativeValue* JsScreenListener::CreateScreenIdArray(NativeEngine& engine, const std::vector<ScreenId>& data)
{
    NativeValue* arrayValue = engine.CreateArray(data.size());
    NativeArray* array = ConvertNativeValueTo<NativeArray>(arrayValue);
    if (array == nullptr) {
        WLOGFE("Failed to create screenid array");
        return engine.CreateUndefined();
    }
    uint32_t index = 0;
    for (const auto& item : data) {
        array->SetElement(index++, CreateJsValue(engine, static_cast<uint32_t>(item)));
    }
    return arrayValue;
}
} // namespace Rosen
} // namespace OHOS