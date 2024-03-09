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
#include "session_info.h"

namespace OHOS {
namespace Rosen {
napi_valuetype GetType(napi_env env, napi_value value);
class JsExtensionWindow {
public:
    explicit JsExtensionWindow(const std::shared_ptr<Rosen::ExtensionWindow> extensionWindow, int32_t hostWindowId);
    JsExtensionWindow(const std::shared_ptr<Rosen::ExtensionWindow> extensionWindow,
        sptr<AAFwk::SessionInfo> sessionInfo);
    ~JsExtensionWindow();
    static napi_value CreateJsExtensionWindow(napi_env env, sptr<Rosen::Window> window, int32_t hostWindowId);
    static napi_value CreateJsExtensionWindowObject(napi_env env, sptr<Rosen::Window> window,
    sptr<AAFwk::SessionInfo> sessionInfo);
    static void Finalizer(napi_env env, void* data, void* hint);
    static napi_value GetWindowAvoidArea(napi_env env, napi_callback_info info);
    static napi_value RegisterExtensionWindowCallback(napi_env env, napi_callback_info info);
    static napi_value UnRegisterExtensionWindowCallback(napi_env env, napi_callback_info info);
    static napi_value HideNonSecureWindows(napi_env env, napi_callback_info info);
    static napi_value LoadContent(napi_env env, napi_callback_info info);
    static napi_value LoadContentByName(napi_env env, napi_callback_info info);
    static napi_value ShowWindow(napi_env env, napi_callback_info info);
    static napi_value IsWindowShowingSync(napi_env env, napi_callback_info info);
    static napi_value SetUIContent(napi_env env, napi_callback_info info);
    static napi_value DestroyWindow(napi_env env, napi_callback_info info);
    static napi_value SetWindowBackgroundColorSync(napi_env env, napi_callback_info info);
    static napi_value CreateSubWindowWithOptions(napi_env env, napi_callback_info info);
private:
    napi_value OnGetWindowAvoidArea(napi_env env, napi_callback_info info);
    napi_value OnRegisterExtensionWindowCallback(napi_env env, napi_callback_info info);
    napi_value OnUnRegisterExtensionWindowCallback(napi_env env, napi_callback_info info);
    napi_value OnHideNonSecureWindows(napi_env env, napi_callback_info info);
    napi_value OnLoadContent(napi_env env, napi_callback_info info, bool isLoadedByName);
    napi_value OnShowWindow(napi_env env, napi_callback_info info);
    napi_value OnIsWindowShowingSync(napi_env env, napi_callback_info info);
    napi_value OnSetUIContent(napi_env env, napi_callback_info info);
    napi_value OnDestroyWindow(napi_env env, napi_callback_info info);
    napi_value OnSetWindowBackgroundColorSync(napi_env env, napi_callback_info info);
    napi_value OnCreateSubWindowWithOptions(napi_env env, napi_callback_info info);
    
    static napi_value GetProperties(napi_env env, napi_callback_info info);

    std::shared_ptr<Rosen::ExtensionWindow> extensionWindow_;
    int32_t hostWindowId_;
    sptr<AAFwk::SessionInfo> sessionInfo_ = nullptr;
    std::unique_ptr<JsExtensionWindowRegisterManager> extensionRegisterManager_ = nullptr;
};
}  // namespace Rosen
}  // namespace OHOS
#endif  // OHOS_JS_EXTENSION_WINDOW_H
