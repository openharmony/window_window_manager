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

#include "window_root.h"

#include <cinttypes>
#include <display_power_mgr_client.h>
#include <hisysevent.h>
#include <transaction/rs_transaction.h>
#include "display_manager_service_inner.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "window_manager_service.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowRoot"};
}

ScreenId WindowRoot::GetScreenGroupId(DisplayId displayId, bool& isRecordedDisplay)
{
    for (auto iter : displayIdMap_) {
        auto displayIdVec = iter.second;
        if (std::find(displayIdVec.begin(), displayIdVec.end(), displayId) != displayIdVec.end()) {
            isRecordedDisplay = true;
            return iter.first;
        }
    }
    isRecordedDisplay = false;
    WLOGFE("Current display is not be recorded, displayId: %{public}" PRIu64 "", displayId);
    return DisplayManagerServiceInner::GetInstance().GetScreenGroupIdByDisplayId(displayId);
}

sptr<WindowNodeContainer> WindowRoot::GetOrCreateWindowNodeContainer(DisplayId displayId)
{
    bool isRecordedDisplay;
    ScreenId screenGroupId = GetScreenGroupId(displayId, isRecordedDisplay);
    auto iter = windowNodeContainerMap_.find(screenGroupId);
    if (iter != windowNodeContainerMap_.end()) {
        // if container exist for screenGroup and display is not be recorded, process expand display
        if (!isRecordedDisplay) {
            ProcessExpandDisplayCreate(displayId, screenGroupId);
        }
        return iter->second;
    }

    // In case of have no container for default display, create container
    WLOGFE("Create container for current display, displayId: %{public}" PRIu64 "", displayId);
    return CreateWindowNodeContainer(displayId);
}

sptr<WindowNodeContainer> WindowRoot::CreateWindowNodeContainer(DisplayId displayId)
{
    const sptr<DisplayInfo> displayInfo = DisplayManagerServiceInner::GetInstance().GetDisplayById(displayId);
    if (displayInfo == nullptr || !CheckDisplayInfo(displayInfo)) {
        WLOGFE("get display failed or get invailed display info, displayId :%{public}" PRIu64 "", displayId);
        return nullptr;
    }

    ScreenId screenGroupId = DisplayManagerServiceInner::GetInstance().GetScreenGroupIdByDisplayId(displayId);
    WLOGFI("create new container for display, width: %{public}d, height: %{public}d, isMinimized:%{public}d, "
        "screenGroupId:%{public}" PRIu64", displayId:%{public}" PRIu64"", displayInfo->GetWidth(),
        displayInfo->GetHeight(), isMinimizedByOtherWindow_, screenGroupId, displayId);
    sptr<WindowNodeContainer> container = new WindowNodeContainer(displayInfo);
    container->SetMinimizedByOther(isMinimizedByOtherWindow_);
    windowNodeContainerMap_.insert(std::make_pair(screenGroupId, container));
    std::vector<DisplayId> displayVec = { displayId };
    displayIdMap_.insert(std::make_pair(screenGroupId, displayVec));
    return container;
}

bool WindowRoot::CheckDisplayInfo(const sptr<DisplayInfo>& display)
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

void WindowRoot::NotifyKeyboardSizeChangeInfo(const sptr<WindowNode>& node,
    const sptr<WindowNodeContainer>& container, Rect rect)
{
    if (node == nullptr || container == nullptr) {
        WLOGFE("invalid parameter");
        return;
    }

    if (node->GetWindowType() != WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        return;
    }

    auto callingWindow = GetWindowNode(node->GetCallingWindow());
    if (callingWindow == nullptr) {
        WLOGFI("callingWindow: %{public}u does not be set", node->GetCallingWindow());
        callingWindow = GetWindowNode(container->GetFocusWindow());
    }
    if (callingWindow != nullptr && callingWindow->GetWindowToken() != nullptr &&
        (callingWindow->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN ||
        callingWindow->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_PRIMARY ||
        callingWindow->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_SECONDARY)) {
        WLOGFI("keyboard size change callingWindow: [%{public}s, %{public}u], " \
            "input rect: [%{public}d, %{public}d, %{public}u, %{public}u]",
            callingWindow->GetWindowName().c_str(), callingWindow->GetWindowId(),
            rect.posX_, rect.posY_, rect.width_, rect.height_);
        sptr<OccupiedAreaChangeInfo> info = new OccupiedAreaChangeInfo(OccupiedAreaType::TYPE_INPUT, rect);
        callingWindow->GetWindowToken()->UpdateOccupiedAreaChangeInfo(info);
        return;
    }
    WLOGFE("does not have correct callingWindow for input method window");
}

sptr<WindowNode> WindowRoot::GetWindowNode(uint32_t windowId) const
{
    auto iter = windowNodeMap_.find(windowId);
    if (iter == windowNodeMap_.end()) {
        return nullptr;
    }
    return iter->second;
}

sptr<WindowNode> WindowRoot::FindWindowNodeWithToken(const sptr<IRemoteObject>& token) const
{
    if (token == nullptr) {
        WLOGFE("token is null");
        return nullptr;
    }
    auto iter = std::find_if(windowNodeMap_.begin(), windowNodeMap_.end(),
        [token](const std::map<uint32_t, sptr<WindowNode>>::value_type& pair) {
            if (!(WindowHelper::IsSubWindow(pair.second->GetWindowType()))) {
                return pair.second->abilityToken_ == token;
            }
            return false;
        });
    if (iter == windowNodeMap_.end()) {
        WLOGFE("cannot find windowNode");
        return nullptr;
    }
    return iter->second;
}

void WindowRoot::AddDeathRecipient(sptr<WindowNode> node)
{
    if (node == nullptr) {
        WLOGFE("AddDeathRecipient failed, node is nullptr");
        return;
    }

    auto remoteObject = node->GetWindowToken()->AsObject();
    windowIdMap_.insert(std::make_pair(remoteObject, node->GetWindowId()));

    if (windowDeath_ == nullptr) {
        WLOGFI("failed to create death Recipient ptr WindowDeathRecipient");
        return;
    }
    if (!remoteObject->AddDeathRecipient(windowDeath_)) {
        WLOGFI("failed to add death recipient");
    }
}

WMError WindowRoot::SaveWindow(const sptr<WindowNode>& node)
{
    if (node == nullptr) {
        WLOGFE("add window failed, node is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }

    WLOGFI("save windowId %{public}u", node->GetWindowId());
    windowNodeMap_.insert(std::make_pair(node->GetWindowId(), node));
    if (node->GetWindowToken()) {
        AddDeathRecipient(node);
        node->GetWindowToken()->UpdateWindowStretchable(isWindowStretchable_);
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
    avoidArea = container->GetAvoidAreaByType(avoidAreaType, node->GetDisplayId());
    return avoidArea;
}

void WindowRoot::MinimizeAllAppWindows(DisplayId displayId)
{
    auto container = GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("can't find window node container, failed!");
        return;
    }
    return container->MinimizeAllAppWindows(displayId);
}

void WindowRoot::ToggleShownStateForAllAppWindows()
{
    std::vector<DisplayId> displays = DisplayManagerServiceInner::GetInstance().GetAllDisplayIds();
    std::vector<sptr<WindowNodeContainer>> containers;
    bool isAllAppWindowsEmpty = true;
    for (auto displayId : displays) {
        auto container = GetOrCreateWindowNodeContainer(displayId);
        if (container == nullptr) {
            WLOGFE("can't find window node container, failed!");
            continue;
        }
        containers.emplace_back(container);
        isAllAppWindowsEmpty = isAllAppWindowsEmpty && container->IsAppWindowsEmpty();
    }
    std::for_each(containers.begin(), containers.end(),
        [this, isAllAppWindowsEmpty] (sptr<WindowNodeContainer> container) {
        std::function<bool(uint32_t)> restoreFunc = [this](uint32_t windowId) {
            auto windowNode = GetWindowNode(windowId);
            if (windowNode == nullptr) {
                return false;
            }
            windowNode->GetWindowToken()->UpdateWindowState(WindowState::STATE_SHOWN);
            auto property = windowNode->GetWindowToken()->GetWindowProperty();
            if (property == nullptr) {
                return false;
            }
            WindowManagerService::GetInstance().AddWindow(property);
            return true;
        };
        if (isAllAppWindowsEmpty) {
            container->ToggleShownStateForAllAppWindows(restoreFunc, true);
        } else {
            container->ToggleShownStateForAllAppWindows(restoreFunc, false);
        }
    });
}

WMError WindowRoot::MaxmizeWindow(uint32_t windowId)
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
    auto property = node->GetWindowProperty();
    uint32_t flags = property->GetWindowFlags() & (~(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID)));
    property->SetWindowFlags(flags);
    container->NotifySystemBarDismiss(node);
    return WMError::WM_OK;
}

WMError WindowRoot::AddWindowNode(uint32_t parentId, sptr<WindowNode>& node, bool fromRemoteAnimation)
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
    if (fromRemoteAnimation) {
        return container->ShowInTransition(node);
    }
    // limit number of main window
    int mainWindowNumber = container->GetWindowCountByType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    if (mainWindowNumber >= maxAppWindowNumber_) {
        container->MinimizeOldestAppWindow();
    }

    auto parentNode = GetWindowNode(parentId);
    WMError res = container->AddWindowNode(node, parentNode);
    if (res == WMError::WM_OK && WindowHelper::IsSubWindow(node->GetWindowType())) {
        if (parentNode == nullptr) {
            WLOGFE("window type is invalid");
            return WMError::WM_ERROR_INVALID_TYPE;
        }
        sptr<WindowNode> parent = nullptr;
        container->RaiseZOrderForAppWindow(parentNode, parent);
    }
    if (res == WMError::WM_OK && node->GetWindowProperty()->GetFocusable()) {
        container->SetFocusWindow(node->GetWindowId());
        needCheckFocusWindow = true;
    }
    if (res == WMError::WM_OK) {
        container->SetActiveWindow(node->GetWindowId(), false);
        NotifyKeyboardSizeChangeInfo(node, container, node->GetWindowRect());
        for (auto& child : node->children_) {
            if (child == nullptr || !child->currentVisibility_) {
                break;
            }
            HandleKeepScreenOn(child->GetWindowId(), child->IsKeepScreenOn());
        }
        HandleKeepScreenOn(node->GetWindowId(), node->IsKeepScreenOn());
    }
    WLOGFI("windowId:%{public}u, name:%{public}s, orientation:%{public}u, type:%{public}u, isMainWindow:%{public}d",
        node->GetWindowId(), node->GetWindowName().c_str(), static_cast<uint32_t>(node->GetRequestedOrientation()),
        node->GetWindowType(), WindowHelper::IsMainWindow(node->GetWindowType()));
    if (res == WMError::WM_OK && WindowHelper::IsMainWindow(node->GetWindowType()) &&
        node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN) {
        DisplayManagerServiceInner::GetInstance().
            SetOrientationFromWindow(node->GetDisplayId(), node->GetRequestedOrientation());
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
        WLOGFE("remove window failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    container->DropShowWhenLockedWindowIfNeeded(node);
    UpdateFocusWindowWithWindowRemoved(node, container);
    UpdateActiveWindowWithWindowRemoved(node, container);
    UpdateBrightnessWithWindowRemoved(windowId, container);
    WMError res = container->RemoveWindowNode(node);
    if (res == WMError::WM_OK) {
        Rect rect = { 0, 0, 0, 0 };
        NotifyKeyboardSizeChangeInfo(node, container, rect);
        for (auto& child : node->children_) {
            if (child == nullptr) {
                break;
            }
            HandleKeepScreenOn(child->GetWindowId(), false);
        }
        HandleKeepScreenOn(windowId, false);
    }
    return res;
}

WMError WindowRoot::UpdateWindowNode(uint32_t windowId, WindowUpdateReason reason)
{
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("update window failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    return container->UpdateWindowNode(node, reason);
}

WMError WindowRoot::UpdateSizeChangeReason(uint32_t windowId, WindowSizeChangeReason reason)
{
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("update window size change reason failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    container->UpdateSizeChangeReason(node, reason);
    return WMError::WM_OK;
}

void WindowRoot::SetBrightness(uint32_t windowId, float brightness)
{
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("set brightness failed, window container could not be found");
        return;
    }
    if (!WindowHelper::IsAppWindow(node->GetWindowType())) {
        WLOGFI("non app window does not support set brightness");
        return;
    }
    if (windowId == container->GetActiveWindow()) {
        if (container->GetDisplayBrightness() != brightness) {
            WLOGFI("set brightness with value: %{public}u", container->ToOverrideBrightness(brightness));
            DisplayPowerMgr::DisplayPowerMgrClient::GetInstance().OverrideBrightness(
                container->ToOverrideBrightness(brightness));
            container->SetDisplayBrightness(brightness);
        }
        container->SetBrightnessWindow(windowId);
    }
}

void WindowRoot::HandleKeepScreenOn(uint32_t windowId, bool requireLock)
{
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("handle keep screen on failed, window container could not be found");
        return;
    }
    container->HandleKeepScreenOn(node, requireLock);
}

void WindowRoot::UpdateFocusableProperty(uint32_t windowId)
{
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("handle focusable failed, window container could not be found");
        return;
    }

    if (windowId != container->GetFocusWindow() || node->GetWindowProperty()->GetFocusable()) {
        return;
    }
    auto nextFocusableWindow = container->GetNextFocusableWindow(windowId);
    if (nextFocusableWindow != nullptr) {
        WLOGFI("adjust focus window, next focus window id: %{public}u", nextFocusableWindow->GetWindowId());
        container->SetFocusWindow(nextFocusableWindow->GetWindowId());
    }
}

WMError WindowRoot::SetWindowMode(sptr<WindowNode>& node, WindowMode dstMode)
{
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("set window mode failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    return container->SetWindowMode(node, dstMode);
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
        UpdateFocusWindowWithWindowRemoved(node, container);
        UpdateActiveWindowWithWindowRemoved(node, container);
        UpdateBrightnessWithWindowRemoved(windowId, container);
        HandleKeepScreenOn(windowId, false);
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
                    HandleKeepScreenOn(id, false);
                    DestroyWindowInner(node);
                }
            }
            if (res == WMError::WM_OK) {
                Rect rect = { 0, 0, 0, 0 };
                NotifyKeyboardSizeChangeInfo(node, container, rect);
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

void WindowRoot::UpdateFocusWindowWithWindowRemoved(const sptr<WindowNode>& node,
    const sptr<WindowNodeContainer>& container) const
{
    if (node == nullptr || container == nullptr) {
        WLOGFE("window is invalid");
        return;
    }
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        WLOGFI("window is divider, do not get next focus window.");
        return;
    }
    uint32_t windowId = node->GetWindowId();
    uint32_t focusedWindowId = container->GetFocusWindow();
    WLOGFI("current window: %{public}u, focus window: %{public}u", windowId, focusedWindowId);
    if (WindowHelper::IsMainWindow(node->GetWindowType())) {
        if (windowId != focusedWindowId) {
            auto iter = std::find_if(node->children_.begin(), node->children_.end(),
                                     [focusedWindowId](sptr<WindowNode> node) {
                                         return node->GetWindowId() == focusedWindowId;
                                     });
            if (iter == node->children_.end()) {
                return;
            }
        }
        if (!node->children_.empty()) {
            auto firstChild = node->children_.front();
            if (firstChild->priority_ < 0) {
                windowId = firstChild->GetWindowId();
            }
        }
    } else {
        if (windowId != focusedWindowId) {
            return;
        }
    }
    auto nextFocusableWindow = container->GetNextFocusableWindow(windowId);
    if (nextFocusableWindow != nullptr) {
        WLOGFI("adjust focus window, next focus window id: %{public}u", nextFocusableWindow->GetWindowId());
        container->SetFocusWindow(nextFocusableWindow->GetWindowId());
    }
}

void WindowRoot::UpdateActiveWindowWithWindowRemoved(const sptr<WindowNode>& node,
    const sptr<WindowNodeContainer>& container) const
{
    if (node == nullptr || container == nullptr) {
        WLOGFE("window is invalid");
        return;
    }
    uint32_t windowId = node->GetWindowId();
    uint32_t activeWindowId = container->GetActiveWindow();
    WLOGFI("current window: %{public}u, active window: %{public}u", windowId, activeWindowId);
    if (WindowHelper::IsMainWindow(node->GetWindowType())) {
        if (windowId != activeWindowId) {
            auto iter = std::find_if(node->children_.begin(), node->children_.end(),
                                     [activeWindowId](sptr<WindowNode> node) {
                                         return node->GetWindowId() == activeWindowId;
                                     });
            if (iter == node->children_.end()) {
                return;
            }
        }
        if (!node->children_.empty()) {
            auto firstChild = node->children_.front();
            if (firstChild->priority_ < 0) {
                windowId = firstChild->GetWindowId();
            }
        }
    } else {
        if (windowId != activeWindowId) {
            return;
        }
    }
    auto nextActiveWindow = container->GetNextActiveWindow(windowId);
    if (nextActiveWindow != nullptr) {
        WLOGFI("adjust active window, next active window id: %{public}u", nextActiveWindow->GetWindowId());
        container->SetActiveWindow(nextActiveWindow->GetWindowId(), true);
    }
}

void WindowRoot::UpdateBrightnessWithWindowRemoved(uint32_t windowId, const sptr<WindowNodeContainer>& container) const
{
    if (container == nullptr) {
        WLOGFE("window container could not be found");
        return;
    }
    if (windowId == container->GetBrightnessWindow()) {
        WLOGFI("adjust brightness window with active window: %{public}u", container->GetActiveWindow());
        container->UpdateBrightness(container->GetActiveWindow(), true);
    }
}

bool WindowRoot::isVerticalDisplay(sptr<WindowNode>& node) const
{
    auto container = const_cast<WindowRoot*>(this)->GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("get display direction failed, window container could not be found");
        return false;
    }
    return container->isVerticalDisplay(node->GetDisplayId());
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

WMError WindowRoot::RequestActiveWindow(uint32_t windowId)
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
    auto res =  container->SetActiveWindow(windowId, false);
    WLOGFI("windowId:%{public}u, name:%{public}s, orientation:%{public}u, type:%{public}u, isMainWindow:%{public}d",
        windowId, node->GetWindowName().c_str(), static_cast<uint32_t>(node->GetRequestedOrientation()),
        node->GetWindowType(), WindowHelper::IsMainWindow(node->GetWindowType()));
    if (res == WMError::WM_OK && WindowHelper::IsMainWindow(node->GetWindowType()) &&
        node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN) {
        DisplayManagerServiceInner::GetInstance().
            SetOrientationFromWindow(node->GetDisplayId(), node->GetRequestedOrientation());
    }
    return res;
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

void WindowRoot::ProcessWindowStateChange(WindowState state, WindowStateChangeReason reason)
{
    for (auto& elem : windowNodeContainerMap_) {
        if (elem.second == nullptr) {
            continue;
        }
        elem.second->ProcessWindowStateChange(state, reason);
    }
}

void WindowRoot::NotifySystemBarTints()
{
    WLOGFD("notify current system bar tints");
    for (auto& it : windowNodeContainerMap_) {
        if (it.second != nullptr) {
            it.second->NotifySystemBarTints(displayIdMap_[it.first]);
        }
    }
}

WMError WindowRoot::RaiseZOrderForAppWindow(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        WLOGFW("add window failed, node is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
        if (container == nullptr) {
            WLOGFW("window container could not be found");
            return WMError::WM_ERROR_NULLPTR;
        }
        container->RaiseSplitRelatedWindowToTop(node);
        return WMError::WM_OK;
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
    WMError ret = container->SwitchLayoutPolicy(mode, displayId, true);
    if (ret != WMError::WM_OK) {
        WLOGFW("set window layout mode failed displayId: %{public}" PRIu64 ", ret: %{public}d", displayId, ret);
    }
    return ret;
}

std::vector<DisplayId> WindowRoot::GetAllDisplayIds() const
{
    std::vector<DisplayId> displayIds;
    for (auto& it : windowNodeContainerMap_) {
        if (!it.second) {
            return std::vector<DisplayId>();
        }
        std::vector<DisplayId>& displayIdVec = const_cast<WindowRoot*>(this)->displayIdMap_[it.first];
        for (auto displayId : displayIdVec) {
            displayIds.push_back(displayId);
        }
    }
    return displayIds;
}

std::string WindowRoot::GenAllWindowsLogInfo() const
{
    std::ostringstream os;
    WindowNodeOperationFunc func = [&os](sptr<WindowNode> node) {
        if (node == nullptr) {
            WLOGE("WindowNode is nullptr");
            return false;
        }
        os<<"window_name:"<<node->GetWindowName()<<",id:"<<node->GetWindowId()<<
           ",focusable:"<<node->GetWindowProperty()->GetFocusable()<<";";
        return false;
    };

    for (auto& elem : windowNodeContainerMap_) {
        if (elem.second == nullptr) {
            continue;
        }
        std::vector<DisplayId>& displayIdVec = const_cast<WindowRoot*>(this)->displayIdMap_[elem.first];
        for (auto& displayId : displayIdVec) {
            os << "Display " << displayId << ":";
        }
        elem.second->TraverseWindowTree(func, true);
    }
    return os.str();
}

void WindowRoot::FocusFaultDetection() const
{
    if (!needCheckFocusWindow) {
        return;
    }
    bool needReport = true;
    uint32_t focusWinId = INVALID_WINDOW_ID;
    for (auto& elem : windowNodeContainerMap_) {
        if (elem.second == nullptr) {
            continue;
        }
        focusWinId = elem.second->GetFocusWindow();
        if (focusWinId != INVALID_WINDOW_ID) {
            needReport = false;
            sptr<WindowNode> windowNode = GetWindowNode(focusWinId);
            if (windowNode == nullptr || !windowNode->currentVisibility_) {
                needReport = true;
                WLOGFE("The focus windowNode is nullptr or is invisible, focusWinId: %{public}u", focusWinId);
                break;
            }
        }
    }
    if (needReport) {
        std::string windowLog(GenAllWindowsLogInfo());
        WLOGFE("The focus window is faulty, focusWinId:%{public}u, %{public}s", focusWinId, windowLog.c_str());
        int32_t ret = OHOS::HiviewDFX::HiSysEvent::Write(
            OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
            "NO_FOCUS_WINDOW",
            OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
            "PID", getpid(),
            "UID", getuid(),
            "PACKAGE_NAME", "foundation",
            "PROCESS_NAME", "foundation",
            "MSG", windowLog);
        if (ret != 0) {
            WLOGFE("Write HiSysEvent error, ret:%{public}d", ret);
        }
    }
}

void WindowRoot::ProcessExpandDisplayCreate(DisplayId displayId, ScreenId screenGroupId)
{
    const sptr<DisplayInfo> displayInfo = DisplayManagerServiceInner::GetInstance().GetDisplayById(displayId);
    if (displayInfo == nullptr || !CheckDisplayInfo(displayInfo)) {
        WLOGFE("get display failed or get invailed display info, displayId :%{public}" PRIu64 "", displayId);
        return;
    }
    auto container = windowNodeContainerMap_[screenGroupId];
    if (container == nullptr) {
        WLOGFE("window node container is nullptr, displayId :%{public}" PRIu64 "", displayId);
        return;
    }
    // add displayId in displayIdMap
    displayIdMap_[screenGroupId].push_back(displayId);
    container->ProcessDisplayCreate(displayInfo);
    WLOGFI("[Display Create] Container exist, add new display, displayId: %{public}" PRIu64", Rect: ["
        "%{public}d, %{public}d, %{public}u, %{public}u]", displayId, displayInfo->GetOffsetX(),
        displayInfo->GetOffsetY(), displayInfo->GetWidth(), displayInfo->GetHeight());
}

void WindowRoot::ProcessDisplayCreate(DisplayId displayId)
{
    ScreenId screenGroupId = DisplayManagerServiceInner::GetInstance().GetScreenGroupIdByDisplayId(displayId);
    auto iter = windowNodeContainerMap_.find(screenGroupId);
    if (iter == windowNodeContainerMap_.end()) {
        CreateWindowNodeContainer(displayId);
        WLOGFI("[Display Create] Create new container for display, displayId: %{public}" PRIu64"", displayId);
    } else {
        if (std::find(displayIdMap_[screenGroupId].begin(), displayIdMap_[screenGroupId].end(), displayId) !=
            displayIdMap_[screenGroupId].end()) {
            WLOGFI("[Display Create] Current display is already exist, displayId: %{public}" PRIu64"", displayId);
            return;
        }
        ProcessExpandDisplayCreate(displayId, screenGroupId);
    }
}

void WindowRoot::ProcessDisplayDestroy(DisplayId displayId)
{
    WLOGFI("[Display Destroy] displayId: %{public}" PRIu64" ", displayId);
}

void WindowRoot::ProcessDisplayChange(const sptr<DisplayInfo>& displayInfo, DisplayStateChangeType type)
{
    DisplayId displayId = displayInfo->GetDisplayId();
    ScreenId screenGroupId = DisplayManagerServiceInner::GetInstance().GetScreenGroupIdByDisplayId(displayId);
    auto& displayIdVec = displayIdMap_[screenGroupId];
    auto iter = windowNodeContainerMap_.find(screenGroupId);
    if (iter == windowNodeContainerMap_.end() || std::find(displayIdVec.begin(),
        displayIdVec.end(), displayId) == displayIdVec.end()) {
        WLOGFE("[Display Change] could not find display, change failed, displayId: %{public}" PRIu64"", displayId);
        return;
    }
    // container process display change
    auto container = iter->second;
    if (container == nullptr) {
        WLOGFE("window node container is nullptr, displayId :%{public}" PRIu64 "", displayId);
    }

    switch (type) {
        case DisplayStateChangeType::SIZE_CHANGE:
        case DisplayStateChangeType::UPDATE_ROTATION: {
            WLOGFI("update display: %{public}" PRIu64" rotation", displayId);
            container->SetDisplayOrientation(displayId, displayInfo->GetOrientation());
            container->SetDisplaySize(displayId, displayInfo->GetWidth(), displayInfo->GetHeight());
            break;
        }
        case DisplayStateChangeType::VIRTUAL_PIXEL_RATIO_CHANGE: {
            WLOGFI("update display: %{public}" PRIu64" virtual pixel ratio", displayId);
            container->SetDisplayVirtualPixelRatio(displayId, displayInfo->GetVirtualPixelRatio());
            break;
        }
        default: {
            break;
        }
    }
}

float WindowRoot::GetVirtualPixelRatio(DisplayId displayId) const
{
    auto container = const_cast<WindowRoot*>(this)->GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("window container could not be found");
        return 1.0;  // Use DefaultVPR 1.0
    }
    return container->GetDisplayVirtualPixelRatio(displayId);
}

WMError WindowRoot::GetAccessibilityWindowInfo(sptr<AccessibilityWindowInfo>& windowInfo)
{
    for (auto iter = windowNodeContainerMap_.begin(); iter != windowNodeContainerMap_.end(); ++iter) {
        auto container = iter->second;
        std::vector<sptr<WindowInfo>> windowList;
        container->GetWindowList(windowList);
        for (auto window : windowList) {
            windowInfo->windowList_.emplace_back(window);
        }
    }
    return WMError::WM_OK;
}

void WindowRoot::SetMaxAppWindowNumber(int windowNum)
{
    maxAppWindowNumber_ = windowNum;
}

void WindowRoot::SetMinimizedByOtherWindow(bool isMinimizedByOtherWindow)
{
    WLOGFI("isMinimizedByOtherWindow:%{public}d", isMinimizedByOtherWindow);
    isMinimizedByOtherWindow_ = isMinimizedByOtherWindow;
}

WMError WindowRoot::GetModeChangeHotZones(DisplayId displayId,
    ModeChangeHotZones& hotZones, const ModeChangeHotZonesConfig& config)
{
    auto container = GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("GetModeChangeHotZones failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    container->GetModeChangeHotZones(displayId, hotZones, config);
    return WMError::WM_OK;
}

void WindowRoot::NotifyVirtualPixelRatioChange(sptr<DisplayInfo> displayInfo)
{
    WLOGFD("window should be updated for virtual pixel ratio changed");
    auto container = GetOrCreateWindowNodeContainer(displayInfo->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("can't find window node container, failed!");
        return;
    }
    container->UpdateVirtualPixelRatio(displayInfo->GetDisplayId(), displayInfo->GetVirtualPixelRatio());
}

void WindowRoot::SetWindowStretchable(bool stretchable)
{
    WLOGFI("set window stretchable to %{publec}d", stretchable);
    isWindowStretchable_ = stretchable;
}
} // namespace Rosen
} // namespace OHOS
