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

#include "accessibility_connection.h"

#include "display_manager_service_inner.h"
#include "window_manager.h"
#include "window_manager_agent_controller.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "AccessibilityConnection"};
}

void AccessibilityConnection::NotifyAccessibilityInfo(const sptr<WindowNode>& node, WindowUpdateType type)
{
    auto container = windowRoot_->GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (node == nullptr || container == nullptr) {
        WLOGFE("window node or container is null");
        return;
    }
    bool focusChange = false;
    uint32_t focusWindowId = container->GetFocusWindow();
    auto iter = focusedWindowMap_.find(container);
    if (iter == focusedWindowMap_.end()) {
        focusedWindowMap_.insert(std::make_pair(container, focusWindowId));
        if (focusWindowId != INVALID_WINDOW_ID) {
            focusChange = true;
        }
    } else {
        if (focusWindowId != iter->second) {
            focusChange = true;
            iter->second = focusWindowId;
        }
    }
    WLOGFI("notify accessibility window info: [%{public}u %{public}u %{public}d %{public}d]",
        node->GetWindowId(), focusWindowId, focusChange, static_cast<int32_t>(type));
    // need handle focus change
    switch (type) {
        case WindowUpdateType::WINDOW_UPDATE_ADDED:
            if (node->currentVisibility_) {
                NotifyAccessibilityWindowInfo(container, node, type);
            }
            break;
        case WindowUpdateType::WINDOW_UPDATE_REMOVED:
            NotifyAccessibilityWindowInfo(container, node, type);
            break;
        case WindowUpdateType::WINDOW_UPDATE_PROPERTY:
            NotifyAccessibilityWindowInfo(container, node, type);
            break;
        default:
            break;
    }
    if (focusChange) {
        auto focusWindowNode = windowRoot_->GetWindowNode(focusWindowId);
        if (focusWindowNode == nullptr) {
            WLOGFE("could not find window");
            return;
        }
        NotifyAccessibilityWindowInfo(container, focusWindowNode, WindowUpdateType::WINDOW_UPDATE_FOCUSED);
    }
}

void AccessibilityConnection::NotifyAccessibilityWindowInfo(const sptr<WindowNodeContainer>& container,
    const sptr<WindowNode>& node, WindowUpdateType type) const
{
    sptr<WindowInfo> windowInfo = new (std::nothrow) WindowInfo();
    sptr<AccessibilityWindowInfo> accessibilityWindowInfo = new (std::nothrow) AccessibilityWindowInfo();
    if (windowInfo != nullptr && accessibilityWindowInfo != nullptr) {
        FillWindowInfo(node, container->GetFocusWindow(), windowInfo);
        accessibilityWindowInfo->currentWindowInfo_ = windowInfo;
        GetWindowList(container, accessibilityWindowInfo->windowList_);
        WindowManagerAgentController::GetInstance().NotifyAccessibilityWindowInfo(accessibilityWindowInfo, type);
    }
}

void AccessibilityConnection::GetWindowList(const sptr<WindowNodeContainer>& container,
    std::vector<sptr<WindowInfo>>& windowList) const
{
    std::vector<sptr<WindowNode>> nodes;
    container->TraverseContainer(nodes);
    for (auto& node : nodes) {
        sptr<WindowInfo> windowInfo = new (std::nothrow) WindowInfo();
        if (windowInfo != nullptr) {
            FillWindowInfo(node, container->GetFocusWindow(), windowInfo);
            windowList.emplace_back(windowInfo);
        }
    }
}

void AccessibilityConnection::FillWindowInfo(const sptr<WindowNode>& node, uint32_t focusedWindow,
    sptr<WindowInfo>& windowInfo) const
{
    windowInfo->wid_ = static_cast<int32_t>(node->GetWindowId());
    windowInfo->windowRect_ = node->GetWindowRect();
    windowInfo->focused_ = node->GetWindowId() == focusedWindow;
    windowInfo->displayId_ = node->GetDisplayId();
    windowInfo->mode_ = node->GetWindowMode();
    windowInfo->type_ = node->GetWindowType();
    auto property = node->GetWindowProperty();
    if (property != nullptr) {
        windowInfo->isDecorEnable_ = property->GetDecorEnable();
    }
}

void AccessibilityConnection::GetAccessibilityWindowInfo(sptr<AccessibilityWindowInfo>& windowInfo) const
{
    std::map<ScreenId, sptr<WindowNodeContainer>> windowNodeContainers;
    std::vector<DisplayId> displayIds = DisplayManagerServiceInner::GetInstance().GetAllDisplayIds();
    for (DisplayId displayId : displayIds) {
        ScreenId screenGroupId = DisplayManagerServiceInner::GetInstance().GetScreenGroupIdByDisplayId(displayId);
        auto container = windowRoot_->GetOrCreateWindowNodeContainer(displayId);
        if (windowNodeContainers.count(screenGroupId) == 0 && container != nullptr) {
            windowNodeContainers.insert(std::make_pair(screenGroupId, container));
            GetWindowList(container, windowInfo->windowList_);
        }
    }
}
}
