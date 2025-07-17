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

#ifndef OHOS_ANI_WINDOW_MANAGER_H
#define OHOS_ANI_WINDOW_MANAGER_H

#include "ani.h"
#include "ani_window_register_manager.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
#ifdef _WIN32
#define WINDOW_EXPORT __attribute__((dllexport))
#else
#define WINDOW_EXPORT __attribute__((visibility("default")))
#endif

class AniWindowManager {
public:
    explicit AniWindowManager();

    static ani_status AniWindowManagerInit(ani_env* env);
    static ani_object WindowStageCreate(ani_env* env, ani_long scene);
    static ani_object GetWindowsByCoordinate(ani_env* env, ani_long nativeObj, ani_object getWindowsParam);
    static ani_ref GetLastWindow(ani_env* env, ani_long nativeObj, ani_object context);
    static ani_ref FindWindow(ani_env* env, ani_long nativeObj, ani_string windowName);
    static ani_ref CreateWindow(ani_env* env, ani_long nativeObj, ani_object configuration);
    static void MinimizeAll(ani_env* env, ani_long nativeObj, ani_long displayId);
    static void RegisterWindowManagerCallback(ani_env* env, ani_long nativeObj, ani_string type, ani_ref callback);
    static void UnregisterWindowManagerCallback(ani_env* env, ani_long nativeObj, ani_string type, ani_ref callback);
    static void ShiftAppWindowFocus(ani_env* env, ani_object obj, ani_long nativeObj,
        ani_double sourceWindowId, ani_double targetWindowId);
private:
    ani_object OnGetWindowsByCoordinate(ani_env* env, ani_object getWindowsParam);
    ani_ref OnGetLastWindow(ani_env* env, ani_object context);
    ani_ref OnFindWindow(ani_env* env, ani_string windowName);
    ani_ref OnCreateWindow(ani_env* env, ani_object configuration);
    void OnMinimizeAll(ani_env* env, ani_long displayId);
    void OnShiftAppWindowFocus(ani_env* env, ani_double sourceWindowId, ani_double targetWindowId);
    ani_object GetTopWindowTask(ani_env* env, void* contextPtr, bool newApi);
    void OnRegisterWindowManagerCallback(ani_env* env, ani_string type, ani_ref callback);
    void OnUnregisterWindowManagerCallback(ani_env* env, ani_string type, ani_ref callback);

    std::unique_ptr<AniWindowRegisterManager> registerManager_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ANI_WINDOW_MANAGER_H