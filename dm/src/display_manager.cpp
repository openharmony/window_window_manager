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

#include "display_manager.h"

#include <chrono>
#include <cinttypes>
#include <transaction/rs_interfaces.h>
#include <ui/rs_surface_node.h>

#include "display_manager_adapter.h"
#include "display_manager_agent_default.h"
#include "dm_common.h"
#include "screen_manager.h"
#include "singleton_delegator.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManager"};
    const static uint32_t MAX_DISPLAY_SIZE = 32;
    const static uint32_t MAX_INTERVAL_US = 5000;
}
WM_IMPLEMENT_SINGLE_INSTANCE(DisplayManager)

class DisplayManager::Impl : public RefBase {
public:
    Impl(std::recursive_mutex& mutex) : mutex_(mutex) {}
    ~Impl();
    static inline SingletonDelegator<DisplayManager> delegator;
    bool CheckRectValid(const Media::Rect& rect, int32_t oriHeight, int32_t oriWidth) const;
    bool CheckSizeValid(const Media::Size& size, int32_t oriHeight, int32_t oriWidth) const;
    sptr<Display> GetDefaultDisplay();
    sptr<Display> GetDefaultDisplaySync();
    sptr<Display> GetDisplayById(DisplayId displayId);
    DMError HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow);

    bool IsFoldable();

    FoldStatus GetFoldStatus();

    FoldDisplayMode GetFoldDisplayMode();

    void SetFoldDisplayMode(const FoldDisplayMode);

    void SetFoldStatusLocked(bool locked);

    sptr<FoldCreaseRegion> GetCurrentFoldCreaseRegion();

    DMError RegisterDisplayListener(sptr<IDisplayListener> listener);
    DMError UnregisterDisplayListener(sptr<IDisplayListener> listener);
    bool SetDisplayState(DisplayState state, DisplayStateCallback callback);
    DMError RegisterDisplayPowerEventListener(sptr<IDisplayPowerEventListener> listener);
    DMError UnregisterDisplayPowerEventListener(sptr<IDisplayPowerEventListener> listener);
    DMError RegisterScreenshotListener(sptr<IScreenshotListener> listener);
    DMError UnregisterScreenshotListener(sptr<IScreenshotListener> listener);
    DMError RegisterPrivateWindowListener(sptr<IPrivateWindowListener> listener);
    DMError UnregisterPrivateWindowListener(sptr<IPrivateWindowListener> listener);
    DMError RegisterFoldStatusListener(sptr<IFoldStatusListener> listener);
    DMError UnregisterFoldStatusListener(sptr<IFoldStatusListener> listener);
    DMError RegisterDisplayUpdateListener(sptr<IDisplayUpdateListener> listener);
    DMError UnregisterDisplayUpdateListener(sptr<IDisplayUpdateListener> listener);
    DMError RegisterDisplayModeListener(sptr<IDisplayModeListener> listener);
    DMError UnregisterDisplayModeListener(sptr<IDisplayModeListener> listener);
    DMError RegisterAvailableAreaListener(sptr<IAvailableAreaListener> listener);
    DMError UnregisterAvailableAreaListener(sptr<IAvailableAreaListener> listener);
    sptr<Display> GetDisplayByScreenId(ScreenId screenId);
    void OnRemoteDied();
private:
    void ClearDisplayStateCallback();
    void ClearFoldStatusCallback();
    void ClearDisplayModeCallback();
    void NotifyPrivateWindowStateChanged(bool hasPrivate);
    void NotifyScreenshot(sptr<ScreenshotInfo> info);
    void NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status);
    void NotifyDisplayStateChanged(DisplayId id, DisplayState state);
    void NotifyDisplayChangedEvent(sptr<DisplayInfo> info, DisplayChangeEvent event);
    void NotifyDisplayCreate(sptr<DisplayInfo> info);
    void NotifyDisplayDestroy(DisplayId);
    void NotifyDisplayChange(sptr<DisplayInfo> displayInfo);
    bool UpdateDisplayInfoLocked(sptr<DisplayInfo>);
    void NotifyFoldStatusChanged(FoldStatus foldStatus);
    void NotifyDisplayChangeInfoChanged(const sptr<DisplayChangeInfo>& info);
    void NotifyDisplayModeChanged(FoldDisplayMode displayMode);
    void NotifyAvailableAreaChanged(DMRect rect);
    void Clear();

    DisplayId defaultDisplayId_ = DISPLAY_ID_INVALID;
    std::map<DisplayId, sptr<Display>> displayMap_;
    DisplayStateCallback displayStateCallback_;
    std::recursive_mutex& mutex_;
    std::set<sptr<IDisplayListener>> displayListeners_;
    std::set<sptr<IDisplayPowerEventListener>> powerEventListeners_;
    std::set<sptr<IScreenshotListener>> screenshotListeners_;
    std::set<sptr<IPrivateWindowListener>> privateWindowListeners_;
    std::set<sptr<IFoldStatusListener>> foldStatusListeners_;
    std::set<sptr<IDisplayUpdateListener>> displayUpdateListeners_;
    std::set<sptr<IDisplayModeListener>> displayModeListeners_;
    std::set<sptr<IAvailableAreaListener>> availableAreaListeners_;
    class DisplayManagerListener;
    sptr<DisplayManagerListener> displayManagerListener_;
    class DisplayManagerAgent;
    sptr<DisplayManagerAgent> displayStateAgent_;
    sptr<DisplayManagerAgent> powerEventListenerAgent_;
    class DisplayManagerScreenshotAgent;
    sptr<DisplayManagerScreenshotAgent> screenshotListenerAgent_;
    class DisplayManagerPrivateWindowAgent;
    sptr<DisplayManagerPrivateWindowAgent> privateWindowListenerAgent_;
    class DisplayManagerFoldStatusAgent;
    sptr<DisplayManagerFoldStatusAgent> foldStatusListenerAgent_;
    class DisplayManagerDisplayUpdateAgent;
    sptr<DisplayManagerDisplayUpdateAgent> displayUpdateListenerAgent_;
    class DisplayManagerDisplayModeAgent;
    sptr<DisplayManagerDisplayModeAgent> displayModeListenerAgent_;
    class DisplayManagerAvailableAreaAgent;
    sptr<DisplayManagerAvailableAreaAgent> availableAreaListenerAgent_;
};

class DisplayManager::Impl::DisplayManagerListener : public DisplayManagerAgentDefault {
public:
    explicit DisplayManagerListener(sptr<Impl> impl) : pImpl_(impl)
    {
    }

    void OnDisplayCreate(sptr<DisplayInfo> displayInfo) override
    {
        if (displayInfo == nullptr || displayInfo->GetDisplayId() == DISPLAY_ID_INVALID) {
            WLOGFE("OnDisplayCreate, displayInfo is invalid.");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("OnDisplayCreate, impl is nullptr.");
            return;
        }
        pImpl_->NotifyDisplayCreate(displayInfo);
        std::set<sptr<IDisplayListener>> displayListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
            displayListeners = pImpl_->displayListeners_;
        }
        for (auto listener : displayListeners) {
            listener->OnCreate(displayInfo->GetDisplayId());
        }
    };

    void OnDisplayDestroy(DisplayId displayId) override
    {
        if (displayId == DISPLAY_ID_INVALID) {
            WLOGFE("OnDisplayDestroy, displayId is invalid.");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("OnDisplayDestroy, impl is nullptr.");
            return;
        }
        pImpl_->NotifyDisplayDestroy(displayId);
        std::set<sptr<IDisplayListener>> displayListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
            displayListeners = pImpl_->displayListeners_;
        }
        for (auto listener : displayListeners) {
            listener->OnDestroy(displayId);
        }
    };

    void OnDisplayChange(sptr<DisplayInfo> displayInfo, DisplayChangeEvent event) override
    {
        if (displayInfo == nullptr || displayInfo->GetDisplayId() == DISPLAY_ID_INVALID) {
            WLOGFE("OnDisplayChange, displayInfo is invalid.");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("OnDisplayChange, impl is nullptr.");
            return;
        }
        WLOGD("OnDisplayChange. display %{public}" PRIu64", event %{public}u", displayInfo->GetDisplayId(), event);
        pImpl_->NotifyDisplayChange(displayInfo);
        std::set<sptr<IDisplayListener>> displayListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
            displayListeners = pImpl_->displayListeners_;
        }
        for (auto listener : displayListeners) {
            listener->OnChange(displayInfo->GetDisplayId());
        }
    };
private:
    sptr<Impl> pImpl_;
};

class DisplayManager::Impl::DisplayManagerAgent : public DisplayManagerAgentDefault {
public:
    explicit DisplayManagerAgent(sptr<Impl> impl) : pImpl_(impl)
    {
    }
    ~DisplayManagerAgent() = default;

    virtual void NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status) override
    {
        pImpl_->NotifyDisplayPowerEvent(event, status);
    }

    virtual void NotifyDisplayStateChanged(DisplayId id, DisplayState state) override
    {
        pImpl_->NotifyDisplayStateChanged(id, state);
    }
private:
    sptr<Impl> pImpl_;
};

class DisplayManager::Impl::DisplayManagerScreenshotAgent : public DisplayManagerAgentDefault {
public:
    explicit DisplayManagerScreenshotAgent(sptr<Impl> impl) : pImpl_(impl)
    {
    }
    ~DisplayManagerScreenshotAgent() = default;

    virtual void OnScreenshot(sptr<ScreenshotInfo> info) override
    {
        pImpl_->NotifyScreenshot(info);
    }
private:
    sptr<Impl> pImpl_;
};

class DisplayManager::Impl::DisplayManagerPrivateWindowAgent : public DisplayManagerAgentDefault {
public:
    explicit DisplayManagerPrivateWindowAgent(sptr<Impl> impl) : pImpl_(impl)
    {
    }
    ~DisplayManagerPrivateWindowAgent() = default;

    virtual void NotifyPrivateWindowStateChanged(bool hasPrivate) override
    {
        pImpl_->NotifyPrivateWindowStateChanged(hasPrivate);
    }
private:
    sptr<Impl> pImpl_;
};

class DisplayManager::Impl::DisplayManagerFoldStatusAgent : public DisplayManagerAgentDefault {
public:
    explicit DisplayManagerFoldStatusAgent(sptr<Impl> impl) : pImpl_(impl)
    {
    }
    ~DisplayManagerFoldStatusAgent() = default;

    virtual void NotifyFoldStatusChanged(FoldStatus foldStatus) override
    {
        pImpl_->NotifyFoldStatusChanged(foldStatus);
    }
private:
    sptr<Impl> pImpl_;
};

class DisplayManager::Impl::DisplayManagerDisplayUpdateAgent : public DisplayManagerAgentDefault {
public:
    explicit DisplayManagerDisplayUpdateAgent(sptr<Impl> impl) : pImpl_(impl)
    {
    }
    ~DisplayManagerDisplayUpdateAgent() = default;

    virtual void NotifyDisplayChangeInfoChanged(const sptr<DisplayChangeInfo>& info) override
    {
        pImpl_->NotifyDisplayChangeInfoChanged(info);
    }
private:
    sptr<Impl> pImpl_;
};


class DisplayManager::Impl::DisplayManagerDisplayModeAgent : public DisplayManagerAgentDefault {
public:
    explicit DisplayManagerDisplayModeAgent(sptr<Impl> impl) : pImpl_(impl)
    {
    }
    ~DisplayManagerDisplayModeAgent() = default;

    virtual void NotifyDisplayModeChanged(FoldDisplayMode displayMode) override
    {
        pImpl_->NotifyDisplayModeChanged(displayMode);
    }
private:
    sptr<Impl> pImpl_;
};

class DisplayManager::Impl::DisplayManagerAvailableAreaAgent : public DisplayManagerAgentDefault {
public:
    explicit DisplayManagerAvailableAreaAgent(sptr<Impl> impl) : pImpl_(impl)
    {
    }
    ~DisplayManagerAvailableAreaAgent() = default;

    virtual void NotifyAvailableAreaChanged(DMRect area) override
    {
        pImpl_->NotifyAvailableAreaChanged(area);
    }
private:
    sptr<Impl> pImpl_;
};

bool DisplayManager::Impl::CheckRectValid(const Media::Rect& rect, int32_t oriHeight, int32_t oriWidth) const
{
    if (rect.left < 0) {
        return false;
    }
    if (rect.top < 0) {
        return false;
    }
    if (rect.width < 0) {
        return false;
    }
    if (rect.height < 0) {
        return false;
    }
    if (rect.width + rect.left > oriWidth) {
        return false;
    }
    if (rect.height + rect.top > oriHeight) {
        return false;
    }
    return true;
}

bool DisplayManager::Impl::CheckSizeValid(const Media::Size& size, int32_t oriHeight, int32_t oriWidth) const
{
    if (size.width < 0) {
        return false;
    }
    if (size.height < 0) {
        return false;
    }
    if (size.width > MAX_RESOLUTION_SIZE_SCREENSHOT) {
        return false;
    }
    if (size.height > MAX_RESOLUTION_SIZE_SCREENSHOT) {
        return false;
    }
    return true;
}

void DisplayManager::Impl::ClearDisplayStateCallback()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    displayStateCallback_ = nullptr;
    if (displayStateAgent_ != nullptr) {
        SingletonContainer::Get<DisplayManagerAdapter>().UnregisterDisplayManagerAgent(displayStateAgent_,
            DisplayManagerAgentType::DISPLAY_STATE_LISTENER);
        displayStateAgent_ = nullptr;
    }
}

void DisplayManager::Impl::ClearFoldStatusCallback()
{
    DMError res = DMError::DM_OK;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (foldStatusListenerAgent_ != nullptr) {
        res = SingletonContainer::Get<DisplayManagerAdapter>().UnregisterDisplayManagerAgent(foldStatusListenerAgent_,
            DisplayManagerAgentType::FOLD_STATUS_CHANGED_LISTENER);
        foldStatusListenerAgent_ = nullptr;
        WLOGI("ClearFoldStatusCallback foldStatusListenerAgent_ is nullptr !");
    }
    if (res != DMError::DM_OK) {
        WLOGFW("UnregisterDisplayManagerAgent FOLD_STATUS_CHANGED_LISTENER failed !");
    }
}

void DisplayManager::Impl::ClearDisplayStateCallback()
{
    DMError res = DMError::DM_OK;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (displayModeListenerAgent_ != nullptr) {
        res = SingletonContainer::Get<DisplayManagerAdapter>().UnregisterDisplayManagerAgent(displayModeListenerAgent_,
            DisplayManagerAgentType::DISPLAY_MODE_CHANGED_LISTENER);
        displayModeListenerAgent_ = nullptr;
        WLOGI("ClearDisplayStateCallback displayModeListenerAgent_ is nullptr !");
    }
    if (res != DMError::DM_OK) {
        WLOGFW("UnregisterDisplayManagerAgent DISPLAY_MODE_CHANGED_LISTENER failed !");
    }
}

void DisplayManager::Impl::Clear()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DMError res = DMError::DM_OK;
    if (displayManagerListener_ != nullptr) {
        res = SingletonContainer::Get<DisplayManagerAdapter>().UnregisterDisplayManagerAgent(
            displayManagerListener_, DisplayManagerAgentType::DISPLAY_EVENT_LISTENER);
    }
    displayManagerListener_ = nullptr;
    if (res != DMError::DM_OK) {
        WLOGFW("UnregisterDisplayManagerAgent DISPLAY_EVENT_LISTENER failed !");
    }
    res = DMError::DM_OK;
    if (powerEventListenerAgent_ != nullptr) {
        res = SingletonContainer::Get<DisplayManagerAdapter>().UnregisterDisplayManagerAgent(
            powerEventListenerAgent_, DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER);
    }
    powerEventListenerAgent_ = nullptr;
    if (res != DMError::DM_OK) {
        WLOGFW("UnregisterDisplayManagerAgent DISPLAY_POWER_EVENT_LISTENER failed !");
    }
    ClearDisplayStateCallback();
    ClearFoldStatusCallback();
    ClearDisplayStateCallback();
}

DisplayManager::Impl::~Impl()
{
    Clear();
}

DisplayManager::DisplayManager() : pImpl_(new Impl(mutex_))
{
}

DisplayManager::~DisplayManager()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    destroyed_ = true;
}

DisplayId DisplayManager::GetDefaultDisplayId()
{
    auto info = SingletonContainer::Get<DisplayManagerAdapter>().GetDefaultDisplayInfo();
    if (info == nullptr) {
        return DISPLAY_ID_INVALID;
    }
    return info->GetDisplayId();
}

sptr<Display> DisplayManager::Impl::GetDefaultDisplay()
{
    auto displayInfo = SingletonContainer::Get<DisplayManagerAdapter>().GetDefaultDisplayInfo();
    if (displayInfo == nullptr) {
        return nullptr;
    }
    auto displayId = displayInfo->GetDisplayId();
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!UpdateDisplayInfoLocked(displayInfo)) {
        displayMap_.erase(displayId);
        return nullptr;
    }
    return displayMap_[displayId];
}

sptr<Display> DisplayManager::Impl::GetDefaultDisplaySync()
{
    static std::chrono::steady_clock::time_point lastRequestTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    auto interval = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastRequestTime).count();
    if (defaultDisplayId_ != DISPLAY_ID_INVALID && interval < MAX_INTERVAL_US) {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        auto iter = displayMap_.find(defaultDisplayId_);
        if (iter != displayMap_.end()) {
            return displayMap_[defaultDisplayId_];
        }
    }

    auto displayInfo = SingletonContainer::Get<DisplayManagerAdapter>().GetDefaultDisplayInfo();
    if (displayInfo == nullptr) {
        return nullptr;
    }
    auto displayId = displayInfo->GetDisplayId();
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!UpdateDisplayInfoLocked(displayInfo)) {
        displayMap_.erase(displayId);
        return nullptr;
    }
    lastRequestTime = currentTime;
    defaultDisplayId_ = displayId;
    return displayMap_[displayId];
}

sptr<Display> DisplayManager::Impl::GetDisplayById(DisplayId displayId)
{
    auto displayInfo = SingletonContainer::Get<DisplayManagerAdapter>().GetDisplayInfo(displayId);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!UpdateDisplayInfoLocked(displayInfo)) {
        displayMap_.erase(displayId);
        return nullptr;
    }
    return displayMap_[displayId];
}

sptr<Display> DisplayManager::GetDisplayById(DisplayId displayId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (destroyed_) {
        return nullptr;
    }
    return pImpl_->GetDisplayById(displayId);
}

sptr<Display> DisplayManager::GetDisplayByScreen(ScreenId screenId)
{
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screenId is invalid.");
        return nullptr;
    }
    sptr<Display> display = pImpl_->GetDisplayByScreenId(screenId);
    if (display == nullptr) {
        WLOGFE("get display by screenId failed. screen %{public}" PRIu64"", screenId);
    }
    return display;
}

sptr<Display> DisplayManager::Impl::GetDisplayByScreenId(ScreenId screenId)
{
    sptr<DisplayInfo> displayInfo = SingletonContainer::Get<DisplayManagerAdapter>().GetDisplayInfoByScreenId(screenId);
    if (displayInfo == nullptr) {
        WLOGFE("get display by screenId: displayInfo is null");
        return nullptr;
    }
    DisplayId displayId = displayInfo->GetDisplayId();
    if (displayId == DISPLAY_ID_INVALID) {
        WLOGFE("get display by screenId: invalid displayInfo");
        return nullptr;
    }

    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!UpdateDisplayInfoLocked(displayInfo)) {
        displayMap_.erase(displayId);
        return nullptr;
    }
    return displayMap_[displayId];
}

std::shared_ptr<Media::PixelMap> DisplayManager::GetScreenshot(DisplayId displayId, DmErrorCode* errorCode)
{
    if (displayId == DISPLAY_ID_INVALID) {
        WLOGFE("displayId invalid!");
        return nullptr;
    }
    std::shared_ptr<Media::PixelMap> screenShot =
        SingletonContainer::Get<DisplayManagerAdapter>().GetDisplaySnapshot(displayId, errorCode);
    if (screenShot == nullptr) {
        WLOGFE("DisplayManager::GetScreenshot failed!");
        return nullptr;
    }

    return screenShot;
}

std::shared_ptr<Media::PixelMap> DisplayManager::GetScreenshot(DisplayId displayId, const Media::Rect &rect,
    const Media::Size &size, int rotation, DmErrorCode* errorCode)
{
    if (displayId == DISPLAY_ID_INVALID) {
        WLOGFE("displayId invalid!");
        return nullptr;
    }

    std::shared_ptr<Media::PixelMap> screenShot =
        SingletonContainer::Get<DisplayManagerAdapter>().GetDisplaySnapshot(displayId, errorCode);
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

DMError DisplayManager::DisableScreenshot(bool disableOrNot)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().DisableDisplaySnapshot(disableOrNot);
}

sptr<Display> DisplayManager::GetDefaultDisplay()
{
    return pImpl_->GetDefaultDisplay();
}

sptr<Display> DisplayManager::GetDefaultDisplaySync()
{
    return pImpl_->GetDefaultDisplaySync();
}

std::vector<DisplayId> DisplayManager::GetAllDisplayIds()
{
    return SingletonContainer::Get<DisplayManagerAdapter>().GetAllDisplayIds();
}

std::vector<sptr<Display>> DisplayManager::GetAllDisplays()
{
    std::vector<sptr<Display>> res;
    auto displayIds = GetAllDisplayIds();
    for (auto displayId: displayIds) {
        const sptr<Display> display = GetDisplayById(displayId);
        if (display != nullptr) {
            res.emplace_back(display);
        } else {
            WLOGFE("DisplayManager::GetAllDisplays display %" PRIu64" nullptr!", displayId);
        }
    }
    return res;
}

DMError DisplayManager::HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow)
{
    return pImpl_->HasPrivateWindow(displayId, hasPrivateWindow);
}

DMError DisplayManager::Impl::HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().HasPrivateWindow(displayId, hasPrivateWindow);
}

bool DisplayManager::IsFoldable()
{
    return pImpl_->IsFoldable();
}

bool DisplayManager::Impl::IsFoldable()
{
    return SingletonContainer::Get<DisplayManagerAdapter>().IsFoldable();
}

FoldStatus DisplayManager::GetFoldStatus()
{
    return pImpl_->GetFoldStatus();
}

FoldStatus DisplayManager::Impl::GetFoldStatus()
{
    return SingletonContainer::Get<DisplayManagerAdapter>().GetFoldStatus();
}

FoldDisplayMode DisplayManager::GetFoldDisplayMode()
{
    return pImpl_->GetFoldDisplayMode();
}

FoldDisplayMode DisplayManager::Impl::GetFoldDisplayMode()
{
    return SingletonContainer::Get<DisplayManagerAdapter>().GetFoldDisplayMode();
}

void DisplayManager::SetFoldDisplayMode(const FoldDisplayMode mode)
{
    pImpl_->SetFoldDisplayMode(mode);
}

void DisplayManager::Impl::SetFoldDisplayMode(const FoldDisplayMode mode)
{
    SingletonContainer::Get<DisplayManagerAdapter>().SetFoldDisplayMode(mode);
}

void DisplayManager::SetFoldStatusLocked(bool locked)
{
    pImpl_->SetFoldStatusLocked(locked);
}

void DisplayManager::Impl::SetFoldStatusLocked(bool locked)
{
    SingletonContainer::Get<DisplayManagerAdapter>().SetFoldStatusLocked(locked);
}

sptr<FoldCreaseRegion> DisplayManager::GetCurrentFoldCreaseRegion()
{
    return pImpl_->GetCurrentFoldCreaseRegion();
}

sptr<FoldCreaseRegion> DisplayManager::Impl::GetCurrentFoldCreaseRegion()
{
    return SingletonContainer::Get<DisplayManagerAdapter>().GetCurrentFoldCreaseRegion();
}

DMError DisplayManager::Impl::RegisterDisplayListener(sptr<IDisplayListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DMError ret = DMError::DM_OK;
    if (displayManagerListener_ == nullptr) {
        displayManagerListener_ = new DisplayManagerListener(this);
        ret = SingletonContainer::Get<DisplayManagerAdapter>().RegisterDisplayManagerAgent(
            displayManagerListener_,
            DisplayManagerAgentType::DISPLAY_EVENT_LISTENER);
    }
    if (ret != DMError::DM_OK) {
        WLOGFW("RegisterDisplayManagerAgent failed !");
        displayManagerListener_ = nullptr;
    } else {
        displayListeners_.insert(listener);
    }
    return ret;
}

void DisplayManager::Impl::NotifyPrivateWindowStateChanged(bool hasPrivate)
{
    std::set<sptr<IPrivateWindowListener>> privateWindowListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        privateWindowListeners = privateWindowListeners_;
    }
    for (auto& listener : privateWindowListeners) {
        listener->OnPrivateWindow(hasPrivate);
    }
}

DMError DisplayManager::RegisterPrivateWindowListener(sptr<IPrivateWindowListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("RegisterPrivateWindowListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->RegisterPrivateWindowListener(listener);
}

DMError DisplayManager::UnregisterPrivateWindowListener(sptr<IPrivateWindowListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("UnregisterPrivateWindowListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->UnregisterPrivateWindowListener(listener);
}

DMError DisplayManager::Impl::RegisterPrivateWindowListener(sptr<IPrivateWindowListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DMError ret = DMError::DM_OK;
    if (privateWindowListenerAgent_ == nullptr) {
        privateWindowListenerAgent_ = new DisplayManagerPrivateWindowAgent(this);
        ret = SingletonContainer::Get<DisplayManagerAdapter>().RegisterDisplayManagerAgent(
            privateWindowListenerAgent_,
            DisplayManagerAgentType::PRIVATE_WINDOW_LISTENER);
    }
    if (ret != DMError::DM_OK) {
        WLOGFW("RegisterDisplayManagerAgent failed !");
        privateWindowListenerAgent_ = nullptr;
    } else {
        WLOGI("privateWindowListener register success");
        privateWindowListeners_.insert(listener);
    }
    return ret;
}

DMError DisplayManager::Impl::UnregisterPrivateWindowListener(sptr<IPrivateWindowListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = std::find(privateWindowListeners_.begin(), privateWindowListeners_.end(), listener);
    if (iter == privateWindowListeners_.end()) {
        WLOGFE("could not find this listener");
        return DMError::DM_ERROR_NULLPTR;
    }
    privateWindowListeners_.erase(iter);
    DMError ret = DMError::DM_OK;
    if (privateWindowListeners_.empty() && privateWindowListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<DisplayManagerAdapter>().UnregisterDisplayManagerAgent(
            privateWindowListenerAgent_,
            DisplayManagerAgentType::PRIVATE_WINDOW_LISTENER);
        privateWindowListenerAgent_ = nullptr;
    }
    return ret;
}
DMError DisplayManager::RegisterDisplayListener(sptr<IDisplayListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("RegisterDisplayListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->RegisterDisplayListener(listener);
}

DMError DisplayManager::Impl::UnregisterDisplayListener(sptr<IDisplayListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = std::find(displayListeners_.begin(), displayListeners_.end(), listener);
    if (iter == displayListeners_.end()) {
        WLOGFE("could not find this listener");
        return DMError::DM_ERROR_NULLPTR;
    }
    displayListeners_.erase(iter);
    DMError ret = DMError::DM_OK;
    if (displayListeners_.empty() && displayManagerListener_ != nullptr) {
        ret = SingletonContainer::Get<DisplayManagerAdapter>().UnregisterDisplayManagerAgent(
            displayManagerListener_,
            DisplayManagerAgentType::DISPLAY_EVENT_LISTENER);
        displayManagerListener_ = nullptr;
    }
    return ret;
}

DMError DisplayManager::UnregisterDisplayListener(sptr<IDisplayListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("UnregisterDisplayListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->UnregisterDisplayListener(listener);
}

DMError DisplayManager::Impl::RegisterDisplayPowerEventListener(sptr<IDisplayPowerEventListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DMError ret = DMError::DM_OK;
    if (powerEventListenerAgent_ == nullptr) {
        powerEventListenerAgent_ = new DisplayManagerAgent(this);
        ret = SingletonContainer::Get<DisplayManagerAdapter>().RegisterDisplayManagerAgent(
            powerEventListenerAgent_,
            DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER);
    }
    if (ret != DMError::DM_OK) {
        WLOGFW("RegisterDisplayManagerAgent failed !");
        powerEventListenerAgent_ = nullptr;
    } else {
        powerEventListeners_.insert(listener);
    }
    WLOGFD("RegisterDisplayPowerEventListener end");
    return ret;
}

DMError DisplayManager::RegisterDisplayPowerEventListener(sptr<IDisplayPowerEventListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->RegisterDisplayPowerEventListener(listener);
}

DMError DisplayManager::Impl::UnregisterDisplayPowerEventListener(sptr<IDisplayPowerEventListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = std::find(powerEventListeners_.begin(), powerEventListeners_.end(), listener);
    if (iter == powerEventListeners_.end()) {
        WLOGFE("could not find this listener");
        return DMError::DM_ERROR_NULLPTR;
    }
    powerEventListeners_.erase(iter);
    DMError ret = DMError::DM_OK;
    if (powerEventListeners_.empty() && powerEventListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<DisplayManagerAdapter>().UnregisterDisplayManagerAgent(
            powerEventListenerAgent_,
            DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER);
        powerEventListenerAgent_ = nullptr;
    }
    WLOGFD("UnregisterDisplayPowerEventListener end");
    return ret;
}

DMError DisplayManager::UnregisterDisplayPowerEventListener(sptr<IDisplayPowerEventListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->UnregisterDisplayPowerEventListener(listener);
}

DMError DisplayManager::Impl::RegisterScreenshotListener(sptr<IScreenshotListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DMError ret = DMError::DM_OK;
    if (screenshotListenerAgent_ == nullptr) {
        screenshotListenerAgent_ = new DisplayManagerScreenshotAgent(this);
        ret = SingletonContainer::Get<DisplayManagerAdapter>().RegisterDisplayManagerAgent(
            screenshotListenerAgent_,
            DisplayManagerAgentType::SCREENSHOT_EVENT_LISTENER);
    }
    if (ret != DMError::DM_OK) {
        WLOGFW("RegisterDisplayManagerAgent failed !");
        screenshotListenerAgent_ = nullptr;
    } else {
        screenshotListeners_.insert(listener);
    }
    return ret;
}

DMError DisplayManager::RegisterScreenshotListener(sptr<IScreenshotListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("RegisterScreenshotListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->RegisterScreenshotListener(listener);
}

DMError DisplayManager::Impl::UnregisterScreenshotListener(sptr<IScreenshotListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = std::find(screenshotListeners_.begin(), screenshotListeners_.end(), listener);
    if (iter == screenshotListeners_.end()) {
        WLOGFE("could not find this listener");
        return DMError::DM_ERROR_NULLPTR;
    }
    screenshotListeners_.erase(iter);
    DMError ret = DMError::DM_OK;
    if (screenshotListeners_.empty() && screenshotListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<DisplayManagerAdapter>().UnregisterDisplayManagerAgent(
            screenshotListenerAgent_,
            DisplayManagerAgentType::SCREENSHOT_EVENT_LISTENER);
        screenshotListenerAgent_ = nullptr;
    }
    return ret;
}

DMError DisplayManager::UnregisterScreenshotListener(sptr<IScreenshotListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("UnregisterScreenshotListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->UnregisterScreenshotListener(listener);
}

void DisplayManager::Impl::NotifyFoldStatusChanged(FoldStatus foldStatus)
{
    std::set<sptr<IFoldStatusListener>> foldStatusListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        foldStatusListeners = foldStatusListeners_;
    }
    for (auto& listener : foldStatusListeners) {
        listener->OnFoldStatusChanged(foldStatus);
    }
}

DMError DisplayManager::RegisterFoldStatusListener(sptr<IFoldStatusListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("IFoldStatusListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->RegisterFoldStatusListener(listener);
}

DMError DisplayManager::Impl::RegisterFoldStatusListener(sptr<IFoldStatusListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DMError ret = DMError::DM_OK;
    if (foldStatusListenerAgent_ == nullptr) {
        foldStatusListenerAgent_ = new DisplayManagerFoldStatusAgent(this);
        ret = SingletonContainer::Get<DisplayManagerAdapter>().RegisterDisplayManagerAgent(
            foldStatusListenerAgent_,
            DisplayManagerAgentType::FOLD_STATUS_CHANGED_LISTENER);
    }
    if (ret != DMError::DM_OK) {
        WLOGFW("RegisterFoldStatusListener failed !");
        foldStatusListenerAgent_ = nullptr;
    } else {
        WLOGI("IFoldStatusListener register success");
        foldStatusListeners_.insert(listener);
    }
    return ret;
}

DMError DisplayManager::UnregisterFoldStatusListener(sptr<IFoldStatusListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("UnregisterFoldStatusListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->UnregisterFoldStatusListener(listener);
}

DMError DisplayManager::Impl::UnregisterFoldStatusListener(sptr<IFoldStatusListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = std::find(foldStatusListeners_.begin(), foldStatusListeners_.end(), listener);
    if (iter == foldStatusListeners_.end()) {
        WLOGFE("could not find this listener");
        return DMError::DM_ERROR_NULLPTR;
    }
    foldStatusListeners_.erase(iter);
    DMError ret = DMError::DM_OK;
    if (foldStatusListeners_.empty() && foldStatusListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<DisplayManagerAdapter>().UnregisterDisplayManagerAgent(
            foldStatusListenerAgent_,
            DisplayManagerAgentType::FOLD_STATUS_CHANGED_LISTENER);
        foldStatusListenerAgent_ = nullptr;
    }
    return ret;
}

void DisplayManager::Impl::NotifyDisplayChangeInfoChanged(const sptr<DisplayChangeInfo>& info)
{
    std::set<sptr<IDisplayUpdateListener>> displayUpdateListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        displayUpdateListeners = displayUpdateListeners_;
    }
    for (auto& listener : displayUpdateListeners) {
        listener->OnDisplayUpdate(info);
    }
}

DMError DisplayManager::RegisterDisplayUpdateListener(sptr<IDisplayUpdateListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("IDisplayUpdateListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->RegisterDisplayUpdateListener(listener);
}

DMError DisplayManager::Impl::RegisterDisplayUpdateListener(sptr<IDisplayUpdateListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DMError ret = DMError::DM_OK;
    if (displayUpdateListenerAgent_ == nullptr) {
        displayUpdateListenerAgent_ = new DisplayManagerDisplayUpdateAgent(this);
        ret = SingletonContainer::Get<DisplayManagerAdapter>().RegisterDisplayManagerAgent(
            displayUpdateListenerAgent_,
            DisplayManagerAgentType::DISPLAY_UPDATE_LISTENER);
    }
    if (ret != DMError::DM_OK) {
        WLOGFW("RegisterDisplayUpdateListener failed !");
        displayUpdateListenerAgent_ = nullptr;
    } else {
        WLOGI("IDisplayUpdateListener register success");
        displayUpdateListeners_.insert(listener);
    }
    return ret;
}

DMError DisplayManager::UnregisterDisplayUpdateListener(sptr<IDisplayUpdateListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("UnregisterDisplayUpdateListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->UnregisterDisplayUpdateListener(listener);
}

DMError DisplayManager::Impl::UnregisterDisplayUpdateListener(sptr<IDisplayUpdateListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = std::find(displayUpdateListeners_.begin(), displayUpdateListeners_.end(), listener);
    if (iter == displayUpdateListeners_.end()) {
        WLOGFE("could not find this listener");
        return DMError::DM_ERROR_NULLPTR;
    }
    displayUpdateListeners_.erase(iter);
    DMError ret = DMError::DM_OK;
    if (displayUpdateListeners_.empty() && displayUpdateListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<DisplayManagerAdapter>().UnregisterDisplayManagerAgent(
            displayUpdateListenerAgent_,
            DisplayManagerAgentType::DISPLAY_UPDATE_LISTENER);
        displayUpdateListenerAgent_ = nullptr;
    }
    return ret;
}

void DisplayManager::Impl::NotifyDisplayModeChanged(FoldDisplayMode displayMode)
{
    std::set<sptr<IDisplayModeListener>> displayModeListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        displayModeListeners = displayModeListeners_;
    }
    for (auto& listener : displayModeListeners) {
        listener->OnDisplayModeChanged(displayMode);
    }
}

void DisplayManager::Impl::NotifyAvailableAreaChanged(DMRect rect)
{
    std::set<sptr<IAvailableAreaListener>> availableAreaListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        availableAreaListeners = availableAreaListeners_;
    }
    for (auto& listener : availableAreaListeners) {
        listener->OnAvailableAreaChanged(rect);
    }
}

DMError DisplayManager::RegisterDisplayModeListener(sptr<IDisplayModeListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("IDisplayModeListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->RegisterDisplayModeListener(listener);
}

DMError DisplayManager::Impl::RegisterDisplayModeListener(sptr<IDisplayModeListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DMError ret = DMError::DM_OK;
    if (displayModeListenerAgent_ == nullptr) {
        displayModeListenerAgent_ = new DisplayManagerDisplayModeAgent(this);
        ret = SingletonContainer::Get<DisplayManagerAdapter>().RegisterDisplayManagerAgent(
            displayModeListenerAgent_,
            DisplayManagerAgentType::DISPLAY_MODE_CHANGED_LISTENER);
    }
    if (ret != DMError::DM_OK) {
        WLOGFW("RegisterDisplayModeListener failed !");
        displayModeListenerAgent_ = nullptr;
    } else {
        WLOGI("IDisplayModeListener register success");
        displayModeListeners_.insert(listener);
    }
    return ret;
}

DMError DisplayManager::UnregisterDisplayModeListener(sptr<IDisplayModeListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("UnregisterPrivateWindowListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->UnregisterDisplayModeListener(listener);
}

DMError DisplayManager::Impl::UnregisterDisplayModeListener(sptr<IDisplayModeListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = std::find(displayModeListeners_.begin(), displayModeListeners_.end(), listener);
    if (iter == displayModeListeners_.end()) {
        WLOGFE("could not find this listener");
        return DMError::DM_ERROR_NULLPTR;
    }
    displayModeListeners_.erase(iter);
    DMError ret = DMError::DM_OK;
    if (displayModeListeners_.empty() && displayModeListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<DisplayManagerAdapter>().UnregisterDisplayManagerAgent(
            displayModeListenerAgent_,
            DisplayManagerAgentType::DISPLAY_MODE_CHANGED_LISTENER);
        displayModeListenerAgent_ = nullptr;
    }
    return ret;
}

DMError DisplayManager::RegisterAvailableAreaListener(sptr<IAvailableAreaListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("RegisterAvailableAreaListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->RegisterAvailableAreaListener(listener);
}

DMError DisplayManager::Impl::RegisterAvailableAreaListener(sptr<IAvailableAreaListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    DMError ret = DMError::DM_OK;
    if (availableAreaListenerAgent_ == nullptr) {
        availableAreaListenerAgent_ = new DisplayManagerAvailableAreaAgent(this);
        ret = SingletonContainer::Get<DisplayManagerAdapter>().RegisterDisplayManagerAgent(
            availableAreaListenerAgent_,
            DisplayManagerAgentType::AVAILABLE_AREA_CHANGED_LISTENER);
    }
    if (ret != DMError::DM_OK) {
        WLOGFW("RegisterAvailableAreaListener failed !");
        availableAreaListenerAgent_ = nullptr;
    } else {
        WLOGI("IAvailableAreaListener register success");
        availableAreaListeners_.insert(listener);
    }
    return ret;
}

DMError DisplayManager::UnregisterAvailableAreaListener(sptr<IAvailableAreaListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("UnregisterPrivateWindowListener listener is nullptr.");
        return DMError::DM_ERROR_NULLPTR;
    }
    return pImpl_->UnregisterAvailableAreaListener(listener);
}

DMError DisplayManager::Impl::UnregisterAvailableAreaListener(sptr<IAvailableAreaListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = std::find(availableAreaListeners_.begin(), availableAreaListeners_.end(), listener);
    if (iter == availableAreaListeners_.end()) {
        WLOGFE("could not find this listener");
        return DMError::DM_ERROR_NULLPTR;
    }
    availableAreaListeners_.erase(iter);
    DMError ret = DMError::DM_OK;
    if (availableAreaListeners_.empty() && availableAreaListenerAgent_ != nullptr) {
        ret = SingletonContainer::Get<DisplayManagerAdapter>().UnregisterDisplayManagerAgent(
            availableAreaListenerAgent_,
            DisplayManagerAgentType::AVAILABLE_AREA_CHANGED_LISTENER);
        availableAreaListenerAgent_ = nullptr;
    }
    return ret;
}

void DisplayManager::Impl::NotifyScreenshot(sptr<ScreenshotInfo> info)
{
    WLOGFD("NotifyScreenshot trigger:[%{public}s] displayId:%{public}" PRIu64" size:%{public}zu",
        info->GetTrigger().c_str(), info->GetDisplayId(), screenshotListeners_.size());
    std::set<sptr<IScreenshotListener>> screenshotListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        screenshotListeners = screenshotListeners_;
    }
    for (auto& listener : screenshotListeners) {
        listener->OnScreenshot(*info);
    }
}

void DisplayManager::Impl::NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status)
{
    WLOGFD("NotifyDisplayPowerEvent event:%{public}u, status:%{public}u, size:%{public}zu", event, status,
        powerEventListeners_.size());
    std::set<sptr<IDisplayPowerEventListener>> powerEventListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        powerEventListeners = powerEventListeners_;
    }
    for (auto& listener : powerEventListeners) {
        listener->OnDisplayPowerEvent(event, status);
    }
}

void DisplayManager::Impl::NotifyDisplayStateChanged(DisplayId id, DisplayState state)
{
    WLOGFD("state:%{public}u", state);
    DisplayStateCallback displayStateCallback = nullptr;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        displayStateCallback = displayStateCallback_;
    }
    if (displayStateCallback) {
        displayStateCallback(state);
        ClearDisplayStateCallback();
        return;
    }
    WLOGFW("callback_ target is not set!");
}

void DisplayManager::Impl::NotifyDisplayCreate(sptr<DisplayInfo> info)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    UpdateDisplayInfoLocked(info);
}

void DisplayManager::Impl::NotifyDisplayDestroy(DisplayId displayId)
{
    WLOGFD("displayId:%{public}" PRIu64".", displayId);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    displayMap_.erase(displayId);
}

void DisplayManager::Impl::NotifyDisplayChange(sptr<DisplayInfo> displayInfo)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    UpdateDisplayInfoLocked(displayInfo);
}

bool DisplayManager::Impl::UpdateDisplayInfoLocked(sptr<DisplayInfo> displayInfo)
{
    if (displayInfo == nullptr) {
        WLOGFE("displayInfo is null");
        return false;
    }
    DisplayId displayId = displayInfo->GetDisplayId();
    WLOGFD("displayId:%{public}" PRIu64".", displayId);
    if (displayId == DISPLAY_ID_INVALID) {
        WLOGFE("displayId is invalid.");
        return false;
    }
    auto iter = displayMap_.find(displayId);
    if (iter != displayMap_.end() && iter->second != nullptr) {
        WLOGFD("get screen in screen map");
        iter->second->UpdateDisplayInfo(displayInfo);
        return true;
    }
    sptr<Display> display = new Display("", displayInfo);
    displayMap_[displayId] = display;
    return true;
}

bool DisplayManager::WakeUpBegin(PowerStateChangeReason reason)
{
    WLOGFD("WakeUpBegin start, reason:%{public}u", reason);
    return SingletonContainer::Get<DisplayManagerAdapter>().WakeUpBegin(reason);
}

bool DisplayManager::WakeUpEnd()
{
    WLOGFD("WakeUpEnd start");
    return SingletonContainer::Get<DisplayManagerAdapter>().WakeUpEnd();
}

bool DisplayManager::SuspendBegin(PowerStateChangeReason reason)
{
    // dms->wms notify other windows to hide
    WLOGFD("SuspendBegin start, reason:%{public}u", reason);
    return SingletonContainer::Get<DisplayManagerAdapter>().SuspendBegin(reason);
}

bool DisplayManager::SuspendEnd()
{
    WLOGFD("SuspendEnd start");
    return SingletonContainer::Get<DisplayManagerAdapter>().SuspendEnd();
}

bool DisplayManager::Impl::SetDisplayState(DisplayState state, DisplayStateCallback callback)
{
    WLOGFD("state:%{public}u", state);
    bool ret = true;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (displayStateCallback_ != nullptr || callback == nullptr) {
            WLOGFI("previous callback not called or callback invalid");
            return false;
        }
        displayStateCallback_ = callback;

        if (displayStateAgent_ == nullptr) {
            displayStateAgent_ = new DisplayManagerAgent(this);
            ret = SingletonContainer::Get<DisplayManagerAdapter>().RegisterDisplayManagerAgent(
                displayStateAgent_,
                DisplayManagerAgentType::DISPLAY_STATE_LISTENER) == DMError::DM_OK;
        }
    }
    ret = ret && SingletonContainer::Get<DisplayManagerAdapter>().SetDisplayState(state);
    if (!ret) {
        ClearDisplayStateCallback();
    }
    return ret;
}

bool DisplayManager::SetDisplayState(DisplayState state, DisplayStateCallback callback)
{
    return pImpl_->SetDisplayState(state, callback);
}

DisplayState DisplayManager::GetDisplayState(DisplayId displayId)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().GetDisplayState(displayId);
}

bool DisplayManager::SetScreenBrightness(uint64_t screenId, uint32_t level)
{
    WLOGFD("screenId:%{public}" PRIu64", level:%{public}u,", screenId, level);
    RSInterfaces::GetInstance().SetScreenBacklight(screenId, level);
    return true;
}

uint32_t DisplayManager::GetScreenBrightness(uint64_t screenId) const
{
    uint32_t level = static_cast<uint32_t>(RSInterfaces::GetInstance().GetScreenBacklight(screenId));
    WLOGFD("screenId:%{public}" PRIu64", level:%{public}u,", screenId, level);
    return level;
}

void DisplayManager::NotifyDisplayEvent(DisplayEvent event)
{
    // Unlock event dms->wms restore other hidden windows
    WLOGFD("DisplayEvent:%{public}u", event);
    SingletonContainer::Get<DisplayManagerAdapter>().NotifyDisplayEvent(event);
}

bool DisplayManager::Freeze(std::vector<DisplayId> displayIds)
{
    WLOGFD("freeze display");
    if (displayIds.size() == 0) {
        WLOGFE("freeze display fail, num of display is 0");
        return false;
    }
    if (displayIds.size() > MAX_DISPLAY_SIZE) {
        WLOGFE("freeze display fail, displayIds size is bigger than %{public}u.", MAX_DISPLAY_SIZE);
        return false;
    }
    return SingletonContainer::Get<DisplayManagerAdapter>().SetFreeze(displayIds, true);
}

bool DisplayManager::Unfreeze(std::vector<DisplayId> displayIds)
{
    WLOGFD("unfreeze display");
    if (displayIds.size() == 0) {
        WLOGFE("unfreeze display fail, num of display is 0");
        return false;
    }
    if (displayIds.size() > MAX_DISPLAY_SIZE) {
        WLOGFE("unfreeze display fail, displayIds size is bigger than %{public}u.", MAX_DISPLAY_SIZE);
        return false;
    }
    return SingletonContainer::Get<DisplayManagerAdapter>().SetFreeze(displayIds, false);
}

DMError DisplayManager::AddSurfaceNodeToDisplay(DisplayId displayId,
    std::shared_ptr<class RSSurfaceNode>& surfaceNode)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().AddSurfaceNodeToDisplay(displayId, surfaceNode);
}

DMError DisplayManager::RemoveSurfaceNodeFromDisplay(DisplayId displayId,
    std::shared_ptr<class RSSurfaceNode>& surfaceNode)
{
    return SingletonContainer::Get<DisplayManagerAdapter>().RemoveSurfaceNodeFromDisplay(displayId, surfaceNode);
}

void DisplayManager::Impl::OnRemoteDied()
{
    WLOGFI("dms is died");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    displayManagerListener_ = nullptr;
    displayStateAgent_ = nullptr;
    powerEventListenerAgent_ = nullptr;
    screenshotListenerAgent_ = nullptr;
    privateWindowListenerAgent_ = nullptr;
    foldStatusListenerAgent_ = nullptr;
}

void DisplayManager::OnRemoteDied()
{
    pImpl_->OnRemoteDied();
}
} // namespace OHOS::Rosen