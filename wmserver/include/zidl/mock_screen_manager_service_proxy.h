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

#ifndef OHOS_ROSEN_MOCK_SCREEN_MANAGER_PROXY_H
#define OHOS_ROSEN_MOCK_SCREEN_MANAGER_PROXY_H

#include <iremote_proxy.h>
#include "mock_screen_manager_service_interface.h"

namespace OHOS {
namespace Rosen {
class MockScreenManagerServiceProxy : public IRemoteProxy<IMockScreenManagerInterface> {
public:
    explicit MockScreenManagerServiceProxy(const sptr<IRemoteObject>& impl) :
        IRemoteProxy<IMockScreenManagerInterface>(impl) {};
    ~MockScreenManagerServiceProxy() {};

    void GetScreenDumpInfo(const std::vector<std::string>& params, std::string& info) override;
private:
    static inline BrokerDelegator<MockScreenManagerServiceProxy> delegator_;
};
}
}
#endif // OHOS_ROSEN_MOCK_SCREEN_MANAGER_PROXY_H