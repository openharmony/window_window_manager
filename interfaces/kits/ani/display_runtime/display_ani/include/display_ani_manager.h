/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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
#ifndef DISPLAY_MANAGER_ANI_H
#define DISPLAY_MANAGER_ANI_H

#include "ani.h"
#include "display.h"
#include "display_ani_listener.h"

namespace OHOS {
namespace Rosen {

class DisplayManagerAni {
public:
    explicit DisplayManagerAni();
    static ani_status InitDisplayManagerAni(ani_namespace displayNameSpace, ani_env* env);

    static ani_int GetFoldDisplayModeAni(ani_env* env);
    static ani_boolean IsFoldableAni(ani_env* env);
    static ani_int GetFoldStatus(ani_env* env);
    static ani_boolean IsCaptured(ani_env* env);
    static void GetCurrentFoldCreaseRegion(ani_env* env, ani_object obj, ani_long nativeObj);

    static void GetAllDisplaysAni(ani_env* env, ani_object arrayObj);
    static void GetDisplayByIdSyncAni(ani_env* env, ani_object obj, ani_long displayId);
    static void GetDefaultDisplaySyncAni(ani_env* env, ani_object obj);

    static void RegisterCallback(ani_env* env, ani_string type,
        ani_ref callback, ani_long nativeObj);
    static void UnRegisterCallback(ani_env* env, ani_string type,
        ani_long nativeObj, ani_ref callback);
    DMError UnRegisterDisplayListenerWithType(std::string type, ani_env* env, ani_ref callback);
    DMError UnregisterAllDisplayListenerWithType(std::string type);
    DmErrorCode ProcessRegisterCallback(ani_env* env, std::string& typeStr,
        sptr<DisplayAniListener> displayAniListener);
    static ani_boolean HasPrivateWindow(ani_env* env, ani_long displayId);
    static void GetAllDisplayPhysicalResolution(ani_env* env, ani_object arrayObj, ani_long nativeObj);
    static void FinalizerDisplay(ani_env* env, ani_object displayObj, ani_long nativeObj);
private:
    void OnRegisterCallback(ani_env* env, ani_string type, ani_ref callback);
    void OnUnRegisterCallback(ani_env* env, ani_string type, ani_ref callback);
    void OnGetCurrentFoldCreaseRegion(ani_env* env, ani_object obj);
    void OnGetAllDisplayPhysicalResolution(ani_env* env, ani_object arrayObj);
    void OnFinalizerDisplay(ani_env* env, ani_object displayObj);
    bool IsCallbackRegistered(ani_env* env, const std::string& type, ani_ref callback);
    std::mutex mtx_;
    std::map<std::string, std::map<ani_ref, sptr<DisplayAniListener>>> jsCbMap_;
};
}
}
#endif