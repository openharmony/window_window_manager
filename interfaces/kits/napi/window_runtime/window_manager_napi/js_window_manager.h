/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "js_runtime_utils.h"
#include "js_window_register_manager.h"
#include "native_engine/native_engine.h"
#include "native_engine/native_reference.h"
#include "native_engine/native_value.h"
#include "wm_common.h"

#ifndef OHOS_JS_WINDOW_MANAGER_H
#define OHOS_JS_WINDOW_MANAGER_H
namespace OHOS {
namespace Rosen {
napi_value JsWindowManagerInit(napi_env env, napi_value exportObj);
class JsWindowManager {
public:
    JsWindowManager();
    ~JsWindowManager();
    static void Finalizer(napi_env env, void* data, void* hint);
    static napi_value Create(napi_env env, napi_callback_info info);
    static napi_value CreateWindow(napi_env env, napi_callback_info info);
    static napi_value FindWindow(napi_env env, napi_callback_info info);
    static napi_value FindWindowSync(napi_env env, napi_callback_info info);
    static napi_value MinimizeAll(napi_env env, napi_callback_info info);
    static napi_value ToggleShownStateForAllAppWindows(napi_env env, napi_callback_info info);
    static napi_value RegisterWindowManagerCallback(napi_env env, napi_callback_info info);
    static napi_value UnregisterWindowMangerCallback(napi_env env, napi_callback_info info);
    static napi_value GetTopWindow(napi_env env, napi_callback_info info);
    static napi_value GetLastWindow(napi_env env, napi_callback_info info);
    static napi_value SetWindowLayoutMode(napi_env env, napi_callback_info info);
    static napi_value SetGestureNavigationEnabled(napi_env env, napi_callback_info info);
    static napi_value SetWaterMarkImage(napi_env env, napi_callback_info info);
private:
    static napi_value OnCreate(napi_env env, napi_callback_info info);
    static napi_value OnCreateWindow(napi_env env, napi_callback_info info);
    static napi_value OnFindWindow(napi_env env, napi_callback_info info);
    static napi_value OnFindWindowSync(napi_env env, napi_callback_info info);
    static napi_value OnMinimizeAll(napi_env env, napi_callback_info info);
    static napi_value OnToggleShownStateForAllAppWindows(napi_env env, napi_callback_info info);
    napi_value OnRegisterWindowManagerCallback(napi_env env, napi_callback_info info);
    napi_value OnUnregisterWindowManagerCallback(napi_env env, napi_callback_info info);
    static napi_value OnGetTopWindow(napi_env env, napi_callback_info info);
    static napi_value OnGetLastWindow(napi_env env, napi_callback_info info);
    static napi_value OnSetWindowLayoutMode(napi_env env, napi_callback_info info);
    static napi_value OnSetGestureNavigationEnabled(napi_env env, napi_callback_info info);
    static napi_value OnSetWaterMarkImage(napi_env env, napi_callback_info info);
    static bool ParseConfigOption(
        napi_env env, napi_value jsObject, WindowOption& option, void*& contextPtr);
    std::unique_ptr<JsWindowRegisterManager> registerManager_ = nullptr;
};
}  // namespace Rosen
}  // namespace OHOS

#endif
