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
#ifndef OHOS_ANI_DISPLAY_REGISTER_MANAGER_H
#define OHOS_ANI_DISPLAY_REGISTER_MANAGER_H
#include <map>
#include <mutex>

#include "dm_common.h"
#include "refbase.h"
#include "display_ani_listener.h"
#include "display_manager.h"

namespace OHOS {
namespace Rosen {
enum class RegisterListenerType : uint32_t {

};
    
class DisplayAniRegisterManager {
public:
    DisplayAniRegisterManager();
    ~DisplayAniRegisterManager();
    ani_object RegisterListener(std::string type, ani_env* env, ani_ref callback);
    ani_object UnregisterListener(std::string type, ani_env* env, ani_ref callback);
    DMError UnRegisterDisplayListenerWithType(std::string type, ani_env* env, ani_ref callback);
    DMError UnregisterAllDisplayListenerWithType(std::string type);
private:
    std::map<std::string, std::map<ani_ref, sptr<DisplayAniListener>>> jsCbMap_;
    std::set<sptr<DisplayManager::IDisplayListener>> displayListeners_;
    std::mutex mutex_;
};
} // namespace Rosen
} // namespace OHOS
    
#endif