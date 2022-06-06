/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_JS_SCREEN_RECORDER_H
#define OHOS_JS_SCREEN_RECORDER_H
#include "js_runtime_utils.h"
#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"
#include "screen_recorder.h"

namespace OHOS {
namespace Rosen {
NativeValue* JsScreenRecorderInit(NativeEngine* engine, NativeValue* exportObj);
NativeValue* CreateJsVirtualScreenRecorderObject(NativeEngine& engine, sptr<ScreenRecorder>& screenRecorder);
class JsScreenRecorder {
public:
    explicit JsScreenRecorder(NativeEngine* engine) {
    }

    ~JsScreenRecorder() = default;

    static void Finalizer(NativeEngine* engine, void* data, void* hint);
    static NativeValue* GetVirtualScreenRecorder(NativeEngine* engine, NativeCallbackInfo* info);
private:
    NativeValue* OnGetVirtualScreenRecorder(NativeEngine& engine, NativeCallbackInfo& info);
};

class JsVirtualScreenRecorder final {
public:
    explicit JsVirtualScreenRecorder(const sptr<ScreenRecorder>&);
    ~JsVirtualScreenRecorder();
    static void Finalizer(NativeEngine* engine, void* data, void* hint);
    static NativeValue* GetInputSurface(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* Start(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* Stop(NativeEngine* engine, NativeCallbackInfo* info);

private:
    sptr<ScreenRecorder> screenRecorder_ = nullptr;
    NativeValue* OnGetInputSurface(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnStart(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnStop(NativeEngine& engine, NativeCallbackInfo& info);
};
}  // namespace Rosen
}  // namespace OHOS
#endif