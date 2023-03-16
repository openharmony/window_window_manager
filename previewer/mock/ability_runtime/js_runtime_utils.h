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

#ifndef OHOS_ABILITY_RUNTIME_JS_RUNTIME_UTILS_H
#define OHOS_ABILITY_RUNTIME_JS_RUNTIME_UTILS_H

#include <cstdint>
#include "native_engine/native_engine.h"

namespace OHOS {
namespace AbilityRuntime {
template<class T>
inline T* ConvertNativeValueTo(NativeValue* value)
{
    return nullptr;
}

template<class T>
inline NativeValue* CreateJsValue(NativeEngine& engine, const T& value)
{
    return engine.CreateUndefined();
}

template<class T>
inline bool ConvertFromJsValue(NativeEngine& engine, NativeValue* jsValue, T& value)
{
    return true;
}

template<class T>
NativeValue* CreateNativeArray(NativeEngine& engine, const std::vector<T>& data)
{
    return nullptr;
}

template<class T>
T* CheckParamsAndGetThis(NativeEngine* engine, NativeCallbackInfo* info, const char* name = nullptr)
{
    return nullptr;
}

NativeValue* CreateJsError(NativeEngine& engine, int32_t errCode, const std::string& message = std::string());
void BindNativeFunction(NativeEngine& engine, NativeObject& object, const char* name,
    const char* moduleName, NativeCallback func);

class AsyncTask final {
public:
    using ExecuteCallback = std::function<void()>;
    using CompleteCallback = std::function<void(NativeEngine&, AsyncTask&, int32_t)>;

    static void Schedule(const std::string& name, NativeEngine& engine, std::unique_ptr<AsyncTask>&& task);

    AsyncTask(NativeDeferred* deferred, std::unique_ptr<ExecuteCallback>&& execute,
        std::unique_ptr<CompleteCallback>&& complete);
    AsyncTask(NativeReference* callbackRef, std::unique_ptr<ExecuteCallback>&& execute,
        std::unique_ptr<CompleteCallback>&& complete);
    ~AsyncTask();

    void Resolve(NativeEngine& engine, NativeValue* value);
    void Reject(NativeEngine& engine, NativeValue* error);
    void ResolveWithNoError(NativeEngine& engine, NativeValue* value);
    void ResolveWithCustomize(NativeEngine& engine, NativeValue* error, NativeValue* value);
    void RejectWithCustomize(NativeEngine& engine, NativeValue* error, NativeValue* value);

private:
    static void Execute(NativeEngine* engine, void* data);
    static void Complete(NativeEngine* engine, int32_t status, void* data);

    bool Start(const std::string &name, NativeEngine& engine);
};

std::unique_ptr<AsyncTask> CreateAsyncTaskWithLastParam(NativeEngine& engine, NativeValue* lastParam,
    AsyncTask::ExecuteCallback&& execute, AsyncTask::CompleteCallback&& complete, NativeValue** result);

std::unique_ptr<AsyncTask> CreateAsyncTaskWithLastParam(NativeEngine& engine, NativeValue* lastParam,
    AsyncTask::ExecuteCallback&& execute, nullptr_t, NativeValue** result);

std::unique_ptr<AsyncTask> CreateAsyncTaskWithLastParam(NativeEngine& engine, NativeValue* lastParam,
    nullptr_t, AsyncTask::CompleteCallback&& complete, NativeValue** result);

std::unique_ptr<AsyncTask> CreateAsyncTaskWithLastParam(NativeEngine& engine, NativeValue* lastParam,
    nullptr_t, nullptr_t, NativeValue** result);
}  // namespace AbilityRuntime
}  // namespace OHOS

#endif  // OHOS_ABILITY_RUNTIME_JS_RUNTIME_UTILS_H
