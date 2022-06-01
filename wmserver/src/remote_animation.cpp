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
#include <common/rs_rect.h>
#include <rs_window_animation_finished_callback.h>
#include "minimize_app.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "wm_trace.h"
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
    WLOGFI("RSWindowAnimation: set window animation controller!");
    if (controller == nullptr) {
        WLOGFE("RSWindowAnimation: failed to set window animation controller, controller is null!");
        return WMError::WM_ERROR_NULLPTR;
    }

    if (windowAnimationController_ != nullptr) {
        WLOGFI("RSWindowAnimation: maybe user switch!");
    }

    windowAnimationController_ = controller;
    return WMError::WM_OK;
}

bool RemoteAnimation::CheckAnimationController()
{
    if (windowAnimationController_ == nullptr) {
        WLOGFI("RSWindowAnimation: windowAnimationController_ null!");
        return false;
    }
    return true;
}

bool RemoteAnimation::CheckTransition(sptr<WindowTransitionInfo> srcInfo, const sptr<WindowNode>& srcNode,
    sptr<WindowTransitionInfo> dstInfo, const sptr<WindowNode>& dstNode)
{
    if (srcNode != nullptr && !srcNode->leashWinSurfaceNode_ && !srcNode->surfaceNode_) {
        WLOGFI("RSWindowAnimation: srcNode has no surface!");
        return false;
    }

    if (dstNode != nullptr && !dstNode->leashWinSurfaceNode_ && !dstNode->surfaceNode_) {
        WLOGFI("RSWindowAnimation: dstNode has no surface!");
        return false;
    }

    return CheckAnimationController();
}

void RemoteAnimation::OnRemoteDie(const sptr<IRemoteObject>& remoteObject)
{
    WLOGFI("RSWindowAnimation: OnRemoteDie!");
    if (windowAnimationController_ != nullptr && windowAnimationController_->AsObject() == remoteObject) {
        windowAnimationController_ = nullptr;
    }
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

WMError RemoteAnimation::NotifyAnimationTransition(sptr<WindowTransitionInfo> srcInfo,
    sptr<WindowTransitionInfo> dstInfo, const sptr<WindowNode>& srcNode,
    const sptr<WindowNode>&  dstNode)
{
    if (!dstNode || !dstNode->startingWindowShown_) {
        WLOGFE("RSWindowAnimation: no startingWindow for dst window id:%{public}u!", dstNode->GetWindowId());
        return WMError::WM_ERROR_NO_REMOTE_ANIMATION;
    }
    WLOGFI("RSWindowAnimation: nofity animation transition with dst currId:%{public}u!", dstNode->GetWindowId());
    sptr<RSWindowAnimationFinishedCallback> finishedCallback = new(std::nothrow) RSWindowAnimationFinishedCallback(
        []() {
            WLOGFI("RSWindowAnimation: on finish transition with minimizeAll!");
            WM_SCOPED_ASYNC_END(static_cast<int32_t>(TraceTaskId::REMOTE_ANIMATION),
                "wms:async:ShowRemoteAnimation");
            MinimizeApp::ExecuteMinimizeAll();
        }
    );
    if (finishedCallback == nullptr) {
        WLOGFE("New RSIWindowAnimationFinishedCallback failed");
        return WMError::WM_ERROR_NO_MEM;
    }

    auto dstTarget = CreateWindowAnimationTarget(dstInfo, dstNode);
    if (dstTarget == nullptr) {
        WLOGFE("RSWindowAnimation: Failed to create dst target!");
        finishedCallback->OnAnimationFinished();
        return WMError::WM_ERROR_NO_MEM;
    }
    dstNode->isPlayAnimationShow_ = true;
    bool needMinimizeSrcNode = MinimizeApp::IsNodeNeedMinimize(srcNode);
    // from app to app
    if (needMinimizeSrcNode && srcNode != nullptr) {
        auto srcTarget = CreateWindowAnimationTarget(srcInfo, srcNode);
        // to avoid normal animation
        srcNode->isPlayAnimationHide_ = true;
        WLOGFI("RSWindowAnimation: app transition from id:%{public}u to id:%{public}u!",
            srcNode->GetWindowId(), dstNode->GetWindowId());
        windowAnimationController_->OnAppTransition(srcTarget, dstTarget, finishedCallback);
        return WMError::WM_OK;
    }
    // start app from launcher
    if (srcNode != nullptr && srcNode->GetWindowType() == WindowType::WINDOW_TYPE_DESKTOP) {
        WLOGFI("RSWindowAnimation: start app id:%{public}u from launcher!", dstNode->GetWindowId());
        windowAnimationController_->OnStartApp(StartingAppType::FROM_LAUNCHER, dstTarget, finishedCallback);
        return WMError::WM_OK;
    }

    // start app from recent
    if (srcInfo != nullptr && srcInfo->GetIsRecent()) {
        WLOGFI("RSWindowAnimation: start app id:%{public}u from recent!", dstNode->GetWindowId());
        windowAnimationController_->OnStartApp(StartingAppType::FROM_RECENT, dstTarget, finishedCallback);
        return WMError::WM_OK;
    }

    // start app from other
    WLOGFI("RSWindowAnimation: start app id:%{public}u from other!", dstNode->GetWindowId());
    windowAnimationController_->OnStartApp(StartingAppType::FROM_OTHER, dstTarget, finishedCallback);
    return WMError::WM_OK;
}

WMError RemoteAnimation::NotifyAnimationMinimize(sptr<WindowTransitionInfo> srcInfo, const sptr<WindowNode>& srcNode)
{
    sptr<RSWindowAnimationTarget> srcTarget = CreateWindowAnimationTarget(srcInfo, srcNode);
    if (srcTarget == nullptr) {
        return WMError::WM_ERROR_NO_MEM;
    }
    WLOGFI("RSWindowAnimation: nofity animation minimize Id:%{public}u!", srcNode->GetWindowId());
    srcNode->isPlayAnimationHide_ = true;
    auto minimizeFunc = [srcNode]() {
        if (srcNode != nullptr && srcNode->abilityToken_ != nullptr) {
            WLOGFI("minimize windowId: %{public}u, name:%{public}s",
                srcNode->GetWindowId(), srcNode->GetWindowName().c_str());
            WM_SCOPED_ASYNC_END(static_cast<int32_t>(TraceTaskId::REMOTE_ANIMATION),
                "wms:async:ShowRemoteAnimation");
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
    sptr<RSWindowAnimationTarget> srcTarget = CreateWindowAnimationTarget(srcInfo, srcNode);
    if (srcTarget == nullptr) {
        return WMError::WM_ERROR_NO_MEM;
    }
    WLOGFI("RSWindowAnimation: nofity animation close id:%{public}u!", srcNode->GetWindowId());
    srcNode->isPlayAnimationHide_ = true;
    auto closeFunc = [srcNode]() {
        if (srcNode != nullptr && srcNode->abilityToken_ != nullptr) {
            WLOGFI("close windowId: %{public}u, name:%{public}s",
                srcNode->GetWindowId(), srcNode->GetWindowName().c_str());
            WM_SCOPED_ASYNC_END(static_cast<int32_t>(TraceTaskId::REMOTE_ANIMATION),
                "wms:async:ShowRemoteAnimation");
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

sptr<RSWindowAnimationTarget> RemoteAnimation::CreateWindowAnimationTarget(sptr<WindowTransitionInfo> info,
    const sptr<WindowNode>& windowNode)
{
    if (info == nullptr || windowNode == nullptr) {
        WLOGFW("Failed to create window animation target, info or window node is null!");
        return nullptr;
    }

    sptr<RSWindowAnimationTarget> windowAnimationTarget = new(std::nothrow) RSWindowAnimationTarget();
    if (windowAnimationTarget == nullptr) {
        WLOGFE("New RSWindowAnimationTarget failed");
        return nullptr;
    }
    windowAnimationTarget->bundleName_ = info->GetBundleName();
    windowAnimationTarget->abilityName_ = info->GetAbilityName();

    if (WindowHelper::IsAppWindow(windowNode->GetWindowType())) {
        windowAnimationTarget->surfaceNode_ = windowNode->leashWinSurfaceNode_;
    } else {
        windowAnimationTarget->surfaceNode_ = windowNode->surfaceNode_;
    }

    if (windowAnimationTarget->surfaceNode_ == nullptr) {
        WLOGFE("Window surface node is null!");
        return nullptr;
    }

    auto& stagingProperties = windowAnimationTarget->surfaceNode_->GetStagingProperties();
    auto bounds = stagingProperties.GetBounds();
    // 0, 1, 2, 3: convert bounds to RectF
    auto boundsRect = RectF(bounds[0], bounds[1], bounds[2], bounds[3]);
    windowAnimationTarget->windowBounds_ = RRect(boundsRect, stagingProperties.GetCornerRadius());
    return windowAnimationTarget;
}
} // Rosen
} // OHOS
