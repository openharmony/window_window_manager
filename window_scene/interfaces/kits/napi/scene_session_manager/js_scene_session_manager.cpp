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
#include "napi_common_want.h"
#include "session/host/include/scene_persistence.h"
#include "session/host/include/scene_persistent_storage.h"
#include "session/host/include/session.h"
#include "session_manager/include/scene_session_manager.h"
#include "want.h"
#include "window_manager_hilog.h"

#include "js_root_scene_session.h"
#include "js_scene_session.h"
#include "js_scene_utils.h"
#include "js_window_scene_config.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsSceneSessionManager" };
const std::string CREATE_SPECIFIC_SCENE_CB = "createSpecificSession";
const std::string GESTURE_NAVIGATION_ENABLED_CHANGE_CB = "gestureNavigationEnabledChange";
} // namespace

NativeValue* JsSceneSessionManager::Init(NativeEngine* engine, NativeValue* exportObj)
{
    WLOGI("[NAPI]JsSceneSessionManager Init");
    if (engine == nullptr || exportObj == nullptr) {
        WLOGFE("Engine or exportObj is null!");
        return nullptr;
    }

    auto object = ConvertNativeValueTo<NativeObject>(exportObj);
    if (object == nullptr) {
        WLOGFE("[NAPI]Object is null!");
        return nullptr;
    }

    std::unique_ptr<JsSceneSessionManager> jsSceneSessionManager = std::make_unique<JsSceneSessionManager>(*engine);
    object->SetNativePointer(jsSceneSessionManager.release(), JsSceneSessionManager::Finalizer, nullptr);
    object->SetProperty("SessionState", CreateJsSessionState(*engine));
    object->SetProperty("SessionType", SessionTypeInit(engine));

    const char* moduleName = "JsSceneSessionManager";
    BindNativeFunction(*engine, *object, "getRootSceneSession", moduleName, JsSceneSessionManager::GetRootSceneSession);
    BindNativeFunction(*engine, *object, "requestSceneSession", moduleName, JsSceneSessionManager::RequestSceneSession);
    BindNativeFunction(*engine, *object, "requestSceneSessionActivation", moduleName,
        JsSceneSessionManager::RequestSceneSessionActivation);
    BindNativeFunction(*engine, *object, "requestSceneSessionBackground", moduleName,
        JsSceneSessionManager::RequestSceneSessionBackground);
    BindNativeFunction(*engine, *object, "requestSceneSessionDestruction", moduleName,
        JsSceneSessionManager::RequestSceneSessionDestruction);
    BindNativeFunction(*engine, *object, "on", moduleName, JsSceneSessionManager::RegisterCallback);
    BindNativeFunction(*engine, *object, "getWindowSceneConfig", moduleName,
        JsSceneSessionManager::GetWindowSceneConfig);
    BindNativeFunction(*engine, *object, "processBackEvent", moduleName, JsSceneSessionManager::ProcessBackEvent);
    BindNativeFunction(*engine, *object, "updateFocus", moduleName, JsSceneSessionManager::UpdateFocus);
    BindNativeFunction(*engine, *object, "switchUser", moduleName, JsSceneSessionManager::SwitchUser);
    BindNativeFunction(*engine, *object, "requestSceneSessionByCall", moduleName,
        JsSceneSessionManager::RequestSceneSessionByCall);
    BindNativeFunction(*engine, *object, "startAbilityBySpecified", moduleName,
        JsSceneSessionManager::StartAbilityBySpecified);
    BindNativeFunction(*engine, *object, "getSessionSnapshot", moduleName, JsSceneSessionManager::GetSessionSnapshot);
    return engine->CreateUndefined();
}

JsSceneSessionManager::JsSceneSessionManager(NativeEngine& engine) : engine_(engine)
{
    listenerFunc_ = {
        { CREATE_SPECIFIC_SCENE_CB, &JsSceneSessionManager::ProcessCreateSpecificSessionRegister },
        { GESTURE_NAVIGATION_ENABLED_CHANGE_CB,
            &JsSceneSessionManager::ProcessGestureNavigationEnabledChangeListener },
    };
}

void JsSceneSessionManager::OnCreateSpecificSession(const sptr<SceneSession>& sceneSession)
{
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
                WLOGFE("[NAPI]specific session is nullptr");
                return;
            }
            NativeValue* jsSceneSessionObj = JsSceneSession::Create(*eng, specificSession);
            if (jsSceneSessionObj == nullptr) {
                WLOGFE("[NAPI]jsSceneSessionObj is nullptr");
                return;
            }
            NativeValue* argv[] = { jsSceneSessionObj };
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSessionManager::OnCreateSpecificSession", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}


void JsSceneSessionManager::OnGestureNavigationEnabledUpdate(bool enable)
{
    WLOGFI("[NAPI]OnGestureNavigationEnabledUpdate");
    auto iter = jsCbMap_.find(GESTURE_NAVIGATION_ENABLED_CHANGE_CB);
    if (iter == jsCbMap_.end()) {
        return;
    }

    auto jsCallBack = iter->second;
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [this, enable, jsCallBack, eng = &engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            NativeValue* argv[] = {CreateJsValue(*eng, enable)};
            engine.CallFunction(engine.CreateUndefined(), jsCallBack->Get(), argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSessionManager::OnGestureNavigationEnabledUpdate", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSessionManager::ProcessCreateSpecificSessionRegister()
{
    NotifyCreateSpecificSessionFunc func = [this](const sptr<SceneSession>& session) {
        WLOGFD("NotifyCreateSpecificSessionFunc");
        this->OnCreateSpecificSession(session);
    };
    SceneSessionManager::GetInstance().SetCreateSpecificSessionListener(func);
}

void JsSceneSessionManager::ProcessGestureNavigationEnabledChangeListener()
{
    ProcessGestureNavigationEnabledChangeFunc func = [this](bool enable) {
        WLOGFD("GestureNavigationEnabledUpdate");
        this->OnGestureNavigationEnabledUpdate(enable);
    };
    SceneSessionManager::GetInstance().SetGestureNavigationEnabledChangeListener(func);
}

NativeValue* JsSceneSessionManager::RegisterCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]RegisterCallback");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(engine, info);
    return (me != nullptr) ? me->OnRegisterCallback(*engine, *info) : nullptr;
}

NativeValue* JsSceneSessionManager::UpdateFocus(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGI("[NAPI]UpdateFocus");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(engine, info);
    return (me != nullptr) ? me->OnUpdateFocus(*engine, *info) : nullptr;
}

NativeValue* JsSceneSessionManager::ProcessBackEvent(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]ProcessBackEvent");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(engine, info);
    return (me != nullptr) ? me->OnProcessBackEvent(*engine, *info) : nullptr;
}

NativeValue* JsSceneSessionManager::SwitchUser(NativeEngine *engine, NativeCallbackInfo *info)
{
    WLOGFI("[NAPI]SwitchUser");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(engine, info);
    return (me != nullptr) ? me->OnSwitchUser(*engine, *info) : nullptr;
}

void JsSceneSessionManager::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGI("[NAPI]Finalizer");
    std::unique_ptr<JsSceneSessionManager>(static_cast<JsSceneSessionManager*>(data));
}

NativeValue* JsSceneSessionManager::GetRootSceneSession(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGI("[NAPI]GetRootSceneSession");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(engine, info);
    return (me != nullptr) ? me->OnGetRootSceneSession(*engine, *info) : nullptr;
}

NativeValue* JsSceneSessionManager::RequestSceneSession(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGI("[NAPI]RequestSceneSession");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(engine, info);
    return (me != nullptr) ? me->OnRequestSceneSession(*engine, *info) : nullptr;
}

NativeValue* JsSceneSessionManager::RequestSceneSessionActivation(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGI("[NAPI]RequestSceneSessionActivation");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(engine, info);
    return (me != nullptr) ? me->OnRequestSceneSessionActivation(*engine, *info) : nullptr;
}

NativeValue* JsSceneSessionManager::RequestSceneSessionBackground(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGI("[NAPI]RequestSceneSessionBackground");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(engine, info);
    return (me != nullptr) ? me->OnRequestSceneSessionBackground(*engine, *info) : nullptr;
}

NativeValue* JsSceneSessionManager::RequestSceneSessionDestruction(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGI("[NAPI]RequestSceneSessionDestruction");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(engine, info);
    return (me != nullptr) ? me->OnRequestSceneSessionDestruction(*engine, *info) : nullptr;
}

NativeValue* JsSceneSessionManager::RequestSceneSessionByCall(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGI("[NAPI]RequestSceneSessionByCall");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(engine, info);
    return (me != nullptr) ? me->OnRequestSceneSessionByCall(*engine, *info) : nullptr;
}

NativeValue* JsSceneSessionManager::StartAbilityBySpecified(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGI("[NAPI]StartAbilityBySpecified");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(engine, info);
    return (me != nullptr) ? me->OnStartAbilityBySpecified(*engine, *info) : nullptr;
}

NativeValue* JsSceneSessionManager::GetWindowSceneConfig(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGI("[NAPI]GetWindowSceneConfig");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(engine, info);
    return (me != nullptr) ? me->OnGetWindowSceneConfig(*engine, *info) : nullptr;
}

NativeValue* JsSceneSessionManager::GetSessionSnapshot(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGI("[NAPI]GetSessionSnapshot");
    JsSceneSessionManager* me = CheckParamsAndGetThis<JsSceneSessionManager>(engine, info);
    return (me != nullptr) ? me->OnGetSessionSnapshot(*engine, *info) : nullptr;
}

bool JsSceneSessionManager::IsCallbackRegistered(const std::string& type, NativeValue* jsListenerObject)
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

NativeValue* JsSceneSessionManager::OnRegisterCallback(NativeEngine& engine, NativeCallbackInfo& info)
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
        WLOGFE("[NAPI]Callback is nullptr or not callable");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    if (IsCallbackRegistered(cbType, value)) {
        return engine.CreateUndefined();
    }

    (this->*listenerFunc_[cbType])();
    std::shared_ptr<NativeReference> callbackRef;
    callbackRef.reset(engine.CreateReference(value, 1));
    jsCbMap_[cbType] = callbackRef;
    WLOGFI("[NAPI]Register end, type = %{public}s", cbType.c_str());
    return engine.CreateUndefined();
}

NativeValue* JsSceneSessionManager::OnUpdateFocus(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc < 2) { // 2: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    int64_t persistentId;
    if (!ConvertFromJsValue(engine, info.argv[0], persistentId)) {
        WLOGFE("[NAPI]Failed to convert parameter to persistentId");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    bool isFocused;
    if (!ConvertFromJsValue(engine, info.argv[1], isFocused)) {
        WLOGFE("[NAPI]Failed to convert parameter to bool");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    SceneSessionManager::GetInstance().UpdateFocus(static_cast<uint64_t>(persistentId), isFocused);
    return engine.CreateUndefined();
}

NativeValue* JsSceneSessionManager::OnProcessBackEvent(NativeEngine& engine, NativeCallbackInfo& info)
{
    SceneSessionManager::GetInstance().ProcessBackEvent();
    return engine.CreateUndefined();
}

NativeValue* JsSceneSessionManager::OnSwitchUser(NativeEngine& engine, NativeCallbackInfo& info) 
{
    if (info.argc < 3) { // 3: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    int32_t oldUserId;
    if (!ConvertFromJsValue(engine, info.argv[0], oldUserId)) {
        WLOGFE("[NAPI]Failed to convert parameter to oldUserId");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    int32_t newUserId;
    if (!ConvertFromJsValue(engine, info.argv[1], newUserId)) {
        WLOGFE("[NAPI]Failed to convert parameter to newUserId");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    std::string fileDir;
    if (!ConvertFromJsValue(engine, info.argv[2], fileDir)) {
        WLOGFE("[NAPI]Failed to convert parameter to fileDir");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    WSErrorCode ret =
        WS_JS_TO_ERROR_CODE_MAP.at(SceneSessionManager::GetInstance().SwitchUser(oldUserId, newUserId, fileDir));
    if (ret != WSErrorCode::WS_OK) {
        engine.Throw(CreateJsError(engine,static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY),
            "System is abnormal"));
    }
    return engine.CreateUndefined();
}

NativeValue* JsSceneSessionManager::OnGetRootSceneSession(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGI("[NAPI]OnGetRootSceneSession");
    sptr<RootSceneSession> rootSceneSession = SceneSessionManager::GetInstance().GetRootSceneSession();
    if (rootSceneSession == nullptr) {
        engine.Throw(
            CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY), "System is abnormal"));
        return engine.CreateUndefined();
    }

    if (rootScene_ == nullptr) {
        rootScene_ = new RootScene();
    }
    RootScene::staticRootScene_ = rootScene_;
    rootSceneSession->SetLoadContentFunc([rootScene = rootScene_]
        (const std::string& contentUrl, NativeEngine* engine, NativeValue* storage, AbilityRuntime::Context* context) {
            rootScene->LoadContent(contentUrl, engine, storage, context);
            ScenePersistentStorage::InitDir(context->GetFilesDir());
            SceneSessionManager::GetInstance().InitPersistentStorage();
        });

    NativeValue* jsRootSceneSessionObj = JsRootSceneSession::Create(engine, rootSceneSession);
    if (jsRootSceneSessionObj == nullptr) {
        WLOGFE("[NAPI]jsRootSceneSessionObj is nullptr");
        engine.Throw(
            CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY), "System is abnormal"));
        return engine.CreateUndefined();
    }
    return jsRootSceneSessionObj;
}

NativeValue* JsSceneSessionManager::OnRequestSceneSession(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGI("[NAPI]OnRequestSceneSession");
    WSErrorCode errCode = WSErrorCode::WS_OK;
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
    }

    SessionInfo sessionInfo;
    AAFwk::Want want;
    if (errCode == WSErrorCode::WS_OK) {
        NativeObject* nativeObj = ConvertNativeValueTo<NativeObject>(info.argv[0]);
        if (nativeObj == nullptr) {
            WLOGFE("[NAPI]Failed to convert object to session info");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        } else if (!ConvertSessionInfoFromJs(engine, nativeObj, sessionInfo)) {
            WLOGFE("[NAPI]Failed to get session info from js object");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        }
    }
    if (errCode == WSErrorCode::WS_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    if (info.argc == 2 && info.argv[1]->TypeOf() != NATIVE_UNDEFINED) {
        OHOS::AppExecFwk::UnwrapWant(reinterpret_cast<napi_env>(&engine),
            reinterpret_cast<napi_value>(info.argv[1]), want);
        sessionInfo.want = new(std::nothrow) AAFwk::Want(want);
    }

    WLOGFI("[NAPI]SessionInfo [%{public}s, %{public}s, %{public}s], errCode = %{public}d",
        sessionInfo.bundleName_.c_str(), sessionInfo.moduleName_.c_str(), sessionInfo.abilityName_.c_str(), errCode);
    sptr<SceneSession> sceneSession = SceneSessionManager::GetInstance().RequestSceneSession(sessionInfo);
    if (sceneSession == nullptr) {
        engine.Throw(
            CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY), "System is abnormal"));
        return engine.CreateUndefined();
    } else {
        NativeValue* jsSceneSessionObj = JsSceneSession::Create(engine, sceneSession);
        if (jsSceneSessionObj == nullptr) {
            WLOGFE("jsSceneSessionObj is nullptr");
            engine.Throw(CreateJsError(
                engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY), "System is abnormal"));
        }
        return jsSceneSessionObj;
    }
}

NativeValue* JsSceneSessionManager::OnRequestSceneSessionActivation(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGI("[NAPI]OnRequestSceneSessionActivation");
    WSErrorCode errCode = WSErrorCode::WS_OK;
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
    }

    sptr<SceneSession> sceneSession = nullptr;
    if (errCode == WSErrorCode::WS_OK) {
        auto jsSceneSessionObj = ConvertNativeValueTo<NativeObject>(info.argv[0]);
        if (jsSceneSessionObj == nullptr) {
            WLOGFE("[NAPI]Failed to get js scene session object");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        } else {
            auto jsSceneSession = static_cast<JsSceneSession*>(jsSceneSessionObj->GetNativePointer());
            if (jsSceneSession == nullptr) {
                WLOGFE("[NAPI]Failed to get scene session from js object");
                errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
            }
            sceneSession = jsSceneSession->GetNativeSession();
        }
    }

    if (errCode == WSErrorCode::WS_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }

    AsyncTask::CompleteCallback complete = [sceneSession](NativeEngine& engine, AsyncTask& task, int32_t status) {
        if (sceneSession == nullptr) {
            task.Reject(engine,
                CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY), "Invalid params."));
            return;
        }
        WSErrorCode ret =
            WS_JS_TO_ERROR_CODE_MAP.at(SceneSessionManager::GetInstance().RequestSceneSessionActivation(sceneSession));
        if (ret == WSErrorCode::WS_OK) {
            task.Resolve(engine, engine.CreateUndefined());
        } else {
            task.Reject(
                engine, CreateJsError(engine, static_cast<int32_t>(ret), "Request scene session activation failed"));
        }
        WLOGFI("[NAPI]request scene session activation end: [%{public}s, %{public}s, %{public}s]",
            sceneSession->GetSessionInfo().bundleName_.c_str(), sceneSession->GetSessionInfo().moduleName_.c_str(),
            sceneSession->GetSessionInfo().abilityName_.c_str());
    };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        ((info.argv[1] != nullptr && info.argv[1]->TypeOf() == NATIVE_FUNCTION) ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsSceneSessionManager::OnRequestSceneSessionActivation", engine,
        CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsSceneSessionManager::OnRequestSceneSessionBackground(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGI("[NAPI]OnRequestSceneSessionBackground");
    WSErrorCode errCode = WSErrorCode::WS_OK;
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
    }

    sptr<SceneSession> sceneSession = nullptr;
    if (errCode == WSErrorCode::WS_OK) {
        auto jsSceneSessionObj = ConvertNativeValueTo<NativeObject>(info.argv[0]);
        if (jsSceneSessionObj == nullptr) {
            WLOGFE("[NAPI]Failed to get js scene session object");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        } else {
            auto jsSceneSession = static_cast<JsSceneSession*>(jsSceneSessionObj->GetNativePointer());
            if (jsSceneSession == nullptr) {
                WLOGFE("[NAPI]Failed to get scene session from js object");
                errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
            }
            sceneSession = jsSceneSession->GetNativeSession();
        }
    }

    if (errCode == WSErrorCode::WS_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }

    AsyncTask::CompleteCallback complete = [sceneSession](NativeEngine& engine, AsyncTask& task, int32_t status) {
        if (sceneSession == nullptr) {
            task.Reject(engine,
                CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY), "Invalid params."));
            return;
        }
        WSErrorCode ret =
            WS_JS_TO_ERROR_CODE_MAP.at(SceneSessionManager::GetInstance().RequestSceneSessionBackground(sceneSession));
        if (ret == WSErrorCode::WS_OK) {
            task.Resolve(engine, engine.CreateUndefined());
        } else {
            task.Reject(
                engine, CreateJsError(engine, static_cast<int32_t>(ret), "Request scene session background failed"));
        }
        WLOGFI("[NAPI]request scene session background end: [%{public}s, %{public}s, %{public}s]",
            sceneSession->GetSessionInfo().bundleName_.c_str(), sceneSession->GetSessionInfo().moduleName_.c_str(),
            sceneSession->GetSessionInfo().abilityName_.c_str());
    };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        ((info.argv[1] != nullptr && info.argv[1]->TypeOf() == NATIVE_FUNCTION) ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsSceneSessionManager::OnRequestSceneSessionBackground", engine,
        CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsSceneSessionManager::OnRequestSceneSessionDestruction(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGI("[NAPI]OnRequestSceneSessionDestruction");
    WSErrorCode errCode = WSErrorCode::WS_OK;
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
    }

    sptr<SceneSession> sceneSession = nullptr;
    if (errCode == WSErrorCode::WS_OK) {
        auto jsSceneSessionObj = ConvertNativeValueTo<NativeObject>(info.argv[0]);
        if (jsSceneSessionObj == nullptr) {
            WLOGFE("[NAPI]Failed to get js scene session object");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        } else {
            auto jsSceneSession = static_cast<JsSceneSession*>(jsSceneSessionObj->GetNativePointer());
            if (jsSceneSession == nullptr) {
                WLOGFE("[NAPI]Failed to get scene session from js object");
                errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
            }
            sceneSession = jsSceneSession->GetNativeSession();
        }
    }

    if (errCode == WSErrorCode::WS_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }

    AsyncTask::CompleteCallback complete = [sceneSession](NativeEngine& engine, AsyncTask& task, int32_t status) {
        if (sceneSession == nullptr) {
            task.Reject(engine,
                CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY), "Invalid params."));
            return;
        }
        WSErrorCode ret =
            WS_JS_TO_ERROR_CODE_MAP.at(SceneSessionManager::GetInstance().RequestSceneSessionDestruction(sceneSession));
        if (ret == WSErrorCode::WS_OK) {
            task.Resolve(engine, engine.CreateUndefined());
        } else {
            task.Reject(
                engine, CreateJsError(engine, static_cast<int32_t>(ret), "Request scene session destruction failed"));
        }
        WLOGFI("[NAPI]request scene session destruction end: [%{public}s, %{public}s, %{public}s]",
            sceneSession->GetSessionInfo().bundleName_.c_str(), sceneSession->GetSessionInfo().moduleName_.c_str(),
            sceneSession->GetSessionInfo().abilityName_.c_str());
    };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        ((info.argv[1] != nullptr && info.argv[1]->TypeOf() == NATIVE_FUNCTION) ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsSceneSessionManager::OnRequestSceneSessionDestruction", engine,
        CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsSceneSessionManager::OnRequestSceneSessionByCall(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGI("[NAPI]OnRequestSceneSessionByCall");
    WSErrorCode errCode = WSErrorCode::WS_OK;
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
    }

    sptr<SceneSession> sceneSession = nullptr;
    if (errCode == WSErrorCode::WS_OK) {
        auto jsSceneSessionObj = ConvertNativeValueTo<NativeObject>(info.argv[0]);
        if (jsSceneSessionObj == nullptr) {
            WLOGFE("[NAPI]Failed to get js scene session object");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        } else {
            auto jsSceneSession = static_cast<JsSceneSession*>(jsSceneSessionObj->GetNativePointer());
            if (jsSceneSession == nullptr) {
                WLOGFE("[NAPI]Failed to get scene session from js object");
                errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
            }
            sceneSession = jsSceneSession->GetNativeSession();
        }
    }

    if (errCode == WSErrorCode::WS_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }

    AsyncTask::CompleteCallback complete = [sceneSession](NativeEngine& engine, AsyncTask& task, int32_t status) {
        if (sceneSession == nullptr) {
            task.Reject(engine,
                CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY), "Invalid params."));
            return;
        }
        WSErrorCode ret =
            WS_JS_TO_ERROR_CODE_MAP.at(SceneSessionManager::GetInstance().RequestSceneSessionByCall(sceneSession));
        if (ret == WSErrorCode::WS_OK) {
            task.Resolve(engine, engine.CreateUndefined());
        } else {
            task.Reject(
                engine, CreateJsError(engine, static_cast<int32_t>(ret), "Request scene session by call failed"));
        }
        WLOGFI("[NAPI]request scene session by call end: [%{public}s, %{public}s]",
            sceneSession->GetSessionInfo().bundleName_.c_str(), sceneSession->GetSessionInfo().abilityName_.c_str());
    };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        ((info.argv[1] != nullptr && info.argv[1]->TypeOf() == NATIVE_FUNCTION) ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsSceneSessionManager::OnRequestSceneSessionByCall", engine,
        CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsSceneSessionManager::OnStartAbilityBySpecified(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGI("[NAPI]OnStartAbilityBySpecified");
    WSErrorCode errCode = WSErrorCode::WS_OK;
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
    }

    SessionInfo sessionInfo;
    if (errCode == WSErrorCode::WS_OK) {
        NativeObject* nativeObj = ConvertNativeValueTo<NativeObject>(info.argv[0]);
        if (nativeObj == nullptr) {
            WLOGFE("[NAPI]Failed to convert object to session info");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        } else if (!ConvertSessionInfoFromJs(engine, nativeObj, sessionInfo)) {
            WLOGFE("[NAPI]Failed to get session info from js object");
            errCode = WSErrorCode::WS_ERROR_INVALID_PARAM;
        }
    }

    if (errCode == WSErrorCode::WS_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }

    WLOGFI("[NAPI]SessionInfo [%{public}s, %{public}s, %{public}s], errCode = %{public}d",
        sessionInfo.bundleName_.c_str(), sessionInfo.moduleName_.c_str(), sessionInfo.abilityName_.c_str(), errCode);
    SceneSessionManager::GetInstance().StartAbilityBySpecified(sessionInfo);
    return engine.CreateUndefined();
}

NativeValue* JsSceneSessionManager::OnGetWindowSceneConfig(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("[NAPI]OnGetWindowSceneConfig");
    const AppWindowSceneConfig& windowSceneConfig = SceneSessionManager::GetInstance().GetWindowSceneConfig();
    NativeValue* jsWindowSceneConfigObj = JsWindowSceneConfig::CreateWindowSceneConfig(engine, windowSceneConfig);
    if (jsWindowSceneConfigObj == nullptr) {
        WLOGFE("[NAPI]jsWindowSceneConfigObj is nullptr");
        engine.Throw(CreateJsError(engine,
            static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY), "System is abnormal"));
    }
    return jsWindowSceneConfigObj;
}

NativeValue* JsSceneSessionManager::OnGetSessionSnapshot(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    int64_t persistentId;
    if (!ConvertFromJsValue(engine, info.argv[0], persistentId)) {
        WLOGFE("[NAPI]Failed to convert parameter to persistentId");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    std::string path = SceneSessionManager::GetInstance().GetSessionSnapshot(static_cast<uint64_t>(persistentId));
    NativeValue* result = engine.CreateString(path.c_str(), path.length());
    return result;
}

} // namespace OHOS::Rosen
