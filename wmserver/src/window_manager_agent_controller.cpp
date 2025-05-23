/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "window_manager_agent_controller.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowManagerAgentController"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(WindowManagerAgentController)

WMError WindowManagerAgentController::RegisterWindowManagerAgent(const sptr<IWindowManagerAgent>& windowManagerAgent,
    WindowManagerAgentType type)
{
    return wmAgentContainer_.RegisterAgent(windowManagerAgent, type) ? WMError::WM_OK : WMError::WM_ERROR_NULLPTR;
}

WMError WindowManagerAgentController::UnregisterWindowManagerAgent(const sptr<IWindowManagerAgent>& windowManagerAgent,
    WindowManagerAgentType type)
{
    return wmAgentContainer_.UnregisterAgent(windowManagerAgent, type) ? WMError::WM_OK : WMError::WM_ERROR_NULLPTR;
}

void WindowManagerAgentController::UpdateFocusChangeInfo(const sptr<FocusChangeInfo>& focusChangeInfo, bool focused)
{
    for (auto& agent : wmAgentContainer_.GetAgentsByType(WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS)) {
        agent->UpdateFocusChangeInfo(focusChangeInfo, focused);
    }
}

void WindowManagerAgentController::UpdateWindowModeTypeInfo(WindowModeType type)
{
    for (auto& agent : wmAgentContainer_.GetAgentsByType(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_MODE)) {
        agent->UpdateWindowModeTypeInfo(type);
    }
}

void WindowManagerAgentController::UpdateSystemBarRegionTints(DisplayId displayId, const SystemBarRegionTints& tints)
{
    WLOGFD("tints size: %{public}u", static_cast<uint32_t>(tints.size()));
    if (tints.empty()) {
        return;
    }
    for (auto& agent : wmAgentContainer_.GetAgentsByType(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_SYSTEM_BAR)) {
        agent->UpdateSystemBarRegionTints(displayId, tints);
    }
}

void WindowManagerAgentController::NotifyAccessibilityWindowInfo(
    const std::vector<sptr<AccessibilityWindowInfo>>& infos, WindowUpdateType type)
{
    WLOGD("NotifyAccessibilityWindowInfo");
    for (auto& agent : wmAgentContainer_.GetAgentsByType(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_UPDATE)) {
        agent->NotifyAccessibilityWindowInfo(infos, type);
    }
}

void WindowManagerAgentController::UpdateWindowVisibilityInfo(
    const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfos)
{
    WLOGFD("Size:%{public}zu", windowVisibilityInfos.size());
    for (auto& agent : wmAgentContainer_.GetAgentsByType(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_VISIBILITY)) {
        agent->UpdateWindowVisibilityInfo(windowVisibilityInfos);
    }
}

void WindowManagerAgentController::NotifyDisplayIdChange(uint32_t windowId, DisplayId displayId)
{
    for (auto& agent : wmAgentContainer_.GetAgentsByType(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_DISPLAY_ID)) {
        agent->NotifyDisplayIdChange(windowId, displayId);
    }
}

void WindowManagerAgentController::UpdateCameraFloatWindowStatus(uint32_t accessTokenId, bool isShowing)
{
    for (auto& agent : wmAgentContainer_.GetAgentsByType(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_CAMERA_FLOAT)) {
        agent->UpdateCameraFloatWindowStatus(accessTokenId, isShowing);
    }
}

void WindowManagerAgentController::NotifyWaterMarkFlagChangedResult(bool showWaterMark)
{
    WLOGFD("NotifyWaterMarkFlagChanged with result:%{public}d", showWaterMark);
    for (auto& agent : wmAgentContainer_.GetAgentsByType(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WATER_MARK_FLAG)) {
        agent->NotifyWaterMarkFlagChangedResult(showWaterMark);
    }
}

void WindowManagerAgentController::NotifyGestureNavigationEnabledResult(bool enable)
{
    WLOGFD("NotifyGestureNavigationEnabledResult with result:%{public}d", enable);
    for (auto& agent : wmAgentContainer_.GetAgentsByType(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_GESTURE_NAVIGATION_ENABLED)) {
        agent->NotifyGestureNavigationEnabledResult(enable);
    }
}

void WindowManagerAgentController::UpdatePiPWindowStateChanged(const std::string& bundleName, bool isForeground)
{
    TLOGD(WmsLogTag::WMS_PIP, "bundleName:%{public}s, state:%{public}d", bundleName.c_str(), isForeground);
    for (auto& agent : wmAgentContainer_.GetAgentsByType(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_PIP)) {
        agent->UpdatePiPWindowStateChanged(bundleName, isForeground);
    }
}
} // namespace Rosen
} // namespace OHOS