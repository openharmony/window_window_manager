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

#ifndef OHOS_WINDOW_SCENE_JS_SCENE_SESSION_H
#define OHOS_WINDOW_SCENE_JS_SCENE_SESSION_H

#include <map>

#include <js_runtime_utils.h>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include <refbase.h>

#include "interfaces/include/ws_common.h"
#include "session/host/include/scene_session.h"
#include "js_scene_utils.h"

namespace OHOS::Rosen {
class SceneSession;
class JsSceneSession : public std::enable_shared_from_this<JsSceneSession> {
public:
    JsSceneSession(NativeEngine& engine, const sptr<SceneSession>& session);
    ~JsSceneSession();

    static NativeValue* Create(NativeEngine& engine, const sptr<SceneSession>& session);
    static void Finalizer(NativeEngine* engine, void* data, void* hint);

    sptr<SceneSession> GetNativeSession() const;

private:
    static NativeValue* RegisterCallback(NativeEngine* engine, NativeCallbackInfo* info);
    NativeValue* OnRegisterCallback(NativeEngine& engine, NativeCallbackInfo& info);
    bool IsCallbackRegistered(const std::string& type, NativeValue* jsListenerObject);

    void ProcessPendingSceneSessionActivationRegister();
    void ProcessSessionStateChangeRegister();
    void ProcessSessionEventRegister();
    void ProcessCreateSpecificSessionRegister();
    void ProcessSessionRectChangeRegister();
    void ProcessRaiseToTopRegister();
    void ProcessBackPressedRegister();
    void ProcessSessionFocusableChangeRegister();
    void ProcessClickRegister();
    void ProcessTerminateSessionRegister();
    void ProcessSessionExceptionRegister();
    void ProcessSystemBarPropertyChangeRegister();
    void ProcessNeedAvoidRegister();

    void PendingSessionActivation(SessionInfo& info);
    void OnSessionStateChange(const SessionState& state);
    void OnSessionEvent(uint32_t actionId);
    void OnCreateSpecificSession(const sptr<SceneSession>& session);
    void OnSessionRectChange(const WSRect& rect);
    void OnRaiseToTop();
    void OnBackPressed();
    void OnSessionFocusableChange(bool isFocusable);
    void OnClick();
    void TerminateSession(const SessionInfo& info);
    void OnSessionException(const SessionInfo& info);
    static JsSessionType GetApiType(WindowType type);
    void OnSystemBarPropertyChange(const SystemBarProperty& property);
    void OnNeedAvoid(bool status);

    NativeEngine& engine_;
    wptr<SceneSession> weakSession_ = nullptr;
    wptr<SceneSession::SessionChangeCallback> sessionchangeCallback_ = nullptr;
    std::map<std::string, std::shared_ptr<NativeReference>> jsCbMap_;
    using Func = void(JsSceneSession::*)();
    std::map<std::string, Func> listenerFunc_;
};
} // namespace OHOS::Rosen

#endif // OHOS_WINDOW_SCENE_JS_SCENE_SESSION_H
