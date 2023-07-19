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

#include "mock_screen_manager_service.h"

#include <system_ability_definition.h>
#include "scene_board_judgement.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_DISPLAY, "MockScreenManagerService" };
}

WM_IMPLEMENT_SINGLE_INSTANCE(MockScreenManagerService)
bool registerResult = !SceneBoardJudgement::IsSceneBoardEnabled() ? false :
    SystemAbility::MakeAndRegisterAbility(&SingletonContainer::Get<MockScreenManagerService>());

MockScreenManagerService::MockScreenManagerService() : SystemAbility(DISPLAY_MANAGER_SERVICE_SA_ID, true)
{
}

void MockScreenManagerService::OnStart()
{
    WLOGFI("OnStart begin");
    RegisterMockScreenManagerService();
}

void MockScreenManagerService::GetScreenDumpInfo(const std::vector<std::string>& params, std::string& info)
{
    WLOGFD("GetScreenDumpInfo begin");
}

int MockScreenManagerService::Dump(int fd, const std::vector<std::u16string>& args)
{
    WLOGFI("Dump begin");
    return 0;
}

bool MockScreenManagerService::RegisterMockScreenManagerService()
{
    WLOGFI("registerResult %{public}d", registerResult);
    if (!registerResult) {
        !SceneBoardJudgement::IsSceneBoardEnabled() ? false :
            SystemAbility::MakeAndRegisterAbility(&SingletonContainer::Get<MockScreenManagerService>());
    }
    if (!Publish(this)) {
        WLOGFE("Publish failed");
        return false;
    }
    WLOGFI("Publish mock screen manager service success");
    return true;
}

} // namespace Rosen
} // namespace OHOS