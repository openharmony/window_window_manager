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

#ifndef OHOS_ROSEN_EXTENSION_DATA_HANDLE_MOCK_H
#define OHOS_ROSEN_EXTENSION_DATA_HANDLE_MOCK_H

#include <gmock/gmock.h>

#include "common/include/extension_data_handler.h"

namespace OHOS::Rosen::Extension {
class MockDataHandler : public DataHandler {
public:
    MockDataHandler() = default;
    ~MockDataHandler() override = default;

    MOCK_METHOD(DataHandlerErr,
                SendData,
                (const AAFwk::Want& data, AAFwk::Want& reply, const DataTransferConfig& config),
                (override));
    MOCK_METHOD(bool, WriteInterfaceToken, (MessageParcel & data), (override));

    DataHandlerErr SendDataAsync(SubSystemId subSystemId, uint32_t customId, const AAFwk::Want& toSend) override
    {
        return DataHandlerErr::OK;
    }

    // Helper methods to expose protected methods for testing
    using DataHandler::NotifyDataConsumer;
};
} // namespace OHOS::Rosen::Extension
#endif // OHOS_ROSEN_EXTENSION_DATA_HANDLE_MOCK_H
