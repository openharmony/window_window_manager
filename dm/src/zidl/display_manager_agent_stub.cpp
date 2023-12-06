/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "zidl/display_manager_agent_stub.h"

#include <vector>

#include "display_info.h"
#include "display_change_info.h"
#include "dm_common.h"
#include "marshalling_helper.h"
#include "screen_info.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManagerAgentStub"};
}

int32_t DisplayManagerAgentStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    WLOGFD("code:%{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return -1;
    }
    switch (code) {
        case TRANS_ID_NOTIFY_DISPLAY_POWER_EVENT: {
            NotifyDisplaypowerevent();
            break;
        }
        case TRANS_ID_NOTIFY_DISPLAY_STATE_CHANGED: {
            NotifyDisplaystateChanged();
            break;
        }
        case TRANS_ID_ON_SCREEN_CONNECT: {
            ScreenConnect();
            break;
        }
        case TRANS_ID_ON_SCREEN_DISCONNECT: {
            ScreenDisconnect();
            break;
        }
        case TRANS_ID_ON_SCREEN_CHANGED: {
            ScreenChanged();
            break;
        }
        case TRANS_ID_ON_SCREENGROUP_CHANGED: {
            ScreenGroupChanged();
            break;
        }
        case TRANS_ID_ON_DISPLAY_CONNECT: {
            DisplayConnect();
            break;
        }
        case TRANS_ID_ON_DISPLAY_DISCONNECT: {
            DisplayDisconnect();
            break;
        }
        case TRANS_ID_ON_DISPLAY_CHANGED: {
            DisplayChanged();
            break;
        }
        case TRANS_ID_ON_SCREEN_SHOT: {
            ScreenShot();
            break;
        }
        case TRANS_ID_ON_PRIVATE_WINDOW: {
            PrivateWindow();
            break;
        }
        case TRANS_ID_ON_FOLD_STATUS_CHANGED: {
            FoldStatusChanged();
            break;
        }
        case TRANS_ID_ON_DISPLAY_CHANGE_INFO_CHANGED: {
            DisplayChangeInfoChanged();
            break;
        }
        case TRANS_ID_ON_DISPLAY_MODE_CHANGED: {
            DisplayModechanged();
            break;
        }
        case TRANS_ID_ON_AVAILABLE_AREA_CHANGED: {
            AvailableAreaChanged();
            break;
        }
        default: {
            WLOGFW("unknown transaction code %{public}d", code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return 0;
}

void DisplayManagerAgentStub::NotifyDisplaypowerevent()
{
    DisplayPowerEvent event = static_cast<DisplayPowerEvent>(data.ReadUint32());
    EventStatus status = static_cast<EventStatus>(data.ReadUint32());
    NotifyDisplayPowerEvent(event, status);
}

void DisplayManagerAgentStub::NotifyDisplaystateChanged()
{
    DisplayState state = static_cast<DisplayState>(data.ReadUint32());
    DisplayId id = static_cast<DisplayId>(data.ReadUint64());
    NotifyDisplayStateChanged(id, state);
}

void DisplayManagerAgentStub::ScreenConnect()
{
    sptr<ScreenInfo> screenInfo = data.ReadParcelable<ScreenInfo>();
    OnScreenConnect(screenInfo);
}

void DisplayManagerAgentStub::ScreenDisconnect()
{
    ScreenId screenId;
    if (!data.ReadUint64(screenId)) {
        WLOGFE("Read ScreenId failed");
        return -1;
    }
    OnScreenDisconnect(screenId);
}

void DisplayManagerAgentStub::ScreenChanged()
{
    sptr<ScreenInfo> screenInfo = data.ReadParcelable<ScreenInfo>();
    uint32_t event;
    if (!data.ReadUint32(event)) {
        WLOGFE("Read ScreenChangeEvent failed");
        return -1;
    }
    OnScreenChange(screenInfo, static_cast<ScreenChangeEvent>(event));
}

void DisplayManagerAgentStub::ScreenGroupChanged()
{
    std::string trigger;
    if (!data.ReadString(trigger)) {
        WLOGFE("Read trigger failed");
        return -1;
    }
    std::vector<sptr<ScreenInfo>> screenInfos;
    if (!MarshallingHelper::UnmarshallingVectorParcelableObj<ScreenInfo>(data, screenInfos)) {
        WLOGFE("Read ScreenInfo failed");
        return -1;
    }
    uint32_t event;
    if (!data.ReadUint32(event)) {
        WLOGFE("Read ScreenChangeEvent failed");
        return -1;
    }
    OnScreenGroupChange(trigger, screenInfos, static_cast<ScreenGroupChangeEvent>(event));
}

void DisplayManagerAgentStub::DisplayConnect()
{
    sptr<DisplayInfo> displayInfo = data.ReadParcelable<DisplayInfo>();
    OnDisplayCreate(displayInfo);
}

void DisplayManagerAgentStub::DisplayDisconnect()
{
    DisplayId displayId;
    if (!data.ReadUint64(displayId)) {
        WLOGFE("Read DisplayId failed");
        return -1;
    }
    OnDisplayDestroy(displayId);
}

void DisplayManagerAgentStub::DisplayChanged()
{
    sptr<DisplayInfo> displayInfo = data.ReadParcelable<DisplayInfo>();
    uint32_t event;
    if (!data.ReadUint32(event)) {
        WLOGFE("Read DisplayChangeEvent failed");
        return -1;
    }
    OnDisplayChange(displayInfo, static_cast<DisplayChangeEvent>(event));
}

void DisplayManagerAgentStub::ScreenShot()
{
    sptr<ScreenshotInfo> snapshotInfo = data.ReadParcelable<ScreenshotInfo>();
    OnScreenshot(snapshotInfo);
}

void DisplayManagerAgentStub::PrivateWindow()
{
    bool hasPrivate = data.ReadBool();
    NotifyPrivateWindowStateChanged(hasPrivate);
}

void DisplayManagerAgentStub::FoldStatusChanged()
{
    uint32_t foldStatus;
    if (!data.ReadUint32(foldStatus)) {
        WLOGFE("Read FoldStatus failed");
        return -1;
    }
    NotifyFoldStatusChanged(static_cast<FoldStatus>(foldStatus));
}

void DisplayManagerAgentStub::DisplayChangeInfoChanged()
{
    sptr<DisplayChangeInfo> info;
    info = DisplayChangeInfo::Unmarshalling(data);
    if (!info) {
        WLOGFE("Read DisplayChangeInfo failed");
        return -1;
    }
    NotifyDisplayChangeInfoChanged(info);
}

void DisplayManagerAgentStub::DisplayModechanged()
{
    uint32_t displayMode;
    if (!data.ReadUint32(displayMode)) {
        WLOGFE("Read FoldDisplayMode failed");
        return -1;
    }
    NotifyDisplayModeChanged(static_cast<FoldDisplayMode>(displayMode));
}

void DisplayManagerAgentStub::AvailableAreaChanged()
{
    DMRect rect;
    rect.posX_ = data.ReadInt32();
    rect.posY_ = data.ReadInt32();
    rect.width_ = data.ReadUint32();
    rect.height_ = data.ReadUint32();
    NotifyAvailableAreaChanged(rect);
}

} // namespace OHOS::Rosen
