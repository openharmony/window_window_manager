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
} // namespace

napi_value JsSceneSession::Create(napi_env env, const sptr<SceneSession>& session)
{
    WLOGI("[NAPI]Create");
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
    BindNativeFunction(env, objValue, "setPrivacyMode", moduleName, JsSceneSession::SetPrivacyMode);
    BindNativeFunction(env, objValue, "setFloatingScale", moduleName, JsSceneSession::SetFloatingScale);
    BindNativeFunction(env, objValue, "setSystemSceneOcclusionAlpha", moduleName,
        JsSceneSession::SetSystemSceneOcclusionAlpha);
    BindNativeFunction(env, objValue, "setFocusable", moduleName, JsSceneSession::SetFocusable);

    return objValue;
}

JsSessionType JsSceneSession::GetApiType(WindowType type)
{
    auto iter = WINDOW_TO_JS_SESSION_TYPE_MAP.find(type);
    if (iter == WINDOW_TO_JS_SESSION_TYPE_MAP.end()) {
        WLOGFE("[NAPI]window type: %{public}u cannot map to api type!", type);
        return JsSessionType::TYPE_UNDEFINED;
    } else {
        return iter->second;
    }
}

JsSceneSession::JsSceneSession(napi_env env, const sptr<SceneSession>& session)
    : env_(env), weakSession_(session)
{
    listenerFunc_ = {
        { PENDING_SCENE_CB,                      &JsSceneSession::ProcessPendingSceneSessionActivationRegister },
        { SESSION_STATE_CHANGE_CB,               &JsSceneSession::ProcessSessionStateChangeRegister },
        { SESSION_EVENT_CB,                      &JsSceneSession::ProcessSessionEventRegister },
        { SESSION_RECT_CHANGE_CB,                &JsSceneSession::ProcessSessionRectChangeRegister },
        { CREATE_SPECIFIC_SCENE_CB,              &JsSceneSession::ProcessCreateSpecificSessionRegister },
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
        { FORCE_HIDE_CHANGE_CB,                  &JsSceneSession::ProcessForceHideChangeRegister }
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
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->UnregisterSessionStateChangeListenser();
}

void JsSceneSession::ClearCbMap(bool needRemove)
{
    if (!needRemove) {
        return;
    }
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [this](napi_env env, NapiAsyncTask& task, int32_t status) {
            WLOGFI("clear callbackMap");
            this->jsCbMap_.clear();
    });
    napi_ref callback = nullptr;
    NapiAsyncTask::ScheduleHighQos("JsSceneSession::ClearCbMap", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::ProcessSessionDefaultAnimationFlagChangeRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onWindowAnimationFlagChange_ = std::bind(
        &JsSceneSession::OnDefaultAnimationFlagChange, this, std::placeholders::_1);
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
    auto iter = jsCbMap_.find(NEED_DEFAULT_ANIMATION_FLAG_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [isNeedDefaultAnimationFlag, jsCallBack, eng = env_](
            napi_env env, NapiAsyncTask& task, int32_t status) {
            napi_value jsSessionDefaultAnimationFlagObj = CreateJsValue(env, isNeedDefaultAnimationFlag);
            napi_value argv[] = { jsSessionDefaultAnimationFlagObj };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::OnDefaultAnimationFlagChange", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
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

void JsSceneSession::ProcessBindDialogTargetRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onBindDialogTarget_ = std::bind(&JsSceneSession::OnBindDialogTarget,
        this, std::placeholders::_1);
    WLOGFD("ProcessBindDialogTargetRegister success");
}

void JsSceneSession::ProcessSessionRectChangeRegister()
{
    NotifySessionRectChangeFunc func = [this](const WSRect& rect, const SizeChangeReason& reason) {
        this->OnSessionRectChange(rect, reason);
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
    sessionchangeCallback->onRaiseToTop_ = std::bind(&JsSceneSession::OnRaiseToTop, this);
    WLOGFD("ProcessRaiseToTopRegister success");
}

void JsSceneSession::ProcessRaiseToTopForPointDownRegister()
{
    NotifyRaiseToTopForPointDownFunc func = [this]() {
        this->OnRaiseToTopForPointDown();
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
    sessionchangeCallback->onRaiseAboveTarget_ = std::bind(&JsSceneSession::OnRaiseAboveTarget,
        this, std::placeholders::_1);
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

void JsSceneSession::ProcessTerminateSessionRegisterNew()
{
    WLOGFD("begin to run ProcessTerminateSessionRegisterNew");
    NotifyTerminateSessionFuncNew func = [this](const SessionInfo& info, bool needStartCaller) {
        this->TerminateSessionNew(info, needStartCaller);
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
    NotifyTerminateSessionFuncTotal func = [this](const SessionInfo& info, TerminateType terminateType) {
        this->TerminateSessionTotal(info, terminateType);
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
    NotifyPendingSessionToForegroundFunc func = [this](const SessionInfo& info) {
        this->PendingSessionToForeground(info);
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
    NotifyPendingSessionToBackgroundForDelegatorFunc func = [this](const SessionInfo& info) {
        this->PendingSessionToBackgroundForDelegator(info);
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
    NotifySessionFocusableChangeFunc func = [this](bool isFocusable) {
        this->OnSessionFocusableChange(isFocusable);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetSessionFocusableChangeListener(func);
    WLOGFI("ProcessSessionFocusableChangeRegister success");
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
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [eventId, jsCallBack, eng = env_](napi_env env, NapiAsyncTask& task, int32_t status) {
            napi_value jsSessionStateObj = CreateJsValue(env, eventId);
            napi_value argv[] = { jsSessionStateObj };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::OnSessionEvent", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::ProcessBackPressedRegister()
{
    NotifyBackPressedFunc func = [this](bool needMoveToBackground) {
        this->OnBackPressed(needMoveToBackground);
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

void JsSceneSession::ProcessIsCustomAnimationPlaying()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onIsCustomAnimationPlaying_ = std::bind(
        &JsSceneSession::OnIsCustomAnimationPlaying, this, std::placeholders::_1);
    WLOGFD("ProcessIsCustomAnimationPlaying success");
}

void JsSceneSession::ProcessShowWhenLockedRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->OnShowWhenLocked_ = std::bind(
        &JsSceneSession::OnShowWhenLocked, this, std::placeholders::_1);
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
    sessionchangeCallback->OnRequestedOrientationChange_ = std::bind(
        &JsSceneSession::OnReuqestedOrientationChange, this, std::placeholders::_1);
    WLOGFD("ProcessRequestedOrientationChange success");
}

void JsSceneSession::ProcessForceHideChangeRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->OnForceHideChange_ = std::bind(&JsSceneSession::OnForceHideChange,
        this, std::placeholders::_1);
    WLOGFD("ProcessForceHideChangeRegister success");
}

void JsSceneSession::OnForceHideChange(bool hide)
{
    WLOGFI("[NAPI]OnForceHideChange, hide: %{public}u", hide);
    auto iter = jsCbMap_.find(FORCE_HIDE_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [hide, jsCallBack, eng = env_](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            napi_value jsSessionForceHideObj = CreateJsValue(env, hide);
            napi_value argv[] = { jsSessionForceHideObj };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });
    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::OnForceHideChange", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
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
    WLOGI("[NAPI]SetFocusable");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetFocusable(env, info) : nullptr;
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

napi_value JsSceneSession::SetFloatingScale(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetFloatingScale");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetFloatingScale(env, info) : nullptr;
}

bool JsSceneSession::IsCallbackRegistered(napi_env env, const std::string& type, napi_value jsListenerObject)
{
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
    jsCbMap_[cbType] = callbackRef;
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
    size_t argc = 4;
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
    WLOGFI("[NAPI]OnSetFocusable end");
    return NapiGetUndefined(env);
}

void JsSceneSession::OnCreateSpecificSession(const sptr<SceneSession>& sceneSession)
{
    WLOGFD("OnCreateSpecificSession");
    if (sceneSession == nullptr) {
        WLOGFI("[NAPI]sceneSession is nullptr");
        return;
    }

    WLOGFD("[NAPI]OnCreateSpecificSession");
    auto iter = jsCbMap_.find(CREATE_SPECIFIC_SCENE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }

    auto jsCallBack = iter->second;
    wptr<SceneSession> weakSession(sceneSession);
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [this, weakSession, jsCallBack, eng = env_](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto specificSession = weakSession.promote();
            if (specificSession == nullptr) {
                WLOGFE("[NAPI]root session or target session or env is nullptr");
                return;
            }
            napi_value jsSceneSessionObj = Create(eng, specificSession);
            if (jsSceneSessionObj == nullptr || !jsCallBack) {
                WLOGFE("[NAPI]jsSceneSessionObj or jsCallBack is nullptr");
                return;
            }
            WLOGFI("CreateJsSceneSessionObject success");
            napi_value argv[] = { jsSceneSessionObj };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::OnCreateSpecificSession", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnBindDialogTarget(const sptr<SceneSession>& sceneSession)
{
    WLOGFI("OnBindDialogTarget");
    if (sceneSession == nullptr) {
        WLOGFI("[NAPI]sceneSession is nullptr");
        return;
    }

    WLOGFI("[NAPI]OnBindDialogTarget");
    auto iter = jsCbMap_.find(BIND_DIALOG_TARGET_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }

    auto jsCallBack = iter->second;
    wptr<SceneSession> weakSession(sceneSession);
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [this, weakSession, jsCallBack, eng = env_](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto specificSession = weakSession.promote();
            if (specificSession == nullptr) {
                WLOGFE("[NAPI]root session or target session or env is nullptr");
                return;
            }
            napi_value jsSceneSessionObj = Create(eng, specificSession);
            if (jsSceneSessionObj == nullptr || !jsCallBack) {
                WLOGFE("[NAPI]jsSceneSessionObj or jsCallBack is nullptr");
                return;
            }
            WLOGFI("CreateJsSceneSessionObject success");
            napi_value argv[] = { jsSceneSessionObj };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::OnBindDialogTarget", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnSessionStateChange(const SessionState& state)
{
    WLOGFD("[NAPI]OnSessionStateChange, state: %{public}u", static_cast<uint32_t>(state));
    auto iter = jsCbMap_.find(SESSION_STATE_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [state, jsCallBack, eng = env_](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            napi_value jsSessionStateObj = CreateJsValue(env, state);
            napi_value argv[] = { jsSessionStateObj };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::OnSessionStateChange", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnSessionRectChange(const WSRect& rect, const SizeChangeReason& reason)
{
    if (rect.IsEmpty()) {
        WLOGFD("Rect is empty, there is no need to notify");
        return;
    }
    WLOGFD("[NAPI]OnSessionRectChange");
    auto iter = jsCbMap_.find(SESSION_RECT_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [rect, reason, jsCallBack, eng = env_](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            napi_value jsSessionStateObj = CreateJsSessionRect(env, rect);
            napi_value sizeChangeReason = CreateJsValue(env, static_cast<int32_t>(reason));
            napi_value argv[] = { jsSessionStateObj, sizeChangeReason };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::OnSessionRectChange", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnRaiseToTop()
{
    WLOGFI("[NAPI]OnRaiseToTop");
    auto iter = jsCbMap_.find(RAISE_TO_TOP_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [jsCallBack, eng = env_](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            napi_value argv[] = {};
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), 0, argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::OnRaiseToTop", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnRaiseToTopForPointDown()
{
    WLOGFI("[NAPI]OnRaiseToTopForPointDown");
    auto iter = jsCbMap_.find(RAISE_TO_TOP_POINT_DOWN_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [jsCallBack, eng = env_](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            napi_value argv[] = {};
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), 0, argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::OnRaiseToTopForPointDown", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnRaiseAboveTarget(int32_t subWindowId)
{
    WLOGFI("[NAPI]OnRaiseAboveTarget");
    auto iter = jsCbMap_.find(RAISE_ABOVE_TARGET_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [jsCallBack, eng = env_, subWindowId](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            napi_value jsSceneSessionObj = CreateJsValue(env, subWindowId);
            if (jsSceneSessionObj == nullptr) {
                WLOGFE("[NAPI]jsSceneSessionObj is nullptr");
                return;
            }
            napi_value argv[] = {
                [0]=CreateJsError(env, 0),
                [1]=jsSceneSessionObj
            };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::OnRaiseAboveTarget", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}


void JsSceneSession::OnSessionFocusableChange(bool isFocusable)
{
    WLOGFI("[NAPI]OnSessionFocusableChange, state: %{public}u", isFocusable);
    auto iter = jsCbMap_.find(SESSION_FOCUSABLE_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [isFocusable, jsCallBack, eng = env_](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            napi_value jsSessionFocusableObj = CreateJsValue(env, isFocusable);
            napi_value argv[] = { jsSessionFocusableObj };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::OnSessionFocusableChange", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnSessionTouchableChange(bool touchable)
{
    WLOGFI("[NAPI]OnSessionTouchableChange, state: %{public}u", touchable);
    auto iter = jsCbMap_.find(SESSION_TOUCHABLE_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [touchable, jsCallBack, eng = env_](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            napi_value jsSessionTouchableObj = CreateJsValue(env, touchable);
            napi_value argv[] = { jsSessionTouchableObj };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::OnSessionTouchableChange", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnClick()
{
    WLOGFI("[NAPI]OnClick");
    auto iter = jsCbMap_.find(CLICK_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [jsCallBack, eng = env_](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            napi_value argv[] = { };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), 0, argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::OnClick", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::PendingSessionActivation(SessionInfo& info)
{
    WLOGI("[NAPI]pending session activation: bundleName %{public}s, moduleName %{public}s, abilityName %{public}s, \
        appIndex %{public}d, reuse %{public}d", info.bundleName_.c_str(), info.moduleName_.c_str(),
        info.abilityName_.c_str(), info.appIndex_, info.reuse);
    if (info.persistentId_ == 0) {
        SceneSessionManager::GetInstance().CheckIfReuseSession(info);
        sptr<SceneSession> sceneSession = nullptr;
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
        auto sceneSession = SceneSessionManager::GetInstance().GetSceneSession(info.persistentId_);
        if (sceneSession == nullptr) {
            WLOGFE("GetSceneSession return nullptr");
            return;
        }
        sceneSession->SetSessionInfo(info);
    }

    PendingSessionActivationInner(info);
}

void JsSceneSession::PendingSessionActivationInner(SessionInfo& info)
{
    auto iter = jsCbMap_.find(PENDING_SCENE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [sessionInfo, jsCallBack](napi_env env, NapiAsyncTask& task, int32_t status) {
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
            napi_value argv[] = { jsSessionInfo };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::PendingSessionActivation", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnBackPressed(bool needMoveToBackground)
{
    WLOGFI("[NAPI]OnBackPressed needMoveToBackground %{public}d", needMoveToBackground);
    auto iter = jsCbMap_.find(BACK_PRESSED_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [needMoveToBackground, jsCallBack, eng = env_](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            napi_value jsNeedMoveToBackgroundObj = CreateJsValue(env, needMoveToBackground);
            napi_value argv[] = { jsNeedMoveToBackgroundObj };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::OnBackPressed", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::TerminateSession(const SessionInfo& info)
{
    WLOGFI("[NAPI]run TerminateSession, bundleName = %{public}s, abilityName = %{public}s, persistentId = %{public}d",
        info.bundleName_.c_str(), info.abilityName_.c_str(), info.persistentId_);
    auto iter = jsCbMap_.find(TERMINATE_SESSION_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [sessionInfo, jsCallBack](napi_env env, NapiAsyncTask& task, int32_t status) {
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
            napi_value argv[] = { jsSessionInfo };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::TerminateSession", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::TerminateSessionNew(const SessionInfo& info, bool needStartCaller)
{
    WLOGFI("[NAPI]run TerminateSessionNew, bundleName = %{public}s, id = %{public}s",
        info.bundleName_.c_str(), info.abilityName_.c_str());
    auto iter = jsCbMap_.find(TERMINATE_SESSION_CB_NEW);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [needStartCaller, jsCallBack](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            napi_value jsNeedStartCaller = CreateJsValue(env, needStartCaller);
            if (jsNeedStartCaller == nullptr) {
                WLOGFE("[NAPI]this target jsNeedStartCaller is nullptr");
                return;
            }
            napi_value argv[] = { jsNeedStartCaller };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::TerminateSessionNew", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::TerminateSessionTotal(const SessionInfo& info, TerminateType terminateType)
{
    WLOGFI("[NAPI]run TerminateSession, bundleName = %{public}s, id = %{public}s, terminateType = %{public}d",
        info.bundleName_.c_str(), info.abilityName_.c_str(), static_cast<int32_t>(terminateType));
    auto iter = jsCbMap_.find(TERMINATE_SESSION_CB_TOTAL);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [terminateType, jsCallBack](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            napi_value jsTerminateType = CreateJsValue(env, static_cast<int32_t>(terminateType));
            if (jsTerminateType == nullptr) {
                WLOGFE("[NAPI]this target jsTerminateType is nullptr");
                return;
            }
            napi_value argv[] = { jsTerminateType  };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::terminateSessionTotal", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::UpdateSessionLabel(const std::string &label)
{
    WLOGFI("[NAPI]run UpdateSessionLabel");
    auto iter = jsCbMap_.find(UPDATE_SESSION_LABEL_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [label, jsCallBack](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            napi_value jsLabel = CreateJsValue(env, label);
            if (jsLabel == nullptr) {
                WLOGFE("[NAPI]this target jsLabel is nullptr");
                return;
            }
            napi_value argv[] = { jsLabel  };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::UpdateSessionLabel", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::ProcessUpdateSessionLabelRegister()
{
    WLOGFD("begin to run ProcessUpdateSessionLabelRegister");
    NofitySessionLabelUpdatedFunc func = [this](const std::string& label) {
        this->UpdateSessionLabel(label);
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
    NofitySessionIconUpdatedFunc func = [this](const std::string& iconPath) {
        this->UpdateSessionIcon(iconPath);
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
    auto iter = jsCbMap_.find(UPDATE_SESSION_ICON_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [iconPath, jsCallBack](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (!jsCallBack) {
                WLOGFE("[NAPI]jsCallBack is nullptr");
                return;
            }
            napi_value jsIconPath = CreateJsValue(env, iconPath);
            if (jsIconPath == nullptr) {
                WLOGFE("[NAPI]this target jsIconPath is nullptr");
                return;
            }
            napi_value argv[] = { jsIconPath  };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::UpdateSessionIcon", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
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
    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [sessionInfo, jsCallBack](napi_env env, NapiAsyncTask& task, int32_t status) {
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
            napi_value argv[] = { jsSessionInfo };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::TerminateSession", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::PendingSessionToForeground(const SessionInfo& info)
{
    WLOGFI("[NAPI]run PendingSessionToForeground, bundleName = %{public}s, id = %{public}s",
        info.bundleName_.c_str(), info.abilityName_.c_str());
    auto iter = jsCbMap_.find(PENDING_SESSION_TO_FOREGROUND_CB);
    if (iter == jsCbMap_.end()) {
        WLOGFE("[NAPI]fail to find pending session to foreground callback");
        return;
    }
    auto jsCallBack = iter->second;
    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [sessionInfo, jsCallBack](napi_env env, NapiAsyncTask& task, int32_t status) {
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
            napi_value argv[] = { jsSessionInfo };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::PendingSessionToForeground", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::PendingSessionToBackgroundForDelegator(const SessionInfo& info)
{
    WLOGFI("[NAPI]run PendingSessionToBackgroundForDelegator, bundleName = %{public}s, id = %{public}s",
        info.bundleName_.c_str(), info.abilityName_.c_str());
    auto iter = jsCbMap_.find(PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR_CB);
    if (iter == jsCbMap_.end()) {
        WLOGFE("[NAPI]fail to find pending session to background for delegator callback");
        return;
    }
    auto jsCallBack = iter->second;
    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [sessionInfo, jsCallBack](napi_env env, NapiAsyncTask& task, int32_t status) {
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
            napi_value argv[] = { jsSessionInfo };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::PendingSessionToBackgroundForDelegator", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnSystemBarPropertyChange(const std::unordered_map<WindowType, SystemBarProperty>& propertyMap)
{
    WLOGFI("[NAPI]OnSystemBarPropertyChange");
    auto iter = jsCbMap_.find(SYSTEMBAR_PROPERTY_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [jsCallBack, propertyMap, eng = env_](napi_env env, NapiAsyncTask& task, int32_t status) {
            napi_value jsSessionStateObj = CreateJsSystemBarPropertyArrayObject(env, propertyMap);
            if (jsSessionStateObj == nullptr) {
                WLOGFE("[NAPI]jsSessionStateObj is nullptr");
                return;
            }
            napi_value argv[] = { jsSessionStateObj };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::OnSystemBarPropertyChange", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnNeedAvoid(bool status)
{
    WLOGFI("[NAPI]OnNeedAvoid %{public}d", status);
    auto iter = jsCbMap_.find(NEED_AVOID_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [jsCallBack, needAvoid = status, eng = env_](napi_env env, NapiAsyncTask& task, int32_t status) {
            napi_value jsSessionStateObj = CreateJsValue(env, needAvoid);
            napi_value argv[] = { jsSessionStateObj };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::OnNeedAvoid", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnIsCustomAnimationPlaying(bool status)
{
    WLOGFI("[NAPI]OnIsCustomAnimationPlaying %{public}d", status);
    auto iter = jsCbMap_.find(CUSTOM_ANIMATION_PLAYING_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [jsCallBack, isPlaying = status, eng = env_](napi_env env, NapiAsyncTask& task, int32_t status) {
            napi_value jsSessionStateObj = CreateJsValue(env, isPlaying);
            napi_value argv[] = { jsSessionStateObj };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::OnIsCustomAnimationPlaying", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnShowWhenLocked(bool showWhenLocked)
{
    WLOGFI("[NAPI]OnShowWhenLocked %{public}d", showWhenLocked);
    auto iter = jsCbMap_.find(SHOW_WHEN_LOCKED_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [jsCallBack, flag = showWhenLocked, eng = env_](napi_env env, NapiAsyncTask& task, int32_t status) {
            napi_value jsSessionStateObj = CreateJsValue(env, flag);
            napi_value argv[] = { jsSessionStateObj };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::OnShowWhenLocked", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::OnReuqestedOrientationChange(uint32_t orientation)
{
    WLOGFI("[NAPI]OnReuqestedOrientationChange %{public}u", orientation);
    auto iter = jsCbMap_.find(REQUESTED_ORIENTATION_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }
    auto jsCallBack = iter->second;
    if (WINDOW_ORIENTATION_TO_JS_SESSION_MAP.count(static_cast<Orientation>(orientation)) == 0) {
        WLOGFE("[NAPI]failed %{public}u since no this type", orientation);
        return;
    }
    uint32_t value = static_cast<uint32_t>(WINDOW_ORIENTATION_TO_JS_SESSION_MAP.at(
        static_cast<Orientation>(orientation)));
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [jsCallBack, rotation = value, eng = env_](napi_env env, NapiAsyncTask& task, int32_t status) {
            napi_value jsSessionRotationObj = CreateJsValue(env, rotation);
            napi_value argv[] = { jsSessionRotationObj };
            napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
            WLOGFI("[NAPI]change rotation success %{public}u", rotation);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsSceneSession::OnReuqestedOrientationChange", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

napi_value JsSceneSession::OnSetShowRecent(napi_env env, napi_callback_info info)
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is null");
        return NapiGetUndefined(env);
    }
    session->SetShowRecent(true);
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
        WLOGFE("[NAPI]session is null");
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

sptr<SceneSession> JsSceneSession::GetNativeSession() const
{
    return weakSession_.promote();
}
} // namespace OHOS::Rosen
