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

#include "ani_window_stage.h"

#include <memory>

#include "ani.h"
#include "ani_window.h"
#include "ani_window_manager.h"
#include "ani_window_utils.h"
#include "permission.h"
#include "pixel_map.h"
#include "pixel_map_taihe_ani.h"
#include "window_manager_hilog.h"
#include "window_scene.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "js_window_stage.h"
#include "ani_extension_window_config.h"

using OHOS::Rosen::WindowScene;

namespace OHOS {
namespace Rosen {
namespace {
/* used for free, ani has no destructor right now, only free when aniObj freed */
static std::map<ani_object, AniWindowStage*> g_localObjs;
const uint32_t MIN_RESOURCE_ID = 0x1000000;
const uint32_t MAX_RESOURCE_ID = 0xffffffff;

} // namespace

AniWindowStage::AniWindowStage(const std::shared_ptr<Rosen::WindowScene>& windowScene)
    : windowScene_(windowScene), registerManager_(std::make_unique<AniWindowRegisterManager>())
{
}
AniWindowStage::~AniWindowStage()
{
    TLOGE(WmsLogTag::DEFAULT, "[ANI] Ani WindowStage died");
}

ani_object AniWindowStage::NativeTransferStatic(ani_env* aniEnv, ani_class cls, ani_object input)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[ANI]");
    void *unwrapResult = nullptr;
    if (!arkts_esvalue_unwrap(aniEnv, input, &unwrapResult)) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] fail to unwrap input");
        return AniWindowUtils::AniThrowError(aniEnv, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    if (unwrapResult == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] unwrapResult is nullptr");
        return AniWindowUtils::AniThrowError(aniEnv, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    JsWindowStage* jsWindowStage = static_cast<JsWindowStage*>(unwrapResult);
    if (jsWindowStage == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] jsWindowStage is nullptr");
        return AniWindowUtils::AniThrowError(aniEnv, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    std::shared_ptr<WindowScene> windowScene = jsWindowStage->GetWindowScene().lock();
    return CreateAniWindowStage(aniEnv, windowScene);
}

ani_object AniWindowStage::NativeTransferDynamic(ani_env* aniEnv, ani_class cls, ani_long nativeObj)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[ANI]");
    AniWindowStage* aniWindowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (aniWindowStage == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] aniWindowStage is nullptr");
        return AniWindowUtils::AniThrowError(aniEnv, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    napi_env napiEnv {};
    if (!arkts_napi_scope_open(aniEnv, &napiEnv)) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] napi scope open fail");
        return AniWindowUtils::AniThrowError(aniEnv, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    napi_value jsWindowStage = CreateJsWindowStage(napiEnv, aniWindowStage->GetWindowScene().lock());
    hybridgref ref {};
    if (!hybridgref_create_from_napi(napiEnv, jsWindowStage, &ref)) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] create hybridgref fail");
        return AniWindowUtils::AniThrowError(aniEnv, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    ani_object result {};
    if (!hybridgref_get_esvalue(aniEnv, ref, &result)) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] get esvalue fail");
        return AniWindowUtils::AniThrowError(aniEnv, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    if (!hybridgref_delete_from_napi(napiEnv, ref)) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] delete hybridgref fail");
    }
    if (!arkts_napi_scope_close_n(napiEnv, 0, nullptr, nullptr)) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] napi close scope fail");
        return AniWindowUtils::AniThrowError(aniEnv, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    return result;
}

void AniWindowStage::LoadContent(ani_env* env, ani_object obj, ani_long nativeObj, ani_string path,
    ani_object storage)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[ANI]");
    AniWindowStage* aniWindowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (aniWindowStage != nullptr) {
        aniWindowStage->OnLoadContent(env, path, storage);
    } else {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] aniWindowStage is nullptr");
    }
}

void AniWindowStage::OnLoadContent(ani_env* env, ani_string path, ani_object storage)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[ANI]");
    auto windowScene = GetWindowScene().lock();
    if (windowScene == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI]windowScene is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    auto mainWindow = windowScene->GetMainWindow();
    if (mainWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] mainWindow is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    std::string contentPath;
    AniWindowUtils::GetStdString(env, path, contentPath);
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(mainWindow->NapiSetUIContent(contentPath, env, storage));
    TLOGI(WmsLogTag::WMS_LIFE, "[ANI] Window [%{public}u, %{public}s] load content end, ret=%{public}d",
        mainWindow->GetWindowId(), mainWindow->GetWindowName().c_str(), ret);
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "Window load content failed");
    }
}

ani_ref AniWindowStage::GetMainWindow(ani_env* env)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI] Get main window");
    std::shared_ptr<WindowScene> weakScene = windowScene_.lock();
    if (weakScene == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] WindowScene_ is nullptr");
        return AniWindowUtils::CreateAniUndefined(env);
    }

    sptr<Window> windowScene = weakScene->GetMainWindow();
    if (windowScene == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] Get main window failed");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    TLOGI(WmsLogTag::DEFAULT, "[ANI] Get main window [%{public}u, %{public}s]",
        windowScene->GetWindowId(), windowScene->GetWindowName().c_str());

    return CreateAniWindowObject(env, windowScene);
}

void DropWindowStageByAni(ani_object aniObj)
{
    auto obj = g_localObjs.find(reinterpret_cast<ani_object>(aniObj));
    if (obj != g_localObjs.end()) {
        delete obj->second;
    }
    g_localObjs.erase(obj);
}

AniWindowStage* GetWindowStageFromAni(void* aniObj)
{
    auto obj = g_localObjs.find(reinterpret_cast<ani_object>(aniObj));
    if (obj == g_localObjs.end()) {
        return nullptr;
    }
    return obj->second;
}

ani_object CreateAniWindowStage(ani_env* env, std::shared_ptr<Rosen::WindowScene>& windowScene)
__attribute__((no_sanitize("cfi")))
{
    if (env == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] null env");
        return nullptr;
    }
    TLOGD(WmsLogTag::DEFAULT, "[ANI] create wstage");

    ani_status ret;
    ani_class cls = nullptr;
    if ((ret = env->FindClass("L@ohos/window/window/WindowStageInternal;", &cls)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] null env %{public}u", ret);
        return cls;
    }

    std::unique_ptr<AniWindowStage> windowStage = std::make_unique<AniWindowStage>(windowScene);

    ani_method initFunc = nullptr;
    if ((ret = env->Class_FindMethod(cls, "<ctor>", ":V", &initFunc)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] get ctor fail %{public}u", ret);
        return nullptr;
    }
    ani_object obj = nullptr;
    if ((ret = env->Object_New(cls, initFunc, &obj)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] obj new fail %{public}u", ret);
        return nullptr;
    }
    ani_method setObjFunc = nullptr;
    if ((ret = env->Class_FindMethod(cls, "setNativeObj", "J:V", &setObjFunc)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] call setNativeObj fail %{public}u", ret);
        return nullptr;
    }
    env->Object_CallMethod_Void(obj, setObjFunc, reinterpret_cast<ani_long>(windowStage.get()));
    g_localObjs.insert(std::pair(obj, windowStage.release()));
    return obj;
}

AniWindowStage* GetWindowStageFromEnv(ani_env* env, ani_class cls, ani_object obj)
{
    ani_field nativeObjName {};
    ani_status ret;
    if ((ret = env->Class_FindField(cls, "nativeObj", &nativeObjName)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] obj fetch field %{public}u", ret);
        return nullptr;
    }
    ani_long nativeObj {};
    if ((ret = env->Object_GetField_Long(obj, nativeObjName, &nativeObj)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] obj fetch long %{public}u", ret);
        return nullptr;
    }
    return reinterpret_cast<AniWindowStage*>(nativeObj);
}

void AniWindowStage::DisableWindowDecor(ani_env* env, ani_object obj, ani_long nativeObj)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindowStage* aniWindowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (aniWindowStage != nullptr) {
        aniWindowStage->OnDisableWindowDecor(env);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindowStage is nullptr");
    }
}
void AniWindowStage::OnDisableWindowDecor(ani_env* env)
{
    auto windowScene = GetWindowScene().lock();
    if (windowScene == nullptr) {
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    auto mainWindow = windowScene->GetMainWindow();
    if (mainWindow == nullptr) {
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(mainWindow->DisableAppWindowDecor());
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret);
        return;
    }
}
    
void AniWindowStage::SetShowOnLockScreen(ani_env* env, ani_class cls, ani_long nativeObj, ani_boolean showOnLockScreen)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindowStage* aniWindowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (aniWindowStage != nullptr) {
        aniWindowStage->OnSetShowOnLockScreen(env, showOnLockScreen);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindowStage is nullptr");
    }
}
void AniWindowStage::OnSetShowOnLockScreen(ani_env* env, ani_boolean showOnLockScreen)
{
    if (!Permission::IsSystemCalling() && !Permission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]set show on lock screen permission denied!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_NOT_SYSTEM_APP);
        return;
    }
    auto windowScene = GetWindowScene().lock();
    if (windowScene == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]windowScene is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    auto mainWindow = windowScene->GetMainWindow();
    if (mainWindow == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] mainWindowis nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    if (showOnLockScreen) {
        mainWindow->AddWindowFlag(OHOS::Rosen::WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED);
    } else {
        mainWindow->RemoveWindowFlag(OHOS::Rosen::WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED);
    }
    TLOGE(WmsLogTag::DEFAULT, "[ANI] OnSetShowOnLockScreen end!");
}

void AniWindowStage::SetImageForRecent(ani_env* env, ani_class cls, ani_long nativeObj, ani_object imageResourceId,
    ani_int value)
{
    TLOGI(WmsLogTag::WMS_PATTERN, "[ANI]");
    AniWindowStage* aniWindowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (aniWindowStage != nullptr) {
        aniWindowStage->OnSetImageForRecent(env, imageResourceId, value);
    } else {
        TLOGE(WmsLogTag::WMS_PATTERN, "[ANI] aniWindowStage is nullptr");
    }
}

void AniWindowStage::OnSetImageForRecent(ani_env* env, ani_object imageResource, ani_int value)
{
    auto windowScene = GetWindowScene().lock();
    if (windowScene == nullptr) {
        TLOGE(WmsLogTag::WMS_PATTERN, "[ANI]windowScene is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    auto mainWindow = windowScene->GetMainWindow();
    if (mainWindow == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] mainWindow is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    ani_class longClass {};
    env->FindClass("std.core.Long", &longClass);
    ani_class pixelMapClass {};
    env->FindClass("@ohos.multimedia.image.image.PixelMap", &pixelMapClass);
    ani_boolean isLong = ANI_FALSE;
    env->Object_InstanceOf(imageResource, longClass, &isLong);
    ani_boolean isPixelMap = ANI_FALSE;
    env->Object_InstanceOf(imageResource, pixelMapClass, &isPixelMap);
 
    ani_long imageResourceId;
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    if (isLong) {
        env->Object_CallMethodByName_Long(imageResource, "unboxed", ":l", &imageResourceId);
        if (imageResourceId < MIN_RESOURCE_ID || imageResourceId > MAX_RESOURCE_ID) {
            TLOGE(WmsLogTag::WMS_PATTERN, "imageResourceId invalid: %{public}d", static_cast<int32_t>(imageResourceId));
            AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_ILLEGAL_PARAM);
            return;
        }
    } else if (isPixelMap) {
        pixelMap = Media::PixelMapTaiheAni::GetNativePixelMap(env, imageResource);
        if (pixelMap == nullptr) {
            TLOGE(WmsLogTag::WMS_PATTERN, "Get pixelMap error");
            AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_ILLEGAL_PARAM);
            return;
        }
    } else {
        TLOGE(WmsLogTag::WMS_PATTERN, "imageResource invalid");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }
    if (value < static_cast<int32_t>(ImageFit::FILL) || value > static_cast<int32_t>(ImageFit::MATRIX)) {
        TLOGE(WmsLogTag::WMS_PATTERN, "imageFit invalid: %{public}d", value);
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_ILLEGAL_PARAM);
        return;
    }
    Ark_ImageFit arkImageFit = static_cast<Ark_ImageFit>(value);
    ImageFit imageFit = ImageFit::FILL;
    AniWindowUtils::ConvertImageFit(imageFit, arkImageFit);
    TLOGI(WmsLogTag::WMS_PATTERN, "value: %{public}d, imageFit: %{public}d", value, imageFit);
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (pixelMap) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(mainWindow->SetImageForRecentPixelMap(pixelMap, imageFit));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(mainWindow->SetImageForRecent(imageResourceId, imageFit));
    }
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret);
        return;
    }
}

void AniWindowStage::RemoveImageForRecent(ani_env* env, ani_class cls, ani_long nativeObj)
{
    TLOGI(WmsLogTag::WMS_PATTERN, "[ANI]");
    AniWindowStage* aniWindowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (aniWindowStage != nullptr) {
        aniWindowStage->OnRemoveImageForRecent(env);
    } else {
        TLOGE(WmsLogTag::WMS_PATTERN, "[ANI] aniWindowStage is nullptr");
    }
}
 
void AniWindowStage::OnRemoveImageForRecent(ani_env* env)
{
    auto windowScene = GetWindowScene().lock();
    if (windowScene == nullptr) {
        TLOGE(WmsLogTag::WMS_PATTERN, "[ANI]windowScene is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    auto mainWindow = windowScene->GetMainWindow();
    if (mainWindow == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] mainWindow is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(mainWindow->RemoveImageForRecent());
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret);
        return;
    }
}

void AniWindowStage::SetCustomDensity(ani_env* env, ani_object obj, ani_long nativeObj,
    ani_double density, ani_boolean applyToSubWindow)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    AniWindowStage* aniWindowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (aniWindowStage != nullptr) {
        aniWindowStage->OnSetCustomDensity(env, density, applyToSubWindow);
    } else {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] aniWindowStage is nullptr");
        return;
    }
}

void AniWindowStage::OnSetCustomDensity(ani_env* env, ani_double density, ani_boolean applyToSubWindow)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    auto windowScene = GetWindowScene().lock();
    if (windowScene == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI]windowScene is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    auto window = windowScene->GetMainWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] mainWindow is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
        window->SetCustomDensity(static_cast<float>(density), applyToSubWindow));
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "Window [%{public}u,%{public}s] set density=%{public}f, result=%{public}d",
        window->GetWindowId(), window->GetWindowName().c_str(), density, ret);
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret);
        return;
    }
}

void AniWindowStage::SetDefaultDensityEnabled(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean enabled)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    AniWindowStage* aniWindowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (aniWindowStage != nullptr) {
        aniWindowStage->OnSetDefaultDensityEnabled(env, enabled);
    } else {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] aniWindowStage is nullptr");
        return;
    }
}

void AniWindowStage::OnSetDefaultDensityEnabled(ani_env* env, ani_boolean enabled)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    auto windowScene = GetWindowScene().lock();
    if (windowScene == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI]windowScene is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    auto window = windowScene->GetMainWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] mainWindow is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetDefaultDensityEnabled(enabled));
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "Window [%{public}u,%{public}s] enabled=%{public}u ret=%{public}u",
        window->GetWindowId(), window->GetWindowName().c_str(), enabled, ret);
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret);
        return;
    }
}

ani_ref AniWindowStage::OnCreateSubWindow(ani_env* env, ani_string name)
{
    std::string windowName;
    ani_status ret = AniWindowUtils::GetStdString(env, name, windowName);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] invalid param of name");
        return AniWindowUtils::CreateAniUndefined(env);
    }

    auto weakScene = windowScene_.lock();
    if (weakScene == nullptr) {
        TLOGI(WmsLogTag::DEFAULT, "[ANI] Window scene is nullptr");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    sptr<Rosen::WindowOption> windowOption = new Rosen::WindowOption();
    windowOption->SetWindowType(Rosen::WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    windowOption->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    auto window = weakScene->CreateWindow(windowName, windowOption);
    if (window == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] Create window failed");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
    return CreateAniWindowObject(env, window);
}

void AniWindowStage::RegisterWindowCallback(ani_env* env, ani_object obj, ani_long nativeObj, ani_string type,
    ani_ref callback)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindowStage* aniWindowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (aniWindowStage != nullptr) {
        aniWindowStage->OnRegisterWindowCallback(env, type, callback);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindowStage is nullptr");
    }
}

void AniWindowStage::OnRegisterWindowCallback(ani_env* env, ani_string type, ani_ref callback)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto windowScene = GetWindowScene().lock();
    if (windowScene == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]windowScene is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    auto mainWindow = windowScene->GetMainWindow();
    if (mainWindow == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] mainWindow is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    std::string cbType;
    AniWindowUtils::GetStdString(env, type, cbType);
    TLOGI(WmsLogTag::DEFAULT, "[ANI] type:%{public}s", cbType.c_str());
    WmErrorCode ret = registerManager_->RegisterListener(mainWindow, cbType, CaseType::CASE_STAGE, env, callback, 0);
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret);
        return;
    }
}

void AniWindowStage::UnregisterWindowCallback(ani_env* env, ani_object obj, ani_long nativeObj, ani_string type,
    ani_ref callback)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindowStage* aniWindowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (aniWindowStage != nullptr) {
        aniWindowStage->OnUnregisterWindowCallback(env, type, callback);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindowStage is nullptr");
    }
}

void AniWindowStage::OnUnregisterWindowCallback(ani_env* env, ani_string type, ani_ref callback)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto windowScene = GetWindowScene().lock();
    if (windowScene == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]windowScene is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    auto mainWindow = windowScene->GetMainWindow();
    if (mainWindow == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] mainWindow is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    std::string cbType;
    AniWindowUtils::GetStdString(env, type, cbType);
    TLOGI(WmsLogTag::DEFAULT, "[ANI] type:%{public}s", cbType.c_str());
    WmErrorCode ret = registerManager_->UnregisterListener(mainWindow, cbType, CaseType::CASE_STAGE, env, callback);
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret);
        return;
    }
}
}  // namespace Rosen
}  // namespace OHOS

static ani_object WindowStageCreate(ani_env* env, ani_long scene)
{
    using namespace OHOS::Rosen;
    std::shared_ptr<WindowScene> scenePtr;
    return CreateAniWindowStage(env, scenePtr); // just for test
}

static ani_ref WindowGetMainWindow(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    TLOGD(WmsLogTag::DEFAULT, "[ANI]");
    AniWindowStage* windowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (windowStage == nullptr || windowStage->GetWindowScene().lock() == nullptr) {
        TLOGD(WmsLogTag::DEFAULT, "[ANI] windowStage is nullptr");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    return windowStage->GetMainWindow(env);
}

static ani_ref CreateSubWindow(ani_env* env, ani_object obj, ani_long nativeObj, ani_string name)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindowStage* windowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (windowStage == nullptr || windowStage->GetWindowScene().lock() == nullptr) {
        TLOGD(WmsLogTag::DEFAULT, "[ANI] windowStage is nullptr");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    return windowStage->OnCreateSubWindow(env, name);
}

extern "C" {
using namespace OHOS::Rosen;
ANI_EXPORT ani_status ExtensionWindowConfig_ANI_Constructor(ani_vm *vm, uint32_t *result);
std::array g_methods = {
    ani_native_function {"loadContentSync",
        "JLstd/core/String;Larkui/stateManagement/storage/localStorage/LocalStorage;:V",
        reinterpret_cast<void *>(AniWindowStage::LoadContent)},
    ani_native_function {"disableWindowDecorSync", nullptr,
        reinterpret_cast<void *>(AniWindowStage::DisableWindowDecor)},
    ani_native_function {"setShowOnLockScreenSync",
        nullptr, reinterpret_cast<void *>(AniWindowStage::SetShowOnLockScreen)},
    ani_native_function {"getMainWindowSync", "J:L@ohos/window/window/Window;",
        reinterpret_cast<void *>(WindowGetMainWindow)},
    ani_native_function {"createSubWindowSync", "lC{std.core.String}:C{@ohos.window.window.Window}",
        reinterpret_cast<void *>(CreateSubWindow)},
    ani_native_function {"onSync", nullptr,
        reinterpret_cast<void *>(AniWindowStage::RegisterWindowCallback)},
    ani_native_function {"offSync", nullptr,
        reinterpret_cast<void *>(AniWindowStage::UnregisterWindowCallback)},
    ani_native_function {"nativeTransferStatic", "Lstd/interop/ESValue;:Lstd/core/Object;",
        reinterpret_cast<void *>(AniWindowStage::NativeTransferStatic)},
    ani_native_function {"nativeTransferDynamic", "J:Lstd/interop/ESValue;",
        reinterpret_cast<void *>(AniWindowStage::NativeTransferDynamic)},
    ani_native_function {"setImageForRecentSync", "lX{C{std.core.Long}C{@ohos.multimedia.image.image.PixelMap}}i:",
        reinterpret_cast<void *>(AniWindowStage::SetImageForRecent)},
    ani_native_function {"removeImageForRecentSync", "l:",
        reinterpret_cast<void *>(AniWindowStage::RemoveImageForRecent)},
    ani_native_function {"setCustomDensitySync", "ldz:",
        reinterpret_cast<void *>(AniWindowStage::SetCustomDensity)},
    ani_native_function {"setDefaultDensityEnabledSync", "lz:",
        reinterpret_cast<void *>(AniWindowStage::SetDefaultDensityEnabled)},
};

std::array g_functions = {
    ani_native_function {"CreateWindowStage", "J:L@ohos/window/window/WindowStageInternal;",
        reinterpret_cast<void *>(WindowStageCreate)},
    ani_native_function {"getLastWindowSync", nullptr, reinterpret_cast<void *>(AniWindowManager::GetLastWindow)},
    ani_native_function {"shiftAppWindowFocusSync", "JII:V",
        reinterpret_cast<void *>(AniWindowManager::ShiftAppWindowFocus)},
    ani_native_function {"getAllMainWindowInfo", "J:Lescompat/Array;",
        reinterpret_cast<void *>(AniWindowManager::GetAllMainWindowInfo)},
    ani_native_function {"getMainWindowSnapshot",
        "JLescompat/Array;L@ohos/window/window/WindowSnapshotConfiguration;:Lescompat/Array;",
        reinterpret_cast<void *>(AniWindowManager::GetMainWindowSnapshot)},
    ani_native_function {"createWindowSync",
        "lC{@ohos.window.window.Configuration}:C{@ohos.window.window.Window}",
        reinterpret_cast<void *>(AniWindowManager::CreateWindow)},
    ani_native_function {"findWindowSync",
        "JLstd/core/String;:L@ohos/window/window/Window;",
        reinterpret_cast<void *>(AniWindowManager::FindWindow)},
    ani_native_function {"minimizeAllSync", "JJ:V", reinterpret_cast<void *>(AniWindowManager::MinimizeAll)},
    ani_native_function {"destroyAniExtConfig", nullptr, reinterpret_cast<void *>(AniExtensionWindowConfig::Finalizer)},
};

ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    using namespace OHOS::Rosen;
    ani_status ret;
    ani_env* env;
    if ((ret = vm->GetEnv(ANI_VERSION_1, &env)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] null env");
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    if ((ret = env->FindClass("L@ohos/window/window/WindowStageInternal;", &cls)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] can't find class %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    for (auto method : g_methods) {
        if ((ret = env->Class_BindNativeMethods(cls, &method, 1u)) != ANI_OK) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI] bind window static method fail %{public}u, %{public}s, %{public}s",
                ret, method.name, method.signature);
            return ANI_NOT_FOUND;
        }
    }
    *result = ANI_VERSION_1;

    // just for test
    ani_namespace ns;
    if ((ret = env->FindNamespace("L@ohos/window/window;", &ns)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] find ns %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    for (auto method : g_functions) {
        if ((ret = env->Namespace_BindNativeFunctions(ns, &method, 1u)) != ANI_OK) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI] bind window static method fail %{public}u, %{public}s, %{public}s",
                ret, method.name, method.signature);
            return ANI_NOT_FOUND;
        }
    }
    AniWindowManager::AniWindowManagerInit(env, ns);

    OHOS::Rosen::ANI_Window_Constructor(vm, result);
    ExtensionWindowConfig_ANI_Constructor(vm, result);
    return ANI_OK;
}
}