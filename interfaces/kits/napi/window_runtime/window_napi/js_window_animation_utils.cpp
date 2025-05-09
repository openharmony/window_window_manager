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

#include <iomanip>
#include <regex>
#include <sstream>

#include "accesstoken_kit.h"
#include "bundle_constants.h"
#include "ipc_skeleton.h"
#include "js_err_utils.h"
#include "js_window.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;

napi_value ConvertTransitionAnimationToJsValue(napi_env env, std::shared_ptr<TransitionAnimation> transitionAnimation)
{
    napi_value objValue = nullptr;
    if (!transitionAnimation) {
        return objValue;
    }
    CHECK_NAPI_CREATE_OBJECT_RETURN_IF_NULL(env, objValue);
    napi_value configJsValue = ConvertWindowAnimationOptionsToJsValue(env, transitionAnimation->config);
    if (!configJsValue) {
        return nullptr;
    }
    napi_set_named_property(env, objValue, "config", configJsValue);
    napi_set_named_property(env, objValue, "opacity", CreateJsValue(env, transitionAnimation->opacity));

    return objValue;
}

napi_value ConvertWindowAnimationOptionsToJsValue(napi_env env,
    const WindowAnimationOptions& animationConfig)
{
    napi_value configJsValue = nullptr;
    CHECK_NAPI_CREATE_OBJECT_RETURN_IF_NULL(env, configJsValue);
    napi_set_named_property(env, configJsValue, "curve", CreateJsValue(env, animationConfig.curve));
    switch (animationConfig.curve) {
        case WindowAnimationCurve::LINEAR: {
            napi_set_named_property(env, configJsValue, "duration", CreateJsValue(env, animationConfig.duration));
            break;
        }
        case WindowAnimationCurve::INTERPOLATION_SPRING: {
            napi_value params = nullptr;
            napi_create_array(env, &params);
            for (int i = 0; i < ANIMATION_PARAM_SIZE; ++i) {
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
    if (!ConvertWindowAnimationOptionsFromJsValue(env, jsAnimationConfig, transitionAnimation.config, result) ||
        !CheckWindowAnimationOptions(env, transitionAnimation.config, result)) {
        return false;
    }
    double opacity = 1.0f;
    ParseJsValue(jsObject, env, "opacity", opacity);
    if (opacity < 0.0 || opacity > 1.0) {
        result = WmErrorCode::WM_ERROR_ILLEGAL_PARAM;
        return false;
    }
    transitionAnimation.opacity = static_cast<float>(opacity);    
    return true;
}

bool CheckWindowAnimationOptions(napi_env env, WindowAnimationOptions& animationConfig, WmErrorCode& result)
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
            for (int i = 1; i < ANIMATION_PARAM_SIZE; ++i) {
                if (animationConfig.param[i] <= 0.0) {
                    TLOGI(WmsLogTag::WMS_ANIMATION, "Interpolation spring param %{public}u is invalid: %{public}f",
                        i, animationConfig.param[i]);
                    animationConfig.param[i] = 1.0;
                }
            }
            break;
        }
        default:
            result = WmErrorCode::WM_ERROR_ILLEGAL_PARAM;
            return false;
    }
    return true;
}

bool ConvertWindowAnimationOptionsFromJsValue(napi_env env, napi_value jsAnimationConfig,
    WindowAnimationOptions& animationConfig, WmErrorCode& result)
{
    if (jsAnimationConfig == nullptr) {
        result = WmErrorCode::WM_ERROR_INVALID_PARAM;
        return false;
    }
    uint32_t curve;
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
        case static_cast<uint32_t>(WindowAnimationCurve::INTERPOLATION_SPRING): {
            napi_value paramsValue = nullptr;
            napi_get_named_property(env, jsAnimationConfig, "param", &paramsValue);
            for (int i = 0; i < ANIMATION_PARAM_SIZE; ++i) {
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
