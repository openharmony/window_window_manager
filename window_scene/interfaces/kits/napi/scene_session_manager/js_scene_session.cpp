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

#include "js_scene_session.h"

#include <memory>

#include "js_scene_utils.h"
#include "session/host/include/scene_session.h"
#include "session_manager/include/scene_session_manager.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsSceneSession" };
const std::string PENDING_SCENE_CB = "pendingSceneSessionActivation";
} // namespace

NativeValue* CreateJsSceneSessionObject(NativeEngine& engine, const sptr<SceneSession>& session)
{
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);

    WLOGFI("[NAPI]CreateJsSceneSession");
    std::unique_ptr<JsSceneSession> jsSceneSession = std::make_unique<JsSceneSession>(engine, session);
    object->SetNativePointer(jsSceneSession.release(), JsSceneSession::Finalizer, nullptr);
    object->SetProperty("persistentId", engine.CreateBigInt(session->GetPersistentId()));
    BindFunctions(engine, object, "JsSceneSession");
    return objValue;
}

void JsSceneSession::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFI("[NAPI]Finalizer");
    std::unique_ptr<JsSceneSession>(static_cast<JsSceneSession*>(data));
}

void BindFunctions(NativeEngine& engine, NativeObject* object, const char* moduleName)
{
    BindNativeFunction(engine, *object, "on", moduleName, JsSceneSession::RegisterCallback);
}

NativeValue* JsSceneSession::RegisterCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]RegisterCallback");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(engine, info);
    return (me != nullptr) ? me->OnRegisterCallback(*engine, *info) : nullptr;
}

NativeValue* JsSceneSession::OnRegisterCallback(NativeEngine& engine, NativeCallbackInfo& info)
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
    if (!value->IsCallable()) {
        WLOGFE("[NAPI]Callback(info->argv[1]) is not callable");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    if (IsCallbackRegistered(cbType, value)) {
        return engine.CreateUndefined();
    }
    if (session_ == nullptr) {
        WLOGFE("[NAPI]session_ is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }

    std::weak_ptr<JsSceneSession> sessionWptr(shared_from_this());
    NotifyPendingSessionActivationFunc func = [sessionWptr](const SessionInfo& info) {
        auto jsSceneSession = sessionWptr.lock();
        if (jsSceneSession == nullptr) {
            WLOGFE("[NAPI]this scene session");
            return;
        }
        jsSceneSession->PendingSessionActivation(info);
    };
    session_->SetPendingSessionActivationEventListener(func);
    std::shared_ptr<NativeReference> callbackRef;
    callbackRef.reset(engine.CreateReference(value, 1));
    jsCbMap_[cbType] = callbackRef;
    WLOGFI("[NAPI]Register end, type = %{public}s, callback = %{public}p", cbType.c_str(), value);
    return engine.CreateUndefined();
}

void JsSceneSession::PendingSessionActivation(const SessionInfo& info)
{
    WLOGFI("[NAPI]pending session activation: bundleName = %{public}s, id = %{public}s", info.bundleName_.c_str(),
        info.abilityName_.c_str());
    auto iter = jsCbMap_.find(PENDING_SCENE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    std::weak_ptr<JsSceneSession> sessionWptr(shared_from_this());
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [sessionWptr, info, jsCallBack](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto jsSessionWptr = sessionWptr.lock();
            if (jsSessionWptr == nullptr) {
                WLOGFE("[NAPI]root session or target session or engine is nullptr");
                return;
            }
            NativeValue* jsSceneInfo = CreateJsSceneInfo(engine, info);
            if (jsSceneInfo == nullptr) {
                WLOGFE("[NAPI]this target session info is nullptr");
            }
            NativeValue* argv[] = { jsSceneInfo };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::PendingSessionActivation", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

bool JsSceneSession::IsCallbackRegistered(std::string type, NativeValue* jsListenerObject)
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

sptr<SceneSession> JsSceneSession::GetNativeSession() const
{
    return session_;
}
} // namespace OHOS::Rosen