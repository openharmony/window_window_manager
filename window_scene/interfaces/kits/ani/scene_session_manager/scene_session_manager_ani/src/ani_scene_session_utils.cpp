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
#include "ani_err_utils.h"
#include "ani_scene_session_utils.h"
#include "ani.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {


ani_object AniSceneSessionUtils::AniThrowError(ani_env* env, WmErrorCode errorCode, std::string msg)
{
    AniErrUtils::ThrowBusinessError(env, errorCode, msg);
    return AniSceneSessionUtils::CreateAniUndefined(env);
}

ani_object AniSceneSessionUtils::CreateAniUndefined(ani_env* env)
{
    ani_ref aniRef;
    env->GetUndefined(&aniRef);
    return static_cast<ani_object>(aniRef);
}

ani_status AniSceneSessionUtils::NewAniObject(ani_env* env, ani_class cls,
    const char *signature, ani_object* result, ...)
{
    ani_method aniCtor;
    auto ret = env->Class_FindMethod(cls, "<ctor>", signature, &aniCtor);
    if (ret != ANI_OK) {
        return ret;
    }
    va_list args;
    va_start(args, result);
    ani_status status = env->Object_New(cls, aniCtor, result, args);
    va_end(args);
    return status;
}

}
}