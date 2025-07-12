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
#include "screen_ani.h"
 
#include "dm_common.h"
#include "window_manager_hilog.h"
#include "ani_err_utils.h"
 
namespace OHOS {
namespace Rosen {
ScreenAni::ScreenAni(const sptr<Screen>& screen) : screen_(screen)
{
}
 
void ScreenAni::SetDensityDpi(ani_env* env, ani_object obj, ani_double densityDpi)
{
    ani_long screenNativeRef;
    if (ANI_OK != env->Object_GetFieldByName_Long(obj, "screenNativeObj", &screenNativeRef)) {
        TLOGE(WmsLogTag::DMS, "[ANI] screenAni native null ptr");
        return;
    }
    ScreenAni* screenAni = reinterpret_cast<ScreenAni*>(screenNativeRef);
    screenAni->OnSetDensityDpi(env, obj, densityDpi);
}
 
void ScreenAni::OnSetDensityDpi(ani_env* env, ani_object obj, ani_double densityDpi)
{
    DmErrorCode ret = DM_JS_TO_ERROR_CODE_MAP.at(screen_->SetDensityDpi(static_cast<uint32_t>(densityDpi)));
    if (ret != DmErrorCode::DM_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] OnSetDensityDpi fail");
        AniErrUtils::ThrowBusinessError(env, ret, "JsScreen::OnSetDensityDpi failed.");
    }
}
 
}
}