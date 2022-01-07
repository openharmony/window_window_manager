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

#ifndef OHOS_JS_WINDOW_LISTENER_H
#define OHOS_JS_WINDOW_LISTENER_H

#include <map>
#include <mutex>
#include <unordered_set>
#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"
#include "window.h"
namespace OHOS {
namespace Rosen {
class JsWindowListener : public IWindowChangeListener {
public:
    explicit JsWindowListener(NativeEngine* engine) : engine_(engine) {}
    virtual ~JsWindowListener() = default;
    void OnSizeChange(Rect rect) override;
    bool AddJsListenerObject(std::string type, NativeValue* jsListenerObject);
    void RemoveJsListenerObject(std::string type, NativeValue* jsListenerObject);
private:
    void CallJsMethod(const char* methodName, NativeValue* const* argv = nullptr, size_t argc = 0);
    bool IsCallbackExists(std::string type, NativeValue* jsListenerObject);
    NativeEngine* engine_ = nullptr;
    std::map<std::string, std::unordered_set<std::unique_ptr<NativeReference>>> jsWinodwListenerObjectMap_;
    std::mutex listenerMutex_;
};
}  // namespace Rosen
}  // namespace OHOS
#endif /* OHOS_JS_WINDOW_LISTENER_H */