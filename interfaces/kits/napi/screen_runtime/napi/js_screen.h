/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_JS_SCREEN_H
#define OHOS_JS_SCREEN_H
#include <js_runtime_utils.h>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include <refbase.h>

#include "screen.h"

namespace OHOS {
namespace Rosen {
napi_value CreateJsScreenObject(napi_env env, sptr<Screen>& screen);
napi_value CreateJsScreenModeArrayObject(napi_env env, std::vector<sptr<SupportedScreenModes>> screenModes);
napi_value CreateJsScreenModeObject(napi_env env, const sptr<SupportedScreenModes>& mode);
napi_value NapiGetUndefined(napi_env env);
napi_valuetype GetType(napi_env env, napi_value value);
class JsScreen final {
public:
    explicit JsScreen(const sptr<Screen>& screen);
    ~JsScreen();
    static void Finalizer(napi_env env, void* data, void* hint);
    static napi_value SetScreenActiveMode(napi_env env, napi_callback_info info);
    static napi_value SetOrientation(napi_env env, napi_callback_info info);
    static napi_value SetDensityDpi(napi_env env, napi_callback_info info);

private:
    sptr<Screen> screen_ = nullptr;
    napi_value OnSetOrientation(napi_env env, napi_callback_info info);
    napi_value OnSetScreenActiveMode(napi_env env, napi_callback_info info);
    napi_value OnSetDensityDpi(napi_env env, napi_callback_info info);
};
}  // namespace Rosen
}  // namespace OHOS
#endif