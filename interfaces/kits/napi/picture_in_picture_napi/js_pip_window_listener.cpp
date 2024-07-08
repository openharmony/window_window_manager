/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "js_pip_window_listener.h"

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
    //constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsListener"};
}

JsPiPWindowListener::~JsPiPWindowListener()
{
    TLOGI(WmsLogTag::WMS_PIP, "[NAPI]~JsWindowListener");
}

void JsPiPWindowListener::OnPreparePictureInPictureStart()
{
    OnPipListenerCallback(PiPState::ABOUT_TO_START, 0);
}

void JsPiPWindowListener::OnPictureInPictureStart()
{
    OnPipListenerCallback(PiPState::STARTED, 0);
}

void JsPiPWindowListener::OnPreparePictureInPictureStop()
{
    OnPipListenerCallback(PiPState::ABOUT_TO_STOP, 0);
}

void JsPiPWindowListener::OnPictureInPictureStop()
{
    OnPipListenerCallback(PiPState::STOPPED, 0);
}

void JsPiPWindowListener::OnRestoreUserInterface()
{
    OnPipListenerCallback(PiPState::ABOUT_TO_RESTORE, 0);
}

void JsPiPWindowListener::OnPictureInPictureOperationError(int32_t errorCode)
{
    OnPipListenerCallback(PiPState::ERROR, errorCode);
}

void CallBackJsMethod(napi_env env, napi_value method, napi_value const* argv, size_t argc)
{
    if (method == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "empty method, call method failed");
        return;
    }
    if (env == nullptr) {
        return;
    }
    napi_call_function(env, NapiGetUndefined(env), method, argc, argv, nullptr);
}

void JsPiPWindowListener::OnPipListenerCallback(PiPState state, int32_t errorCode)
{
    TLOGI(WmsLogTag::WMS_PIP, "state: %{public}d", static_cast<int32_t>(state));
    auto jsCallback = jsCallBack_;
    std::string error = std::to_string(errorCode);
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback> (
        [jsCallback, state, error] (napi_env env, NapiAsyncTask &task, int32_t status) {
            napi_value argv[] = {CreateJsValue(env, static_cast<int32_t>(state)), CreateJsValue(env, error)};
            CallBackJsMethod(env, jsCallback->GetNapiValue(), argv, ArraySize(argv));
        }
    );

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsPiPWindowListener::OnPipListenerCallback",
        env_, std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsPiPWindowListener::OnActionEvent(const std::string& actionEvent, int32_t statusCode)
{
    TLOGI(WmsLogTag::WMS_PIP, "actionEvent: %{public}s", actionEvent.c_str());
    napi_value result = nullptr;
    std::unique_ptr<NapiAsyncTask> napiAsyncTask = CreateEmptyAsyncTask(env_, nullptr, &result);
    auto asyncTask = [jsCallback = jsCallBack_, controlType, statusCode, env = env_, task = napiAsyncTask.get()]() {
        napi_value propertyValue = nullptr;
        napi_create_object(env, &propertyValue);
        if (propertyValue == nullptr) {
            TLOGI(WmsLogTag::WMS_PIP, "propertyValue is nullptr");
            return;
        }
        napi_set_named_property(env, propertyValue, "controlType", CreateJsValue(env, controlType));
        napi_set_named_property(env, propertyValue, "status", CreateJsValue(env, statusCode));
        napi_value argv[] = {propertyValue};
        CallBackJsMethod(env, jsCallback->GetNapiValue(), argv, ArraySize(argv));
        delete task;
    };
    if (napi_send_event(env_, asyncTask, napi_eprio_high) != napi_status::napi_ok) {
        napiAsyncTask->Reject(env_, CreateJsError(env_, 1, "send event failed"));
    } else {
        napiAsyncTask.release();
    }

    auto jsCallback = jsCallBack_;
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback> (
        [jsCallback, actionEvent, statusCode] (napi_env env, NapiAsyncTask &task, int32_t status) {
            napi_value argv[2] = {CreateJsValue(env, actionEvent), CreateJsValue(env, statusCode)};
            CallBackJsMethod(env, jsCallback->GetNapiValue(), argv, ArraySize(argv));
        }
    );

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsPipController::PiPActionObserverImpl::OnActionEvent",
        env_, std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsPiPWindowListener::OnControlEvent(PiPControlType controlType, PiPControlStatus statusCode)
{
    TLOGI(WmsLogTag::WMS_PIP, "controlType:%{public}u, statusCode:%{public}d", controlType, statusCode);
    napi_value result = nullptr;
    std::unique_ptr<NapiAsyncTask> napiAsyncTask = CreateEmptyAsyncTask(env_, nullptr, &result);
    auto asyncTask = [jsCallback = jsCallBack_, controlType, statusCode, env = env_, task = napiAsyncTask.get()]() {
        napi_value propertyValue = nullptr;
        napi_create_object(env, &propertyValue);
        if (propertyValue == nullptr) {
            TLOGI(WmsLogTag::WMS_PIP, "propertyValue is nullptr");
            return;
        }
        napi_set_named_property(env, propertyValue, "controlType", CreateJsValue(env, controlType));
        napi_set_named_property(env, propertyValue, "status", CreateJsValue(env, statusCode));
        napi_value argv[] = {propertyValue};
        CallBackJsMethod(env, jsCallback->GetNapiValue(), argv, ArraySize(argv));
        delete task;
    };
    if (napi_send_event(env_, asyncTask, napi_eprio_high) != napi_status::napi_ok) {
        napiAsyncTask->Reject(env_, CreateJsError(env_, 1, "send event failed"));
    } else {
        napiAsyncTask.release();
    }
}
} // namespace Rosen
} // namespace OHOS