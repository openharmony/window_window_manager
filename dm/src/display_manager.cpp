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
#include "display_manager_agent.h"
#include "dm_common.h"
#include "singleton_delegator.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DisplayManager"};
    constexpr uint32_t MAX_SCREEN_BRIGHTNESS_VALUE = 100;
}
WM_IMPLEMENT_SINGLE_INSTANCE(DisplayManager)

class DisplayManager::Impl : public RefBase {
friend class DisplayManager;
private:
    constexpr static int32_t MAX_RESOLUTION_SIZE_SCREENSHOT = 15360; // max resolution, 16K
    static inline SingletonDelegator<DisplayManager> delegator;
    std::recursive_mutex mutex_;
    std::vector<sptr<IDisplayPowerEventListener>> powerEventListeners_;
    sptr<DisplayManagerAgent> powerEventListenerAgent_;
    sptr<DisplayManagerAgent> displayStateAgent_;
    DisplayStateCallback displayStateCallback_;

    bool CheckRectValid(const Media::Rect& rect, int32_t oriHeight, int32_t oriWidth) const;
    bool CheckSizeValid(const Media::Size& size, int32_t oriHeight, int32_t oriWidth) const;
    void ClearDisplayStateCallback();
};

bool DisplayManager::Impl::CheckRectValid(const Media::Rect& rect, int32_t oriHeight, int32_t oriWidth) const
{
    if (!((rect.left >= 0) and (rect.left < oriWidth) and (rect.top >= 0) and (rect.top < oriHeight))) {
        WLOGFE("rect left or top invalid!");
        return false;
    }

    if (!((rect.width > 0) and (rect.width <= (oriWidth - rect.left)) and
        (rect.height > 0) and (rect.height <= (oriHeight - rect.top)))) {
        if (!((rect.width == 0) and (rect.height == 0))) {
            WLOGFE("rect height or width invalid!");
            return false;
        }
    }
    return true;
}

bool DisplayManager::Impl::CheckSizeValid(const Media::Size& size, int32_t oriHeight, int32_t oriWidth) const
{
    if (!((size.width > 0) and (size.height > 0))) {
        if (!((size.width == 0) and (size.height == 0))) {
            WLOGFE("width or height invalid!");
            return false;
        }
    }

    if ((size.width > MAX_RESOLUTION_SIZE_SCREENSHOT) or (size.height > MAX_RESOLUTION_SIZE_SCREENSHOT)) {
        WLOGFE("width or height too big!");
        return false;
    }
    return true;
}

void DisplayManager::Impl::ClearDisplayStateCallback()
{
    displayStateCallback_ = nullptr;
    if (displayStateAgent_ != nullptr) {
        SingletonContainer::Get<DisplayManagerAdapter>().UnregisterDisplayManagerAgent(displayStateAgent_,
            DisplayManagerAgentType::DISPLAY_STATE_LISTENER);
        displayStateAgent_ = nullptr;
    }
}

DisplayManager::DisplayManager() : pImpl_(new Impl())
{
}

DisplayManager::~DisplayManager()
{
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

std::shared_ptr<Media::PixelMap> DisplayManager::GetScreenshot(DisplayId displayId, const Media::Rect &rect,
                                                               const Media::Size &size, int rotation)
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

    // check parameters
    int32_t oriHeight = screenShot->GetHeight();
    int32_t oriWidth = screenShot->GetWidth();
    if (!pImpl_->CheckRectValid(rect, oriHeight, oriWidth)) {
        WLOGFE("rect invalid! left %{public}d, top %{public}d, w %{public}d, h %{public}d",
            rect.left, rect.top, rect.width, rect.height);
        return nullptr;
    }
    if (!pImpl_->CheckSizeValid(size, oriHeight, oriWidth)) {
        WLOGFE("size invalid! w %{public}d, h %{public}d", rect.width, rect.height);
        return nullptr;
    }
    
    // create crop dest pixelmap
    Media::InitializationOptions opt;
    opt.size.width = size.width;
    opt.size.height = size.height;
    opt.scaleMode = Media::ScaleMode::FIT_TARGET_SIZE;
    opt.editable = false;
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

bool DisplayManager::RegisterDisplayPowerEventListener(sptr<IDisplayPowerEventListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return false;
    }
    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    pImpl_->powerEventListeners_.push_back(listener);
    bool ret = true;
    if (pImpl_->powerEventListenerAgent_ == nullptr) {
        pImpl_->powerEventListenerAgent_ = new DisplayManagerAgent();
        ret = SingletonContainer::Get<DisplayManagerAdapter>().RegisterDisplayManagerAgent(
            pImpl_->powerEventListenerAgent_,
            DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER);
        if (!ret) {
            WLOGFW("RegisterDisplayManagerAgent failed ! remove listener!");
            pImpl_->powerEventListeners_.pop_back();
        }
    }
    WLOGFI("RegisterDisplayPowerEventListener end");
    return ret;
}

bool DisplayManager::UnregisterDisplayPowerEventListener(sptr<IDisplayPowerEventListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return false;
    }

    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    auto iter = std::find(pImpl_->powerEventListeners_.begin(), pImpl_->powerEventListeners_.end(), listener);
    if (iter == pImpl_->powerEventListeners_.end()) {
        WLOGFE("could not find this listener");
        return false;
    }
    pImpl_->powerEventListeners_.erase(iter);
    bool ret = true;
    if (pImpl_->powerEventListeners_.empty() && pImpl_->powerEventListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<DisplayManagerAdapter>().UnregisterDisplayManagerAgent(
            pImpl_->powerEventListenerAgent_,
            DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER);
        pImpl_->powerEventListenerAgent_ = nullptr;
    }
    WLOGFI("UnregisterDisplayPowerEventListener end");
    return ret;
}

void DisplayManager::NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status)
{
    WLOGFI("NotifyDisplayPowerEvent event:%{public}u, status:%{public}u, size:%{public}zu", event, status,
        pImpl_->powerEventListeners_.size());
    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    for (auto& listener : pImpl_->powerEventListeners_) {
        listener->OnDisplayPowerEvent(event, status);
    }
}

void DisplayManager::NotifyDisplayStateChanged(DisplayState state)
{
    WLOGFI("state:%{public}u", state);
    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    if (pImpl_->displayStateCallback_) {
        pImpl_->displayStateCallback_(state);
        WLOGFW("displayStateCallback_ end");
        pImpl_->ClearDisplayStateCallback();
        return;
    }
    WLOGFW("callback_ target is not set!");
}

bool DisplayManager::WakeUpBegin(PowerStateChangeReason reason)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().WakeUpBegin(reason);
}

bool DisplayManager::WakeUpEnd()
{
    return SingletonContainer::Get<DisplayManagerAdapter>().WakeUpEnd();
}

bool DisplayManager::SuspendBegin(PowerStateChangeReason reason)
{
    // dms->wms notify other windows to hide
    return SingletonContainer::Get<DisplayManagerAdapter>().SuspendBegin(reason);
}

bool DisplayManager::SuspendEnd()
{
    return SingletonContainer::Get<DisplayManagerAdapter>().SuspendEnd();
}

bool DisplayManager::SetScreenPowerForAll(DisplayPowerState state, PowerStateChangeReason reason)
{
    // TODO: should get all screen ids
    WLOGFI("state:%{public}u, reason:%{public}u", state, reason);
    ScreenId defaultId = GetDefaultDisplayId();
    if (defaultId == DISPLAY_ID_INVALD) {
        WLOGFI("defaultId invalid!");
        return false;
    }
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
    WLOGFI("SetScreenPowerStatus end");
    return SingletonContainer::Get<DisplayManagerAdapter>().SetScreenPowerForAll(state, reason);
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
    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    if (pImpl_->displayStateCallback_ != nullptr || callback == nullptr) {
        WLOGFI("previous callback not called or callback invalid");
        return false;
    }
    pImpl_->displayStateCallback_ = callback;
    bool ret = true;
    if (pImpl_->displayStateAgent_ == nullptr) {
        pImpl_->displayStateAgent_ = new DisplayManagerAgent();
        ret = SingletonContainer::Get<DisplayManagerAdapter>().RegisterDisplayManagerAgent(
            pImpl_->displayStateAgent_,
            DisplayManagerAgentType::DISPLAY_STATE_LISTENER);
    }

    ret &= SingletonContainer::Get<DisplayManagerAdapter>().SetDisplayState(state);
    if (!ret) {
        pImpl_->ClearDisplayStateCallback();
    }
    return ret;
}

DisplayState DisplayManager::GetDisplayState(DisplayId displayId)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().GetDisplayState(displayId);
}

bool DisplayManager::SetScreenBrightness(uint64_t screenId, uint32_t level)
{
    WLOGFI("screenId:%{public}" PRIu64", level:%{public}u,", screenId, level);
    if (level > MAX_SCREEN_BRIGHTNESS_VALUE) {
        WLOGFW("level:%{public}u, exceed max value!", level);
        return false;
    }
    RSInterfaces::GetInstance().SetScreenBacklight(screenId, level);
    return true;
}

uint32_t DisplayManager::GetScreenBrightness(uint64_t screenId) const
{
    uint32_t level = RSInterfaces::GetInstance().GetScreenBacklight(screenId);
    WLOGFI("screenId:%{public}" PRIu64", level:%{public}u,", screenId, level);
    return level;
}

void DisplayManager::NotifyDisplayEvent(DisplayEvent event)
{
    // Unlock event dms->wms restore other hidden windows
    SingletonContainer::Get<DisplayManagerAdapter>().NotifyDisplayEvent(event);
}
} // namespace OHOS::Rosen