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

#ifndef OHOS_JS_WINDOW_STAGE_H
#define OHOS_JS_WINDOW_STAGE_H
#include <map>
#include <refbase.h>
#include "native_engine/native_engine.h"
#include "native_engine/native_reference.h"
#include "native_engine/native_value.h"
#include "window_scene.h"
namespace OHOS {
namespace Rosen {
#ifdef _WIN32
#define WINDOW_EXPORT __attribute__((dllexport))
#else
#define WINDOW_EXPORT __attribute__((visibility("default")))
#endif
WINDOW_EXPORT napi_value CreateJsWindowStage(napi_env env, std::shared_ptr<Rosen::WindowScene> windowScene);
class JsWindowStage {
public:
    explicit JsWindowStage(const std::shared_ptr<Rosen::WindowScene>& windowScene);
    ~JsWindowStage();
    static void Finalizer(napi_env env, void* data, void* hint);
    static napi_value SetUIContent(napi_env env, napi_callback_info info);
    static napi_value GetMainWindow(napi_env env, napi_callback_info info);
    static napi_value GetMainWindowSync(napi_env env, napi_callback_info info);
    static napi_value On(napi_env env, napi_callback_info info);
    static napi_value Off(napi_env env, napi_callback_info info);
    static napi_value LoadContent(napi_env env, napi_callback_info info);
    static napi_value LoadContentByName(napi_env env, napi_callback_info info);
    static napi_value GetWindowMode(napi_env env, napi_callback_info info);
    static napi_value CreateSubWindow(napi_env env, napi_callback_info info);
    static napi_value CreateSubWindowWithOptions(napi_env env, napi_callback_info info);
    static napi_value GetSubWindow(napi_env env, napi_callback_info info);
    static napi_value SetShowOnLockScreen(napi_env env, napi_callback_info info);
    static napi_value DisableWindowDecor(napi_env env, napi_callback_info info);

private:
    napi_value OnSetUIContent(napi_env env, napi_callback_info info);
    napi_value OnGetMainWindow(napi_env env, napi_callback_info info);
    napi_value OnGetMainWindowSync(napi_env env, napi_callback_info info);
    napi_value OnEvent(napi_env env, napi_callback_info info);
    napi_value OffEvent(napi_env env, napi_callback_info info);
    napi_value OnLoadContent(napi_env env, napi_callback_info info, bool isLoadedByName);
    napi_value OnGetWindowMode(napi_env env, napi_callback_info info);
    napi_value OnCreateSubWindow(napi_env env, napi_callback_info info);
    napi_value OnCreateSubWindowWithOptions(napi_env env, napi_callback_info info);
    napi_value OnGetSubWindow(napi_env env, napi_callback_info info);
    napi_value OnSetShowOnLockScreen(napi_env env, napi_callback_info info);
    napi_value OnDisableWindowDecor(napi_env env, napi_callback_info info);

    bool ParseSubWindowOptions(napi_env env, napi_value jsObject, WindowOption& option);

    std::weak_ptr<Rosen::WindowScene> windowScene_;
};
}  // namespace Rosen
}  // namespace OHOS
#endif  // OHOS_JS_WINDOW_STAGE_H
