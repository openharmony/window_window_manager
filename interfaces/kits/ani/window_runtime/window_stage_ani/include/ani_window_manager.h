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
    static ani_status AniWindowManagerInit(ani_env* env, ani_namespace windowNameSpace);
    static ani_ref GetLastWindow(ani_env* env, ani_long nativeObj, ani_object context);
    static void ShiftAppWindowFocus(ani_env* env, ani_long nativeObj,
        ani_int sourceWindowId, ani_int targetWindowId);
    static ani_object GetAllMainWindowInfo(ani_env* env, ani_long nativeObj, ani_object context);
    static ani_object GetMainWindowSnapshot(
        ani_env* env, ani_long nativeObj, ani_object windowId, ani_object config);
private:
    ani_ref OnGetLastWindow(ani_env* env, ani_object context);
    void OnShiftAppWindowFocus(ani_env* env, ani_int sourceWindowId, ani_int targetWindowId);
    ani_object GetTopWindowTask(ani_env* env, void* contextPtr, bool newApi);
    ani_object OnGetAllMainWindowInfo(ani_env* env, ani_object context);
    ani_object OnGetMainWindowSnapshot(
        ani_env* env, ani_object windowId, ani_object config);
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ANI_WINDOW_MANAGER_H