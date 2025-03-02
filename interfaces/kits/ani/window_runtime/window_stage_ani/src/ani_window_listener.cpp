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

#include "event_handler.h"
#include "event_runner.h"
#include "js_runtime_utils.h"
#include "window_manager_hilog.h"
#include "ani_common_utils.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "AniListener"};
}

AniWindowListener::~AniWindowListener()
{
    WLOGI("[NAPI]~AniWindowListener");
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

ani_status AniWindowListener::CallAniMethodVoid(ani_object object, const char* cls,
    const char* method, const char* signature, ...)
{
    ani_class aniClass;
    ani_status ret = env_->FindClass(cls, &aniClass);
    if (ret != ANI_OK) {
        return ret;
    }
    ani_method aniMethod;
    ret = env_->Class_FindMethod(aniClass, method, signature, &aniMethod);
    if (ret != ANI_OK) {
        return ret;
    }
    va_list args;
    va_start(args, signature);
    ret = env_->Object_CallMethod_Void(object, aniMethod, args);
    va_end(args);
    return ret;
}

void AniWindowListener::OnSizeChange(Rect rect, WindowSizeChangeReason reason,
    const std::shared_ptr<RSTransaction>& rsTransaction)
{
    WLOGI("[NAPI]OnSizeChange, wh[%{public}u, %{public}u], reason = %{public}u", rect.width_, rect.height_, reason);
    if (currRect_.width_ == rect.width_ && currRect_.height_ == rect.height_ &&
        reason != WindowSizeChangeReason::DRAG_END) {
        WLOGFD("[NAPI]no need to change size");
        return;
    }
    CallAniMethodVoid(static_cast<ani_object>(aniCallBack_), "L@ohos/window/Callback", "invoke",
        "L@ohos/window/window/Rect;:V", AniCommonUtils::CreateAniRect(env_, rect));
    currRect_ = rect;
}

void AniWindowListener::OnModeChange(WindowMode mode, bool hasDeco)
{
}

void AniWindowListener::OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints)
{
}

void AniWindowListener::OnAvoidAreaChanged(const AvoidArea avoidArea, AvoidAreaType type)
{
}

void AniWindowListener::LifeCycleCallBack(LifeCycleEventType eventType)
{
    WLOGI("[NAPI]LifeCycleCallBack, envent type: %{public}u", eventType);
    CallAniMethodVoid(static_cast<ani_object>(aniCallBack_), "L@ohos/window/Callback", "invoke",
        "L@ohos/window/window/WindowEventType;:V", static_cast<ani_int>(eventType));
}

void AniWindowListener::AfterForeground()
{
    if (state_ == WindowState::STATE_INITIAL || state_ == WindowState::STATE_HIDDEN) {
        LifeCycleCallBack(LifeCycleEventType::FOREGROUND);
        state_ = WindowState::STATE_SHOWN;
    } else {
        WLOGFD("[NAPI]window is already shown");
    }
}

void AniWindowListener::AfterBackground()
{
    if (state_ == WindowState::STATE_INITIAL || state_ == WindowState::STATE_SHOWN) {
        LifeCycleCallBack(LifeCycleEventType::BACKGROUND);
        state_ = WindowState::STATE_HIDDEN;
    } else {
        WLOGFD("[NAPI]window is already hide");
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
}

void AniWindowListener::OnDialogTargetTouch() const
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

void AniWindowListener::OnWindowNoInteractionCallback()
{
}

void AniWindowListener::OnWindowStatusChange(WindowStatus windowstatus)
{
    CallAniMethodVoid(static_cast<ani_object>(aniCallBack_), "L@ohos/window/Callback", "invoke",
        "L@ohos/window/window/WindowStatusType;:V", static_cast<ani_int>(windowstatus));
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
} // namespace Rosen
} // namespace OHOS