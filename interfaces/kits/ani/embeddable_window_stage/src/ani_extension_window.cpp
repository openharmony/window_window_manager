/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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


#include "ani.h"
#include "window_manager_hilog.h"
#include "extension_window_impl.h"

#include "ani_extension_window.h"


namespace OHOS {
namespace Rosen {

namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "AniExtWindow"};
}

AniExtensionWindow::AniExtensionWindow(const std::shared_ptr<Rosen::ExtensionWindow> extensionWindow,
    int32_t hostWindowId) : extensionWindow_(extensionWindow), hostWindowId_(hostWindowId) {
        TLOGI(WmsLogTag::WMS_UIEXT, "%{public}d", hostWindowId_);
}

AniExtensionWindow::AniExtensionWindow(const std::shared_ptr<Rosen::ExtensionWindow> extensionWindow,
    sptr<AAFwk::SessionInfo> sessionInfo)
    : extensionWindow_(extensionWindow), hostWindowId_(-1), sessionInfo_(sessionInfo) {
}

ani_object AniExtensionWindow::CreateAniExtensionWindow(ani_env* env, sptr<Rosen::Window> window, int32_t hostWindowId)
{
    if (env == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "AniExtensionWindow env or window is nullptr");
        return nullptr;
    }

    ani_status ret;
    ani_class cls = nullptr;
    if ((ret = env->FindClass("L@ohos/uiExtensionHost/uiExtensionHost/UIExtensionHostInternal;", &cls)) != ANI_OK) {
        WLOGFE("[ANI] null env %{public}u", ret);
        return cls;
    }

    std::shared_ptr<ExtensionWindow> extensionWindow = std::make_shared<ExtensionWindowImpl>(window);
    std::unique_ptr<AniExtensionWindow> aniExtensionWindow =
        std::make_unique<AniExtensionWindow>(extensionWindow, hostWindowId);
    WLOGFI("[ANI] native obj %{public}p", aniExtensionWindow.get());

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
    env->Object_CallMethod_Void(obj, setObjFunc, reinterpret_cast<ani_long>(aniExtensionWindow.get()));
    return obj;
}
WMError AniExtensionWindow::GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea)
{
    if (extensionWindow_ == nullptr) {
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    return extensionWindow_->GetAvoidAreaByType(type, avoidArea);
}

static ani_int ExtWindowGetProperties(ani_env* env, ani_object obj, ani_object propertyRef)
{
    return (ani_int)(0u);
}
static ani_int ExtWindowSetProperties(ani_env* env, ani_object obj, ani_object propertyRef)
{
    return (ani_int)(0u);
}
static WMError ExtWindowSetAvoidArea2Ts(ani_env* env, ani_object obj, ani_int areaType, AvoidArea avoidArea)
{
    bool visible = (areaType != (int)AvoidAreaType::TYPE_CUTOUT);
    ani_class cls = nullptr;
    ani_status ret;
    if ((ret = env->FindClass("L@ohos/uiExtensionHost/uiExtensionHost/UIExtensionHostInternal;", &cls)) != ANI_OK) {
        WLOGFE("[ANI] find cls %{public}u", ret);
        return WMError::WM_DO_NOTHING;
    }
    //ani_field visibleField {};
    //if ((ret = env->Class_FindField(cls, "visible", &visibleField)) != ANI_OK) {
    //    WLOGFE("[ANI] null env %{public}u", ret);
    //    return ANI_NOT_FOUND;
    //}
    ani_method method {};
    if ((ret = env->Class_FindMethod(cls, "setAreaVisible", "", &method)) != ANI_OK) {
        WLOGFE("[ANI] find setAreaVisible failed %{public}u", ret);
        return WMError::WM_DO_NOTHING;
    }
    env->Object_CallMethod_Void(obj, method, visible);
    return WMError::WM_OK;
}

static ani_int ExtWindowGetWindowAvoidArea(ani_env* env, ani_object obj, ani_long win,
    ani_int areaType, ani_object area)
{
    bool err = ((areaType > (int)AvoidAreaType::TYPE_NAVIGATION_INDICATOR) ||
        (areaType < (int)AvoidAreaType::TYPE_SYSTEM));
    if (err) {
        return (int)WMError::WM_ERROR_INVALID_PARAM;
    }
    AniExtensionWindow* winPtr = reinterpret_cast<AniExtensionWindow*>(win);
    AvoidArea avoidArea;
    WMError ret = winPtr->GetAvoidAreaByType((AvoidAreaType)areaType, avoidArea);
    if (ret == WMError::WM_OK) {
        return (ani_int)ExtWindowSetAvoidArea2Ts(env, area, areaType, avoidArea);
    }
    return (ani_int)ret;
}
static ani_int ExtWindowOnAvoidAreaChange(ani_env* env, ani_object obj, ani_object callback)
{
    return (ani_int)(0u);
}
static ani_int ExtWindowOffAvoidAreaChange(ani_env* env, ani_object obj, ani_object callback)
{
    return (ani_int)(0u);
}
static ani_int ExtWindowOnWindowSizeChange(ani_env* env, ani_object obj, ani_object callback)
{
    return (ani_int)(0u);
}
static ani_int ExtWindowOffWindowSizeChange(ani_env* env, ani_object obj, ani_object callback)
{
    return (ani_int)(0u);
}

// test from ts
ani_object createExtentionWindow(ani_env* env, ani_long win, ani_int hostId)
{
    WLOGFI("[ANI] create extwindow with window 0x%{public}p %{public}d", reinterpret_cast<void*>(win), hostId);
    sptr<Rosen::Window> winPtr;
    return AniExtensionWindow::CreateAniExtensionWindow(env, winPtr, (int32_t)hostId);
}
}
} // namespace

extern "C" {
ANI_EXPORT ani_status ExtensionWindow_ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    using namespace OHOS::Rosen;
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, OHOS::Rosen::HILOG_DOMAIN_WINDOW, "AniExtWindow"};
    ani_status ret;
    ani_env *env;
    if ((ret = vm->GetEnv(ANI_VERSION_1, &env)) != ANI_OK) {
        WLOGFE("[ANI] null env");
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    if ((ret = env->FindClass("L@ohos/uiExtensionHost/uiExtensionHost/UIExtensionHostInternal;", &cls)) != ANI_OK) {
        WLOGFE("[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    std::array methods = {
        ani_native_function {"getProperties", "J:Lstd/core/Object;", reinterpret_cast<void *>(ExtWindowGetProperties)},
        ani_native_function {"setProperties", "Lstd/core/Object;:V", reinterpret_cast<void *>(ExtWindowSetProperties)},
        ani_native_function {"getWindowAvoidArea", nullptr, reinterpret_cast<void *>(ExtWindowGetWindowAvoidArea)},
        ani_native_function {"onAvoidAreaChange", nullptr, reinterpret_cast<void *>(ExtWindowOnAvoidAreaChange)},
        ani_native_function {"offAvoidAreaChange", nullptr, reinterpret_cast<void *>(ExtWindowOffAvoidAreaChange)},
        ani_native_function {"onWindowSizeChange", nullptr, reinterpret_cast<void *>(ExtWindowOnWindowSizeChange)},
        ani_native_function {"offWindowSizeChange", nullptr, reinterpret_cast<void *>(ExtWindowOffWindowSizeChange)},
    };
    if ((ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size())) != ANI_OK) {
        WLOGFE("[ANI] bind fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    *result = ANI_VERSION_1;

    // test
    ani_namespace ns;
    if ((ret = env->FindNamespace("L@ohos/uiExtensionHost/uiExtensionHost;", &ns)) != ANI_OK) {
        WLOGFE("[ANI] find ns %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    std::array functions = {
        ani_native_function {"createExtentionWindow", nullptr, reinterpret_cast<void *>(createExtentionWindow)},
    };
    if ((ret = env->Namespace_BindNativeFunctions(ns, functions.data(), functions.size())) != ANI_OK) {
        WLOGFE("[ANI] bind ns func %{public}u", ret);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}
}
