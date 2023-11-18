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

#ifndef OHOS_ROSEN_SESSION_IPC_INTERFACE_CODE_H
#define OHOS_ROSEN_SESSION_IPC_INTERFACE_CODE_H

namespace OHOS {
namespace Rosen {
enum class SessionInterfaceCode {
    // Base
    TRANS_ID_CONNECT = 0,
    TRANS_ID_FOREGROUND,
    TRANS_ID_BACKGROUND,
    TRANS_ID_DISCONNECT,
    TRANS_ID_ACTIVE_PENDING_SESSION,
    TRANS_ID_UPDATE_ACTIVE_STATUS,
    TRANS_ID_TERMINATE,
    TRANS_ID_EXCEPTION,

    // Scene
    TRANS_ID_SESSION_EVENT = 100,
    TRANS_ID_UPDATE_SESSION_RECT,
    TRANS_ID_CREATE_AND_CONNECT_SPECIFIC_SESSION,
    TRANS_ID_DESTROY_AND_DISCONNECT_SPECIFIC_SESSION,
    TRANS_ID_RAISE_TO_APP_TOP,
    TRANS_ID_BACKPRESSED,
    TRANS_ID_MARK_PROCESSED,
    TRANS_ID_SET_MAXIMIZE_MODE,
    TRANS_ID_GET_MAXIMIZE_MODE,
    TRANS_ID_NEED_AVOID,
    TRANS_ID_GET_AVOID_AREA,
    TRANS_ID_SET_SESSION_PROPERTY,
    TRANS_ID_SET_ASPECT_RATIO,
    TRANS_ID_UPDATE_WINDOW_ANIMATION_FLAG,
    TRANS_ID_UPDATE_CUSTOM_ANIMATION,
    TRANS_ID_RAISE_ABOVE_TARGET,

    // Extension
    TRANS_ID_TRANSFER_ABILITY_RESULT = 500,
    TRANS_ID_TRANSFER_EXTENSION_DATA = 501,
    TRANS_ID_NOTIFY_REMOTE_READY = 502,
    TRANS_ID_NOTIFY_EXTENSION_DIED = 503,
    TRANS_ID_NOTIFY_ASYNC_ON,
    TRANS_ID_NOTIFY_SYNC_ON,
    TRANS_ID_NOTIFY_REPORT_ACCESSIBILITY_EVENT,

    //PictureInPicture
    TRANS_ID_NOTIFY_PIP_WINDOW_PREPARE_CLOSE = 800
};
} // namespace Rosen
} // namespace OHOS

#endif  // OHOS_ROSEN_SESSION_IPC_INTERFACE_CODE_H
