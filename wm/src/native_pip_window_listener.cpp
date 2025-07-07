/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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

#include "native_pip_window_listener.h"
#include "window_manager_hilog.h"
#include "picture_in_picture_manager.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;

NativePiPWindowListener::~NativePiPWindowListener()
{
    TLOGI(WmsLogTag::WMS_PIP, "~NativePiPWindowListener");
}

void NativePiPWindowListener::OnPreparePictureInPictureStart(uint32_t controllerId)
{
    OnPipListenerCallback(controllerId, PiPState::ABOUT_TO_START, 0);
}

void NativePiPWindowListener::OnPictureInPictureStart(uint32_t controllerId)
{
    OnPipListenerCallback(controllerId, PiPState::STARTED, 0);
}

void NativePiPWindowListener::OnPreparePictureInPictureStop(uint32_t controllerId)
{
    OnPipListenerCallback(controllerId, PiPState::ABOUT_TO_STOP, 0);
}

void NativePiPWindowListener::OnPictureInPictureStop(uint32_t controllerId)
{
    OnPipListenerCallback(controllerId, PiPState::STOPPED, 0);
}

void NativePiPWindowListener::OnRestoreUserInterface(uint32_t controllerId)
{
    OnPipListenerCallback(controllerId, PiPState::ABOUT_TO_RESTORE, 0);
}

void NativePiPWindowListener::OnPictureInPictureOperationError(uint32_t controllerId, int32_t errorCode)
{
    OnPipListenerCallback(controllerId, PiPState::ERROR, errorCode);
}

void NativePiPWindowListener::OnPipListenerCallback(uint32_t controllerId, PiPState state, int32_t errorCode)
{
    TLOGI(WmsLogTag::WMS_PIP, "controllerId: %{public}u, state: %{public}d, errorCode: %{public}d", controllerId,
        static_cast<int32_t>(state), errorCode);
    if (lifeCycleCallBack_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "lifeCycleCallback is nullptr");
        return;
    }
    lifeCycleCallBack_(controllerId, state, errorCode);
}

void NativePiPWindowListener::OnControlEvent(uint32_t controllerId, PiPControlType controlType,
    PiPControlStatus statusCode)
{
    if (controlEventCallBack_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "controlEventCallback is nullptr");
        return;
    }
    TLOGI(WmsLogTag::WMS_PIP, "controllerId:%{public}u, controlType:%{public}u, controlStatus:%{public}d",
        controllerId, static_cast<uint32_t>(controlType),  static_cast<int32_t>(statusCode));
    controlEventCallBack_(controllerId, controlType,  statusCode);
}

void NativePiPWindowListener::OnPipSizeChange(uint32_t controllerId, const PiPWindowSize& size)
{
    if (windowSizeCallBack_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "windowSizeCallback is nullptr");
        return;
    }
    TLOGI(WmsLogTag::WMS_PIP, "controllerId:%{public}u, width:%{public}u, height:%{public}u, scale:%{public}f",
        controllerId, size.width, size.height, size.scale);
    windowSizeCallBack_(controllerId, size.width, size.height, size.scale);
}

void NativePiPWindowListener::OnPipStart(uint32_t controllerId, uint8_t requestId, uint64_t surfaceId)
{
    if (pipStartCallback_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "startCallback is nullptr");
        return;
    }
    TLOGI(WmsLogTag::WMS_PIP, "controllerId:%{public}u, requestId:%{public}u, surfaceId:%{public}" PRIu64"",
        controllerId, requestId, surfaceId);
    pipStartCallback_(controllerId, requestId, surfaceId);
}
} // namespace Rosen
} // namespace OHOS