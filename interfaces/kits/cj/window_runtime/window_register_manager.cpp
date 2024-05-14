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
#include <transaction/rs_interfaces.h>
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
CjWindowRegisterManager::CjWindowRegisterManager()
{
    listenerProcess_[CaseType::CASE_WINDOW_MANAGER] = {
        {SYSTEM_BAR_TINT_CHANGE_CB,            &CjWindowRegisterManager::ProcessSystemBarChangeRegister               },
        {GESTURE_NAVIGATION_ENABLED_CHANGE_CB, &CjWindowRegisterManager::ProcessGestureNavigationEnabledChangeRegister},
        {WATER_MARK_FLAG_CHANGE_CB,            &CjWindowRegisterManager::ProcessWaterMarkFlagChangeRegister           },
    };
    listenerProcess_[CaseType::CASE_WINDOW] = {
        {WINDOW_SIZE_CHANGE_CB,                &CjWindowRegisterManager::ProcessWindowChangeRegister               },
        {SYSTEM_AVOID_AREA_CHANGE_CB,          &CjWindowRegisterManager::ProcessSystemAvoidAreaChangeRegister      },
        {AVOID_AREA_CHANGE_CB,                 &CjWindowRegisterManager::ProcessAvoidAreaChangeRegister            },
        {LIFECYCLE_EVENT_CB,                   &CjWindowRegisterManager::ProcessLifeCycleEventRegister             },
        {WINDOW_EVENT_CB,                      &CjWindowRegisterManager::ProcessLifeCycleEventRegister             },
        {KEYBOARD_HEIGHT_CHANGE_CB,            &CjWindowRegisterManager::ProcessOccupiedAreaChangeRegister         },
        {TOUCH_OUTSIDE_CB,                     &CjWindowRegisterManager::ProcessTouchOutsideRegister               },
        {SCREENSHOT_EVENT_CB,                  &CjWindowRegisterManager::ProcessScreenshotRegister                 },
        {DIALOG_TARGET_TOUCH_CB,               &CjWindowRegisterManager::ProcessDialogTargetTouchRegister          },
        {DIALOG_DEATH_RECIPIENT_CB,            &CjWindowRegisterManager::ProcessDialogDeathRecipientRegister       },
        {WINDOW_STATUS_CHANGE_CB,              &CjWindowRegisterManager::ProcessWindowStatusChangeRegister         },
        {WINDOW_TITLE_BUTTON_RECT_CHANGE_CB,   &CjWindowRegisterManager::ProcessWindowTitleButtonRectChangeRegister},
        {WINDOW_VISIBILITY_CHANGE_CB,          &CjWindowRegisterManager::ProcessWindowVisibilityChangeRegister     },
    };
    listenerProcess_[CaseType::CASE_STAGE] = {
        {WINDOW_STAGE_EVENT_CB,                &CjWindowRegisterManager::ProcessLifeCycleEventRegister    }
    };
}

WmErrorCode CjWindowRegisterManager::ProcessSystemBarChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister)
{
    return WmErrorCode::WM_OK;
}

WmErrorCode CjWindowRegisterManager::ProcessGestureNavigationEnabledChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister)
{
    return WmErrorCode::WM_OK;
}

WmErrorCode CjWindowRegisterManager::ProcessWaterMarkFlagChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister)
{
    return WmErrorCode::WM_OK;
}

WmErrorCode CjWindowRegisterManager::ProcessWindowChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister)
{
    return WmErrorCode::WM_OK;
}

WmErrorCode CjWindowRegisterManager::ProcessSystemAvoidAreaChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister)
{
    return WmErrorCode::WM_OK;
}

WmErrorCode CjWindowRegisterManager::ProcessAvoidAreaChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister)
{
    return WmErrorCode::WM_OK;
}

WmErrorCode CjWindowRegisterManager::ProcessLifeCycleEventRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister)
{
    return WmErrorCode::WM_OK;
}

WmErrorCode CjWindowRegisterManager::ProcessOccupiedAreaChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister)
{
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_SUB, "[WindowRegister] window is nullptr");
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

WmErrorCode CjWindowRegisterManager::ProcessTouchOutsideRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister)
{
    return WmErrorCode::WM_OK;
}

WmErrorCode CjWindowRegisterManager::ProcessScreenshotRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister)
{
    return WmErrorCode::WM_OK;
}

WmErrorCode CjWindowRegisterManager::ProcessDialogTargetTouchRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister)
{
    return WmErrorCode::WM_OK;
}

WmErrorCode CjWindowRegisterManager::ProcessDialogDeathRecipientRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister)
{
    return WmErrorCode::WM_OK;
}

WmErrorCode CjWindowRegisterManager::ProcessWindowStatusChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister)
{
    return WmErrorCode::WM_OK;
}

WmErrorCode CjWindowRegisterManager::ProcessWindowTitleButtonRectChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister)
{
    return WmErrorCode::WM_OK;
}

WmErrorCode CjWindowRegisterManager::ProcessWindowVisibilityChangeRegister(
    sptr<CjWindowListener> listener, sptr<Window> window, bool isRegister)
{
    return WmErrorCode::WM_OK;
}

bool CjWindowRegisterManager::IsCallbackRegistered(std::string type, int64_t callbackObject)
{
    std::shared_lock<std::shared_mutex> lock(mtx_);
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
    CaseType caseType, int64_t callbackObject)
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
    sptr<CjWindowListener> windowManagerListener = new(std::nothrow) CjWindowListener(callbackObject);
    if (windowManagerListener == nullptr) {
        TLOGE(WmsLogTag::WMS_SUB, "[WindowRegister]New CjWindowListener failed");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    windowManagerListener->SetMainEventHandler();
    WmErrorCode ret = (this->*listenerProcess_[caseType][type])(windowManagerListener, window, true);
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
            WmErrorCode ret = (this->*listenerProcess_[caseType][type])(it->second, window, false);
            if (ret != WmErrorCode::WM_OK) {
                TLOGE(WmsLogTag::WMS_SUB,
                    "[WindowRegister]Unregister type %{public}s failed, no value", type.c_str());
                return ret;
            }
            cjCbMap_[type].erase(it++);
        }
    } else {
        TLOGE(WmsLogTag::WMS_SUB,
            "[WindowRegister]Unregister type %{public}s failed because not found callback!", type.c_str());
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    return WmErrorCode::WM_OK;
}
}
}
