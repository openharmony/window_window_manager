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
#include "js_window_utils.h"
#include "native_engine/native_reference.h"
#include "window_manager_hilog.h"
#include "window_option.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "JsWindowManager"};
}

class JsWindowManager {
public:
    JsWindowManager() = default;
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

private:
    std::weak_ptr<Context> context_;
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
        WLOGFI("JsOnCreateWindow is called");
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
        std::string windowName;
        if (!ConvertFromJsValue(engine, info.argv[0], windowName)) {
            WLOGFE("Failed to convert parameter to windowName");
            return engine.CreateUndefined();
        }

        AsyncTask::CompleteCallback complete =
            [windowName](NativeEngine& engine, AsyncTask& task, int32_t status) {
                sptr<Window> window = Window::Find(windowName);
                if (window != nullptr) {
                    task.Resolve(engine, CreateJsWindowObject(engine, window));
                    WLOGFI("JsWindowManager::OnFindWindow success");
                } else {
                    task.Reject(engine, CreateJsError(engine,
                        static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "JsWindow::OnFindWindow failed."));
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

    std::unique_ptr<JsWindowManager> jsWinManager = std::make_unique<JsWindowManager>();
    object->SetNativePointer(jsWinManager.release(), JsWindowManager::Finalizer, nullptr);

    BindNativeFunction(*engine, *object, "create", JsWindowManager::CreateWindow);
    BindNativeFunction(*engine, *object, "find", JsWindowManager::FindWindow);

    return engine->CreateUndefined();
}
}  // namespace Rosen
}  // namespace OHOS