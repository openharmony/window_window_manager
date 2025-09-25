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

#ifndef OHOS_ROSEN_SESSION_LIFECYCLE_LISTENER_INTERFACE_H
#define OHOS_ROSEN_SESSION_LIFECYCLE_LISTENER_INTERFACE_H

#include <iremote_broker.h>
#include <iremote_object.h>

namespace OHOS::Rosen {

class ISessionLifecycleListener : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.ISessionLifecycleListener");

    enum class ISessionLifecycleListenerMessage : uint32_t {
        TRANS_ON_LIFECYCLE_EVENT = 0,
    };

    enum SessionLifecycleEvent {
        CREATED = 0,
        DESTROYED,
        FOREGROUND,
        BACKGROUND,
        ACTIVE,
        INACTIVE,
        RESUME,
        PAUSE,

        // more
        TRANSFER_TO_TARGET_SCREEN,
        EVENT_END,
    };

    struct LifecycleEventPayload : public Parcelable {
        bool Marshalling(Parcel& parcel) const override
        {
            return parcel.WriteString(bundleName_) &&
                   parcel.WriteString(moduleName_) &&
                   parcel.WriteString(abilityName_) &&
                   parcel.WriteInt32(appIndex_) &&
                   parcel.WriteInt32(persistentId_) &&
                   parcel.WriteUint32(resultCode_) &&
                   parcel.WriteUint64(fromScreenId_) &&
                   parcel.WriteUint64(toScreenId_);
        }

        static LifecycleEventPayload* Unmarshalling(Parcel& parcel)
        {
            auto payload = new LifecycleEventPayload();
            if (!parcel.ReadString(payload->bundleName_) ||
                !parcel.ReadString(payload->moduleName_) ||
                !parcel.ReadString(payload->abilityName_) ||
                !parcel.ReadInt32(payload->appIndex_) ||
                !parcel.ReadInt32(payload->persistentId_) ||
                !parcel.ReadUint32(payload->resultCode_) ||
                !parcel.ReadUint64(payload->fromScreenId_) ||
                !parcel.ReadUint64(payload->toScreenId_)) {
                delete payload;
                return nullptr;
            }
            return payload;
        }

        std::string bundleName_;
        std::string moduleName_;
        std::string abilityName_;
        int32_t appIndex_ = 0;
        int32_t persistentId_ = 0;
        uint32_t resultCode_ = 0;
        uint64_t fromScreenId_ = 0;
        uint64_t toScreenId_ = 0;
    };

    virtual void OnLifecycleEvent(SessionLifecycleEvent event, const LifecycleEventPayload& payload) {};
};
} // namespace OHOS
#endif // OHOS_ROSEN_SESSION_LIFECYCLE_LISTENER_INTERFACE_H