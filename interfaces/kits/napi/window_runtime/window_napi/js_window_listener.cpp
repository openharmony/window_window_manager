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
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsWindowListener"};
}

constexpr uint32_t AVOID_AREA_NUM = 4;

void JsWindowListener::AddCallback(NativeValue* jsListenerObject)
{
    WLOGFI("JsWindowListener::AddCallback is called");
    std::lock_guard<std::mutex> lock(mtx_);
    std::unique_ptr<NativeReference> callbackRef;
    callbackRef.reset(engine_->CreateReference(jsListenerObject, 1));
    jsCallBack_.push_back(std::move(callbackRef));
    WLOGFI("JsWindowListener::AddCallback success jsCallBack_ size: %{public}d!",
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
    WLOGFI("JsWindowListener::RemoveCallback success jsCallBack_ size: %{public}d!",
        static_cast<uint32_t>(jsCallBack_.size()));
    return;
}

void JsWindowListener::CallJsMethod(const char* methodName, NativeValue* const* argv, size_t argc)
{
    WLOGFI("CallJsMethod methodName = %{public}s", methodName);
    std::lock_guard<std::mutex> lock(mtx_);
    if (engine_ == nullptr || jsCallBack_.empty()) {
        WLOGFE("engine_ nullptr or jsCallBack_ is empty");
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

void JsWindowListener::OnSizeChange(Rect rect, WindowSizeChangeReason reason)
{
    WLOGFI("JsWindowListener::OnSizeChange is called");
    // js callback should run in js thread
    std::unique_ptr<AsyncTask::CompleteCallback> complete = std::make_unique<AsyncTask::CompleteCallback> (
        [this, rect] (NativeEngine &engine, AsyncTask &task, int32_t status) {
            NativeValue* sizeValue = engine_->CreateObject();
            NativeObject* object = ConvertNativeValueTo<NativeObject>(sizeValue);
            if (object == nullptr) {
                WLOGFE("Failed to convert rect to jsObject");
                return;
            }
            object->SetProperty("width", CreateJsValue(*engine_, rect.width_));
            object->SetProperty("height", CreateJsValue(*engine_, rect.height_));
            NativeValue* argv[] = {sizeValue};
            CallJsMethod(WINDOW_SIZE_CHANGE_CB.c_str(), argv, ArraySize(argv));
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule(*engine_, std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsWindowListener::OnModeChange(WindowMode mode)
{
    WLOGFI("JsWindowListener::OnModeChange is called");
}

void JsWindowListener::OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints)
{
    WLOGFI("JsWindowListener::OnSystemBarPropertyChange is called");
    // js callback should run in js thread
    std::unique_ptr<AsyncTask::CompleteCallback> complete = std::make_unique<AsyncTask::CompleteCallback> (
        [this, displayId, tints] (NativeEngine &engine, AsyncTask &task, int32_t status) {
            NativeValue* propertyValue = engine_->CreateObject();
            NativeObject* object = ConvertNativeValueTo<NativeObject>(propertyValue);
            if (object == nullptr) {
                WLOGFE("Failed to convert prop to jsObject");
                return;
            }
            object->SetProperty("displayId", CreateJsValue(*engine_, static_cast<uint32_t>(displayId)));
            object->SetProperty("regionTint", CreateJsSystemBarRegionTintArrayObject(*engine_, tints));
            NativeValue* argv[] = {propertyValue};
            CallJsMethod(SYSTEM_BAR_TINT_CHANGE_CB.c_str(), argv, ArraySize(argv));
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule(*engine_, std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsWindowListener::OnAvoidAreaChanged(const std::vector<Rect> avoidAreas)
{
    WLOGFI("JsWindowListener::OnAvoidAreaChanged is called");
    // js callback should run in js thread
    std::unique_ptr<AsyncTask::CompleteCallback> complete = std::make_unique<AsyncTask::CompleteCallback> (
        [this, avoidAreas] (NativeEngine &engine, AsyncTask &task, int32_t status) {
            NativeValue* avoidAreaValue = engine_->CreateObject();
            NativeObject* object = ConvertNativeValueTo<NativeObject>(avoidAreaValue);
            if (object == nullptr) {
                WLOGFE("JsWindowListener::OnAvoidAreaChanged Failed to convert rect to jsObject");
                return;
            }

            if (static_cast<uint32_t>(avoidAreas.size()) != AVOID_AREA_NUM) {
                WLOGFE("AvoidAreas size is not 4 (left, top, right, bottom). Current avoidAreas size is %{public}u",
                    static_cast<uint32_t>(avoidAreas.size()));
                return;
            }

            object->SetProperty("leftRect", GetRectAndConvertToJsValue(*engine_, avoidAreas[0]));   // idx 0 : left
            object->SetProperty("topRect", GetRectAndConvertToJsValue(*engine_, avoidAreas[1]));    // idx 1 : top
            object->SetProperty("rightRect", GetRectAndConvertToJsValue(*engine_, avoidAreas[2]));  // idx 2 : right
            object->SetProperty("bottomRect", GetRectAndConvertToJsValue(*engine_, avoidAreas[3])); // idx 3 : bottom
            NativeValue* argv[] = {avoidAreaValue};
            CallJsMethod(SYSTEM_AVOID_AREA_CHANGE_CB.c_str(), argv, ArraySize(argv));
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule(*engine_, std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}
} // namespace Rosen
} // namespace OHOS