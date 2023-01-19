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
#include <refbase.h>

namespace OHOS::Rosen {
class JsScreenSessionManager final {
public:
    JsScreenSessionManager() = default;
    ~JsScreenSessionManager() = default;

    static NativeValue* Init(NativeEngine* engine, NativeValue* exportObj);
    static void Finalizer(NativeEngine* engine, void* data, void* hint);

private:
    static NativeValue* RegisterCallback(NativeEngine* engine, NativeCallbackInfo* info);

    NativeValue* OnRegisterCallback(NativeEngine& engine, const NativeCallbackInfo& info);
    void ProcessRegisterScreenConnection(NativeEngine& engine, const std::shared_ptr<NativeReference>& callback);
};
} // namespace OHOS::Rosen

#endif // OHOS_WINDOW_SCENE_JS_SCREEN_SESSION_MANAGER_H
