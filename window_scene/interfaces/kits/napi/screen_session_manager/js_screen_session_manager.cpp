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

#include "js_screen_session_manager.h"

#include <js_runtime_utils.h>

#include "display_manager.h"
#include "interfaces/include/ws_common.h"
#include "js_screen_session.h"
#include "js_screen_utils.h"
#include "window_manager_hilog.h"

#ifdef POWER_MANAGER_ENABLE
#include "shutdown/shutdown_client.h"
#endif

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsScreenSessionManager" };
const std::string ON_SCREEN_CONNECTION_CHANGE_CALLBACK = "screenConnectChange";
} // namespace

JsScreenSessionManager::JsScreenSessionManager(napi_env env) : env_(env) {}

napi_value JsScreenSessionManager::Init(napi_env env, napi_value exportObj)
{
    WLOGD("Init.");
    if (env == nullptr || exportObj == nullptr) {
        WLOGFE("Failed to init, env or exportObj is null!");
        return nullptr;
    }

    auto jsScreenSessionManager = std::make_unique<JsScreenSessionManager>(env);
    napi_wrap(env, exportObj, jsScreenSessionManager.release(), JsScreenSessionManager::Finalizer, nullptr, nullptr);
    napi_set_named_property(env, exportObj, "ScreenConnectChangeType",
        JsScreenUtils::CreateJsScreenConnectChangeType(env));
    napi_set_named_property(env, exportObj, "ScreenPropertyChangeReason",
        JsScreenUtils::CreateJsScreenPropertyChangeReason(env));

    const char* moduleName = "JsScreenSessionManager";
    BindNativeFunction(env, exportObj, "on", moduleName, JsScreenSessionManager::RegisterCallback);
    BindNativeFunction(env, exportObj, "updateScreenRotationProperty", moduleName,
        JsScreenSessionManager::UpdateScreenRotationProperty);
    BindNativeFunction(env, exportObj, "getCurvedScreenCompressionArea", moduleName,
        JsScreenSessionManager::GetCurvedCompressionArea);
    BindNativeFunction(env, exportObj, "registerShutdownCallback", moduleName,
        JsScreenSessionManager::RegisterShutdownCallback);
    BindNativeFunction(env, exportObj, "unRegisterShutdownCallback", moduleName,
        JsScreenSessionManager::UnRegisterShutdownCallback);
    BindNativeFunction(env, exportObj, "getPhyScreenProperty", moduleName,
        JsScreenSessionManager::GetPhyScreenProperty);
    BindNativeFunction(env, exportObj, "notifyScreenLockEvent", moduleName,
        JsScreenSessionManager::NotifyScreenLockEvent);
    return NapiGetUndefined(env);
}

void JsScreenSessionManager::Finalizer(napi_env env, void* data, void* hint)
{
    std::unique_ptr<JsScreenSessionManager>(static_cast<JsScreenSessionManager*>(data));
}

void JsScreenSessionManager::OnScreenConnected(const sptr<ScreenSession>& screenSession)
{
    if (screenConnectionCallback_ == nullptr) {
        return;
    }

    std::shared_ptr<NativeReference> callback_ = screenConnectionCallback_;
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [callback_, screenSession](napi_env env, NapiAsyncTask& task, int32_t status) {
            napi_value objValue = nullptr;
            napi_create_object(env, &objValue);
            if (objValue == nullptr) {
                WLOGFE("Object is null!");
                return;
            }

            napi_set_named_property(env, objValue, "screenSession", JsScreenSession::Create(env, screenSession));
            napi_set_named_property(env, objValue, "screenConnectChangeType", CreateJsValue(env, 0));

            napi_value argv[] = { objValue };
            napi_value method = callback_->GetNapiValue();
            if (method == nullptr) {
                WLOGFE("Failed to get method callback from object!");
                return;
            }
            napi_call_function(env, NapiGetUndefined(env), method, ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsScreenSessionManager::OnScreenConnect", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsScreenSessionManager::OnScreenDisconnected(const sptr<ScreenSession>& screenSession)
{
    if (screenConnectionCallback_ == nullptr) {
        return;
    }

    std::shared_ptr<NativeReference> callback_ = screenConnectionCallback_;
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [callback_, screenSession](napi_env env, NapiAsyncTask& task, int32_t status) {
            napi_value objValue = nullptr;
            napi_create_object(env, &objValue);
            if (objValue == nullptr) {
                WLOGFE("Object is null!");
                return;
            }

            napi_set_named_property(env, objValue, "screenSession", JsScreenSession::Create(env, screenSession));
            napi_set_named_property(env, objValue, "screenConnectChangeType", CreateJsValue(env, 1));

            napi_value argv[] = { objValue };
            napi_value method = callback_->GetNapiValue();
            if (method == nullptr) {
                WLOGFE("Failed to get method callback from object!");
                return;
            }
            napi_call_function(env, NapiGetUndefined(env), method, ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsScreenSessionManager::OnScreenDisconnect", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

bool JsScreenSessionManager::OnTakeOverShutdown(bool isReboot)
{
    if (!shutdownCallback_) {
        return false;
    }
    std::shared_ptr<NativeReference> callback_ = shutdownCallback_;
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [callback_, isReboot](napi_env env, NapiAsyncTask& task, int32_t status) {
            napi_value argv[] = {CreateJsValue(env, isReboot)};
            napi_value method = callback_->GetNapiValue();
            if (method == nullptr) {
                WLOGFE("Failed to get method callback from object!");
                return;
            }
            napi_call_function(env, NapiGetUndefined(env), method, ArraySize(argv), argv, nullptr);
        }
    );
    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsScreenSessionManager::OnTakeOverShutdown", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
    return true;
}


napi_value JsScreenSessionManager::RegisterShutdownCallback(napi_env env, napi_callback_info info)
{
    WLOGD("Register RegisterShutdownCallback.");
    JsScreenSessionManager* me = CheckParamsAndGetThis<JsScreenSessionManager>(env, info);
    return (me != nullptr) ? me->OnRegisterShutdownCallback(env, info) : nullptr;
}

napi_value JsScreenSessionManager::UnRegisterShutdownCallback(napi_env env, napi_callback_info info)
{
    WLOGD("Register UnRegisterShutdownCallback.");
    JsScreenSessionManager* me = CheckParamsAndGetThis<JsScreenSessionManager>(env, info);
    return (me != nullptr) ? me->OnUnRegisterShutdownCallback(env, info) : nullptr;
}

napi_value JsScreenSessionManager::GetPhyScreenProperty(napi_env env, napi_callback_info info)
{
    WLOGD("Register GetPhyScreenProperty.");
    JsScreenSessionManager* me = CheckParamsAndGetThis<JsScreenSessionManager>(env, info);
    return (me != nullptr) ? me->OnGetPhyScreenProperty(env, info) : nullptr;
}

napi_value JsScreenSessionManager::OnRegisterShutdownCallback(napi_env env, const napi_callback_info info)
{
    WLOGD("[NAPI]OnRegisterShutdownCallback");
    if (shutdownCallback_ != nullptr) {
        WLOGFE("Failed to register callback, callback exits");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_REPEAT_OPERATION)));
        return NapiGetUndefined(env);
    }
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    napi_value value = argv[0];
    if (!NapiIsCallable(env, value)) {
        WLOGFE("Failed to register callback, param is not callable");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    napi_ref result = nullptr;
    napi_create_reference(env, value, 1, &result);
    std::shared_ptr<NativeReference> callbackRef(reinterpret_cast<NativeReference*>(result));
    shutdownCallback_ = callbackRef;
#ifdef POWER_MANAGER_ENABLE
    PowerMgr::ShutdownClient::GetInstance().RegisterShutdownCallback(this, PowerMgr::ShutdownPriority::LOW);
#else
    WLOGFD("Can not find the sub system of PowerMgr");
#endif
    return NapiGetUndefined(env);
}

napi_value JsScreenSessionManager::OnUnRegisterShutdownCallback(napi_env env, const napi_callback_info info)
{
    WLOGD("[NAPI]OnUnRegisterShutdownCallback");
#ifdef POWER_MANAGER_ENABLE
    PowerMgr::ShutdownClient::GetInstance().UnRegisterShutdownCallback(this);
#else
    WLOGFD("Can not find the sub system of PowerMgr");
#endif
    shutdownCallback_ = nullptr;
    return NapiGetUndefined(env);
}

napi_value JsScreenSessionManager::RegisterCallback(napi_env env, napi_callback_info info)
{
    WLOGD("Register callback.");
    JsScreenSessionManager* me = CheckParamsAndGetThis<JsScreenSessionManager>(env, info);
    return (me != nullptr) ? me->OnRegisterCallback(env, info) : nullptr;
}

napi_value JsScreenSessionManager::UpdateScreenRotationProperty(napi_env env, napi_callback_info info)
{
    WLOGD("Update screen rotation property.");
    JsScreenSessionManager* me = CheckParamsAndGetThis<JsScreenSessionManager>(env, info);
    return (me != nullptr) ? me->OnUpdateScreenRotationProperty(env, info) : nullptr;
}

napi_value JsScreenSessionManager::OnRegisterCallback(napi_env env, const napi_callback_info info)
{
    WLOGD("On register callback.");
    if (screenConnectionCallback_ != nullptr) {
        return NapiGetUndefined(env);
    }
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 2) { // 2: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    std::string callbackType;
    if (!ConvertFromJsValue(env, argv[0], callbackType)) {
        WLOGFE("Failed to convert parameter to callback type.");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    if (callbackType != ON_SCREEN_CONNECTION_CHANGE_CALLBACK) {
        WLOGFE("Unsupported callback type: %{public}s.", callbackType.c_str());
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    napi_value value = argv[1];
    if (!NapiIsCallable(env, value)) {
        WLOGFE("Failed to register callback, callback is not callable!");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    napi_ref result = nullptr;
    napi_create_reference(env, value, 1, &result);
    std::shared_ptr<NativeReference> callbackRef(reinterpret_cast<NativeReference*>(result));
    screenConnectionCallback_ = callbackRef;
    ScreenSessionManagerClient::GetInstance().RegisterScreenConnectionListener(this);
    return NapiGetUndefined(env);
}

napi_value JsScreenSessionManager::OnUpdateScreenRotationProperty(napi_env env,
    const napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 3) { // 3: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t screenId;
    if (!ConvertFromJsValue(env, argv[0], screenId)) {
        WLOGFE("[NAPI]Failed to convert parameter to screenId");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    RRect bounds;
    napi_value nativeObj = argv[1];
    if (nativeObj == nullptr) {
        WLOGFE("[NAPI]Failed to convert object to RRect bounds");
        return NapiGetUndefined(env);
    } else if (!ConvertRRectFromJs(env, nativeObj, bounds)) {
        WLOGFE("[NAPI]Failed to get bounds from js object");
        return NapiGetUndefined(env);
    }
    int rotation;
    if (!ConvertFromJsValue(env, argv[2], rotation)) { // 2: the 3rd argv
        WLOGFE("[NAPI]Failed to convert parameter to rotation");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    ScreenSessionManagerClient::GetInstance().UpdateScreenRotationProperty(screenId, bounds, rotation);
    return NapiGetUndefined(env);
}

napi_value JsScreenSessionManager::NotifyScreenLockEvent(napi_env env, napi_callback_info info)
{
    WLOGD("Notify screen lock event.");
    JsScreenSessionManager* me = CheckParamsAndGetThis<JsScreenSessionManager>(env, info);
    return (me != nullptr) ? me->OnNotifyScreenLockEvent(env, info) : nullptr;
}

napi_value JsScreenSessionManager::OnNotifyScreenLockEvent(napi_env env,
    const napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t event;
    if (!ConvertFromJsValue(env, argv[0], event)) {
        WLOGFE("[NAPI]Failed to convert parameter to display event");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    DisplayManager::GetInstance().NotifyDisplayEvent(static_cast<DisplayEvent>(event));
    return NapiGetUndefined(env);
}

napi_value JsScreenSessionManager::GetCurvedCompressionArea(napi_env env, napi_callback_info info)
{
    WLOGD("[NAPI]GetCurvedCompressionArea");
    JsScreenSessionManager* me = CheckParamsAndGetThis<JsScreenSessionManager>(env, info);
    return (me != nullptr) ? me->OnGetCurvedCompressionArea(env, info) : nullptr;
}

napi_value JsScreenSessionManager::OnGetCurvedCompressionArea(napi_env env, const napi_callback_info info)
{
    WLOGD("[NAPI]OnGetCurvedCompressionArea");
    napi_value result = nullptr;
    napi_create_uint32(env, ScreenSessionManagerClient::GetInstance().GetCurvedCompressionArea(), &result);
    return result;
}

napi_value JsScreenSessionManager::OnGetPhyScreenProperty(napi_env env, const napi_callback_info info)
{
    WLOGD("[NAPI]OnGetPhyScreenProperty");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t screenId;
    if (!ConvertFromJsValue(env, argv[0], screenId)) {
        WLOGFE("[NAPI]Failed to convert parameter to screenId");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    ScreenProperty screenProperty = ScreenSessionManagerClient::GetInstance().GetPhyScreenProperty(screenId);
    return JsScreenUtils::CreateJsScreenProperty(env, screenProperty);
}
} // namespace OHOS::Rosen
