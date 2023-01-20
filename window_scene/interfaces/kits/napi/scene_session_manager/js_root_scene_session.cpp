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

#include "utils/include/window_scene_hilog.h"

namespace OHOS::Rosen {

using namespace AbilityRuntime;

JsRootSceneSession::JsRootSceneSession(NativeEngine& engine, sptr<RootSceneSession> session)
    :JsSceneSession(engine, session)
{
}

NativeValue* JsRootSceneSession::Create(NativeEngine& engine, const sptr<RootSceneSession>& session)
{
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);

    WLOGFD("Create js root scene session.");
    auto jsRootSceneSession = std::make_unique<JsRootSceneSession>(engine, session);
    const char *moduleName = "JsRootSceneSession";
    BindNativeFunction(engine, *object, "on", moduleName, JsRootSceneSession::RegisterCallback);
    BindNativeFunction(engine, *object, "loadContent", moduleName, JsRootSceneSession::LoadContent);
    return objValue;
}

static NativeValue* LoadContent(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("Load content.");
    JsRootSceneSession* me = CheckParamsAndGetThis<JsRootSceneSession>(engine, info);
    return (me != nullptr) ? me->OnLoadContent(*engine, *info) : nullptr;
}

NativeValue* OnLoadContent(NativeEngine& engine, NativeCallbackInfo& info)
{
    // load ui content from ets file.
}
} // namespace OHOS::Rosen
