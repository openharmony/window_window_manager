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


#include "ani_window.h"

#include <cstdint>
#include <memory>

#include "ani.h"
#include "ani_common_utils.h"
#include "window_helper.h"
#include "window_scene.h"
#include "window_manager.h"
#include "window_manager_hilog.h"

using OHOS::Rosen::WindowScene;

namespace OHOS {
namespace Rosen {
constexpr int32_t MIN_DECOR_HEIGHT = 37;
constexpr int32_t MAX_DECOR_HEIGHT = 112;

namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "AniWindow"};
/* used for free, ani has no destructor right now, only free when aniObj freed */
static std::map<ani_object, AniWindow*> localObjs;
} // namespace

AniWindow::AniWindow(const sptr<Window>& window)
    : windowToken_(window)
{
}

AniWindow* AniWindow::GetWindowObjectFromEnv(ani_env* env, ani_object obj)
{
    ani_class cls = nullptr;
    ani_status ret;
    if ((ret = env->FindClass("L@ohos/window/window/Window;", &cls)) != ANI_OK) {
        WLOGFE("[ANI] null env %{public}u", ret);
        return nullptr;
    }
    ani_field nativeObjName {};
    if ((ret = env->Class_FindField(cls, "nativeObj", &nativeObjName)) != ANI_OK) {
        WLOGFE("[ANI] obj fetch field %{public}u", ret);
        return nullptr;
    }
    ani_long nativeObj {};
    if ((ret = env->Object_GetField_Long(obj, nativeObjName, &nativeObj)) != ANI_OK) {
        WLOGFE("[ANI] obj fetch long %{public}u", ret);
        return nullptr;
    }
    return reinterpret_cast<AniWindow*>(nativeObj);
}

void DropWindowObjectByAni(ani_object aniObj)
{
    auto obj = localObjs.find(reinterpret_cast<ani_object>(aniObj));
    if (obj != localObjs.end()) {
        delete obj->second;
    }
    localObjs.erase(obj);
}

AniWindow* GetWindowObjectFromAni(void* aniObj)
{
    auto obj = localObjs.find(reinterpret_cast<ani_object>(aniObj));
    if (obj == localObjs.end()) {
        return nullptr;
    }
    return obj->second;
}

ani_object CreateAniWindowObject(ani_env* env, sptr<Window>& window)
__attribute__((no_sanitize("cfi")))
{
    if (env == nullptr) {
        WLOGFE("[ANI] null env");
        return nullptr;
    }
    WLOGFD("[ANI] create window obj");

    ani_status ret;
    ani_class cls = nullptr;
    if ((ret = env->FindClass("L@ohos/window/window/WindowInternal;", &cls)) != ANI_OK) {
        WLOGFE("[ANI] null env %{public}u", ret);
        return nullptr;
    }
    std::unique_ptr<AniWindow> aniWindow = std::make_unique<AniWindow>(window);
    WLOGFI("[ANI] native obj %{public}p", aniWindow.get());

    ani_method initFunc = nullptr;
    if ((ret = env->Class_FindMethod(cls, "<ctor>", ":V", &initFunc)) != ANI_OK) {
        WLOGFE("[ANI] get ctor fail %{public}u", ret);
        return nullptr;
    }
    ani_object obj = nullptr;
    if ((ret = env->Object_New(cls, initFunc, &obj)) != ANI_OK) {
        WLOGFE("[ANI] obj new fail %{public}u", ret);
        return nullptr;
    }
    ani_method setObjFunc = nullptr;
    if ((ret = env->Class_FindMethod(cls, "setNativeObj", "J:V", &setObjFunc)) != ANI_OK) {
        WLOGFE("[ANI] get ctor fail %{public}u", ret);
        return nullptr;
    }
    env->Object_CallMethod_Void(obj, setObjFunc, aniWindow.get());
    localObjs.insert(std::pair(obj, aniWindow.release()));
    return obj;
}

ani_int AniWindow::GetWindowDecorHeight(ani_env* env)
{
    int32_t height {0};
    wptr<Window> weakToken(windowToken_);
    auto window = weakToken.promote();
    if (window == nullptr) {
        WLOGFE("window is nullptr");
        return 0;
    }
    WMError ret = windowToken_->GetDecorHeight(height);
    if (ret != WMError::WM_OK) {
        if (ret == WMError::WM_ERROR_DEVICE_NOT_SUPPORT) {
            return 0;
        }
        return 0;
    }
    WLOGI("Window [%{public}u, %{public}s] OnGetDecorHeight end, height = %{public}d",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), height);
    return static_cast<ani_int>(height);
}

ani_object AniWindow::SetWindowBackgroundColorSync(ani_env* env, const std::string& color)
{
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetBackgroundColor(color));
    if (ret == WmErrorCode::WM_OK) {
        WLOGI("Window [%{public}u, %{public}s] set background color end",
            windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
        return AniCommonUtils::CreateAniUndefined(env);
    } else {
        return AniCommonUtils::AniThrowError(env, ret);
    }
}

ani_object AniWindow::SetImmersiveModeEnabledState(ani_env* env, bool enable)
{
    if (!WindowHelper::IsMainWindow(windowToken_->GetType()) &&
        !WindowHelper::IsSubWindow(windowToken_->GetType())) {
        TLOGE(WmsLogTag::WMS_IMMS, "[ANI]OnSetImmersiveModeEnabledState is not allowed since invalid window type");
        return AniCommonUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_CALLING);
    }
    if (windowToken_->IsPcOrPadFreeMultiWindowMode()) {
        TLOGE(WmsLogTag::WMS_IMMS, "device not support");
        return AniCommonUtils::CreateAniUndefined(env);
    }

    TLOGI(WmsLogTag::WMS_IMMS, "[ANI]OnSetImmersiveModeEnabledState to %{public}d", static_cast<int32_t>(enable));
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetImmersiveModeEnabledState(static_cast<bool>(enable)));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_IMMS, "Window immersive mode set enabled failed, ret = %{public}d", ret);
        return AniCommonUtils::AniThrowError(env, WmErrorCode::WM_ERROR_SYSTEM_ABNORMALLY);
    }

    TLOGI(WmsLogTag::WMS_IMMS, "window [%{public}u, %{public}s] OnSetImmersiveModeEnabledState end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
    return AniCommonUtils::CreateAniUndefined(env);
}

ani_object AniWindow::SetWindowDecorVisible(ani_env* env, bool isVisible)
{
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetDecorVisible(static_cast<bool>(isVisible)));
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("Window decor set visible failed");
        return AniCommonUtils::AniThrowError(env, ret);
    }
    WLOGI("Window [%{public}u, %{public}s] OnSetWindowDecorVisible end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
    return AniCommonUtils::CreateAniUndefined(env);
}

ani_object AniWindow::SetWindowDecorHeight(ani_env* env, ani_int height)
{
    if (height < MIN_DECOR_HEIGHT || height > MAX_DECOR_HEIGHT) {
        WLOGFE("height should greater than 37 or smaller than 112");
        return 0;
    }

    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetDecorHeight(static_cast<int32_t>(height)));
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("Set window decor height failed");
        return 0;
    }
    WLOGI("Window [%{public}u, %{public}s] OnSetDecorHeight end, height = %{public}d",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), height);
    return AniCommonUtils::CreateAniUndefined(env);
}

ani_object AniWindow::GetWindowPropertiesSync(ani_env* env)
{
    return AniCommonUtils::CreateWindowsProperties(env, windowToken_);
}

ani_boolean AniWindow::IsWindowSupportWideGamut(ani_env* env) {
    return static_cast<ani_boolean>(windowToken_->IsSupportWideGamut());
}

ani_object AniWindow::SetWindowLayoutFullScreen(ani_env* env, ani_boolean isLayoutFullScreen) {
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "windowToken_ is nullptr");
        return AniCommonUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }

    if (windowToken_->IsPcOrPadFreeMultiWindowMode()) {
        TLOGE(WmsLogTag::WMS_IMMS, "device not support");
        return AniCommonUtils::CreateAniUndefined(env);
    }

    WMError ret = windowToken_->SetLayoutFullScreen(static_cast<bool>(isLayoutFullScreen));
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_IMMS, "fullscreen set error");
        return AniCommonUtils::CreateAniUndefined(env);
    }
    return 0;
}

ani_object AniWindow::SetSystemBarProperties(ani_env* env, ani_object aniSystemBarProperties) {
    if (windowToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "windowToken_ is nullptr");
        return AniCommonUtils::CreateAniUndefined(env);
    }
    std::map<WindowType, SystemBarProperty> properties;
    std::map<WindowType, SystemBarPropertyFlag> jsSystemBarPropertyFlags;

    if (!AniCommonUtils::SetSystemBarPropertiesFromAni(env, properties, jsSystemBarPropertyFlags,
        aniSystemBarProperties, windowToken_)) {
        TLOGE(WmsLogTag::WMS_IMMS, "Failed to convert parameter to systemBarProperties");
    }
    return 0;
}

ani_object AniWindow::SetSpecificSystemBarEnabled(ani_env* env, ani_string name, ani_boolean enable,
    ani_boolean enableAnimation) {
    WLOGI("SetSystemBarEnable");
    std::map<WindowType, SystemBarProperty> systemBarProperties;
    if (!AniCommonUtils::SetSpecificSystemBarEnabled(env, systemBarProperties, name, enable, enableAnimation)) {
        return AniCommonUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    return 0;
}
}  // namespace Rosen
}  // namespace OHOS

static ani_int WindowGetWindowDecorHeight(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    WLOGI("[ANI]GetWindowDecorHeight");
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    return aniWindow->GetWindowDecorHeight(env);
}

static ani_int WindowSetWindowBackgroundColorSync(ani_env* env, ani_object obj, ani_long nativeObj, ani_string color)
{
    using namespace OHOS::Rosen;
    std::string colorStr;
    OHOS::Rosen::AniCommonUtils::GetStdString(env, color, colorStr);
    WLOGFI("[ANI] SetWindowBackgroundColorSync %{public}s", colorStr.c_str());
    AniWindow* aniWindow = reinterpret_cast<AniWindow*>(nativeObj);
    aniWindow->SetWindowBackgroundColorSync(env, colorStr);
    return ANI_OK;
}

ani_object CreateAniWindow(ani_env* env, OHOS::sptr<OHOS::Rosen::Window>& window)
__attribute__((no_sanitize("cfi")))
{
    using namespace OHOS::Rosen;
    if (env == nullptr) {
        WLOGFE("[ANI] null env");
        return nullptr;
    }
    WLOGFD("[ANI] create wstage");

    ani_status ret;
    ani_class cls = nullptr;
    if ((ret = env->FindClass("L@ohos/window/window/WindowInternal;", &cls)) != ANI_OK) {
        WLOGFE("[ANI] null env %{public}u", ret);
        return cls;
    }

    std::unique_ptr<AniWindow> uniqueWindow = std::make_unique<AniWindow>(window);
    WLOGFI("[ANI] native obj %{public}p", uniqueWindow.get());

    ani_field contextField;
    if ((ret = env->Class_FindField(cls, "nativeObj", &contextField)) != ANI_OK) {
        WLOGFE("[ANI] get field fail %{public}u", ret);
        return nullptr;
    }

    ani_method initFunc = nullptr;
    if ((ret = env->Class_FindMethod(cls, "<ctor>", ":V", &initFunc)) != ANI_OK) {
        WLOGFE("[ANI] get ctor fail %{public}u", ret);
        return nullptr;
    }
    ani_object obj = nullptr;
    if ((ret = env->Object_New(cls, initFunc, &obj)) != ANI_OK) {
        WLOGFE("[ANI] obj new fail %{public}u", ret);
        return nullptr;
    }
    ani_method setObjFunc = nullptr;
    if ((ret = env->Class_FindMethod(cls, "setNativeObj", "J:V", &setObjFunc)) != ANI_OK) {
        WLOGFE("[ANI] get ctor fail %{public}u", ret);
        return nullptr;
    }
    env->Object_CallMethod_Void(obj, setObjFunc, reinterpret_cast<ani_long>(uniqueWindow.get()));
    localObjs.insert(std::pair(obj, uniqueWindow.release()));

    WLOGFI("[ANI] window stage created  %{public}p", reinterpret_cast<void*>(obj));
    return obj;
}


static ani_object WindowCreate(ani_env* env, ani_long window)
{
    using namespace OHOS::Rosen;
    WLOGFI("[ANI] create window with scene 0x%{public}p %{public}d", reinterpret_cast<void*>(env), (int32_t)window);
    OHOS::sptr<Window> windowPtr;
    return CreateAniWindow(env, windowPtr); // just for test
}

ani_status OHOS::Rosen::ANI_Window_Constructor(ani_vm *vm, uint32_t *result)
{
    using namespace OHOS::Rosen;
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, OHOS::Rosen::HILOG_DOMAIN_WINDOW, "JsWindowStage"};
    ani_status ret;
    ani_env *env;
    if ((ret = vm->GetEnv(ANI_VERSION_1, &env)) != ANI_OK) {
        WLOGFE("[ANI] null env");
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    if ((ret = env->FindClass("L@ohos/window/window/WindowInternal;", &cls)) != ANI_OK) {
        WLOGFE("[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    std::array methods = {
        ani_native_function {"getWindowDecorHeight", "J:I", reinterpret_cast<void *>(WindowGetWindowDecorHeight)},
        ani_native_function {"setWindowBackgroundColorSync", "JLstd/core/String;:I",
            reinterpret_cast<void *>(WindowSetWindowBackgroundColorSync)},
    };
    if ((ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size())) != ANI_OK) {
        WLOGFE("[ANI] bind window method fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    *result = ANI_VERSION_1;

    // just for test
    ani_namespace ns;
    if ((ret = env->FindNamespace("L@ohos/window/window;", &ns)) != ANI_OK) {
        WLOGFE("[ANI] find ns %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    std::array functions = {
        ani_native_function {"CreateWindow", "J:L@ohos/window/window/WindowInternal;",
            reinterpret_cast<void *>(WindowCreate)},
    };
    if ((ret = env->Namespace_BindNativeFunctions(ns, functions.data(), functions.size())) != ANI_OK) {
        WLOGFE("[ANI] bind ns window func %{public}u", ret);
    }
    return ANI_OK;
}