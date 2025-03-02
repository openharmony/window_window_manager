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
#include "ani_common_utils.h"
#include "ani_window.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "AniWindowManager"};
}

AniWindowManager* AniWindowManager::GetWindowManagerFromEnv(ani_env* env, ani_class cls, ani_object obj)
{
    ani_field nativeObjName {};
    ani_status ret;
    if ((ret = env->Class_FindField(cls, "nativeObj", &nativeObjName)) != ANI_OK) {
        WLOGFE("[ANI] obj fetch field %{public}u", ret);
        return nullptr;
    }
    ani_long nativeObj {};
    if ((ret = env->Object_GetField_Long(obj, nativeObjName, &nativeObj)) != ANI_OK) {
        WLOGFE("[ANI] obj fetch long %{public}u", ret);
        return nullptr;
    }
    return reinterpret_cast<AniWindowManager*>(nativeObj);
}

ani_object AniWindowManager::GetLastWindow(ani_env* env, ani_class cls, ani_object obj, ani_object context)
{
    auto aniWindowManager = GetWindowManagerFromEnv(env, cls, obj);
    return aniWindowManager != nullptr ? aniWindowManager->OnGetLastWindow(env, context) : nullptr;
}

ani_object AniWindowManager::OnGetLastWindow(ani_env* env, ani_object context)
{
    void* contextPtr = nullptr;
    WMError errCode = WMError::WM_OK;
    AniCommonUtils::GetNativeContext(env, context, contextPtr, errCode);
    if (errCode != WMError::WM_OK) {
        return AniCommonUtils::AniThrowError(env, errCode, "Invalidate params");
    }
    return GetTopWindowTask(env, contextPtr, true);
}

ani_object AniWindowManager::GetTopWindowTask(ani_env* env, void* contextPtr, bool newApi)
{
    struct TopWindowInfoList {
        sptr<Window> window = nullptr;
        AppExecFwk::Ability* ability = nullptr;
        int32_t errorCode = 0;
        std::string errMsg = "";
    };
    std::shared_ptr<TopWindowInfoList> lists = std::make_shared<TopWindowInfoList>();
    bool isOldApi = AniCommonUtils::GetAPI7Ability(env, lists->ability);
    if (isOldApi) {
        if (lists->ability->GetWindow() == nullptr) {
            if (newApi) {
                return AniCommonUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY,
                    "FA mode can not get ability window");
            } else {
                return AniCommonUtils::AniThrowError(env, WMError::WM_ERROR_NULLPTR,
                    "FA mode can not get ability window");
            }
        }
        auto window = Window::GetTopWindowWithId(lists->ability->GetWindow()->GetWindowId());
        return CreateAniWindowObject(env, window);
    } else {
        auto context = static_cast<std::weak_ptr<AbilityRuntime::Context>*>(contextPtr);
        if (contextPtr == nullptr || context == nullptr) {
            if (newApi) {
                return AniCommonUtils::AniThrowError(env, WmErrorCode::WM_ERROR_STATE_ABNORMALLY,
                    "Stage mode without context");
            } else {
                return AniCommonUtils::AniThrowError(env, WMError::WM_ERROR_NULLPTR, "Stage mode without context");
            }
        }
        auto window = Window::GetTopWindowWithContext(context->lock());
        return CreateAniWindowObject(env, window);
    }
}
}  // namespace Rosen
}  // namespace OHOS
