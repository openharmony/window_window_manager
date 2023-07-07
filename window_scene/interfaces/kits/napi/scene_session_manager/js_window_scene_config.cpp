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

#include "js_window_scene_config.h"

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsWindowSceneConfig" };
} // namespace

NativeValue* JsWindowSceneConfig::CreateWindowSceneConfig(NativeEngine& engine, const AppWindowSceneConfig& config)
{
    WLOGI("[NAPI]CreateWindowSceneConfig");
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Object is null!");
        return engine.CreateUndefined();
    }

    object->SetProperty("floatCornerRadius", CreateJsValue(engine, config.floatCornerRadius_));

    object->SetProperty("focusedShadow", CreateShadowValue(engine, config, true));
    object->SetProperty("unfocusedShadow", CreateShadowValue(engine, config, false));
    object->SetProperty("keyboardAnimation", CreateKeyboardAnimationValue(engine, config));
    object->SetProperty("windowAnimation", CreateWindowAnimationValue(engine, config));
    return objValue;
}

NativeValue* JsWindowSceneConfig::CreateShadowValue(NativeEngine& engine, const AppWindowSceneConfig& config,
    bool focused)
{
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Object is null!");
        return engine.CreateUndefined();
    }

    object->SetProperty("offsetX", CreateJsValue(engine,
        focused ? config.focusedShadow_.offsetX_ : config.unfocusedShadow_.offsetX_));
    object->SetProperty("offsetY", CreateJsValue(engine,
        focused ? config.focusedShadow_.offsetY_ : config.unfocusedShadow_.offsetY_));
    object->SetProperty("radius", CreateJsValue(engine,
        focused ? config.focusedShadow_.radius_ : config.unfocusedShadow_.radius_));
    object->SetProperty("color", CreateJsValue(engine,
        focused ? config.focusedShadow_.color_ : config.unfocusedShadow_.color_));

    return objValue;
}

NativeValue* JsWindowSceneConfig::CreateWindowAnimationValue(NativeEngine& engine, const AppWindowSceneConfig& config)
{
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Object is null!");
        return engine.CreateUndefined();
    }
    object->SetProperty("duration", CreateJsValue(engine, config.windowAnimation_.duration_));
    object->SetProperty("curveType", CreateJsValue(engine, config.windowAnimation_.curveType_));
    object->SetProperty("ctrlX1", CreateJsValue(engine, config.windowAnimation_.ctrlX1_));
    object->SetProperty("ctrlY1", CreateJsValue(engine, config.windowAnimation_.ctrlY1_));
    object->SetProperty("ctrlX2", CreateJsValue(engine, config.windowAnimation_.ctrlX2_));
    object->SetProperty("ctrlY2", CreateJsValue(engine, config.windowAnimation_.ctrlY2_));
    object->SetProperty("scaleX", CreateJsValue(engine, config.windowAnimation_.scaleX_));
    object->SetProperty("scaleY", CreateJsValue(engine, config.windowAnimation_.scaleY_));
    object->SetProperty("rotationX", CreateJsValue(engine, config.windowAnimation_.rotationX_));
    object->SetProperty("rotationY", CreateJsValue(engine, config.windowAnimation_.rotationY_));
    object->SetProperty("rotationZ", CreateJsValue(engine, config.windowAnimation_.rotationZ_));
    object->SetProperty("angle", CreateJsValue(engine, config.windowAnimation_.angle_));
    object->SetProperty("translateX", CreateJsValue(engine, config.windowAnimation_.translateX_));
    object->SetProperty("translateY", CreateJsValue(engine, config.windowAnimation_.translateY_));
    object->SetProperty("opacity", CreateJsValue(engine, config.windowAnimation_.opacity_));
    return objValue;
}

NativeValue* JsWindowSceneConfig::CreateKeyboardAnimationValue(NativeEngine& engine,
    const AppWindowSceneConfig& config)
{
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Object is null!");
        return engine.CreateUndefined();
    }

    object->SetProperty("curveType", CreateJsValue(engine, config.keyboardAnimation_.curveType_));
    object->SetProperty("ctrlX1", CreateJsValue(engine, config.keyboardAnimation_.ctrlX1_));
    object->SetProperty("ctrlY1", CreateJsValue(engine, config.keyboardAnimation_.ctrlY1_));
    object->SetProperty("ctrlX2", CreateJsValue(engine, config.keyboardAnimation_.ctrlX2_));
    object->SetProperty("ctrlY2", CreateJsValue(engine, config.keyboardAnimation_.ctrlY2_));
    object->SetProperty("durationIn", CreateJsValue(engine, config.keyboardAnimation_.durationIn_));
    object->SetProperty("durationOut", CreateJsValue(engine, config.keyboardAnimation_.durationOut_));

    return objValue;
}

JsWindowSceneConfig::JsWindowSceneConfig()
{
    WLOGFD("Construct JsWindowSceneConfig");
}

JsWindowSceneConfig::~JsWindowSceneConfig()
{
    WLOGFD("Destroy  JsWindowSceneConfig");
}

} // namespace OHOS::Rosen
