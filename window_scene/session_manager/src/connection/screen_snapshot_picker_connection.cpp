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

#include "connection/screen_snapshot_picker_connection.h"

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
constexpr int32_t TRANS_CMD_SEND_SNAPSHOT_RECT = 2;
constexpr int32_t RES_FAILURE = -1;
constexpr int32_t RES_SUCCESS = 0;

ScreenSnapshotPickerConnection &ScreenSnapshotPickerConnection::GetInstance()
{
    static ScreenSnapshotPickerConnection screenSnapshotPickerConnection;
    return screenSnapshotPickerConnection;
}

bool ScreenSnapshotPickerConnection::SnapshotPickerConnectExtension()
{
    if (bundleName_ == "" || abilityName_ == "") {
        TLOGE(WmsLogTag::DMS, "screen snapshot bundleName:%{public}s or abilityName:%{public}s is empty",
            bundleName_.c_str(), abilityName_.c_str());
        return false;
    }
    TLOGI(WmsLogTag::DMS, "bundleName:%{public}s, abilityName:%{public}s",
        bundleName_.c_str(), abilityName_.c_str());
    if (abilityConnection_ != nullptr) {
        TLOGI(WmsLogTag::DMS, "screen snapshot already connected");
        return true;
    }
    abilityConnection_ = std::make_unique<ScreenSessionAbilityConnection>();
    if (abilityConnection_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "connection is nullptr");
        return false;
    }
    bool ret = abilityConnection_->ScreenSessionConnectExtension(bundleName_, abilityName_);
    if (!ret) {
        TLOGE(WmsLogTag::DMS, "ScreenSessionConnectExtension failed");
        return false;
    }
    TLOGI(WmsLogTag::DMS, "SnapshotPickerConnectExtension succeed");
    return true;
}

int32_t ScreenSnapshotPickerConnection::GetScreenSnapshotInfo(Media::Rect &rect, ScreenId &screenId)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString16(Str8ToStr16("SA"));
    data.WriteString16(Str8ToStr16("ScreenSessionManager"));

    if (abilityConnection_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "ability connection is nullptr");
        return RES_FAILURE;
    }
    int32_t ret = abilityConnection_->SendMessage(TRANS_CMD_SEND_SNAPSHOT_RECT, data, reply);
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::DMS, "send message failed");
        return RES_FAILURE;
    }
    screenId = static_cast<ScreenId>(reply.ReadInt32());
    rect.left = reply.ReadInt32();
    rect.top = reply.ReadInt32();
    rect.width = reply.ReadInt32();
    rect.height = reply.ReadInt32();
    TLOGI(WmsLogTag::DMS, "snapshot area info screenId:%{public}" PRIu64", \
        left:%{public}d, top:%{public}d, width:%{public}d, height:%{public}d",
        screenId, rect.left, rect.top, rect.width, rect.height);

    return RES_SUCCESS;
}

void ScreenSnapshotPickerConnection::SnapshotPickerDisconnectExtension()
{
    if (abilityConnection_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "ability connect failed");
        return;
    }

    abilityConnection_->ScreenSessionDisconnectExtension();
    TLOGI(WmsLogTag::DMS, "SnapshotPickerDisconnectExtension exit");
}

void ScreenSnapshotPickerConnection::SetBundleName(const std::string &bundleName)
{
    bundleName_ = bundleName;
}

void ScreenSnapshotPickerConnection::SetAbilityName(const std::string &abilityName)
{
    abilityName_ = abilityName;
}
} // namespace OHOS::Rosen