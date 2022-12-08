/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "window_scene_hilog.h"

#include "js_scene_utils.h"
#include "scene_session.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsSceneSession"};
    const std::string START_SCENE_CB = "startScene";
}

NativeValue* CreateJsSceneSessionObject(NativeEngine& engine, const sptr<SceneSession>& session,
    const std::shared_ptr<JsSceneSession>& jsSceneSession)
{
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);

    WLOGFI("[NAPI]CreateJsSceneSession");

    object->SetNativePointer(new std::weak_ptr<JsSceneSession>(jsSceneSession), JsSceneSession::Finalizer, nullptr);
    object->SetProperty("abilityInfo", CreateJsAbilityInfo(engine, session));
    object->SetProperty("persistentId", CreateJsValue(engine, session->GetPersistentId()));
    BindFunctions(engine, object, "JsSceneSession");
    return objValue;
}

void JsSceneSession::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFI("[NAPI]Finalizer");
    delete static_cast<std::weak_ptr<JsSceneSession>*>(data);
}

void BindFunctions(NativeEngine& engine, NativeObject* object, const char *moduleName)
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
        WLOGFI("[NAPI]Callback(info->argv[1]) is not callable");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    std::shared_ptr<NativeReference> callbackRef;
    callbackRef.reset(engine.CreateReference(value, 1));
    jsCbMap_.insert(std::make_pair(cbType, callbackRef));
    WLOGFI("[NAPI]Register end, type = %{public}s, callback = %{public}p", cbType.c_str(), value);
    return engine.CreateUndefined();
}

void JsSceneSession::StartScene(const sptr<SceneSession>& session)
{
    if (jsCbMap_.find(START_SCENE_CB) == jsCbMap_.end()) {
        return;
    }
    jsCallBack_ = jsCbMap_[START_SCENE_CB];
    WLOGFI("[NAPI]start scene: name = %{public}s, id = %{public}u", session->GetAbilityInfo().abilityName_.c_str(),
        session->GetPersistentId());
    std::weak_ptr<JsSceneSession> sessionWptr(shared_from_this());
    auto complete = std::make_unique<AsyncTask::CompleteCallback> (
        [sessionWptr, session, eng = engine_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto jsSceneSession = sessionWptr.lock();
            if (jsSceneSession == nullptr || eng == nullptr || session) {
                WLOGFE("[NAPI]this scene session or target session or engine is nullptr");
                return;
            }
            auto jsSceneSessionRef = session->GetJsSceneSessionRef();
            if (jsSceneSessionRef == nullptr) {
                WLOGFE("[NAPI]this target session reference is nullptr");
            }
            NativeValue* argv[] = { jsSceneSessionRef->Get() };
            jsSceneSession->CallJsMethod(START_SCENE_CB.c_str(), argv, ArraySize(argv));
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsSceneSession::StartScene", *engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsSceneSession::CallJsMethod(const char* methodName, NativeValue* const* argv, size_t argc)
{
    WLOGFI("[NAPI]CallJsMethod methodName = %{public}s", methodName);
    if (engine_ == nullptr || jsCallBack_ == nullptr) {
        WLOGFE("[NAPI]engine_ nullptr or jsCallBack_ is nullptr");
        return;
    }
    NativeValue* method = jsCallBack_->Get();
    if (method == nullptr) {
        WLOGFE("[NAPI]Failed to get method callback from object");
        return;
    }
    engine_->CallFunction(engine_->CreateUndefined(), method, argv, argc);
}
}
