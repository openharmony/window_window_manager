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

#ifndef OHOS_ROSEN_DATA_HANDLER_H
#define OHOS_ROSEN_DATA_HANDLER_H

#include <cstdint>
#include <functional>
#include <optional>

namespace OHOS::AAFwk {
class Want;
}  // namespace OHOS::AAFwk

namespace OHOS::Rosen {

enum class SubSystemId : uint8_t { WM_UIEXT = 0, ARKUI_UIEXT, ABILITY_UIEXT, INVALID };

enum class DataHandlerErr : uint32_t {
    // common
    OK = 0,
    INVALID_PARAMETER,
    INTERNAL_ERROR,
    DUPLICATE_REGISTRATION,
    NULL_PTR,

    // ipc processing errors
    IPC_SEND_FAILED,
    WRITE_PARCEL_ERROR,
    READ_PARCEL_ERROR,

    // callback errors
    NO_CONSUME_CALLBACK,
    INVALID_CALLBACK,
};

using DataConsumeCallback =
    std::function<int32_t(SubSystemId id, uint32_t customId, AAFwk::Want&& data, std::optional<AAFwk::Want>& reply)>;

/**
 * @class IDataHandler
 * @brief Interface for handling data operations between subsystems by using the IPC.
 *
 * This class defines an interface for sending and receiving data between different
 * subsystems, as well as registering and unregistering data consumers. It provides
 * methods for both synchronous and asynchronous data transmission.
 */
class IDataHandler {
public:
    IDataHandler() = default;
    virtual ~IDataHandler() = default;

    /**
     * @brief Sends data synchronously to a specified subsystem and receives a reply.
     *
     * @param subSystemId The identifier of the target subsystem.
     * @param customId A custom identifier for the data being sent.
     * @param toSend The Want object containing the data to be sent.
     * @param reply A reference to a Want object that will be filled with the reply data.
     * @return DataHandlerErr::OK if the data was successfully sent and a reply was received, other errcode otherwise.
     */
    virtual DataHandlerErr SendDataSync(SubSystemId subSystemId, uint32_t customId, const AAFwk::Want& toSend,
                                        AAFwk::Want& reply) = 0;

    /**
     * @brief Sends data synchronously to a specified subsystem without reply.
     *
     * @param subSystemId The identifier of the target subsystem.
     * @param customId A custom identifier for the data being sent.
     * @param toSend The Want object containing the data to be sent.
     * @return DataHandlerErr::OK if the data was successfully sent, other errcode otherwise.
     */
    virtual DataHandlerErr SendDataSync(SubSystemId subSystemId, uint32_t customId, const AAFwk::Want& toSend) = 0;

    /**
     * @brief Sends data asynchronously to a specified subsystem without reply.
     *
     * @param subSystemId The identifier of the target subsystem.
     * @param customId A custom identifier for the data being sent.
     * @param toSend The Want object containing the data to be sent.
     * @return DataHandlerErr::OK if the data was successfully sent, other errcode otherwise.
     */
    virtual DataHandlerErr SendDataAsync(SubSystemId subSystemId, uint32_t customId, const AAFwk::Want& toSend) = 0;

    /**
     * @brief Registers a data consumer for a specific subsystemId.
     *
     * @param SubSystemId The identifier of the data to be consumed.
     * @param callback A DataConsumerInfo object containing the callback function and options.
     * @return DataHandlerErr::OK if the data consumer was successfully registered, other errcode otherwise.
     */
    virtual DataHandlerErr RegisterDataConsumer(SubSystemId subSystemId, DataConsumeCallback&& callback) = 0;

    /**
     * @brief Unregisters a data consumer for a specific subSystemId.
     *
     * @param SubSystemId The identifier of the data to be consumed.
     */
    virtual void UnregisterDataConsumer(SubSystemId subSystemId) = 0;
};
}  // namespace OHOS::Rosen

#endif  // OHOS_ROSEN_DATA_HANDLER_H