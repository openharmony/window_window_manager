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

#include <cinttypes>
#include <vector>
#include <new>
#include "js_runtime_utils.h"
#include "js_screen.h"
#include "js_screen_listener.h"
#include "native_engine/native_reference.h"
#include "screen_manager.h"
#include "singleton_container.h"
#include "surface_utils.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
constexpr size_t ARGC_THREE = 3;
constexpr int32_t INDEX_ONE = 1;
constexpr uint32_t MAX_SCREENS_NUM = 1000;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "JsScreenManager"};
}

class JsScreenManager {
public:
explicit JsScreenManager(napi_env env) {
}

~JsScreenManager() = default;

static void Finalizer(napi_env env, void* data, void* hint)
{
    WLOGI("JsScreenManager::Finalizer is called");
    std::unique_ptr<JsScreenManager>(static_cast<JsScreenManager*>(data));
}

static napi_value GetAllScreens(napi_env env, napi_callback_info info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(env, info);
    return (me != nullptr) ? me->OnGetAllScreens(env, info) : nullptr;
}

static napi_value RegisterScreenManagerCallback(napi_env env, napi_callback_info info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(env, info);
    return (me != nullptr) ? me->OnRegisterScreenManagerCallback(env, info) : nullptr;
}

static napi_value UnregisterScreenMangerCallback(napi_env env, napi_callback_info info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(env, info);
    return (me != nullptr) ? me->OnUnregisterScreenManagerCallback(env, info) : nullptr;
}

static napi_value MakeMirror(napi_env env, napi_callback_info info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(env, info);
    return (me != nullptr) ? me->OnMakeMirror(env, info) : nullptr;
}

static napi_value MakeExpand(napi_env env, napi_callback_info info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(env, info);
    return (me != nullptr) ? me->OnMakeExpand(env, info) : nullptr;
}

static napi_value StopMirror(napi_env env, napi_callback_info info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(env, info);
    return (me != nullptr) ? me->OnStopMirror(env, info) : nullptr;
}

static napi_value StopExpand(napi_env env, napi_callback_info info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(env, info);
    return (me != nullptr) ? me->OnStopExpand(env, info) : nullptr;
}

static napi_value CreateVirtualScreen(napi_env env, napi_callback_info info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(env, info);
    return (me != nullptr) ? me->OnCreateVirtualScreen(env, info) : nullptr;
}

static napi_value DestroyVirtualScreen(napi_env env, napi_callback_info info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(env, info);
    return (me != nullptr) ? me->OnDestroyVirtualScreen(env, info) : nullptr;
}

static napi_value SetVirtualScreenSurface(napi_env env, napi_callback_info info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(env, info);
    return (me != nullptr) ? me->OnSetVirtualScreenSurface(env, info) : nullptr;
}

static napi_value IsScreenRotationLocked(napi_env env, napi_callback_info info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(env, info);
    return (me != nullptr) ? me->OnIsScreenRotationLocked(env, info) : nullptr;
}

static napi_value SetScreenRotationLocked(napi_env env, napi_callback_info info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(env, info);
    return (me != nullptr) ? me->OnSetScreenRotationLocked(env, info) : nullptr;
}
private:
std::map<std::string, std::map<std::unique_ptr<NativeReference>, sptr<JsScreenListener>>> jsCbMap_;
std::mutex mtx_;

napi_value OnGetAllScreens(napi_env env, napi_callback_info info)
{
    WLOGI("OnGetAllScreens is called");
    NapiAsyncTask::CompleteCallback complete =
        [this](napi_env env, NapiAsyncTask& task, int32_t status) {
            std::vector<sptr<Screen>> screens;
            auto res = DM_JS_TO_ERROR_CODE_MAP.at(SingletonContainer::Get<ScreenManager>().GetAllScreens(screens));
            if (res != DmErrorCode::DM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(res),
                    "JsScreenManager::OnGetAllScreens failed."));
            } else if (!screens.empty()) {
                task.Resolve(env, CreateJsScreenVectorObject(env, screens));
                WLOGI("JsScreenManager::OnGetAllScreens success");
            } else {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN),
                    "JsScreenManager::OnGetAllScreens failed."));
            }
        };
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value lastParam = nullptr;
    if (argc >= ARGC_ONE && argv[ARGC_ONE - 1] != nullptr &&
        GetType(env, argv[ARGC_ONE - 1]) == napi_function) {
        lastParam = argv[ARGC_ONE - 1];
    }
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsScreenManager::OnGetAllScreens",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value CreateJsScreenVectorObject(napi_env env, std::vector<sptr<Screen>>& screens)
{
    napi_value arrayValue = nullptr;
    napi_create_array_with_length(env, screens.size(), &arrayValue);
    if (arrayValue == nullptr) {
        WLOGFE("Failed to get screens");
        return NapiGetUndefined(env);
    }
    size_t i = 0;
    for (auto& screen : screens) {
        if (screen == nullptr) {
            continue;
        }
        napi_set_element(env, arrayValue, i++, CreateJsScreenObject(env, screen));
    }
    return arrayValue;
}

bool IfCallbackRegistered(napi_env env, const std::string& type, napi_value jsListenerObject)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGI("JsScreenManager::IfCallbackRegistered methodName %{public}s not registered!", type.c_str());
        return false;
    }

    for (auto& iter : jsCbMap_[type]) {
        bool isEquals = false;
        napi_strict_equals(env, jsListenerObject, iter.first->GetNapiValue(), &isEquals);
        if (isEquals) {
            WLOGFE("JsScreenManager::IfCallbackRegistered callback already registered!");
            return true;
        }
    }
    return false;
}

DmErrorCode RegisterScreenListenerWithType(napi_env env, const std::string& type, napi_value value)
{
    if (IfCallbackRegistered(env, type, value)) {
        WLOGFE("JsScreenManager::RegisterScreenListenerWithType callback already registered!");
        return DmErrorCode::DM_ERROR_INVALID_CALLING;
    }
    std::unique_ptr<NativeReference> callbackRef;
    napi_ref result = nullptr;
    napi_create_reference(env, value, 1, &result);
    callbackRef.reset(reinterpret_cast<NativeReference*>(result));
    sptr<JsScreenListener> screenListener = new(std::nothrow) JsScreenListener(env);
    if (screenListener == nullptr) {
        WLOGFE("screenListener is nullptr");
        return DmErrorCode::DM_ERROR_INVALID_SCREEN;
    }
    if (type == EVENT_CONNECT || type == EVENT_DISCONNECT || type == EVENT_CHANGE) {
        DmErrorCode ret = DM_JS_TO_ERROR_CODE_MAP.at(
            SingletonContainer::Get<ScreenManager>().RegisterScreenListener(screenListener));
        if (ret != DmErrorCode::DM_OK) {
            return ret;
        }
        WLOGI("JsScreenManager::RegisterScreenListenerWithType success");
    } else {
        WLOGFE("JsScreenManager::RegisterScreenListenerWithType failed method: %{public}s not support!",
            type.c_str());
        return DmErrorCode::DM_ERROR_INVALID_CALLING;
    }
    screenListener->AddCallback(type, value);
    jsCbMap_[type][std::move(callbackRef)] = screenListener;
    return DmErrorCode::DM_OK;
}

DmErrorCode UnregisterAllScreenListenerWithType(const std::string& type)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGFE("JsScreenManager::UnregisterAllScreenListenerWithType methodName %{public}s not registered!",
            type.c_str());
        return DmErrorCode::DM_ERROR_INVALID_CALLING;
    }
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
        it->second->RemoveAllCallback();
        if (type == EVENT_CONNECT || type == EVENT_DISCONNECT || type == EVENT_CHANGE) {
            sptr<ScreenManager::IScreenListener> thisListener(it->second);
            SingletonContainer::Get<ScreenManager>().UnregisterScreenListener(thisListener);
            WLOGI("JsScreenManager::UnregisterAllScreenListenerWithType success");
        }
        jsCbMap_[type].erase(it++);
    }
    jsCbMap_.erase(type);
    return DmErrorCode::DM_OK;
}

DmErrorCode UnRegisterScreenListenerWithType(napi_env env, const std::string& type, napi_value value)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGI("JsScreenManager::UnRegisterScreenListenerWithType methodName %{public}s not registered!",
            type.c_str());
        return DmErrorCode::DM_ERROR_INVALID_CALLING;
    }
    if (value == nullptr) {
        WLOGFE("JsScreenManager::UnRegisterScreenListenerWithType value is nullptr");
        return DmErrorCode::DM_ERROR_INVALID_SCREEN;
    }
    DmErrorCode ret = DmErrorCode::DM_OK;
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
        bool isEquals = false;
        napi_strict_equals(env, value, it->first->GetNapiValue(), &isEquals);
        if (isEquals) {
            it->second->RemoveCallback(type, value);
            if (type == EVENT_CONNECT || type == EVENT_DISCONNECT || type == EVENT_CHANGE) {
                sptr<ScreenManager::IScreenListener> thisListener(it->second);
                ret = DM_JS_TO_ERROR_CODE_MAP.at(
                    SingletonContainer::Get<ScreenManager>().UnregisterScreenListener(thisListener));
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
    if (ret == DmErrorCode::DM_OK) {
        WLOGFI("JsScreenManager::UnRegisterScreenListenerWithType success");
    }
    return ret;
}

bool NapiIsCallable(napi_env env, napi_value value)
{
    bool result = false;
    napi_is_callable(env, value, &result);
    return result;
}

static napi_value NapiThrowInvalidParam(napi_env env)
{
    napi_throw(env, CreateJsError(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
    return NapiGetUndefined(env);
}

napi_value OnRegisterScreenManagerCallback(napi_env env, napi_callback_info info)
{
    WLOGI("JsScreenManager::OnRegisterScreenManagerCallback is called");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_TWO) {
        WLOGFE("Params not match");
        return NapiThrowInvalidParam(env);
    }
    std::string cbType;
    if (!ConvertFromJsValue(env, argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        return NapiThrowInvalidParam(env);
    }
    napi_value value = argv[INDEX_ONE];
    if (value == nullptr) {
        WLOGI("JsScreenManager::OnRegisterScreenManagerCallback argv[1] is nullptr");
        return NapiThrowInvalidParam(env);
    }
    if (!NapiIsCallable(env, value)) {
        WLOGI("JsScreenManager::OnRegisterScreenManagerCallback argv[1] is not callable");
        return NapiThrowInvalidParam(env);
    }
    std::lock_guard<std::mutex> lock(mtx_);
    DmErrorCode ret = RegisterScreenListenerWithType(env, cbType, value);
    if (ret != DmErrorCode::DM_OK) {
        WLOGFE("JsScreenManager::OnRegisterScreenManagerCallback failed");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(ret)));
    }
    return NapiGetUndefined(env);
}

napi_value OnUnregisterScreenManagerCallback(napi_env env, napi_callback_info info)
{
    WLOGI("JsScreenManager::OnUnregisterScreenCallback is called");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        WLOGFE("Params not match");
        return NapiThrowInvalidParam(env);
    }
    std::string cbType;
    if (!ConvertFromJsValue(env, argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        return NapiThrowInvalidParam(env);
    }
    std::lock_guard<std::mutex> lock(mtx_);
    if (argc == ARGC_ONE) {
        UnregisterAllScreenListenerWithType(cbType);
    } else {
        napi_value value = argv[INDEX_ONE];
        if ((value == nullptr) || (!NapiIsCallable(env, value))) {
            DmErrorCode ret = UnregisterAllScreenListenerWithType(cbType);
            if (ret != DmErrorCode::DM_OK) {
                WLOGFE("JsScreenManager::OnUnRegisterAllScreenManagerCallback failed");
                napi_throw(env, CreateJsError(env, static_cast<int32_t>(ret)));
            }
        } else {
            DmErrorCode ret = UnRegisterScreenListenerWithType(env, cbType, value);
            if (ret != DmErrorCode::DM_OK) {
                WLOGFE("JsScreenManager::OnUnRegisterScreenManagerCallback failed");
                napi_throw(env, CreateJsError(env, static_cast<int32_t>(ret)));
            }
        }
    }
    return NapiGetUndefined(env);
}

napi_value OnMakeMirror(napi_env env, napi_callback_info info)
{
    WLOGI("OnMakeMirror is called");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_TWO) {
        return NapiThrowInvalidParam(env);
    }

    int64_t mainScreenId;
    if (!ConvertFromJsValue(env, argv[0], mainScreenId)) {
        return NapiThrowInvalidParam(env);
    }
    napi_value array = argv[INDEX_ONE];
    if (array == nullptr) {
        return NapiThrowInvalidParam(env);
    }
    uint32_t size = 0;
    napi_get_array_length(env, array, &size);
    std::vector<ScreenId> screenIds;
    for (uint32_t i = 0; i < size; i++) {
        uint32_t screenId;
        napi_value value = nullptr;
        napi_get_element(env, array, i, &value);
        if (!ConvertFromJsValue(env, value, screenId)) {
            return NapiThrowInvalidParam(env);
        }
        screenIds.emplace_back(static_cast<ScreenId>(screenId));
    }

    NapiAsyncTask::CompleteCallback complete =
        [mainScreenId, screenIds](napi_env env, NapiAsyncTask& task, int32_t status) {
            ScreenId screenGroupId = INVALID_SCREEN_ID;
            DmErrorCode ret = DM_JS_TO_ERROR_CODE_MAP.at(
                SingletonContainer::Get<ScreenManager>().MakeMirror(mainScreenId, screenIds, screenGroupId));
            if (ret == DmErrorCode::DM_OK) {
                task.Resolve(env, CreateJsValue(env, static_cast<uint32_t>(screenGroupId)));
            } else {
                task.Reject(env, 
                    CreateJsError(env, static_cast<int32_t>(ret), "JsScreenManager::OnMakeMirror failed."));
            }
        };
    napi_value lastParam = nullptr;
    if (argc >= ARGC_THREE && argv[ARGC_THREE - 1] != nullptr &&
        GetType(env, argv[ARGC_THREE - 1]) == napi_function) {
        lastParam = argv[ARGC_THREE - 1];
    }
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsScreenManager::OnMakeMirror",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value OnMakeExpand(napi_env env, napi_callback_info info)
{
    WLOGI("OnMakeExpand is called");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        WLOGFE("Params not match");
        return NapiThrowInvalidParam(env);
    }

    napi_value array = argv[0];
    if (array == nullptr) {
        WLOGFE("Failed to get options");
        return NapiThrowInvalidParam(env);
    }
    uint32_t size = 0;
    napi_get_array_length(env, array, &size);
    std::vector<ExpandOption> options;
    for (uint32_t i = 0; i < size; ++i) {
        napi_value object = nullptr;
        napi_get_element(env, array, i, &object);
        ExpandOption expandOption;
        int32_t res = GetExpandOptionFromJs(env, object, expandOption);
        if (res == -1) {
            WLOGE("expandoption param %{public}d error!", i);
            return NapiThrowInvalidParam(env);
        }
        options.emplace_back(expandOption);
    }

    NapiAsyncTask::CompleteCallback complete =
        [options](napi_env env, NapiAsyncTask& task, int32_t status) {
            ScreenId screenGroupId = INVALID_SCREEN_ID;
            DmErrorCode ret = DM_JS_TO_ERROR_CODE_MAP.at(
                SingletonContainer::Get<ScreenManager>().MakeExpand(options, screenGroupId));
            if (ret == DmErrorCode::DM_OK) {
                task.Resolve(env, CreateJsValue(env, static_cast<uint32_t>(screenGroupId)));
                WLOGI("MakeExpand success");
            } else {
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(ret), "JsScreenManager::OnMakeExpand failed."));
                WLOGFE("MakeExpand failed");
            }
        };
    napi_value lastParam = nullptr;
    if (argc >= ARGC_TWO && argv[ARGC_TWO - 1] != nullptr && GetType(env, argv[ARGC_TWO - 1]) == napi_function) {
        lastParam = argv[ARGC_TWO - 1];
    }
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsScreenManager::OnMakeExpand",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value OnStopMirror(napi_env env, napi_callback_info info)
{
    WLOGI("OnStopMirror is called");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        return NapiThrowInvalidParam(env);
    }

    napi_value array = argv[0];
    if (array == nullptr) {
        return NapiThrowInvalidParam(env);
    }
    uint32_t size = 0;
    napi_get_array_length(env, array, &size);
    if (size > MAX_SCREENS_NUM) {
        return NapiThrowInvalidParam(env);
    }
    std::vector<ScreenId> screenIds;
    for (uint32_t i = 0; i < size; i++) {
        uint32_t screenId;
        napi_value value = nullptr;
        napi_get_element(env, array, i, &value);
        if (!ConvertFromJsValue(env, value, screenId)) {
            return NapiThrowInvalidParam(env);
        }
        screenIds.emplace_back(static_cast<ScreenId>(screenId));
    }

    NapiAsyncTask::CompleteCallback complete =
        [ screenIds](napi_env env, NapiAsyncTask& task, int32_t status) {
            DmErrorCode ret = DM_JS_TO_ERROR_CODE_MAP.at(
                SingletonContainer::Get<ScreenManager>().StopMirror(screenIds));
            if (ret == DmErrorCode::DM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(ret),"JsScreenManager::OnStopMirror failed."));
            }
        };
    napi_value lastParam = nullptr;
    if (argc >= ARGC_TWO && argv[ARGC_TWO - 1] != nullptr && GetType(env, argv[ARGC_TWO - 1]) == napi_function) {
        lastParam = argv[ARGC_TWO - 1];
    }
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsScreenManager::OnStopMirror",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value OnStopExpand(napi_env env, napi_callback_info info)
{
    WLOGI("OnStopExpand is called");
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        WLOGFE("Params not match");
        return NapiThrowInvalidParam(env);
    }

    napi_value array = argv[0];
    if (array == nullptr) {
        return NapiThrowInvalidParam(env);
    }
    uint32_t size = 0;
    napi_get_array_length(env, array, &size);
    if (size > MAX_SCREENS_NUM) {
        return NapiThrowInvalidParam(env);
    }
    std::vector<ScreenId> screenIds;
    for (uint32_t i = 0; i < size; i++) {
        uint32_t screenId;
        napi_value value = nullptr;
        napi_get_element(env, array, i, &value);
        if (!ConvertFromJsValue(env, value, screenId)) {
            return NapiThrowInvalidParam(env);
        }
        screenIds.emplace_back(static_cast<ScreenId>(screenId));
    }

    NapiAsyncTask::CompleteCallback complete =
        [screenIds](napi_env env, NapiAsyncTask& task, int32_t status) {
            DmErrorCode ret = DM_JS_TO_ERROR_CODE_MAP.at(
                SingletonContainer::Get<ScreenManager>().StopExpand(screenIds));
            if (ret == DmErrorCode::DM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
                WLOGI("MakeExpand success");
            } else {
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(ret), "JsScreenManager::OnStopExpand failed."));
                WLOGFE("MakeExpand failed");
            }
        };
    napi_value lastParam = nullptr;
    if (argc >= ARGC_TWO && argv[ARGC_TWO - 1] != nullptr && GetType(env, argv[ARGC_TWO - 1]) == napi_function) {
        lastParam = argv[ARGC_TWO - 1];
    }
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsScreenManager::OnStopExpand",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

static int32_t GetExpandOptionFromJs(napi_env env, napi_value optionObject, ExpandOption& option)
{
    napi_value screedIdValue = nullptr;
    napi_value startXValue = nullptr;
    napi_value startYValue = nullptr;
    uint32_t screenId;
    uint32_t startX;
    uint32_t startY;
    napi_get_named_property(env, optionObject, "screenId", &screedIdValue);
    napi_get_named_property(env, optionObject, "startX", &startXValue);
    napi_get_named_property(env, optionObject, "startY", &startYValue);
    if (!ConvertFromJsValue(env, screedIdValue, screenId)) {
        WLOGFE("Failed to convert screedIdValue to callbackType");
        return -1;
    }
    if (!ConvertFromJsValue(env, startXValue, startX)) {
        WLOGFE("Failed to convert startXValue to callbackType");
        return -1;
    }
    if (!ConvertFromJsValue(env, startYValue, startY)) {
        WLOGFE("Failed to convert startYValue to callbackType");
        return -1;
    }
    option = {screenId, startX, startY};
    return 0;
}

napi_value OnCreateVirtualScreen(napi_env env, napi_callback_info info)
{
    WLOGI("JsScreenManager::OnCreateVirtualScreen is called");
    DmErrorCode errCode = DmErrorCode::DM_OK;
    VirtualScreenOption option;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
    } else {
        napi_value object = argv[0];
        if (object == nullptr) {
            WLOGFE("Failed to convert parameter to VirtualScreenOption.");
            errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
        } else {
            errCode = GetVirtualScreenOptionFromJs(env, object, option);
        }
    }
    if (errCode == DmErrorCode::DM_ERROR_INVALID_PARAM) {
        return NapiThrowInvalidParam(env);
    }
    NapiAsyncTask::CompleteCallback complete =
        [option](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto screenId = SingletonContainer::Get<ScreenManager>().CreateVirtualScreen(option);
            auto screen = SingletonContainer::Get<ScreenManager>().GetScreenById(screenId);
            if (screen == nullptr) {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN),
                    "ScreenManager::CreateVirtualScreen failed."));
                WLOGFE("ScreenManager::CreateVirtualScreen failed.");
                return;
            }
            task.Resolve(env, CreateJsScreenObject(env, screen));
            WLOGI("JsScreenManager::OnCreateVirtualScreen success");
        };
    napi_value lastParam = nullptr;
    if (argc >= ARGC_TWO && argv[ARGC_TWO - 1] != nullptr &&
        GetType(env, argv[ARGC_TWO - 1]) == napi_function) {
        lastParam = argv[ARGC_TWO - 1];
    }
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsScreenManager::OnCreateVirtualScreen",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

DmErrorCode GetVirtualScreenOptionFromJs(napi_env env, napi_value optionObject, VirtualScreenOption& option)
{
    napi_value name = nullptr;
    napi_get_named_property(env, optionObject, "name", &name);
    if (!ConvertFromJsValue(env, name, option.name_)) {
        WLOGFE("Failed to convert parameter to name.");
        return DmErrorCode::DM_ERROR_INVALID_PARAM;
    }
    napi_value width = nullptr;
    napi_get_named_property(env, optionObject, "width", &width);
    if (!ConvertFromJsValue(env, width, option.width_)) {
        WLOGFE("Failed to convert parameter to width.");
        return DmErrorCode::DM_ERROR_INVALID_PARAM;
    }
    napi_value height = nullptr;
    napi_get_named_property(env, optionObject, "height", &height);
    if (!ConvertFromJsValue(env, height, option.height_)) {
        WLOGFE("Failed to convert parameter to height.");
        return DmErrorCode::DM_ERROR_INVALID_PARAM;
    }
    napi_value density = nullptr;
    napi_get_named_property(env, optionObject, "density", &density);
    double densityValue;
    if (!ConvertFromJsValue(env, density, densityValue)) {
        WLOGFE("Failed to convert parameter to density.");
        return DmErrorCode::DM_ERROR_INVALID_PARAM;
    }
    option.density_ = static_cast<float>(densityValue);

    napi_value surfaceIdNativeValue = nullptr;
    napi_get_named_property(env, optionObject, "surfaceId", &surfaceIdNativeValue);
    if (!GetSurfaceFromJs(env, surfaceIdNativeValue, option.surface_)) {
        return DmErrorCode::DM_ERROR_INVALID_PARAM;
    }
    return DmErrorCode::DM_OK;
}

bool GetSurfaceFromJs(napi_env env, napi_value surfaceIdNativeValue, sptr<Surface>& surface)
{
    if (surfaceIdNativeValue == nullptr || GetType(env, surfaceIdNativeValue) != napi_string) {
        WLOGFE("Failed to convert parameter to surface. Invalidate params.");
        return false;
    }

    char buffer[PATH_MAX];
    size_t length = 0;
    uint64_t surfaceId = 0;
    if (napi_get_value_string_utf8(env, surfaceIdNativeValue, buffer, PATH_MAX, &length) != napi_ok) {
        WLOGFE("Failed to convert parameter to surface.");
        return false;
    }
    std::istringstream inputStream(buffer);
    inputStream >> surfaceId;
    surface = SurfaceUtils::GetInstance()->GetSurface(surfaceId);
    if (surface == nullptr) {
        WLOGI("GetSurfaceFromJs failed, surfaceId:%{public}" PRIu64"", surfaceId);
    }
    return true;
}

napi_value OnDestroyVirtualScreen(napi_env env, napi_callback_info info)
{
    WLOGI("JsScreenManager::OnDestroyVirtualScreen is called");
    DmErrorCode errCode = DmErrorCode::DM_OK;
    int64_t screenId = -1LL;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
    } else {
        if (!ConvertFromJsValue(env, argv[0], screenId)) {
            WLOGFE("Failed to convert parameter to screen id.");
            errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
        }
    }
    if (errCode == DmErrorCode::DM_ERROR_INVALID_PARAM || screenId == -1LL) {
        WLOGFE("JsScreenManager::OnDestroyVirtualScreen failed, Invalidate params.");
        return NapiThrowInvalidParam(env);
    }
    NapiAsyncTask::CompleteCallback complete =
        [screenId](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto res = DM_JS_TO_ERROR_CODE_MAP.at(
                SingletonContainer::Get<ScreenManager>().DestroyVirtualScreen(screenId));
            if (res != DmErrorCode::DM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(res),
                    "ScreenManager::DestroyVirtualScreen failed."));
                WLOGFE("ScreenManager::DestroyVirtualScreen failed.");
                return;
            }
            task.Resolve(env, NapiGetUndefined(env));
            WLOGI("JsScreenManager::OnDestroyVirtualScreen success");
        };
    napi_value lastParam = nullptr;
    if (argc >= ARGC_TWO && argv[ARGC_TWO - 1] != nullptr &&
        GetType(env, argv[ARGC_TWO - 1]) == napi_function) {
        lastParam = argv[ARGC_TWO - 1];
    }
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsScreenManager::OnDestroyVirtualScreen",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value OnSetVirtualScreenSurface(napi_env env, napi_callback_info info)
{
    WLOGI("JsScreenManager::OnSetVirtualScreenSurface is called");
    DmErrorCode errCode = DmErrorCode::DM_OK;
    int64_t screenId = -1LL;
    sptr<Surface> surface;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_TWO) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
    } else {
        if (!ConvertFromJsValue(env, argv[0], screenId)) {
            WLOGFE("Failed to convert parameter to screen id.");
            errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
        }
        if (!GetSurfaceFromJs(env, argv[1], surface)) {
            WLOGFE("Failed to convert parameter to surface");
            errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
        }
    }
    if (errCode == DmErrorCode::DM_ERROR_INVALID_PARAM || surface == nullptr) {
        WLOGFE("JsScreenManager::OnSetVirtualScreenSurface failed, Invalidate params.");
        return NapiThrowInvalidParam(env);
    }
    NapiAsyncTask::CompleteCallback complete =
        [screenId, surface](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto res = DM_JS_TO_ERROR_CODE_MAP.at(
                SingletonContainer::Get<ScreenManager>().SetVirtualScreenSurface(screenId, surface));
            if (res != DmErrorCode::DM_OK) {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(res),
                    "ScreenManager::SetVirtualScreenSurface failed."));
                WLOGFE("ScreenManager::SetVirtualScreenSurface failed.");
                return;
            }
            task.Resolve(env, NapiGetUndefined(env));
            WLOGI("JsScreenManager::OnSetVirtualScreenSurface success");
        };
    napi_value lastParam = nullptr;
    if (argc >= ARGC_THREE && argv[ARGC_THREE - 1] != nullptr &&
        GetType(env, argv[ARGC_THREE - 1]) == napi_function) {
        lastParam = argv[ARGC_THREE - 1];
    }
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsScreenManager::OnSetVirtualScreenSurface",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value OnIsScreenRotationLocked(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    WLOGI("OnIsScreenRotationLocked is called");
    NapiAsyncTask::CompleteCallback complete =
        [](napi_env env, NapiAsyncTask& task, int32_t status) {
            bool isLocked = false;
            auto res = DM_JS_TO_ERROR_CODE_MAP.at(
                SingletonContainer::Get<ScreenManager>().IsScreenRotationLocked(isLocked));
            if (res == DmErrorCode::DM_OK) {
                task.Resolve(env, CreateJsValue(env, isLocked));
                WLOGFI("OnIsScreenRotationLocked success");
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(res),
                                                "JsScreenManager::OnIsScreenRotationLocked failed."));
                WLOGFE("OnIsScreenRotationLocked failed");
            }
        };
    napi_value lastParam = nullptr;
    if (argc >= ARGC_ONE && argv[ARGC_ONE - 1] != nullptr &&
        GetType(env, argv[ARGC_ONE - 1]) == napi_function) {
        lastParam = argv[ARGC_ONE - 1];
    }
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsScreenManager::OnIsScreenRotationLocked",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value OnSetScreenRotationLocked(napi_env env, napi_callback_info info)
{
    WLOGI("JsScreenManager::OnSetScreenRotationLocked is called");
    DmErrorCode errCode = DmErrorCode::DM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < ARGC_ONE) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", argc);
        errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
    }
    bool isLocked = false;
    if (errCode == DmErrorCode::DM_OK) {
        napi_value nativeVal = argv[0];
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to isLocked");
            errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
        } else {
            napi_get_value_bool(env, nativeVal, &isLocked);
        }
    }
    if (errCode == DmErrorCode::DM_ERROR_INVALID_PARAM) {
        WLOGFE("JsScreenManager::OnSetScreenRotationLocked failed, Invalidate params.");
        return NapiThrowInvalidParam(env);
    }

    NapiAsyncTask::CompleteCallback complete =
        [isLocked](napi_env env, NapiAsyncTask& task, int32_t status) {
            auto res = DM_JS_TO_ERROR_CODE_MAP.at(
                SingletonContainer::Get<ScreenManager>().SetScreenRotationLocked(isLocked));
            if (res == DmErrorCode::DM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
                WLOGFI("OnSetScreenRotationLocked success");
            } else {
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(res),
                                                  "JsScreenManager::OnSetScreenRotationLocked failed."));
                WLOGFE("OnSetScreenRotationLocked failed");
            }
        };
    napi_value lastParam = (argc <= ARGC_ONE) ? nullptr :
        ((argv[ARGC_TWO - 1] != nullptr && GetType(env, argv[ARGC_TWO - 1]) == napi_function) ?
        argv[1] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsScreenManager::OnSetScreenRotationLocked",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}
};

napi_value InitScreenOrientation(napi_env env)
{
    WLOGI("JsScreenManager::InitScreenOrientation called");

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

    napi_set_named_property(env, objValue, "UNSPECIFIED",
        CreateJsValue(env, static_cast<int32_t>(Orientation::UNSPECIFIED)));
    napi_set_named_property(env, objValue, "VERTICAL",
        CreateJsValue(env, static_cast<int32_t>(Orientation::VERTICAL)));
    napi_set_named_property(env, objValue, "HORIZONTAL",
        CreateJsValue(env, static_cast<int32_t>(Orientation::HORIZONTAL)));
    napi_set_named_property(env, objValue, "REVERSE_VERTICAL",
        CreateJsValue(env, static_cast<int32_t>(Orientation::REVERSE_VERTICAL)));
    napi_set_named_property(env, objValue, "REVERSE_HORIZONTAL",
        CreateJsValue(env, static_cast<int32_t>(Orientation::REVERSE_HORIZONTAL)));
    return objValue;
}

napi_value InitScreenSourceMode(napi_env env)
{
    WLOGI("JsScreenManager::InitScreenSourceMode called");

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

    napi_set_named_property(env, objValue, "SCREEN_MAIN",
        CreateJsValue(env, static_cast<uint32_t>(ScreenSourceMode::SCREEN_MAIN)));
    napi_set_named_property(env, objValue, "SCREEN_MIRROR",
        CreateJsValue(env, static_cast<uint32_t>(ScreenSourceMode::SCREEN_MIRROR)));
    napi_set_named_property(env, objValue, "SCREEN_EXTEND",
        CreateJsValue(env, static_cast<uint32_t>(ScreenSourceMode::SCREEN_EXTEND)));
    napi_set_named_property(env, objValue, "SCREEN_ALONE",
        CreateJsValue(env, static_cast<uint32_t>(ScreenSourceMode::SCREEN_ALONE)));
    return objValue;
}

napi_value InitDisplayErrorCode(napi_env env)
{
    WLOGI("JsDisplayManager::InitDisplayErrorCode called");

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
    napi_set_named_property(env, objValue, "DM_ERROR_NOT_SYSTEM_APP",
        CreateJsValue(env, static_cast<int32_t>(DmErrorCode::DM_ERROR_NOT_SYSTEM_APP)));
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
    WLOGI("JsDisplayManager::InitDisplayError called");

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

napi_value JsScreenManagerInit(napi_env env, napi_value exportObj)
{
    WLOGI("JsScreenManagerInit is called");

    if (env == nullptr || exportObj == nullptr) {
        WLOGFE("JsScreenManagerInit env or exportObj is nullptr");
        return nullptr;
    }

    std::unique_ptr<JsScreenManager> jsScreenManager = std::make_unique<JsScreenManager>(env);
    napi_wrap(env, exportObj, jsScreenManager.release(), JsScreenManager::Finalizer, nullptr, nullptr);
    napi_set_named_property(env, exportObj, "Orientation", InitScreenOrientation(env));
    napi_set_named_property(env, exportObj, "ScreenSourceMode", InitScreenSourceMode(env));
    napi_set_named_property(env, exportObj, "DmErrorCode", InitDisplayErrorCode(env));
    napi_set_named_property(env, exportObj, "DMError", InitDisplayError(env));

    const char *moduleName = "JsScreenManager";
    BindNativeFunction(env, exportObj, "getAllScreens", moduleName, JsScreenManager::GetAllScreens);
    BindNativeFunction(env, exportObj, "on", moduleName, JsScreenManager::RegisterScreenManagerCallback);
    BindNativeFunction(env, exportObj, "off", moduleName, JsScreenManager::UnregisterScreenMangerCallback);
    BindNativeFunction(env, exportObj, "makeMirror", moduleName, JsScreenManager::MakeMirror);
    BindNativeFunction(env, exportObj, "makeExpand", moduleName, JsScreenManager::MakeExpand);
    BindNativeFunction(env, exportObj, "stopMirror", moduleName, JsScreenManager::StopMirror);
    BindNativeFunction(env, exportObj, "stopExpand", moduleName, JsScreenManager::StopExpand);
    BindNativeFunction(env, exportObj, "createVirtualScreen", moduleName, JsScreenManager::CreateVirtualScreen);
    BindNativeFunction(env, exportObj, "destroyVirtualScreen", moduleName, JsScreenManager::DestroyVirtualScreen);
    BindNativeFunction(env, exportObj, "setVirtualScreenSurface", moduleName,
        JsScreenManager::SetVirtualScreenSurface);
    BindNativeFunction(env, exportObj, "setScreenRotationLocked", moduleName,
        JsScreenManager::SetScreenRotationLocked);
    BindNativeFunction(env, exportObj, "isScreenRotationLocked", moduleName,
        JsScreenManager::IsScreenRotationLocked);
    return NapiGetUndefined(env);
}
}  // namespace Rosen
}  // namespace OHOS
