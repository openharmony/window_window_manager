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

#include <map>
#include <refbase.h>
#include "js_runtime_utils.h"
#include "picture_in_picture_controller.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
napi_value CreateJsPipControllerObject(napi_env env, sptr<PictureInPictureController>& pipController);
class JsPipController {
public:
    explicit JsPipController(const sptr<PictureInPictureController>& pipController, napi_env env);
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

    bool IfCallbackRegistered(napi_env env, const std::string& type, napi_value jsListenerObject);
    WmErrorCode RegisterListenerWithType(napi_env env, const std::string& type, napi_value value);
    WmErrorCode UnRegisterListenerWithType(napi_env env, const std::string& type);

    void ProcessStateChangeRegister();
    void ProcessActionEventRegister();
    void ProcessStateChangeUnRegister();
    void ProcessActionEventUnRegister();

    sptr<PictureInPictureController> pipController_;
    napi_env env_;
    using Func = void(JsPipController::*)();
    std::map<std::string, Func> registerFunc_;
    std::map<std::string, Func> unRegisterFunc_;
    std::map<std::string, std::shared_ptr<NativeReference>> jsCbMap_;
    std::mutex mtx_;
    static std::mutex pipMutex_;

public:
    class PiPLifeCycleImpl : public IPiPLifeCycle {
    public:
        PiPLifeCycleImpl(napi_env env, std::shared_ptr<NativeReference> callback)
            : engine_(env), jsCallBack_(callback) {}
        ~PiPLifeCycleImpl() {}
        void OnPreparePictureInPictureStart() override;
        void OnPictureInPictureStart() override;
        void OnPreparePictureInPictureStop() override;
        void OnPictureInPictureStop() override;
        void OnPictureInPictureOperationError(int32_t errorCode) override;
        void OnRestoreUserInterface() override;

    private:
        void OnPipListenerCallback(PiPState state, int32_t errorCode);
        napi_env engine_ = nullptr;
        std::shared_ptr<NativeReference> jsCallBack_ = nullptr;
        std::mutex mtx_;
    };

    class PiPActionObserverImpl : public IPiPActionObserver {
    public:
        PiPActionObserverImpl(napi_env env, std::shared_ptr<NativeReference> callback)
            : engine_(env), jsCallBack_(callback) {}
        ~PiPActionObserverImpl() {}
        void OnActionEvent(const std::string& actionEvent) override;
    private:
        napi_env engine_ = nullptr;
        std::shared_ptr<NativeReference> jsCallBack_ = nullptr;
        std::mutex mtx_;
    };
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_JS_PIP_CONTROLLER_H
