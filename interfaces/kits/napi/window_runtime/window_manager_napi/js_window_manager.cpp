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
#include "js_runtime_utils.h"
#include "js_window.h"
#include "js_window_listener.h"
#include "js_window_utils.h"
#include "native_engine/native_reference.h"
#include "window_manager.h"
#include "window_manager_hilog.h"
#include "window_option.h"
#include "singleton_container.h"
namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "JsWindowManager"};
}

class JsWindowManager {
public:
    explicit JsWindowManager(NativeEngine* engine) {
    }

    ~JsWindowManager() = default;

    static void Finalizer(NativeEngine* engine, void* data, void* hint)
    {
        WLOGFI("JsWindowManager::Finalizer is called");
        std::unique_ptr<JsWindowManager>(static_cast<JsWindowManager*>(data));
    }

    static NativeValue* CreateWindow(NativeEngine* engine, NativeCallbackInfo* info)
    {
        JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(engine, info);
        return (me != nullptr) ? me->OnCreateWindow(*engine, *info) : nullptr;
    }

    static NativeValue* FindWindow(NativeEngine* engine, NativeCallbackInfo* info)
    {
        JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(engine, info);
        return (me != nullptr) ? me->OnFindWindow(*engine, *info) : nullptr;
    }

    static NativeValue* RegisterWindowManagerCallback(NativeEngine* engine, NativeCallbackInfo* info)
    {
        JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(engine, info);
        return (me != nullptr) ? me->OnRegisterWindowMangerCallback(*engine, *info) : nullptr;
    }

    static NativeValue* UnregisterWindowMangerCallback(NativeEngine* engine, NativeCallbackInfo* info)
    {
        JsWindowManager* me = CheckParamsAndGetThis<JsWindowManager>(engine, info);
        return (me != nullptr) ? me->OnUnregisterWindowManagerCallback(*engine, *info) : nullptr;
    }

private:
    std::weak_ptr<Context> context_;
    std::map<std::string, std::map<std::unique_ptr<NativeReference>, sptr<JsWindowListener>>> jsCbMap_;
    std::mutex mtx_;

    bool GetNativeContext(NativeValue* nativeContext)
    {
        if (nativeContext != nullptr) {
            // Parse info->argv[0] as abilitycontext
            auto objContext = AbilityRuntime::ConvertNativeValueTo<NativeObject>(nativeContext);
            if (objContext == nullptr) {
                return false;
            }
            auto context = static_cast<std::weak_ptr<AbilityRuntime::Context>*>(objContext->GetNativePointer());
            context_ = context->lock();
        }
        return true;
    }

    NativeValue* OnCreateWindow(NativeEngine& engine, NativeCallbackInfo& info)
    {
        WLOGFI("JsWindowManager::JsOnCreateWindow is called");
        if (info.argc <= 0) {
            WLOGFE("parames num not match!");
            return engine.CreateUndefined();
        }
        NativeValue* nativeString = nullptr;
        NativeValue* nativeContext = nullptr;
        NativeValue* nativeType = nullptr;
        NativeValue* callback = nullptr;
        if (info.argv[0]->TypeOf() == NATIVE_STRING) {
            nativeString = info.argv[0];
            nativeType = info.argv[ARGC_ONE];
            callback = (info.argc == ARGC_TWO) ? nullptr : info.argv[INDEX_TWO];
        } else {
            nativeContext = info.argv[0];
            nativeString = info.argv[ARGC_ONE];
            nativeType = info.argv[ARGC_TWO];
            callback = (info.argc == ARGC_THREE) ? nullptr : info.argv[INDEX_THREE];
        }
        std::string windowName;
        if (!ConvertFromJsValue(engine, nativeString, windowName)) {
            WLOGFE("Failed to convert parameter to windowName");
            return engine.CreateUndefined();
        }
        NativeNumber* type = ConvertNativeValueTo<NativeNumber>(nativeType);
        if (type == nullptr) {
            WLOGFE("Failed to convert parameter to windowType");
            return engine.CreateUndefined();
        }
        WindowType winType = static_cast<WindowType>(static_cast<uint32_t>(*type));
        if (!GetNativeContext(nativeContext)) {
            return engine.CreateUndefined();
        }
        AsyncTask::CompleteCallback complete =
            [weak = context_, windowName, winType](NativeEngine& engine, AsyncTask& task, int32_t status) {
                sptr<WindowOption> windowOption = new WindowOption();
                windowOption->SetWindowType(winType);
                sptr<Window> window = Window::Create(windowName, windowOption, weak.lock());
                if (window != nullptr) {
                    task.Resolve(engine, CreateJsWindowObject(engine, window));
                } else {
                    task.Reject(engine, CreateJsError(engine,
                        static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "JsWindowManager::OnCreateWindow failed."));
                }
            };
        NativeValue* result = nullptr;
        AsyncTask::Schedule(
            engine, CreateAsyncTaskWithLastParam(engine, callback, nullptr, std::move(complete), &result));
        return result;
    }

    NativeValue* OnFindWindow(NativeEngine& engine, NativeCallbackInfo& info)
    {
        WLOGFI("JsWindowManager::JsOnFindWindow is called");
        std::string windowName;
        if (!ConvertFromJsValue(engine, info.argv[0], windowName)) {
            WLOGFE("Failed to convert parameter to windowName");
            return engine.CreateUndefined();
        }

        AsyncTask::CompleteCallback complete =
            [windowName](NativeEngine& engine, AsyncTask& task, int32_t status) {
                std::shared_ptr<NativeReference> jsWindowObj = FindJsWindowObject(windowName);
                if (jsWindowObj != nullptr && jsWindowObj->Get() != nullptr) {
                    task.Resolve(engine, jsWindowObj->Get());
                    WLOGFI("JsWindowManager::OnFindWindow success");
                } else {
                    task.Reject(engine, CreateJsError(engine,
                        static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "JsWindowManager::OnFindWindow failed."));
                }
            };

        NativeValue* lastParam = (info.argc == ARGC_ONE) ? nullptr : info.argv[INDEX_ONE];
        if (lastParam == nullptr) {
            WLOGFI("JsWindowManager::OnFindWindow lastParam is nullptr");
        }
        NativeValue* result = nullptr;
        AsyncTask::Schedule(
            engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
        return result;
    }

    bool IfCallbackRegistered(std::string type, NativeValue* jsListenerObject)
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

    void RegisterWmListenerWithType(NativeEngine& engine, std::string type, NativeValue* value)
    {
        // should do type check
        if (IfCallbackRegistered(type, value)) {
            WLOGFE("JsWindowManager::RegisterWmListenerWithType callback already registered!");
            return;
        }
        std::unique_ptr<NativeReference> callbackRef;
        callbackRef.reset(engine.CreateReference(value, 1));
        sptr<JsWindowListener> windowManagerListener = new JsWindowListener(&engine);
        if (type.compare("systemUiTintChange") == 0) {
            sptr<ISystemBarChangedListener> thisListener(windowManagerListener);
            SingletonContainer::Get<WindowManager>().RegisterSystemBarChangedListener(thisListener);
            WLOGFI("JsWindowManager::RegisterWmListenerWithType systemUiTintChange success");
        } else {
            WLOGFE("JsWindowManager::RegisterWmListenerWithType failed method: %{public}s not support!",
                type.c_str());
            return;
        }
        windowManagerListener->AddCallback(value);
        jsCbMap_[type][std::move(callbackRef)] = windowManagerListener;
        return;
    }

    void UnregisterAllWmListenerWithType(std::string type)
    {
        if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
            WLOGFI("JsWindowManager::UnregisterAllWmListenerWithType methodName %{public}s not registerted!",
                type.c_str());
            return;
        }
        for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
            it->second->RemoveAllCallback();
            if (type.compare("systemUiTintChange") == 0) {
                sptr<ISystemBarChangedListener> thisListener(it->second);
                SingletonContainer::Get<WindowManager>().UnregisterSystemBarChangedListener(thisListener);
                WLOGFI("JsWindowManager::UnregisterAllWmListenerWithType systemUiTintChange success");
            }
            jsCbMap_[type].erase(it++);
        }
        jsCbMap_.erase(type);
        return;
    }

    void UnregisterWmListenerWithType(std::string type, NativeValue* value)
    {
        if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
            WLOGFI("JsWindowManager::UnregisterWmListenerWithType methodName %{public}s not registerted!",
                type.c_str());
            return;
        }
        for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
            if (value->StrictEquals(it->first->Get())) {
                it->second->RemoveCallback(value);
                if (type.compare("systemUiTintChange") == 0) {
                    sptr<ISystemBarChangedListener> thisListener(it->second);
                    SingletonContainer::Get<WindowManager>().UnregisterSystemBarChangedListener(thisListener);
                    WLOGFI("JsWindowManager::UnregisterWmListenerWithType systemUiTintChange success");
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

    NativeValue* OnRegisterWindowMangerCallback(NativeEngine& engine, NativeCallbackInfo& info)
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

    NativeValue* OnUnregisterWindowManagerCallback(NativeEngine& engine, NativeCallbackInfo& info)
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
};

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

    std::unique_ptr<JsWindowManager> jsWinManager = std::make_unique<JsWindowManager>(engine);
    object->SetNativePointer(jsWinManager.release(), JsWindowManager::Finalizer, nullptr);

    BindNativeFunction(*engine, *object, "create", JsWindowManager::CreateWindow);
    BindNativeFunction(*engine, *object, "find", JsWindowManager::FindWindow);
    BindNativeFunction(*engine, *object, "on", JsWindowManager::RegisterWindowManagerCallback);
    BindNativeFunction(*engine, *object, "off", JsWindowManager::UnregisterWindowMangerCallback);
    return engine->CreateUndefined();
}
}  // namespace Rosen
}  // namespace OHOS