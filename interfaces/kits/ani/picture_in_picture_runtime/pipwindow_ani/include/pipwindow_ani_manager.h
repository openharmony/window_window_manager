/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef PIPWINDOW_MANAGER_ANI_H
#define PIPWINDOW_MANAGER_ANI_H

#include "ani.h"

namespace OHOS {
namespace Rosen {

class PiPWindowManagerAni {
public:
    explicit PiPWindowManagerAni();

    static ani_status initPiPWindowManagerAni(ani_namespace pipWindowNameSpace, ani_env* env);
    static ani_boolean isPiPEnabledAni(ani_env* env);
};
}
}
#endif