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

#include <vector>
#include <new>

#include <hitrace_meter.h>
#include "js_runtime_utils.h"
#include "native_engine/native_reference.h"
#include "display_manager.h"
#include "window_manager_hilog.h"
#include "singleton_container.h"
#include "js_display_listener.h"
#include "js_display.h"
#include "js_display_manager.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
constexpr int32_t INDEX_ONE = 1;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "JsDisplayManager"};
}

class JsDisplayManager {
public:
explicit JsDisplayManager(NativeEngine* engine) {
}

~JsDisplayManager() = default;

static void Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGI("Finalizer is called");
    std::unique_ptr<JsDisplayManager>(static_cast<JsDisplayManager*>(data));
}

static NativeValue* GetDefaultDisplay(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(engine, info);
    return (me != nullptr) ? me->OnGetDefaultDisplay(*engine, *info) : nullptr;
}

static NativeValue* GetDefaultDisplaySync(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(engine, info);
    return (me != nullptr) ? me->OnGetDefaultDisplaySync(*engine, *info) : nullptr;
}

static NativeValue* GetAllDisplay(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(engine, info);
    return (me != nullptr) ? me->OnGetAllDisplay(*engine, *info) : nullptr;
}

static NativeValue* GetAllDisplays(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(engine, info);
    return (me != nullptr) ? me->OnGetAllDisplays(*engine, *info) : nullptr;
}

static NativeValue* RegisterDisplayManagerCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(engine, info);
    return (me != nullptr) ? me->OnRegisterDisplayManagerCallback(*engine, *info) : nullptr;
}

static NativeValue* UnregisterDisplayManagerCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(engine, info);
    return (me != nullptr) ? me->OnUnregisterDisplayManagerCallback(*engine, *info) : nullptr;
}

static NativeValue* HasPrivateWindow(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(engine, info);
    return (me != nullptr) ? me->OnHasPrivateWindow(*engine, *info) : nullptr;
}

static NativeValue* IsFoldable(NativeEngine* engine, NativeCallbackInfo* info)
{
    auto* me = CheckParamsAndGetThis<JsDisplayManager>(engine, info);
    return (me != nullptr) ? me->OnIsFoldable(*engine, *info) : nullptr;
}

static NativeValue* GetFoldStatus(NativeEngine* engine, NativeCallbackInfo* info)
{
    auto* me = CheckParamsAndGetThis<JsDisplayManager>(engine, info);
    return (me != nullptr) ? me->OnGetFoldStatus(*engine, *info) : nullptr;
}

static NativeValue* GetFoldDisplayMode(NativeEngine* engine, NativeCallbackInfo* info)
{
    auto* me = CheckParamsAndGetThis<JsDisplayManager>(engine, info);
    return (me != nullptr) ? me->OnGetFoldDisplayMode(*engine, *info) : nullptr;
}

static NativeValue* SetFoldDisplayMode(NativeEngine* engine, NativeCallbackInfo* info)
{
    auto* me = CheckParamsAndGetThis<JsDisplayManager>(engine, info);
    return (me != nullptr) ? me->OnSetFoldDisplayMode(*engine, *info) : nullptr;
}

static NativeValue* GetCurrentFoldCreaseRegion(NativeEngine* engine, NativeCallbackInfo* info)
{
    auto* me = CheckParamsAndGetThis<JsDisplayManager>(engine, info);
    return (me != nullptr) ? me->OnGetCurrentFoldCreaseRegion(*engine, *info) : nullptr;
}

private:
std::map<std::string, std::map<std::unique_ptr<NativeReference>, sptr<JsDisplayListener>>> jsCbMap_;
std::mutex mtx_;

NativeValue* OnGetDefaultDisplay(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGI("GetDefaultDisplay called");
    DMError errCode = DMError::DM_OK;
    if (info.argc != 0 && info.argc != ARGC_ONE) {
        WLOGFE("OnGetDefaultDisplay params not match");
        errCode = DMError::DM_ERROR_INVALID_PARAM;
    }

    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != DMError::DM_OK) {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(errCode), "JsDisplayManager::OnGetDefaultDisplay failed."));
            }
            HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "Async:GetDefaultDisplay");
            sptr<Display> display = SingletonContainer::Get<DisplayManager>().GetDefaultDisplay();
            if (display != nullptr) {
                task.Resolve(engine, CreateJsDisplayObject(engine, display));
                WLOGI("OnGetDefaultDisplay success");
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(DMError::DM_ERROR_NULLPTR), "JsDisplayManager::OnGetDefaultDisplay failed."));
            }
        };
    NativeValue* lastParam = nullptr;
    if (info.argc == ARGC_ONE && info.argv[0]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[0];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsDisplayManager::OnGetDefaultDisplay",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* OnGetDefaultDisplaySync(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGI("GetDefaultDisplaySync called");
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "Sync:GetDefaultDisplay");
    sptr<Display> display = SingletonContainer::Get<DisplayManager>().GetDefaultDisplaySync();
    if (display == nullptr) {
        WLOGFE("OnGetDefaultDisplaySync, display is nullptr.");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN)));
        return engine.CreateUndefined();
    }
    return CreateJsDisplayObject(engine, display);
}

NativeValue* OnGetAllDisplay(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGI("GetAllDisplay called");
    DMError errCode = DMError::DM_OK;
    if (info.argc != 0 && info.argc != ARGC_ONE) {
        WLOGFE("OnGetAllDisplay params not match");
        errCode = DMError::DM_ERROR_INVALID_PARAM;
    }

    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != DMError::DM_OK) {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(errCode), "JsDisplayManager::OnGetAllDisplay failed."));
            }
            std::vector<sptr<Display>> displays = SingletonContainer::Get<DisplayManager>().GetAllDisplays();
            if (!displays.empty()) {
                task.Resolve(engine, CreateJsDisplayArrayObject(engine, displays));
                WLOGI("GetAllDisplays success");
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(DMError::DM_ERROR_NULLPTR), "JsDisplayManager::OnGetAllDisplay failed."));
            }
        };

    NativeValue* lastParam = nullptr;
    if (info.argc == ARGC_ONE && info.argv[0]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[0];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsDisplayManager::OnGetAllDisplay",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* OnGetAllDisplays(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGI("GetAllDisplays is called");

    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            std::vector<sptr<Display>> displays = SingletonContainer::Get<DisplayManager>().GetAllDisplays();
            if (!displays.empty()) {
                task.Resolve(engine, CreateJsDisplayArrayObject(engine, displays));
                WLOGI("GetAllDisplays success");
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN),
                    "JsDisplayManager::OnGetAllDisplays failed."));
            }
        };

    NativeValue* lastParam = nullptr;
    if (info.argc >= ARGC_ONE && info.argv[ARGC_ONE - 1] != nullptr &&
        info.argv[ARGC_ONE - 1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[0];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsDisplayManager::OnGetAllDisplays",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

DMError RegisterDisplayListenerWithType(NativeEngine& engine, const std::string& type, NativeValue* value)
{
    if (IfCallbackRegistered(type, value)) {
        WLOGFE("RegisterDisplayListenerWithType callback already registered!");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    std::unique_ptr<NativeReference> callbackRef;
    callbackRef.reset(engine.CreateReference(value, 1));
    sptr<JsDisplayListener> displayListener = new(std::nothrow) JsDisplayListener(&engine);
    DMError ret = DMError::DM_OK;
    if (displayListener == nullptr) {
        WLOGFE("displayListener is nullptr");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    if (type == EVENT_ADD || type == EVENT_REMOVE || type == EVENT_CHANGE) {
        ret = SingletonContainer::Get<DisplayManager>().RegisterDisplayListener(displayListener);
    } else if (type == EVENT_PRIVATE_MODE_CHANGE) {
        ret = SingletonContainer::Get<DisplayManager>().RegisterPrivateWindowListener(displayListener);
    } else if (type == EVENT_FOLD_STATUS_CHANGED) {
        ret = SingletonContainer::Get<DisplayManager>().RegisterFoldStatusListener(displayListener);
    } else if (type == EVENT_DISPLAY_MODE_CHANGED) {
        ret = SingletonContainer::Get<DisplayManager>().RegisterDisplayModeListener(displayListener);
    } else {
        WLOGFE("RegisterDisplayListenerWithType failed, %{public}s not support", type.c_str());
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    if (ret != DMError::DM_OK) {
        WLOGFE("RegisterDisplayListenerWithType failed, ret: %{public}u", ret);
        return ret;
    }
    displayListener->AddCallback(type, value);
    jsCbMap_[type][std::move(callbackRef)] = displayListener;
    return DMError::DM_OK;
}

bool IfCallbackRegistered(const std::string& type, NativeValue* jsListenerObject)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGI("IfCallbackRegistered methodName %{public}s not registered!", type.c_str());
        return false;
    }

    for (auto& iter : jsCbMap_[type]) {
        if (jsListenerObject->StrictEquals(iter.first->Get())) {
            WLOGFE("IfCallbackRegistered callback already registered!");
            return true;
        }
    }
    return false;
}

DMError UnregisterAllDisplayListenerWithType(const std::string& type)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGI("UnregisterAllDisplayListenerWithType methodName %{public}s not registered!",
            type.c_str());
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    DMError ret = DMError::DM_OK;
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
        it->second->RemoveAllCallback();
        if (type == EVENT_ADD || type == EVENT_REMOVE || type == EVENT_CHANGE) {
            sptr<DisplayManager::IDisplayListener> thisListener(it->second);
            ret = SingletonContainer::Get<DisplayManager>().UnregisterDisplayListener(thisListener);
            WLOGFD("unregister displayListener, type: %{public}s ret: %{public}u", type.c_str(), ret);
        } else if (type == EVENT_PRIVATE_MODE_CHANGE) {
            sptr<DisplayManager::IPrivateWindowListener> thisListener(it->second);
            ret = SingletonContainer::Get<DisplayManager>().UnregisterPrivateWindowListener(thisListener);
            WLOGFD("unregister privateWindowListener, ret: %{public}u", ret);
        }
        jsCbMap_[type].erase(it++);
    }
    jsCbMap_.erase(type);
    return ret;
}

DMError UnRegisterDisplayListenerWithType(const std::string& type, NativeValue* value)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGI("UnRegisterDisplayListenerWithType methodName %{public}s not registered!",
            type.c_str());
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    DMError ret = DMError::DM_OK;
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
        if (value->StrictEquals(it->first->Get())) {
            it->second->RemoveCallback(type, value);
            if (type == EVENT_ADD || type == EVENT_REMOVE || type == EVENT_CHANGE) {
                sptr<DisplayManager::IDisplayListener> thisListener(it->second);
                ret = SingletonContainer::Get<DisplayManager>().UnregisterDisplayListener(thisListener);
                WLOGFD("unregister displayListener, type: %{public}s ret: %{public}u", type.c_str(), ret);
            } else if (type == EVENT_PRIVATE_MODE_CHANGE) {
                sptr<DisplayManager::IPrivateWindowListener> thisListener(it->second);
                ret = SingletonContainer::Get<DisplayManager>().UnregisterPrivateWindowListener(thisListener);
                WLOGFD("unregister privateWindowListener, ret: %{public}u", ret);
            } else {
                ret = DMError::DM_ERROR_INVALID_PARAM;
                WLOGFE("unregister displaylistener with type failed, %{public}s not matched", type.c_str());
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
    return ret;
}

NativeValue* OnRegisterDisplayManagerCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGI("OnRegisterDisplayManagerCallback is called");
    if (info.argc < ARGC_TWO) {
        WLOGFE("JsDisplayManager Params not match: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    std::string cbType;
    if (!ConvertFromJsValue(engine, info.argv[0], cbType)) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        WLOGFE("Failed to convert parameter to callbackType");
        return engine.CreateUndefined();
    }
    NativeValue* value = info.argv[INDEX_ONE];
    if (value == nullptr) {
        WLOGI("OnRegisterDisplayManagerCallback info->argv[1] is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    if (!value->IsCallable()) {
        WLOGI("OnRegisterDisplayManagerCallback info->argv[1] is not callable");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    std::lock_guard<std::mutex> lock(mtx_);
    DmErrorCode ret = DM_JS_TO_ERROR_CODE_MAP.at(RegisterDisplayListenerWithType(engine, cbType, value));
    if (ret != DmErrorCode::DM_OK) {
        WLOGFE("Failed to register display listener with type");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    return engine.CreateUndefined();
}

NativeValue* OnUnregisterDisplayManagerCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGI("OnUnregisterDisplayCallback is called");
    if (info.argc < ARGC_ONE) {
        WLOGFE("JsDisplayManager Params not match %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    std::string cbType;
    if (!ConvertFromJsValue(engine, info.argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    std::lock_guard<std::mutex> lock(mtx_);
    DmErrorCode ret;
    if (info.argc == ARGC_ONE) {
        ret = DM_JS_TO_ERROR_CODE_MAP.at(UnregisterAllDisplayListenerWithType(cbType));
    } else {
        NativeValue* value = info.argv[INDEX_ONE];
        if ((value == nullptr) || (!value->IsCallable())) {
            ret = DM_JS_TO_ERROR_CODE_MAP.at(UnregisterAllDisplayListenerWithType(cbType));
        } else {
            ret = DM_JS_TO_ERROR_CODE_MAP.at(UnRegisterDisplayListenerWithType(cbType, value));
        }
    }
    if (ret != DmErrorCode::DM_OK) {
        WLOGFE("failed to unregister display listener with type");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    return engine.CreateUndefined();
}

NativeValue* OnHasPrivateWindow(NativeEngine& engine, NativeCallbackInfo& info)
{
    bool hasPrivateWindow = false;
    if (info.argc < ARGC_ONE) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    int64_t displayId = static_cast<int64_t>(DISPLAY_ID_INVALID);
    if (!ConvertFromJsValue(engine, info.argv[0], displayId)) {
        WLOGFE("[NAPI]Failed to convert parameter to displayId");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    if (displayId < 0) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    DmErrorCode errCode = DM_JS_TO_ERROR_CODE_MAP.at(
        SingletonContainer::Get<DisplayManager>().HasPrivateWindow(displayId, hasPrivateWindow));
    WLOGI("[NAPI]Display id = %{public}" PRIu64", hasPrivateWindow = %{public}u err = %{public}d",
        static_cast<uint64_t>(displayId), hasPrivateWindow, errCode);
    if (errCode != DmErrorCode::DM_OK) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(errCode)));
        return engine.CreateUndefined();
    }
    return engine.CreateBoolean(hasPrivateWindow);
}

NativeValue* CreateJsDisplayArrayObject(NativeEngine& engine, std::vector<sptr<Display>>& displays)
{
    WLOGI("CreateJsDisplayArrayObject is called");
    NativeValue* arrayValue = engine.CreateArray(displays.size());
    NativeArray* array = ConvertNativeValueTo<NativeArray>(arrayValue);
    if (array == nullptr) {
        WLOGFE("Failed to create display array");
        return engine.CreateUndefined();
    }
    int32_t i = 0;
    for (auto& display : displays) {
        if (display == nullptr) {
            continue;
        }
        array->SetElement(i++, CreateJsDisplayObject(engine, display));
    }
    return arrayValue;
}

NativeValue* OnIsFoldable(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc >= ARGC_ONE) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    bool foldable = SingletonContainer::Get<DisplayManager>().IsFoldable();
    WLOGI("[NAPI]" PRIu64", isFoldable = %{public}u", foldable);
    return engine.CreateBoolean(foldable);
}

NativeValue* OnGetFoldStatus(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc >= ARGC_ONE) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    FoldStatus status = SingletonContainer::Get<DisplayManager>().GetFoldStatus();
    WLOGI("[NAPI]" PRIu64", getFoldStatus = %{public}u", status);
    return CreateJsValue(engine, status);
}

NativeValue* OnGetFoldDisplayMode(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc >= ARGC_ONE) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    FoldDisplayMode mode = SingletonContainer::Get<DisplayManager>().GetFoldDisplayMode();
    WLOGI("[NAPI]" PRIu64", getFoldDisplayMode = %{public}u", mode);
    return CreateJsValue(engine, mode);
}

NativeValue* OnSetFoldDisplayMode(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc < ARGC_ONE) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    FoldDisplayMode mode = FoldDisplayMode::UNKNOWN;
    if (!ConvertFromJsValue(engine, info.argv[0], mode)) {
        WLOGFE("[NAPI]Failed to convert parameter to FoldDisplayMode");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    SingletonContainer::Get<DisplayManager>().SetFoldDisplayMode(mode);
    WLOGI("[NAPI]" PRIu64", setFoldDisplayMode");
    return engine.CreateUndefined();
}

NativeValue* OnGetCurrentFoldCreaseRegion(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc >= ARGC_ONE) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    sptr<FoldCreaseRegion> region = SingletonContainer::Get<DisplayManager>().GetCurrentFoldCreaseRegion();
    WLOGI("[NAPI]" PRIu64", getCurrentFoldCreaseRegion");
    return CreateJsFoldCreaseRegionObject(engine, region);
}

NativeValue* CreateJsFoldCreaseRegionObject(NativeEngine& engine, sptr<FoldCreaseRegion> region)
{
    WLOGI("JsDisplay::CreateJsFoldCreaseRegionObject is called");
    NativeValue* objValue = engine.CreateObject();
    auto* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert prop to jsObject");
        return engine.CreateUndefined();
    }
    if (region == nullptr) {
        WLOGFE("Get null fold crease region");
        return engine.CreateUndefined();
    }
    DisplayId displayId = region->GetDisplayId();
    std::vector<DMRect> creaseRects = region->GetCreaseRects();
    object->SetProperty("displayId", CreateJsValue(engine, static_cast<uint32_t>(displayId)));
    object->SetProperty("creaseRects", CreateJsCreaseRectsArrayObject(engine, creaseRects));
    return objValue;
}

NativeValue* CreateJsCreaseRectsArrayObject(NativeEngine& engine, std::vector<DMRect> creaseRects)
{
    NativeValue* arrayValue = engine.CreateArray(creaseRects.size());
    auto* array = ConvertNativeValueTo<NativeArray>(arrayValue);
    size_t i = 0;
    for (const auto& rect : creaseRects) {
        array->SetElement(i++, CreateJsRectObject(engine, rect));
    }
    return arrayValue;
}
};

NativeValue* InitDisplayState(NativeEngine* engine)
{
    WLOGI("InitDisplayState called");

    if (engine == nullptr) {
        WLOGFE("engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    object->SetProperty("STATE_UNKNOWN", CreateJsValue(*engine, static_cast<int32_t>(DisplayStateMode::STATE_UNKNOWN)));
    object->SetProperty("STATE_OFF", CreateJsValue(*engine, static_cast<int32_t>(DisplayStateMode::STATE_OFF)));
    object->SetProperty("STATE_ON", CreateJsValue(*engine, static_cast<int32_t>(DisplayStateMode::STATE_ON)));
    object->SetProperty("STATE_DOZE",
        CreateJsValue(*engine, static_cast<int32_t>(DisplayStateMode::STATE_DOZE)));
    object->SetProperty("STATE_DOZE_SUSPEND",
        CreateJsValue(*engine, static_cast<int32_t>(DisplayStateMode::STATE_DOZE_SUSPEND)));
    object->SetProperty("STATE_VR",
        CreateJsValue(*engine, static_cast<int32_t>(DisplayStateMode::STATE_VR)));
    object->SetProperty("STATE_ON_SUSPEND",
        CreateJsValue(*engine, static_cast<int32_t>(DisplayStateMode::STATE_ON_SUSPEND)));
    return objValue;
}

NativeValue* InitOrientation(NativeEngine* engine)
{
    WLOGI("InitOrientation called");

    if (engine == nullptr) {
        WLOGFE("engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    object->SetProperty("PORTRAIT", CreateJsValue(*engine, static_cast<uint32_t>(DisplayOrientation::PORTRAIT)));
    object->SetProperty("LANDSCAPE", CreateJsValue(*engine, static_cast<uint32_t>(DisplayOrientation::LANDSCAPE)));
    object->SetProperty("PORTRAIT_INVERTED",
        CreateJsValue(*engine, static_cast<uint32_t>(DisplayOrientation::PORTRAIT_INVERTED)));
    object->SetProperty("LANDSCAPE_INVERTED",
        CreateJsValue(*engine, static_cast<uint32_t>(DisplayOrientation::LANDSCAPE_INVERTED)));
    return objValue;
}

NativeValue* InitDisplayErrorCode(NativeEngine* engine)
{
    WLOGI("InitDisplayErrorCode called");

    if (engine == nullptr) {
        WLOGFE("engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    object->SetProperty("DM_ERROR_NO_PERMISSION",
        CreateJsValue(*engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_NO_PERMISSION)));
    object->SetProperty("DM_ERROR_INVALID_PARAM",
        CreateJsValue(*engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
    object->SetProperty("DM_ERROR_DEVICE_NOT_SUPPORT",
        CreateJsValue(*engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_DEVICE_NOT_SUPPORT)));
    object->SetProperty("DM_ERROR_INVALID_SCREEN",
        CreateJsValue(*engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN)));
    object->SetProperty("DM_ERROR_INVALID_CALLING",
        CreateJsValue(*engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_CALLING)));
    object->SetProperty("DM_ERROR_SYSTEM_INNORMAL",
        CreateJsValue(*engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL)));

    return objValue;
}

NativeValue* InitDisplayError(NativeEngine* engine)
{
    WLOGI("InitDisplayError called");

    if (engine == nullptr) {
        WLOGFE("engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    object->SetProperty("DM_ERROR_INIT_DMS_PROXY_LOCKED",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED)));
    object->SetProperty("DM_ERROR_IPC_FAILED",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_IPC_FAILED)));
    object->SetProperty("DM_ERROR_REMOTE_CREATE_FAILED",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_REMOTE_CREATE_FAILED)));
    object->SetProperty("DM_ERROR_NULLPTR",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_NULLPTR)));
    object->SetProperty("DM_ERROR_INVALID_PARAM",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_INVALID_PARAM)));
    object->SetProperty("DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED)));
    object->SetProperty("DM_ERROR_DEATH_RECIPIENT",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_DEATH_RECIPIENT)));
    object->SetProperty("DM_ERROR_INVALID_MODE_ID",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_INVALID_MODE_ID)));
    object->SetProperty("DM_ERROR_WRITE_DATA_FAILED",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_WRITE_DATA_FAILED)));
    object->SetProperty("DM_ERROR_RENDER_SERVICE_FAILED",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_RENDER_SERVICE_FAILED)));
    object->SetProperty("DM_ERROR_UNREGISTER_AGENT_FAILED",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_UNREGISTER_AGENT_FAILED)));
    object->SetProperty("DM_ERROR_INVALID_CALLING",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_INVALID_CALLING)));
    object->SetProperty("DM_ERROR_UNKNOWN",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_UNKNOWN)));

    return objValue;
}

NativeValue* InitFoldStatus(NativeEngine* engine)
{
    WLOGI("InitFoldStatus called");

    if (engine == nullptr) {
        WLOGFE("engine is nullptr");
        return nullptr;
    }

    NativeValue* objValue = engine->CreateObject();
    auto* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }
    object->SetProperty("FOLD_STATUS_UNKNOWN", CreateJsValue(*engine, static_cast<uint32_t>(FoldStatus::UNKNOWN)));
    object->SetProperty("FOLD_STATUS_EXPANDED", CreateJsValue(*engine, static_cast<uint32_t>(FoldStatus::EXPAND)));
    object->SetProperty("FOLD_STATUS_FOLDED", CreateJsValue(*engine, static_cast<uint32_t>(FoldStatus::FOLDED)));
    object->SetProperty("FOLD_STATUS_HALF_FOLDED",
        CreateJsValue(*engine, static_cast<uint32_t>(FoldStatus::HALF_FOLD)));
    return objValue;
}

NativeValue* InitFoldDisplayMode(NativeEngine* engine)
{
    WLOGI("IniFoldDisplayMode called");

    if (engine == nullptr) {
        WLOGFE("engine is nullptr");
        return nullptr;
    }

    NativeValue* objValue = engine->CreateObject();
    auto* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    object->SetProperty("FOLD_DISPLAY_MODE_UNKNOWN",
        CreateJsValue(*engine, static_cast<uint32_t>(FoldDisplayMode::UNKNOWN)));
    object->SetProperty("FOLD_DISPLAY_MODE_FULL", CreateJsValue(*engine, static_cast<uint32_t>(FoldDisplayMode::FULL)));
    object->SetProperty("FOLD_DISPLAY_MODE_MAIN", CreateJsValue(*engine, static_cast<uint32_t>(FoldDisplayMode::MAIN)));
    object->SetProperty("FOLD_DISPLAY_MODE_SUB", CreateJsValue(*engine, static_cast<uint32_t>(FoldDisplayMode::SUB)));
    object->SetProperty("FOLD_DISPLAY_MODE_COORDINATION",
        CreateJsValue(*engine, static_cast<uint32_t>(FoldDisplayMode::COORDINATION)));
    return objValue;
}

NativeValue* JsDisplayManagerInit(NativeEngine* engine, NativeValue* exportObj)
{
    WLOGI("JsDisplayManagerInit is called");

    if (engine == nullptr || exportObj == nullptr) {
        WLOGFE("JsDisplayManagerInit engine or exportObj is nullptr");
        return nullptr;
    }

    NativeObject* object = ConvertNativeValueTo<NativeObject>(exportObj);
    if (object == nullptr) {
        WLOGFE("JsDisplayManagerInit object is nullptr");
        return nullptr;
    }

    std::unique_ptr<JsDisplayManager> jsDisplayManager = std::make_unique<JsDisplayManager>(engine);
    object->SetNativePointer(jsDisplayManager.release(), JsDisplayManager::Finalizer, nullptr);

    object->SetProperty("DisplayState", InitDisplayState(engine));
    object->SetProperty("Orientation", InitOrientation(engine));
    object->SetProperty("DmErrorCode", InitDisplayErrorCode(engine));
    object->SetProperty("DMError", InitDisplayError(engine));
    object->SetProperty("FoldStatus", InitFoldStatus(engine));
    object->SetProperty("FoldDisplayMode", InitFoldDisplayMode(engine));

    const char *moduleName = "JsDisplayManager";
    BindNativeFunction(*engine, *object, "getDefaultDisplay", moduleName, JsDisplayManager::GetDefaultDisplay);
    BindNativeFunction(*engine, *object, "getDefaultDisplaySync", moduleName, JsDisplayManager::GetDefaultDisplaySync);
    BindNativeFunction(*engine, *object, "getAllDisplay", moduleName, JsDisplayManager::GetAllDisplay);
    BindNativeFunction(*engine, *object, "getAllDisplays", moduleName, JsDisplayManager::GetAllDisplays);
    BindNativeFunction(*engine, *object, "hasPrivateWindow", moduleName, JsDisplayManager::HasPrivateWindow);
    BindNativeFunction(*engine, *object, "isFoldable", moduleName, JsDisplayManager::IsFoldable);
    BindNativeFunction(*engine, *object, "getFoldStatus", moduleName, JsDisplayManager::GetFoldStatus);
    BindNativeFunction(*engine, *object, "getFoldDisplayMode", moduleName, JsDisplayManager::GetFoldDisplayMode);
    BindNativeFunction(*engine, *object, "setFoldDisplayMode", moduleName, JsDisplayManager::SetFoldDisplayMode);
    BindNativeFunction(*engine, *object, "getCurrentFoldCreaseRegion", moduleName,
        JsDisplayManager::GetCurrentFoldCreaseRegion);
    BindNativeFunction(*engine, *object, "on", moduleName, JsDisplayManager::RegisterDisplayManagerCallback);
    BindNativeFunction(*engine, *object, "off", moduleName, JsDisplayManager::UnregisterDisplayManagerCallback);
    return engine->CreateUndefined();
}
}  // namespace Rosen
}  // namespace OHOS