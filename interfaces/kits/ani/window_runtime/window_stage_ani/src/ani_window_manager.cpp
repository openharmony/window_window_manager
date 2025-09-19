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
#include "ani_window_utils.h"
#include "window_manager_hilog.h"
#include "window_scene.h"
#include "window_helper.h"
#include "window_manager.h"
#include "permission.h"
#include "singleton_container.h"
#include "pixel_map.h"
#include "../../../../../../wm/include/get_snapshot_callback.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t MAIN_WINDOW_SNAPSGOT_TIMEOUT = 5000;
}
ani_status AniWindowManager::AniWindowManagerInit(ani_env* env, ani_namespace windowNameSpace)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    ani_function setObjFunc = nullptr;
    ani_status ret = env->Namespace_FindFunction(windowNameSpace, "setNativeObj", "J:V", &setObjFunc);
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
}  // namespace Rosen
}  // namespace OHOS