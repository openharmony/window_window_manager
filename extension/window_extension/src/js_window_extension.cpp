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

#include "js_window_extension.h"

#include <napi_common_want.h>
#include <native_engine/native_reference.h>
#include <native_engine/native_value.h>
#include <js_extension_context.h>
#include <js_runtime_utils.h>

#include "js_window.h"
#include "js_window_extension_context.h"
#include "window_extension_connection.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JSWindowExtension"};
}

class DispatchInputEventListener : public IDispatchInputEventListener {
public:
    void OnDispatchPointerEvent(std::shared_ptr<MMI::PointerEvent>& inputEvent) override
    {
        WLOGFI("called");
    }
    void OnDispatchKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent) override
    {
        WLOGFI("called");
    }
};

JsWindowExtension* JsWindowExtension::Create(const std::unique_ptr<AbilityRuntime::Runtime>& runtime)
{
    WLOGFD("Create runtime");
    return new JsWindowExtension(static_cast<AbilityRuntime::JsRuntime&>(*runtime));
}

JsWindowExtension::JsWindowExtension(AbilityRuntime::JsRuntime& jsRuntime) : jsRuntime_(jsRuntime) {}
JsWindowExtension::~JsWindowExtension() = default;

void JsWindowExtension::Init(const std::shared_ptr<AbilityRuntime::AbilityLocalRecord> &record,
    const std::shared_ptr<AbilityRuntime::OHOSApplication> &application,
    std::shared_ptr<AbilityRuntime::AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    WindowExtension::Init(record, application, handler, token);
    std::string srcPath;
    GetSrcPath(srcPath);
    if (srcPath.empty()) {
        WLOGFE("Failed to get srcPath");
        return;
    }

    std::string moduleName(Extension::abilityInfo_->moduleName);
    moduleName.append("::").append(abilityInfo_->name);
    WLOGFI("JsWindowExtension::Init module:%{public}s,srcPath:%{public}s.", moduleName.c_str(), srcPath.c_str());
    AbilityRuntime::HandleScope handleScope(jsRuntime_);

    jsObj_ = jsRuntime_.LoadModule(moduleName, srcPath);
    if (jsObj_ == nullptr) {
        WLOGFE("Failed to get jsObj_");
        return;
    }
    WLOGFI("JsWindowExtension::Init ConvertNativeValueTo.");
    NativeObject* obj = AbilityRuntime::ConvertNativeValueTo<NativeObject>(jsObj_->Get());
    if (obj == nullptr) {
        WLOGFE("Failed to get JsWindowExtension object");
        return;
    }

    auto context = GetContext();
    if (context == nullptr) {
        WLOGFE("Failed to get context");
        return;
    }
    WLOGFI("JsWindowExtension::Init CreateJsWindowExtensionContext.");

    NativeValue* contextObj = CreateJsWindowExtensionContext(jsRuntime_.GetNativeEngine(), context);
    shellContextRef_ = jsRuntime_.LoadSystemModule("application.WindowExtensionContext", &contextObj, 1);
    contextObj = shellContextRef_->Get();
    WLOGFI("JsWindowExtension::Init Bind.");
    context->Bind(jsRuntime_, shellContextRef_.get());
    WLOGFI("JsWindowExtension::SetProperty.");
    obj->SetProperty("context", contextObj);

    auto nativeObj = AbilityRuntime::ConvertNativeValueTo<NativeObject>(contextObj);
    if (nativeObj == nullptr) {
        WLOGFE("Failed to get extension native object");
        return;
    }

    WLOGFI("Set extension context pointer: %{public}p", context.get());

    nativeObj->SetNativePointer(new std::weak_ptr<AbilityRuntime::Context>(context),
        [](NativeEngine*, void* data, void*) {
            WLOGFI("Finalizer for weak_ptr extension context is called");
            delete static_cast<std::weak_ptr<AbilityRuntime::Context>*>(data);
        }, nullptr);

    WLOGFI("JsWindowExtension::Init end.");
}

void JsWindowExtension::GetSrcPath(std::string &srcPath) const
{
    if (!Extension::abilityInfo_) {
        WLOGFE("abilityInfo_ is nullptr");
        return;
    }

    if (!Extension::abilityInfo_->isModuleJson) {
        srcPath.append(Extension::abilityInfo_->package);
        srcPath.append("/assets/js/");
        if (!Extension::abilityInfo_->srcPath.empty()) {
            srcPath.append(Extension::abilityInfo_->srcPath);
        }
        srcPath.append("/").append(Extension::abilityInfo_->name).append(".abc");
        return;
    }

    if (!Extension::abilityInfo_->srcEntrance.empty()) {
        srcPath.append(Extension::abilityInfo_->moduleName + "/");
        srcPath.append(Extension::abilityInfo_->srcEntrance);
        srcPath.erase(srcPath.rfind('.'));
        srcPath.append(".abc");
    }
}

sptr<IRemoteObject> JsWindowExtension::OnConnect(const AAFwk::Want &want)
{
    WLOGFI("called.");
    Extension::OnConnect(want);
    NativeEngine& engine = jsRuntime_.GetNativeEngine();
    std::unique_ptr<AbilityRuntime::AsyncTask::CompleteCallback> complete =
        std::make_unique<AbilityRuntime::AsyncTask::CompleteCallback>(
        [=] (NativeEngine &engine, AbilityRuntime::AsyncTask &task, int32_t status) {
        NativeEngine* nativeEngine = &jsRuntime_.GetNativeEngine();
        napi_value napiWant = OHOS::AppExecFwk::WrapWant(reinterpret_cast<napi_env>(nativeEngine), want);
        NativeValue* nativeWant = reinterpret_cast<NativeValue*>(napiWant);
        NativeValue* argv[] = { nativeWant };
        CallJsMethod("onConnect", argv, AbilityRuntime::ArraySize(argv));
        }
    );
    NativeReference* callback = nullptr;
    std::unique_ptr<AbilityRuntime::AsyncTask::ExecuteCallback> execute = nullptr;
    AbilityRuntime::AsyncTask::Schedule(engine,
        std::make_unique<AbilityRuntime::AsyncTask>(callback, std::move(execute), std::move(complete)));

    if (!stub_) {
        WLOGFE("stub is nullptr.");
        return nullptr;
    }
    WLOGFD("Create stub successfully!");
    return stub_->AsObject();
}

void JsWindowExtension::OnDisconnect(const AAFwk::Want &want)
{
    Extension::OnDisconnect(want);
    NativeEngine& engine = jsRuntime_.GetNativeEngine();
    std::unique_ptr<AbilityRuntime::AsyncTask::CompleteCallback> complete =
        std::make_unique<AbilityRuntime::AsyncTask::CompleteCallback>(
        [=] (NativeEngine &engine, AbilityRuntime::AsyncTask &task, int32_t status) {
        NativeEngine* nativeEngine = &jsRuntime_.GetNativeEngine();
        napi_value napiWant = OHOS::AppExecFwk::WrapWant(reinterpret_cast<napi_env>(nativeEngine), want);
        NativeValue* nativeWant = reinterpret_cast<NativeValue*>(napiWant);
        NativeValue* argv[] = { nativeWant };
        CallJsMethod("onDisconnect", argv, AbilityRuntime::ArraySize(argv));
        }
    );
    NativeReference* callback = nullptr;
    std::unique_ptr<AbilityRuntime::AsyncTask::ExecuteCallback> execute = nullptr;
    AbilityRuntime::AsyncTask::Schedule(engine,
       std::make_unique<AbilityRuntime::AsyncTask>(callback, std::move(execute), std::move(complete)));
    WLOGFI("called.");
}

void JsWindowExtension::OnStart(const AAFwk::Want &want)
{
    Extension::OnStart(want);

    AbilityRuntime::ElementName elementName = want.GetElement();
    std::string windowName = elementName.GetBundleName();

    stub_ = new(std::nothrow)WindowExtensionStubImpl(windowName);
    WLOGFI("JsWindowExtension OnStart begin..");
    Rect rect { want.GetIntParam(RECT_FORM_KEY_POS_X, 0),
    want.GetIntParam(RECT_FORM_KEY_POS_Y, 0),
    want.GetIntParam(RECT_FORM_KEY_WIDTH, 0),
    want.GetIntParam(RECT_FORM_KEY_HEIGHT, 0) };
    if (stub_ != nullptr) {
        auto context = GetContext();
        if (context == nullptr) {
            WLOGFE("get context failed");
            return;
        }
        sptr<Window> window = stub_->CreateWindow(rect, context);
        if (window == nullptr) {
            WLOGFE("create window failed");
            return;
        }
        OnWindowCreated();
        WLOGFI("ability context onWindowReady rect x =%{public}d y=%{public}d w=%{public}d h=%{public}d ",
            rect.posX_, rect.posY_, rect.width_, rect.height_);
    }
}

void JsWindowExtension::OnWindowCreated() const
{
    NativeEngine& engine = jsRuntime_.GetNativeEngine();
    std::unique_ptr<AbilityRuntime::AsyncTask::CompleteCallback> complete =
        std::make_unique<AbilityRuntime::AsyncTask::CompleteCallback>(
        [=] (NativeEngine &engine, AbilityRuntime::AsyncTask &task, int32_t status) {
            auto window = stub_->GetWindow();
            if (window == nullptr) {
                WLOGFE("get window failed");
                return;
            }
            NativeValue* value =  CreateJsWindowObject(engine, window);
            if (value == nullptr) {
                WLOGFE("Create js window failed");
                return;
            }
            NativeValue* argv[] = { value };
            CallJsMethod("onWindowReady", argv, AbilityRuntime::ArraySize(argv));
        }
    );

    NativeReference* callback = nullptr;
    std::unique_ptr<AbilityRuntime::AsyncTask::ExecuteCallback> execute = nullptr;
    AbilityRuntime::AsyncTask::Schedule(engine,
        std::make_unique<AbilityRuntime::AsyncTask>(callback, std::move(execute), std::move(complete)));
}

NativeValue* JsWindowExtension::CallJsMethod(const char* name, NativeValue* const* argv, size_t argc) const
{
    WLOGFI("called (%{public}s), begin", name);

    if (!jsObj_) {
        WLOGFW("Not found ServiceExtension.js");
        return nullptr;
    }

    AbilityRuntime::HandleScope handleScope(jsRuntime_);
    auto& nativeEngine = jsRuntime_.GetNativeEngine();

    NativeValue* value = jsObj_->Get();
    NativeObject* obj = AbilityRuntime::ConvertNativeValueTo<NativeObject>(value);
    if (obj == nullptr) {
        WLOGFE("Failed to get ServiceExtension object");
        return nullptr;
    }

    NativeValue* method = obj->GetProperty(name);
    if (method == nullptr || method->TypeOf() != NATIVE_FUNCTION) {
        WLOGFE("Failed to get '%{public}s' from ServiceExtension object", name);
        return nullptr;
    }
    WLOGFI("(%{public}s), success", name);
    return nativeEngine.CallFunction(value, method, argv, argc);
}
} // namespace Rosen
} // namespace OHOS
