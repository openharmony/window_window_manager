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
#include "window_manager_hilog.h"
#include "session_manager_lite.h"
#include "focus_change_info.h"
#include "singleton_delegator.h"


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
        isProxyValid_ = true;
    }
    return true;
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

} // namespace Rosen
} // namespace OHOS
