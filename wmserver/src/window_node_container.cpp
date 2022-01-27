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

#include <algorithm>
#include <ability_manager_client.h>
#include <cinttypes>

#include "window_helper.h"
#include "window_manager_agent_controller.h"
#include "window_inner_manager.h"
#include "window_manager_hilog.h"
#include "wm_trace.h"
#include "common_event_manager.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WindowNodeContainer"};
}

WindowNodeContainer::WindowNodeContainer(uint64_t screenId, uint32_t width, uint32_t height) : screenId_(screenId)
{
    struct RSDisplayNodeConfig config = {screenId};
    displayNode_ = RSDisplayNode::Create(config);
    Rect displayRect = {
        .posX_ = 0,
        .posY_ = 0,
        .width_ = width,
        .height_ = height
    };
    displayRects_->InitRect(displayRect);
    UpdateDisplayInfo();
    UpdateAvoidAreaFunc func = std::bind(&WindowNodeContainer::OnAvoidAreaChange, this, std::placeholders::_1);
    avoidController_ = new AvoidAreaController(func);
}

WindowNodeContainer::~WindowNodeContainer()
{
    Destroy();
}

WMError WindowNodeContainer::MinimizeOtherFullScreenAbility()
{
    if (appWindowNode_->children_.empty()) {
        return WMError::WM_OK;
    }
    for (auto iter = appWindowNode_->children_.begin(); iter < appWindowNode_->children_.end() - 1; ++iter) {
        if ((*iter)->GetWindowMode() != WindowMode::WINDOW_MODE_FULLSCREEN) {
            continue;
        }
        if  ((*iter)->abilityToken_ != nullptr) {
            WLOGFI("Find previous fullscreen window, notify ability to minimize");
            AAFwk::AbilityManagerClient::GetInstance()->MinimizeAbility((*iter)->abilityToken_);
        }
    }
    return WMError::WM_OK;
}

WMError WindowNodeContainer::MinimizeAllAppNodeAbility()
{
    if (appWindowNode_->children_.empty()) {
        return WMError::WM_OK;
    }
    for (auto iter = appWindowNode_->children_.begin(); iter != appWindowNode_->children_.end(); ++iter) {
        if  ((*iter)->abilityToken_ != nullptr) {
            WLOGFI("Notify ability to minimize");
            AAFwk::AbilityManagerClient::GetInstance()->MinimizeAbility((*iter)->abilityToken_);
        }
    }
    return WMError::WM_OK;
}

WMError WindowNodeContainer::AddWindowNode(sptr<WindowNode>& node, sptr<WindowNode>& parentNode)
{
    if (!node->surfaceNode_ || !displayNode_) {
        WLOGFE("surface node or display node is nullptr!");
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
        if (node->GetWindowType() == WindowType::WINDOW_TYPE_STATUS_BAR ||
            node->GetWindowType() == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
            sysBarNodeMap_[node->GetWindowType()] = node;
        }
    }
    node->parent_ = parentNode;

    if (node->IsSplitMode()) {
        WMError ret = HandleSplitWindowModeChange(node, true);
        if (ret != WMError::WM_OK) {
            WLOGFE("Add split window failed!");
            return ret;
        }
    }
    UpdateWindowTree(node);
    UpdateRSTree(node, true);
    AssignZOrder();
    layoutPolicy_->AddWindowNode(node);
    UpdateFocusWindow();
    if (avoidController_->IsAvoidAreaNode(node)) {
        avoidController_->AddAvoidAreaNode(node);
        NotifyIfSystemBarRegionChanged();
    }
    NotifyIfSystemBarTintChanged();
    DumpScreenWindowTree();
    WLOGFI("AddWindowNode windowId: %{public}d end", node->GetWindowId());
    return WMError::WM_OK;
}

WMError WindowNodeContainer::UpdateWindowNode(sptr<WindowNode>& node)
{
    if (!node->surfaceNode_ || !displayNode_) {
        WLOGFE("surface node or display node is nullptr!");
        return WMError::WM_ERROR_NULLPTR;
    }
    layoutPolicy_->UpdateWindowNode(node);
    if (avoidController_->IsAvoidAreaNode(node)) {
        avoidController_->UpdateAvoidAreaNode(node);
        NotifyIfSystemBarRegionChanged();
    }
    NotifyIfSystemBarTintChanged();
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
    if (displayNode_ == nullptr) {
        WLOGFE("displayNode_ is nullptr");
        return false;
    }
    if (isAdd) {
        displayNode_->AddChild(node->surfaceNode_, -1);
        for (auto& child : node->children_) {
            if (child->currentVisibility_) {
                displayNode_->AddChild(child->surfaceNode_, -1);
            }
        }
    } else {
        displayNode_->RemoveChild(node->surfaceNode_);
        for (auto& child : node->children_) {
            displayNode_->RemoveChild(child->surfaceNode_);
        }
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
        if (child->surfaceNode_ != nullptr && displayNode_ != nullptr) {
            displayNode_->RemoveChild(child->surfaceNode_);
            child->surfaceNode_ = nullptr;
        }
    }
    node->children_.clear();
    return WMError::WM_OK;
}

WMError WindowNodeContainer::RemoveWindowNode(sptr<WindowNode>& node)
{
    if (node == nullptr || !node->surfaceNode_) {
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
    for (auto& child : node->children_) {
        child->currentVisibility_ = false;
    }

    if (node->IsSplitMode()) {
        WMError ret = HandleSplitWindowModeChange(node, false);
        if (ret != WMError::WM_OK) {
            WLOGFE("Remove split window failed!");
            return ret;
        }
    }
    UpdateRSTree(node, false);
    UpdateFocusWindow();
    layoutPolicy_->RemoveWindowNode(node);
    if (avoidController_->IsAvoidAreaNode(node)) {
        avoidController_->RemoveAvoidAreaNode(node);
        NotifyIfSystemBarRegionChanged();
    }
    NotifyIfSystemBarTintChanged();
    DumpScreenWindowTree();
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
    displayNode_ = nullptr;
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

void WindowNodeContainer::UpdateFocusWindow()
{
    for (auto iter = appWindowNode_->children_.rbegin(); iter < appWindowNode_->children_.rend(); iter++) {
        if ((*iter)->GetWindowProperty()->GetFocusable()) {
            WLOGFI("find appWindow focused id %{public}d;", (*iter)->GetWindowId());
            SetFocusWindow((*iter)->GetWindowId());
            return;
        }
    }
    for (auto iter = belowAppWindowNode_->children_.rbegin(); iter < belowAppWindowNode_->children_.rend(); iter++) {
        if ((*iter)->GetWindowProperty()->GetFocusable()) {
            WLOGFI("find belowAppWindow focused id %{public}d;", (*iter)->GetWindowId());
            SetFocusWindow((*iter)->GetWindowId());
            return;
        }
    }
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
    }
}

void WindowNodeContainer::AssignZOrder()
{
    zOrder_ = 0;
    for (auto& node : belowAppWindowNode_->children_) {
        AssignZOrder(node);
    }
    for (auto& node : appWindowNode_->children_) {
        AssignZOrder(node);
    }
    for (auto& node : aboveAppWindowNode_->children_) {
        AssignZOrder(node);
    }
}

void WindowNodeContainer::AssignZOrder(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        return;
    }
    auto iter = node->children_.begin();
    for (; iter < node->children_.end(); ++iter) {
        if ((*iter)->priority_ < 0) {
            if ((*iter)->surfaceNode_) {
                (*iter)->surfaceNode_->SetPositionZ(zOrder_);
                ++zOrder_;
            }
        } else {
            break;
        }
    }
    if (node->surfaceNode_) {
        node->surfaceNode_->SetPositionZ(zOrder_);
        ++zOrder_;
    }
    for (; iter < node->children_.end(); ++iter) {
        if ((*iter)->surfaceNode_) {
            (*iter)->surfaceNode_->SetPositionZ(zOrder_);
            ++zOrder_;
        }
    }
}

WMError WindowNodeContainer::SetFocusWindow(uint32_t windowId)
{
    if (focusedWindow_ == windowId) {
        WLOGFI("focused window do not change");
        return WMError::WM_OK;
    }
    UpdateFocusStatus(focusedWindow_, false);
    focusedWindow_ = windowId;
    UpdateFocusStatus(focusedWindow_, true);
    return WMError::WM_OK;
}

uint32_t WindowNodeContainer::GetFocusWindow() const
{
    return focusedWindow_;
}

sptr<WindowNode> WindowNodeContainer::GetTopImmersiveNode() const
{
    if (appWindowNode_->children_.empty()) {
        return nullptr;
    }
    auto iter = appWindowNode_->children_.rbegin();
    for (; iter < appWindowNode_->children_.rend(); ++iter) {
        auto mode = (*iter)->GetWindowMode();
        auto flags = (*iter)->GetWindowFlags();
        if (mode == WindowMode::WINDOW_MODE_FULLSCREEN &&
            !(flags & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID))) {
            return (*iter);
        }
    }
    return nullptr;
}

void WindowNodeContainer::NotifyIfSystemBarTintChanged()
{
    WM_FUNCTION_TRACE();
    auto node = GetTopImmersiveNode();
    SystemBarRegionTints tints;
    if (node == nullptr) { // use default system bar
        WLOGFI("no immersive window on top");
        for (auto it : sysBarTintMap_) {
            if (it.second.prop_ == SystemBarProperty()) {
                continue;
            }
            WLOGFI("system bar prop change to default");
            sysBarTintMap_[it.first].prop_ = SystemBarProperty();
            sysBarTintMap_[it.first].type_ = it.first;
            tints.emplace_back(sysBarTintMap_[it.first]);
        }
    } else { // use node-defined system bar
        WLOGFI("top immersive window id: %{public}d", node->GetWindowId());
        auto& sysBarPropMap = node->GetSystemBarProperty();
        for (auto it : sysBarTintMap_) {
            if (sysBarPropMap.find(it.first) == sysBarPropMap.end()) {
                continue;
            }
            auto& prop = sysBarPropMap.find(it.first)->second;
            if (it.second.prop_ == prop) {
                continue;
            }
            WLOGFI("system bar prop update winId: %{public}d, type: %{public}d" \
                "visible: %{public}d, Color: %{public}x | %{public}x",
                node->GetWindowId(), static_cast<int32_t>(it.first),
                prop.enable_, prop.backgroundColor_, prop.contentColor_);
            sysBarTintMap_[it.first].prop_ = prop;
            sysBarTintMap_[it.first].type_ = it.first;
            tints.emplace_back(sysBarTintMap_[it.first]);
        }
    }
    WindowManagerAgentController::GetInstance().UpdateSystemBarRegionTints(screenId_, tints);
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
    WindowManagerAgentController::GetInstance().UpdateSystemBarRegionTints(screenId_, tints);
}

void WindowNodeContainer::TraverseContainer(std::vector<sptr<WindowNode>>& windowNodes)
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

void WindowNodeContainer::TraverseWindowNode(sptr<WindowNode>& node, std::vector<sptr<WindowNode>>& windowNodes)
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

void WindowNodeContainer::UpdateDisplayInfo()
{
    const Rect& primaryRect = displayRects_->GetRectByWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    const Rect& secondaryRect =  displayRects_->GetRectByWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    const Rect& displayRect =  displayRects_->GetRectByWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    layoutPolicy_->UpdateDisplayInfo(primaryRect, secondaryRect, displayRect);
}

void WindowNodeContainer::LayoutDividerWindow(sptr<WindowNode>& node)
{
    layoutPolicy_->UpdateLayoutRect(node);
    auto layoutRect = node->GetLayoutRect();
    displayRects_->SetSplitRect(layoutRect); // calculate primary/secondary depend on divider rect
    UpdateDisplayInfo();
    WLOGFI("UpdateDividerRects WinId: %{public}d, Rect: %{public}d %{public}d %{public}d %{public}d",
        node->GetWindowId(), layoutRect.posX_, layoutRect.posY_, layoutRect.width_, layoutRect.height_);
}

void WindowNodeContainer::DisplayRects::InitRect(Rect& oriDisplayRect)
{
    if (oriDisplayRect.width_ < oriDisplayRect.height_) {
        isVertical_ = true;
    }
    displayRect_ = oriDisplayRect;
    if (!isVertical_) {
        dividerRect_ = { static_cast<uint32_t>((displayRect_.width_ - DIVIDER_WIDTH) * DEFAULT_SPLIT_RATIO), 0,
                DIVIDER_WIDTH, displayRect_.height_ };
    } else {
        dividerRect_ = { 0, static_cast<uint32_t>((displayRect_.height_ - DIVIDER_WIDTH) * DEFAULT_SPLIT_RATIO),
               displayRect_.width_, DIVIDER_WIDTH };
    }
    WLOGFI("init dividerRect :[%{public}d, %{public}d, %{public}d, %{public}d]",
        dividerRect_.posX_, dividerRect_.posY_, dividerRect_.width_, dividerRect_.height_);
    SetSplitRect(dividerRect_);
}

void WindowNodeContainer::DisplayRects::SetSplitRect(float ratio)
{
    if (!isVertical_) {
        dividerRect_.posX_ = displayDependRect_.posX_ +
            static_cast<uint32_t>((displayDependRect_.width_ - dividerRect_.width_) * ratio);
    } else {
        dividerRect_.posY_ = displayDependRect_.posY_ +
            static_cast<uint32_t>((displayDependRect_.height_ - dividerRect_.height_) * ratio);
    }
    WLOGFI("set dividerRect :[%{public}d, %{public}d, %{public}d, %{public}d]",
        dividerRect_.posX_, dividerRect_.posY_, dividerRect_.width_, dividerRect_.height_);
    SetSplitRect(dividerRect_);
}

void WindowNodeContainer::DisplayRects::SetSplitRect(const Rect& divRect)
{
    dividerRect_.width_ = divRect.width_;
    dividerRect_.height_ = divRect.height_;
    if (!isVertical_) {
        primaryRect_.posX_ = displayRect_.posX_;
        primaryRect_.posY_ = displayRect_.posY_;
        primaryRect_.width_ = divRect.posX_;
        primaryRect_.height_ = displayRect_.height_;

        secondaryRect_.posX_ = divRect.posX_ + dividerRect_.width_;
        secondaryRect_.posY_ = displayRect_.posY_;
        secondaryRect_.width_ = displayRect_.width_ - secondaryRect_.posX_;
        secondaryRect_.height_ = displayRect_.height_;
    } else {
        primaryRect_.posX_ = displayRect_.posX_;
        primaryRect_.posY_ = displayRect_.posY_;
        primaryRect_.height_ = divRect.posY_;
        primaryRect_.width_ = displayRect_.width_;

        secondaryRect_.posX_ = displayRect_.posX_;
        secondaryRect_.posY_ = divRect.posY_ + dividerRect_.height_;
        secondaryRect_.height_ = displayRect_.height_ - secondaryRect_.posY_;
        secondaryRect_.width_ = displayRect_.width_;
    }
}

Rect WindowNodeContainer::DisplayRects::GetDividerRect() const
{
    return dividerRect_;
}

Rect WindowNodeContainer::DisplayRects::GetRectByWindowMode(const WindowMode& mode) const
{
    if (mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY) {
        return primaryRect_;
    } else if (mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        return secondaryRect_;
    } else {
        return displayRect_;
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
    WLOGFI("-------- Screen %{public}" PRIu64" dump window info begin---------", screenId_);
    WLOGFI("WinId Type Mode Flag ZOrd [   x    y    w    h]");
    std::vector<sptr<WindowNode>> windowNodes;
    TraverseContainer(windowNodes);
    int zOrder = windowNodes.size();
    for (auto node : windowNodes) {
        Rect rect = node->GetLayoutRect();
        WLOGFI("%{public}5d %{public}4d %{public}4d %{public}4d %{public}4d [%{public}4d %{public}4d " \
            "%{public}4d %{public}4d]", node->GetWindowId(), node->GetWindowType(), node->GetWindowMode(),
            node->GetWindowFlags(), --zOrder, rect.posX_, rect.posY_, rect.width_, rect.height_);
    }
    WLOGFI("-------- Screen %{public}" PRIu64" dump window info end  ---------", screenId_);
}

uint64_t WindowNodeContainer::GetScreenId() const
{
    return screenId_;
}

Rect WindowNodeContainer::GetDisplayRect() const
{
    return displayRects_->GetRectByWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
}

std::shared_ptr<RSDisplayNode> WindowNodeContainer::GetDisplayNode() const
{
    return displayNode_;
}

bool WindowNodeContainer::isVerticalDisplay() const
{
    return displayRects_->isVertical_;
}

void WindowNodeContainer::SendSplitScreenEvent(WindowMode mode)
{
    // should define in common_event_support.h and @ohos.commonEvent.d.ts
    WLOGFI("send split sceen event , trigger mode is %{public}d", mode);
    const std::string eventName = "common.event.SPLIT_SCREEN";
    AAFwk::Want want;
    want.SetAction(eventName);
    EventFwk::CommonEventData commonEventData;
    commonEventData.SetWant(want);
    if (mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY) {
        commonEventData.SetData("Secondary");
    } else {
        commonEventData.SetData("Primary");
    }
    EventFwk::CommonEventManager::PublishCommonEvent(commonEventData);
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
            return *iter;
        }
    }
    return nullptr;
}

WMError WindowNodeContainer::HandleModeChangeToSplit(sptr<WindowNode>& triggerNode)
{
    WLOGFI("HandleModeChangeToSplit %{public}d", triggerNode->GetWindowId());
    auto pairNode = FindSplitPairNode(triggerNode);
    displayRects_->displayDependRect_ = layoutPolicy_->GetDependDisplayRects(); // get depend display rect for split
    WLOGFI("displayDependRect :[%{public}d, %{public}d, %{public}d, %{public}d]",
        displayRects_->displayDependRect_.posX_, displayRects_->displayDependRect_.posY_,
        displayRects_->displayDependRect_.width_, displayRects_->displayDependRect_.height_);
    if (pairNode != nullptr) {
        WLOGFI("Window %{public}d find pair %{public}d", triggerNode->GetWindowId(), pairNode->GetWindowId());
        WMError ret = UpdateWindowPairInfo(triggerNode, pairNode);
        if (ret != WMError::WM_OK) {
            return ret;
        }
    } else {
        // sent split event
        displayRects_->SetSplitRect(DEFAULT_SPLIT_RATIO);
        SendSplitScreenEvent(triggerNode->GetWindowMode());
    }
    UpdateDisplayInfo();
    return WMError::WM_OK;
}

WMError WindowNodeContainer::HandleModeChangeFromSplit(sptr<WindowNode>& triggerNode)
{
    WLOGFI("HandleModeChangeFromSplit %{public}d", triggerNode->GetWindowId());
    if (pairedWindowMap_.find(triggerNode->GetWindowId()) != pairedWindowMap_.end()) {
        WindowPairInfo info = pairedWindowMap_.at(triggerNode->GetWindowId());
        auto pairNode = info.pairNode;
        pairNode->GetWindowProperty()->ResumeLastWindowMode();
        pairNode->GetWindowToken()->UpdateWindowMode(pairNode->GetWindowMode());
        triggerNode->GetWindowProperty()->ResumeLastWindowMode();
        triggerNode->GetWindowToken()->UpdateWindowMode(triggerNode->GetWindowMode());
        pairedWindowMap_.erase(pairNode->GetWindowId());
        pairedWindowMap_.erase(triggerNode->GetWindowId());
        WLOGFI("Split out, Id[%{public}d, %{public}d], Mode[%{public}d, %{public}d]",
            triggerNode->GetWindowId(), pairNode->GetWindowId(),
            triggerNode->GetWindowMode(), pairNode->GetWindowMode());
    } else {
        WLOGFE("Split out, but can not find pair in map  %{public}d", triggerNode->GetWindowId());
        return WMError::WM_OK;
    }
    if (pairedWindowMap_.empty()) {
        WLOGFI("Notify devider to destroy");
        SingletonContainer::Get<WindowInnerManager>().SendMessage(INNER_WM_DESTROY_DIVIDER, screenId_);
    }
    return WMError::WM_OK;
}

WMError WindowNodeContainer::HandleSplitWindowModeChange(sptr<WindowNode>& triggerNode, bool isChangeToSplit)
{
    WM_FUNCTION_TRACE();
    return isChangeToSplit ? HandleModeChangeToSplit(triggerNode) : HandleModeChangeFromSplit(triggerNode);
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
        WMError ret = UpdateWindowNode(pairNode);
        if (ret != WMError::WM_OK) {
            WLOGFE("Update window pair info failed");
            return ret;
        }
        WLOGFI("Pair FullScreen [%{public}d, %{public}d], Mode[%{public}d, %{public}d], splitRatio = %{public}f",
            triggerNode->GetWindowId(), pairNode->GetWindowId(), triggerMode, pairDstMode, splitRatio);
    } else {
        if (pairedWindowMap_.count(pairNode->GetWindowId() != 0)) {
            WindowPairInfo info = pairedWindowMap_.at(pairNode->GetWindowId());
            auto prevPairNode = info.pairNode;
            prevPairNode->GetWindowProperty()->ResumeLastWindowMode();
            prevPairNode->GetWindowToken()->UpdateWindowMode(prevPairNode->GetWindowMode());

            pairedWindowMap_.erase(prevPairNode->GetWindowId());
            pairedWindowMap_.erase(pairNode->GetWindowId());

            splitRatio = info.splitRatio;
            WLOGFI("Pair Split [%{public}d, %{public}d], Mode[%{public}d, %{public}d], splitRatio = %{public}f",
                triggerNode->GetWindowId(), pairNode->GetWindowId(), triggerMode, pairSrcMode, splitRatio);
        }
    }

    pairedWindowMap_.insert(std::pair<uint32_t, WindowPairInfo>(triggerNode->GetWindowId(),
        {pairNode, splitRatio}));
    pairedWindowMap_.insert(std::pair<uint32_t, WindowPairInfo>(pairNode->GetWindowId(),
        {triggerNode, 1 - splitRatio}));
    displayRects_->SetSplitRect(splitRatio);
    WLOGFI("Notify devider to create");
    Rect dividerRect = displayRects_->GetDividerRect();
    SingletonContainer::Get<WindowInnerManager>().SendMessage(INNER_WM_CREATE_DIVIDER, screenId_, dividerRect);
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
}
}