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
#include "js_window_utils.h"
#include "window_manager_hilog.h"
namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "JsWindowListener"};
}

constexpr uint32_t AVOID_AREA_NUM = 4;

void JsWindowListener::AddCallback(NativeValue* jsListenerObject)
{
    WLOGFI("JsWindowListener::AddCallbackAndRegister is called");
    std::lock_guard<std::mutex> lock(mtx_);
    std::unique_ptr<NativeReference> callbackRef;
    callbackRef.reset(engine_->CreateReference(jsListenerObject, 1));
    jsCallBack_.push_back(std::move(callbackRef));
    WLOGFI("JsWindowListener::AddCallbackAndRegister success jsCallBack_ size: %{public}d!",
        static_cast<uint32_t>(jsCallBack_.size()));
    return;
}

void JsWindowListener::RemoveAllCallback()
{
    std::lock_guard<std::mutex> lock(mtx_);
    jsCallBack_.clear();
}

void JsWindowListener::RemoveCallback(NativeValue* jsListenerObject)
{
    std::lock_guard<std::mutex> lock(mtx_);
    for (auto iter = jsCallBack_.begin(); iter != jsCallBack_.end();) {
        if (jsListenerObject->StrictEquals((*iter)->Get())) {
            jsCallBack_.erase(iter);
        } else {
            iter++;
        }
    }
    WLOGFI("JsWindowListener::AddCallbackAndRegister success jsCallBack_ size: %{public}d!",
        static_cast<uint32_t>(jsCallBack_.size()));
    return;
}

void JsWindowListener::CallJsMethod(const char* methodName, NativeValue* const* argv, size_t argc)
{
    WLOGFI("CallJsMethod methodName = %{public}s", methodName);
    if (engine_ == nullptr) {
        WLOGFE("engine_ nullptr");
        return;
    }
    for (auto iter = jsCallBack_.begin(); iter != jsCallBack_.end(); iter++) {
        NativeValue* method = (*iter)->Get();
        if (method == nullptr) {
            WLOGFE("Failed to get method callback from object");
            continue;
        }
        engine_->CallFunction(engine_->CreateUndefined(), method, argv, argc);
    }
}

void JsWindowListener::OnSizeChange(Rect rect)
{
    WLOGFI("JsWindowListener::OnSizeChange is called");
    std::lock_guard<std::mutex> lock(mtx_);
    if (jsCallBack_.empty()) {
        WLOGFE("JsWindowListener::OnSizeChange windowSizeChanged not register!");
        return;
    }

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

void JsWindowListener::OnSystemBarPropertyChange(uint64_t displayId, SystemBarProps props)
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGFI("JsWindowListener::OnSystemBarPropertyChange is called");
    if (jsCallBack_.empty()) {
        WLOGFE("JsWindowListener::OnSystemBarPropertyChange systemUiTintChange not register!");
        return;
    }
    NativeValue* propertyValue = engine_->CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(propertyValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert prop to jsObject");
        return;
    }
    object->SetProperty("displayId", CreateJsValue(*engine_, static_cast<uint32_t>(displayId)));
    object->SetProperty("regionTint", CreateJsSystemBarRegionTintArrayObject(*engine_, props));
    NativeValue* argv[] = {propertyValue};
    CallJsMethod("systemUiTintChange", argv, ArraySize(argv));
}

void JsWindowListener::OnAvoidAreaChanged(const std::vector<Rect> avoidAreas)
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGFI("OnAvoidAreaChanged is called");
    if (jsCallBack_.empty()) {
        WLOGFE("OnAvoidAreaChanged systemAvoidAreaChange not register!");
        return;
    }

    NativeValue* avoidAreaValue = engine_->CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(avoidAreaValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert rect to jsObject");
        return;
    }

    if (static_cast<uint32_t>(avoidAreas.size()) != AVOID_AREA_NUM) {
        WLOGFE("AvoidAreas size is not 4 (left, top, right, bottom). Current avoidAreas size is %{public}u",
            static_cast<uint32_t>(avoidAreas.size()));
        return;
    }

    object->SetProperty("leftRect", GetRectAndConvertToJsValue(*engine_, avoidAreas[0]));   // idx 0 : leftRect
    object->SetProperty("topRect", GetRectAndConvertToJsValue(*engine_, avoidAreas[1]));    // idx 1 : topRect
    object->SetProperty("rightRect", GetRectAndConvertToJsValue(*engine_, avoidAreas[2]));  // idx 2 : rightRect
    object->SetProperty("bottomRect", GetRectAndConvertToJsValue(*engine_, avoidAreas[3])); // idx 3 : bottomRect
    NativeValue* argv[] = {avoidAreaValue};
    CallJsMethod("systemAvoidAreaChange", argv, ArraySize(argv));
}
} // namespace Rosen
} // namespace OHOS