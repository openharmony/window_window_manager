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
#include "window_manager_hilog.h"
#include "permission.h"
#include "window_scene.h"

using OHOS::Rosen::WindowScene;

namespace OHOS {
namespace Rosen {

namespace {
/* used for free, ani has no destructor right now, only free when aniObj freed */
static std::map<ani_object, AniWindowStage*> localObjs;
} // namespace

AniWindowStage::AniWindowStage(const std::shared_ptr<Rosen::WindowScene>& windowScene)
    : windowScene_(windowScene)
{
}
AniWindowStage::~AniWindowStage()
{
    TLOGE(WmsLogTag::DEFAULT, "Ani WindowStage died");
}

void AniWindowStage::LoadContent(ani_env* env, const std::string& content)
{
    auto weakScene = windowScene_.lock();
    sptr<Window> win = weakScene ? weakScene->GetMainWindow() : nullptr;
    if (win == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[NAPI]Get window failed");
        return;
    }
    win->NapiSetUIContent(content, env, nullptr);
}

ani_object AniWindowStage::GetMainWindow(ani_env* env)
{
    std::shared_ptr<WindowScene> weakScene = windowScene_.lock();
    ani_object aniMainWindow = CreateAniWindowStage(env, weakScene);
    return aniMainWindow;
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

static void GetStdString(ani_env* env, ani_string str, std::string& result)
{
    ani_size sz {};
    env->String_GetUTF8Size(str, &sz);
    result.resize(sz + 1);
    env->String_GetUTF8SubString(str, 0, sz, result.data(), result.size(), &sz);
    result.resize(sz);
}

ani_object CreateAniWindowStage(ani_env* env, std::shared_ptr<Rosen::WindowScene>& windowScene)
__attribute__((no_sanitize("cfi")))
{
    if (env == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]null env");
        return nullptr;
    }
    TLOGD(WmsLogTag::DEFAULT, "[ANI]create wstage");

    ani_status ret;
    ani_class cls = nullptr;
    if ((ret = env->FindClass("L@ohos/window/window/WindowStageInternal;", &cls)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]null env %{public}u", ret);
        return cls;
    }

    std::unique_ptr<AniWindowStage> windowStage = std::make_unique<AniWindowStage>(windowScene);
    TLOGD(WmsLogTag::DEFAULT, "[ANI]native obj %{public}p", windowStage.get());

    ani_field contextField;
    if ((ret = env->Class_FindField(cls, "nativeObj", &contextField)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]get field fail %{public}u", ret);
        return nullptr;
    }

    ani_method initFunc = nullptr;
    if ((ret = env->Class_FindMethod(cls, "<ctor>", ":V", &initFunc)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]get ctor fail %{public}u", ret);
        return nullptr;
    }
    ani_object obj = nullptr;
    if ((ret = env->Object_New(cls, initFunc, &obj)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]obj new fail %{public}u", ret);
        return nullptr;
    }
    ani_method setObjFunc = nullptr;
    if ((ret = env->Class_FindMethod(cls, "setNativeObj", "J:V", &setObjFunc)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]get ctor fail %{public}u", ret);
        return nullptr;
    }
    env->Object_CallMethod_Void(obj, setObjFunc, reinterpret_cast<ani_long>(windowStage.get()));
    localObjs.insert(std::pair(obj, windowStage.release()));

    TLOGD(WmsLogTag::DEFAULT, "[ANI]window stage created  %{public}p", reinterpret_cast<void*>(obj));
    return obj;
}

AniWindowStage* GetWindowStageFromEnv(ani_env* env, ani_class cls, ani_object obj)
{
    ani_field nativeObjName {};
    ani_status ret;
    if ((ret = env->Class_FindField(cls, "nativeObj", &nativeObjName)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]obj fetch field %{public}u", ret);
        return nullptr;
    }
    ani_long nativeObj {};
    if ((ret = env->Object_GetField_Long(obj, nativeObjName, &nativeObj)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]obj fetch long %{public}u", ret);
        return nullptr;
    }
    return reinterpret_cast<AniWindowStage*>(nativeObj);
}
}  // namespace Rosen
}  // namespace OHOS


static ani_int WindowStageLoadContent(ani_env* env, ani_object obj,
    ani_long nativeObj, ani_string content)
{
    using namespace OHOS::Rosen;
    AniWindowStage* windowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    std::string contentStr;
    GetStdString(env, content, contentStr);
    TLOGD(WmsLogTag::DEFAULT, "[ANI]loadcontent 0x%{public}p: %{public}s", windowStage, contentStr.c_str());
    windowStage->LoadContent(env, contentStr);
    return (ani_int)0u;
}

static ani_int DisableWindowDecor(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    AniWindowStage* windowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    auto windowScene = windowStage->GetWindowScene().lock();
    if (windowScene == nullptr) {
        return static_cast<ani_int>(WMError::WM_DO_NOTHING);
    }
    auto mainWindow = windowScene->GetMainWindow();
    if (mainWindow == nullptr) {
        return static_cast<ani_int>(WMError::WM_DO_NOTHING);
    }
    return static_cast<ani_int>(mainWindow->DisableAppWindowDecor());
}

static ani_int SetShowOnLockScreen(ani_env* env, ani_class cls, ani_long nativeObj,
    ani_boolean showOnLockScreen)
{
    using namespace OHOS::Rosen;
    if (!Permission::IsSystemCalling() && !Permission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DEFAULT, "set show on lock screen permission denied!");
        return static_cast<ani_int>(WMError::WM_ERROR_NOT_SYSTEM_APP);
    }
    AniWindowStage* windowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    auto windowScene = windowStage->GetWindowScene().lock();
    if (windowScene == nullptr) {
        return static_cast<ani_int>(WMError::WM_DO_NOTHING);
    }
    auto mainWindow = windowScene->GetMainWindow();
    if (mainWindow == nullptr) {
        return static_cast<ani_int>(WMError::WM_DO_NOTHING);
    }
    WMError ret;
    if (showOnLockScreen) {
        ret = mainWindow->AddWindowFlag(OHOS::Rosen::WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED);
    } else {
        ret = mainWindow->RemoveWindowFlag(OHOS::Rosen::WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED);
    }
    return static_cast<ani_int>(ret);
}

static ani_object WindowStageCreate(ani_env* env, ani_long scene)
{
    using namespace OHOS::Rosen;
    TLOGD(WmsLogTag::DEFAULT, "[ANI]create windowstage with scene 0x%{public}p %{public}d",
        reinterpret_cast<void*>(env), (int32_t)scene);
    std::shared_ptr<WindowScene> scenePtr;
    return CreateAniWindowStage(env, scenePtr); // just for test
}

static ani_object WindowGetMainWindow(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    TLOGD(WmsLogTag::DEFAULT, "[ANI]GetMainWindow");
    AniWindowStage* windowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (windowStage == nullptr) {
        TLOGD(WmsLogTag::DEFAULT, "[ANI]windowStage is nullptr");
    }
    return windowStage->GetMainWindow(env);
}

extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    using namespace OHOS::Rosen;
    ani_status ret;
    ani_env* env;
    if ((ret = vm->GetEnv(ANI_VERSION_1, &env)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]null env");
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    if ((ret = env->FindClass("L@ohos/window/window/WindowStageInternal;", &cls)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]can't find class %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    std::array methods = {
        ani_native_function {"loadContent", "JLstd/core/String;:I", reinterpret_cast<void *>(WindowStageLoadContent)},
        ani_native_function {"disableWindowDecor", "J:I", reinterpret_cast<void *>(DisableWindowDecor)},
        ani_native_function {"setShowOnLockScreen", "JZ:I", reinterpret_cast<void *>(SetShowOnLockScreen)},
        ani_native_function {"getMainWindowSync", "J:L@ohos/window/window/Window;",
            reinterpret_cast<void *>(WindowGetMainWindow)},
    };
    if ((ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size())) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]bind fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    *result = ANI_VERSION_1;

    // just for test
    ani_namespace ns;
    if ((ret = env->FindNamespace("L@ohos/window/window;", &ns)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]find ns %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    std::array functions = {
        ani_native_function {"CreateWindowStage", "J:L@ohos/window/window/WindowStageInternal;",
            reinterpret_cast<void *>(WindowStageCreate)},
    };
    if ((ret = env->Namespace_BindNativeFunctions(ns, functions.data(), functions.size())) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]bind ns func %{public}u", ret);
        return ANI_NOT_FOUND;
    }

    OHOS::Rosen::ANI_Window_Constructor(vm, result);
    return ANI_OK;
}
}
