/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_JS_EXTENSION_WINDOW_H
#define OHOS_JS_EXTENSION_WINDOW_H

#include "extension_window.h"
#include "extension_window_impl.h"
#include "js_extension_window_register_manager.h"
#include "native_engine/native_engine.h"
#include "native_engine/native_reference.h"
#include "native_engine/native_value.h"

namespace OHOS {
namespace Rosen {
class JsExtensionWindow {
public:
    explicit JsExtensionWindow(const std::shared_ptr<Rosen::ExtensionWindow> extensionWindow);
    ~JsExtensionWindow();
    static napi_value CreateJsExtensionWindow(napi_env env, sptr<Rosen::Window> window);
    static void Finalizer(napi_env env, void* data, void* hint);
    static napi_value GetWindowAvoidArea(napi_env env, napi_callback_info info);
    static napi_value RegisterExtensionWindowCallback(napi_env env, napi_callback_info info);
    static napi_value UnRegisterExtensionWindowCallback(napi_env env, napi_callback_info info);
    static napi_value HideNonSecureWindows(napi_env env, napi_callback_info info);
private:
    napi_value OnGetWindowAvoidArea(napi_env env, napi_callback_info info);
    napi_value OnRegisterExtensionWindowCallback(napi_env env, napi_callback_info info);
    napi_value OnUnRegisterExtensionWindowCallback(napi_env env, napi_callback_info info);
    napi_value OnHideNonSecureWindows(napi_env env, napi_callback_info info);

    static napi_value GetProperties(napi_env env, napi_callback_info info);

    std::shared_ptr<Rosen::ExtensionWindow> extensionWindow_;
    std::unique_ptr<JsExtensionWindowRegisterManager> extensionRegisterManager_ = nullptr;
};
}  // namespace Rosen
}  // namespace OHOS
#endif  // OHOS_JS_EXTENSION_WINDOW_H
