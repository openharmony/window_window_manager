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
#include <hitrace_meter.h>

#include "ani.h"
#include "display_ani.h"
#include "singleton_container.h"
#include "display_manager.h"
#include "window_manager_hilog.h"
#include "dm_common.h"
#include "refbase.h"


namespace OHOS {
namespace Rosen {

class DisplayAniUtils {
public:
static ani_object convertRect(DMRect rect, ani_env* env);

static ani_array_ref convertRects(std::vector<DMRect> rects, ani_env* env);

static ani_object convertDisplay(sptr<Display> display, ani_env* env);

static ani_array_ref convertDisplays(std::vector<sptr<Display>> displays, ani_env* env);

static void GetStdString(ani_env *env, ani_string str, std::string &result);

static ani_status NewAniObject(ani_env* env, ani_class cls, const char *signature, ani_object* result, ...);

static ani_status NewAniObjectNoParams(ani_env* env, ani_class cls, ani_object* object);

static ani_object CreateAniUndefined(ani_env* env);

};
}
}