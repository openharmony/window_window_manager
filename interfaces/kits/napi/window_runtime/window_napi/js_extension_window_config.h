/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_JS_EXTENSION_WINDOW_CONFIG_H
#define OHOS_JS_EXTENSION_WINDOW_CONFIG_H
#include "native_engine/native_engine.h"
#include "native_engine/native_reference.h"
#include "native_engine/native_value.h"
#include "js_runtime_utils.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
#ifdef _WIN32
#define WINDOW_EXPORT __attribute__((dllexport))
#else
#define WINDOW_EXPORT __attribute__((visibility("default")))
#endif

WINDOW_EXPORT napi_value CreateJsExtensionWindowConfig(napi_env env,
    const std::shared_ptr<ExtensionWindowConfig>& extensionWindowConfig);

class JsExtensionWindowConfig {
public:
    explicit JsExtensionWindowConfig(const std::shared_ptr<ExtensionWindowConfig>& extensionWindowConfig);
    ~JsExtensionWindowConfig();
    static void Finalizer(napi_env env, void* data, void* hint);
    static napi_value GetWindowName(napi_env env, napi_callback_info info);
    static napi_value GetWindowAttribute(napi_env env, napi_callback_info info);
    static napi_value GetWindowRect(napi_env env, napi_callback_info info);
    static napi_value GetWindowRectLeft(napi_env env, napi_callback_info info);
    static napi_value GetWindowRectTop(napi_env env, napi_callback_info info);
    static napi_value GetWindowRectWidth(napi_env env, napi_callback_info info);
    static napi_value GetWindowRectHeight(napi_env env, napi_callback_info info);
    static napi_value GetSubWindowOptions(napi_env env, napi_callback_info info);
    static napi_value GetSubWindowOptionsTitle(napi_env env, napi_callback_info info);
    static napi_value GetSubWindowOptionsDecorEnabled(napi_env env, napi_callback_info info);
    static napi_value GetSubWindowOptionsIsModal(napi_env env, napi_callback_info info);
    static napi_value GetSubWindowOptionsIsTopmost(napi_env env, napi_callback_info info);
    static napi_value GetSystemWindowOptions(napi_env env, napi_callback_info info);
    static napi_value GetSystemWindowOptionsWindowType(napi_env env, napi_callback_info info);
    static napi_value SetWindowName(napi_env env, napi_callback_info info);
    static napi_value SetWindowAttribute(napi_env env, napi_callback_info info);
    static napi_value SetWindowRect(napi_env env, napi_callback_info info);
    static napi_value SetWindowRectLeft(napi_env env, napi_callback_info info);
    static napi_value SetWindowRectTop(napi_env env, napi_callback_info info);
    static napi_value SetWindowRectWidth(napi_env env, napi_callback_info info);
    static napi_value SetWindowRectHeight(napi_env env, napi_callback_info info);
    static napi_value SetSubWindowOptions(napi_env env, napi_callback_info info);
    static napi_value SetSubWindowOptionsTitle(napi_env env, napi_callback_info info);
    static napi_value SetSubWindowOptionsDecorEnabled(napi_env env, napi_callback_info info);
    static napi_value SetSubWindowOptionsIsModal(napi_env env, napi_callback_info info);
    static napi_value SetSubWindowOptionsIsTopmost(napi_env env, napi_callback_info info);
    static napi_value SetSystemWindowOptions(napi_env env, napi_callback_info info);
    static napi_value SetSystemWindowOptionsWindowType(napi_env env, napi_callback_info info);

private:
    napi_value OnGetWindowName(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnGetWindowAttribute(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnGetWindowRect(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnGetWindowRectLeft(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnGetWindowRectTop(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnGetWindowRectWidth(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnGetWindowRectHeight(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnGetSubWindowOptions(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnGetSubWindowOptionsTitle(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnGetSubWindowOptionsDecorEnabled(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnGetSubWindowOptionsIsModal(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnGetSubWindowOptionsIsTopmost(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnGetSystemWindowOptions(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnGetSystemWindowOptionsWindowType(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnSetWindowName(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnSetWindowAttribute(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnSetWindowRect(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnSetWindowRectLeft(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnSetWindowRectTop(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnSetWindowRectWidth(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnSetWindowRectHeight(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnSetSubWindowOptions(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnSetSubWindowOptionsTitle(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnSetSubWindowOptionsDecorEnabled(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnSetSubWindowOptionsIsModal(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnSetSubWindowOptionsIsTopmost(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnSetSystemWindowOptions(napi_env env, AbilityRuntime::NapiCallbackInfo& info);
    napi_value OnSetSystemWindowOptionsWindowType(napi_env env, AbilityRuntime::NapiCallbackInfo& info);

    std::shared_ptr<ExtensionWindowConfig> extensionWindowConfig_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_JS_EXTENSION_WINDOW_CONFIG_H