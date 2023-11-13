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

#include "js_pip_manager.h"
#include "window_manager_hilog.h"
#include "picture_in_picture_manager.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace
{
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsPipManager"};   
}

napi_value NapiGetUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

napi_value NapiThrowInvalidParam(napi_env env)
{
    napi_throw(env, AbilityRuntime::CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
    return NapiGetUndefined(env);
}

JsPipManager::JsPipManager()
{
    
}

JsPipManager::~JsPipManager()
{

}

void JsPipManager::Finalizer(napi_env env, void* data, void* hint)
{
    WLOGFD("Finalizer");
    std::unique_ptr<JsPipManager>(static_cast<JsPipManager*>(data));
}

napi_value JsPipManager::TriggerAction(napi_env env, napi_callback_info info)
{
    JsPipManager* me = CheckParamsAndGetThis<JsPipManager>(env, info);
    return (me != nullptr) ? me->OnTriggerAction(env, info) : nullptr;
}

napi_value JsPipManager::Restore(napi_env env, napi_callback_info info)
{
    JsPipManager* me = CheckParamsAndGetThis<JsPipManager>(env, info);
    return (me != nullptr) ? me->OnRestore(env, info) : nullptr;
}

napi_value JsPipManager::Close(napi_env env, napi_callback_info info)
{
     JsPipManager* me = CheckParamsAndGetThis<JsPipManager>(env, info);
    return (me != nullptr) ? me->OnClose(env, info) : nullptr;
}

napi_value JsPipManager::StartMove(napi_env env, napi_callback_info info)
{
     JsPipManager* me = CheckParamsAndGetThis<JsPipManager>(env, info);
    return (me != nullptr) ? me->OnStartMove(env, info) : nullptr;
}

napi_value JsPipManager::ProcessScale(napi_env env, napi_callback_info info)
{
     JsPipManager* me = CheckParamsAndGetThis<JsPipManager>(env, info);
    return (me != nullptr) ? me->OnProcessScale(env, info) : nullptr;
}

napi_value JsPipManager::InitXComponentController(napi_env env, napi_callback_info info)
{
     JsPipManager* me = CheckParamsAndGetThis<JsPipManager>(env, info);
    return (me != nullptr) ? me->OnInitXComponentController(env, info) : nullptr;
}

napi_value JsPipManager::OnTriggerAction(napi_env env, napi_callback_info info)
{
    WLOGFD("[NAPI]OnTriggerAction");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != 1) {
        WLOGFE("[NAPI]Argc count is invalid: %{public}zu", argc);
        return NapiThrowInvalidParam(env);
    }
    std::string actionName = "";
    if(!ConvertFromJsValue(env, argv[0], actionName)) {
        WLOGFE("[NAPI]Failed to convert paramter to string");
         return NapiGetUndefined(env);
    }
    PictureInPictureManager::DoActionEvent(actionName);
    return NapiGetUndefined(env);
}

napi_value JsPipManager::OnRestore(napi_env env, napi_callback_info info)
{
    WLOGFD("[NAPI]OnRestore");
    PictureInPictureManager::DoRestore();
    return NapiGetUndefined(env);
}

napi_value JsPipManager::OnClose(napi_env env, napi_callback_info info)
{
    WLOGFD("[NAPI]OnClose");
    PictureInPictureManager::DoClose(true);
    return NapiGetUndefined(env);
}

napi_value JsPipManager::OnStartMove(napi_env env, napi_callback_info info)
{
    WLOGFD("[NAPI]OnStartMove");
    PictureInPictureManager::DoStartMove();
    return NapiGetUndefined(env);
}

napi_value JsPipManager::OnProcessScale(napi_env env, napi_callback_info info)
{
    WLOGFD("[NAPI]OnProcessScale");
    PictureInPictureManager::DoScale();
    return NapiGetUndefined(env);
}

napi_value JsPipManager::OnInitXComponentController(napi_env env, napi_callback_info info)
{
    WLOGFD("[NAPI]OnInitXComponentController");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != 2) {
        WLOGFE("[NAPI]Argc count is invalid: %{public}zu", argc);
        return NapiThrowInvalidParam(env);
    }
    napi_value xComponentController = argv[0];
    void* xComponentControllerResult = nullptr;
    napi_unwrap(env, xComponentController, &xComponentControllerResult);
    int32_t windowId = 0;
    if (!ConvertFromJsValue(env, argv[1], windowId)) {
        WLOGFE("[NAPI]Failed to convert windowId to int");
        return NapiGetUndefined(env);
    }
    sptr<PictureInPictureController> controller = PictureInPictureManager::GetPipControllerInfo(windowId);
    if(controller == nullptr) {
        WLOGFE("[NAPI]Failed to get pipController");
        return NapiGetUndefined(env);
    }
    WMError err = controller->SetXComponentController(xComponentController);
    if (err != WMError::WM_OK) {
        WLOGFE("SetXComponentController failed");
    }
    return NapiGetUndefined(env);
}

napi_value JsPipManagerInit(napi_env env, napi_value exportObj)
{
    WLOGFD("JsPipManagerInit");
    if (env == nullptr || exportObj == nullptr) {
        WLOGFE("JsPipManagerInit failed, env or exportObj is null");
        return nullptr;
    }
    std::unique_ptr<JsPipManager> jsPipManager = std::make_unique<JsPipManager>();
    napi_wrap(env, exportObj, jsPipManager.release(), JsPipManager::Finalizer, nullptr, nullptr);
    const char* moduleName = "JsPipManager";
    BindNativeFunction(env, exportObj, "triggerAction", moduleName, JsPipManager::TriggerAction);
    BindNativeFunction(env, exportObj, "restore", moduleName, JsPipManager::Restore);
    BindNativeFunction(env, exportObj, "close", moduleName, JsPipManager::Close);
    BindNativeFunction(env, exportObj, "startMove", moduleName, JsPipManager::StartMove);
    BindNativeFunction(env, exportObj, "processScale", moduleName, JsPipManager::ProcessScale);
    BindNativeFunction(env, exportObj, "initXComponentController", moduleName, JsPipManager::InitXComponentController);
    return NapiGetUndefined(env);
}
} // namespace Rosen
} // namespace OHOS

