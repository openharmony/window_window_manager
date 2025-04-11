/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "window_register_manager.h"
#include <cstdint>
#include <transaction/rs_interfaces.h>
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
CjWindowRegisterManager::CjWindowRegisterManager()
{
    InitWindowManagerListeners();
    InitWindowListeners();
    InitStageListeners();
}

void CjWindowRegisterManager::InitWindowManagerListeners()
{
    listenerProcess_[CaseType::CASE_WINDOW_MANAGER] = {
        {SYSTEM_BAR_TINT_CHANGE_CB, [this](sptr<CjWindowListener> listener,
            sptr<Window> window, const RegListenerInfo& info)
            {return this->ProcessSystemBarChangeRegister(listener, window, info); } },
        {GESTURE_NAVIGATION_ENABLED_CHANGE_CB,
            [this](sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
            {return this->ProcessGestureNavigationEnabledChangeRegister(listener, window, info); } },
        {WATER_MARK_FLAG_CHANGE_CB, [this](sptr<CjWindowListener> listener, sptr<Window> window,
            const RegListenerInfo& info)
            {return this->ProcessWaterMarkFlagChangeRegister(listener, window, info); } },
    };
}

void CjWindowRegisterManager::InitWindowListeners()
{
    listenerProcess_[CaseType::CASE_WINDOW] = {
        {WINDOW_SIZE_CHANGE_CB,
            [this](sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
            {return this->ProcessWindowChangeRegister(listener, window, info); } },
        {SYSTEM_AVOID_AREA_CHANGE_CB,
            [this](sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
            {return this->ProcessSystemAvoidAreaChangeRegister(listener, window, info); } },
        {AVOID_AREA_CHANGE_CB,
            [this](sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
            {return this->ProcessAvoidAreaChangeRegister(listener, window, info); } },
        {LIFECYCLE_EVENT_CB, [this](sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
            {return this->ProcessLifeCycleEventRegister(listener, window, info); } },
        {WINDOW_EVENT_CB, [this](sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
            {return this->ProcessLifeCycleEventRegister(listener, window, info); } },
        {KEYBOARD_HEIGHT_CHANGE_CB,
            [this](sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
            {return this->ProcessOccupiedAreaChangeRegister(listener, window, info); } },
        {TOUCH_OUTSIDE_CB, [this](sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
            {return this->ProcessTouchOutsideRegister(listener, window, info); } },
        {SCREENSHOT_EVENT_CB,
            [this](sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
            {return this->ProcessScreenshotRegister(listener, window, info); } },
        {DIALOG_TARGET_TOUCH_CB,
            [this](sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
            {return this->ProcessDialogTargetTouchRegister(listener, window, info); } },
        {DIALOG_DEATH_RECIPIENT_CB,
            [this](sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
            {return this->ProcessDialogDeathRecipientRegister(listener, window, info); } },
        {WINDOW_STATUS_CHANGE_CB,
            [this](sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
            {return this->ProcessWindowStatusChangeRegister(listener, window, info); } },
        {WINDOW_TITLE_BUTTON_RECT_CHANGE_CB,
            [this](sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
            {return this->ProcessWindowTitleButtonRectChangeRegister(listener, window, info); } },
        {WINDOW_VISIBILITY_CHANGE_CB,
            [this](sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
            {return this->ProcessWindowVisibilityChangeRegister(listener, window, info); } },
        {WINDOW_SUB_WINDOW_CLOSE_CB,
            [this](sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
            {return this->ProcessSubWindowCloseRegister(listener, window, info); } },
        {WINDOW_RECT_CHANGE_CB,
            [this](sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
            {return this->ProcessWindowRectChangeRegister(listener, window, info); } },
        {WINDOW_NO_INTERACTION_DETECTED_CB,
            [this](sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
            {return this->ProcessNoInteractionDetectedRegister(listener, window, info); } },
    };
}

void CjWindowRegisterManager::InitStageListeners()
{
    listenerProcess_[CaseType::CASE_STAGE] = {
        {WINDOW_STAGE_EVENT_CB,
            [this](sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
            {return this->ProcessLifeCycleEventRegister(listener, window, info); } },
    };
}

WmErrorCode CjWindowRegisterManager::ProcessSystemBarChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
{
    return WmErrorCode::WM_OK;
}

WmErrorCode CjWindowRegisterManager::ProcessGestureNavigationEnabledChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
{
    return WmErrorCode::WM_OK;
}

WmErrorCode CjWindowRegisterManager::ProcessWaterMarkFlagChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
{
    return WmErrorCode::WM_OK;
}

WmErrorCode CjWindowRegisterManager::ProcessWindowChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
{
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[WindowRegister] window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IWindowChangeListener> thisListener(listener);
    WmErrorCode ret;
    if (info.isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterWindowChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterWindowChangeListener(thisListener));
    }
    return ret;
}

WmErrorCode CjWindowRegisterManager::ProcessSystemAvoidAreaChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
{
    return WmErrorCode::WM_OK;
}

WmErrorCode CjWindowRegisterManager::ProcessAvoidAreaChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
{
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[WindowRegister] window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IAvoidAreaChangedListener> thisListener(listener);
    WmErrorCode ret;
    if (info.isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterAvoidAreaChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterAvoidAreaChangeListener(thisListener));
    }
    return ret;
}

WmErrorCode CjWindowRegisterManager::ProcessLifeCycleEventRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
{
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_SUB, "[WindowRegister] window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IWindowLifeCycle> thisListener(listener);
    WmErrorCode ret;
    if (info.isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterLifeCycleListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterLifeCycleListener(thisListener));
    }
    return ret;
}

WmErrorCode CjWindowRegisterManager::ProcessOccupiedAreaChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
{
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_SUB, "[WindowRegister] window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IOccupiedAreaChangeListener> thisListener(listener);
    WmErrorCode ret;
    if (info.isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterOccupiedAreaChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterOccupiedAreaChangeListener(thisListener));
    }
    return ret;
}

WmErrorCode CjWindowRegisterManager::ProcessTouchOutsideRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
{
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[WindowRegister] window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<ITouchOutsideListener> thisListener(listener);
    WmErrorCode ret;
    if (info.isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterTouchOutsideListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterTouchOutsideListener(thisListener));
    }
    return ret;
}

WmErrorCode CjWindowRegisterManager::ProcessScreenshotRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
{
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[WindowRegister] window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IScreenshotListener> thisListener(listener);
    WmErrorCode ret;
    if (info.isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterScreenshotListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterScreenshotListener(thisListener));
    }
    return ret;
}

WmErrorCode CjWindowRegisterManager::ProcessDialogTargetTouchRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
{
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "[WindowRegister] window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IDialogTargetTouchListener> thisListener(listener);
    WmErrorCode ret;
    if (info.isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterDialogTargetTouchListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterDialogTargetTouchListener(thisListener));
    }
    return ret;
}

WmErrorCode CjWindowRegisterManager::ProcessDialogDeathRecipientRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
{
    return WmErrorCode::WM_OK;
}

WmErrorCode CjWindowRegisterManager::ProcessWindowStatusChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
{
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[WindowRegister] window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IWindowStatusChangeListener> thisListener(listener);
    WmErrorCode ret;
    if (info.isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterWindowStatusChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterWindowStatusChangeListener(thisListener));
    }
    return ret;
}

WmErrorCode CjWindowRegisterManager::ProcessWindowTitleButtonRectChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
{
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[WindowRegister] window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IWindowTitleButtonRectChangedListener> thisListener(listener);
    WmErrorCode ret;
    if (info.isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterWindowTitleButtonRectChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterWindowTitleButtonRectChangeListener(thisListener));
    }
    return ret;
}

WmErrorCode CjWindowRegisterManager::ProcessWindowVisibilityChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
{
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[WindowRegister] window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IWindowVisibilityChangedListener> thisListener(listener);
    WmErrorCode ret;
    if (info.isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterWindowVisibilityChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterWindowVisibilityChangeListener(thisListener));
    }
    return ret;
}

WmErrorCode CjWindowRegisterManager::ProcessWindowRectChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
{
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[WindowRegister] window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IWindowRectChangeListener> thisListener(listener);
    WmErrorCode ret;
    if (info.isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterWindowRectChangeListener(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterWindowRectChangeListener(thisListener));
    }
    return ret;
}

WmErrorCode CjWindowRegisterManager::ProcessSubWindowCloseRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
{
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[WindowRegister] window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<ISubWindowCloseListener> thisListener(listener);
    WmErrorCode ret;
    if (info.isRegister) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterSubWindowCloseListeners(thisListener));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterSubWindowCloseListeners(thisListener));
    }
    return ret;
}

WmErrorCode CjWindowRegisterManager::ProcessNoInteractionDetectedRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, const RegListenerInfo& info)
{
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[WindowRegister] window is nullptr");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<IWindowNoInteractionListener> thisListener(listener);
    if (!info.isRegister) {
        return WM_JS_TO_ERROR_CODE_MAP.at(window->UnregisterWindowNoInteractionListener(thisListener));
    }
    constexpr int64_t secToMicrosecRatio = 1000;
    constexpr int64_t noInteractionMax = LLONG_MAX / secToMicrosecRatio;
    const int64_t timeout = info.parameter;
    if (timeout <= 0 || (timeout > noInteractionMax)) {
        TLOGE(WmsLogTag::DEFAULT,
            "invalid parameter: no-interaction-timeout %{public}" PRId64 " is not in(0s~%{public}" PRId64,
            timeout, noInteractionMax);
        return WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    thisListener->SetTimeout(timeout * secToMicrosecRatio);
    return WM_JS_TO_ERROR_CODE_MAP.at(window->RegisterWindowNoInteractionListener(thisListener));
}

bool CjWindowRegisterManager::IsCallbackRegistered(std::string type, int64_t callbackObject)
{
    if (cjCbMap_.empty() || cjCbMap_.find(type) == cjCbMap_.end()) {
        TLOGI(WmsLogTag::WMS_SUB, "[WindowRegister]Method %{public}s has not been registerted", type.c_str());
        return false;
    }

    for (auto iter = cjCbMap_[type].begin(); iter != cjCbMap_[type].end(); ++iter) {
        if (callbackObject == iter->first) {
            TLOGE(WmsLogTag::WMS_SUB,
                "[WindowRegister]Method %{public}s has already been registered", type.c_str());
            return true;
        }
    }
    return false;
}

WmErrorCode CjWindowRegisterManager::RegisterListener(sptr<Window> window, std::string type,
    CaseType caseType, int64_t callbackObject, int64_t parameter)
{
    std::lock_guard<std::shared_mutex> lock(mtx_);
    if (IsCallbackRegistered(type, callbackObject)) {
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    if (listenerProcess_[caseType].count(type) == 0) {
        TLOGE(WmsLogTag::WMS_SUB, "[WindowRegister]Type %{public}s is not supported", type.c_str());
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    if (callbackObject == NONE_CALLBACK_OBJECT) {
        TLOGE(WmsLogTag::WMS_SUB, "[WindowRegister]Invalid callback object %{public}" PRId64, callbackObject);
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    sptr<CjWindowListener> windowManagerListener = new(std::nothrow) CjWindowListener(callbackObject, caseType);
    if (windowManagerListener == nullptr) {
        TLOGE(WmsLogTag::WMS_SUB, "[WindowRegister]New CjWindowListener failed");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    windowManagerListener->SetMainEventHandler();
    WmErrorCode ret = listenerProcess_[caseType][type](windowManagerListener, window, {true, parameter});
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_SUB, "[WindowRegister]Register type %{public}s failed", type.c_str());
        return ret;
    }
    cjCbMap_[type][callbackObject] = windowManagerListener;
    TLOGI(WmsLogTag::WMS_SUB, "[WindowRegister]Register type %{public}s success! callback map size: %{public}zu",
        type.c_str(), cjCbMap_[type].size());
    return WmErrorCode::WM_OK;
}

WmErrorCode CjWindowRegisterManager::UnregisterListener(sptr<Window> window, std::string type,
    CaseType caseType, int64_t callbackObject)
{
    std::lock_guard<std::shared_mutex> lock(mtx_);
    if (cjCbMap_.empty() || cjCbMap_.find(type) == cjCbMap_.end()) {
        TLOGE(WmsLogTag::WMS_SUB, "[WindowRegister]Type %{public}s was not registerted", type.c_str());
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    if (listenerProcess_[caseType].count(type) == 0) {
        TLOGE(WmsLogTag::WMS_SUB, "[WindowRegister]Type %{public}s is not supported", type.c_str());
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    if (callbackObject == NONE_CALLBACK_OBJECT) {
        for (auto it = cjCbMap_[type].begin(); it != cjCbMap_[type].end();) {
            WmErrorCode ret = listenerProcess_[caseType][type](it->second, window, {false, 0});
            if (ret != WmErrorCode::WM_OK) {
                TLOGE(WmsLogTag::WMS_SUB,
                    "[WindowRegister]Unregister type %{public}s failed, no value", type.c_str());
                return ret;
            }
            cjCbMap_[type].erase(it++);
        }
    } else {
        auto& innerMap = cjCbMap_[type];
        auto it = innerMap.find(callbackObject);
        if (it != innerMap.end()) {
            WmErrorCode ret = listenerProcess_[caseType][type](it->second, window, {false, 0});
            if (ret != WmErrorCode::WM_OK) {
                TLOGE(WmsLogTag::DEFAULT, "[WindowRegister]Unregister type %{public}s failed, ret: %{public}d!",
                    type.c_str(), static_cast<int32_t>(ret));
                return ret;
            }
            innerMap.erase(it);
            return ret;
        } else {
            TLOGE(WmsLogTag::DEFAULT,
                "[WindowRegister]Unregister type %{public}s failed because not found callback!", type.c_str());
            return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
        }
    }
    return WmErrorCode::WM_OK;
}
}
}
