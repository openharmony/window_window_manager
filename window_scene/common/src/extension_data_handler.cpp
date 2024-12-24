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

#include "common/include/extension_data_handler.h"

#include <cstdint>
#include <optional>
#include <sstream>
#include <string>
#include <utility>

#include <hitrace_meter.h>
#include <iremote_proxy.h>
#include <message_parcel.h>
#include <want.h>

#include "window_manager_hilog.h"

namespace OHOS::Rosen::Extension {

bool DataTransferConfig::Marshalling(Parcel& parcel) const
{
    return parcel.WriteUint8(static_cast<uint8_t>(subSystemId)) && parcel.WriteUint32(customId) &&
           parcel.WriteBool(needReply) && parcel.WriteBool(needSyncSend);
}

DataTransferConfig* DataTransferConfig::Unmarshalling(Parcel& parcel)
{
    auto config = new (std::nothrow) DataTransferConfig();
    uint8_t subSystemIdValue = static_cast<uint8_t>(SubSystemId::INVALID);
    parcel.ReadUint8(subSystemIdValue);
    if (subSystemIdValue >= static_cast<uint8_t>(SubSystemId::INVALID)) {
        delete config;
        return nullptr;
    }
    if (!parcel.ReadUint32(config->customId) || !parcel.ReadBool(config->needReply) ||
        !parcel.ReadBool(config->needSyncSend)) {
        delete config;
        return nullptr;
    }
    config->subSystemId = static_cast<SubSystemId>(subSystemIdValue);
    return config;
}

std::string DataTransferConfig::ToString() const
{
    std::stringstream ss;
    ss << "subSystemId: " << static_cast<uint16_t>(subSystemId) << ", customId: " << customId
       << ", needReply: " << needReply << ", needSyncSend: " << needSyncSend;
    return ss.str();
}

DataHandlerErr DataHandler::RegisterDataConsumer(SubSystemId subSystemId, DataConsumeCallback&& callback)
{
    std::lock_guard lock(mutex_);
    auto it = consumers_.find(subSystemId);
    if (it != consumers_.end()) {
        // A consumer already exists for this SubSystemId
        TLOGE(WmsLogTag::WMS_UIEXT, "Consumer already exists for subSystemId: %{public}hhu", subSystemId);
        return DataHandlerErr::DUPLICATE_REGISTRATION;
    }

    consumers_[subSystemId] = std::move(callback);
    return DataHandlerErr::OK;
}

void DataHandler::UnregisterDataConsumer(SubSystemId subSystemId)
{
    std::lock_guard lock(mutex_);
    consumers_.erase(subSystemId);
    TLOGI(WmsLogTag::WMS_UIEXT, "Unregister consumer for subSystemId: %{public}hhu", subSystemId);
}

DataHandlerErr DataHandler::PrepareData(MessageParcel& data, AAFwk::Want& toSend, const DataTransferConfig& config)
{
    if (!WriteInterfaceToken(data)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed, %{public}s", config.ToString().c_str());
        return DataHandlerErr::WRITE_PARCEL_ERROR;
    }

    if (!data.WriteParcelable(&config)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed, %{public}s", config.ToString().c_str());
        return DataHandlerErr::WRITE_PARCEL_ERROR;
    }

    if (!data.WriteParcelable(&toSend)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed, %{public}s", config.ToString().c_str());
        return DataHandlerErr::WRITE_PARCEL_ERROR;
    }
    return DataHandlerErr::OK;
}

DataHandlerErr DataHandler::ParseReply(MessageParcel& replyParcel, AAFwk::Want& reply, const DataTransferConfig& config)
{
    if (!config.needReply) {
        return DataHandlerErr::OK;
    }

    uint32_t replayCode = 0;
    if (!replyParcel.ReadUint32(replayCode)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed, %{public}s", config.ToString().c_str());
        return DataHandlerErr::READ_PARCEL_ERROR;
    }

    if (config.needReply) {
        auto response = replyParcel.ReadParcelable<AAFwk::Want>();
        if (!response) {
            TLOGE(WmsLogTag::WMS_UIEXT, "failed, %{public}s", config.ToString().c_str());
            return DataHandlerErr::READ_PARCEL_ERROR;
        }
        reply = *response;
    }

    return static_cast<DataHandlerErr>(replayCode);
}

// process data from peer
void DataHandler::NotifyDataConsumer(MessageParcel& data, MessageParcel& replyParcel)
{
    auto config = data.ReadParcelable<DataTransferConfig>();
    if (config == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed");
        replyParcel.WriteUint32(static_cast<uint32_t>(DataHandlerErr::READ_PARCEL_ERROR));
        return;
    }

    auto want = data.ReadParcelable<AAFwk::Want>();
    if (want == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed");
        replyParcel.WriteUint32(static_cast<uint32_t>(DataHandlerErr::READ_PARCEL_ERROR));
        return;
    }

    std::optional<AAFwk::Want> reply;
    if (config->needReply) {
        reply = std::make_optional<AAFwk::Want>();
    }

    auto ret = NotifyDataConsumer(std::move(*want), reply, *config);
    replyParcel.WriteUint32(static_cast<uint32_t>(ret));
    if (config->needReply && reply) {
        replyParcel.WriteParcelable(&(reply.value()));
    }

    return;
}

void DataHandler::SetEventHandler(const std::shared_ptr<AppExecFwk::EventHandler>& eventHandler)
{
    eventHandler_ = eventHandler;
}

void DataHandler::SetRemoteProxyObject(const sptr<IRemoteObject>& remoteObject)
{
    if (!remoteObject || !remoteObject->IsProxyObject()) {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed, not proxy object");
        return;
    }

    std::lock_guard lock(mutex_);
    remoteProxy_ = remoteObject;
}

DataHandlerErr DataHandler::SendDataSync(SubSystemId subSystemId, uint32_t customId, AAFwk::Want& data,
                                         AAFwk::Want& reply)
{
    DataTransferConfig config;
    config.needSyncSend = true;
    config.needReply = true;
    config.subSystemId = subSystemId;
    config.customId = customId;
    return SendData(data, reply, config);
}

DataHandlerErr DataHandler::SendDataSync(SubSystemId subSystemId, uint32_t customId, AAFwk::Want& data)
{
    DataTransferConfig config;
    config.needSyncSend = true;
    config.subSystemId = subSystemId;
    config.customId = customId;
    AAFwk::Want reply;
    return SendData(data, reply, config);
}

DataHandlerErr DataHandler::SendDataAsync(SubSystemId subSystemId, uint32_t customId, AAFwk::Want& data)
{
    DataTransferConfig config;
    config.subSystemId = subSystemId;
    config.customId = customId;
    AAFwk::Want reply;
    return SendData(data, reply, config);
}

DataHandlerErr DataHandler::NotifyDataConsumer(AAFwk::Want&& data, std::optional<AAFwk::Want>& reply,
                                               const DataTransferConfig& config)
{
    DataConsumeCallback callback;
    {
        std::lock_guard lock(mutex_);
        auto it = consumers_.find(config.subSystemId);
        if (it == consumers_.end()) {
            TLOGE(WmsLogTag::WMS_UIEXT, "failed, %{public}s", config.ToString().c_str());
            return DataHandlerErr::NO_CONSUME_CALLBACK;
        }
        callback = it->second;
    }

    if (!callback) {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed, %{public}s", config.ToString().c_str());
        return DataHandlerErr::INVALID_CALLBACK;
    }

    // sync mode
    if (config.needSyncSend) {
        auto ret = callback(config.subSystemId, config.customId, std::move(data), reply);
        TLOGI(WmsLogTag::WMS_UIEXT, "subSystemId: %{public}hhu, customId: %{public}u, ret: %{public}d",
              config.subSystemId, config.customId, ret);
        return DataHandlerErr::OK;
    }

    // async mode
    auto task = [input = std::move(data), subSystemId = config.subSystemId, customId = config.customId,
                 func = callback]() mutable {
        std::optional<AAFwk::Want> reply;
        auto ret = func(subSystemId, customId, std::move(input), reply);
        TLOGNI(WmsLogTag::WMS_UIEXT, "subSystemId: %{public}hhu, customId: %{public}u, ret: %{public}d", subSystemId,
               customId, ret);
    };

    std::stringstream ss;
    ss << "NotifyDataConsumer_" << static_cast<uint32_t>(config.subSystemId) << "_" << config.customId;
    PostAsyncTask(std::move(task), ss.str(), 0);
    return DataHandlerErr::OK;
}

void DataHandler::PostAsyncTask(Task&& task, const std::string& name, int64_t delayTime)
{
    if (!eventHandler_) {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "s:%s", name.c_str());
        task();
        return;
    }

    auto runner = eventHandler_->GetEventRunner();
    if (!runner || runner->IsCurrentRunnerThread()) {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "s:%s", name.c_str());
        task();
        return;
    }

    auto localTask = [task = std::move(task), name] {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "s:%s", name.c_str());
        task();
    };

    auto ret = eventHandler_->PostTask(std::move(localTask), "uiext:" + name, delayTime,
                                       AppExecFwk::EventQueue::Priority::IMMEDIATE);
    if (!ret) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Post task failed, name: %{public}s", name.c_str());
    }
}
}  // namespace OHOS::Rosen::Extension