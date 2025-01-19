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

#include "oh_window.h"

#include <cstdint>
#include <functional>
#include <mutex>

#include "image/pixelmap_native.h"
#include "pixelmap_native_impl.h"
#include "ui_content.h"

#include <event_handler.h>
#include <event_runner.h>
#include "oh_window_comm.h"
#include "window.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t NORMAL_STATE_CHANGE = 0;
constexpr bool SHOW_WITH_NO_ANIMATION = false;
constexpr bool SHOW_WITH_FOCUS = true;
std::shared_ptr<OHOS::AppExecFwk::EventHandler> g_eventHandler;
std::once_flag g_onceFlagForInitEventHandler;

/*
 * Used to map from WMError to WindowManager_ErrorCode.
 */
const std::unordered_map<WMError, WindowManager_ErrorCode> OH_WINDOW_TO_ERROR_CODE_MAP {
    { WMError::WM_OK,                           WindowManager_ErrorCode::OK                                          },
    { WMError::WM_ERROR_INVALID_PARAM,          WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INVALID_PARAM      },
    { WMError::WM_ERROR_DEVICE_NOT_SUPPORT,     WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_DEVICE_NOT_SUPPORT },
    { WMError::WM_ERROR_INVALID_WINDOW,         WindowManager_ErrorCode::INVAILD_WINDOW_ID                           },
    { WMError::WM_ERROR_INVALID_CALLING,        WindowManager_ErrorCode::SERVICE_ERROR                               },
    { WMError::WM_ERROR_NULLPTR,                WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_STATE_ABNORMALLY   },
    { WMError::WM_ERROR_SYSTEM_ABNORMALLY,      WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_SYSTEM_ABNORMALLY  },
};

/*
 * Used to map from WindowType to WindowManager_WindowType.
 */
const std::unordered_map<WindowType, WindowManager_WindowType> OH_WINDOW_TO_WINDOW_TYPE_MAP {
    { WindowType::WINDOW_TYPE_APP_SUB_WINDOW,      WindowManager_WindowType::WINDOW_MANAGER_WINDOW_TYPE_APP    },
    { WindowType::WINDOW_TYPE_DIALOG,              WindowManager_WindowType::WINDOW_MANAGER_WINDOW_TYPE_DIALOG },
    { WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,     WindowManager_WindowType::WINDOW_MANAGER_WINDOW_TYPE_MAIN   },
    { WindowType::WINDOW_TYPE_FLOAT,               WindowManager_WindowType::WINDOW_MANAGER_WINDOW_TYPE_FLOAT  },
};

void TransformedToWindowManagerRect(const Rect& rect, WindowManager_Rect& wmRect)
{
    wmRect.posX = rect.posX_;
    wmRect.posY = rect.posY_;
    wmRect.width = rect.width_;
    wmRect.height = rect.height_;
}

void TransformedToWindowManagerAvoidArea(const AvoidArea& allAvoidArea, WindowManager_AvoidArea* avoidArea)
{
    if (avoidArea == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "avoidArea is nullptr");
        return;
    }
    TransformedToWindowManagerRect(allAvoidArea.topRect_, avoidArea->topRect);
    TransformedToWindowManagerRect(allAvoidArea.leftRect_, avoidArea->leftRect);
    TransformedToWindowManagerRect(allAvoidArea.rightRect_, avoidArea->rightRect);
    TransformedToWindowManagerRect(allAvoidArea.bottomRect_, avoidArea->bottomRect);
}

inline bool IsMainWindow(WindowType type)
{
    return (type >= WindowType::APP_MAIN_WINDOW_BASE && type < WindowType::APP_MAIN_WINDOW_END);
}

inline bool IsMainWindowAndNotShown(WindowType type, WindowState state)
{
    return (IsMainWindow(type) && state != WindowState::STATE_SHOWN);
}
}

std::shared_ptr<OHOS::AppExecFwk::EventHandler> GetMainEventHandler()
{
    std::call_once(g_onceFlagForInitEventHandler, [] {
        g_eventHandler =
            std::make_shared<OHOS::AppExecFwk::EventHandler>(OHOS::AppExecFwk::EventRunner::GetMainEventRunner());
    });
    return g_eventHandler;
}

WindowManager_ErrorCode ShowWindowInner(int32_t windowId)
{
    auto eventHandler = GetMainEventHandler();
    WindowManager_ErrorCode ret = WindowManager_ErrorCode::OK;
    eventHandler->PostSyncTask([windowId, &ret] {
        auto window = Window::GetWindowWithId(windowId);
        if (window == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "window is null, windowId:%{public}d", windowId);
            ret = WindowManager_ErrorCode::INVAILD_WINDOW_ID;
            return;
        }
        if (IsMainWindowAndNotShown(window->GetType(), window->GetWindowState())) {
            TLOGNW(WmsLogTag::WMS_LIFE,
                "window Type %{public}u and window state %{public}u is not supported, [%{public}u, %{public}s]",
                static_cast<uint32_t>(window->GetType()), static_cast<uint32_t>(window->GetWindowState()),
                window->GetWindowId(), window->GetWindowName().c_str());
            ret = WindowManager_ErrorCode::OK;
            return;
        }
        if (window->Show(NORMAL_STATE_CHANGE, SHOW_WITH_NO_ANIMATION, SHOW_WITH_FOCUS) == WMError::WM_OK) {
            ret = WindowManager_ErrorCode::OK;
        } else {
            ret = WindowManager_ErrorCode::SERVICE_ERROR;
        }
        TLOGNI(WmsLogTag::WMS_LIFE, "Window [%{public}u, %{public}s] show with ret=%{public}d",
            window->GetWindowId(), window->GetWindowName().c_str(), ret);
    }, __func__);
    return ret;
}

WindowManager_ErrorCode IsWindowShownInner(int32_t windowId, bool* isShow)
{
    if (isShow == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "isShow is null");
        return WindowManager_ErrorCode::SERVICE_ERROR;
    }
    auto eventHandler = GetMainEventHandler();
    WindowManager_ErrorCode ret = WindowManager_ErrorCode::OK;
    eventHandler->PostSyncTask([windowId, isShow, &ret] {
        auto window = Window::GetWindowWithId(windowId);
        if (window == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "window is null, windowId:%{public}d", windowId);
            ret = WindowManager_ErrorCode::INVAILD_WINDOW_ID;
            return;
        }
        *isShow = window->GetWindowState() == WindowState::STATE_SHOWN;
    }, __func__);
    return ret;
}
} // namespace Rosen
} // namespace OHOS

namespace {
WindowManager_ErrorCode OH_WindowManager_ShowWindow(int32_t windowId)
{
    return OHOS::Rosen::ShowWindowInner(windowId);
}

WindowManager_ErrorCode OH_WindowManager_IsWindowShown(int32_t windowId, bool* isShow)
{
    return OHOS::Rosen::IsWindowShownInner(windowId, isShow);
}

int32_t OH_WindowManager_GetWindowAvoidArea(
    int32_t windowId, WindowManager_AvoidAreaType type, WindowManager_AvoidArea* avoidArea)
{
    if (avoidArea == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "avoidArea is null, windowId:%{public}d", windowId);
        return WindowManager_ErrorCode::WINDOW_MANAGER_ERRORCODE_INVALID_PARAM;
    }
    auto eventHandler = OHOS::Rosen::GetMainEventHandler();
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
        TransformedToWindowManagerAvoidArea(allAvoidArea, avoidArea);
    }, __func__);
    return errCode;
}

int32_t OH_WindowManager_SetWindowStatusBarEnabled(int32_t windowId, bool enabled, bool enableAnimation)
{
    auto eventHandler = OHOS::Rosen::GetMainEventHandler();
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
    auto eventHandler = OHOS::Rosen::GetMainEventHandler();
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
    auto eventHandler = OHOS::Rosen::GetMainEventHandler();
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
    auto eventHandler = OHOS::Rosen::GetMainEventHandler();
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
    auto eventHandler = OHOS::Rosen::GetMainEventHandler();
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
    auto eventHandler = OHOS::Rosen::GetMainEventHandler();
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
    auto eventHandler = OHOS::Rosen::GetMainEventHandler();
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
    auto eventHandler = OHOS::Rosen::GetMainEventHandler();
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
    auto eventHandler = OHOS::Rosen::GetMainEventHandler();
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
        TransformedToWindowManagerRect(window->GetRect(), windowProperties->windowRect);
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
        TransformedToWindowManagerRect(drawableRect, windowProperties->drawableRect);
    }, __func__);
    return errCode;
}
} // namespace