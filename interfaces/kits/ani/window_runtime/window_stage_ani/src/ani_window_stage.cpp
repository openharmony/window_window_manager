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
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "js_window_stage.h"
#include "permission.h"
#include "window_manager_hilog.h"
#include "window_scene.h"

using OHOS::Rosen::WindowScene;

namespace OHOS {
namespace Rosen {
// methods of filling the image in recent
enum class ImageFit {
    FILL,
    CONTAIN,
    COVER,
    FIT_WIDTH,
    FIT_HEIGHT,
    NONE,
    SCALE_DOWN,
    TOP_LEFT,
    TOP,
    TOP_RIGHT,
    LEFT,
    CENTER,
    RIGHT,
    BOTTOM_LEFT,
    BOTTOM,
    BOTTOM_RIGHT,
    MATRIX,
};
namespace {
/* used for free, ani has no destructor right now, only free when aniObj freed */
static std::map<ani_object, AniWindowStage*> localObjs;
constexpr int64_t MIN_RESOURCE_ID = 0x1000000;
constexpr int64_t MAX_RESOURCE_ID = 0xffffffff;
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
        return AniWindowUtils::CreateAniUndefined(aniEnv);
    }
    if (unwrapResult == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] unwrapResult is nullptr");
        return AniWindowUtils::CreateAniUndefined(aniEnv);
    }
    JsWindowStage* jsWindowStage = static_cast<JsWindowStage*>(unwrapResult);
    if (jsWindowStage == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] jsWindowStage is nullptr");
        return AniWindowUtils::CreateAniUndefined(aniEnv);
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
        return AniWindowUtils::CreateAniUndefined(aniEnv);
    }
    napi_env napiEnv {};
    if (!arkts_napi_scope_open(aniEnv, &napiEnv)) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] napi scope open fail");
        return AniWindowUtils::CreateAniUndefined(aniEnv);
    }
    napi_value jsWindowStage = CreateJsWindowStage(napiEnv, aniWindowStage->GetWindowScene().lock());
    hybridgref ref {};
    if (!hybridgref_create_from_napi(napiEnv, jsWindowStage, &ref)) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] create hybridgref fail");
        return AniWindowUtils::CreateAniUndefined(aniEnv);
    }
    ani_object result {};
    if (!hybridgref_get_esvalue(aniEnv, ref, &result)) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] get esvalue fail");
        return AniWindowUtils::CreateAniUndefined(aniEnv);
    }
    if (!hybridgref_delete_from_napi(napiEnv, ref)) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] delete hybridgref fail");
    }
    if (!arkts_napi_scope_close_n(napiEnv, 0, nullptr, nullptr)) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] napi close scope fail");
        return AniWindowUtils::CreateAniUndefined(aniEnv);
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
    auto obj = localObjs.find(reinterpret_cast<ani_object>(aniObj));
    if (obj != localObjs.end()) {
        delete obj->second;
    }
    localObjs.erase(obj);
}

AniWindowStage* GetWindowStageFromAni(void* aniObj)
{
    auto obj = localObjs.find(reinterpret_cast<ani_object>(aniObj));
    if (obj == localObjs.end()) {
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
    if ((ret = env->FindClass("@ohos.window.window.WindowStageInternal", &cls)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] null env %{public}u", ret);
        return cls;
    }

    std::unique_ptr<AniWindowStage> windowStage = std::make_unique<AniWindowStage>(windowScene);
    TLOGD(WmsLogTag::DEFAULT, "[ANI] native obj %{public}p", windowStage.get());

    ani_method initFunc = nullptr;
    if ((ret = env->Class_FindMethod(cls, "<ctor>", ":", &initFunc)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] get ctor fail %{public}u", ret);
        return nullptr;
    }
    ani_object obj = nullptr;
    if ((ret = env->Object_New(cls, initFunc, &obj)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] obj new fail %{public}u", ret);
        return nullptr;
    }
    ani_method setObjFunc = nullptr;
    if ((ret = env->Class_FindMethod(cls, "setNativeObj", "l:", &setObjFunc)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] call setNativeObj fail %{public}u", ret);
        return nullptr;
    }
    env->Object_CallMethod_Void(obj, setObjFunc, reinterpret_cast<ani_long>(windowStage.get()));
    localObjs.insert(std::pair(obj, windowStage.release()));

    TLOGD(WmsLogTag::DEFAULT, "[ANI] window stage created  %{public}p", reinterpret_cast<void*>(obj));
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
        TLOGE(WmsLogTag::DEFAULT, "[ANI] mainWindow is nullptr!");
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

void AniWindowStage::SetWindowModal(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean isModal)
{
    TLOGI(WmsLogTag::WMS_HIERARCHY, "[ANI]");
    AniWindowStage* aniWindowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (aniWindowStage != nullptr) {
        aniWindowStage->OnSetWindowModal(env, isModal);
    } else {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "[ANI] aniWindowStage is nullptr");
    }
}

void AniWindowStage::OnSetWindowModal(ani_env* env, ani_boolean isModal)
{
    TLOGI(WmsLogTag::WMS_HIERARCHY, "[ANI]");
    auto windowScene = GetWindowScene().lock();
    if (windowScene == nullptr) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "[ANI]windowScene is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    auto window = windowScene->GetMainWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "[ANI] mainWindow is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    if (!window->IsPcOrPadFreeMultiWindowMode()) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "device not support");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_DEVICE_NOT_SUPPORT);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetWindowModal(isModal));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "failed, ret is %{public}d", ret);
        AniWindowUtils::AniThrowError(env, ret, "Set main window modal failed");
        return;
    }
    TLOGI(WmsLogTag::WMS_HIERARCHY, "id:%{public}u, name:%{public}s, isModal:%{public}d",
        window->GetWindowId(), window->GetWindowName().c_str(), isModal);
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

void AniWindowStage::SetWindowRectAutoSave(ani_env* env, ani_boolean enabled, ani_boolean isSaveBySpecifiedFlag)
{
    TLOGI(WmsLogTag::WMS_LAYOUT_PC, "[ANI]");
    auto windowScene = GetWindowScene().lock();
    if (windowScene == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] WindowScene is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    auto mainWindow = windowScene->GetMainWindow();
    if (mainWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] MainWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(mainWindow->SetWindowRectAutoSave(enabled, isSaveBySpecifiedFlag));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] Enable recover position failed!");
        AniWindowUtils::AniThrowError(env, ret);
    } else {
        TLOGI(WmsLogTag::WMS_MAIN, "[ANI] id %{public}d isSaveBySpecifiedFlag: %{public}d "
            "enable:%{public}d", mainWindow->GetWindowId(), isSaveBySpecifiedFlag, enabled);
    }
}

ani_boolean AniWindowStage::IsWindowRectAutoSave(ani_env* env)
{
    TLOGI(WmsLogTag::WMS_LAYOUT_PC, "[ANI]");
    auto windowScene = GetWindowScene().lock();
    if (windowScene == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] WindowScene is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return ani_boolean(false);
    }
    auto mainWindow = windowScene->GetMainWindow();
    if (mainWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] MainWindow is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return ani_boolean(false);
    }
    bool enabled = false;
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(mainWindow->IsWindowRectAutoSave(enabled));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "[ANI] Get the auto-save state of the window rect failed!");
        AniWindowUtils::AniThrowError(env, ret);
        return ani_boolean(false);
    } else {
        return ani_boolean(enabled);
    }
}

void AniWindowStage::RemoveStartingWindow(ani_env* env)
{
    TLOGI(WmsLogTag::WMS_STARTUP_PAGE, "[ANI]");
    auto windowScene = GetWindowScene().lock();
    if (windowScene == nullptr) {
        TLOGE(WmsLogTag::WMS_STARTUP_PAGE, "[ANI] windowScene is null");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    auto window = windowScene->GetMainWindow();
    if (window == nullptr) {
        TLOGE(WmsLogTag::WMS_STARTUP_PAGE, "[ANI] window is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->NotifyRemoveStartingWindow());
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_STARTUP_PAGE, "[ANI] Notify remove starting window failed");
        AniWindowUtils::AniThrowError(env, ret);
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
        TLOGI(WmsLogTag::DEFAULT, "[ANI] WindowScene is nullptr");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    sptr<Rosen::WindowOption> windowOption = new Rosen::WindowOption();
    windowOption->SetWindowType(Rosen::WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    windowOption->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    auto window = weakScene->CreateWindow(windowName, windowOption);
    if (window == nullptr) {
        return AniWindowUtils::CreateAniUndefined(env);
    }
    return CreateAniWindowObject(env, window);
}

void AniWindowStage::SetCustomDensity(ani_env* env, ani_object obj, ani_long nativeObj, ani_double density)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "[ANI]");
    AniWindowStage* aniWindowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (aniWindowStage != nullptr) {
        aniWindowStage->OnSetCustomDensity(env, density);
    } else {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "[ANI] aniWindowStage is nullptr");
        return;
    }
}

void AniWindowStage::OnSetCustomDensity(ani_env* env, ani_double density)
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
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetCustomDensity(static_cast<float>(density), false));
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
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "Window [%{public}u,%{public}s] enabled=%{public}u ret=%{public}d",
        window->GetWindowId(), window->GetWindowName().c_str(), enabled, ret);
}

void AniWindowStage::SetImageForRecent(ani_env* env, ani_object obj, ani_long nativeObj,
    ani_long imgResourceId, ani_int value)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindowStage* aniWindowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (aniWindowStage != nullptr) {
        aniWindowStage->OnSetImageForRecent(env, imgResourceId, value);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindowStage is nullptr");
    }
}

void AniWindowStage::OnSetImageForRecent(ani_env* env, ani_long imgResourceId, ani_int value)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto windowScene = GetWindowScene().lock();
    if (windowScene == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]windowScene is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    int64_t resourceId = static_cast<int64_t>(imgResourceId);
    if (resourceId < MIN_RESOURCE_ID || resourceId > MAX_RESOURCE_ID) {
        TLOGE(WmsLogTag::WMS_PATTERN, "imgRsourceId invalid: %{public}" PRId64, resourceId);
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_ILLEGAL_PARAM);
        return;
    }
    int32_t imageFit = static_cast<int32_t>(value);
    if (imageFit < static_cast<int32_t>(ImageFit::FILL) ||
        imageFit > static_cast<int32_t>(ImageFit::MATRIX)) {
        TLOGE(WmsLogTag::WMS_PATTERN, "imageFit invalid: %{public}d", imageFit);
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_ILLEGAL_PARAM);
        return;
    }
    auto mainWindow = windowScene->GetMainWindow();
    if (mainWindow == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] mainWindow is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(mainWindow->SetImageForRecent(imgResourceId,
        static_cast<OHOS::Rosen::ImageFit>(value)));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] Set image for recent failed.");
        AniWindowUtils::AniThrowError(env, ret);
    }
}
}  // namespace Rosen
}  // namespace OHOS

