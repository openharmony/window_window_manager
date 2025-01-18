/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "cstdint"
#include "event_handler.h"
#include "event_runner.h"
#include "functional"
#include "oh_window_comm.h"
#include "oh_window.h"
#include "window.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<AppExecFwk::EventHandler> GetMainEventHandler()
{
    static std::shared_ptr<AppExecFwk::EventHandler> eventHandler;
    if (eventHandler == nullptr) {
        auto mainRunner = AppExecFwk::EventRunner::GetMainEventRunner();
        if (mainRunner == nullptr) {
            return nullptr;
        }
        eventHandler = std::make_shared<AppExecFwk::EventHandler>(mainRunner);
    }
    return eventHandler;
}

/**
 * @brief Used to map from WMError to WmErrorCode.
 */
const std::map<WMError, WindowManager_ErrorCode> WM_NDK_TO_ERROR_CODE_MAP {
    {WMError::WM_OK,                           WindowManager_ErrorCode::OK                                          },
    {WMError::WM_ERROR_INVALID_PARAM,          WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INVALID_PARAM      },
    {WMError::WM_ERROR_DEVICE_NOT_SUPPORT,     WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORT },
    {WMError::WM_ERROR_INVALID_WINDOW,         WindowManager_ErrorCode::INVAILD_WINDOW_ID                           },
    {WMError::WM_ERROR_INVALID_CALLING,        WindowManager_ErrorCode::SERVICE_ERROR                               },
    {WMError::WM_ERROR_NULLPTR,                WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_STATE_ABNORMALLY   },
    {WMError::WM_ERROR_SYSTEM_ABNORMALLY,      WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_SYSTEM_ABNORMALLY  },
};

static WindowManager_Rect TransformedToWindowManagerRect(const Rect rect)
{
    WindowManager_Rect wmRect;
    wmRect.posX = rect.posX_;
    wmRect.posY = rect.posY_;
    wmRect.width = rect.width_;
    wmRect.height = rect.height_;
    return wmRect;
}

static void TransformedToWindowManagerAvoidArea(WindowManager_AvoidArea* avoidArea, const AvoidArea allAvoidArea)
{
    avoidArea->topRect = TransformedToWindowManagerRect(allAvoidArea.topRect_);
    avoidArea->leftRect = TransformedToWindowManagerRect(allAvoidArea.leftRect_);
    avoidArea->rightRect = TransformedToWindowManagerRect(allAvoidArea.rightRect_);
    avoidArea->bottomRect = TransformedToWindowManagerRect(allAvoidArea.bottomRect_);
}

WindowManager_ErrorCode OH_Window_GetWindowAvoidArea(
    int32_t windowId, WindowManager_AvoidAreaType type, WindowManager_AvoidArea* avoidArea)
{
    TLOGD(WmsLogTag::WMS_IMMS, "windowId:%{public}d", windowId);
    if (avoidArea == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "avoidArea is null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INVALID_PARAM;        
    }
    auto eventHandler = GetMainEventHandler();
    if (eventHandler == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "eventHandler null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::SERVICE_ERROR;
    }
    WindowManager_ErrorCode errCode = WindowManager_ErrorCode::SERVICE_ERROR;
    eventHandler->PostSyncTask([windowId, type, avoidArea, &errCode, where = __func__] {
        auto window = OHOS::Rosen::Window::GetWindowWithId(windowId);
        if (window == nullptr) {
            TLOGNE(WmsLogTag::WMS_IMMS, "%{public}s window is null, windowId:%{public}d", where, windowId);
            errCode = WindowManager_ErrorCode::INVAILD_WINDOW_ID;
            return;
        }
        AvoidArea allAvoidArea;
        errCode = WM_NDK_TO_ERROR_CODE_MAP.at(
            window->GetAvoidAreaByType(static_cast<AvoidAreaType>(type), allAvoidArea));
        TransformedToWindowManagerAvoidArea(avoidArea, allAvoidArea);
    }, __func__);
    return errCode;
}

WindowManager_ErrorCode OH_Window_SetWindowStatusBarEnabled(int32_t windowId, bool enabled, bool enableAnimation)
{
    TLOGD(WmsLogTag::WMS_IMMS, "windowId:%{public}d, enabled:%{public}d, enableAnimation:%{public}d",
        windowId, enabled, enableAnimation);
    auto eventHandler = GetMainEventHandler();
    if (eventHandler == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "eventHandler null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::SERVICE_ERROR;
    }
    WindowManager_ErrorCode errCode = WindowManager_ErrorCode::SERVICE_ERROR;
    eventHandler->PostSyncTask([windowId, enabled, enableAnimation, &errCode, where = __func__] {
        auto window = OHOS::Rosen::Window::GetWindowWithId(windowId);
        if (window == nullptr) {
            TLOGNE(WmsLogTag::WMS_IMMS, "%{public}s window is null, windowId:%{public}d", where, windowId);
            errCode = WindowManager_ErrorCode::INVAILD_WINDOW_ID;
            return;
        }
        if (window->IsPcWindow()) {
            TLOGNE(WmsLogTag::WMS_IMMS, "%{public}s device is not support", where);
            errCode = WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORT;
            return;
        }
        auto property = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
        property.enable_ = enabled;
        property.enableAnimation_ = enableAnimation;
        errCode = WM_NDK_TO_ERROR_CODE_MAP.at(
            window->SetSpecificBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
    }, __func__);
    return errCode;
}

WindowManager_ErrorCode OH_Window_SetWindowStatusBarColor(int32_t windowId, int32_t color)
{
    TLOGD(WmsLogTag::WMS_IMMS, "windowId:%{public}d, color:%{public}d", windowId, color);
    auto eventHandler = GetMainEventHandler();
    if (eventHandler == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "eventHandler null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::SERVICE_ERROR;
    }
    WindowManager_ErrorCode errCode = WindowManager_ErrorCode::SERVICE_ERROR;
    eventHandler->PostSyncTask([windowId, color, &errCode, where = __func__] {
        auto window = OHOS::Rosen::Window::GetWindowWithId(windowId);
        if (window == nullptr) {
            TLOGNE(WmsLogTag::WMS_IMMS, "%{public}s window is null, windowId:%{public}d", where, windowId);
            errCode = WindowManager_ErrorCode::INVAILD_WINDOW_ID;
            return;
        }
        if (window->IsPcWindow()) {
            TLOGNE(WmsLogTag::WMS_IMMS, "%{public}s device is not support", where);
            errCode = WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORT;
            return;
        }
        auto property = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
        property.contentColor_ = color;
        errCode = WM_NDK_TO_ERROR_CODE_MAP.at(
            window->SetSpecificBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
    }, __func__);
    return errCode;
}

WindowManager_ErrorCode OH_Window_SetWindowNavigationBarEnabled(int32_t windowId, bool enabled, bool enableAnimation)
{
    TLOGD(WmsLogTag::WMS_IMMS, "windowId:%{public}d, enabled:%{public}d, enableAnimation:%{public}d",
        windowId, enabled, enableAnimation);
    auto eventHandler = GetMainEventHandler();
    if (eventHandler == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "eventHandler null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::SERVICE_ERROR;
    }
    WindowManager_ErrorCode errCode = WindowManager_ErrorCode::SERVICE_ERROR;
    eventHandler->PostSyncTask([windowId, enabled, enableAnimation, &errCode, where = __func__] {
        auto window = OHOS::Rosen::Window::GetWindowWithId(windowId);
        if (window == nullptr) {
            TLOGNE(WmsLogTag::WMS_IMMS, "%{public}s window is null, windowId:%{public}d", where, windowId);
            errCode = WindowManager_ErrorCode::INVAILD_WINDOW_ID;
            return;
        }
        if (window->IsPcWindow()) {
            TLOGNE(WmsLogTag::WMS_IMMS, "%{public}s device is not support", where);
            errCode = WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORT;
            return;
        }
        auto property = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR);
        property.enable_ = enabled;
        property.enableAnimation_ = enableAnimation;
        errCode = WM_NDK_TO_ERROR_CODE_MAP.at(
            window->SetSpecificBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR, property));
    }, __func__);
    return errCode;
}
}  // namespace Rosen
}  // namespace OHOS