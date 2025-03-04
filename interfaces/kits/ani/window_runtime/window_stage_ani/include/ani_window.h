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

#ifndef OHOS_ANI_WINDOW_H
#define OHOS_ANI_WINDOW_H

#include "ani.h"
#include "window.h"
#include "ani_window_register_manager.h"

namespace OHOS {
namespace Rosen {
#ifdef _WIN32
#define WINDOW_EXPORT __attribute__((dllexport))
#else
#define WINDOW_EXPORT __attribute__((visibility("default")))
#endif


class AniWindow {
public:
    explicit AniWindow(const sptr<Window>& window);
    explicit AniWindow(const std::shared_ptr<OHOS::Rosen::Window>& window);
    sptr<Window> GetWindow() { return windowToken_; }

    /* window obj stored in ANI */
    static AniWindow* GetWindowObjectFromEnv(ani_env* env, ani_object obj);

    static ani_object GetWindowAvoidArea(ani_env* env, ani_object obj, ani_int type);
    static ani_object SetWindowColorSpace(ani_env* env, ani_object obj, ani_int colorSpace);
    static void SetPreferredOrientation(ani_env* env, ani_object obj, ani_int orientation);
    static void SetWindowPrivacyMode(ani_env* env, ani_object obj, ani_boolean isPrivacyMode);
    static void Recover(ani_env* env, ani_object obj);
    static void SetWindowKeepScreenOn(ani_env* env, ani_object obj, ani_boolean isKeepScreenOn);
    static void SetWaterMarkFlag(ani_env* env, ani_object obj, ani_boolean enable);
    static void LoadContent(ani_env* env, ani_object obj, ani_string path);
    static void LoadContent(ani_env* env, ani_object obj, ani_string path, ani_object storage);
    static ani_object GetUIContext(ani_env* env, ani_object obj);
    static ani_object RegisterWindowCallback(ani_env* env, ani_object obj, ani_string type, ani_ref callback);
    static ani_object UnregisterWindowCallback(ani_env* env, ani_object obj, ani_string type, ani_ref callback);
    ani_int GetWindowDecorHeight(ani_env* env);
    ani_object SetWindowBackgroundColorSync(ani_env* env, const std::string& color);
    ani_object SetImmersiveModeEnabledState(ani_env* env, bool enable);
    ani_object SetWindowDecorVisible(ani_env* env, bool isVisible);
    ani_object SetWindowDecorHeight(ani_env* env, ani_int height);
    ani_object GetWindowPropertiesSync(ani_env* env);
    ani_boolean IsWindowSupportWideGamut(ani_env* env);
    ani_object SetWindowLayoutFullScreen(ani_env* env, ani_boolean isLayoutFullScreen);
    ani_object SetSystemBarProperties(ani_env* env, ani_object aniSystemBarProperties);
    ani_object SetSpecificSystemBarEnabled(ani_env* env, ani_string, ani_boolean enable, ani_boolean enableAnimation);

private:
    ani_object OnGetWindowAvoidArea(ani_env* env, ani_int type);
    ani_object OnSetWindowColorSpace(ani_env* env, ani_int colorSpace);
    void OnSetPreferredOrientation(ani_env* env, ani_int orientation);
    void OnSetWindowPrivacyMode(ani_env* env, ani_boolean isPrivacyMode);
    void OnRecover(ani_env* env);
    void OnSetWindowKeepScreenOn(ani_env* env, ani_boolean isKeepScreenOn);
    void OnSetWaterMarkFlag(ani_env* env, ani_boolean enable);
    void OnLoadContent(ani_env* env, ani_string path);
    void OnLoadContent(ani_env* env, ani_string path, ani_object storage);
    ani_object OnGetUIContext(ani_env* env);
    ani_object OnRegisterWindowCallback(ani_env* env, ani_string type, ani_ref callback);
    ani_object OnUnregisterWindowCallback(ani_env* env, ani_string type, ani_ref callback);

    sptr<Window> windowToken_ = nullptr;
    std::unique_ptr<AniWindowRegisterManager> registerManager_ = nullptr;
};

/* window obj stored in ANI */
AniWindow* GetWindowObjectFromAni(void* aniObj);
ani_object CreateAniWindowObject(ani_env* env, sptr<Window>& window);
void DropWindowObjectByAni(ani_object obj);
ani_status ANI_Window_Constructor(ani_vm *vm, uint32_t *result);
}  // namespace Rosen
}  // namespace OHOS
#endif  // OHOS_ANI_WINDOW_H
