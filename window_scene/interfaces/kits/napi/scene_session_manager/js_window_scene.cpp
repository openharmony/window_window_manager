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

#include "js_window_scene.h"

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsWindowScene" };
} // namespace

NativeValue* JsWindowScene::CreateWindowSceneConfig(NativeEngine& engine, const AppWindowSceneConfig& config)
{
    WLOGI("[NAPI]CreateWindowSceneConfig");
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Object is null!");
        return engine.CreateUndefined();
    }

    object->SetProperty("fullScreenCornerRadius", CreateJsValue(engine, config.fullScreenCornerRadius_));
    object->SetProperty("splitCornerRadius", CreateJsValue(engine, config.splitCornerRadius_));
    object->SetProperty("floatCornerRadius", CreateJsValue(engine, config.floatCornerRadius_));

    object->SetProperty("focusedShadow", CreateShadowValue(engine, config, true));
    object->SetProperty("unfocusedShadow", CreateShadowValue(engine, config, false));

    return objValue;
}

NativeValue* JsWindowScene::CreateShadowValue(NativeEngine& engine, const AppWindowSceneConfig& config, bool focused)
{
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Object is null!");
        return engine.CreateUndefined();
    }

    object->SetProperty("elevation", CreateJsValue(engine,
        focused ? config.focusedShadow_.elevation_ : config.unfocusedShadow_.elevation_));
    object->SetProperty("offsetX", CreateJsValue(engine,
        focused ? config.focusedShadow_.offsetX_ : config.unfocusedShadow_.offsetX_));
    object->SetProperty("offsetY", CreateJsValue(engine,
        focused ? config.focusedShadow_.offsetY_ : config.unfocusedShadow_.offsetY_));
    object->SetProperty("alpha", CreateJsValue(engine,
        focused ? config.focusedShadow_.alpha_ : config.unfocusedShadow_.alpha_));
    object->SetProperty("radius", CreateJsValue(engine,
        focused ? config.focusedShadow_.radius_ : config.unfocusedShadow_.radius_));
    object->SetProperty("color", CreateJsValue(engine,
        focused ? config.focusedShadow_.color_ : config.unfocusedShadow_.color_));

    return objValue;
}

JsWindowScene::JsWindowScene()
{
    WLOGFD("JsWindowScene");
}

JsWindowScene::~JsWindowScene()
{
    WLOGFD("~JsWindowScene");
}

} // namespace OHOS::Rosen
