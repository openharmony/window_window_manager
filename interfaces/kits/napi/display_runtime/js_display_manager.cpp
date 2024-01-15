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
explicit JsDisplayManager(napi_env env) {
}

~JsDisplayManager() = default;

static void Finalizer(napi_env env, void* data, void* hint)
{
    WLOGI("Finalizer is called");
    std::unique_ptr<JsDisplayManager>(static_cast<JsDisplayManager*>(data));
}

static napi_value GetDefaultDisplay(napi_env env, napi_callback_info info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(env, info);
    return (me != nullptr) ? me->OnGetDefaultDisplay(env, info) : nullptr;
}

static napi_value GetDefaultDisplaySync(napi_env env, napi_callback_info info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(env, info);
    return (me != nullptr) ? me->OnGetDefaultDisplaySync(env, info) : nullptr;
}

static napi_value GetAllDisplay(napi_env env, napi_callback_info info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(env, info);
    return (me != nullptr) ? me->OnGetAllDisplay(env, info) : nullptr;
}

static napi_value GetAllDisplays(napi_env env, napi_callback_info info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(env, info);
    return (me != nullptr) ? me->OnGetAllDisplays(env, info) : nullptr;
}

static napi_value RegisterDisplayManagerCallback(napi_env env, napi_callback_info info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(env, info);
    return (me != nullptr) ? me->OnRegisterDisplayManagerCallback(env, info) : nullptr;
}

static napi_value UnregisterDisplayManagerCallback(napi_env env, napi_callback_info info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(env, info);
    return (me != nullptr) ? me->OnUnregisterDisplayManagerCallback(env, info) : nullptr;
}

static napi_value HasPrivateWindow(napi_env env, napi_callback_info info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(env, info);
    return (me != nullptr) ? me->OnHasPrivateWindow(env, info) : nullptr;
}

static napi_value IsFoldable(napi_env env, napi_callback_info info)
{
    auto* me = CheckParamsAndGetThis<JsDisplayManager>(env, info);
    return (me != nullptr) ? me->OnIsFoldable(env, info) : nullptr;
}

static napi_value GetFoldStatus(napi_env env, napi_callback_info info)
{
    auto* me = CheckParamsAndGetThis<JsDisplayManager>(env, info);
    return (me != nullptr) ? me->OnGetFoldStatus(env, info) : nullptr;
}

static napi_value GetFoldDisplayMode(napi_env env, napi_callback_info info)
{
    auto* me = CheckParamsAndGetThis<JsDisplayManager>(env, info);
    return (me != nullptr) ? me->OnGetFoldDisplayMode(env, info) : nullptr;
}

static napi_value SetFoldDisplayMode(napi_env env, napi_callback_info info)
{
    auto* me = CheckParamsAndGetThis<JsDisplayManager>(env, info);
    return (me != nullptr) ? me->OnSetFoldDisplayMode(env, info) : nullptr;
}

static napi_value SetFoldStatusLocked(napi_env env, napi_callback_info info)
{
    auto* me = CheckParamsAndGetThis<JsDisplayManager>(env, info);
    return (me != nullptr) ? me->OnSetFoldStatusLocked(env, info) : nullptr;
}

static napi_value GetCurrentFoldCreaseRegion(napi_env env, napi_callback_info info)
{
    auto* me = CheckParamsAndGetThis<JsDisplayManager>(env, info);
    return (me != nullptr) ? me->OnGetCurrentFoldCreaseRegion(env, info) : nullptr;
}

private:
std::map<std::string, std::map<std::unique_ptr<NativeReference>, sptr<JsDisplayListener>>> jsCbMap_;
std::mutex mtx_;

napi_value OnGetDefaultDisplay(napi_env env, napi_callback_info info)
{
    WLOGI("GetDefaultDisplay called");
    DMError errCode = DMError::DM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != 0 && argc != ARGC_ONE) {
        WLOGFE("OnGetDefaultDisplay params not match");
        errCode = DMError::DM_ERROR_INVALID_PARAM;
    }

    NapiAsyncTask::CompleteCallback complete =
        [=](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (errCode != DMError::DM_OK) {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(errCode), "JsDisplayManager::OnGetDefaultDisplay failed."));
            }
            HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "Async:GetDefaultDisplay");
            sptr<Display> display = SingletonContainer::Get<DisplayManager>().GetDefaultDisplay();
            if (display != nullptr) {
                task.Resolve(env, CreateJsDisplayObject(env, display));
                WLOGI("OnGetDefaultDisplay success");
            } else {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(DMError::DM_ERROR_NULLPTR), "JsDisplayManager::OnGetDefaultDisplay failed."));
            }
        };
    napi_value lastParam = nullptr;
    if (argc == ARGC_ONE && GetType(env, argv[0]) == napi_function) {
        lastParam = argv[0];
    }
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsDisplayManager::OnGetDefaultDisplay",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value OnGetDefaultDisplaySync(napi_env env, napi_callback_info info)
{
    WLOGD("GetDefaultDisplaySync called");
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "Sync:GetDefaultDisplay");
    sptr<Display> display = SingletonContainer::Get<DisplayManager>().GetDefaultDisplaySync();
    if (display == nullptr) {
        WLOGFE("OnGetDefaultDisplaySync, display is nullptr.");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN)));
        return NapiGetUndefined(env);
    }
    return CreateJsDisplayObject(env, display);
}

napi_value OnGetAllDisplay(napi_env env, napi_callback_info info)
{
    WLOGD("GetAllDisplay called");
    DMError errCode = DMError::DM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != 0 && argc != ARGC_ONE) {
        WLOGFE("OnGetAllDisplay params not match");
        errCode = DMError::DM_ERROR_INVALID_PARAM;
    }

    NapiAsyncTask::CompleteCallback complete =
        [=](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (errCode != DMError::DM_OK) {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(errCode), "JsDisplayManager::OnGetAllDisplay failed."));
            }
            std::vector<sptr<Display>> displays = SingletonContainer::Get<DisplayManager>().GetAllDisplays();
            if (!displays.empty()) {
                task.Resolve(env, CreateJsDisplayArrayObject(env, displays));
                WLOGI("GetAllDisplays success");
            } else {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(DMError::DM_ERROR_NULLPTR), "JsDisplayManager::OnGetAllDisplay failed."));
            }
        };

    napi_value lastParam = nullptr;
    if (argc == ARGC_ONE && GetType(env, argv[0]) == napi_function) {
        lastParam = argv[0];
    }
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsDisplayManager::OnGetAllDisplay",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value OnGetAllDisplays(napi_env env, napi_callback_info info)
{
    WLOGD("GetAllDisplays is called");

    NapiAsyncTask::CompleteCallback complete =
        [=](napi_env env, NapiAsyncTask& task, int32_t status) {
            std::vector<sptr<Display>> displays = SingletonContainer::Get<DisplayManager>().GetAllDisplays();
            if (!displays.empty()) {
                task.Resolve(env, CreateJsDisplayArrayObject(env, displays));
                WLOGD("GetAllDisplays success");
            } else {
                auto errorPending = false;
                napi_is_exception_pending(env, &errorPending);
                if (errorPending) {
                    napi_value exception = nullptr;
                    napi_get_and_clear_last_exception(env, &exception);
                }
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN),
                    "JsDisplayManager::OnGetAllDisplays failed."));
            }
        };
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value lastParam = nullptr;
    if (argc >= ARGC_ONE && argv[ARGC_ONE - 1] != nullptr &&
        GetType(env, argv[ARGC_ONE - 1]) == napi_function) {
        lastParam = argv[0];
    }
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsDisplayManager::OnGetAllDisplays",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

DMError RegisterDisplayListenerWithType(napi_env env, const std::string& type, napi_value value)
{
    if (IfCallbackRegistered(env, type, value)) {
        WLOGFE("RegisterDisplayListenerWithType callback already registered!");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    std::unique_ptr<NativeReference> callbackRef;
    napi_ref result = nullptr;
    napi_create_reference(env, value, 1, &result);
    callbackRef.reset(reinterpret_cast<NativeReference*>(result));
    sptr<JsDisplayListener> displayListener = new(std::nothrow) JsDisplayListener(env);
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
    } else if (type == EVENT_AVAILABLE_AREA_CHANGED) {
        ret = SingletonContainer::Get<DisplayManager>().RegisterAvailableAreaListener(displayListener);
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

bool IfCallbackRegistered(napi_env env, const std::string& type, napi_value jsListenerObject)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGI("IfCallbackRegistered methodName %{public}s not registered!", type.c_str());
        return false;
    }

    for (auto& iter : jsCbMap_[type]) {
        bool isEquals = false;
        napi_strict_equals(env, jsListenerObject, iter.first->GetNapiValue(), &isEquals);
        if (isEquals) {
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

DMError UnRegisterDisplayListenerWithType(napi_env env, const std::string& type, napi_value value)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGI("UnRegisterDisplayListenerWithType methodName %{public}s not registered!",
            type.c_str());
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    DMError ret = DMError::DM_OK;
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
        bool isEquals = false;
        napi_strict_equals(env, value, it->first->GetNapiValue(), &isEquals);
        if (isEquals) {
            it->second->RemoveCallback(env, type, value);
            if (type == EVENT_ADD || type == EVENT_REMOVE || type == EVENT_CHANGE) {
                sptr<DisplayManager::IDisplayListener> thisListener(it->second);
                ret = SingletonContainer::Get<DisplayManager>().UnregisterDisplayListener(thisListener);
                WLOGFD("unregister displayListener, type: %{public}s ret: %{public}u", type.c_str(), ret);
            } else if (type == EVENT_PRIVATE_MODE_CHANGE) {
                sptr<DisplayManager::IPrivateWindowListener> thisListener(it->second);
                ret = SingletonContainer::Get<DisplayManager>().UnregisterPrivateWindowListener(thisListener);
                WLOGFD("unregister privateWindowListener, ret: %{public}u", ret);
            } else if (type == EVENT_AVAILABLE_AREA_CHANGED) {
                sptr<DisplayManager::IAvailableAreaListener> thisListener(it->second);
                ret = SingletonContainer::Get<DisplayManager>().UnregisterAvailableAreaListener(thisListener);
                WLOGFD("unregister IAvailableAreaListener, ret: %{public}u", ret);
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

bool NapiIsCallable(napi_env env, napi_value value)
{
    bool result = false;
    napi_is_callable(env, value, &result);
    return result;
}

napi_value OnRegisterDisplayManagerCallback(napi_env env, napi_callback_info info)
{
    WLOGD("OnRegisterDisplayManagerCallback is called");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_TWO) {
        WLOGFE("JsDisplayManager Params not match: %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    std::string cbType;
    if (!ConvertFromJsValue(env, argv[0], cbType)) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        WLOGFE("Failed to convert parameter to callbackType");
        return NapiGetUndefined(env);
    }
    napi_value value = argv[INDEX_ONE];
    if (value == nullptr) {
        WLOGI("OnRegisterDisplayManagerCallback info->argv[1] is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    if (!NapiIsCallable(env, value)) {
        WLOGI("OnRegisterDisplayManagerCallback info->argv[1] is not callable");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    std::lock_guard<std::mutex> lock(mtx_);
    DmErrorCode ret = DM_JS_TO_ERROR_CODE_MAP.at(RegisterDisplayListenerWithType(env, cbType, value));
    if (ret != DmErrorCode::DM_OK) {
        WLOGFE("Failed to register display listener with type");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    return NapiGetUndefined(env);
}

napi_value OnUnregisterDisplayManagerCallback(napi_env env, napi_callback_info info)
{
    WLOGI("OnUnregisterDisplayCallback is called");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        WLOGFE("JsDisplayManager Params not match %{public}zu", argc);
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    std::string cbType;
    if (!ConvertFromJsValue(env, argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    std::lock_guard<std::mutex> lock(mtx_);
    DmErrorCode ret;
    if (argc == ARGC_ONE) {
        ret = DM_JS_TO_ERROR_CODE_MAP.at(UnregisterAllDisplayListenerWithType(cbType));
    } else {
        napi_value value = argv[INDEX_ONE];
        if ((value == nullptr) || (!NapiIsCallable(env, value))) {
            ret = DM_JS_TO_ERROR_CODE_MAP.at(UnregisterAllDisplayListenerWithType(cbType));
        } else {
            ret = DM_JS_TO_ERROR_CODE_MAP.at(UnRegisterDisplayListenerWithType(env, cbType, value));
        }
    }
    if (ret != DmErrorCode::DM_OK) {
        WLOGFW("failed to unregister display listener with type");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    return NapiGetUndefined(env);
}

napi_value OnHasPrivateWindow(napi_env env, napi_callback_info info)
{
    bool hasPrivateWindow = false;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    int64_t displayId = static_cast<int64_t>(DISPLAY_ID_INVALID);
    if (!ConvertFromJsValue(env, argv[0], displayId)) {
        WLOGFE("[NAPI]Failed to convert parameter to displayId");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    if (displayId < 0) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    DmErrorCode errCode = DM_JS_TO_ERROR_CODE_MAP.at(
        SingletonContainer::Get<DisplayManager>().HasPrivateWindow(displayId, hasPrivateWindow));
    WLOGI("[NAPI]Display id = %{public}" PRIu64", hasPrivateWindow = %{public}u err = %{public}d",
        static_cast<uint64_t>(displayId), hasPrivateWindow, errCode);
    if (errCode != DmErrorCode::DM_OK) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(errCode)));
        return NapiGetUndefined(env);
    }
    napi_value result;
    napi_get_boolean(env, hasPrivateWindow, &result);
    return result;
}

napi_value CreateJsDisplayArrayObject(napi_env env, std::vector<sptr<Display>>& displays)
{
    WLOGD("CreateJsDisplayArrayObject is called");
    napi_value arrayValue = nullptr;
    napi_create_array_with_length(env, displays.size(), &arrayValue);
    if (arrayValue == nullptr) {
        WLOGFE("Failed to create display array");
        return NapiGetUndefined(env);
    }
    int32_t i = 0;
    for (auto& display : displays) {
        if (display == nullptr) {
            continue;
        }
        napi_set_element(env, arrayValue, i++, CreateJsDisplayObject(env, display));
    }
    return arrayValue;
}

napi_value OnIsFoldable(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc >= ARGC_ONE) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    bool foldable = SingletonContainer::Get<DisplayManager>().IsFoldable();
    WLOGD("[NAPI]" PRIu64", isFoldable = %{public}u", foldable);
    napi_value result;
    napi_get_boolean(env, foldable, &result);
    return result;
}

napi_value OnGetFoldStatus(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc >= ARGC_ONE) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    FoldStatus status = SingletonContainer::Get<DisplayManager>().GetFoldStatus();
    WLOGD("[NAPI]" PRIu64", getFoldStatus = %{public}u", status);
    return CreateJsValue(env, status);
}

napi_value OnGetFoldDisplayMode(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc >= ARGC_ONE) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    FoldDisplayMode mode = SingletonContainer::Get<DisplayManager>().GetFoldDisplayMode();
    WLOGD("[NAPI]" PRIu64", getFoldDisplayMode = %{public}u", mode);
    return CreateJsValue(env, mode);
}

napi_value OnSetFoldDisplayMode(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    FoldDisplayMode mode = FoldDisplayMode::UNKNOWN;
    if (!ConvertFromJsValue(env, argv[0], mode)) {
        WLOGFE("[NAPI]Failed to convert parameter to FoldDisplayMode");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    SingletonContainer::Get<DisplayManager>().SetFoldDisplayMode(mode);
    WLOGI("[NAPI]" PRIu64", setFoldDisplayMode");
    return NapiGetUndefined(env);
}

napi_value OnSetFoldStatusLocked(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    bool locked = false;
    if (!ConvertFromJsValue(env, argv[0], locked)) {
        WLOGFE("[NAPI]Failed to convert parameter to SetFoldStatusLocked");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    SingletonContainer::Get<DisplayManager>().SetFoldStatusLocked(locked);
    WLOGI("[NAPI]" PRIu64", SetFoldStatusLocked");
    return NapiGetUndefined(env);
}

napi_value OnGetCurrentFoldCreaseRegion(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc >= ARGC_ONE) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    sptr<FoldCreaseRegion> region = SingletonContainer::Get<DisplayManager>().GetCurrentFoldCreaseRegion();
    WLOGI("[NAPI]" PRIu64", getCurrentFoldCreaseRegion");
    return CreateJsFoldCreaseRegionObject(env, region);
}

napi_value CreateJsFoldCreaseRegionObject(napi_env env, sptr<FoldCreaseRegion> region)
{
    WLOGI("JsDisplay::CreateJsFoldCreaseRegionObject is called");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to convert prop to jsObject");
        return NapiGetUndefined(env);
    }
    if (region == nullptr) {
        WLOGFW("Get null fold crease region");
        return NapiGetUndefined(env);
    }
    DisplayId displayId = region->GetDisplayId();
    std::vector<DMRect> creaseRects = region->GetCreaseRects();
    napi_set_named_property(env, objValue, "displayId", CreateJsValue(env, static_cast<uint32_t>(displayId)));
    napi_set_named_property(env, objValue, "creaseRects", CreateJsCreaseRectsArrayObject(env, creaseRects));
    return objValue;
}

napi_value CreateJsCreaseRectsArrayObject(napi_env env, std::vector<DMRect> creaseRects)
{
    napi_value arrayValue = nullptr;
    napi_create_array_with_length(env, creaseRects.size(), &arrayValue);
    size_t i = 0;
    for (const auto& rect : creaseRects) {
        napi_set_element(env, arrayValue, i++, CreateJsRectObject(env, rect));
    }
    return arrayValue;
}
};

napi_value InitDisplayState(napi_env env)
{
    WLOGD("InitDisplayState called");

    if (env == nullptr) {
        WLOGFE("env is nullptr");
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }
    napi_set_named_property(env, objValue, "STATE_UNKNOWN",
        CreateJsValue(env, static_cast<int32_t>(DisplayStateMode::STATE_UNKNOWN)));
    napi_set_named_property(env, objValue, "STATE_OFF",
        CreateJsValue(env, static_cast<int32_t>(DisplayStateMode::STATE_OFF)));
    napi_set_named_property(env, objValue, "STATE_ON",
        CreateJsValue(env, static_cast<int32_t>(DisplayStateMode::STATE_ON)));
    napi_set_named_property(env, objValue, "STATE_DOZE",
        CreateJsValue(env, static_cast<int32_t>(DisplayStateMode::STATE_DOZE)));
    napi_set_named_property(env, objValue, "STATE_DOZE_SUSPEND",
        CreateJsValue(env, static_cast<int32_t>(DisplayStateMode::STATE_DOZE_SUSPEND)));
    napi_set_named_property(env, objValue, "STATE_VR",
        CreateJsValue(env, static_cast<int32_t>(DisplayStateMode::STATE_VR)));
    napi_set_named_property(env, objValue, "STATE_ON_SUSPEND",
        CreateJsValue(env, static_cast<int32_t>(DisplayStateMode::STATE_ON_SUSPEND)));
    return objValue;
}

napi_value InitOrientation(napi_env env)
{
    WLOGD("InitOrientation called");

    if (env == nullptr) {
        WLOGFE("env is nullptr");
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    napi_set_named_property(env, objValue, "PORTRAIT",
        CreateJsValue(env, static_cast<uint32_t>(DisplayOrientation::PORTRAIT)));
    napi_set_named_property(env, objValue, "LANDSCAPE",
        CreateJsValue(env, static_cast<uint32_t>(DisplayOrientation::LANDSCAPE)));
    napi_set_named_property(env, objValue, "PORTRAIT_INVERTED",
        CreateJsValue(env, static_cast<uint32_t>(DisplayOrientation::PORTRAIT_INVERTED)));
    napi_set_named_property(env, objValue, "LANDSCAPE_INVERTED",
        CreateJsValue(env, static_cast<uint32_t>(DisplayOrientation::LANDSCAPE_INVERTED)));
    return objValue;
}

napi_value InitDisplayErrorCode(napi_env env)
{
    WLOGD("InitDisplayErrorCode called");

    if (env == nullptr) {
        WLOGFE("env is nullptr");
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    napi_set_named_property(env, objValue, "DM_ERROR_NO_PERMISSION",
        CreateJsValue(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_NO_PERMISSION)));
    napi_set_named_property(env, objValue, "DM_ERROR_INVALID_PARAM",
        CreateJsValue(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
    napi_set_named_property(env, objValue, "DM_ERROR_DEVICE_NOT_SUPPORT",
        CreateJsValue(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_DEVICE_NOT_SUPPORT)));
    napi_set_named_property(env, objValue, "DM_ERROR_INVALID_SCREEN",
        CreateJsValue(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN)));
    napi_set_named_property(env, objValue, "DM_ERROR_INVALID_CALLING",
        CreateJsValue(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_CALLING)));
    napi_set_named_property(env, objValue, "DM_ERROR_SYSTEM_INNORMAL",
        CreateJsValue(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL)));

    return objValue;
}

napi_value InitDisplayError(napi_env env)
{
    WLOGD("InitDisplayError called");

    if (env == nullptr) {
        WLOGFE("env is nullptr");
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    napi_set_named_property(env, objValue, "DM_ERROR_INIT_DMS_PROXY_LOCKED",
        CreateJsValue(env, static_cast<int32_t>(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED)));
    napi_set_named_property(env, objValue, "DM_ERROR_IPC_FAILED",
        CreateJsValue(env, static_cast<int32_t>(DMError::DM_ERROR_IPC_FAILED)));
    napi_set_named_property(env, objValue, "DM_ERROR_REMOTE_CREATE_FAILED",
        CreateJsValue(env, static_cast<int32_t>(DMError::DM_ERROR_REMOTE_CREATE_FAILED)));
    napi_set_named_property(env, objValue, "DM_ERROR_NULLPTR",
        CreateJsValue(env, static_cast<int32_t>(DMError::DM_ERROR_NULLPTR)));
    napi_set_named_property(env, objValue, "DM_ERROR_INVALID_PARAM",
        CreateJsValue(env, static_cast<int32_t>(DMError::DM_ERROR_INVALID_PARAM)));
    napi_set_named_property(env, objValue, "DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED",
        CreateJsValue(env, static_cast<int32_t>(DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED)));
    napi_set_named_property(env, objValue, "DM_ERROR_DEATH_RECIPIENT",
        CreateJsValue(env, static_cast<int32_t>(DMError::DM_ERROR_DEATH_RECIPIENT)));
    napi_set_named_property(env, objValue, "DM_ERROR_INVALID_MODE_ID",
        CreateJsValue(env, static_cast<int32_t>(DMError::DM_ERROR_INVALID_MODE_ID)));
    napi_set_named_property(env, objValue, "DM_ERROR_WRITE_DATA_FAILED",
        CreateJsValue(env, static_cast<int32_t>(DMError::DM_ERROR_WRITE_DATA_FAILED)));
    napi_set_named_property(env, objValue, "DM_ERROR_RENDER_SERVICE_FAILED",
        CreateJsValue(env, static_cast<int32_t>(DMError::DM_ERROR_RENDER_SERVICE_FAILED)));
    napi_set_named_property(env, objValue, "DM_ERROR_UNREGISTER_AGENT_FAILED",
        CreateJsValue(env, static_cast<int32_t>(DMError::DM_ERROR_UNREGISTER_AGENT_FAILED)));
    napi_set_named_property(env, objValue, "DM_ERROR_INVALID_CALLING",
        CreateJsValue(env, static_cast<int32_t>(DMError::DM_ERROR_INVALID_CALLING)));
    napi_set_named_property(env, objValue, "DM_ERROR_UNKNOWN",
        CreateJsValue(env, static_cast<int32_t>(DMError::DM_ERROR_UNKNOWN)));

    return objValue;
}

napi_value InitFoldStatus(napi_env env)
{
    WLOGD("InitFoldStatus called");

    if (env == nullptr) {
        WLOGFE("env is nullptr");
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }
    napi_set_named_property(env, objValue, "FOLD_STATUS_UNKNOWN",
        CreateJsValue(env, static_cast<uint32_t>(FoldStatus::UNKNOWN)));
    napi_set_named_property(env, objValue, "FOLD_STATUS_EXPANDED",
        CreateJsValue(env, static_cast<uint32_t>(FoldStatus::EXPAND)));
    napi_set_named_property(env, objValue, "FOLD_STATUS_FOLDED",
        CreateJsValue(env, static_cast<uint32_t>(FoldStatus::FOLDED)));
    napi_set_named_property(env, objValue, "FOLD_STATUS_HALF_FOLDED",
        CreateJsValue(env, static_cast<uint32_t>(FoldStatus::HALF_FOLD)));
    return objValue;
}

napi_value InitFoldDisplayMode(napi_env env)
{
    WLOGD("IniFoldDisplayMode called");

    if (env == nullptr) {
        WLOGFE("env is nullptr");
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    napi_set_named_property(env, objValue, "FOLD_DISPLAY_MODE_UNKNOWN",
        CreateJsValue(env, static_cast<uint32_t>(FoldDisplayMode::UNKNOWN)));
    napi_set_named_property(env, objValue, "FOLD_DISPLAY_MODE_FULL",
        CreateJsValue(env, static_cast<uint32_t>(FoldDisplayMode::FULL)));
    napi_set_named_property(env, objValue, "FOLD_DISPLAY_MODE_MAIN",
        CreateJsValue(env, static_cast<uint32_t>(FoldDisplayMode::MAIN)));
    napi_set_named_property(env, objValue, "FOLD_DISPLAY_MODE_SUB",
        CreateJsValue(env, static_cast<uint32_t>(FoldDisplayMode::SUB)));
    napi_set_named_property(env, objValue, "FOLD_DISPLAY_MODE_COORDINATION",
        CreateJsValue(env, static_cast<uint32_t>(FoldDisplayMode::COORDINATION)));
    return objValue;
}

napi_value InitColorSpace(napi_env env)
{
    WLOGD("InitColorSpace called");

    if (env == nullptr) {
        WLOGFE("env is nullptr");
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    napi_set_named_property(env, objValue, "UNKNOWN",
        CreateJsValue(env, static_cast<uint32_t>(ColorSpace::UNKNOWN)));
    napi_set_named_property(env, objValue, "ADOBE_RGB",
        CreateJsValue(env, static_cast<uint32_t>(ColorSpace::ADOBE_RGB)));
    napi_set_named_property(env, objValue, "BT2020_HLG",
        CreateJsValue(env, static_cast<uint32_t>(ColorSpace::BT2020_HLG)));
    napi_set_named_property(env, objValue, "BT2020_PQ",
        CreateJsValue(env, static_cast<uint32_t>(ColorSpace::BT2020_PQ)));
    napi_set_named_property(env, objValue, "BT601_EBU",
        CreateJsValue(env, static_cast<uint32_t>(ColorSpace::BT601_EBU)));
    napi_set_named_property(env, objValue, "BT601_SMPTE_C",
        CreateJsValue(env, static_cast<uint32_t>(ColorSpace::BT601_SMPTE_C)));
    napi_set_named_property(env, objValue, "BT709",
        CreateJsValue(env, static_cast<uint32_t>(ColorSpace::BT709)));
    napi_set_named_property(env, objValue, "P3_HLG",
        CreateJsValue(env, static_cast<uint32_t>(ColorSpace::P3_HLG)));
    napi_set_named_property(env, objValue, "P3_PQ",
        CreateJsValue(env, static_cast<uint32_t>(ColorSpace::P3_PQ)));
    napi_set_named_property(env, objValue, "DISPLAY_P3",
        CreateJsValue(env, static_cast<uint32_t>(ColorSpace::DISPLAY_P3)));
    napi_set_named_property(env, objValue, "SRGB",
        CreateJsValue(env, static_cast<uint32_t>(ColorSpace::SRGB)));
    napi_set_named_property(env, objValue, "LINEAR_SRGB",
        CreateJsValue(env, static_cast<uint32_t>(ColorSpace::LINEAR_SRGB)));
    napi_set_named_property(env, objValue, "LINEAR_P3",
        CreateJsValue(env, static_cast<uint32_t>(ColorSpace::LINEAR_P3)));
    napi_set_named_property(env, objValue, "LINEAR_BT2020",
        CreateJsValue(env, static_cast<uint32_t>(ColorSpace::LINEAR_BT2020)));
    return objValue;
}

napi_value InitHDRFormat(napi_env env)
{
    WLOGD("InitHDRFormat called");

    if (env == nullptr) {
        WLOGFE("env is nullptr");
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    napi_set_named_property(env, objValue, "NONE",
        CreateJsValue(env, static_cast<uint32_t>(HDRFormat::NONE)));
    napi_set_named_property(env, objValue, "VIDEO_HLG",
        CreateJsValue(env, static_cast<uint32_t>(HDRFormat::VIDEO_HLG)));
    napi_set_named_property(env, objValue, "VIDEO_HDR10",
        CreateJsValue(env, static_cast<uint32_t>(HDRFormat::VIDEO_HDR10)));
    napi_set_named_property(env, objValue, "VIDEO_HDR_VIVID",
        CreateJsValue(env, static_cast<uint32_t>(HDRFormat::VIDEO_HDR_VIVID)));
    napi_set_named_property(env, objValue, "IMAGE_HDR_VIVID_DUAL",
        CreateJsValue(env, static_cast<uint32_t>(HDRFormat::IMAGE_HDR_VIVID_DUAL)));
    napi_set_named_property(env, objValue, "IMAGE_HDR_VIVID_SINGLE",
        CreateJsValue(env, static_cast<uint32_t>(HDRFormat::IMAGE_HDR_VIVID_SINGLE)));
    napi_set_named_property(env, objValue, "IMAGE_HDR_ISO_DUAL",
        CreateJsValue(env, static_cast<uint32_t>(HDRFormat::IMAGE_HDR_ISO_DUAL)));
    napi_set_named_property(env, objValue, "IMAGE_HDR_ISO_SINGLE",
        CreateJsValue(env, static_cast<uint32_t>(HDRFormat::IMAGE_HDR_ISO_SINGLE)));
    return objValue;
}

napi_value JsDisplayManagerInit(napi_env env, napi_value exportObj)
{
    WLOGD("JsDisplayManagerInit is called");

    if (env == nullptr || exportObj == nullptr) {
        WLOGFE("JsDisplayManagerInit env or exportObj is nullptr");
        return nullptr;
    }

    std::unique_ptr<JsDisplayManager> jsDisplayManager = std::make_unique<JsDisplayManager>(env);
    napi_wrap(env, exportObj, jsDisplayManager.release(), JsDisplayManager::Finalizer, nullptr, nullptr);

    napi_set_named_property(env, exportObj, "DisplayState", InitDisplayState(env));
    napi_set_named_property(env, exportObj, "Orientation", InitOrientation(env));
    napi_set_named_property(env, exportObj, "DmErrorCode", InitDisplayErrorCode(env));
    napi_set_named_property(env, exportObj, "DMError", InitDisplayError(env));
    napi_set_named_property(env, exportObj, "FoldStatus", InitFoldStatus(env));
    napi_set_named_property(env, exportObj, "FoldDisplayMode", InitFoldDisplayMode(env));
    napi_set_named_property(env, exportObj, "ColorSpace", InitColorSpace(env));
    napi_set_named_property(env, exportObj, "HDRFormat", InitHDRFormat(env));

    const char *moduleName = "JsDisplayManager";
    BindNativeFunction(env, exportObj, "getDefaultDisplay", moduleName, JsDisplayManager::GetDefaultDisplay);
    BindNativeFunction(env, exportObj, "getDefaultDisplaySync", moduleName, JsDisplayManager::GetDefaultDisplaySync);
    BindNativeFunction(env, exportObj, "getAllDisplay", moduleName, JsDisplayManager::GetAllDisplay);
    BindNativeFunction(env, exportObj, "getAllDisplays", moduleName, JsDisplayManager::GetAllDisplays);
    BindNativeFunction(env, exportObj, "hasPrivateWindow", moduleName, JsDisplayManager::HasPrivateWindow);
    BindNativeFunction(env, exportObj, "isFoldable", moduleName, JsDisplayManager::IsFoldable);
    BindNativeFunction(env, exportObj, "getFoldStatus", moduleName, JsDisplayManager::GetFoldStatus);
    BindNativeFunction(env, exportObj, "getFoldDisplayMode", moduleName, JsDisplayManager::GetFoldDisplayMode);
    BindNativeFunction(env, exportObj, "setFoldDisplayMode", moduleName, JsDisplayManager::SetFoldDisplayMode);
    BindNativeFunction(env, exportObj, "setFoldStatusLocked", moduleName, JsDisplayManager::SetFoldStatusLocked);
    BindNativeFunction(env, exportObj, "getCurrentFoldCreaseRegion", moduleName,
        JsDisplayManager::GetCurrentFoldCreaseRegion);
    BindNativeFunction(env, exportObj, "on", moduleName, JsDisplayManager::RegisterDisplayManagerCallback);
    BindNativeFunction(env, exportObj, "off", moduleName, JsDisplayManager::UnregisterDisplayManagerCallback);
    return NapiGetUndefined(env);
}
}  // namespace Rosen
}  // namespace OHOS
