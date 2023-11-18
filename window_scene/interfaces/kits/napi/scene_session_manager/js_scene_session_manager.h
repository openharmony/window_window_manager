/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_WINDOW_SCENE_JS_SCENE_SESSION_MANAGER_H
#define OHOS_WINDOW_SCENE_JS_SCENE_SESSION_MANAGER_H

#include <map>

#include "interfaces/include/ws_common.h"
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include "interfaces/kits/napi/scene_session_manager/js_scene_utils.h"
#include "root_scene.h"
#include "session/host/include/scene_session.h"
#include "ability_info.h"
#include "task_scheduler.h"

namespace OHOS::Rosen {
class JsSceneSessionManager final {
public:
    explicit JsSceneSessionManager(napi_env env);
    ~JsSceneSessionManager() = default;

    static napi_value Init(napi_env env, napi_value exportObj);
    static void Finalizer(napi_env env, void* data, void* hint);

    static napi_value GetRootSceneSession(napi_env env, napi_callback_info info);
    static napi_value RequestSceneSession(napi_env env, napi_callback_info info);
    static napi_value UpdateSceneSessionWant(napi_env env, napi_callback_info info);
    static napi_value RequestSceneSessionActivation(napi_env env, napi_callback_info info);
    static napi_value RequestSceneSessionBackground(napi_env env, napi_callback_info info);
    static napi_value RequestSceneSessionDestruction(napi_env env, napi_callback_info info);
    static napi_value NotifyForegroundInteractiveStatus(napi_env env, napi_callback_info info);
    static napi_value RequestSceneSessionByCall(napi_env env, napi_callback_info info);
    static napi_value StartAbilityBySpecified(napi_env env, napi_callback_info info);
    static napi_value RegisterCallback(napi_env env, napi_callback_info info);
    static napi_value GetWindowSceneConfig(napi_env env, napi_callback_info info);
    static napi_value ProcessBackEvent(napi_env env, napi_callback_info info);
    static napi_value UpdateFocus(napi_env env, napi_callback_info info);
    static napi_value SwitchUser(napi_env env, napi_callback_info info);
    static napi_value GetSessionSnapshotFilePath(napi_env env, napi_callback_info info);
    static napi_value InitWithRenderServiceAdded(napi_env env, napi_callback_info info);
    static napi_value GetAllAbilityInfos(napi_env env, napi_callback_info info);
    static napi_value PrepareTerminate(napi_env env, napi_callback_info info);
    static napi_value PerfRequestEx(napi_env env, napi_callback_info info);
    static napi_value UpdateWindowMode(napi_env env, napi_callback_info info);
    static napi_value GetRootSceneUIContext(napi_env env, napi_callback_info info);
    static napi_value SendTouchEvent(napi_env env, napi_callback_info info);
    static napi_value RequestFocusStatus(napi_env env, napi_callback_info info);
    static napi_value SetScreenLocked(napi_env env, napi_callback_info info);
    static napi_value PreloadInLakeApp(napi_env env, napi_callback_info info);
    static napi_value AddWindowDragHotArea(napi_env env, napi_callback_info info);
    static napi_value UpdateMaximizeMode(napi_env env, napi_callback_info info);
    static napi_value NotifyAINavigationBarShowStatus(napi_env env, napi_callback_info info);

private:
    napi_value OnRegisterCallback(napi_env env, napi_callback_info info);
    napi_value OnGetRootSceneSession(napi_env env, napi_callback_info info);
    napi_value OnRequestSceneSession(napi_env env, napi_callback_info info);
    napi_value OnUpdateSceneSessionWant(napi_env env, napi_callback_info info);
    napi_value OnRequestSceneSessionActivation(napi_env env, napi_callback_info info);
    napi_value OnRequestSceneSessionBackground(napi_env env, napi_callback_info info);
    napi_value OnRequestSceneSessionDestruction(napi_env env, napi_callback_info info);
    napi_value OnNotifyForegroundInteractiveStatus(napi_env env, napi_callback_info info);
    napi_value OnRequestSceneSessionByCall(napi_env env, napi_callback_info info);
    napi_value OnStartAbilityBySpecified(napi_env env, napi_callback_info info);
    napi_value OnGetWindowSceneConfig(napi_env env, napi_callback_info info);
    napi_value OnProcessBackEvent(napi_env env, napi_callback_info info);
    napi_value OnUpdateFocus(napi_env env, napi_callback_info info);
    napi_value OnSwitchUser(napi_env env, napi_callback_info info);
    napi_value OnGetSessionSnapshotFilePath(napi_env env, napi_callback_info info);
    napi_value OnInitWithRenderServiceAdded(napi_env env, napi_callback_info info);
    napi_value OnGetAllAbilityInfos(napi_env env, napi_callback_info info);
    napi_value CreateAbilityInfos(napi_env env, const std::vector<AppExecFwk::AbilityInfo>& abilityInfos);
    napi_value CreateAbilityItemInfo(napi_env env, const AppExecFwk::AbilityInfo& abilityInfo);
    napi_value CreateWindowModes(napi_env env, const std::vector<AppExecFwk::SupportWindowMode>& windowModes);
    napi_value CreateWindowSize(napi_env env, const AppExecFwk::AbilityInfo& abilityInfo);
    napi_value OnPrepareTerminate(napi_env env, napi_callback_info info);
    napi_value OnPerfRequestEx(napi_env env, napi_callback_info info);
    napi_value OnUpdateWindowMode(napi_env env, napi_callback_info info);
    napi_value OnGetRootSceneUIContext(napi_env env, napi_callback_info info);
    napi_value OnSendTouchEvent(napi_env env, napi_callback_info info);
    napi_value OnRequestFocusStatus(napi_env env, napi_callback_info info);
    napi_value OnSetScreenLocked(napi_env env, napi_callback_info info);
    napi_value OnPreloadInLakeApp(napi_env env, napi_callback_info info);
    napi_value OnAddWindowDragHotArea(napi_env env, napi_callback_info info);
    napi_value OnUpdateMaximizeMode(napi_env env, napi_callback_info info);
    napi_value OnNotifyAINavigationBarShowStatus(napi_env env, napi_callback_info info);

    void OnStatusBarEnabledUpdate(bool enable);
    void OnGestureNavigationEnabledUpdate(bool enable);
    void OnCreateSpecificSession(const sptr<SceneSession>& sceneSession);
    void OnOutsideDownEvent(int32_t x, int32_t y);
    void OnShiftFocus(int32_t persistentId);
    void OnShowPiPMainWindow(int32_t persistentId);
    void ProcessCreateSpecificSessionRegister();
    void ProcessStatusBarEnabledChangeListener();
    void ProcessGestureNavigationEnabledChangeListener();
    void ProcessOutsideDownEvent();
    void ProcessShiftFocus();
    void ProcessShowPiPMainWindow();
    bool IsCallbackRegistered(napi_env env, const std::string& type, napi_value jsListenerObject);
    void RegisterDumpRootSceneElementInfoListener();
    void RegisterVirtualPixelRatioChangeListener();
    void SetIsClearSession(napi_env env, napi_value jsSceneSessionObj, sptr<SceneSession>& sceneSession);

    napi_env env_;
    std::map<std::string, std::shared_ptr<NativeReference>> jsCbMap_;
    using Func = void(JsSceneSessionManager::*)();
    std::map<std::string, Func> listenerFunc_;

    sptr<RootScene> rootScene_;
    std::shared_ptr<MainThreadScheduler> taskScheduler_;
};
} // namespace OHOS::Rosen

#endif // OHOS_WINDOW_SCENE_JS_SCENE_SESSION_MANAGER_H