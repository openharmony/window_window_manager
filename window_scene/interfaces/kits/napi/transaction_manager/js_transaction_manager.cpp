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

#include "js_transaction_manager.h"

#include <transaction/rs_sync_transaction_controller.h>
#include <transaction/rs_transaction.h>

#include "js_runtime_utils.h"
#include "scene_session_manager.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsTransactionManager" };
} // namespace

napi_value NapiGetUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

napi_value JsTransactionManager::Init(napi_env env, napi_value exportObj)
{
    WLOGI("[NAPI]JsTransactionManager Init");
    if (env == nullptr || exportObj == nullptr) {
        WLOGFE("env or exportObj is null!");
        return nullptr;
    }

    std::unique_ptr<JsTransactionManager> jsTransactionManager = std::make_unique<JsTransactionManager>(env);
    napi_wrap(env, exportObj, jsTransactionManager.release(), JsTransactionManager::Finalizer, nullptr, nullptr);

    const char* moduleName = "JsTransactionManager";
    BindNativeFunction(env, exportObj, "openSyncTransaction", moduleName,
        JsTransactionManager::OpenSyncTransaction);
    BindNativeFunction(env, exportObj, "closeSyncTransaction", moduleName,
        JsTransactionManager::CloseSyncTransaction);
    return NapiGetUndefined(env);
}

JsTransactionManager::JsTransactionManager(napi_env env)
{}

void JsTransactionManager::Finalizer(napi_env env, void* data, void* hint)
{
    WLOGI("[NAPI]Finalizer");
    std::unique_ptr<JsTransactionManager>(static_cast<JsTransactionManager*>(data));
}

napi_value JsTransactionManager::OpenSyncTransaction(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]OpenSyncTransaction");
    JsTransactionManager* me = CheckParamsAndGetThis<JsTransactionManager>(env, info);
    return (me != nullptr) ? me->OnOpenSyncTransaction(env, info) : nullptr;
}

napi_value JsTransactionManager::CloseSyncTransaction(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]CloseSyncTransaction");
    JsTransactionManager* me = CheckParamsAndGetThis<JsTransactionManager>(env, info);
    return (me != nullptr) ? me->OnCloseSyncTransaction(env, info) : nullptr;
}

napi_value JsTransactionManager::OnOpenSyncTransaction(napi_env env, napi_callback_info info)
{
    auto task = []() {
        auto transactionController = RSSyncTransactionController::GetInstance();
        if (transactionController) {
            RSTransaction::FlushImplicitTransaction();
            transactionController->OpenSyncTransaction();
        };
    };
    auto handler = SceneSessionManager::GetInstance().GetTaskScheduler();
    if (handler) {
        // must sync to include change
        handler->PostVoidSyncTask(task);
    } else {
        task();
    }
    return NapiGetUndefined(env);
}

napi_value JsTransactionManager::OnCloseSyncTransaction(napi_env env, napi_callback_info info)
{
    auto task = []() {
        auto transactionController = RSSyncTransactionController::GetInstance();
        if (transactionController) {
            transactionController->CloseSyncTransaction();
        }
    };
    auto handler = SceneSessionManager::GetInstance().GetTaskScheduler();
    if (handler) {
        handler->PostAsyncTask(task);
    } else {
        task();
    }
    return NapiGetUndefined(env);
}
} // namespace OHOS::Rosen
