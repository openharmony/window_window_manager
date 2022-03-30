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

#include "window_extension_connection.h"

#include "ability_manager_client.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
boolean ConnectExtension(const AppExecFwk::ElementName &element, Rect rect, int32_t uid)
{
    AAFwk::Want want = CreateWant(element);
    want.WriteInt32(RECT_FORM_KEY_POS_X, rect.posX_);
    want.WriteInt32(RECT_FORM_KEY_POS_Y, rect.posY_);
    want.WriteInt32(RECT_FORM_KEY_WIDTH, rect.width_);
    want.WriteInt32(RECT_FORM_KEY_HEIGHT, rect.height_);

    if (AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(
        want, this, nullptr, uid / UID_MASK) != ERR_OK) {
        WLOGFE("ConnectAbility failed!");
        return false;
    }
    return true;
}

boolean Show()
{
}

boolean Hide()
{
}

boolean RequestFocus()
{
}

void OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int resultCode) override;
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

    if (!proxy_->AsObject() || !proxy_->AsObject()->AddDeathRecipient(deathRecipient_)) {
        HILOG_ERROR("Failed to add death recipient");
    }
}
void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override;
{
}

} // namespace Rosen
} // namespace OHOS