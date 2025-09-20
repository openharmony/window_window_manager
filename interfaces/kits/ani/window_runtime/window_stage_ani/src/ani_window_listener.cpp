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
    ani_status ret = env_->GlobalReference_Delete(aniCallBack_);
    TLOGI(WmsLogTag::DEFAULT, "[ANI]~AniWindowListener ret:%{public}d", static_cast<int32_t>(ret));
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
            nullptr, thisListener->aniCallBack_, AniWindowUtils::CreateAniSize(eng, rect.width_, rect.height_));
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
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto task = [self = weakRef_, eng = env_, displayId, tints] {
        auto thisListener = self.promote();
        if (thisListener == nullptr || eng == nullptr || thisListener->aniCallback_ == nullptr) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI]this listener, eng or callback is nullptr");
            return;
        }
        AniWindowUtils::CallAniFunctionVoid(eng, "L@ohos/window/window;", "runSystemBarTintChangeCallback",
            nullptr, thisListener->aniCallback_, AniWindowUtils::CreateAniSystemBarTintState(eng, displayId, tints));
    };
    if (!eventHandler_) {
        TLOGE(WmsLogTag::DEFAULT, "get main event handler failed!");
        return;
    }
    eventHandler_->PostTask(task, __func__, 0, AppExecFwk::EventQueue::Priority::HIGH);
}

void AniWindowListener::OnAvoidAreaChanged(const AvoidArea avoidArea, AvoidAreaType type,
    const sptr<OccupiedAreaChangeInfo>& info)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto task = [self = weakRef_, eng = env_, avoidArea, type] {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "AniWindowListener::OnAvoidAreaChanged");
        auto thisListener = self.promote();
        if (thisListener == nullptr || eng == nullptr || thisListener->aniCallback_ == nullptr) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI]this listener, eng or callback is nullptr");
            return;
        }
        auto nativeAvoidArea = AniWindowUtils::CreateAniAvoidArea(eng, avoidArea, type);
        AniWindowUtils::CallAniFunctionVoid(eng, "L@ohos/window/window;", "runAvoidAreaChangeCallback",
            nullptr, thisListener->aniCallback_, nativeAvoidArea, static_cast<ani_int>(type));
    };
    if (!eventHandler_) {
        TLOGE(WmsLogTag::DEFAULT, "get main event handler failed!");
        return;
    }
    eventHandler_->PostTask(task, __func__, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void AniWindowListener::OnSystemDensityChanged(float density)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto task = [self = weakRef_, density, eng = env_] {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "AniWindowListener::OnSystemDensityChanged");
        auto thisListener = self.promote();
        if (thisListener == nullptr || eng == nullptr || thisListener->aniCallback_ == nullptr) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI]this listener, eng or callback is nullptr");
            return;
        }
        AniWindowUtils::CallAniFunctionVoid(eng, "L@ohos/window/window;", "runSystemDensityChangeCallback",
            nullptr, thisListener->aniCallback_, static_cast<ani_float>(density));
    };
    if (!eventHandler_) {
        TLOGE(WmsLogTag::DEFAULT, "get main event handler failed!");
        return;
    }
    eventHandler_->PostTask(task, __func__, 0, AppExecFwk::EventQueue::Priority::HIGH);
}

void AniWindowListener::OnDisplayIdChanged(DisplayId displayId)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto task = [self = weakRef_, displayId, eng = env_] {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "AniWindowListener::OnDisplayIdChanged");
        auto thisListener = self.promote();
        if (thisListener == nullptr || eng == nullptr || thisListener->aniCallback_ == nullptr) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI]this listener, eng or callback is nullptr");
            return;
        }
        AniWindowUtils::CallAniFunctionVoid(eng, "L@ohos/window/window;", "runDisplayIdChangeCallback",
            nullptr, thisListener->aniCallback_, static_cast<ani_long>(displayId));
    };
    if (!eventHandler_) {
        TLOGE(WmsLogTag::DEFAULT, "get main event handler failed!");
        return;
    }
    eventHandler_->PostTask(task, __func__, 0, AppExecFwk::EventQueue::Priority::HIGH);
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
            nullptr, thisListener->aniCallBack_, static_cast<ani_int>(eventType));
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
            nullptr, thisListener->aniCallBack_, static_cast<ani_int>(eventType));
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
}

void AniWindowListener::OnScreenshot()
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto task = [self = weakRef_, eng = env_] {
        auto thisListener = self.promote();
        if (thisListener == nullptr || eng == nullptr || thisListener->aniCallback_ == nullptr) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI]this listener, eng or callback is nullptr");
            return;
        }
        AniWindowUtils::CallAniFunctionVoid(eng, "L@ohos/window/window;", "runWindowListenerVoidArgCallback",
            nullptr, thisListener->aniCallback_);
    };
    if (!eventHandler_) {
        TLOGE(WmsLogTag::DEFAULT, "get main event handler failed!");
        return;
    }
    eventHandler_->PostTask(task, __func__, 0, AppExecFwk::EventQueue::Priority::HIGH);
}

void AniWindowListener::OnDialogTargetTouch() const
{
}

void AniWindowListener::OnDialogDeathRecipient() const
{
}

void AniWindowListener::OnGestureNavigationEnabledUpdate(bool enable)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto task = [self = weakRef_, eng = env_, enable] {
        auto thisListener = self.promote();
        if (thisListener == nullptr || eng == nullptr || thisListener->aniCallback_ == nullptr) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI]this listener, eng or callback is nullptr");
            return;
        }
        AniWindowUtils::CallAniFunctionVoid(eng, "L@ohos/window/window;", "runWindowListenerBooleanArgCallback",
            nullptr, thisListener->aniCallback_, ani_boolean(enable));
    };
    if (!eventHandler_) {
        TLOGE(WmsLogTag::DEFAULT, "get main event handler failed!");
        return;
    }
    eventHandler_->PostTask(task, __func__, 0, AppExecFwk::EventQueue::Priority::HIGH);
}

void AniWindowListener::OnWaterMarkFlagUpdate(bool showWaterMark)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto task = [self = weakRef_, eng = env_, showWaterMark] {
        auto thisListener = self.promote();
        if (thisListener == nullptr || eng == nullptr || thisListener->aniCallback_ == nullptr) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI]this listener, eng or callback is nullptr");
            return;
        }
        AniWindowUtils::CallAniFunctionVoid(eng, "L@ohos/window/window;", "runWindowListenerBooleanArgCallback",
            nullptr, thisListener->aniCallback_, ani_boolean(showWaterMark));
    };
    if (!eventHandler_) {
        TLOGE(WmsLogTag::DEFAULT, "get main event handler failed!");
        return;
    }
    eventHandler_->PostTask(task, __func__, 0, AppExecFwk::EventQueue::Priority::HIGH);
}

void AniWindowListener::OnWindowNoInteractionCallback()
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
            nullptr, thisListener->aniCallBack_, static_cast<ani_int>(windowstatus));
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
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto task = [self = weakRef_, eng = env_, isVisible] {
        auto thisListener = self.promote();
        if (thisListener == nullptr || eng == nullptr || thisListener->aniCallback_ == nullptr) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI]this listener, eng or callback is nullptr");
            return;
        }
        AniWindowUtils::CallAniFunctionVoid(eng, "L@ohos/window/window;", "runWindowListenerBooleanArgCallback",
            nullptr, thisListener->aniCallback_, ani_boolean(isVisible));
    };
    if (!eventHandler_) {
        TLOGE(WmsLogTag::DEFAULT, "get main event handler failed!");
        return;
    }
    eventHandler_->PostTask(task, __func__, 0, AppExecFwk::EventQueue::Priority::HIGH);
}

void AniWindowListener::OnWindowHighlightChange(bool isHighlight)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto task = [self = weakRef_, eng = env_, isHighlight] {
        auto thisListener = self.promote();
        if (thisListener == nullptr || eng == nullptr || thisListener->aniCallback_ == nullptr) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI]this listener, eng or callback is nullptr");
            return;
        }
        AniWindowUtils::CallAniFunctionVoid(eng, "L@ohos/window/window;", "runWindowListenerBooleanArgCallback",
            nullptr, thisListener->aniCallback_, ani_boolean(isHighlight));
    };
    if (!eventHandler_) {
        TLOGE(WmsLogTag::DEFAULT, "get main event handler failed!");
        return;
    }
    eventHandler_->PostTask(task, __func__, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
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

void AniWindowListener::OnRotationChange(const RotationChangeInfo& rotationChangeInfo,
    RotationChangeResult& rotationChangeResult)
{
    TLOGI(WmsLogTag::WMS_ROTATION, "[ANI]");
    auto task = [self = weakRef_, eng = env_, rotationChangeInfo, &rotationChangeResult] {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "AniWindowListener::OnRotationChange");
        auto thisListener = self.promote();
        if (thisListener == nullptr || eng == nullptr || thisListener->aniCallBack_ == nullptr) {
            TLOGE(WmsLogTag::WMS_ROTATION, "[ANI]this listener, eng or callback is nullptr");
            return;
        }
        ani_object rotationInfoObj = AniWindowUtils::CreateAniRotationChangeInfo(eng, rotationChangeInfo);
        if (rotationInfoObj == nullptr) {
            TLOGE(WmsLogTag::WMS_ROTATION, "failed to create ani object");
            return;
        }
        ani_ref rotationChangeResultObj;
        AniWindowUtils::CallAniFunctionRef(eng, rotationChangeResultObj, thisListener->aniCallBack_, 1,
            rotationInfoObj);
        if (rotationChangeResultObj != nullptr) {
            AniWindowUtils::ParseRotationChangeResult(eng, static_cast<ani_object>(rotationChangeResultObj),
                rotationChangeResult);
        }
    };
    if (!eventHandler_ ||
        (eventHandler_->GetEventRunner() && eventHandler_->GetEventRunner()->IsCurrentRunnerThread())) {
        TLOGW(WmsLogTag::WMS_ROTATION, "get main event handler failed or current is already main thread!");
        return task();
    }
    eventHandler_->PostSyncTask(task, __func__, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}
} // namespace Rosen
} // namespace OHOS