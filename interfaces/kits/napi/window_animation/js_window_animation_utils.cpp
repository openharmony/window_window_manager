/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include "js_window_animation_utils.h"

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;

template<class T>
bool ParseJsValue(napi_value jsObject, napi_env env, const std::string& name, T& data)
{
    napi_value value = nullptr;
    napi_get_named_property(env, jsObject, name.c_str(), &value);
    napi_valuetype type = napi_undefined;
    napi_typeof(env, value, &type);
    if (type != napi_undefined) {
        if (!AbilityRuntime::ConvertFromJsValue(env, value, data)) {
            return false;
        }
    } else {
        return false;
    }
    return true;
}

napi_value ConvertTransitionAnimationToJsValue(napi_env env, std::shared_ptr<TransitionAnimation> transitionAnimation)
{
    napi_value objValue = nullptr;
    if (!transitionAnimation) {
        return objValue;
    }
    CHECK_NAPI_CREATE_OBJECT_RETURN_IF_NULL(env, objValue);
    napi_value configJsValue = ConvertWindowAnimationOptionToJsValue(env, transitionAnimation->config);
    if (!configJsValue) {
        return nullptr;
    }
    napi_set_named_property(env, objValue, "config", configJsValue);
    napi_set_named_property(env, objValue, "opacity", CreateJsValue(env, transitionAnimation->opacity));

    return objValue;
}

napi_value ConvertStartAnimationOptionsToJsValue(napi_env env,
    std::shared_ptr<StartAnimationOptions> startAnimationOptions)
{
    napi_value objValue = nullptr;
    if (!startAnimationOptions) {
        return objValue;
    }
    CHECK_NAPI_CREATE_OBJECT_RETURN_IF_NULL(env, objValue);
    napi_set_named_property(env, objValue, "type", CreateJsValue(env, startAnimationOptions->animationType));
    return objValue;
}

napi_value ConvertStartAnimationSystemOptionsToJsValue(napi_env env,
    std::shared_ptr<StartAnimationSystemOptions> startAnimationSystemOptions)
{
    napi_value objValue = nullptr;
    if (!startAnimationSystemOptions) {
        return objValue;
    }
    CHECK_NAPI_CREATE_OBJECT_RETURN_IF_NULL(env, objValue);
    napi_set_named_property(env, objValue, "type", CreateJsValue(env, startAnimationSystemOptions->animationType));
    if (startAnimationSystemOptions->animationConfig != nullptr) {
        napi_value configJsValue = ConvertWindowAnimationOptionToJsValue(env,
            *(startAnimationSystemOptions->animationConfig));
        if (configJsValue != nullptr) {
            napi_set_named_property(env, objValue, "animationConfig", configJsValue);
        }
    }
    return objValue;
}

napi_value ConvertWindowAnimationOptionToJsValue(napi_env env,
    const WindowAnimationOption& animationConfig)
{
    napi_value configJsValue = nullptr;
    CHECK_NAPI_CREATE_OBJECT_RETURN_IF_NULL(env, configJsValue);
    napi_set_named_property(env, configJsValue, "curve", CreateJsValue(env, animationConfig.curve));
    switch (animationConfig.curve) {
        case WindowAnimationCurve::LINEAR: {
            napi_set_named_property(env, configJsValue, "duration", CreateJsValue(env, animationConfig.duration));
            break;
        }
        case WindowAnimationCurve::INTERPOLATION_SPRING:
        case WindowAnimationCurve::CUBIC_BEZIER_CURVE: {
            napi_value params = nullptr;
            napi_create_array(env, &params);
            for (uint32_t i = 0; i < ANIMATION_PARAM_SIZE; ++i) {
                napi_value element;
                napi_create_double(env, static_cast<double>(animationConfig.param[i]), &element);
                napi_set_element(env, params, i, element);
            }
            napi_set_named_property(env, configJsValue, "param", params);
            break;
        }
        default:
            break;
    }
    return configJsValue;
}

bool ConvertTransitionAnimationFromJsValue(napi_env env, napi_value jsObject, TransitionAnimation& transitionAnimation,
    WmErrorCode& result)
{
    napi_value jsAnimationConfig = nullptr;
    napi_get_named_property(env, jsObject, "config", &jsAnimationConfig);
    if (!ConvertWindowAnimationOptionFromJsValue(env, jsAnimationConfig, transitionAnimation.config, result) ||
        !CheckWindowAnimationOption(env, transitionAnimation.config, result)) {
        return false;
    }
    double opacity = 1.0f;
    napi_value jsOpacityValue = nullptr;
    napi_get_named_property(env, jsObject, "opacity", &jsOpacityValue);
    napi_valuetype type = napi_undefined;
    napi_typeof(env, jsOpacityValue, &type);
    if (type != napi_undefined) {
        if (!AbilityRuntime::ConvertFromJsValue(env, jsOpacityValue, opacity)) {
            result = WmErrorCode::WM_ERROR_INVALID_PARAM;
            return false;
        } else if (opacity < 0.0 || opacity > 1.0) {
            result = WmErrorCode::WM_ERROR_ILLEGAL_PARAM;
            return false;
        }
    }
    transitionAnimation.opacity = static_cast<float>(opacity);
    return true;
}

bool ConvertStartAnimationOptionsFromJsValue(napi_env env, napi_value jsObject,
    StartAnimationOptions& startAnimationOptions)
{
    uint32_t animationType = 0;
    if (!ParseJsValue(jsObject, env, "type", animationType)) {
        return false;
    }
    startAnimationOptions.animationType = static_cast<AnimationType>(animationType);
    return true;
}

// TODO 加默认错误码参数，用来兼容需要错误码的场景
// TODO windowTransitionType新增转换机制，START需要注册
bool ConvertStartAnimationSystemOptionsFromJsValue(napi_env env, napi_value jsObject,
    StartAnimationSystemOptions& startAnimationSystemOptions)
{
    WmErrorCode result = WmErrorCode::WM_OK;
    uint32_t animationType = 0;
    if (!ParseJsValue(jsObject, env, "type", animationType)) {
        return false;
    }
    startAnimationSystemOptions.animationType = static_cast<AnimationType>(animationType);
    napi_value jsAnimationConfig = nullptr;
    napi_get_named_property(env, jsObject, "config", &jsAnimationConfig);
    if (jsAnimationConfig == nullptr) {
        return true;
    }
    if (!ConvertWindowAnimationOptionFromJsValue(env, jsAnimationConfig, *(startAnimationSystemOptions.animationConfig),
        result) ||!CheckWindowAnimationOption(env, *(startAnimationSystemOptions.animationConfig), result)) {
        return false;
    }
    return true;
}

bool CheckWindowAnimationOption(napi_env env, WindowAnimationOption& animationConfig, WmErrorCode& result)
{
    switch (animationConfig.curve) {
        case WindowAnimationCurve::LINEAR: {
            if (animationConfig.duration > ANIMATION_MAX_DURATION) {
                TLOGE(WmsLogTag::WMS_ANIMATION, "Duration is invalid: %{public}u", animationConfig.duration);
                result = WmErrorCode::WM_ERROR_ILLEGAL_PARAM;
                return false;
            }
            break;
        }
        case WindowAnimationCurve::INTERPOLATION_SPRING: {
            for (uint32_t i = 1; i < ANIMATION_PARAM_SIZE; ++i) {
                if (animationConfig.param[i] <= 0.0) {
                    TLOGI(WmsLogTag::WMS_ANIMATION, "Interpolation spring param %{public}u is invalid: %{public}f",
                        i, animationConfig.param[i]);
                    animationConfig.param[i] = 1.0;
                }
            }
            break;
        }
        case WindowAnimationCurve::CUBIC_BEZIER_CURVE: {
            break;
        }
        default:
            result = WmErrorCode::WM_ERROR_ILLEGAL_PARAM;
            return false;
    }
    return true;
}

bool ConvertWindowAnimationOptionFromJsValue(napi_env env, napi_value jsAnimationConfig,
    WindowAnimationOption& animationConfig, WmErrorCode& result)
{
    if (jsAnimationConfig == nullptr) {
        result = WmErrorCode::WM_ERROR_INVALID_PARAM;
        return false;
    }
    uint32_t curve = 0;
    if (!ParseJsValue(jsAnimationConfig, env, "curve", curve)) {
        result = WmErrorCode::WM_ERROR_INVALID_PARAM;
        return false;
    }
    animationConfig.curve = static_cast<WindowAnimationCurve>(curve);
    uint32_t duration = 0;
    std::array<double, ANIMATION_PARAM_SIZE> params;
    switch (curve) {
        case static_cast<uint32_t>(WindowAnimationCurve::LINEAR): {
            if (!ParseJsValue(jsAnimationConfig, env, "duration", duration)) {
                result = WmErrorCode::WM_ERROR_INVALID_PARAM;
                return false;
            }
            animationConfig.duration = duration;
            break;
        }
        case static_cast<uint32_t>(WindowAnimationCurve::INTERPOLATION_SPRING): 
        case static_cast<uint32_t>(WindowAnimationCurve::CUBIC_BEZIER_CURVE): {
            napi_value paramsValue = nullptr;
            napi_get_named_property(env, jsAnimationConfig, "param", &paramsValue);
            for (uint32_t i = 0; i < ANIMATION_PARAM_SIZE; ++i) {
                napi_value element;
                napi_get_element(env, paramsValue, i, &element);
                if (!ConvertFromJsValue(env, element, params[i])) {
                    result = WmErrorCode::WM_ERROR_INVALID_PARAM;
                    return false;
                }
                animationConfig.param[i] = static_cast<float>(params[i]);
            }
            break;
        }
        default:
            result = WmErrorCode::WM_ERROR_ILLEGAL_PARAM;
            return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
