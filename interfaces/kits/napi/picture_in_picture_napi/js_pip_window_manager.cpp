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
#include "window_scene_session_impl.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
constexpr int32_t ARGC_ONE = 1;

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

    void* contextPtr = nullptr;
    std::string navigationId;
    uint32_t templateTyle;
    uint32_t width;
    uint32_t height;

    napi_get_named_property(env, optionObject, "ctx", &contextPtrValue);
    napi_get_named_property(env, optionObject, "navigationId", &navigationIdValue);
    napi_get_named_property(env, optionObject, "templateType", &templateTypeValue);
    napi_get_named_property(env, optionObject, "contentWidth", &widthValue);
    napi_get_named_property(env, optionObject, "contentHeight", &heightValue);

    napi_unwrap(env, contextPtrValue, &contextPtr);
    if (!ConvertFromJsValue(env, navigationIdValue, navigationId)) {
        WLOGFE("Failed to convert navigationIdValue to stringType");
        return -1;
    }
    if (!ConvertFromJsValue(env, templateTypeValue, templateTyle)) {
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
    option.SetContext(contextPtr);
    option.SetNavigationId(navigationId);
    option.SetPipTemplate(templateTyle);
    option.SetContentSize(width, height);
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

napi_value JsPipWindowManager::IsPictureInPictureEnabled(napi_env env, napi_callback_info info)
{
    JsPipWindowManager* me = CheckParamsAndGetThis<JsPipWindowManager>(env, info);
    return (me != nullptr) ? me->OnIsPictureInPictureEnabled(env, info) : nullptr;
}

napi_value JsPipWindowManager::OnIsPictureInPictureEnabled(napi_env env, napi_callback_info info)
{
    WLOGFD("OnIsSupportPictureInPicture");
    bool state = true;
    return CreateJsValue(env, state);
}

napi_value JsPipWindowManager::CreatePictureInPictureController(napi_env env, napi_callback_info info)
{
    JsPipWindowManager* me = CheckParamsAndGetThis<JsPipWindowManager>(env, info);
    return (me != nullptr) ? me->OnCreatePictureInPictureController(env, info) : nullptr;
}

napi_value JsPipWindowManager::OnCreatePictureInPictureController(napi_env env, napi_callback_info info)
{
    WLOGI("OnCreatePictureInPictureController is called");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != ARGC_ONE) {
        WLOGFE("Argc count is invalid: %{public}zu", argc);
        return NapiThrowInvalidParam(env);
    }
    napi_value config = argv[0];
    PipOption pipOption;
    int32_t errCode = GetPictureInPictureOptionFromJs(env, config, pipOption);
    if (errCode == -1) {
        WLOGFE("Configuration is invalid: %{public}zu", argc);
        return NapiThrowInvalidParam(env);
    }
    NapiAsyncTask::CompleteCallback complete =
        [pipOption](napi_env env, NapiAsyncTask& task, int32_t status) {
            sptr<PipOption> pipOptionPtr = new PipOption(pipOption);
            auto context = static_cast<std::weak_ptr<AbilityRuntime::Context>*>(pipOption.GetContext());
            sptr<WindowSessionImpl> mainWindow = WindowSceneSessionImpl::GetMainWindowWithContext(context->lock());
            sptr<PictureInPictureController> pipController =
                new PictureInPictureController(pipOptionPtr, mainWindow->GetPersistentId());
            task.Resolve(env, CreateJsPipControllerObject(env, pipController));
        };
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsPipWindowManager::OnCreatePictureInPictureController", env,
        CreateAsyncTaskWithLastParam(env, nullptr, nullptr, std::move(complete), &result));
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
    BindNativeFunction(env, exportObj, "create", moduleName, JsPipWindowManager::CreatePictureInPictureController);
    BindNativeFunction(env, exportObj, "isPictureInPictureEnabled", moduleName,
        JsPipWindowManager::IsPictureInPictureEnabled);
    InitEnums(env, exportObj);
    return NapiGetUndefined(env);
}
}
}