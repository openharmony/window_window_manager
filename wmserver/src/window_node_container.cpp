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

#include "window_node_container.h"

#include <ability_manager_client.h>
#include <algorithm>
#include <cinttypes>
#include <ctime>

#include "common_event_manager.h"
#include "display_manager_service_inner.h"
#include "dm_common.h"
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_layout_policy_cascade.h"
#include "window_layout_policy_tile.h"
#include "window_manager_agent_controller.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "wm_common_inner.h"
#include "wm_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowNodeContainer"};
    constexpr int WINDOW_NAME_MAX_LENGTH = 10;
    const std::string SPLIT_SCREEN_EVENT_NAME = "common.event.SPLIT_SCREEN";
    const char DISABLE_WINDOW_ANIMATION_PATH[] = "/etc/disable_window_animation";
}

WindowNodeContainer::WindowNodeContainer(DisplayId displayId, uint32_t width, uint32_t height) : displayId_(displayId)
{
    displayRect_ = {
        .posX_ = 0,
        .posY_ = 0,
        .width_ = width,
        .height_ = height
    };
    layoutPolicys_[WindowLayoutMode::CASCADE] =
        new WindowLayoutPolicyCascade(displayRect_, displayId_,
            belowAppWindowNode_, appWindowNode_, aboveAppWindowNode_);
    layoutPolicys_[WindowLayoutMode::TILE] =
        new WindowLayoutPolicyTile(displayRect_, displayId_,
            belowAppWindowNode_, appWindowNode_, aboveAppWindowNode_);
    layoutPolicy_ = layoutPolicys_[WindowLayoutMode::CASCADE];
    layoutPolicy_->Launch();
    UpdateAvoidAreaFunc func = std::bind(&WindowNodeContainer::OnAvoidAreaChange, this, std::placeholders::_1);
    avoidController_ = new AvoidAreaController(displayId, func);
}

WindowNodeContainer::~WindowNodeContainer()
{
    Destroy();
}

void WindowNodeContainer::UpdateDisplayRect(uint32_t width, uint32_t height)
{
    WLOGFI("update display rect, w/h=%{public}u/%{public}u", width, height);
    displayRect_ = {
        .posX_ = 0,
        .posY_ = 0,
        .width_ = width,
        .height_ = height
    };
    layoutPolicy_->LayoutWindowTree();
}

WMError WindowNodeContainer::MinimizeStructuredAppWindowsExceptSelf(const sptr<WindowNode>& node)
{
    std::vector<uint32_t> exceptionalIds = { node->GetWindowId() };
    std::vector<WindowMode> exceptionalModes = { WindowMode::WINDOW_MODE_FLOATING, WindowMode::WINDOW_MODE_PIP };
    return MinimizeAppNodeExceptOptions(false, exceptionalIds, exceptionalModes);
}

WMError WindowNodeContainer::AddWindowNode(sptr<WindowNode>& node, sptr<WindowNode>& parentNode)
{
    if (!node->surfaceNode_) {
        WLOGFE("surface node is nullptr!");
        return WMError::WM_ERROR_NULLPTR;
    }
    sptr<WindowNode> root = FindRoot(node->GetWindowType());
    if (root == nullptr) {
        WLOGFE("root window node is nullptr!");
        return WMError::WM_ERROR_NULLPTR;
    }
    node->requestedVisibility_ = true;
    if (parentNode != nullptr) { // subwindow
        if (parentNode->parent_ != root) {
            WLOGFE("window type and parent window not match or try to add subwindow to subwindow, which is forbidden");
            return WMError::WM_ERROR_INVALID_PARAM;
        }
        node->currentVisibility_ = parentNode->currentVisibility_;
    } else { // mainwindow
        parentNode = root;
        node->currentVisibility_ = true;
        for (auto& child : node->children_) {
            child->currentVisibility_ = child->requestedVisibility_;
        }
        if (WindowHelper::IsAvoidAreaWindow(node->GetWindowType())) {
            sysBarNodeMap_[node->GetWindowType()] = node;
        }
    }
    node->parent_ = parentNode;

    if (node->IsSplitMode()) {
        WMError ret = EnterSplitWindowMode(node);
        if (ret != WMError::WM_OK) {
            WLOGFE("Add split window failed!");
            return ret;
        }
    }
    UpdateWindowTree(node);
    if (node->IsSplitMode() || node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        RaiseSplitRelatedWindowToTop(node);
    }
    UpdateRSTree(node, true);
    AssignZOrder();
    layoutPolicy_->AddWindowNode(node);
    if (WindowHelper::IsAvoidAreaWindow(node->GetWindowType())) {
        avoidController_->AvoidControl(node, AvoidControlType::AVOID_NODE_ADD);
        NotifyIfSystemBarRegionChanged();
    } else {
        NotifyIfSystemBarTintChanged();
    }
    std::vector<sptr<WindowVisibilityInfo>> infos;
    UpdateWindowVisibilityInfos(infos);
    DumpScreenWindowTree();
    NotifyAccessibilityWindowInfo(node, WindowUpdateType::WINDOW_UPDATE_ADDED);
    WLOGFI("AddWindowNode windowId: %{public}d end", node->GetWindowId());
    return WMError::WM_OK;
}

WMError WindowNodeContainer::UpdateWindowNode(sptr<WindowNode>& node, WindowUpdateReason reason)
{
    if (!node->surfaceNode_) {
        WLOGFE("surface node is nullptr!");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (WindowHelper::IsMainWindow(node->GetWindowType()) && WindowHelper::IsSwitchCascadeReason(reason)) {
        SwitchLayoutPolicy(WindowLayoutMode::CASCADE);
    }
    layoutPolicy_->UpdateWindowNode(node);
    if (WindowHelper::IsAvoidAreaWindow(node->GetWindowType())) {
        avoidController_->AvoidControl(node, AvoidControlType::AVOID_NODE_UPDATE);
        NotifyIfSystemBarRegionChanged();
    } else {
        NotifyIfSystemBarTintChanged();
    }
    DumpScreenWindowTree();
    WLOGFI("UpdateWindowNode windowId: %{public}d end", node->GetWindowId());
    return WMError::WM_OK;
}

void WindowNodeContainer::UpdateWindowTree(sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    node->priority_ = zorderPolicy_->GetWindowPriority(node->GetWindowType());
    RaiseInputMethodWindowPriorityIfNeeded(node);
    auto parentNode = node->parent_;
    auto position = parentNode->children_.end();
    for (auto iter = parentNode->children_.begin(); iter < parentNode->children_.end(); ++iter) {
        if ((*iter)->priority_ > node->priority_) {
            position = iter;
            break;
        }
    }
    parentNode->children_.insert(position, node);
}

bool WindowNodeContainer::UpdateRSTree(sptr<WindowNode>& node, bool isAdd)
{
    WM_FUNCTION_TRACE();
    static const bool IsWindowAnimationEnabled = ReadIsWindowAnimationEnabledProperty();

    auto updateRSTreeFunc = [&]() {
        auto& dms = DisplayManagerServiceInner::GetInstance();
        if (isAdd) {
            dms.UpdateRSTree(displayId_, node->surfaceNode_, true);
            for (auto& child : node->children_) {
                if (child->currentVisibility_) {
                    dms.UpdateRSTree(displayId_, child->surfaceNode_, true);
                }
            }
        } else {
            dms.UpdateRSTree(displayId_, node->surfaceNode_, false);
            for (auto& child : node->children_) {
                dms.UpdateRSTree(displayId_, child->surfaceNode_, false);
            }
        }
    };

    if (IsWindowAnimationEnabled) {
        // default transition duration: 350ms
        static const RSAnimationTimingProtocol timingProtocol(350);
        // default transition curve: EASE OUT
        static const Rosen::RSAnimationTimingCurve curve = Rosen::RSAnimationTimingCurve::EASE_OUT;

        // add or remove window with transition animation
        RSNode::Animate(timingProtocol, curve, updateRSTreeFunc);
    } else {
        // add or remove window without animation
        updateRSTreeFunc();
    }

    return true;
}

WMError WindowNodeContainer::DestroyWindowNode(sptr<WindowNode>& node, std::vector<uint32_t>& windowIds)
{
    WMError ret = RemoveWindowNode(node);
    if (ret != WMError::WM_OK) {
        WLOGFE("RemoveWindowNode failed");
        return ret;
    }
    node->surfaceNode_ = nullptr;
    windowIds.push_back(node->GetWindowId());

    for (auto& child : node->children_) { // destroy sub window if exists
        windowIds.push_back(child->GetWindowId());
        child->parent_ = nullptr;
        if (child->surfaceNode_ != nullptr) {
            WLOGFI("child surfaceNode set nullptr");
            child->surfaceNode_ = nullptr;
        }
    }
    node->children_.clear();
    return WMError::WM_OK;
}

WMError WindowNodeContainer::RemoveWindowNode(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        WLOGFE("window node or surface node is nullptr, invalid");
        return WMError::WM_ERROR_DESTROYED_OBJECT;
    }
    if (node->parent_ == nullptr) {
        WLOGFW("can't find parent of this node");
    } else {
        // remove this node from parent
        auto iter = std::find(node->parent_->children_.begin(), node->parent_->children_.end(), node);
        if (iter != node->parent_->children_.end()) {
            node->parent_->children_.erase(iter);
        } else {
            WLOGFE("can't find this node in parent");
        }
        node->parent_ = nullptr;
    }
    node->requestedVisibility_ = false;
    node->currentVisibility_ = false;
    node->hasDecorated_ = false;
    node->isCovered_ = true;
    std::vector<sptr<WindowVisibilityInfo>> infos = {new WindowVisibilityInfo(node->GetWindowId(),
        node->GetCallingPid(), node->GetCallingUid(), false)};
    for (auto& child : node->children_) {
        if (child->currentVisibility_) {
            child->currentVisibility_ = false;
            child->isCovered_ = true;
            infos.emplace_back(new WindowVisibilityInfo(child->GetWindowId(), child->GetCallingPid(),
                child->GetCallingUid(), false));
        }
    }

    if (node->IsSplitMode()) {
        WMError ret = ExitSplitWindowMode(node);
        if (ret != WMError::WM_OK) {
            WLOGFE("Remove split window failed!");
            return ret;
        }
    }

    UpdateRSTree(node, false);
    layoutPolicy_->RemoveWindowNode(node);
    if (WindowHelper::IsAvoidAreaWindow(node->GetWindowType())) {
        avoidController_->AvoidControl(node, AvoidControlType::AVOID_NODE_REMOVE);
        NotifyIfSystemBarRegionChanged();
    } else {
        NotifyIfSystemBarTintChanged();
    }
    UpdateWindowVisibilityInfos(infos);
    DumpScreenWindowTree();
    NotifyAccessibilityWindowInfo(node, WindowUpdateType::WINDOW_UPDATE_REMOVED);
    WLOGFI("RemoveWindowNode windowId: %{public}d end", node->GetWindowId());
    return WMError::WM_OK;
}

const std::vector<uint32_t>& WindowNodeContainer::Destroy()
{
    removedIds_.clear();
    for (auto& node : belowAppWindowNode_->children_) {
        DestroyWindowNode(node, removedIds_);
    }
    for (auto& node : appWindowNode_->children_) {
        DestroyWindowNode(node, removedIds_);
    }
    for (auto& node : aboveAppWindowNode_->children_) {
        DestroyWindowNode(node, removedIds_);
    }
    return removedIds_;
}

sptr<WindowNode> WindowNodeContainer::FindRoot(WindowType type) const
{
    if (WindowHelper::IsAppWindow(type) || type == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        return appWindowNode_;
    }
    if (WindowHelper::IsBelowSystemWindow(type)) {
        return belowAppWindowNode_;
    }
    if (WindowHelper::IsAboveSystemWindow(type)) {
        return aboveAppWindowNode_;
    }
    return nullptr;
}

sptr<WindowNode> WindowNodeContainer::FindWindowNodeById(uint32_t id) const
{
    std::vector<sptr<WindowNode>> rootNodes = { aboveAppWindowNode_, appWindowNode_, belowAppWindowNode_ };
    for (auto& rootNode : rootNodes) {
        for (auto& node : rootNode->children_) {
            if (node->GetWindowId() == id) {
                return node;
            }
            for (auto& subNode : node->children_) {
                if (subNode->GetWindowId() == id) {
                    return subNode;
                }
            }
        }
    }
    return nullptr;
}

void WindowNodeContainer::UpdateFocusStatus(uint32_t id, bool focused) const
{
    auto node = FindWindowNodeById(id);
    if (node == nullptr) {
        WLOGFW("cannot find focused window id:%{public}d", id);
    } else {
        node->GetWindowToken()->UpdateFocusStatus(focused);
        if (node->abilityToken_ == nullptr) {
            WLOGFI("abilityToken is null, window : %{public}d", id);
        }
        WindowManagerAgentController::GetInstance().UpdateFocusStatus(
            node->GetWindowId(), node->abilityToken_, node->GetWindowType(), node->GetDisplayId(), focused);
        sptr<FocusChangeInfo> focusChangeInfo = new FocusChangeInfo(node->GetWindowId(), node->GetDisplayId(),
            node->GetCallingPid(), node->GetCallingUid(), node->GetWindowType(), node->abilityToken_);
        WindowManagerAgentController::GetInstance().UpdateFocusChangeInfo(
            focusChangeInfo, focused);
    }
}

void WindowNodeContainer::AssignZOrder()
{
    zOrder_ = 0;
    WindowNodeOperationFunc func = [this](sptr<WindowNode> node) {
        if (node->surfaceNode_ == nullptr) {
            WLOGE("AssignZOrder: surfaceNode is nullptr, window Id:%{public}u", node->GetWindowId());
            return false;
        }
        node->surfaceNode_->SetPositionZ(zOrder_);
        ++zOrder_;
        return false;
    };
    TraverseWindowTree(func, false);
}

WMError WindowNodeContainer::SetFocusWindow(uint32_t windowId)
{
    if (focusedWindow_ == windowId) {
        WLOGFI("focused window do not change");
        return WMError::WM_DO_NOTHING;
    }
    UpdateFocusStatus(focusedWindow_, false);
    focusedWindow_ = windowId;
    sptr<WindowNode> node = FindWindowNodeById(windowId);
    NotifyAccessibilityWindowInfo(node, WindowUpdateType::WINDOW_UPDATE_FOCUSED);
    UpdateFocusStatus(focusedWindow_, true);
    return WMError::WM_OK;
}

uint32_t WindowNodeContainer::GetFocusWindow() const
{
    return focusedWindow_;
}

bool WindowNodeContainer::IsAboveSystemBarNode(sptr<WindowNode> node) const
{
    int32_t curPriority = zorderPolicy_->GetWindowPriority(node->GetWindowType());
    if ((curPriority > zorderPolicy_->GetWindowPriority(WindowType::WINDOW_TYPE_STATUS_BAR)) &&
        (curPriority > zorderPolicy_->GetWindowPriority(WindowType::WINDOW_TYPE_NAVIGATION_BAR))) {
        return true;
    }
    return false;
}

bool WindowNodeContainer::IsFullImmersiveNode(sptr<WindowNode> node) const
{
    auto mode = node->GetWindowMode();
    auto flags = node->GetWindowFlags();
    return mode == WindowMode::WINDOW_MODE_FULLSCREEN &&
        !(flags & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
}

bool WindowNodeContainer::IsSplitImmersiveNode(sptr<WindowNode> node) const
{
    auto type = node->GetWindowType();
    return node->IsSplitMode() || type == WindowType::WINDOW_TYPE_DOCK_SLICE;
}

std::unordered_map<WindowType, SystemBarProperty> WindowNodeContainer::GetExpectImmersiveProperty() const
{
    std::unordered_map<WindowType, SystemBarProperty> sysBarPropMap {
        { WindowType::WINDOW_TYPE_STATUS_BAR,     SystemBarProperty() },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SystemBarProperty() },
    };

    std::vector<sptr<WindowNode>> rootNodes = { aboveAppWindowNode_, appWindowNode_, belowAppWindowNode_ };
    for (auto& node : rootNodes) {
        for (auto iter = node->children_.rbegin(); iter < node->children_.rend(); ++iter) {
            auto& sysBarPropMapNode = (*iter)->GetSystemBarProperty();
            if (IsAboveSystemBarNode(*iter)) {
                continue;
            }
            if (IsFullImmersiveNode(*iter)) {
                WLOGFI("Top immersive window id: %{public}d. Use full immersive prop", (*iter)->GetWindowId());
                for (auto it : sysBarPropMap) {
                    sysBarPropMap[it.first] = (sysBarPropMapNode.find(it.first))->second;
                }
                return sysBarPropMap;
            } else if (IsSplitImmersiveNode(*iter)) {
                WLOGFI("Top split window id: %{public}d. Use split immersive prop", (*iter)->GetWindowId());
                for (auto it : sysBarPropMap) {
                    sysBarPropMap[it.first] = (sysBarPropMapNode.find(it.first))->second;
                    sysBarPropMap[it.first].enable_ = false;
                }
                return sysBarPropMap;
            }
        }
    }

    WLOGFI("No immersive window on top. Use default systembar Property");
    return sysBarPropMap;
}

void WindowNodeContainer::NotifyIfSystemBarTintChanged()
{
    WM_FUNCTION_TRACE();
    auto expectSystemBarProp = GetExpectImmersiveProperty();
    SystemBarRegionTints tints;
    for (auto it : sysBarTintMap_) {
        auto expectProp = expectSystemBarProp.find(it.first)->second;
        if (it.second.prop_ == expectProp) {
            continue;
        }
        WLOGFI("System bar prop update, Type: %{public}d, Visible: %{public}d, Color: %{public}x | %{public}x",
            static_cast<int32_t>(it.first), expectProp.enable_, expectProp.backgroundColor_, expectProp.contentColor_);
        sysBarTintMap_[it.first].prop_ = expectProp;
        sysBarTintMap_[it.first].type_ = it.first;
        tints.emplace_back(sysBarTintMap_[it.first]);
    }
    WindowManagerAgentController::GetInstance().UpdateSystemBarRegionTints(displayId_, tints);
}

void WindowNodeContainer::NotifyIfSystemBarRegionChanged()
{
    WM_FUNCTION_TRACE();
    SystemBarRegionTints tints;
    for (auto it : sysBarTintMap_) { // split screen mode not support yet
        auto sysNode = sysBarNodeMap_[it.first];
        if (sysNode == nullptr || it.second.region_ == sysNode->GetLayoutRect()) {
            continue;
        }
        auto& newRegion = sysNode->GetLayoutRect();
        sysBarTintMap_[it.first].region_ = newRegion;
        sysBarTintMap_[it.first].type_ = it.first;
        tints.emplace_back(sysBarTintMap_[it.first]);
        WLOGFI("system bar region update, type: %{public}d" \
            "region: [%{public}d, %{public}d, %{public}d, %{public}d]",
            static_cast<int32_t>(it.first), newRegion.posX_, newRegion.posY_, newRegion.width_, newRegion.height_);
    }
    WindowManagerAgentController::GetInstance().UpdateSystemBarRegionTints(displayId_, tints);
}

void WindowNodeContainer::NotifySystemBarDismiss(sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    if (node == nullptr) {
        WLOGE("could not find window");
        return;
    }
    SystemBarRegionTints tints;
    auto& sysBarPropMapNode = node->GetSystemBarProperty();
    for (auto it : sysBarPropMapNode) {
        it.second.enable_ = false;
        node->SetSystemBarProperty(it.first, it.second);
        WLOGFI("set system bar enable to false, id: %{public}u, type: %{public}d",
            node->GetWindowId(), static_cast<int32_t>(it.first));
        if (sysBarTintMap_[it.first].prop_.enable_) {
            sysBarTintMap_[it.first].prop_.enable_ = false;
            tints.emplace_back(sysBarTintMap_[it.first]);
            WLOGFI("notify system bar dismiss, type: %{public}d", static_cast<int32_t>(it.first));
        }
    }
    WindowManagerAgentController::GetInstance().UpdateSystemBarRegionTints(displayId_, tints);
}

void WindowNodeContainer::NotifySystemBarTints()
{
    WM_FUNCTION_TRACE();
    SystemBarRegionTints tints;
    for (auto it : sysBarTintMap_) {
        WLOGFD("system bar cur notify, type: %{public}d, " \
            "visible: %{public}d, color: %{public}x | %{public}x, " \
            "region: [%{public}d, %{public}d, %{public}d, %{public}d]",
            static_cast<int32_t>(it.first),
            sysBarTintMap_[it.first].region_.posX_, sysBarTintMap_[it.first].region_.posY_,
            sysBarTintMap_[it.first].region_.width_, sysBarTintMap_[it.first].region_.height_,
            sysBarTintMap_[it.first].prop_.enable_,
            sysBarTintMap_[it.first].prop_.backgroundColor_, sysBarTintMap_[it.first].prop_.contentColor_);
        tints.push_back(sysBarTintMap_[it.first]);
    }
    WindowManagerAgentController::GetInstance().UpdateSystemBarRegionTints(displayId_, tints);
}

bool WindowNodeContainer::IsTopAppWindow(uint32_t windowId) const
{
    if (appWindowNode_->children_.empty()) {
        WLOGFE("app root does not have any node");
        return false;
    }
    auto node = *(appWindowNode_->children_.rbegin());
    if (node == nullptr) {
        WLOGFE("window tree does not have any node");
        return false;
    }

    for (auto iter = node->children_.rbegin(); iter < node->children_.rend(); iter++) {
        if ((*iter)->priority_ > 0) {
            return (*iter)->GetWindowId() == windowId;
        } else {
            break;
        }
    }
    return node->GetWindowId() == windowId;
}

void WindowNodeContainer::RaiseOrderedWindowToTop(std::vector<uint32_t> orderedIds,
    std::vector<sptr<WindowNode>>& windowNodes)
{
    for (auto iter = appWindowNode_->children_.begin(); iter != appWindowNode_->children_.end();) {
        uint32_t wid = (*iter)->GetWindowId();
        auto idIter = std::find_if(orderedIds.begin(), orderedIds.end(),
            [wid] (uint32_t id) { return id == wid; });
        if (idIter != orderedIds.end()) {
            orderedIds.erase(idIter);
            sptr<WindowNode> node = *iter;
            iter = windowNodes.erase(iter);
            UpdateWindowTree(node);
            WLOGFI("raise group window to top %{public}d", node->GetWindowId());
        } else {
            iter++;
        }
    }
    return;
}

void WindowNodeContainer::RaiseWindowToTop(uint32_t windowId, std::vector<sptr<WindowNode>>& windowNodes)
{
    auto iter = std::find_if(windowNodes.begin(), windowNodes.end(),
                             [windowId](sptr<WindowNode> node) {
                                 return node->GetWindowId() == windowId;
                             });
    // raise app node window to top
    if (iter != windowNodes.end()) {
        sptr<WindowNode> node = *iter;
        windowNodes.erase(iter);
        UpdateWindowTree(node);
        WLOGFI("raise window to top %{public}d", node->GetWindowId());
    }
}

void WindowNodeContainer::NotifyAccessibilityWindowInfo(const sptr<WindowNode>& node, WindowUpdateType type) const
{
    if (node == nullptr) {
        WLOGFE("window node is null");
        return;
    }
    bool isNeedNotify = false;
    switch (type) {
        case WindowUpdateType::WINDOW_UPDATE_ADDED:
            if (node->currentVisibility_) {
                isNeedNotify = true;
            }
            break;
        case WindowUpdateType::WINDOW_UPDATE_FOCUSED:
            if (node->GetWindowId() == focusedWindow_) {
                isNeedNotify = true;
            }
            break;
        case WindowUpdateType::WINDOW_UPDATE_REMOVED:
            isNeedNotify = true;
            break;
        default:
            break;
    }
    if (isNeedNotify) {
        std::vector<sptr<WindowInfo>> windowList;
        GetWindowList(windowList);
        sptr<WindowInfo> windowInfo = new WindowInfo();
        windowInfo->wid_ = static_cast<int32_t>(node->GetWindowId());
        windowInfo->windowRect_ = node->GetLayoutRect();
        windowInfo->focused_ = node->GetWindowId() == focusedWindow_;
        windowInfo->mode_ = node->GetWindowMode();
        windowInfo->type_ = node->GetWindowType();
        sptr<AccessibilityWindowInfo> accessibilityWindowInfo = new AccessibilityWindowInfo();
        accessibilityWindowInfo->currentWindowInfo_ = windowInfo;
        accessibilityWindowInfo->windowList_ = windowList;
        WindowManagerAgentController::GetInstance().NotifyAccessibilityWindowInfo(accessibilityWindowInfo, type);
    }
}

void WindowNodeContainer::GetWindowList(std::vector<sptr<WindowInfo>>& windowList) const
{
    std::vector<sptr<WindowNode>> windowNodes;
    TraverseContainer(windowNodes);
    for (auto node : windowNodes) {
        sptr<WindowInfo> windowInfo = new WindowInfo();
        windowInfo->wid_ = static_cast<int32_t>(node->GetWindowId());
        windowInfo->windowRect_ = node->GetLayoutRect();
        windowInfo->focused_ = node->GetWindowId() == focusedWindow_;
        windowInfo->mode_ = node->GetWindowMode();
        windowInfo->type_ = node->GetWindowType();
        windowList.emplace_back(windowInfo);
    }
}

void WindowNodeContainer::TraverseContainer(std::vector<sptr<WindowNode>>& windowNodes) const
{
    for (auto& node : belowAppWindowNode_->children_) {
        TraverseWindowNode(node, windowNodes);
    }
    for (auto& node : appWindowNode_->children_) {
        TraverseWindowNode(node, windowNodes);
    }
    for (auto& node : aboveAppWindowNode_->children_) {
        TraverseWindowNode(node, windowNodes);
    }
    std::reverse(windowNodes.begin(), windowNodes.end());
}

void WindowNodeContainer::TraverseWindowNode(sptr<WindowNode>& node, std::vector<sptr<WindowNode>>& windowNodes) const
{
    if (node == nullptr) {
        return;
    }
    auto iter = node->children_.begin();
    for (; iter < node->children_.end(); ++iter) {
        if ((*iter)->priority_ < 0) {
            windowNodes.emplace_back(*iter);
        } else {
            break;
        }
    }
    windowNodes.emplace_back(node);
    for (; iter < node->children_.end(); ++iter) {
        windowNodes.emplace_back(*iter);
    }
}

std::vector<Rect> WindowNodeContainer::GetAvoidAreaByType(AvoidAreaType avoidAreaType)
{
    return avoidController_->GetAvoidAreaByType(avoidAreaType);
}

void WindowNodeContainer::OnAvoidAreaChange(const std::vector<Rect>& avoidArea)
{
    layoutPolicy_->UpdateDefaultFoatingRect();
    for (auto& node : appWindowNode_->children_) {
        if (node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN && node->GetWindowToken() != nullptr) {
            // notify client
            node->GetWindowToken()->UpdateAvoidArea(avoidArea);
        }
    }
}

void WindowNodeContainer::DumpScreenWindowTree()
{
    WLOGFI("-------- display %{public}" PRIu64" dump window info begin---------", displayId_);
    WLOGFI("WindowName WinId Type Mode Flag ZOrd [   x    y    w    h]");
    uint32_t zOrder = zOrder_;
    WindowNodeOperationFunc func = [&zOrder](sptr<WindowNode> node) {
        Rect rect = node->GetLayoutRect();
        const std::string& windowName = node->GetWindowName().size() < WINDOW_NAME_MAX_LENGTH ?
            node->GetWindowName() : node->GetWindowName().substr(0, WINDOW_NAME_MAX_LENGTH);
        WLOGI("DumpScreenWindowTree: %{public}10s %{public}5u %{public}4u %{public}4u %{public}4u %{public}4u " \
            "[%{public}4d %{public}4d %{public}4u %{public}4u]",
            windowName.c_str(), node->GetWindowId(), node->GetWindowType(), node->GetWindowMode(),
            node->GetWindowFlags(), --zOrder, rect.posX_, rect.posY_, rect.width_, rect.height_);
        return false;
    };
    TraverseWindowTree(func, true);
    WLOGFI("-------- display %{public}" PRIu64" dump window info end  ---------", displayId_);
}

uint64_t WindowNodeContainer::GetScreenId() const
{
    return DisplayManagerServiceInner::GetInstance().GetRSScreenId(displayId_);
}

DisplayId WindowNodeContainer::GetDisplayId() const
{
    return displayId_;
}

Rect WindowNodeContainer::GetDisplayRect() const
{
    return displayRect_;
}

Rect WindowNodeContainer::GetDisplayLimitRect() const
{
    return layoutPolicy_->GetDisplayLimitRect();
}

bool WindowNodeContainer::isVerticalDisplay() const
{
    return displayRect_.width_ < displayRect_.height_;
}

void WindowNodeContainer::NotifyWindowStateChange(WindowState state, WindowStateChangeReason reason)
{
    switch (reason) {
        case WindowStateChangeReason::KEYGUARD: {
            int32_t topPriority = zorderPolicy_->GetWindowPriority(WindowType::WINDOW_TYPE_KEYGUARD);
            TraverseAndUpdateWindowState(state, topPriority);
            break;
        }
        default:
            return;
    }
}

void WindowNodeContainer::TraverseAndUpdateWindowState(WindowState state, int32_t topPriority)
{
    std::vector<sptr<WindowNode>> rootNodes = { belowAppWindowNode_, appWindowNode_, aboveAppWindowNode_ };
    for (auto& node : rootNodes) {
        UpdateWindowState(node, topPriority, state);
    }
}

void WindowNodeContainer::UpdateWindowState(sptr<WindowNode> node, int32_t topPriority, WindowState state)
{
    if (node == nullptr) {
        return;
    }
    if (node->parent_ != nullptr && node->currentVisibility_) {
        if (node->priority_ < topPriority) {
            node->GetWindowToken()->UpdateWindowState(state);
        }
    }
    for (auto& childNode : node->children_) {
        UpdateWindowState(childNode, topPriority, state);
    }
}

sptr<WindowNode> WindowNodeContainer::FindDividerNode() const
{
    for (auto iter = appWindowNode_->children_.begin(); iter != appWindowNode_->children_.end(); iter++) {
        if ((*iter)->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
            return *iter;
        }
    }
    return nullptr;
}

void WindowNodeContainer::RaiseSplitRelatedWindowToTop(sptr<WindowNode>& node)
{
    sptr<WindowNode> deviderNode = nullptr;
    sptr<WindowNode> primaryNode = nullptr;
    sptr<WindowNode> secondaryNode = nullptr;
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE && !pairedWindowMap_.empty()) {
        deviderNode = node;
        primaryNode = pairedWindowMap_.begin()->second.pairNode_;
        secondaryNode = pairedWindowMap_[primaryNode->GetWindowId()].pairNode_;
        std::vector<uint32_t> raiseNodeIds = {secondaryNode->GetWindowId(), primaryNode->GetWindowId()};
        // raise primary and secondary window to top, keep raw zorder
        RaiseOrderedWindowToTop(raiseNodeIds, appWindowNode_->children_);
        // raise divider final, keep divider on top
        RaiseWindowToTop(deviderNode->GetWindowId(), appWindowNode_->children_);
    } else if (!pairedWindowMap_.empty()) {
        deviderNode = FindDividerNode();
        primaryNode = node;
        secondaryNode = pairedWindowMap_.at(primaryNode->GetWindowId()).pairNode_;
        RaiseWindowToTop(secondaryNode->GetWindowId(), appWindowNode_->children_);
        RaiseWindowToTop(primaryNode->GetWindowId(), appWindowNode_->children_);
        if (deviderNode != nullptr) {
            // raise divider final, keep divider on top
            RaiseWindowToTop(deviderNode->GetWindowId(), appWindowNode_->children_);
        }
    } else {
        // raise self if not paired
        RaiseWindowToTop(node->GetWindowId(), appWindowNode_->children_);
    }
    AssignZOrder();
    return;
}

WMError WindowNodeContainer::RaiseZOrderForAppWindow(sptr<WindowNode>& node, sptr<WindowNode>& parentNode)
{
    if (node == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    if (IsTopAppWindow(node->GetWindowId())) {
        WLOGFI("it is already top app window, id: %{public}d", node->GetWindowId());
        return WMError::WM_ERROR_INVALID_TYPE;
    }
    if (WindowHelper::IsSubWindow(node->GetWindowType())) {
        if (parentNode == nullptr) {
            WLOGFE("window type is invalid");
            return WMError::WM_ERROR_NULLPTR;
        }
        RaiseWindowToTop(node->GetWindowId(), parentNode->children_); // raise itself
        if (parentNode->IsSplitMode()) {
            RaiseSplitRelatedWindowToTop(parentNode);
        } else {
            RaiseWindowToTop(node->GetParentId(), appWindowNode_->children_); // raise parent window
        }
    } else if (WindowHelper::IsMainWindow(node->GetWindowType())) {
        if (node->IsSplitMode()) {
            RaiseSplitRelatedWindowToTop(node);
        } else {
            RaiseWindowToTop(node->GetWindowId(), appWindowNode_->children_);
        }
    } else {
        // do nothing
    }
    AssignZOrder();
    WLOGFI("RaiseZOrderForAppWindow finished");
    DumpScreenWindowTree();
    return WMError::WM_OK;
}

sptr<WindowNode> WindowNodeContainer::GetNextFocusableWindow(uint32_t windowId) const
{
    sptr<WindowNode> nextFocusableWindow;
    bool previousFocusedWindowFound = false;
    WindowNodeOperationFunc func = [windowId, &nextFocusableWindow, &previousFocusedWindowFound](
        sptr<WindowNode> node) {
        if (previousFocusedWindowFound && node->GetWindowProperty()->GetFocusable()) {
            nextFocusableWindow = node;
            return true;
        }
        if (node->GetWindowId() == windowId) {
            previousFocusedWindowFound = true;
        }
        return false;
    };
    TraverseWindowTree(func, true);
    return nextFocusableWindow;
}

void WindowNodeContainer::MinimizeAllAppWindows()
{
    WMError ret =  MinimizeAppNodeExceptOptions(true);
    SwitchLayoutPolicy(WindowLayoutMode::CASCADE);
    if (ret != WMError::WM_OK) {
        WLOGFE("Minimize all app window failed");
    }
    return;
}

void WindowNodeContainer::SendSplitScreenEvent(sptr<WindowNode>& node)
{
    // reset ipc identity
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    AAFwk::Want want;
    want.SetAction(SPLIT_SCREEN_EVENT_NAME);
    std::string modeData = (node->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_PRIMARY) ? "Secondary" : "Primary";
    int32_t missionId = -1;
    AAFwk::AbilityManagerClient::GetInstance()->GetMissionIdByToken(node->abilityToken_, missionId);
    WLOGFI("split window missionId is: %{public}d", missionId);
    want.SetParam("windowMode", modeData);
    want.SetParam("missionId", missionId);
    EventFwk::CommonEventData commonEventData;
    commonEventData.SetWant(want);
    EventFwk::CommonEventManager::PublishCommonEvent(commonEventData);
    // set ipc identity to raw
    IPCSkeleton::SetCallingIdentity(identity);
    WLOGFI("send split sceen event finish.");
}

sptr<WindowNode> WindowNodeContainer::FindSplitPairNode(sptr<WindowNode>& triggerNode) const
{
    auto triggerMode = triggerNode->GetWindowMode();
    for (auto iter = appWindowNode_->children_.rbegin(); iter != appWindowNode_->children_.rend(); iter++) {
        if ((*iter)->GetWindowId() == triggerNode->GetWindowId()) {
            continue;
        }
        // Find Top FullScreen main winodow or top paired split mode app main window
        if ((*iter)->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN ||
            (triggerMode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY &&
            (*iter)->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) ||
            (triggerMode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY &&
            (*iter)->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_PRIMARY)) {
            WLOGFI("Find pair node mode is: %{public}d", static_cast<uint32_t>((*iter)->GetWindowMode()));
            return *iter;
        }
    }
    return nullptr;
}

void WindowNodeContainer::MinimizeWindowFromAbility(const sptr<WindowNode>& node, bool fromUser)
{
    if (node->abilityToken_ == nullptr) {
        WLOGFW("Target abilityToken is nullptr, windowId:%{public}u", node->GetWindowId());
        return;
    }
    AAFwk::AbilityManagerClient::GetInstance()->MinimizeAbility(node->abilityToken_, fromUser);
}

WMError WindowNodeContainer::MinimizeAppNodeExceptOptions(bool fromUser, const std::vector<uint32_t> &exceptionalIds,
                                                          const std::vector<WindowMode> &exceptionalModes)
{
    if (appWindowNode_->children_.empty()) {
        return WMError::WM_OK;
    }
    for (auto& appNode : appWindowNode_->children_) {
        // exclude exceptional window
        if (std::find(exceptionalIds.begin(), exceptionalIds.end(), appNode->GetWindowId()) != exceptionalIds.end() ||
            std::find(exceptionalModes.begin(), exceptionalModes.end(),
                appNode->GetWindowMode()) != exceptionalModes.end() ||
                appNode->GetWindowType() != WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
            continue;
        }
        // minimize window
        WLOGFI("minimize window, windowId:%{public}u", appNode->GetWindowId());
        MinimizeWindowFromAbility(appNode, fromUser);
    }
    return WMError::WM_OK;
}

WMError WindowNodeContainer::EnterSplitWindowMode(sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    WLOGFI("Enter split window mode: %{public}d", node->GetWindowId());
    SwitchLayoutPolicy(WindowLayoutMode::CASCADE); // window split mode is belong to cascade
    auto pairNode = FindSplitPairNode(node);
    if (pairNode != nullptr) {
        WLOGFI("Window %{public}d find pair %{public}d", node->GetWindowId(), pairNode->GetWindowId());
        WMError ret = UpdateWindowPairInfo(node, pairNode);
        if (ret != WMError::WM_OK) {
            return ret;
        }
        SingletonContainer::Get<WindowInnerManager>().SendMessage(INNER_WM_CREATE_DIVIDER, displayId_);
        std::vector<uint32_t> exceptionalIds;
        for (auto iter = pairedWindowMap_.begin(); iter != pairedWindowMap_.end(); iter++) {
            exceptionalIds.emplace_back(iter->first);
        }
        std::vector<WindowMode> exceptionalModes = { WindowMode::WINDOW_MODE_FLOATING, WindowMode::WINDOW_MODE_PIP };
        ret = MinimizeAppNodeExceptOptions(false, exceptionalIds, exceptionalModes);
        if (ret != WMError::WM_OK) {
            return ret;
        }
    } else {
        SendSplitScreenEvent(node);
    }
    return WMError::WM_OK;
}

void WindowNodeContainer::ResetLayoutPolicy()
{
    layoutPolicy_->Reset();
}

WMError WindowNodeContainer::ExitSplitWindowMode(sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    WLOGFI("exit split window mode %{public}d", node->GetWindowId());
    node->GetWindowProperty()->ResumeLastWindowMode();
    node->GetWindowToken()->UpdateWindowMode(node->GetWindowMode());
    if (pairedWindowMap_.count(node->GetWindowId()) != 0) {
        auto pairNode = pairedWindowMap_.at(node->GetWindowId()).pairNode_;
        pairNode->GetWindowProperty()->ResumeLastWindowMode();
        pairNode->GetWindowToken()->UpdateWindowMode(pairNode->GetWindowMode());
        pairedWindowMap_.erase(pairNode->GetWindowId());
        pairedWindowMap_.erase(node->GetWindowId());
        WLOGFI("resume pair node mode, Id[%{public}d, %{public}d], Mode[%{public}d, %{public}d]", node->GetWindowId(),
            pairNode->GetWindowId(), node->GetWindowMode(), pairNode->GetWindowMode());
    }
    if (pairedWindowMap_.empty()) {
        WLOGFI("send destroy msg to divider, Id: %{public}d", node->GetWindowId());
        SingletonContainer::Get<WindowInnerManager>().SendMessage(INNER_WM_DESTROY_DIVIDER, displayId_);
    }
    ResetLayoutPolicy();
    return WMError::WM_OK;
}

WMError WindowNodeContainer::UpdateWindowPairInfo(sptr<WindowNode>& triggerNode, sptr<WindowNode>& pairNode)
{
    float splitRatio = DEFAULT_SPLIT_RATIO;
    WindowMode triggerMode = triggerNode->GetWindowMode();
    WindowMode pairSrcMode = pairNode->GetWindowMode();
    if (pairSrcMode == WindowMode::WINDOW_MODE_FULLSCREEN) {
        WindowMode pairDstMode = (triggerMode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY) ?
            WindowMode::WINDOW_MODE_SPLIT_SECONDARY : WindowMode::WINDOW_MODE_SPLIT_PRIMARY;
        pairNode->SetWindowMode(pairDstMode);
        pairNode->GetWindowToken()->UpdateWindowMode(pairDstMode);
        WMError ret = UpdateWindowNode(pairNode, WindowUpdateReason::UPDATE_MODE);
        if (ret != WMError::WM_OK) {
            WLOGFE("Update window pair info failed");
            return ret;
        }
        WLOGFI("Pair FullScreen [%{public}d, %{public}d], Mode[%{public}d, %{public}d], splitRatio = %{public}f",
            triggerNode->GetWindowId(), pairNode->GetWindowId(), triggerMode, pairDstMode, splitRatio);
    } else {
        if (pairedWindowMap_.count(pairNode->GetWindowId()) != 0) {
            WindowPairInfo info = pairedWindowMap_.at(pairNode->GetWindowId());
            auto prevPairNode = info.pairNode_;
            WLOGFI("%{public}d node is paird , pre paired id is %{public}d,",
                pairNode->GetWindowId(), prevPairNode->GetWindowId());
            prevPairNode->GetWindowProperty()->ResumeLastWindowMode();
            prevPairNode->GetWindowToken()->UpdateWindowMode(prevPairNode->GetWindowMode());

            pairedWindowMap_.erase(prevPairNode->GetWindowId());
            pairedWindowMap_.erase(pairNode->GetWindowId());

            splitRatio = info.splitRatio_;
            WLOGFI("Pair Split [%{public}d, %{public}d], Mode[%{public}d, %{public}d], splitRatio = %{public}f",
                triggerNode->GetWindowId(), pairNode->GetWindowId(), triggerMode, pairSrcMode, splitRatio);
        } else {
            WLOGFI("%{public}d node is not paird", pairNode->GetWindowId());
        }
    }
    pairedWindowMap_.insert(std::pair<uint32_t, WindowPairInfo>(triggerNode->GetWindowId(),
        {pairNode, splitRatio}));
    pairedWindowMap_.insert(std::pair<uint32_t, WindowPairInfo>(pairNode->GetWindowId(),
        {triggerNode, 1 - splitRatio}));
    return WMError::WM_OK;
}

WMError WindowNodeContainer::SwitchLayoutPolicy(WindowLayoutMode dstMode, bool reorder)
{
    WLOGFI("SwitchLayoutPolicy src: %{public}d dst: %{public}d reorder: %{public}d",
        static_cast<uint32_t>(layoutMode_), static_cast<uint32_t>(dstMode), static_cast<uint32_t>(reorder));
    if (dstMode < WindowLayoutMode::BASE || dstMode >= WindowLayoutMode::END) {
        WLOGFE("invalid layout mode");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (layoutMode_ != dstMode) {
        if (layoutMode_ == WindowLayoutMode::CASCADE && !pairedWindowMap_.empty()) {
            pairedWindowMap_.clear();
            SingletonContainer::Get<WindowInnerManager>().SendMessage(INNER_WM_DESTROY_DIVIDER, displayId_);
        }
        layoutMode_ = dstMode;
        layoutPolicy_->Clean();
        layoutPolicy_ = layoutPolicys_[dstMode];
        layoutPolicy_->Launch();
        DumpScreenWindowTree();
    } else {
        WLOGFI("Current layout mode is already: %{public}d", static_cast<uint32_t>(dstMode));
    }
    if (reorder) {
        if (!pairedWindowMap_.empty()) {
            // exit divider window when reorder
            pairedWindowMap_.clear();
            SingletonContainer::Get<WindowInnerManager>().SendMessage(INNER_WM_DESTROY_DIVIDER, displayId_);
        }
        layoutPolicy_->Reorder();
        DumpScreenWindowTree();
    }
    NotifyIfSystemBarTintChanged();
    return WMError::WM_OK;
}

void WindowNodeContainer::RaiseInputMethodWindowPriorityIfNeeded(const sptr<WindowNode>& node) const
{
    if (node->GetWindowType() != WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        return;
    }
    auto iter = std::find_if(aboveAppWindowNode_->children_.begin(), aboveAppWindowNode_->children_.end(),
                             [](sptr<WindowNode> node) {
        return node->GetWindowType() == WindowType::WINDOW_TYPE_KEYGUARD;
    });
    if (iter != aboveAppWindowNode_->children_.end()) {
        WLOGFI("raise input method float window priority.");
        node->priority_ = WINDOW_TYPE_RAISED_INPUT_METHOD;
    }
}

void WindowNodeContainer::MoveWindowNode(sptr<WindowNodeContainer>& container)
{
    DisplayId from = container->GetDisplayId();
    WLOGFI("disconnect expand display: %{public}" PRId64 ", move window node to display: "
        "%{public}" PRId64 "", from, displayId_);
    for (auto& node : container->aboveAppWindowNode_->children_) {
        WLOGFI("aboveAppWindowNode_: move windowNode: %{public}d", node->GetWindowId());
        aboveAppWindowNode_->children_.push_back(node);
        layoutPolicy_->AddWindowNode(node);
    }
    for (auto& node : container->appWindowNode_->children_) {
        WLOGFI("appWindowNode_: move windowNode: %{public}d", node->GetWindowId());
        appWindowNode_->children_.push_back(node);
        layoutPolicy_->AddWindowNode(node);
    }
    for (auto& node : container->belowAppWindowNode_->children_) {
        WLOGFI("belowAppWindowNode_: move windowNode: %{public}d", node->GetWindowId());
        belowAppWindowNode_->children_.push_back(node);
        layoutPolicy_->AddWindowNode(node);
    }
}

void WindowNodeContainer::TraverseWindowTree(const WindowNodeOperationFunc& func, bool isFromTopToBottom) const
{
    std::vector<sptr<WindowNode>> rootNodes = { belowAppWindowNode_, appWindowNode_, aboveAppWindowNode_ };
    if (isFromTopToBottom) {
        std::reverse(rootNodes.begin(), rootNodes.end());
    }

    for (auto& node : rootNodes) {
        if (isFromTopToBottom) {
            for (auto iter = node->children_.rbegin(); iter != node->children_.rend(); ++iter) {
                if (TraverseFromTopToBottom(*iter, func)) {
                    return;
                }
            }
        } else {
            for (auto iter = node->children_.begin(); iter != node->children_.end(); ++iter) {
                if (TraverseFromBottomToTop(*iter, func)) {
                    return;
                }
            }
        }
    }
}

bool WindowNodeContainer::TraverseFromTopToBottom(sptr<WindowNode> node, const WindowNodeOperationFunc& func) const
{
    if (node == nullptr) {
        return false;
    }
    auto iterBegin = node->children_.rbegin();
    for (; iterBegin != node->children_.rend(); ++iterBegin) {
        if ((*iterBegin)->priority_ <= 0) {
            break;
        }
        if (func(*iterBegin)) {
            return true;
        }
    }
    if (func(node)) {
        return true;
    }
    for (; iterBegin != node->children_.rend(); ++iterBegin) {
        if (func(*iterBegin)) {
            return true;
        }
    }
    return false;
}

bool WindowNodeContainer::TraverseFromBottomToTop(sptr<WindowNode> node, const WindowNodeOperationFunc& func) const
{
    if (node == nullptr) {
        return false;
    }
    auto iterBegin = node->children_.begin();
    for (; iterBegin != node->children_.end(); ++iterBegin) {
        if ((*iterBegin)->priority_ >= 0) {
            break;
        }
        if (func(*iterBegin)) {
            return true;
        }
    }
    if (func(node)) {
        return true;
    }
    for (; iterBegin != node->children_.end(); ++iterBegin) {
        if (func(*iterBegin)) {
            return true;
        }
    }
    return false;
}

void WindowNodeContainer::UpdateWindowVisibilityInfos(std::vector<sptr<WindowVisibilityInfo>>& infos)
{
    currentCoveredArea_.clear();
    WindowNodeOperationFunc func = [this, &infos](sptr<WindowNode> node) {
        if (node == nullptr) {
            return false;
        }
        Rect layoutRect = node->GetLayoutRect();
        int32_t nodeX = std::max(0, layoutRect.posX_);
        int32_t nodeY = std::max(0, layoutRect.posY_);
        int32_t nodeXEnd = std::min(displayRect_.posX_ + static_cast<int32_t>(displayRect_.width_),
            layoutRect.posX_ + static_cast<int32_t>(layoutRect.width_));
        int32_t nodeYEnd = std::min(displayRect_.posY_ + static_cast<int32_t>(displayRect_.height_),
            layoutRect.posY_ + static_cast<int32_t>(layoutRect.height_));

        Rect rectInDisplay = {nodeX, nodeY,
                              static_cast<uint32_t>(nodeXEnd - nodeX), static_cast<uint32_t>(nodeYEnd - nodeY)};
        bool isCovered = false;
        for (auto& rect : currentCoveredArea_) {
            if (rectInDisplay.IsInsideOf(rect)) {
                isCovered = true;
                WLOGD("UpdateWindowVisibilityInfos: find covered window:%{public}u", node->GetWindowId());
                break;
            }
        }
        if (!isCovered) {
            currentCoveredArea_.emplace_back(rectInDisplay);
        }
        if (isCovered != node->isCovered_) {
            node->isCovered_ = isCovered;
            infos.emplace_back(new WindowVisibilityInfo(node->GetWindowId(), node->GetCallingPid(),
                node->GetCallingUid(), !isCovered));
            WLOGD("UpdateWindowVisibilityInfos: covered status changed window:%{public}u, covered:%{public}d",
                node->GetWindowId(), isCovered);
        }
        return false;
    };
    TraverseWindowTree(func, true);
    WindowManagerAgentController::GetInstance().UpdateWindowVisibilityInfo(infos);
}

float WindowNodeContainer::GetVirtualPixelRatio() const
{
    return layoutPolicy_->GetVirtualPixelRatio();
}

bool WindowNodeContainer::ReadIsWindowAnimationEnabledProperty()
{
    if (access(DISABLE_WINDOW_ANIMATION_PATH, F_OK) == 0) {
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
