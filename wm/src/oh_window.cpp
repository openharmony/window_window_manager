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
#include "image/pixelmap_native.h"
#include "pixelmap_native_impl.h"
#include "ui_content.h"
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
        return WindowManager_ErrorCode::INVALID_PARAM;        
    }
    WMError errCode = WMError::WM_OK;
    auto eventHandler = GetMainEventHandler();
    if (eventHandler == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "eventHandler null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::SERVICE_ERROR;
    }
    eventHandler->PostSyncTask([windowId, type, avoidArea, &errCode, where = __func__] {
        auto window = OHOS::Rosen::Window::GetWindowWithId(windowId);
        if (window == nullptr) {
            TLOGNE(WmsLogTag::WMS_IMMS, "%{public}s window is null, windowId:%{public}d", where, windowId);
            errCode = WMError::WM_ERROR_INVALID_WINDOW;
            return;
        }
        AvoidArea allAvoidArea;
        errCode = window->GetAvoidAreaByType(static_cast<AvoidAreaType>(type), allAvoidArea);
        TransformedToWindowManagerAvoidArea(avoidArea, allAvoidArea);
    }, __func__);
    if (errCode == WMError::WM_ERROR_INVALID_WINDOW) {
        return WindowManager_ErrorCode::INVAILD_WINDOW_ID;
    }
    return errCode == WMError::WM_OK ? WindowManager_ErrorCode::OK : WindowManager_ErrorCode::SERVICE_ERROR;
}

WindowManager_ErrorCode OH_Window_SetWindowStatusBarEnabled(int32_t windowId, bool enabled, bool enableAnimation)
{
    TLOGD(WmsLogTag::WMS_IMMS, "windowId:%{public}d, enabled:%{public}d, enableAnimation:%{public}d",
        windowId, enabled, enableAnimation);
    WMError errCode = WMError::WM_OK;
    auto eventHandler = GetMainEventHandler();
    if (eventHandler == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "eventHandler null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::SERVICE_ERROR;
    }
    eventHandler->PostSyncTask([windowId, enabled, enableAnimation, &errCode] {
        auto window = OHOS::Rosen::Window::GetWindowWithId(windowId);
        if (window == nullptr) {
            TLOGNE(WmsLogTag::WMS_IMMS, "window is null, windowId:%{public}d", windowId);
            errCode = WMError::WM_ERROR_INVALID_WINDOW;
            return;
        }
        auto property = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
        property.enable_ = enabled;
        property.enableAnimation_ = enableAnimation;
        errCode = window->SetSpecificBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property);
    }, __func__);
    if (errCode == WMError::WM_ERROR_INVALID_WINDOW) {
        return WindowManager_ErrorCode::INVAILD_WINDOW_ID;
    }
    return errCode == WMError::WM_OK ? WindowManager_ErrorCode::OK : WindowManager_ErrorCode::SERVICE_ERROR;
}

WindowManager_ErrorCode OH_Window_SetWindowStatusBarColor(int32_t windowId, int32_t color)
{
    TLOGD(WmsLogTag::WMS_IMMS, "windowId:%{public}d, color:%{public}d", windowId, color);
    WMError errCode = WMError::WM_OK;
    auto eventHandler = GetMainEventHandler();
    if (eventHandler == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "eventHandler null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::SERVICE_ERROR;
    }
    eventHandler->PostSyncTask([windowId, color, &errCode] {
        auto window = OHOS::Rosen::Window::GetWindowWithId(windowId);
        if (window == nullptr) {
            TLOGNE(WmsLogTag::WMS_IMMS, "window is null, windowId:%{public}d", windowId);
            errCode = WMError::WM_ERROR_INVALID_WINDOW;
            return;
        }
        auto property = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
        property.contentColor_ = color;
        errCode = window->SetSpecificBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property);
    }, __func__);
    if (errCode == WMError::WM_ERROR_INVALID_WINDOW) {
        return WindowManager_ErrorCode::INVAILD_WINDOW_ID;
    }
    return errCode == WMError::WM_OK ? WindowManager_ErrorCode::OK : WindowManager_ErrorCode::SERVICE_ERROR;
}

WindowManager_ErrorCode OH_Window_SetWindowNavigationBarEnabled(int32_t windowId, bool enabled, bool enableAnimation)
{
    TLOGD(WmsLogTag::WMS_IMMS, "windowId:%{public}d, enabled:%{public}d, enableAnimation:%{public}d",
        windowId, enabled, enableAnimation);
    WMError errCode = WMError::WM_OK;
    auto eventHandler = GetMainEventHandler();
    if (eventHandler == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "eventHandler null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::SERVICE_ERROR;
    }
    eventHandler->PostSyncTask([windowId, enabled, enableAnimation, &errCode] {
        auto window = OHOS::Rosen::Window::GetWindowWithId(windowId);
        if (window == nullptr) {
            TLOGNE(WmsLogTag::WMS_IMMS, "window is null, windowId:%{public}d", windowId);
            errCode = WMError::WM_ERROR_INVALID_WINDOW;
            return;
        }
        auto property = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR);
        property.enable_ = enabled;
        property.enableAnimation_ = enableAnimation;
        errCode = window->SetSpecificBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR, property);
    }, __func__);
    if (errCode == WMError::WM_ERROR_INVALID_WINDOW) {
        return WindowManager_ErrorCode::INVAILD_WINDOW_ID;
    }
    return errCode == WMError::WM_OK ? WindowManager_ErrorCode::OK : WindowManager_ErrorCode::SERVICE_ERROR;
}
}  // namespace Rosen
}  // namespace OHOS