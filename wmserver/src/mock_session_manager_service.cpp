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

#include "mock_session_manager_service.h"
#include <iremote_broker.h>
#include <iremote_object.h>
#include <system_ability_definition.h>
#include <sstream>

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "MOCK_SMS"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(MockSessionManagerService)

MockSessionManagerService::MockSessionManagerService() : SystemAbility(WINDOW_MANAGER_SERVICE_ID, true)
{
}

bool MockSessionManagerService::RegisterMockSessionManagerService()
{
    WLOGFD("Register mock session manager service");
    bool res = SystemAbility::MakeAndRegisterAbility(&SingletonContainer::Get<MockSessionManagerService>());
    if (!res) {
        WLOGFE("register failed");
    }
    if (!Publish(this)) {
        WLOGFE("Publish failed");
    }
    return true;
}

void MockSessionManagerService::OnStart()
{
    WLOGFD("OnStart begin");
}

bool MockSessionManagerService::SetSessionManagerService(const sptr<IRemoteObject>& sessionManagerService)
{
    if (!sessionManagerService) {
        WLOGFE("sessionManagerService is nullptr");
        return false;
    }
    sessionManagerService_ = sessionManagerService;
    RegisterMockSessionManagerService();
    WLOGFD("sessionManagerService set success!");
    return true;
}

sptr<IRemoteObject> MockSessionManagerService::GetSessionManagerService()
{
    if (sessionManagerService_) {
        WLOGFE("sessionManagerService is nullptr");
        return nullptr;
    }
    return sessionManagerService_;
}
} // namespace Rosen
} // namespace OHOS
