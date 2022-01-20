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

#include "window_manager_agent_controller.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WindowManagerAgentController"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(WindowManagerAgentController)

void WindowManagerAgentController::RegisterWindowManagerAgent(const sptr<IWindowManagerAgent>& windowManagerAgent,
    WindowManagerAgentType type)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    wmAgentContainer_.RegisterAgentLocked(windowManagerAgent, type);
}

void WindowManagerAgentController::UnregisterWindowManagerAgent(const sptr<IWindowManagerAgent>& windowManagerAgent,
    WindowManagerAgentType type)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    wmAgentContainer_.UnregisterAgentLocked(windowManagerAgent, type);
}

void WindowManagerAgentController::UpdateFocusStatus(uint32_t windowId, const sptr<IRemoteObject>& abilityToken,
    WindowType windowType, int32_t displayId, bool focused)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    WLOGFI("UpdateFocusStatus");
    for (auto& agent : wmAgentContainer_.GetAgentsByType(WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS)) {
        agent->UpdateFocusStatus(windowId, abilityToken, windowType, displayId, focused);
    }
}

void WindowManagerAgentController::UpdateSystemBarRegionTints(uint64_t displayId, const SystemBarRegionTints& tints)
{
    if (tints.empty()) {
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    WLOGFI("UpdateSystemBarRegionTints");
    for (auto& agent : wmAgentContainer_.GetAgentsByType(
        WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_SYSTEM_BAR)) {
        agent->UpdateSystemBarRegionTints(displayId, tints);
    }
}
}
}