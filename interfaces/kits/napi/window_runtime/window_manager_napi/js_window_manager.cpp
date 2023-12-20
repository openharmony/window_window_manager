/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "js_window_manager.h"
#include <ability.h>
#include <cinttypes>
#include <hitrace_meter.h>
#include <new>
#include <transaction/rs_interfaces.h>
#include "ability_context.h"
#include "display_manager.h"
#include "dm_common.h"
#include "wm_common.h"
#include "js_window.h"
#include "js_window_utils.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "window_option.h"
#include "pixel_map_napi.h"
#include "permission.h"
#include "singleton_container.h"
namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsWindowManager"};
}

JsWindowManager::JsWindowManager() : registerManager_(std::make_unique<JsWindowRegisterManager>())
{
}

JsWindowManager::~JsWindowManager()
{
}

void JsWindowManager::Finalizer(napi_env env, void* data, void* hint)
{
    WLOGI("Finalizer");
    std::unique_ptr<JsWindowManager>(static_cast<JsWindowManager*>(data));
}

napi_value JsWindowManager::Create(napi_env env, napi_callback_info info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(env, info);
    return (me != nullptr) ? me->OnCreate(env, info) : nullptr;
}

napi_value JsWindowManager::CreateWindow(napi_env env, napi_callback_info info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(env, info);
    return (me != nullptr) ? me->OnCreateWindow(env, info) : nullptr;
}

napi_value JsWindowManager::FindWindow(napi_env env, napi_callback_info info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(env, info);
    return (me != nullptr) ? me->OnFindWindow(env, info) : nullptr;
}

napi_value JsWindowManager::FindWindowSync(napi_env env, napi_callback_info info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(env, info);
    return (me != nullptr) ? me->OnFindWindowSync(env, info) : nullptr;
}

napi_value JsWindowManager::MinimizeAll(napi_env env, napi_callback_info info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(env, info);
    return (me != nullptr) ? me->OnMinimizeAll(env, info) : nullptr;
}

napi_value JsWindowManager::ToggleShownStateForAllAppWindows(napi_env env, napi_callback_info info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(env, info);
    return (me != nullptr) ? me->OnToggleShownStateForAllAppWindows(env, info) : nullptr;
}

napi_value JsWindowManager::RegisterWindowManagerCallback(napi_env env, napi_callback_info info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(env, info);
    return (me != nullptr) ? me->OnRegisterWindowManagerCallback(env, info) : nullptr;
}

napi_value JsWindowManager::UnregisterWindowMangerCallback(napi_env env, napi_callback_info info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(env, info);
    return (me != nullptr) ? me->OnUnregisterWindowManagerCallback(env, info) : nullptr;
}

napi_value JsWindowManager::GetTopWindow(napi_env env, napi_callback_info info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(env, info);
    return (me != nullptr) ? me->OnGetTopWindow(env, info) : nullptr;
}

napi_value JsWindowManager::GetLastWindow(napi_env env, napi_callback_info info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(env, info);
    return (me != nullptr) ? me->OnGetLastWindow(env, info) : nullptr;
}

napi_value JsWindowManager::SetWindowLayoutMode(napi_env env, napi_callback_info info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(env, info);
    return (me != nullptr) ? me->OnSetWindowLayoutMode(env, info) : nullptr;
}

napi_value JsWindowManager::SetGestureNavigationEnabled(napi_env env, napi_callback_info info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(env, info);
    return (me != nullptr) ? me->OnSetGestureNavigationEnabled(env, info) : nullptr;
}

napi_value JsWindowManager::SetWaterMarkImage(napi_env env, napi_callback_info info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(env, info);
    return (me != nullptr) ? me->OnSetWaterMarkImage(env, info) : nullptr;
}

napi_value JsWindowManager::ShiftAppWindowFocus(napi_env env, napi_callback_info info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(env, info);
    return (me != nullptr) ? me->OnShiftAppWindowFocus(env, info) : nullptr;
}

static void GetNativeContext(napi_env env, napi_value nativeContext, void*& contextPtr, WMError& errCode)
{
    AppExecFwk::Ability* ability = nullptr;
    bool isOldApi = GetAPI7Ability(env, ability);
    WLOGFD("FA mode:%{public}u", isOldApi);
    if (isOldApi) {
        return;
    }
    if (nativeContext != nullptr) {
        napi_unwrap(env, nativeContext, &contextPtr);
    }
}

static uint32_t GetParentId(napi_env env)
{
    AppExecFwk::Ability* ability = nullptr;
    uint32_t parentId = 0;
    bool isOldApi = GetAPI7Ability(env, ability);
    if (isOldApi) {
        if (ability == nullptr) {
            WLOGE("FA mode GetAPI7Ability failed");
            return parentId;
        }
        auto window = ability->GetWindow();
        if (window == nullptr) {
            WLOGE("Get mainWindow failed");
            return parentId;
        }
        parentId = window->GetWindowId();
    }
    return parentId;
}

static bool GetWindowTypeAndParentId(napi_env env, uint32_t& parentId, WindowType& winType,
    napi_value nativeString, napi_value nativeType)
{
    napi_value type = nativeType;
    if (type == nullptr) {
        WLOGFE("Failed to convert parameter to windowType");
        return false;
    }
    uint32_t resultValue = 0;
    napi_get_value_uint32(env, type, &resultValue);
    if (resultValue >= static_cast<uint32_t>(ApiWindowType::TYPE_BASE) &&
        resultValue < static_cast<uint32_t>(ApiWindowType::TYPE_END)) {
        winType = JS_TO_NATIVE_WINDOW_TYPE_MAP.at(static_cast<ApiWindowType>(resultValue));
    } else {
        WLOGFE("Type %{public}u is not supported", resultValue);
        return false;
    }

    AppExecFwk::Ability* ability = nullptr;
    bool isOldApi = GetAPI7Ability(env, ability);
    if (isOldApi) {
        if (ability == nullptr || !WindowHelper::IsSubWindow(winType)) {
            WLOGE("FA mode GetAPI7Ability failed or type %{public}u is not subWinodw", winType);
            return false;
        }
        auto window = ability->GetWindow();
        if (window == nullptr) {
            WLOGE("Get mainWindow failed");
            return false;
        }
        parentId = window->GetWindowId();
    } else {
        if (!WindowHelper::IsSystemWindow(winType)) {
            WLOGFE("Only SystemWindow support create in stage mode, type is %{public}u", winType);
            return false;
        }
    }
    return true;
}

static void CreateNewSystemWindowTask(void* contextPtr, sptr<WindowOption> windowOption,
    napi_env env, NapiAsyncTask& task)
{
    WLOGI("CreateSystemWindowTask");
    if (windowOption == nullptr) {
        int32_t err = static_cast<int32_t>(WmErrorCode::WM_ERROR_SYSTEM_ABNORMALLY);
        task.Reject(env, CreateJsError(env, err, "New window option failed"));
        WLOGFE("New window option failed");
        return;
    }
    auto context = static_cast<std::weak_ptr<AbilityRuntime::Context>*>(contextPtr);
    if (contextPtr == nullptr || context == nullptr) {
        int32_t err = static_cast<int32_t>(WmErrorCode::WM_ERROR_CONTEXT_ABNORMALLY);
        task.Reject(env, CreateJsError(env, err, "Context is nullptr"));
        WLOGFE("Context is nullptr");
        return;
    }
    if (windowOption->GetWindowType() == WindowType::WINDOW_TYPE_FLOAT ||
        windowOption->GetWindowType() == WindowType::WINDOW_TYPE_FLOAT_CAMERA) {
        auto abilityContext = Context::ConvertTo<AbilityRuntime::AbilityContext>(context->lock());
        if (abilityContext != nullptr) {
            if (!CheckCallingPermission("ohos.permission.SYSTEM_FLOAT_WINDOW")) {
                int32_t err = static_cast<int32_t>(WmErrorCode::WM_ERROR_NO_PERMISSION);
                task.Reject(env, CreateJsError(env, err, "TYPE_FLOAT CheckCallingPermission failed"));
                return;
            }
        }
    }
    WMError wmError = WMError::WM_OK;
    sptr<Window> window = Window::Create(windowOption->GetWindowName(), windowOption, context->lock(), wmError);
    WmErrorCode wmErrorCode = WM_JS_TO_ERROR_CODE_MAP.at(wmError);
    if (window != nullptr && wmErrorCode == WmErrorCode::WM_OK) {
        task.Resolve(env, CreateJsWindowObject(env, window));
    } else {
        WLOGFE("Create window failed");
        int32_t err = static_cast<int32_t>(wmErrorCode);
        task.Reject(env, CreateJsError(env, err, "Create window failed"));
    }
}

static void CreateSystemWindowTask(void* contextPtr, std::string windowName, WindowType winType,
    napi_env env, NapiAsyncTask& task)
{
    WLOGFD("CreateSystemWindowTask");
    auto context = static_cast<std::weak_ptr<AbilityRuntime::Context>*>(contextPtr);
    if (contextPtr == nullptr || context == nullptr) {
        int32_t err = static_cast<int32_t>(WMError::WM_ERROR_NULLPTR);
        task.Reject(env, CreateJsError(env, err, "Context is nullptr"));
        WLOGFE("Context is nullptr");
        return;
    }
    if (winType == WindowType::WINDOW_TYPE_FLOAT || winType == WindowType::WINDOW_TYPE_FLOAT_CAMERA) {
        auto abilityContext = Context::ConvertTo<AbilityRuntime::AbilityContext>(context->lock());
        if (abilityContext != nullptr) {
            if (!CheckCallingPermission("ohos.permission.SYSTEM_FLOAT_WINDOW")) {
                int32_t err = static_cast<int32_t>(WMError::WM_ERROR_INVALID_PERMISSION);
                task.Reject(env, CreateJsError(env, err, "TYPE_FLOAT CheckCallingPermission failed"));
                return;
            }
        }
    }
    sptr<WindowOption> windowOption = new(std::nothrow) WindowOption();
    if (windowOption == nullptr) {
        int32_t err = static_cast<int32_t>(WMError::WM_ERROR_NULLPTR);
        task.Reject(env, CreateJsError(env, err, "New window option failed"));
        WLOGFE("New window option failed");
        return;
    }
    windowOption->SetWindowType(winType);
    WMError wmError = WMError::WM_OK;
    sptr<Window> window = Window::Create(windowName, windowOption, context->lock(), wmError);
    WmErrorCode wmErrorCode = WM_JS_TO_ERROR_CODE_MAP.at(wmError);
    if (window != nullptr && wmErrorCode == WmErrorCode::WM_OK) {
        task.Resolve(env, CreateJsWindowObject(env, window));
    } else {
        WLOGFE("Create window failed");
        int32_t err = static_cast<int32_t>(wmErrorCode);
        task.Reject(env, CreateJsError(env, err, "Create window failed"));
    }
}

static void CreateNewSubWindowTask(sptr<WindowOption> windowOption, napi_env env, NapiAsyncTask& task)
{
    if (windowOption == nullptr) {
        int32_t err = static_cast<int32_t>(WmErrorCode::WM_ERROR_SYSTEM_ABNORMALLY);
        task.Reject(env, CreateJsError(env, err, "New window option failed"));
        WLOGFE("New window option failed");
        return;
    }
    windowOption->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    if (windowOption->GetParentId() == INVALID_WINDOW_ID) {
        uint32_t parentId = GetParentId(env);
        if (!parentId) {
            int32_t err = static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
            task.Reject(env, CreateJsError(env, err, "parent window missed"));
            WLOGFE("can not find parent window");
            return;
        }
        windowOption->SetParentId(parentId);
    }
    sptr<Window> window = Window::Create(windowOption->GetWindowName(), windowOption);
    if (window != nullptr) {
        task.Resolve(env, CreateJsWindowObject(env, window));
    } else {
        WLOGFE("Create window failed");
        int32_t err = static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        task.Reject(env, CreateJsError(env, err, "Create window failed"));
    }
}

static void CreateSubWindowTask(uint32_t parentWinId, std::string windowName, WindowType winType,
    napi_env env, NapiAsyncTask& task, bool newErrorCode = false)
{
    WLOGI("CreateSubWindowTask, parent id = %{public}u", parentWinId);
    sptr<WindowOption> windowOption = new(std::nothrow) WindowOption();
    if (windowOption == nullptr) {
        int32_t err = newErrorCode ? static_cast<int32_t>(WmErrorCode::WM_ERROR_SYSTEM_ABNORMALLY)
            : static_cast<int32_t>(WMError::WM_ERROR_NULLPTR);
        task.Reject(env, CreateJsError(env, err, "New window option failed"));
        WLOGFE("New window option failed");
        return;
    }
    windowOption->SetWindowType(winType);
    windowOption->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    windowOption->SetParentId(parentWinId);
    sptr<Window> window = Window::Create(windowName, windowOption);
    if (window != nullptr) {
        task.Resolve(env, CreateJsWindowObject(env, window));
    } else {
        WLOGFE("Create window failed");
        int32_t err = newErrorCode ? static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)
            : static_cast<int32_t>(WMError::WM_ERROR_NULLPTR);
        task.Reject(env, CreateJsError(env, err, "Create window failed"));
    }
}

static bool isConfigOptionWindowTypeValid(napi_env env, WindowOption& option)
{
    WindowType type = option.GetWindowType();
    AppExecFwk::Ability* ability = nullptr;
    bool isOldApi = GetAPI7Ability(env, ability);
    if (isOldApi) {
        if (ability == nullptr || !WindowHelper::IsSubWindow(type)) {
            WLOGE("FA mode GetAPI7Ability failed or convert parameter to invalid winType %{public}u", type);
            return false;
        }
    } else {
        if (!WindowHelper::IsSystemWindow(type)) {
            WLOGFE("Stage mode convert parameter to invalid winType %{public}u", type);
            return false;
        }
    }

    return true;
}

napi_value JsWindowManager::OnCreate(napi_env env, napi_callback_info info)
{
    WLOGFD("OnCreate");
    napi_value nativeString = nullptr, nativeContext = nullptr, nativeType = nullptr, callback = nullptr;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc >= 2 && GetType(env, argv[0]) == napi_string) { // 2: minimum params num
        nativeString = argv[0];
        nativeType = argv[1];
        callback = (argc == 2) ? nullptr : (GetType(env, argv[2]) == napi_function ? argv[2] : nullptr); // 2: index
    } else if (argc >= 3) { // 3: minimum params num
        nativeContext = GetType(env, argv[0]) == napi_object ? argv[0] : nullptr;
        nativeString = argv[1];
        nativeType = argv[2]; // 2: index of type
        callback = (argc == 3) ? nullptr : (GetType(env, argv[3]) == napi_function ? argv[3] : nullptr); // 3: index
    }
    std::string windowName;
    WMError errCode = WMError::WM_OK;
    if (!ConvertFromJsValue(env, nativeString, windowName)) {
        WLOGFE("Failed to convert parameter to windowName");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    uint32_t parentId = INVALID_WINDOW_ID;
    WindowType winType = WindowType::SYSTEM_WINDOW_BASE;
    if (errCode == WMError::WM_OK &&
        !GetWindowTypeAndParentId(env, parentId, winType, nativeString, nativeType)) {
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    void* contextPtr = nullptr;
    GetNativeContext(env, nativeContext, contextPtr, errCode);

    WLOGFD("Window name = %{public}s, type = %{public}u, err = %{public}d", windowName.c_str(), winType, errCode);
    NapiAsyncTask::CompleteCallback complete =
        [=](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode), "Invalidate params"));
                return;
            }
            if (parentId == INVALID_WINDOW_ID) {
                return CreateSystemWindowTask(contextPtr, windowName, winType, env, task);
            } else {
                return CreateSubWindowTask(parentId, windowName, winType, env, task);
            }
        };
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindowManager::OnCreate", env,
        CreateAsyncTaskWithLastParam(env, callback, nullptr, std::move(complete), &result));
    return result;
}

bool JsWindowManager::ParseConfigOption(napi_env env, napi_value jsObject,
    WindowOption& option, void*& contextPtr)
{
    std::string windowName;
    if (ParseJsValue(jsObject, env, "name", windowName)) {
        option.SetWindowName(windowName);
    } else {
        WLOGFE("Failed to convert parameter to windowName");
        return false;
    }

    uint32_t winType;
    if (ParseJsValue(jsObject, env, "windowType", winType)) {
        if (winType >= static_cast<uint32_t>(ApiWindowType::TYPE_BASE) &&
            winType < static_cast<uint32_t>(ApiWindowType::TYPE_END)) {
            option.SetWindowType(JS_TO_NATIVE_WINDOW_TYPE_MAP.at(static_cast<ApiWindowType>(winType)));
        } else {
            option.SetWindowType(static_cast<WindowType>(winType));
        }
    } else {
        WLOGFE("Failed to convert parameter to winType");
        return false;
    }

    if (!isConfigOptionWindowTypeValid(env, option)) {
        return false;
    }

    napi_value value = nullptr;
    napi_get_named_property(env, jsObject, "ctx", &value);
    if (GetType(env, value) == napi_undefined) {
        return true;
    }
    WMError errCode = WMError::WM_OK;
    GetNativeContext(env, value, contextPtr, errCode);
    if (errCode != WMError::WM_OK) {
        return false;
    }

    int64_t displayId = static_cast<int64_t>(DISPLAY_ID_INVALID);
    if (ParseJsValue(jsObject, env, "displayId", displayId)) {
        if (displayId < 0 ||
            SingletonContainer::Get<DisplayManager>().GetDisplayById(static_cast<uint64_t>(displayId)) == nullptr) {
            return false;
        }
        option.SetDisplayId(displayId);
    } else {
        return true;
    }

    int64_t parentId = -1;
    if (ParseJsValue(jsObject, env, "parentId", parentId)) {
        option.SetParentId(parentId);
    }

    return true;
}

napi_value JsWindowManager::OnCreateWindow(napi_env env, napi_callback_info info)
{
    WLOGFD("Called");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    napi_value nativeObj = argv[0];
    if (nativeObj == nullptr) {
        WLOGFE("Failed to convert object to CreateWindow");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    WindowOption option;
    void* contextPtr = nullptr;
    if (!ParseConfigOption(env, nativeObj, option, contextPtr)) {
        WLOGFE("Failed to parse config");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    napi_value callback = nullptr;
    if (argc > 1) {
        callback = GetType(env, argv[1]) == napi_function ? argv[1] : nullptr; // 1: index of callback
    }
    NapiAsyncTask::CompleteCallback complete =
        [=](napi_env env, NapiAsyncTask& task, int32_t status) {
            sptr<WindowOption> windowOption = new WindowOption(option);
            if (WindowHelper::IsSystemWindow(option.GetWindowType())) {
                return CreateNewSystemWindowTask(contextPtr, windowOption, env, task);
            }
            if (WindowHelper::IsSubWindow(option.GetWindowType())) {
                return CreateNewSubWindowTask(windowOption, env, task);
            }
        };

    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindowManager::OnCreateWindow", env,
        CreateAsyncTaskWithLastParam(env, callback, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindowManager::OnFindWindow(napi_env env, napi_callback_info info)
{
    WLOGFD("OnFindWindow");
    std::string windowName;
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1 || argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    } else {
        if (!ConvertFromJsValue(env, argv[0], windowName)) {
            WLOGFE("Failed to convert parameter to windowName");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        }
    }
    WLOGI("Window name = %{public}s, err = %{public}d", windowName.c_str(), errCode);
    NapiAsyncTask::CompleteCallback complete =
        [=](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode), "Invalidate params"));
                return;
            }
            HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "WM:Find %s", windowName.c_str());
            std::shared_ptr<NativeReference> jsWindowObj = FindJsWindowObject(windowName);
            if (jsWindowObj != nullptr && jsWindowObj->GetNapiValue() != nullptr) {
                WLOGI("Find window: %{public}s, use exist js window", windowName.c_str());
                task.Resolve(env, jsWindowObj->GetNapiValue());
            } else {
                sptr<Window> window = Window::Find(windowName);
                if (window == nullptr) {
                    WLOGFE("Cannot find window: %{public}s", windowName.c_str());
                    task.Reject(env, CreateJsError(env,
                        static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "Cannot find window"));
                } else {
                    task.Resolve(env, CreateJsWindowObject(env, window));
                    WLOGI("Find window: %{public}s, create js window", windowName.c_str());
                }
            }
        };

    napi_value lastParam = (argc <= 1) ? nullptr :
        (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindowManager::OnFindWindow",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindowManager::OnFindWindowSync(napi_env env, napi_callback_info info)
{
    WLOGFD("OnFindWindowSync");
    std::string windowName;
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    } else {
        if (!ConvertFromJsValue(env, argv[0], windowName)) {
            WLOGFE("Failed to convert parameter to windowName");
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    WLOGI("Window name = %{public}s, err = %{public}d", windowName.c_str(), errCode);
    std::shared_ptr<NativeReference> jsWindowObj = FindJsWindowObject(windowName);
    if (jsWindowObj != nullptr && jsWindowObj->GetNapiValue() != nullptr) {
        WLOGI("Find window: %{public}s, use exist js window", windowName.c_str());
        return jsWindowObj->GetNapiValue();
    } else {
        sptr<Window> window = Window::Find(windowName);
        if (window == nullptr) {
            napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
            return NapiGetUndefined(env);
        } else {
            return CreateJsWindowObject(env, window);
        }
    }
}

napi_value JsWindowManager::OnMinimizeAll(napi_env env, napi_callback_info info)
{
    WLOGI("OnMinimizeAll");
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    int64_t displayId = static_cast<int64_t>(DISPLAY_ID_INVALID);
    if (errCode == WmErrorCode::WM_OK && !ConvertFromJsValue(env, argv[0], displayId)) {
        WLOGFE("Failed to convert parameter to displayId");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    if (displayId < 0 ||
        SingletonContainer::Get<DisplayManager>().GetDisplayById(static_cast<uint64_t>(displayId)) == nullptr) {
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        WLOGFE("JsWindowManager::OnMinimizeAll failed, Invalidate params.");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    WLOGI("Display id = %{public}" PRIu64", err = %{public}d", static_cast<uint64_t>(displayId), errCode);
    NapiAsyncTask::CompleteCallback complete =
        [=](napi_env env, NapiAsyncTask& task, int32_t status) {
            HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "WM:MinimizeAll: " PRIu64"",
                static_cast<uint64_t>(displayId));
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
                SingletonContainer::Get<WindowManager>().MinimizeAllAppWindows(static_cast<uint64_t>(displayId)));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
                WLOGFI("OnMinimizeAll success");
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "OnMinimizeAll failed"));
            }
        };
    napi_value lastParam = (argc <= 1) ? nullptr :
        ((argv[1] != nullptr && GetType(env, argv[1]) == napi_function) ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindowManager::OnMinimizeAll",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindowManager::OnToggleShownStateForAllAppWindows(napi_env env, napi_callback_info info)
{
    WLOGI("OnToggleShownStateForAllAppWindows");
    NapiAsyncTask::CompleteCallback complete =
        [=](napi_env env, NapiAsyncTask& task, int32_t status) {
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
                SingletonContainer::Get<WindowManager>().ToggleShownStateForAllAppWindows());
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
                WLOGI("OnToggleShownStateForAllAppWindows success");
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret),
                    "OnToggleShownStateForAllAppWindows failed"));
            }
        };
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value lastParam = (argc <= 0) ? nullptr :
        (GetType(env, argv[0]) == napi_function ? argv[0] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindowManager::OnToggleShownStateForAllAppWindows",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindowManager::OnRegisterWindowManagerCallback(napi_env env, napi_callback_info info)
{
    WLOGFD("OnRegisterWindowManagerCallback");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 2) { // 2: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    std::string cbType;
    if (!ConvertFromJsValue(env, argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    napi_value value = argv[1];
    if (!NapiIsCallable(env, value)) {
        WLOGI("Callback(argv[1]) is not callable");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    WmErrorCode ret = registerManager_->RegisterListener(nullptr, cbType, CaseType::CASE_WINDOW_MANAGER, env, value);
    if (ret != WmErrorCode::WM_OK) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(ret)));
        return NapiGetUndefined(env);
    }
    WLOGI("Register end, type = %{public}s", cbType.c_str());
    return NapiGetUndefined(env);
}

napi_value JsWindowManager::OnUnregisterWindowManagerCallback(napi_env env, napi_callback_info info)
{
    WLOGFD("OnUnregisterWindowManagerCallback");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    std::string cbType;
    if (!ConvertFromJsValue(env, argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    napi_value value = nullptr;
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (argc == 1) {
        ret = registerManager_->UnregisterListener(nullptr, cbType, CaseType::CASE_WINDOW_MANAGER, env, value);
    } else {
        value = argv[1];
        if ((value == nullptr) || (!NapiIsCallable(env, value))) {
            ret = registerManager_->UnregisterListener(nullptr, cbType, CaseType::CASE_WINDOW_MANAGER, env, nullptr);
        } else {
            ret = registerManager_->UnregisterListener(nullptr, cbType, CaseType::CASE_WINDOW_MANAGER, env, value);
        }
    }
    if (ret != WmErrorCode::WM_OK) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(ret)));
        return NapiGetUndefined(env);
    }
    WLOGI("Unregister end, type = %{public}s", cbType.c_str());
    return NapiGetUndefined(env);
}

static void GetTopWindowTask(void* contextPtr, napi_env env, NapiAsyncTask& task, bool newApi)
{
    std::string windowName;
    sptr<Window> window = nullptr;
    AppExecFwk::Ability* ability = nullptr;
    bool isOldApi = GetAPI7Ability(env, ability);
    int32_t error;
    if (isOldApi) {
        if (ability->GetWindow() == nullptr) {
            error = newApi ? static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY) :
                static_cast<int32_t>(WMError::WM_ERROR_NULLPTR);
            task.Reject(env, CreateJsError(env, error, "FA mode can not get ability window"));
            WLOGE("FA mode can not get ability window");
            return;
        }
        window = Window::GetTopWindowWithId(ability->GetWindow()->GetWindowId());
    } else {
        auto context = static_cast<std::weak_ptr<AbilityRuntime::Context>*>(contextPtr);
        if (contextPtr == nullptr || context == nullptr) {
            error = newApi ? static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY) :
                static_cast<int32_t>(WMError::WM_ERROR_NULLPTR);
            task.Reject(env, CreateJsError(env, error, "Stage mode without context"));
            WLOGFE("Stage mode without context");
            return;
        }
        window = Window::GetTopWindowWithContext(context->lock());
    }
    if (window == nullptr) {
        error = newApi ? static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY) :
            static_cast<int32_t>(WMError::WM_ERROR_NULLPTR);
        task.Reject(env, CreateJsError(env, error, "Get top window failed"));
        WLOGFE("Get top window failed");
        return;
    }
    windowName = window->GetWindowName();
    std::shared_ptr<NativeReference> jsWindowObj = FindJsWindowObject(windowName);
    if (jsWindowObj != nullptr && jsWindowObj->GetNapiValue() != nullptr) {
        task.Resolve(env, jsWindowObj->GetNapiValue());
    } else {
        task.Resolve(env, CreateJsWindowObject(env, window));
    }
    WLOGD("Get top window %{public}s success", windowName.c_str());
    return;
}

napi_value JsWindowManager::OnGetTopWindow(napi_env env, napi_callback_info info)
{
    WLOGFD("OnGetTopWindow");
    WMError errCode = WMError::WM_OK;
    napi_value nativeContext = nullptr;
    napi_value nativeCallback = nullptr;
    void* contextPtr = nullptr;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc > 2) { // 2: maximum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    } else {
        if (argc > 0 && GetType(env, argv[0]) == napi_object) { // (context, callback?)
            nativeContext = argv[0];
            nativeCallback = (argc == 1) ? nullptr :
                (GetType(env, argv[1]) == napi_function ? argv[1] : nullptr);
        } else { // (callback?)
            nativeCallback = (argc == 0) ? nullptr :
                (GetType(env, argv[0]) == napi_function ? argv[0] : nullptr);
        }
        GetNativeContext(env, nativeContext, contextPtr, errCode);
    }

    WLOGI("err %{public}u", errCode);
    NapiAsyncTask::CompleteCallback complete =
        [=](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (errCode != WMError::WM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(errCode), "Invalidate params"));
                return;
            }
            return GetTopWindowTask(contextPtr, env, task, false);
        };
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindowManager::OnGetTopWindow",
        env, CreateAsyncTaskWithLastParam(env, nativeCallback, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindowManager::OnGetLastWindow(napi_env env, napi_callback_info info)
{
    WLOGFD("OnGetLastWindow");
    WMError errCode = WMError::WM_OK;
    napi_value nativeContext = nullptr;
    napi_value nativeCallback = nullptr;
    void* contextPtr = nullptr;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    } else {
        nativeContext = argv[0];
        nativeCallback = (argc == 1) ? nullptr : argv[1];
        GetNativeContext(env, nativeContext, contextPtr, errCode);
    }
    if (errCode != WMError::WM_OK) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    NapiAsyncTask::CompleteCallback complete =
        [=](napi_env env, NapiAsyncTask& task, int32_t status) {
            return GetTopWindowTask(contextPtr, env, task, true);
        };
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindowManager::OnGetTopWindow",
        env, CreateAsyncTaskWithLastParam(env, nativeCallback, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindowManager::OnSetWindowLayoutMode(napi_env env, napi_callback_info info)
{
    WLOGFD("OnSetWindowLayoutMode");
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: minimum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    WindowLayoutMode winLayoutMode = WindowLayoutMode::CASCADE;
    if (errCode == WmErrorCode::WM_OK) {
        napi_value nativeMode = argv[0];
        if (nativeMode == nullptr) {
            WLOGFE("Failed to convert parameter to windowLayoutMode");
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            uint32_t resultValue = 0;
            napi_get_value_uint32(env, nativeMode, &resultValue);
            winLayoutMode = static_cast<WindowLayoutMode>(resultValue);
        }
    }
    if (winLayoutMode != WindowLayoutMode::CASCADE && winLayoutMode != WindowLayoutMode::TILE) {
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        WLOGFE("JsWindowManager::OnSetWindowLayoutMode failed, Invalidate params.");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    WLOGI("LayoutMode = %{public}u, err = %{public}d", winLayoutMode, errCode);
    NapiAsyncTask::CompleteCallback complete =
        [=](napi_env env, NapiAsyncTask& task, int32_t status) {
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
                SingletonContainer::Get<WindowManager>().SetWindowLayoutMode(winLayoutMode));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
                WLOGD("SetWindowLayoutMode success");
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "SetWindowLayoutMode failed"));
            }
        };
    // 1: maximum params num; 1: index of callback
    napi_value lastParam = (argc <= 1) ? nullptr :
        ((argv[1] != nullptr && GetType(env, argv[1]) == napi_function) ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindowManager::OnSetWindowLayoutMode",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindowManager::OnSetGestureNavigationEnabled(napi_env env, napi_callback_info info)
{
    WLOGFD("OnSetGestureNavigationEnabled");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: minimum params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    napi_value nativeBool = argv[0];
    if (nativeBool == nullptr) {
        WLOGFE("Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    bool gestureNavigationEnable = false;
    napi_get_value_bool(env, nativeBool, &gestureNavigationEnable);

    WLOGI("Set gesture navigation enable as %{public}d", gestureNavigationEnable);
    NapiAsyncTask::CompleteCallback complete =
        [gestureNavigationEnable](napi_env env, NapiAsyncTask& task, int32_t status) {
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
                SingletonContainer::Get<WindowManager>().SetGestureNavigaionEnabled(gestureNavigationEnable));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
                WLOGD("SetGestureNavigationEnabled success");
            } else {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(ret), "SetGestureNavigationEnabled failed"));
            }
        };
    // 1: maximum params num; 1: index of callback
    napi_value lastParam = (argc <= 1) ? nullptr :
        ((argv[1] != nullptr && GetType(env, argv[1]) == napi_function) ? argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindowManager::OnSetGestureNavigationEnabled",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindowManager::OnSetWaterMarkImage(napi_env env, napi_callback_info info)
{
    WLOGFD("OnSetWaterMarkImage");
    napi_value nativeObject = nullptr;
    napi_value nativeBoolean = nullptr;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 2) { // 2: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    } else {
        if (argc > 0 && GetType(env, argv[0]) == napi_object) {
            nativeObject = argv[0];
            nativeBoolean = (argc == 1) ? nullptr : (GetType(env, argv[1]) == napi_boolean ? argv[1] : nullptr);
        }
    }
    
    std::shared_ptr<Media::PixelMap> pixelMap;
    pixelMap = OHOS::Media::PixelMapNapi::GetPixelMap(env, nativeObject);
    if (pixelMap == nullptr) {
        WLOGFE("Failed to convert parameter to PixelMap");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    if (nativeBoolean == nullptr) {
        WLOGFE("Failed to convert parameter to bool");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    bool isShow = false;
    napi_get_value_bool(env, nativeBoolean, &isShow);
    if (!Permission::IsSystemCalling()) {
        WLOGFE("set watermark image permission denied!");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_NOT_SYSTEM_APP)));
        return NapiGetUndefined(env);
    }

    NapiAsyncTask::CompleteCallback complete =
        [=](napi_env env, NapiAsyncTask& task, int32_t status) {
            RSInterfaces::GetInstance().ShowWatermark(pixelMap, isShow);
            task.Resolve(env, NapiGetUndefined(env));
            WLOGD("OnSetWaterMarkImage success");
        };
    // 2: maximum params num; 2: index of callback
    napi_value lastParam = (argc <= 2) ? nullptr :
        (GetType(env, argv[2]) == napi_function ? argv[2] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindowManager::OnSetWaterMarkImage",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindowManager::OnShiftAppWindowFocus(napi_env env, napi_callback_info info)
{
    WLOGFD("OnShiftAppWindowFocus");
    WMError errCode = WMError::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != 2) { // 2: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    int32_t sourcePersistentId = static_cast<int32_t>(INVALID_WINDOW_ID);
    int32_t targetPersistentId = static_cast<int32_t>(INVALID_WINDOW_ID);
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(env, argv[0], sourcePersistentId)) {
        WLOGFE("Failed to convert parameter to source window Id");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(env, argv[1], targetPersistentId)) {
        WLOGFE("Failed to convert parameter to target window Id");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    if (errCode == WMError::WM_ERROR_INVALID_PARAM) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    NapiAsyncTask::CompleteCallback complete =
        [=](napi_env env, NapiAsyncTask& task, int32_t status) {
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
                SingletonContainer::Get<WindowManager>().ShiftAppWindowFocus(sourcePersistentId, targetPersistentId));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
                WLOGD("OnShiftAppWindowFocus success");
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "ShiftAppWindowFocus failed"));
            }
        };
    // only return promiss<void>
    napi_value lastParam = nullptr;
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsWindowManager::OnShiftAppWindowFocus",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsWindowManagerInit(napi_env env, napi_value exportObj)
{
    WLOGFD("JsWindowManagerInit");

    if (env == nullptr || exportObj == nullptr) {
        WLOGFE("JsWindowManagerInit env or exportObj is nullptr");
        return nullptr;
    }

    std::unique_ptr<JsWindowManager> jsWinManager = std::make_unique<JsWindowManager>();
    napi_wrap(env, exportObj, jsWinManager.release(), JsWindowManager::Finalizer, nullptr, nullptr);
    napi_set_named_property(env, exportObj, "WindowType", WindowTypeInit(env));
    napi_set_named_property(env, exportObj, "AvoidAreaType", AvoidAreaTypeInit(env));
    napi_set_named_property(env, exportObj, "WindowMode", WindowModeInit(env));
    napi_set_named_property(env, exportObj, "ColorSpace", ColorSpaceInit(env));
    napi_set_named_property(env, exportObj, "WindowStageEventType", WindowStageEventTypeInit(env));
    napi_set_named_property(env, exportObj, "WindowEventType", WindowEventTypeInit(env));
    napi_set_named_property(env, exportObj, "WindowLayoutMode", WindowLayoutModeInit(env));
    napi_set_named_property(env, exportObj, "Orientation", OrientationInit(env));
    napi_set_named_property(env, exportObj, "BlurStyle", BlurStyleInit(env));
    napi_set_named_property(env, exportObj, "WmErrorCode", WindowErrorCodeInit(env));
    napi_set_named_property(env, exportObj, "WMError", WindowErrorInit(env));
    const char *moduleName = "JsWindowManager";
    BindNativeFunction(env, exportObj, "create", moduleName, JsWindowManager::Create);
    BindNativeFunction(env, exportObj, "createWindow", moduleName, JsWindowManager::CreateWindow);
    BindNativeFunction(env, exportObj, "find", moduleName, JsWindowManager::FindWindow);
    BindNativeFunction(env, exportObj, "findWindow", moduleName, JsWindowManager::FindWindowSync);
    BindNativeFunction(env, exportObj, "on", moduleName, JsWindowManager::RegisterWindowManagerCallback);
    BindNativeFunction(env, exportObj, "off", moduleName, JsWindowManager::UnregisterWindowMangerCallback);
    BindNativeFunction(env, exportObj, "getTopWindow", moduleName, JsWindowManager::GetTopWindow);
    BindNativeFunction(env, exportObj, "getLastWindow", moduleName, JsWindowManager::GetLastWindow);
    BindNativeFunction(env, exportObj, "minimizeAll", moduleName, JsWindowManager::MinimizeAll);
    BindNativeFunction(env, exportObj, "toggleShownStateForAllAppWindows", moduleName,
        JsWindowManager::ToggleShownStateForAllAppWindows);
    BindNativeFunction(env, exportObj, "setWindowLayoutMode", moduleName, JsWindowManager::SetWindowLayoutMode);
    BindNativeFunction(env, exportObj, "setGestureNavigationEnabled", moduleName,
        JsWindowManager::SetGestureNavigationEnabled);
    BindNativeFunction(env, exportObj, "setWaterMarkImage", moduleName, JsWindowManager::SetWaterMarkImage);
    BindNativeFunction(env, exportObj, "shiftAppWindowFocus", moduleName, JsWindowManager::ShiftAppWindowFocus);
    return NapiGetUndefined(env);
}
}  // namespace Rosen
}  // namespace OHOS
