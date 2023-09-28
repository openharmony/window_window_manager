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
    explicit JsTransactionManager(NativeEngine& engine);
    ~JsTransactionManager() = default;

    static NativeValue* Init(NativeEngine* engine, NativeValue* exportObj);
    static void Finalizer(NativeEngine* engine, void* data, void* hint);

    static NativeValue* OpenSyncTransaction(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* CloseSyncTransaction(NativeEngine* engine, NativeCallbackInfo* info);

private:
    NativeValue* OnOpenSyncTransaction(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnCloseSyncTransaction(NativeEngine& engine, NativeCallbackInfo& info);
};
} // namespace OHOS::Rosen

#endif // OHOS_WINDOW_SCENE_JS_TRANSACTION_MANAGER_H
