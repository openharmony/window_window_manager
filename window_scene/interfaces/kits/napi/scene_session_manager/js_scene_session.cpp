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
const std::string BUFFER_AVAILABLE_CHANGE_CB = "bufferAvailableChange";
const std::string SESSION_EVENT_CB = "sessionEvent";
const std::string SESSION_RECT_CHANGE_CB = "sessionRectChange";
const std::string CREATE_SUB_SESSION_CB = "createSpecificSession";
const std::string BIND_DIALOG_TARGET_CB = "bindDialogTarget";
const std::string RAISE_TO_TOP_CB = "raiseToTop";
const std::string RAISE_TO_TOP_POINT_DOWN_CB = "raiseToTopForPointDown";
const std::string BACK_PRESSED_CB = "backPressed";
const std::string SESSION_FOCUSABLE_CHANGE_CB = "sessionFocusableChange";
const std::string SESSION_TOUCHABLE_CHANGE_CB = "sessionTouchableChange";
const std::string CLICK_CB = "click";
const std::string TERMINATE_SESSION_CB = "terminateSession";
const std::string TERMINATE_SESSION_CB_NEW = "terminateSessionNew";
const std::string TERMINATE_SESSION_CB_TOTAL = "terminateSessionTotal";
const std::string UPDATE_SESSION_LABEL_CB = "updateSessionLabel";
const std::string UPDATE_SESSION_ICON_CB = "updateSessionIcon";
const std::string SESSION_EXCEPTION_CB = "sessionException";
const std::string SYSTEMBAR_PROPERTY_CHANGE_CB = "systemBarPropertyChange";
const std::string NEED_AVOID_CB = "needAvoid";
const std::string PENDING_SESSION_TO_FOREGROUND_CB = "pendingSessionToForeground";
const std::string PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR_CB = "pendingSessionToBackgroundForDelegator";
const std::string NEED_DEFAULT_ANIMATION_FLAG_CHANGE_CB = "needDefaultAnimationFlagChange";
const std::string CUSTOM_ANIMATION_PLAYING_CB = "isCustomAnimationPlaying";
const std::string SHOW_WHEN_LOCKED_CB = "sessionShowWhenLockedChange";
const std::string REQUESTED_ORIENTATION_CHANGE_CB = "sessionRequestedOrientationChange";
const std::string RAISE_ABOVE_TARGET_CB = "raiseAboveTarget";
const std::string FORCE_HIDE_CHANGE_CB = "sessionForceHideChange";
const std::string TOUCH_OUTSIDE_CB = "touchOutside";
const std::string WINDOW_DRAG_HOT_AREA_CB = "windowDragHotArea";
const std::string SESSIONINFO_LOCKEDSTATE_CHANGE_CB = "sessionInfoLockedStateChange";
const std::string PREPARE_CLOSE_PIP_SESSION = "prepareClosePiPSession";
constexpr int SCALE_ARG_COUNT = 4;
constexpr int ARG_INDEX_0 = 0;
constexpr int ARG_INDEX_1 = 1;
constexpr int ARG_INDEX_2 = 2;
constexpr int ARG_INDEX_3 = 3;
} // namespace

std::map<int32_t, napi_ref> JsSceneSession::jsSceneSessionMap_;

napi_value JsSceneSession::Create(napi_env env, const sptr<SceneSession>& session)
{
    WLOGD("[NAPI]Create");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr || session == nullptr) {
        WLOGFE("[NAPI]Object or session is null!");
        return NapiGetUndefined(env);
    }

    std::unique_ptr<JsSceneSession> jsSceneSession = std::make_unique<JsSceneSession>(env, session);
    napi_wrap(env, objValue, jsSceneSession.release(), JsSceneSession::Finalizer, nullptr, nullptr);
    napi_set_named_property(env, objValue, "persistentId",
        CreateJsValue(env, static_cast<int32_t>(session->GetPersistentId())));
    napi_set_named_property(env, objValue, "parentId",
        CreateJsValue(env, static_cast<int32_t>(session->GetParentPersistentId())));
    napi_set_named_property(env, objValue, "type",
        CreateJsValue(env, static_cast<uint32_t>(GetApiType(session->GetWindowType()))));
    napi_set_named_property(env, objValue, "isAppType", CreateJsValue(env, session->IsFloatingWindowAppType()));

    const char* moduleName = "JsSceneSession";
    BindNativeFunction(env, objValue, "on", moduleName, JsSceneSession::RegisterCallback);
    BindNativeFunction(env, objValue, "updateNativeVisibility", moduleName, JsSceneSession::UpdateNativeVisibility);
    BindNativeFunction(env, objValue, "setShowRecent", moduleName, JsSceneSession::SetShowRecent);
    BindNativeFunction(env, objValue, "setZOrder", moduleName, JsSceneSession::SetZOrder);
    BindNativeFunction(env, objValue, "setTouchable", moduleName, JsSceneSession::SetTouchable);
    BindNativeFunction(env, objValue, "setSystemActive", moduleName, JsSceneSession::SetSystemActive);
    BindNativeFunction(env, objValue, "setPrivacyMode", moduleName, JsSceneSession::SetPrivacyMode);
    BindNativeFunction(env, objValue, "setFloatingScale", moduleName, JsSceneSession::SetFloatingScale);
    BindNativeFunction(env, objValue, "setSystemSceneOcclusionAlpha", moduleName,
        JsSceneSession::SetSystemSceneOcclusionAlpha);
    BindNativeFunction(env, objValue, "setFocusable", moduleName, JsSceneSession::SetFocusable);
    BindNativeFunction(env, objValue, "setSystemSceneBlockingFocus", moduleName,
        JsSceneSession::SetSystemSceneBlockingFocus);
    BindNativeFunction(env, objValue, "setSCBKeepKeyboard", moduleName, JsSceneSession::SetSCBKeepKeyboard);
    BindNativeFunction(env, objValue, "setOffset", moduleName, JsSceneSession::SetOffset);
    BindNativeFunction(env, objValue, "setScale", moduleName, JsSceneSession::SetScale);
    BindNativeFunction(env, objValue, "requestHideKeyboard", moduleName, JsSceneSession::RequestHideKeyboard);
    napi_ref jsRef = nullptr;
    napi_status status = napi_create_reference(env, objValue, 1, &jsRef);
    if (status != napi_ok) {
        WLOGFE("do not get ref ");
    }
    jsSceneSessionMap_[session->GetPersistentId()] = jsRef;
    BindNativeFunction(env, objValue, "updateSizeChangeReason", moduleName, JsSceneSession::UpdateSizeChangeReason);
    return objValue;
}

JsSceneSession::JsSceneSession(napi_env env, const sptr<SceneSession>& session)
    : env_(env), weakSession_(session)
{
    listenerFunc_ = {
        { PENDING_SCENE_CB,                      &JsSceneSession::ProcessPendingSceneSessionActivationRegister },
        { SESSION_STATE_CHANGE_CB,               &JsSceneSession::ProcessSessionStateChangeRegister },
        { BUFFER_AVAILABLE_CHANGE_CB,            &JsSceneSession::ProcessBufferAvailableChangeRegister},
        { SESSION_EVENT_CB,                      &JsSceneSession::ProcessSessionEventRegister },
        { SESSION_RECT_CHANGE_CB,                &JsSceneSession::ProcessSessionRectChangeRegister },
        { CREATE_SUB_SESSION_CB,                 &JsSceneSession::ProcessCreateSubSessionRegister },
        { BIND_DIALOG_TARGET_CB,                 &JsSceneSession::ProcessBindDialogTargetRegister },
        { RAISE_TO_TOP_CB,                       &JsSceneSession::ProcessRaiseToTopRegister },
        { RAISE_TO_TOP_POINT_DOWN_CB,            &JsSceneSession::ProcessRaiseToTopForPointDownRegister },
        { BACK_PRESSED_CB,                       &JsSceneSession::ProcessBackPressedRegister },
        { SESSION_FOCUSABLE_CHANGE_CB,           &JsSceneSession::ProcessSessionFocusableChangeRegister },
        { SESSION_TOUCHABLE_CHANGE_CB,           &JsSceneSession::ProcessSessionTouchableChangeRegister },
        { CLICK_CB,                              &JsSceneSession::ProcessClickRegister },
        { TERMINATE_SESSION_CB,                  &JsSceneSession::ProcessTerminateSessionRegister },
        { TERMINATE_SESSION_CB_NEW,              &JsSceneSession::ProcessTerminateSessionRegisterNew },
        { TERMINATE_SESSION_CB_TOTAL,            &JsSceneSession::ProcessTerminateSessionRegisterTotal },
        { SESSION_EXCEPTION_CB,                  &JsSceneSession::ProcessSessionExceptionRegister },
        { UPDATE_SESSION_LABEL_CB,               &JsSceneSession::ProcessUpdateSessionLabelRegister },
        { UPDATE_SESSION_ICON_CB,                &JsSceneSession::ProcessUpdateSessionIconRegister },
        { SYSTEMBAR_PROPERTY_CHANGE_CB,          &JsSceneSession::ProcessSystemBarPropertyChangeRegister },
        { NEED_AVOID_CB,                         &JsSceneSession::ProcessNeedAvoidRegister },
        { PENDING_SESSION_TO_FOREGROUND_CB,      &JsSceneSession::ProcessPendingSessionToForegroundRegister },
        { PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR_CB,
            &JsSceneSession::ProcessPendingSessionToBackgroundForDelegatorRegister },
        { NEED_DEFAULT_ANIMATION_FLAG_CHANGE_CB, &JsSceneSession::ProcessSessionDefaultAnimationFlagChangeRegister },
        { CUSTOM_ANIMATION_PLAYING_CB,           &JsSceneSession::ProcessIsCustomAnimationPlaying },
        { SHOW_WHEN_LOCKED_CB,                   &JsSceneSession::ProcessShowWhenLockedRegister },
        { REQUESTED_ORIENTATION_CHANGE_CB,       &JsSceneSession::ProcessRequestedOrientationChange },
        { RAISE_ABOVE_TARGET_CB,                 &JsSceneSession::ProcessRaiseAboveTargetRegister },
        { FORCE_HIDE_CHANGE_CB,                  &JsSceneSession::ProcessForceHideChangeRegister },
        { TOUCH_OUTSIDE_CB,                      &JsSceneSession::ProcessTouchOutsideRegister },
        { WINDOW_DRAG_HOT_AREA_CB,               &JsSceneSession::ProcessWindowDragHotAreaRegister },
        { SESSIONINFO_LOCKEDSTATE_CHANGE_CB,     &JsSceneSession::ProcessSessionInfoLockedStateChangeRegister },
        { PREPARE_CLOSE_PIP_SESSION,             &JsSceneSession::ProcessPrepareClosePiPSessionRegister},
    };

    sptr<SceneSession::SessionChangeCallback> sessionchangeCallback = new (std::nothrow)
        SceneSession::SessionChangeCallback();
    if (sessionchangeCallback != nullptr) {
        if (session != nullptr) {
            session->RegisterSessionChangeCallback(sessionchangeCallback);
        }
        sessionchangeCallback->clearCallbackFunc_ = [weak = weak_from_this()](bool needRemove, int32_t persistentId) {
            auto weakJsSceneSession = weak.lock();
            if (weakJsSceneSession) weakJsSceneSession->ClearCbMap(needRemove, persistentId);
        };
        sessionchangeCallback_ = sessionchangeCallback;
        WLOGFD("RegisterSessionChangeCallback success");
    }
    taskScheduler_ = std::make_shared<MainThreadScheduler>(env);
}

JsSceneSession::~JsSceneSession()
{
    WLOGD("~JsSceneSession");
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFD("session is nullptr");
        return;
    }
    session->UnregisterSessionChangeListeners();
    SceneSessionManager::GetInstance().UnregisterCreateSubSessionListener(session->GetPersistentId());
}

void JsSceneSession::ProcessWindowDragHotAreaRegister()
{

    WLOGFI("[NAPI]ProcessWindowDragHotAreaRegister");
    NotifyWindowDragHotAreaFunc func = [weak = weak_from_this()](int32_t type, const SizeChangeReason& reason) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnWindowDragHotArea(type, reason);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetWindowDragHotAreaListener(func);
}

void JsSceneSession::OnWindowDragHotArea(int32_t type, const SizeChangeReason& reason)
{
    WLOGFI("[NAPI]OnWindowDragHotArea");
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(WINDOW_DRAG_HOT_AREA_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    WSRect rect = session->GetSessionTargetRect();
    auto task = [jsCallBack, env = env_, type, reason, rect]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsHotAreaType = CreateJsValue(env, type);
        if (jsHotAreaType == nullptr) {
            WLOGFE("[NAPI]jsHotAreaType is nullptr");
            return;
        }
        napi_value jsHotAreaReason = CreateJsValue(env, reason);
        if (jsHotAreaReason == nullptr) {
            WLOGFE("[NAPI]jsHotAreaReason is nullptr");
            return;
        }
        napi_value jsHotAreaRect = CreateJsSessionRect(env, rect);
        if (jsHotAreaRect == nullptr) {
            WLOGFE("[NAPI]jsHotAreaRect is nullptr");
            return;
        }
        napi_value argv[] = {[0] = jsHotAreaType, [1] = jsHotAreaReason, [2] = jsHotAreaRect};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnWindowDragHotArea");
}

void JsSceneSession::ProcessSessionInfoLockedStateChangeRegister()
{
    NotifySessionInfoLockedStateChangeFunc func = [weak = weak_from_this()](bool lockedState) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnSessionInfoLockedStateChange(lockedState);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetSessionInfoLockedStateChangeListener(func);
    WLOGFD("ProcessSessionInfoLockedStateChangeRegister success");
}

void JsSceneSession::OnSessionInfoLockedStateChange(bool lockedState)
{
    WLOGFI("[NAPI]OnSessionInfoLockedStateChange, state: %{public}u", lockedState);
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(SESSIONINFO_LOCKEDSTATE_CHANGE_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [lockedState, jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionInfoLockedStateObj = CreateJsValue(env, lockedState);
        napi_value argv[] = {jsSessionInfoLockedStateObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnSessionInfoLockedStateChange: state " + std::to_string(lockedState));
}

void JsSceneSession::ClearCbMap(bool needRemove, int32_t persistentId)
{
    if (!needRemove) {
        return;
    }
    auto task = [this, persistentId]() {
        WLOGFI("[WMSLife] clear callbackMap with persistent id, %{public}d", persistentId);
        {
            std::unique_lock<std::shared_mutex> lock(jsCbMapMutex_);
            jsCbMap_.clear();
        }
        auto iter = jsSceneSessionMap_.find(persistentId);
        if (iter != jsSceneSessionMap_.end()) {
            napi_delete_reference(env_, iter->second);
            jsSceneSessionMap_.erase(iter);
        } else {
            WLOGFE("[WMSLife] deleteRef failed , %{public}d", persistentId);
        }
    };
    taskScheduler_->PostMainThreadTask(task, "ClearCbMap PID:" + std::to_string(persistentId));
}

void JsSceneSession::ProcessSessionDefaultAnimationFlagChangeRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onWindowAnimationFlagChange_ = [weak = weak_from_this()](bool isNeedDefaultAnimationFlag) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnDefaultAnimationFlagChange(isNeedDefaultAnimationFlag);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    sessionchangeCallback->onWindowAnimationFlagChange_(session->IsNeedDefaultAnimation());
    WLOGFD("ProcessSessionDefaultAnimationFlagChangeRegister success");
}

void JsSceneSession::OnDefaultAnimationFlagChange(bool isNeedDefaultAnimationFlag)
{
    WLOGFI("[NAPI]OnDefaultAnimationFlagChange, flag: %{public}u", isNeedDefaultAnimationFlag);
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(NEED_DEFAULT_ANIMATION_FLAG_CHANGE_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [isNeedDefaultAnimationFlag, jsCallBack, env = env_]() {
        napi_value jsSessionDefaultAnimationFlagObj = CreateJsValue(env, isNeedDefaultAnimationFlag);
        napi_value argv[] = {jsSessionDefaultAnimationFlagObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnDefaultAnimationFlagChange, flag:" + std::to_string(isNeedDefaultAnimationFlag));
}

void JsSceneSession::ProcessPendingSceneSessionActivationRegister()
{
    NotifyPendingSessionActivationFunc func = [weak = weak_from_this()](SessionInfo& info) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->PendingSessionActivation(info);
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
    NotifySessionStateChangeFunc func = [weak = weak_from_this()](const SessionState& state) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnSessionStateChange(state);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetSessionStateChangeListenser(func);
    WLOGFD("ProcessSessionStateChangeRegister success");
}

void JsSceneSession::ProcessBufferAvailableChangeRegister()
{
    NotifyBufferAvailableChangeFunc func = [weak = weak_from_this()](const bool isAvailable) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnBufferAvailableChange(isAvailable);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetBufferAvailableChangeListener(func);
    WLOGFD("ProcessBufferAvailableChangeRegister success");
}

void JsSceneSession::ProcessCreateSubSessionRegister()
{
    NotifyCreateSubSessionFunc func = [weak = weak_from_this()](const sptr<SceneSession>& sceneSession) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnCreateSubSession(sceneSession);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    SceneSessionManager::GetInstance().RegisterCreateSubSessionListener(session->GetPersistentId(), func);
    WLOGFD("ProcessCreateSubSessionRegister success, id: %{public}d", session->GetPersistentId());
}

void JsSceneSession::ProcessBindDialogTargetRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("[WMSDialog] sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onBindDialogTarget_ = [weak = weak_from_this()](const sptr<SceneSession>& sceneSession) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnBindDialogTarget(sceneSession);
    };
    WLOGFD("[WMSDialog] ProcessBindDialogTargetRegister success");
}

void JsSceneSession::ProcessSessionRectChangeRegister()
{
    NotifySessionRectChangeFunc func = [weak = weak_from_this()](const WSRect& rect, const SizeChangeReason& reason) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnSessionRectChange(rect, reason);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetSessionRectChangeCallback(func);
    WLOGFD("ProcessSessionRectChangeRegister success");
}

void JsSceneSession::ProcessRaiseToTopRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onRaiseToTop_ = [weak = weak_from_this()]() {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnRaiseToTop(); 
    };
    WLOGFD("ProcessRaiseToTopRegister success");
}

void JsSceneSession::ProcessRaiseToTopForPointDownRegister()
{
    NotifyRaiseToTopForPointDownFunc func = [weak = weak_from_this()]() {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnRaiseToTopForPointDown(); 
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetRaiseToAppTopForPointDownFunc(func);
    WLOGFD("ProcessRaiseToTopForPointDownRegister success");
}

void JsSceneSession::ProcessRaiseAboveTargetRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onRaiseAboveTarget_ = [weak = weak_from_this()](int32_t subWindowId){
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnRaiseAboveTarget(subWindowId);
    };  
    WLOGFD("ProcessRaiseToTopRegister success");
}

void JsSceneSession::ProcessSessionEventRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->OnSessionEvent_ = [weak = weak_from_this()](uint32_t eventId) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnSessionEvent(eventId);
    };
    WLOGFD("ProcessSessionEventRegister success");
}

void JsSceneSession::ProcessTerminateSessionRegister()
{
    WLOGFD("begin to run ProcessTerminateSessionRegister");
    NotifyTerminateSessionFunc func = [weak = weak_from_this()](const SessionInfo& info) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->TerminateSession(info);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetTerminateSessionListener(func);
    WLOGFD("ProcessTerminateSessionRegister success");
}

void JsSceneSession::ProcessTerminateSessionRegisterNew()
{
    WLOGFD("begin to run ProcessTerminateSessionRegisterNew");
    NotifyTerminateSessionFuncNew func = [weak = weak_from_this()](const SessionInfo& info, bool needStartCaller) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->TerminateSessionNew(info, needStartCaller);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetTerminateSessionListenerNew(func);
    WLOGFD("ProcessTerminateSessionRegisterNew success");
}

void JsSceneSession::ProcessTerminateSessionRegisterTotal()
{
    WLOGFD("begin to run ProcessTerminateSessionRegisterTotal");
    NotifyTerminateSessionFuncTotal func = [weak = weak_from_this()](const SessionInfo& info, TerminateType terminateType) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->TerminateSessionTotal(info, terminateType);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetTerminateSessionListenerTotal(func);
    WLOGFD("ProcessTerminateSessionRegisterTotal success");
}

void JsSceneSession::ProcessPendingSessionToForegroundRegister()
{
    WLOGFD("begin to run ProcessPendingSessionToForegroundRegister");
    NotifyPendingSessionToForegroundFunc func = [weak = weak_from_this()](const SessionInfo& info) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->PendingSessionToForeground(info);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetPendingSessionToForegroundListener(func);
    WLOGFD("ProcessPendingSessionToForegroundRegister success");
}

void JsSceneSession::ProcessPendingSessionToBackgroundForDelegatorRegister()
{
    WLOGFD("begin to run ProcessPendingSessionToBackgroundForDelegatorRegister");
    auto weak = weak_from_this();
    NotifyPendingSessionToBackgroundForDelegatorFunc func = [weak = weak_from_this()](const SessionInfo& info) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->PendingSessionToBackgroundForDelegator(info);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetPendingSessionToBackgroundForDelegatorListener(func);
    WLOGFD("ProcessPendingSessionToBackgroundForDelegatorRegister success");
}

void JsSceneSession::ProcessSessionFocusableChangeRegister()
{
    NotifySessionFocusableChangeFunc func = [weak = weak_from_this()](bool isFocusable) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnSessionFocusableChange(isFocusable);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[WMSComm]session is nullptr");
        return;
    }
    session->SetSessionFocusableChangeListener(func);
    WLOGFI("[WMSFocus]ProcessSessionFocusableChangeRegister success");
}

void JsSceneSession::ProcessSessionTouchableChangeRegister()
{
    NotifySessionTouchableChangeFunc func = [weak = weak_from_this()](bool touchable) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnSessionTouchableChange(touchable);
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
    NotifySessionExceptionFunc func = [weak = weak_from_this()](const SessionInfo& info, bool needRemoveSession) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnSessionException(info, needRemoveSession);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetSessionExceptionListener(func, true);
    WLOGFD("ProcessSessionExceptionRegister success");
}

void JsSceneSession::ProcessClickRegister()
{
    NotifyClickFunc func = [weak = weak_from_this()]() {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnClick();
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
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(SESSION_EVENT_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [eventId, jsCallBack, env = env_]() {
        napi_value jsSessionStateObj = CreateJsValue(env, eventId);
        napi_value argv[] = {jsSessionStateObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    taskScheduler_->PostMainThreadTask(task, "OnSessionEvent, EventId:" + std::to_string(eventId));
}

void JsSceneSession::ProcessBackPressedRegister()
{
    NotifyBackPressedFunc func = [weak = weak_from_this()](bool needMoveToBackground) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnBackPressed(needMoveToBackground);
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
    sessionchangeCallback->OnSystemBarPropertyChange_ = [weak = weak_from_this()]
        (const std::unordered_map<WindowType, SystemBarProperty>& propertyMap) {
            auto weakJsSceneSession = weak.lock();
            if (weakJsSceneSession) weakJsSceneSession->OnSystemBarPropertyChange(propertyMap);
    };
    WLOGFD("ProcessSystemBarPropertyChangeRegister success");
}

void JsSceneSession::ProcessNeedAvoidRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->OnNeedAvoid_ = [weak = weak_from_this()](bool status) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnNeedAvoid(status);
    };
    WLOGFD("ProcessNeedAvoidRegister success");
}

void JsSceneSession::ProcessIsCustomAnimationPlaying()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onIsCustomAnimationPlaying_ = [weak = weak_from_this()](bool status) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnIsCustomAnimationPlaying(status);
    };
    WLOGFD("ProcessIsCustomAnimationPlaying success");
}

void JsSceneSession::ProcessShowWhenLockedRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->OnShowWhenLocked_ = [weak = weak_from_this()](bool showWhenLocked) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnShowWhenLocked(showWhenLocked);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    sessionchangeCallback->OnShowWhenLocked_(session->IsShowWhenLocked());
    WLOGFD("ProcessShowWhenLockedRegister success");
}

void JsSceneSession::ProcessRequestedOrientationChange()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->OnRequestedOrientationChange_ = [weak = weak_from_this()](uint32_t orientation) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnReuqestedOrientationChange(orientation);
    };
    WLOGFD("ProcessRequestedOrientationChange success");
}

void JsSceneSession::ProcessForceHideChangeRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->OnForceHideChange_ = [weak = weak_from_this()](bool hide) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnForceHideChange(hide);
    };
    WLOGFD("ProcessForceHideChangeRegister success");
}

void JsSceneSession::OnForceHideChange(bool hide)
{
    WLOGFI("[NAPI]OnForceHideChange, hide: %{public}u", hide);
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(FORCE_HIDE_CHANGE_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [hide, jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionForceHideObj = CreateJsValue(env, hide);
        napi_value argv[] = {jsSessionForceHideObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnForceHideChange, hide:" + std::to_string(hide));
}

void JsSceneSession::ProcessTouchOutsideRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->OnTouchOutside_ = [weak = weak_from_this()]() {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnTouchOutside();
    };
    WLOGFD("ProcessTouchOutsideRegister success");
}

void JsSceneSession::OnTouchOutside()
{
    WLOGFI("[NAPI]OnTouchOutside");
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(TOUCH_OUTSIDE_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value argv[] = {};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), 0, argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task);
}

void JsSceneSession::Finalizer(napi_env env, void* data, void* hint)
{
    WLOGI("[NAPI]Finalizer");
    std::unique_ptr<JsSceneSession>(static_cast<JsSceneSession*>(data));
}

napi_value JsSceneSession::RegisterCallback(napi_env env, napi_callback_info info)
{
    WLOGD("[NAPI]RegisterCallback");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnRegisterCallback(env, info) : nullptr;
}

napi_value JsSceneSession::UpdateNativeVisibility(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]UpdateNativeVisibility");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnUpdateNativeVisibility(env, info) : nullptr;
}

napi_value JsSceneSession::SetPrivacyMode(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetPrivacyMode");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetPrivacyMode(env, info) : nullptr;
}

napi_value JsSceneSession::SetSystemSceneOcclusionAlpha(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetSystemSceneOcclusionAlpha");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetSystemSceneOcclusionAlpha(env, info) : nullptr;
}

napi_value JsSceneSession::SetFocusable(napi_env env, napi_callback_info info)
{
    WLOGI("[WMSFocus][NAPI]SetFocusable");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetFocusable(env, info) : nullptr;
}

napi_value JsSceneSession::SetSystemSceneBlockingFocus(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetSystemSceneBlockingFocus");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetSystemSceneBlockingFocus(env, info) : nullptr;
}

napi_value JsSceneSession::UpdateSizeChangeReason(napi_env env, napi_callback_info info)
{
    WLOGD("[NAPI]UpdateSizeChangeReason");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnUpdateSizeChangeReason(env, info) : nullptr;
}

napi_value JsSceneSession::SetShowRecent(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetShowRecent");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetShowRecent(env, info) : nullptr;
}

napi_value JsSceneSession::SetZOrder(napi_env env, napi_callback_info info)
{
    WLOGD("[NAPI]SetZOrder");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetZOrder(env, info) : nullptr;
}

napi_value JsSceneSession::SetTouchable(napi_env env, napi_callback_info info)
{
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetTouchable(env, info): nullptr;
}

napi_value JsSceneSession::SetSystemActive(napi_env env, napi_callback_info info)
{
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetSystemActive(env, info): nullptr;
}

napi_value JsSceneSession::SetFloatingScale(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetFloatingScale");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetFloatingScale(env, info) : nullptr;
}

napi_value JsSceneSession::SetSCBKeepKeyboard(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetSCBKeepKeyboard");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetSCBKeepKeyboard(env, info) : nullptr;
}

napi_value JsSceneSession::SetOffset(napi_env env, napi_callback_info info) {
    WLOGI("[NAPI]SetOffset");
    JsSceneSession *me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetOffset(env, info) : nullptr;
}

bool JsSceneSession::IsCallbackRegistered(napi_env env, const std::string& type, napi_value jsListenerObject)
{
    std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
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

bool JsSceneSession::IsCallbackTypeSupported(const std::string& type)
{
    if (listenerFunc_.find(type) != listenerFunc_.end()) {
        return true;
    }
    return false;
}

napi_value JsSceneSession::OnRegisterCallback(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 2) { // 2: params num
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
    if (!IsCallbackTypeSupported(cbType)) {
        WLOGFE("[NAPI]callback type is not supported, type = %{public}s", cbType.c_str());
        return NapiGetUndefined(env);
    }
    if (IsCallbackRegistered(env, cbType, value)) {
        WLOGFE("[NAPI]callback is registered, type = %{public}s", cbType.c_str());
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    std::shared_ptr<NativeReference> callbackRef;
    napi_ref result = nullptr;
    napi_create_reference(env, value, 1, &result);
    callbackRef.reset(reinterpret_cast<NativeReference*>(result));
    {
        std::unique_lock<std::shared_mutex> lock(jsCbMapMutex_);
        jsCbMap_[cbType] = callbackRef;
    }
    (this->*listenerFunc_[cbType])();
    WLOGFD("[NAPI]Register end, type = %{public}s", cbType.c_str());
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnUpdateNativeVisibility(napi_env env, napi_callback_info info)
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
    bool visible = false;
    if (!ConvertFromJsValue(env, argv[0], visible)) {
        WLOGFE("[NAPI]Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    session->UpdateNativeVisibility(visible);
    SceneSessionManager::GetInstance().UpdatePrivateStateAndNotify(session->GetPersistentId());
    WLOGFI("[NAPI]OnUpdateNativeVisibility end");
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetPrivacyMode(napi_env env, napi_callback_info info)
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
    bool isPrivacy = false;
    if (!ConvertFromJsValue(env, argv[0], isPrivacy)) {
        WLOGFE("[NAPI]Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    session->SetPrivacyMode(isPrivacy);
    SceneSessionManager::GetInstance().UpdatePrivateStateAndNotify(session->GetPersistentId());
    WLOGFI("[NAPI]OnSetPrivacyMode end");
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetSystemSceneOcclusionAlpha(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    double alpha = 0.f;
    if (!ConvertFromJsValue(env, argv[0], alpha)) {
        WLOGFE("[NAPI]Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    session->SetSystemSceneOcclusionAlpha(alpha);
    WLOGFI("[NAPI]OnSetSystemSceneOcclusionAlpha end");
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetFocusable(napi_env env, napi_callback_info info)
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
    bool isFocusable = false;
    if (!ConvertFromJsValue(env, argv[0], isFocusable)) {
        WLOGFE("[NAPI]Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    session->SetFocusable(isFocusable);
    WLOGFI("[WMSFocus][NAPI]OnSetFocusable end");
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetSystemSceneBlockingFocus(napi_env env, napi_callback_info info)
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
    bool blocking = false;
    if (!ConvertFromJsValue(env, argv[0], blocking)) {
        WLOGFE("[NAPI]Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    session->SetSystemSceneBlockingFocus(blocking);
    WLOGFI("[NAPI]OnSetSystemSceneBlockingFocus end");
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnUpdateSizeChangeReason(napi_env env, napi_callback_info info)
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
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    if (!ConvertFromJsValue(env, argv[0], reason)) {
        WLOGFE("[NAPI]Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    session->UpdateSizeChangeReason(reason);
    WLOGI("[NAPI]UpdateSizeChangeReason reason: %{public}u end", reason);
    return NapiGetUndefined(env);
}

void JsSceneSession::OnCreateSubSession(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("[NAPI]sceneSession is nullptr");
        return;
    }

    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(CREATE_SUB_SESSION_CB);
        if (iter == jsCbMap_.end()) {
            WLOGFE("[WMSSub][NAPI]Can't find callback, id: %{public}d", sceneSession->GetPersistentId());
            return;
        }
        jsCallBack = iter->second;
    }

    WLOGFI("[WMSLife][NAPI]OnCreateSubSession, id: %{public}d, parentId: %{public}d",
        sceneSession->GetPersistentId(), sceneSession->GetParentPersistentId());
    wptr<SceneSession> weakSession(sceneSession);
    auto task = [this, weakSession, jsCallBack, env = env_]() {
        auto specificSession = weakSession.promote();
        if (specificSession == nullptr) {
            WLOGFE("[WMSLife][NAPI]root session or target session or env is nullptr");
            return;
        }
        napi_value jsSceneSessionObj = Create(env, specificSession);
        if (jsSceneSessionObj == nullptr || !jsCallBack) {
            WLOGFE("[WMSLife][NAPI]jsSceneSessionObj or jsCallBack is nullptr");
            return;
        }
        WLOGFI("[WMSLife]CreateJsSceneSessionObject success, id: %{public}d, parentId: %{public}d",
            specificSession->GetPersistentId(), specificSession->GetParentPersistentId());
        napi_value argv[] = {jsSceneSessionObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnCreateSpecificSession PID:" + std::to_string(sceneSession->GetPersistentId()));
}

void JsSceneSession::OnBindDialogTarget(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFI("[NAPI]sceneSession is nullptr");
        return;
    }

    WLOGFI("[NAPI][WMSDialog] OnBindDialogTarget, id: %{public}d", sceneSession->GetPersistentId());
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(BIND_DIALOG_TARGET_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    wptr<SceneSession> weakSession(sceneSession);
    auto task = [this, weakSession, jsCallBack, env = env_]() {
        auto specificSession = weakSession.promote();
        if (specificSession == nullptr) {
            WLOGFE("[NAPI]root session or target session or env is nullptr");
            return;
        }
        napi_value jsSceneSessionObj = Create(env, specificSession);
        if (jsSceneSessionObj == nullptr || !jsCallBack) {
            WLOGFE("[NAPI]jsSceneSessionObj or jsCallBack is nullptr");
            return;
        }
        WLOGFI("[WMSDialog] CreateJsSceneSessionObject success, id: %{public}d", specificSession->GetPersistentId());
        napi_value argv[] = {jsSceneSessionObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnBindDialogTarget, PID:" +
        std::to_string(sceneSession->GetPersistentId()));
}

void JsSceneSession::OnSessionStateChange(const SessionState& state)
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFW("[WMSLife] session is nullptr");
        return;
    }

    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(SESSION_STATE_CHANGE_CB);
        if (iter == jsCbMap_.end()) {
            WLOGFW("[WMSLife] Not find sessionStateChangeCallback, id: %{public}d", session->GetPersistentId());
            return;
        }
        jsCallBack = iter->second;
    }

    WLOGFI("[WMSLife] id: %{public}d, state: %{public}d", session->GetPersistentId(), state);
    auto task = [state, jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionStateObj = CreateJsValue(env, state);
        napi_value argv[] = {jsSessionStateObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnSessionStateChange, state:" + std::to_string(static_cast<int>(state)));
}

void JsSceneSession::OnBufferAvailableChange(const bool isBufferAvailable)
{
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(BUFFER_AVAILABLE_CHANGE_CB);
        if (iter == jsCbMap_.end()) {
            WLOGFW("[NAPI]Not found bufferAvailableChange key in jsCbMap_");
            return;
        }
        jsCallBack = iter->second;
    }
    WLOGFD("[NAPI]OnBufferAvailableChange, state: %{public}u", isBufferAvailable);
    auto task = [isBufferAvailable, jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsBufferAvailableObj = CreateJsValue(env, isBufferAvailable);
        napi_value argv[] = { jsBufferAvailableObj };
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task);
}

void JsSceneSession::OnSessionRectChange(const WSRect& rect, const SizeChangeReason& reason)
{
    if (rect.IsEmpty()) {
        WLOGFD("Rect is empty, there is no need to notify");
        return;
    }
    WLOGFD("[NAPI]OnSessionRectChange");
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(SESSION_RECT_CHANGE_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [rect, reason, jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionStateObj = CreateJsSessionRect(env, rect);
        napi_value sizeChangeReason = CreateJsValue(env, static_cast<int32_t>(reason));
        napi_value argv[] = {jsSessionStateObj, sizeChangeReason};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    std::string rectInfo = "OnSessionRectChange [" + std::to_string(rect.posX_) + "," + std::to_string(rect.posY_)
        + "], [" + std::to_string(rect.width_) + ", " + std::to_string(rect.height_);
    taskScheduler_->PostMainThreadTask(task, rectInfo);
}

void JsSceneSession::OnRaiseToTop()
{
    WLOGFI("[NAPI]OnRaiseToTop");
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(RAISE_TO_TOP_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value argv[] = {};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), 0, argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnRaiseToTop");
}

void JsSceneSession::OnRaiseToTopForPointDown()
{
    WLOGFI("[NAPI]OnRaiseToTopForPointDown");
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(RAISE_TO_TOP_POINT_DOWN_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value argv[] = {};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), 0, argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnRaiseToTopForPointDown");
}

void JsSceneSession::OnRaiseAboveTarget(int32_t subWindowId)
{
    WLOGFI("[NAPI]OnRaiseAboveTarget");
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(RAISE_ABOVE_TARGET_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [jsCallBack, env = env_, subWindowId]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsSceneSessionObj = CreateJsValue(env, subWindowId);
        if (jsSceneSessionObj == nullptr) {
            WLOGFE("[NAPI]jsSceneSessionObj is nullptr");
            return;
        }
        napi_value argv[] = {[0] = CreateJsError(env, 0), [1] = jsSceneSessionObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnRaiseAboveTarget: " + std::to_string(subWindowId));
}


void JsSceneSession::OnSessionFocusableChange(bool isFocusable)
{
    WLOGFI("[WMSFocus][NAPI]OnSessionFocusableChange, state: %{public}u", isFocusable);
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(SESSION_FOCUSABLE_CHANGE_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [isFocusable, jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionFocusableObj = CreateJsValue(env, isFocusable);
        napi_value argv[] = {jsSessionFocusableObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnSessionFocusableChange, state:" + std::to_string(isFocusable));
}

void JsSceneSession::OnSessionTouchableChange(bool touchable)
{
    WLOGFI("[NAPI]OnSessionTouchableChange, state: %{public}u", touchable);
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(SESSION_TOUCHABLE_CHANGE_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [touchable, jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionTouchableObj = CreateJsValue(env, touchable);
        napi_value argv[] = {jsSessionTouchableObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnSessionTouchableChange: state " + std::to_string(touchable));
}

void JsSceneSession::OnClick()
{
    WLOGFD("[NAPI]OnClick");
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(CLICK_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value argv[] = {};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), 0, argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnClick");
}

void JsSceneSession::PendingSessionActivation(SessionInfo& info)
{
    WLOGI("[NAPI]pending session activation: bundleName %{public}s, moduleName %{public}s, abilityName %{public}s, \
        appIndex %{public}d, reuse %{public}d", info.bundleName_.c_str(), info.moduleName_.c_str(),
        info.abilityName_.c_str(), info.appIndex_, info.reuse);
    sptr<SceneSession> sceneSession = nullptr;
    if (info.persistentId_ == 0) {
        auto result = SceneSessionManager::GetInstance().CheckIfReuseSession(info);
        if (result == BrokerStates::BROKER_NOT_START) {
            WLOGE("[NAPI] The BrokerStates is not opened");
            return;
        }
        if (info.reuse) {
            WLOGFI("session need to be reusesd.");
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
                return;
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
            return;
        }
        sceneSession->SetSessionInfo(info);
    }
    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto task = [this, sessionInfo]() {
        PendingSessionActivationInner(sessionInfo);
    };
    sceneSession->PostLifeCycleTask(task, "PendingSessionActivation", LifeCycleTaskType::START);
}

void JsSceneSession::PendingSessionActivationInner(std::shared_ptr<SessionInfo> sessionInfo)
{
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(PENDING_SCENE_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    napi_env& env_ref = env_;
    auto task = [sessionInfo, jsCallBack, env_ref]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        if (sessionInfo == nullptr) {
            WLOGFE("[NAPI]sessionInfo is nullptr");
            return;
        }
        napi_value jsSessionInfo = CreateJsSessionInfo(env_ref, *sessionInfo);
        if (jsSessionInfo == nullptr) {
            WLOGFE("[NAPI]this target session info is nullptr");
            return;
        }
        napi_value argv[] = {jsSessionInfo};
        napi_call_function(env_ref, NapiGetUndefined(env_ref),
            jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "PendingSessionActivationInner");
}

void JsSceneSession::OnBackPressed(bool needMoveToBackground)
{
    WLOGFI("[NAPI]OnBackPressed needMoveToBackground %{public}d", needMoveToBackground);
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(BACK_PRESSED_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [needMoveToBackground, jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsNeedMoveToBackgroundObj = CreateJsValue(env, needMoveToBackground);
        napi_value argv[] = {jsNeedMoveToBackgroundObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnBackPressed:" + std::to_string(needMoveToBackground));
}

void JsSceneSession::TerminateSession(const SessionInfo& info)
{
    WLOGFI("[NAPI]run TerminateSession, bundleName = %{public}s, abilityName = %{public}s, persistentId = %{public}d",
        info.bundleName_.c_str(), info.abilityName_.c_str(), info.persistentId_);
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(TERMINATE_SESSION_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto task = [sessionInfo, jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        if (sessionInfo == nullptr) {
            WLOGFE("[NAPI]sessionInfo is nullptr");
            return;
        }
        napi_value jsSessionInfo = CreateJsSessionInfo(env, *sessionInfo);
        if (jsSessionInfo == nullptr) {
            WLOGFE("[NAPI]this target session info is nullptr");
            return;
        }
        napi_value argv[] = {jsSessionInfo};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "TerminateSession name:" + info.abilityName_);
}

void JsSceneSession::TerminateSessionNew(const SessionInfo& info, bool needStartCaller)
{
    WLOGFI("[NAPI]run TerminateSessionNew, bundleName = %{public}s, id = %{public}s",
        info.bundleName_.c_str(), info.abilityName_.c_str());
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(TERMINATE_SESSION_CB_NEW);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [needStartCaller, jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsNeedStartCaller = CreateJsValue(env, needStartCaller);
        if (jsNeedStartCaller == nullptr) {
            WLOGFE("[NAPI]this target jsNeedStartCaller is nullptr");
            return;
        }
        napi_value argv[] = {jsNeedStartCaller};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "TerminateSessionNew, name:" + info.abilityName_);
}

void JsSceneSession::TerminateSessionTotal(const SessionInfo& info, TerminateType terminateType)
{
    WLOGFI("[NAPI]run TerminateSession, bundleName = %{public}s, id = %{public}s, terminateType = %{public}d",
        info.bundleName_.c_str(), info.abilityName_.c_str(), static_cast<int32_t>(terminateType));
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(TERMINATE_SESSION_CB_TOTAL);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [terminateType, jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsTerminateType = CreateJsValue(env, static_cast<int32_t>(terminateType));
        if (jsTerminateType == nullptr) {
            WLOGFE("[NAPI]this target jsTerminateType is nullptr");
            return;
        }
        napi_value argv[] = {jsTerminateType};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "TerminateSessionTotal:name:" + info.abilityName_);
}

void JsSceneSession::UpdateSessionLabel(const std::string &label)
{
    WLOGFI("[NAPI]run UpdateSessionLabel");
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(UPDATE_SESSION_LABEL_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [label, jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsLabel = CreateJsValue(env, label);
        if (jsLabel == nullptr) {
            WLOGFE("[NAPI]this target jsLabel is nullptr");
            return;
        }
        napi_value argv[] = {jsLabel};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "UpdateSessionLabel");
}

void JsSceneSession::ProcessUpdateSessionLabelRegister()
{
    WLOGFD("begin to run ProcessUpdateSessionLabelRegister");
    NofitySessionLabelUpdatedFunc func = [weak = weak_from_this()](const std::string& label) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->UpdateSessionLabel(label);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetUpdateSessionLabelListener(func);
    WLOGFD("ProcessUpdateSessionLabelRegister success");
}

void JsSceneSession::ProcessUpdateSessionIconRegister()
{
    WLOGFD("begin to run ProcessUpdateSessionIconRegister");
    NofitySessionIconUpdatedFunc func = [weak = weak_from_this()](const std::string& iconPath) {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->UpdateSessionIcon(iconPath);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetUpdateSessionIconListener(func);
    WLOGFD("ProcessUpdateSessionIconRegister success");
}

void JsSceneSession::UpdateSessionIcon(const std::string &iconPath)
{
    WLOGFI("[NAPI]run UpdateSessionIcon");
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(UPDATE_SESSION_ICON_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [iconPath, jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsIconPath = CreateJsValue(env, iconPath);
        if (jsIconPath == nullptr) {
            WLOGFE("[NAPI]this target jsIconPath is nullptr");
            return;
        }
        napi_value argv[] = {jsIconPath};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "UpdateSessionIcon");
}

void JsSceneSession::OnSessionException(const SessionInfo& info, bool needRemoveSession)
{
    WLOGFI("[NAPI]run OnSessionException, bundleName = %{public}s, id = %{public}s",
        info.bundleName_.c_str(), info.abilityName_.c_str());
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(SESSION_EXCEPTION_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto task = [sessionInfo, needRemoveSession, jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        if (sessionInfo == nullptr) {
            WLOGFE("[NAPI]sessionInfo is nullptr");
            return;
        }
        napi_value jsSessionInfo = CreateJsSessionInfo(env, *sessionInfo);
        napi_value jsNeedRemoveSession = CreateJsValue(env, needRemoveSession);
        if (jsSessionInfo == nullptr) {
            WLOGFE("[NAPI]this target session info is nullptr");
            return;
        }
        napi_value argv[] = {[0] = jsSessionInfo, [1] = jsNeedRemoveSession};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnSessionException, name" + info.bundleName_);
}

void JsSceneSession::PendingSessionToForeground(const SessionInfo& info)
{
    WLOGFI("[NAPI]run PendingSessionToForeground, bundleName = %{public}s, id = %{public}s",
        info.bundleName_.c_str(), info.abilityName_.c_str());
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(PENDING_SESSION_TO_FOREGROUND_CB);
        if (iter == jsCbMap_.end()) {
            WLOGFE("[NAPI]fail to find pending session to foreground callback");
            return;
        }
        jsCallBack = iter->second;
    }
    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto task = [sessionInfo, jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        if (sessionInfo == nullptr) {
            WLOGFE("[NAPI]sessionInfo is nullptr");
            return;
        }
        napi_value jsSessionInfo = CreateJsSessionInfo(env, *sessionInfo);
        if (jsSessionInfo == nullptr) {
            WLOGFE("[NAPI]this target session info is nullptr");
            return;
        }
        napi_value argv[] = {jsSessionInfo};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "PendingSessionToForeground:" + info.bundleName_);
}

void JsSceneSession::PendingSessionToBackgroundForDelegator(const SessionInfo& info)
{
    WLOGFI("[NAPI]run PendingSessionToBackgroundForDelegator, bundleName = %{public}s, id = %{public}s",
        info.bundleName_.c_str(), info.abilityName_.c_str());
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR_CB);
        if (iter == jsCbMap_.end()) {
            WLOGFE("[NAPI]fail to find pending session to background for delegator callback");
            return;
        }
        jsCallBack = iter->second;
    }
    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto task = [sessionInfo, jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        if (sessionInfo == nullptr) {
            WLOGFE("[NAPI]sessionInfo is nullptr");
            return;
        }
        napi_value jsSessionInfo = CreateJsSessionInfo(env, *sessionInfo);
        if (jsSessionInfo == nullptr) {
            WLOGFE("[NAPI]this target session info is nullptr");
            return;
        }
        napi_value argv[] = {jsSessionInfo};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "PendingSessionToBackgroundForDelegator, name:" + info.bundleName_);
}

void JsSceneSession::OnSystemBarPropertyChange(const std::unordered_map<WindowType, SystemBarProperty>& propertyMap)
{
    WLOGFI("[NAPI][WMSImms]OnSystemBarPropertyChange");
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(SYSTEMBAR_PROPERTY_CHANGE_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [jsCallBack, propertyMap, env = env_]() {
        napi_value jsSessionStateObj = CreateJsSystemBarPropertyArrayObject(env, propertyMap);
        if (jsSessionStateObj == nullptr) {
            WLOGFE("[NAPI]jsSessionStateObj is nullptr");
            return;
        }
        napi_value argv[] = {jsSessionStateObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnSystemBarPropertyChange");
}

void JsSceneSession::OnNeedAvoid(bool status)
{
    WLOGFI("[NAPI][WMSImms]OnNeedAvoid %{public}d", status);
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(NEED_AVOID_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [jsCallBack, needAvoid = status, env = env_]() {
        napi_value jsSessionStateObj = CreateJsValue(env, needAvoid);
        napi_value argv[] = {jsSessionStateObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnNeedAvoid:" + std::to_string(status));
}

void JsSceneSession::OnIsCustomAnimationPlaying(bool status)
{
    WLOGFI("[NAPI]OnIsCustomAnimationPlaying %{public}d", status);
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(CUSTOM_ANIMATION_PLAYING_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [jsCallBack, isPlaying = status, env = env_]() {
        napi_value jsSessionStateObj = CreateJsValue(env, isPlaying);
        napi_value argv[] = {jsSessionStateObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnIsCustomAnimationPlaying:" + std::to_string(status));
}

void JsSceneSession::OnShowWhenLocked(bool showWhenLocked)
{
    WLOGFI("[NAPI]OnShowWhenLocked %{public}d", showWhenLocked);
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(SHOW_WHEN_LOCKED_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [jsCallBack, flag = showWhenLocked, env = env_]() {
        napi_value jsSessionStateObj = CreateJsValue(env, flag);
        napi_value argv[] = {jsSessionStateObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnShowWhenLocked:" +std::to_string(showWhenLocked));
}

void JsSceneSession::OnReuqestedOrientationChange(uint32_t orientation)
{
    WLOGFI("[NAPI]OnReuqestedOrientationChange %{public}u", orientation);
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(REQUESTED_ORIENTATION_CHANGE_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    if (WINDOW_ORIENTATION_TO_JS_SESSION_MAP.count(static_cast<Orientation>(orientation)) == 0) {
        WLOGFE("[NAPI]failed %{public}u since no this type", orientation);
        return;
    }
    uint32_t value = static_cast<uint32_t>(WINDOW_ORIENTATION_TO_JS_SESSION_MAP.at(
        static_cast<Orientation>(orientation)));
    auto task = [jsCallBack, rotation = value, env = env_]() {
        napi_value jsSessionRotationObj = CreateJsValue(env, rotation);
        napi_value argv[] = {jsSessionRotationObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        WLOGFI("[NAPI]change rotation success %{public}u", rotation);
    };
    taskScheduler_->PostMainThreadTask(task, "OnReuqestedOrientationChange:orientation" +std::to_string(orientation));
}

napi_value JsSceneSession::OnSetShowRecent(napi_env env, napi_callback_info info)
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is null");
        return NapiGetUndefined(env);
    }
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    bool showRecent = true;
    if (argc == ARGC_ONE && GetType(env, argv[0]) == napi_boolean) {
        if (!ConvertFromJsValue(env, argv[0], showRecent)) {
            WLOGFE("[NAPI]Failed to convert parameter to bool");
            return NapiGetUndefined(env);
        }
    }
    session->SetShowRecent(showRecent);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetSystemActive(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
       WLOGFE("[NAPI]argc is invalid : %{public}zu", argc);
       napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM), 
       "Input parameter is missing or invalid"));
       return NapiGetUndefined(env);
    }
    bool scbSystemActive = false;
    if (!ConvertFromJsValue(env, argv[0], scbSystemActive)){
        WLOGFE("[NAPI] Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM), 
        "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI] session_ is null");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY), 
        "session is null"));
        return NapiGetUndefined(env);
    }
    session->SetSystemActive(scbSystemActive);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetZOrder(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    uint32_t zOrder;
    if (!ConvertFromJsValue(env, argv[0], zOrder)) {
        WLOGFE("[NAPI]Failed to convert parameter to uint32_t");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFW("[NAPI]session is null");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY),
            "session is null"));
        return NapiGetUndefined(env);
    }
    session->SetZOrder(zOrder);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetFloatingScale(napi_env env, napi_callback_info info)
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
    double_t floatingScale = 1.0;
    if (!ConvertFromJsValue(env, argv[0], floatingScale)) {
        WLOGFE("[NAPI]Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    session->SetFloatingScale(static_cast<float_t>(floatingScale));
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetTouchable(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != 1) {
        WLOGFE("[NAPI] Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input Parameter is missing or invalid" ));
        return NapiGetUndefined(env);
    }

    bool touchable = false;
    if (!ConvertFromJsValue(env, argv[0], touchable)) {
      WLOGFE("[NAPI] Failed to  convert parameter to touchable");
      napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
        "Input parameter is missing or invalid" ));
      return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI] Session is null");
        napi_throw(env, CreateJsError(env,
            static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY), "session is null"));
        return NapiGetUndefined(env);
    }

    session->SetSystemTouchable(touchable);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetSCBKeepKeyboard(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool scbKeepKeyboardFlag = false;
    if (!ConvertFromJsValue(env, argv[0], scbKeepKeyboardFlag)) {
        WLOGFE("[NAPI]Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    session->SetSCBKeepKeyboard(scbKeepKeyboardFlag);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetOffset(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != 2) {
        WLOGFE("[NAPI]Argc count is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    double offsetX = 0.0f;
    if (!ConvertFromJsValue(env, argv[0], offsetX)) {
        WLOGFE("[NAPI]Failed to convert parameter to double");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    double offsetY = 0.0f;
    if (!ConvertFromJsValue(env, argv[1], offsetY)) {
        WLOGFE("[NAPI]Failed to convert parameter to double");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    session->SetOffset(static_cast<float>(offsetX), static_cast<float>(offsetY));
    return NapiGetUndefined(env);
}

void JsSceneSession::ProcessPrepareClosePiPSessionRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onPrepareClosePiPSession_ = [weak = weak_from_this()]() {
        auto weakJsSceneSession = weak.lock();
        if (weakJsSceneSession) weakJsSceneSession->OnPrepareClosePiPSession();
    };
    WLOGFD("ProcessPrepareClosePiPSessionRegister success");
}

void JsSceneSession::OnPrepareClosePiPSession()
{
    WLOGFI("[NAPI]OnPrepareClosePiPSession");
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(PREPARE_CLOSE_PIP_SESSION);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter-> second;
    }
    auto task = [jsCallBack, env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value argv[] = {};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), 0, argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnPrepareClosePiPSession");
}

napi_value JsSceneSession::SetScale(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetScale");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetScale(env, info) : nullptr;
}

napi_value JsSceneSession::OnSetScale(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < SCALE_ARG_COUNT) { // SCALE_ARG_COUNT: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    double_t scaleX = 1.0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_0], scaleX)) {
        WLOGFE("[NAPI]Failed to convert parameter to scaleX");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    double_t scaleY = 1.0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_1], scaleY)) {
        WLOGFE("[NAPI]Failed to convert parameter to scaleY");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    double_t pivotX = 0.0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_2], pivotX)) {
        WLOGFE("[NAPI]Failed to convert parameter to pivotX");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    double_t pivotY = 0.0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_3], pivotY)) {
        WLOGFE("[NAPI]Failed to convert parameter to pivotY");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    session->SetScale(static_cast<float_t>(scaleX), static_cast<float_t>(scaleY), static_cast<float_t>(pivotX),
        static_cast<float_t>(pivotY));
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::RequestHideKeyboard(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]RequestHideKeyboard");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnRequestHideKeyboard(env, info) : nullptr;
}

napi_value JsSceneSession::OnRequestHideKeyboard(napi_env env, napi_callback_info info)
{
    WLOGFI("[NAPI]OnRequestHideKeyboard");
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    session->RequestHideKeyboard();
    return NapiGetUndefined(env);
}


sptr<SceneSession> JsSceneSession::GetNativeSession() const
{
    return weakSession_.promote();
}
} // namespace OHOS::Rosen
