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
    auto displayScreen = DisplayManagerServiceInner::GetInstance()->GetDisplayById(displayId);
    if (displayScreen == nullptr) {
        WLOGFE("get display failed displayId:%{public}d", displayId);
        return nullptr;
    }
    WLOGFI("create new window node container display width:%{public}d, height:%{public}d, screenId:%{public}llu",
        displayScreen->GetWidth(), displayScreen->GetHeight(), displayScreen->GetId());
    sptr<WindowNodeContainer> container = new WindowNodeContainer(displayScreen->GetId(),
        static_cast<uint32_t>(displayScreen->GetWidth()), static_cast<uint32_t>(displayScreen->GetHeight()));
    windowNodeContainerMap_.insert({ displayId, container });
    return container;
}

const std::map<int32_t, sptr<WindowNodeContainer>>& WindowRoot::GetWindowNodeContainerMap() const
{
    return windowNodeContainerMap_;
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
        WLOGFE("window node could not be found");
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
    if (windowIdMap_.count(window->AsObject()) == 0) {
        WLOGFI("window remote object has been destroyed");
        return WMError::WM_ERROR_DESTROYED_OBJECT;
    }

    if (window->AsObject() != nullptr) {
        window->AsObject()->RemoveDeathRecipient(windowDeath_);
    }
    windowIdMap_.erase(window->AsObject());
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
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    return container->SetFocusWindow(windowId);
}

void WindowRoot::ClearWindow(const sptr<IRemoteObject>& remoteObject)
{
    auto iter = windowIdMap_.find(remoteObject);
    if (iter == windowIdMap_.end()) {
        WLOGFE("window id could not be found");
        return;
    }
    uint32_t windowId = iter->second;
    DestroyWindow(windowId);
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
    callback_(object);
}
}
}
