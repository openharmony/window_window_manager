/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "js_extension_window.h"

#include "js_extension_window_utils.h"
#include "js_runtime_utils.h"
#include "js_window_utils.h"
#include "js_window.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "extension_window.h"
#include "ui_content.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsExtensionWindow"};
constexpr Rect g_emptyRect = {0, 0, 0, 0};
} // namespace

JsExtensionWindow::JsExtensionWindow(
    const std::shared_ptr<Rosen::ExtensionWindow> extensionWindow,
    int32_t hostWindowId)
    : extensionWindow_(extensionWindow), hostWindowId_(hostWindowId),
    extensionRegisterManager_(std::make_unique<JsExtensionWindowRegisterManager>()) {
}

JsExtensionWindow::JsExtensionWindow(const std::shared_ptr<Rosen::ExtensionWindow> extensionWindow,
    sptr<AAFwk::SessionInfo> sessionInfo)
    : extensionWindow_(extensionWindow), sessionInfo_(sessionInfo),
    extensionRegisterManager_(std::make_unique<JsExtensionWindowRegisterManager>()) {
}

JsExtensionWindow::~JsExtensionWindow() {}

napi_value JsExtensionWindow::CreateJsExtensionWindow(napi_env env, sptr<Rosen::Window> window, int32_t hostWindowId)
{
    WLOGI("JsExtensionWindow CreateJsExtensionWindow");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);

    if (env == nullptr || window == nullptr) {
        WLOGFE("JsExtensionWindow env or window is nullptr");
        return nullptr;
    }

    std::shared_ptr<ExtensionWindow> extensionWindow = std::make_shared<ExtensionWindowImpl>(window);
    std::unique_ptr<JsExtensionWindow> jsExtensionWindow =
        std::make_unique<JsExtensionWindow>(extensionWindow, hostWindowId);
    napi_wrap(env, objValue, jsExtensionWindow.release(), JsExtensionWindow::Finalizer, nullptr, nullptr);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_GETTER("properties", JsExtensionWindow::GetProperties)
    };
    NAPI_CALL(env, napi_define_properties(env, objValue, sizeof(desc) / sizeof(desc[0]), desc));

    const char *moduleName = "JsExtensionWindow";
    BindNativeFunction(env, objValue, "getWindowAvoidArea", moduleName, JsExtensionWindow::GetWindowAvoidArea);
    BindNativeFunction(env, objValue, "on", moduleName, JsExtensionWindow::RegisterExtensionWindowCallback);
    BindNativeFunction(env, objValue, "off", moduleName, JsExtensionWindow::UnRegisterExtensionWindowCallback);
    BindNativeFunction(env, objValue, "hideNonSecureWindows", moduleName, JsExtensionWindow::HideNonSecureWindows);
    BindNativeFunction(env, objValue, "createSubWindowWithOptions", moduleName,
        JsExtensionWindow::CreateSubWindowWithOptions);

    return objValue;
}

napi_value JsExtensionWindow::CreateJsExtensionWindowObject(napi_env env, sptr<Rosen::Window> window,
    sptr<AAFwk::SessionInfo> sessionInfo)
{
    WLOGI("JsExtensionWindow CreateJsExtensionWindow");
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);

    if (env == nullptr || window == nullptr) {
        WLOGFE("JsExtensionWindow env or window is nullptr");
        return nullptr;
    }

    std::shared_ptr<ExtensionWindow> extensionWindow = std::make_shared<ExtensionWindowImpl>(window);
    std::unique_ptr<JsExtensionWindow> jsExtensionWindow = std::make_unique<JsExtensionWindow>(extensionWindow,
        sessionInfo);
    napi_wrap(env, objValue, jsExtensionWindow.release(), JsExtensionWindow::Finalizer, nullptr, nullptr);

    const char *moduleName = "JsExtensionWindow";
    BindNativeFunction(env, objValue, "on", moduleName, JsExtensionWindow::RegisterExtensionWindowCallback);
    BindNativeFunction(env, objValue, "off", moduleName, JsExtensionWindow::UnRegisterExtensionWindowCallback);
    BindNativeFunction(env, objValue, "showWindow", moduleName, JsExtensionWindow::ShowWindow);
    BindNativeFunction(env, objValue, "destroyWindow", moduleName, JsExtensionWindow::DestroyWindow);
    BindNativeFunction(env, objValue, "loadContent", moduleName, JsExtensionWindow::LoadContent);
    BindNativeFunction(env, objValue, "loadContentByName", moduleName, JsExtensionWindow::LoadContentByName);
    BindNativeFunction(env, objValue, "setUIContent", moduleName, JsExtensionWindow::SetUIContent);
    BindNativeFunction(env, objValue, "isWindowShowing", moduleName, JsExtensionWindow::IsWindowShowingSync);
    BindNativeFunction(env, objValue, "setWindowBackgroundColor", moduleName,
        JsExtensionWindow::SetWindowBackgroundColorSync);

    return objValue;
}

void JsExtensionWindow::Finalizer(napi_env env, void* data, void* hint)
{
    WLOGI("Finalizer is called");
    std::unique_ptr<JsExtensionWindow>(static_cast<JsExtensionWindow*>(data));
}

napi_value JsExtensionWindow::GetWindowAvoidArea(napi_env env, napi_callback_info info)
{
    WLOGI("GetWindowAvoidArea is called");
    JsExtensionWindow* me = CheckParamsAndGetThis<JsExtensionWindow>(env, info);
    return (me != nullptr) ? me->OnGetWindowAvoidArea(env, info) : nullptr;
}

napi_value JsExtensionWindow::RegisterExtensionWindowCallback(napi_env env, napi_callback_info info)
{
    WLOGI("RegisterExtensionWindowCallback is called");
    JsExtensionWindow* me = CheckParamsAndGetThis<JsExtensionWindow>(env, info);
    return (me != nullptr) ? me->OnRegisterExtensionWindowCallback(env, info) : nullptr;
}

napi_value JsExtensionWindow::UnRegisterExtensionWindowCallback(napi_env env, napi_callback_info info)
{
    WLOGI("UnRegisterExtensionWindowCallback is called");
    JsExtensionWindow* me = CheckParamsAndGetThis<JsExtensionWindow>(env, info);
    return (me != nullptr) ? me->OnUnRegisterExtensionWindowCallback(env, info) : nullptr;
}

napi_value JsExtensionWindow::HideNonSecureWindows(napi_env env, napi_callback_info info)
{
    WLOGI("HideNonSecureWindows is called");
    JsExtensionWindow* me = CheckParamsAndGetThis<JsExtensionWindow>(env, info);
    return (me != nullptr) ? me->OnHideNonSecureWindows(env, info) : nullptr;
}

napi_value JsExtensionWindow::CreateSubWindowWithOptions(napi_env env, napi_callback_info info)
{
    WLOGI("CreateSubWindowWithOptions is called");
    JsExtensionWindow* me = CheckParamsAndGetThis<JsExtensionWindow>(env, info);
    return (me != nullptr) ? me->OnCreateSubWindowWithOptions(env, info) : nullptr;
}

napi_value JsExtensionWindow::LoadContent(napi_env env, napi_callback_info info)
{
    WLOGI("LoadContent is called");
    JsExtensionWindow* me = CheckParamsAndGetThis<JsExtensionWindow>(env, info);
    return (me != nullptr) ? me->OnLoadContent(env, info, false) : nullptr;
}

napi_value JsExtensionWindow::LoadContentByName(napi_env env, napi_callback_info info)
{
    WLOGI("LoadContentByName is called");
    JsExtensionWindow* me = CheckParamsAndGetThis<JsExtensionWindow>(env, info);
    return (me != nullptr) ? me->OnLoadContent(env, info, true) : nullptr;
}

napi_value JsExtensionWindow::ShowWindow(napi_env env, napi_callback_info info)
{
    WLOGI("ShowWindow is called");
    JsExtensionWindow* me = CheckParamsAndGetThis<JsExtensionWindow>(env, info);
    return (me != nullptr) ? me->OnShowWindow(env, info) : nullptr;
}

napi_value JsExtensionWindow::IsWindowShowingSync(napi_env env, napi_callback_info info)
{
    WLOGI("IsShowing is called");
    JsExtensionWindow* me = CheckParamsAndGetThis<JsExtensionWindow>(env, info);
    return (me != nullptr) ? me->OnIsWindowShowingSync(env, info) : nullptr;
}

napi_value JsExtensionWindow::SetUIContent(napi_env env, napi_callback_info info)
{
    WLOGI("LoadContent is called");
    JsExtensionWindow* me = CheckParamsAndGetThis<JsExtensionWindow>(env, info);
    return (me != nullptr) ? me->OnSetUIContent(env, info) : nullptr;
}

napi_value JsExtensionWindow::DestroyWindow(napi_env env, napi_callback_info info)
{
    WLOGI("Destroy is called");
    JsExtensionWindow* me = CheckParamsAndGetThis<JsExtensionWindow>(env, info);
    return (me != nullptr) ? me->OnDestroyWindow(env, info) : nullptr;
}

napi_value JsExtensionWindow::SetWindowBackgroundColorSync(napi_env env, napi_callback_info info)
{
    WLOGI("SetBackgroundColor is called");
    JsExtensionWindow* me = CheckParamsAndGetThis<JsExtensionWindow>(env, info);
    return (me != nullptr) ? me->OnSetWindowBackgroundColorSync(env, info) : nullptr;
}

napi_valuetype GetType(napi_env env, napi_value value)
{
    napi_valuetype res = napi_undefined;
    napi_typeof(env, value, &res);
    return res;
}

static void LoadContentTask(std::shared_ptr<NativeReference> contentStorage, std::string contextUrl,
    const std::shared_ptr<Rosen::ExtensionWindow> win, napi_env env, NapiAsyncTask& task,
    sptr<IRemoteObject> parentToken, bool isLoadedByName)
{
    napi_value nativeStorage = (contentStorage == nullptr) ? nullptr : contentStorage->GetNapiValue();
    sptr<Window> windowImpl = win->GetWindow();
    WMError ret;
    if (isLoadedByName) {
        ret = windowImpl->SetUIContentByName(contextUrl, env, nativeStorage);
    } else {
        ret = windowImpl->NapiSetUIContent(contextUrl, env, nativeStorage, false, parentToken);
    }
    if (ret == WMError::WM_OK) {
        task.Resolve(env, NapiGetUndefined(env));
    } else {
        task.Reject(env, CreateJsError(env, static_cast<int32_t>(ret), "Window load content failed"));
    }
    WLOGI("Window [%{public}u, %{public}s] load content end, ret = %{public}d",
        windowImpl->GetWindowId(), windowImpl->GetWindowName().c_str(), ret);
    return;
}

napi_value JsExtensionWindow::OnSetWindowBackgroundColorSync(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    std::string color;
    if (errCode == WmErrorCode::WM_OK && !ConvertFromJsValue(env, argv[0], color)) {
        WLOGFE("Failed to convert parameter to background color");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    sptr<Window> windowImpl = extensionWindow_->GetWindow();
    if (windowImpl == nullptr) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowImpl->SetBackgroundColor(color));
    if (ret == WmErrorCode::WM_OK) {
        WLOGI("Window [%{public}u, %{public}s] set background color end",
            windowImpl->GetWindowId(), windowImpl->GetWindowName().c_str());
        return NapiGetUndefined(env);
    } else {
        return NapiThrowError(env, ret);
    }
}

napi_value JsExtensionWindow::OnDestroyWindow(napi_env env, napi_callback_info info)
{
    NapiAsyncTask::CompleteCallback complete =
        [this, extwin = extensionWindow_](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (extwin == nullptr) {
                WLOGFE("extensionWindow is null");
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            sptr<Window> windowImpl = extwin->GetWindow();
            if (windowImpl == nullptr) {
                WLOGFE("window is nullptr or get invalid param");
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowImpl->Destroy());
            WLOGI("Window [%{public}u, %{public}s] destroy end, ret = %{public}d",
                windowImpl->GetWindowId(), windowImpl->GetWindowName().c_str(), ret);
            if (ret != WmErrorCode::WM_OK) {
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(ret),
                    "Window destroy failed"));
                return;
            }
            windowImpl = nullptr; // ensure window dtor when finalizer invalid
            task.Resolve(env, NapiGetUndefined(env));
        };
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value lastParam = (argc == 0) ? nullptr :
        ((argv[0] != nullptr && GetType(env, argv[0]) == napi_function) ? argv[0] : nullptr);
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsExtensionWindow::OnDestroyWindow",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsExtensionWindow::OnIsWindowShowingSync(napi_env env, napi_callback_info info)
{
    sptr<Window> windowImpl = extensionWindow_->GetWindow();
    if (windowImpl == nullptr) {
        WLOGFE("window is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    bool state = (windowImpl->GetWindowState() == WindowState::STATE_SHOWN);
    WLOGI("Window [%{public}u, %{public}s] get show state end, state = %{public}u",
        windowImpl->GetWindowId(), windowImpl->GetWindowName().c_str(), state);
    return CreateJsValue(env, state);
}

napi_value JsExtensionWindow::OnShowWindow(napi_env env, napi_callback_info info)
{
    NapiAsyncTask::CompleteCallback complete =
        [extwin = extensionWindow_](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (extwin == nullptr) {
                WLOGFE("extensionWindow is null");
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            sptr<Window> windowImpl = extwin->GetWindow();
            if (windowImpl == nullptr) {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                WLOGFE("window is nullptr or get invalid param");
                return;
            }
            WMError ret = windowImpl->Show(0, false);
            WLOGI("Window [%{public}u, %{public}s] show with ret = %{public}d",
                windowImpl->GetWindowId(), windowImpl->GetWindowName().c_str(), ret);
            if (ret == WMError::WM_OK) {
                task.Resolve(env, NapiGetUndefined(env));
            } else {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(WM_JS_TO_ERROR_CODE_MAP.at(ret)), "Window show failed"));
            }
            WLOGI("Window [%{public}u, %{public}s] show end, ret = %{public}d",
                windowImpl->GetWindowId(), windowImpl->GetWindowName().c_str(), ret);
        };
    napi_value result = nullptr;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value lastParam = (argc == 0) ? nullptr :
        ((argv[0] != nullptr && GetType(env, argv[0]) == napi_function) ? argv[0] : nullptr);
    NapiAsyncTask::Schedule("JsExtensionWindow::OnShowWindow",
        env, CreateAsyncTaskWithLastParam(env, lastParam, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsExtensionWindow::OnSetUIContent(napi_env env, napi_callback_info info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 2 maximum param num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    std::string contextUrl;
    if (errCode == WmErrorCode::WM_OK && !ConvertFromJsValue(env, argv[0], contextUrl)) {
        WLOGFE("Failed to convert parameter to context url");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    napi_value callBack = nullptr;
    if (argc >= 2) { // 2 param num
        callBack = argv[1];
    }
    std::shared_ptr<NativeReference> contentStorage = nullptr;
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    sptr<IRemoteObject> parentToken = sessionInfo_->parentToken;
    NapiAsyncTask::CompleteCallback complete =
        [extwin = extensionWindow_, contentStorage, contextUrl, parentToken]
        (napi_env env, NapiAsyncTask& task, int32_t status) {
            if (extwin == nullptr) {
                WLOGFE("Window is nullptr");
                task.Reject(env,
                    CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            LoadContentTask(contentStorage, contextUrl, extwin, env, task, parentToken, false);
        };
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsExtensionWindow::OnSetUIContent",
        env, CreateAsyncTaskWithLastParam(env, callBack, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsExtensionWindow::OnLoadContent(napi_env env, napi_callback_info info, bool isLoadedByName)
{
    WLOGI("OnLoadContent is called");
    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    std::string contextUrl;
    if (!ConvertFromJsValue(env, argv[0], contextUrl)) {
        WLOGI("Failed to convert parameter to context url");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    napi_value storage = nullptr;
    napi_value callBack = nullptr;
    napi_value value1 = argv[1];
    napi_value value2 = argv[2];
    if (GetType(env, value1) == napi_function) {
        callBack = value1;
    } else if (GetType(env, value1) == napi_object) {
        storage = value1;
    }
    if (GetType(env, value2) == napi_function) {
        callBack = value2;
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        WLOGI("Window is null or get invalid param");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }

    std::shared_ptr<NativeReference> contentStorage = nullptr;
    if (storage != nullptr) {
        napi_ref result = nullptr;
        napi_create_reference(env, storage, 1, &result);
        contentStorage = std::shared_ptr<NativeReference>(reinterpret_cast<NativeReference*>(result));
    }
    
    sptr<IRemoteObject> parentToken = sessionInfo_->parentToken;
    NapiAsyncTask::CompleteCallback complete =
        [extwin = extensionWindow_, contentStorage, contextUrl, parentToken, isLoadedByName](
            napi_env env, NapiAsyncTask& task, int32_t status) {
            if (extwin == nullptr) {
                WLOGFE("Window is nullptr or get invalid param");
                task.Reject(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            LoadContentTask(contentStorage, contextUrl, extwin, env, task, parentToken, isLoadedByName);
        };
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsExtensionWindow::OnLoadContent",
        env, CreateAsyncTaskWithLastParam(env, callBack, nullptr, std::move(complete), &result));
    return result;
}

napi_value JsExtensionWindow::OnGetWindowAvoidArea(napi_env env, napi_callback_info info)
{
    WLOGI("OnGetWindowAvoidArea is called");

    WmErrorCode errCode = WmErrorCode::WM_OK;
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) { // 1: params num
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    AvoidAreaType avoidAreaType = AvoidAreaType::TYPE_SYSTEM;
    napi_value nativeMode = argv[0];
    if (nativeMode == nullptr) {
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    } else {
        uint32_t resultValue = 0;
        napi_get_value_uint32(env, nativeMode, &resultValue);
        avoidAreaType = static_cast<AvoidAreaType>(resultValue);
        errCode = ((avoidAreaType > AvoidAreaType::TYPE_NAVIGATION_INDICATOR) ||
                   (avoidAreaType < AvoidAreaType::TYPE_SYSTEM)) ?
                  WmErrorCode::WM_ERROR_INVALID_PARAM : WmErrorCode::WM_OK;
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    if (extensionWindow_ == nullptr) {
        WLOGFE("extensionWindow_ is nullptr");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STAGE_ABNORMALLY)));
        return CreateJsValue(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_STAGE_ABNORMALLY));
    }
    // getAvoidRect by avoidAreaType
    AvoidArea avoidArea;
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(extensionWindow_->GetAvoidAreaByType(avoidAreaType, avoidArea));
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("OnGetAvoidAreaByType failed");
        avoidArea.topRect_ = g_emptyRect;
        avoidArea.leftRect_ = g_emptyRect;
        avoidArea.rightRect_ = g_emptyRect;
        avoidArea.bottomRect_ = g_emptyRect;
    }
    napi_value avoidAreaObj = ConvertAvoidAreaToJsValue(env, avoidArea, avoidAreaType);
    if (avoidAreaObj != nullptr) {
        WLOGI("avoidAreaObj is finish");
        return avoidAreaObj;
    } else {
        WLOGFE("avoidAreaObj is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
}

napi_value JsExtensionWindow::OnRegisterExtensionWindowCallback(napi_env env, napi_callback_info info)
{
    sptr<Window> windowImpl = extensionWindow_->GetWindow();
    if (windowImpl == nullptr) {
        WLOGFE("WindowImpl is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 2) { // 2: params num
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    std::string cbType;
    if (!ConvertFromJsValue(env, argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    napi_value value = argv[1];
    if (!NapiIsCallable(env, value)) {
        WLOGFE("Callback(info->argv[1]) is not callable");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    WmErrorCode ret = extensionRegisterManager_->RegisterListener(windowImpl, cbType, CaseType::CASE_WINDOW,
        env, value);
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("Callback(info->argv[1]) is not callable");
        return NapiThrowError(env, ret);
    }
    WLOGI("Register end, window [%{public}u, %{public}s], type = %{public}s",
          windowImpl->GetWindowId(), windowImpl->GetWindowName().c_str(), cbType.c_str());
    return NapiGetUndefined(env);
}

napi_value JsExtensionWindow::OnUnRegisterExtensionWindowCallback(napi_env env, napi_callback_info info)
{
    sptr<Window> windowImpl = extensionWindow_->GetWindow();
    if (windowImpl == nullptr) {
        WLOGFE("windowImpl is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    std::string cbType;
    if (!ConvertFromJsValue(env, argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    napi_value value = nullptr;
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (argc == 1) {
        ret = extensionRegisterManager_->UnregisterListener(windowImpl, cbType, CaseType::CASE_WINDOW, env, value);
    } else {
        value = argv[1];
        if (value == nullptr || !NapiIsCallable(env, value)) {
            ret = extensionRegisterManager_->UnregisterListener(windowImpl, cbType, CaseType::CASE_WINDOW,
                env, nullptr);
        } else {
            ret = extensionRegisterManager_->UnregisterListener(windowImpl, cbType, CaseType::CASE_WINDOW, env, value);
        }
    }

    if (ret != WmErrorCode::WM_OK) {
        return NapiThrowError(env, ret);
    }
    WLOGI("UnRegister end, window [%{public}u, %{public}s], type = %{public}s",
          windowImpl->GetWindowId(), windowImpl->GetWindowName().c_str(), cbType.c_str());
    return NapiGetUndefined(env);
}

napi_value JsExtensionWindow::OnHideNonSecureWindows(napi_env env, napi_callback_info info)
{
    if (extensionWindow_ == nullptr) {
        WLOGFE("extensionWindow_ is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    sptr<Window> windowImpl = extensionWindow_->GetWindow();
    if (windowImpl == nullptr) {
        WLOGFE("windowImpl is nullptr");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc < 1) {
        WLOGFE("Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    bool shouldHide = false;
    if (!ConvertFromJsValue(env, argv[0], shouldHide)) {
        WLOGFE("Failed to convert parameter to bool");
        return NapiThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }

    WmErrorCode ret = WmErrorCode::WM_OK;
    ret = WM_JS_TO_ERROR_CODE_MAP.at(extensionWindow_->HideNonSecureWindows(shouldHide));
    if (ret != WmErrorCode::WM_OK) {
        return NapiThrowError(env, ret);
    }
    WLOGI("OnHideNonSecureWindows end, window [%{public}u, %{public}s], shouldHide:%{public}u",
          windowImpl->GetWindowId(), windowImpl->GetWindowName().c_str(), shouldHide);
    return NapiGetUndefined(env);
}

napi_value JsExtensionWindow::GetProperties(napi_env env, napi_callback_info info)
{
    WLOGI("GetProperties is called");
    napi_value jsThis;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsThis, nullptr));

    JsExtensionWindow* jsExtensionWindow = nullptr;
    NAPI_CALL(env, napi_unwrap(env, jsThis, reinterpret_cast<void**>(&jsExtensionWindow)));
    if (!jsExtensionWindow || !jsExtensionWindow->extensionWindow_) {
        WLOGFE("window is nullptr");
        return nullptr;
    }
    sptr<Rosen::Window> window = jsExtensionWindow->extensionWindow_->GetWindow();
    return CreateJsExtensionWindowPropertiesObject(env, window);
}

napi_value JsExtensionWindow::OnCreateSubWindowWithOptions(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    std::string windowName;
    if (!ConvertFromJsValue(env, argv[0], windowName)) {
        WLOGFE("[NAPI]Failed to convert parameter to windowName");
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    WindowOption option;
    std::string title;
    if (!ParseJsValue(argv[1], env, "title", title)) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    bool decorEnabled;
    if (!ParseJsValue(argv[1], env, "decorEnabled", decorEnabled)) {
        napi_throw(env, CreateJsError(env, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return NapiGetUndefined(env);
    }
    option = SetOption(option, title, decorEnabled, hostWindowId_);
    NapiAsyncTask::CompleteCallback complete =
        [weak = extensionWindow_, windowName, option](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (weak == nullptr) {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY), "extensionWindow_ is null"));
            }
            sptr<Rosen::WindowOption> windowOption = new WindowOption(option);
            JsExtensionWindow::SetWindowOption(windowOption);
            auto extWindow = weak->GetWindow();
            if (extWindow == nullptr) {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY), "extensio's window is null"));
            }
            auto window = Window::Create(windowName, windowOption, extWindow->GetContext());
            if (window == nullptr) {
                task.Reject(env, CreateJsError(env,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY), "create sub window failed"));
                return;
            }
            task.Resolve(env, CreateJsWindowObject(env, window));
            WLOGI("[NAPI]Create sub window %{public}s end", windowName.c_str());
        };
    napi_value callback = (argv[2] != nullptr && GetType(env, argv[2]) == napi_function) ? argv[2] : nullptr;
    napi_value result = nullptr;
    NapiAsyncTask::Schedule("JsExtensionWindow::OnCreateSubWindowWithOptions",
        env, CreateAsyncTaskWithLastParam(env, callback, nullptr, std::move(complete), &result));
    return result;
}

void JsExtensionWindow::SetWindowOption(sptr<Rosen::WindowOption> windowOption)
{
    windowOption->SetWindowType(Rosen::WindowType::WINDOW_TYPE_APP_EXTENSION_SUB_WINDOW);
    windowOption->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    windowOption->SetOnlySupportSceneBoard(true);
    windowOption->SetWindowTag(WindowTag::SUB_WINDOW);
}

WindowOption JsExtensionWindow::SetOption(WindowOption option, std::string title,
    bool decorEnabled)
{
    option.SetSubWindowTitle(title);
    option.SetSubWindowDecorEnable(decorEnabled);
    option.SetParentId(hostWindowId_);
    return option;
}
}  // namespace Rosen
}  // namespace OHOS
