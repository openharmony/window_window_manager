/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the Licenses at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is be distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "window_adapter_lite.h"
#include <key_event.h>
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "scene_board_judgement.h"
#include "session_manager_lite.h"
#include "focus_change_info.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowAdapterLite"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(WindowAdapterLite)

#define INIT_PROXY_CHECK_RETURN(ret)        \
    do {                                    \
        if (!InitSSMProxy()) {              \
            WLOGFE("InitSSMProxy failed!"); \
            return ret;                     \
        }                                   \
    } while (false)

WMError WindowAdapterLite::RegisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    {
        std::unique_lock<std::shared_mutex> lock(windowManagerLiteAgentMapMutex_);
        if (windowManagerLiteAgentMap_.find(type) == windowManagerLiteAgentMap_.end()) {
            windowManagerLiteAgentMap_[type] = std::set<sptr<IWindowManagerAgent>>();
        }
        windowManagerLiteAgentMap_[type].insert(windowManagerAgent);
    }
    return windowManagerServiceProxy_->RegisterWindowManagerAgent(type, windowManagerAgent);
}

WMError WindowAdapterLite::UnregisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    auto ret = windowManagerServiceProxy_->UnregisterWindowManagerAgent(type, windowManagerAgent);
    std::unique_lock<std::shared_mutex> lock(windowManagerLiteAgentMapMutex_);
    if (windowManagerLiteAgentMap_.find(type) == windowManagerLiteAgentMap_.end()) {
        TLOGW(WmsLogTag::WMS_MULTI_USER, "WindowManagerAgentType = %{public}d not found", type);
        return ret;
    }

    auto& agentSet = windowManagerLiteAgentMap_[type];
    auto agent = std::find(agentSet.begin(), agentSet.end(), windowManagerAgent);
    if (agent == agentSet.end()) {
        TLOGW(WmsLogTag::WMS_MULTI_USER, "Cannot find agent,  type = %{public}d", type);
        return ret;
    }
    agentSet.erase(agent);
    return ret;
}

void WindowAdapterLite::ReregisterWindowManagerLiteAgent()
{
    std::shared_lock<std::shared_mutex> lock(windowManagerLiteAgentMapMutex_);
    if ((!windowManagerServiceProxy_) || (!windowManagerServiceProxy_->AsObject())) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "windowManagerServiceProxy_ is null");
        return;
    }
    for (const auto& it : windowManagerLiteAgentMap_) {
        TLOGI(WmsLogTag::WMS_MULTI_USER, "Window manager agent type = %{public}" PRIu32 ", size = %{public}" PRIu64,
            it.first, static_cast<uint64_t>(it.second.size()));
        for (auto& agent : it.second) {
            if (windowManagerServiceProxy_->RegisterWindowManagerAgent(it.first, agent) != WMError::WM_OK) {
                TLOGW(WmsLogTag::WMS_MULTI_USER, "Reregister window manager agent failed");
            }
        }
    }
}

WMError WindowAdapterLite::CheckWindowId(int32_t windowId, int32_t &pid)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->CheckWindowId(windowId, pid);
}

WMError WindowAdapterLite::GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->GetVisibilityWindowInfo(infos);
}

bool WindowAdapterLite::InitSSMProxy()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isProxyValid_) {
        windowManagerServiceProxy_ = SessionManagerLite::GetInstance().GetSceneSessionManagerLiteProxy();
        if ((!windowManagerServiceProxy_) || (!windowManagerServiceProxy_->AsObject())) {
            WLOGFE("Failed to get system scene session manager services");
            return false;
        }
        wmsDeath_ = new (std::nothrow) WMSDeathRecipient();
        if (!wmsDeath_) {
            WLOGFE("Failed to create death Recipient ptr WMSDeathRecipient");
            return false;
        }
        sptr<IRemoteObject> remoteObject = windowManagerServiceProxy_->AsObject();
        if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(wmsDeath_)) {
            WLOGFE("Failed to add death recipient");
            return false;
        }
        // U0 system user needs to subscribe OnUserSwitch event
        int32_t clientUserId = GetUserIdByUid(getuid());
        if (clientUserId == SYSTEM_USERID && !isRegisteredUserSwitchListener_) {
            SessionManagerLite::GetInstance().RegisterUserSwitchListener(
                std::bind(&WindowAdapterLite::OnUserSwitch, this));
            isRegisteredUserSwitchListener_ = true;
        }
        isProxyValid_ = true;
    }
    return true;
}

void WindowAdapterLite::OnUserSwitch()
{
    TLOGI(WmsLogTag::WMS_MULTI_USER, "User switched lite");
    ClearWindowAdapter();
    InitSSMProxy();
    ReregisterWindowManagerLiteAgent();
}

void WindowAdapterLite::ClearWindowAdapter()
{
    WLOGD("ClearWindowAdapter");
    if ((windowManagerServiceProxy_ != nullptr) && (windowManagerServiceProxy_->AsObject() != nullptr)) {
        windowManagerServiceProxy_->AsObject()->RemoveDeathRecipient(wmsDeath_);
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    isProxyValid_ = false;
}

void WMSDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    WLOGD("OnRemoteDied");
    if (wptrDeath == nullptr) {
        WLOGFE("wptrDeath is null");
        return;
    }
    WLOGD("promote Death");
    sptr<IRemoteObject> object = wptrDeath.promote();
    if (!object) {
        WLOGFE("object is null");
        return;
    }
    WLOGI("wms OnRemoteDied");
    SingletonContainer::Get<WindowAdapterLite>().ClearWindowAdapter();
    SingletonContainer::Get<SessionManagerLite>().ClearSessionManagerProxy();
}

void WindowAdapterLite::GetFocusWindowInfo(FocusChangeInfo& focusInfo)
{
    INIT_PROXY_CHECK_RETURN();
    WLOGFI("use Foucus window info proxy");
    return windowManagerServiceProxy_->GetFocusWindowInfo(focusInfo);
}

WMError WindowAdapterLite::GetWindowModeType(WindowModeType& windowModeType)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    WLOGFD("get window mode type");
    return windowManagerServiceProxy_->GetWindowModeType(windowModeType);
}

WMError WindowAdapterLite::GetMainWindowInfos(int32_t topNum, std::vector<MainWindowInfo>& topNInfo)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    TLOGD(WmsLogTag::WMS_MAIN, "get top main window info");
    return windowManagerServiceProxy_->GetMainWindowInfos(topNum, topNInfo);
}
} // namespace Rosen
} // namespace OHOS
