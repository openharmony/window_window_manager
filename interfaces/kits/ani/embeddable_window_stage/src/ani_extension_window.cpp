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

#include "ani_extension_window.h"

#include "ani.h"
#include "ani_extension_window_register_manager.h"
#include "extension_window_impl.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {

AniExtensionWindow::AniExtensionWindow(
    const std::shared_ptr<Rosen::ExtensionWindow> extensionWindow, int32_t hostWindowId)
    :extensionWindow_(extensionWindow),
    hostWindowId_(hostWindowId),
    extensionRegisterManager_(std::make_unique<AniExtensionWindowRegisterManager>())
{
    TLOGI(WmsLogTag::WMS_UIEXT, "%{public}d", hostWindowId_);
}

AniExtensionWindow::AniExtensionWindow(
    const std::shared_ptr<Rosen::ExtensionWindow> extensionWindow, sptr<AAFwk::SessionInfo> sessionInfo)
    :extensionWindow_(extensionWindow),
    hostWindowId_(-1),
    sessionInfo_(sessionInfo),
    extensionRegisterManager_(std::make_unique<AniExtensionWindowRegisterManager>())
{
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
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI] null env %{public}u", ret);
        return cls;
    }

    std::shared_ptr<ExtensionWindow> extensionWindow = std::make_shared<ExtensionWindowImpl>(window);
    std::unique_ptr<AniExtensionWindow> aniExtensionWindow =
        std::make_unique<AniExtensionWindow>(extensionWindow, hostWindowId);

    ani_field contextField;
    if ((ret = env->Class_FindField(cls, "nativeObj", &contextField)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI] get field fail %{public}u", ret);
        return nullptr;
    }

    ani_method initFunc = nullptr;
    if ((ret = env->Class_FindMethod(cls, "<ctor>", ":V", &initFunc)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI] get ctor fail %{public}u", ret);
        return nullptr;
    }
    ani_object obj = nullptr;
    if ((ret = env->Object_New(cls, initFunc, &obj)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI] obj new fail %{public}u", ret);
        return nullptr;
    }
    ani_method setObjFunc = nullptr;
    if ((ret = env->Class_FindMethod(cls, "setNativeObj", "J:V", &setObjFunc)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI] get ctor fail %{public}u", ret);
        return nullptr;
    }
    env->Object_CallMethod_Void(obj, setObjFunc, reinterpret_cast<ani_long>(aniExtensionWindow.release()));
    return obj;
}

WMError AniExtensionWindow::GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea)
{
    if (!WindowIsValid()) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI] window not available");
        return WMError::WM_DO_NOTHING;
    }
    return extensionWindow_->GetAvoidAreaByType(type, avoidArea);
}

WMError AniExtensionWindow::OnSetWaterMarkFlag(ani_env* env, ani_boolean enable)
{
    if (extensionWindow_ == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extensionWindow_ is nullptr");
        return WMError::WM_DO_NOTHING;
    }
    WmErrorCode ret = WmErrorCode::WM_OK;
    ret = WM_JS_TO_ERROR_CODE_MAP.at(extensionWindow_->SetWaterMarkFlag(enable));
    if (ret != WmErrorCode::WM_OK) {
        return WMError::WM_DO_NOTHING;
    }
    TLOGI(WmsLogTag::WMS_UIEXT, "[ANI] OnSetWaterMarkFlag success,isEnable:%{public}u.",
        enable);
    return WMError::WM_OK;
}

WMError AniExtensionWindow::OnHidePrivacyContentForHost(ani_env* env, ani_boolean shouldHide)
{
    if (extensionWindow_ == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "extensionWindow_ is nullptr");
        return WMError::WM_DO_NOTHING;
    }
    WmErrorCode ret = WmErrorCode::WM_OK;
    ret = WM_JS_TO_ERROR_CODE_MAP.at(extensionWindow_->HidePrivacyContentForHost(shouldHide));
    if (ret != WmErrorCode::WM_OK) {
        return WMError::WM_DO_NOTHING;
    }
    TLOGI(WmsLogTag::WMS_UIEXT,
        "[ANI] OnHidePrivacyContentForHost success,shouldHide:%{public}u.", shouldHide);
    return WMError::WM_OK;
}

bool AniExtensionWindow::WindowIsValid()
{
    if (extensionWindow_ == nullptr) {
        return false;
    }
    sptr<Rosen::Window> window = extensionWindow_->GetWindow();
    if (window == nullptr) {
        return false;
    }
    return true;
}

WMError AniExtensionWindow::UnregisterListener(const std::string& cbType, ani_env* env, ani_object fn)
{
    if (!WindowIsValid()) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI] window not available");
        return WMError::WM_DO_NOTHING;
    }
    sptr<Rosen::Window> window = extensionWindow_->GetWindow();
    return extensionRegisterManager_->UnregisterListener(window, cbType, CaseType::CASE_WINDOW, env, fn);
}

WMError AniExtensionWindow::RegisterListener(const std::string& cbType, ani_env* env, ani_object fn, ani_object fnArg)
{
    if (!WindowIsValid()) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI] window not available");
        return WMError::WM_DO_NOTHING;
    }
    sptr<Rosen::Window> window = extensionWindow_->GetWindow();
    return extensionRegisterManager_->RegisterListener(window, cbType, CaseType::CASE_WINDOW, env, fn, fnArg);
}

WMError AniExtensionWindow::GetRect(Rect& rect)
{
    if (!WindowIsValid()) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI] window not available");
        return WMError::WM_DO_NOTHING;
    }
    rect = extensionWindow_->GetWindow()->GetRect();
    return WMError::WM_OK;
}

static ani_int ExtWindowSetRect(ani_env* env, ani_object obj, OHOS::Rosen::Rect& rect)
{
    ani_status ret {};
    if ((ret = env->Object_SetFieldByName_Double(obj, "<property>left", rect.posX_)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI] set left field %{public}u", ret);
        return (ani_int)WMError::WM_DO_NOTHING;
    }
    if ((ret = env->Object_SetFieldByName_Double(obj, "<property>top", rect.posY_)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI] set top field %{public}u", ret);
        return (ani_int)WMError::WM_DO_NOTHING;
    }
    if ((ret = env->Object_SetFieldByName_Double(obj, "<property>width", rect.width_)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI] set width field %{public}u", ret);
        return (ani_int)WMError::WM_DO_NOTHING;
    }
    if ((ret = env->Object_SetFieldByName_Double(obj, "<property>height", rect.height_)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI] set height field %{public}u", ret);
        return (ani_int)WMError::WM_DO_NOTHING;
    }
    return (ani_int)WMError::WM_OK;
}

static ani_int ExtWindowSetRectMember(ani_env* env, ani_object obj, const char* member, OHOS::Rosen::Rect& rect)
{
    ani_status ret {};
    ani_ref rectRef {};
    if ((ret = env->Object_GetFieldByName_Ref(obj, member, &rectRef)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI] get field %{public}s %{public}u", member, ret);
        return (ani_int)WMError::WM_DO_NOTHING;
    };
    return ExtWindowSetRect(env, (ani_object)rectRef, rect);
}

static ani_int ExtWindowGetProperties(ani_env* env, ani_object obj, ani_long nativeObj, ani_object propertyRef)
{
    WMError retCode = WMError::WM_OK;
    AniExtensionWindow* aniExt = reinterpret_cast<AniExtensionWindow*>(nativeObj);
    Rect rect {};
    if ((retCode = aniExt->GetRect(rect)) != WMError::WM_OK) {
        return (ani_int)retCode;
    };
    return ExtWindowSetRectMember(env, propertyRef, "<property>uiExtensionHostWindowProxyRect", rect);
}

static ani_int ExtWindowSetProperties(ani_env* env, ani_object obj, ani_long nativeObj, ani_object propertyRef)
{
    return (ani_int)WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
}

static ani_int ExtWindowGetWindowAvoidArea(ani_env* env, ani_object obj, ani_long win,
    ani_int areaType, ani_object area)
{
    bool err = ((areaType > (int)AvoidAreaType::TYPE_NAVIGATION_INDICATOR) ||
        (areaType < (int)AvoidAreaType::TYPE_SYSTEM));
    if (err) {
        return (ani_int)WMError::WM_ERROR_INVALID_PARAM;
    }
    AvoidArea avoidArea {};
    ani_status ret {};
    ret = env->Object_SetFieldByName_Boolean(area, "<property>visible", areaType != (int)AvoidAreaType::TYPE_CUTOUT);
    if (ret != ANI_OK) {
        TLOGI(WmsLogTag::WMS_UIEXT, "[ANI] set visible faild %{public}d", (int)ret);
        return (ani_int)WMError::WM_DO_NOTHING;
    }
    AniExtensionWindow* winPtr = reinterpret_cast<AniExtensionWindow*>(win);
    WMError retCode = winPtr->GetAvoidAreaByType((AvoidAreaType)areaType, avoidArea);
    if (retCode != WMError::WM_OK) {
        return (ani_int)retCode;
    }
    if ((retCode = (WMError)ExtWindowSetRectMember(env, area, "<property>leftRect", avoidArea.leftRect_)) !=
        WMError::WM_OK) {
        return (ani_int)retCode;
    }
    if ((retCode = (WMError)ExtWindowSetRectMember(env, area, "<property>rightRect", avoidArea.rightRect_)) !=
        WMError::WM_OK) {
        return (ani_int)retCode;
    }
    if ((retCode = (WMError)ExtWindowSetRectMember(env, area, "<property>topRect", avoidArea.topRect_)) !=
        WMError::WM_OK) {
        return (ani_int)retCode;
    }
    if ((retCode = (WMError)ExtWindowSetRectMember(env, area, "<property>bottomRect", avoidArea.bottomRect_)) !=
        WMError::WM_OK) {
        return (ani_int)retCode;
    }
    return (ani_int)WMError::WM_OK;
}

static ani_int ExtWindowSetWaterMarkFlag(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean enable)
{
    AniExtensionWindow* winPtr = reinterpret_cast<AniExtensionWindow*>(nativeObj);
    WMError ret = WMError::WM_DO_NOTHING;
    if (winPtr != nullptr) {
        ret = winPtr->OnSetWaterMarkFlag(env, enable);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] AniExtensionWindow is nullptr");
    }
    return (ani_int)ret;
}

static ani_int ExtWindowHidePrivacyContentForHost(ani_env* env, ani_object obj, ani_long nativeObj,
    ani_boolean shouldHide)
{
    AniExtensionWindow* winPtr = reinterpret_cast<AniExtensionWindow*>(nativeObj);
    WMError ret = WMError::WM_DO_NOTHING;
    if (winPtr != nullptr) {
        ret = winPtr->OnHidePrivacyContentForHost(env, shouldHide);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] AniExtensionWindow is nullptr");
    }
    return (ani_int)ret;
}

static ani_int ExtWindowOnAvoidAreaChange(ani_env* env, ani_object obj, ani_long nativeObj,
    ani_object fn, ani_object fnArg)
{
    AniExtensionWindow* winPtr = reinterpret_cast<AniExtensionWindow*>(nativeObj);
    return (ani_int)winPtr->RegisterListener("avoidAreaChange", env, fn, fnArg);
}

static ani_int ExtWindowOffAvoidAreaChange(ani_env* env, ani_object obj, ani_long nativeObj,
    ani_object fn)
{
    AniExtensionWindow* winPtr = reinterpret_cast<AniExtensionWindow*>(nativeObj);
    return (ani_int)winPtr->UnregisterListener("avoidAreaChange", env, fn);
}
static ani_int ExtWindowOnWindowSizeChange(ani_env* env, ani_object obj, ani_long nativeObj,
    ani_object fn, ani_object fnArg)
{
    AniExtensionWindow* winPtr = reinterpret_cast<AniExtensionWindow*>(nativeObj);
    return (ani_int)winPtr->RegisterListener("windowSizeChange", env, fn, fnArg);
}
static ani_int ExtWindowOffWindowSizeChange(ani_env* env, ani_object obj, ani_long nativeObj,
    ani_object fn)
{
    AniExtensionWindow* winPtr = reinterpret_cast<AniExtensionWindow*>(nativeObj);
    return (ani_int)winPtr->UnregisterListener("windowSizeChange", env, fn);
}

// test from ts
ani_object createExtentionWindow(ani_env* env, ani_long win, ani_int hostId)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "[ANI] create extwindow with hostId:%{public}d", hostId);
    sptr<Rosen::Window> winPtr;
    return AniExtensionWindow::CreateAniExtensionWindow(env, winPtr, (int32_t)hostId);
}
}
} // namespace

extern "C" {
using namespace OHOS::Rosen;
std::array extensionWindowNativeMethods = {
    ani_native_function {"getProperties", "JLstd/core/Object;:I", reinterpret_cast<void *>(ExtWindowGetProperties)},
    ani_native_function {"setProperties", "JLstd/core/Object;:I", reinterpret_cast<void *>(ExtWindowSetProperties)},
    ani_native_function {"getWindowAvoidArea", "JILstd/core/Object;:I",
        reinterpret_cast<void *>(ExtWindowGetWindowAvoidArea)},
    ani_native_function {"setWaterMarkFlag", "JZ:V",
        reinterpret_cast<void *>(ExtWindowSetWaterMarkFlag)},
    ani_native_function {"hidePrivacyContentForHost", "JZ:V",
        reinterpret_cast<void *>(ExtWindowHidePrivacyContentForHost)},
    ani_native_function {"onAvoidAreaChange", "JLstd/core/Object;Lstd/core/Object;:I",
        reinterpret_cast<void *>(ExtWindowOnAvoidAreaChange)},
    ani_native_function {"onWindowSizeChange", "JLstd/core/Object;Lstd/core/Object;:I",
        reinterpret_cast<void *>(ExtWindowOnWindowSizeChange)},
    ani_native_function {"offAvoidAreaChange", "JLstd/core/Object;:I",
        reinterpret_cast<void *>(ExtWindowOffAvoidAreaChange)},
    ani_native_function {"offWindowSizeChange", "JLstd/core/Object;:I",
        reinterpret_cast<void *>(ExtWindowOffWindowSizeChange)},
    };
ANI_EXPORT ani_status ExtensionWindow_ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    using namespace OHOS::Rosen;
    TLOGD(WmsLogTag::WMS_UIEXT, "[ANI] init ExtensionWindow begin");
    ani_status ret;
    ani_env* env;
    if ((ret = vm->GetEnv(ANI_VERSION_1, &env)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI] null env");
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    if ((ret = env->FindClass("L@ohos/uiExtensionHost/uiExtensionHost/UIExtensionHostInternal;", &cls)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }

    if ((ret = env->Class_BindNativeMethods(cls, extensionWindowNativeMethods.data(),
        extensionWindowNativeMethods.size())) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI] bind fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    *result = ANI_VERSION_1;

    // test
    ani_namespace ns;
    if ((ret = env->FindNamespace("L@ohos/uiExtensionHost/uiExtensionHost;", &ns)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI] find ns %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    std::array functions = {
        ani_native_function {"createExtentionWindow", nullptr, reinterpret_cast<void *>(createExtentionWindow)},
    };
    if ((ret = env->Namespace_BindNativeFunctions(ns, functions.data(), functions.size())) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "[ANI] bind ns func %{public}u", ret);
        return ANI_NOT_FOUND;
    }

    TLOGD(WmsLogTag::WMS_UIEXT, "[ANI] init ExtensionWindow end");
    return ANI_OK;
}
}
