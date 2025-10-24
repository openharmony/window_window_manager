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
#include <memory>

#include "ani.h"
#include "ani_window.h"
#include "ani_window_manager.h"
#include "ani_window_stage.h"
#include "ani_window_utils.h"
#include "ani_transition_controller.h"
#include "permission.h"
#include "window_manager_hilog.h"
#include "window_scene.h"

using namespace OHOS::Rosen;


static void SetWindowRectAutoSave(ani_env* env, ani_object obj, ani_long nativeObj,
                                  ani_boolean enabled, ani_boolean isSaveBySpecifiedFlag)
{
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "[ANI] start");
    AniWindowStage* windowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (windowStage == nullptr || windowStage->GetWindowScene().lock() == nullptr) {
        TLOGD(WmsLogTag::WMS_LAYOUT_PC, "[ANI] windowStage is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    windowStage->SetWindowRectAutoSave(env, enabled, isSaveBySpecifiedFlag);
}

static ani_boolean IsWindowRectAutoSave(ani_env* env, ani_object obj, ani_long nativeObj)
{
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "[ANI] start");
    AniWindowStage* windowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (windowStage == nullptr || windowStage->GetWindowScene().lock() == nullptr) {
        TLOGD(WmsLogTag::WMS_LAYOUT_PC, "[ANI] windowStage is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return ani_boolean(false);
    }
    return windowStage->IsWindowRectAutoSave(env);
}

static void RemoveStartingWindow(ani_env* env, ani_object obj, ani_long nativeObj)
{
    TLOGD(WmsLogTag::WMS_STARTUP_PAGE, "[ANI] start");
    AniWindowStage* windowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (windowStage == nullptr || windowStage->GetWindowScene().lock() == nullptr) {
        TLOGD(WmsLogTag::WMS_STARTUP_PAGE, "[ANI] windowStage is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        return;
    }
    windowStage->RemoveStartingWindow(env);
}

static ani_ref WindowGetMainWindow(ani_env* env, ani_object obj, ani_long nativeObj)
{
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
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindowStage* windowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (windowStage == nullptr || windowStage->GetWindowScene().lock() == nullptr) {
        TLOGD(WmsLogTag::DEFAULT, "[ANI] windowStage is nullptr");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    return windowStage->OnCreateSubWindow(env, name);
}

extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    ani_status ret = ANI_OK;
    ani_env* env = nullptr;
    if ((ret = vm->GetEnv(ANI_VERSION_1, &env)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] null env");
        return ANI_NOT_FOUND;
    }
    ani_class cls = nullptr;
    if ((ret = env->FindClass("@ohos.window.window.WindowStageInternal", &cls)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] can't find class %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    std::array methods = {
        ani_native_function {"setWindowRectAutoSave", "lzz:",
            reinterpret_cast<void *>(SetWindowRectAutoSave)},
        ani_native_function {"isWindowRectAutoSave", "l:z",
            reinterpret_cast<void *>(IsWindowRectAutoSave)},
        ani_native_function {"removeStartingWindow", "l:",
            reinterpret_cast<void *>(RemoveStartingWindow)},
        ani_native_function {"loadContentSync",
            "lC{std.core.String}C{arkui.stateManagement.storage.localStorage.LocalStorage}:",
            reinterpret_cast<void *>(AniWindowStage::LoadContent)},
        ani_native_function {"disableWindowDecorSync", nullptr,
            reinterpret_cast<void *>(AniWindowStage::DisableWindowDecor)},
        ani_native_function {"setShowOnLockScreenSync",
            nullptr, reinterpret_cast<void *>(AniWindowStage::SetShowOnLockScreen)},
        ani_native_function {"setWindowModal", "lz:",
            reinterpret_cast<void *>(AniWindowStage::SetWindowModal)},
        ani_native_function {"setCustomDensitySync", "ld:",
            reinterpret_cast<void *>(AniWindowStage::SetCustomDensity)},
        ani_native_function {"setDefaultDensityEnabledSync", "lz:",
            reinterpret_cast<void *>(AniWindowStage::SetDefaultDensityEnabled)},
        ani_native_function {"getMainWindowSync", "l:C{@ohos.window.window.Window}",
            reinterpret_cast<void *>(WindowGetMainWindow)},
        ani_native_function {"setImageForRecentSync", "lli:",
            reinterpret_cast<void *>(AniWindowStage::SetImageForRecent)},
        ani_native_function {"createSubWindowSync", "lC{std.core.String}:C{@ohos.window.window.Window}",
            reinterpret_cast<void *>(CreateSubWindow)},
        ani_native_function {"getSubWindowSync", "l:C{std.core.Array}",
            reinterpret_cast<void *>(AniWindowStage::GetSubWindow)},
        ani_native_function {"createSubWindowWithOptionsSync",
            "lC{std.core.String}C{@ohos.window.window.SubWindowOptions}:C{@ohos.window.window.Window}",
            reinterpret_cast<void *>(AniWindowStage::CreateSubWindowWithOptions)},
        ani_native_function {"onSync", nullptr,
            reinterpret_cast<void *>(AniWindowStage::RegisterWindowCallback)},
        ani_native_function {"offSync", nullptr,
            reinterpret_cast<void *>(AniWindowStage::UnregisterWindowCallback)},
    };
    for (auto method : methods) {
        if ((ret = env->Class_BindNativeMethods(cls, &method, 1u)) != ANI_OK) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI] bind window method fail %{public}u, %{public}s, %{public}s",
                ret, method.name, method.signature);
            return ANI_NOT_FOUND;
        }
    }
    std::array staticMethods = {
        ani_native_function {"nativeTransferStatic", "C{std.interop.ESValue}:C{std.core.Object}",
            reinterpret_cast<void *>(AniWindowStage::NativeTransferStatic)},
        ani_native_function {"nativeTransferDynamic", "l:C{std.interop.ESValue}",
            reinterpret_cast<void *>(AniWindowStage::NativeTransferDynamic)},
    };
    for (auto method : staticMethods) {
        if ((ret = env->Class_BindStaticNativeMethods(cls, &method, 1u)) != ANI_OK) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI] bind window static method fail %{public}u, %{public}s, %{public}s",
                ret, method.name, method.signature);
            return ANI_NOT_FOUND;
        }
    }
    *result = ANI_VERSION_1;
    AniWindowManager::AniWindowManagerInit(env);
    OHOS::Rosen::ANI_Transition_Controller_Constructor(vm, result);
    OHOS::Rosen::ANI_Window_Constructor(vm, result);
    return ANI_OK;
}
}