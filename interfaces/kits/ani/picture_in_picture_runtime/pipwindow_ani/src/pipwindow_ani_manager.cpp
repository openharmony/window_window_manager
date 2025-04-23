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
#include <algorithm>

#include "ani.h"
#include "singleton_container.h"
#include "pipwindow_ani_manager.h"
#include "window_manager_hilog.h"
#include "dm_common.h"
#include "refbase.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {

PiPWindowManagerAni::PiPWindowManagerAni()
{
}

ani_status PiPWindowManagerAni::initPiPWindowManagerAni(ani_namespace pipWindowNameSpace, ani_env* env)
{
    TLOGI(WmsLogTag::WMS_PIP, "[ANI]");
    ani_function setObjFunc = nullptr;
    ani_status ret = env->Namespace_FindFunction(pipWindowNameSpace, "setPipWindowMgrRef", "J:V", &setObjFunc);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_PIP, "[ANI] find setNativeObj func fail %{public}u", ret);
        return ret;
    }
    std::unique_ptr<PiPWindowManagerAni> anipipWindowManager = std::make_unique<PiPWindowManagerAni>();
    ret = env->Function_Call_Void(setObjFunc, anipipWindowManager.release());
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_PIP, "[ANI] find setNativeObj func fail %{public}u", ret);
        return ret;
    }
    return ret;
}

ani_boolean PiPWindowManagerAni::isPiPEnabledAni(ani_env* env)
{
    bool isPiPEnabled = true;
    TLOGI(WmsLogTag::WMS_PIP, "[ANI] isPiPEnabled = %{public}u", isPiPEnabled);
    return static_cast<ani_boolean>(isPiPEnabled);
}
}
}
