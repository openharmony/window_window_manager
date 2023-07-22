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

#ifndef OHOS_ROSEN_MOCK_SCREEN_MANAGER_SERVICE_H
#define OHOS_ROSEN_MOCK_SCREEN_MANAGER_SERVICE_H

#include <system_ability.h>
#include <iremote_object.h>

#include "dm_common.h"
#include "singleton_delegator.h"
#include "zidl/mock_screen_manager_service_stub.h"

namespace OHOS {
namespace Rosen {
class MockScreenManagerService : public SystemAbility, public MockScreenManagerServiceStub {
DECLARE_SYSTEM_ABILITY(MockScreenManagerService);
WM_DECLARE_SINGLE_INSTANCE_BASE(MockScreenManagerService);
public:
    void OnStart() override;
    void GetScreenDumpInfo(const std::vector<std::string>& params, std::string& info) override;
    int Dump(int fd, const std::vector<std::u16string>& args) override;
    bool RegisterMockScreenManagerService();
    void SetSessionManagerService(const sptr<IRemoteObject>& sessionManagerService);
protected:
    MockScreenManagerService();
    virtual ~MockScreenManagerService() = default;
private:
    void ShowHelpInfo(std::string& dumpInfo);
    void ShowIllegalArgsInfo(std::string& dumpInfo);
    int DumpScreenInfo(const std::vector<std::string>& args, std::string& dumpInfo);
    int DumpAllScreenInfo(std::string& dumpInfo);
    int DumpSpecifiedScreenInfo(ScreenId screenId, std::string& dumpInfo);
    bool IsValidDigitString(const std::string& idStr) const;

    void InitScreenSessionManager();

    static inline SingletonDelegator<MockScreenManagerService> delegator;
    sptr<IRemoteObject> sessionManagerService_ = nullptr;
    sptr<IRemoteObject> screenSessionManager_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_MOCK_SCREEN_MANAGER_SERVICE_H