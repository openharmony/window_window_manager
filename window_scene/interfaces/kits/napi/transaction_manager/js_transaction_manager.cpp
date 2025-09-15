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
#include "rs_adapter.h"
#include "scene_session_manager.h"
#include "screen_session_manager_client.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsTransactionManager" };
} // namespace

napi_value NapiGetUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

napi_value NapiThrowError(napi_env env, WSErrorCode err, const std::string& msg)
{
    napi_throw(env, CreateJsError(env, static_cast<int32_t>(err), msg));
    return NapiGetUndefined(env);
}

napi_value JsTransactionManager::Init(napi_env env, napi_value exportObj)
{
    TLOGD(WmsLogTag::DEFAULT, "JsTransactionManager Init");
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
    BindNativeFunction(env, exportObj, "closeSyncTransactionWithVsync", moduleName,
        JsTransactionManager::CloseSyncTransactionWithVsync);
    return NapiGetUndefined(env);
}

JsTransactionManager::JsTransactionManager(napi_env env)
{}

void JsTransactionManager::Finalizer(napi_env env, void* data, void* hint)
{
    TLOGD(WmsLogTag::DEFAULT, "Enter");
    std::unique_ptr<JsTransactionManager>(static_cast<JsTransactionManager*>(data));
}

napi_value JsTransactionManager::OpenSyncTransaction(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::DEFAULT, "Enter");
    JsTransactionManager* me = CheckParamsAndGetThis<JsTransactionManager>(env, info);
    return (me != nullptr) ? me->OnOpenSyncTransaction(env, info) : nullptr;
}

napi_value JsTransactionManager::CloseSyncTransaction(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::DEFAULT, "Enter");
    JsTransactionManager* me = CheckParamsAndGetThis<JsTransactionManager>(env, info);
    return (me != nullptr) ? me->OnCloseSyncTransaction(env, info) : nullptr;
}

napi_value JsTransactionManager::CloseSyncTransactionWithVsync(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::DEFAULT, "Enter");
    JsTransactionManager* me = CheckParamsAndGetThis<JsTransactionManager>(env, info);
    return (me != nullptr) ? me->OnCloseSyncTransactionWithVsync(env, info) : nullptr;
}

ScreenId ParseScreenIdFromArgs(napi_env env, napi_callback_info info)
{
    RETURN_IF_RS_CLIENT_MULTI_INSTANCE_DISABLED(SCREEN_ID_INVALID);
    size_t argc = 1;
    napi_value argv[1] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    if (argc != 1) {
        TLOGW(WmsLogTag::DEFAULT, "Argc is invalid: %{public}zu", argc);
        return SCREEN_ID_INVALID;
    }

    int64_t screenIdValue = static_cast<int64_t>(SCREEN_ID_INVALID);
    if (!ConvertFromJsValue(env, argv[0], screenIdValue)) {
        TLOGW(WmsLogTag::DEFAULT, "Failed to convert parameter to screenId");
        return SCREEN_ID_INVALID;
    }

    if (screenIdValue == static_cast<int64_t>(SCREEN_ID_INVALID)) {
        TLOGW(WmsLogTag::DEFAULT, "Invalid screenId");
        return SCREEN_ID_INVALID;
    }

    return static_cast<ScreenId>(screenIdValue);
}

std::pair<ScreenId, bool> ParseScreenIdAndInnerProcessFromArgs(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    int64_t screenIdValue = static_cast<int64_t>(SCREEN_ID_INVALID);
    bool isInnerProcess = false;
    if (argc != ARGC_TWO) {
        TLOGW(WmsLogTag::DEFAULT, "Argc is invalid: %{public}zu", argc);
        return std::make_pair(SCREEN_ID_INVALID, isInnerProcess);
    }

    if (!ConvertFromJsValue(env, argv[0], screenIdValue)) {
        TLOGW(WmsLogTag::DEFAULT, "Failed to convert parameter to screenId");
        return std::make_pair(SCREEN_ID_INVALID, isInnerProcess);
    }

    if (screenIdValue == static_cast<int64_t>(SCREEN_ID_INVALID)) {
        TLOGW(WmsLogTag::DEFAULT, "Invalid screenId");
        return std::make_pair(SCREEN_ID_INVALID, isInnerProcess);
    }

    if (!ConvertFromJsValue(env, argv[1], isInnerProcess)) {
        TLOGW(WmsLogTag::DEFAULT, "Failed to convert parameter to isInnerProcess");
        return std::make_pair(SCREEN_ID_INVALID, isInnerProcess);
    }
    TLOGD(WmsLogTag::DEFAULT, "isInnerProcess: %{public}d", isInnerProcess);
    return std::make_pair(static_cast<ScreenId>(screenIdValue), isInnerProcess);
}

napi_value JsTransactionManager::OnOpenSyncTransaction(napi_env env, napi_callback_info info)
{
    std::pair<ScreenId, bool> res = ParseScreenIdAndInnerProcessFromArgs(env, info);
    ScreenId screenId = res.first;
    bool isInnerProcess = res.second;
    auto task = [screenId, isInnerProcess] {
        auto rsUIContext = ScreenSessionManagerClient::GetInstance().GetRSUIContext(screenId);
        RSTransactionAdapter::FlushImplicitTransaction(rsUIContext);
        RSSyncTransactionAdapter::OpenSyncTransaction(rsUIContext, isInnerProcess);
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
    ScreenId screenId = ParseScreenIdFromArgs(env, info);
    auto task = [screenId] {
        auto rsUIContext = ScreenSessionManagerClient::GetInstance().GetRSUIContext(screenId);
        RSSyncTransactionAdapter::CloseSyncTransaction(rsUIContext);
    };
    if (auto handler = SceneSessionManager::GetInstance().GetTaskScheduler()) {
        handler->PostAsyncTask(task, __func__);
    } else {
        task();
    }
    return NapiGetUndefined(env);
}

napi_value JsTransactionManager::OnCloseSyncTransactionWithVsync(napi_env env, napi_callback_info info)
{
    ScreenId screenId = ParseScreenIdFromArgs(env, info);
    auto task = [screenId] {
        auto rsUIContext = ScreenSessionManagerClient::GetInstance().GetRSUIContext(screenId);
        RSSyncTransactionAdapter::CloseSyncTransaction(rsUIContext);
    };
    SceneSessionManager::GetInstance().CloseSyncTransaction(task);
    return NapiGetUndefined(env);
}
} // namespace OHOS::Rosen
