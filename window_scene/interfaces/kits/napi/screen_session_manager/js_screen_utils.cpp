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

#include "js_screen_utils.h"

#include <js_runtime_utils.h>

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsScreenUtils" };
}

NativeValue* JsScreenUtils::CreateJsScreenProperty(NativeEngine& engine, const ScreenProperty& screenProperty)
{
    auto objValue = engine.CreateObject();
    if (objValue == nullptr) {
        WLOGFE("Failed to create object!");
        return engine.CreateUndefined();
    }

    auto object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert object!");
        return engine.CreateUndefined();
    }

    object->SetProperty("rotation", CreateJsValue(engine, screenProperty.GetRotation()));
    object->SetProperty("bounds", CreateJsRRect(engine, screenProperty.GetBounds()));
    return objValue;
}

NativeValue* JsScreenUtils::CreateJsRRect(NativeEngine& engine, const RRect& rrect)
{
    auto objValue = engine.CreateObject();
    if (objValue == nullptr) {
        WLOGFE("Failed to create object!");
        return engine.CreateUndefined();
    }

    auto object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert object!");
        return engine.CreateUndefined();
    }

    object->SetProperty("left", CreateJsValue(engine, rrect.rect_.left_));
    object->SetProperty("top", CreateJsValue(engine, rrect.rect_.top_));
    object->SetProperty("width", CreateJsValue(engine, rrect.rect_.width_));
    object->SetProperty("height", CreateJsValue(engine, rrect.rect_.height_));
    object->SetProperty("radius", CreateJsValue(engine, rrect.radius_[0].x_));
    return objValue;
}

NativeValue* JsScreenUtils::CreateJsScreenConnectChangeType(NativeEngine& engine)
{
    auto objValue = engine.CreateObject();
    if (objValue == nullptr) {
        WLOGFE("Failed to create object!");
        return engine.CreateUndefined();
    }

    auto object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert object!");
        return engine.CreateUndefined();
    }

    object->SetProperty("CONNECT", CreateJsValue(engine, 0));
    object->SetProperty("DISCONNECT", CreateJsValue(engine, 1));
    return objValue;
}

NativeValue* JsScreenUtils::CreateJsScreenPropertyChangeReason(NativeEngine& engine)
{
    auto objValue = engine.CreateObject();
    if (objValue == nullptr) {
        WLOGFE("Failed to create object!");
        return engine.CreateUndefined();
    }
    auto object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert object!");
        return engine.CreateUndefined();
    }
    object->SetProperty("UNDEFINED", CreateJsValue(engine,
        static_cast<int32_t>(ScreenPropertyChangeReason::UNDEFINED)));
    object->SetProperty("ROTATION", CreateJsValue(engine,
        static_cast<int32_t>(ScreenPropertyChangeReason::ROTATION)));
    object->SetProperty("CHANGE_MODE", CreateJsValue(engine,
        static_cast<int32_t>(ScreenPropertyChangeReason::CHANGE_MODE)));
    object->SetProperty("FOLD_SCREEN_EXPAND", CreateJsValue(engine,
        static_cast<int32_t>(ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND)));
    object->SetProperty("SCREEN_CONNECT", CreateJsValue(engine,
        static_cast<int32_t>(ScreenPropertyChangeReason::SCREEN_CONNECT)));
    object->SetProperty("SCREEN_DISCONNECT", CreateJsValue(engine,
        static_cast<int32_t>(ScreenPropertyChangeReason::SCREEN_DISCONNECT)));
    return objValue;
}

bool ConvertRRectFromJs(NativeEngine& engine, NativeObject* jsObject, RRect& bound)
{
    NativeValue* jsLeft = jsObject->GetProperty("left");
    NativeValue* jsTop = jsObject->GetProperty("top");
    NativeValue* jsWidth = jsObject->GetProperty("width");
    NativeValue* jsHeight = jsObject->GetProperty("height");
    NativeValue* jsRadius = jsObject->GetProperty("radius");

    if (jsLeft->TypeOf() != NATIVE_UNDEFINED) {
        int32_t left;
        if (!ConvertFromJsValue(engine, jsLeft, left)) {
            WLOGFE("[NAPI]Failed to convert parameter to left");
            return false;
        }
        bound.rect_.left_ = left;
    }
    if (jsTop->TypeOf() != NATIVE_UNDEFINED) {
        int32_t top;
        if (!ConvertFromJsValue(engine, jsTop, top)) {
            WLOGFE("[NAPI]Failed to convert parameter to top");
            return false;
        }
        bound.rect_.top_ = top;
    }
    if (jsWidth->TypeOf() != NATIVE_UNDEFINED) {
        int32_t width;
        if (!ConvertFromJsValue(engine, jsWidth, width)) {
            WLOGFE("[NAPI]Failed to convert parameter to width");
            return false;
        }
        bound.rect_.width_ = width;
    }
    if (jsHeight->TypeOf() != NATIVE_UNDEFINED) {
        int32_t height;
        if (!ConvertFromJsValue(engine, jsHeight, height)) {
            WLOGFE("[NAPI]Failed to convert parameter to height");
            return false;
        }
        bound.rect_.height_ = height;
    }
    if (jsRadius->TypeOf() != NATIVE_UNDEFINED) {
        int radius;
        if (!ConvertFromJsValue(engine, jsRadius, radius)) {
            WLOGFE("[NAPI]Failed to convert parameter to radius");
            return false;
        }
        bound.radius_[0].x_ = static_cast<float>(radius);
    }
    return true;
}
} // namespace OHOS::Rosen
