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
#include <hitrace_meter.h>
#include <transaction/rs_transaction.h>
#include "minimize_app.h"
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "RemoteAnimation"};
}
bool RemoteAnimation::isRemoteAnimationEnable_ = true;

sptr<RSIWindowAnimationController> RemoteAnimation::windowAnimationController_ = nullptr;
std::weak_ptr<AppExecFwk::EventHandler> RemoteAnimation::wmsTaskHandler_;
wptr<WindowRoot> RemoteAnimation::windowRoot_;

std::map<TransitionReason, TransitionEvent> eventMap_ = {
    {TransitionReason::CLOSE, TransitionEvent::CLOSE},
    {TransitionReason::MINIMIZE, TransitionEvent::MINIMIZE},
    {TransitionReason::BACK, TransitionEvent::BACK}
};

WMError RemoteAnimation::SetWindowAnimationController(const sptr<RSIWindowAnimationController>& controller,
    const sptr<WindowRoot>& windowRoot)
{
    WLOGFI("RSWindowAnimation: set window animation controller!");
    if (!isRemoteAnimationEnable_) {
        WLOGE("RSWindowAnimation: failed to set window animation controller, remote animation is not enabled");
        return WMError::WM_ERROR_NO_REMOTE_ANIMATION;
    }
    if (controller == nullptr) {
        WLOGFE("RSWindowAnimation: failed to set window animation controller, controller is null!");
        return WMError::WM_ERROR_NULLPTR;
    }

    if (windowAnimationController_ != nullptr) {
        WLOGFI("RSWindowAnimation: maybe user switch!");
    }

    windowAnimationController_ = controller;
    windowRoot_ = windowRoot;
    return WMError::WM_OK;
}

void RemoteAnimation::SetMainTaskHandler(std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    wmsTaskHandler_ = handler;
}

bool RemoteAnimation::CheckAnimationController()
{
    if (windowAnimationController_ == nullptr) {
        WLOGFD("RSWindowAnimation: windowAnimationController_ null!");
        return false;
    }
    return true;
}

bool RemoteAnimation::CheckRemoteAnimationEnabled(DisplayId displayId)
{
    // When the screen is locked, remote animation cannot take effect because the launcher is frozen.
    auto winRoot = windowRoot_.promote();
    if (winRoot == nullptr) {
        return false;
    }
    auto container = winRoot->GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr || container->IsScreenLocked()) {
        return false;
    }
    return CheckAnimationController();
}

bool RemoteAnimation::CheckTransition(sptr<WindowTransitionInfo> srcInfo, const sptr<WindowNode>& srcNode,
    sptr<WindowTransitionInfo> dstInfo, const sptr<WindowNode>& dstNode)
{
    if (srcNode == nullptr && dstNode == nullptr) {
        return false;
    }

    if (srcNode != nullptr && !srcNode->leashWinSurfaceNode_ && !srcNode->surfaceNode_) {
        WLOGFI("RSWindowAnimation: srcNode has no surface!");
        return false;
    }

    if (dstNode != nullptr && !dstNode->leashWinSurfaceNode_ && !dstNode->surfaceNode_) {
        WLOGFI("RSWindowAnimation: dstNode has no surface!");
        return false;
    }

    auto node = (dstNode != nullptr ? dstNode : srcNode);
    return CheckRemoteAnimationEnabled(node->GetDisplayId());
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

static sptr<RSWindowAnimationFinishedCallback> GetTransitionFinishedCallback(const sptr<WindowNode>& dstNode)
{
    wptr<WindowNode> weak = dstNode;
    auto callback = [weak]() {
        WLOGFI("RSWindowAnimation: on finish transition with minimize pre fullscreen!");
        auto weakNode = weak.promote();
        if (weakNode == nullptr) {
            WLOGFE("windowNode is nullptr!");
            return;
        }
        FinishAsyncTraceArgs(HITRACE_TAG_WINDOW_MANAGER, static_cast<int32_t>(TraceTaskId::REMOTE_ANIMATION),
            "wms:async:ShowRemoteAnimation");
        if (weakNode->state_ != WindowNodeState::SHOW_ANIMATION_PLAYING) {
            WLOGFI("node:%{public}d is not play show animation!", weakNode->GetWindowId());
            return;
        }
        MinimizeApp::ExecuteMinimizeAll(); // minimize execute in show animation
        RSAnimationTimingProtocol timingProtocol(200); // animation time
        RSNode::Animate(timingProtocol, RSAnimationTimingCurve::EASE_OUT, [weakNode]() {
            auto winRect = weakNode->GetWindowRect();
            WLOGFD("name:%{public}s id:%{public}u winRect:[x:%{public}d, y:%{public}d, w:%{public}d, h:%{public}d]",
                weakNode->GetWindowName().c_str(), weakNode->GetWindowId(),
                winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
            if (!weakNode->leashWinSurfaceNode_) {
                return;
            }
            weakNode->leashWinSurfaceNode_->SetBounds(
                winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
            RSTransaction::FlushImplicitTransaction();
        });
    };
    sptr<RSWindowAnimationFinishedCallback> finishedCallback = new(std::nothrow) RSWindowAnimationFinishedCallback(
        callback);
    return finishedCallback;
}

WMError RemoteAnimation::NotifyAnimationStartApp(sptr<WindowTransitionInfo> srcInfo,
    const sptr<WindowNode>& srcNode, const sptr<WindowNode>& dstNode,
    sptr<RSWindowAnimationTarget>& dstTarget, sptr<RSWindowAnimationFinishedCallback>& finishedCallback)
{
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

WMError RemoteAnimation::NotifyAnimationTransition(sptr<WindowTransitionInfo> srcInfo,
    sptr<WindowTransitionInfo> dstInfo, const sptr<WindowNode>& srcNode,
    const sptr<WindowNode>& dstNode)
{
    if (!dstNode || !dstNode->startingWindowShown_) {
        WLOGFE("RSWindowAnimation: no startingWindow for dst window id:%{public}u!", dstNode->GetWindowId());
        return WMError::WM_ERROR_NO_REMOTE_ANIMATION;
    }
    WLOGFI("RSWindowAnimation: notify animation transition with dst currId:%{public}u!", dstNode->GetWindowId());
    auto finishedCallback = GetTransitionFinishedCallback(dstNode);
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
    dstNode->state_ = WindowNodeState::SHOW_ANIMATION_PLAYING;
    // when exit immersive, startingWindow (0,0,w,h), but app need avoid
    bool needAvoid = (dstNode->GetWindowFlags() & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
    auto winRoot = windowRoot_.promote();
    if (needAvoid && winRoot) {
        auto avoidRect = winRoot->GetDisplayRectWithoutSystemBarAreas(dstNode->GetDisplayId());
        WLOGFI("name:%{public}s id:%{public}u avoidRect:[x:%{public}d, y:%{public}d, w:%{public}d, h:%{public}d]",
            dstNode->GetWindowName().c_str(), dstNode->GetWindowId(),
            avoidRect.posX_, avoidRect.posY_, avoidRect.width_, avoidRect.height_);
        if (WindowHelper::IsMainFullScreenWindow(dstNode->GetWindowType(), dstNode->GetWindowMode())) {
            auto& stagingProperties = dstTarget->surfaceNode_->GetStagingProperties();
            auto boundsRect = RectF(avoidRect.posX_, avoidRect.posY_, avoidRect.width_, avoidRect.height_);
            dstTarget->windowBounds_ = RRect(boundsRect, stagingProperties.GetCornerRadius());
        }
    }

    dstNode->isPlayAnimationShow_ = true;
    bool needMinimizeSrcNode = MinimizeApp::IsNodeNeedMinimize(srcNode);
    // from app to app
    if (needMinimizeSrcNode && srcNode != nullptr) {
        auto srcTarget = CreateWindowAnimationTarget(srcInfo, srcNode);
        // to avoid normal animation
        srcNode->isPlayAnimationHide_ = true;
        srcNode->state_ = WindowNodeState::HIDE_ANIMATION_PLAYING;
        if (winRoot) {
            winRoot->RemoveWindowNode(srcNode->GetWindowId());
        }
        WLOGFI("RSWindowAnimation: app transition from id:%{public}u to id:%{public}u!",
            srcNode->GetWindowId(), dstNode->GetWindowId());
        windowAnimationController_->OnAppTransition(srcTarget, dstTarget, finishedCallback);
        return WMError::WM_OK;
    }
    return NotifyAnimationStartApp(srcInfo, srcNode, dstNode, dstTarget, finishedCallback);
}

WMError RemoteAnimation::NotifyAnimationMinimize(sptr<WindowTransitionInfo> srcInfo, const sptr<WindowNode>& srcNode)
{
    sptr<RSWindowAnimationTarget> srcTarget = CreateWindowAnimationTarget(srcInfo, srcNode);
    if (srcTarget == nullptr) {
        return WMError::WM_ERROR_NO_MEM;
    }
    WLOGFI("RSWindowAnimation: notify animation minimize Id:%{public}u!", srcNode->GetWindowId());
    srcNode->isPlayAnimationHide_ = true;
    srcNode->state_ = WindowNodeState::HIDE_ANIMATION_PLAYING;
    auto winRoot = windowRoot_.promote();
    if (winRoot != nullptr) {
        winRoot->RemoveWindowNode(srcNode->GetWindowId());
    }
    wptr<WindowNode> weak = srcNode;
    auto minimizeFunc = [weak]() {
        auto weakNode = weak.promote();
        if (weakNode == nullptr || weakNode->state_ != WindowNodeState::HIDE_ANIMATION_PLAYING) {
            WLOGFE("windowNode is nullptr or is not play hide animation!");
            return;
        }
        FinishAsyncTraceArgs(HITRACE_TAG_WINDOW_MANAGER, static_cast<int32_t>(TraceTaskId::REMOTE_ANIMATION),
            "wms:async:ShowRemoteAnimation");
        WindowInnerManager::GetInstance().MinimizeAbility(weak, true);
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

WMError RemoteAnimation::NotifyAnimationClose(sptr<WindowTransitionInfo> srcInfo, const sptr<WindowNode>& srcNode,
    TransitionEvent event)
{
    sptr<RSWindowAnimationTarget> srcTarget = CreateWindowAnimationTarget(srcInfo, srcNode);
    if (srcTarget == nullptr) {
        return WMError::WM_ERROR_NO_MEM;
    }
    WLOGFI("RSWindowAnimation: notify animation close id:%{public}u!", srcNode->GetWindowId());
    srcNode->isPlayAnimationHide_ = true;
    srcNode->state_ = WindowNodeState::HIDE_ANIMATION_PLAYING;
    auto winRoot = windowRoot_.promote();
    if (winRoot != nullptr) {
        winRoot->RemoveWindowNode(srcNode->GetWindowId());
    }
    wptr<WindowNode> weak = srcNode;
    auto closeFunc = [weak, event]() {
        auto weakNode = weak.promote();
        if (weakNode == nullptr || weakNode->state_ != WindowNodeState::HIDE_ANIMATION_PLAYING) {
            WLOGFE("windowNode is nullptr or is not play hide animation!");
            return;
        }
        if (weakNode != nullptr && weakNode->abilityToken_ != nullptr) {
            if (event == TransitionEvent::CLOSE) {
                WLOGFI("close windowId: %{public}u, name:%{public}s",
                    weakNode->GetWindowId(), weakNode->GetWindowName().c_str());
                AAFwk::AbilityManagerClient::GetInstance()->CloseAbility(weakNode->abilityToken_);
            } else if (event == TransitionEvent::BACK) {
                WLOGFI("terminate windowId: %{public}u, name:%{public}s",
                    weakNode->GetWindowId(), weakNode->GetWindowName().c_str());
                AAFwk::Want resultWant;
                AAFwk::AbilityManagerClient::GetInstance()->TerminateAbility(weakNode->abilityToken_, -1, &resultWant);
            }
            FinishAsyncTraceArgs(HITRACE_TAG_WINDOW_MANAGER, static_cast<int32_t>(TraceTaskId::REMOTE_ANIMATION),
                "wms:async:ShowRemoteAnimation");
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

WMError RemoteAnimation::NotifyAnimationByHome()
{
    if (!CheckAnimationController()) {
        return WMError::WM_ERROR_NO_REMOTE_ANIMATION;
    }
    WLOGFI("RSWindowAnimation: notify animation by home");
    auto needMinimizeAppNodes = MinimizeApp::GetNeedMinimizeAppNodes();
    std::vector<sptr<RSWindowAnimationTarget>> animationTargets;
    for (auto& weakNode : needMinimizeAppNodes) {
        auto srcNode = weakNode.promote();
        sptr<WindowTransitionInfo> srcInfo = new(std::nothrow) WindowTransitionInfo();
        sptr<RSWindowAnimationTarget> srcTarget = CreateWindowAnimationTarget(srcInfo, srcNode, true);
        if (srcTarget == nullptr) {
            continue;
        }
        srcNode->isPlayAnimationHide_ = true;
        srcNode->state_ = WindowNodeState::HIDE_ANIMATION_PLAYING;
        auto winRoot = windowRoot_.promote();
        if (winRoot != nullptr) {
            winRoot->RemoveWindowNode(srcNode->GetWindowId());
        }
        animationTargets.emplace_back(srcTarget);
    }
    auto func = [needMinimizeAppNodes]() {
        WLOGFI("NotifyAnimationByHome in animation callback");
        for (auto& weakNode : needMinimizeAppNodes) {
            auto srcNode = weakNode.promote();
            if (srcNode == nullptr || srcNode->state_ != WindowNodeState::HIDE_ANIMATION_PLAYING) {
                WLOGFE("windowNode is nullptr or is not play hide animation!");
                continue;
            }
        }
        MinimizeApp::ExecuteMinimizeAll();
        FinishAsyncTraceArgs(HITRACE_TAG_WINDOW_MANAGER, static_cast<int32_t>(TraceTaskId::REMOTE_ANIMATION),
            "wms:async:ShowRemoteAnimation");
    };
    sptr<RSWindowAnimationFinishedCallback> finishedCallback = new(std::nothrow) RSWindowAnimationFinishedCallback(
        func);
    if (finishedCallback == nullptr) {
        WLOGFE("New RSIWindowAnimationFinishedCallback failed");
        return WMError::WM_ERROR_NO_MEM;
    }
    // need use OnMinimizeWindows with controller
    windowAnimationController_->OnMinimizeAllWindow(animationTargets, finishedCallback);
    return WMError::WM_OK;
}

void RemoteAnimation::NotifyAnimationTargetsUpdate(std::vector<uint32_t>& fullScreenWinIds,
    std::vector<uint32_t>& floatWinIds)
{
    auto handler = wmsTaskHandler_.lock();
    if (handler == nullptr) {
        WLOGFE("wmsTaskHandler_ is nullptr");
        return;
    }
    // need post task when visit windowRoot node map
    auto task = [fullScreenWinIds, floatWinIds]() {
        if (!CheckAnimationController()) {
            return;
        }
        auto winRoot = windowRoot_.promote();
        if (winRoot == nullptr) {
            WLOGFE("window root is nullptr");
            return;
        }
        std::vector<sptr<RSWindowAnimationTarget>> floatAnimationTargets;
        std::vector<sptr<RSWindowAnimationTarget>> fullScreenAnimationTargets;
        for (auto& id : fullScreenWinIds) {
            auto fullScreenNode = winRoot->GetWindowNode(id);
            sptr<RSWindowAnimationTarget> fullScreenTarget = CreateWindowAnimationTarget(nullptr, fullScreenNode, true);
            if (fullScreenTarget != nullptr) {
                fullScreenAnimationTargets.emplace_back(fullScreenTarget);
            }
        }
        for (auto& id : floatWinIds) {
            auto floatNode = winRoot->GetWindowNode(id);
            sptr<RSWindowAnimationTarget> floatTarget = CreateWindowAnimationTarget(nullptr, floatNode, true);
            if (floatTarget != nullptr) {
                floatAnimationTargets.emplace_back(floatTarget);
            }
        }
        // delete when need all fullscreen targets
        sptr<RSWindowAnimationTarget> fullScreenAnimationTarget = nullptr;
        if (!fullScreenAnimationTargets.empty()) {
            fullScreenAnimationTarget = fullScreenAnimationTargets[0];
        }
        windowAnimationController_->OnWindowAnimationTargetsUpdate(fullScreenAnimationTarget,
            floatAnimationTargets);
    };
    bool ret = handler->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    if (!ret) {
        WLOGFE("EventHandler PostTask Failed");
        task();
    }
}

WMError RemoteAnimation::NotifyAnimationScreenUnlock(std::function<void(void)> callback)
{
    if (!CheckAnimationController()) {
        return WMError::WM_ERROR_NO_REMOTE_ANIMATION;
    }
    WLOGFI("NotifyAnimationScreenUnlock");
    sptr<RSWindowAnimationFinishedCallback> finishedCallback = new(std::nothrow) RSWindowAnimationFinishedCallback(
        callback);
    if (finishedCallback == nullptr) {
        WLOGFE("New RSIWindowAnimationFinishedCallback failed");
        return WMError::WM_ERROR_NO_MEM;
    }
    windowAnimationController_->OnScreenUnlock(finishedCallback);
    return WMError::WM_OK;
}

sptr<RSWindowAnimationTarget> RemoteAnimation::CreateWindowAnimationTarget(sptr<WindowTransitionInfo> info,
    const sptr<WindowNode>& windowNode, bool isUpdate)
{
    if (windowNode == nullptr) {
        WLOGFW("Failed to create window animation target, window node is null!");
        return nullptr;
    }

    sptr<RSWindowAnimationTarget> windowAnimationTarget = new(std::nothrow) RSWindowAnimationTarget();
    if (windowAnimationTarget == nullptr) {
        WLOGFE("New RSWindowAnimationTarget failed");
        return nullptr;
    }

    if (isUpdate) {
        windowAnimationTarget->bundleName_ = windowNode->abilityInfo_.bundleName_;
        windowAnimationTarget->abilityName_ = windowNode->abilityInfo_.abilityName_;
    } else if (info) { // use for back, minimize, close
        windowAnimationTarget->bundleName_ = info->GetBundleName();
        windowAnimationTarget->abilityName_ = info->GetAbilityName();
    }

    windowAnimationTarget->missionId_ = windowNode->abilityInfo_.missionId_;
    windowAnimationTarget->windowId_ = windowNode->GetWindowId();
    windowAnimationTarget->displayId_ = windowNode->GetDisplayId();
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
    auto rect = windowNode->GetWindowRect();
    // 0, 1, 2, 3: convert bounds to RectF
    auto boundsRect = RectF(rect.posX_, rect.posY_, rect.width_, rect.height_);
    windowAnimationTarget->windowBounds_ = RRect(boundsRect, stagingProperties.GetCornerRadius());
    return windowAnimationTarget;
}
} // Rosen
} // OHOS
