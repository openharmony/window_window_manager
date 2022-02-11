/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "js_window.h"
#include "js_window_utils.h"
#include "window_helper.h"
#include "window_manager.h"
#include "window_manager_hilog.h"
#include "window_option.h"
#include "singleton_container.h"
namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsWindowManager"};
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
        WLOGE("JsWindowManager::GetAPI7Ability ability is %{public}p!", ability);
    }
    return true;
}

bool JsWindowManager::GetNativeContext(NativeValue* nativeContext)
{
    if (nativeContext != nullptr) {
        // Parse info->argv[0] as abilitycontext
        auto objContext = AbilityRuntime::ConvertNativeValueTo<NativeObject>(nativeContext);
        if (objContext == nullptr) {
            WLOGFE("ConvertNativeValueTo Context Object failed");
            return false;
        }
        auto context = static_cast<std::weak_ptr<AbilityRuntime::Context>*>(objContext->GetNativePointer());
        context_ = context->lock();
    }
    return true;
}

static bool CheckJsWindowNameAndType(NativeEngine& engine, std::string& windowName, WindowType& winType,
    NativeValue* nativeString, NativeValue* nativeType)
{
    if (!ConvertFromJsValue(engine, nativeString, windowName)) {
        WLOGFE("Failed to convert parameter to windowName");
        return false;
    }
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
    if (!WindowHelper::IsSystemWindow(winType)) {
        WLOGFE("Only SystemWindow support create!");
        return false;
    }
    return true;
}

NativeValue* JsWindowManager::OnCreateWindow(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindowManager::OnCreateWindow is called");
    WMError errCode = WMError::WM_OK;
    if (info.argc < ARGC_THREE) {
        WLOGFE("JsWindowManager::OnCreateWindow params less than 3!");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    NativeValue* nativeString = nullptr;
    NativeValue* nativeContext = nullptr;
    NativeValue* nativeType = nullptr;
    NativeValue* callback = nullptr;
    if (errCode == WMError::WM_OK) {
        if (info.argv[0]->TypeOf() != NATIVE_OBJECT) {
            WLOGFE("JsWindowManager::OnCreateWindow first should be context!");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            nativeContext = info.argv[0];
            nativeString = info.argv[ARGC_ONE];
            nativeType = info.argv[ARGC_TWO];
            callback = (info.argc == ARGC_THREE) ? nullptr :
                (info.argv[INDEX_THREE]->TypeOf() == NATIVE_FUNCTION ? info.argv[INDEX_THREE] : nullptr);
        }
    }
    std::string windowName;
    WindowType winType;
    if (!CheckJsWindowNameAndType(engine, windowName, winType, nativeString, nativeType) ||
        !GetNativeContext(nativeContext)) {
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    AsyncTask::CompleteCallback complete =
        [weak = context_, windowName, winType, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            sptr<WindowOption> windowOption = new WindowOption();
            windowOption->SetWindowType(winType);
            sptr<Window> window = Window::Create(windowName, windowOption, weak.lock());
            if (window != nullptr) {
                task.Resolve(engine, CreateJsWindowObject(engine, window));
                WLOGFI("JsWindowManager::OnCreateWindow success");
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
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
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(engine, info.argv[0], windowName)) {
        WLOGFE("Failed to convert parameter to windowName");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
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
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "JsWindowManager::OnFindWindow failed."));
            }
        };

    NativeValue* lastParam = (info.argc <= ARGC_ONE) ? nullptr :
        (info.argv[INDEX_ONE]->TypeOf() == NATIVE_FUNCTION ? info.argv[INDEX_ONE] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindowManager::OnMinimizeAll(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindowManager::OnMinimizeAll is called");
    WMError errCode = WMError::WM_OK;
    if (info.argc < ARGC_ONE) {
        WLOGFE("param is too small!");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    int64_t displayId = 0;
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(engine, info.argv[0], displayId)) {
        WLOGFE("Failed to convert parameter to displayId");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    WLOGFI("displayId %{public}" PRIu64"", static_cast<uint64_t>(displayId));
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
    NativeValue* lastParam = (info.argc <= ARGC_ONE) ? nullptr :
        (info.argv[INDEX_ONE]->TypeOf() == NATIVE_FUNCTION ? info.argv[INDEX_ONE] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

bool JsWindowManager::IfCallbackRegistered(std::string type, NativeValue* jsListenerObject)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGFI("JsWindowManager::IfCallbackRegistered methodName %{public}s not registertd!", type.c_str());
        return false;
    }

    for (auto iter = jsCbMap_[type].begin(); iter != jsCbMap_[type].end(); iter++) {
        if (jsListenerObject->StrictEquals(iter->first->Get())) {
            WLOGFE("JsWindowManager::IfCallbackRegistered callback already registered!");
            return true;
        }
    }
    return false;
}

void JsWindowManager::RegisterWmListenerWithType(NativeEngine& engine, std::string type, NativeValue* value)
{
    // should do type check
    if (IfCallbackRegistered(type, value)) {
        WLOGFE("JsWindowManager::RegisterWmListenerWithType callback already registered!");
        return;
    }
    std::unique_ptr<NativeReference> callbackRef;
    callbackRef.reset(engine.CreateReference(value, 1));

    sptr<JsWindowListener> windowManagerListener = new JsWindowListener(&engine);
    if (type.compare(SYSTEM_BAR_TINT_CHANGE_CB) == 0) {
        sptr<ISystemBarChangedListener> thisListener(windowManagerListener);
        SingletonContainer::Get<WindowManager>().RegisterSystemBarChangedListener(thisListener);
        WLOGFI("JsWindowManager::RegisterWmListenerWithType systemBarTintChange success");
    } else {
        WLOGFE("JsWindowManager::RegisterWmListenerWithType failed method: %{public}s not support!",
            type.c_str());
        return;
    }
    windowManagerListener->AddCallback(value);
    jsCbMap_[type][std::move(callbackRef)] = windowManagerListener;
    return;
}

void JsWindowManager::UnregisterAllWmListenerWithType(std::string type)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGFI("JsWindowManager::UnregisterAllWmListenerWithType methodName %{public}s not registerted!",
            type.c_str());
        return;
    }
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
        it->second->RemoveAllCallback();
        if (type.compare(SYSTEM_BAR_TINT_CHANGE_CB) == 0) {
            sptr<ISystemBarChangedListener> thisListener(it->second);
            SingletonContainer::Get<WindowManager>().UnregisterSystemBarChangedListener(thisListener);
            WLOGFI("JsWindowManager::UnregisterAllWmListenerWithType systemBarTintChange success");
        }
        jsCbMap_[type].erase(it++);
    }
    jsCbMap_.erase(type);
    return;
}

void JsWindowManager::UnregisterWmListenerWithType(std::string type, NativeValue* value)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGFI("JsWindowManager::UnregisterWmListenerWithType methodName %{public}s not registerted!",
            type.c_str());
        return;
    }
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
        if (value->StrictEquals(it->first->Get())) {
            it->second->RemoveCallback(value);
            if (type.compare(SYSTEM_BAR_TINT_CHANGE_CB) == 0) {
                sptr<ISystemBarChangedListener> thisListener(it->second);
                SingletonContainer::Get<WindowManager>().UnregisterSystemBarChangedListener(thisListener);
                WLOGFI("JsWindowManager::UnregisterWmListenerWithType systemBarTintChange success");
            }
            jsCbMap_[type].erase(it++);
            break;
        } else {
            it++;
        }
    }
    // one type with multi jscallback, erase type when there is no callback in one type
    if (jsCbMap_[type].empty()) {
        jsCbMap_.erase(type);
    }
    return;
}

NativeValue* JsWindowManager::OnRegisterWindowMangerCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindowManager::OnRegisterWindowMangerCallback is called");
    if (info.argc != ARGC_TWO) {
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
    std::lock_guard<std::mutex> lock(mtx_);
    RegisterWmListenerWithType(engine, cbType, value);
    return engine.CreateUndefined();
}

NativeValue* JsWindowManager::OnUnregisterWindowManagerCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindowManager::OnUnregisterWindowCallback is called");
    if (info.argc == 0) {
        WLOGFE("Params not match");
        return engine.CreateUndefined();
    }
    std::string cbType;
    if (!ConvertFromJsValue(engine, info.argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        return engine.CreateUndefined();
    }
    std::lock_guard<std::mutex> lock(mtx_);
    if (info.argc == 1) {
        UnregisterAllWmListenerWithType(cbType);
    } else {
        NativeValue* value = info.argv[1];
        if (!value->IsCallable()) {
            WLOGFI("JsWindowManager::OnUnregisterWindowManagerCallback info->argv[1] is not callable");
            return engine.CreateUndefined();
        }
        UnregisterWmListenerWithType(cbType, value);
    }
    return engine.CreateUndefined();
}

static void GetTopWindowTask(void* contextPtr, bool isNewApi, NativeEngine& engine, AsyncTask& task, int32_t status)
{
    std::string windowName;
    sptr<Window> window = nullptr;
    if (!isNewApi) {
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
                "JsWindow::onGetTopWindow newAPI failed."));
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
        task.Resolve(engine, CreateJsWindowObject(engine, window));
    }
    WLOGFI("JsWindowManager::OnGetTopWindow success");
    return;
}

NativeValue* JsWindowManager::OnGetTopWindow(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindowManager::OnGetTopWindow is called");
    NativeValue* nativeContext = nullptr;
    NativeValue* nativeCallback = nullptr;
    bool isNewApi = true;
    if (info.argc > 0 && info.argv[0]->TypeOf() == NATIVE_OBJECT) { // (context, callback?)
        isNewApi = true;
        nativeContext = info.argv[0];
        nativeCallback = (info.argc == ARGC_ONE) ? nullptr :
            (info.argv[INDEX_ONE]->TypeOf() == NATIVE_FUNCTION ? info.argv[INDEX_ONE] : nullptr);
    } else { // (callback?)
        isNewApi = false;
        nativeCallback = (info.argc == 0) ? nullptr :
            (info.argv[0]->TypeOf() == NATIVE_FUNCTION ? info.argv[0] : nullptr);
    }
    void* contextPtr = nullptr;
    WMError errCode = WMError::WM_OK;
    if (nativeContext != nullptr) {
        // Parse info->argv[0] as abilitycontext
        auto objContext = AbilityRuntime::ConvertNativeValueTo<NativeObject>(nativeContext);
        if (objContext == nullptr) {
            WLOGFE("ConvertNativeValueTo Context Object failed");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        }
        contextPtr = objContext->GetNativePointer();
    }
    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            return GetTopWindowTask(contextPtr, isNewApi, engine, task, status);
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
    if (info.argc < ARGC_TWO) {
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
            SingletonContainer::Get<WindowManager>().SetWindowLayoutMode(winLayoutMode,
                static_cast<uint64_t>(displayId));
            task.Resolve(engine, engine.CreateUndefined());
            WLOGFI("JsWindowManager::OnSetWindowLayoutMode success");
        };
    NativeValue* lastParam = (info.argc < ARGC_THREE) ? nullptr :
        (info.argv[INDEX_TWO]->TypeOf() == NATIVE_FUNCTION ? info.argv[INDEX_TWO] : nullptr);
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