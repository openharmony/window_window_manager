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
#include "window_manager_hilog.h"
#include "window_scene.h"

using OHOS::Rosen::WindowScene;

namespace OHOS {
namespace Rosen {

namespace {
/* used for free, ani has no destructor right now, only free when aniObj freed */
static std::map<ani_object, AniWindowStage*> g_localObjs;
} // namespace

AniWindowStage::AniWindowStage(const std::shared_ptr<Rosen::WindowScene>& windowScene)
    : windowScene_(windowScene), registerManager_(std::make_unique<AniWindowRegisterManager>())
{
}
AniWindowStage::~AniWindowStage()
{
    TLOGE(WmsLogTag::DEFAULT, "[ANI] Ani WindowStage died");
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
std::array methods = {
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
};

std::array functions = {
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
    if ((ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size())) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] bind fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    *result = ANI_VERSION_1;

    // just for test
    ani_namespace ns;
    if ((ret = env->FindNamespace("L@ohos/window/window;", &ns)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] find ns %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    if ((ret = env->Namespace_BindNativeFunctions(ns, functions.data(), functions.size())) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] bind ns func %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    AniWindowManager::AniWindowManagerInit(env, ns);

    OHOS::Rosen::ANI_Window_Constructor(vm, result);
    return ANI_OK;
}
}