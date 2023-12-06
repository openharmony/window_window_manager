/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "js_pip_controller.h"

#include <refbase.h>
#include "js_pip_utils.h"
#include "js_runtime_utils.h"
#include "picture_in_picture_controller.h"
#include "picture_in_picture_manager.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "picture_in_picture_interface.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsPipController"};
    constexpr int32_t NUMBER_TWO = 2;
    const std::string STATE_CHANGE_CB = "stateChange";
    const std::string CONTROL_PANEL_ACTION_EVENT_CB = "controlPanelActionEvent";
}

std::mutex JsPipController::pipMutex_;

void BindFunctions(napi_env env, napi_value object, const char *moduleName)
{
    BindNativeFunction(env, object, "startPiP", moduleName, JsPipController::StartPictureInPicture);
    BindNativeFunction(env, object, "stopPiP", moduleName, JsPipController::StopPictureInPicture);
    BindNativeFunction(env, object, "updateContentSize", moduleName, JsPipController::UpdateContentSize);
    BindNativeFunction(env, object, "setAutoStartEnabled", moduleName, JsPipController::SetAutoStartEnabled);
    BindNativeFunction(env, object, "on", moduleName, JsPipController::RegisterCallback);
    BindNativeFunction(env, object, "off", moduleName, JsPipController::UnregisterCallback);
}

napi_value CreateJsPipControllerObject(napi_env env, sptr<PictureInPictureController>& pipController)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);

    WLOGI("CreateJsPipController");
    std::unique_ptr<JsPipController> jsPipController = std::make_unique<JsPipController>(pipController, env);
    napi_wrap(env, objValue, jsPipController.release(), JsPipController::Finalizer, nullptr, nullptr);

    BindFunctions(env, objValue, "JsPipController");
    return objValue;
}

JsPipController::JsPipController(const sptr<PictureInPictureController>& pipController, napi_env env)
    : pipController_(pipController), env_(env)
{
    registerFunc_ = {
        { STATE_CHANGE_CB, &JsPipController::ProcessStateChangeRegister},
        { CONTROL_PANEL_ACTION_EVENT_CB, &JsPipController::ProcessActionEventRegister},
    };
    unRegisterFunc_ = {
        { STATE_CHANGE_CB, &JsPipController::ProcessStateChangeUnRegister},
        { CONTROL_PANEL_ACTION_EVENT_CB, &JsPipController::ProcessActionEventUnRegister},
    };
}

JsPipController::~JsPipController()
{
}

void JsPipController::Finalizer(napi_env env, void* data, void* hint)
{
    WLOGFD("Finalizer is called");
    std::unique_ptr<JsPipController>(static_cast<JsPipController*>(data));
}

napi_value JsPipController::StartPictureInPicture(napi_env env, napi_callback_info info)
{
    JsPipController* me = CheckParamsAndGetThis<JsPipController>(env, info);
    return (me != nullptr) ? me->OnStartPictureInPicture(env, info) : nullptr;
}

napi_value JsPipController::OnStartPictureInPicture(napi_env env, napi_callback_info info)
{
    WLOGI("OnStartPictureInPicture is called");
    std::lock_guard<std::mutex> lock(pipMutex_);
    if (PictureInPictureManager::ShouldAbortPipStart()) {
        WLOGFI("OnStartPictureInPicture abort");
        return NapiGetUndefined(env);
    }
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value callback = nullptr;
    if (argc > 0) {
        callback = GetType(env, argv[0]) == napi_function ? argv[0] : nullptr; // 1: index of callback
    }
    NapiAsyncTask::CompleteCallback complete =
        [weak = wptr<PictureInPictureController>(this->pipController_)]
            (napi_env env, NapiAsyncTask& task, int32_t status) {
            if (weak == nullptr) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_PIP_STATE_ABNORMALLY),
                    "JsPipController::OnStartPictureInPicture failed."));
                return;
            }
            WMError errCode = weak->StartPictureInPicture(StartPipType::USER_START);
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WM_JS_TO_ERROR_CODE_MAP.at(errCode)),
                    "JsPipController::OnStartPictureInPicture failed."));
                return;
            }
            task.Resolve(env, NapiGetUndefined(env));
        };
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsPipController::OnStartPictureInPicture", env,
        CreateAsyncTaskWithLastParam(env, callback, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsPipController::StopPictureInPicture(napi_env env, napi_callback_info info)
{
    JsPipController* me = CheckParamsAndGetThis<JsPipController>(env, info);
    return (me != nullptr) ? me->OnStopPictureInPicture(env, info) : nullptr;
}

napi_value JsPipController::OnStopPictureInPicture(napi_env env, napi_callback_info info)
{
    WLOGI("OnStopPictureInPicture is called");
    std::lock_guard<std::mutex> lock(pipMutex_);
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value callback = nullptr;
    if (argc > 0) {
        callback = GetType(env, argv[0]) == napi_function ? argv[0] : nullptr; // 1: index of callback
    }
    NapiAsyncTask::CompleteCallback complete =
        [weak = wptr<PictureInPictureController>(this->pipController_)]
            (napi_env env, NapiAsyncTask& task, int32_t status) {
            if (weak == nullptr) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "JsPipController::OnStopPictureInPicture failed."));
                return;
            }
            WMError errCode = weak->StopPictureInPicture(true, true, StopPipType::USER_STOP);
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WM_JS_TO_ERROR_CODE_MAP.at(errCode)),
                    "JsPipController::OnStopPictureInPicture failed."));
                return;
            }
            task.Resolve(env, NapiGetUndefined(env));
        };
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsPipController::OnStopPictureInPicture", env,
        CreateAsyncTaskWithLastParam(env, callback, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsPipController::SetAutoStartEnabled(napi_env env, napi_callback_info info)
{
    JsPipController* me = CheckParamsAndGetThis<JsPipController>(env, info);
    return (me != nullptr) ? me->OnSetAutoStartEnabled(env, info) : nullptr;
}

napi_value JsPipController::OnSetAutoStartEnabled(napi_env env, napi_callback_info info)
{
    WLOGI("OnSetAutoStartEnabled is called");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != 1) {
        WLOGFE("Argc count is invalid: %{public}zu", argc);
        return NapiThrowInvalidParam(env);
    }
    bool enable = false;
    if (!ConvertFromJsValue(env, argv[0], enable)) {
        WLOGFE("[NAPI]Failed to convert parameter to bool");
        return NapiGetUndefined(env);
    }
    if (pipController_ == nullptr) {
        WLOGFE("[NAPI]OnSetAutoStartEnabled error, controller is nullptr");
        return NapiGetUndefined(env);
    }
    pipController_->SetAutoStartEnabled(enable);
    return NapiGetUndefined(env);
}

napi_value JsPipController::UpdateContentSize(napi_env env, napi_callback_info info)
{
    JsPipController* me = CheckParamsAndGetThis<JsPipController>(env, info);
    return (me != nullptr) ? me->OnUpdateContentSize(env, info) : nullptr;
}

napi_value JsPipController::OnUpdateContentSize(napi_env env, napi_callback_info info)
{
    WLOGI("OnUpdateContentSize is called");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != NUMBER_TWO) {
        WLOGFE("Argc count is invalid: %{public}zu", argc);
        return NapiThrowInvalidParam(env);
    }
    uint32_t width = 0;
    if (!ConvertFromJsValue(env, argv[0], width) || width == 0) {
        WLOGFE("Failed to convert parameter to uint32_t or parameter is invalid");
        return NapiThrowInvalidParam(env);
    }
    uint32_t height = 0;
    if (!ConvertFromJsValue(env, argv[1], height) || height == 0) {
        WLOGFE("Failed to convert parameter to uint32_t or parameter is invalid");
        return NapiThrowInvalidParam(env);
    }
    if (pipController_ == nullptr) {
        WLOGFE("OnUpdateContentSize error, controller is nullptr");
        return NapiThrowInvalidParam(env);
    }
    pipController_->UpdateContentSize(width, height);
    return NapiGetUndefined(env);
}

napi_value JsPipController::RegisterCallback(napi_env env, napi_callback_info info)
{
    JsPipController* me = CheckParamsAndGetThis<JsPipController>(env, info);
    return (me != nullptr) ? me->OnRegisterCallback(env, info) : nullptr;
}

napi_value JsPipController::OnRegisterCallback(napi_env env, napi_callback_info info)
{
    WLOGI("OnRegisterCallback is called");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < NUMBER_TWO) {
        WLOGFE("JsPipController Params not match: %{public}zu", argc);
        return NapiThrowInvalidParam(env);
    }
    std::string cbType;
    if (!ConvertFromJsValue(env, argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        return NapiThrowInvalidParam(env);
    }
    napi_value value = argv[1];
    if (value == nullptr || !NapiIsCallable(env, value)) {
        WLOGFE("Callback is nullptr or not callable");
        return NapiThrowInvalidParam(env);
    }
    std::lock_guard<std::mutex> lock(mtx_);
    WmErrorCode ret = RegisterListenerWithType(env, cbType, value);
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("OnRegisterCallback failed");
        return NapiThrowInvalidParam(env);
    }
    return NapiGetUndefined(env);
}

WmErrorCode JsPipController::RegisterListenerWithType(napi_env env, const std::string& type, napi_value value)
{
    if (IfCallbackRegistered(env, type, value)) {
        WLOGFE("Callback already registered!");
        return WmErrorCode::WM_ERROR_INVALID_CALLING;
    }
    std::shared_ptr<NativeReference> callbackRef;
    napi_ref result = nullptr;
    napi_create_reference(env, value, 1, &result);
    callbackRef.reset(reinterpret_cast<NativeReference*>(result));
    jsCbMap_[type] = callbackRef;
    (this->*registerFunc_[type])();
    return WmErrorCode::WM_OK;
}

bool JsPipController::IfCallbackRegistered(napi_env env, const std::string& type, napi_value jsListenerObject)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGFI("methodName %{public}s not registered!", type.c_str());
        return false;
    }

    for (auto iter = jsCbMap_.begin(); iter != jsCbMap_.end();  ++iter) {
        bool isEquals = false;
        napi_strict_equals(env, jsListenerObject, iter->second->GetNapiValue(), &isEquals);
        if (isEquals) {
            WLOGFE("Callback already registered!");
            return true;
        }
    }
    return false;
}

void JsPipController::ProcessStateChangeRegister()
{
    if (jsCbMap_.empty() || jsCbMap_.find(STATE_CHANGE_CB) == jsCbMap_.end()) {
        WLOGFE("Register state change error");
        return;
    }
    if (pipController_ == nullptr) {
        WLOGFE("controller is nullptr");
        return;
    }
    sptr<IPiPLifeCycle> lifeCycle = new JsPipController::PiPLifeCycleImpl(env_, jsCbMap_[STATE_CHANGE_CB]);
    pipController_->SetPictureInPictureLifecycle(lifeCycle);
}

void JsPipController::ProcessActionEventRegister()
{
    if (jsCbMap_.empty() || jsCbMap_.find(CONTROL_PANEL_ACTION_EVENT_CB) == jsCbMap_.end()) {
        WLOGFE("Register action event error");
        return;
    }
    if (pipController_ == nullptr) {
        WLOGFE("controller is nullptr");
        return;
    }
    sptr<IPiPActionObserver> actionObserver =
        new JsPipController::PiPActionObserverImpl(env_, jsCbMap_[CONTROL_PANEL_ACTION_EVENT_CB]);
    pipController_->SetPictureInPictureActionObserver(actionObserver);
}

void JsPipController::ProcessStateChangeUnRegister()
{
    if (pipController_ == nullptr) {
        WLOGFE("controller is nullptr");
        return;
    }
    pipController_->SetPictureInPictureLifecycle(nullptr);
}

void JsPipController::ProcessActionEventUnRegister()
{
    if (pipController_ == nullptr) {
        WLOGFE("controller is nullptr");
        return;
    }
    pipController_->SetPictureInPictureActionObserver(nullptr);
}

napi_value JsPipController::UnregisterCallback(napi_env env, napi_callback_info info)
{
    JsPipController* me = CheckParamsAndGetThis<JsPipController>(env, info);
    return (me != nullptr) ? me->OnUnregisterCallback(env, info) : nullptr;
}

napi_value JsPipController::OnUnregisterCallback(napi_env env, napi_callback_info info)
{
    WLOGI("OnUnregisterCallback is called");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != 1) {
        WLOGFE("JsPipController Params not match: %{public}zu", argc);
        return NapiThrowInvalidParam(env);
    }
    std::string cbType;
    if (!ConvertFromJsValue(env, argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to string");
        return NapiThrowInvalidParam(env);
    }
    std::lock_guard<std::mutex> lock(mtx_);
    WmErrorCode ret = UnRegisterListenerWithType(env, cbType);
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("OnUnregisterCallback failed");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(ret)));
    }
    return NapiGetUndefined(env);
}

WmErrorCode JsPipController::UnRegisterListenerWithType(napi_env env, const std::string& type)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGFI("methodName %{public}s not registered!", type.c_str());
        return WmErrorCode::WM_ERROR_INVALID_CALLING;
    }
    jsCbMap_.erase(type);
    (this->*unRegisterFunc_[type])();
    return WmErrorCode::WM_OK;
}

void CallJsMethod(napi_env env, napi_value method, napi_value const* argv, size_t argc)
{
    if (method == nullptr) {
        WLOGFE("empty method, call method failed");
        return;
    }
    if (env == nullptr) {
        WLOGFE("env_ nullptr");
        return;
    }
    napi_call_function(env, NapiGetUndefined(env), method, argc, argv, nullptr);
}

void JsPipController::PiPLifeCycleImpl::OnPreparePictureInPictureStart()
{
    WLOGI("OnPreparePictureInPictureStart");
    OnPipListenerCallback(PipState::ABOUT_TO_START, 0);
}

void JsPipController::PiPLifeCycleImpl::OnPictureInPictureStart()
{
    WLOGI("OnPictureInPictureStart");
    OnPipListenerCallback(PipState::STARTED, 0);
}

void JsPipController::PiPLifeCycleImpl::OnPreparePictureInPictureStop()
{
    WLOGI("OnPreparePictureInPictureStop");
    OnPipListenerCallback(PipState::ABOUT_TO_STOP, 0);
}

void JsPipController::PiPLifeCycleImpl::OnPictureInPictureStop()
{
    WLOGI("OnPictureInPictureStop");
    OnPipListenerCallback(PipState::STOPPED, 0);
}

void JsPipController::PiPLifeCycleImpl::OnRestoreUserInterface()
{
    WLOGI("OnRestoreUserInterface");
    OnPipListenerCallback(PipState::ABOUT_TO_RESTORE, 0);
}

void JsPipController::PiPLifeCycleImpl::OnPictureInPictureOperationError(int32_t errorCode)
{
    WLOGI("OnPictureInPictureOperationError");
    OnPipListenerCallback(PipState::ERROR, errorCode);
}

void JsPipController::PiPLifeCycleImpl::OnPipListenerCallback(PipState state, int32_t errorCode)
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGI("OnPipListenerCallback is called, state: %{public}d", static_cast<int32_t>(state));
    auto jsCallback = jsCallBack_;
    std::string error = std::to_string(errorCode);
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback> (
        [jsCallback, state, error] (napi_env env, NapiAsyncTask &task, int32_t status) {
            napi_value argv[] = {CreateJsValue(env, static_cast<int32_t>(state)), CreateJsValue(env, error)};
            CallJsMethod(env, jsCallback->GetNapiValue(), argv, ArraySize(argv));
        }
    );

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsPipController::PiPLifeCycleImpl::OnPipListenerCallback",
        engine_, std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsPipController::PiPActionObserverImpl::OnActionEvent(const std::string& actionEvent)
{
    std::lock_guard<std::mutex> lock(mtx_);
    WLOGI("OnActionEvent is called, actionEvent: %{public}s", actionEvent.c_str());
    auto jsCallback = jsCallBack_;
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback> (
        [jsCallback, actionEvent] (napi_env env, NapiAsyncTask &task, int32_t status) {
            napi_value argv[] = {CreateJsValue(env, actionEvent)};
            CallJsMethod(env, jsCallback->GetNapiValue(), argv, ArraySize(argv));
        }
    );

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsPipController::PiPActionObserverImpl::OnActionEvent",
        engine_, std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}
} // namespace Rosen
} // namespace OHOS
