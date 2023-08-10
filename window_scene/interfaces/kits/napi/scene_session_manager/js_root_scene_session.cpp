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

JsRootSceneSession::JsRootSceneSession(NativeEngine& engine, const sptr<RootSceneSession>& rootSceneSession)
    : engine_(engine), rootSceneSession_(rootSceneSession)
{}

NativeValue* JsRootSceneSession::Create(NativeEngine& engine, const sptr<RootSceneSession>& rootSceneSession)
{
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Object is null!");
        return engine.CreateUndefined();
    }

    auto jsRootSceneSession = std::make_unique<JsRootSceneSession>(engine, rootSceneSession);
    object->SetNativePointer(jsRootSceneSession.release(), JsRootSceneSession::Finalizer, nullptr);

    const char* moduleName = "JsRootSceneSession";
    BindNativeFunction(engine, *object, "loadContent", moduleName, JsRootSceneSession::LoadContent);
    BindNativeFunction(engine, *object, "on", moduleName, JsRootSceneSession::RegisterCallback);
    return objValue;
}

void JsRootSceneSession::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGD("Finalizer.");
    std::unique_ptr<JsRootSceneSession>(static_cast<JsRootSceneSession*>(data));
}

NativeValue* JsRootSceneSession::RegisterCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGD("RegisterCallback.");
    JsRootSceneSession* me = CheckParamsAndGetThis<JsRootSceneSession>(engine, info);
    return (me != nullptr) ? me->OnRegisterCallback(*engine, *info) : nullptr;
}

NativeValue* JsRootSceneSession::LoadContent(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGD("LoadContent.");
    JsRootSceneSession* me = CheckParamsAndGetThis<JsRootSceneSession>(engine, info);
    return (me != nullptr) ? me->OnLoadContent(*engine, *info) : nullptr;
}

NativeValue* JsRootSceneSession::OnRegisterCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc < 2) { // 2: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    std::string cbType;
    if (!ConvertFromJsValue(engine, info.argv[0], cbType)) {
        WLOGFE("[NAPI]Failed to convert parameter to callbackType");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    NativeValue* value = info.argv[1];
    if (value == nullptr || !value->IsCallable()) {
        WLOGFE("[NAPI]Invalid argument");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    if (IsCallbackRegistered(cbType, value)) {
        return engine.CreateUndefined();
    }
    if (rootSceneSession_ == nullptr) {
        WLOGFE("[NAPI]root session is nullptr");
        engine.Throw(CreateJsError(
            engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM), "Root scene session is null!"));
        return engine.CreateUndefined();
    }

    NotifyPendingSessionActivationFunc func = [this](SessionInfo& info) {
        this->PendingSessionActivation(info);
    };
    rootSceneSession_->SetPendingSessionActivationEventListener(func);
    std::shared_ptr<NativeReference> callbackRef;
    callbackRef.reset(engine.CreateReference(value, 1));
    jsCbMap_[cbType] = callbackRef;
    WLOGFI("[NAPI]Register end, type = %{public}s", cbType.c_str());
    return engine.CreateUndefined();
}

NativeValue* JsRootSceneSession::OnLoadContent(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGD("[NAPI]OnLoadContent");
    if (info.argc < 2) { // 2: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    std::string contentUrl;
    NativeValue* context = info.argv[1];
    NativeValue* storage = info.argc < 3 ? nullptr : info.argv[2];
    if (!ConvertFromJsValue(engine, info.argv[0], contentUrl)) {
        WLOGFE("[NAPI]Failed to convert parameter to content url");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    auto contextObj = ConvertNativeValueTo<NativeObject>(context);
    if (contextObj == nullptr) {
        WLOGFE("[NAPI]Failed to get context object");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    auto contextNativePointer = static_cast<std::weak_ptr<Context>*>(contextObj->GetNativePointer());
    if (contextNativePointer == nullptr) {
        WLOGFE("[NAPI]Failed to get context pointer from js object");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    auto contextWeakPtr = *contextNativePointer;
    SceneSessionManager::GetInstance().SetRootSceneContext(contextWeakPtr.lock().get());

    std::shared_ptr<NativeReference> contentStorage =
        (storage == nullptr) ? nullptr : std::shared_ptr<NativeReference>(engine.CreateReference(storage, 1));
    AsyncTask::CompleteCallback complete = [rootSceneSession = rootSceneSession_,
        contentUrl, contextWeakPtr, contentStorage](NativeEngine& engine, AsyncTask& task, int32_t status) {
        if (rootSceneSession == nullptr) {
            WLOGFE("[NAPI]rootSceneSession is nullptr");
            task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY)));
            return;
        }
        NativeValue* nativeStorage = contentStorage ? contentStorage->Get() : nullptr;
        rootSceneSession->LoadContent(contentUrl, &engine, nativeStorage, contextWeakPtr.lock().get());
    };
    NativeValue* lastParam = nullptr;
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsRootSceneSession::OnLoadContent", engine,
        CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

bool JsRootSceneSession::IsCallbackRegistered(std::string type, NativeValue* jsListenerObject)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGFI("[NAPI]Method %{public}s has not been registered", type.c_str());
        return false;
    }

    for (auto iter = jsCbMap_.begin(); iter != jsCbMap_.end(); ++iter) {
        if (jsListenerObject->StrictEquals(iter->second->Get())) {
            WLOGFE("[NAPI]Method %{public}s has already been registered", type.c_str());
            return true;
        }
    }
    return false;
}

void JsRootSceneSession::PendingSessionActivation(SessionInfo& info)
{
    WLOGI("[NAPI]pending session activation: bundleName %{public}s, moduleName %{public}s, abilityName %{public}s",
        info.bundleName_.c_str(), info.moduleName_.c_str(), info.abilityName_.c_str());
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
                sceneSession->GetSessionInfo().persistentId_ = realCallerSessionId;
            }
            info.callerPersistentId_ = realCallerSessionId;
        }
    }
    auto iter = jsCbMap_.find(PENDING_SCENE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }

    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [info, jsCallBack](NativeEngine& engine, AsyncTask& task, int32_t status) {
            NativeValue* jsSessionInfo = CreateJsSessionInfo(engine, info);
            if (jsSessionInfo == nullptr) {
                WLOGFE("[NAPI]this target session info is nullptr");
                return;
            }
            NativeValue* argv[] = { jsSessionInfo };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::PendingSessionActivation", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

sptr<SceneSession> JsRootSceneSession::GenSceneSession(SessionInfo& info)
{
    sptr<SceneSession> sceneSession;
    if (info.persistentId_ == 0) {
        sceneSession = SceneSessionManager::GetInstance().RequestSceneSession(info);
        if (sceneSession == nullptr) {
            WLOGFE("RequestSceneSession return nullptr");
            return sceneSession;
        }
        info.persistentId_ = sceneSession->GetPersistentId();
        sceneSession->GetSessionInfo().persistentId_ = sceneSession->GetPersistentId();
    } else {
        sceneSession = SceneSessionManager::GetInstance().GetSceneSession(info.persistentId_);
        if (sceneSession == nullptr) {
            sceneSession = SceneSessionManager::GetInstance().RequestSceneSession(info);
            if (sceneSession == nullptr) {
                WLOGFE("GetSceneSession RequestSceneSession return nullptr");
                return sceneSession;
            }
            WLOGFI("GetSceneSession return RequestSceneSession");
            return sceneSession;
        }
        sceneSession->GetSessionInfo().want = info.want;
        sceneSession->GetSessionInfo().callerToken_ = info.callerToken_;
        sceneSession->GetSessionInfo().requestCode = info.requestCode;
        sceneSession->GetSessionInfo().callerPersistentId_ = info.callerPersistentId_;
    }
    return sceneSession;
}
} // namespace OHOS::Rosen
