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

#include "js_embeddable_window_stage.h"
#include <string>
#include "js_runtime_utils.h"
#include "js_extension_window.h"
#include "js_extension_window_utils.h"
#include "window_manager_hilog.h"
#include "permission.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsEmbeddableWindowStage"};
} // namespace

JsEmbeddableWindowStage::JsEmbeddableWindowStage(sptr<Rosen::Window> window, sptr<AAFwk::SessionInfo> sessionInfo)
    : windowExtensionSessionImpl_(window), sessionInfo_(sessionInfo),
    extwinRegisterManager_(std::make_unique<JsExtensionWindowRegisterManager>())
{
}

JsEmbeddableWindowStage::~JsEmbeddableWindowStage()
{
}

void JsEmbeddableWindowStage::Finalizer(napi_env env, void* data, void* hint)
{
    WLOGI("[NAPI]Finalizer");
    std::unique_ptr<JsEmbeddableWindowStage>(static_cast<JsEmbeddableWindowStage*>(data));
}

napi_value JsEmbeddableWindowStage::GetMainWindow(napi_env env, napi_callback_info info)
{
    WLOGFD("[NAPI]GetMainWindow");
    JsEmbeddableWindowStage* me = CheckParamsAndGetThis<JsEmbeddableWindowStage>(env, info);
    return (me != nullptr) ? me->OnGetMainWindow(env, info) : nullptr;
}

napi_value JsEmbeddableWindowStage::GetMainWindowSync(napi_env env, napi_callback_info info)
{
    WLOGFD("[NAPI]GetMainWindowSync");
    JsEmbeddableWindowStage* me = CheckParamsAndGetThis<JsEmbeddableWindowStage>(env, info);
    return (me != nullptr) ? me->OnGetMainWindowSync(env, info) : nullptr;
}

napi_value JsEmbeddableWindowStage::On(napi_env env, napi_callback_info info)
{
    WLOGFD("[NAPI]On");
    JsEmbeddableWindowStage* me = CheckParamsAndGetThis<JsEmbeddableWindowStage>(env, info);
    return (me != nullptr) ? me->OnEvent(env, info) : nullptr;
}

napi_value JsEmbeddableWindowStage::Off(napi_env env, napi_callback_info info)
{
    WLOGFD("[NAPI]Off");
    JsEmbeddableWindowStage* me = CheckParamsAndGetThis<JsEmbeddableWindowStage>(env, info);
    return (me != nullptr) ? me->OffEvent(env, info) : nullptr;
}

napi_value JsEmbeddableWindowStage::LoadContent(napi_env env, napi_callback_info info)
{
    WLOGFD("[NAPI]LoadContent");
    JsEmbeddableWindowStage* me = CheckParamsAndGetThis<JsEmbeddableWindowStage>(env, info);
    return (me != nullptr) ? me->OnLoadContent(env, info, false) : nullptr;
}

napi_value JsEmbeddableWindowStage::LoadContentByName(napi_env env, napi_callback_info info)
{
    WLOGFD("[NAPI]LoadContentByName");
    JsEmbeddableWindowStage* me = CheckParamsAndGetThis<JsEmbeddableWindowStage>(env, info);
    return (me != nullptr) ? me->OnLoadContent(env, info, true) : nullptr;
}

napi_value JsEmbeddableWindowStage::OnGetMainWindow(napi_env env, napi_callback_info info)
{
    NapiAsyncTask::CompleteCallback complete =
        [windowImpl = windowExtensionSessionImpl_, sessionInfo = sessionInfo_]
        (napi_env env, NapiAsyncTask& task, int32_t status) {
            if (windowImpl != nullptr) {
                task.Resolve(env, Rosen::JsExtensionWindow::CreateJsExtensionWindowObject(env,
                windowImpl, sessionInfo));
                WLOGI("[NAPI]Get main window [%{public}u, %{public}s]",
                    windowImpl->GetWindowId(), windowImpl->GetWindowName().c_str());
            } else {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "Get main window failed."));
            }
        };
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value callback = (argv[0] != nullptr && GetType(env, argv[0]) == napi_function) ?
        argv[0] : nullptr;
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsEmbeddableWindowStage::OnGetMainWindow",
        env, CreateAsyncTaskWithLastParam(env, callback, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsEmbeddableWindowStage::OnGetMainWindowSync(napi_env env, napi_callback_info info)
{
    if (windowExtensionSessionImpl_ == nullptr) {
        WLOGFE("[NAPI]window is null");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return NapiGetUndefined(env);
    }

    return Rosen::JsExtensionWindow::CreateJsExtensionWindowObject(env,
        windowExtensionSessionImpl_, sessionInfo_);
}

napi_value JsEmbeddableWindowStage::OnEvent(napi_env env, napi_callback_info info)
{
    sptr<Window> windowImpl = windowExtensionSessionImpl_;
    if (windowImpl == nullptr) {
        WLOGFE("[NAPI]windowImpl is null");
        return NapiGetUndefined(env);
    }
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 2) { // 2: minimum param nums
        WLOGFE("[NAPI]argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    // Parse argv[0] as string
    std::string eventString;
    if (!ConvertFromJsValue(env, argv[0], eventString)) {
        WLOGFE("[NAPI]Failed to convert parameter to string");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    napi_value value = argv[1];
    if (!NapiIsCallable(env, value)) {
        WLOGFE("[NAPI]Callback(argv[1]) is not callable");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    WmErrorCode ret = extwinRegisterManager_->RegisterListener(windowImpl, eventString,
        CaseType::CASE_STAGE, env, value);
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("[NAPI]RegisterListener fail");
        return NapiThrowError(env, ret);
    }
    WLOGI("[NAPI]Window [%{public}u, %{public}s] register event %{public}s",
        windowImpl->GetWindowId(), windowImpl->GetWindowName().c_str(), eventString.c_str());

    return NapiGetUndefined(env);
}

napi_value JsEmbeddableWindowStage::OffEvent(napi_env env, napi_callback_info info)
{
    sptr<Window> windowImpl = windowExtensionSessionImpl_;
    if (windowImpl == nullptr) {
        WLOGFE("[NAPI]windowImpl is null");
        return NapiGetUndefined(env);
    }
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    // Parse argv[0] as string
    std::string eventString;
    if (!ConvertFromJsValue(env, argv[0], eventString)) {
        WLOGFE("[NAPI]Failed to convert parameter to string");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    if (eventString.compare("windowStageEvent") != 0) {
        WLOGFE("[NAPI]Envent %{public}s is invalid", eventString.c_str());
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    napi_value value = nullptr;
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (argc == 1) {
        ret = extwinRegisterManager_->UnregisterListener(windowImpl, eventString, CaseType::CASE_STAGE, env, nullptr);
    } else {
        value = argv[1];
        if (value != nullptr && GetType(env, value) == napi_function) {
            ret = extwinRegisterManager_->UnregisterListener(windowImpl, eventString, CaseType::CASE_STAGE, env, value);
        } else {
            ret = extwinRegisterManager_->UnregisterListener(windowImpl, eventString,
                CaseType::CASE_STAGE, env, nullptr);
        }
    }
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("[NAPI]UnregisterListener fail");
        return NapiThrowError(env, ret);
    }
    WLOGI("[NAPI]Window [%{public}u, %{public}s] unregister event %{public}s",
        windowImpl->GetWindowId(), windowImpl->GetWindowName().c_str(), eventString.c_str());

    return NapiGetUndefined(env);
}

static void LoadContentTask(std::shared_ptr<NativeReference> contentStorage, std::string contextUrl,
    sptr<Window> windowImpl, napi_env env, NapiAsyncTask& task, sptr<IRemoteObject> parentToken,
    bool isLoadedByName)
{
    napi_value nativeStorage = (contentStorage == nullptr) ? nullptr : contentStorage->GetNapiValue();
    WMError ret;
    if (isLoadedByName) {
        ret = windowImpl->SetUIContentByName(contextUrl, env, nativeStorage);
    } else {
        ret = windowImpl->NapiSetUIContent(contextUrl, env, nativeStorage, false, parentToken);
    }
    if (ret == WMError::WM_OK) {
        task.Resolve(env, NapiGetUndefined(env));
    } else {
        task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window load content failed"));
    }
    WLOGI("[NAPI]Window [%{public}u, %{public}s] load content end, ret = %{public}d",
        windowImpl->GetWindowId(), windowImpl->GetWindowName().c_str(), ret);
    return;
}

napi_value JsEmbeddableWindowStage::OnLoadContent(napi_env env, napi_callback_info info, bool isLoadedByName)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    std::string contextUrl;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (!ConvertFromJsValue(env, argv[0], contextUrl)) {
        WLOGFE("[NAPI]Failed to convert parameter to context url");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    napi_value storage = nullptr;
    napi_value callBack = nullptr;
    napi_value value1 = argv[1];
    napi_value value2 = argv[2]; // 2: param index
    if (GetType(env, value1) == napi_function) {
        callBack = value1;
    } else if (GetType(env, value1) == napi_object) {
        storage = value1;
    }
    if (GetType(env, value2) == napi_function) {
        callBack = value2;
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        WLOGFE("[NAPI]Window scene is null or get invalid param");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    std::shared_ptr<NativeReference> contentStorage = nullptr;
    if (storage != nullptr) {
        napi_ref result = nullptr;
        napi_create_reference(env, storage, 1, &result);
        contentStorage = std::shared_ptr<NativeReference>(reinterpret_cast<NativeReference*>(result));
    }

    sptr<IRemoteObject> parentToken = sessionInfo_->parentToken;
    NapiAsyncTask::CompleteCallback complete =
        [window = windowExtensionSessionImpl_, contentStorage, contextUrl, parentToken, isLoadedByName](
            napi_env env, NapiAsyncTask& task, int32_t status) {
            if (window == nullptr) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                WLOGFE("[NAPI]Get windowExtensionSessionImpl failed");
                return;
            }
            LoadContentTask(contentStorage, contextUrl, window, env, task, parentToken, isLoadedByName);
        };
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsEmbeddableWindowStage::OnLoadContent",
        env, CreateAsyncTaskWithLastParam(env, callBack, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsEmbeddableWindowStage::CreateJsEmbeddableWindowStage(napi_env env, sptr<Rosen::Window> window,
    sptr<AAFwk::SessionInfo> sessionInfo)
{
    WLOGFD("[NAPI]CreateJsEmbeddableWindowStage");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);

    std::unique_ptr<JsEmbeddableWindowStage> jsEmbeddableWindowStage = std::make_unique<JsEmbeddableWindowStage>(window,
        sessionInfo);
    napi_wrap(env, objValue, jsEmbeddableWindowStage.release(), JsEmbeddableWindowStage::Finalizer, nullptr, nullptr);

    const char *moduleName = "JsEmbeddableWindowStage";
    BindNativeFunction(env,
        objValue, "loadContent", moduleName, JsEmbeddableWindowStage::LoadContent);
    BindNativeFunction(env,
        objValue, "loadContentByName", moduleName, JsEmbeddableWindowStage::LoadContentByName);
    BindNativeFunction(env,
        objValue, "getMainWindow", moduleName, JsEmbeddableWindowStage::GetMainWindow);
    BindNativeFunction(env,
        objValue, "getMainWindowSync", moduleName, JsEmbeddableWindowStage::GetMainWindowSync);
    BindNativeFunction(env, objValue, "on", moduleName, JsEmbeddableWindowStage::On);
    BindNativeFunction(env, objValue, "off", moduleName, JsEmbeddableWindowStage::Off);

    return objValue;
}
}  // namespace Rosen
}  // namespace OHOS
