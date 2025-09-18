/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "ani_window_listener.h"

#include <hitrace_meter.h>

#include "ani_window_utils.h"
#include "event_handler.h"
#include "event_runner.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;

AniWindowListener::~AniWindowListener()
{
    ani_status ret = ANI_OK;
    if (env_ != nullptr && aniCallback_ != nullptr) {
        ret = env_->GlobalReference_Delete(aniCallback_);
    }
    TLOGI(WmsLogTag::DEFAULT, "[ANI]~AniWindowListener ret: %{public}u", ret);
}

void AniWindowListener::OnLastStrongRef(const void *)
{
}

void AniWindowListener::SetMainEventHandler()
{
    auto mainRunner = AppExecFwk::EventRunner::GetMainEventRunner();
    if (mainRunner == nullptr) {
        return;
    }
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(mainRunner);
}

void AniWindowListener::SetTimeout(int64_t timeout)
{
    noInteractionTimeout_ = timeout;
}

int64_t AniWindowListener::GetTimeout() const
{
    return noInteractionTimeout_;
}

void AniWindowListener::OnSizeChange(Rect rect, WindowSizeChangeReason reason,
    const std::shared_ptr<RSTransaction>& rsTransaction)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI] wh[%{public}u, %{public}u], reason = %{public}u", rect.width_,
        rect.height_, reason);
    if (currRect_.width_ == rect.width_ && currRect_.height_ == rect.height_ &&
        reason != WindowSizeChangeReason::DRAG_END) {
        TLOGI(WmsLogTag::DEFAULT, "[ANI]no need to change size");
        return;
    }
    currRect_ = rect;

    auto task = [self = weakRef_, rect, eng = env_] () {
        auto thisListener = self.promote();
        if (thisListener == nullptr || eng == nullptr) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI]this listener or eng is nullptr");
            return;
        }
        AniWindowUtils::CallAniFunctionVoid(eng, "L@ohos/window/window;", "runWindowSizeCallBack",
            nullptr, thisListener->aniCallback_, AniWindowUtils::CreateAniSize(eng, rect.width_, rect.height_));
    };
    if (!eventHandler_) {
        TLOGE(WmsLogTag::DEFAULT, "get main event handler failed!");
        return;
    }
    eventHandler_->PostTask(task, "wms:AniWindowListener::SizeChangeCallBack", 0,
        AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void AniWindowListener::OnModeChange(WindowMode mode, bool hasDeco)
{
}

void AniWindowListener::OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints)
{
}

void AniWindowListener::OnAvoidAreaChanged(const AvoidArea avoidArea, AvoidAreaType type,
    const sptr<OccupiedAreaChangeInfo>& info)
{
}

void AniWindowListener::LifeCycleCallBack(LifeCycleEventType eventType)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]LifeCycleCallBack, envent type: %{public}u", eventType);
    auto task = [self = weakRef_, eventType, eng = env_] () {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "AniWindowListener::LifeCycleCallBack");
        auto thisListener = self.promote();
        if (thisListener == nullptr || eng == nullptr) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI]this listener or eng is nullptr");
            return;
        }
        AniWindowUtils::CallAniFunctionVoid(eng, "L@ohos/window/window;", "runWindowEventCallBack",
            nullptr, thisListener->aniCallback_, static_cast<ani_int>(eventType));
    };
    if (!eventHandler_) {
        TLOGE(WmsLogTag::DEFAULT, "get main event handler failed!");
        return;
    }
    eventHandler_->PostTask(task, "wms:AniWindowListener::LifeCycleCallBack", 0,
        AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void AniWindowListener::WindowStageLifecycleCallback(WindowStageLifeCycleEventType eventType)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]windowStageLifecycleCallback, envent type: %{public}u", eventType);
    auto task = [self = weakRef_, eventType, eng = env_] () {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "AniWindowListener::windowStageLifecycleCallback");
        auto thisListener = self.promote();
        if (thisListener == nullptr || eng == nullptr) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI]this listener or eng is nullptr");
            return;
        }
        AniWindowUtils::CallAniFunctionVoid(eng, "L@ohos/window/window;", "runWindowStageLifecycleEventCallBack",
            nullptr, thisListener->aniCallback_, static_cast<ani_int>(eventType));
    };
    if (!eventHandler_) {
        TLOGE(WmsLogTag::DEFAULT, "get main event handler failed!");
        return;
    }
    eventHandler_->PostTask(task, "wms:AniWindowListener::windowStageLifecycleCallback", 0,
        AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void AniWindowListener::AfterForeground()
{
    if (state_ == WindowState::STATE_INITIAL || state_ == WindowState::STATE_HIDDEN) {
        LifeCycleCallBack(LifeCycleEventType::FOREGROUND);
        state_ = WindowState::STATE_SHOWN;
    } else {
        TLOGD(WmsLogTag::DEFAULT, "[ANI]window is already shown");
    }
}

void AniWindowListener::AfterBackground()
{
    if (state_ == WindowState::STATE_INITIAL || state_ == WindowState::STATE_SHOWN) {
        LifeCycleCallBack(LifeCycleEventType::BACKGROUND);
        state_ = WindowState::STATE_HIDDEN;
    } else {
        TLOGD(WmsLogTag::DEFAULT, "[ANI]window is already hide");
    }
}

void AniWindowListener::AfterFocused()
{
    LifeCycleCallBack(LifeCycleEventType::ACTIVE);
}

void AniWindowListener::AfterUnfocused()
{
    LifeCycleCallBack(LifeCycleEventType::INACTIVE);
}

void AniWindowListener::AfterResumed()
{
    if (caseType_ == CaseType::CASE_STAGE) {
        LifeCycleCallBack(LifeCycleEventType::RESUMED);
    }
}

void AniWindowListener::AfterPaused()
{
    if (caseType_ == CaseType::CASE_STAGE) {
        LifeCycleCallBack(LifeCycleEventType::PAUSED);
    }
}

void AniWindowListener::AfterDestroyed()
{
    if (caseType_ == CaseType::CASE_WINDOW) {
        LifeCycleCallBack(LifeCycleEventType::DESTROYED);
    }
}

void AniWindowListener::OnSizeChange(const sptr<OccupiedAreaChangeInfo>& info,
    const std::shared_ptr<RSTransaction>& rsTransaction)
{
}

void AniWindowListener::OnTouchOutside() const
{
    TLOGI(WmsLogTag::WMS_EVENT, "[ANI]in");
    auto task = [self = weakRef_, eng = env_] () {
        auto thisListener = self.promote();
        if (thisListener == nullptr || eng == nullptr || thisListener->aniCallback_ == nullptr) {
            TLOGE(WmsLogTag::WMS_EVENT, "[ANI]thisListener, eng or callback is nullptr!");
            return;
        }
        AniWindowUtils::CallAniFunctionVoid(eng, "@ohos.window.window", "runWindowTouchOutCallback",
            nullptr, thisListener->aniCallback_);
    };
    if (!eventHandler_) {
        TLOGE(WmsLogTag::WMS_EVENT, "get main event handler failed!");
        return;
    }
    eventHandler_->PostTask(task, "wms:AniWindowListener::TouchOutsideCallback", 0,
        AppExecFwk::EventQueue::Priority::HIGH);
}

void AniWindowListener::OnDialogTargetTouch() const
{
    TLOGI(WmsLogTag::WMS_EVENT, "[ANI]in");
    auto task = [self = weakRef_, eng = env_] () {
        auto thisListener = self.promote();
        if (thisListener == nullptr || eng == nullptr || thisListener->aniCallback_ == nullptr) {
            TLOGE(WmsLogTag::WMS_EVENT, "[ANI]thisListener, eng or callback is nullptr!");
            return;
        }
        AniWindowUtils::CallAniFunctionVoid(eng, "@ohos.window.window", "runWindowDialogTargetCallback",
            nullptr, thisListener->aniCallback_);
    };
    if (!eventHandler_) {
        TLOGE(WmsLogTag::WMS_EVENT, "get main event handler failed!");
        return;
    }
    eventHandler_->PostTask(task, "wms:AniWindowListener::DialogTargetTouchCallback", 0,
        AppExecFwk::EventQueue::Priority::HIGH);
}

void AniWindowListener::OnWindowNoInteractionCallback()
{
    TLOGI(WmsLogTag::WMS_EVENT, "[ANI]in");
    auto task = [self = weakRef_, vm = vm_] () {
        auto thisListener = self.promote();
        if (thisListener == nullptr || vm == nullptr || thisListener->aniCallback_ == nullptr) {
            TLOGE(WmsLogTag::WMS_EVENT, "[ANI]thisListener, vm or callback is nullptr!");
            return;
        }
        ani_env* env = nullptr;
        ani_status ret = vm->GetEnv(ANI_VERSION_1, &env);
        if (ret != ANI_OK || env == nullptr) {
            TLOGE(WmsLogTag::WMS_EVENT, "[ANI]Get env failed, ret: %{public}u", ret);
            return;
        }
        AniWindowUtils::CallAniFunctionVoid(env, "@ohos.window.window", "runWindowNoInteractionCallback",
            "C{std.core.Object}:", thisListener->aniCallback_);
    };
    if (!eventHandler_) {
        TLOGE(WmsLogTag::WMS_EVENT, "Get main event handler failed!");
        return;
    }
    eventHandler_->PostTask(task, "wms:AniWindowListener::WindowNoInteractionCallback", 0,
        AppExecFwk::EventQueue::Priority::HIGH);
}

void AniWindowListener::OnScreenshot()
{
}

void AniWindowListener::OnDialogDeathRecipient() const
{
}

void AniWindowListener::OnGestureNavigationEnabledUpdate(bool enable)
{
}

void AniWindowListener::OnWaterMarkFlagUpdate(bool showWaterMark)
{
}

void AniWindowListener::OnWindowStatusChange(WindowStatus windowstatus)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI] windowstatus: %{public}u", windowstatus);
    auto task = [self = weakRef_, windowstatus, eng = env_] () {
        auto thisListener = self.promote();
        if (thisListener == nullptr || eng == nullptr) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI]this listener or eng is nullptr");
            return;
        }
        AniWindowUtils::CallAniFunctionVoid(eng, "L@ohos/window/window;", "runWindowStatusCallBack",
            nullptr, thisListener->aniCallback_, static_cast<ani_int>(windowstatus));
    };
    if (!eventHandler_) {
        TLOGE(WmsLogTag::DEFAULT, "get main event handler failed!");
        return;
    }
    eventHandler_->PostTask(task, "wms:AniWindowListener::StatusChangeCallBack", 0,
        AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void AniWindowListener::OnWindowVisibilityChangedCallback(const bool isVisible)
{
}

void AniWindowListener::OnWindowTitleButtonRectChanged(const TitleButtonRect& titleButtonRect)
{
}

void AniWindowListener::OnRectChange(Rect rect, WindowSizeChangeReason reason)
{
}

void AniWindowListener::OnSubWindowClose(bool& terminateCloseProcess)
{
}

void AniWindowListener::OnMainWindowClose(bool& terminateCloseProcess)
{
}

void AniWindowListener::AfterLifecycleForeground()
{
    if (caseType_ == CaseType::CASE_STAGE) {
        if (state_ == WindowState::STATE_INITIAL || state_ == WindowState::STATE_HIDDEN) {
            WindowStageLifecycleCallback(WindowStageLifeCycleEventType::FOREGROUND);
            state_ = WindowState::STATE_SHOWN;
        }
    }
}

void AniWindowListener::AfterLifecycleBackground()
{
    if (caseType_ == CaseType::CASE_STAGE) {
        if (state_ == WindowState::STATE_INITIAL || state_ == WindowState::STATE_SHOWN) {
            WindowStageLifecycleCallback(WindowStageLifeCycleEventType::BACKGROUND);
            state_ = WindowState::STATE_HIDDEN;
        }
    }
}

void AniWindowListener::AfterLifecycleResumed()
{
    if (caseType_ == CaseType::CASE_STAGE) {
        WindowStageLifecycleCallback(WindowStageLifeCycleEventType::RESUMED);
    }
}

void AniWindowListener::AfterLifecyclePaused()
{
    if (caseType_ == CaseType::CASE_STAGE) {
        WindowStageLifecycleCallback(WindowStageLifeCycleEventType::PAUSED);
    }
}
} // namespace Rosen
} // namespace OHOS