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

#include <cstdint>
#include <functional>
#include <mutex>

#include <event_handler.h>
#include <event_runner.h>
#include "oh_window.h"
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

WindowManager_ErrorCode OH_WindowManager_ShowWindow(int32_t windowId)
{
    return OHOS::Rosen::ShowWindowInner(windowId);
}

WindowManager_ErrorCode OH_WindowManager_IsWindowShown(int32_t windowId, bool* isShow)
{
    return OHOS::Rosen::IsWindowShownInner(windowId, isShow);
}