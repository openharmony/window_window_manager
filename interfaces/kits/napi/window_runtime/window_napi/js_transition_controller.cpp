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

#include "js_transition_controller.h"
#include "js_runtime_utils.h"
#include "window.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "window_option.h"
namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsTransitionController"};
}

static int jsTransCxtCtorCnt = 0;
static int jsTransCxtDtorCnt = 0;
static int jsTransCtrlCtorCnt = 0;
static int jsTransCtrlDtorCnt = 0;

JsTransitionContext::JsTransitionContext(sptr<Window> window, bool isShownTransContext)
    : windowToken_(window), isShownTransContext_(isShownTransContext)
{
    WLOGFI("[NAPI] JsTransitionContext constructorCnt: %{public}d", ++jsTransCxtCtorCnt);
}

JsTransitionContext::~JsTransitionContext()
{
    WLOGFI("[NAPI] ~JsTransitionContext deConstructorCnt: %{public}d", ++jsTransCxtDtorCnt);
}

void JsTransitionContext::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFI("[NAPI]JsTransitionContext::Finalizer");
    std::unique_ptr<JsTransitionContext>(static_cast<JsTransitionContext*>(data));
}

NativeValue* JsTransitionContext::CompleteTransition(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("[NAPI]CompleteTransition");
    JsTransitionContext* me = CheckParamsAndGetThis<JsTransitionContext>(engine, info);
    return (me != nullptr) ? me->OnCompleteTransition(*engine, *info) : nullptr;
}

NativeValue* JsTransitionContext::OnCompleteTransition(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("[NAPI]OnCompleteTransition");
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool transitionCompleted = false;
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(engine, info.argv[0], transitionCompleted)) {
        WLOGFE("[NAPI]Failed to convert parameter to transitionCompleted");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    AsyncTask::CompleteCallback complete =
        [weakWindow = windowToken_, errCode, transitionCompleted, isShownTransContext = isShownTransContext_](
            NativeEngine& engine, AsyncTask& task, int32_t status) {
            HandleScope handleScope(engine);
            auto window = weakWindow.promote();
            if (window == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            WMError ret = WMError::WM_OK;
            if (!isShownTransContext) {
                window->UpdateSurfaceNodeAfterCustomAnimation(false); // remove from rs tree after animation
                if (!transitionCompleted) {
                    ret = window->Show(); // hide success
                }
            } else {
                if (!transitionCompleted) {
                    ret = window->Hide(); // hide success
                }
            }
            if (ret != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window destroy failed"));
                return;
            }
            WLOGFI("[NAPI]Window [%{public}u, %{public}s] CompleteTransition %{public}d end",
                window->GetWindowId(), window->GetWindowName().c_str(), transitionCompleted);
            task.Resolve(engine, engine.CreateUndefined());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsTransitionContext::OnCompleteTransition",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

static NativeValue* CreateJsTransitionContextObject(NativeEngine& engine, std::shared_ptr<NativeReference> jsWin,
    sptr<Window> window, bool isShownTransContext)
{
    WLOGFI("[NAPI]CreateJsTransitionContextObject");
    NativeValue *objValue = engine.CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Failed to get object");
        return nullptr;
    }
    std::unique_ptr<JsTransitionContext> jsTransContext = std::make_unique<JsTransitionContext>(window,
        isShownTransContext);
    object->SetNativePointer(jsTransContext.release(), JsTransitionContext::Finalizer, nullptr);
    if (jsWin != nullptr && jsWin->Get() != nullptr) {
        object->SetProperty("toWindow", jsWin->Get());
    } else {
        WLOGFE("[NAPI]jsWin is nullptr");
        return nullptr;
    }

    const char *moduleName = "JsTransitionContext";
    BindNativeFunction(engine, *object, "completeTransition", moduleName, JsTransitionContext::CompleteTransition);
    return objValue;
}

JsTransitionController::JsTransitionController(NativeEngine& engine, std::shared_ptr<NativeReference> jsWin,
    sptr<Window> window)
    : engine_(engine), jsWin_(jsWin), windowToken_(window), weakRef_(wptr<JsTransitionController> (this))
{
    WLOGFI("[NAPI] JsTransitionController constructorCnt: %{public}d", ++jsTransCtrlCtorCnt);
}

JsTransitionController::~JsTransitionController()
{
    WLOGFI("[NAPI] ~JsTransitionController deConstructorCnt: %{public}d", ++jsTransCtrlDtorCnt);
}

void JsTransitionController::AnimationForShown()
{
    WLOGFI("[NAPI]AnimationForShown");
    std::unique_ptr<AsyncTask::CompleteCallback> complete = std::make_unique<AsyncTask::CompleteCallback> (
        [self = weakRef_](NativeEngine&, AsyncTask&, int32_t) {
            auto thisController = self.promote();
            if (thisController == nullptr) {
                WLOGFE("this transition Controller is null!");
                return;
            }
            HandleScope handleScope(thisController->engine_);
            auto jsWin = thisController->jsWin_.lock();
            auto window = thisController->windowToken_.promote();
            if (jsWin == nullptr || window == nullptr) {
                WLOGFE("native window or jsWindow is null!");
                return;
            }
            auto state = window->GetWindowState();
            if (state != WindowState::STATE_SHOWN) {
                WLOGFE("animation shown configuration for state %{public}u not support!", static_cast<uint32_t>(state));
                return;
            }
            NativeValue* jsTransContextObj = CreateJsTransitionContextObject(
                thisController->engine_, jsWin, window, true);
            if (jsTransContextObj == nullptr) {
                return;
            }
            NativeValue *argv[] = { jsTransContextObj };
            thisController->CallJsMethod("animationForShown", argv, ArraySize(argv));
            // add to rs tree before animation
            window->UpdateSurfaceNodeAfterCustomAnimation(true);
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsTransitionController::AnimationForShown", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsTransitionController::AnimationForHidden()
{
    WLOGFI("[NAPI]AnimationForHidden");
    std::unique_ptr<AsyncTask::CompleteCallback> complete = std::make_unique<AsyncTask::CompleteCallback> (
        [self = weakRef_](NativeEngine&, AsyncTask&, int32_t) {
            auto thisController = self.promote();
            if (thisController == nullptr) {
                WLOGFE("this transition Controller is null!");
                return;
            }
            HandleScope handleScope(thisController->engine_);
            auto jsWin = thisController->jsWin_.lock();
            auto window = thisController->windowToken_.promote();
            if (jsWin == nullptr || window == nullptr) {
                WLOGFE("native window or jsWindow is null!");
                return;
            }
            auto state = window->GetWindowState();
            if (state != WindowState::STATE_HIDDEN) {
                WLOGFE("animation hidden configuration for state %{public}u not support!",
                    static_cast<uint32_t>(state));
                return;
            }
            NativeValue* jsTransContextObj = CreateJsTransitionContextObject(
                thisController->engine_, jsWin, window, false);
            if (jsTransContextObj == nullptr) {
                return;
            }
            NativeValue *argv[] = { jsTransContextObj };
            thisController->CallJsMethod("animationForHidden", argv, ArraySize(argv));
        }
    );
    NativeReference* callback = nullptr;
    std::unique_ptr<AsyncTask::ExecuteCallback> execute = nullptr;
    AsyncTask::Schedule("JsTransitionController::AnimationForHidden", engine_,
        std::make_unique<AsyncTask>(callback, std::move(execute), std::move(complete)));
}

void JsTransitionController::CallJsMethod(const std::string& methodName, NativeValue* const* argv, size_t argc)
{
    WLOGFI("Call js function:%{public}s.", methodName.c_str());
    auto self = jsTransControllerObj_.lock();
    if (self == nullptr) {
        WLOGFE("JsController is null!");
        return;
    }
    auto jsControllerValue = self->Get();
    auto jsControllerObj = ConvertNativeValueTo<NativeObject>(jsControllerValue);
    if (jsControllerObj == nullptr) {
        WLOGFE("JsControllerObj is null!");
        return;
    }
    auto method = jsControllerObj->GetProperty(methodName.c_str());
    if (method == nullptr || method->TypeOf() == NATIVE_UNDEFINED) {
        WLOGFE("Failed to get %{public}s from object", methodName.c_str());
        return;
    }
    engine_.CallFunction(jsControllerValue, method, argv, argc);
}

void JsTransitionController::SetJsController(std::shared_ptr<NativeReference> jsVal)
{
    jsTransControllerObj_ = jsVal;
}
}
}