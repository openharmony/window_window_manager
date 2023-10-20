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
#include "js_scene_utils.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsWindowSceneConfig" };
} // namespace

napi_value JsWindowSceneConfig::CreateWindowSceneConfig(napi_env env, const AppWindowSceneConfig& config)
{
    WLOGI("[NAPI]CreateWindowSceneConfig");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("[NAPI]Object is null!");
        return NapiGetUndefined(env);
    }

    napi_set_named_property(env, objValue, "floatCornerRadius", CreateJsValue(env, config.floatCornerRadius_));
    napi_set_named_property(env, objValue, "focusedShadow", CreateShadowValue(env, config, true));
    napi_set_named_property(env, objValue, "unfocusedShadow", CreateShadowValue(env, config, false));
    napi_set_named_property(env, objValue, "keyboardAnimationIn", CreateKeyboardAnimationValue(env, config.keyboardAnimationIn_));
    napi_set_named_property(env, objValue, "keyboardAnimationOut", CreateKeyboardAnimationValue(env, config.keyboardAnimationOut_));
    napi_set_named_property(env, objValue, "windowAnimation", CreateWindowAnimationValue(env, config));
    return objValue;
}

napi_value JsWindowSceneConfig::CreateShadowValue(napi_env env, const AppWindowSceneConfig& config,
    bool focused)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("[NAPI]Object is null!");
        return NapiGetUndefined(env);
    }

    napi_set_named_property(env, objValue, "offsetX", CreateJsValue(env,
        focused ? config.focusedShadow_.offsetX_ : config.unfocusedShadow_.offsetX_));
    napi_set_named_property(env, objValue, "offsetY", CreateJsValue(env,
        focused ? config.focusedShadow_.offsetY_ : config.unfocusedShadow_.offsetY_));
    napi_set_named_property(env, objValue, "radius", CreateJsValue(env,
        focused ? config.focusedShadow_.radius_ : config.unfocusedShadow_.radius_));
    napi_set_named_property(env, objValue, "color", CreateJsValue(env,
        focused ? config.focusedShadow_.color_ : config.unfocusedShadow_.color_));

    return objValue;
}

napi_value JsWindowSceneConfig::CreateWindowAnimationValue(napi_env env, const AppWindowSceneConfig& config)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("[NAPI]Object is null!");
        return NapiGetUndefined(env);
    }
    napi_set_named_property(env, objValue, "duration", CreateJsValue(env, config.windowAnimation_.duration_));
    napi_set_named_property(env, objValue, "curveType", CreateJsValue(env, config.windowAnimation_.curveType_));
    napi_set_named_property(env, objValue, "ctrlX1", CreateJsValue(env, config.windowAnimation_.ctrlX1_));
    napi_set_named_property(env, objValue, "ctrlY1", CreateJsValue(env, config.windowAnimation_.ctrlY1_));
    napi_set_named_property(env, objValue, "ctrlX2", CreateJsValue(env, config.windowAnimation_.ctrlX2_));
    napi_set_named_property(env, objValue, "ctrlY2", CreateJsValue(env, config.windowAnimation_.ctrlY2_));
    napi_set_named_property(env, objValue, "scaleX", CreateJsValue(env, config.windowAnimation_.scaleX_));
    napi_set_named_property(env, objValue, "scaleY", CreateJsValue(env, config.windowAnimation_.scaleY_));
    napi_set_named_property(env, objValue, "rotationX", CreateJsValue(env, config.windowAnimation_.rotationX_));
    napi_set_named_property(env, objValue, "rotationY", CreateJsValue(env, config.windowAnimation_.rotationY_));
    napi_set_named_property(env, objValue, "rotationZ", CreateJsValue(env, config.windowAnimation_.rotationZ_));
    napi_set_named_property(env, objValue, "angle", CreateJsValue(env, config.windowAnimation_.angle_));
    napi_set_named_property(env, objValue, "translateX", CreateJsValue(env, config.windowAnimation_.translateX_));
    napi_set_named_property(env, objValue, "translateY", CreateJsValue(env, config.windowAnimation_.translateY_));
    napi_set_named_property(env, objValue, "opacity", CreateJsValue(env, config.windowAnimation_.opacity_));
    return objValue;
}

napi_value JsWindowSceneConfig::CreateKeyboardAnimationValue(napi_env env,
    const KeyboardSceneAnimationConfig& config)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("[NAPI]Object is null!");
        return NapiGetUndefined(env);
    }

    napi_set_named_property(env, objValue, "curveType", CreateJsValue(env, config.curveType_));
    napi_set_named_property(env, objValue, "ctrlX1", CreateJsValue(env, config.ctrlX1_));
    napi_set_named_property(env, objValue, "ctrlY1", CreateJsValue(env, config.ctrlY1_));
    napi_set_named_property(env, objValue, "ctrlX2", CreateJsValue(env, config.ctrlX2_));
    napi_set_named_property(env, objValue, "ctrlY2", CreateJsValue(env, config.ctrlY2_));
    napi_set_named_property(env, objValue, "duration", CreateJsValue(env, config.duration_));

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
