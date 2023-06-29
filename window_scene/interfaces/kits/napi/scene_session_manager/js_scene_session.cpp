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

#include "js_scene_utils.h"
#include "js_scene_session.h"

#include "session/host/include/session.h"
#include "session_manager/include/scene_session_manager.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsSceneSession" };
const std::string PENDING_SCENE_CB = "pendingSceneSessionActivation";
const std::string SESSION_STATE_CHANGE_CB = "sessionStateChange";
const std::string SESSION_EVENT_CB = "sessionEvent";
const std::string SESSION_RECT_CHANGE_CB = "sessionRectChange";
const std::string CREATE_SPECIFIC_SCENE_CB = "createSpecificSession";
const std::string RAISE_TO_TOP_CB = "raiseToTop";
const std::string BACK_PRESSED_CB = "backPressed";
const std::string SESSION_FOCUSABLE_CHANGE_CB = "sessionFocusableChange";
const std::string SESSION_TOUCHABLE_CHANGE_CB = "sessionTouchableChange";
const std::string CLICK_CB = "click";
const std::string TERMINATE_SESSION_CB = "terminateSession";
const std::string SESSION_EXCEPTION_CB = "sessionException";
const std::string SYSTEMBAR_PROPERTY_CHANGE_CB = "systemBarPropertyChange";
const std::string NEED_AVOID_CB = "needAvoid";
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
    object->SetProperty("parentId", CreateJsValue(engine, static_cast<int64_t>(session->GetParentPersistentId())));
    object->SetProperty("type", CreateJsValue(engine, static_cast<uint32_t>(GetApiType(session->GetWindowType()))));
    const char* moduleName = "JsSceneSession";
    BindNativeFunction(engine, *object, "on", moduleName, JsSceneSession::RegisterCallback);

    return objValue;
}

JsSessionType JsSceneSession::GetApiType(WindowType type)
{
    auto iter = WINDOW_TO_JS_SESSION_TYPE_MAP.find(type);
    if (iter == WINDOW_TO_JS_SESSION_TYPE_MAP.end()) {
        WLOGFE("[NAPI]window type cannot map to api type!");
        return JsSessionType::TYPE_UNDEFINED;
    } else {
        return iter->second;
    }
}

JsSceneSession::JsSceneSession(NativeEngine& engine, const sptr<SceneSession>& session)
    : engine_(engine), weakSession_(session)
{
    listenerFunc_ = {
        { PENDING_SCENE_CB,               &JsSceneSession::ProcessPendingSceneSessionActivationRegister },
        { SESSION_STATE_CHANGE_CB,        &JsSceneSession::ProcessSessionStateChangeRegister },
        { SESSION_EVENT_CB,               &JsSceneSession::ProcessSessionEventRegister },
        { SESSION_RECT_CHANGE_CB,         &JsSceneSession::ProcessSessionRectChangeRegister },
        { CREATE_SPECIFIC_SCENE_CB,       &JsSceneSession::ProcessCreateSpecificSessionRegister },
        { RAISE_TO_TOP_CB,                &JsSceneSession::ProcessRaiseToTopRegister },
        { BACK_PRESSED_CB,                &JsSceneSession::ProcessBackPressedRegister },
        { SESSION_FOCUSABLE_CHANGE_CB,    &JsSceneSession::ProcessSessionFocusableChangeRegister },
        { SESSION_TOUCHABLE_CHANGE_CB,    &JsSceneSession::ProcessSessionTouchableChangeRegister },
        { CLICK_CB,                       &JsSceneSession::ProcessClickRegister },
        { TERMINATE_SESSION_CB,           &JsSceneSession::ProcessTerminateSessionRegister },
        { SESSION_EXCEPTION_CB,           &JsSceneSession::ProcessSessionExceptionRegister },
        { SYSTEMBAR_PROPERTY_CHANGE_CB,   &JsSceneSession::ProcessSystemBarPropertyChangeRegister },
        { NEED_AVOID_CB,          &JsSceneSession::ProcessNeedAvoidRegister },
    };

    sptr<SceneSession::SessionChangeCallback> sessionchangeCallback = new (std::nothrow)
        SceneSession::SessionChangeCallback();
    if (sessionchangeCallback != nullptr) {
        if (session != nullptr) {
            session->RegisterSessionChangeCallback(sessionchangeCallback);
        }
        sessionchangeCallback_ = sessionchangeCallback;
        WLOGFD("RegisterSessionChangeCallback success");
    }
}

JsSceneSession::~JsSceneSession()
{
    WLOGD("~JsSceneSession");
}

void JsSceneSession::ProcessPendingSceneSessionActivationRegister()
{
    NotifyPendingSessionActivationFunc func = [this](SessionInfo& info) {
        this->PendingSessionActivation(info);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetPendingSessionActivationEventListener(func);
    WLOGFD("ProcessPendingSceneSessionActivationRegister success");
}

void JsSceneSession::ProcessSessionStateChangeRegister()
{
    NotifySessionStateChangeFunc func = [this](const SessionState& state) {
        this->OnSessionStateChange(state);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetSessionStateChangeListenser(func);
    WLOGFD("ProcessSessionStateChangeRegister success");
}

void JsSceneSession::ProcessCreateSpecificSessionRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onCreateSpecificSession_ = std::bind(&JsSceneSession::OnCreateSpecificSession,
        this, std::placeholders::_1);
    WLOGFD("ProcessCreateSpecificSessionRegister success");
}

void JsSceneSession::ProcessSessionRectChangeRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onRectChange_ = std::bind(&JsSceneSession::OnSessionRectChange, this, std::placeholders::_1);
    WLOGFD("ProcessSessionRectChangeRegister success");
}

void JsSceneSession::ProcessRaiseToTopRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onRaiseToTop_ = std::bind(&JsSceneSession::OnRaiseToTop, this);
    WLOGFD("ProcessRaiseToTopRegister success");
}

void JsSceneSession::ProcessSessionEventRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->OnSessionEvent_ = std::bind(&JsSceneSession::OnSessionEvent, this, std::placeholders::_1);
    WLOGFD("ProcessSessionEventRegister success");
}

void JsSceneSession::ProcessTerminateSessionRegister()
{
    WLOGFD("begin to run ProcessTerminateSessionRegister");
    NotifyTerminateSessionFunc func = [this](const SessionInfo& info) {
        this->TerminateSession(info);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetTerminateSessionListener(func);
    WLOGFD("ProcessTerminateSessionRegister success");
}

void JsSceneSession::ProcessSessionFocusableChangeRegister()
{
    NotifySessionFocusableChangeFunc func = [this](bool isFocusable) {
        this->OnSessionFocusableChange(isFocusable);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetSessionFocusableChangeListener(func);
    WLOGFD("ProcessSessionFocusableChangeRegister success");
}

void JsSceneSession::ProcessSessionTouchableChangeRegister()
{
    NotifySessionTouchableChangeFunc func = [this](bool touchable) {
        this->OnSessionTouchableChange(touchable);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetSessionTouchableChangeListener(func);
    WLOGFD("ProcessSessionTouchableChangeRegister success");
}

void JsSceneSession::ProcessSessionExceptionRegister()
{
    WLOGFD("begin to run ProcessSessionExceptionRegister");
    NotifySessionExceptionFunc func = [this](const SessionInfo& info) {
        this->OnSessionException(info);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetSessionExceptionListener(func);
    WLOGFD("ProcessSessionExceptionRegister success");
}

void JsSceneSession::ProcessClickRegister()
{
    NotifyClickFunc func = [this]() {
        this->OnClick();
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetClickListener(func);
    WLOGFD("ProcessClickChangeRegister success");
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

void JsSceneSession::ProcessBackPressedRegister()
{
    NotifyBackPressedFunc func = [this]() {
        this->OnBackPressed();
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetBackPressedListenser(func);
}

void JsSceneSession::ProcessSystemBarPropertyChangeRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->OnSystemBarPropertyChange_ = std::bind(
        &JsSceneSession::OnSystemBarPropertyChange, this, std::placeholders::_1);
    WLOGFD("ProcessSystemBarPropertyChangeRegister success");
}

void JsSceneSession::ProcessNeedAvoidRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->OnNeedAvoid_ = std::bind(
        &JsSceneSession::OnNeedAvoid, this, std::placeholders::_1);
    WLOGFD("ProcessNeedAvoidRegister success");
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
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }

    std::shared_ptr<NativeReference> callbackRef;
    callbackRef.reset(engine.CreateReference(value, 1));
    jsCbMap_[cbType] = callbackRef;
    (this->*listenerFunc_[cbType])();
    WLOGFI("[NAPI]Register end, type = %{public}s", cbType.c_str());
    return engine.CreateUndefined();
}

void JsSceneSession::OnCreateSpecificSession(const sptr<SceneSession>& sceneSession)
{
    WLOGFI("OnCreateSpecificSession");
    if (sceneSession == nullptr) {
        WLOGFI("[NAPI]sceneSession is nullptr");
        return;
    }

    WLOGFI("[NAPI]OnCreateSpecificSession");
    auto iter = jsCbMap_.find(CREATE_SPECIFIC_SCENE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }

    auto jsCallBack = iter->second;
    wptr<SceneSession> weakSession(sceneSession);
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [this, weakSession, jsCallBack, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto specificSession = weakSession.promote();
            if (specificSession == nullptr) {
                WLOGFE("[NAPI]root session or target session or engine is nullptr");
                return;
            }
            NativeValue* jsSceneSessionObj = Create(*eng, specificSession);
            if (jsSceneSessionObj == nullptr || !jsCallBack) {
                WLOGFE("[NAPI]jsSceneSessionObj or jsCallBack is nullptr");
                return;
            }
            WLOGFI("CreateJsSceneSessionObject success");
            NativeValue* argv[] = { jsSceneSessionObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnCreateSpecificSession", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
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
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            NativeValue* jsSessionStateObj = CreateJsValue(engine, state);
            NativeValue* argv[] = { jsSessionStateObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnSessionStateChange", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnSessionRectChange(const WSRect& rect)
{
    WLOGFI("[NAPI]OnSessionRectChange");
    auto iter = jsCbMap_.find(SESSION_RECT_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [rect, jsCallBack, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            NativeValue* jsSessionStateObj = CreateJsSessionRect(engine, rect);
            NativeValue* argv[] = { jsSessionStateObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnSessionRectChange", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnRaiseToTop()
{
    WLOGFI("[NAPI]OnRaiseToTop");
    auto iter = jsCbMap_.find(RAISE_TO_TOP_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [jsCallBack, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            NativeValue* argv[] = {};
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, 0);
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnRaiseToTop", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnSessionFocusableChange(bool isFocusable)
{
    WLOGFI("[NAPI]OnSessionFocusableChange, state: %{public}u", isFocusable);
    auto iter = jsCbMap_.find(SESSION_FOCUSABLE_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [isFocusable, jsCallBack, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            NativeValue* jsSessionFocusableObj = CreateJsValue(engine, isFocusable);
            NativeValue* argv[] = { jsSessionFocusableObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnSessionFocusableChange", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnSessionTouchableChange(bool touchable)
{
    WLOGFI("[NAPI]OnSessionTouchableChange, state: %{public}u", touchable);
    auto iter = jsCbMap_.find(SESSION_TOUCHABLE_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [touchable, jsCallBack, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            NativeValue* jsSessionTouchableObj = CreateJsValue(engine, touchable);
            NativeValue* argv[] = { jsSessionTouchableObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnSessionTouchableChange", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnClick()
{
    WLOGFI("[NAPI]OnClick");
    auto iter = jsCbMap_.find(CLICK_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [jsCallBack, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            NativeValue* argv[] = { };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, 0);
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnClick", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::PendingSessionActivation(SessionInfo& info)
{
    WLOGI("[NAPI]pending session activation: bundleName %{public}s, moduleName %{public}s, abilityName %{public}s",
        info.bundleName_.c_str(), info.moduleName_.c_str(), info.abilityName_.c_str());
    if (info.persistentId_ == 0) {
        auto sceneSession = SceneSessionManager::GetInstance().RequestSceneSession(info);
        if (sceneSession == nullptr) {
            WLOGFE("RequestSceneSession return nullptr");
            return;
        }
        info.persistentId_ = sceneSession->GetPersistentId();
    } else {
        auto sceneSession = SceneSessionManager::GetInstance().GetSceneSession(info.persistentId_);
        if (sceneSession == nullptr) {
            WLOGFE("GetSceneSession return nullptr");
            return;
        }
        sceneSession->GetSessionInfo().want = info.want;
        sceneSession->GetSessionInfo().callerToken_ = info.callerToken_;
        sceneSession->GetSessionInfo().requestCode = info.requestCode;
    }
    auto iter = jsCbMap_.find(PENDING_SCENE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [info, jsCallBack](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
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

void JsSceneSession::OnBackPressed()
{
    WLOGFI("[NAPI]OnBackPressed");
    auto iter = jsCbMap_.find(BACK_PRESSED_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [jsCallBack, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), {}, 0);
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnBackPressed", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::TerminateSession(const SessionInfo& info)
{
    WLOGFI("[NAPI]run TerminateSession, bundleName = %{public}s, id = %{public}s",
        info.bundleName_.c_str(), info.abilityName_.c_str());
    auto iter = jsCbMap_.find(TERMINATE_SESSION_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [info, jsCallBack](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
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
    AsyncTask::Schedule("JsSceneSession::TerminateSession", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnSessionException(const SessionInfo& info)
{
    WLOGFI("[NAPI]run OnSessionException, bundleName = %{public}s, id = %{public}s",
        info.bundleName_.c_str(), info.abilityName_.c_str());
    auto iter = jsCbMap_.find(SESSION_EXCEPTION_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [info, jsCallBack](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
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
    AsyncTask::Schedule("JsSceneSession::TerminateSession", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnSystemBarPropertyChange(const std::unordered_map<WindowType, SystemBarProperty>& propertyMap)
{
    WLOGFI("[NAPI]OnSystemBarPropertyChange");
    auto iter = jsCbMap_.find(SYSTEMBAR_PROPERTY_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [jsCallBack, propertyMap, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            NativeValue* jsSessionStateObj = CreateJsSystemBarPropertyArrayObject(engine, propertyMap);
            NativeValue* argv[] = { jsSessionStateObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnSystemBarPropertyChange", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnNeedAvoid(bool status)
{
    WLOGFI("[NAPI]OnNeedAvoid %{public}d", status);
    auto iter = jsCbMap_.find(NEED_AVOID_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [jsCallBack, needAvoid = status, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            NativeValue* jsSessionStateObj = CreateJsValue(engine, needAvoid);
            NativeValue* argv[] = { jsSessionStateObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::OnNeedAvoid", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

sptr<SceneSession> JsSceneSession::GetNativeSession() const
{
    return weakSession_.promote();
}
} // namespace OHOS::Rosen
