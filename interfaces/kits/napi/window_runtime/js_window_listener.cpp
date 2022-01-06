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
#include "js_window_listener.h"
#include "js_runtime_utils.h"
#include "window_manager_hilog.h"
namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "JsWindowListener"};
}
bool JsWindowListener::IsCallbackExists(std::string type, NativeValue* jsListenerObject)
{
    if (jsWinodwListenerObjectMap_.find(type) == jsWinodwListenerObjectMap_.end()) {
        WLOGFE("JsWindowListener::IsCallbackExists methodName %{public}s not exist!", type.c_str());
        return false;
    }
    for (auto iter = jsWinodwListenerObjectMap_[type].begin();
            iter != jsWinodwListenerObjectMap_[type].end(); iter++) {
        if (jsListenerObject->StrictEquals((*iter)->Get())) {
            WLOGFI("JsWindowListener::AddJsListenerObject callback already exists!");
            return true;
        }
    }
    return false;
}

bool JsWindowListener::AddJsListenerObject(std::string type, NativeValue* jsListenerObject)
{
    WLOGFI("JsWindowListener::AddJsListenerObject is called");
    std::lock_guard<std::mutex> lock(listenerMutex_);
    std::unique_ptr<NativeReference> callbackRef;
    callbackRef.reset(engine_->CreateReference(jsListenerObject, 1));
    if (IsCallbackExists(type, jsListenerObject)) {
        WLOGFI("JsWindowListener::AddJsListenerObject jsWinodwListenerObjectMap_ size: %{public}d!",
            jsWinodwListenerObjectMap_[type].size());
        return false;
    }
    jsWinodwListenerObjectMap_[type].insert(std::move(callbackRef));
    WLOGFI("JsWindowListener::AddJsListenerObject failed jsWinodwListenerObjectMap_ size: %{public}d!",
        jsWinodwListenerObjectMap_[type].size());
    return true;
}

void JsWindowListener::RemoveJsListenerObject(std::string type, NativeValue* jsListenerObject)
{
    WLOGFI("JsWindowListener::RemoveJsListenerObject is called");
    std::lock_guard<std::mutex> lock(listenerMutex_);
    if (jsWinodwListenerObjectMap_.find(type) == jsWinodwListenerObjectMap_.end()) {
        WLOGFE("methodName %{public}s not exist!", type.c_str());
        return;
    }
    if (jsListenerObject == nullptr) {
        jsWinodwListenerObjectMap_.erase(type);
    } else {
        for (auto iter = jsWinodwListenerObjectMap_[type].begin();
                iter != jsWinodwListenerObjectMap_[type].end(); iter++) {
            if (jsListenerObject->StrictEquals((*iter)->Get())) {
                jsWinodwListenerObjectMap_[type].erase(iter);
            }
        }
    }
    WLOGFI("JsWindowListener::RemoveJsListenerObject jsWinodwListenerObjectMap_ size: %{public}d!",
        jsWinodwListenerObjectMap_[type].size());
    WLOGFI("JsWindowListener::RemoveJsListenerObject success!");
}

void JsWindowListener::OnSizeChange(Rect rect)
{
    WLOGFI("JsWindowListener::OnSizeChange is called");
    NativeValue* sizeValue = engine_->CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(sizeValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert rect to jsObject");
        return;
    }
    object->SetProperty("width", CreateJsValue(*engine_, rect.width_));
    object->SetProperty("height", CreateJsValue(*engine_, rect.height_));
    NativeValue* argv[] = {sizeValue};
    CallJsMethod("windowSizeChange", argv, ArraySize(argv));
}

void JsWindowListener::CallJsMethod(const char* methodName, NativeValue* const* argv, size_t argc)
{
    WLOGFI("CallJsMethod methodName = %{public}s", methodName);
    if (engine_ == nullptr) {
        WLOGFE("engine_ nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(listenerMutex_);
    std::string type(methodName);
    if (jsWinodwListenerObjectMap_.find(type) == jsWinodwListenerObjectMap_.end()) {
        WLOGFE("methodName %{public}s not exist!", methodName);
        return;
    }
    for (auto iter = jsWinodwListenerObjectMap_[type].begin(); iter != jsWinodwListenerObjectMap_[type].end(); iter++) {
        NativeValue* method = (*iter)->Get();
        if (method == nullptr) {
            WLOGFE("Failed to get method callback from object");
            return;
        }
        engine_->CallFunction(engine_->CreateUndefined(), method, argv, argc);
    }
}
} // namespace Rosen
} // namespace OHOS