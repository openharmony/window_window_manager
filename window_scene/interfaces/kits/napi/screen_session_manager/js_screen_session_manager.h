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

#ifndef OHOS_WINDOW_SCENE_JS_SCREEN_SESSION_MANAGER_H
#define OHOS_WINDOW_SCENE_JS_SCREEN_SESSION_MANAGER_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

#include "screen_session_manager_client.h"

#ifdef POWER_MANAGER_ENABLE
#include "shutdown/takeover_shutdown_callback_stub.h"
#endif

namespace OHOS::Rosen {
class JsScreenSessionManager final : public IScreenConnectionListener,
    public PowerMgr::TakeOverShutdownCallbackStub {
public:
    explicit JsScreenSessionManager(napi_env env);
    virtual ~JsScreenSessionManager() = default;

    static napi_value Init(napi_env env, napi_value exportObj);
    static void Finalizer(napi_env env, void* data, void* hint);

    void OnScreenConnected(const sptr<ScreenSession>& screenSession) override;
    void OnScreenDisconnected(const sptr<ScreenSession>& screenSession) override;
    bool OnTakeOverShutdown(bool isReboot) override;

private:
    static napi_value RegisterCallback(napi_env env, napi_callback_info info);
    static napi_value UpdateScreenRotationProperty(napi_env env, napi_callback_info info);
    static napi_value GetCurvedCompressionArea(napi_env env, napi_callback_info info);
    static napi_value RegisterShutdownCallback(napi_env env, napi_callback_info info);
    static napi_value UnRegisterShutdownCallback(napi_env env, napi_callback_info info);
    static napi_value GetPhyScreenProperty(napi_env env, napi_callback_info info);
    static napi_value NotifyScreenLockEvent(napi_env env, napi_callback_info info);
    static napi_value UpdateAvailableArea(napi_env env, napi_callback_info info);
    static napi_value NotifyFoldToExpandCompletion(napi_env env, napi_callback_info info);
    static napi_value GetFoldStatus(napi_env env, napi_callback_info info);
    static napi_value GetScreenSnapshot(napi_env env, napi_callback_info info);

    napi_value OnRegisterCallback(napi_env env, const napi_callback_info info);
    napi_value OnUpdateScreenRotationProperty(napi_env env, const napi_callback_info info);
    napi_value OnGetCurvedCompressionArea(napi_env env, const napi_callback_info info);
    napi_value OnRegisterShutdownCallback(napi_env env, const napi_callback_info info);
    napi_value OnUnRegisterShutdownCallback(napi_env env, const napi_callback_info info);
    napi_value OnGetPhyScreenProperty(napi_env env, const napi_callback_info info);
    napi_value OnNotifyScreenLockEvent(napi_env env, const napi_callback_info info);
    napi_value OnUpdateAvailableArea(napi_env env, const napi_callback_info info);
    napi_value OnNotifyFoldToExpandCompletion(napi_env env, const napi_callback_info info);
    napi_value OnGetFoldStatus(napi_env env, const napi_callback_info info);
    napi_value OnGetScreenSnapshot(napi_env env, const napi_callback_info info);

    std::shared_ptr<NativeReference> screenConnectionCallback_;
    std::shared_ptr<NativeReference> shutdownCallback_;
    napi_env env_;
};
} // namespace OHOS::Rosen

#endif // OHOS_WINDOW_SCENE_JS_SCREEN_SESSION_MANAGER_H
