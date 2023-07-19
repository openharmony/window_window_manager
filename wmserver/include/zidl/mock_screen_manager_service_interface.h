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

#ifndef OHOS_ROSEN_MOCK_SCREEN_MANAGER_INTERFACE_H
#define OHOS_ROSEN_MOCK_SCREEN_MANAGER_INTERFACE_H

#include <iremote_broker.h>
#include <refbase.h>
#include <parcel.h>

namespace OHOS {
namespace Rosen {

class IMockScreenManagerInterface : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IMockScreenManager");
    enum class MockScreenManagerServiceMessage : uint32_t {
        TRANS_ID_GET_SCREEN_DUMP_INFO,
    };

    virtual void GetScreenDumpInfo(const std::vector<std::string>& params, std::string& info) = 0;
};
}
}
#endif // OHOS_ROSEN_MOCK_SESSION_MANAGER_INTERFACE_H