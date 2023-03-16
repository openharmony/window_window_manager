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

#include "js_runtime_utils.h"

namespace OHOS {
namespace AbilityRuntime {

NativeValue* CreateJsError(NativeEngine& engine, int32_t errCode, const std::string& message)
{
    return nullptr;
}

void BindNativeFunction(NativeEngine& engine, NativeObject& object, const char* name,
    const char* moduleName, NativeCallback func)
{
    return;
}

AsyncTask::AsyncTask(NativeDeferred* deferred, std::unique_ptr<AsyncTask::ExecuteCallback>&& execute,
    std::unique_ptr<AsyncTask::CompleteCallback>&& complete)
{
    return;
}

AsyncTask::AsyncTask(NativeReference* callbackRef, std::unique_ptr<AsyncTask::ExecuteCallback>&& execute,
    std::unique_ptr<AsyncTask::CompleteCallback>&& complete)
{
    return;
}

AsyncTask::~AsyncTask() = default;

void AsyncTask::Schedule(const std::string &name, NativeEngine& engine, std::unique_ptr<AsyncTask>&& task)
{
    return;
}

void AsyncTask::Resolve(NativeEngine& engine, NativeValue* value)
{
    return;
}

void AsyncTask::ResolveWithNoError(NativeEngine& engine, NativeValue* value)
{
    return;
}

void AsyncTask::Reject(NativeEngine& engine, NativeValue* error)
{
    return;
}

void AsyncTask::ResolveWithCustomize(NativeEngine& engine, NativeValue* error, NativeValue* value)
{
    return;
}

void AsyncTask::RejectWithCustomize(NativeEngine& engine, NativeValue* error, NativeValue* value)
{
    return;
}

void AsyncTask::Execute(NativeEngine* engine, void* data)
{
    return;
}

void AsyncTask::Complete(NativeEngine* engine, int32_t status, void* data)
{
    return;
}

bool AsyncTask::Start(const std::string &name, NativeEngine& engine)
{
    return true;
}

std::unique_ptr<AsyncTask> CreateAsyncTaskWithLastParam(NativeEngine& engine, NativeValue* lastParam,
    AsyncTask::ExecuteCallback&& execute, AsyncTask::CompleteCallback&& complete, NativeValue** result)
{
    return nullptr;
}

std::unique_ptr<AsyncTask> CreateAsyncTaskWithLastParam(NativeEngine& engine, NativeValue* lastParam,
    AsyncTask::ExecuteCallback&& execute, nullptr_t, NativeValue** result)
{
    return nullptr;
}

std::unique_ptr<AsyncTask> CreateAsyncTaskWithLastParam(NativeEngine& engine, NativeValue* lastParam,
    nullptr_t, AsyncTask::CompleteCallback&& complete, NativeValue** result)
{
    return nullptr;
}

std::unique_ptr<AsyncTask> CreateAsyncTaskWithLastParam(NativeEngine& engine, NativeValue* lastParam,
    nullptr_t, nullptr_t, NativeValue** result)
{
    return nullptr;
}
}  // namespace AbilityRuntime
}  // namespace OHOS
