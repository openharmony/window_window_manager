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
#include "display_ani_register_manager.h"
#include "display_ani_listener.h"
#include "singleton_container.h"
#include "display_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
    
DisplayAniRegisterManager::DisplayAniRegisterManager(){

}

DisplayAniRegisterManager::~DisplayAniRegisterManager(){

}

DMError DisplayAniRegisterManager::RegisterListener(std::string type, ani_env* env, ani_ref callback)
{
    sptr<DisplayAniListener> displayAniListener = new(std::nothrow) DisplayAniListener(env); 
    if (type == EVENT_ADD) {
        // add callback to listener
        displayAniListener->AddCallback(type, callback);
        // add listener to map
        jsCbMap_[type][callback] = displayAniListener;
        auto ret = SingletonContainer::Get<DisplayManager>().RegisterDisplayListener(displayAniListener);
        return ret;
    }
    return DMError::DM_OK;
}

DMError DisplayAniRegisterManager::UnregisterListener(sptr<DisplayManager::IDisplayListener> listener, std::string type, ani_env* env, ani_ref value)
{
    return DMError::DM_OK;
}

}
}