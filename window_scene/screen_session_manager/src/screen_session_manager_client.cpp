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

#include "screen_session_manager_client.h"

#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "permission.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenSessionManagerClient" };
const std::string SCREEN_CAPTURE_PERMISSION = "ohos.permission.CAPTURE_SCREEN";
} // namespace

WM_IMPLEMENT_SINGLE_INSTANCE(ScreenSessionManagerClient)

void ScreenSessionManagerClient::ConnectToServer()
{
    if (screenSessionManager_) {
        return;
    }
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityMgr) {
        WLOGFE("Failed to get system ability mgr");
        return;
    }

    auto remoteObject = systemAbilityMgr->GetSystemAbility(DISPLAY_MANAGER_SERVICE_SA_ID);
    if (!remoteObject) {
        WLOGFE("Failed to get display manager service");
        return;
    }

    screenSessionManager_ = iface_cast<IScreenSessionManager>(remoteObject);
    if (!screenSessionManager_) {
        WLOGFE("Failed to get screen session manager proxy");
        return;
    }
    screenSessionManager_->SetClient(this);
}

void ScreenSessionManagerClient::RegisterScreenConnectionListener(IScreenConnectionListener* listener)
{
    if (listener == nullptr) {
        WLOGFE("Failed to register screen connection listener, listener is null");
        return;
    }

    screenConnectionListener_ = listener;
    ConnectToServer();
}

void ScreenSessionManagerClient::RegisterScreenInfoChangeListener(const ScreenInfoChangeClientListener& listener)
{
    if (listener == nullptr) {
        WLOGFE("Failed RegisterScreenInfoChangeListener while listener is nullptr");
        return;
    }
    screenInfoChangeListener_  = listener;
}

void ScreenSessionManagerClient::OnScreenConnectionChanged(ScreenId screenId, ScreenEvent screenEvent,
    ScreenId rsId, const std::string& name)
{
    WLOGFI("screenId: %{public}" PRIu64 " screenEvent: %{public}d rsId: %{public}" PRIu64 " name: %{public}s",
        screenId, static_cast<int>(screenEvent), rsId, name.c_str());
    if (screenEvent == ScreenEvent::CONNECTED) {
        auto screenProperty = screenSessionManager_->GetScreenProperty(screenId);
        auto displayNode = screenSessionManager_->GetDisplayNode(screenId);
        sptr<ScreenSession> screenSession = new ScreenSession(screenId, rsId, name, screenProperty, displayNode);
        {
            std::lock_guard<std::mutex> lock(screenSessionMapMutex_);
            screenSessionMap_.emplace(screenId, screenSession);
        }
        if (screenConnectionListener_) {
            screenConnectionListener_->OnScreenConnected(screenSession);
        }
        screenSession->Connect();
        return;
    }
    if (screenEvent == ScreenEvent::DISCONNECTED) {
        auto screenSession = GetScreenSession(screenId);
        if (!screenSession) {
            WLOGFE("screenSession is null");
            return;
        }
        if (screenConnectionListener_) {
            screenConnectionListener_->OnScreenDisconnected(screenSession);
        }
        {
            std::lock_guard<std::mutex> lock(screenSessionMapMutex_);
            screenSessionMap_.erase(screenId);
        }
    }
}

sptr<ScreenSession> ScreenSessionManagerClient::GetScreenSession(ScreenId screenId) const
{
    std::lock_guard<std::mutex> lock(screenSessionMapMutex_);
    auto iter = screenSessionMap_.find(screenId);
    if (iter == screenSessionMap_.end()) {
        WLOGFD("Error found screen session with id: %{public}" PRIu64, screenId);
        return nullptr;
    }
    return iter->second;
}

void ScreenSessionManagerClient::OnPropertyChanged(ScreenId screenId,
    const ScreenProperty& property, ScreenPropertyChangeReason reason)
{
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is null");
        return;
    }
    screenSession->PropertyChange(property, reason);
    if (screenInfoChangeListener_) {
        screenInfoChangeListener_(screenId);
    }
}

void ScreenSessionManagerClient::OnPowerStatusChanged(DisplayPowerEvent event, EventStatus status,
    PowerStateChangeReason reason)
{
    for (auto screenSession:screenSessionMap_) {
        (screenSession.second)->PowerStatusChange(event, status, reason);
    }
}

void ScreenSessionManagerClient::OnSensorRotationChanged(ScreenId screenId, float sensorRotation)
{
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is null");
        return;
    }
    screenSession->SensorRotationChange(sensorRotation);
}

void ScreenSessionManagerClient::OnScreenOrientationChanged(ScreenId screenId, float screenOrientation)
{
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is null");
        return;
    }
    screenSession->ScreenOrientationChange(screenOrientation);
}

void ScreenSessionManagerClient::OnScreenRotationLockedChanged(ScreenId screenId, bool isLocked)
{
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is null");
        return;
    }
    screenSession->SetScreenRotationLocked(isLocked);
}

void ScreenSessionManagerClient::RegisterDisplayChangeListener(const sptr<IDisplayChangeListener>& listener)
{
    displayChangeListener_ = listener;
}

void ScreenSessionManagerClient::OnDisplayStateChanged(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    if (displayChangeListener_) {
        displayChangeListener_->OnDisplayStateChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    }
}

void ScreenSessionManagerClient::OnGetSurfaceNodeIdsFromMissionIdsChanged(std::vector<uint64_t>& missionIds,
    std::vector<uint64_t>& surfaceNodeIds)
{
    if (displayChangeListener_) {
        displayChangeListener_->OnGetSurfaceNodeIdsFromMissionIds(missionIds, surfaceNodeIds);
    }
}

void ScreenSessionManagerClient::OnScreenshot(DisplayId displayId)
{
    if (displayChangeListener_) {
        displayChangeListener_->OnScreenshot(displayId);
    }
}

void ScreenSessionManagerClient::OnImmersiveStateChanged(bool& immersive)
{
    if (displayChangeListener_ != nullptr) {
        displayChangeListener_->OnImmersiveStateChange(immersive);
    }
}

std::unordered_map<ScreenId, ScreenProperty> ScreenSessionManagerClient::GetAllScreensProperties() const
{
    std::lock_guard<std::mutex> lock(screenSessionMapMutex_);
    std::unordered_map<ScreenId, ScreenProperty> screensProperties;
    for (const auto& iter: screenSessionMap_) {
        auto session = iter.second;
        if (session == nullptr) {
            continue;
        }
        screensProperties[iter.first] = session->GetScreenProperty();
    }
    return screensProperties;
}

FoldDisplayMode ScreenSessionManagerClient::GetFoldDisplayMode() const
{
    if (screenSessionManager_ == nullptr) {
        WLOGFE("screenSessionManager_ is null while get displayMode");
        return FoldDisplayMode::UNKNOWN;
    }
    return screenSessionManager_->GetFoldDisplayMode();
}

void ScreenSessionManagerClient::UpdateScreenRotationProperty(ScreenId screenId, const RRect& bounds, float rotation)
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return;
    }
    screenSessionManager_->UpdateScreenRotationProperty(screenId, bounds, rotation);

    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is null");
        return;
    }
    auto foldDisplayMode = screenSessionManager_->GetFoldDisplayMode();
    screenSession->UpdateToInputManager(bounds, rotation, foldDisplayMode);
    if (screenInfoChangeListener_) {
        screenInfoChangeListener_(screenId);
    }
}

void ScreenSessionManagerClient::SetDisplayNodeScreenId(ScreenId screenId, ScreenId displayNodeScreenId)
{
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is null");
        return;
    }
    screenSession->SetDisplayNodeScreenId(displayNodeScreenId);
}

uint32_t ScreenSessionManagerClient::GetCurvedCompressionArea()
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return 0;
    }
    return screenSessionManager_->GetCurvedCompressionArea();
}

ScreenProperty ScreenSessionManagerClient::GetPhyScreenProperty(ScreenId screenId)
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return {};
    }
    return screenSessionManager_->GetPhyScreenProperty(screenId);
}

__attribute__((no_sanitize("cfi")))
void ScreenSessionManagerClient::NotifyDisplayChangeInfoChanged(const sptr<DisplayChangeInfo>& info)
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return;
    }
    screenSessionManager_->NotifyDisplayChangeInfoChanged(info);
}

void ScreenSessionManagerClient::SetScreenPrivacyState(bool hasPrivate)
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return;
    }
    screenSessionManager_->SetScreenPrivacyState(hasPrivate);
}

void ScreenSessionManagerClient::UpdateAvailableArea(ScreenId screenId, DMRect area)
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return;
    }
    screenSessionManager_->UpdateAvailableArea(screenId, area);
}

FoldStatus ScreenSessionManagerClient::GetFoldStatus()
{
    if (!screenSessionManager_) {
        WLOGFE("screenSessionManager_ is null");
        return FoldStatus::UNKNOWN;
    }
    return screenSessionManager_->GetFoldStatus();
}

std::shared_ptr<Media::PixelMap> ScreenSessionManagerClient::GetScreenSnapshot(ScreenId screenId,
    float scaleX, float scaleY)
{
    if (!Permission::IsSystemCalling() || !Permission::CheckCallingPermission(SCREEN_CAPTURE_PERMISSION)) {
        WLOGFE("get screen session is not system calling, permission denied!");
        return nullptr;
    }

    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("get screen session is null");
        return nullptr;
    }
    return screenSession->GetScreenSnapshot(scaleX, scaleY);
}
} // namespace OHOS::Rosen
