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

#include "js_screen_session.h"

#include <js_runtime_utils.h>

#include "interfaces/include/ws_common.h"
#include "js_screen_utils.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsScreenSession" };
const std::string ON_CONNECTION_CALLBACK = "connect";
const std::string ON_DISCONNECTION_CALLBACK = "disconnect";
const std::string ON_PROPERTY_CHANGE_CALLBACK = "propertyChange";
} // namespace

NativeValue* JsScreenSession::Create(NativeEngine& engine, const sptr<ScreenSession>& screenSession)
{
    WLOGD("Create.");
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Object is null!");
        return engine.CreateUndefined();
    }

    auto jsScreenSession = std::make_unique<JsScreenSession>(engine, screenSession);
    object->SetNativePointer(jsScreenSession.release(), JsScreenSession::Finalizer, nullptr);
    object->SetProperty("screenId", CreateJsValue(engine, static_cast<int64_t>(screenSession->GetScreenId())));

    const char* moduleName = "JsScreenSession";
    BindNativeFunction(engine, *object, "on", moduleName, JsScreenSession::RegisterCallback);

    return objValue;
}

void JsScreenSession::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGD("Finalizer.");
    std::unique_ptr<JsScreenSession>(static_cast<JsScreenSession*>(data));
}

JsScreenSession::JsScreenSession(NativeEngine& engine, const sptr<ScreenSession>& screenSession)
    : engine_(engine), screenSession_(screenSession)
{}

JsScreenSession::~JsScreenSession() {}

void JsScreenSession::RegisterScreenChangeListener()
{
    if (screenSession_ == nullptr) {
        WLOGFE("Failed to register screen change listener, session is null!");
        return;
    }

    screenSession_->RegisterScreenChangeListener(this);
}

NativeValue* JsScreenSession::RegisterCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGD("Register callback.");
    JsScreenSession* me = CheckParamsAndGetThis<JsScreenSession>(engine, info);
    return (me != nullptr) ? me->OnRegisterCallback(*engine, *info) : nullptr;
}

NativeValue* JsScreenSession::OnRegisterCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGD("On register callback.");
    if (info.argc < 2) { // 2: params num
        WLOGFE("Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    std::string callbackType;
    if (!ConvertFromJsValue(engine, info.argv[0], callbackType)) {
        WLOGFE("Failed to convert parameter to callback type.");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    NativeValue* callback = info.argv[1];
    if (!callback->IsCallable()) {
        WLOGFE("Failed to register callback, callback is not callable!");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    if (mCallback_.count(callbackType)) {
        WLOGFE("Failed to register callback, callback is already existed!");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_REPEAT_OPERATION)));
        return engine.CreateUndefined();
    }

    std::shared_ptr<NativeReference> callbackRef(engine.CreateReference(callback, 1));
    mCallback_[callbackType] = callbackRef;
    RegisterScreenChangeListener();

    return engine.CreateUndefined();
}

void JsScreenSession::CallJsCallback(const std::string& callbackType)
{
    WLOGD("Call js callback: %{public}s.", callbackType.c_str());
    if (mCallback_.count(callbackType) == 0) {
        WLOGFE("Callback is unregistered!");
        return;
    }

    auto jsCallbackRef = mCallback_[callbackType];
    wptr<ScreenSession> screenSessionWeak(screenSession_);
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [jsCallbackRef, callbackType, screenSessionWeak](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (jsCallbackRef == nullptr) {
                WLOGFE("Call js callback %{public}s failed, jsCallbackRef is null!", callbackType.c_str());
                return;
            }
            auto method = jsCallbackRef->Get();
            if (method == nullptr) {
                WLOGFE("Call js callback %{public}s failed, method is null!", callbackType.c_str());
                return;
            }

            if (callbackType == ON_CONNECTION_CALLBACK || callbackType == ON_DISCONNECTION_CALLBACK) {
                auto screenSession = screenSessionWeak.promote();
                if (screenSession == nullptr) {
                    WLOGFE("Call js callback %{public}s failed, screenSession is null!", callbackType.c_str());
                    return;
                }
                NativeValue* argv[] = { JsScreenUtils::CreateJsScreenProperty(
                    engine, screenSession->GetScreenProperty()) };
                engine.CallFunction(engine.CreateUndefined(), method, argv, ArraySize(argv));
            } else {
                NativeValue* argv[] = {};
                engine.CallFunction(engine.CreateUndefined(), method, argv, 0);
            }
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsScreenSession::" + callbackType, engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsScreenSession::OnConnect()
{
    CallJsCallback(ON_CONNECTION_CALLBACK);
}

void JsScreenSession::OnDisconnect()
{
    CallJsCallback(ON_DISCONNECTION_CALLBACK);
}

void JsScreenSession::OnPropertyChange(const ScreenProperty& newProperty, ScreenPropertyChangeReason reason)
{
    const std::string callbackType = ON_PROPERTY_CHANGE_CALLBACK;
    WLOGD("Call js callback: %{public}s.", callbackType.c_str());
    if (mCallback_.count(callbackType) == 0) {
        WLOGFE("Callback %{public}s is unregistered!", callbackType.c_str());
        return;
    }

    auto jsCallbackRef = mCallback_[callbackType];
    wptr<ScreenSession> screenSessionWeak(screenSession_);
    auto complete = std::make_unique<AsyncTask::CompleteCallback>(
        [jsCallbackRef, callbackType, screenSessionWeak](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (jsCallbackRef == nullptr) {
                WLOGFE("Call js callback %{public}s failed, jsCallbackRef is null!", callbackType.c_str());
                return;
            }
            auto method = jsCallbackRef->Get();
            if (method == nullptr) {
                WLOGFE("Call js callback %{public}s failed, method is null!", callbackType.c_str());
                return;
            }
            auto screenSession = screenSessionWeak.promote();
            if (screenSession == nullptr) {
                WLOGFE("Call js callback %{public}s failed, screenSession is null!", callbackType.c_str());
                return;
            }
            NativeValue* propertyChangeReason = CreateJsValue(engine, static_cast<int32_t>(reason));
            NativeValue* argv[] = { JsScreenUtils::CreateJsScreenProperty(
                engine, screenSession->GetScreenProperty()), propertyChangeReason };
            engine.CallFunction(engine.CreateUndefined(), method, argv, ArraySize(argv));
        });

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsScreenSession::" + callbackType, engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}
} // namespace OHOS::Rosen
