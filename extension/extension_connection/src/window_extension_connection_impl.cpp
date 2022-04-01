/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#include "window_extension_connection_impl.h"

#include "ability_manager_client.h"
#include "element_name.h"
#include <iremote_object.h>
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowExtensionConnectionImpl"};
}

void WindowExtensionConnectionImpl::ConnectExtension(const AppExecFwk::ElementName &element, Rect rect,
        uint32_t uid, sptr<IWindowExtensionCallback>& callback)
{
    AAFwk::Want want;
    want.SetElement(element);
    // want.SetParam(RECT_FORM_KEY_POS_X, rect.posX_);
    // want.SetParam(RECT_FORM_KEY_POS_Y, rect.posY_);
    // want.SetParam(RECT_FORM_KEY_WIDTH, rect.width_);
    // want.SetParam(RECT_FORM_KEY_HEIGHT, rect.height_);
    if (stub_ == nullptr) {
        // stub_ = new(std::nothrow) WindowExtensionServerStub();
    }
    if (AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(
        want, this, stub_, uid / 200000) != ERR_OK) { // 200000 is uid mask
        WLOGFE("ConnectAbility failed!");
        return;
    }
    componentCallback_ = callback;
}

void WindowExtensionConnectionImpl::Show()
{
    if (proxy_ != nullptr) {
        proxy_->Show();
    }
}

void WindowExtensionConnectionImpl::Hide()
{
    if (proxy_) {
        proxy_->Hide();
    }
}

void WindowExtensionConnectionImpl::RequestFocus()
{
    if (proxy_ != nullptr) {
        proxy_->RequestFocus();
    }
}

void WindowExtensionConnectionImpl::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    if (!remoteObject) {
        WLOGFE("remote object is null");
        return;
    }

    if (!proxy_) {
        proxy_ = new(std::nothrow) WindowExtensionClientProxy(remoteObject);
        if (!proxy_) {
            WLOGFE("get proxy failed");
            return;
        }
    }
    if (!deathRecipient_) {
        deathRecipient_ = new(std::nothrow) WindowExtensionClientRecipient();
        if (!deathRecipient_) {
            WLOGFE("get death recipient failed");
            return;
        }
    }

    // if (!proxy_->AsObject() || !proxy_->AsObject()->AddDeathRecipient(deathRecipient_)) {
    //     WLOGFE("Failed to add death recipient");
    // }
}
void WindowExtensionConnectionImpl::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
}

} // namespace Rosen
} // namespace OHOS