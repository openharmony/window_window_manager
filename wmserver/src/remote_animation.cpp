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
#include <rs_window_animation_finished_callback.h>
#include "window_helper.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "RemoteAnimation"};
}

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

bool RemoteAnimation::CheckTransition(sptr<WindowTransitionInfo> srcInfo, const sptr<WindowNode>& srcNode,
    sptr<WindowTransitionInfo> dstInfo, const sptr<WindowNode>& dstNode)
{
    if (srcInfo->GetBundleName().find("permission") != std::string::npos ||
        dstInfo->GetBundleName().find("permission") != std::string::npos) {
        return false;
    }

    if (srcNode != nullptr && !srcNode->leashWinSurfaceNode_ && !srcNode->surfaceNode_) {
        return false;
    }

    if (dstNode != nullptr && !dstNode->leashWinSurfaceNode_ && !dstNode->surfaceNode_) {
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
        }
        return TransitionEvent::UNKNOWN;
    } else {
        if (WindowHelper::IsMainWindow(dstInfo->GetWindowType())) {
            return TransitionEvent::APP_TRANSITION;
        } else if (dstInfo->GetWindowType() == WindowType::WINDOW_TYPE_DESKTOP) {
            return TransitionEvent::HOME;
        }
    }
    return TransitionEvent::UNKNOWN;
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
        if (WindowHelper::IsAppWindow(node->GetWindowType()) && node->leashWinSurfaceNode_) {
            target->surfaceNode_ = node->leashWinSurfaceNode_;
        } else {
            target->surfaceNode_ = node->surfaceNode_;
        }
    }
    return target;
}

WMError RemoteAnimation::NotifyAnimationTransition(sptr<WindowTransitionInfo> srcInfo,
    sptr<WindowTransitionInfo> dstInfo, const sptr<WindowNode>& srcNode,
    const sptr<WindowNode>&  dstNode, bool needMinimizeSrcNode)
{
    sptr<RSWindowAnimationTarget> srcTarget = SetAnimationTarget(srcNode, srcInfo);
    sptr<RSWindowAnimationTarget> dstTarget = SetAnimationTarget(dstNode, dstInfo);
    if (srcTarget == nullptr || dstTarget == nullptr) {
        return WMError::WM_ERROR_NO_MEM;
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
        return WMError::WM_ERROR_NO_MEM;
    }
    windowAnimationController_->OnTransition(srcTarget, dstTarget, finishedCallback);
    return WMError::WM_OK;
}

WMError RemoteAnimation::NotifyAnimationMinimize(sptr<WindowTransitionInfo> srcInfo, const sptr<WindowNode>& srcNode)
{
    sptr<RSWindowAnimationTarget> srcTarget = SetAnimationTarget(srcNode, srcInfo);
    if (srcTarget == nullptr) {
        return WMError::WM_ERROR_NO_MEM;
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
        return WMError::WM_ERROR_NO_MEM;
    }
    windowAnimationController_->OnMinimizeWindow(srcTarget, finishedCallback);
    return WMError::WM_OK;
}

WMError RemoteAnimation::NotifyAnimationClose(sptr<WindowTransitionInfo> srcInfo, const sptr<WindowNode>& srcNode)
{
    sptr<RSWindowAnimationTarget> srcTarget = SetAnimationTarget(srcNode, srcInfo);
    if (srcTarget == nullptr) {
        return WMError::WM_ERROR_NO_MEM;
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
        return WMError::WM_ERROR_NO_MEM;
    }
    windowAnimationController_->OnCloseWindow(srcTarget, finishedCallback);
    return WMError::WM_OK;
}
} // Rosen
} // OHOS
