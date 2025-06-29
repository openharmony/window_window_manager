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
#ifndef SCREEN_MANAGER_ANI_H
#define SCREEN_MANAGER_ANI_H

#include <mutex>
#include "dm_common.h"
#include "refbase.h"
#include "screen_manager.h"

#include "ani.h"
#include "screen_ani_listener.h"

namespace OHOS {
namespace Rosen {

class ScreenManagerAni {
public:
    explicit ScreenManagerAni();
    static void registerCallback(ani_env* env, ani_string type,
        ani_ref callback, ani_long nativeObj);
    static void unRegisterCallback(ani_env* env, ani_string type,
        ani_long nativeObj, ani_ref callback);
    static ani_status initScreenManagerAni(ani_namespace screenNameSpace, ani_env* env);
    DmErrorCode processRegisterCallback(ani_env* env, std::string& typeStr,
        sptr<ScreenAniListener> screenAniListener);
    static ani_double makeMirror(ani_env* env, ani_double mainScreen, ani_object mirrorScreen);
    static void getAllScreens(ani_env* env, ani_object screensAni);
private:
    void onRegisterCallback(ani_env* env, ani_string type, ani_ref callback);
    void onUnRegisterCallback(ani_env* env, ani_string type, ani_ref callback);
    DMError UnRegisterScreenListenerWithType(std::string type, ani_env* env, ani_ref callback);
    DMError UnRegisterAllScreenListenerWithType(std::string type);
    std::mutex mtx_;
    std::map<std::string, std::map<ani_ref, sptr<ScreenAniListener>>> jsCbMap_;
};
}
}
#endif