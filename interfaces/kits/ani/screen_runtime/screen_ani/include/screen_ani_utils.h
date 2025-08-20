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
#ifndef OHOS_ANI_SCREEN_UTILS_H
#define OHOS_ANI_SCREEN_UTILS_H

#include <hitrace_meter.h>

#include "ani.h"
#include "singleton_container.h"

namespace OHOS {
namespace Rosen {

class ScreenAniUtils {
public:
static ani_status GetStdString(ani_env *env, ani_string ani_str, std::string &result);

static ani_object CreateAniUndefined(ani_env* env);

static ani_status GetAniString(ani_env* env, const std::string& str, ani_string* result);

static ani_status CallAniFunctionVoid(ani_env *env, const char* ns,
    const char* fn, const char* signature, ...);
};
}
}
#endif