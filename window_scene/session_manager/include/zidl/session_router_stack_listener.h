/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_SESSION_ROUTER_STACK_LISTENER_H
#define OHOS_SESSION_ROUTER_STACK_LISTENER_H

#include <iremote_broker.h>
#include <iremote_object.h>

#include "wm_common.h"

namespace OHOS::Rosen {

class RouterStackInfo : public Parcelable {
public:
    RouterStackInfo() = default;

    RouterStackInfo(int32_t winId, std::string routerStackInfo) : winId_(winId), routerStackInfo_(routerStackInfo) {};

    ~RouterStackInfo() = default;

    bool Marshalling(Parcel &parcel) const
    {
        return parcel.WriteInt32(winId_) && parcel.WriteString(routerStackInfo_) &&
            parcel.WriteInt32(static_cast<int32_t>(errCode_));
    }

    static RouterStackInfo *Unmarshalling(Parcel &parcel)
    {
        auto routerStackInfo = new (std::nothrow) RouterStackInfo();
        if (routerStackInfo == nullptr) {
            return nullptr;
        }
        int32_t errCode = 0;
        bool res =
            parcel.ReadInt32(routerStackInfo->winId_) && parcel.ReadString(routerStackInfo->routerStackInfo_)
                && parcel.ReadInt32(errCode);
        routerStackInfo->errCode_ = static_cast<WMError>(errCode);
        if (!res) {
            delete routerStackInfo;
            return nullptr;
        }
        return routerStackInfo;
    }

    int32_t winId_{0};
    std::string routerStackInfo_;
    WMError errCode_ = WMError::WM_OK;
};

class ISessionRouterStackListener : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.ISessionRouterStackListener");

    enum class SessionRouterStackListenerMessage : uint32_t {
        TRANS_ID_GET_ROUTER_STACK_INFO = 0,
    };

    virtual void SendRouterStackInfo(const sptr<RouterStackInfo>& routerStackInfo) {};
};
} // namespace OHOS
#endif //OHOS_SESSION_ROUTER_STACK_LISTENER_H