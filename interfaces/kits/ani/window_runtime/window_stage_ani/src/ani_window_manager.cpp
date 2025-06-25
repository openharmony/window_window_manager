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
#include "ani_window.h"
#include "ani_window_stage.h"
#include "ani_window.h"
#include "ani_window_stage.h"
#include "ani_window_utils.h"
#include "singleton_container.h"
#include "window_manager.h"
#include "window_manager_hilog.h"
#include "window_scene.h"
#include "ani_window_utils.h"

namespace OHOS {
namespace Rosen {
using DisplayId = uint64_t;
namespace {
const std::string PIP_WINDOW = "pip_window";
constexpr int32_t INVALID_COORDINATE = -1;
}
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
            reinterpret_cast<void *>(AniWindowManager::WindowStageCreate)},
        ani_native_function {"GetWindowsByCoordinate",
            "JL@ohos/window/window/GetWindowsByCoordinateParam;:Lescompat/Array;",
            reinterpret_cast<void *>(AniWindowManager::GetWindowsByCoordinate)},
        ani_native_function {"getLastWindowSync",
            "JLapplication/BaseContext/BaseContext;:L@ohos/window/window/Window;",
            reinterpret_cast<void *>(AniWindowManager::GetLastWindow)},
        ani_native_function {"findWindowSync",
            "JLstd/core/String;:L@ohos/window/window/Window;",
            reinterpret_cast<void *>(AniWindowManager::FindWindow)},
        ani_native_function {"minimizeAllSync", "JD:V", reinterpret_cast<void *>(AniWindowManager::MinimizeAll)},
        ani_native_function {"shiftAppWindowFocusSync", "JDD:V",
            reinterpret_cast<void *>(AniWindowManager::ShiftAppWindowFocus)},
        ani_native_function {"onSync", nullptr,
            reinterpret_cast<void *>(AniWindowManager::RegisterWindowManagerCallback)},
        ani_native_function {"offSync", nullptr,
            reinterpret_cast<void *>(AniWindowManager::UnregisterWindowManagerCallback)},
        ani_native_function {"windowDestroyCallback", nullptr, reinterpret_cast<void *>(AniWindow::Finalizer)},
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
        TLOGE(WmsLogTag::DEFAULT, "[ANI] call setNativeObj func fail %{public}u", ret);
        return ret;
    }
    return ret;
}

ani_object AniWindowManager::WindowStageCreate(ani_env* env, ani_long scene)
{
    TLOGD(WmsLogTag::DEFAULT, "[ANI] create windowstage with scene 0x%{public}p %{public}d",
        reinterpret_cast<void*>(env), (int32_t)scene);
    std::shared_ptr<WindowScene> scenePtr;
    return CreateAniWindowStage(env, scenePtr);
}

ani_object AniWindowManager::GetWindowsByCoordinate(ani_env* env, ani_long nativeObj, ani_object getWindowsParam)
{
    TLOGI(WmsLogTag::WMS_PC, "[ANI]");
    AniWindowManager* aniWindowManager = reinterpret_cast<AniWindowManager*>(nativeObj);
    return aniWindowManager != nullptr ? aniWindowManager->OnGetWindowsByCoordinate(env, getWindowsParam) : nullptr;
}

ani_ref AniWindowManager::OnGetLastWindow(ani_env* env, ani_object getWindowsParam)
{
    int64_t displayId = static_cast<int64_t>(DISPLAY_ID_INVALID);
    ani_double aniDisplayId;
    if (ANI_OK != env->Object_GetPropertyByName_Double(getWindowsParam, "displayId", &aniDisplayId)) {
        TLOGE(WmsLogTag::WMS_PC, "[ANI] Failed to convert parameter to displayId");
        return AniWindowsUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    displayId = static_cast<int64_t>(aniDisplayId);
    if (displayId < 0 ||
        SingletonContainer::Get<DisplayManager>().GetDisplayById(displayId) == nullptr) {
        TLOGE(WmsLogTag::WMS_PC, "[ANI] invalid displayId");
        return AniWindowsUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    int32_t windowNumber = 0;
    ani_double aniWindowNumber;
    if (ANI_OK != env->Object_GetPropertyByName_Double(getWindowsParam, "windowNumber", &aniWindowNumber)) {
        windowNumber = static_cast<int32_t>(aniWindowNumber);
    }
    int32_t x = INVALID_COORDINATE;
    ani_double aniX;
    if (ANI_OK != env->Object_GetPropertyByName_Double(getWindowsParam, "x", &aniX)) {
        x = static_cast<int32_t>(aniX);
    }
    int32_t y = INVALID_COORDINATE;
    ani_double aniY;
    if (ANI_OK != env->Object_GetPropertyByName_Double(getWindowsParam, "y", &aniY)) {
        y = static_cast<int32_t>(aniY);
    }
    std::vector<int32_t> windowIds;
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(SingletonContainer::Get<WindowManager>().
        GetWindowIdsByCoordinate(displayId, windowNumber, x, y, windowIds));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_PC, "[ANI] getWindowsByCoordinate failed");
        return AniWindowsUtils::AniThrowError(env, ret);
    }
    std::vector<ani_ref> windows(windowIds.size());
    for (size_t i = 0; i < windowIds.size(); i++) {
        sptr<Window> window = Window::GetWindowWithId(windowIds[i]);
        windows[i] = CreateAniWindowObject(env, window);
    }
    return AniWindowsUtils::CreateAniWindowArray(env, windows);
}

ani_ref AniWindowManager::GetLastWindow(ani_env* env, ani_long nativeObj, ani_object context)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindowManager* aniWindowManager = reinterpret_cast<AniWindowManager*>(nativeObj);
    return aniWindowManager != nullptr ? aniWindowManager->OnGetLastWindow(env, context) : nullptr;
}

ani_ref AniWindowManager::OnGetLastWindow(ani_env* env, ani_object aniContext)
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

ani_ref AniWindowManager::FindWindow(ani_env* env, ani_long nativeObj, ani_string windowName)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindowManager* aniWindowManager = reinterpret_cast<AniWindowManager*>(nativeObj);
    return aniWindowManager != nullptr ? aniWindowManager->OnFindWindow(env, windowName) : nullptr;
}

ani_ref AniWindowManager::OnFindWindow(ani_env* env, ani_string windowName)
{
    std::string name;
    AniWindowUtils::GetStdString(env, windowName, name);
    TLOGI(WmsLogTag::DEFAULT, "[ANI]Window name=%{public}s", name.c_str());
    if (name.compare(PIP_WINDOW) == 0) {
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    ani_ref aniWindowObj = FindAniWindowObject(name);
    if (aniWindowObj != nullptr) {
        TLOGD(WmsLogTag::DEFAULT, "[ANI]Find window: %{public}s, use exist js window", name.c_str());
        return aniWindowObj;
    } else {
        sptr<Window> window = Window::Find(name);
        if (window == nullptr) {
            return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
        } else {
            return CreateAniWindowObject(env, window);
        }
    }
}

void AniWindowManager::MinimizeAll(ani_env* env, ani_long nativeObj, ani_double displayId)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[ANI]");
    AniWindowManager* aniWindowManager = reinterpret_cast<AniWindowManager*>(nativeObj);
    if (aniWindowManager != nullptr) {
        aniWindowManager->OnMinimizeAll(env, displayId);
    } else {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] aniWindowManager is nullptr");
    }
}

void AniWindowManager::OnMinimizeAll(ani_env* env, ani_double displayId)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[ANI]");
    if (static_cast<uint64_t>(displayId) < 0 ||
        SingletonContainer::Get<DisplayManager>().GetDisplayById(static_cast<uint64_t>(displayId)) == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] Minimize all failed, Invalidate params.");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
        SingletonContainer::Get<WindowManager>().MinimizeAllAppWindows(static_cast<uint64_t>(displayId)));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] Minimize all failed, ret:%{public}d", static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, ret, "OnMinimizeAll failed");
        return;
    }
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
        env, callback, ani_double(0));
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

void AniWindowManager::ShiftAppWindowFocus(ani_env* env, ani_object obj, ani_long nativeObj,
    ani_double sourceWindowId, ani_double targetWindowId)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindowManager* aniWindowManager = reinterpret_cast<AniWindowManager*>(nativeObj);
    aniWindowManager->OnShiftAppWindowFocus(env, sourceWindowId, targetWindowId);
}

void AniWindowManager::OnShiftAppWindowFocus(ani_env* env, ani_double sourceWindowId, ani_double targetWindowId)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
        SingletonContainer::Get<WindowManager>().ShiftAppWindowFocus(sourceWindowId, targetWindowId));
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "ShiftAppWindowFocus failed.");
    }
    return ;
}
}  // namespace Rosen
}  // namespace OHOS