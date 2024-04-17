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

#include "zidl/pattern_detach_callback_proxy.h"

#include <ipc_types.h>
#include <message_option.h>
#include <message_parcel.h>

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {

void PatternDetachCallbackProxy::OnPatternDetach(int32_t persisitentId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_LIFE, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteInt32(persisitentId)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Write persisitentId failed");
        return;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(PatternDetachCallbackMessage::TRANS_ID_PATTERN_ON_DETACH),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_LIFE, "SendRequest failed");
    }
}

} // namespace Rosen
} // namespace OHOS

