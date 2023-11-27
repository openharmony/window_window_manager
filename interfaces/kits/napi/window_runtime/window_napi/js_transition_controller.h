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

#ifndef OHOS_JS_TRANSITION_CONTROLLER_H
#define OHOS_JS_TRANSITION_CONTROLLER_H

#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
class JsTransitionContext {
public:
    JsTransitionContext(sptr<Window> window, bool isShownTransContext);
    ~JsTransitionContext();
    static void Finalizer(napi_env env, void* data, void* hint);
    static napi_value CompleteTransition(napi_env env, napi_callback_info info);
private:
    napi_value OnCompleteTransition(napi_env env, napi_callback_info info);
    wptr<Window> windowToken_;
    bool isShownTransContext_ = false;
};

class JsTransitionController : public IAnimationTransitionController {
public:
    JsTransitionController(napi_env env, std::shared_ptr<NativeReference> jsWin, sptr<Window> window);
    ~JsTransitionController();
    void AnimationForShown() override;
    void AnimationForHidden() override;
    void SetJsController(std::shared_ptr<NativeReference> jsVal);
private:
    void CallJsMethod(const std::string& methodName, napi_value const * argv, size_t argc);
    napi_env env_;
    std::weak_ptr<NativeReference> jsTransControllerObj_;
    std::weak_ptr<NativeReference> jsWin_;
    wptr<Window> windowToken_;
    wptr<JsTransitionController> weakRef_  = nullptr;
};
}
}
#endif