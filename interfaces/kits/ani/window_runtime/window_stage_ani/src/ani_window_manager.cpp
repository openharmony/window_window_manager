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

#include "ability_context.h"
#include "ani.h"
#include "ani_window.h"
#include "ani_window_utils.h"
#include "permission.h"
#include "singleton_container.h"
#include "window_manager.h"
#include "window_manager_hilog.h"
#include "window_scene.h"
#include "window_helper.h"
#include "window_manager.h"
#include "window_option.h"
#include "permission.h"
#include "singleton_container.h"
#include "pixel_map.h"
#include "../../../../../../wm/include/get_snapshot_callback.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t MAIN_WINDOW_SNAPSGOT_TIMEOUT = 5000;
const std::string PIP_WINDOW = "pip_window";
}

AniWindowManager::AniWindowManager() : registerManager_(std::make_unique<AniWindowRegisterManager>())
{
}

ani_status AniWindowManager::AniWindowManagerInit(ani_env* env, ani_namespace windowNameSpace)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    ani_namespace ns;
    ani_status ret;
    if ((ret = env->FindNamespace("L@ohos/window/window;", &ns)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] find ns %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    std::array functions = {
        ani_native_function {"onSync", nullptr,
            reinterpret_cast<void *>(AniWindowManager::RegisterWindowManagerCallback)},
        ani_native_function {"offSync", nullptr,
            reinterpret_cast<void *>(AniWindowManager::UnregisterWindowManagerCallback)},
        ani_native_function {"setWindowLayoutMode", "JL@ohos/window/window/WindowLayoutMode;:V",
            reinterpret_cast<void *>(AniWindowManager::SetWindowLayoutMode)},
        ani_native_function {"shiftAppWindowTouchEventSync", "liii:",
            reinterpret_cast<void *>(AniWindowManager::ShiftAppWindowTouchEvent)},
    };
    if ((ret = env->Namespace_BindNativeFunctions(ns, functions.data(), functions.size())) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] bind ns func %{public}u", ret);
        return ANI_NOT_FOUND;
    }

    ani_function setObjFunc = nullptr;
    ret = env->Namespace_FindFunction(windowNameSpace, "setNativeObj", "J:V", &setObjFunc);
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

void AniWindowManager::ShiftAppWindowFocus(ani_env* env, ani_long nativeObj,
    ani_int sourceWindowId, ani_int targetWindowId)
{
    AniWindowManager* aniWindowManager = reinterpret_cast<AniWindowManager*>(nativeObj);
    if (aniWindowManager != nullptr) {
        aniWindowManager->OnShiftAppWindowFocus(env, sourceWindowId, targetWindowId);
    } else {
        TLOGE(WmsLogTag::WMS_FOCUS, "[ANI] aniWindowManager is nullptr");
    }
}

void AniWindowManager::OnShiftAppWindowFocus(ani_env* env, ani_int sourceWindowId, ani_int targetWindowId)
{
    TLOGI(WmsLogTag::WMS_FOCUS, "[ANI] sourceWindowId: %{public}d targetWindowId: %{public}d",
        static_cast<int32_t>(sourceWindowId), static_cast<int32_t>(targetWindowId));
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
        SingletonContainer::Get<WindowManager>().ShiftAppWindowFocus(sourceWindowId, targetWindowId));
    if (ret != WmErrorCode::WM_OK) {
        AniWindowUtils::AniThrowError(env, ret, "ShiftAppWindowFocus failed.");
    }
    return ;
}

ani_object AniWindowManager::GetAllMainWindowInfo(ani_env* env, ani_long nativeObj, ani_object context)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[ANI]");
    AniWindowManager* aniWindowManager = reinterpret_cast<AniWindowManager*>(nativeObj);
    return aniWindowManager != nullptr ? aniWindowManager->OnGetAllMainWindowInfo(env, context) : nullptr;
}

ani_object AniWindowManager::OnGetAllMainWindowInfo(ani_env* env, ani_object context)
{
    std::vector<sptr<MainWindowInfo>> infos;
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
        SingletonContainer::Get<WindowManager>().GetAllMainWindowInfo(infos));
    if (ret != WmErrorCode::WM_OK) {
        TLOGI(WmsLogTag::WMS_LIFE, "Get All MainWindowInfo failed, ret: %{public}d", static_cast<int32_t>(ret));
        return AniWindowUtils::AniThrowError(env, ret, "Window Get All Main Window failed");
    }
    return AniWindowUtils::CreateAniMainWindowInfoArray(env, infos);
}

ani_object AniWindowManager::GetMainWindowSnapshot(
    ani_env* env, ani_long nativeObj, ani_object windowId, ani_object config)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[ANI]");
    AniWindowManager* aniWindowManager = reinterpret_cast<AniWindowManager*>(nativeObj);
    return aniWindowManager != nullptr ?
        aniWindowManager->OnGetMainWindowSnapshot(env, windowId, config) : nullptr;
}

ani_object AniWindowManager::OnGetMainWindowSnapshot(
    ani_env* env, ani_object windowId, ani_object config)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[ANI]");
    sptr<GetSnapshotCallback> getSnapshotCallback = sptr<GetSnapshotCallback>::MakeSptr();
    auto pixelMaps = std::make_shared<std::vector<std::shared_ptr<Media::PixelMap>>>();
    std::shared_ptr<WMError> errCode = std::make_shared<WMError>(WMError::WM_OK);
    getSnapshotCallback->RegisterFunc([env, errCode, pixelMaps, getSnapshotCallback]
        (WMError errCodeResult, const std::vector<std::shared_ptr<Media::PixelMap>>& pixelMapResult) {
            TLOGI(WmsLogTag::WMS_LIFE, "getSnapshotCallback errCodeResult: %{public}d",
                static_cast<int32_t>(errCodeResult));
            if (errCodeResult != WMError::WM_OK) {
                *errCode = errCodeResult;
            }
            *pixelMaps = pixelMapResult;
            getSnapshotCallback->OnNotifyResult();
        });
    std::vector<int32_t> windowIdList;
    WindowSnapshotConfiguration windowSnapshotConfiguration;
    AniWindowUtils::GetIntVector(env, windowId, windowIdList);
    AniWindowUtils::GetWindowSnapshotConfiguration(env, config, windowSnapshotConfiguration);
    TLOGI(WmsLogTag::WMS_LIFE, "windowIdList size: %{public}d", static_cast<int32_t>(windowIdList.size()));
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(SingletonContainer::Get<WindowManager>().
        GetMainWindowSnapshot(windowIdList, windowSnapshotConfiguration, getSnapshotCallback->AsObject()));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LIFE, "Get snapshot not ok!");
        return AniWindowUtils::AniThrowError(env, ret);
    } else {
        getSnapshotCallback->GetSyncResult(static_cast<int32_t>(MAIN_WINDOW_SNAPSGOT_TIMEOUT));
        if (*errCode == WMError::WM_OK) {
            return AniWindowUtils::CreateAniPixelMapArray(env, *pixelMaps);
        }
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_SYSTEM_ABNORMALLY);
    }
}

ani_ref AniWindowManager::CreateWindow(ani_env* env, ani_long nativeObj, ani_object configuration)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    AniWindowManager* aniWindowManager = reinterpret_cast<AniWindowManager*>(nativeObj);
    return aniWindowManager != nullptr ? aniWindowManager->OnCreateWindow(env, configuration) : nullptr;
}

ani_ref CreateAniSystemWindow(ani_env* env, void* contextPtr, sptr<WindowOption> windowOption)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    if (windowOption == nullptr) {
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_SYSTEM_ABNORMALLY);
    }
    auto context = static_cast<std::weak_ptr<AbilityRuntime::Context>*>(contextPtr);
    if (contextPtr == nullptr || context == nullptr) {
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_CONTEXT_ABNORMALLY,
            "[ANI] Context is nullptr");
    }
    if (windowOption->GetWindowType() == WindowType::WINDOW_TYPE_FLOAT ||
        windowOption->GetWindowType() == WindowType::WINDOW_TYPE_FLOAT_CAMERA) {
        auto abilityContext = AbilityRuntime::Context::ConvertTo<AbilityRuntime::AbilityContext>(context->lock());
        if (abilityContext != nullptr) {
            if (!Permission::CheckCallingPermission("ohos.permission.SYSTEM_FLOAT_WINDOW")) {
                return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_NO_PERMISSION,
                    "[ANI] TYPE_FLOAT CheckCallingPermission failed");
            }
        }
    }
    WMError wmError = WMError::WM_OK;
    sptr<Window> window = Window::Create(windowOption->GetWindowName(), windowOption, context->lock(), wmError);
    WmErrorCode wmErrorCode = WM_JS_TO_ERROR_CODE_MAP.at(wmError);
    if (window != nullptr && wmErrorCode == WmErrorCode::WM_OK) {
        return CreateAniWindowObject(env, window);
    } else {
        return AniWindowUtils::AniThrowError(env, wmErrorCode, "Create window failed");
    }
}

ani_ref CreateAniSubWindow(ani_env* env, sptr<WindowOption> windowOption)
{
    if (windowOption == nullptr) {
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_SYSTEM_ABNORMALLY);
    }
    windowOption->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    if (windowOption->GetParentId() == INVALID_WINDOW_ID) {
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY,
            "[ANI] Parent window missed");
    }

    sptr<Window> window = Window::Create(windowOption->GetWindowName(), windowOption);
    if (window == nullptr) {
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    } else {
        return CreateAniWindowObject(env, window);
    }
}

bool ParseRequiredConfigOption(ani_env* env, ani_object configuration, WindowOption &option)
{
    ani_ref result;
    env->Object_GetPropertyByName_Ref(configuration, "name", &result);
    ani_string aniWindowName = reinterpret_cast<ani_string>(result);
    std::string windowName;
    AniWindowUtils::GetStdString(env, aniWindowName, windowName);
    TLOGI(WmsLogTag::DEFAULT, "[ANI] WindowName: %{public}s", windowName.c_str());
    option.SetWindowName(windowName);

    ani_int ret;
    env->Object_GetPropertyByName_Ref(configuration, "windowType", &result);
    auto status = env->EnumItem_GetValue_Int(static_cast<ani_enum_item>(result), &ret);
    if (status != ANI_OK) {
        TLOGI(WmsLogTag::DEFAULT, "[ANI] Fail to throw err, status: %{public}d", static_cast<int32_t>(status));
        return false;
    }
    uint32_t winType = static_cast<uint32_t>(ret);
    TLOGI(WmsLogTag::DEFAULT, "[ANI] winType: %{public}u", winType);
    if (winType >= static_cast<uint32_t>(ApiWindowType::TYPE_BASE) &&
        winType < static_cast<uint32_t>(ApiWindowType::TYPE_END)) {
        option.SetWindowType(JS_TO_NATIVE_WINDOW_TYPE_MAP.at(static_cast<ApiWindowType>(winType)));
    } else {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] Invalid winType");
        return false;
    }
    return true;
}

bool ParseConfigOption(ani_env* env, ani_object configuration, WindowOption &option, void*& contextPtr)
{
    if (!ParseRequiredConfigOption(env, configuration, option)) {
        return false;
    }

    ani_ref result;
    env->Object_GetPropertyByName_Ref(configuration, "title", &result);
    ani_boolean isTitleUndefined = false;
    env->Reference_IsUndefined(result, &isTitleUndefined);
    if (!isTitleUndefined) {
        ani_string aniDialogTitle = reinterpret_cast<ani_string>(result);
        std::string dialogTitle;
        AniWindowUtils::GetStdString(env, aniDialogTitle, dialogTitle);
        TLOGI(WmsLogTag::DEFAULT, "[ANI] dialogTitle: %{public}s", dialogTitle.c_str());
        option.SetDialogTitle(dialogTitle);
    }

    env->Object_GetPropertyByName_Ref(configuration, "ctx", &result);
    ani_boolean isCtxUndefined = false;
    env->Reference_IsUndefined(result, &isCtxUndefined);
    if (!isCtxUndefined) {
        ani_object aniContextPtr = reinterpret_cast<ani_object>(result);
        contextPtr = AniWindowUtils::GetAbilityContext(env, aniContextPtr);
        auto context = static_cast<std::weak_ptr<AbilityRuntime::Context>*>(contextPtr);
        if (context == nullptr) {
            TLOGE(WmsLogTag::DEFAULT, "[ANI] context is nullptr");
            return AniWindowUtils::AniThrowError(env, WMError::WM_ERROR_NULLPTR, "Stage mode without context");
        }
    }

    ani_boolean dialogDecorEnable;
    env->Object_GetPropertyByName_Boolean(configuration, "decorEnabled", &dialogDecorEnable);
    option.SetDialogDecorEnable(dialogDecorEnable);

    ani_double ret;
    env->Object_GetPropertyByName_Double(configuration, "displayId", &ret);
    int64_t displayId = static_cast<int64_t>(ret);
    if (displayId < 0 ||
        SingletonContainer::Get<DisplayManager>().GetDisplayById(static_cast<uint64_t>(displayId)) == nullptr) {
        TLOGI(WmsLogTag::DEFAULT, "[ANI] DisplayId is invalid");
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
    }
    option.SetDisplayId(displayId);

    env->Object_GetPropertyByName_Double(configuration, "parentId", &ret);
    int64_t parentId = static_cast<int64_t>(ret);
    option.SetParentId(parentId);

    return true;
}

ani_ref AniWindowManager::OnCreateWindow(ani_env* env, ani_object configuration)
{
    WindowOption option;
    void* contextPtr = nullptr;
    if (!ParseConfigOption(env, configuration, option, contextPtr)) {
        return AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM, "[ANI] Failed to parse config");
    }
    sptr<WindowOption> windowOption = new WindowOption(option);
    if (WindowHelper::IsSystemWindow(option.GetWindowType())) {
        return CreateAniSystemWindow(env, contextPtr, windowOption);
    } else if (WindowHelper::IsSubWindow(option.GetWindowType())) {
        return CreateAniSubWindow(env, windowOption);
    } else {
        return AniWindowUtils::AniThrowError(env, WMError::WM_ERROR_NULLPTR, "[ANI] Create window failed");
    }
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

void AniWindowManager::MinimizeAll(ani_env* env, ani_long nativeObj, ani_long displayId)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[ANI]");
    AniWindowManager* aniWindowManager = reinterpret_cast<AniWindowManager*>(nativeObj);
    if (aniWindowManager != nullptr) {
        aniWindowManager->OnMinimizeAll(env, displayId);
    } else {
        TLOGE(WmsLogTag::WMS_LIFE, "[ANI] aniWindowManager is nullptr");
    }
}

void AniWindowManager::OnMinimizeAll(ani_env* env, ani_long displayId)
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

void AniWindowManager::OnSetWindowLayoutMode(ani_env* env, ani_enum_item mode)
{
    if (!Permission::IsSystemCalling() && !Permission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] permission denied!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_NOT_SYSTEM_APP);
        return;
    }

    uint32_t modeType;
    ani_status ani_ret = AniWindowUtils::GetEnumValue(env, mode, modeType);
    if (ani_ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] GetEnumValue failed, ret: %{public}d", ani_ret);
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }

    WindowLayoutMode winLayoutMode = static_cast<WindowLayoutMode>(modeType);
    if (winLayoutMode != WindowLayoutMode::CASCADE && winLayoutMode != WindowLayoutMode::TILE) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] Invalid winLayoutMode: %{public}u", winLayoutMode);
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_INVALID_PARAM);
        return;
    }

    const WMError ret = SingletonContainer::Get<WindowManager>().SetWindowLayoutMode(winLayoutMode);
    const WmErrorCode errorCode = AniWindowUtils::ToErrorCode(ret);
    if (errorCode != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] Failed, modeType: %{public}u, ret: %{public}d",
            modeType, static_cast<int32_t>(ret));
        AniWindowUtils::AniThrowError(env, errorCode);
        return;
    }

    TLOGD(WmsLogTag::WMS_LAYOUT, "[ANI] Success, modeType: %{public}u", modeType);
}

void AniWindowManager::SetWindowLayoutMode(ani_env* env, ani_long nativeObj, ani_enum_item mode)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "[ANI]");
    AniWindowManager* aniWindowManager = reinterpret_cast<AniWindowManager*>(nativeObj);
    if (aniWindowManager != nullptr) {
        aniWindowManager->OnSetWindowLayoutMode(env, mode);
    } else {
        TLOGE(WmsLogTag::WMS_LAYOUT, "[ANI] aniWindowManager is nullptr");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
}

void AniWindowManager::ShiftAppWindowTouchEvent(ani_env* env, ani_long nativeObj, ani_int sourceWindowId,
    ani_int targetWindowId, ani_int fingerId)
{
    AniWindowManager* aniWindowManager = reinterpret_cast<AniWindowManager*>(nativeObj);
    if (aniWindowManager != nullptr) {
        aniWindowManager->OnShiftAppWindowTouchEvent(env, sourceWindowId, targetWindowId, fingerId);
    } else {
        TLOGE(WmsLogTag::WMS_PC, "[ANI]aniWindowManager is nullptr!");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY);
    }
}

void AniWindowManager::OnShiftAppWindowTouchEvent(ani_env* env, ani_int sourceWindowId, ani_int targetWindowId,
    ani_int fingerId)
{
    TLOGI(WmsLogTag::WMS_PC, "[ANI]sourceWindowId: %{public}d, targetWindowId: %{public}d, fingerId: %{public}d",
        sourceWindowId, targetWindowId, fingerId);
    if (sourceWindowId <= static_cast<int32_t>(INVALID_WINDOW_ID) ||
        targetWindowId <= static_cast<int32_t>(INVALID_WINDOW_ID) ||
        (fingerId <= static_cast<int32_t>(INVALID_FINGER_ID))) {
        TLOGE(WmsLogTag::WMS_PC, "[ANI]invalid sourceWindowId or targetWindowId or fingerId");
        AniWindowUtils::AniThrowError(env, WmErrorCode::WM_ERROR_ILLEGAL_PARAM, "shiftAppWindowTouchEvent failed");
        return;
    }
    WmErrorCode ret = AniWindowUtils::ToErrorCode(
        SingletonContainer::Get<WindowManager>().ShiftAppWindowTouchEvent(sourceWindowId, targetWindowId, fingerId));
    if (ret != WmErrorCode::WM_OK) {
        TLOGE(WmsLogTag::WMS_PC, "[ANI]ShiftAppWindowTouchEvent failed, ret: %{public}d", ret);
        AniWindowUtils::AniThrowError(env, ret, "ShiftAppWindowTouchEvent failed.");
    }
    return;
}

}  // namespace Rosen
}  // namespace OHOS