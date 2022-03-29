/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#include "js_window_extension_context.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JSWindowExtensionContext"};
}

class JsWindowExtensionContext final {
public:
    explicit JsWindowExtensionContext(const std::shared_ptr<JsWindowExtensionContext>& context) : context_(context) {}
    ~JsWindowExtensionContext() = default;

    static void Finalizer(NativeEngine* engine, void* data, void* hint)
    {
        WLOGFI("Finalizer is called");
        std::unique_ptr<JsExtensionContext>(static_cast<JsExtensionContext*>(data));
    }

    void OnWindowCreate(sptr<Window> window)
    {
        // call js api
    }

NativeValue* CreateJsServiceExtensionContext(NativeEngine& engine, std::shared_ptr<ServiceExtensionContext> context)
{
    WLOGFI("CreateJsServiceExtensionContext begin");
    NativeValue* objValue = CreateJsExtensionContext(engine, context);
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);

    std::unique_ptr<JsServiceExtensionContext> jsContext = std::make_unique<JsServiceExtensionContext>(context);
    object->SetNativePointer(jsContext.release(), JsServiceExtensionContext::Finalizer, nullptr);

    // make handler
    handler_ = std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::GetMainEventRunner());
    return objValue;
}
};
} // namespace Rosen
} // namespace OHOS