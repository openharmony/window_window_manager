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
#include "js_window_register_manager.h"
#include "singleton_container.h"
#include "window_manager.h"
#include "window_manager_hilog.h"
#include "js_runtime_utils.h"
namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsRegisterManager"};

    enum class REGISTER_LISTENER_TYPE : uint32_t {
        SYSTEM_BAR_TINT_CHANGE_CB,
        GESTURE_NAVIGATION_ENABLED_CHANGE_CB,
        WATER_MARK_FLAG_CHANGE_CB,
        WINDOW_SIZE_CHANGE_CB,
        SYSTEM_AVOID_AREA_CHANGE_CB,
        AVOID_AREA_CHANGE_CB,
        LIFECYCLE_EVENT_CB,
        WINDOW_EVENT_CB,
        KEYBOARD_HEIGHT_CHANGE_CB,
        TOUCH_OUTSIDE_CB,
        SCREENSHOT_EVENT_CB,
        DIALOG_TARGET_TOUCH_CB,
        DIALOG_DEATH_RECIPIENT_CB,
        WINDOW_STATUS_CHANGE_CB,
        WINDOW_TITLE_BUTTON_RECT_CHANGE_CB,
        WINDOW_VISIBILITY_CHANGE_CB,
        WINDOW_NO_INTERACTION_DETECT_CB,
        WINDOW_RECT_CHANGE_CB,
        SUB_WINDOW_CLOSE_CB,
        WINDOW_STAGE_EVENT_CB,
    };

    const std::map<std::string, RegisterListenerType> WINDOW_MANAGER_LISTENER_MAP{
        // white register list for window manager
        {SYSTEM_BAR_TINT_CHANGE_CB, RegisterListenerType::SYSTEM_BAR_TINT_CHANGE_CB},
        {GESTURE_NAVIGATION_ENABLED_CHANGE_CB, RegisterListenerType::GESTURE_NAVIGATION_ENABLED_CHANGE_CB},
        {WATER_MARK_FLAG_CHANGE_CB, RegisterListenerType::WATER_MARK_FLAG_CHANGE_CB},
    };
    const std::map<std::string, RegisterListenerType> WINDOW_LISTENER_MAP{
        // white register list for window
        {WINDOW_SIZE_CHANGE_CB, RegisterListenerType::WINDOW_SIZE_CHANGE_CB},
        {SYSTEM_AVOID_AREA_CHANGE_CB, RegisterListenerType::SYSTEM_AVOID_AREA_CHANGE_CB},
        {AVOID_AREA_CHANGE_CB, RegisterListenerType::AVOID_AREA_CHANGE_CB},
        {LIFECYCLE_EVENT_CB, RegisterListenerType::LIFECYCLE_EVENT_CB},
        {WINDOW_EVENT_CB, RegisterListenerType::WINDOW_EVENT_CB},
        {KEYBOARD_HEIGHT_CHANGE_CB, RegisterListenerType::KEYBOARD_HEIGHT_CHANGE_CB},
        {TOUCH_OUTSIDE_CB, RegisterListenerType::TOUCH_OUTSIDE_CB},
        {SCREENSHOT_EVENT_CB, RegisterListenerType::SCREENSHOT_EVENT_CB},
        {DIALOG_TARGET_TOUCH_CB, RegisterListenerType::DIALOG_TARGET_TOUCH_CB},
        {DIALOG_DEATH_RECIPIENT_CB, RegisterListenerType::DIALOG_DEATH_RECIPIENT_CB},
        {WINDOW_STATUS_CHANGE_CB, RegisterListenerType::WINDOW_STATUS_CHANGE_CB},
        {WINDOW_TITLE_BUTTON_RECT_CHANGE_CB, RegisterListenerType::WINDOW_TITLE_BUTTON_RECT_CHANGE_CB},
        {WINDOW_VISIBILITY_CHANGE_CB, RegisterListenerType::WINDOW_VISIBILITY_CHANGE_CB},
        {WINDOW_NO_INTERACTION_DETECT_CB, RegisterListenerType::WINDOW_NO_INTERACTION_DETECT_CB},
        {WINDOW_RECT_CHANGE_CB, RegisterListenerType::WINDOW_RECT_CHANGE_CB},
        {SUB_WINDOW_CLOSE_CB, RegisterListenerType::SUB_WINDOW_CLOSE_CB},
    };
    const std::map<std::string, RegisterListenerType> WINDOW_STAGE_LISTENER_MAP{
        // white register list for window stage
        {WINDOW_STAGE_EVENT_CB, RegisterListenerType::WINDOW_STAGE_EVENT_CB},
    };

    const std::map<CaseType, std::map<std::string, RegisterListenerType>> listenerCodeMap_{
        {CaseType::CASE_WINDOW_MANAGER, WINDOW_MANAGER_LISTENER_MAP},
        {CaseType::CASE_WINDOW, WINDOW_LISTENER_MAP},
        {CaseType::CASE_STAGE, WINDOW_STAGE_LISTENER_MAP},
    };
}

JsWindowRegisterManager::JsWindowRegisterManager()
{
}

JsWindowRegisterManager::~JsWindowRegisterManager()
{
}

WmErrorCode JsWindowRegisterManager::ProcessWindowChangeRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, napi_value parameter)
{
    if (window == nullptr) {
        WLOGFE("[NAPI]Window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IWindowChangeListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterWindowChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterWindowChangeListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessSystemAvoidAreaChangeRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, napi_value parameter)
{
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "[NAPI]Window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "[NAPI]listener is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    listener->SetIsDeprecatedInterface(true);
    sptr<IAvoidAreaChangedListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterAvoidAreaChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterAvoidAreaChangeListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessAvoidAreaChangeRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, napi_value parameter)
{
    if (window == nullptr) {
        WLOGFE("[NAPI]Window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IAvoidAreaChangedListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterAvoidAreaChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterAvoidAreaChangeListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessLifeCycleEventRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, napi_value parameter)
{
    if (window == nullptr) {
        WLOGFE("[NAPI]Window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IWindowLifeCycle> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterLifeCycleListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterLifeCycleListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessOccupiedAreaChangeRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, napi_value parameter)
{
    if (window == nullptr) {
        WLOGFE("[NAPI]Window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IOccupiedAreaChangeListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterOccupiedAreaChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterOccupiedAreaChangeListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessSystemBarChangeRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, napi_value parameter)
{
    sptr<ISystemBarChangedListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(
            SingletonContainer::Get<WindowManager>().RegisterSystemBarChangedListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(
            SingletonContainer::Get<WindowManager>().UnregisterSystemBarChangedListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessGestureNavigationEnabledChangeRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, napi_value parameter)
{
    sptr<IGestureNavigationEnabledChangedListener> thisListener(listener);
    WmErrorCode ret;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(
            SingletonContainer::Get<WindowManager>().RegisterGestureNavigationEnabledChangedListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(
            SingletonContainer::Get<WindowManager>().UnregisterGestureNavigationEnabledChangedListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessWaterMarkFlagChangeRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, napi_value parameter)
{
    sptr<IWaterMarkFlagChangedListener> thisListener(listener);
    WmErrorCode ret;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(
            SingletonContainer::Get<WindowManager>().RegisterWaterMarkFlagChangedListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(
            SingletonContainer::Get<WindowManager>().UnregisterWaterMarkFlagChangedListener(thisListener));
    }
    return ret;
}
WmErrorCode JsWindowRegisterManager::ProcessTouchOutsideRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, napi_value parameter)
{
    WLOGI("called");
    if (window == nullptr) {
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<ITouchOutsideListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterTouchOutsideListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterTouchOutsideListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessWindowVisibilityChangeRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, napi_value parameter)
{
    WLOGD("called");
    if (window == nullptr || listener == nullptr) {
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IWindowVisibilityChangedListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterWindowVisibilityChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterWindowVisibilityChangeListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessWindowNoInteractionRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, napi_value parameter)
{
    if (window == nullptr) {
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IWindowNoInteractionListener> thisListener(listener);

    if (!isRegister) {
        return WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterWindowNoInteractionListener(thisListener));
    }

    int64_t timeout = 0;
    if (parameter == nullptr || !ConvertFromJsNumber(env, parameter, timeout)) {
        WLOGFE("Failed to convert parameter to timeout");
        return WmErrorCode::WM_ERROR_INVALID_PARAM;
    }

    constexpr int64_t secToMicrosecRatio = 1000;
    constexpr int64_t noInteractionMax = LLONG_MAX / secToMicrosecRatio;
    if (timeout <= 0 || (timeout > noInteractionMax)) {
        WLOGFE("invalid parameter: no-interaction-timeout %{public}" PRId64 " is not in(0s~%{public}" PRId64,
            timeout, noInteractionMax);
        return WmErrorCode::WM_ERROR_INVALID_PARAM;
    }

    thisListener->SetTimeout(timeout * secToMicrosecRatio);

    return WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterWindowNoInteractionListener(thisListener));
}

WmErrorCode JsWindowRegisterManager::ProcessScreenshotRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, napi_value parameter)
{
    WLOGI("called");
    if (window == nullptr) {
        WLOGFE("%{public}sregister screenshot listener failed. window is null", isRegister? "" : "un");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IScreenshotListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterScreenshotListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterScreenshotListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessDialogTargetTouchRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, napi_value parameter)
{
    if (window == nullptr) {
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IDialogTargetTouchListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterDialogTargetTouchListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterDialogTargetTouchListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessDialogDeathRecipientRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, napi_value parameter)
{
    if (window == nullptr) {
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IDialogDeathRecipientListener> thisListener(listener);
    if (isRegister) {
        window->RegisterDialogDeathRecipientListener(thisListener);
    } else {
        window->UnregisterDialogDeathRecipientListener(thisListener);
    }
    return WmErrorCode::WM_OK;
}

WmErrorCode JsWindowRegisterManager::ProcessWindowTitleButtonRectChangeRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, napi_value parameter)
{
    WLOGD("called");
    if (window == nullptr) {
        WLOGFE("[NAPI]Window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IWindowTitleButtonRectChangedListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterWindowTitleButtonRectChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterWindowTitleButtonRectChangeListener(thisListener));
    }
    return ret;
}

bool JsWindowRegisterManager::IsCallbackRegistered(napi_env env, std::string type, napi_value jsListenerObject)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGI("[NAPI]Method %{public}s has not been registerted", type.c_str());
        return false;
    }

    for (auto iter = jsCbMap_[type].begin(); iter != jsCbMap_[type].end(); ++iter) {
        bool isEquals = false;
        napi_strict_equals(env, jsListenerObject, iter->first->GetNapiValue(), &isEquals);
        if (isEquals) {
            WLOGFE("[NAPI]Method %{public}s has already been registered", type.c_str());
            return true;
        }
    }
    return false;
}

WmErrorCode JsWindowRegisterManager::RegisterListener(sptr<Window> window, std::string type,
    CaseType caseType, napi_env env, napi_value callback, napi_value parameter)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (IsCallbackRegistered(env, type, callback)) {
        return WmErrorCode::WM_OK;
    }
    if (listenerCodeMap_[CaseType].count(type) == 0) {
        WLOGFE("[NAPI]Type %{public}s is not supported", type.c_str());
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    napi_ref result = nullptr;
    napi_create_reference(env, callback, 1, &result);
    std::shared_ptr<NativeReference> callbackRef(reinterpret_cast<NativeReference*>(result));
    sptr<JsWindowListener> windowManagerListener = new(std::nothrow) JsWindowListener(env, callbackRef, caseType);
    if (windowManagerListener == nullptr) {
        WLOGFE("[NAPI]New JsWindowListener failed");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    windowManagerListener->SetMainEventHandler();
    WmErrorCode ret = ProcessRegisterListener(type, caseType, windowManagerListener, window, true, env, parameter);
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("[NAPI]Register type %{public}s failed", type.c_str());
        return ret;
    }
    jsCbMap_[type][callbackRef] = windowManagerListener;
    WLOGI("[NAPI]Register type %{public}s success! callback map size: %{public}zu",
        type.c_str(), jsCbMap_[type].size());
    return WmErrorCode::WM_OK;
}

WmErrorCode JsWindowRegisterManager::ProcessRegisterListener(const std::string& type, CaseType caseType,
    const sptr<JsWindowListener>& listener, const sptr<Window>& window, bool isRegister, napi_env env,
    napi_value parameter) {
    WmErrorCode ret = WmErrorCode::WM_OK;
    RegisterListenerType listenerType = listenerCodeMap_[type];
    if (caseType == CaseType::CASE_WINDOW_MANAGER) {
        switch (static_cast<int>(listenerType)) {
            case static_cast<int>(RegisterListenerType::SYSTEM_BAR_TINT_CHANGE_CB):
                ret = ProcessSystemBarChangeRegister(windowManagerListener, window, true, env, parameter);
                break;
            case static_cast<int>(RegisterListenerType::GESTURE_NAVIGATION_ENABLED_CHANGE_CB):
                ret = ProcessGestureNavigationEnabledChangeRegister(windowManagerListener, window, true,
                    env, parameter);
                break;
            case static_cast<int>(RegisterListenerType::WATER_MARK_FLAG_CHANGE_CB):
                ret = ProcessWaterMarkFlagChangeRegister(windowManagerListener, window, true, env, parameter);
                break;
            default:
                break;
        }
    } else if (caseType == CaseType::CASE_WINDOW) {
        switch (static_cast<int>(listenerType)) {
            case static_cast<int>(RegisterListenerType::WINDOW_SIZE_CHANGE_CB):
                ret = ProcessWindowChangeRegister(windowManagerListener, window, true, env, parameter);
                break;
            case static_cast<int>(RegisterListenerType::SYSTEM_AVOID_AREA_CHANGE_CB):
                ret = ProcessSystemAvoidAreaChangeRegister(windowManagerListener, window, true, env, parameter);
                break;
            case static_cast<int>(RegisterListenerType::AVOID_AREA_CHANGE_CB):
                ret = ProcessAvoidAreaChangeRegister(windowManagerListener, window, true, env, parameter);
                break;
            case static_cast<int>(RegisterListenerType::LIFECYCLE_EVENT_CB):
                ret = ProcessLifeCycleEventRegister(windowManagerListener, window, true, env, parameter);
                break;
            case static_cast<int>(RegisterListenerType::WINDOW_EVENT_CB):
                ret = ProcessLifeCycleEventRegister(windowManagerListener, window, true, env, parameter);
                break;
            case static_cast<int>(RegisterListenerType::KEYBOARD_HEIGHT_CHANGE_CB):
                ret = ProcessOccupiedAreaChangeRegister(windowManagerListener, window, true, env, parameter);
                break;
            case static_cast<int>(RegisterListenerType::TOUCH_OUTSIDE_CB):
                ret = ProcessTouchOutsideRegister(windowManagerListener, window, true, env, parameter);
                break;
            case static_cast<int>(RegisterListenerType::SCREENSHOT_EVENT_CB):
                ret = ProcessScreenshotRegister(windowManagerListener, window, true, env, parameter);
                break;
            case static_cast<int>(RegisterListenerType::DIALOG_TARGET_TOUCH_CB):
                ret = ProcessDialogTargetTouchRegister(windowManagerListener, window, true, env, parameter);
                break;
            case static_cast<int>(RegisterListenerType::DIALOG_DEATH_RECIPIENT_CB):
                ret = ProcessDialogDeathRecipientRegister(windowManagerListener, window, true, env, parameter);
                break;
            case static_cast<int>(RegisterListenerType::WINDOW_STATUS_CHANGE_CB):
                ret = ProcessWindowStatusChangeRegister(windowManagerListener, window, true, env, parameter);
                break;
            case static_cast<int>(RegisterListenerType::WINDOW_TITLE_BUTTON_RECT_CHANGE_CB):
                ret = ProcessWindowTitleButtonRectChangeRegister(windowManagerListener, window, true, env, parameter);
                break;
            case static_cast<int>(RegisterListenerType::WINDOW_VISIBILITY_CHANGE_CB):
                ret = ProcessWindowVisibilityChangeRegister(windowManagerListener, window, true, env, parameter);
                break;
            case static_cast<int>(RegisterListenerType::WINDOW_NO_INTERACTION_DETECT_CB):
                ret = ProcessWindowNoInteractionRegister(windowManagerListener, window, true, env, parameter);
                break;
            case static_cast<int>(RegisterListenerType::WINDOW_RECT_CHANGE_CB):
                ret = ProcessWindowRectChangeRegister(windowManagerListener, window, true, env, parameter);
                break;
            case static_cast<int>(RegisterListenerType::SUB_WINDOW_CLOSE_CB):
                ret = ProcessSubWindowCloseRegister(windowManagerListener, window, true, env, parameter);
                break;
            default:
                break;
        }
    } else if (caseType == CaseType::CASE_STAGE) {
        if (listenerType == RegisterListenerType::WINDOW_STAGE_EVENT_CB) {
            ret = ProcessLifeCycleEventRegister(windowManagerListener, window, true, env, parameter);
        }
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::UnregisterListener(sptr<Window> window, std::string type,
    CaseType caseType, napi_env env, napi_value value)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGFW("[NAPI]Type %{public}s was not registerted", type.c_str());
        return WmErrorCode::WM_OK;
    }
    if (listenerCodeMap_[CaseType].count(type) == 0) {
        WLOGFE("[NAPI]Type %{public}s is not supported", type.c_str());
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    if (value == nullptr) {
        for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
            WmErrorCode ret = ProcessUnRegisterListener(type, caseType, it->second, window, false, env, nullptr);
            if (ret != WmErrorCode::WM_OK) {
                WLOGFE("[NAPI]Unregister type %{public}s failed, no value", type.c_str());
                return ret;
            }
            jsCbMap_[type].erase(it++);
        }
    } else {
        bool findFlag = false;
        for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end(); ++it) {
            bool isEquals = false;
            napi_strict_equals(env, value, it->first->GetNapiValue(), &isEquals);
            if (!isEquals) {
                continue;
            }
            findFlag = true;
            WmErrorCode ret = ProcessUnRegisterListener(type, caseType, it->second, window, false, env, nullptr);
            if (ret != WmErrorCode::WM_OK) {
                WLOGFE("[NAPI]Unregister type %{public}s failed", type.c_str());
                return ret;
            }
            jsCbMap_[type].erase(it);
            break;
        }
        if (!findFlag) {
            WLOGFW("[NAPI]Unregister type %{public}s failed because not found callback!", type.c_str());
            return WmErrorCode::WM_OK;
        }
    }
    WLOGI("[NAPI]Unregister type %{public}s success! callback map size: %{public}zu",
        type.c_str(), jsCbMap_[type].size());
    // erase type when there is no callback in one type
    if (jsCbMap_[type].empty()) {
        jsCbMap_.erase(type);
    }
    return WmErrorCode::WM_OK;
}

WmErrorCode JsWindowRegisterManager::ProcessUnRegisterListener(const std::string& type, CaseType caseType,
    const sptr<JsWindowListener>& listener, const sptr<Window>& window, bool isRegister, napi_env env,
    napi_value parameter) {
    WmErrorCode ret = WmErrorCode::WM_OK;
    RegisterListenerType listenerType = listenerCodeMap_[type];
    if (caseType == CaseType::CASE_WINDOW_MANAGER) {
        switch (static_cast<int>(listenerType)) {
            case static_cast<int>(RegisterListenerType::SYSTEM_BAR_TINT_CHANGE_CB):
                ret = ProcessSystemBarChangeRegister(it->second, window, false, env, nullptr);
                break;
            case static_cast<int>(RegisterListenerType::GESTURE_NAVIGATION_ENABLED_CHANGE_CB):
                ret = ProcessGestureNavigationEnabledChangeRegister(it->second, window, false, env, nullptr);
                break;
            case static_cast<int>(RegisterListenerType::WATER_MARK_FLAG_CHANGE_CB):
                ret = ProcessWaterMarkFlagChangeRegister(it->second, window, false, env, nullptr);
                break;
            default:
                break;
        }
    } else if (caseType == CaseType::CASE_WINDOW) {
        switch (static_cast<int>(listenerType)) {
            case static_cast<int>(RegisterListenerType::WINDOW_SIZE_CHANGE_CB):
                ret = ProcessWindowChangeRegister(it->second, window, false, env, nullptr);
                break;
            case static_cast<int>(RegisterListenerType::SYSTEM_AVOID_AREA_CHANGE_CB):
                ret = ProcessSystemAvoidAreaChangeRegister(it->second, window, false, env, nullptr);
                break;
            case static_cast<int>(RegisterListenerType::AVOID_AREA_CHANGE_CB):
                ret = ProcessAvoidAreaChangeRegister(it->second, window, false, env, nullptr);
                break;
            case static_cast<int>(RegisterListenerType::LIFECYCLE_EVENT_CB):
                ret = ProcessLifeCycleEventRegister(it->second, window, false, env, nullptr);
                break;
            case static_cast<int>(RegisterListenerType::WINDOW_EVENT_CB):
                ret = ProcessLifeCycleEventRegister(it->second, window, false, env, nullptr);
                break;
            case static_cast<int>(RegisterListenerType::KEYBOARD_HEIGHT_CHANGE_CB):
                ret = ProcessOccupiedAreaChangeRegister(it->second, window, false, env, nullptr);
                break;
            case static_cast<int>(RegisterListenerType::TOUCH_OUTSIDE_CB):
                ret = ProcessTouchOutsideRegister(it->second, window, false, env, nullptr);
                break;
            case static_cast<int>(RegisterListenerType::SCREENSHOT_EVENT_CB):
                ret = ProcessScreenshotRegister(it->second, window, false, env, nullptr);
                break;
            case static_cast<int>(RegisterListenerType::DIALOG_TARGET_TOUCH_CB):
                ret = ProcessDialogTargetTouchRegister(it->second, window, false, env, nullptr);
                break;
            case static_cast<int>(RegisterListenerType::DIALOG_DEATH_RECIPIENT_CB):
                ret = ProcessDialogDeathRecipientRegister(it->second, window, false, env, nullptr);
                break;
            case static_cast<int>(RegisterListenerType::WINDOW_STATUS_CHANGE_CB):
                ret = ProcessWindowStatusChangeRegister(it->second, window, false, env, nullptr);
                break;
            case static_cast<int>(RegisterListenerType::WINDOW_TITLE_BUTTON_RECT_CHANGE_CB):
                ret = ProcessWindowTitleButtonRectChangeRegister(it->second, window, false, env, nullptr);
                break;
            case static_cast<int>(RegisterListenerType::WINDOW_VISIBILITY_CHANGE_CB):
                ret = ProcessWindowVisibilityChangeRegister(it->second, window, false, env, nullptr);
                break;
            case static_cast<int>(RegisterListenerType::WINDOW_NO_INTERACTION_DETECT_CB):
                ret = ProcessWindowNoInteractionRegister(it->second, window, false, env, nullptr);
                break;
            case static_cast<int>(RegisterListenerType::WINDOW_RECT_CHANGE_CB):
                ret = ProcessWindowRectChangeRegister(it->second, window, false, env, nullptr);
                break;
            case static_cast<int>(RegisterListenerType::SUB_WINDOW_CLOSE_CB):
                ret = ProcessSubWindowCloseRegister(it->second, window, false, env, nullptr);
                break;
            default:
                break;
        }
    } else if (caseType == CaseType::CASE_STAGE) {
        if (listenerType == RegisterListenerType::WINDOW_STAGE_EVENT_CB) {
            ret = ProcessLifeCycleEventRegister(it->second, window, false, env, nullptr);
        }
    }
}

WmErrorCode JsWindowRegisterManager::ProcessWindowStatusChangeRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, napi_value parameter)
{
    if (window == nullptr) {
        WLOGFE("[NAPI]Window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IWindowStatusChangeListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterWindowStatusChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterWindowStatusChangeListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessWindowRectChangeRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, napi_value parameter)
{
    if (window == nullptr) {
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IWindowRectChangeListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterWindowRectChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterWindowRectChangeListener(thisListener));
    }
    return ret;
}

WmErrorCode JsWindowRegisterManager::ProcessSubWindowCloseRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister, napi_env env, napi_value parameter)
{
    if (window == nullptr) {
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<ISubWindowCloseListener> thisListener(listener);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterSubWindowCloseListeners(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterSubWindowCloseListeners(thisListener));
    }
    return ret;
}
} // namespace Rosen
} // namespace OHOS