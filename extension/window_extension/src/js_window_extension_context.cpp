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

#include "js_runtime_utils.h"
#include "js_extension_context.h"

#include "service_extension_context.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JSWindowExtensionContext"};
    std::shared_ptr<AppExecFwk::EventHandler> handler_;
}

JsWindowExtensionContext::JsWindowExtensionContext(const std::shared_ptr<JsWindowExtensionContext>& context)
    : context_(context) {}

void JsWindowExtensionContext::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFI("Finalizer is called");
    std::unique_ptr<JsWindowExtensionContext>(static_cast<JsWindowExtensionContext*>(data));
}

void JsWindowExtensionContext::OnWindowCreate(sptr<Window> window)
{
    // call js api
}

NativeValue* CreateJsWindowExtensionContext(NativeEngine& engine,
    std::shared_ptr<AbilityRuntime::ServiceExtensionContext> context)
{
    WLOGFI("CreateJsWindowExtensionContext begin");
    NativeValue* objValue = CreateJsExtensionContext(engine, context); // TODO CreateJsBaseContext?
    NativeObject* object = AbilityRuntime::ConvertNativeValueTo<NativeObject>(objValue);

    std::unique_ptr<JsWindowExtensionContext> jsContext
        = std::make_unique<JsWindowExtensionContext>(context);
    object->SetNativePointer(jsContext.release(), JsWindowExtensionContext::Finalizer, nullptr);

    // make handler
    handler_ = std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::GetMainEventRunner());
    return objValue;
}
} // namespace Rosen
} // namespace OHOS