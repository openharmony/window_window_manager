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
#include "session_manager/include/screen_session_manager.h"

namespace OHOS::Rosen {
class JsScreenSessionManager final : public IScreenConnectionListener {
public:
    explicit JsScreenSessionManager(NativeEngine& engine);
    ~JsScreenSessionManager();

    static NativeValue* Init(NativeEngine* engine, NativeValue* exportObj);
    static void Finalizer(NativeEngine* engine, void* data, void* hint);

    void OnScreenConnect(sptr<ScreenSession>& screenSession) override;
    void OnScreenDisconnect(sptr<ScreenSession>& screenSession) override;

private:
    static NativeValue* RegisterCallback(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* UpdateScreenRotationProperty(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* GetCurvedCompressionArea(NativeEngine* engine, NativeCallbackInfo* info);

    NativeValue* OnRegisterCallback(NativeEngine& engine, const NativeCallbackInfo& info);
    NativeValue* OnUpdateScreenRotationProperty(NativeEngine& engine, const NativeCallbackInfo& info);
    NativeValue* OnGetCurvedCompressionArea(NativeEngine& engine, const NativeCallbackInfo& info);

    std::shared_ptr<NativeReference> screenConnectionCallback_;
    NativeEngine& engine_;
};
} // namespace OHOS::Rosen

#endif // OHOS_WINDOW_SCENE_JS_SCREEN_SESSION_MANAGER_H
