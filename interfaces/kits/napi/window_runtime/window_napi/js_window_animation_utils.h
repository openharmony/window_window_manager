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

#ifndef OHOS_JS_WINDOW_ANIMATION_UTILS_H
#define OHOS_JS_WINDOW_ANIMATION_UTILS_H
#include <array>
#include <map>
#include "js_runtime_utils.h"
#include "js_window_utils.h"
#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"
#include "window.h"

#ifndef WINDOW_PREVIEW
#include "window_manager.h"
#else
#include "mock/window_manager.h"
#endif

#include "window_helper.h"
#include "window_option.h"
#include "window_visibility_info.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {

#define CHECK_NAPI_CREATE_OBJECT_RETURN_IF_NULL(env, objValue) \
    do {                                                       \
        napi_create_object((env), &(objValue));                \
        if ((objValue) == nullptr) {                           \
            TLOGE(WmsLogTag::DEFAULT, "Failed to get object"); \
            return nullptr;                                    \
        }                                                      \
    } while (0)

napi_value ConvertTransitionAnimationToJsValue(napi_env env,
    std::shared_ptr<TransitionAnimation> transitionAnimation);
napi_value ConvertWindowAnimationOptionToJsValue(napi_env env,
    const WindowAnimationOption& animationConfig);
bool ConvertTransitionAnimationFromJsValue(napi_env env, napi_value jsObject,
    TransitionAnimation& transitionAnimation, WmErrorCode& result);
bool ConvertWindowAnimationOptionFromJsValue(napi_env env, napi_value config,
    WindowAnimationOption& animationConfig, WmErrorCode& result);
bool CheckWindowAnimationOption(napi_env env, WindowAnimationOption& animationConfig, WmErrorCode& result);
}
}
#endif
