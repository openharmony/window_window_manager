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
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
constexpr size_t ARGC_TWO = 2;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "jsPipController"};
}

void BindFunctions(napi_env env, napi_value object, const char *moduleName)
{
    BindNativeFunction(env, object, "startPip", moduleName, JsPipController::StartPictureInPicture);
    BindNativeFunction(env, object, "stopPip", moduleName, JsPipController::StopPictureInPicture);
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
    std::unique_ptr<JsPipController> jsPipController = std::make_unique<JsPipController>(pipController);
    napi_wrap(env, objValue, jsPipController.release(), JsPipController::Finalizer, nullptr, nullptr);

    BindFunctions(env, objValue, "JsPipController");
    return objValue;
}

JsPipController::JsPipController(const sptr<PictureInPictureController>& pipController) : pipController_(pipController)
{
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
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value callback = nullptr;
    if (argc > 0) {
        callback = GetType(env, argv[0]) == napi_function ? argv[0] : nullptr; // 1: index of callback
    }
    NapiAsyncTask::CompleteCallback complete =
        [this](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (this->pipController_ == nullptr) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_PIP_STATE_ABNORMALLY),
                    "JsPipController::OnStartPictureInPicture failed."));
                return;
            }
            WMError errCode = this->pipController_->StartPictureInPicture();
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WM_JS_TO_ERROR_CODE_MAP.at(errCode)),
                    "JsPipController::OnStartPictureInPicture failed."));
                return;
            }
            task.Resolve(env, NapiGetUndefined(env));
            WLOGI("JsPipController::OnStartPictureInPicture success");
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
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value callback = nullptr;
    if (argc > 0) {
        callback = GetType(env, argv[0]) == napi_function ? argv[0] : nullptr; // 1: index of callback
    }
    NapiAsyncTask::CompleteCallback complete =
        [this](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (this->pipController_ == nullptr) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "JsPipController::OnStopPictureInPicture failed."));
                return;
            }
            WMError errCode = this->pipController_->StopPictureInPicture(true);
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WM_JS_TO_ERROR_CODE_MAP.at(errCode)),
                    "JsPipController::OnStopPictureInPicture failed."));
                return;
            }
            task.Resolve(env, NapiGetUndefined(env));
            WLOGI("JsPipController::OnStopPictureInPicture success");
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
    if (argc != ARGC_TWO) {
        WLOGFE("Argc count is invalid: %{public}zu", argc);
        return NapiThrowInvalidParam(env);
    }
    uint32_t width = 0;
    if (!ConvertFromJsValue(env, argv[0], width) || width <= 0) {
        WLOGFE("Failed to convert parameter to uint32_t or parameter is invalid");
        return NapiThrowInvalidParam(env);
    }
    uint32_t height = 0;
    if (!ConvertFromJsValue(env, argv[1], height) || height <= 0) {
        WLOGFE("Failed to convert parameter to uint32_t or parameter is invalid");
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
    return NapiGetUndefined(env);
}

napi_value JsPipController::UnregisterCallback(napi_env env, napi_callback_info info)
{
    JsPipController* me = CheckParamsAndGetThis<JsPipController>(env, info);
    return (me != nullptr) ? me->OnUnregisterCallback(env, info) : nullptr;
}

napi_value JsPipController::OnUnregisterCallback(napi_env env, napi_callback_info info)
{
    WLOGI("OnUnregisterCallback is called");
    return NapiGetUndefined(env);
}
} // namespace Rosen
} // namespace OHOS
