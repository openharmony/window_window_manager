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

#include "js_window_stage.h"
#include <string>
#include "js_runtime_utils.h"
#include "js_window.h"
#include "js_window_utils.h"
#include "window_manager_hilog.h"
namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
const int CONTENT_STORAGE_ARG = 2;
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsWindowStage"};
} // namespace

void JsWindowStage::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFI("JsWindowStage::Finalizer is called");
    std::unique_ptr<JsWindowStage>(static_cast<JsWindowStage*>(data));
}

NativeValue* JsWindowStage::SetUIContent(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("JsWindowStage::SetUIContent is called");
    JsWindowStage* me = CheckParamsAndGetThis<JsWindowStage>(engine, info);
    return (me != nullptr) ? me->OnSetUIContent(*engine, *info) : nullptr;
}

NativeValue* JsWindowStage::GetMainWindow(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("JsWindowStage::GetMainWindow is called");
    JsWindowStage* me = CheckParamsAndGetThis<JsWindowStage>(engine, info);
    return (me != nullptr) ? me->OnGetMainWindow(*engine, *info) : nullptr;
}

NativeValue* JsWindowStage::On(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("JsWindowStage::On is called");
    JsWindowStage* me = CheckParamsAndGetThis<JsWindowStage>(engine, info);
    return (me != nullptr) ? me->OnEvent(*engine, *info) : nullptr;
}

NativeValue* JsWindowStage::Off(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("JsWindowStage::Off is called");
    JsWindowStage* me = CheckParamsAndGetThis<JsWindowStage>(engine, info);
    return (me != nullptr) ? me->OffEvent(*engine, *info) : nullptr;
}

NativeValue* JsWindowStage::LoadContent(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("JsWindowStage::LoadContent is called");
    JsWindowStage* me = CheckParamsAndGetThis<JsWindowStage>(engine, info);
    return (me != nullptr) ? me->OnLoadContent(*engine, *info) : nullptr;
}

NativeValue* JsWindowStage::GetWindowMode(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("JsWindowStage::GetWindowMode is called");
    JsWindowStage* me = CheckParamsAndGetThis<JsWindowStage>(engine, info);
    return (me != nullptr) ? me->OnGetWindowMode(*engine, *info) : nullptr;
}

NativeValue* JsWindowStage::CreateSubWindow(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("JsWindowStage::CreateSubWindow is called");
    JsWindowStage* me = CheckParamsAndGetThis<JsWindowStage>(engine, info);
    return (me != nullptr) ? me->OnCreateSubWindow(*engine, *info) : nullptr;
}

NativeValue* JsWindowStage::GetSubWindow(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("JsWindowStage::GetSubWindow is called");
    JsWindowStage* me = CheckParamsAndGetThis<JsWindowStage>(engine, info);
    return (me != nullptr) ? me->OnGetSubWindow(*engine, *info) : nullptr;
}

void JsWindowStage::AfterForeground()
{
    LifeCycleCallBack(WindowStageEventType::FOREGROUND);
}

void JsWindowStage::AfterBackground()
{
    LifeCycleCallBack(WindowStageEventType::BACKGROUND);
}

void JsWindowStage::AfterFocused()
{
    LifeCycleCallBack(WindowStageEventType::ACTIVE);
}

void JsWindowStage::AfterUnfocused()
{
    LifeCycleCallBack(WindowStageEventType::INACTIVE);
}

void JsWindowStage::LifeCycleCallBack(WindowStageEventType type)
{
    WLOGFI("JsWindowStage::LifeCycleCallBack is called, type: %{public}d", type);
    std::unique_ptr<AsyncTask::CompleteCallback> complete = std::make_unique<AsyncTask::CompleteCallback>(
        [=] (NativeEngine &engine, AsyncTask &task, int32_t status) {
            NativeValue* argv[] = {CreateJsValue(*engine_, static_cast<uint32_t>(type))};
            for (auto &iter : eventCallbackMap_) {
                NativeValue* method = (iter.first)->Get();
                if (method == nullptr) {
                    WLOGFE("callback is null");
                    return;
                }
                engine_->CallFunction(engine_->CreateUndefined(), method, argv, ArraySize(argv));
            }
        }
    );
    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule(*engine_, std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

NativeValue* JsWindowStage::OnSetUIContent(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindowStage::OnSetUIContent is called");
    if (info.argc < 2) { // 2: minimum param nums
        WLOGFE("JsWindowStage::OnSetUIContent Not enough params");
        return engine.CreateUndefined();
    }
    if (windowScene_ == nullptr || windowScene_->GetMainWindow() == nullptr) {
        WLOGFE("JsWindowStage::OnSetUIContent windowScene_ or MainWindow is nullptr");
        return engine.CreateUndefined();
    }

    // Parse info->argv[0] as abilitycontext
    auto objContext = ConvertNativeValueTo<NativeObject>(info.argv[0]);
    if (objContext == nullptr) {
        WLOGFE("JsWindowStage::OnSetUIContent info->argv[0] InValid");
        return engine.CreateUndefined();
    }

    // Parse info->argv[1] as url
    std::string contextUrl;
    if (!ConvertFromJsValue(engine, info.argv[1], contextUrl)) {
        WLOGFE("JsWindowStage::OnSetUIContent failed to convert parameter to url");
        return engine.CreateUndefined();
    }
    WLOGFI("JsWindowStage::OnSetUIContent Get url: %{public}s", contextUrl.c_str());

    windowScene_->GetMainWindow()->SetUIContent(contextUrl, &engine, info.argv[CONTENT_STORAGE_ARG]);

    return engine.CreateUndefined();
}

NativeValue* JsWindowStage::OnGetMainWindow(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindowStage::OnGetMainWindow is called");
    WMError errCode = WMError::WM_OK;
    if (windowScene_ == nullptr) {
        WLOGFE("JsWindowStage::OnGetMainWindow windowScene_ is nullptr");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            auto window = windowScene_->GetMainWindow();
            if (window != nullptr) {
                task.Resolve(engine, OHOS::Rosen::CreateJsWindowObject(engine, window));
                WLOGFI("JsWindowStage::OnGetMainWindow success");
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(Rosen::WMError::WM_ERROR_NULLPTR),
                    "JsWindowStage::OnGetMainWindow failed."));
            }
        };
    NativeValue* callback = info.argv[0]->TypeOf() == NATIVE_FUNCTION ? info.argv[0] : nullptr;
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, callback, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindowStage::OnEvent(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindowStage::OnEvent is called");
    if (windowScene_ == nullptr) {
        WLOGFE("JsWindowStage::OnEvent windowScene_ is nullptr");
        return engine.CreateUndefined();
    }

    if (info.argc < 2) { // 2: minimum param nums
        WLOGFE("JsWindowStage::OnEvent wrong input params");
        return engine.CreateUndefined();
    }

    // Parse info->argv[0] as string
    std::string eventString;
    if (!ConvertFromJsValue(engine, info.argv[0], eventString)) {
        WLOGFE("JsWindowStage::OnEvent info->argv[0] Failed to convert parameter to string");
        return engine.CreateUndefined();
    }
    if (eventString.compare("windowStageEvent") != 0) {
        WLOGFE("JsWindowStage::OnEvent info->argv[0] is %{public}s, InValid",
            eventString.c_str());
        return engine.CreateUndefined();
    }

    NativeValue* value = info.argv[1];
    if (!value->IsCallable()) {
        WLOGFE("JsWindowStage::OnEvent info->argv[1] is not callable");
        return engine.CreateUndefined();
    }

    std::shared_ptr<NativeReference> refence = nullptr;
    refence.reset(engine.CreateReference(value, 1));
    eventCallbackMap_[refence] = 1;
    engine_ = &engine;
    WLOGFI("JsWindowStage::OnEvent eventCallbackMap_ size: %{public}d",
        static_cast<uint32_t>(eventCallbackMap_.size()));
    if (!regLifeCycleListenerFlag_) {
        auto window = windowScene_->GetMainWindow();
        if (window != nullptr) {
            sptr<IWindowLifeCycle> listener = this;
            window->RegisterLifeCycleListener(listener);
            regLifeCycleListenerFlag_ = true;
            WLOGFI("JsWindowStage::OnEvent regist lifecycle success");
        }
    } else {
        WLOGFI("JsWindowStage::OnEvent already regist lifecycle");
    }
    return engine.CreateUndefined();
}

NativeValue* JsWindowStage::OffEvent(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindowStage::OffEvent is called");
    if (windowScene_ == nullptr) {
        WLOGFE("JsWindowStage::OffEvent windowScene_ is nullptr");
        return engine.CreateUndefined();
    }
    if (info.argc < 1 || info.argc > 2) { // 1: minimum param nums, 2: maximum param nums
        WLOGFE("JsWindowStage::OffEvent wrong input params");
        return engine.CreateUndefined();
    }

    // Parse info->argv[0] as string
    std::string eventString;
    if (!ConvertFromJsValue(engine, info.argv[0], eventString)) {
        WLOGFE("JsWindowStage::OffEvent info->argv[0] Failed to convert parameter to string");
        return engine.CreateUndefined();
    }
    if (eventString.compare("windowStageEvent") != 0) {
        WLOGFE("JsWindowStage::OffEvent info->argv[0] is InValid");
        return engine.CreateUndefined();
    }

    NativeValue* value = info.argv[1];
    if (value->IsCallable()) {
        WLOGFI("JsWindowStage::OffEvent info->argv[1] is callable type");
        for (auto iter = eventCallbackMap_.begin(); iter != eventCallbackMap_.end(); iter++) {
            std::shared_ptr<NativeReference> callback = iter->first;
            if (value->StrictEquals(callback->Get())) {
                eventCallbackMap_.erase(iter);
                break;
            }
        }
        return engine.CreateUndefined();
    } else if (value->TypeOf() == NativeValueType::NATIVE_UNDEFINED) {
        WLOGFI("JsWindowStage::OffEvent info->argv[1] is native undefined type");
        eventCallbackMap_.clear();
    } else {
        WLOGFE("JsWindowStage::OffEvent info->argv[1] is InValid param");
    }
    return engine.CreateUndefined();
}

NativeValue* JsWindowStage::OnLoadContent(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindowStage::OnLoadContent is called");
    WMError errCode = WMError::WM_OK;
    if (info.argc <= 0 || windowScene_ == nullptr) {
        WLOGFE("JsWindowStage param not match or windowScene_ is nullptr");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    std::string contextUrl;
    if (!ConvertFromJsValue(engine, info.argv[0], contextUrl)) {
        WLOGFE("Failed to convert parameter to context url");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    NativeValue* storage = nullptr;
    NativeValue* callBack = nullptr;
    NativeValue* value1 = info.argv[INDEX_ONE];
    NativeValue* value2 = info.argv[INDEX_TWO];
    if (value1->TypeOf() == NATIVE_FUNCTION) {
        callBack = value1;
    } else if (value1->TypeOf() == NATIVE_OBJECT) {
        storage = value1;
    }
    if (value2->TypeOf() == NATIVE_FUNCTION) {
        callBack = value2;
    }
    std::shared_ptr<NativeReference> contentStorage = (storage == nullptr) ? nullptr :
        std::shared_ptr<NativeReference>(engine.CreateReference(storage, 1));
    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            auto win = windowScene_->GetMainWindow();
            if (win == nullptr) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                return;
            }
            NativeValue* nativeStorage = (contentStorage == nullptr) ? nullptr : contentStorage->Get();
            Rosen::WMError ret = win->SetUIContent(contextUrl, &engine, nativeStorage, false);
            if (ret != Rosen::WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret)));
                return;
            }
            task.Resolve(engine, engine.CreateUndefined());
            WLOGFI("JsWindowStage::OnLoadContent success");
        };
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, callBack, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindowStage::OnGetWindowMode(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindowStage::OnGetWindowMode is called");
    AsyncTask::CompleteCallback complete =
        [this](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto window = windowScene_->GetMainWindow();
            if (window == nullptr) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(Rosen::WMError::WM_ERROR_NULLPTR),
                    "JsWindowStage::OnGetWindowMode failed."));
                WLOGFE("JsWindowStage window is nullptr");
                return;
            }
            Rosen::WindowMode mode = window->GetMode();
            if (NATIVE_TO_JS_WINDOW_MODE_MAP.count(mode) != 0) {
                task.Resolve(engine, CreateJsValue(engine, NATIVE_TO_JS_WINDOW_MODE_MAP.at(mode)));
                WLOGFI("JsWindowStage OnGetWindowMode success");
            } else {
                task.Resolve(engine, CreateJsValue(engine, mode));
                WLOGFI("JsWindowStage OnGetWindowMode success, but not in apimode");
            }
        };
    NativeValue* callback = info.argv[0]->TypeOf() == NATIVE_FUNCTION ? info.argv[0] : nullptr;
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, callback, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindowStage::OnCreateSubWindow(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindowStage::OnCreateSubWindow is called");
    WMError errCode = WMError::WM_OK;
    if (info.argc < ARGC_ONE) {
        WLOGFE("JsWindowStage::OnCreateSubWindow params less than 1!");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    std::string windowName;
    if (!ConvertFromJsValue(engine, info.argv[0], windowName)) {
        WLOGFE("Failed to convert parameter to windowName");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            sptr<Rosen::WindowOption> windowOption = new Rosen::WindowOption();
            windowOption->SetWindowType(Rosen::WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
            windowOption->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
            auto window = windowScene_->CreateWindow(windowName, windowOption);
            if (window == nullptr) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(Rosen::WMError::WM_ERROR_NULLPTR),
                    "JsWindowStage::OnCreateSubWindow failed."));
                WLOGFE("JsWindowStage window is nullptr");
                return;
            }
            task.Resolve(engine, CreateJsWindowObject(engine, window));
            WLOGFI("JsWindowStage OnCreateSubWindow success");
        };
    NativeValue* callback = info.argv[INDEX_ONE]->TypeOf() == NATIVE_FUNCTION ? info.argv[INDEX_ONE] : nullptr;
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, callback, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindowStage::CreateJsSubWindowArrayObject(NativeEngine& engine,
    std::vector<sptr<Window>> subWinVec)
{
    WLOGFI("JsWindowUtils::CreateJsSubWindowArrayObject is called");
    NativeValue* objValue = engine.CreateArray(subWinVec.size());
    NativeArray* array = ConvertNativeValueTo<NativeArray>(objValue);
    if (array == nullptr) {
        WLOGFE("Failed to convert subWinVec to jsArrayObject");
        return nullptr;
    }
    uint32_t index = 0;
    for (size_t i = 0; i < subWinVec.size(); i++) {
        array->SetElement(index++, CreateJsWindowObject(engine, subWinVec[i]));
    }
    return objValue;
}

NativeValue* JsWindowStage::OnGetSubWindow(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsWindowStage::OnGetSubWindow is called");
    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            std::vector<sptr<Window>> subWindowVec = windowScene_->GetSubWindow();
            if (subWindowVec.empty()) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(WMError::WM_ERROR_INVALID_OPERATION),
                    "JsWindowStage::OnGetSubWindow failed."));
                WLOGFE("JsWindowStage subWindow is empty");
                return;
            }
            task.Resolve(engine, CreateJsSubWindowArrayObject(engine, subWindowVec));
            WLOGFI("JsWindowStage OnGetSubWindow success");
        };
    NativeValue* callback = info.argv[0]->TypeOf() == NATIVE_FUNCTION ? info.argv[0] : nullptr;
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, callback, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* CreateJsWindowStage(NativeEngine& engine,
    std::shared_ptr<Rosen::WindowScene> windowScene)
{
    WLOGFI("JsWindowStage::CreateJsWindowStage is called");
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);

    std::unique_ptr<JsWindowStage> jsWindowStage = std::make_unique<JsWindowStage>(windowScene);
    object->SetNativePointer(jsWindowStage.release(), JsWindowStage::Finalizer, nullptr);

    BindNativeFunction(engine,
        *object, "setUIContent", JsWindowStage::SetUIContent);
    BindNativeFunction(engine,
        *object, "loadContent", JsWindowStage::LoadContent);
    BindNativeFunction(engine,
        *object, "getMainWindow", JsWindowStage::GetMainWindow);
    BindNativeFunction(engine,
        *object, "getWindowMode", JsWindowStage::GetWindowMode);
    BindNativeFunction(engine,
        *object, "createSubWindow", JsWindowStage::CreateSubWindow);
    BindNativeFunction(engine,
        *object, "getSubWindow", JsWindowStage::GetSubWindow);
    BindNativeFunction(engine, *object, "on", JsWindowStage::On);
    BindNativeFunction(engine, *object, "off", JsWindowStage::Off);

    return objValue;
}
}  // namespace Rosen
}  // namespace OHOS
