/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "window_root.h"

#include <cinttypes>

#include "display_manager_service_inner.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WindowRoot"};
}
sptr<WindowNodeContainer> WindowRoot::GetOrCreateWindowNodeContainer(int32_t displayId)
{
    auto iter = windowNodeContainerMap_.find(displayId);
    if (iter != windowNodeContainerMap_.end()) {
        return iter->second;
    }
    auto abstractDisplay = DisplayManagerServiceInner::GetInstance().GetDisplayById(displayId);
    if (abstractDisplay == nullptr) {
        WLOGFE("get display failed displayId:%{public}d", displayId);
        return nullptr;
    }
    WLOGFI("create new window node container display width:%{public}d, height:%{public}d, screenId:%{public}" PRIu64"",
        abstractDisplay->GetWidth(), abstractDisplay->GetHeight(), abstractDisplay->GetId());

    UpdateFocusStatusFunc focusStatusFunc = std::bind(&WindowRoot::UpdateFocusStatus, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    UpdateSystemBarPropsFunc sysBarUpdateFunc = std::bind(&WindowRoot::UpdateSystemBarProperties, this,
        std::placeholders::_1, std::placeholders::_2);
    WindowNodeContainerCallbacks callbacks = {
        focusStatusFunc,
        sysBarUpdateFunc
    };
    sptr<WindowNodeContainer> container = new WindowNodeContainer(abstractDisplay->GetId(),
        static_cast<uint32_t>(abstractDisplay->GetWidth()), static_cast<uint32_t>(abstractDisplay->GetHeight()),
        callbacks);
    windowNodeContainerMap_.insert({ displayId, container });
    return container;
}

void WindowRoot::NotifyDisplayRemoved(int32_t displayId)
{
    auto container = GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFI("this display does not have any window");
        return;
    }
    std::vector<uint32_t> windowIds = container->Destroy();
    for (auto id : windowIds) {
        auto node = GetWindowNode(id);
        DestroyWindowInner(node);
    }
    windowNodeContainerMap_.erase(displayId);
}

sptr<WindowNode> WindowRoot::GetWindowNode(uint32_t windowId) const
{
    auto iter = windowNodeMap_.find(windowId);
    if (iter == windowNodeMap_.end()) {
        return nullptr;
    }
    return iter->second;
}

WMError WindowRoot::SaveWindow(const sptr<WindowNode>& node)
{
    if (node == nullptr) {
        WLOGFE("add window failed, node is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    WLOGFI("save windowId %{public}d", node->GetWindowId());
    windowNodeMap_.insert({ node->GetWindowId(), node });
    auto remoteObject = node->GetWindowToken()->AsObject();
    windowIdMap_.insert({ remoteObject, node->GetWindowId() });

    if (windowDeath_ == nullptr) {
        WLOGFI("failed to create death Recipient ptr WindowDeathRecipient");
        return WMError::WM_OK;
    }
    if (!remoteObject->AddDeathRecipient(windowDeath_)) {
        WLOGFI("failed to add death recipient");
    }
    return WMError::WM_OK;
}

WMError WindowRoot::MinimizeOtherFullScreenAbility(sptr<WindowNode>& node)
{
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("MinimizeAbility failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    return container->MinimizeOtherFullScreenAbility();
}

WMError WindowRoot::AddWindowNode(uint32_t parentId, sptr<WindowNode>& node)
{
    if (node == nullptr) {
        WLOGFE("add window failed, node is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto parentNode = GetWindowNode(parentId);

    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("add window failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }

    return container->AddWindowNode(node, parentNode);
}

WMError WindowRoot::RemoveWindowNode(uint32_t windowId)
{
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("add window failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    return container->RemoveWindowNode(node);
}

WMError WindowRoot::UpdateWindowNode(uint32_t windowId)
{
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("add window failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    return container->UpdateWindowNode(node);
}

WMError WindowRoot::DestroyWindow(uint32_t windowId)
{
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        return WMError::WM_ERROR_DESTROYED_OBJECT;
    }
    WMError res;
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container != nullptr) {
        std::vector<uint32_t> windowIds;
        res = container->DestroyWindowNode(node, windowIds);
        for (auto id : windowIds) {
            node = GetWindowNode(id);
            DestroyWindowInner(node);
        }
        return res;
    }
    res = DestroyWindowInner(node);
    WLOGFI("destroy window failed, window container could not be found");
    return res;
}

WMError WindowRoot::DestroyWindowInner(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        WLOGFE("window has been destroyed");
        return WMError::WM_ERROR_DESTROYED_OBJECT;
    }

    sptr<IWindow> window = node->GetWindowToken();
    if ((window != nullptr) && (window->AsObject() != nullptr)) {
        if (windowIdMap_.count(window->AsObject()) == 0) {
            WLOGFI("window remote object has been destroyed");
            return WMError::WM_ERROR_DESTROYED_OBJECT;
        }

        if (window->AsObject() != nullptr) {
            window->AsObject()->RemoveDeathRecipient(windowDeath_);
        }
        windowIdMap_.erase(window->AsObject());
    }

    windowNodeMap_.erase(node->GetWindowId());
    return WMError::WM_OK;
}

WMError WindowRoot::RequestFocus(uint32_t windowId)
{
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (!node->currentVisibility_) {
        WLOGFE("could not request focus before it has shown");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    return container->SetFocusWindow(windowId);
}

void WindowRoot::RegisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    windowManagerAgents_[type].push_back(windowManagerAgent);
    if (windowManagerAgentDeath_ == nullptr) {
        WLOGFI("failed to create death Recipient ptr WindowManagerAgentDeathRecipient");
        return;
    }
    if (!windowManagerAgent->AsObject()->AddDeathRecipient(windowManagerAgentDeath_)) {
        WLOGFI("failed to add death recipient");
    }
}

void WindowRoot::UnregisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    auto iter = std::find(windowManagerAgents_[type].begin(), windowManagerAgents_[type].end(), windowManagerAgent);
    if (iter == windowManagerAgents_[type].end()) {
        WLOGFE("could not find this listener");
        return;
    }
    windowManagerAgents_[type].erase(iter);
}

void WindowRoot::UnregisterWindowManagerAgent(const sptr<IRemoteObject>& object)
{
    for (auto agents : windowManagerAgents_) {
        for (auto iter = agents.second.begin(); iter < agents.second.end(); ++iter) {
            if ((*iter)->AsObject() != nullptr && (*iter)->AsObject() == object) {
                iter = agents.second.erase(iter);
            }
        }
    }
}

void WindowRoot::UpdateFocusStatus(uint32_t windowId, const sptr<IRemoteObject>& abilityToken, WindowType windowType,
    int32_t displayId, bool focused)
{
    for (auto& windowManagerAgent : windowManagerAgents_[WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS]) {
        windowManagerAgent->UpdateFocusStatus(windowId, abilityToken, windowType, displayId, focused);
    }
}

std::shared_ptr<RSSurfaceNode> WindowRoot::GetSurfaceNodeByAbilityToken(const sptr<IRemoteObject> &abilityToken) const
{
    for (auto iter = windowNodeMap_.begin(); iter != windowNodeMap_.end(); iter++) {
        if (iter->second->abilityToken_ != abilityToken) {
            continue;
        }
        return iter->second->surfaceNode_;
    }
    WLOGFE("could not find required abilityToken!");
    return nullptr;
}

void WindowRoot::UpdateSystemBarProperties(uint64_t displayId, const SystemBarProps& props)
{
    if (props.empty()) {
        return;
    }
    for (auto& agent : windowManagerAgents_[WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_SYSTEM_BAR]) {
        agent->UpdateSystemBarProperties(displayId, props);
    }
}

void WindowRoot::OnRemoteDied(const sptr<IRemoteObject>& remoteObject)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = windowIdMap_.find(remoteObject);
    if (iter == windowIdMap_.end()) {
        WLOGFE("window id could not be found");
        return;
    }
    uint32_t windowId = iter->second;
    callback_(Event::REMOTE_DIED, windowId);
}

void WindowRoot::ClearWindowManagerAgent(const sptr<IRemoteObject>& remoteObject)
{
    if (remoteObject == nullptr) {
        WLOGFI("remoteObject is null");
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    UnregisterWindowManagerAgent(remoteObject);
    remoteObject->RemoveDeathRecipient(windowManagerAgentDeath_);
}

void WindowDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    if (wptrDeath == nullptr) {
        WLOGFE("wptrDeath is null");
        return;
    }

    sptr<IRemoteObject> object = wptrDeath.promote();
    if (!object) {
        WLOGFE("object is null");
        return;
    }
    WLOGFI("WindowDeathRecipient callback");
    callback_(object);
}

void WindowManagerAgentDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    if (wptrDeath == nullptr) {
        WLOGFE("wptrDeath is null");
        return;
    }

    sptr<IRemoteObject> object = wptrDeath.promote();
    if (!object) {
        WLOGFE("object is null");
        return;
    }
    WLOGFI("WindowManagerAgentDeathRecipient callback");
    callback_(object);
}
}
}
