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
#include "ani_common_utils.h"

#include <cstdint>
#include <regex>
#include "ani.h"

#include "foundation/arkui/ace_engine/interfaces/inner_api/ace/ui_content.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
void AniCommonUtils::GetStdString(ani_env *env, ani_string str, std::string &result)
{
    ani_size sz {};
    env->String_GetUTF8Size(str, &sz);
    result.resize(sz + 1);
    env->String_GetUTF8SubString(str, 0, sz, result.data(), result.size(), &sz);
}

ani_status AniCommonUtils::NewAniObjectNoParams(ani_env* env, const char* cls, ani_object* object)
{
    ani_class aniClass;
    ani_status ret = env->FindClass(cls, &aniClass);
    if (ret != ANI_OK) {
        return ret;
    }
    ani_method aniCtor;
    ret = env->Class_FindMethod(aniClass, "<ctor>", "V:V", &aniCtor);
    if (ret != ANI_OK) {
        return ret;
    }
    return env->Object_New(aniClass, aniCtor, object);
}

ani_status AniCommonUtils::NewAniObject(ani_env* env, const char* cls, const char* signature, ani_object* result, ...)
{
    ani_class aniClass;
    ani_status ret = env->FindClass(cls, &aniClass);
    if (ret != ANI_OK) {
        return ret;
    }
    ani_method aniCtor;
    ret = env->Class_FindMethod(aniClass, "<ctor>", signature, &aniCtor);
    if (ret != ANI_OK) {
        return ret;
    }
    va_list args;
    va_start(args, result);
    ani_status status = env->Object_New(aniClass, aniCtor, result, args);
    va_end(args);
    return status;
}

ani_object AniCommonUtils::CreateAniUndefined(ani_env* env)
{
    ani_ref aniRef;
    env->GetUndefined(&aniRef);
    return static_cast<ani_object>(aniRef);
}

ani_object AniCommonUtils::AniThrowError(ani_env* env, WMError error, std::string msg)
{
    return AniCommonUtils::CreateAniUndefined(env);
}

ani_object AniCommonUtils::AniThrowError(ani_env* env, WmErrorCode error, std::string msg)
{
    return AniCommonUtils::CreateAniUndefined(env);
}

ani_object AniCommonUtils::CreateAniRect(ani_env* env, const Rect& rect)
{
    ani_object aniRect;
    if (AniCommonUtils::NewAniObjectNoParams(env, "L@ohos/window/window/Rect", &aniRect) != ANI_OK) {
        return AniCommonUtils::CreateAniUndefined(env);
    }
    env->Object_SetFieldByName_Int(aniRect, "left", static_cast<ani_int>(rect.posX_));
    env->Object_SetFieldByName_Int(aniRect, "top", static_cast<ani_int>(rect.posY_));
    env->Object_SetFieldByName_Int(aniRect, "width", static_cast<ani_int>(rect.width_));
    env->Object_SetFieldByName_Int(aniRect, "height", static_cast<ani_int>(rect.height_));
    return aniRect;
}

ani_object AniCommonUtils::CreateAniAvoidArea(ani_env* env, const AvoidArea& avoidArea, AvoidAreaType type)
{
    ani_object aniAvoidArea;
    if (AniCommonUtils::NewAniObjectNoParams(env, "L@ohos/window/window/AvoidArea", &aniAvoidArea) != ANI_OK) {
        return AniCommonUtils::CreateAniUndefined(env);
    }
    env->Object_SetFieldByName_Boolean(aniAvoidArea, "visible",
        static_cast<ani_boolean>(type != AvoidAreaType::TYPE_CUTOUT));
    env->Object_SetFieldByName_Ref(aniAvoidArea, "leftRect", CreateAniRect(env, avoidArea.leftRect_));
    env->Object_SetFieldByName_Ref(aniAvoidArea, "topRect", CreateAniRect(env, avoidArea.topRect_));
    env->Object_SetFieldByName_Ref(aniAvoidArea, "rightRect", CreateAniRect(env, avoidArea.rightRect_));
    env->Object_SetFieldByName_Ref(aniAvoidArea, "bottomRect", CreateAniRect(env, avoidArea.bottomRect_));
    return aniAvoidArea;
}

bool AniCommonUtils::GetAPI7Ability(ani_env* env, AppExecFwk::Ability* &ability)
{
    return true;
}

void AniCommonUtils::GetNativeContext(ani_env* env, ani_object nativeContext, void*& contextPtr, WMError& errCode)
{
}

void AniCommonUtils::GetAniString(ani_env *env, const std::string &str, ani_string *result)
{
    env->String_NewUTF8(str.c_str(), static_cast<ani_size>(str.size()), result);
}
} // namespace Rosen
} // namespace OHOS