/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "js_runtime_utils.h"
#include "js_window_listener.h"
#include "native_engine/native_engine.h"
#include "native_engine/native_reference.h"
#include "native_engine/native_value.h"
#include "wm_common.h"

#ifndef OHOS_JS_WINDOW_MANAGER_H
#define OHOS_JS_WINDOW_MANAGER_H
namespace OHOS {
namespace Rosen {
NativeValue* JsWindowManagerInit(NativeEngine* engine, NativeValue* exportObj);
class JsWindowManager {
public:
    JsWindowManager() {}
    ~JsWindowManager() = default;
    static void Finalizer(NativeEngine* engine, void* data, void* hint);
    static NativeValue* CreateWindow(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* FindWindow(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* MinimizeAll(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* RegisterWindowManagerCallback(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* UnregisterWindowMangerCallback(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* GetTopWindow(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetWindowLayoutMode(NativeEngine* engine, NativeCallbackInfo* info);

private:
    bool GetNativeContext(NativeValue* nativeContext);
    bool IfCallbackRegistered(std::string type, NativeValue* jsListenerObject);
    void RegisterWmListenerWithType(NativeEngine& engine, std::string type, NativeValue* value);
    void UnregisterAllWmListenerWithType(std::string type);
    void UnregisterWmListenerWithType(std::string type, NativeValue* value);

    NativeValue* OnCreateWindow(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnFindWindow(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnMinimizeAll(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnRegisterWindowMangerCallback(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnUnregisterWindowManagerCallback(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnGetTopWindow(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetWindowLayoutMode(NativeEngine& engine, NativeCallbackInfo& info);
    std::weak_ptr<AbilityRuntime::Context> context_;
    std::map<std::string, std::map<std::unique_ptr<NativeReference>, sptr<JsWindowListener>>> jsCbMap_;
    std::mutex mtx_;
};
}  // namespace Rosen
}  // namespace OHOS

#endif
