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
#include <new>
#include "ability_context.h"
#include "display_manager.h"
#include "dm_common.h"
#include "js_window.h"
#include "js_window_utils.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "window_option.h"
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
void JsWindowManager::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFI("JsWindowManager::Finalizer is called");
    std::unique_ptr<JsWindowManager>(static_cast<JsWindowManager*>(data));
}

NativeValue* JsWindowManager::CreateWindow(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(engine, info);
    return (me != nullptr) ? me->OnCreateWindow(*engine, *info) : nullptr;
}

NativeValue* JsWindowManager::FindWindow(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(engine, info);
    return (me != nullptr) ? me->OnFindWindow(*engine, *info) : nullptr;
}

NativeValue* JsWindowManager::MinimizeAll(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(engine, info);
    return (me != nullptr) ? me->OnMinimizeAll(*engine, *info) : nullptr;
}

NativeValue* JsWindowManager::RegisterWindowManagerCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(engine, info);
    return (me != nullptr) ? me->OnRegisterWindowMangerCallback(*engine, *info) : nullptr;
}

NativeValue* JsWindowManager::UnregisterWindowMangerCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(engine, info);
    return (me != nullptr) ? me->OnUnregisterWindowManagerCallback(*engine, *info) : nullptr;
}

NativeValue* JsWindowManager::GetTopWindow(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(engine, info);
    return (me != nullptr) ? me->OnGetTopWindow(*engine, *info) : nullptr;
}

NativeValue* JsWindowManager::SetWindowLayoutMode(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(engine, info);
    return (me != nullptr) ? me->OnSetWindowLayoutMode(*engine, *info) : nullptr;
}

static bool GetAPI7Ability(NativeEngine& engine, AppExecFwk::Ability* &ability)
{
    napi_value global;
    auto env = reinterpret_cast<napi_env>(&engine);
    if (napi_get_global(env, &global) != napi_ok) {
        WLOGFE("JsWindowManager::GetAPI7Ability get global failed");
        return false;
    }
    napi_value jsAbility;
    napi_status status = napi_get_named_property(env, global, "ability", &jsAbility);
    if (status != napi_ok || jsAbility == nullptr) {
        WLOGFE("JsWindowManager::GetAPI7Ability get global failed");
        return false;
    }

    if (napi_get_value_external(env, jsAbility, reinterpret_cast<void **>(&ability)) != napi_ok) {
        WLOGFE("JsWindowManager::GetAPI7Ability get global failed");
        return false;
    }
    if (ability == nullptr) {
        return false;
    } else {
        WLOGI("JsWindowManager::GetAPI7Ability ability is success!");
    }
    return true;
}

static void GetNativeContext(NativeValue* nativeContext, void*& contextPtr, WMError& errCode)
{
    if (nativeContext != nullptr) {
        auto objContext = AbilityRuntime::ConvertNativeValueTo<NativeObject>(nativeContext);
        if (objContext == nullptr) {
            WLOGFE("ConvertNativeValueTo Context Object failed");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
            return;
        }
        contextPtr = objContext->GetNativePointer();
    }
}

static bool GetWindowTypeAndParentName(NativeEngine& engine, std::string& parentName, WindowType& winType,
    NativeValue* nativeString, NativeValue* nativeType)
{
    NativeNumber* type = ConvertNativeValueTo<NativeNumber>(nativeType);
    if (type == nullptr) {
        WLOGFE("Failed to convert parameter to windowType");
        return false;
    }
    // adapt to the old version
    if (static_cast<uint32_t>(*type) >= static_cast<uint32_t>(WindowType::SYSTEM_WINDOW_BASE)) {
        winType = static_cast<WindowType>(static_cast<uint32_t>(*type));
    } else {
        if (static_cast<uint32_t>(*type) >= static_cast<uint32_t>(ApiWindowType::TYPE_BASE) &&
            static_cast<uint32_t>(*type) <= static_cast<uint32_t>(ApiWindowType::TYPE_END)) {
            winType = JS_TO_NATIVE_WINDOW_TYPE_MAP.at(static_cast<ApiWindowType>(static_cast<uint32_t>(*type)));
        } else {
            WLOGFE("Do not surppot this type");
            return false;
        }
    }
    AppExecFwk::Ability* ability = nullptr;
    bool isOldApi = GetAPI7Ability(engine, ability);
    if (isOldApi) {
        if (ability == nullptr || !WindowHelper::IsSubWindow(winType)) {
            WLOGE("JsWindowManager FA mode GetAPI7Ability failed or type should be subWinodw!");
            return false;
        }
        auto window = ability->GetWindow();
        if (window == nullptr) {
            WLOGE("JsWindowManager CheckJsWindowType in oldApi get mainWindow failed");
            return false;
        }
        parentName = window->GetWindowName();
    } else {
        if (!WindowHelper::IsSystemWindow(winType)) {
            WLOGFE("Only SystemWindow support create in stage mode!");
            return false;
        }
    }
    return true;
}

static void CreateSystemWindowTask(void* contextPtr, std::string windowName, WindowType winType,
    NativeEngine& engine, AsyncTask& task)
{
    WLOGFI("JsWindowManager::CreateSystemWindowTask is called");
    auto context = static_cast<std::weak_ptr<AbilityRuntime::Context>*>(contextPtr);
    if (contextPtr == nullptr || context == nullptr) {
        task.Reject(engine, CreateJsError(engine,
            static_cast<int32_t>(WMError::WM_ERROR_NULLPTR),
            "JsWindow::OnCreateWindow newAPI failed."));
        WLOGFE("JsWindowManager::OnCreateWindow in newApi use with empty context!");
        return;
    }
    if (winType == WindowType::WINDOW_TYPE_FLOAT) {
        auto abilityContext = Context::ConvertTo<AbilityRuntime::AbilityContext>(context->lock());
        if (abilityContext != nullptr) {
            if (!CheckCallingPermission("ohos.permission.SYSTEM_FLOAT_WINDOW")) {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WMError::WM_ERROR_INVALID_PERMISSION),
                    "JsWindow::OnCreateWindow newAPI failed."));
                WLOGFE("JsWindowManager::OnCreateWindow in newApi TYPE_FLOAT CheckCallingPermission failed!");
                return;
            }
        }
    }
    sptr<WindowOption> windowOption = new(std::nothrow) WindowOption();
    if (windowOption == nullptr) {
        task.Reject(engine, CreateJsError(engine,
            static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "JsWindowManager::OnCreateWindow failed."));
        WLOGFE("JsWindowManager::OnCreateWindow windowOption malloc failed");
        return;
    }
    windowOption->SetWindowType(winType);
    sptr<Window> window = Window::Create(windowName, windowOption, context->lock());
    if (window != nullptr) {
        task.Resolve(engine, CreateJsWindowObject(engine, window, false));
        WLOGFI("JsWindowManager::OnCreateWindow success");
    } else {
        task.Reject(engine, CreateJsError(engine,
            static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "JsWindowManager::OnCreateWindow failed."));
    }
}

static void CreateSubWindowTask(std::string parentWinName, std::string windowName, WindowType winType,
    NativeEngine& engine, AsyncTask& task)
{
    WLOGFI("JsWindowManager::CreateSubWindowTask is called");
    sptr<WindowOption> windowOption = new(std::nothrow) WindowOption();
    if (windowOption == nullptr) {
        task.Reject(engine, CreateJsError(engine,
            static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "JsWindowManager::OnCreateWindow failed."));
        WLOGFE("JsWindowManager::OnCreateWindow windowOption malloc failed");
        return;
    }
    windowOption->SetWindowType(winType);
    windowOption->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    windowOption->SetParentName(parentWinName);
    sptr<Window> window = Window::Create(windowName, windowOption);
    if (window != nullptr) {
        task.Resolve(engine, CreateJsWindowObject(engine, window, true));
        WLOGFI("JsWindowManager::OnCreateWindow success");
    } else {
        task.Reject(engine, CreateJsError(engine,
            static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "JsWindowManager::OnCreateWindow failed."));
    }
}

NativeValue* JsWindowManager::OnCreateWindow(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindowManager::OnCreateWindow is called");
    NativeValue* nativeString = nullptr;
    NativeValue* nativeContext = nullptr;
    NativeValue* nativeType = nullptr;
    NativeValue* callback = nullptr;
    if (info.argc >= 2 && info.argv[0]->TypeOf() == NATIVE_STRING) { // 2: minimum params num
        nativeString = info.argv[0];
        nativeType = info.argv[1];
        // 2: minimum params num
        callback = (info.argc == 2) ? nullptr :
            (info.argv[2]->TypeOf() == NATIVE_FUNCTION ? info.argv[2] : nullptr); // 2: index of callback
    } else if (info.argc >= 3) { // 3: minimum params num
        nativeContext = info.argv[0]->TypeOf() == NATIVE_OBJECT ? info.argv[0] : nullptr;
        nativeString = info.argv[1];
        nativeType = info.argv[2]; // 2: index of type
        // 3: minimum params num;
        callback = (info.argc == 3) ? nullptr :
            (info.argv[3]->TypeOf() == NATIVE_FUNCTION ? info.argv[3] : nullptr); // 3: index of callback
    }
    std::string windowName;
    WMError errCode = WMError::WM_OK;
    if (!ConvertFromJsValue(engine, nativeString, windowName)) {
        WLOGFE("Failed to convert parameter to windowName");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    std::string parentName;
    WindowType winType = WindowType::SYSTEM_WINDOW_BASE;
    if (errCode == WMError::WM_OK &&
        !GetWindowTypeAndParentName(engine, parentName, winType, nativeString, nativeType)) {
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    void* contextPtr = nullptr;
    GetNativeContext(nativeContext, contextPtr, errCode);
    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            if (parentName.empty()) {
                return CreateSystemWindowTask(contextPtr, windowName, winType, engine, task);
            } else {
                return CreateSubWindowTask(parentName, windowName, winType, engine, task);
            }
        };
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, callback, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindowManager::OnFindWindow(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindowManager::JsOnFindWindow is called");
    std::string windowName;
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("param not match!");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    } else {
        if (!ConvertFromJsValue(engine, info.argv[0], windowName)) {
            WLOGFE("Failed to convert parameter to windowName");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        }
    }
    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            std::shared_ptr<NativeReference> jsWindowObj = FindJsWindowObject(windowName);
            if (jsWindowObj != nullptr && jsWindowObj->Get() != nullptr) {
                task.Resolve(engine, jsWindowObj->Get());
                WLOGFI("JsWindowManager::OnFindWindow success");
            } else {
                sptr<Window> window = Window::Find(windowName);
                if (window == nullptr) {
                    task.Reject(engine, CreateJsError(engine,
                        static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "JsWindowManager::OnFindWindow failed."));
                } else {
                    AppExecFwk::Ability* ability = nullptr;
                    task.Resolve(engine, CreateJsWindowObject(engine, window, GetAPI7Ability(engine, ability)));
                }
            }
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindowManager::OnMinimizeAll(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindowManager::OnMinimizeAll is called");
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("param is too small!");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    int64_t displayId = static_cast<int64_t>(DISPLAY_ID_INVALID);
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(engine, info.argv[0], displayId)) {
        WLOGFE("Failed to convert parameter to displayId");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    if (displayId < 0 ||
        SingletonContainer::Get<DisplayManager>().GetDisplayById(static_cast<uint64_t>(displayId)) == nullptr) {
        WLOGFE("displayId is invalid");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    } else {
        WLOGFI("displayId %{public}" PRIu64"", static_cast<uint64_t>(displayId));
    }
    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            SingletonContainer::Get<WindowManager>().MinimizeAllAppWindows(static_cast<uint64_t>(displayId));
            task.Resolve(engine, engine.CreateUndefined());
            WLOGFI("JsWindowManager::OnMinimizeAll success");
        };
    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindowManager::OnRegisterWindowMangerCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindowManager::OnRegisterWindowMangerCallback is called");
    if (info.argc != 2) { // 2: params num
        WLOGFE("Params not match");
        return engine.CreateUndefined();
    }
    std::string cbType;
    if (!ConvertFromJsValue(engine, info.argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        return engine.CreateUndefined();
    }
    NativeValue* value = info.argv[1];
    if (!value->IsCallable()) {
        WLOGFI("JsWindowManager::OnRegisterWindowMangerCallback info->argv[1] is not callable");
        return engine.CreateUndefined();
    }

    registerManager_->RegisterListener(nullptr, cbType, CaseType::CASE_WINDOW_MANAGER, engine, value);
    WLOGFI("JsWindowManager::OnRegisterWindowMangerCallback end!");
    return engine.CreateUndefined();
}

NativeValue* JsWindowManager::OnUnregisterWindowManagerCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindowManager::OnUnregisterWindowCallback is called");
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("Params not match");
        return engine.CreateUndefined();
    }
    std::string cbType;
    if (!ConvertFromJsValue(engine, info.argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        return engine.CreateUndefined();
    }
    if (info.argc == 1) {
        registerManager_->UnregisterListener(nullptr, cbType, CaseType::CASE_WINDOW_MANAGER, nullptr);
    } else {
        NativeValue* value = info.argv[1];
        if (!value->IsCallable()) {
            WLOGFI("JsWindowManager::OnUnregisterWindowManagerCallback info->argv[1] is not callable");
            return engine.CreateUndefined();
        }
        registerManager_->UnregisterListener(nullptr, cbType, CaseType::CASE_WINDOW_MANAGER, value);
    }
    WLOGFI("JsWindowManager::OnUnregisterWindowCallback end!");
    return engine.CreateUndefined();
}

static void GetTopWindowTask(void* contextPtr, bool isOldApi, NativeEngine& engine, AsyncTask& task)
{
    std::string windowName;
    sptr<Window> window = nullptr;
    if (isOldApi) {
        AppExecFwk::Ability* ability = nullptr;
        if (!GetAPI7Ability(engine, ability)) {
            task.Reject(engine, CreateJsError(engine,
                static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "JsWindow::onGetTopWindow failed."));
            WLOGE("JsWindowManager get top windowfailed with null ability");
            return;
        }
        window = Window::GetTopWindowWithId(ability->GetWindow()->GetWindowId());
    } else {
        auto context = static_cast<std::weak_ptr<AbilityRuntime::Context>*>(contextPtr);
        if (contextPtr == nullptr || context == nullptr) {
            task.Reject(engine, CreateJsError(engine,
                static_cast<int32_t>(WMError::WM_ERROR_NULLPTR),
                "JsWindow::OnGetTopWindow newAPI failed."));
            WLOGFE("JsWindowManager::OnGetTopWindow in newApi use with empty context!");
            return;
        }
        window = Window::GetTopWindowWithContext(context->lock());
    }
    if (window == nullptr) {
        task.Reject(engine, CreateJsError(engine,
            static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "JsWindowManager::OnGetTopWindow failed."));
        WLOGFE("JsWindowManager::OnGetTopWindow failed");
        return;
    }
    windowName = window->GetWindowName();
    std::shared_ptr<NativeReference> jsWindowObj = FindJsWindowObject(windowName);
    if (jsWindowObj != nullptr && jsWindowObj->Get() != nullptr) {
        task.Resolve(engine, jsWindowObj->Get());
    } else {
        task.Resolve(engine, CreateJsWindowObject(engine, window, isOldApi));
    }
    WLOGFI("JsWindowManager::OnGetTopWindow success");
    return;
}

NativeValue* JsWindowManager::OnGetTopWindow(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindowManager::OnGetTopWindow is called");
    WMError errCode = WMError::WM_OK;
    NativeValue* nativeContext = nullptr;
    NativeValue* nativeCallback = nullptr;
    void* contextPtr = nullptr;
    bool isOldApi = false;
    if (info.argc > 2) { // 2: maximum params num
        WLOGFE("param not match!");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    } else {
        if (info.argc > 0 && info.argv[0]->TypeOf() == NATIVE_OBJECT) { // (context, callback?)
            isOldApi = false;
            nativeContext = info.argv[0];
            nativeCallback = (info.argc == 1) ? nullptr :
                (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
        } else { // (callback?)
            isOldApi = true;
            nativeCallback = (info.argc == 0) ? nullptr :
                (info.argv[0]->TypeOf() == NATIVE_FUNCTION ? info.argv[0] : nullptr);
        }
        GetNativeContext(nativeContext, contextPtr, errCode);
    }

    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            return GetTopWindowTask(contextPtr, isOldApi, engine, task);
        };
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, nativeCallback, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindowManager::OnSetWindowLayoutMode(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindowManager::OnSetWindowLayoutMode is called");
    WMError errCode = WMError::WM_OK;
    if (info.argc < 2 || info.argc > 3) { // 2: minimum params num; 3: maximum params num
        WLOGFE("JsWindowManager::OnSetWindowLayoutMode params too small");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    WindowLayoutMode winLayoutMode = WindowLayoutMode::CASCADE;
    int64_t displayId = 0;
    if (errCode == WMError::WM_OK) {
        NativeNumber* nativeMode = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
        if (nativeMode == nullptr) {
            WLOGFE("Failed to convert parameter to windowLayoutMode");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            winLayoutMode = static_cast<WindowLayoutMode>(static_cast<uint32_t>(*nativeMode));
        }

        if (!ConvertFromJsValue(engine, info.argv[1], displayId)) {
            WLOGFE("Failed to convert parameter to displayId");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        }
    }

    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = SingletonContainer::Get<WindowManager>().SetWindowLayoutMode(winLayoutMode,
                static_cast<uint64_t>(displayId));
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
                WLOGFI("JsWindowManager::OnSetWindowLayoutMode success");
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "do failed"));
            }
        };
    // 2: maximum params num; 2: index of callback
    NativeValue* lastParam = (info.argc <= 2) ? nullptr :
        (info.argv[2]->TypeOf() == NATIVE_FUNCTION ? info.argv[2] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindowManagerInit(NativeEngine* engine, NativeValue* exportObj)
{
    WLOGFI("JsWindowManagerInit is called");

    if (engine == nullptr || exportObj == nullptr) {
        WLOGFE("JsWindowManagerInit engine or exportObj is nullptr");
        return nullptr;
    }

    NativeObject* object = ConvertNativeValueTo<NativeObject>(exportObj);
    if (object == nullptr) {
        WLOGFE("JsWindowManagerInit object is nullptr");
        return nullptr;
    }

    std::unique_ptr<JsWindowManager> jsWinManager = std::make_unique<JsWindowManager>();
    object->SetNativePointer(jsWinManager.release(), JsWindowManager::Finalizer, nullptr);

    BindNativeFunction(*engine, *object, "create", JsWindowManager::CreateWindow);
    BindNativeFunction(*engine, *object, "find", JsWindowManager::FindWindow);
    BindNativeFunction(*engine, *object, "on", JsWindowManager::RegisterWindowManagerCallback);
    BindNativeFunction(*engine, *object, "off", JsWindowManager::UnregisterWindowMangerCallback);
    BindNativeFunction(*engine, *object, "getTopWindow", JsWindowManager::GetTopWindow);
    BindNativeFunction(*engine, *object, "minimizeAll", JsWindowManager::MinimizeAll);
    BindNativeFunction(*engine, *object, "setWindowLayoutMode", JsWindowManager::SetWindowLayoutMode);
    return engine->CreateUndefined();
}
}  // namespace Rosen
}  // namespace OHOS