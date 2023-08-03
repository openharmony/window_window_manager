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

#ifndef OHOS_WINDOW_SCENE_JS_ROOT_SCENE_SESSION_H
#define OHOS_WINDOW_SCENE_JS_ROOT_SCENE_SESSION_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include "session/host/include/root_scene_session.h"

namespace OHOS::Rosen {
class JsRootSceneSession : public RefBase {
public:
    JsRootSceneSession(NativeEngine& engine, const sptr<RootSceneSession>& rootSceneSession);
    ~JsRootSceneSession() = default;

    static NativeValue* Create(NativeEngine& engine, const sptr<RootSceneSession>& rootSceneSession);
    static void Finalizer(NativeEngine* engine, void* data, void* hint);

    static NativeValue* RegisterCallback(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* LoadContent(NativeEngine* engine, NativeCallbackInfo* info);

private:
    NativeValue* OnRegisterCallback(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnLoadContent(NativeEngine& engine, NativeCallbackInfo& info);
    bool IsCallbackRegistered(std::string type, NativeValue* jsListenerObject);
    void PendingSessionActivation(SessionInfo& info);
    sptr<SceneSession> GenSceneSession(SessionInfo& info);

    NativeEngine& engine_;
    std::map<std::string, std::shared_ptr<NativeReference>> jsCbMap_;
    sptr<RootSceneSession> rootSceneSession_;
};
} // namespace OHOS::Rosen

#endif // OHOS_WINDOW_SCENE_JS_ROOT_SCENE_SESSION_H
