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

#include <context.h>
#include <js_runtime_utils.h>
#include "configuration.h"
#include "interfaces/include/ws_common.h"
#include "napi_common_want.h"
#include "native_value.h"
#include "pixel_map_napi.h"
#include "session/host/include/scene_persistence.h"
#include "session/host/include/scene_persistent_storage.h"
#include "session/host/include/session.h"
#include "session_manager/include/scene_session_manager.h"
#include <ui_content.h>
#include "want.h"
#include "window_manager_hilog.h"

#include "js_root_scene_session.h"
#include "js_scene_session.h"
#include "js_scene_utils.h"
#include "js_window_scene_config.h"
#ifdef SOC_PERF_ENABLE
#include "socperf_client.h"
#endif
#ifdef RESOURCE_SCHEDULE_SERVICE_ENABLE
#include "res_sched_client.h"
#endif

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsSceneSessionManager" };
constexpr int MIN_ARG_COUNT = 3;
constexpr int ARG_INDEX_1 = 1;
constexpr int ARG_INDEX_TWO = 2;
constexpr int ARG_INDEX_3 = 3;
constexpr int32_t RESTYPE_RECLAIM = 100001;
const std::string RES_PARAM_RECLAIM_TAG = "reclaimTag";
const std::string CREATE_SYSTEM_SESSION_CB = "createSpecificSession";
const std::string CREATE_KEYBOARD_SESSION_CB = "createKeyboardSession";
const std::string RECOVER_SCENE_SESSION_CB = "recoverSceneSession";
const std::string STATUS_BAR_ENABLED_CHANGE_CB = "statusBarEnabledChange";
const std::string GESTURE_NAVIGATION_ENABLED_CHANGE_CB = "gestureNavigationEnabledChange";
const std::string OUTSIDE_DOWN_EVENT_CB = "outsideDownEvent";
const std::string START_UI_ABILITY_ERROR = "startUIAbilityError";
const std::string ARG_DUMP_HELP = "-h";
const std::string SHIFT_FOCUS_CB = "shiftFocus";
const std::string CALLING_WINDOW_ID_CHANGE_CB = "callingWindowIdChange";
const std::string SWITCH_TO_ANOTHER_USER_CB = "switchToAnotherUser";
} // namespace

napi_value JsSceneSessionManager::Init(napi_env env, napi_value exportObj)
{
    WLOGI("[NAPI]JsSceneSessionManager Init");
    if (env == nullptr || exportObj == nullptr) {
        WLOGFE("env or exportObj is null!");
        return nullptr;
    }

    std::unique_ptr<JsSceneSessionManager> jsSceneSessionManager = std::make_unique<JsSceneSessionManager>(env);
    napi_wrap(env, exportObj, jsSceneSessionManager.release(), JsSceneSessionManager::Finalizer, nullptr, nullptr);

    napi_set_named_property(env, exportObj, "SessionState", CreateJsSessionState(env));
    napi_set_named_property(env, exportObj, "SessionType", SessionTypeInit(env));
    napi_set_named_property(env, exportObj, "KeyboardGravity", KeyboardGravityInit(env));
    napi_set_named_property(env, exportObj, "SessionSizeChangeReason", CreateJsSessionSizeChangeReason(env));
    napi_set_named_property(env, exportObj, "StartupVisibility", CreateJsSessionStartupVisibility(env));
    napi_set_named_property(env, exportObj, "ProcessMode", CreateJsSessionProcessMode(env));

    const char* moduleName = "JsSceneSessionManager";
    BindNativeFunction(env, exportObj, "getRootSceneSession", moduleName, JsSceneSessionManager::GetRootSceneSession);
    BindNativeFunction(env, exportObj, "requestSceneSession", moduleName, JsSceneSessionManager::RequestSceneSession);
    BindNativeFunction(env, exportObj, "updateSceneSessionWant",
        moduleName, JsSceneSessionManager::UpdateSceneSessionWant);
    BindNativeFunction(env, exportObj, "requestSceneSessionActivation", moduleName,
        JsSceneSessionManager::RequestSceneSessionActivation);
    BindNativeFunction(env, exportObj, "requestSceneSessionBackground", moduleName,
        JsSceneSessionManager::RequestSceneSessionBackground);
    BindNativeFunction(env, exportObj, "requestSceneSessionDestruction", moduleName,
        JsSceneSessionManager::RequestSceneSessionDestruction);
    BindNativeFunction(env, exportObj, "notifyForegroundInteractiveStatus", moduleName,
        JsSceneSessionManager::NotifyForegroundInteractiveStatus);
    BindNativeFunction(env, exportObj, "isSceneSessionValid", moduleName,
        JsSceneSessionManager::IsSceneSessionValid);
    BindNativeFunction(env, exportObj, "on", moduleName, JsSceneSessionManager::RegisterCallback);
    BindNativeFunction(env, exportObj, "getWindowSceneConfig", moduleName,
        JsSceneSessionManager::GetWindowSceneConfig);
    BindNativeFunction(env, exportObj, "processBackEvent", moduleName, JsSceneSessionManager::ProcessBackEvent);
    BindNativeFunction(env, exportObj, "checkSceneZOrder", moduleName, JsSceneSessionManager::CheckSceneZOrder);
    BindNativeFunction(env, exportObj, "updateFocus", moduleName, JsSceneSessionManager::UpdateFocus);
    BindNativeFunction(env, exportObj, "initUserInfo", moduleName, JsSceneSessionManager::InitUserInfo);
    BindNativeFunction(env, exportObj, "requestSceneSessionByCall", moduleName,
        JsSceneSessionManager::RequestSceneSessionByCall);
    BindNativeFunction(env, exportObj, "startAbilityBySpecified", moduleName,
        JsSceneSessionManager::StartAbilityBySpecified);
    BindNativeFunction(env, exportObj, "startUIAbilityBySCB", moduleName,
        JsSceneSessionManager::StartUIAbilityBySCB);
    BindNativeFunction(env, exportObj, "changeUIAbilityVisibilityBySCB", moduleName,
        JsSceneSessionManager::ChangeUIAbilityVisibilityBySCB);
    BindNativeFunction(env, exportObj, "getSessionSnapshot", moduleName,
        JsSceneSessionManager::GetSessionSnapshotFilePath);
    BindNativeFunction(env, exportObj, "InitWithRenderServiceAdded", moduleName,
        JsSceneSessionManager::InitWithRenderServiceAdded);
    BindNativeFunction(env, exportObj, "getAllAbilityInfo", moduleName, JsSceneSessionManager::GetAllAbilityInfos);
    BindNativeFunction(env, exportObj, "prepareTerminate", moduleName, JsSceneSessionManager::PrepareTerminate);
    BindNativeFunction(env, exportObj, "perfRequestEx", moduleName, JsSceneSessionManager::PerfRequestEx);
    BindNativeFunction(env, exportObj, "updateWindowMode", moduleName, JsSceneSessionManager::UpdateWindowMode);
    BindNativeFunction(env, exportObj, "getRootSceneUIContext", moduleName,
        JsSceneSessionManager::GetRootSceneUIContext);
    BindNativeFunction(env, exportObj, "sendTouchEvent", moduleName, JsSceneSessionManager::SendTouchEvent);
    BindNativeFunction(env, exportObj, "addWindowDragHotArea", moduleName, JsSceneSessionManager::AddWindowDragHotArea);
    BindNativeFunction(env, exportObj, "preloadInLakeApp", moduleName, JsSceneSessionManager::PreloadInLakeApp);
    BindNativeFunction(env, exportObj, "requestFocusStatus", moduleName, JsSceneSessionManager::RequestFocusStatus);
    BindNativeFunction(env, exportObj, "resetFocusedOnShow", moduleName, JsSceneSessionManager::ResetFocusedOnShow);
    BindNativeFunction(env, exportObj, "requestAllAppSessionUnfocus", moduleName,
        JsSceneSessionManager::RequestAllAppSessionUnfocus);
    BindNativeFunction(env, exportObj, "setScreenLocked", moduleName, JsSceneSessionManager::SetScreenLocked);
    BindNativeFunction(env, exportObj, "updateMaximizeMode", moduleName, JsSceneSessionManager::UpdateMaximizeMode);
    BindNativeFunction(env, exportObj, "reportData", moduleName, JsSceneSessionManager::ReportData);
    BindNativeFunction(env, exportObj, "updateSessionDisplayId", moduleName,
        JsSceneSessionManager::UpdateSessionDisplayId);
    BindNativeFunction(env, exportObj, "notifySwitchingToCurrentUser", moduleName,
        JsSceneSessionManager::NotifySwitchingToCurrentUser);
    BindNativeFunction(env, exportObj, "updateConfig", moduleName,
        JsSceneSessionManager::UpdateConfig);
    BindNativeFunction(env, exportObj, "notifySessionRecoverStatus", moduleName,
        JsSceneSessionManager::NotifySessionRecoverStatus);
    BindNativeFunction(env, exportObj, "notifyAINavigationBarShowStatus", moduleName,
        JsSceneSessionManager::NotifyAINavigationBarShowStatus);
    BindNativeFunction(env, exportObj, "updateTitleInTargetPos", moduleName, JsSceneSessionManager::UpdateTitleInTargetPos);
    BindNativeFunction(env, exportObj, "setSystemAnimatedScenes", moduleName,
        JsSceneSessionManager::SetSystemAnimatedScenes);
    BindNativeFunction(env, exportObj, "getSessionSnapshotPixelMap", moduleName,
        JsSceneSessionManager::GetSessionSnapshotPixelMap);
    BindNativeFunction(env, exportObj, "getIsLayoutFullScreen", moduleName,
        JsSceneSessionManager::GetIsLayoutFullScreen);
    BindNativeFunction(env, exportObj, "switchFreeMultiWindow", moduleName,
        JsSceneSessionManager::SwitchFreeMultiWindow);
    BindNativeFunction(env, exportObj, "getFreeMultiWindowConfig", moduleName,
        JsSceneSessionManager::GetFreeMultiWindowConfig);
    BindNativeFunction(env, exportObj, "getCustomDecorHeight", moduleName, JsSceneSessionManager::GetCustomDecorHeight);
    return NapiGetUndefined(env);
}

JsSceneSessionManager::JsSceneSessionManager(napi_env env) : env_(env)
{
    listenerFunc_ = {
        { CREATE_SYSTEM_SESSION_CB,     &JsSceneSessionManager::ProcessCreateSystemSessionRegister },
        { CREATE_KEYBOARD_SESSION_CB,   &JsSceneSessionManager::ProcessCreateKeyboardSessionRegister },
        { RECOVER_SCENE_SESSION_CB,     &JsSceneSessionManager::ProcessRecoverSceneSessionRegister },
        { STATUS_BAR_ENABLED_CHANGE_CB, &JsSceneSessionManager::ProcessStatusBarEnabledChangeListener},
        { OUTSIDE_DOWN_EVENT_CB,        &JsSceneSessionManager::ProcessOutsideDownEvent },
        { SHIFT_FOCUS_CB,               &JsSceneSessionManager::ProcessShiftFocus },
        { CALLING_WINDOW_ID_CHANGE_CB,  &JsSceneSessionManager::ProcessCallingSessionIdChangeRegister},
        { START_UI_ABILITY_ERROR,       &JsSceneSessionManager::ProcessStartUIAbilityErrorRegister},
        { GESTURE_NAVIGATION_ENABLED_CHANGE_CB,
            &JsSceneSessionManager::ProcessGestureNavigationEnabledChangeListener },
        { SWITCH_TO_ANOTHER_USER_CB,    &JsSceneSessionManager::ProcessSwitchingToAnotherUserRegister },
    };
    taskScheduler_ = std::make_shared<MainThreadScheduler>(env);
}

void JsSceneSessionManager::OnCreateSystemSession(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        TLOGI(WmsLogTag::WMS_LIFE, "[NAPI]sceneSession is nullptr");
        return;
    }
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(CREATE_SYSTEM_SESSION_CB);
        if (iter == jsCbMap_.end()) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]Can't find callback, id: %{public}d", sceneSession->GetPersistentId());
            return;
        }
        TLOGD(WmsLogTag::WMS_LIFE, "[NAPI]OnCreateSystemSession, id: %{public}d", sceneSession->GetPersistentId());
        jsCallBack = iter->second;
    }
    wptr<SceneSession> weakSession(sceneSession);
    auto task = [this, weakSession, jsCallBack, env = env_]() {
        auto specificSession = weakSession.promote();
        if (specificSession == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]specific session is nullptr");
            return;
        }
        napi_value jsSceneSessionObj = JsSceneSession::Create(env, specificSession);
        if (jsSceneSessionObj == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]jsSceneSessionObj is nullptr");
            return;
        }
        napi_value argv[] = {jsSceneSessionObj};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnCreateSystemSession");
}

void JsSceneSessionManager::OnCreateKeyboardSession(const sptr<SceneSession>& keyboardSession,
    const sptr<SceneSession>& panelSession)
{
    if (keyboardSession == nullptr || panelSession == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "[NAPI]keyboard or panel session is nullptr");
        return;
    }
    auto iter = jsCbMap_.find(CREATE_KEYBOARD_SESSION_CB);
    if (iter == jsCbMap_.end()) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "[NAPI]Can't find callback, id: %{public}d", keyboardSession->GetPersistentId());
        return;
    }

    TLOGI(WmsLogTag::WMS_KEYBOARD, "[NAPI]keyboardId: %{public}d, panelId: %{public}d",
        keyboardSession->GetPersistentId(), panelSession->GetPersistentId());
    auto jsCallBack = iter->second;
    wptr<SceneSession> weakKeyboardSession(keyboardSession);
    wptr<SceneSession> weakPanelSession(panelSession);
    auto task = [this, weakKeyboardSession, weakPanelSession, jsCallBack, env = env_]() {
        if (!jsCallBack) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "[NAPI]jsCallBack is nullptr");
            return;
        }
        auto keyboardSession = weakKeyboardSession.promote();
        auto panelSession = weakPanelSession.promote();
        if (keyboardSession == nullptr || panelSession == nullptr) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "[NAPI]keyboard or panel session is nullptr");
            return;
        }
        napi_value keyboardSessionObj = JsSceneSession::Create(env, keyboardSession);
        if (keyboardSessionObj == nullptr) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "[NAPI]keyboardSessionObj is nullptr");
            return;
        }
        napi_value panelSessionObj = JsSceneSession::Create(env, panelSession);
        if (panelSessionObj == nullptr) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "[NAPI]panelSessionObj is nullptr");
            return;
        }
        napi_value argv[] = { keyboardSessionObj, panelSessionObj };
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnCreateKeyboardSession");
}

void JsSceneSessionManager::OnRecoverSceneSession(const sptr<SceneSession>& sceneSession, const SessionInfo& info)
{
    if (sceneSession == nullptr) {
        WLOGFI("[NAPI]sceneSession is nullptr");
        return;
    }
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(RECOVER_SCENE_SESSION_CB);
        if (iter == jsCbMap_.end()) {
            WLOGFW("[NAPI]Not found recoverSceneSession key in jsCbMap_");
            return;
        }
        jsCallBack = iter->second;
    }
    WLOGFI("[NAPI]OnRecoverSceneSession");
    wptr<SceneSession> weakSession(sceneSession);
    std::shared_ptr<SessionInfo> sessionInfo = std::make_shared<SessionInfo>(info);
    auto task = [this, weakSession, sessionInfo, jsCallBack, env = env_]() {
        auto sceneSession = weakSession.promote();
        if (sceneSession == nullptr) {
            WLOGFE("[NAPI]sceneSession is nullptr");
            return;
        }
        napi_value jsSceneSessionObj = JsSceneSession::Create(env, sceneSession);
        if (jsSceneSessionObj == nullptr) {
            WLOGFE("[NAPI]jsSceneSessionObj is nullptr");
            return;
        }

        if (sessionInfo == nullptr) {
            WLOGFE("[NAPI]sessionInfo is nullptr");
            return;
        }
        napi_value jsSessionRecoverInfo =
            CreateJsSessionRecoverInfo(env, *sessionInfo, sceneSession->GetSessionProperty());
        if (jsSessionRecoverInfo == nullptr) {
            WLOGFE("[NAPI]this target session info is nullptr");
            return;
        }

        napi_value argv[] = { jsSceneSessionObj, jsSessionRecoverInfo };
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    WLOGFI("[NAPI]OnRecoverSceneSession post task");
    taskScheduler_->PostMainThreadTask(task, "OnRecoverSceneSession");
}

void JsSceneSessionManager::OnStatusBarEnabledUpdate(bool enable)
{
    WLOGFI("[NAPI]OnStatusBarEnabledUpdate");
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(STATUS_BAR_ENABLED_CHANGE_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [this, enable, jsCallBack, env = env_]() {
        napi_value argv[] = {CreateJsValue(env, enable)};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnStatusBarEnabledUpdate, Enable" + std::to_string(enable));
}

void JsSceneSessionManager::OnGestureNavigationEnabledUpdate(bool enable)
{
    WLOGFI("[NAPI]OnGestureNavigationEnabledUpdate");
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(GESTURE_NAVIGATION_ENABLED_CHANGE_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [this, enable, jsCallBack, env = env_]() {
        napi_value argv[] = {CreateJsValue(env, enable)};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnGestureNavigationEnabledUpdate" + std::to_string(enable));
}

void JsSceneSessionManager::OnStartUIAbilityError(const uint32_t errorCode)
{
    WLOGFI("[NAPI]OnStartUIAbilityError");
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(START_UI_ABILITY_ERROR);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [this, errorCode, jsCallBack, env = env_]() {
        napi_value argv[] = {CreateJsValue(env, errorCode)};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnStartUIAbilityError, errorCode: " + std::to_string(errorCode));
}

void JsSceneSessionManager::OnOutsideDownEvent(int32_t x, int32_t y)
{
    WLOGFD("[NAPI]OnOutsideDownEvent");
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(OUTSIDE_DOWN_EVENT_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [this, x, y, jsCallBack, env = env_]() {
        napi_value objValue = nullptr;
        napi_create_object(env, &objValue);
        if (objValue == nullptr) {
            WLOGFE("[NAPI]Object is null!");
            return;
        }

        napi_set_named_property(env, objValue, "x", CreateJsValue(env_, x));
        napi_set_named_property(env, objValue, "y", CreateJsValue(env_, y));
        napi_value argv[] = {objValue};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    std::string info = "OnOutsideDownEvent:[" + std::to_string(x) + ", " + std::to_string(y) + "]";
    taskScheduler_->PostMainThreadTask(task, info);
}

void JsSceneSessionManager::OnShiftFocus(int32_t persistentId)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "[NAPI]OnShiftFocus");
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(SHIFT_FOCUS_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [this, persistentId, jsCallBack, env = env_]() {
        napi_value argv[] = {CreateJsValue(env, persistentId)};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnShiftFocus, PID:" + std::to_string(persistentId));
}

void JsSceneSessionManager::OnCallingSessionIdChange(uint32_t windowId)
{
    TLOGD(WmsLogTag::WMS_KEYBOARD, "[NAPI]OnCallingSessionIdChange");
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(CALLING_WINDOW_ID_CHANGE_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [this, windowId, jsCallBack, env = env_]() {
        napi_value argv[] = { CreateJsValue(env, windowId) };
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), ArraySize(argv), argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnCallingSessionIdChange, windowId:" + std::to_string(windowId));
}

void JsSceneSessionManager::ProcessCreateSystemSessionRegister()
{
    NotifyCreateSystemSessionFunc func = [this](const sptr<SceneSession>& session) {
        TLOGI(WmsLogTag::WMS_SYSTEM, "NotifyCreateSystemSessionFunc");
        this->OnCreateSystemSession(session);
    };
    SceneSessionManager::GetInstance().SetCreateSystemSessionListener(func);
}

void JsSceneSessionManager::ProcessCreateKeyboardSessionRegister()
{
    NotifyCreateKeyboardSessionFunc func = [this](const sptr<SceneSession>& keyboardSession,
        const sptr<SceneSession>& panelSession) {
        this->OnCreateKeyboardSession(keyboardSession, panelSession);
    };
    SceneSessionManager::GetInstance().SetCreateKeyboardSessionListener(func);
}

void JsSceneSessionManager::ProcessStartUIAbilityErrorRegister()
{
    ProcessStartUIAbilityErrorFunc func = [this](uint32_t startUIAbilityError) {
        WLOGFD("ProcessStartUIAbilityErrorFunc called, startUIAbilityError: %{public}d", startUIAbilityError);
        this->OnStartUIAbilityError(startUIAbilityError);
    };
    SceneSessionManager::GetInstance().SetStartUIAbilityErrorListener(func);
}

void JsSceneSessionManager::ProcessRecoverSceneSessionRegister()
{
    NotifyRecoverSceneSessionFunc func = [this](const sptr<SceneSession>& session, const SessionInfo& sessionInfo) {
        TLOGD(WmsLogTag::WMS_RECOVER, "RecoverSceneSession");
        this->OnRecoverSceneSession(session, sessionInfo);
    };
    SceneSessionManager::GetInstance().SetRecoverSceneSessionListener(func);
}

void JsSceneSessionManager::ProcessStatusBarEnabledChangeListener()
{
    ProcessStatusBarEnabledChangeFunc func = [this](bool enable) {
        WLOGFD("StatusBarEnabledUpdate");
        this->OnStatusBarEnabledUpdate(enable);
    };
    SceneSessionManager::GetInstance().SetStatusBarEnabledChangeListener(func);
}

void JsSceneSessionManager::ProcessGestureNavigationEnabledChangeListener()
{
    ProcessGestureNavigationEnabledChangeFunc func = [this](bool enable) {
        WLOGFD("GestureNavigationEnabledUpdate");
        this->OnGestureNavigationEnabledUpdate(enable);
    };
    SceneSessionManager::GetInstance().SetGestureNavigationEnabledChangeListener(func);
}

void JsSceneSessionManager::ProcessOutsideDownEvent()
{
    ProcessOutsideDownEventFunc func = [this](int32_t x, int32_t y) {
        WLOGFD("ProcessOutsideDownEvent called");
        this->OnOutsideDownEvent(x, y);
    };
    SceneSessionManager::GetInstance().SetOutsideDownEventListener(func);
}

void JsSceneSessionManager::ProcessShiftFocus()
{
    ProcessShiftFocusFunc func = [this](int32_t persistentId) {
        TLOGD(WmsLogTag::WMS_FOCUS, "ProcessShiftFocus called");
        this->OnShiftFocus(persistentId);
    };
    NotifySCBAfterUpdateFocusFunc focusedCallback = [this]() {
        TLOGD(WmsLogTag::WMS_FOCUS, "scb uicontent focus");
        const auto& uiContent = RootScene::staticRootScene_->GetUIContent();
        if (uiContent == nullptr) {
            WLOGFE("[WMSComm]uiContent is nullptr");
            return;
        }
        uiContent->Focus();
    };
    NotifySCBAfterUpdateFocusFunc unfocusedCallback = [this]() {
        TLOGD(WmsLogTag::WMS_FOCUS, "scb uicontent unfocus");
        const auto& uiContent = RootScene::staticRootScene_->GetUIContent();
        if (uiContent == nullptr) {
            WLOGFE("[WMSComm]uiContent is nullptr");
            return;
        }
        uiContent->UnFocus();
    };
    SceneSessionManager::GetInstance().SetShiftFocusListener(func);
    SceneSessionManager::GetInstance().SetSCBFocusedListener(focusedCallback);
    SceneSessionManager::GetInstance().SetSCBUnfocusedListener(unfocusedCallback);
}

void JsSceneSessionManager::ProcessCallingSessionIdChangeRegister()
{
    ProcessCallingSessionIdChangeFunc func = [this](uint32_t callingSessionId) {
        WLOGFD("ProcessCallingSessionIdChangeRegister called, callingSessionId: %{public}d", callingSessionId);
        this->OnCallingSessionIdChange(callingSessionId);
    };
    SceneSessionManager::GetInstance().SetCallingSessionIdSessionListenser(func);
}

void JsSceneSessionManager::ProcessSwitchingToAnotherUserRegister()
{
    ProcessSwitchingToAnotherUserFunc func = [this]() {
        TLOGD(WmsLogTag::WMS_MULTI_USER, "SwitchingToAnotherUserFunc called");
        this->OnSwitchingToAnotherUser();
    };
    SceneSessionManager::GetInstance().SetSwitchingToAnotherUserListener(func);
}

void JsSceneSessionManager::OnSwitchingToAnotherUser()
{
    TLOGD(WmsLogTag::WMS_MULTI_USER, "[NAPI]On switching to another user");
    std::shared_ptr<NativeReference> jsCallBack = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(jsCbMapMutex_);
        auto iter = jsCbMap_.find(SWITCH_TO_ANOTHER_USER_CB);
        if (iter == jsCbMap_.end()) {
            return;
        }
        jsCallBack = iter->second;
    }
    auto task = [this, jsCallBack, env = env_]() {
        if (!jsCallBack) {
            TLOGE(WmsLogTag::WMS_MULTI_USER, "[NAPI]jsCallBack is nullptr");
            return;
        }
        napi_value argv[] = {};
        napi_call_function(env, NapiGetUndefined(env), jsCallBack->GetNapiValue(), 0, argv, nullptr);
    };
    taskScheduler_->PostMainThreadTask(task, "OnSwitchingToAnotherUser");
}

napi_value JsSceneSessionManager::RegisterCallback(napi_env env, napi_callback_info info)
{
    WLOGFD("[NAPI]RegisterCallback");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnRegisterCallback(env, info) : nullptr;
}

napi_value JsSceneSessionManager::UpdateFocus(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "UpdateFocus");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnUpdateFocus(env, info) : nullptr;
}

napi_value JsSceneSessionManager::ProcessBackEvent(napi_env env, napi_callback_info info)
{
    WLOGFI("[NAPI]ProcessBackEvent");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnProcessBackEvent(env, info) : nullptr;
}

napi_value JsSceneSessionManager::CheckSceneZOrder(napi_env env, napi_callback_info info)
{
    WLOGD("[NAPI]CheckSceneZOrder");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnCheckSceneZOrder(env, info) : nullptr;
}

napi_value JsSceneSessionManager::InitUserInfo(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_MAIN, "[NAPI]Init user info");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnInitUserInfo(env, info) : nullptr;
}

void JsSceneSessionManager::Finalizer(napi_env env, void* data, void* hint)
{
    WLOGI("[NAPI]Finalizer");
    std::unique_ptr<JsSceneSessionManager>(static_cast<JsSceneSessionManager*>(data));
}

napi_value JsSceneSessionManager::GetRootSceneSession(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]GetRootSceneSession");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnGetRootSceneSession(env, info) : nullptr;
}

napi_value JsSceneSessionManager::RequestSceneSession(napi_env env, napi_callback_info info)
{
    WLOGD("[NAPI]RequestSceneSession");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnRequestSceneSession(env, info) : nullptr;
}

napi_value JsSceneSessionManager::UpdateSceneSessionWant(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]UpdateSceneSessionWant");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnUpdateSceneSessionWant(env, info) : nullptr;
}

napi_value JsSceneSessionManager::RequestSceneSessionActivation(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_LIFE, "[NAPI]");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnRequestSceneSessionActivation(env, info) : nullptr;
}

napi_value JsSceneSessionManager::RequestSceneSessionBackground(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_LIFE, "[NAPI]");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnRequestSceneSessionBackground(env, info) : nullptr;
}

napi_value JsSceneSessionManager::RequestSceneSessionDestruction(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_LIFE, "[NAPI]");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnRequestSceneSessionDestruction(env, info) : nullptr;
}

napi_value JsSceneSessionManager::NotifyForegroundInteractiveStatus(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]NotifyForegroundInteractiveStatus");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnNotifyForegroundInteractiveStatus(env, info) : nullptr;
}

napi_value JsSceneSessionManager::IsSceneSessionValid(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_LIFE, "[NAPI]");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnIsSceneSessionValid(env, info) : nullptr;
}

napi_value JsSceneSessionManager::RequestSceneSessionByCall(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]RequestSceneSessionByCall");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnRequestSceneSessionByCall(env, info) : nullptr;
}

napi_value JsSceneSessionManager::StartAbilityBySpecified(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]StartAbilityBySpecified");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnStartAbilityBySpecified(env, info) : nullptr;
}

napi_value JsSceneSessionManager::StartUIAbilityBySCB(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]StartUIAbilityBySCB");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnStartUIAbilityBySCB(env, info) : nullptr;
}

napi_value JsSceneSessionManager::ChangeUIAbilityVisibilityBySCB(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]ChangeUIAbilityVisibilityBySCB");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnChangeUIAbilityVisibilityBySCB(env, info) : nullptr;
}

napi_value JsSceneSessionManager::GetWindowSceneConfig(napi_env env, napi_callback_info info)
{
    WLOGD("[NAPI]GetWindowSceneConfig");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnGetWindowSceneConfig(env, info) : nullptr;
}

napi_value JsSceneSessionManager::GetSessionSnapshotFilePath(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]GetSessionSnapshotFilePath");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnGetSessionSnapshotFilePath(env, info) : nullptr;
}

napi_value JsSceneSessionManager::InitWithRenderServiceAdded(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]InitWithRenderServiceAdded");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnInitWithRenderServiceAdded(env, info) : nullptr;
}

napi_value JsSceneSessionManager::SetSystemAnimatedScenes(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetSystemAnimatedScenes");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnSetSystemAnimatedScenes(env, info) : nullptr;
}

napi_value JsSceneSessionManager::GetAllAbilityInfos(napi_env env, napi_callback_info info)
{
    WLOGD("[NAPI]GetAllAbilityInfos");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnGetAllAbilityInfos(env, info) : nullptr;
}

napi_value JsSceneSessionManager::PrepareTerminate(napi_env env, napi_callback_info info)
{
    WLOGD("[NAPI]PrepareTerminate");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnPrepareTerminate(env, info) : nullptr;
}

napi_value JsSceneSessionManager::PerfRequestEx(napi_env env, napi_callback_info info)
{
    WLOGD("[NAPI]PerfRequestEx");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnPerfRequestEx(env, info) : nullptr;
}

napi_value JsSceneSessionManager::UpdateWindowMode(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]UpdateWindowMode");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnUpdateWindowMode(env, info) : nullptr;
}

napi_value JsSceneSessionManager::AddWindowDragHotArea(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]AddWindowDragHotArea");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnAddWindowDragHotArea(env, info) : nullptr;
}

napi_value JsSceneSessionManager::GetRootSceneUIContext(napi_env env, napi_callback_info info)
{
    WLOGD("[NAPI]GetRootSceneUIContext");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnGetRootSceneUIContext(env, info) : nullptr;
}

napi_value JsSceneSessionManager::SendTouchEvent(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SendTouchEvent");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnSendTouchEvent(env, info) : nullptr;
}

napi_value JsSceneSessionManager::PreloadInLakeApp(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]PreloadInLakeApp");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnPreloadInLakeApp(env, info) : nullptr;
}

napi_value JsSceneSessionManager::RequestFocusStatus(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "RequestFocusStatus");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnRequestFocusStatus(env, info) : nullptr;
}

napi_value JsSceneSessionManager::ResetFocusedOnShow(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "ResetFocusedOnShow");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnResetFocusedOnShow(env, info) : nullptr;
}

napi_value JsSceneSessionManager::RequestAllAppSessionUnfocus(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "[NAPI]RequestAllAppSessionUnfocus");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnRequestAllAppSessionUnfocus(env, info) : nullptr;
}

napi_value JsSceneSessionManager::SetScreenLocked(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]SetScreenLocked");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnSetScreenLocked(env, info) : nullptr;
}

napi_value JsSceneSessionManager::UpdateMaximizeMode(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]UpdateMaximizeMode");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnUpdateMaximizeMode(env, info) : nullptr;
}

napi_value JsSceneSessionManager::ReportData(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]ReportData");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnReportData(env, info) : nullptr;
}

napi_value JsSceneSessionManager::UpdateSessionDisplayId(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]UpdateSessionDisplayId");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnUpdateSessionDisplayId(env, info) : nullptr;
}

napi_value JsSceneSessionManager::NotifySwitchingToCurrentUser(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_MULTI_USER, "[NAPI]Notify switching to current user");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnNotifySwitchingToCurrentUser(env, info) : nullptr;
}

napi_value JsSceneSessionManager::UpdateConfig(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]UpdateConfig");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnUpdateConfig(env, info) : nullptr;
}

napi_value JsSceneSessionManager::NotifyAINavigationBarShowStatus(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]NotifyAINavigationBarShowStatus");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnNotifyAINavigationBarShowStatus(env, info) : nullptr;
}

napi_value JsSceneSessionManager::NotifySessionRecoverStatus(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]NotifySessionRecoverStatus");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnNotifySessionRecoverStatus(env, info) : nullptr;
}

napi_value JsSceneSessionManager::UpdateTitleInTargetPos(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]UpdateTitleInTargetPos");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnUpdateTitleInTargetPos(env, info) : nullptr;
}

napi_value JsSceneSessionManager::GetSessionSnapshotPixelMap(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]GetSessionSnapshotPixelMap");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnGetSessionSnapshotPixelMap(env, info) : nullptr;
}

napi_value JsSceneSessionManager::GetIsLayoutFullScreen(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]GetIsLayoutFullScreen");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnGetIsLayoutFullScreen(env, info) : nullptr;
}

napi_value JsSceneSessionManager::SwitchFreeMultiWindow(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "[NAPI]");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnSwitchFreeMultiWindow(env, info) : nullptr;
}

napi_value JsSceneSessionManager::GetFreeMultiWindowConfig(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::DEFAULT, "[NAPI]");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnGetFreeMultiWindowConfig(env, info) : nullptr;
}

bool JsSceneSessionManager::IsCallbackRegistered(napi_env env, const std::string& type, napi_value jsListenerObject)
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

napi_value JsSceneSessionManager::OnRegisterCallback(napi_env env, napi_callback_info info)
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
        WLOGFE("[NAPI]Callback is nullptr or not callable");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    if (IsCallbackRegistered(env, cbType, value)) {
        return NapiGetUndefined(env);
    }

    (this->*listenerFunc_[cbType])();
    std::shared_ptr<NativeReference> callbackRef;
    napi_ref result = nullptr;
    napi_create_reference(env, value, 1, &result);
    callbackRef.reset(reinterpret_cast<NativeReference*>(result));
    {
        std::unique_lock<std::shared_mutex> lock(jsCbMapMutex_);
        jsCbMap_[cbType] = callbackRef;
    }
    WLOGFD("[NAPI]Register end, type = %{public}s", cbType.c_str());
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnUpdateFocus(napi_env env, napi_callback_info info)
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
    int32_t persistentId;
    if (!ConvertFromJsValue(env, argv[0], persistentId)) {
        WLOGFE("[NAPI]Failed to convert parameter to persistentId");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isFocused;
    if (!ConvertFromJsValue(env, argv[1], isFocused)) {
        WLOGFE("[NAPI]Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    SceneSessionManager::GetInstance().UpdateFocus(persistentId, isFocused);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnProcessBackEvent(napi_env env, napi_callback_info info)
{
    SceneSessionManager::GetInstance().ProcessBackEvent();
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnCheckSceneZOrder(napi_env env, napi_callback_info info)
{
    SceneSessionManager::GetInstance().CheckSceneZOrder();
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnGetAllAbilityInfos(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_TWO || argc > ARGC_THREE) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    AAFwk::Want want;
    bool ret = OHOS::AppExecFwk::UnwrapWant(env, argv[0], want);
    if (!ret) {
        WLOGFE("[NAPI]Failed to convert parameter to want");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t userId;
    if (!ConvertFromJsValue(env, argv[1], userId)) {
        WLOGFE("[NAPI]Failed to convert parameter to userId");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto errCode = std::make_shared<int32_t>(static_cast<int32_t>(WSErrorCode::WS_OK));
    auto scbAbilityInfos = std::make_shared<std::vector<SCBAbilityInfo>>();
    auto execute = [obj = this, want, userId, infos = scbAbilityInfos, errCode] () {
        auto code = WS_JS_TO_ERROR_CODE_MAP.at(
            SceneSessionManager::GetInstance().GetAllAbilityInfos(want, userId, *infos));
        *errCode = static_cast<int32_t>(code);
    };
    auto complete = [obj = this, errCode, infos = scbAbilityInfos]
        (napi_env env, NapiAsyncTask &task, int32_t status) {
        if (*errCode != static_cast<int32_t>(WSErrorCode::WS_OK)) {
            std::string errMsg = "invalid params can not get All AbilityInfos!";
            task.RejectWithCustomize(env, CreateJsValue(env, *errCode), CreateJsValue(env, errMsg));
            return;
        }
        task.ResolveWithCustomize(env, CreateJsValue(env, static_cast<int32_t>(WSErrorCode::WS_OK)),
            obj->CreateAbilityInfos(env, *infos));
    };
    napi_value result = nullptr;
    napi_value callback = (argc == 2) ? nullptr : argv[2];
    NapiAsyncTask::Schedule("JsSceneSessionManager::OnGetAllAbilityInfos",
        env, CreateAsyncTaskWithLastParam(env, callback, std::move(execute), std::move(complete), &result));
    return result;
}

napi_value JsSceneSessionManager::CreateAbilityInfos(napi_env env,
    const std::vector<SCBAbilityInfo>& scbAbilityInfos)
{
    napi_value arrayValue = nullptr;
    napi_create_array_with_length(env, scbAbilityInfos.size(), &arrayValue);
    auto index = 0;
    for (const auto& scbAbilityInfo : scbAbilityInfos) {
        napi_set_element(env, arrayValue, index++, CreateSCBAbilityInfo(env, scbAbilityInfo));
    }
    return arrayValue;
}

napi_value JsSceneSessionManager::CreateSCBAbilityInfo(napi_env env, const SCBAbilityInfo& scbAbilityInfo)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("CreateObject failed");
        return NapiGetUndefined(env);
    }
    napi_set_named_property(env, objValue, "abilityItemInfo", CreateAbilityItemInfo(env, scbAbilityInfo.abilityInfo_));
    napi_set_named_property(env, objValue, "sdkVersion", CreateJsValue(env, scbAbilityInfo.sdkVersion_));
    return objValue;
}

napi_value JsSceneSessionManager::CreateAbilityItemInfo(napi_env env, const AppExecFwk::AbilityInfo& abilityInfo)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("CreateObject failed");
        return NapiGetUndefined(env);
    }
    napi_set_named_property(env, objValue, "appIconId", CreateJsValue(env, abilityInfo.iconId));
    napi_set_named_property(env, objValue, "appLabelId", CreateJsValue(env, abilityInfo.labelId));
    napi_set_named_property(env, objValue, "bundleName", CreateJsValue(env, abilityInfo.bundleName));
    napi_set_named_property(env, objValue, "moduleName", CreateJsValue(env, abilityInfo.moduleName));
    napi_set_named_property(env, objValue, "name", CreateJsValue(env, abilityInfo.name));
    napi_set_named_property(env, objValue, "launchType",
        CreateJsValue(env, static_cast<int32_t>(abilityInfo.launchMode)));
    napi_set_named_property(env, objValue, "supportWindowModes", CreateWindowModes(env, abilityInfo.windowModes));
    napi_set_named_property(env, objValue, "windowSize", CreateWindowSize(env, abilityInfo));
    napi_set_named_property(env, objValue, "orientation",
        CreateJsValue(env, static_cast<int32_t>(abilityInfo.orientation)));
    napi_set_named_property(env, objValue, "excludeFromSession", CreateJsValue(env, abilityInfo.excludeFromMissions));
    napi_set_named_property(env, objValue, "unclearableSession", CreateJsValue(env, abilityInfo.unclearableMission));
    napi_set_named_property(env, objValue, "continuable", CreateJsValue(env, abilityInfo.continuable));
    napi_set_named_property(env, objValue, "removeSessionAfterTerminate",
        CreateJsValue(env, abilityInfo.removeMissionAfterTerminate));
    napi_set_named_property(env, objValue, "preferMultiWindowOrientation",
        CreateJsValue(env, abilityInfo.preferMultiWindowOrientation));
    return objValue;
}

napi_value JsSceneSessionManager::CreateWindowModes(napi_env env,
    const std::vector<AppExecFwk::SupportWindowMode>& windowModes)
{
    napi_value arrayValue = nullptr;
    napi_create_array_with_length(env, windowModes.size(), &arrayValue);
    auto index = 0;
    for (const auto& windowMode : windowModes) {
        napi_set_element(env, arrayValue, index++, CreateJsValue(env, static_cast<int32_t>(windowMode)));
    }
    return arrayValue;
}

napi_value JsSceneSessionManager::CreateWindowSize(napi_env env, const AppExecFwk::AbilityInfo& abilityInfo)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("CreateObject failed");
        return NapiGetUndefined(env);
    }
    napi_set_named_property(env, objValue, "maxWindowRatio", CreateJsValue(env, abilityInfo.maxWindowRatio));
    napi_set_named_property(env, objValue, "minWindowRatio", CreateJsValue(env, abilityInfo.minWindowRatio));
    napi_set_named_property(env, objValue, "maxWindowWidth", CreateJsValue(env, abilityInfo.maxWindowWidth));
    napi_set_named_property(env, objValue, "minWindowWidth", CreateJsValue(env, abilityInfo.minWindowWidth));
    napi_set_named_property(env, objValue, "maxWindowHeight", CreateJsValue(env, abilityInfo.maxWindowHeight));
    napi_set_named_property(env, objValue, "minWindowHeight", CreateJsValue(env, abilityInfo.minWindowHeight));
    return objValue;
}

napi_value JsSceneSessionManager::OnInitUserInfo(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_TWO) {
        TLOGE(WmsLogTag::WMS_MAIN, "[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t userId;
    if (!ConvertFromJsValue(env, argv[0], userId)) { // 1: params num
        TLOGE(WmsLogTag::WMS_MAIN, "[NAPI]Failed to convert parameter to userId");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    std::string fileDir;
    if (!ConvertFromJsValue(env, argv[1], fileDir)) { // 2: params num
        TLOGE(WmsLogTag::WMS_MAIN, "NAPI]Failed to convert parameter to fileDir");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    WSErrorCode ret =
        WS_JS_TO_ERROR_CODE_MAP.at(SceneSessionManager::GetInstance().InitUserInfo(userId, fileDir));
    if (ret != WSErrorCode::WS_OK) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY),
            "System is abnormal"));
    }
    return NapiGetUndefined(env);
}

void JsSceneSessionManager::RegisterDumpRootSceneElementInfoListener()
{
    DumpRootSceneElementInfoFunc func = [this](const std::vector<std::string>& params,
        std::vector<std::string>& infos) {
        if (params.size() == 1 && params[0] == ARG_DUMP_HELP) { // 1 params num
            Ace::UIContent::ShowDumpHelp(infos);
            WLOGFD("Dump ArkUI help info");
        } else if (RootScene::staticRootScene_->GetUIContent()) {
            RootScene::staticRootScene_->GetUIContent()->DumpInfo(params, infos);
            WLOGFD("Dump ArkUI element info");
        }
    };
    SceneSessionManager::GetInstance().SetDumpRootSceneElementInfoListener(func);
}

void JsSceneSessionManager::RegisterVirtualPixelRatioChangeListener()
{
    ProcessVirtualPixelRatioChangeFunc func = [this](float density, const Rect& rect) {
        WLOGFI("VirtualPixelRatioChangeListener %{public}d,%{public}d,%{public}d,%{public}d; %{public}f",
            rect.posX_, rect.posY_, rect.width_, rect.height_, density);
        RootScene::staticRootScene_->SetDisplayDensity(density);
        RootScene::staticRootScene_->UpdateViewportConfig(rect, WindowSizeChangeReason::UNDEFINED);
    };
    SceneSessionManager::GetInstance().SetVirtualPixelRatioChangeListener(func);
}

napi_value JsSceneSessionManager::OnGetRootSceneSession(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]OnGetRootSceneSession");
    sptr<RootSceneSession> rootSceneSession = SceneSessionManager::GetInstance().GetRootSceneSession();
    if (rootSceneSession == nullptr) {
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY), "System is abnormal"));
        return NapiGetUndefined(env);
    }

    if (rootScene_ == nullptr) {
        rootScene_ = new RootScene();
    }
    RootScene::staticRootScene_ = rootScene_;
    RegisterDumpRootSceneElementInfoListener();
    RegisterVirtualPixelRatioChangeListener();
    rootSceneSession->SetLoadContentFunc([rootScene = rootScene_]
        (const std::string& contentUrl, napi_env env, napi_value storage, AbilityRuntime::Context* context) {
            rootScene->LoadContent(contentUrl, env, storage, context);
            ScenePersistentStorage::InitDir(context->GetPreferencesDir());
            SceneSessionManager::GetInstance().InitPersistentStorage();
        });
    rootScene_->SetFrameLayoutFinishCallback([]() {
        SceneSessionManager::GetInstance().NotifyUpdateRectAfterLayout();
        SceneSessionManager::GetInstance().FlushWindowInfoToMMI();
    });
    RootScene::SetOnConfigurationUpdatedCallback([](const std::shared_ptr<AppExecFwk::Configuration>& configuration) {
        SceneSessionManager::GetInstance().OnConfigurationUpdated(configuration);
    });
    napi_value jsRootSceneSessionObj = JsRootSceneSession::Create(env, rootSceneSession);
    if (jsRootSceneSessionObj == nullptr) {
        WLOGFE("[NAPI]jsRootSceneSessionObj is nullptr");
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY), "System is abnormal"));
        return NapiGetUndefined(env);
    }
    return jsRootSceneSessionObj;
}

napi_value JsSceneSessionManager::OnRequestSceneSession(napi_env env, napi_callback_info info)
{
    WLOGD("[NAPI]OnRequestSceneSession");
    WSErrorCode errCode = WSErrorCode::WS_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
    }

    SessionInfo sessionInfo;
    AAFwk::Want want;
    if (errCode == WSErrorCode::WS_OK) {
        napi_value nativeObj = argv[0];
        if (nativeObj == nullptr) {
            WLOGFE("[NAPI]Failed to convert object to session info");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        } else if (!ConvertSessionInfoFromJs(env, nativeObj, sessionInfo)) {
            WLOGFE("[NAPI]Failed to get session info from js object");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        }
    }
    if (errCode == WSErrorCode::WS_ERROR_INVALID_PARAM) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    if (argc == ARGC_TWO && GetType(env, argv[1]) != napi_undefined) {
        OHOS::AppExecFwk::UnwrapWant(env, argv[1], want);
        sessionInfo.want = std::make_shared<AAFwk::Want>(want);
    }

    WLOGFI("[NAPI]SessionInfo [%{public}s, %{public}s, %{public}s], errCode = %{public}d",
        sessionInfo.bundleName_.c_str(), sessionInfo.moduleName_.c_str(), sessionInfo.abilityName_.c_str(), errCode);
    sptr<SceneSession> sceneSession = SceneSessionManager::GetInstance().RequestSceneSession(sessionInfo);
    if (sceneSession == nullptr) {
        napi_throw(env,
            CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY), "System is abnormal"));
        return NapiGetUndefined(env);
    } else {
        napi_value jsSceneSessionObj = JsSceneSession::Create(env, sceneSession);
        if (jsSceneSessionObj == nullptr) {
            WLOGFE("jsSceneSessionObj is nullptr");
            napi_throw(env, CreateJsError(
                env, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY), "System is abnormal"));
        }
        return jsSceneSessionObj;
    }
}

napi_value JsSceneSessionManager::OnUpdateSceneSessionWant(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]OnUpdateSceneSessionWant");
    WSErrorCode errCode = WSErrorCode::WS_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_TWO) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
    }

    SessionInfo sessionInfo;
    AAFwk::Want want;
    if (errCode == WSErrorCode::WS_OK) {
        napi_value nativeObj = argv[0];
        if (nativeObj == nullptr) {
            WLOGFE("[NAPI]Failed to convert object to session info");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        } else if (!ConvertSessionInfoFromJs(env, nativeObj, sessionInfo)) {
            WLOGFE("[NAPI]Failed to get session info from js object");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        } else if (!OHOS::AppExecFwk::UnwrapWant(env, argv[1], want)) {
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        }
    }
    if (errCode == WSErrorCode::WS_ERROR_INVALID_PARAM) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    sessionInfo.want = std::make_shared<AAFwk::Want>(want);

    WLOGFI("[NAPI]SessionInfo [%{public}s, %{public}s, %{public}s, %{public}d]", sessionInfo.bundleName_.c_str(),
        sessionInfo.moduleName_.c_str(), sessionInfo.abilityName_.c_str(), sessionInfo.persistentId_);
    SceneSessionManager::GetInstance().UpdateSceneSessionWant(sessionInfo);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnRequestSceneSessionActivation(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[NAPI]");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_TWO) { // 2: params num
        TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM), "InputInvalid"));
        return NapiGetUndefined(env);
    }
    napi_value jsSceneSessionObj = argv[0];
    if (jsSceneSessionObj == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]Failed to get js scene session object");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM), "InputInvalid"));
        return NapiGetUndefined(env);
    }
    void* pointerResult = nullptr;
    napi_unwrap(env, jsSceneSessionObj, &pointerResult);
    auto jsSceneSession = static_cast<JsSceneSession*>(pointerResult);
    if (jsSceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]Failed to get scene session from js object");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM), "InputInvalid"));
        return NapiGetUndefined(env);
    }
    sptr<SceneSession> sceneSession = jsSceneSession->GetNativeSession();
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]sceneSession is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_SYSTEM_ABNORMALLY),
            "sceneSession is nullptr"));
        return NapiGetUndefined(env);
    }

    bool isNewActive = true;
    ConvertFromJsValue(env, argv[1], isNewActive);

    SceneSessionManager::GetInstance().RequestSceneSessionActivation(sceneSession, isNewActive);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnRequestSceneSessionBackground(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[NAPI]");
    WSErrorCode errCode = WSErrorCode::WS_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) { // 1: params num
        TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]Argc is invalid: %{public}zu", argc);
        errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
    }

    sptr<SceneSession> sceneSession = nullptr;
    if (errCode == WSErrorCode::WS_OK) {
        napi_value jsSceneSessionObj = argv[0];
        if (jsSceneSessionObj == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]Failed to get js scene session object");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        } else {
            void* pointerResult = nullptr;
            napi_unwrap(env, jsSceneSessionObj, &pointerResult);
            auto jsSceneSession = static_cast<JsSceneSession*>(pointerResult);
            if (jsSceneSession == nullptr) {
                TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]Failed to get scene session from js object");
                errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
            } else {
                sceneSession = jsSceneSession->GetNativeSession();
            }
        }
    }
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]sceneSession is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_SYSTEM_ABNORMALLY),
            "sceneSession is nullptr"));
        return NapiGetUndefined(env);
    }

    if (errCode == WSErrorCode::WS_ERROR_INVALID_PARAM) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool isDelegator = false;
    if (argc >= ARGC_TWO && GetType(env, argv[ARGC_ONE]) == napi_boolean) {
        ConvertFromJsValue(env, argv[ARGC_ONE], isDelegator);
        TLOGI(WmsLogTag::WMS_LIFE, "[NAPI]isDelegator: %{public}u", isDelegator);
    }

    bool isToDesktop = false;
    if (argc >= ARGC_THREE && GetType(env, argv[ARGC_TWO]) == napi_boolean) {
        ConvertFromJsValue(env, argv[ARGC_TWO], isToDesktop);
        TLOGI(WmsLogTag::WMS_LIFE, "[NAPI]isToDesktop: %{public}u", isToDesktop);
    }

    bool isSaveSnapshot = true;
    if (argc >= ARGC_FOUR && GetType(env, argv[ARGC_THREE]) == napi_boolean) {
        ConvertFromJsValue(env, argv[ARGC_THREE], isSaveSnapshot);
        TLOGI(WmsLogTag::WMS_LIFE, "[NAPI]isSaveSnapshot: %{public}u", isSaveSnapshot);
    }

    SceneSessionManager::GetInstance().RequestSceneSessionBackground(sceneSession, isDelegator, isToDesktop,
        isSaveSnapshot);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnRequestSceneSessionDestruction(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[NAPI]");
    WSErrorCode errCode = WSErrorCode::WS_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]Argc is invalid: %{public}zu", argc);
        errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
    }

    bool needRemoveSession = false;
    if (argc == ARGC_TWO && GetType(env, argv[1]) == napi_boolean) {
        ConvertFromJsValue(env, argv[1], needRemoveSession);
        TLOGI(WmsLogTag::WMS_LIFE, "[NAPI]needRemoveSession: %{public}u", needRemoveSession);
    }

    sptr<SceneSession> sceneSession = nullptr;
    JsSceneSession* jsSceneSession;
    if (errCode == WSErrorCode::WS_OK) {
        napi_value jsSceneSessionObj = argv[0];
        if (jsSceneSessionObj == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]Failed to get js scene session object");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        } else {
            void* pointerResult = nullptr;
            napi_unwrap(env, jsSceneSessionObj, &pointerResult);
            jsSceneSession = static_cast<JsSceneSession*>(pointerResult);
            if (jsSceneSession == nullptr) {
                TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]Failed to get scene session from js object");
                errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
            } else {
                sceneSession = jsSceneSession->GetNativeSession();
                SetIsClearSession(env, jsSceneSessionObj, sceneSession);
            }
        }
    }
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]sceneSession is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_SYSTEM_ABNORMALLY),
            "sceneSession is nullptr"));
        return NapiGetUndefined(env);
    }

    if (errCode == WSErrorCode::WS_ERROR_INVALID_PARAM) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    SceneSessionManager::GetInstance().RequestSceneSessionDestruction(sceneSession, needRemoveSession);
    auto localScheduler = SceneSessionManager::GetInstance().GetTaskScheduler();
    auto clearTask = [jsSceneSession, needRemoveSession, persistentId = sceneSession->GetPersistentId()] () {
        if (jsSceneSession != nullptr) {
            jsSceneSession->ClearCbMap(needRemoveSession, persistentId);
        }
    };
    localScheduler->PostAsyncTask(clearTask, "Clear callback Map");
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnNotifyForegroundInteractiveStatus(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]OnNotifyForegroundInteractiveStatus");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_TWO) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM), "InputInvalid"));
        return NapiGetUndefined(env);
    }

    napi_value jsSceneSessionObj = argv[0];
    if (jsSceneSessionObj == nullptr) {
        WLOGFE("[NAPI]Failed to get js scene session object");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM), "InputInvalid"));
        return NapiGetUndefined(env);
    }
    void* pointerResult = nullptr;
    napi_unwrap(env, jsSceneSessionObj, &pointerResult);
    auto jsSceneSession = static_cast<JsSceneSession*>(pointerResult);
    if (jsSceneSession == nullptr) {
        WLOGFE("[NAPI]Failed to get scene session from js object");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM), "InputInvalid"));
        return NapiGetUndefined(env);
    }
    sptr<SceneSession> sceneSession = jsSceneSession->GetNativeSession();
    if (sceneSession == nullptr) {
        WLOGFE("[NAPI]sceneSession is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_SYSTEM_ABNORMALLY),
            "sceneSession is nullptr"));
        return NapiGetUndefined(env);
    }

    bool interactive = true;
    ConvertFromJsValue(env, argv[1], interactive);

    SceneSessionManager::GetInstance().NotifyForegroundInteractiveStatus(sceneSession, interactive);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnIsSceneSessionValid(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[NAPI]");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM), "InputInvalid"));
        return NapiGetUndefined(env);
    }
    napi_value jsSceneSessionObj = argv[0];
    if (jsSceneSessionObj == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]Failed to get js scene session object");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM), "InputInvalid"));
        return NapiGetUndefined(env);
    }
    void* pointerResult = nullptr;
    napi_unwrap(env, jsSceneSessionObj, &pointerResult);
    auto jsSceneSession = static_cast<JsSceneSession*>(pointerResult);
    if (jsSceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]Failed to get scene session from js object");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM), "InputInvalid"));
        return NapiGetUndefined(env);
    }
    napi_value result = nullptr;
    sptr<SceneSession> sceneSession = jsSceneSession->GetNativeSession();
    napi_get_boolean(env, sceneSession != nullptr, &result);
    return result;
}

void JsSceneSessionManager::SetIsClearSession(napi_env env, napi_value jsSceneSessionObj,
    sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("[NAPI]SetIsClearSession: sceneSession is nullptr");
        return;
    }
    napi_value jsOperatorType = nullptr;
    napi_get_named_property(env, jsSceneSessionObj, "operatorType", &jsOperatorType);
    if (GetType(env, jsOperatorType) != napi_undefined) {
        int32_t operatorType = -1;
        if (ConvertFromJsValue(env, jsOperatorType, operatorType)) {
            WLOGFI("[NAPI]operatorType: %{public}d", operatorType);
            if (operatorType == SessionOperationType::TYPE_CLEAR) {
                sceneSession->SetSessionInfoIsClearSession(true);
            }
        }
    }
}

napi_value JsSceneSessionManager::OnRequestSceneSessionByCall(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]OnRequestSceneSessionByCall");
    WSErrorCode errCode = WSErrorCode::WS_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
    }

    sptr<SceneSession> sceneSession = nullptr;
    if (errCode == WSErrorCode::WS_OK) {
        napi_value jsSceneSessionObj = argv[0];
        if (jsSceneSessionObj == nullptr) {
            WLOGFE("[NAPI]Failed to get js scene session object");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        } else {
            void* pointerResult = nullptr;
            napi_unwrap(env, jsSceneSessionObj, &pointerResult);
            auto jsSceneSession = static_cast<JsSceneSession*>(pointerResult);
            if (jsSceneSession == nullptr) {
                WLOGFE("[NAPI]Failed to get scene session from js object");
                errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
            } else {
                sceneSession = jsSceneSession->GetNativeSession();
            }
        }
    }
    if (sceneSession == nullptr) {
        WLOGFE("[NAPI]sceneSession is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_SYSTEM_ABNORMALLY),
            "sceneSession is nullptr"));
        return NapiGetUndefined(env);
    }

    if (errCode == WSErrorCode::WS_ERROR_INVALID_PARAM) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    SceneSessionManager::GetInstance().RequestSceneSessionByCall(sceneSession);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnStartAbilityBySpecified(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]OnStartAbilityBySpecified");
    WSErrorCode errCode = WSErrorCode::WS_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
    }

    SessionInfo sessionInfo;
    if (errCode == WSErrorCode::WS_OK) {
        napi_value nativeObj = argv[0];
        if (nativeObj == nullptr) {
            WLOGFE("[NAPI]Failed to convert object to session info");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        } else if (!ConvertSessionInfoFromJs(env, nativeObj, sessionInfo)) {
            WLOGFE("[NAPI]Failed to get session info from js object");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        }
    }

    if (errCode == WSErrorCode::WS_ERROR_INVALID_PARAM) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    AAFwk::Want want;
    if (OHOS::AppExecFwk::UnwrapWant(env, argv[1], want)) {
        sessionInfo.want = std::make_shared<AAFwk::Want>(want);
    }

    WLOGFI("[NAPI]SessionInfo [%{public}s, %{public}s, %{public}s], errCode = %{public}d",
        sessionInfo.bundleName_.c_str(), sessionInfo.moduleName_.c_str(), sessionInfo.abilityName_.c_str(), errCode);
    SceneSessionManager::GetInstance().StartAbilityBySpecified(sessionInfo);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnStartUIAbilityBySCB(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]OnStartUIAbilityBySCB");
    WSErrorCode errCode = WSErrorCode::WS_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
    }

    sptr<SceneSession> sceneSession = nullptr;
    if (errCode == WSErrorCode::WS_OK) {
        napi_value nativeObj = argv[0];
        if (nativeObj == nullptr) {
            WLOGFE("[NAPI]Failed to convert object to session info");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        } else {
            void* pointerResult = nullptr;
            napi_unwrap(env, nativeObj, &pointerResult);
            auto jsSceneSession = static_cast<JsSceneSession*>(pointerResult);
            if (jsSceneSession == nullptr) {
                errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
            } else {
                sceneSession = jsSceneSession->GetNativeSession();
            }
        }
    }

    if (sceneSession == nullptr) {
        WLOGFE("[NAPI]sceneSession is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_SYSTEM_ABNORMALLY),
            "sceneSession is nullptr"));
        return NapiGetUndefined(env);
    }

    if (errCode == WSErrorCode::WS_ERROR_INVALID_PARAM) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    SceneSessionManager::GetInstance().StartUIAbilityBySCB(sceneSession);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnChangeUIAbilityVisibilityBySCB(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]OnChangeUIAbilityVisibilityBySCB");
    WSErrorCode errCode = WSErrorCode::WS_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
    }

    sptr<SceneSession> sceneSession = nullptr;
    if (errCode == WSErrorCode::WS_OK) {
        napi_value nativeObj = argv[0];
        if (nativeObj == nullptr) {
            WLOGFE("[NAPI]Failed to convert object to session info");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        } else {
            void* pointerResult = nullptr;
            napi_unwrap(env, nativeObj, &pointerResult);
            auto jsSceneSession = static_cast<JsSceneSession*>(pointerResult);
            if (jsSceneSession == nullptr) {
                errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
            } else {
                sceneSession = jsSceneSession->GetNativeSession();
            }
        }
    }

    if (sceneSession == nullptr) {
        WLOGFE("[NAPI]sceneSession is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_SYSTEM_ABNORMALLY),
            "sceneSession is nullptr"));
        return NapiGetUndefined(env);
    }

    if (errCode == WSErrorCode::WS_ERROR_INVALID_PARAM) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    bool visibility = true;
    ConvertFromJsValue(env, argv[1], visibility);

    SceneSessionManager::GetInstance().ChangeUIAbilityVisibilityBySCB(sceneSession, visibility);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnGetWindowSceneConfig(napi_env env, napi_callback_info info)
{
    WLOGFD("[NAPI]OnGetWindowSceneConfig");
    const AppWindowSceneConfig& windowSceneConfig = SceneSessionManager::GetInstance().GetWindowSceneConfig();
    napi_value jsWindowSceneConfigObj = JsWindowSceneConfig::CreateWindowSceneConfig(env, windowSceneConfig);
    if (jsWindowSceneConfigObj == nullptr) {
        WLOGFE("[NAPI]jsWindowSceneConfigObj is nullptr");
        napi_throw(env, CreateJsError(env,
            static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY), "System is abnormal"));
    }
    return jsWindowSceneConfigObj;
}

napi_value JsSceneSessionManager::OnGetSessionSnapshotFilePath(napi_env env, napi_callback_info info)
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
    int32_t persistentId;
    if (!ConvertFromJsValue(env, argv[0], persistentId)) {
        WLOGFE("[NAPI]Failed to convert parameter to persistentId");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    std::string path = SceneSessionManager::GetInstance().GetSessionSnapshotFilePath(persistentId);
    napi_value result = nullptr;
    napi_create_string_utf8(env, path.c_str(), path.length(), &result);
    return result;
}

napi_value JsSceneSessionManager::OnInitWithRenderServiceAdded(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]OnInitWithRenderServiceAdded");
    SceneSessionManager::GetInstance().InitWithRenderServiceAdded();
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnSetSystemAnimatedScenes(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
        "Input parameter is missing."));
        return NapiGetUndefined(env);
    }
    uint32_t sceneCode;
    if (!ConvertFromJsValue(env, argv[0], sceneCode)) {
        WLOGFE("[NAPI]Faile to convert parameter to sceneCode.");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is invalid."));
        return NapiGetUndefined(env);
    }

    SystemAnimatedSceneType sceneType = static_cast<SystemAnimatedSceneType>(sceneCode);
    WMError ret = SceneSessionManager::GetInstance().SetSystemAnimatedScenes(sceneType);
    if (ret != WMError::WM_OK) {
        WmErrorCode wmErrorCode = WM_JS_TO_ERROR_CODE_MAP.at(ret);
        WLOGFE("[NAPI]Set system animated scene failed, return %{public}d", wmErrorCode);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(wmErrorCode),
            "Set system animated scene failed."));
        return NapiGetUndefined(env);
    }
    WLOGFE("[NAPI]Set system animated scene succeed, return WmErrorCode::WM_OK");
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnPrepareTerminate(napi_env env, napi_callback_info info)
{
    WLOGI("[NAPI]OnPrepareTerminate");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t persistentId;
    if (!ConvertFromJsValue(env, argv[0], persistentId)) {
        WLOGFE("[NAPI]Failed to convert parameter to persistentId");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isPrepareTerminate = false;
    SceneSessionManager::GetInstance().PrepareTerminate(persistentId, isPrepareTerminate);
    napi_value result = nullptr;
    napi_get_boolean(env, isPrepareTerminate, &result);
    return result;
}

napi_value JsSceneSessionManager::OnPerfRequestEx(napi_env env, napi_callback_info info)
{
    WLOGD("[NAPI]OnPerfRequestEx");
#ifdef SOC_PERF_ENABLE
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_TWO) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t cmdId;
    bool onOffTag = false;
    if (!ConvertFromJsValue(env, argv[0], cmdId) || !ConvertFromJsValue(env, argv[1], onOffTag)) {
        WLOGFE("[NAPI]Failed to convert parameter to cmdId or onOffTag");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    std::string msg = "";
    if (argc == ARGC_THREE) {
        if (!ConvertFromJsValue(env, argv[ARGC_TWO], msg)) {
            WLOGFE("[NAPI]Failed to convert parameter to cmd msg");
            napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                "Input parameter is missing or invalid"));
            return NapiGetUndefined(env);
        }
    }
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(cmdId, onOffTag, msg);
    WLOGFD("[NAPI]PerfRequestEx success cmdId: %{public}d onOffTag: %{public}u msg:%{public}s",
        cmdId, static_cast<uint32_t>(onOffTag), msg.c_str());
#endif
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnUpdateWindowMode(napi_env env, napi_callback_info info)
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
    int32_t persistentId;
    if (!ConvertFromJsValue(env, argv[0], persistentId)) {
        WLOGFE("[NAPI]Failed to convert parameter to persistentId");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t windowMode;
    if (!ConvertFromJsValue(env, argv[1], windowMode)) {
        WLOGFE("[NAPI]Failed to convert parameter to windowMode");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    SceneSessionManager::GetInstance().UpdateWindowMode(persistentId, windowMode);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnAddWindowDragHotArea(napi_env env, napi_callback_info info)
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
    uint32_t type;
    if (!ConvertFromJsValue(env, argv[0], type)) {
        WLOGFE("[NAPI]Failed to convert parameter to type");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    WSRect area;
    napi_value nativeObj = argv[1];
    if (nativeObj == nullptr || !ConvertRectInfoFromJs(env, nativeObj, area)) {
        WLOGFE("[NAPI]Failed to convert parameter to area");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    SceneSessionManager::GetInstance().AddWindowDragHotArea(type, area);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnGetRootSceneUIContext(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc >= 1) {
        WLOGFE("Argc is invalid: %{public}zu, expect zero params", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    if (RootScene::staticRootScene_ == nullptr) {
        WLOGFE("Root scene is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return NapiGetUndefined(env);
    }

    const auto& uiContent = RootScene::staticRootScene_->GetUIContent();
    if (uiContent == nullptr) {
        WLOGFE("uiContent is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return NapiGetUndefined(env);
    }
    napi_value uiContext = uiContent->GetUINapiContext();
    if (uiContext == nullptr) {
        WLOGFE("uiContext obtained from jsEngine is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return NapiGetUndefined(env);
    }
    WLOGFD("OnGetRootSceneUIContext success");
    return uiContext;
}

napi_value JsSceneSessionManager::OnSendTouchEvent(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != ARGC_TWO) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    napi_value nativeObj = argv[0];
    if (nativeObj == nullptr) {
        WLOGFE("[NAPI]Failed to convert object");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto pointerEvent = MMI::PointerEvent::Create();
    if (!ConvertPointerEventFromJs(env, nativeObj, *pointerEvent)) {
        WLOGFE("[NAPI]Failed to convert pointer event");
        return NapiGetUndefined(env);
    }
    uint32_t zIndex;
    if (!ConvertFromJsValue(env, argv[1], zIndex)) {
        WLOGFE("[NAPI]Failed to convert parameter to zIndex");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    SceneSessionManager::GetInstance().SendTouchEvent(pointerEvent, zIndex);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnPreloadInLakeApp(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != ARGC_ONE) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    std::string bundleName = "";
    if (!ConvertFromJsValue(env, argv[0], bundleName)) {
        WLOGFE("[NAPI]Failed to convert parameter to bundleName");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    auto localScheduler = SceneSessionManager::GetInstance().GetTaskScheduler();
    auto preloadTask = [bundleName] () {
        SceneSessionManager::GetInstance().PreloadInLakeApp(bundleName);
    };
    localScheduler->PostAsyncTask(preloadTask);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnRequestFocusStatus(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < MIN_ARG_COUNT) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t persistentId;
    if (!ConvertFromJsValue(env, argv[0], persistentId)) {
        WLOGFE("[NAPI]Failed to convert parameter to persistentId");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isFocused = false;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_1], isFocused)) {
        WLOGFE("[NAPI]Failed to convert parameter to isFocused");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool byForeground = false;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_TWO], byForeground)) {
        WLOGFE("[NAPI]Failed to convert parameter to byForeground");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    FocusChangeReason reason = FocusChangeReason::DEFAULT;
    if (argc > MIN_ARG_COUNT) {
        if (!ConvertFromJsValue(env, argv[ARG_INDEX_3], reason)) {
            TLOGI(WmsLogTag::WMS_FOCUS, "[NAPI]Failed to convert parameter to reason");
            napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                "Input parameter is missing or invalid"));
            return NapiGetUndefined(env);
        }
    }
    TLOGI(WmsLogTag::WMS_FOCUS, "[NAPI]OnRequestFocusStatus persistentId: %{public}d, isFocused: %{public}d, "
        "byForeground: %{public}d, reason: %{public}d", persistentId, isFocused, byForeground, reason);

    SceneSessionManager::GetInstance().RequestFocusStatus(persistentId, isFocused, byForeground, reason);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnResetFocusedOnShow(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != ARGC_ONE) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t persistentId;
    if (!ConvertFromJsValue(env, argv[0], persistentId)) {
        WLOGFE("[NAPI]Failed to convert parameter to persistentId");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    SceneSessionManager::GetInstance().ResetFocusedOnShow(persistentId);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnRequestAllAppSessionUnfocus(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc >= ARGC_ONE) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    SceneSessionManager::GetInstance().RequestAllAppSessionUnfocus();
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnSetScreenLocked(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != ARGC_ONE) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isScreenLocked = false;
    if (!ConvertFromJsValue(env, argv[0], isScreenLocked)) {
        WLOGFE("[NAPI]Failed to convert parameter to isScreenLocked");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    SceneSessionManager::GetInstance().SetScreenLocked(isScreenLocked);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnUpdateMaximizeMode(napi_env env, napi_callback_info info)
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
    int32_t persistentId;
    if (!ConvertFromJsValue(env, argv[0], persistentId)) {
        WLOGFE("[NAPI]Failed to convert parameter to persistentId");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isMaximize = false;
    if (!ConvertFromJsValue(env, argv[1], isMaximize)) {
        WLOGFE("[NAPI]Failed to convert parameter to isMaximize");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    SceneSessionManager::GetInstance().UpdateMaximizeMode(persistentId, isMaximize);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnUpdateSessionDisplayId(napi_env env, napi_callback_info info)
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
    int32_t persistentId;
    if (!ConvertFromJsValue(env, argv[0], persistentId)) {
        WLOGFE("[NAPI]Failed to convert parameter to persistentId");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t screenId;
    if (!ConvertFromJsValue(env, argv[1], screenId)) {
        WLOGFE("[NAPI]Failed to convert parameter to screenId");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    SceneSessionManager::GetInstance().UpdateSessionDisplayId(persistentId, screenId);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnNotifySwitchingToCurrentUser(napi_env env, napi_callback_info info)
{
    SceneSessionManager::GetInstance().NotifySwitchingToCurrentUser();
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnUpdateConfig(napi_env env, napi_callback_info info)
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
    SessionInfo sessionInfo;
    AppExecFwk::Configuration config;
    bool informAllApp = false;
    napi_value nativeObj = argv[0];
    if (!ConvertSessionInfoFromJs(env, nativeObj, sessionInfo)) {
        WLOGFE("[NAPI]Failed to convert parameter to sessionInfo");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    if (!ConvertConfigurationFromJs(env, argv[1], config)) {
        WLOGFE("[NAPI]Failed to convert parameter to config");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_TWO], informAllApp)) {
        WLOGFE("[NAPI]Failed to convert parameter to informAllApp");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    SceneSessionManager::GetInstance().UpdateConfig(sessionInfo, config, informAllApp);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnNotifyAINavigationBarShowStatus(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_THREE) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isVisible = false;
    if (!ConvertFromJsValue(env, argv[0], isVisible)) {
        WLOGFE("[NAPI]Failed to convert parameter to isVisible");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    WSRect barArea;
    if (argv[1] == nullptr || !ConvertRectInfoFromJs(env, argv[1], barArea)) {
        WLOGFE("[NAPI]Failed to convert parameter to barArea");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int64_t displayId = -1;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_TWO], displayId)) {
        TLOGE(WmsLogTag::WMS_LIFE, "[NAPI]Failed to convert parameter to displayId");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    SceneSessionManager::GetInstance().NotifyAINavigationBarShowStatus(
        isVisible, barArea, static_cast<uint64_t>(displayId));
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnNotifySessionRecoverStatus(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != ARGC_TWO) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isRecovering = false;
    if (!ConvertFromJsValue(env, argv[0], isRecovering)) {
        WLOGFE("[NAPI]Failed to convert parameter to isRecovering");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    WLOGFD("[NAPI]IsRecovering: %{public}u", isRecovering);
    // Recovered sessions persistentId list as second argument
    std::vector<int32_t> recoveredPersistentIds;
    if (!ConvertInt32ArrayFromJs(env, argv[1], recoveredPersistentIds)) {
        WLOGFE("[NAPI]Failed to convert recovered persistentId array");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
                            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    if (!isRecovering) {
        // Sceneboard recover finished
        SceneSessionManager::GetInstance().NotifyRecoveringFinished();
        SceneSessionManager::GetInstance().UpdateRecoveredSessionInfo(recoveredPersistentIds);
    } else {
        SceneSessionManager::GetInstance().SetAlivePersistentIds(recoveredPersistentIds);
    }
    SceneSessionManager::GetInstance().SetEnableInputEvent(!isRecovering);

    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnUpdateTitleInTargetPos(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 3) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t persistentId;
    if (!ConvertFromJsValue(env, argv[0], persistentId)) {
        WLOGFE("[NAPI]Failed to convert parameter to persistentId");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool isShow = false;
    if (!ConvertFromJsValue(env, argv[1], isShow)) {
        WLOGFE("[NAPI]Failed to convert parameter to isShow");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t height;
    if (!ConvertFromJsValue(env, argv[2], height)) {
        WLOGFE("[NAPI]Failed to convert parameter to height");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    SceneSessionManager::GetInstance().UpdateTitleInTargetPos(persistentId, isShow, height);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnReportData(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARG_INDEX_3) { // ReportData args must be greater than three
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t resType;
    if (!ConvertFromJsValue(env, argv[0], resType)) {
        WLOGFE("[NAPI]Failed to convert parameter to resType");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t value;
    if (!ConvertFromJsValue(env, argv[ARG_INDEX_1], value)) { // second args is int value
        WLOGFE("[NAPI]Failed to convert parameter to value");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    std::unordered_map<std::string, std::string> mapPayload;
    if (!ConvertStringMapFromJs(env, argv[ARG_INDEX_TWO], mapPayload)) {
        WLOGFE("[NAPI]Failed to convert parameter to pauloadPid");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    mapPayload["srcPid"] = std::to_string(getprocpid());
    if (resType == RESTYPE_RECLAIM) {
        std::string reclaimTag = mapPayload[RES_PARAM_RECLAIM_TAG];
        WLOGFI("handle reclaim type, reclaimTag=%{public}s", reclaimTag.c_str());
        if (reclaimTag == "true") {
            auto retId = SceneSessionManager::GetInstance().ReclaimPurgeableCleanMem();
            WLOGFI("trim ReclaimPurgeableCleanMem finished, retId:%{public}d", retId);
            return NapiGetUndefined(env);
        }
    }
#ifdef RESOURCE_SCHEDULE_SERVICE_ENABLE
    OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(resType, value, mapPayload);
#endif
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnGetSessionSnapshotPixelMap(napi_env env, napi_callback_info info)
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
    int32_t persistentId;
    if (!ConvertFromJsValue(env, argv[0], persistentId)) {
        WLOGFE("[NAPI]Failed to convert parameter to persistentId");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t scaleValue;
    if (!ConvertFromJsValue(env, argv[1], scaleValue)) {
        WLOGFE("[NAPI]Failed to convert parameter to scaleValue");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    const float scaleParam = (scaleValue > 0.0 && scaleValue < 1.0) ? static_cast<float>(scaleValue) : 0.0f;
    NapiAsyncTask::CompleteCallback complete =
        [persistentId, scaleParam](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto pixelMap = SceneSessionManager::GetInstance().GetSessionSnapshotPixelMap(persistentId, scaleParam);
            napi_value nativeData = nullptr;
            if (pixelMap) {
                nativeData = Media::PixelMapNapi::CreatePixelMap(env, pixelMap);
            }
            if (nativeData) {
                WLOGD("[NAPI]pixelmap W x H = %{public}d x %{public}d", pixelMap->GetWidth(), pixelMap->GetHeight());
                task.Resolve(env, nativeData);
            } else {
                WLOGE("[NAPI]create native pixelmap fail");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
            }
        };
    napi_value result = nullptr;
    napi_value lastParam = argv[1];
    NapiAsyncTask::Schedule("JsSceneSessionManager::OnGetSessionSnapshotPixelMap",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsSceneSessionManager::OnGetIsLayoutFullScreen(napi_env env, napi_callback_info info)
{
    bool isLayoutFullScreen = false;
    SceneSessionManager::GetInstance().GetIsLayoutFullScreen(isLayoutFullScreen);
    napi_value result = nullptr;
    napi_get_boolean(env, isLayoutFullScreen, &result);
    return result;
}

napi_value JsSceneSessionManager::OnSwitchFreeMultiWindow(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[NAPI] Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    bool enable;
    if (!ConvertFromJsValue(env, argv[0], enable)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[NAPI] Failed to convert parameter to intoFreeMultiWindow bool value");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    SceneSessionManager::GetInstance().SwitchFreeMultiWindow(enable);
    return NapiGetUndefined(env);
}

napi_value JsSceneSessionManager::OnGetFreeMultiWindowConfig(napi_env env, napi_callback_info info)
{
    auto systemConfig = SceneSessionManager::GetInstance().GetSystemSessionConfig();
    return JsWindowSceneConfig::CreateFreeMultiWindowConfig(env, systemConfig);
}

napi_value JsSceneSessionManager::GetCustomDecorHeight(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "[NAPI]GetCustomDecorHeight");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(env, info);
    return (me != nullptr) ? me->OnGetCustomDecorHeight(env, info) : nullptr;
}

napi_value JsSceneSessionManager::OnGetCustomDecorHeight(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t persistentId;
    if (!ConvertFromJsValue(env, argv[0], persistentId)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[NAPI]Failed to convert parameter to persistentId");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    int32_t customDecorHeight = SceneSessionManager::GetInstance().GetCustomDecorHeight(persistentId);
    napi_value result = nullptr;
    napi_create_int32(env, customDecorHeight, &result);
    return result;
}
} // namespace OHOS::Rosen