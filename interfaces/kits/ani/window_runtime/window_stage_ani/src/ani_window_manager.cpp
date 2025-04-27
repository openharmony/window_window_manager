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

#include "ani_window_manager.h"

#include <ability.h>

#include "ani.h"
#include "window_scene.h"
#include "window_manager_hilog.h"
#include "ani_window_utils.h"
#include "ani_window.h"

namespace OHOS {
namespace Rosen {
AniWindowManager::AniWindowManager() : registerManager_(std::make_unique<AniWindowRegisterManager>())
{
}

ani_status AniWindowManager::AniWindowManagerInit(ani_env* env)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    ani_namespace ns;
    ani_status ret;
    if ((ret = env->FindNamespace("L@ohos/window/window;", &ns)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] find ns %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    std::array functions = {
        ani_native_function {"CreateWindowStage", "J:L@ohos/window/window/WindowStageInternal;",
            reinterpret_cast<void *>(WindowStageCreate)},
        ani_native_function {"getLastWindowSync",
            "JLapplication/BaseContext/BaseContext;:L@ohos/window/window/Window;",
            reinterpret_cast<void *>(AniWindowManager::GetLastWindow)},
        ani_native_function {"onSync", nullptr,
            reinterpret_cast<void *>(AniWindowManager::RegisterWindowManagerCallback)},
        ani_native_function {"offSync", nullptr,
            reinterpret_cast<void *>(AniWindowManager::UnregisterWindowManagerCallback)},
    };
    if ((ret = env->Namespace_BindNativeFunctions(ns, functions.data(), functions.size())) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] bind ns func %{public}u", ret);
        return ANI_NOT_FOUND;
    }

    ani_function setObjFunc = nullptr;
    ret = env->Namespace_FindFunction(ns, "setNativeObj", "J:V", &setObjFunc);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] find setNativeObj func fail %{public}u", ret);
        return ret;
    }
    std::unique_ptr<AniWindowManager> aniWinManager = std::make_unique<AniWindowManager>();
    ret = env->Function_Call_Void(setObjFunc, aniWinManager.release());
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] find setNativeObj func fail %{public}u", ret);
        return ret;
    }
    return ret;
}

ani_object AniWindowManager::GetLastWindow(ani_env* env, ani_long nativeObj, ani_object context)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindowManager* aniWindowManager = reinterpret_cast<AniWindowManager*>(nativeObj);
    return aniWindowManager != nullptr ? aniWindowManager->OnGetLastWindow(env, context) : nullptr;
}

ani_object AniWindowManager::OnGetLastWindow(ani_env* env, ani_object aniContext)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    auto contextPtr = AniWindowUtils::GetAbilityContext(env, aniContext);
    TLOGI(WmsLogTag::DEFAULT, "[ANI] nativeContextLong : %{public}p", contextPtr);
    auto context = static_cast<std::weak_ptr<AbilityRuntime::Context>*>(contextPtr);
    if (context == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] context is nullptr");
        return AniWindowUtils::AniThrowError(env, WMError::WM_ERROR_NULLPTR, "Stage mode without context");
    }
    auto window = Window::GetTopWindowWithContext(context->lock());
    if (window == nullptr || window->GetWindowState() == WindowState::STATE_DESTROYED) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] window is nullptr or destroyed");
        return AniWindowUtils::AniThrowError(env, WMError::WM_ERROR_NULLPTR, "Get top window failed");
    }
    return CreateAniWindowObject(env, window);
}

void AniWindowManager::RegisterWindowManagerCallback(ani_env* env, ani_long nativeObj,
    ani_string type, ani_ref callback)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindowManager* aniWindowManager = reinterpret_cast<AniWindowManager*>(nativeObj);
    if (aniWindowManager != nullptr) {
        aniWindowManager->OnRegisterWindowManagerCallback(env, type, callback);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindowManager is nullptr");
    }
}

void AniWindowManager::OnRegisterWindowManagerCallback(ani_env* env, ani_string type, ani_ref callback)
{
    std::string cbType;
    AniWindowUtils::GetStdString(env, type, cbType);
    TLOGI(WmsLogTag::DEFAULT, "[ANI] type:%{public}s", cbType.c_str());
    WmErrorCode ret = registerManager_->RegisterListener(nullptr, cbType, CaseType::CASE_WINDOW_MANAGER,
        env, callback);
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret);
    }
}

void AniWindowManager::UnregisterWindowManagerCallback(ani_env* env, ani_long nativeObj,
    ani_string type, ani_ref callback)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindowManager* aniWindowManager = reinterpret_cast<AniWindowManager*>(nativeObj);
    if (aniWindowManager != nullptr) {
        aniWindowManager->OnUnregisterWindowManagerCallback(env, type, callback);
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] aniWindowManager is nullptr");
    }
}

void AniWindowManager::OnUnregisterWindowManagerCallback(ani_env* env, ani_string type, ani_ref callback)
{
    std::string cbType;
    AniWindowUtils::GetStdString(env, type, cbType);
    TLOGI(WmsLogTag::DEFAULT, "[ANI] type:%{public}s", cbType.c_str());
    WmErrorCode ret = registerManager_->UnregisterListener(nullptr, cbType, CaseType::CASE_WINDOW_MANAGER,
        env, callback);
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret);
    }
}
}  // namespace Rosen
}  // namespace OHOS