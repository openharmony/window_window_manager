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

#include "remote_animation.h"

#include <ability_manager_client.h>
#include <display_manager_service_inner.h>
#include <rs_window_animation_finished_callback.h>
#include <transaction/rs_transaction.h>
#include "window_helper.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "RemoteAnimation"};
    static SkBitmap g_bitmap;
}

SurfaceDraw RemoteAnimation::surfaceDraw_;
sptr<RSIWindowAnimationController> RemoteAnimation::windowAnimationController_ = nullptr;
std::map<TransitionReason, TransitionEvent> eventMap_ = {
    {TransitionReason::CLOSE, TransitionEvent::CLOSE},
    {TransitionReason::MINIMIZE, TransitionEvent::MINIMIZE},
    {TransitionReason::BACK, TransitionEvent::BACK}
};

WMError RemoteAnimation::SetWindowAnimationController(const sptr<RSIWindowAnimationController>& controller)
{
    if (controller == nullptr) {
        WLOGFE("RSWindowAnimation: failed to set window animation controller, controller is null!");
        return WMError::WM_ERROR_NULLPTR;
    }

    if (windowAnimationController_ != nullptr) {
        WLOGFE("RSWindowAnimation: failed to set window animation controller, Already had a controller!");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }

    windowAnimationController_ = controller;
    return WMError::WM_OK;
}

bool RemoteAnimation::CheckTransition(sptr<WindowTransitionInfo> srcInfo,
    sptr<WindowTransitionInfo> dstInfo)
{
    if (srcInfo->GetBundleName().find("permission") != std::string::npos ||
        dstInfo->GetBundleName().find("permission") != std::string::npos) {
        return false;
    }

    if (windowAnimationController_ == nullptr) {
        return false;
    }
    return true;
}

TransitionEvent RemoteAnimation::GetTransitionEvent(sptr<WindowTransitionInfo> srcInfo,
    sptr<WindowTransitionInfo> dstInfo, const sptr<WindowNode>& srcNode, const sptr<WindowNode>& dstNode)
{
    auto transitionReason = srcInfo->GetTransitionReason(); // src reason same as dst reason
    if (srcNode != nullptr && eventMap_.find(transitionReason) != eventMap_.end()) {
        return eventMap_[transitionReason];
    }
    WLOGFI("Ability Transition");
    if (dstNode == nullptr) {
        if (dstInfo->GetAbilityToken() == nullptr) {
            WLOGFE("target window abilityToken is null");
            return TransitionEvent::UNKNOWN;
        } else {
            return TransitionEvent::COLD_START;
        }
    } else {
        if (WindowHelper::IsMainWindow(dstInfo->GetWindowType())) {
            return TransitionEvent::HOT_START;
        } else if (dstInfo->GetWindowType() == WindowType::WINDOW_TYPE_DESKTOP) {
            return TransitionEvent::HOME;
        }
    }
    return TransitionEvent::UNKNOWN;
}

sptr<WindowNode> RemoteAnimation::CreateWindowNode(sptr<WindowTransitionInfo> info, uint32_t winId)
{
    sptr<WindowProperty> property = new(std::nothrow) WindowProperty();
    if (property == nullptr) {
        return nullptr;
    }
    property->SetRequestRect(info->GetWindowRect());
    property->SetWindowMode(info->GetWindowMode());
    property->SetDisplayId(info->GetDisplayId());
    property->SetWindowType(info->GetWindowType());
    if (info->GetShowFlagWhenLocked()) {
        property->AddWindowFlag(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED);
    }
    property->SetWindowId(winId);
    sptr<WindowNode> node = new(std::nothrow) WindowNode(property);
    if (node == nullptr) {
        return nullptr;
    }
    node->abilityToken_ = info->GetAbilityToken();

    if (CreateLeashAndStartingSurfaceNode(node) != WMError::WM_OK) {
        return nullptr;
    }

    return node;
}

WMError RemoteAnimation::CreateLeashAndStartingSurfaceNode(sptr<WindowNode>& node)
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "leashWindow" + std::to_string(node->GetWindowId());
    node->leashWinSurfaceNode_ = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    if (node->leashWinSurfaceNode_ == nullptr) {
        WLOGFE("create leashWinSurfaceNode failed");
        return WMError::WM_ERROR_NULLPTR;
    }

    rsSurfaceNodeConfig.SurfaceNodeName = "startingWindow" + std::to_string(node->GetWindowId());
    node->startingWinSurfaceNode_ = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    if (node->startingWinSurfaceNode_ == nullptr) {
        WLOGFE("create startingWinSurfaceNode failed");
        node->leashWinSurfaceNode_ = nullptr;
        return WMError::WM_ERROR_NULLPTR;
    }
    return WMError::WM_OK;
}

void RemoteAnimation::DrawStartingWindow(sptr<WindowNode>& node)
{
    Rect rect = node->GetWindowRect();
    const char *staringImgPath_ = "/etc/window/resources/starting_window.png";
    if (!surfaceDraw_.DecodeImageFile(staringImgPath_, g_bitmap)) {
        WLOGFE("load starting image failed");
        return;
    }
    surfaceDraw_.DrawBitmap(node->startingWinSurfaceNode_, rect, g_bitmap, 0xffffffff);
}

void RemoteAnimation::HandleClientWindowCreate(sptr<WindowNode>& node, sptr<IWindow>& window,
    uint32_t& windowId, const std::shared_ptr<RSSurfaceNode>& surfaceNode)
{
    node->surfaceNode_ = surfaceNode;
    node->SetWindowToken(window);
    node->SetCallingPid();
    node->SetCallingUid();
    windowId = node->GetWindowId();
    WLOGFI("after set Id:%{public}u, requestRect:[%{public}d, %{public}d, %{public}u, %{public}u]",
        node->GetWindowId(), node->GetRequestRect().posX_, node->GetRequestRect().posY_,
        node->GetRequestRect().width_, node->GetRequestRect().height_);

    // Register FirstFrame Callback to rs, replace startwin
    auto firstFrameCompleteCallback = [node]() {
        WLOGFI("Replace surfaceNode, id: %{public}u", node->GetWindowId());
        node->leashWinSurfaceNode_->RemoveChild(node->startingWinSurfaceNode_);
        node->leashWinSurfaceNode_->AddChild(node->surfaceNode_, -1);
        RSTransaction::FlushImplicitTransaction();
    };
    node->surfaceNode_->SetBufferAvailableCallback(firstFrameCompleteCallback);
    RSTransaction::FlushImplicitTransaction();
}

sptr<RSWindowAnimationTarget> RemoteAnimation::SetAnimationTarget(const sptr<WindowNode>& node,
    sptr<WindowTransitionInfo> info)
{
    sptr<RSWindowAnimationTarget> target = new(std::nothrow) RSWindowAnimationTarget();
    if (target == nullptr) {
        WLOGFE("new RSWindowAnimationTarget failed");
        return nullptr;
    }
    target->bundleName_ = info->GetBundleName();
    target->abilityName_ = info->GetAbilityName();
    if (node != nullptr) {
        if (WindowHelper::IsAppWindow(node->GetWindowType())) {
            target->surfaceNode_ = node->leashWinSurfaceNode_;
        } else {
            target->surfaceNode_ = node->surfaceNode_;
        }
    }
    return target;
}

void RemoteAnimation::NotifyAnimationTransition(sptr<WindowTransitionInfo> srcInfo,
    sptr<WindowTransitionInfo> dstInfo, const sptr<WindowNode>& srcNode,
    const sptr<WindowNode>&  dstNode, bool needMinimizeSrcNode)
{
    sptr<RSWindowAnimationTarget> srcTarget = SetAnimationTarget(srcNode, srcInfo);
    sptr<RSWindowAnimationTarget> dstTarget = SetAnimationTarget(dstNode, dstInfo);
    if (srcTarget == nullptr || dstTarget == nullptr) {
        return;
    }
    if (needMinimizeSrcNode && srcNode != nullptr) { // from App To App
        srcTarget->actionType_ = WindowAnimationActionType::GO_BACKGROUND;
        dstTarget->actionType_ = WindowAnimationActionType::GO_FOREGROUND;
        srcNode->isPlayAnimationHide_ = true;
        dstNode->isPlayAnimationShow_ = true;
    } else {
        // from launcher/recent/aa to app
        srcTarget->actionType_ = WindowAnimationActionType::NO_CHANGE;
        dstTarget->actionType_ = WindowAnimationActionType::GO_FOREGROUND;
        dstNode->isPlayAnimationShow_ = true;
    }
    auto callback = [srcNode, needMinimizeSrcNode]() {
        if (needMinimizeSrcNode && srcNode != nullptr && srcNode->abilityToken_ != nullptr) {
            WLOGFI("minimize windowId: %{public}u, name:%{public}s",
                srcNode->GetWindowId(), srcNode->GetWindowName().c_str());
            AAFwk::AbilityManagerClient::GetInstance()->MinimizeAbility(srcNode->abilityToken_, false);
        }
    };
    sptr<RSWindowAnimationFinishedCallback> finishedCallback = new(std::nothrow) RSWindowAnimationFinishedCallback(
        callback);
    if (finishedCallback == nullptr) {
        WLOGFE("New RSIWindowAnimationFinishedCallback failed");
        return;
    }
    windowAnimationController_->OnTransition(srcTarget, dstTarget, finishedCallback);
}

void RemoteAnimation::UpdateRSTree(sptr<WindowNode>& node)
{
    auto& dms = DisplayManagerServiceInner::GetInstance();
    DisplayId displayId = node->GetDisplayId();
    if (!node->surfaceNode_) { // cold start
        if (!WindowHelper::IsMainWindow(node->GetWindowType())) {
            WLOGFE("window id:%{public}d type: %{public}u is not Main Window!",
                node->GetWindowId(), static_cast<uint32_t>(node->GetWindowType()));
        }
        dms.UpdateRSTree(displayId, node->leashWinSurfaceNode_, true);
        node->leashWinSurfaceNode_->AddChild(node->startingWinSurfaceNode_, -1);
    } else { // hot start
        if (node->leashWinSurfaceNode_) { // to app
            dms.UpdateRSTree(displayId, node->leashWinSurfaceNode_, true);
        } else { // to launcher
            dms.UpdateRSTree(displayId, node->surfaceNode_, true);
        }
    }
}

void RemoteAnimation::NotifyAnimationMinimize(sptr<WindowTransitionInfo> srcInfo, const sptr<WindowNode>& srcNode)
{
    sptr<RSWindowAnimationTarget> srcTarget = SetAnimationTarget(srcNode, srcInfo);
    if (srcTarget == nullptr) {
        return;
    }
    srcTarget->actionType_ = WindowAnimationActionType::MINIMIZE;
    srcNode->isPlayAnimationHide_ = true;
    auto minimizeFunc = [srcNode]() {
        if (srcNode != nullptr && srcNode->abilityToken_ != nullptr) {
            WLOGFI("minimize windowId: %{public}u, name:%{public}s",
                srcNode->GetWindowId(), srcNode->GetWindowName().c_str());
            AAFwk::AbilityManagerClient::GetInstance()->MinimizeAbility(srcNode->abilityToken_, true);
        }
    };
    sptr<RSWindowAnimationFinishedCallback> finishedCallback = new(std::nothrow) RSWindowAnimationFinishedCallback(
        minimizeFunc);
    if (finishedCallback == nullptr) {
        WLOGFE("New RSIWindowAnimationFinishedCallback failed");
        return;
    }
    windowAnimationController_->OnMinimizeWindow(srcTarget, finishedCallback);
}

void RemoteAnimation::NotifyAnimationClose(sptr<WindowTransitionInfo> srcInfo, const sptr<WindowNode>& srcNode)
{
    sptr<RSWindowAnimationTarget> srcTarget = SetAnimationTarget(srcNode, srcInfo);
    if (srcTarget == nullptr) {
        return;
    }
    srcTarget->actionType_ = WindowAnimationActionType::CLOSE;
    srcNode->isPlayAnimationHide_ = true;
    auto closeFunc = [srcNode]() {
        if (srcNode != nullptr && srcNode->abilityToken_ != nullptr) {
            WLOGFI("close windowId: %{public}u, name:%{public}s",
                srcNode->GetWindowId(), srcNode->GetWindowName().c_str());
            AAFwk::AbilityManagerClient::GetInstance()->CloseAbility(srcNode->abilityToken_);
        }
    };
    sptr<RSWindowAnimationFinishedCallback> finishedCallback = new(std::nothrow) RSWindowAnimationFinishedCallback(
        closeFunc);
    if (finishedCallback == nullptr) {
        WLOGFE("New RSIWindowAnimationFinishedCallback failed");
        return;
    }
    windowAnimationController_->OnCloseWindow(srcTarget, finishedCallback);
}
} // Rosen
} // OHOS
