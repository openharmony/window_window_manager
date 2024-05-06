/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "session_manager_agent_controller.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionManagerAgentController"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(SessionManagerAgentController)

WMError SessionManagerAgentController::RegisterWindowManagerAgent(const sptr<IWindowManagerAgent>& windowManagerAgent,
    WindowManagerAgentType type)
{
    TLOGI(WmsLogTag::WMS_SYSTEM, "type: %{public}u", static_cast<uint32_t>(type));
    return smAgentContainer_.RegisterAgent(windowManagerAgent, type) ? WMError::WM_OK : WMError::WM_ERROR_NULLPTR;
}

WMError SessionManagerAgentController::UnregisterWindowManagerAgent(const sptr<IWindowManagerAgent>& windowManagerAgent,
    WindowManagerAgentType type)
{
    TLOGI(WmsLogTag::WMS_SYSTEM, "type: %{public}u", static_cast<uint32_t>(type));
    return smAgentContainer_.UnregisterAgent(windowManagerAgent, type) ? WMError::WM_OK : WMError::WM_ERROR_NULLPTR;
}

void SessionManagerAgentController::UpdateCameraFloatWindowStatus(uint32_t accessTokenId, bool isShowing)
{
    for (auto& agent : smAgentContainer_.GetAgentsByType(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_CAMERA_FLOAT)) {
        if (agent != nullptr) {
            agent->UpdateCameraFloatWindowStatus(accessTokenId, isShowing);
        }
    }
}

void SessionManagerAgentController::UpdateFocusChangeInfo(const sptr<FocusChangeInfo>& focusChangeInfo, bool isFocused)
{
    for (auto& agent : smAgentContainer_.GetAgentsByType(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS)) {
        if (agent != nullptr) {
            agent->UpdateFocusChangeInfo(focusChangeInfo, isFocused);
        }
    }
}

void SessionManagerAgentController::UpdateWindowModeTypeInfo(WindowModeType type)
{
    TLOGD(WmsLogTag::WMS_MAIN, "SessionManagerAgentController UpdateWindowModeTypeInfo type: %{public}d",
        static_cast<uint8_t>(type));
    for (auto& agent : smAgentContainer_.GetAgentsByType(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_MODE)) {
        if (agent != nullptr) {
            agent->UpdateWindowModeTypeInfo(type);
        }
    }
}

void SessionManagerAgentController::NotifyAccessibilityWindowInfo(
    const std::vector<sptr<AccessibilityWindowInfo>>& infos, WindowUpdateType type)
{
    for (auto& agent : smAgentContainer_.GetAgentsByType(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_UPDATE)) {
        if (agent != nullptr) {
            agent->NotifyAccessibilityWindowInfo(infos, type);
        }
    }
}

void SessionManagerAgentController::NotifyWaterMarkFlagChangedResult(bool hasWaterMark)
{
    WLOGFD("NotifyWaterMarkFlagChanged with result:%{public}u", static_cast<uint32_t>(hasWaterMark));
    for (auto& agent : smAgentContainer_.GetAgentsByType(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WATER_MARK_FLAG)) {
        if (agent != nullptr) {
            agent->NotifyWaterMarkFlagChangedResult(hasWaterMark);
        }
    }
}

void SessionManagerAgentController::UpdateWindowVisibilityInfo(
    const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfos)
{
    for (auto& agent : smAgentContainer_.GetAgentsByType(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_VISIBILITY)) {
        agent->UpdateWindowVisibilityInfo(windowVisibilityInfos);
    }
}

void SessionManagerAgentController::UpdateVisibleWindowNum(
    const std::vector<VisibleWindowNumInfo>& visibleWindowNumInfo)
{
    for (const auto& num : visibleWindowNumInfo) {
        TLOGI(WmsLogTag::WMS_MAIN, "displayId = %{public}d, visibleWindowNum = %{public}d",
            num.displayId, num.visibleWindowNum);
    }
    for (auto& agent : smAgentContainer_.GetAgentsByType(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_VISIBLE_WINDOW_NUM)) {
        if (agent != nullptr) {
            agent->UpdateVisibleWindowNum(visibleWindowNumInfo);
        }
    }
}

void SessionManagerAgentController::UpdateWindowDrawingContentInfo(
    const std::vector<sptr<WindowDrawingContentInfo>>& windowDrawingContentInfos)
{
    WLOGFD("Size:%{public}zu", windowDrawingContentInfos.size());
    for (auto& agent : smAgentContainer_.GetAgentsByType(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_DRAWING_STATE)) {
        agent->UpdateWindowDrawingContentInfo(windowDrawingContentInfos);
    }
}

void SessionManagerAgentController::UpdateCameraWindowStatus(uint32_t accessTokenId, bool isShowing)
{
    TLOGI(WmsLogTag::WMS_SYSTEM, "accessTokenId:%{private}u, isShowing:%{public}d", accessTokenId,
        static_cast<int>(isShowing));
    for (auto &agent: smAgentContainer_.GetAgentsByType(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_CAMERA_WINDOW)) {
        if (agent != nullptr) {
            agent->UpdateCameraWindowStatus(accessTokenId, isShowing);
        }
    }
}
} // namespace Rosen
} // namespace OHOS