/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "display_manager.h"

#include <cinttypes>
#include <transaction/rs_interfaces.h>

#include "display_manager_adapter.h"
#include "dm_common.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DisplayManager"};
}

DisplayId DisplayManager::GetDefaultDisplayId()
{
    return SingletonContainer::Get<DisplayManagerAdapter>().GetDefaultDisplayId();
}

const sptr<Display> DisplayManager::GetDisplayById(DisplayId displayId)
{
    sptr<Display> display = SingletonContainer::Get<DisplayManagerAdapter>().GetDisplayById(displayId);
    if (display == nullptr) {
        WLOGFE("DisplayManager::GetDisplayById failed!\n");
        return nullptr;
    }
    return display;
}

std::shared_ptr<Media::PixelMap> DisplayManager::GetScreenshot(DisplayId displayId)
{
    if (displayId == DISPLAY_ID_INVALD) {
        WLOGFE("displayId invalid!");
        return nullptr;
    }
    std::shared_ptr<Media::PixelMap> screenShot =
        SingletonContainer::Get<DisplayManagerAdapter>().GetDisplaySnapshot(displayId);
    if (screenShot == nullptr) {
        WLOGFE("DisplayManager::GetScreenshot failed!");
        return nullptr;
    }

    return screenShot;
}

bool DisplayManager::CheckRectOffsetValid(int32_t param) const
{
    if (param < 0 || param > MAX_RESOLUTION_VALUE) {
        return false;
    }
    return true;
}

bool DisplayManager::CheckRectSizeValid(int32_t param) const
{
    if (param < MIN_RESOLUTION_VALUE || param > MAX_RESOLUTION_VALUE) {
        return false;
    }
    return true;
}

std::shared_ptr<Media::PixelMap> DisplayManager::GetScreenshot(DisplayId displayId, const Media::Rect &rect,
                                                               const Media::Size &size, int rotation)
{
    if (displayId == DISPLAY_ID_INVALD) {
        WLOGFE("displayId invalid!");
        return nullptr;
    }
    if (!CheckRectOffsetValid(rect.left) || !CheckRectOffsetValid(rect.top) ||
        !CheckRectSizeValid(rect.width) || !CheckRectSizeValid(rect.height)) {
        WLOGFE("rect invalid! left %{public}d, top %{public}d, w %{public}d, h %{public}d",
            rect.left, rect.top, rect.width, rect.height);
        return nullptr;
    }
    if (!CheckRectSizeValid(size.width) || !CheckRectSizeValid(size.height)) {
        WLOGFE("size invalid! w %{public}d, h %{public}d", rect.width, rect.height);
        return nullptr;
    }
    std::shared_ptr<Media::PixelMap> screenShot =
        SingletonContainer::Get<DisplayManagerAdapter>().GetDisplaySnapshot(displayId);
    if (screenShot == nullptr) {
        WLOGFE("DisplayManager::GetScreenshot failed!");
        return nullptr;
    }

    // create crop dest pixelmap
    Media::InitializationOptions opt;
    opt.size.width = size.width;
    opt.size.height = size.height;
    opt.scaleMode = Media::ScaleMode::FIT_TARGET_SIZE;
    opt.editable = false;
    opt.useSourceIfMatch = true;

    auto pixelMap = Media::PixelMap::Create(*screenShot, rect, opt);
    if (pixelMap == nullptr) {
        WLOGFE("Media::PixelMap::Create failed!");
        return nullptr;
    }
    std::shared_ptr<Media::PixelMap> dstScreenshot(pixelMap.release());

    return dstScreenshot;
}

const sptr<Display> DisplayManager::GetDefaultDisplay()
{
    return GetDisplayById(GetDefaultDisplayId());
}

std::vector<DisplayId> DisplayManager::GetAllDisplayIds()
{
    std::vector<DisplayId> res;
    res.push_back(GetDefaultDisplayId());
    // 获取所有displayIds
    return res;
}

std::vector<const sptr<Display>> DisplayManager::GetAllDisplays()
{
    std::vector<const sptr<Display>> res;
    auto displayIds = GetAllDisplayIds();
    for (auto displayId: displayIds) {
        const sptr<Display> display = GetDisplayById(displayId);
        if (display != nullptr) {
            res.push_back(display);
        } else {
            WLOGFE("DisplayManager::GetAllDisplays display %" PRIu64" nullptr!", displayId);
        }
    }
    return res;
}

DisplayId DisplayManager::CreateVirtualDisplay(const std::string &name, uint32_t width, uint32_t height,
    sptr<Surface> surface, DisplayId displayIdToMirror, int32_t flags)
{
    WLOGFI("DisplayManager::CreateVirtualDisplay multi params");
    VirtualDisplayInfo info(name, width, height, displayIdToMirror, flags);
    return SingletonContainer::Get<DisplayManagerAdapter>().CreateVirtualDisplay(info, surface);
}

bool DisplayManager::DestroyVirtualDisplay(DisplayId displayId)
{
    WLOGFI("DisplayManager::DestroyVirtualDisplay override params");
    return SingletonContainer::Get<DisplayManagerAdapter>().DestroyVirtualDisplay(displayId);
}

bool DisplayManager::WakeUpBegin(PowerStateChangeReason reason)
{
    return true;
}

bool DisplayManager::WakeUpEnd()
{
    return true;
}

bool DisplayManager::SuspendBegin(PowerStateChangeReason reason)
{
    // dms->wms notify other windows to hide
    return SingletonContainer::Get<DisplayManagerAdapter>().SuspendBegin(reason);
}

bool DisplayManager::SuspendEnd()
{
    return true;
}

bool DisplayManager::SetScreenPowerForAll(DisplayPowerState state, PowerStateChangeReason reason)
{
    // TODO: should get all screen ids
    ScreenId defaultId = GetDefaultDisplayId();
    if (defaultId == DISPLAY_ID_INVALD) {
        return false;
    }
    WLOGFI("state:%{public}u, reason:%{public}u, defaultId:%{public}" PRIu64".", state, reason, defaultId);
    ScreenPowerStatus status;
    switch (state) {
        case DisplayPowerState::POWER_ON: {
            status = ScreenPowerStatus::POWER_STATUS_ON;
            break;
        }
        case DisplayPowerState::POWER_OFF: {
            status = ScreenPowerStatus::POWER_STATUS_OFF;
            break;
        }
        default: {
            WLOGFW("SetScreenPowerStatus state not support");
            return false;
        }
    }
    RSInterfaces::GetInstance().SetScreenPowerStatus(defaultId, status);
    return true;
}

DisplayPowerState DisplayManager::GetScreenPower(uint64_t screenId)
{
    DisplayPowerState res = static_cast<DisplayPowerState>(RSInterfaces::GetInstance().GetScreenPowerStatus(screenId));
    WLOGFI("GetScreenPower:%{public}u, defaultId:%{public}" PRIu64".", res, screenId);
    return res;
}

bool DisplayManager::SetDisplayState(DisplayState state, DisplayStateCallback callback)
{
    WLOGFI("state:%{public}u", state);
    return SingletonContainer::Get<DisplayManagerAdapter>().SetDisplayState(state, callback);
}

DisplayState DisplayManager::GetDisplayState(uint64_t displayId)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().GetDisplayState(displayId);
}

bool DisplayManager::SetScreenBrightness(uint64_t screenId, uint32_t level)
{
    return true;
}

uint32_t DisplayManager::GetScreenBrightness(uint64_t screenId) const
{
    return 0;
}

void DisplayManager::NotifyDisplayEvent(DisplayEvent event)
{
    // Unlock event dms->wms restore other hidden windows
    SingletonContainer::Get<DisplayManagerAdapter>().NotifyDisplayEvent(event);
}
} // namespace OHOS::Rosen