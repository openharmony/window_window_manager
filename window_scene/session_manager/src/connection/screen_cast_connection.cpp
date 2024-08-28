/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "connection/screen_cast_connection.h"

#include "window_manager_hilog.h"

namespace OHOS::Rosen {

static const std::map<int32_t, std::pair<std::string, std::string>> PARAM_FLAG_MAP = {
    {0, {"requestReason", "onPlugOut"}},
    {1, {"requestReason", "onPlugIn"}}
};

ScreenCastConnection &ScreenCastConnection::GetInstance()
{
    static ScreenCastConnection screenCastConnection;
    return screenCastConnection;
}

bool ScreenCastConnection::CastConnectExtension(const int32_t &paramFlag)
{
    if (bundleName_.empty() || abilityName_.empty()) {
        TLOGE(WmsLogTag::DMS, "screen cast bundleName or abilityName is empty");
        return false;
    }
    TLOGI(WmsLogTag::DMS, "bundleName: %{public}s, abilityName: %{public}s",
        bundleName_.c_str(), abilityName_.c_str());
    if (abilityConnection_ != nullptr) {
        TLOGI(WmsLogTag::DMS, "screen cast already connected");
        return true;
    }
    abilityConnection_ = std::make_unique<ScreenSessionAbilityConnection>();
    if (abilityConnection_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "connection is nullptr");
        return false;
    }
    std::vector<std::pair<std::string, std::string>> paramMap;
    auto iter = PARAM_FLAG_MAP.find(paramFlag);
    if (iter != PARAM_FLAG_MAP.end()) {
        TLOGE(WmsLogTag::DMS, "The paramFlag does not exist!");
        return false;
    }
    paramMap.insert(iter->second);
    bool ret = abilityConnection_->ScreenSessionConnectExtension(bundleName_, abilityName_, paramMap);
    if (!ret) {
        TLOGE(WmsLogTag::DMS, "ScreenSessionConnectExtension failed");
        return false;
    }
    TLOGI(WmsLogTag::DMS, "CastConnectExtension succeed");
    return true;
}

void ScreenCastConnection::CastDisconnectExtension()
{
    if (abilityConnection_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "ability connect failed");
        return;
    }
    abilityConnection_->ScreenSessionDisconnectExtension();
    abilityConnection_ = nullptr;
    TLOGI(WmsLogTag::DMS, "CastDisconnectExtension exit");
}

void ScreenCastConnection::SetBundleName(const std::string &bundleName)
{
    bundleName_ = bundleName;
}

void ScreenCastConnection::SetAbilityName(const std::string &abilityName)
{
    abilityName_ = abilityName;
}

std::string ScreenCastConnection::GetBundleName() const
{
    return bundleName_;
}

std::string ScreenCastConnection::GetAbilityName() const
{
    return abilityName_;
}

bool ScreenCastConnection::IsConnectedSync()
{
    if (abilityConnection_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "ability connection is nullptr");
        return false;
    }
    return abilityConnection_->IsConnectedSync();
}

int32_t ScreenCastConnection::SendMessageToCastService(const int32_t &transCode, MessageParcel &data,
    MessageParcel &reply)
{
    if (abilityConnection_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "ability connection is nullptr");
        return -1;
    }
    return abilityConnection_->SendMessage(transCode, data, reply);
}
} // namespace OHOS::Rosen
