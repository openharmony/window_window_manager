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

#ifndef OHOS_ROSEN_SCREEN_SESSION_MANAGER_LITE_INTERFACE_H
#define OHOS_ROSEN_SCREEN_SESSION_MANAGER_LITE_INTERFACE_H

#include <iremote_broker.h>
#include <cinttypes>
#include "dm_common.h"
#include "zidl/display_manager_agent_interface.h"
#include "display_info.h"
#include "fold_screen_info.h"
#include "screen_group_info.h"

namespace OHOS {
namespace Rosen {

class IScreenSessionManagerLite : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IScreenSessionManagerLite");

    enum class ScreenManagerLiteMessage : uint32_t {
        TRANS_ID_GET_DEFAULT_DISPLAY_INFO = 0,
        TRANS_ID_GET_DISPLAY_BY_ID,
        TRANS_ID_GET_DISPLAY_BY_SCREEN,
        TRANS_ID_DISABLE_DISPLAY_SNAPSHOT,
        TRANS_ID_REGISTER_DISPLAY_MANAGER_AGENT,
        TRANS_ID_UNREGISTER_DISPLAY_MANAGER_AGENT,
        TRANS_ID_WAKE_UP_BEGIN,
        TRANS_ID_WAKE_UP_END,
        TRANS_ID_SUSPEND_BEGIN,
        TRANS_ID_SUSPEND_END,
        TRANS_ID_SET_SPECIFIED_SCREEN_POWER,
        TRANS_ID_SET_SCREEN_POWER_FOR_ALL,
        TRANS_ID_GET_SCREEN_POWER,
        TRANS_ID_SET_DISPLAY_STATE,
        TRANS_ID_GET_DISPLAY_STATE,
        TRANS_ID_GET_ALL_DISPLAYIDS,
        TRANS_ID_NOTIFY_DISPLAY_EVENT,
        TRANS_ID_GET_SCREEN_INFO_BY_ID,
        TRANS_ID_GET_SCREEN_GROUP_INFO_BY_ID,
        TRANS_ID_SET_SCREEN_ACTIVE_MODE,
        TRANS_ID_GET_ALL_SCREEN_INFOS,
        TRANS_ID_SET_ORIENTATION,
        TRANS_ID_SET_VIRTUAL_PIXEL_RATIO,
        TRANS_ID_IS_SCREEN_ROTATION_LOCKED,
        TRANS_ID_SET_SCREEN_ROTATION_LOCKED,
        TRANS_ID_HAS_PRIVATE_WINDOW,
        TRANS_ID_SCENE_BOARD_SET_FOLD_DISPLAY_MODE,
        TRANS_ID_SCENE_BOARD_GET_FOLD_DISPLAY_MODE,
        TRANS_ID_SCENE_BOARD_IS_FOLDABLE,
        TRANS_ID_SCENE_BOARD_GET_FOLD_STATUS,
        TRANS_ID_SCENE_BOARD_GET_CURRENT_FOLD_CREASE_REGION,
    };

    virtual DMError RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) { return DMError::DM_OK; }
    virtual DMError UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) { return DMError::DM_OK; }
    virtual std::vector<DisplayId> GetAllDisplayIds() { return std::vector<DisplayId>{}; }
    virtual sptr<DisplayInfo> GetDisplayInfoById(DisplayId displayId) { return nullptr; }
    virtual sptr<DisplayInfo> GetDefaultDisplayInfo() { return nullptr; }
    virtual sptr<DisplayInfo> GetDisplayInfoByScreen(ScreenId screenId) {return nullptr; }
    virtual DMError HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow) { return DMError::DM_OK; }
    virtual DMError DisableDisplaySnapshot(bool disableOrNot) { return DMError::DM_ERROR_INVALID_PERMISSION; }
    virtual bool WakeUpBegin(PowerStateChangeReason reason) { return false; }
    virtual bool WakeUpEnd() { return false; }
    virtual bool SuspendBegin(PowerStateChangeReason reason) { return false; }
    virtual bool SuspendEnd() { return false; }
    virtual bool SetDisplayState(DisplayState state) { return false; }
    virtual DisplayState GetDisplayState(DisplayId displayId) {return DisplayState::UNKNOWN; }
    virtual void NotifyDisplayEvent(DisplayEvent event) {}
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_SCREEN_SESSION_MANAGER_LITE_INTERFACE_H
