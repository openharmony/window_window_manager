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
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManager"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(DisplayManager)

class DisplayManager::Impl : public RefBase {
public:
    static inline SingletonDelegator<DisplayManager> delegator;
    std::recursive_mutex mutex_;
    std::vector<sptr<IDisplayPowerEventListener>> powerEventListeners_;
    sptr<DisplayManagerAgent> powerEventListenerAgent_;
    sptr<DisplayManagerAgent> displayStateAgent_;
    DisplayStateCallback displayStateCallback_;

    std::vector<sptr<IDisplayListener>> displayListeners_;

    bool CheckRectValid(const Media::Rect& rect, int32_t oriHeight, int32_t oriWidth) const;
    bool CheckSizeValid(const Media::Size& size, int32_t oriHeight, int32_t oriWidth) const;
    void ClearDisplayStateCallback();
};

class DisplayManager::DisplayManagerListener : public DisplayManagerAgentDefault {
public:
    DisplayManagerListener(sptr<Impl> impl) : pImpl_(impl)
    {
    }

    void OnDisplayCreate(sptr<DisplayInfo> displayInfo) override
    {
        if (displayInfo == nullptr || displayInfo->GetDisplayId() == DISPLAY_ID_INVALD) {
            WLOGFE("OnDisplayCreate, displayInfo is invalid.");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("OnDisplayCreate, impl is nullptr.");
            return;
        }
        for (auto listener : pImpl_->displayListeners_) {
            listener->OnCreate(displayInfo->GetDisplayId());
        }
    };

    void OnDisplayDestroy(DisplayId displayId) override
    {
        if (displayId == DISPLAY_ID_INVALD) {
            WLOGFE("OnDisplayDestroy, displayId is invalid.");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("OnDisplayDestroy, impl is nullptr.");
            return;
        }
        for (auto listener : pImpl_->displayListeners_) {
            listener->OnDestroy(displayId);
        }
    };

    void OnDisplayChange(const sptr<DisplayInfo> displayInfo, DisplayChangeEvent event) override
    {
        if (displayInfo == nullptr || displayInfo->GetDisplayId() == DISPLAY_ID_INVALD) {
            WLOGFE("OnDisplayChange, displayInfo is invalid.");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("OnDisplayChange, impl is nullptr.");
            return;
        }
        WLOGD("OnDisplayChange. display %{public}" PRIu64", event %{public}u", displayInfo->GetDisplayId(), event);
        for (auto listener : pImpl_->displayListeners_) {
            listener->OnChange(displayInfo->GetDisplayId(), event);
        }
    };
private:
    sptr<Impl> pImpl_;
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

const sptr<Display> DisplayManager::GetDisplayByScreen(ScreenId screenId)
{
    return nullptr;
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

bool DisplayManager::RegisterDisplayListener(sptr<IDisplayListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("RegisterDisplayListener listener is nullptr.");
        return false;
    }
    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    pImpl_->displayListeners_.push_back(listener);
    bool ret = true;
    if (displayManagerListener_ == nullptr) {
        displayManagerListener_ = new DisplayManagerListener(pImpl_);
        ret = SingletonContainer::Get<DisplayManagerAdapter>().RegisterDisplayManagerAgent(
            displayManagerListener_,
            DisplayManagerAgentType::DISPLAY_EVENT_LISTENER);
        if (!ret) {
            WLOGFW("RegisterDisplayManagerAgent failed ! remove listener!");
            pImpl_->displayListeners_.pop_back();
        }
    }
    return ret;
}

bool DisplayManager::UnregisterDisplayListener(sptr<IDisplayListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("UnregisterDisplayListener listener is nullptr.");
        return false;
    }
    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    auto iter = std::find(pImpl_->displayListeners_.begin(), pImpl_->displayListeners_.end(), listener);
    if (iter == pImpl_->displayListeners_.end()) {
        WLOGFE("could not find this listener");
        return false;
    }
    pImpl_->displayListeners_.erase(iter);
    bool ret = true;
    if (pImpl_->displayListeners_.empty() && displayManagerListener_ != nullptr) {
        ret = SingletonContainer::Get<DisplayManagerAdapter>().UnregisterDisplayManagerAgent(
            displayManagerListener_,
            DisplayManagerAgentType::DISPLAY_EVENT_LISTENER);
        displayManagerListener_ = nullptr;
    }
    return ret;
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

void DisplayManager::NotifyDisplayStateChanged(DisplayId id, DisplayState state)
{
    WLOGFI("state:%{public}u", state);
    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    if (pImpl_->displayStateCallback_) {
        pImpl_->displayStateCallback_(state);
        pImpl_->ClearDisplayStateCallback();
        return;
    }
    WLOGFW("callback_ target is not set!");
}

void DisplayManager::NotifyDisplayChangedEvent(const sptr<DisplayInfo> info, DisplayChangeEvent event)
{
    WLOGI("NotifyDisplayChangedEvent event:%{public}u, size:%{public}zu", event, pImpl_->displayListeners_.size());
    std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
    for (auto& listener : pImpl_->displayListeners_) {
        listener->OnChange(info->GetDisplayId(), event);
    }
}

bool DisplayManager::WakeUpBegin(PowerStateChangeReason reason)
{
    WLOGFI("WakeUpBegin start, reason:%{public}u", reason);
    return SingletonContainer::Get<DisplayManagerAdapter>().WakeUpBegin(reason);
}

bool DisplayManager::WakeUpEnd()
{
    WLOGFI("WakeUpEnd start");
    return SingletonContainer::Get<DisplayManagerAdapter>().WakeUpEnd();
}

bool DisplayManager::SuspendBegin(PowerStateChangeReason reason)
{
    // dms->wms notify other windows to hide
    WLOGFI("SuspendBegin start, reason:%{public}u", reason);
    return SingletonContainer::Get<DisplayManagerAdapter>().SuspendBegin(reason);
}

bool DisplayManager::SuspendEnd()
{
    WLOGFI("SuspendEnd start");
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
    RSInterfaces::GetInstance().SetScreenBacklight(screenId, level);
    return level == GetScreenBrightness(screenId);
}

uint32_t DisplayManager::GetScreenBrightness(uint64_t screenId) const
{
    uint32_t level = static_cast<uint32_t>(RSInterfaces::GetInstance().GetScreenBacklight(screenId));
    WLOGFI("screenId:%{public}" PRIu64", level:%{public}u,", screenId, level);
    return level;
}

void DisplayManager::NotifyDisplayEvent(DisplayEvent event)
{
    // Unlock event dms->wms restore other hidden windows
    WLOGFI("DisplayEvent:%{public}u", event);
    SingletonContainer::Get<DisplayManagerAdapter>().NotifyDisplayEvent(event);
}
} // namespace OHOS::Rosen