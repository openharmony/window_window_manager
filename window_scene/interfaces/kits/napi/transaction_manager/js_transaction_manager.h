/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_WINDOW_SCENE_JS_TRANSACTION_MANAGER_H
#define OHOS_WINDOW_SCENE_JS_TRANSACTION_MANAGER_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

namespace OHOS::Rosen {
class JsTransactionManager final {
public:
    explicit JsTransactionManager(napi_env env);
    ~JsTransactionManager() = default;

    static napi_value Init(napi_env env, napi_value exportObj);
    static void Finalizer(napi_env env, void* data, void* hint);

    static napi_value OpenSyncTransaction(napi_env env, napi_callback_info info);
    static napi_value CloseSyncTransaction(napi_env env, napi_callback_info info);

private:
    napi_value OnOpenSyncTransaction(napi_env env, napi_callback_info info);
    napi_value OnCloseSyncTransaction(napi_env env, napi_callback_info info);
};
} // namespace OHOS::Rosen

#endif // OHOS_WINDOW_SCENE_JS_TRANSACTION_MANAGER_H
