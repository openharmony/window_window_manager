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
#include "window_helper.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowRoot"};
}
sptr<WindowNodeContainer> WindowRoot::GetOrCreateWindowNodeContainer(DisplayId displayId)
{
    auto iter = windowNodeContainerMap_.find(displayId);
    if (iter != windowNodeContainerMap_.end()) {
        return iter->second;
    }
    const sptr<AbstractDisplay> abstractDisplay = DisplayManagerServiceInner::GetInstance().GetDisplayById(displayId);
    if (abstractDisplay == nullptr) {
        WLOGFE("get display failed displayId:%{public}" PRId64 "", displayId);
        return nullptr;
    }

    if (!CheckDisplayInfo(abstractDisplay)) {
        WLOGFE("get display invailed infp:%{public}" PRId64 "", displayId);
        return nullptr;
    }

    WLOGFI("create new window node container display width:%{public}d, height:%{public}d, screenId:%{public}" PRIu64"",
        abstractDisplay->GetWidth(), abstractDisplay->GetHeight(), abstractDisplay->GetId());
    sptr<WindowNodeContainer> container = new WindowNodeContainer(abstractDisplay->GetId(),
        static_cast<uint32_t>(abstractDisplay->GetWidth()), static_cast<uint32_t>(abstractDisplay->GetHeight()));
    windowNodeContainerMap_.insert({ displayId, container });
    return container;
}

bool WindowRoot::CheckDisplayInfo(const sptr<AbstractDisplay>& display)
{
    const int32_t minWidth = 50;
    const int32_t minHeight = 50;
    const int32_t maxWidth = 7680;
    const int32_t maxHeight = 7680; // 8k resolution
    if (display->GetWidth() < minWidth || display->GetWidth() > maxWidth ||
        display->GetHeight() < minHeight || display->GetHeight() > maxHeight) {
        return false;
    }
    return true;
}

void WindowRoot::NotifyDisplayRemoved(DisplayId displayId)
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

WMError WindowRoot::MinimizeStructuredAppWindowsExceptSelf(sptr<WindowNode>& node)
{
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("MinimizeAbility failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    return container->MinimizeStructuredAppWindowsExceptSelf(node);
}

std::vector<Rect> WindowRoot::GetAvoidAreaByType(uint32_t windowId, AvoidAreaType avoidAreaType)
{
    std::vector<Rect> avoidArea;
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return avoidArea;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("add window failed, window container could not be found");
        return avoidArea;
    }
    avoidArea = container->GetAvoidAreaByType(avoidAreaType);
    return avoidArea;
}

void WindowRoot::MinimizeAllAppWindows(DisplayId displayId)
{
    auto container = GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("can't find window node container, failed!");
        return;
    }
    return container->MinimizeAllAppWindows();
}

WMError WindowRoot::AddWindowNode(uint32_t parentId, sptr<WindowNode>& node)
{
    if (node == nullptr) {
        WLOGFE("add window failed, node is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }

    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("add window failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }

    auto parentNode = GetWindowNode(parentId);
    WMError res = container->AddWindowNode(node, parentNode);
    if (WindowHelper::IsSubWindow(node->GetWindowType())) {
        if (parentNode == nullptr) {
            WLOGFE("window type is invalid");
            return WMError::WM_ERROR_INVALID_TYPE;
        }
        sptr<WindowNode> parent = nullptr;
        container->RaiseZOrderForAppWindow(parentNode, parent);
    }
    if (res == WMError::WM_OK && node->GetWindowProperty()->GetFocusable()) {
        container->SetFocusWindow(node->GetWindowId());
    }
    return res;
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
    if (windowId != container->GetFocusWindow()) {
        return container->RemoveWindowNode(node);
    }
    auto nextFocusableWindow = container->GetNextFocusableWindow(windowId);
    if (nextFocusableWindow != nullptr) {
        WLOGFI("adjust focus window, pre focus window id: %{public}u", nextFocusableWindow->GetWindowId());
        container->SetFocusWindow(nextFocusableWindow->GetWindowId());
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

WMError WindowRoot::EnterSplitWindowMode(sptr<WindowNode>& node)
{
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("Enter split window mode failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    return container->EnterSplitWindowMode(node);
}

WMError WindowRoot::ExitSplitWindowMode(sptr<WindowNode>& node)
{
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("Exit split window mode failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    return container->ExitSplitWindowMode(node);
}

WMError WindowRoot::DestroyWindow(uint32_t windowId, bool onlySelf)
{
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        return WMError::WM_ERROR_DESTROYED_OBJECT;
    }
    WMError res;
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container != nullptr) {
        if (onlySelf) {
            for (auto& child : node->children_) {
                child->parent_ = nullptr;
            }
            res = container->RemoveWindowNode(node);
            if (res != WMError::WM_OK) {
                WLOGFE("RemoveWindowNode failed");
            }
            return DestroyWindowInner(node);
        } else {
            std::vector<uint32_t> windowIds;
            res = container->DestroyWindowNode(node, windowIds);
            for (auto id : windowIds) {
                node = GetWindowNode(id);
                if (node != nullptr) {
                    DestroyWindowInner(node);
                }
            }
            return res;
        }
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

bool WindowRoot::isVerticalDisplay(sptr<WindowNode>& node) const
{
    auto container = const_cast<WindowRoot*>(this)->GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("get display direction failed, window container could not be found");
        return false;
    }
    return container->isVerticalDisplay();
}

WMError WindowRoot::RequestFocus(uint32_t windowId)
{
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (!node->currentVisibility_) {
        WLOGFE("could not request focus before it does not be shown");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (node->GetWindowProperty()->GetFocusable()) {
        return container->SetFocusWindow(windowId);
    }
    return WMError::WM_ERROR_INVALID_OPERATION;
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

void WindowRoot::NotifyWindowStateChange(WindowState state, WindowStateChangeReason reason)
{
    for (auto& elem : windowNodeContainerMap_) {
        if (elem.second == nullptr) {
            continue;
        }
        elem.second->NotifyWindowStateChange(state, reason);
    }
}

void WindowRoot::NotifyDisplayChange(sptr<AbstractDisplay> abstractDisplay)
{
    WLOGFD("window should be updated for display changed");
    auto container = GetOrCreateWindowNodeContainer(abstractDisplay->GetId());
    if (container == nullptr) {
        WLOGFE("can't find window node container, failed!");
        return;
    }
    container->UpdateDisplayRect(abstractDisplay->GetWidth(), abstractDisplay->GetHeight());
}

WMError WindowRoot::RaiseZOrderForAppWindow(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        WLOGFW("add window failed, node is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (!WindowHelper::IsAppWindow(node->GetWindowType())) {
        WLOGFW("window is not app window");
        return WMError::WM_ERROR_INVALID_TYPE;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFW("add window failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }

    auto parentNode = GetWindowNode(node->GetParentId());
    return container->RaiseZOrderForAppWindow(node, parentNode);
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

WMError WindowRoot::GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId)
{
    if (windowNodeMap_.find(mainWinId) == windowNodeMap_.end()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    auto node = windowNodeMap_[mainWinId];
    if (!node->currentVisibility_) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (!node->children_.empty()) {
        auto iter = node->children_.rbegin();
        if (WindowHelper::IsSubWindow((*iter)->GetWindowType())) {
            topWinId = (*iter)->GetWindowId();
            return WMError::WM_OK;
        }
    }
    topWinId = mainWinId;
    return WMError::WM_OK;
}

WMError WindowRoot::SetWindowLayoutMode(DisplayId displayId, WindowLayoutMode mode)
{
    auto container = GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    WMError ret = container->SwitchLayoutPolicy(mode, true);
    if (ret != WMError::WM_OK) {
        WLOGFW("set windoe layout mode failed displayId: %{public}" PRId64 ", ret: %{public}d", displayId, ret);
    }
    return ret;
}
}
}
