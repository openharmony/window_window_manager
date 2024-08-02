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
#include "xcomponent_controller.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
using namespace Ace;
namespace {
    constexpr int32_t NUMBER_ONE = 1;
}

napi_value NapiGetUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

napi_value NapiThrowInvalidParam(napi_env env, std::string msg = "")
{
    napi_throw(env, AbilityRuntime::CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM), msg));
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
    TLOGD(WmsLogTag::WMS_PIP, "[NAPI]JsPipManager::Finalizer");
    std::unique_ptr<JsPipManager>(static_cast<JsPipManager*>(data));
}

napi_value JsPipManager::InitXComponentController(napi_env env, napi_callback_info info)
{
    JsPipManager* me = CheckParamsAndGetThis<JsPipManager>(env, info);
    return (me != nullptr) ? me->OnInitXComponentController(env, info) : nullptr;
}

napi_value JsPipManager::OnInitXComponentController(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_PIP, "[NAPI]JsPipManager::OnInitXComponentController");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < NUMBER_ONE) {
        TLOGE(WmsLogTag::WMS_PIP, "[NAPI]Argc count is invalid: %{public}zu", argc);
        return NapiThrowInvalidParam(env);
    }
    napi_value xComponentController = argv[0];
    std::shared_ptr<XComponentController> xComponentControllerResult =
        XComponentController::GetXComponentControllerFromNapiValue(env, xComponentController);
    sptr<Window> pipWindow = Window::Find(PIP_WINDOW_NAME);
    if (!pipWindow) {
        TLOGE(WmsLogTag::WMS_PIP, "[NAPI]Failed to find pip window");
        return NapiGetUndefined(env);
    }
    int32_t windowId = static_cast<int32_t>(pipWindow->GetWindowId());
    sptr<PictureInPictureController> pipController = PictureInPictureManager::GetPipControllerInfo(windowId);
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "[NAPI]Failed to get pictureInPictureController");
        return NapiGetUndefined(env);
    }
    TLOGI(WmsLogTag::WMS_PIP, "[NAPI]set xComponentController to window: %{public}u", windowId);
    WMError errCode = pipController->SetXComponentController(xComponentControllerResult);
    if (errCode != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_PIP, "[NAPI]Failed to set xComponentController");
    }
    return NapiGetUndefined(env);
}

napi_value JsPipManager::GetCustomUIController(napi_env env, napi_callback_info info)
{
    JsPipManager* me = CheckParamsAndGetThis<JsPipManager>(env, info);
    return (me != nullptr) ? me->OnGetCustomUIController(env, info) : nullptr;
}

napi_value JsPipManager::OnGetCustomUIController(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_PIP, "[NAPI]");
    sptr<Window> pipWindow = Window::Find(PIP_WINDOW_NAME);
    if (pipWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "[NAPI]Failed to find pip window");
        return NapiGetUndefined(env);
    }
    int32_t windowId = static_cast<int32_t>(pipWindow->GetWindowId());
    TLOGI(WmsLogTag::WMS_PIP, "[NAPI]winId: %{public}u", windowId);
    sptr<PictureInPictureController> pipController = PictureInPictureManager::GetPipControllerInfo(windowId);
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "[NAPI]Failed to get pictureInPictureController");
        return NapiGetUndefined(env);
    }
    napi_ref ref = pipController->GetCustomNodeController();
    if (ref == nullptr) {
        TLOGI(WmsLogTag::WMS_PIP, "[NAPI] invalid custom UI controller");
        return NapiGetUndefined(env);
    }
    napi_value uiController = nullptr;
    napi_get_reference_value(env, ref, &uiController);
    return uiController;
}

napi_value JsPipManager::GetTypeNode(napi_env env, napi_callback_info info)
{
    JsPipManager* me = CheckParamsAndGetThis<JsPipManager>(env, info);
    return (me != nullptr) ? me->OnGetTypeNode(env, info) : nullptr;
}

napi_value JsPipManager::OnGetTypeNode(napi_env env, napi_callback_info info)
{
    TLOGD(WmsLogTag::WMS_PIP, "[NAPI]");
    sptr<Window> pipWindow = Window::Find(PIP_WINDOW_NAME);
    if (pipWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "[NAPI]Failed to find pip window");
        return NapiGetUndefined(env);
    }
    int32_t windowId = static_cast<int32_t>(pipWindow->GetWindowId());
    TLOGI(WmsLogTag::WMS_PIP, "[NAPI]winId: %{public}u", windowId);
    sptr<PictureInPictureController> pipController = PictureInPictureManager::GetPipControllerInfo(windowId);
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "[NAPI]Failed to get pictureInPictureController");
        return NapiGetUndefined(env);
    }
    napi_ref ref = pipController->GetTypeNode();
    if (ref == nullptr) {
        TLOGI(WmsLogTag::WMS_PIP, "[NAPI] invalid typeNode");
        return NapiGetUndefined(env);
    }
    napi_value typeNode = nullptr;
    napi_get_reference_value(env, ref, &typeNode);
    return typeNode;
}

void JsPipManager::RegisterListener(napi_env env, const std::string type, napi_value value)
{
    std::shared_ptr<NativeReference> callbackRef;
    napi_ref result = nullptr;
    napi_create_reference(env, value, 1, &result);
    callbackRef.reset(reinterpret_cast<NativeReference*>(result));
    PictureInPictureManager::callbackRef_ = callbackRef;
    TLOGI(WmsLogTag::WMS_PIP, "Register type %{public}s success!", type.c_str());
}

napi_value JsPipManager::RegisterCallback(napi_env env, napi_callback_info info)
{
    JsPipManager* me = CheckParamsAndGetThis<JsPipManager>(env, info);
    return (me != nullptr) ? me->OnRegisterCallback(env, info) : nullptr;
}

bool NapiIsCallable(napi_env env, napi_value value)
{
    bool result = false;
    napi_is_callable(env, value, &result);
    return result;
}

napi_value JsPipManager::OnRegisterCallback(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_PIP, "[NAPI]");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 2) {
        TLOGE(WmsLogTag::WMS_PIP, "Params count not match: %{public}zu", argc);
        return NapiThrowInvalidParam(env);
    }
    std::string cbType;
    if (!ConvertFromJsValue(env, argv[0], cbType)) {
        TLOGE(WmsLogTag::WMS_PIP, "Failed to convert param to cbType");
        return NapiThrowInvalidParam(env);
    }
    napi_value value = argv[1];
    if (value == nullptr || !NapiIsCallable(env, value)) {
        TLOGE(WmsLogTag::WMS_PIP, "Callback is null or not callable");
        return NapiThrowInvalidParam(env);
    }
    RegisterListener(env, cbType, value);
    return NapiGetUndefined(env);
}

napi_value JsPipManager::UnregisterCallback(napi_env env, napi_callback_info info)
{
    JsPipManager* me = CheckParamsAndGetThis<JsPipManager>(env, info);
    return (me != nullptr) ? me->OnUnregisterCallback(env, info) : nullptr;
}

napi_value JsPipManager::OnUnregisterCallback(napi_env env, napi_callback_info info)
{
    TLOGI(WmsLogTag::WMS_PIP, "[NAPI]");
    PictureInPictureManager::callbackRef_ = nullptr;
    return NapiGetUndefined(env);
}

napi_value JsPipManagerInit(napi_env env, napi_value exportObj)
{
    TLOGD(WmsLogTag::WMS_PIP, "[NAPI]JsPipManager::JsPipManagerInit");
    if (env == nullptr || exportObj == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "JsPipManagerInit failed, env or exportObj is null");
        return nullptr;
    }
    std::unique_ptr<JsPipManager> jsPipManager = std::make_unique<JsPipManager>();
    napi_wrap(env, exportObj, jsPipManager.release(), JsPipManager::Finalizer, nullptr, nullptr);
    const char* moduleName = "JsPipManager";
    BindNativeFunction(env, exportObj, "initXComponentController", moduleName, JsPipManager::InitXComponentController);
    BindNativeFunction(env, exportObj, "getCustomUIController", moduleName, JsPipManager::GetCustomUIController);
    BindNativeFunction(env, exportObj, "getTypeNode", moduleName, JsPipManager::GetTypeNode);
    BindNativeFunction(env, exportObj, "on", moduleName, JsPipManager::RegisterCallback);
    BindNativeFunction(env, exportObj, "off", moduleName, JsPipManager::UnregisterCallback);
    return NapiGetUndefined(env);
}
} // namespace Rosen
} // namespace OHOS

