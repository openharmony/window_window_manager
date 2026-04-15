/*
 * Copyright (c) 2026-2026 Huawei Device Co., Ltd.
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

#ifndef ANI_FV_WINDOW_UTILS_H
#define ANI_FV_WINDOW_UTILS_H

#include "ani.h"
#include "window.h"
#include "js_runtime_utils.h"

namespace OHOS {
namespace Rosen {
ani_object AniThrowError(ani_env* env, WMError wmError, const std::string& message = "");
ani_object AniThrowError(ani_env* env, WmErrorCode wmErrorCode, const std::string& message = "");
ani_status GetTemplateType(ani_env* env, ani_object floatViewConfigurations, uint32_t& templateType);
ani_status GetContextPtr(ani_env* env, ani_object floatViewConfigurations, void*& contextPtr);
ani_ref AniGetUndefined(ani_env* env);
std::string GetErrorMsg(WMError error);
std::string GetErrorMsg(WmErrorCode error);
ani_status CreateBusinessError(ani_env* env, int32_t error, std::string message, ani_object* err);
ani_status GetStdString(ani_env* env, ani_string ani_str, std::string &result);
void* GetAbilityContext(ani_env* env, ani_object aniObj);
ani_status CallAniFunctionVoid(ani_env* env, const char* ns, const char* fn, const char* signature, ...);
ani_status GetAniString(ani_env* env, const std::string& str, ani_string* result);
bool ParseWindowSize(ani_env* env, ani_object windowSize, Rect& rect);
ani_object CreateAniFloatViewStateChangeInfoObject(ani_env* env, const FloatViewState state, const std::string& reason);
ani_object CreateAniFloatViewRectChangeInfoObject(ani_env* env,
    const Rect& rect, double scale, const std::string& reason);
ani_object CreateAniFvWindowInfoObject(ani_env* env,
    const sptr<Window>& window, const FloatViewWindowInfo &windowInfo, const FvWindowState &state);
ani_object CreateAniFloatViewLimitsObject(ani_env* env, const FloatViewLimits& limits);
ani_object CreateAniRatioObject(ani_env* env, const double& min, const double& max);
}
}
#endif // ANI_FV_WINDOW_UTILS_H