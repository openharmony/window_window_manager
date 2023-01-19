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

#include "utils/include/window_scene_hilog.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsScreenUtils"};
}

NativeValue* JsScreenUtils::CreateJsScreenProperty(NativeEngine& engine, const ScreenProperty& screenProperty)
{
    WLOGFD("Create.");
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
    WLOGFD("Create!");
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
} // namespace OHOS::Rosen
