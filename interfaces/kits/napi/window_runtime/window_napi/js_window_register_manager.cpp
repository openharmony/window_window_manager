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
namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsRegisterManager"};
}

JsWindowRegisterManager::JsWindowRegisterManager()
{
    // white register list for window manager
    listenerProcess_[CaseType::CASE_WINDOW_MANAGER] = {
        {SYSTEM_BAR_TINT_CHANGE_CB,            &JsWindowRegisterManager::ProcessSystemBarChangeRegister               },
        {GESTURE_NAVIGATION_ENABLED_CHANGE_CB, &JsWindowRegisterManager::ProcessGestureNavigationEnabledChangeRegister},
        {WATER_MARK_FLAG_CHANGE_CB,            &JsWindowRegisterManager::ProcessWaterMarkFlagChangeRegister           },
    };
    // white register list for window
    listenerProcess_[CaseType::CASE_WINDOW] = {
        { WINDOW_SIZE_CHANGE_CB,       &JsWindowRegisterManager::ProcessWindowChangeRegister          },
        { SYSTEM_AVOID_AREA_CHANGE_CB, &JsWindowRegisterManager::ProcessSystemAvoidAreaChangeRegister },
        { AVOID_AREA_CHANGE_CB,        &JsWindowRegisterManager::ProcessAvoidAreaChangeRegister       },
        { LIFECYCLE_EVENT_CB,          &JsWindowRegisterManager::ProcessLifeCycleEventRegister        },
        { WINDOW_EVENT_CB,             &JsWindowRegisterManager::ProcessLifeCycleEventRegister        },
        { KEYBOARD_HEIGHT_CHANGE_CB,   &JsWindowRegisterManager::ProcessOccupiedAreaChangeRegister    },
        { TOUCH_OUTSIDE_CB,            &JsWindowRegisterManager::ProcessTouchOutsideRegister          },
        { SCREENSHOT_EVENT_CB,         &JsWindowRegisterManager::ProcessScreenshotRegister            },
        { DIALOG_TARGET_TOUCH_CB,      &JsWindowRegisterManager::ProcessDialogTargetTouchRegister     },
        { DIALOG_DEATH_RECIPIENT_CB,   &JsWindowRegisterManager::ProcessDialogDeathRecipientRegister  },
    };
    // white register list for window stage
    listenerProcess_[CaseType::CASE_STAGE] = {
        {WINDOW_STAGE_EVENT_CB,         &JsWindowRegisterManager::ProcessLifeCycleEventRegister    }
    };
}

JsWindowRegisterManager::~JsWindowRegisterManager()
{
}

WmErrorCode JsWindowRegisterManager::ProcessWindowChangeRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister)
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
    sptr<Window> window, bool isRegister)
{
    if (window == nullptr) {
        WLOGFE("[NAPI]Window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    if (listener == nullptr) {
        WLOGFE("[NAPI]listener is nullptr");
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
    sptr<Window> window, bool isRegister)
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
    sptr<Window> window, bool isRegister)
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
    sptr<Window> window, bool isRegister)
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
    sptr<Window> window, bool isRegister)
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
    sptr<Window> window, bool isRegister)
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
    sptr<Window> window, bool isRegister)
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
    sptr<Window> window, bool isRegister)
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

WmErrorCode JsWindowRegisterManager::ProcessScreenshotRegister(sptr<JsWindowListener> listener,
    sptr<Window> window, bool isRegister)
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
    sptr<Window> window, bool isRegister)
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
    sptr<Window> window, bool isRegister)
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

bool JsWindowRegisterManager::IsCallbackRegistered(std::string type, NativeValue* jsListenerObject)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGI("[NAPI]Method %{public}s has not been registerted", type.c_str());
        return false;
    }

    for (auto iter = jsCbMap_[type].begin(); iter != jsCbMap_[type].end(); ++iter) {
        if (jsListenerObject->StrictEquals(iter->first->Get())) {
            WLOGFE("[NAPI]Method %{public}s has already been registered", type.c_str());
            return true;
        }
    }
    return false;
}

WmErrorCode JsWindowRegisterManager::RegisterListener(sptr<Window> window, std::string type,
    CaseType caseType, NativeEngine& engine, NativeValue* value)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (IsCallbackRegistered(type, value)) {
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    if (listenerProcess_[caseType].count(type) == 0) {
        WLOGFE("[NAPI]Type %{public}s is not supported", type.c_str());
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    NativeReference* callbackRef = engine.CreateReference(value, 1);
    sptr<JsWindowListener> windowManagerListener = new(std::nothrow) JsWindowListener(&engine, callbackRef);
    if (windowManagerListener == nullptr) {
        WLOGFE("[NAPI]New JsWindowListener failed");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    WmErrorCode ret = (this->*listenerProcess_[caseType][type])(windowManagerListener, window, true);
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("[NAPI]Register type %{public}s failed", type.c_str());
        return ret;
    }
    jsCbMap_[type][callbackRef] = windowManagerListener;
    WLOGI("[NAPI]Register type %{public}s success! callback map size: %{public}zu",
        type.c_str(), jsCbMap_[type].size());
    return WmErrorCode::WM_OK;
}

WmErrorCode JsWindowRegisterManager::UnregisterListener(sptr<Window> window, std::string type,
    CaseType caseType, NativeValue* value)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGFE("[NAPI]Type %{public}s was not registerted", type.c_str());
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    if (listenerProcess_[caseType].count(type) == 0) {
        WLOGFE("[NAPI]Type %{public}s is not supported", type.c_str());
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    if (value == nullptr) {
        for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
            WmErrorCode ret = (this->*listenerProcess_[caseType][type])(it->second, window, false);
            if (ret != WmErrorCode::WM_OK) {
                WLOGFE("[NAPI]Unregister type %{public}s failed, no value", type.c_str());
                return ret;
            }
            jsCbMap_[type].erase(it++);
        }
    } else {
        bool findFlag = false;
        for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end(); ++it) {
            if (!value->StrictEquals(it->first->Get())) {
                continue;
            }
            findFlag = true;
            WmErrorCode ret = (this->*listenerProcess_[caseType][type])(it->second, window, false);
            if (ret != WmErrorCode::WM_OK) {
                WLOGFE("[NAPI]Unregister type %{public}s failed", type.c_str());
                return ret;
            }
            jsCbMap_[type].erase(it);
            break;
        }
        if (!findFlag) {
            WLOGFE("[NAPI]Unregister type %{public}s failed because not found callback!", type.c_str());
            return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
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
} // namespace Rosen
} // namespace OHOS
