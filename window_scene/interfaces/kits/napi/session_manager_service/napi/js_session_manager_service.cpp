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

#include "js_session_manager_service.h"

#include "js_runtime_utils.h"

#include "session_manager_service.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
using namespace OHOS::AbilityRuntime;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "JsSessionManagerService" };
} // namespace

class JsSessionManagerService {
public:
    JsSessionManagerService() = default;
    ~JsSessionManagerService() = default;

    static void Finalizer(NativeEngine* engine, void* data, void* hint)
    {
        WLOGI("Finalizer is called");
        std::unique_ptr<JsSessionManagerService>(static_cast<JsSessionManagerService*>(data));
    }

    static NativeValue* InitSessionManagerService(NativeEngine* engine, NativeCallbackInfo* info)
    {
        JsSessionManagerService* me = CheckParamsAndGetThis<JsSessionManagerService>(engine, info);
        return (me != nullptr) ? me->OnInitSessionManagerService(*engine, *info) : nullptr;
    }

private:
    NativeValue* OnInitSessionManagerService(NativeEngine& engine, NativeCallbackInfo& info)
    {
        WLOGI("JsSessionManagerService: OnInitSessionManagerService is called");
        SessionManagerService::GetInstance().Init();
        return engine.CreateUndefined();
    }
};

NativeValue* JsSessionManagerServiceInit(NativeEngine* engine, NativeValue* exportObj)
{
    WLOGI("JsSessionManagerServiceInit is called.");
    if (engine == nullptr || exportObj == nullptr) {
        WLOGFE("JsSessionManagerServiceInit engine or exportObj is nullptr");
        return nullptr;
    }

    NativeObject* object = ConvertNativeValueTo<NativeObject>(exportObj);
    if (object == nullptr) {
        WLOGFE("JsSessionManagerServiceInit object is nullptr");
        return nullptr;
    }

    auto jsSessionManagerService = std::make_unique<JsSessionManagerService>();
    object->SetNativePointer(jsSessionManagerService.release(), JsSessionManagerService::Finalizer, nullptr);

    const char* moduleName = "JsSessionManagerService";
    BindNativeFunction(*engine, *object, "initSessionManagerService", moduleName,
        JsSessionManagerService::InitSessionManagerService);
    return engine->CreateUndefined();
}
} // namespace OHOS::Rosen
