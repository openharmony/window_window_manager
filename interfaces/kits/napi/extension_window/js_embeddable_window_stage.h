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

#ifndef OHOS_JS_EMBEDDABLE_WINDOW_STAGE_H
#define OHOS_JS_EMBEDDABLE_WINDOW_STAGE_H
#include <map>
#include <refbase.h>
#include "native_engine/native_engine.h"
#include "native_engine/native_reference.h"
#include "native_engine/native_value.h"
#include "js_extension_window_register_manager.h"
#include "session_info.h"
#include "window.h"
namespace OHOS {
namespace Rosen {
class JsEmbeddableWindowStage {
public:
    JsEmbeddableWindowStage(sptr<Rosen::Window> window, sptr<AAFwk::SessionInfo> sessionInfo);
    ~JsEmbeddableWindowStage();
    static void Finalizer(napi_env env, void* data, void* hint);
    static napi_value CreateJsEmbeddableWindowStage(napi_env env, sptr<Rosen::Window> window,
    sptr<AAFwk::SessionInfo> sessionInfo);
    static napi_value GetMainWindow(napi_env env, napi_callback_info info);
    static napi_value GetMainWindowSync(napi_env env, napi_callback_info info);
    static napi_value On(napi_env env, napi_callback_info info);
    static napi_value Off(napi_env env, napi_callback_info info);
    static napi_value LoadContent(napi_env env, napi_callback_info info);
    static napi_value LoadContentByName(napi_env env, napi_callback_info info);

private:
    napi_value OnGetMainWindow(napi_env env, napi_callback_info info);
    napi_value OnGetMainWindowSync(napi_env env, napi_callback_info info);
    napi_value OnEvent(napi_env env, napi_callback_info info);
    napi_value OffEvent(napi_env env, napi_callback_info info);
    napi_value OnLoadContent(napi_env env, napi_callback_info info, bool isLoadedByName);

    sptr<Window> windowExtensionSessionImpl_;
    sptr<AAFwk::SessionInfo> sessionInfo_ = nullptr;
    std::unique_ptr<JsExtensionWindowRegisterManager> extwinRegisterManager_ = nullptr;
};
}  // namespace Rosen
}  // namespace OHOS
#endif  // OHOS_JS_EMBEDDABLE_WINDOW_STAGE_H
