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

#include "js_pip_window_manager.h"

#include "js_pip_controller.h"
#include "js_pip_utils.h"
#include "js_runtime_utils.h"
#include "window_manager_hilog.h"
#include "window.h"
#include "xcomponent_controller.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
using namespace Ace;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsPipWindowManager"};
}

static int32_t GetPictureInPictureOptionFromJs(napi_env env, napi_value optionObject, PipOption& option)
{
    napi_value contextPtrValue = nullptr;
    napi_value navigationIdValue = nullptr;
    napi_value templateTypeValue = nullptr;
    napi_value widthValue = nullptr;
    napi_value heightValue = nullptr;
    napi_value xComponentControllerValue = nullptr;
    void* contextPtr = nullptr;
    std::string navigationId;
    uint32_t templateType;
    uint32_t width;
    uint32_t height;

    napi_get_named_property(env, optionObject, "context", &contextPtrValue);
    napi_get_named_property(env, optionObject, "navigationId", &navigationIdValue);
    napi_get_named_property(env, optionObject, "templateType", &templateTypeValue);
    napi_get_named_property(env, optionObject, "contentWidth", &widthValue);
    napi_get_named_property(env, optionObject, "contentHeight", &heightValue);
    napi_get_named_property(env, optionObject, "componentController", &xComponentControllerValue);
    napi_unwrap(env, contextPtrValue, &contextPtr);
    if (!ConvertFromJsValue(env, navigationIdValue, navigationId)) {
        WLOGFE("Failed to convert navigationIdValue to stringType");
        return -1;
    }
    if (!ConvertFromJsValue(env, templateTypeValue, templateType)) {
        WLOGFE("Failed to convert templateTypeValue to uint32_tType");
        return -1;
    }
    if (!ConvertFromJsValue(env, widthValue, width)) {
        WLOGFE("Failed to convert widthValue to uint32_tType");
        return -1;
    }
    if (!ConvertFromJsValue(env, heightValue, height)) {
        WLOGFE("Failed to convert heightValue to uint32_tType");
        return -1;
    }
    std::shared_ptr<XComponentController> xComponentControllerResult =
        XComponentController::GetXComponentControllerFromNapiValue(xComponentControllerValue);
    option.SetContext(contextPtr);
    option.SetNavigationId(navigationId);
    option.SetPipTemplate(templateType);
    option.SetContentSize(width, height);
    option.SetXComponentController(xComponentControllerResult);
    return 0;
}

JsPipWindowManager::JsPipWindowManager()
{
}

JsPipWindowManager::~JsPipWindowManager()
{
}

void JsPipWindowManager::Finalizer(napi_env env, void* data, void* hint)
{
    WLOGFD("Finalizer");
    std::unique_ptr<JsPipWindowManager>(static_cast<JsPipWindowManager*>(data));
}

napi_value JsPipWindowManager::IsPipEnabled(napi_env env, napi_callback_info info)
{
    JsPipWindowManager* me = CheckParamsAndGetThis<JsPipWindowManager>(env, info);
    return (me != nullptr) ? me->OnIsPipEnabled(env, info) : nullptr;
}

napi_value JsPipWindowManager::OnIsPipEnabled(napi_env env, napi_callback_info info)
{
    WLOGFD("OnIsPipEnabled called");
    return CreateJsValue(env, true);
}

napi_value JsPipWindowManager::CreatePipController(napi_env env, napi_callback_info info)
{
    JsPipWindowManager* me = CheckParamsAndGetThis<JsPipWindowManager>(env, info);
    return (me != nullptr) ? me->OnCreatePipController(env, info) : nullptr;
}

napi_value JsPipWindowManager::OnCreatePipController(napi_env env, napi_callback_info info)
{
    WLOGI("OnCreatePipController called");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Missing args when creating pipController");
        return NapiThrowInvalidParam(env);
    }
    napi_value config = argv[0];
    if (config == nullptr) {
        WLOGFE("Failed to convert object to pip Configuration");
        return NapiThrowInvalidParam(env);
    }
    PipOption pipOption;
    int32_t errCode = GetPictureInPictureOptionFromJs(env, config, pipOption);
    if (errCode == -1) {
        WLOGFE("Configuration is invalid: %{public}zu", argc);
        return NapiThrowInvalidParam(env);
    }
    napi_value callback = nullptr;
    if (argc > 1) {
        callback = GetType(env, argv[1]) == napi_function ? argv[1] : nullptr; // 1: index of callback
    }
    NapiAsyncTask::CompleteCallback complete =
        [=](napi_env env, NapiAsyncTask& task, int32_t status) {
            sptr<PipOption> pipOptionPtr = new PipOption(pipOption);
            auto context = static_cast<std::weak_ptr<AbilityRuntime::Context>*>(pipOptionPtr->GetContext());
            if (context == nullptr) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(
                    WMError::WM_ERROR_PIP_INTERNAL_ERROR), "Invalid context"));
                return;
            }
            sptr<Window> mainWindow = Window::GetTopWindowWithContext(context->lock());
            sptr<PictureInPictureController> pipController =
                new PictureInPictureController(pipOptionPtr, mainWindow->GetWindowId(), env);
            task.Resolve(env, CreateJsPipControllerObject(env, pipController));
        };
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsPipWindowManager::OnCreatePipController", env,
        CreateAsyncTaskWithLastParam(env, callback, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsPipWindowManagerInit(napi_env env, napi_value exportObj)
{
    WLOGFD("JsPipWindowManagerInit");
    if (env == nullptr || exportObj == nullptr) {
        WLOGFE("JsPipWindowManagerInit env or exportObj is nullptr");
        return nullptr;
    }
    std::unique_ptr<JsPipWindowManager> jsPipManager = std::make_unique<JsPipWindowManager>();
    napi_wrap(env, exportObj, jsPipManager.release(), JsPipWindowManager::Finalizer, nullptr, nullptr);
    const char* moduleName = "JsPipWindowManager";
    BindNativeFunction(env, exportObj, "create", moduleName, JsPipWindowManager::CreatePipController);
    BindNativeFunction(env, exportObj, "isPiPEnabled", moduleName, JsPipWindowManager::IsPipEnabled);
    InitEnums(env, exportObj);
    return NapiGetUndefined(env);
}
}
}