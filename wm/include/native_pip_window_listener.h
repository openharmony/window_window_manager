/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_NATIVE_PIP_WINDOW_LISTENER_H
#define OHOS_NATIVE_PIP_WINDOW_LISTENER_H

#include "picture_in_picture_interface.h"
#include "refbase.h"

namespace OHOS {
namespace Rosen {
typedef void (*NativePipLifeCycleCallback)(uint32_t controllerId, PiPState state, int32_t errcode);
typedef void (*NativePipControlEventCallback)(uint32_t controllerId, PiPControlType controlType,
    PiPControlStatus status);
typedef void (*NativePipResizeCallback)(uint32_t controllerId, uint32_t width, uint32_t height, double scale);
typedef void (*NativePipStartPipCallback)(uint32_t controllerId, uint8_t requestId, uint64_t surfaceId);

class NativePiPWindowListener : public IPiPLifeCycle,
                            public IPiPControlObserver,
                            public IPiPWindowSize,
                            public IPiPStartObserver {
public:
    explicit NativePiPWindowListener(NativePipLifeCycleCallback callback) : lifeCycleCallBack_(callback) {}
    explicit NativePiPWindowListener(NativePipControlEventCallback callback) : controlEventCallBack_(callback) {}
    explicit NativePiPWindowListener(NativePipResizeCallback callback) : windowSizeCallBack_(callback) {}
    explicit NativePiPWindowListener(NativePipStartPipCallback callback) : pipStartCallback_(callback) {}
    ~NativePiPWindowListener();
    NativePipLifeCycleCallback GetLifeCycleCallbackRef() const { return lifeCycleCallBack_; }
    NativePipControlEventCallback GetControlEventCallbackRef() const { return controlEventCallBack_; }
    NativePipResizeCallback GetResizeCallbackRef() const { return windowSizeCallBack_; }
    NativePipStartPipCallback GetStartPipCallbackRef() const { return pipStartCallback_; }
    void OnPreparePictureInPictureStart(uint32_t controllerId) override;
    void OnPictureInPictureStart(uint32_t controllerId) override;
    void OnPreparePictureInPictureStop(uint32_t controllerId) override;
    void OnPictureInPictureStop(uint32_t controllerId) override;
    void OnPictureInPictureOperationError(uint32_t controllerId, int32_t errorCode) override;
    void OnRestoreUserInterface(uint32_t controllerId) override;
    void OnControlEvent(uint32_t controllerId, PiPControlType controlType, PiPControlStatus statusCode) override;
    void OnPipSizeChange(uint32_t controllerId, const PiPWindowSize& size) override;
    void OnPipStart(uint32_t controllerId, uint8_t requestId, uint64_t surfaceId) override;

private:
    void OnPipListenerCallback(uint32_t controllerId, PiPState state, int32_t errorCode);
    NativePipLifeCycleCallback lifeCycleCallBack_ = nullptr;
    NativePipControlEventCallback controlEventCallBack_ = nullptr;
    NativePipResizeCallback windowSizeCallBack_ = nullptr;
    NativePipStartPipCallback pipStartCallback_ = nullptr;
};
}  // namespace Rosen
}  // namespace OHOS
#endif /* OHOS_NATIVE_PIP_WINDOW_LISTENER_H */