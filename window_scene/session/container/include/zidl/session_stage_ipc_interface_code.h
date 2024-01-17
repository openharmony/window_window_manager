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

#ifndef SESSION_STAGE_IPC_INTERFACE_CODE_H
#define SESSION_STAGE_IPC_INTERFACE_CODE_H

/* SAID: 4606 */
namespace OHOS {
namespace Rosen {
enum class SessionStageInterfaceCode {
    TRANS_ID_SET_ACTIVE = 0,
    TRANS_ID_NOTIFY_SIZE_CHANGE,
    TRANS_ID_HANDLE_BACK_EVENT,
    TRANS_ID_NOTIFY_FOCUS_CHANGE,
    TRANS_ID_NOTIFY_DESTROY,
    TRANS_ID_NOTIFY_TRANSFER_COMPONENT_DATA,
    TRANS_ID_NOTIFY_OCCUPIED_AREA_CHANGE_INFO,
    TRANS_ID_UPDATE_AVOID_AREA,
    TRANS_ID_NOTIFY_SCREEN_SHOT,
    TRANS_ID_DUMP_SESSSION_ELEMENT_INFO,
    TRANS_ID_NOTIFY_TOUCH_OUTSIDE,
    TRANS_ID_NOTIFY_WINDOW_MODE_CHANGE,
    TRANS_ID_NOTIFY_DENSITY_CHANGE,
    TRANS_ID_NOTIFY_FOREGROUND_INTERACTIVE_STATUS,
    TRANS_ID_NOTIFY_CONFIGURATION_UPDATED,
    TRANS_ID_NOTIFY_MAXIMIZE_MODE_CHANGE,
    TRANS_ID_NOTIFY_CLOSE_EXIST_PIP_WINDOW,
    TRANS_ID_NOTIFY_SESSION_FOREGROUND,
    TRANS_ID_NOTIFY_SESSION_BACKGROUND,
    TRANS_ID_NOTIFY_TRANSFER_COMPONENT_DATA_SYNC,
    TRANS_ID_NOTIFY_TITLE_POSITION_CHANGE,
    TRANS_ID_UPDATE_WINDOW_DRAWING_STATUS,
    TRANS_ID_NOTIFY_WINDOW_VISIBILITY_CHANGE,
    TRANS_ID_NOTIFY_TRANSFORM_CHANGE,
    TRANS_ID_NOTIFY_DIALOG_STATE_CHANGE,
};
}   // namespace Rosen
}   // namespace OHOS

#endif  // SESSION_STAGE_IPC_INTERFACE_CODE_H