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

#include <js_runtime_utils.h>
#include <map>
#include <memory>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include <refbase.h>
#include <string>

#include "interfaces/include/ws_common.h"

namespace OHOS::Rosen {
class SceneSession;
class JsSceneSession : public std::enable_shared_from_this<JsSceneSession> {
public:
    explicit JsSceneSession(NativeEngine& engine, sptr<SceneSession> session) : engine_(engine), session_(session) {}
    ~JsSceneSession() = default;

    static void Finalizer(NativeEngine* engine, void* data, void* hint);
    static NativeValue* RegisterCallback(NativeEngine* engine, NativeCallbackInfo* info);

    void PendingSessionActivation(const SessionInfo& info);
    sptr<SceneSession> GetNativeSession() const;

private:
    NativeValue* OnRegisterCallback(NativeEngine& engine, NativeCallbackInfo& info);
    bool IsCallbackRegistered(std::string type, NativeValue* jsListenerObject);

    NativeEngine& engine_;
    sptr<SceneSession> session_;
    std::map<std::string, std::shared_ptr<NativeReference>> jsCbMap_;
    std::shared_ptr<NativeReference> jsCallBack_ = nullptr;
};

NativeValue* CreateJsSceneSessionObject(NativeEngine& engine, const sptr<SceneSession>& session);
void BindFunctions(NativeEngine& engine, NativeObject* object, const char* moduleName);
} // namespace OHOS::Rosen
#endif // OHOS_WINDOW_SCENE_JS_SCENE_SESSION_H
