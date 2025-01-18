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
#include "functional"

#include "oh_window.h"
#include "oh_window_comm.h"
#include "window.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

#include "event_handler.h"
#include "event_runner.h"
#include "image/pixelmap_native.h"
#include "pixelmap_native_impl.h"
#include "ui_content.h"


namespace OHOS::Rosen;

static WindowManager_Rect TransformedToWindowManagerRect(const Rect& rect)
{
    WindowManager_Rect wmRect;
    wmRect.posX = rect.posX_;
    wmRect.posY = rect.posY_;
    wmRect.width = rect.width_;
    wmRect.height = rect.height_;
    return wmRect;
}

static void TransformedToWindowManagerAvoidArea(WindowManager_AvoidArea* avoidArea, const AvoidArea& allAvoidArea)
{
    avoidArea->topRect = TransformedToWindowManagerRect(allAvoidArea.topRect_);
    avoidArea->leftRect = TransformedToWindowManagerRect(allAvoidArea.leftRect_);
    avoidArea->rightRect = TransformedToWindowManagerRect(allAvoidArea.rightRect_);
    avoidArea->bottomRect = TransformedToWindowManagerRect(allAvoidArea.bottomRect_);
}

static std::shared_ptr<OHOS::AppExecFwk::EventHandler> GetMainEventHandler()
{
    static std::shared_ptr<OHOS::AppExecFwk::EventHandler> eventHandler;
    if (eventHandler == nullptr) {
        auto mainRunner = OHOS::AppExecFwk::EventRunner::GetMainEventRunner();
        if (mainRunner == nullptr) {
            return nullptr;
        }
        eventHandler = std::make_shared<OHOS::AppExecFwk::EventHandler>(mainRunner);
    }
    return eventHandler;
}

/**
 * @brief Used to map from WMError to WindowManager_ErrorCode.
 */
const std::map<WMError, WindowManager_ErrorCode> OH_WINDOW_TO_ERROR_CODE_MAP {
    { WMError::WM_OK,                           WindowManager_ErrorCode::OK                                          },
    { WMError::WM_ERROR_INVALID_PARAM,          WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INVALID_PARAM      },
    { WMError::WM_ERROR_DEVICE_NOT_SUPPORT,     WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORT },
    { WMError::WM_ERROR_INVALID_WINDOW,         WindowManager_ErrorCode::INVAILD_WINDOW_ID                           },
    { WMError::WM_ERROR_INVALID_CALLING,        WindowManager_ErrorCode::SERVICE_ERROR                               },
    { WMError::WM_ERROR_NULLPTR,                WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_STATE_ABNORMALLY   },
    { WMError::WM_ERROR_SYSTEM_ABNORMALLY,      WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_SYSTEM_ABNORMALLY  },
};

/**
 * @brief Used to map from WindowType to WindowManager_WindowType.
 */
const std::map<WindowType, WindowManager_WindowType> OH_WINDOW_TO_WINDOW_TYPE_MAP {
    { WindowType::WINDOW_TYPE_APP_SUB_WINDOW,      WindowManager_WindowType::WINDOW_MANAGER_WINDOW_TYPE_APP    },
    { WindowType::WINDOW_TYPE_DIALOG,              WindowManager_WindowType::WINDOW_MANAGER_WINDOW_TYPE_DIALOG },
    { WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,     WindowManager_WindowType::WINDOW_MANAGER_WINDOW_TYPE_MAIN   },
    { WindowType::WINDOW_TYPE_FLOAT,               WindowManager_WindowType::WINDOW_MANAGER_WINDOW_TYPE_FLOAT  },
};

int32_t OH_WindowManager_GetWindowAvoidArea(
    int32_t windowId, WindowManager_AvoidAreaType type, WindowManager_AvoidArea* avoidArea)
{
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
        errCode = OH_WINDOW_TO_ERROR_CODE_MAP.at(
            window->GetAvoidAreaByType(static_cast<AvoidAreaType>(type), allAvoidArea));
        TransformedToWindowManagerAvoidArea(avoidArea, allAvoidArea);
    }, __func__);
    return errCode;
}

int32_t OH_WindowManager_SetWindowStatusBarEnabled(int32_t windowId, bool enabled, bool enableAnimation)
{
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
            TLOGNE(WmsLogTag::WMS_IMMS, "%{public}s device is not support, windowId:%{public}d", where, windowId);
            errCode = WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORT;
            return;
        }
        auto property = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
        property.enable_ = enabled;
        property.enableAnimation_ = enableAnimation;
        errCode = OH_WINDOW_TO_ERROR_CODE_MAP.at(
            window->SetSpecificBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
    }, __func__);
    return errCode;
}

int32_t OH_WindowManager_SetWindowStatusBarColor(int32_t windowId, int32_t color)
{
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
            TLOGNE(WmsLogTag::WMS_IMMS, "%{public}s device is not support, windowId:%{public}d", where, windowId);
            errCode = WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORT;
            return;
        }
        auto property = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
        property.contentColor_ = color;
        errCode = OH_WINDOW_TO_ERROR_CODE_MAP.at(
            window->SetSpecificBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
    }, __func__);
    return errCode;
}

int32_t OH_WindowManager_SetWindowNavigationBarEnabled(int32_t windowId, bool enabled, bool enableAnimation)
{
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
            TLOGNE(WmsLogTag::WMS_IMMS, "%{public}s device is not support, windowId:%{public}d", where, windowId);
            errCode = WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORT;
            return;
        }
        auto property = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR);
        property.enable_ = enabled;
        property.enableAnimation_ = enableAnimation;
        errCode = OH_WINDOW_TO_ERROR_CODE_MAP.at(
            window->SetSpecificBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR, property));
    }, __func__);
    return errCode;
}

int32_t OH_WindowManager_Snapshot(int32_t windowId, OH_PixelmapNative* pixelMap)
{
    if (pixelMap == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "pixelMap is null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INVALID_PARAM;
    }
    auto eventHandler = GetMainEventHandler();
    if (eventHandler == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "eventHandler null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::SERVICE_ERROR;
    }
    WindowManager_ErrorCode errCode = WindowManager_ErrorCode::SERVICE_ERROR;
    eventHandler->PostSyncTask([windowId, pixelMap, &errCode, where = __func__]() mutable {
        auto window = OHOS::Rosen::Window::GetWindowWithId(windowId);
        if (window == nullptr) {
            TLOGNE(WmsLogTag::WMS_ATTRIBUTE, "%{public}s window is null, windowId:%{public}d", where, windowId);
            errCode = WindowManager_ErrorCode::INVAILD_WINDOW_ID;
            return;
        }
        pixelMap = new OH_PixelmapNative(window->Snapshot());
    }, __func__);
    return pixelMap != nullptr ? WindowManager_ErrorCode::OK : errCode;
}

int32_t OH_WindowManager_SetWindowBackgroundColor(int32_t windowId, const char* color)
{
    if (color == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "color is null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INVALID_PARAM;
    }
    auto eventHandler = GetMainEventHandler();
    if (eventHandler == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "eventHandler null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::SERVICE_ERROR;
    }
    WindowManager_ErrorCode errCode = WindowManager_ErrorCode::SERVICE_ERROR;
    eventHandler->PostSyncTask([windowId, color, &errCode, where = __func__] {
        auto window = OHOS::Rosen::Window::GetWindowWithId(windowId);
        if (window == nullptr) {
            TLOGNE(WmsLogTag::WMS_ATTRIBUTE, "%{public}s window is null, windowId:%{public}d", where, windowId);
            errCode = WindowManager_ErrorCode::INVAILD_WINDOW_ID;
            return;
        }
        errCode = OH_WINDOW_TO_ERROR_CODE_MAP.at(window->SetBackgroundColor(std::string(color)));
    }, __func__);
    return errCode;
}

int32_t OH_WindowManager_SetWindowBrightness(int32_t windowId, float brightness)
{
    auto eventHandler = GetMainEventHandler();
    if (eventHandler == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "eventHandler null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::SERVICE_ERROR;
    }
    WindowManager_ErrorCode errCode = WindowManager_ErrorCode::SERVICE_ERROR;
    eventHandler->PostSyncTask([windowId, brightness, &errCode, where = __func__] {
        auto window = OHOS::Rosen::Window::GetWindowWithId(windowId);
        if (window == nullptr) {
            TLOGNE(WmsLogTag::WMS_ATTRIBUTE, "%{public}s window is null, windowId:%{public}d", where, windowId);
            errCode = WindowManager_ErrorCode::INVAILD_WINDOW_ID;
            return;
        }
        errCode = OH_WINDOW_TO_ERROR_CODE_MAP.at(window->SetBrightness(brightness));
    }, __func__);
    return errCode;
}

int32_t OH_WindowManager_SetWindowKeepScreenOn(int32_t windowId, bool isKeepScreenOn)
{
    auto eventHandler = GetMainEventHandler();
    if (eventHandler == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "eventHandler null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::SERVICE_ERROR;
    }
    WindowManager_ErrorCode errCode = WindowManager_ErrorCode::SERVICE_ERROR;
    eventHandler->PostSyncTask([windowId, isKeepScreenOn, &errCode, where = __func__] {
        auto window = OHOS::Rosen::Window::GetWindowWithId(windowId);
        if (window == nullptr) {
            TLOGNE(WmsLogTag::WMS_ATTRIBUTE, "%{public}s window is null, windowId:%{public}d", where, windowId);
            errCode = WindowManager_ErrorCode::INVAILD_WINDOW_ID;
            return;
        }
        errCode = OH_WINDOW_TO_ERROR_CODE_MAP.at(window->SetKeepScreenOn(isKeepScreenOn));
    }, __func__);
    return errCode;
}

int32_t OH_WindowManager_SetWindowPrivacyMode(int32_t windowId, bool isPrivacy)
{
    auto eventHandler = GetMainEventHandler();
    if (eventHandler == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "eventHandler null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::SERVICE_ERROR;
    }
    WindowManager_ErrorCode errCode = WindowManager_ErrorCode::SERVICE_ERROR;
    eventHandler->PostSyncTask([windowId, isPrivacy, &errCode, where = __func__] {
        auto window = OHOS::Rosen::Window::GetWindowWithId(windowId);
        if (window == nullptr) {
            TLOGNE(WmsLogTag::WMS_ATTRIBUTE, "%{public}s window is null, windowId:%{public}d", where, windowId);
            errCode = WindowManager_ErrorCode::INVAILD_WINDOW_ID;
            return;
        }
        errCode = OH_WINDOW_TO_ERROR_CODE_MAP.at(window->SetPrivacyMode(isPrivacy));
    }, __func__);
    return errCode;
}

int32_t OH_WindowManager_GetWindowProperties(
    int32_t windowId, WindowManager_WindowProperties* windowProperties)
{
    if (windowProperties == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "windowProperties is null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INVALID_PARAM;
    }
    auto eventHandler = GetMainEventHandler();
    if (eventHandler == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "eventHandler null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::SERVICE_ERROR;
    }
    WindowManager_ErrorCode errCode = WindowManager_ErrorCode::OK;
    eventHandler->PostSyncTask([windowId, windowProperties, &errCode, where = __func__] {
        auto window = OHOS::Rosen::Window::GetWindowWithId(windowId);
        if (window == nullptr) {
            TLOGNE(WmsLogTag::WMS_ATTRIBUTE, "%{public}s window is null, windowId:%{public}d", where, windowId);
            errCode = WindowManager_ErrorCode::INVAILD_WINDOW_ID;
            return;
        }
        if (OH_WINDOW_TO_WINDOW_TYPE_MAP.count(window->GetType()) != 0) {
            windowProperties->type = OH_WINDOW_TO_WINDOW_TYPE_MAP.at(window->GetType());
        } else {
            windowProperties->type = static_cast<WindowManager_WindowType>(window->GetType());
        }
        windowProperties->windowRect = TransformedToWindowManagerRect(window->GetRect());
        windowProperties->isLayoutFullScreen = window->IsLayoutFullScreen();
        windowProperties->isFullScreen = window->IsFullScreen();
        windowProperties->touchable = window->GetTouchable();
        windowProperties->focusable = window->GetFocusable();
        windowProperties->isPrivacyMode = window->IsPrivacyMode();
        windowProperties->isKeepScreenOn = window->IsKeepScreenOn();
        windowProperties->brightness = window->GetBrightness();
        windowProperties->isTransparent = window->IsTransparent();
        windowProperties->id = window->GetWindowId();
        windowProperties->displayId = window->GetDisplayId();
        Rect drawableRect;
        auto uicontent = window->GetUIContent();
        if (uicontent == nullptr) {
            TLOGNE(WmsLogTag::WMS_ATTRIBUTE, "%{public}s uicontent is null, windowId:%{public}d", where, windowId);
            errCode = WindowManager_ErrorCode::SERVICE_ERROR;
            return;
        }
        uicontent->GetAppPaintSize(drawableRect);
        windowProperties->drawableRect = TransformedToWindowManagerRect(drawableRect);
    }, __func__); 
    return errCode;
}