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
const std::string CHANGE_SESSION_VISIBILITY_WITH_STATUS_BAR = "changeSessionVisibilityWithStatusBar";
const std::string SESSION_STATE_CHANGE_CB = "sessionStateChange";
const std::string BUFFER_AVAILABLE_CHANGE_CB = "bufferAvailableChange";
const std::string SESSION_EVENT_CB = "sessionEvent";
const std::string SESSION_RECT_CHANGE_CB = "sessionRectChange";
const std::string SESSION_PIP_CONTROL_STATUS_CHANGE_CB = "sessionPiPControlStatusChange";
const std::string CREATE_SUB_SESSION_CB = "createSpecificSession";
const std::string BIND_DIALOG_TARGET_CB = "bindDialogTarget";
const std::string RAISE_TO_TOP_CB = "raiseToTop";
const std::string RAISE_TO_TOP_POINT_DOWN_CB = "raiseToTopForPointDown";
const std::string BACK_PRESSED_CB = "backPressed";
const std::string SESSION_FOCUSABLE_CHANGE_CB = "sessionFocusableChange";
const std::string SESSION_TOUCHABLE_CHANGE_CB = "sessionTouchableChange";
const std::string SESSION_TOP_MOST_CHANGE_CB = "sessionTopmostChange";
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
const std::string CUSTOM_ANIMATION_PLAYING_CB = "isCustomAnimationPlaying";
const std::string NEED_DEFAULT_ANIMATION_FLAG_CHANGE_CB = "needDefaultAnimationFlagChange";
const std::string SHOW_WHEN_LOCKED_CB = "sessionShowWhenLockedChange";
const std::string REQUESTED_ORIENTATION_CHANGE_CB = "sessionRequestedOrientationChange";
const std::string RAISE_ABOVE_TARGET_CB = "raiseAboveTarget";
const std::string FORCE_HIDE_CHANGE_CB = "sessionForceHideChange";
const std::string WINDOW_DRAG_HOT_AREA_CB = "windowDragHotArea";
const std::string TOUCH_OUTSIDE_CB = "touchOutside";
const std::string SESSIONINFO_LOCKEDSTATE_CHANGE_CB = "sessionInfoLockedStateChange";
const std::string PREPARE_CLOSE_PIP_SESSION = "prepareClosePiPSession";
const std::string LANDSCAPE_MULTI_WINDOW_CB = "landscapeMultiWindow";
const std::string CONTEXT_TRANSPARENT_CB = "contextTransparent";
const std::string KEYBOARD_GRAVITY_CHANGE_CB = "keyboardGravityChange";
const std::string ADJUST_KEYBOARD_LAYOUT_CB = "adjustKeyboardLayout";
const std::string LAYOUT_FULL_SCREEN_CB = "layoutFullScreenChange";
constexpr int ARG_COUNT_4 = 4;
constexpr int ARG_INDEX_0 = 0;
constexpr int ARG_INDEX_1 = 1;
constexpr int ARG_INDEX_2 = 2;
constexpr int ARG_INDEX_3 = 3;

const std::map<std::string, ListenerFuncType> ListenerFuncMap {
    {PENDING_SCENE_CB,                      ListenerFuncType::PENDING_SCENE_CB},
    {CHANGE_SESSION_VISIBILITY_WITH_STATUS_BAR,
        ListenerFuncType::CHANGE_SESSION_VISIBILITY_WITH_STATUS_BAR},
    {SESSION_STATE_CHANGE_CB,               ListenerFuncType::SESSION_STATE_CHANGE_CB},
    {BUFFER_AVAILABLE_CHANGE_CB,            ListenerFuncType::BUFFER_AVAILABLE_CHANGE_CB},
    {SESSION_EVENT_CB,                      ListenerFuncType::SESSION_EVENT_CB},
    {SESSION_RECT_CHANGE_CB,                ListenerFuncType::SESSION_RECT_CHANGE_CB},
    {SESSION_PIP_CONTROL_STATUS_CHANGE_CB,  ListenerFuncType::SESSION_PIP_CONTROL_STATUS_CHANGE_CB},
    {CREATE_SUB_SESSION_CB,                 ListenerFuncType::CREATE_SUB_SESSION_CB},
    {BIND_DIALOG_TARGET_CB,                 ListenerFuncType::BIND_DIALOG_TARGET_CB},
    {RAISE_TO_TOP_CB,                       ListenerFuncType::RAISE_TO_TOP_CB},
    {RAISE_TO_TOP_POINT_DOWN_CB,            ListenerFuncType::RAISE_TO_TOP_POINT_DOWN_CB},
    {BACK_PRESSED_CB,                       ListenerFuncType::BACK_PRESSED_CB},
    {SESSION_FOCUSABLE_CHANGE_CB,           ListenerFuncType::SESSION_FOCUSABLE_CHANGE_CB},
    {SESSION_TOUCHABLE_CHANGE_CB,           ListenerFuncType::SESSION_TOUCHABLE_CHANGE_CB},
    {SESSION_TOP_MOST_CHANGE_CB,            ListenerFuncType::SESSION_TOP_MOST_CHANGE_CB},
    {CLICK_CB,                              ListenerFuncType::CLICK_CB},
    {TERMINATE_SESSION_CB,                  ListenerFuncType::TERMINATE_SESSION_CB},
    {TERMINATE_SESSION_CB_NEW,              ListenerFuncType::TERMINATE_SESSION_CB_NEW},
    {TERMINATE_SESSION_CB_TOTAL,            ListenerFuncType::TERMINATE_SESSION_CB_TOTAL},
    {SESSION_EXCEPTION_CB,                  ListenerFuncType::SESSION_EXCEPTION_CB},
    {UPDATE_SESSION_LABEL_CB,               ListenerFuncType::UPDATE_SESSION_LABEL_CB},
    {UPDATE_SESSION_ICON_CB,                ListenerFuncType::UPDATE_SESSION_ICON_CB},
    {SYSTEMBAR_PROPERTY_CHANGE_CB,          ListenerFuncType::SYSTEMBAR_PROPERTY_CHANGE_CB},
    {NEED_AVOID_CB,                         ListenerFuncType::NEED_AVOID_CB},
    {PENDING_SESSION_TO_FOREGROUND_CB,      ListenerFuncType::PENDING_SESSION_TO_FOREGROUND_CB},
    {PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR_CB,
        ListenerFuncType::PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR_CB},
    {CUSTOM_ANIMATION_PLAYING_CB,           ListenerFuncType::CUSTOM_ANIMATION_PLAYING_CB},
    {NEED_DEFAULT_ANIMATION_FLAG_CHANGE_CB, ListenerFuncType::NEED_DEFAULT_ANIMATION_FLAG_CHANGE_CB},
    {SHOW_WHEN_LOCKED_CB,                   ListenerFuncType::SHOW_WHEN_LOCKED_CB},
    {REQUESTED_ORIENTATION_CHANGE_CB,       ListenerFuncType::REQUESTED_ORIENTATION_CHANGE_CB},
    {RAISE_ABOVE_TARGET_CB,                 ListenerFuncType::RAISE_ABOVE_TARGET_CB},
    {FORCE_HIDE_CHANGE_CB,                  ListenerFuncType::FORCE_HIDE_CHANGE_CB},
    {WINDOW_DRAG_HOT_AREA_CB,               ListenerFuncType::WINDOW_DRAG_HOT_AREA_CB},
    {TOUCH_OUTSIDE_CB,                      ListenerFuncType::TOUCH_OUTSIDE_CB},
    {SESSIONINFO_LOCKEDSTATE_CHANGE_CB,     ListenerFuncType::SESSIONINFO_LOCKEDSTATE_CHANGE_CB},
    {PREPARE_CLOSE_PIP_SESSION,             ListenerFuncType::PREPARE_CLOSE_PIP_SESSION},
    {LANDSCAPE_MULTI_WINDOW_CB,             ListenerFuncType::LANDSCAPE_MULTI_WINDOW_CB},
    {CONTEXT_TRANSPARENT_CB,                ListenerFuncType::CONTEXT_TRANSPARENT_CB},
    {KEYBOARD_GRAVITY_CHANGE_CB,            ListenerFuncType::KEYBOARD_GRAVITY_CHANGE_CB},
    {ADJUST_KEYBOARD_LAYOUT_CB,             ListenerFuncType::ADJUST_KEYBOARD_LAYOUT_CB},
    {LAYOUT_FULL_SCREEN_CB,                 ListenerFuncType::LAYOUT_FULL_SCREEN_CB},
};
} // namespace

std::map<int32_t, napi_ref> JsSceneSession::jsSceneSessionMap_;

napi_value CreateJsPiPControlStatusObject(napi_env env, PiPControlStatusInfo controlStatusInfo)
{
    TLOGI(WmsLogTag::WMS_PIP, "called");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "objValue is nullptr");
        return NapiGetUndefined(env);
    }
    uint32_t controlType = static_cast<uint32_t>(controlStatusInfo.controlType);
    int32_t status = static_cast<int32_t>(controlStatusInfo.status);
    napi_set_named_property(env, objValue, "controlType", CreateJsValue(env, controlType));
    napi_set_named_property(env, objValue, "status", CreateJsValue(env, status));
    return objValue;
}

napi_value CreateJsPiPControlEnableObject(napi_env env, PiPControlEnableInfo controlEnableInfo)
{
    TLOGI(WmsLogTag::WMS_PIP, "called");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "objValue is nullptr");
        return NapiGetUndefined(env);
    }
    uint32_t controlType = static_cast<uint32_t>(controlEnableInfo.controlType);
    int32_t enabled = static_cast<int32_t>(controlEnableInfo.enabled);
    napi_set_named_property(env, objValue, "controlType", CreateJsValue(env, controlType));
    napi_set_named_property(env, objValue, "enabled", CreateJsValue(env, enabled));
    return objValue;
}

static napi_value CreatePipTemplateInfo(napi_env env, const sptr<SceneSession>& session)
{
    TLOGI(WmsLogTag::WMS_PIP, "called");
    napi_value pipTemplateInfoValue = nullptr;
    napi_create_object(env, &pipTemplateInfoValue);
    napi_set_named_property(env, pipTemplateInfoValue, "pipTemplateType",
        CreateJsValue(env, session->GetPiPTemplateInfo().pipTemplateType));
    napi_set_named_property(env, pipTemplateInfoValue, "priority",
        CreateJsValue(env, session->GetPiPTemplateInfo().priority));
    napi_value controlArrayValue = nullptr;
    std::vector<std::uint32_t> controlGroups = session->GetPiPTemplateInfo().controlGroup;
    napi_create_array_with_length(env, controlGroups.size(), &controlArrayValue);
    auto index = 0;
    for (const auto& controlGroup : controlGroups) {
        napi_set_element(env, controlArrayValue, index++, CreateJsValue(env, controlGroup));
    }
    napi_set_named_property(env, pipTemplateInfoValue, "controlGroup", controlArrayValue);
    napi_value controlStatusArrayValue = nullptr;
    std::vector<PiPControlStatusInfo> controlStatusInfoList = session->GetPiPTemplateInfo().pipControlStatusInfoList;
    napi_create_array_with_length(env, controlStatusInfoList.size(), &controlStatusArrayValue);
    auto controlStatusIndex = 0;
    for (const auto& controlStatus : controlStatusInfoList) {
        napi_set_element(env, controlStatusArrayValue, controlStatusIndex++,
            CreateJsPiPControlStatusObject(env, controlStatus));
    }
    napi_set_named_property(env, pipTemplateInfoValue, "pipControlStatusInfoList", controlStatusArrayValue);
    napi_value controlEnableArrayValue = nullptr;
    std::vector<PiPControlEnableInfo> controlEnableInfoList = session->GetPiPTemplateInfo().pipControlEnableInfoList;
    napi_create_array_with_length(env, controlEnableInfoList.size(), &controlEnableArrayValue);
    auto controlEnableIndex = 0;
    for (const auto& controlEnableInfo : controlEnableInfoList) {
        napi_set_element(env, controlEnableArrayValue, controlEnableIndex++,
            CreateJsPiPControlEnableObject(env, controlEnableInfo));
    }
    napi_set_named_property(env, pipTemplateInfoValue, "pipControlEnableInfoList", controlEnableArrayValue);
    return pipTemplateInfoValue;
}

napi_value JsSceneSession::Create(napi_env env, const sptr<SceneSession>& session)
{
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
    napi_set_named_property(env, objValue, "pipTemplateInfo", CreatePipTemplateInfo(env, session));
    napi_set_named_property(env, objValue, "keyboardGravity",
        CreateJsValue(env, static_cast<int32_t>(session->GetKeyboardGravity())));
    napi_set_named_property(env, objValue, "isTopmost",
        CreateJsValue(env, static_cast<int32_t>(session->IsTopmost())));
    napi_set_named_property(env, objValue, "subWindowModalType",
        CreateJsValue(env, static_cast<int32_t>(session->GetSubWindowModalType())));

    const char* moduleName = "JsSceneSession";
    BindNativeMethod(env, objValue, moduleName);
    napi_ref jsRef = nullptr;
    napi_status status = napi_create_reference(env, objValue, 1, &jsRef);
    if (status != napi_ok) {
        WLOGFE("do not get ref ");
    }
    jsSceneSessionMap_[session->GetPersistentId()] = jsRef;
    BindNativeFunction(env, objValue, "updateSizeChangeReason", moduleName, JsSceneSession::UpdateSizeChangeReason);
    return objValue;
}

void JsSceneSession::BindNativeMethod(napi_env env, napi_value objValue, const char* moduleName)
{
    BindNativeFunction(env, objValue, "on", moduleName, JsSceneSession::RegisterCallback);
    BindNativeFunction(env, objValue, "updateNativeVisibility", moduleName, JsSceneSession::UpdateNativeVisibility);
    BindNativeFunction(env, objValue, "setShowRecent", moduleName, JsSceneSession::SetShowRecent);
    BindNativeFunction(env, objValue, "setZOrder", moduleName, JsSceneSession::SetZOrder);
    BindNativeFunction(env, objValue, "setTouchable", moduleName, JsSceneSession::SetTouchable);
    BindNativeFunction(env, objValue, "setSystemActive", moduleName, JsSceneSession::SetSystemActive);
    BindNativeFunction(env, objValue, "setPrivacyMode", moduleName, JsSceneSession::SetPrivacyMode);
    BindNativeFunction(env, objValue, "setSystemSceneOcclusionAlpha",
        moduleName, JsSceneSession::SetSystemSceneOcclusionAlpha);
    BindNativeFunction(env, objValue, "setFloatingScale", moduleName, JsSceneSession::SetFloatingScale);
    BindNativeFunction(env, objValue, "setFocusable", moduleName, JsSceneSession::SetFocusable);
    BindNativeFunction(env, objValue, "setSystemSceneBlockingFocus", moduleName,
        JsSceneSession::SetSystemSceneBlockingFocus);
    BindNativeFunction(env, objValue, "setScale", moduleName, JsSceneSession::SetScale);
    BindNativeFunction(env, objValue, "setWindowLastSafeRect", moduleName, JsSceneSession::SetWindowLastSafeRect);
    BindNativeFunction(env, objValue, "requestHideKeyboard", moduleName, JsSceneSession::RequestHideKeyboard);
    BindNativeFunction(env, objValue, "setSCBKeepKeyboard", moduleName, JsSceneSession::SetSCBKeepKeyboard);
    BindNativeFunction(env, objValue, "setOffset", moduleName, JsSceneSession::SetOffset);
    BindNativeFunction(env, objValue, "setExitSplitOnBackground", moduleName,
        JsSceneSession::SetExitSplitOnBackground);
    BindNativeFunction(env, objValue, "setWaterMarkFlag", moduleName, JsSceneSession::SetWaterMarkFlag);
    BindNativeFunction(env, objValue, "setPipActionEvent", moduleName, JsSceneSession::SetPipActionEvent);
    BindNativeFunction(env, objValue, "setPiPControlEvent", moduleName, JsSceneSession::SetPiPControlEvent);
    BindNativeFunction(env, objValue, "notifyDisplayStatusBarTemporarily", moduleName,
        JsSceneSession::NotifyDisplayStatusBarTemporarily);
    BindNativeFunction(env, objValue, "setTemporarilyShowWhenLocked", moduleName,
        JsSceneSession::SetTemporarilyShowWhenLocked);
    BindNativeFunction(env, objValue, "setSkipDraw", moduleName,
        JsSceneSession::SetSkipDraw);
    BindNativeFunction(env, objValue, "openKeyboardSyncTransaction", moduleName,
        JsSceneSession::OpenKeyboardSyncTransaction);
    BindNativeFunction(env, objValue, "closeKeyboardSyncTransaction", moduleName,
        JsSceneSession::CloseKeyboardSyncTransaction);
    BindNativeFunction(env, objValue, "setSkipSelfWhenShowOnVirtualScreen", moduleName,
        JsSceneSession::SetSkipSelfWhenShowOnVirtualScreen);
    BindNativeFunction(env, objValue, "setCompatibleModeInPc", moduleName,
        JsSceneSession::SetCompatibleModeInPc);
    BindNativeFunction(env, objValue, "setBlankFlag", moduleName, JsSceneSession::SetBlankFlag);
    BindNativeFunction(env, objValue, "setBufferAvailableCallbackEnable", moduleName,
        JsSceneSession::SetBufferAvailableCallbackEnable);
    BindNativeFunction(env, objValue, "isDeviceWakeupByApplication", moduleName,
        JsSceneSession::IsDeviceWakeupByApplication);
    BindNativeFunction(env, objValue, "setIsPcAppInPad", moduleName,
        JsSceneSession::SetIsPcAppInPad);
    BindNativeFunction(env, objValue, "setStartingWindowExitAnimationFlag", moduleName,
        JsSceneSession::SetStartingWindowExitAnimationFlag);
}

JsSceneSession::JsSceneSession(napi_env env, const sptr<SceneSession>& session)
    : env_(env), weakSession_(session)
{
    InitListenerFuncs();
    sptr<SceneSession::SessionChangeCallback> sessionchangeCallback = new (std::nothrow)
        SceneSession::SessionChangeCallback();
    if (sessionchangeCallback != nullptr) {
        if (session != nullptr) {
            session->RegisterSessionChangeCallback(sessionchangeCallback);
        }
        sessionchangeCallback->clearCallbackFunc_ = [this](bool needRemove, int32_t persistentId) {
            this->ClearCbMap(needRemove, persistentId);
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

void JsSceneSession::InitListenerFuncs()
{
}

void JsSceneSession::ProcessPendingSceneSessionActivationRegister()
{
    NotifyPendingSessionActivationFunc func = [this](SessionInfo& info) {
        this->PendingSessionActivation(info);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr");
        return;
    }
    session->SetPendingSessionActivationEventListener(func);
    TLOGD(WmsLogTag::WMS_LIFE, "success");
}

void JsSceneSession::ProcessWindowDragHotAreaRegister()
{
    WLOGFI("[NAPI]ProcessWindowDragHotAreaRegister");
    NotifyWindowDragHotAreaFunc func = [this](uint32_t type, const SizeChangeReason& reason) {
        this->OnWindowDragHotArea(type, reason);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetWindowDragHotAreaListener(func);
}

void JsSceneSession::OnWindowDragHotArea(uint32_t type, const SizeChangeReason& reason)
{
    WLOGFI("[NAPI]OnWindowDragHotArea");

    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    WSRect rect = session->GetSessionTargetRect();
    auto task = [jsCallBack = GetJSCallback(WINDOW_DRAG_HOT_AREA_CB), env = env_, type, reason, rect]() {
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
        napi_value argv[] = {jsHotAreaType, jsHotAreaReason, jsHotAreaRect};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnWindowDragHotArea");
}

void JsSceneSession::ProcessSessionInfoLockedStateChangeRegister()
{
    NotifySessionInfoLockedStateChangeFunc func = [this](bool lockedState) {
        this->OnSessionInfoLockedStateChange(lockedState);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetSessionInfoLockedStateChangeListener(func);
    WLOGFD("ProcessSessionInfoLockedStateChangeRegister success");
}

void JsSceneSession::ProcessLandscapeMultiWindowRegister()
{
    TLOGD(WmsLogTag::WMS_MULTI_WINDOW, "ProcessLandscapeMultiWindowRegister");
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        TLOGE(WmsLogTag::WMS_MULTI_WINDOW, "sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onSetLandscapeMultiWindowFunc_ = [this](bool isLandscapeMultiWindow) {
        this->SetLandscapeMultiWindow(isLandscapeMultiWindow);
    };
    TLOGD(WmsLogTag::WMS_MULTI_WINDOW, "ProcessLandscapeMultiWindowRegister success");
}

void JsSceneSession::SetLandscapeMultiWindow(bool isLandscapeMultiWindow)
{
    TLOGI(WmsLogTag::WMS_MULTI_WINDOW, "[NAPI]SetLandScapeMultiWindow, isLandscapeMultiWindow: %{public}u",
        isLandscapeMultiWindow);

    auto task = [isLandscapeMultiWindow, jsCallBack = GetJSCallback(LANDSCAPE_MULTI_WINDOW_CB), env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionLandscapeMultiWindowObj = CreateJsValue(env, isLandscapeMultiWindow);
        napi_value argv[] = {jsSessionLandscapeMultiWindowObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task,
        "SetLandscapeMultiWindow, isLandscapeMultiWindow:" + std::to_string(isLandscapeMultiWindow));
}

void JsSceneSession::ProcessKeyboardGravityChangeRegister()
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    OnKeyboardGravityChange(session->GetKeyboardGravity());
    
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onKeyboardGravityChange_ = [this](SessionGravity gravity) {
        this->OnKeyboardGravityChange(gravity);
    };
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Register success");
}

void JsSceneSession::OnKeyboardGravityChange(SessionGravity gravity)
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "[NAPI] gravity: %{public}u", gravity);
    auto task = [gravity, jsCallBack = GetJSCallback(KEYBOARD_GRAVITY_CHANGE_CB), env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value gravityObj = CreateJsValue(env, gravity);
        napi_value argv[] = {gravityObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Napi call gravity success, gravity: %{public}u", gravity);
    };
    taskScheduler_->PostMainThreadTask(task, "OnKeyboardGravityChange: gravity " +
        std::to_string(static_cast<int>(gravity)));
}

void JsSceneSession::ProcessAdjustKeyboardLayoutRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onAdjustKeyboardLayout_ = [this](const KeyboardLayoutParams& params) {
        this->OnAdjustKeyboardLayout(params);
    };
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Register success");
}

void JsSceneSession::ProcessLayoutFullScreenChangeRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onLayoutFullScreenChangeFunc_ = [this](bool isLayoutFullScreen) {
        this->OnLayoutFullScreenChange(isLayoutFullScreen);
    };
    TLOGI(WmsLogTag::WMS_LAYOUT, "Register success");
}

void JsSceneSession::OnLayoutFullScreenChange(bool isLayoutFullScreen)
{
    auto task = [isLayoutFullScreen, jsCallBack = GetJSCallback(LAYOUT_FULL_SCREEN_CB), env = env_]() {
        if (!jsCallBack) {
            TLOGE(WmsLogTag::WMS_LAYOUT, "OnLayoutFullScreenChange jsCallBack is nullptr");
            return;
        }
        napi_value paramsObj = CreateJsValue(env, isLayoutFullScreen);
        napi_value argv[] = {paramsObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnLayoutFullScreenChange");
}

void JsSceneSession::OnAdjustKeyboardLayout(const KeyboardLayoutParams& params)
{
    auto task = [params, jsCallBack = GetJSCallback(ADJUST_KEYBOARD_LAYOUT_CB), env = env_]() {
        if (!jsCallBack) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "OnAdjustKeyboardLayout jsCallBack is nullptr");
            return;
        }
        napi_value keyboardLayoutParamsObj = CreateJsKeyboardLayoutParams(env, params);
        if (keyboardLayoutParamsObj == nullptr) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "OnAdjustKeyboardLayout this keyboard layout params obj is nullptr");
        }
        napi_value argv[] = {keyboardLayoutParamsObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnAdjustKeyboardLayout");
}

void JsSceneSession::OnSessionInfoLockedStateChange(bool lockedState)
{
    WLOGFI("[NAPI]OnSessionInfoLockedStateChange, state: %{public}u", lockedState);

    auto task = [lockedState, jsCallBack = GetJSCallback(SESSIONINFO_LOCKEDSTATE_CHANGE_CB), env = env_]() {
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
        TLOGI(WmsLogTag::WMS_LIFE, "clear callbackMap with persistent id, %{public}d", persistentId);
        {
            HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsSceneSession clear jsCbMap");
            std::unique_lock<std::shared_mutex> lock(jsCbMapMutex_);
            jsCbMap_.clear();
        }
        auto iter = jsSceneSessionMap_.find(persistentId);
        if (iter != jsSceneSessionMap_.end()) {
            napi_delete_reference(env_, iter->second);
            jsSceneSessionMap_.erase(iter);
        } else {
            TLOGE(WmsLogTag::WMS_LIFE, "deleteRef failed , %{public}d", persistentId);
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
    sessionchangeCallback->onWindowAnimationFlagChange_ = [this](bool isNeedDefaultAnimationFlag) {
        this->OnDefaultAnimationFlagChange(isNeedDefaultAnimationFlag);
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

    auto task = [isNeedDefaultAnimationFlag,
        jsCallBack = GetJSCallback(NEED_DEFAULT_ANIMATION_FLAG_CHANGE_CB), env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionDefaultAnimationFlagObj = CreateJsValue(env, isNeedDefaultAnimationFlag);
        napi_value argv[] = {jsSessionDefaultAnimationFlagObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    std::string info = "OnDefaultAnimationFlagChange, flag:" + std::to_string(isNeedDefaultAnimationFlag);
    taskScheduler_->PostMainThreadTask(task, info);
}

void JsSceneSession::ProcessChangeSessionVisibilityWithStatusBarRegister()
{
    NotifyChangeSessionVisibilityWithStatusBarFunc func = [this](SessionInfo& info, bool visible) {
        this->ChangeSessionVisibilityWithStatusBar(info, visible);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetChangeSessionVisibilityWithStatusBarEventListener(func);
    WLOGFD("ProcessChangeSessionVisibilityWithStatusBarRegister success");
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

void JsSceneSession::ProcessBufferAvailableChangeRegister()
{
    NotifyBufferAvailableChangeFunc func = [this](const bool isAvailable) {
        this->OnBufferAvailableChange(isAvailable);
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
    NotifyCreateSubSessionFunc func = [this](const sptr<SceneSession>& sceneSession) {
        this->OnCreateSubSession(sceneSession);
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
        TLOGE(WmsLogTag::WMS_DIALOG, "sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onBindDialogTarget_ = [this](const sptr<SceneSession>& sceneSession) {
        this->OnBindDialogTarget(sceneSession);
    };
    TLOGD(WmsLogTag::WMS_DIALOG, "ProcessBindDialogTargetRegister success");
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

void JsSceneSession::ProcessSessionPiPControlStatusChangeRegister()
{
    NotifySessionPiPControlStatusChangeFunc func = [this](WsPiPControlType controlType, WsPiPControlStatus status) {
        this->OnSessionPiPControlStatusChange(controlType, status);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "session is nullptr");
        return;
    }
    session->SetSessionPiPControlStatusChangeCallback(func);
    TLOGI(WmsLogTag::WMS_PIP, "register success");
}

void JsSceneSession::ProcessRaiseToTopRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onRaiseToTop_ = [this] {
        this->OnRaiseToTop();
    };
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
    sessionchangeCallback->onRaiseAboveTarget_ = [this](int32_t subWindowId) {
        this->OnRaiseAboveTarget(subWindowId);
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
    sessionchangeCallback->OnSessionEvent_ = [this](uint32_t eventId, const SessionEventParam& param) {
        this->OnSessionEvent(eventId, param);
    };
    WLOGFD("ProcessSessionEventRegister success");
}

void JsSceneSession::ProcessTerminateSessionRegister()
{
    TLOGD(WmsLogTag::WMS_LIFE, "begin");
    NotifyTerminateSessionFunc func = [this](const SessionInfo& info) {
        this->TerminateSession(info);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr");
        return;
    }
    session->SetTerminateSessionListener(func);
    TLOGD(WmsLogTag::WMS_LIFE, "success");
}

void JsSceneSession::ProcessTerminateSessionRegisterNew()
{
    TLOGD(WmsLogTag::WMS_LIFE, "begin");
    NotifyTerminateSessionFuncNew func = [this](const SessionInfo& info, bool needStartCaller, bool isFromBroker) {
        this->TerminateSessionNew(info, needStartCaller, isFromBroker);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr");
        return;
    }
    session->SetTerminateSessionListenerNew(func);
    TLOGD(WmsLogTag::WMS_LIFE, "success");
}

void JsSceneSession::ProcessTerminateSessionRegisterTotal()
{
    TLOGD(WmsLogTag::WMS_LIFE, "begin");
    NotifyTerminateSessionFuncTotal func = [this](const SessionInfo& info, TerminateType terminateType) {
        this->TerminateSessionTotal(info, terminateType);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr");
        return;
    }
    session->SetTerminateSessionListenerTotal(func);
    TLOGD(WmsLogTag::WMS_LIFE, "success");
}

void JsSceneSession::ProcessPendingSessionToForegroundRegister()
{
    TLOGD(WmsLogTag::WMS_LIFE, "begin");
    NotifyPendingSessionToForegroundFunc func = [this](const SessionInfo& info) {
        this->PendingSessionToForeground(info);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr");
        return;
    }
    session->SetPendingSessionToForegroundListener(func);
    TLOGD(WmsLogTag::WMS_LIFE, "success");
}

void JsSceneSession::ProcessPendingSessionToBackgroundForDelegatorRegister()
{
    TLOGD(WmsLogTag::WMS_LIFE, "begin");
    auto weak = weak_from_this();
    NotifyPendingSessionToBackgroundForDelegatorFunc func = [this](const SessionInfo& info) {
        this->PendingSessionToBackgroundForDelegator(info);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr");
        return;
    }
    session->SetPendingSessionToBackgroundForDelegatorListener(func);
    TLOGD(WmsLogTag::WMS_LIFE, "success");
}

void JsSceneSession::ProcessSessionExceptionRegister()
{
    WLOGFD("begin to run ProcessSessionExceptionRegister");
    NotifySessionExceptionFunc func = [this](const SessionInfo& info, bool needRemoveSession) {
        this->OnSessionException(info, needRemoveSession);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetSessionExceptionListener(func, true);
    WLOGFD("ProcessSessionExceptionRegister success");
}

void JsSceneSession::ProcessSessionTopmostChangeRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onSessionTopmostChange_ = [this](bool topmost) {
        this->OnSessionTopmostChange(topmost);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "session is nullptr");
        return;
    }
    sessionchangeCallback->onSessionTopmostChange_(session->IsTopmost());
    TLOGD(WmsLogTag::WMS_LAYOUT, "ProcessSessionTopmostChangeRegister success");
}

void JsSceneSession::ProcessSessionFocusableChangeRegister()
{
    NotifySessionFocusableChangeFunc func = [this](bool isFocusable) {
        this->OnSessionFocusableChange(isFocusable);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[WMSComm]session is nullptr");
        return;
    }
    session->SetSessionFocusableChangeListener(func);
    TLOGD(WmsLogTag::WMS_FOCUS, "ProcessSessionFocusableChangeRegister success");
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

void JsSceneSession::ProcessContextTransparentRegister()
{
    NotifyContextTransparentFunc func = [this]() {
        this->OnContextTransparent();
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    session->SetContextTransparentFunc(func);
    WLOGFD("ProcessContextTransparentRegister success");
}

void JsSceneSession::OnSessionEvent(uint32_t eventId, const SessionEventParam& param)
{
    WLOGFI("[NAPI]OnSessionEvent, eventId: %{public}d", eventId);

    auto task = [eventId, param, jsCallBack = GetJSCallback(SESSION_EVENT_CB), env = env_]() {
        if (jsCallBack == nullptr) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionStateObj = CreateJsValue(env, eventId);
        napi_value jsSessionParamObj = CreateJsSessionEventParam(env, param);
        napi_value argv[] = {jsSessionStateObj, jsSessionParamObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    taskScheduler_->PostMainThreadTask(task, "OnSessionEvent, EventId:" + std::to_string(eventId));
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
        TLOGE(WmsLogTag::WMS_IMMS, "sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->OnSystemBarPropertyChange_ = [this](
        const std::unordered_map<WindowType, SystemBarProperty>& propertyMap) {
        this->OnSystemBarPropertyChange(propertyMap);
    };
    TLOGD(WmsLogTag::WMS_IMMS, "ProcessSystemBarPropertyChangeRegister success");
}

void JsSceneSession::ProcessNeedAvoidRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->OnNeedAvoid_ = [this](bool status) {
        this->OnNeedAvoid(status);
    };
    TLOGD(WmsLogTag::WMS_IMMS, "ProcessNeedAvoidRegister success");
}

void JsSceneSession::ProcessIsCustomAnimationPlaying()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onIsCustomAnimationPlaying_ = [this](bool status) {
        this->OnIsCustomAnimationPlaying(status);
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
    sessionchangeCallback->OnShowWhenLocked_ = [this](bool showWhenLocked) {
        this->OnShowWhenLocked(showWhenLocked);
    };
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    sessionchangeCallback->OnShowWhenLocked_(session->GetShowWhenLockedFlagValue());
    WLOGFD("ProcessShowWhenLockedRegister success");
}

void JsSceneSession::ProcessRequestedOrientationChange()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        WLOGFE("sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->OnRequestedOrientationChange_ = [this](uint32_t orientation) {
        this->OnReuqestedOrientationChange(orientation);
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
    sessionchangeCallback->OnForceHideChange_ = [this](bool hide) {
        this->OnForceHideChange(hide);
    };
    WLOGFD("ProcessForceHideChangeRegister success");
}

void JsSceneSession::OnForceHideChange(bool hide)
{
    WLOGFI("[NAPI]OnForceHideChange, hide: %{public}u", hide);

    auto task = [hide, jsCallBack = GetJSCallback(FORCE_HIDE_CHANGE_CB), env = env_]() {
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
    sessionchangeCallback->OnTouchOutside_ = [this] { this->OnTouchOutside(); };
    WLOGFD("ProcessTouchOutsideRegister success");
}

void JsSceneSession::OnTouchOutside()
{
    WLOGFI("[NAPI]OnTouchOutside");
    auto task = [jsCallBack = GetJSCallback(TOUCH_OUTSIDE_CB), env = env_]() {
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
    if (Session::IsScbCoreEnabled()) {
        return nullptr;
    }
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

napi_value JsSceneSession::SetSkipSelfWhenShowOnVirtualScreen(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetSkipSelfWhenShowOnVirtualScreen");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetSkipSelfWhenShowOnVirtualScreen(env, info) : nullptr;
}

napi_value JsSceneSession::SetSystemSceneOcclusionAlpha(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetSystemSceneOcclusionAlpha");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetSystemSceneOcclusionAlpha(env, info) : nullptr;
}

napi_value JsSceneSession::SetFocusable(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "[NAPI]SetFocusable");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetFocusable(env, info) : nullptr;
}

napi_value JsSceneSession::SetSystemSceneBlockingFocus(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "[NAPI]SetSystemSceneBlockingFocus");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetSystemSceneBlockingFocus(env, info) : nullptr;
}

napi_value JsSceneSession::UpdateSizeChangeReason(napi_env env, napi_callback_info info)
{
    WLOGD("[NAPI]UpdateSizeChangeReason");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnUpdateSizeChangeReason(env, info) : nullptr;
}

napi_value JsSceneSession::OpenKeyboardSyncTransaction(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_KEYBOARD, "[NAPI]OpenKeyboardSyncTransaction");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnOpenKeyboardSyncTransaction(env, info) : nullptr;
}

napi_value JsSceneSession::CloseKeyboardSyncTransaction(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_KEYBOARD, "[NAPI]CloseKeyboardSyncTransaction");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnCloseKeyboardSyncTransaction(env, info) : nullptr;
}

napi_value JsSceneSession::SetShowRecent(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetShowRecent");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetShowRecent(env, info) : nullptr;
}

napi_value JsSceneSession::SetZOrder(napi_env env, napi_callback_info info)
{
    if (Session::IsScbCoreEnabled()) {
        return nullptr;
    }
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

napi_value JsSceneSession::SetOffset(napi_env env, napi_callback_info info)
{
    if (Session::IsScbCoreEnabled()) {
        return nullptr;
    }
    WLOGI("[NAPI]SetOffset");
    JsSceneSession *me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetOffset(env, info) : nullptr;
}

napi_value JsSceneSession::SetExitSplitOnBackground(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_MULTI_WINDOW, "[NAPI] In");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetExitSplitOnBackground(env, info) : nullptr;
}

napi_value JsSceneSession::SetWaterMarkFlag(napi_env env, napi_callback_info info)
{
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetWaterMarkFlag(env, info) : nullptr;
}

napi_value JsSceneSession::SetPipActionEvent(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_PIP, "[NAPI]SetPipActionEvent");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetPipActionEvent(env, info) : nullptr;
}

napi_value JsSceneSession::SetPiPControlEvent(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_PIP, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetPiPControlEvent(env, info) : nullptr;
}

napi_value JsSceneSession::NotifyDisplayStatusBarTemporarily(napi_env env, napi_callback_info info)
{
    JsSceneSession *me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnNotifyDisplayStatusBarTemporarily(env, info) : nullptr;
}

napi_value JsSceneSession::SetTemporarilyShowWhenLocked(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_SCB, "[NAPI]SetTemporarilyShowWhenLocked");
    JsSceneSession *me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetTemporarilyShowWhenLocked(env, info) : nullptr;
}

napi_value JsSceneSession::SetSkipDraw(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_SCB, "[NAPI]SetSkipDraw");
    JsSceneSession *me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetSkipDraw(env, info) : nullptr;
}

napi_value JsSceneSession::SetCompatibleModeInPc(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_SCB, "[NAPI]SetCompatibleModeInPc");
    JsSceneSession *me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetCompatibleModeInPc(env, info) : nullptr;
}

napi_value JsSceneSession::SetBlankFlag(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_SCB, "[NAPI]SetBlankFlag");
    JsSceneSession *me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetBlankFlag(env, info) : nullptr;
}

napi_value JsSceneSession::SetBufferAvailableCallbackEnable(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_SCB, "[NAPI]SetBufferAvailableCallbackEnable");
    JsSceneSession *me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetBufferAvailableCallbackEnable(env, info) : nullptr;
}

napi_value JsSceneSession::SetIsPcAppInPad(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_SCB, "[NAPI]called");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetIsPcAppInPad(env, info) : nullptr;
}

napi_value JsSceneSession::IsDeviceWakeupByApplication(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_SCB, "[NAPI]called");
    JsSceneSession *me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnIsDeviceWakeupByApplication(env, info) : nullptr;
}

napi_value JsSceneSession::SetStartingWindowExitAnimationFlag(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_SCB, "[NAPI]called");
    JsSceneSession *me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetStartingWindowExitAnimationFlag(env, info) : nullptr;
}

bool JsSceneSession::IsCallbackRegistered(napi_env env, const std::string& type, napi_value jsListenerObject)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsSceneSession::IsCallbackRegistered[%s]", type.c_str());
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

napi_value JsSceneSession::OnRegisterCallback(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 2) { // 2: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        return NapiGetUndefined(env);
    }
    std::string cbType;
    if (!ConvertFromJsValue(env, argv[0], cbType)) {
        WLOGFE("[NAPI]Failed to convert parameter to callbackType");
        return NapiGetUndefined(env);
    }
    napi_value value = argv[1];
    if (value == nullptr || !NapiIsCallable(env, value)) {
        WLOGFE("[NAPI]Invalid argument");
        return NapiGetUndefined(env);
    }
    auto iterFunctionType = ListenerFuncMap.find(cbType);
    if (iterFunctionType == ListenerFuncMap.end()) {
        WLOGFE("[NAPI]callback type is not supported, type = %{public}s", cbType.c_str());
        return NapiGetUndefined(env);
    }
    ListenerFuncType listenerFuncType = iterFunctionType->second;
    if (IsCallbackRegistered(env, cbType, value)) {
        WLOGFE("[NAPI]callback is registered, type = %{public}s", cbType.c_str());
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is nullptr");
        return NapiGetUndefined(env);
    }

    std::shared_ptr<NativeReference> callbackRef;
    napi_ref result = nullptr;
    napi_create_reference(env, value, 1, &result);
    callbackRef.reset(reinterpret_cast<NativeReference*>(result));
    {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsSceneSession set jsCbMap[%s]", cbType.c_str());
        std::unique_lock<std::shared_mutex> lock(jsCbMapMutex_);
        jsCbMap_[cbType] = callbackRef;
    }
    ProcessRegisterCallback(listenerFuncType);
    WLOGFD("[NAPI]Register end, type = %{public}s", cbType.c_str());
    return NapiGetUndefined(env);
}

void JsSceneSession::ProcessRegisterCallback(ListenerFuncType listenerFuncType)
{
    switch (static_cast<uint32_t>(listenerFuncType)) {
        case static_cast<uint32_t>(ListenerFuncType::PENDING_SCENE_CB):
            ProcessPendingSceneSessionActivationRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::CHANGE_SESSION_VISIBILITY_WITH_STATUS_BAR):
            ProcessChangeSessionVisibilityWithStatusBarRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSION_STATE_CHANGE_CB):
            ProcessSessionStateChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::BUFFER_AVAILABLE_CHANGE_CB):
            ProcessBufferAvailableChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSION_EVENT_CB):
            ProcessSessionEventRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSION_RECT_CHANGE_CB):
            ProcessSessionRectChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSION_PIP_CONTROL_STATUS_CHANGE_CB):
            ProcessSessionPiPControlStatusChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::CREATE_SUB_SESSION_CB):
            ProcessCreateSubSessionRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::BIND_DIALOG_TARGET_CB):
            ProcessBindDialogTargetRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::RAISE_TO_TOP_CB):
            ProcessRaiseToTopRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::RAISE_TO_TOP_POINT_DOWN_CB):
            ProcessRaiseToTopForPointDownRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::BACK_PRESSED_CB):
            ProcessBackPressedRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSION_FOCUSABLE_CHANGE_CB):
            ProcessSessionFocusableChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSION_TOUCHABLE_CHANGE_CB):
            ProcessSessionTouchableChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSION_TOP_MOST_CHANGE_CB):
            ProcessSessionTopmostChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::CLICK_CB):
            ProcessClickRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::TERMINATE_SESSION_CB):
            ProcessTerminateSessionRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::TERMINATE_SESSION_CB_NEW):
            ProcessTerminateSessionRegisterNew();
            break;
        case static_cast<uint32_t>(ListenerFuncType::TERMINATE_SESSION_CB_TOTAL):
            ProcessTerminateSessionRegisterTotal();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSION_EXCEPTION_CB):
            ProcessSessionExceptionRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::UPDATE_SESSION_LABEL_CB):
            ProcessUpdateSessionLabelRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::UPDATE_SESSION_ICON_CB):
            ProcessUpdateSessionIconRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SYSTEMBAR_PROPERTY_CHANGE_CB):
            ProcessSystemBarPropertyChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::NEED_AVOID_CB):
            ProcessNeedAvoidRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::PENDING_SESSION_TO_FOREGROUND_CB):
            ProcessPendingSessionToForegroundRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR_CB):
            ProcessPendingSessionToBackgroundForDelegatorRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::CUSTOM_ANIMATION_PLAYING_CB):
            ProcessIsCustomAnimationPlaying();
            break;
        case static_cast<uint32_t>(ListenerFuncType::NEED_DEFAULT_ANIMATION_FLAG_CHANGE_CB):
            ProcessSessionDefaultAnimationFlagChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SHOW_WHEN_LOCKED_CB):
            ProcessShowWhenLockedRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::REQUESTED_ORIENTATION_CHANGE_CB):
            ProcessRequestedOrientationChange();
            break;
        case static_cast<uint32_t>(ListenerFuncType::RAISE_ABOVE_TARGET_CB):
            ProcessRaiseAboveTargetRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::FORCE_HIDE_CHANGE_CB):
            ProcessForceHideChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::WINDOW_DRAG_HOT_AREA_CB):
            ProcessWindowDragHotAreaRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::TOUCH_OUTSIDE_CB):
            ProcessTouchOutsideRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::SESSIONINFO_LOCKEDSTATE_CHANGE_CB):
            ProcessSessionInfoLockedStateChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::PREPARE_CLOSE_PIP_SESSION):
            ProcessPrepareClosePiPSessionRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::LANDSCAPE_MULTI_WINDOW_CB):
            ProcessLandscapeMultiWindowRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::CONTEXT_TRANSPARENT_CB):
            ProcessContextTransparentRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::KEYBOARD_GRAVITY_CHANGE_CB):
            ProcessKeyboardGravityChangeRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::ADJUST_KEYBOARD_LAYOUT_CB):
            ProcessAdjustKeyboardLayoutRegister();
            break;
        case static_cast<uint32_t>(ListenerFuncType::LAYOUT_FULL_SCREEN_CB):
            ProcessLayoutFullScreenChangeRegister();
            break;
        default:
            break;
    }
}

napi_value JsSceneSession::OnUpdateNativeVisibility(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        return NapiGetUndefined(env);
    }
    bool visible = false;
    if (!ConvertFromJsValue(env, argv[0], visible)) {
        WLOGFE("[NAPI]Failed to convert parameter to bool");
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("[NAPI]session is nullptr");
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
    TLOGD(WmsLogTag::WMS_FOCUS, "[NAPI]OnSetFocusable end");
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

napi_value JsSceneSession::OnOpenKeyboardSyncTransaction(napi_env env, napi_callback_info info)
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "[NAPI] session is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    session->OpenKeyboardSyncTransaction();
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnCloseKeyboardSyncTransaction(napi_env env, napi_callback_info info)
{
    size_t argc = ARG_COUNT_4;
    napi_value argv[ARG_COUNT_4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 3) { // 3: params num
        TLOGE(WmsLogTag::WMS_KEYBOARD, "[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    WSRect keyboardPanelRect = { 0, 0, 0, 0 };
    napi_value nativeObj = argv[ARG_INDEX_0];
    if (nativeObj == nullptr || !ConvertRectInfoFromJs(env, nativeObj, keyboardPanelRect)) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "[NAPI]Failed to convert parameter to keyboardPanelRect");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool isKeyboardShow = false;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_1], isKeyboardShow)) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "[NAPI]Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool isRotating = false;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_2], isRotating)) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "[NAPI]Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "[NAPI]session is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    session->CloseKeyboardSyncTransaction(keyboardPanelRect, isKeyboardShow, isRotating);
    return NapiGetUndefined(env);
}

void JsSceneSession::OnCreateSubSession(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("[NAPI]sceneSession is nullptr");
        return;
    }

    TLOGI(WmsLogTag::WMS_LIFE, "[NAPI]OnCreateSubSession, id: %{public}d, parentId: %{public}d",
        sceneSession->GetPersistentId(), sceneSession->GetParentPersistentId());
    wptr<SceneSession> weakSession(sceneSession);
    auto task = [this, weakSession, jsCallBack = GetJSCallback(CREATE_SUB_SESSION_CB), env = env_]() {
        if (jsCallBack == nullptr) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        auto specificSession = weakSession.promote();
        if (specificSession == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]root session or target session or env is nullptr");
            return;
        }
        napi_value jsSceneSessionObj = Create(env, specificSession);
        if (jsSceneSessionObj == nullptr || !jsCallBack) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]jsSceneSessionObj or jsCallBack is nullptr");
            return;
        }
        TLOGI(WmsLogTag::WMS_LIFE, "CreateJsSceneSessionObject success, id: %{public}d, parentId: %{public}d",
            specificSession->GetPersistentId(), specificSession->GetParentPersistentId());
        napi_value argv[] = {jsSceneSessionObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    std::string info = "OnCreateSpecificSession PID:" + std::to_string(sceneSession->GetPersistentId());
    taskScheduler_->PostMainThreadTask(task, info);
}

void JsSceneSession::OnBindDialogTarget(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFI("[NAPI]sceneSession is nullptr");
        return;
    }

    TLOGI(WmsLogTag::WMS_DIALOG, "OnBindDialogTarget, id: %{public}d", sceneSession->GetPersistentId());

    wptr<SceneSession> weakSession(sceneSession);
    auto task = [this, weakSession, jsCallBack = GetJSCallback(BIND_DIALOG_TARGET_CB), env = env_]() {
        if (jsCallBack == nullptr) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
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
        TLOGI(WmsLogTag::WMS_DIALOG, "CreateJsObject success, id: %{public}d", specificSession->GetPersistentId());
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
        TLOGW(WmsLogTag::WMS_LIFE, "session is nullptr");
        return;
    }

    TLOGI(WmsLogTag::WMS_LIFE, "id: %{public}d, state: %{public}d", session->GetPersistentId(), state);
    auto task = [state, jsCallBack = GetJSCallback(SESSION_STATE_CHANGE_CB), env = env_]() {
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
    WLOGFD("[NAPI]OnBufferAvailableChange, state: %{public}u", isBufferAvailable);
    auto task = [isBufferAvailable, jsCallBack = GetJSCallback(BUFFER_AVAILABLE_CHANGE_CB), env = env_]() {
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
    if (reason != SizeChangeReason::MOVE && rect.IsEmpty()) {
        WLOGFD("Rect is empty, there is no need to notify");
        return;
    }
    WLOGFD("[NAPI]OnSessionRectChange");

    auto task = [rect, reason, jsCallBack = GetJSCallback(SESSION_RECT_CHANGE_CB), env = env_]() {
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

void JsSceneSession::OnSessionPiPControlStatusChange(WsPiPControlType controlType, WsPiPControlStatus status)
{
    TLOGI(WmsLogTag::WMS_PIP, "controlType:%{public}u, status:%{public}d", controlType, status);
    auto task = [controlType, status, jsCallBack = GetJSCallback(SESSION_PIP_CONTROL_STATUS_CHANGE_CB),
        env = env_]() {
        if (!jsCallBack) {
            TLOGE(WmsLogTag::WMS_PIP, "[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value controlTypeValue = CreateJsValue(env, controlType);
        napi_value controlStatusValue = CreateJsValue(env, status);
        napi_value argv[] = {controlTypeValue, controlStatusValue};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, __func__);
}

void JsSceneSession::OnRaiseToTop()
{
    WLOGFI("[NAPI]OnRaiseToTop");

    auto task = [jsCallBack = GetJSCallback(RAISE_TO_TOP_CB), env = env_]() {
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

    auto task = [jsCallBack = GetJSCallback(RAISE_TO_TOP_POINT_DOWN_CB), env = env_]() {
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
    auto task = [jsCallBack = GetJSCallback(RAISE_ABOVE_TARGET_CB), env = env_, subWindowId]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsSceneSessionObj = CreateJsValue(env, subWindowId);
        if (jsSceneSessionObj == nullptr) {
            WLOGFE("[NAPI]jsSceneSessionObj is nullptr");
            return;
        }
        napi_value argv[] = {CreateJsError(env, 0), jsSceneSessionObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnRaiseAboveTarget: " + std::to_string(subWindowId));
}

void JsSceneSession::OnSessionFocusableChange(bool isFocusable)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "[NAPI]OnSessionFocusableChange, state: %{public}u", isFocusable);
    auto task = [isFocusable, jsCallBack = GetJSCallback(SESSION_FOCUSABLE_CHANGE_CB), env = env_]() {
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

    auto task = [touchable, jsCallBack = GetJSCallback(SESSION_TOUCHABLE_CHANGE_CB), env = env_]() {
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

void JsSceneSession::OnSessionTopmostChange(bool topmost)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "[NAPI]State: %{public}u", topmost);
    auto task = [topmost, jsCallBack = GetJSCallback(SESSION_TOP_MOST_CHANGE_CB), env = env_]() {
        if (!jsCallBack) {
            TLOGE(WmsLogTag::WMS_LAYOUT, "[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionTouchableObj = CreateJsValue(env, topmost);
        napi_value argv[] = {jsSessionTouchableObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnSessionTopmostChange: state " + std::to_string(topmost));
}

void JsSceneSession::OnClick()
{
    WLOGFD("[NAPI]OnClick");
    auto task = [jsCallBack = GetJSCallback(CLICK_CB), env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value argv[] = {};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), 0, argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnClick");
}

void JsSceneSession::OnContextTransparent()
{
    WLOGFD("[NAPI]OnContextTransparent");

    auto task = [jsCallBack = GetJSCallback(CONTEXT_TRANSPARENT_CB), env = env_]() {
        if (!jsCallBack) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value argv[] = {};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), 0, argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnContextTransparent");
}

void JsSceneSession::ChangeSessionVisibilityWithStatusBar(SessionInfo& info, bool visible)
{
    WLOGI("[NAPI]: bundleName %{public}s, moduleName %{public}s, "
        "abilityName %{public}s, appIndex %{public}d, reuse %{public}d, visible %{public}d",
        info.bundleName_.c_str(), info.moduleName_.c_str(), info.abilityName_.c_str(),
        info.appIndex_, info.reuse, visible);
    auto session = weakSession_.promote();
    if (session == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto task = [this, sessionInfo, visible]() {
        this->ChangeSessionVisibilityWithStatusBarInner(sessionInfo, visible);
    };
    taskScheduler_->PostMainThreadTask(task, "ChangeSessionVisibilityWithStatusBar, visible:" +
        std::to_string(visible));
}

void JsSceneSession::ChangeSessionVisibilityWithStatusBarInner(std::shared_ptr<SessionInfo> sessionInfo, bool visible)
{
    std::shared_ptr<NativeReference> jsCallBack = GetJSCallback(CHANGE_SESSION_VISIBILITY_WITH_STATUS_BAR);
    if (!jsCallBack) {
        WLOGFE("[NAPI]jsCallBack is nullptr");
        return;
    }
    if (sessionInfo == nullptr) {
        WLOGFE("[NAPI]sessionInfo is nullptr");
        return;
    }
    napi_value jsSessionInfo = CreateJsSessionInfo(env_, *sessionInfo);
    if (jsSessionInfo == nullptr) {
        WLOGFE("[NAPI]this target session info is nullptr");
        return;
    }
    napi_value visibleNapiV = CreateJsValue(env_, visible);
    napi_value argv[] = {jsSessionInfo, visibleNapiV};
    napi_call_function(env_, NapiGetUndefined(env_),
        jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
}

sptr<SceneSession> JsSceneSession::GenSceneSession(SessionInfo& info)
{
    sptr<SceneSession> sceneSession = nullptr;
    if (info.persistentId_ == 0) {
        auto result = SceneSessionManager::GetInstance().CheckIfReuseSession(info);
        if (result == BrokerStates::BROKER_NOT_START) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]BrokerStates not started");
            return nullptr;
        }
        if (info.reuse) {
            TLOGI(WmsLogTag::WMS_LIFE, "session need to be reusesd.");
            if (SceneSessionManager::GetInstance().CheckCollaboratorType(info.collaboratorType_)) {
                sceneSession = SceneSessionManager::GetInstance().FindSessionByAffinity(info.sessionAffinity);
            } else {
                sceneSession = SceneSessionManager::GetInstance().GetSceneSessionByName(
                    info.bundleName_, info.moduleName_, info.abilityName_, info.appIndex_);
            }
        }
        if (sceneSession == nullptr) {
            TLOGI(WmsLogTag::WMS_LIFE, "GetSceneSessionByName return nullptr, RequestSceneSession");
            sceneSession = SceneSessionManager::GetInstance().RequestSceneSession(info);
            if (sceneSession == nullptr) {
                TLOGE(WmsLogTag::WMS_LIFE, "RequestSceneSession return nullptr");
                return nullptr;
            }
        } else {
            sceneSession->SetSessionInfo(info);
        }
        info.persistentId_ = sceneSession->GetPersistentId();
        sceneSession->SetSessionInfoPersistentId(sceneSession->GetPersistentId());
    } else {
        sceneSession = SceneSessionManager::GetInstance().GetSceneSession(info.persistentId_);
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "GetSceneSession return nullptr");
            sceneSession = SceneSessionManager::GetInstance().RequestSceneSession(info);
            if (sceneSession == nullptr) {
                TLOGE(WmsLogTag::WMS_LIFE, "retry RequestSceneSession return nullptr");
                return nullptr;
            }
            info.persistentId_ = sceneSession->GetPersistentId();
            sceneSession->SetSessionInfoPersistentId(sceneSession->GetPersistentId());
        } else {
            sceneSession->SetSessionInfo(info);
        }
    }
    return sceneSession;
}

void JsSceneSession::PendingSessionActivation(SessionInfo& info)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[NAPI]bundleName %{public}s, moduleName %{public}s, abilityName %{public}s, "
        "appIndex %{public}d, reuse %{public}d", info.bundleName_.c_str(), info.moduleName_.c_str(),
        info.abilityName_.c_str(), info.appIndex_, info.reuse);
    auto sceneSession = GenSceneSession(info);
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "GenSceneSession failed");
        return;
    }

    if (info.want != nullptr) {
        auto focusedOnShow = info.want->GetBoolParam(AAFwk::Want::PARAM_RESV_WINDOW_FOCUSED, true);
        sceneSession->SetFocusedOnShow(focusedOnShow);
    } else {
        sceneSession->SetFocusedOnShow(true);
    }

    auto callerSession = SceneSessionManager::GetInstance().GetSceneSession(info.callerPersistentId_);
    if (callerSession != nullptr) {
        bool isCalledRightlyByCallerId = info.callerToken_ == callerSession->GetAbilityToken();
        TLOGI(WmsLogTag::WMS_SCB,
            "isCalledRightlyByCallerId result is: %{public}d", isCalledRightlyByCallerId);
        info.isCalledRightlyByCallerId_ = isCalledRightlyByCallerId;
    }
    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto task = [this, sessionInfo]() {
        PendingSessionActivationInner(sessionInfo);
    };
    sceneSession->PostLifeCycleTask(task, "PendingSessionActivation", LifeCycleTaskType::START);
    if (info.fullScreenStart_) {
        sceneSession->NotifySessionFullScreen(true);
    }
}

void JsSceneSession::PendingSessionActivationInner(std::shared_ptr<SessionInfo> sessionInfo)
{
    napi_env& env_ref = env_;
    auto task = [sessionInfo, jsCallBack = GetJSCallback(PENDING_SCENE_CB), env_ref]() {
        if (!jsCallBack) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]jsCallBack is nullptr");
            return;
        }
        if (sessionInfo == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]sessionInfo is nullptr");
            return;
        }
        napi_value jsSessionInfo = CreateJsSessionInfo(env_ref, *sessionInfo);
        if (jsSessionInfo == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]target session info is nullptr");
            return;
        }
        napi_value argv[] = {jsSessionInfo};
        TLOGI(WmsLogTag::WMS_LIFE, "[NAPI]PendingSessionActivationInner task success, id:%{public}d",
            sessionInfo->persistentId_);
        napi_call_function(env_ref, NapiGetUndefined(env_ref),
            jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "PendingSessionActivationInner");
}

void JsSceneSession::OnBackPressed(bool needMoveToBackground)
{
    WLOGFI("[NAPI]OnBackPressed needMoveToBackground %{public}d", needMoveToBackground);

    auto task = [needMoveToBackground, jsCallBack = GetJSCallback(BACK_PRESSED_CB), env = env_]() {
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
    TLOGI(WmsLogTag::WMS_LIFE, "bundle:%{public}s, ability:%{public}s, id:%{public}d",
        info.bundleName_.c_str(), info.abilityName_.c_str(), info.persistentId_);

    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto task = [sessionInfo, jsCallBack = GetJSCallback(TERMINATE_SESSION_CB), env = env_]() {
        if (!jsCallBack) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]jsCallBack is nullptr");
            return;
        }
        if (sessionInfo == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]sessionInfo is nullptr");
            return;
        }
        napi_value jsSessionInfo = CreateJsSessionInfo(env, *sessionInfo);
        if (jsSessionInfo == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]target session info is nullptr");
            return;
        }
        napi_value argv[] = {jsSessionInfo};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "TerminateSession name:" + info.abilityName_);
}

void JsSceneSession::TerminateSessionNew(const SessionInfo& info, bool needStartCaller, bool isFromBroker)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[NAPI]bundleName = %{public}s, abilityName = %{public}s",
        info.bundleName_.c_str(), info.abilityName_.c_str());
    if (isFromBroker == true) {
        needStartCaller = true;
    }
    auto task = [needStartCaller, jsCallBack = GetJSCallback(TERMINATE_SESSION_CB_NEW), env = env_]() {
        if (!jsCallBack) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsNeedStartCaller = CreateJsValue(env, needStartCaller);
        if (jsNeedStartCaller == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]jsNeedStartCaller is nullptr");
            return;
        }
        napi_value argv[] = {jsNeedStartCaller};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "TerminateSessionNew, name:" + info.abilityName_);
}

void JsSceneSession::TerminateSessionTotal(const SessionInfo& info, TerminateType terminateType)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[NAPI]bundleName = %{public}s, abilityName = %{public}s, terminateType = %{public}d",
        info.bundleName_.c_str(), info.abilityName_.c_str(), static_cast<int32_t>(terminateType));

    auto task = [terminateType, jsCallBack = GetJSCallback(TERMINATE_SESSION_CB_TOTAL), env = env_]() {
        if (!jsCallBack) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsTerminateType = CreateJsValue(env, static_cast<int32_t>(terminateType));
        if (jsTerminateType == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]jsTerminateType is nullptr");
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

    auto task = [label, jsCallBack = GetJSCallback(UPDATE_SESSION_LABEL_CB), env = env_]() {
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

    auto task = [iconPath, jsCallBack = GetJSCallback(UPDATE_SESSION_ICON_CB), env = env_]() {
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
    TLOGI(WmsLogTag::WMS_LIFE, "[NAPI]bundleName = %{public}s, abilityName = %{public}s",
        info.bundleName_.c_str(), info.abilityName_.c_str());

    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto task = [sessionInfo, needRemoveSession, jsCallBack = GetJSCallback(SESSION_EXCEPTION_CB), env = env_]() {
        if (!jsCallBack) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]jsCallBack is nullptr");
            return;
        }
        if (sessionInfo == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]sessionInfo is nullptr");
            return;
        }
        napi_value jsSessionInfo = CreateJsSessionInfo(env, *sessionInfo);
        napi_value jsNeedRemoveSession = CreateJsValue(env, needRemoveSession);
        if (jsSessionInfo == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]target session info is nullptr");
            return;
        }
        napi_value argv[] = {jsSessionInfo, jsNeedRemoveSession};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnSessionException, name" + info.bundleName_);
}

void JsSceneSession::PendingSessionToForeground(const SessionInfo& info)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[NAPI]bundleName = %{public}s, abilityName = %{public}s",
        info.bundleName_.c_str(), info.abilityName_.c_str());

    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto task = [sessionInfo, jsCallBack = GetJSCallback(PENDING_SESSION_TO_FOREGROUND_CB), env = env_]() {
        if (!jsCallBack) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]jsCallBack is nullptr");
            return;
        }
        if (sessionInfo == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]sessionInfo is nullptr");
            return;
        }
        napi_value jsSessionInfo = CreateJsSessionInfo(env, *sessionInfo);
        if (jsSessionInfo == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]target session info is nullptr");
            return;
        }
        napi_value argv[] = {jsSessionInfo};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "PendingSessionToForeground:" + info.bundleName_);
}

void JsSceneSession::PendingSessionToBackgroundForDelegator(const SessionInfo& info)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[NAPI]bundleName = %{public}s, abilityName = %{public}s",
        info.bundleName_.c_str(), info.abilityName_.c_str());

    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto task = [sessionInfo,
        jsCallBack = GetJSCallback(PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR_CB), env = env_]() {
        if (!jsCallBack) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]jsCallBack is nullptr");
            return;
        }
        if (sessionInfo == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]sessionInfo is nullptr");
            return;
        }
        napi_value jsSessionInfo = CreateJsSessionInfo(env, *sessionInfo);
        if (jsSessionInfo == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]target session info is nullptr");
            return;
        }
        napi_value argv[] = {jsSessionInfo};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "PendingSessionToBackgroundForDelegator, name:" + info.bundleName_);
}

void JsSceneSession::OnSystemBarPropertyChange(const std::unordered_map<WindowType, SystemBarProperty>& propertyMap)
{
    TLOGD(WmsLogTag::WMS_IMMS, "[NAPI]OnSystemBarPropertyChange");

    auto task = [jsCallBack = GetJSCallback(SYSTEMBAR_PROPERTY_CHANGE_CB), propertyMap, env = env_]() {
        if (jsCallBack == nullptr) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionStateObj = CreateJsSystemBarPropertyArrayObject(env, propertyMap);
        if (jsSessionStateObj == nullptr) {
            TLOGE(WmsLogTag::WMS_IMMS, "[NAPI]jsSessionStateObj is nullptr");
            return;
        }
        napi_value argv[] = {jsSessionStateObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnSystemBarPropertyChange");
}

void JsSceneSession::OnNeedAvoid(bool status)
{
    TLOGD(WmsLogTag::WMS_IMMS, "[NAPI]OnNeedAvoid %{public}d", status);

    auto task = [jsCallBack = GetJSCallback(NEED_AVOID_CB), needAvoid = status, env = env_]() {
        if (jsCallBack == nullptr) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionStateObj = CreateJsValue(env, needAvoid);
        napi_value argv[] = {jsSessionStateObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnNeedAvoid:" + std::to_string(status));
}

void JsSceneSession::OnIsCustomAnimationPlaying(bool status)
{
    WLOGFI("[NAPI]OnIsCustomAnimationPlaying %{public}d", status);

    auto task = [jsCallBack = GetJSCallback(CUSTOM_ANIMATION_PLAYING_CB), isPlaying = status, env = env_]() {
        if (jsCallBack == nullptr) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionStateObj = CreateJsValue(env, isPlaying);
        napi_value argv[] = {jsSessionStateObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnIsCustomAnimationPlaying:" + std::to_string(status));
}

void JsSceneSession::OnShowWhenLocked(bool showWhenLocked)
{
    WLOGFI("[NAPI]OnShowWhenLocked %{public}d", showWhenLocked);

    auto task = [jsCallBack = GetJSCallback(SHOW_WHEN_LOCKED_CB), flag = showWhenLocked, env = env_]() {
        if (jsCallBack == nullptr) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value jsSessionStateObj = CreateJsValue(env, flag);
        napi_value argv[] = {jsSessionStateObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnShowWhenLocked:" +std::to_string(showWhenLocked));
}

void JsSceneSession::OnReuqestedOrientationChange(uint32_t orientation)
{
    WLOGFI("[NAPI]OnReuqestedOrientationChange %{public}u", orientation);

    auto task = [jsCallBack = GetJSCallback(REQUESTED_ORIENTATION_CHANGE_CB), rotation = orientation, env = env_]() {
        if (jsCallBack == nullptr) {
            WLOGFE("[NAPI]jsCallBack is nullptr");
            return;
        }
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

napi_value JsSceneSession::OnSetExitSplitOnBackground(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != 1) {
        TLOGE(WmsLogTag::WMS_MULTI_WINDOW, "[NAPI]Argc count is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool isExitSplitOnBackground = false;
    if (!ConvertFromJsValue(env, argv[0], isExitSplitOnBackground)) {
        TLOGE(WmsLogTag::WMS_MULTI_WINDOW, "[NAPI]Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_MULTI_WINDOW, "[NAPI]session is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    session->SetExitSplitOnBackground(isExitSplitOnBackground);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetWaterMarkFlag(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool isWaterMarkAdded = false;
    if (!ConvertFromJsValue(env, argv[0], isWaterMarkAdded)) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]session is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    if (property == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]property is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    uint32_t currFlag = session->GetSessionProperty()->GetWindowFlags();
    if (isWaterMarkAdded) {
        currFlag = currFlag | static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK);
    } else {
        currFlag = currFlag & ~(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK));
    }
    property->SetSystemCalling(true);
    property->SetWindowFlags(currFlag);
    SceneSessionManager::GetInstance().SetWindowFlags(session, property);
    return NapiGetUndefined(env);
}

void JsSceneSession::ProcessPrepareClosePiPSessionRegister()
{
    auto sessionchangeCallback = sessionchangeCallback_.promote();
    if (sessionchangeCallback == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "sessionchangeCallback is nullptr");
        return;
    }
    sessionchangeCallback->onPrepareClosePiPSession_ = [this] { this->OnPrepareClosePiPSession(); };
    TLOGD(WmsLogTag::WMS_PIP, "ProcessPrepareClosePiPSessionRegister success");
}

void JsSceneSession::OnPrepareClosePiPSession()
{
    TLOGI(WmsLogTag::WMS_PIP, "[NAPI]OnPrepareClosePiPSession");
    auto task = [jsCallBack = GetJSCallback(PREPARE_CLOSE_PIP_SESSION), env = env_]() {
        if (!jsCallBack) {
            TLOGE(WmsLogTag::WMS_PIP, "[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value argv[] = {};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), 0, argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnPrepareClosePiPSession");
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

napi_value JsSceneSession::SetScale(napi_env env, napi_callback_info info)
{
    if (Session::IsScbCoreEnabled()) {
        return nullptr;
    }
    WLOGI("[NAPI]SetScale");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetScale(env, info) : nullptr;
}

napi_value JsSceneSession::OnSetScale(napi_env env, napi_callback_info info)
{
    size_t argc = ARG_COUNT_4;
    napi_value argv[ARG_COUNT_4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARG_COUNT_4) { // ARG_COUNT_4: params num
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

napi_value JsSceneSession::SetWindowLastSafeRect(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "[NAPI]");
    JsSceneSession* me = CheckParamsAndGetThis<JsSceneSession>(env, info);
    return (me != nullptr) ? me->OnSetWindowLastSafeRect(env, info) : nullptr;
}

napi_value JsSceneSession::OnSetWindowLastSafeRect(napi_env env, napi_callback_info info)
{
    size_t argc = ARG_COUNT_4;
    napi_value argv[ARG_COUNT_4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARG_COUNT_4) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t left = 0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_0], left)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[NAPI]Failed to convert parameter to left");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t top = 0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_1], top)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[NAPI]Failed to convert parameter to top");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t width = 0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_2], width)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[NAPI]Failed to convert parameter to width");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t height = 0;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_3], height)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[NAPI]Failed to convert parameter to height");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[NAPI]session is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    WSRect lastRect = { left, top, width, height };
    session->SetLastSafeRect(lastRect);
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

napi_value JsSceneSession::OnSetPipActionEvent(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        TLOGE(WmsLogTag::WMS_PIP, "[NAPI]Argc count is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    std::string action;
    if (!ConvertFromJsValue(env, argv[0], action)) {
        TLOGE(WmsLogTag::WMS_PIP, "[NAPI]Failed to convert parameter to string");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t status = -1;
    if (argc > 1) {
        if (!ConvertFromJsValue(env, argv[1], status)) {
            TLOGE(WmsLogTag::WMS_PIP, "[NAPI]Failed to convert parameter to int");
            napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                          "Input parameter is missing or invalid"));
            return NapiGetUndefined(env);
        }
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "[NAPI]Session is nullptr");
        return NapiGetUndefined(env);
    }
    session->SetPipActionEvent(action, status);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetPiPControlEvent(napi_env env, napi_callback_info info)
{
    size_t argc = ARG_COUNT_4;
    napi_value argv[ARG_COUNT_4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        TLOGE(WmsLogTag::WMS_PIP, "[NAPI]Argc count is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto controlType = WsPiPControlType::VIDEO_PLAY_PAUSE;
    if (!ConvertFromJsValue(env, argv[0], controlType)) {
        TLOGE(WmsLogTag::WMS_PIP, "[NAPI]Failed to convert parameter to int");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto status = WsPiPControlStatus::PLAY;
    if (argc > 1) {
        if (!ConvertFromJsValue(env, argv[1], status)) {
            TLOGE(WmsLogTag::WMS_PIP, "[NAPI]Failed to convert parameter to int");
            napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                "Input parameter is missing or invalid"));
            return NapiGetUndefined(env);
        }
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "[NAPI]Session is nullptr");
        return NapiGetUndefined(env);
    }
    session->SetPiPControlEvent(controlType, status);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnNotifyDisplayStatusBarTemporarily(napi_env env, napi_callback_info info)
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "[NAPI]session is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    bool isTempDisplay = false;
    if (argc == ARGC_ONE && GetType(env, argv[0]) == napi_boolean) {
        if (!ConvertFromJsValue(env, argv[0], isTempDisplay)) {
            TLOGE(WmsLogTag::WMS_IMMS, "[NAPI]failed to convert parameter to bool");
            return NapiGetUndefined(env);
        }
    }
    session->SetIsDisplayStatusBarTemporarily(isTempDisplay);

    TLOGI(WmsLogTag::WMS_IMMS, "Set success with id:%{public}u name:%{public}s isTempDisplay:%{public}u",
        session->GetPersistentId(), session->GetWindowName().c_str(), isTempDisplay);
    return NapiGetUndefined(env);
}

sptr<SceneSession> JsSceneSession::GetNativeSession() const
{
    return weakSession_.promote();
}

napi_value JsSceneSession::OnSetTemporarilyShowWhenLocked(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]argc is invalid : %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isTemporarilyShowWhenLocked = false;
    if (!ConvertFromJsValue(env, argv[0], isTemporarilyShowWhenLocked)) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI] Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI] session_ is null");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    session->SetTemporarilyShowWhenLocked(isTemporarilyShowWhenLocked);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetSkipDraw(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]argc is invalid : %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool skip = false;
    if (!ConvertFromJsValue(env, argv[0], skip)) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI] Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI] session_ is null");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    session->SetSkipDraw(skip);
    return NapiGetUndefined(env);
}

std::shared_ptr<NativeReference> JsSceneSession::GetJSCallback(const std::string& functionName)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsSceneSession::GetJSCallback[%s]", functionName.c_str());
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
    auto iter = jsCbMap_.find(functionName);
    if (iter == jsCbMap_.end()) {
        TLOGE(WmsLogTag::DEFAULT, "%{public}s callback not found!", functionName.c_str());
    } else {
        jsCallBack = iter->second;
    }
    return jsCallBack;
}

napi_value JsSceneSession::OnSetSkipSelfWhenShowOnVirtualScreen(napi_env env, napi_callback_info info)
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
    bool isSkip = false;
    if (!ConvertFromJsValue(env, argv[0], isSkip)) {
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
    session->SetSkipSelfWhenShowOnVirtualScreen(isSkip);
    WLOGFI("[NAPI]OnSetSkipSelfWhenShowOnVirtualScreen end");
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetCompatibleModeInPc(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    if (argc < ARGC_TWO) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool enable = false;
    if (!ConvertFromJsValue(env, argv[0], enable)) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]Failed to convert parameter to enable");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool isSupportDragInPcCompatibleMode = false;
    if (!ConvertFromJsValue(env, argv[ARGC_ONE], isSupportDragInPcCompatibleMode)) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]Failed to convert parameter to isSupportDragInPcCompatibleMode");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]session is nullptr");
        return NapiGetUndefined(env);
    }
    session->SetCompatibleModeInPc(enable, isSupportDragInPcCompatibleMode);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetBlankFlag(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    if (argc < ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool isAddBlank = false;
    if (!ConvertFromJsValue(env, argv[0], isAddBlank)) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]Failed to convert parameter to isAddBlank");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]session is nullptr");
        return NapiGetUndefined(env);
    }
    session->SetBlankFlag(isAddBlank);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetBufferAvailableCallbackEnable(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    if (argc < ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool enable = false;
    if (!ConvertFromJsValue(env, argv[0], enable)) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]Failed to convert parameter to enable");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]session is nullptr");
        return NapiGetUndefined(env);
    }
    session->SetBufferAvailableCallbackEnable(enable);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnIsDeviceWakeupByApplication(napi_env env, napi_callback_info info)
{
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]session is null");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "session is null"));
        return NapiGetUndefined(env);
    }
    auto result = session->IsDeviceWakeupByApplication();
    napi_value jsResult;
    napi_get_boolean(env, result, &jsResult);
    return jsResult;
}

napi_value JsSceneSession::OnSetIsPcAppInPad(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool enable = false;
    if (!ConvertFromJsValue(env, argv[0], enable)) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]Failed to convert parameter to enable");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                                      "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]session is nullptr");
        return NapiGetUndefined(env);
    }
    session->SetIsPcAppInPad(enable);
    return NapiGetUndefined(env);
}

napi_value JsSceneSession::OnSetStartingWindowExitAnimationFlag(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_FOUR;
    napi_value argv[ARGC_FOUR] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    if (argc != ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool enable = true;
    if (!ConvertFromJsValue(env, argv[0], enable)) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]Failed to convert parameter to enable");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    auto session = weakSession_.promote();
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_SCB, "[NAPI]session is null");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "session is null"));
        return NapiGetUndefined(env);
    }
    session->SetStartingWindowExitAnimationFlag(enable);
    return NapiGetUndefined(env);
}

} // namespace OHOS::Rosen
