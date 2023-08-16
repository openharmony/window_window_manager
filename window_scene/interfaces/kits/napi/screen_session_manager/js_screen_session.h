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

#ifndef OHOS_WINDOW_SCENE_JS_SCREEN_SESSION_H
#define OHOS_WINDOW_SCENE_JS_SCREEN_SESSION_H

#include <map>

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include "session/screen/include/screen_session.h"

namespace OHOS::Rosen {
class JsScreenSession : public IScreenChangeListener {
public:
    JsScreenSession(NativeEngine& engine, const sptr<ScreenSession>& screenSession);
    ~JsScreenSession();

    static NativeValue* Create(NativeEngine& engine, const sptr<ScreenSession>& session);
    static void Finalizer(NativeEngine* engine, void* data, void* hint);

private:
    static NativeValue* RegisterCallback(NativeEngine* engine, NativeCallbackInfo* info);
    NativeValue* OnRegisterCallback(NativeEngine& engine, NativeCallbackInfo& info);
    static NativeValue* SetScreenRotationLocked(NativeEngine* engine, NativeCallbackInfo* info);
    NativeValue* OnSetScreenRotationLocked(NativeEngine& engine, NativeCallbackInfo& info);
    void CallJsCallback(const std::string& callbackType);
    void RegisterScreenChangeListener();

    void OnConnect() override;
    void OnDisconnect() override;
    void OnPropertyChange(const ScreenProperty& newProperty, ScreenPropertyChangeReason reason) override;

    NativeEngine& engine_;
    sptr<ScreenSession> screenSession_;
    std::map<std::string, std::shared_ptr<NativeReference>> mCallback_;
};
} // namespace OHOS::Rosen

#endif // OHOS_WINDOW_SCENE_JS_SCREEN_SESSION_H
