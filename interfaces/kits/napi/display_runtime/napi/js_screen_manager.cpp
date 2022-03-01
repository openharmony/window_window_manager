/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "js_screen_manager.h"

#include <vector>
#include <new>
#include <ability.h>
#include "js_runtime_utils.h"
#include "js_screen_listener.h"
#include "native_engine/native_reference.h"
#include "screen_manager.h"
#include "window_manager_hilog.h"
#include "singleton_container.h"
#include "js_screen.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
constexpr int32_t INDEX_ONE = 1;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "JsScreenManager"};
}

class JsScreenManager {
public:
explicit JsScreenManager(NativeEngine* engine) {
}

~JsScreenManager() = default;

static void Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFI("JsScreenManager::Finalizer is called");
    std::unique_ptr<JsScreenManager>(static_cast<JsScreenManager*>(data));
}

static NativeValue* GetAllScreen(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(engine, info);
    return (me != nullptr) ? me->OnGetAllScreen(*engine, *info) : nullptr;
}

static NativeValue* RegisterScreenManagerCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(engine, info);
    return (me != nullptr) ? me->OnRegisterScreenMangerCallback(*engine, *info) : nullptr;
}

static NativeValue* UnregisterScreenMangerCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(engine, info);
    return (me != nullptr) ? me->OnUnregisterScreenManagerCallback(*engine, *info) : nullptr;
}

static NativeValue* MakeMirror(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(engine, info);
    return (me != nullptr) ? me->OnMakeMirror(*engine, *info) : nullptr;
}

static NativeValue* MakeExpand(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(engine, info);
    return (me != nullptr) ? me->OnMakeExpand(*engine, *info) : nullptr;
}

private:
std::map<std::string, std::map<std::unique_ptr<NativeReference>, sptr<JsScreenListener>>> jsCbMap_;
std::mutex mtx_;

NativeValue* OnGetAllScreen(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsScreenManager::OnGetAllScreen is called");
    AsyncTask::CompleteCallback complete =
        [this](NativeEngine& engine, AsyncTask& task, int32_t status) {
            std::vector<sptr<Screen>> screens = SingletonContainer::Get<ScreenManager>().GetAllScreens();
            if (!screens.empty()) {
                task.Resolve(engine, CreateJsScreenVectorObject(engine, screens));
                WLOGFI("JsScreenManager::OnGetAllScreen success");
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(DMError::DM_ERROR_NULLPTR), "JsScreenManager::OnGetAllScreen failed."));
            }
        };
    NativeValue* lastParam = nullptr;
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* CreateJsScreenVectorObject(NativeEngine& engine, std::vector<sptr<Screen>>& screens)
{
    NativeValue* arrayValue = engine.CreateArray(screens.size());
    NativeArray* array = ConvertNativeValueTo<NativeArray>(arrayValue);
    size_t i = 0;
    for (auto& screen : screens) {
        array->SetElement(i++, CreateJsScreenObject(engine, screen));
    }
    return arrayValue;
}

bool IfCallbackRegistered(const std::string& type, NativeValue* jsListenerObject)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGFI("JsScreenManager::IfCallbackRegistered methodName %{public}s not registered!", type.c_str());
        return false;
    }

    for (auto& iter : jsCbMap_[type]) {
        if (jsListenerObject->StrictEquals(iter.first->Get())) {
            WLOGFE("JsScreenManager::IfCallbackRegistered callback already registered!");
            return true;
        }
    }
    return false;
}

void RegisterScreenListenerWithType(NativeEngine& engine, const std::string& type, NativeValue* value)
{
    if (IfCallbackRegistered(type, value)) {
        WLOGFE("JsScreenManager::RegisterScreenListenerWithType callback already registered!");
        return;
    }
    std::unique_ptr<NativeReference> callbackRef;
    callbackRef.reset(engine.CreateReference(value, 1));
    sptr<JsScreenListener> screenListener = new(std::nothrow) JsScreenListener(&engine);
    if (screenListener == nullptr) {
        WLOGFE("screenListener is nullptr");
        return;
    }
    if (type == "connect" || type == "disconnect" || type == "change") {
        SingletonContainer::Get<ScreenManager>().RegisterScreenListener(screenListener);
        WLOGFI("JsScreenManager::RegisterScreenListenerWithType success");
    } else {
        WLOGFE("JsScreenManager::RegisterScreenListenerWithType failed method: %{public}s not support!",
            type.c_str());
        return;
    }
    screenListener->AddCallback(value);
    jsCbMap_[type][std::move(callbackRef)] = screenListener;
}

void UnregisterAllScreenListenerWithType(const std::string& type)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGFI("JsScreenManager::UnregisterAllScreenListenerWithType methodName %{public}s not registered!",
            type.c_str());
        return;
    }
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
        it->second->RemoveAllCallback();
        if (type == "screenConnectEvent" || type == "screenChangeEvent") {
            sptr<ScreenManager::IScreenListener> thisListener(it->second);
            SingletonContainer::Get<ScreenManager>().UnregisterScreenListener(thisListener);
            WLOGFI("JsScreenManager::UnregisterAllScreenListenerWithType success");
        }
        jsCbMap_[type].erase(it++);
    }
    jsCbMap_.erase(type);
}

void UnRegisterScreenListenerWithType(const std::string& type, NativeValue* value)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGFI("JsScreenManager::UnRegisterScreenListenerWithType methodName %{public}s not registered!",
            type.c_str());
        return;
    }
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
        if (value->StrictEquals(it->first->Get())) {
            it->second->RemoveCallback(value);
            if (type == "screenConnectEvent" || type == "screenChangeEvent") {
                sptr<ScreenManager::IScreenListener> thisListener(it->second);
                SingletonContainer::Get<ScreenManager>().UnregisterScreenListener(thisListener);
                WLOGFI("JsScreenManager::UnRegisterScreenListenerWithType success");
            }
            jsCbMap_[type].erase(it++);
            break;
        } else {
            it++;
        }
    }
    if (jsCbMap_[type].empty()) {
        jsCbMap_.erase(type);
    }
}

NativeValue* OnRegisterScreenMangerCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsScreenManager::OnRegisterScreenMangerCallback is called");
    if (info.argc != ARGC_TWO) {
        WLOGFE("Params not match");
        return engine.CreateUndefined();
    }
    std::string cbType;
    if (!ConvertFromJsValue(engine, info.argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        return engine.CreateUndefined();
    }
    NativeValue* value = info.argv[INDEX_ONE];
    if (value == nullptr) {
        WLOGFI("JsScreenManager::OnRegisterScreenMangerCallback info->argv[1] is nullptr");
        return engine.CreateUndefined();
    }
    if (!value->IsCallable()) {
        WLOGFI("JsScreenManager::OnRegisterScreenMangerCallback info->argv[1] is not callable");
        return engine.CreateUndefined();
    }
    std::lock_guard<std::mutex> lock(mtx_);
    RegisterScreenListenerWithType(engine, cbType, value);
    return engine.CreateUndefined();
}

NativeValue* OnUnregisterScreenManagerCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsScreenManager::OnUnregisterScreenCallback is called");
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
    if (info.argc == ARGC_ONE) {
        UnregisterAllScreenListenerWithType(cbType);
    } else {
        NativeValue* value = info.argv[INDEX_ONE];
        if (value == nullptr) {
            WLOGFI("JsScreenManager::OnUnregisterScreenManagerCallback info->argv[1] is nullptr");
            return engine.CreateUndefined();
        }
        if (!value->IsCallable()) {
            WLOGFI("JsScreenManager::OnUnregisterScreenManagerCallback info->argv[1] is not callable");
            return engine.CreateUndefined();
        }
        UnRegisterScreenListenerWithType(cbType, value);
    }
    return engine.CreateUndefined();
}

NativeValue* OnMakeMirror(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsScreenManager::OnMakeMirror is called");
    if (info.argc != ARGC_TWO) {
        WLOGFE("Params not match");
        return engine.CreateUndefined();
    }

    int64_t mainScreenId;
    if (!ConvertFromJsValue(engine, info.argv[0], mainScreenId)) {
        WLOGFE("Failed to convert parameter to callbackType");
        return engine.CreateUndefined();
    }
    NativeArray* array = ConvertNativeValueTo<NativeArray>(info.argv[INDEX_ONE]);
    if (array == nullptr) {
        WLOGFE("Failed to get screenids");
        return engine.CreateUndefined();
    }
    uint32_t size = array->GetLength();
    std::vector<ScreenId> screenIds;
    for (uint32_t i = 0; i < size; i++) {
        uint32_t screenId;
        NativeValue* value = array->GetElement(i);
        if (!ConvertFromJsValue(engine, value, screenId)) {
            WLOGFE("Failed to convert parameter to callbackType");
            return engine.CreateUndefined();
        }
        screenIds.emplace_back(static_cast<ScreenId>(screenId));
    }

    AsyncTask::CompleteCallback complete =
        [mainScreenId, screenIds](NativeEngine& engine, AsyncTask& task, int32_t status) {
            ScreenId id = SingletonContainer::Get<ScreenManager>().MakeMirror(mainScreenId, screenIds);
            if (id != SCREEN_ID_INVALID) {
                task.Resolve(engine, CreateJsValue(engine, static_cast<uint32_t>(id)));
                WLOGFI("JsScreenManager::MakeMirror success");
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(DMError::DM_ERROR_NULLPTR), "JsScreenManager::OnMakeMirror failed."));
                WLOGFE("JsScreenManager::MakeMirror failed");
            }
        };
    NativeValue* lastParam = nullptr;
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* OnMakeExpand(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsScreenManager::OnMakeExpand is called");
    if (info.argc != ARGC_ONE) {
        WLOGFE("Params not match");
        return engine.CreateUndefined();
    }

    NativeArray* array = ConvertNativeValueTo<NativeArray>(info.argv[0]);
    if (array == nullptr) {
        WLOGFE("Failed to get options");
        return engine.CreateUndefined();
    }
    uint32_t size = array->GetLength();
    std::vector<ExpandOption> options;
    for (uint32_t i = 0; i < size; ++i) {
        NativeObject* object = ConvertNativeValueTo<NativeObject>(array->GetElement(i));
        ExpandOption expandOption;
        int32_t res = GetExpandOptionFromJs(engine, object, expandOption);
        if (res == -1) {
            WLOGE("expandoption param %{public}d error!", i);
            return engine.CreateUndefined();
        }
        options.emplace_back(expandOption);
    }

    AsyncTask::CompleteCallback complete =
        [options](NativeEngine& engine, AsyncTask& task, int32_t status) {
            ScreenId id = SingletonContainer::Get<ScreenManager>().MakeExpand(options);
            if (id != SCREEN_ID_INVALID) {
                task.Resolve(engine, CreateJsValue(engine, static_cast<uint32_t>(id)));
                WLOGFI("JsScreenManager::MakeExpand success");
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(DMError::DM_ERROR_NULLPTR), "JsScreenManager::OnMakeExpand failed."));
                WLOGFE("JsScreenManager::MakeExpand failed");
            }
        };
    NativeValue* lastParam = nullptr;
    NativeValue* result = nullptr;
    AsyncTask::Schedule(
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

int32_t GetExpandOptionFromJs(NativeEngine& engine, NativeObject* optionObject, ExpandOption& option)
{
    NativeValue* screedIdValue = optionObject->GetProperty("screenId");
    NativeValue* startXValue = optionObject->GetProperty("startX");
    NativeValue* startYValue = optionObject->GetProperty("startY");
    uint32_t screenId;
    uint32_t startX;
    uint32_t startY;
    if (!ConvertFromJsValue(engine, screedIdValue, screenId)) {
        WLOGFE("Failed to convert parameter to callbackType");
        return -1;
    }
    if (!ConvertFromJsValue(engine, startXValue, startX)) {
        WLOGFE("Failed to convert parameter to callbackType");
        return -1;
    }
    if (!ConvertFromJsValue(engine, startYValue, startY)) {
        WLOGFE("Failed to convert parameter to callbackType");
        return -1;
    }
    option = {screenId, startX, startY};
    return 0;
}
};

NativeValue* JsScreenManagerInit(NativeEngine* engine, NativeValue* exportObj)
{
    WLOGFI("JsScreenManagerInit is called");

    if (engine == nullptr || exportObj == nullptr) {
        WLOGFE("JsScreenManagerInit engine or exportObj is nullptr");
        return nullptr;
    }

    NativeObject* object = ConvertNativeValueTo<NativeObject>(exportObj);
    if (object == nullptr) {
        WLOGFE("JsScreenManagerInit object is nullptr");
        return nullptr;
    }

    std::unique_ptr<JsScreenManager> jsScreenManager = std::make_unique<JsScreenManager>(engine);
    object->SetNativePointer(jsScreenManager.release(), JsScreenManager::Finalizer, nullptr);

    BindNativeFunction(*engine, *object, "getAllScreen", JsScreenManager::GetAllScreen);
    BindNativeFunction(*engine, *object, "on", JsScreenManager::RegisterScreenManagerCallback);
    BindNativeFunction(*engine, *object, "off", JsScreenManager::UnregisterScreenMangerCallback);
    BindNativeFunction(*engine, *object, "makeMirror", JsScreenManager::MakeMirror);
    BindNativeFunction(*engine, *object, "makeExpand", JsScreenManager::MakeExpand);
    return engine->CreateUndefined();
}
}  // namespace Rosen
}  // namespace OHOS