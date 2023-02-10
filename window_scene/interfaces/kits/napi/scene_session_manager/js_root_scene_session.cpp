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

#include "js_root_scene_session.h"

#include <js_runtime_utils.h>
#include "context.h"

#include "utils/include/window_scene_hilog.h"

namespace OHOS::Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsRootSceneSession"};
}

JsRootSceneSession::JsRootSceneSession(NativeEngine& engine, const sptr<RootSceneSession>& rootSceneSession)
    : rootSceneSession_(rootSceneSession)
{
}

NativeValue* JsRootSceneSession::Create(NativeEngine& engine, const sptr<RootSceneSession>& rootSceneSession)
{
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);

    WLOGFD("Create js root scene session.");
    auto jsRootSceneSession = std::make_unique<JsRootSceneSession>(engine, rootSceneSession);
    object->SetNativePointer(jsRootSceneSession.release(), JsRootSceneSession::Finalizer, nullptr);

    const char *moduleName = "JsRootSceneSession";
    // BindNativeFunction(engine, *object, "on", moduleName, JsRootSceneSession::RegisterCallback);
    BindNativeFunction(engine, *object, "loadContent", moduleName, JsRootSceneSession::LoadContent);
    return objValue;
}

void JsRootSceneSession::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFD("Finalizer.");
    std::unique_ptr<JsRootSceneSession>(static_cast<JsRootSceneSession*>(data));
}

NativeValue* JsRootSceneSession::LoadContent(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("Load content.");
    JsRootSceneSession* me = CheckParamsAndGetThis<JsRootSceneSession>(engine, info);
    return (me != nullptr) ? me->OnLoadContent(*engine, *info) : nullptr;
}

NativeValue* JsRootSceneSession::OnLoadContent(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFD("[NAPI]OnLoadContent");
    if (info.argc < 2) { // 2: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    std::string contentUrl;
    NativeValue* context = info.argv[1];
    NativeValue* storage = info.argc < 3 ? nullptr : info.argv[2];
    if (!ConvertFromJsValue(engine, info.argv[0], contentUrl)) {
        WLOGFE("[NAPI]Failed to convert parameter to content url");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_INVALID_PARAM),
            "Input parameter is missing or invalid"));
        return engine.CreateUndefined();
    }
    auto contextObj = ConvertNativeValueTo<NativeObject>(context);
    if (contextObj == nullptr) {
        WLOGFE("[NAPI]Failed to get context object");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    auto contextNativePointer = static_cast<AbilityRuntime::Context*>(contextObj->GetNativePointer());
    if (contextNativePointer == nullptr) {
        WLOGFE("[NAPI]Failed to get context pointer from js object");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }

    std::shared_ptr<NativeReference> contentStorage = (storage == nullptr) ? nullptr :
        std::shared_ptr<NativeReference>(engine.CreateReference(storage, 1));
    NativeValue* nativeStorage = contentStorage ? contentStorage->Get() : nullptr;
    AsyncTask::CompleteCallback complete =
        [rootSceneSession = rootSceneSession_, contentUrl, contextNativePointer, nativeStorage](
            NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (rootSceneSession == nullptr) {
                WLOGFE("[NAPI]rootSceneSession is nullptr");
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WSErrorCode::WS_ERROR_STATE_ABNORMALLY)));
                return;
            }
            rootSceneSession->LoadContent(contentUrl, &engine, nativeStorage, contextNativePointer);
        };
    NativeValue* lastParam = nullptr;
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsRootSceneSession::OnLoadContent",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}
} // namespace OHOS::Rosen
