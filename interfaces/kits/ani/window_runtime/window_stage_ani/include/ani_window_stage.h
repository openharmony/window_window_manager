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

#ifndef OHOS_ANI_WINDOW_STAGE_H
#define OHOS_ANI_WINDOW_STAGE_H

#include "ani.h"
#include "ani_window_register_manager.h"
#include "window_scene.h"

namespace OHOS {
namespace Rosen {
#ifdef _WIN32
#define WINDOW_EXPORT __attribute__((dllexport))
#else
#define WINDOW_EXPORT __attribute__((visibility("default")))
#endif

class AniWindowStage {
    public:
    explicit AniWindowStage(const std::shared_ptr<Rosen::WindowScene>& windowScene);
    ~AniWindowStage();
    static ani_object NativeTransferStatic(ani_env* aniEnv, ani_class cls, ani_object input);
    static ani_object NativeTransferDynamic(ani_env* aniEnv, ani_class cls, ani_long nativeObj);
    static void LoadContent(ani_env* env, ani_object obj, ani_long nativeObj,
        ani_string path, ani_object storage);
    static void DisableWindowDecor(ani_env* env, ani_object obj, ani_long nativeObj);
    static void SetShowOnLockScreen(ani_env* env, ani_class cls, ani_long nativeObj, ani_boolean showOnLockScreen);
    static void SetWindowModal(ani_env* env, ani_object obj, ani_long nativeObj, ani_boolean isModal);
    static void RegisterWindowCallback(ani_env* env, ani_object obj, ani_long nativeObj, ani_string type,
        ani_ref callback);
    static void UnregisterWindowCallback(ani_env* env, ani_object obj, ani_long nativeObj, ani_string type,
        ani_ref callback);

    void SetWindowRectAutoSave(ani_env* env, ani_boolean enabled, ani_boolean isSaveBySpecifiedFlag);
    ani_boolean IsWindowRectAutoSave(ani_env* env);
    void RemoveStartingWindow(ani_env* env);
    std::weak_ptr<WindowScene> GetWindowScene() { return windowScene_; }
    ani_ref GetMainWindow(ani_env* env);
    ani_boolean WindowIsWindowSupportWideGamut(ani_env* env, ani_class cls, ani_object obj);
    ani_ref OnCreateSubWindow(ani_env *env, ani_string name);
private:
    void OnLoadContent(ani_env* env, ani_string path, ani_object storage);
    void OnDisableWindowDecor(ani_env* env);
    void OnSetShowOnLockScreen(ani_env* env, ani_boolean showOnLockScreen);
    void OnSetWindowModal(ani_env* env, ani_boolean isModal);
    void OnRegisterWindowCallback(ani_env* env, ani_string type, ani_ref callback);
    void OnUnregisterWindowCallback(ani_env* env, ani_string type, ani_ref callback);
    std::weak_ptr<WindowScene> windowScene_;
    std::unique_ptr<AniWindowRegisterManager> registerManager_ = nullptr;
};

ani_object CreateAniWindowStage(ani_env* env, std::shared_ptr<Rosen::WindowScene>& windowScene);
AniWindowStage* GetWindowStageFromAni(void* aniObj);
void DropWindowStageByAni(ani_object obj);
}  // namespace Rosen
}  // namespace OHOS
#endif  // OHOS_ANI_WINDOW_STAGE_H
