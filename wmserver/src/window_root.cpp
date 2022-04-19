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
    const sptr<DisplayInfo> displayInfo = DisplayManagerServiceInner::GetInstance().GetDisplayById(displayId);
    if (displayInfo == nullptr) {
        WLOGFE("get display failed displayId:%{public}" PRId64 "", displayId);
        return nullptr;
    }

    if (!CheckDisplayInfo(displayInfo)) {
        WLOGFE("get display invailed infp:%{public}" PRId64 "", displayId);
        return nullptr;
    }

    WLOGFI("create new window node container display width:%{public}d, height:%{public}d, screenId:%{public}" PRIu64"",
        displayInfo->GetWidth(), displayInfo->GetHeight(), displayInfo->GetDisplayId());
    sptr<WindowNodeContainer> container = new WindowNodeContainer(displayInfo->GetDisplayId(),
        static_cast<uint32_t>(displayInfo->GetWidth()), static_cast<uint32_t>(displayInfo->GetHeight()));
    windowNodeContainerMap_.insert(std::make_pair(displayId, container));
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
    windowNodeMap_.insert(std::make_pair(node->GetWindowId(), node));
    auto remoteObject = node->GetWindowToken()->AsObject();
    windowIdMap_.insert(std::make_pair(remoteObject, node->GetWindowId()));

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
        NotifyKeyboardSizeChangeInfo(node, container, node->GetLayoutRect());
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
    UpdateFocusWindowWithWindowRemoved(node, container);
    UpdateActiveWindowWithWindowRemoved(node, container);
    UpdateBrightnessWithWindowRemoved(windowId, container);
    WMError res = container->RemoveWindowNode(node);
    if (res == WMError::WM_OK) {
        Rect rect = { 0, 0, 0, 0 };
        NotifyKeyboardSizeChangeInfo(node, container, rect);
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
        UpdateFocusWindowWithWindowRemoved(node, container);
        UpdateActiveWindowWithWindowRemoved(node, container);
        UpdateBrightnessWithWindowRemoved(windowId, container);
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
    return container->SetActiveWindow(windowId, false);
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

void WindowRoot::NotifyDisplayChange(sptr<DisplayInfo> displayInfo)
{
    WLOGFD("window should be updated for display changed");
    auto container = GetOrCreateWindowNodeContainer(displayInfo->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("can't find window node container, failed!");
        return;
    }
    container->UpdateDisplayRect(displayInfo->GetWidth(), displayInfo->GetHeight());
}

void WindowRoot::NotifySystemBarTints()
{
    WLOGFD("notify current system bar tints");
    for (auto& it : windowNodeContainerMap_) {
        if (it.second != nullptr) {
            it.second->NotifySystemBarTints();
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
    WMError ret = container->SwitchLayoutPolicy(mode, true);
    if (ret != WMError::WM_OK) {
        WLOGFW("set window layout mode failed displayId: %{public}" PRId64 ", ret: %{public}d", displayId, ret);
    }
    return ret;
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
        os<<"Display "<<elem.second->GetDisplayId()<<":";
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

void WindowRoot::NotifyDisplayDestroy(DisplayId expandDisplayId)
{
    WLOGFD("disconnect expand display, get default and expand display container");
    DisplayId defaultDisplayId = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    // get all windowNode below expand display, and reset its displayId
    for (auto iter = windowNodeMap_.begin(); iter != windowNodeMap_.end(); iter++) {
        auto node = iter->second;
        if (node->GetDisplayId() != expandDisplayId) {
            continue;
        }
        node->SetDisplayId(defaultDisplayId);
        node->GetWindowToken()->UpdateDisplayId(expandDisplayId, defaultDisplayId);
    }
    // move windowNode from expand display container to default display container
    auto expandDisplayContainer = GetOrCreateWindowNodeContainer(expandDisplayId);
    auto defaultDisplayContainer = GetOrCreateWindowNodeContainer(defaultDisplayId);
    if (expandDisplayContainer == nullptr || defaultDisplayContainer == nullptr) {
        WLOGFE("window node container is nullptr!");
        return;
    }
    defaultDisplayContainer->MoveWindowNode(expandDisplayContainer);
    windowNodeContainerMap_.erase(expandDisplayId);
}

float WindowRoot::GetVirtualPixelRatio(DisplayId displayId) const
{
    auto container = const_cast<WindowRoot*>(this)->GetOrCreateWindowNodeContainer(displayId);
    return container->GetVirtualPixelRatio();
}

Rect WindowRoot::GetDisplayLimitRect(DisplayId displayId) const
{
    auto container = const_cast<WindowRoot*>(this)->GetOrCreateWindowNodeContainer(displayId);
    return container->GetDisplayLimitRect();
}
} // namespace OHOS::Rosen
} // namespace OHOS
