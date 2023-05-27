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

#include "session/host/include/scene_session.h"
#include "session_manager/include/scene_session_manager.h"
#include "window_manager_hilog.h"

#include "js_scene_utils.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsSceneSession" };
const std::string PENDING_SCENE_CB = "pendingSceneSessionActivation";
const std::string SESSION_STATE_CHANGE_CB = "sessionStateChange";
const std::string SESSION_EVENT_CB = "sessionEvent";
} // namespace

NativeValue* JsSceneSession::Create(NativeEngine& engine, const sptr<SceneSession>& session)
{
    WLOGI("[NAPI]Create");
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr || session == nullptr) {
        WLOGFE("[NAPI]Object or session is null!");
        return engine.CreateUndefined();
    }

    std::unique_ptr<JsSceneSession> jsSceneSession = std::make_unique<JsSceneSession>(engine, session);
    object->SetNativePointer(jsSceneSession.release(), JsSceneSession::Finalizer, nullptr);
    object->SetProperty("persistentId", CreateJsValue(engine, static_cast<int64_t>(session->GetPersistentId())));

    const char* moduleName = "JsSceneSession";
    BindNativeFunction(engine, *object, "on", moduleName, JsSceneSession::RegisterCallback);

    return objValue;
}

JsSceneSession::JsSceneSession(NativeEngine& engine, const sptr<SceneSession>& session)
    : engine_(engine), session_(session)
{
    listenerFunc_ = {
        { PENDING_SCENE_CB,               &JsSceneSession::ProcessPendingSceneSessionActivationRegister },
        { SESSION_STATE_CHANGE_CB,        &JsSceneSession::ProcessSessionStateChangeRegister },
        { SESSION_EVENT_CB,               &JsSceneSession::ProcessSessionEventRegister },
    };
}

JsSceneSession::~JsSceneSession()
{
    WLOGD("~JsSceneSession");
}

void JsSceneSession::ProcessPendingSceneSessionActivationRegister()
{
    NotifyPendingSessionActivationFunc func = [this](const SessionInfo& info) {
        this->PendingSessionActivation(info);
    };
    session_->SetPendingSessionActivationEventListener(func);
}

void JsSceneSession::ProcessSessionStateChangeRegister()
{
    NotifySessionStateChangeFunc func = [this](const SessionState& state) {
        this->OnSessionStateChange(state);
    };
    session_->SetSessionStateChangeListenser(func);
}

void JsSceneSession::ProcessSessionEventRegister()
{
    NotifySessionEventFunc func = [this](int32_t eventId) {
        this->OnSessionEvent(eventId);
    };
    session_->SetSessionEventListener(func);
}

void JsSceneSession::OnSessionEvent(uint32_t eventId)
{
    WLOGFI("[NAPI]OnSessionEvent, eventId: %{public}d", eventId);
    auto iter = jsCbMap_.find(SESSION_EVENT_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [eventId, jsCallBack, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            NativeValue* jsSessionStateObj = CreateJsValue(engine, eventId);
            NativeValue* argv[] = { jsSessionStateObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnSessionEvent", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGI("[NAPI]Finalizer");
    std::unique_ptr<JsSceneSession>(static_cast<JsSceneSession*>(data));
}

NativeValue* JsSceneSession::RegisterCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGI("[NAPI]RegisterCallback");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(engine, info);
    return (me != nullptr) ? me->OnRegisterCallback(*engine, *info) : nullptr;
}

bool JsSceneSession::IsCallbackRegistered(const std::string& type, NativeValue* jsListenerObject)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
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
    if (value == nullptr || !value->IsCallable()) {
        WLOGFE("[NAPI]Invalid argument");
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

    (this->*listenerFunc_[cbType])();
    std::shared_ptr<NativeReference> callbackRef;
    callbackRef.reset(engine.CreateReference(value, 1));
    jsCbMap_[cbType] = callbackRef;
    WLOGFI("[NAPI]Register end, type = %{public}s", cbType.c_str());
    return engine.CreateUndefined();
}

void JsSceneSession::OnSessionStateChange(const SessionState& state)
{
    WLOGFI("[NAPI]OnSessionStateChange, state: %{public}u", static_cast<uint32_t>(state));
    auto iter = jsCbMap_.find(SESSION_STATE_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [state, jsCallBack, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            NativeValue* jsSessionStateObj = CreateJsValue(engine, state);
            NativeValue* argv[] = { jsSessionStateObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnSessionStateChange", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::PendingSessionActivation(const SessionInfo& info)
{
    WLOGFI("[NAPI]pending session activation: bundleName = %{public}s, id = %{public}s", info.bundleName_.c_str(),
        info.abilityName_.c_str());
    auto iter = jsCbMap_.find(PENDING_SCENE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto sessionWptr = weak_from_this();
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [sessionWptr, info, jsCallBack](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto jsSessionWptr = sessionWptr.lock();
            if (jsSessionWptr == nullptr) {
                WLOGFE("[NAPI]root session or target session or engine is nullptr");
                return;
            }
            NativeValue* jsSessionInfo = CreateJsSessionInfo(engine, info);
            if (jsSessionInfo == nullptr) {
                WLOGFE("[NAPI]this target session info is nullptr");
            }
            NativeValue* argv[] = { jsSessionInfo };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::PendingSessionActivation", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

sptr<SceneSession> JsSceneSession::GetNativeSession() const
{
    return session_;
}
} // namespace OHOS::Rosen
