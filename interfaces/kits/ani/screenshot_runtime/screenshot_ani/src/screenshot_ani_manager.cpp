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
#include "screenshot_ani_manager.h"
 
#include <hitrace_meter.h>
#include <algorithm>
 
#include "ani.h"
#include "singleton_container.h"
#include "window_manager_hilog.h"
#include "display_manager.h"
#include "dm_common.h"
#include "pixel_map_taihe_ani.h"
#include "refbase.h"
#include "screenshot_ani_utils.h"
#include "ani_err_utils.h"
 
namespace OHOS::Rosen {
 
ScreenshotManagerAni::ScreenshotManagerAni()
{
}
 
void ScreenshotManagerAni::InitScreenshotManagerAni(ani_namespace nsp, ani_env* env)
{
}
 
ani_object ScreenshotManagerAni::Save(ani_env* env, ani_object options)
{
    TLOGI(WmsLogTag::DMS, "[ANI] begin");
    auto param = std::make_unique<Param>();
    if (param == nullptr) {
        TLOGI(WmsLogTag::DMS, "Create param failed.");
        return nullptr;
    }
    param->option.displayId = DisplayManager::GetInstance().GetDefaultDisplayId();
    ani_boolean optionsUndefined = 0;
    env->Reference_IsUndefined(options, &optionsUndefined);
    param->validInputParam = true;
    if (!optionsUndefined) {
        param->useInputOption = true;
        ani_status ret = ScreenshotAniUtils::GetScreenshotParam(env, param, options);
        if (ret != ANI_OK) {
            AniErrUtils::ThrowBusinessError(env, DmErrorCode::DM_ERROR_INVALID_PARAM, "get screen shot param failed");
            return nullptr;
        }
    }
    param->isPick = false;
    GetScreenshot(env, param);
    if (param->wret != DmErrorCode::DM_OK) {
        AniErrUtils::ThrowBusinessError(env, param->wret, param->errMessage);
        return ScreenshotAniUtils::CreateAniUndefined(env);
    }
    if (param->image != nullptr) {
        TLOGI(WmsLogTag::DMS, "save pixelMap, currentId = %{public}d", static_cast<int>(param->image->currentId));
    }
    auto nativePixelMap = Media::PixelMapTaiheAni::CreateEtsPixelMap(env, param->image);
    return nativePixelMap;
}
 
void ScreenshotManagerAni::GetScreenshot(ani_env *env, std::unique_ptr<Param> &param)
{
    TLOGI(WmsLogTag::DMS, "[ANI] begin");
    if (!param->validInputParam) {
        TLOGI(WmsLogTag::DMS, "Get Screenshot invalidInputParam");
        param->image = nullptr;
        param->wret = DmErrorCode::DM_ERROR_INVALID_PARAM;
        param->errMessage = "Get Screenshot Failed: Invalid input param";
        return;
    }
    CaptureOption option = { param->option.displayId, param->option.isNeedNotify, true,
        param->option.isCaptureFullOfScreen };
    if (!param->isPick && !option.isNeedNotify_) {
        if (param->useInputOption) {
            param->image = DisplayManager::GetInstance().GetScreenshotWithOption(option,
                param->option.rect, param->option.size, param->option.rotation, &param->wret);
        } else {
            param->image = DisplayManager::GetInstance().GetScreenshotWithOption(option, &param->wret);
        }
    } else {
        if (param->useInputOption) {
            TLOGI(WmsLogTag::DMS, "Get Screenshot by input option");
            SnapShotConfig snapConfig;
            snapConfig.displayId_ = param->option.displayId;
            snapConfig.imageRect_ = param->option.rect;
            snapConfig.imageSize_ = param->option.size;
            snapConfig.rotation_ = param->option.rotation;
            snapConfig.isCaptureFullOfScreen_ = param->option.isCaptureFullOfScreen;
            param->image = DisplayManager::GetInstance().GetScreenshotwithConfig(snapConfig, &param->wret, true);
        } else if (param->isPick) {
            TLOGI(WmsLogTag::DMS, "Get Screenshot by picker");
            param->image = DisplayManager::GetInstance().GetSnapshotByPicker(param->imageRect, &param->wret);
        } else {
            TLOGI(WmsLogTag::DMS, "Get Screenshot by default option");
            param->image = DisplayManager::GetInstance().GetScreenshot(param->option.displayId, &param->wret, true,
                param->option.isCaptureFullOfScreen);
        }
    }
    if (param->image == nullptr && param->wret == DmErrorCode::DM_OK) {
        TLOGI(WmsLogTag::DMS, "Get Screenshot failed!");
        param->wret = DmErrorCode::DM_ERROR_INVALID_SCREEN;
        param->errMessage = "Get Screenshot failed: Screenshot image is nullptr";
        return;
    }
}
 
extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::DMS, "[ANI] start to ANI_Constructor");
    ani_status ret;
    ani_env *env;
    if ((ret = vm->GetEnv(ANI_VERSION_1, &env)) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] null env");
        return ANI_NOT_FOUND;
    }
    ani_namespace nsp;
    if ((ret = env->FindNamespace("@ohos.screenshot.screenshot", &nsp)) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    ScreenshotManagerAni::InitScreenshotManagerAni(nsp, env);
    std::array funcs = {
        ani_native_function {"saveSync", nullptr,
            reinterpret_cast<void *>(ScreenshotManagerAni::Save)},
    };
    if ((ret = env->Namespace_BindNativeFunctions(nsp, funcs.data(), funcs.size()))) {
        TLOGE(WmsLogTag::DMS, "[ANI] bind namespace fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    *result = ANI_VERSION_1;
    return ANI_OK;
}
}
 
}