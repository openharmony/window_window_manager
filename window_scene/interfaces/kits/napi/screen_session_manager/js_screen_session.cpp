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
const std::string ON_POWER_STATUS_CHANGE_CALLBACK = "powerStatusChange";
const std::string ON_SENSOR_ROTATION_CHANGE_CALLBACK = "sensorRotationChange";
const std::string ON_SCREEN_ORIENTATION_CHANGE_CALLBACK = "screenOrientationChange";
const std::string ON_SCREEN_ROTATION_LOCKED_CHANGE = "screenRotationLockedChange";
} // namespace

napi_value JsScreenSession::Create(napi_env env, const sptr<ScreenSession>& screenSession)
{
    WLOGD("Create.");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("[NAPI]Object is null!");
        return NapiGetUndefined(env);
    }

    auto jsScreenSession = std::make_unique<JsScreenSession>(env, screenSession);
    napi_wrap(env, objValue, jsScreenSession.release(), JsScreenSession::Finalizer, nullptr, nullptr);
    napi_set_named_property(env, objValue, "screenId",
        CreateJsValue(env, static_cast<int64_t>(screenSession->GetScreenId())));
    napi_set_named_property(env, objValue, "name",
        CreateJsValue(env, static_cast<std::string>(screenSession->GetName())));

    const char* moduleName = "JsScreenSession";
    BindNativeFunction(env, objValue, "on", moduleName, JsScreenSession::RegisterCallback);
    BindNativeFunction(env, objValue, "setScreenRotationLocked", moduleName,
        JsScreenSession::SetScreenRotationLocked);
    BindNativeFunction(env, objValue, "loadContent", moduleName, JsScreenSession::LoadContent);
    return objValue;
}

void JsScreenSession::Finalizer(napi_env env, void* data, void* hint)
{
    WLOGD("Finalizer.");
    std::unique_ptr<JsScreenSession>(static_cast<JsScreenSession*>(data));
}

JsScreenSession::JsScreenSession(napi_env env, const sptr<ScreenSession>& screenSession)
    : env_(env), screenSession_(screenSession)
{
    std::string name = screenSession_ ? screenSession_->GetName() : "UNKNOW";
    screenScene_ = new(std::nothrow) ScreenScene(name);
    if (screenSession_) {
        SetScreenSceneDpiFunc func = [this](float density) {
            WLOGI("Screen Scene Dpi change, new density = %{public}f", density);
            if (!screenScene_ || !screenSession_) {
                WLOGFE("[NAPI]screenScene or screenSession is nullptr");
                return;
            }
            auto screenBounds = screenSession_->GetScreenProperty().GetBounds();
            Rect rect = { screenBounds.rect_.left_, screenBounds.rect_.top_,
                screenBounds.rect_.width_, screenBounds.rect_.height_ };
            screenScene_->SetDisplayDensity(density);
            screenScene_->UpdateViewportConfig(rect, WindowSizeChangeReason::UNDEFINED);
        };
        screenSession_->SetScreenSceneDpiChangeListener(func);
        DestroyScreenSceneFunc destroyFunc = [screenScene = screenScene_]() {
            if (screenScene) {
                screenScene->Destroy();
            }
        };
        screenSession_->SetScreenSceneDestroyListener(destroyFunc);
    }
}

napi_value JsScreenSession::LoadContent(napi_env env, napi_callback_info info)
{
    JsScreenSession* me = CheckParamsAndGetThis<JsScreenSession>(env, info);
    return (me != nullptr) ? me->OnLoadContent(env, info) : nullptr;
}

napi_value JsScreenSession::OnLoadContent(napi_env env, napi_callback_info info)
{
    WLOGD("[NAPI]JsScreenSession::OnLoadContent");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 2) {  // 2: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }
    std::string contentUrl;
    napi_value context = argv[1];
    napi_value storage = argc < 3 ? nullptr : argv[2];
    if (!ConvertFromJsValue(env, argv[0], contentUrl)) {
        WLOGFE("[NAPI]Failed to convert parameter to content url");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return NapiGetUndefined(env);
    }

    if (context == nullptr) {
        WLOGFE("[NAPI]Failed to get context object");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY)));
        return NapiGetUndefined(env);
    }
    void* pointerResult = nullptr;
    napi_unwrap(env, context, &pointerResult);
    auto contextNativePointer = static_cast<std::weak_ptr<Context>*>(pointerResult);
    if (contextNativePointer == nullptr) {
        WLOGFE("[NAPI]Failed to get context pointer from js object");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY)));
        return NapiGetUndefined(env);
    }
    auto contextWeakPtr = *contextNativePointer;

    std::shared_ptr<NativeReference> contentStorage = nullptr;
    if (storage != nullptr) {
        napi_ref ref = nullptr;
        napi_create_reference(env, storage, 1, &ref);
        contentStorage = std::shared_ptr<NativeReference>(reinterpret_cast<NativeReference*>(ref));
    }

    return ScheduleLoadContentTask(env, contentUrl, contextWeakPtr, contentStorage);
}

napi_value JsScreenSession::ScheduleLoadContentTask(napi_env env, const std::string& contentUrl,
    std::weak_ptr<Context> contextWeakPtr, std::shared_ptr<NativeReference> contentStorage)
{
    if (screenScene_ == nullptr) {
        WLOGFE("[NAPI]screenScene is nullptr");
        return NapiGetUndefined(env);
    }
    napi_value nativeStorage = contentStorage ? contentStorage->GetNapiValue() : nullptr;
    screenScene_->LoadContent(contentUrl, env, nativeStorage, contextWeakPtr.lock().get());
    return NapiGetUndefined(env);
}

napi_value JsScreenSession::SetScreenRotationLocked(napi_env env, napi_callback_info info)
{
    JsScreenSession* me = CheckParamsAndGetThis<JsScreenSession>(env, info);
    return (me != nullptr) ? me->OnSetScreenRotationLocked(env, info) : nullptr;
}

napi_value JsScreenSession::OnSetScreenRotationLocked(napi_env env, napi_callback_info info)
{
    WLOGI("JsScreenSession::OnSetScreenRotationLocked is called");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    bool isLocked = true;
    napi_value nativeVal = argv[0];
    if (nativeVal == nullptr) {
        WLOGFE("ConvertNativeValueTo isLocked failed!");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    napi_get_value_bool(env, nativeVal, &isLocked);
    if (screenSession_ == nullptr) {
        WLOGFE("Failed to register screen change listener, session is null!");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    screenSession_->SetScreenRotationLockedFromJs(isLocked);
    WLOGFI("SetScreenRotationLocked %{public}u success.", static_cast<uint32_t>(isLocked));
    return NapiGetUndefined(env);
}

void JsScreenSession::RegisterScreenChangeListener()
{
    if (screenSession_ == nullptr) {
        WLOGFE("Failed to register screen change listener, session is null!");
        return;
    }

    screenSession_->RegisterScreenChangeListener(this);
    WLOGFI("register screen change listener success.");
}

napi_value JsScreenSession::RegisterCallback(napi_env env, napi_callback_info info)
{
    WLOGD("Register callback.");
    JsScreenSession* me = CheckParamsAndGetThis<JsScreenSession>(env, info);
    return (me != nullptr) ? me->OnRegisterCallback(env, info) : nullptr;
}

napi_value JsScreenSession::OnRegisterCallback(napi_env env, napi_callback_info info)
{
    WLOGI("On register callback.");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 2) { // 2: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    std::string callbackType;
    if (!ConvertFromJsValue(env, argv[0], callbackType)) {
        WLOGFE("Failed to convert parameter to callback type.");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    napi_value callback = argv[1];
    if (!NapiIsCallable(env, callback)) {
        WLOGFE("Failed to register callback, callback is not callable!");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    if (mCallback_.count(callbackType)) {
        WLOGFE("Failed to register callback, callback is already existed!");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WSErrorCode::WS_ERROR_REPEAT_OPERATION)));
        return NapiGetUndefined(env);
    }

    napi_ref result = nullptr;
    napi_create_reference(env, callback, 1, &result);
    std::shared_ptr<NativeReference> callbackRef(reinterpret_cast<NativeReference*>(result));
    mCallback_[callbackType] = callbackRef;
    RegisterScreenChangeListener();

    return NapiGetUndefined(env);
}

void JsScreenSession::CallJsCallback(const std::string& callbackType)
{
    WLOGI("Call js callback: %{public}s.", callbackType.c_str());
    if (mCallback_.count(callbackType) == 0) {
        WLOGFE("Callback is unregistered!");
        return;
    }

    auto jsCallbackRef = mCallback_[callbackType];
    wptr<ScreenSession> screenSessionWeak(screenSession_);
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [jsCallbackRef, callbackType, screenSessionWeak](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (jsCallbackRef == nullptr) {
                WLOGFE("Call js callback %{public}s failed, jsCallbackRef is null!", callbackType.c_str());
                return;
            }
            auto method = jsCallbackRef->GetNapiValue();
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
                napi_value argv[] = { JsScreenUtils::CreateJsScreenProperty(
                    env, screenSession->GetScreenProperty()) };
                napi_call_function(env, NapiGetUndefined(env), method, ArraySize(argv), argv, nullptr);
            } else {
                napi_value argv[] = {};
                napi_call_function(env, NapiGetUndefined(env), method, 0, argv, nullptr);
            }
            WLOGI("The js callback has been executed: %{public}s.", callbackType.c_str());
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsScreenSession::" + callbackType, env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsScreenSession::OnConnect(ScreenId screenId)
{
    CallJsCallback(ON_CONNECTION_CALLBACK);
}

void JsScreenSession::OnDisconnect(ScreenId screenId)
{
    CallJsCallback(ON_DISCONNECTION_CALLBACK);
}

void JsScreenSession::OnSensorRotationChange(float sensorRotation, ScreenId screenId)
{
    const std::string callbackType = ON_SENSOR_ROTATION_CHANGE_CALLBACK;
    WLOGD("Call js callback: %{public}s.", callbackType.c_str());
    if (mCallback_.count(callbackType) == 0) {
        WLOGFE("Callback %{public}s is unregistered!", callbackType.c_str());
        return;
    }

    auto jsCallbackRef = mCallback_[callbackType];
    wptr<ScreenSession> screenSessionWeak(screenSession_);
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [jsCallbackRef, callbackType, screenSessionWeak, sensorRotation](
            napi_env env, NapiAsyncTask& task, int32_t status) {
            if (jsCallbackRef == nullptr) {
                WLOGFE("Call js callback %{public}s failed, jsCallbackRef is null!", callbackType.c_str());
                return;
            }
            auto method = jsCallbackRef->GetNapiValue();
            if (method == nullptr) {
                WLOGFE("Call js callback %{public}s failed, method is null!", callbackType.c_str());
                return;
            }
            auto screenSession = screenSessionWeak.promote();
            if (screenSession == nullptr) {
                WLOGFE("Call js callback %{public}s failed, screenSession is null!", callbackType.c_str());
                return;
            }
            napi_value argv[] = { CreateJsValue(env, sensorRotation) };
            napi_call_function(env, NapiGetUndefined(env), method, ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsScreenSession::" + callbackType, env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsScreenSession::OnScreenOrientationChange(float screenOrientation, ScreenId screenId)
{
    const std::string callbackType = ON_SCREEN_ORIENTATION_CHANGE_CALLBACK;
    WLOGI("Call js callback: %{public}s.", callbackType.c_str());
    if (mCallback_.count(callbackType) == 0) {
        WLOGFE("Callback %{public}s is unregistered!", callbackType.c_str());
        return;
    }

    auto jsCallbackRef = mCallback_[callbackType];
    wptr<ScreenSession> screenSessionWeak(screenSession_);
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [jsCallbackRef, callbackType, screenSessionWeak, screenOrientation](
            napi_env env, NapiAsyncTask& task, int32_t status) {
            if (jsCallbackRef == nullptr) {
                WLOGFE("Call js callback %{public}s failed, jsCallbackRef is null!", callbackType.c_str());
                return;
            }
            auto method = jsCallbackRef->GetNapiValue();
            if (method == nullptr) {
                WLOGFE("Call js callback %{public}s failed, method is null!", callbackType.c_str());
                return;
            }
            auto screenSession = screenSessionWeak.promote();
            if (screenSession == nullptr) {
                WLOGFE("Call js callback %{public}s failed, screenSession is null!", callbackType.c_str());
                return;
            }
            napi_value argv[] = { CreateJsValue(env, screenOrientation) };
            napi_call_function(env, NapiGetUndefined(env), method, ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsScreenSession::" + callbackType, env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsScreenSession::OnPropertyChange(const ScreenProperty& newProperty, ScreenPropertyChangeReason reason,
    ScreenId screenId)
{
    const std::string callbackType = ON_PROPERTY_CHANGE_CALLBACK;
    WLOGD("Call js callback: %{public}s.", callbackType.c_str());
    if (mCallback_.count(callbackType) == 0) {
        WLOGFE("Callback %{public}s is unregistered!", callbackType.c_str());
        return;
    }

    auto jsCallbackRef = mCallback_[callbackType];
    wptr<ScreenSession> screenSessionWeak(screenSession_);
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [jsCallbackRef, callbackType, screenSessionWeak, newProperty, reason](
            napi_env env, NapiAsyncTask& task, int32_t status) {
            if (jsCallbackRef == nullptr) {
                WLOGFE("Call js callback %{public}s failed, jsCallbackRef is null!", callbackType.c_str());
                return;
            }
            auto method = jsCallbackRef->GetNapiValue();
            if (method == nullptr) {
                WLOGFE("Call js callback %{public}s failed, method is null!", callbackType.c_str());
                return;
            }
            auto screenSession = screenSessionWeak.promote();
            if (screenSession == nullptr) {
                WLOGFE("Call js callback %{public}s failed, screenSession is null!", callbackType.c_str());
                return;
            }
            napi_value propertyChangeReason = CreateJsValue(env, static_cast<int32_t>(reason));
            napi_value argv[] = { JsScreenUtils::CreateJsScreenProperty(env, newProperty), propertyChangeReason };
            napi_call_function(env, NapiGetUndefined(env), method, ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsScreenSession::" + callbackType, env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsScreenSession::OnPowerStatusChange(DisplayPowerEvent event, EventStatus eventStatus,
    PowerStateChangeReason reason)
{
    const std::string callbackType = ON_POWER_STATUS_CHANGE_CALLBACK;
    WLOGD("[UL_POWER]Call js callback: %{public}s.", callbackType.c_str());
    if (mCallback_.count(callbackType) == 0) {
        WLOGFW("[UL_POWER]Callback %{public}s is unregistered!", callbackType.c_str());
        return;
    }

    auto jsCallbackRef = mCallback_[callbackType];
    wptr<ScreenSession> screenSessionWeak(screenSession_);
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [jsCallbackRef, callbackType, screenSessionWeak, event, eventStatus, reason](
            napi_env env, NapiAsyncTask& task, int32_t status) {
            if (jsCallbackRef == nullptr) {
                WLOGFE("[UL_POWER]Call js callback %{public}s failed, jsCallbackRef is null!", callbackType.c_str());
                return;
            }
            auto method = jsCallbackRef->GetNapiValue();
            if (method == nullptr) {
                WLOGFE("[UL_POWER]Call js callback %{public}s failed, method is null!", callbackType.c_str());
                return;
            }
            auto screenSession = screenSessionWeak.promote();
            if (screenSession == nullptr) {
                WLOGFE("[UL_POWER]Call js callback %{public}s failed, screenSession is null!", callbackType.c_str());
                return;
            }
            napi_value displayPowerEvent = CreateJsValue(env, static_cast<int32_t>(event));
            napi_value powerEventStatus = CreateJsValue(env, static_cast<int32_t>(eventStatus));
            napi_value powerStateChangeReason = CreateJsValue(env, static_cast<int32_t>(reason));
            napi_value argv[] = { displayPowerEvent, powerEventStatus, powerStateChangeReason };
            napi_call_function(env, NapiGetUndefined(env), method, ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsScreenSession::" + callbackType, env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsScreenSession::OnScreenRotationLockedChange(bool isLocked, ScreenId screenId)
{
    const std::string callbackType = ON_SCREEN_ROTATION_LOCKED_CHANGE;
    WLOGD("Call js callback: %{public}s isLocked:%{public}u.", callbackType.c_str(), isLocked);
    if (mCallback_.count(callbackType) == 0) {
        WLOGFE("Callback %{public}s is unregistered!", callbackType.c_str());
        return;
    }

    auto jsCallbackRef = mCallback_[callbackType];
    auto complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [jsCallbackRef, callbackType, isLocked](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (jsCallbackRef == nullptr) {
                WLOGFE("Call js callback %{public}s failed, jsCallbackRef is null!", callbackType.c_str());
                return;
            }
            auto method = jsCallbackRef->GetNapiValue();
            if (method == nullptr) {
                WLOGFE("Call js callback %{public}s failed, method is null!", callbackType.c_str());
                return;
            }
            napi_value argv[] = { CreateJsValue(env, isLocked) };
            napi_call_function(env, NapiGetUndefined(env), method, ArraySize(argv), argv, nullptr);
        });

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsScreenSession::" + callbackType, env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}
} // namespace OHOS::Rosen
