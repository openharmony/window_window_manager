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

#include "js_scene_session_manager.h"

#include <memory>

#include <js_runtime_utils.h>

#include "js_scene_utils.h"
#include "js_root_scene_session.h"
#include "js_scene_session.h"
#include "session_manager/include/scene_session_manager.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsSceneSessionManager"};
}

NativeValue* JsSceneSessionManager::Init(NativeEngine* engine, NativeValue* exportObj)
{
    WLOGFI("[NAPI]JsSceneSessionManager Init");
    if (engine == nullptr || exportObj == nullptr) {
        WLOGFE("Engine or exportObj is null!");
        return nullptr;
    }

    auto object = ConvertNativeValueTo<NativeObject>(exportObj);
    if (object == nullptr) {
        WLOGFE("Object is null");
        return nullptr;
    }

    std::unique_ptr<JsSceneSessionManager> jsSceneSessionManager = std::make_unique<JsSceneSessionManager>();
    object->SetNativePointer(jsSceneSessionManager.release(), JsSceneSessionManager::Finalizer, nullptr);

    const char *moduleName = "JsSceneSessionManager";
    BindNativeFunction(*engine, *object, "getRootSceneSession", moduleName,
        JsSceneSessionManager::GetRootSceneSession);
    BindNativeFunction(*engine, *object, "requestSceneSession", moduleName,
        JsSceneSessionManager::RequestSceneSession);
    BindNativeFunction(*engine, *object, "requestSceneSessionActivation", moduleName,
        JsSceneSessionManager::RequestSceneSessionActivation);
    BindNativeFunction(*engine, *object, "requestSceneSessionBackground", moduleName,
        JsSceneSessionManager::RequestSceneSessionBackground);
    BindNativeFunction(*engine, *object, "requestSceneSessionDestruction", moduleName,
        JsSceneSessionManager::RequestSceneSessionDestruction);
    return engine->CreateUndefined();
}

void JsSceneSessionManager::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFI("[NAPI]Finalizer");
    std::unique_ptr<JsSceneSessionManager>(static_cast<JsSceneSessionManager*>(data));
}

NativeValue* JsSceneSessionManager::GetRootSceneSession(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]GetRootSceneSession");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(engine, info);
    return (me != nullptr) ? me->OnGetRootSceneSession(*engine, *info) : nullptr;
}

NativeValue* JsSceneSessionManager::RequestSceneSession(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]RequestSceneSession");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(engine, info);
    return (me != nullptr) ? me->OnRequestSceneSession(*engine, *info) : nullptr;
}

NativeValue* JsSceneSessionManager::RequestSceneSessionActivation(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]RequestSceneSession");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(engine, info);
    return (me != nullptr) ? me->OnRequestSceneSessionActivation(*engine, *info) : nullptr;
}

NativeValue* JsSceneSessionManager::RequestSceneSessionBackground(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]RequestSceneSession");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(engine, info);
    return (me != nullptr) ? me->OnRequestSceneSessionBackground(*engine, *info) : nullptr;
}

NativeValue* JsSceneSessionManager::RequestSceneSessionDestruction(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]RequestSceneSession");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(engine, info);
    return (me != nullptr) ? me->OnRequestSceneSessionDestruction(*engine, *info) : nullptr;
}

NativeValue* JsSceneSessionManager::OnGetRootSceneSession(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("[NAPI]OnGetRootSceneSession");
    sptr<RootSceneSession> rootSceneSession = SceneSessionManager::GetInstance().GetRootSceneSession();
    if (rootSceneSession == nullptr) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY),
            "System is abnormal"));
        return engine.CreateUndefined();
    } else {
        NativeValue* jsRootSceneSessionObj = JsRootSceneSession::Create(engine, rootSceneSession);
        if (jsRootSceneSessionObj == nullptr) {
            WLOGFE("[NAPI]jsRootSceneSessionObj is nullptr");
            engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY),
                "System is abnormal"));
        }
        return jsRootSceneSessionObj;
    }
}

NativeValue* JsSceneSessionManager::OnRequestSceneSession(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("[NAPI]OnRequestSceneSession");
    WSErrorCode errCode = WSErrorCode::WS_OK;
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
    }
    WindowSession::AbilityInfo abilityInfo;
    if (errCode == WSErrorCode::WS_OK) {
        // parse ability info
        NativeObject* nativeObj = ConvertNativeValueTo<NativeObject>(info.argv[0]);
        if (nativeObj == nullptr) {
            WLOGFE("[NAPI]Failed to convert object to AbilityInfo");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        } else {
            if (!GetAbilityInfoFromJs(engine, nativeObj, abilityInfo)) {
                WLOGFE("[NAPI]Failed to GetSystemBarProperties From Js Object");
                errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
            }
        }

        // // parse scene option
        // NativeNumber* nativeOption = ConvertNativeValueTo<NativeNumber>(info.argv[1]);
        // if (nativeOption == nullptr) {
        //     WLOGFE("[NAPI]Failed to convert parameter to session option");
        //     errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        // } else {
        //     sessionOption = static_cast<SessionOption>(static_cast<uint32_t>(*nativeOption));
        // }
    }

    if (errCode == WSErrorCode::WS_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    WLOGFI("[NAPI]SceneInfo [%{public}s, %{public}s], err = %{public}d",
        abilityInfo.bundleName_.c_str(), abilityInfo.abilityName_.c_str(), errCode);
    sptr<SceneSession> sceneSession = SceneSessionManager::GetInstance().RequestSceneSession(abilityInfo);
    if (sceneSession == nullptr) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY),
            "System is abnormal"));
        return engine.CreateUndefined();
    } else {
        NativeValue* jsSceneSessionObj = CreateJsSceneSessionObject(engine, sceneSession);
        if (jsSceneSessionObj == nullptr) {
            WLOGFE("[NAPI]SceneInfo [%{public}s, %{public}s], jsSceneSessionObj is nullptr",
            abilityInfo.bundleName_.c_str(), abilityInfo.abilityName_.c_str());
            engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY),
                "System is abnormal"));
        }
        return jsSceneSessionObj;
    }
}

NativeValue* JsSceneSessionManager::OnRequestSceneSessionActivation(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("[NAPI]OnRequestSceneSessionActivation");
    WSErrorCode errCode = WSErrorCode::WS_OK;
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
    }

    SceneSession* sceneSession = nullptr;
    if (errCode == WSErrorCode::WS_OK) {
        // find scene session
        auto jsSceneSessionObj = ConvertNativeValueTo<NativeObject>(info.argv[0]);
        if (jsSceneSessionObj == nullptr) {
            WLOGFE("[NAPI]Failed to get js scene session object");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        } else {
            auto jsSceneSession = static_cast<JsSceneSession*>(jsSceneSessionObj->GetNativePointer());
            if (jsSceneSession == nullptr) {
                WLOGFE("[NAPI]Failed to find scene session From Js Object");
                errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
            }
            sceneSession = jsSceneSession->GetNativeSession().GetRefPtr();
        }
    }

    if (errCode == WSErrorCode::WS_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }

    sceneSession->NotifyForeground();

    WLOGFI("[NAPI]RequestSceneSessionActivation: SceneInfo [%{public}s, %{public}s]",
        sceneSession->GetAbilityInfo().bundleName_.c_str(), sceneSession->GetAbilityInfo().abilityName_.c_str());
    AsyncTask::CompleteCallback complete =
        [sceneSession](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (sceneSession == nullptr) {
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY),
                    "Invalidate params."));
                return;
            }
            WSErrorCode ret = WS_JS_TO_ERROR_CODE_MAP.at(
                SceneSessionManager::GetInstance().RequestSceneSessionActivation(sceneSession));
            if (ret == WSErrorCode::WS_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret),
                    "Request scene session activation failed"));
            }
            WLOGFI("[NAPI]Session [%{public}s] request scene session activation end",
                sceneSession->GetAbilityInfo().abilityName_.c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        ((info.argv[1] != nullptr && info.argv[1]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsSceneSessionManager::OnRequestSceneSessionActivation",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsSceneSessionManager::OnRequestSceneSessionBackground(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("[NAPI]OnRequestSceneSessionBackground");
    WSErrorCode errCode = WSErrorCode::WS_OK;
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
    }

    SceneSession* sceneSession = nullptr;
    if (errCode == WSErrorCode::WS_OK) {
        // find scene session
        auto jsSceneSessionObj = ConvertNativeValueTo<NativeObject>(info.argv[0]);
        if (jsSceneSessionObj == nullptr) {
            WLOGFE("[NAPI]Failed to get js scene session object");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        } else {
            auto jsSceneSession = static_cast<JsSceneSession*>(jsSceneSessionObj->GetNativePointer());
            if (jsSceneSession == nullptr) {
                WLOGFE("[NAPI]Failed to find scene session From Js Object");
                errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
            }
            sceneSession = jsSceneSession->GetNativeSession().GetRefPtr();
        }
    }

    if (errCode == WSErrorCode::WS_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }

    sceneSession->NotifyBackground();

    WLOGFI("[NAPI]RequestSceneSessionBackground: SceneInfo [%{public}s, %{public}s]",
        sceneSession->GetAbilityInfo().bundleName_.c_str(), sceneSession->GetAbilityInfo().abilityName_.c_str());
    AsyncTask::CompleteCallback complete =
        [sceneSession](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (sceneSession == nullptr) {
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY),
                    "Invalidate params."));
                return;
            }
            WSErrorCode ret = WS_JS_TO_ERROR_CODE_MAP.at(
                    SceneSessionManager::GetInstance().RequestSceneSessionBackground(sceneSession));
            if (ret == WSErrorCode::WS_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret),
                    "Request scene session background failed"));
            }
            WLOGFI("[NAPI]Session [%{public}s] request scene session background end",
                sceneSession->GetAbilityInfo().abilityName_.c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        ((info.argv[1] != nullptr && info.argv[1]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsSceneSessionManager::OnRequestSceneSessionBackground",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsSceneSessionManager::OnRequestSceneSessionDestruction(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("[NAPI]OnRequestSceneSessionDestruction");
    WSErrorCode errCode = WSErrorCode::WS_OK;
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
    }

    SceneSession* sceneSession = nullptr;
    if (errCode == WSErrorCode::WS_OK) {
        // find scene session
        auto jsSceneSessionObj = ConvertNativeValueTo<NativeObject>(info.argv[0]);
        if (jsSceneSessionObj == nullptr) {
            WLOGFE("[NAPI]Failed to get js scene session object");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        } else {
            auto jsSceneSession = static_cast<JsSceneSession*>(jsSceneSessionObj->GetNativePointer());
            if (jsSceneSession == nullptr) {
                WLOGFE("[NAPI]Failed to find scene session From Js Object");
                errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
            }
            sceneSession = jsSceneSession->GetNativeSession().GetRefPtr();
        }
    }

    if (errCode == WSErrorCode::WS_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }

    WLOGFI("[NAPI]RequestSceneSessionDestruction: SceneInfo [%{public}s, %{public}s]",
        sceneSession->GetAbilityInfo().bundleName_.c_str(), sceneSession->GetAbilityInfo().abilityName_.c_str());
    AsyncTask::CompleteCallback complete =
        [sceneSession](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (sceneSession == nullptr) {
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY),
                    "Invalidate params."));
                return;
            }
            WSErrorCode ret = WS_JS_TO_ERROR_CODE_MAP.at(
                    SceneSessionManager::GetInstance().RequestSceneSessionDestruction(sceneSession));
            if (ret == WSErrorCode::WS_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret),
                    "Request scene session destruction failed"));
            }
            WLOGFI("[NAPI]Session [%{public}s] request scene session destruction end",
                sceneSession->GetAbilityInfo().abilityName_.c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        ((info.argv[1] != nullptr && info.argv[1]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsSceneSessionManager::OnRequestSceneSessionDestruction",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}
}
