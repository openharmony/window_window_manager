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

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

namespace OHOS::Rosen {
class JsSceneSessionManager final {
public:
    JsSceneSessionManager() = default;
    ~JsSceneSessionManager() = default;

    static NativeValue* Init(NativeEngine* engine, NativeValue* exportObj);
    static void Finalizer(NativeEngine* engine, void* data, void* hint);

    static NativeValue* RequestSceneSession(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* RequestSceneSessionActivation(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* RequestSceneSessionBackground(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* RequestSceneSessionDestruction(NativeEngine* engine, NativeCallbackInfo* info);

private:
    NativeValue* OnRequestSceneSession(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnRequestSceneSessionActivation(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnRequestSceneSessionBackground(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnRequestSceneSessionDestruction(NativeEngine& engine, NativeCallbackInfo& info);
};
} // namespace OHOS::Rosen

#endif // OHOS_WINDOW_SCENE_JS_SCENE_SESSION_MANAGER_H
