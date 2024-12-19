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

#ifndef OHOS_ROSEN_EXTENSION_DATA_HANDLER_H
#define OHOS_ROSEN_EXTENSION_DATA_HANDLER_H

#include "data_handler_interface.h"

#include <cstdint>
#include <mutex>
#include <optional>

#include <event_handler.h>
#include <want.h>
#include "message_parcel.h"

namespace OHOS::Rosen::Extension {

struct DataTransferConfig : public Parcelable {
    bool Marshalling(Parcel& parcel) const override;
    static DataTransferConfig* Unmarshalling(Parcel& parcel);
    std::string ToString() const;

    bool needSyncSend_ { false };
    bool needReply_ { false };
    SubSystemId subSystemId_ { SubSystemId::INVALID };
    uint32_t customId_ { 0 };
};

using Task = std::function<void()>;

class DataHandler : public IDataHandler {
public:
    DataHandler(std::shared_ptr<AppExecFwk::EventHandler>& eventHandler) : eventHandler_(eventHandler) {};
    virtual ~DataHandler() = default;

    DataHandlerErr SendDataSync(SubSystemId subSystemId, uint32_t customId, AAFwk::Want& data,
        AAFwk::Want& reply) override;
    DataHandlerErr SendDataSync(SubSystemId subSystemId, uint32_t customId, AAFwk::Want& data) override;
    DataHandlerErr SendDataAsync(SubSystemId subSystemId, uint32_t customId, AAFwk::Want& data) override;
    DataHandlerErr RegisterDataConsumer(SubSystemId dataId, DataConsumeCallback&& callback) override;
    void UnregisterDataConsumer(SubSystemId dataId) override;
    DataHandlerErr NotifyDataConsumer(AAFwk::Want&& data, std::optional<AAFwk::Want> reply,
        const DataTransferConfig& config);

protected:
    virtual DataHandlerErr SendData(AAFwk::Want& data, AAFwk::Want& reply, const DataTransferConfig& config) = 0;
    DataHandlerErr PrepareData(MessageParcel& data, AAFwk::Want& toSend, const DataTransferConfig& config);
    DataHandlerErr ParseReply(MessageParcel& data, AAFwk::Want& reply, const DataTransferConfig& config);
    void PostAsyncTask(Task&& task, const std::string& name, int64_t delayTime);

protected:
    mutable std::mutex mutex_;
    std::unordered_map<SubSystemId, DataConsumeCallback> consumers_;
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_;
};

}  // namespace OHOS::Rosen::Extension

#endif  // OHOS_ROSEN_EXTENSION_DATA_HANDLER_H