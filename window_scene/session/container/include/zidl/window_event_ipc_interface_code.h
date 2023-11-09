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

#ifndef WINDOW_EVENT_IPC_INTERFACE_CODE_H
#define WINDOW_EVENT_IPC_INTERFACE_CODE_H

/* SAID: 4606 */
namespace OHOS {
namespace Rosen {
enum class WindowEventInterfaceCode {
    TRANS_ID_TRANSFER_KEY_EVENT = 0,
    TRANS_ID_TRANSFER_POINTER_EVENT,
    TRANS_ID_TRANSFER_FOCUS_ACTIVE_EVENT,
    TRANS_ID_TRANSFER_FOCUS_WINDOW_ID_EVENT,
    TRANS_ID_TRANSFER_FOCUS_STATE_EVENT,
    TRANS_ID_TRANSFER_BACKPRESSED_EVENT,
    TRANS_ID_TRANSFER_SEARCH_ELEMENT_INFO,
    TRANS_ID_TRANSFER_SEARCH_ELEMENT_INFO_BY_TEXT,
    TRANS_ID_TRANSFER_FIND_FOCUSED_ELEMENT_INFO,
    TRANS_ID_TRANSFER_FOCUS_MOVE_SEARCH,
};
}   // namespace Rosen
}   // namespace OHOS

#endif  // WINDOW_EVENT_IPC_INTERFACE_CODE_H