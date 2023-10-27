/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_JS_PIP_CONTROLLER_H
#define OHOS_JS_PIP_CONTROLLER_H

#include <refbase.h>
#include "js_runtime_utils.h"
#include "picture_in_picture_controller.h"

namespace OHOS {
namespace Rosen {
napi_value CreateJsPipControllerObject(napi_env env, sptr<PictureInPictureController>& pipController);
class JsPipController {
public:
    explicit JsPipController(const sptr<PictureInPictureController>& pipController);
    ~JsPipController();
    static void Finalizer(napi_env env, void* data, void* hint);
    static napi_value StartPictureInPicture(napi_env env, napi_callback_info info);
    static napi_value StopPictureInPicture(napi_env env, napi_callback_info info);
    static napi_value SetAutoStartEnabled(napi_env env, napi_callback_info info);
    static napi_value UpdateContentSize(napi_env env, napi_callback_info info);
    static napi_value RegisterCallback(napi_env env, napi_callback_info info);
    static napi_value UnregisterCallback(napi_env env, napi_callback_info info);
private:
    napi_value OnStartPictureInPicture(napi_env env, napi_callback_info info);
    napi_value OnStopPictureInPicture(napi_env env, napi_callback_info info);
    napi_value OnSetAutoStartEnabled(napi_env env, napi_callback_info info);
    napi_value OnUpdateContentSize(napi_env env, napi_callback_info info);
    napi_value OnRegisterCallback(napi_env env, napi_callback_info info);
    napi_value OnUnregisterCallback(napi_env env, napi_callback_info info);
    sptr<PictureInPictureController> pipController_;
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_JS_PIP_CONTROLLER_H
