/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include <hitrace_meter.h>

#include "event_handler.h"
#include "event_runner.h"
#include "js_window_listener.h"
#include "js_runtime_utils.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsListener"};
}

JsWindowListener::~JsWindowListener()
{
    WLOGI("[NAPI]~JsWindowListener");
}

void JsWindowListener::SetMainEventHandler()
{
    auto mainRunner = AppExecFwk::EventRunner::GetMainEventRunner();
    if (mainRunner == nullptr) {
        return;
    }
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(mainRunner);
}

void JsWindowListener::CallJsMethod(const char* methodName, napi_value const * argv, size_t argc)
{
    WLOGFD("[NAPI]CallJsMethod methodName = %{public}s", methodName);
    if (env_ == nullptr || jsCallBack_ == nullptr) {
        WLOGFE("[NAPI]env_ nullptr or jsCallBack_ is nullptr");
        return;
    }
    napi_value method = jsCallBack_->GetNapiValue();
    if (method == nullptr) {
        WLOGFE("[NAPI]Failed to get method callback from object");
        return;
    }
    napi_value result = nullptr;
    napi_get_undefined(env_, &result);
    napi_call_function(env_, result, method, argc, argv, nullptr);
}

void JsWindowListener::OnSizeChange(Rect rect, WindowSizeChangeReason reason,
    const std::shared_ptr<RSTransaction>& rsTransaction)
{
    WLOGI("[NAPI]OnSizeChange, wh[%{public}u, %{public}u], reason = %{public}u", rect.width_, rect.height_, reason);
    if (currentWidth_ == rect.width_ && currentHeight_ == rect.height_ && reason != WindowSizeChangeReason::DRAG_END) {
        WLOGFD("[NAPI]no need to change size");
        return;
    }
    // js callback should run in js thread
    auto jsCallback = [self = weakRef_, rect, eng = env_] () {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsWindowListener::OnSizeChange");
        auto thisListener = self.promote();
        if (thisListener == nullptr || eng == nullptr) {
            WLOGFE("[NAPI]this listener or eng is nullptr");
            return;
        }
        napi_handle_scope scope = nullptr;
        napi_open_handle_scope(eng, &scope);
        napi_value objValue = nullptr;
        napi_create_object(eng, &objValue);
        if (objValue == nullptr) {
            WLOGFE("Failed to convert rect to jsObject");
            return;
        }
        napi_set_named_property(eng, objValue, "width", CreateJsValue(eng, rect.width_));
        napi_set_named_property(eng, objValue, "height", CreateJsValue(eng, rect.height_));
        napi_value argv[] = {objValue};
        thisListener->CallJsMethod(WINDOW_SIZE_CHANGE_CB.c_str(), argv, ArraySize(argv));
        napi_close_handle_scope(eng, scope);
    };
    if (reason == WindowSizeChangeReason::ROTATION) {
        jsCallback();
    } else {
        if (!eventHandler_) {
            WLOGFE("get main event handler failed!");
            return;
        }
        eventHandler_->PostTask(jsCallback, "wms:JsWindowListener::OnSizeChange", 0,
            AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
    currentWidth_ = rect.width_;
    currentHeight_ = rect.height_;
}

void JsWindowListener::OnModeChange(WindowMode mode, bool hasDeco)
{
    WLOGI("[NAPI]OnModeChange %{public}u", mode);
}

void JsWindowListener::OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints)
{
    WLOGFD("[NAPI]OnSystemBarPropertyChange");
    // js callback should run in js thread
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback> (
        [self = weakRef_, displayId, tints, eng = env_] (napi_env env,
            NapiAsyncTask &task, int32_t status) {
            
            auto thisListener = self.promote();
            if (thisListener == nullptr || eng == nullptr) {
                WLOGFE("[NAPI]this listener or eng is nullptr");
                return;
            }
            napi_value propertyValue = nullptr;
            napi_create_object(eng, &propertyValue);
            if (propertyValue == nullptr) {
                WLOGFE("[NAPI]Failed to convert prop to jsObject");
                return;
            }
            napi_set_named_property(env, propertyValue, "displayId",
                CreateJsValue(eng, static_cast<uint32_t>(displayId)));
            napi_set_named_property(env, propertyValue, "regionTint",
                CreateJsSystemBarRegionTintArrayObject(eng, tints));
            napi_value argv[] = {propertyValue};
            thisListener->CallJsMethod(SYSTEM_BAR_TINT_CHANGE_CB.c_str(), argv, ArraySize(argv));
        }
    );

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsWindowListener::OnSystemBarPropertyChange",
        env_, std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsWindowListener::OnAvoidAreaChanged(const AvoidArea avoidArea, AvoidAreaType type)
{
    WLOGFD("[NAPI]OnAvoidAreaChanged");
    // js callback should run in js thread
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback> (
        [self = weakRef_, avoidArea, type, eng = env_] (napi_env env,
            NapiAsyncTask &task, int32_t status) {
            auto thisListener = self.promote();
            if (thisListener == nullptr || eng == nullptr) {
                WLOGFE("[NAPI]this listener or eng is nullptr");
                return;
            }
            napi_value avoidAreaValue = ConvertAvoidAreaToJsValue(env, avoidArea, type);
            if (avoidAreaValue == nullptr) {
                return;
            }
            if (thisListener->isDeprecatedInterface_) {
                napi_value argv[] = { avoidAreaValue };
                thisListener->CallJsMethod(SYSTEM_AVOID_AREA_CHANGE_CB.c_str(), argv, ArraySize(argv));
            } else {
                napi_value objValue = nullptr;
                napi_create_object(env, &objValue);
                if (objValue == nullptr) {
                    WLOGFE("Failed to get object");
                    return;
                }
                napi_set_named_property(env, objValue, "type", CreateJsValue(env, static_cast<uint32_t>(type)));
                napi_set_named_property(env, objValue, "area", avoidAreaValue);
                napi_value argv[] = { objValue };
                thisListener->CallJsMethod(AVOID_AREA_CHANGE_CB.c_str(), argv, ArraySize(argv));
            }
        }
    );

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsWindowListener::OnAvoidAreaChanged",
        env_, std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsWindowListener::LifeCycleCallBack(LifeCycleEventType eventType)
{
    WLOGI("[NAPI]LifeCycleCallBack, envent type: %{public}u", eventType);
    auto task = [self = weakRef_, eventType, eng = env_] () {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsWindowListener::LifeCycleCallBack");
        auto thisListener = self.promote();
        if (thisListener == nullptr || eng == nullptr) {
            WLOGFE("[NAPI]this listener or eng is nullptr");
            return;
        }
        napi_handle_scope scope = nullptr;
        napi_open_handle_scope(eng, &scope);
        napi_value argv[] = {CreateJsValue(eng, static_cast<uint32_t>(eventType))};
        thisListener->CallJsMethod(LIFECYCLE_EVENT_CB.c_str(), argv, ArraySize(argv));
        napi_close_handle_scope(eng, scope);
    };
    if (!eventHandler_) {
        WLOGFE("get main event handler failed!");
        return;
    }
    eventHandler_->PostTask(task, "wms:JsWindowListener::LifeCycleCallBack", 0,
        AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void JsWindowListener::AfterForeground()
{
    if (state_ == WindowState::STATE_INITIAL || state_ == WindowState::STATE_HIDDEN) {
        LifeCycleCallBack(LifeCycleEventType::FOREGROUND);
        state_ = WindowState::STATE_SHOWN;
    } else {
        WLOGFD("[NAPI]window is already shown");
    }
}

void JsWindowListener::AfterBackground()
{
    if (state_ == WindowState::STATE_INITIAL || state_ == WindowState::STATE_SHOWN) {
        LifeCycleCallBack(LifeCycleEventType::BACKGROUND);
        state_ = WindowState::STATE_HIDDEN;
    } else {
        WLOGFD("[NAPI]window is already hide");
    }
}

void JsWindowListener::AfterFocused()
{
    LifeCycleCallBack(LifeCycleEventType::ACTIVE);
}

void JsWindowListener::AfterUnfocused()
{
    LifeCycleCallBack(LifeCycleEventType::INACTIVE);
}

void JsWindowListener::AfterResumed()
{
    LifeCycleCallBack(LifeCycleEventType::RESUMED);
}

void JsWindowListener::AfterPaused()
{
    LifeCycleCallBack(LifeCycleEventType::PAUSED);
}

void JsWindowListener::AfterDestroyed()
{
    LifeCycleCallBack(LifeCycleEventType::DESTROYED);
}

void JsWindowListener::OnSizeChange(const sptr<OccupiedAreaChangeInfo>& info,
    const std::shared_ptr<RSTransaction>& rsTransaction)
{
    WLOGI("[NAPI]OccupiedAreaChangeInfo, type: %{public}u, " \
        "input rect: [%{public}d, %{public}d, %{public}u, %{public}u]", static_cast<uint32_t>(info->type_),
        info->rect_.posX_, info->rect_.posY_, info->rect_.width_, info->rect_.height_);
    // js callback should run in js thread
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback> (
        [self = weakRef_, info, eng = env_] (napi_env env,
            NapiAsyncTask &task, int32_t status) {
            auto thisListener = self.promote();
            if (thisListener == nullptr || eng == nullptr) {
                WLOGFE("[NAPI]this listener or eng is nullptr");
                return;
            }
            napi_value argv[] = {CreateJsValue(eng, info->rect_.height_)};
            thisListener->CallJsMethod(KEYBOARD_HEIGHT_CHANGE_CB.c_str(), argv, ArraySize(argv));
        }
    );

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsWindowListener::OnSizeChange",
        env_, std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsWindowListener::OnTouchOutside() const
{
    WLOGI("CALLED");
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback> (
        [self = weakRef_] (napi_env env, NapiAsyncTask &task, int32_t status) {
            auto thisListener = self.promote();
            if (thisListener == nullptr) {
                WLOGFE("[NAPI]this listener is nullptr");
                return;
            }
            thisListener->CallJsMethod(TOUCH_OUTSIDE_CB.c_str(), nullptr, 0);
        }
    );

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsWindowListener::OnOutsidePressed",
        env_, std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsWindowListener::OnScreenshot()
{
    WLOGI("CALLED");
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback> (
        [self = wptr<JsWindowListener>(this)] (napi_env env, NapiAsyncTask &task, int32_t status) {
            auto thisListener = self.promote();
            if (thisListener == nullptr) {
                WLOGFE("[NAPI]this listener is nullptr");
                return;
            }
            thisListener->CallJsMethod(SCREENSHOT_EVENT_CB.c_str(), nullptr, 0);
        }
    );

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsWindowListener::OnScreenshot",
        env_, std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsWindowListener::OnDialogTargetTouch() const
{
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback> (
        [self = weakRef_] (napi_env env, NapiAsyncTask &task, int32_t status) {
            auto thisListener = self.promote();
            if (thisListener == nullptr) {
                WLOGFE("[NAPI]this listener is nullptr");
                return;
            }
            thisListener->CallJsMethod(DIALOG_TARGET_TOUCH_CB.c_str(), nullptr, 0);
        }
    );

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsWindowListener::OnDialogTargetTouch",
        env_, std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsWindowListener::OnDialogDeathRecipient() const
{
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback> (
        [self = weakRef_] (napi_env env, NapiAsyncTask &task, int32_t status) {
            auto thisListener = self.promote();
            if (thisListener == nullptr) {
                WLOGFE("[NAPI]this listener is nullptr");
                return;
            }
            thisListener->CallJsMethod(DIALOG_DEATH_RECIPIENT_CB.c_str(), nullptr, 0);
        }
    );

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsWindowListener::OnDialogDeathRecipient",
        env_, std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsWindowListener::OnGestureNavigationEnabledUpdate(bool enable)
{
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback> (
        [self = weakRef_, enable, eng = env_] (napi_env env, NapiAsyncTask &task, int32_t status) {
            auto thisListener = self.promote();
            if (thisListener == nullptr || eng == nullptr) {
                WLOGFE("[NAPI]this listener or eng is nullptr");
                return;
            }
            napi_value argv[] = {CreateJsValue(eng, enable)};
            thisListener->CallJsMethod(GESTURE_NAVIGATION_ENABLED_CHANGE_CB.c_str(), argv, ArraySize(argv));
        }
    );

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsWindowListener::OnGestureNavigationEnabledUpdate",
        env_, std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsWindowListener::OnWaterMarkFlagUpdate(bool showWaterMark)
{
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback> (
        [self = weakRef_, showWaterMark, eng = env_] (napi_env env, NapiAsyncTask &task, int32_t status) {
            auto thisListener = self.promote();
            if (thisListener == nullptr || eng == nullptr) {
                WLOGFE("[NAPI]this listener or eng is nullptr");
                return;
            }
            napi_value argv[] = {CreateJsValue(eng, showWaterMark)};
            thisListener->CallJsMethod(WATER_MARK_FLAG_CHANGE_CB.c_str(), argv, ArraySize(argv));
        }
    );

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsWindowListener::OnWaterMarkFlagUpdate",
        env_, std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsWindowListener::OnWindowStatusChange(WindowStatus windowstatus)
{
    WLOGFD("[NAPI]OnWindowStatusChange");
    // js callback should run in js thread
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback> (
        [self = weakRef_, windowstatus, eng = env_] (napi_env env,
            NapiAsyncTask &task, int32_t status) {
            auto thisListener = self.promote();
            if (thisListener == nullptr || eng == nullptr) {
                WLOGFE("[NAPI]this listener or eng is nullptr");
                return;
            }
            napi_value propertyValue = nullptr;
            napi_create_object(eng, &propertyValue);
            if (propertyValue == nullptr) {
                WLOGFE("[NAPI]Failed to convert prop to jsObject");
                return;
            }
            napi_set_named_property(env, propertyValue, "status",
                CreateJsValue(eng, static_cast<uint32_t>(windowstatus)));
            napi_value argv[] = {propertyValue};
            thisListener->CallJsMethod(WINDOW_STATUS_CHANGE_CB.c_str(), argv, ArraySize(argv));
        }
    );

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsWindowListener::OnWindowStatusChange",
        env_, std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsWindowListener::OnWindowVisibilityChangedCallback(const bool isVisible)
{
    std::unique_ptr<NapiAsyncTask::CompleteCallback> complete = std::make_unique<NapiAsyncTask::CompleteCallback>(
        [self = weakRef_, isVisible, eng = env_] (napi_env env, NapiAsyncTask &task, int32_t status) {
            auto thisListener = self.promote();
            if (thisListener == nullptr || eng == nullptr) {
                WLOGFE("This listener or eng is nullptr");
                return;
            }
            napi_value argv[] = { CreateJsValue(eng, isVisible) };
            thisListener->CallJsMethod(WINDOW_VISIBILITY_CHANGE_CB.c_str(), argv, ArraySize(argv));
        }
    );

    napi_ref callback = nullptr;
    std::unique_ptr<NapiAsyncTask::ExecuteCallback> execute = nullptr;
    NapiAsyncTask::Schedule("JsWindowListener::OnWindowVisibilityChangedCallback", env_,
        std::make_unique<NapiAsyncTask>(callback, std::move(execute), std::move(complete)));
}
} // namespace Rosen
} // namespace OHOS