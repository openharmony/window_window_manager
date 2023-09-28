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
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsTransactionManager" };
} // namespace

NativeValue* JsTransactionManager::Init(NativeEngine* engine, NativeValue* exportObj)
{
    WLOGI("[NAPI]JsTransactionManager Init");
    if (engine == nullptr || exportObj == nullptr) {
        WLOGFE("Engine or exportObj is null!");
        return nullptr;
    }

    auto object = ConvertNativeValueTo<NativeObject>(exportObj);
    if (object == nullptr) {
        WLOGFE("[NAPI]Object is null!");
        return nullptr;
    }

    std::unique_ptr<JsTransactionManager> jsTransactionManager = std::make_unique<JsTransactionManager>(*engine);
    object->SetNativePointer(jsTransactionManager.release(), JsTransactionManager::Finalizer, nullptr);

    const char* moduleName = "JsTransactionManager";
    BindNativeFunction(*engine, *object, "openSyncTransaction", moduleName,
        JsTransactionManager::OpenSyncTransaction);
    BindNativeFunction(*engine, *object, "closeSyncTransaction", moduleName,
        JsTransactionManager::CloseSyncTransaction);
    return engine->CreateUndefined();
}

JsTransactionManager::JsTransactionManager(NativeEngine& engine)
{}

void JsTransactionManager::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGI("[NAPI]Finalizer");
    std::unique_ptr<JsTransactionManager>(static_cast<JsTransactionManager*>(data));
}

NativeValue* JsTransactionManager::OpenSyncTransaction(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGI("[NAPI]OpenSyncTransaction");
    JsTransactionManager* me = CheckParamsAndGetThis<JsTransactionManager>(engine, info);
    return (me != nullptr) ? me->OnOpenSyncTransaction(*engine, *info) : nullptr;
}

NativeValue* JsTransactionManager::CloseSyncTransaction(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGI("[NAPI]CloseSyncTransaction");
    JsTransactionManager* me = CheckParamsAndGetThis<JsTransactionManager>(engine, info);
    return (me != nullptr) ? me->OnCloseSyncTransaction(*engine, *info) : nullptr;
}

NativeValue* JsTransactionManager::OnOpenSyncTransaction(NativeEngine& engine, NativeCallbackInfo& info)
{
    auto transactionController = RSSyncTransactionController::GetInstance();
    if (transactionController) {
        RSTransaction::FlushImplicitTransaction();
        transactionController->OpenSyncTransaction();
    }
    return engine.CreateUndefined();
}

NativeValue* JsTransactionManager::OnCloseSyncTransaction(NativeEngine& engine, NativeCallbackInfo& info)
{
    auto transactionController = RSSyncTransactionController::GetInstance();
    if (transactionController) {
        transactionController->CloseSyncTransaction();
    }
    return engine.CreateUndefined();
}
} // namespace OHOS::Rosen
