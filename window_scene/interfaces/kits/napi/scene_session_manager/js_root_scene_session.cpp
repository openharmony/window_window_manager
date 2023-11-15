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

#include "js_root_scene_session.h"
#include "session_manager/include/scene_session_manager.h"

#include "context.h"
#include <js_runtime_utils.h>
#include "window_manager_hilog.h"

#include "js_scene_utils.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsRootSceneSession" };
const std::string PENDING_SCENE_CB = "pendingSceneSessionActivation";
} // namespace

JsRootSceneSession::JsRootSceneSession(napi_env env, const sptr<RootSceneSession>& rootSceneSession)
    : env_(env), rootSceneSession_(rootSceneSession)
{}

napi_value JsRootSceneSession::Create(napi_env env, const sptr<RootSceneSession>& rootSceneSession)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("[NAPI]Object is null!");
        return NapiGetUndefined(env);
    }

    auto jsRootSceneSession = std::make_unique<JsRootSceneSession>(env, rootSceneSession);
    napi_wrap(env, objValue, jsRootSceneSession.release(), JsRootSceneSession::Finalizer, nullptr, nullptr);

    const char* moduleName = "JsRootSceneSession";
    BindNativeFunction(env, objValue, "loadContent", moduleName, JsRootSceneSession::LoadContent);
    BindNativeFunction(env, objValue, "on", moduleName, JsRootSceneSession::RegisterCallback);
    return objValue;
}

void JsRootSceneSession::Finalizer(napi_env env, void* data, void* hint)
{
    WLOGD("Finalizer.");
    std::unique_ptr<JsRootSceneSession>(static_cast<JsRootSceneSession*>(data));
}

napi_value JsRootSceneSession::RegisterCallback(napi_env env, napi_callback_info info)
{
    WLOGD("RegisterCallback.");
    JsRootSceneSession* me = CheckParamsAndGetThis<JsRootSceneSession>(env, info);
    return (me != nullptr) ? me->OnRegisterCallback(env, info) : nullptr;
}

napi_value JsRootSceneSession::LoadContent(napi_env env, napi_callback_info info)
{
    WLOGD("LoadContent.");
    JsRootSceneSession* me = CheckParamsAndGetThis<JsRootSceneSession>(env, info);
    return (me != nullptr) ? me->OnLoadContent(env, info) : nullptr;
}

bool NapiIsCallable(napi_env env, napi_value value)
{
    bool result = false;
    napi_is_callable(env, value, &result);
    return result;
}

napi_value JsRootSceneSession::OnRegisterCallback(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_TWO) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    std::string cbType;
    if (!ConvertFromJsValue(env, argv[0], cbType)) {
        WLOGFE("[NAPI]Failed to convert parameter to callbackType");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    napi_value value = argv[1];
    if (value == nullptr || !NapiIsCallable(env, value)) {
        WLOGFE("[NAPI]Invalid argument");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    if (IsCallbackRegistered(env, cbType, value)) {
        return NapiGetUndefined(env);
    }
    if (rootSceneSession_ == nullptr) {
        WLOGFE("[NAPI]root session is nullptr");
        napi_throw(env, CreateJsError(
            env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM), "Root scene session is null!"));
        return NapiGetUndefined(env);
    }

    NotifyPendingSessionActivationFunc func = [this](SessionInfo& info) {
        this->PendingSessionActivation(info);
    };
    rootSceneSession_->SetPendingSessionActivationEventListener(func);
    std::shared_ptr<NativeReference> callbackRef;
    napi_ref result = nullptr;
    napi_create_reference(env, value, 1, &result);
    callbackRef.reset(reinterpret_cast<NativeReference*>(result));
    jsCbMap_[cbType] = callbackRef;
    WLOGFD("[NAPI]Register end, type = %{public}s", cbType.c_str());
    return NapiGetUndefined(env);
}

napi_value JsRootSceneSession::OnLoadContent(napi_env env, napi_callback_info info)
{
    WLOGD("[NAPI]OnLoadContent");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_TWO) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    std::string contentUrl;
    napi_value context = argv[1];
    napi_value storage = argc < 3 ? nullptr : argv[2];
    if (!ConvertFromJsValue(env, argv[0], contentUrl)) {
        WLOGFE("[NAPI]Failed to convert parameter to content url");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    if (context == nullptr) {
        WLOGFE("[NAPI]Failed to get context object");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY)));
        return NapiGetUndefined(env);
    }
    void* pointerResult = nullptr;
    napi_unwrap(env, context, &pointerResult);
    auto contextNativePointer = static_cast<std::weak_ptr<Context>*>(pointerResult);
    if (contextNativePointer == nullptr) {
        WLOGFE("[NAPI]Failed to get context pointer from js object");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY)));
        return NapiGetUndefined(env);
    }
    auto contextWeakPtr = *contextNativePointer;
    SceneSessionManager::GetInstance().SetRootSceneContext(contextWeakPtr);

    std::shared_ptr<NativeReference> contentStorage = nullptr;
    if (storage != nullptr) {
        napi_ref ref = nullptr;
        napi_create_reference(env, storage, 1, &ref);
        contentStorage = std::shared_ptr<NativeReference>(reinterpret_cast<NativeReference*>(ref));
    }

    NapiAsyncTask::CompleteCallback complete = [rootSceneSession = rootSceneSession_,
        contentUrl, contextWeakPtr, contentStorage](napi_env env, NapiAsyncTask& task, int32_t status) {
        if (rootSceneSession == nullptr) {
            WLOGFE("[NAPI]rootSceneSession is nullptr");
            task.Reject(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY)));
            return;
        }
        napi_value nativeStorage = contentStorage ? contentStorage->GetNapiValue() : nullptr;
        rootSceneSession->LoadContent(contentUrl, env, nativeStorage, contextWeakPtr.lock().get());
    };
    napi_value lastParam = nullptr, result = nullptr;
    NapiAsyncTask::Schedule("JsRootSceneSession::OnLoadContent", env,
        CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

bool JsRootSceneSession::IsCallbackRegistered(napi_env env, std::string type, napi_value jsListenerObject)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGFI("[NAPI]Method %{public}s has not been registered", type.c_str());
        return false;
    }

    for (auto iter = jsCbMap_.begin(); iter != jsCbMap_.end(); ++iter) {
        bool isEquals = false;
        napi_strict_equals(env, jsListenerObject, iter->second->GetNapiValue(), &isEquals);
        if (isEquals) {
            WLOGFE("[NAPI]Method %{public}s has already been registered", type.c_str());
            return true;
        }
    }
    return false;
}

void JsRootSceneSession::PendingSessionActivation(SessionInfo& info)
{
    WLOGI("[NAPI]pending session activation: bundleName %{public}s, moduleName %{public}s, abilityName %{public}s, \
        appIndex %{public}d, reuse %{public}d", info.bundleName_.c_str(), info.moduleName_.c_str(),
        info.abilityName_.c_str(), info.appIndex_, info.reuse);
    sptr<SceneSession> sceneSession = GenSceneSession(info);
    if (sceneSession == nullptr) {
        WLOGFE("RequestSceneSession return nullptr");
        return;
    }
    
    if (info.want != nullptr) {
        bool isNeedBackToOther = info.want->GetBoolParam(AAFwk::Want::PARAM_BACK_TO_OTHER_MISSION_STACK, false);
        WLOGFI("[NAPI]isNeedBackToOther: %{public}d", isNeedBackToOther);
        if (isNeedBackToOther) {
            int32_t realCallerSessionId = SceneSessionManager::GetInstance().GetFocusedSession();
            WLOGFI("[NAPI]need to back to other session: %{public}d", realCallerSessionId);
            if (sceneSession != nullptr) {
                sceneSession->SetSessionInfoCallerPersistentId(realCallerSessionId);
            }
            info.callerPersistentId_ = realCallerSessionId;
        } else {
            info.callerPersistentId_ = 0;
            if (sceneSession != nullptr) {
                sceneSession->SetSessionInfoCallerPersistentId(0);
            }
        }
    }
    auto iter = jsCbMap_.find(PENDING_SCENE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }

    auto jsCallBack = iter->second;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [info, jsCallBack](napi_env env, NapiAsyncTask& task, int32_t status) {
            napi_value jsSessionInfo = CreateJsSessionInfo(env, info);
            if (jsSessionInfo == nullptr) {
                WLOGFE("[NAPI]this target session info is nullptr");
                return;
            }
            napi_value argv[] = { jsSessionInfo };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::PendingSessionActivation", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

sptr<SceneSession> JsRootSceneSession::GenSceneSession(SessionInfo& info)
{
    sptr<SceneSession> sceneSession;
    if (info.persistentId_ == 0) {
        auto result = SceneSessionManager::GetInstance().CheckIfReuseSession(info);
        if (result == BrokerStates::BROKER_NOT_START) {
            WLOGE("[NAPI] The BrokerStates is not opened");
            return nullptr;
        }

        if (info.reuse) {
            if (SceneSessionManager::GetInstance().CheckCollaboratorType(info.collaboratorType_)) {
                sceneSession = SceneSessionManager::GetInstance().FindSessionByAffinity(
                    info.sessionAffinity);
            } else {
                sceneSession = SceneSessionManager::GetInstance().GetSceneSessionByName(
                    info.bundleName_, info.moduleName_, info.abilityName_, info.appIndex_);
            }
        }
        if (sceneSession == nullptr) {
            WLOGFI("GetSceneSessionByName return nullptr, RequestSceneSession");
            sceneSession = SceneSessionManager::GetInstance().RequestSceneSession(info);
            if (sceneSession == nullptr) {
                WLOGFE("RequestSceneSession return nullptr");
                return sceneSession;
            }
        } else {
            sceneSession->SetSessionInfo(info);
        }
        info.persistentId_ = sceneSession->GetPersistentId();
        sceneSession->SetSessionInfoPersistentId(sceneSession->GetPersistentId());
    } else {
        sceneSession = SceneSessionManager::GetInstance().GetSceneSession(info.persistentId_);
        if (sceneSession == nullptr) {
            WLOGFE("GetSceneSession return nullptr");
            return sceneSession;
        }
        sceneSession->SetSessionInfo(info);
    }
    return sceneSession;
}
} // namespace OHOS::Rosen
