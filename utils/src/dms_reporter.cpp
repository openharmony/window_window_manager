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

#include <hisysevent.h>
#include "dms_reporter.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
WM_IMPLEMENT_SINGLE_INSTANCE(DmsReporter)

enum class StageRes : int32_t {
    SUCCESS = 1,
    FAILED = 2,
};

enum class BizScene : int32_t {
    CONTINUE_APP = 10,
    QUERY_MISSION_INFO = 10,
    REGISTER = 12,
};

enum class BizStage : int32_t {
    REGISTER = 1,
    QUERY_MISSION_INFO = 7,
    CONTINUE_APP = 8,
};

constexpr const char APP_CONTINUE_DOMAIN[] = "APP_CONTINUE";
constexpr const char APP_CONTINUE_EVENT_NAME[] = "APPLICATION_CONTINUE_BEHAVIOR";
constexpr const char PKG_NAME_SCB[] = "com.ohos.sceneboard";
constexpr const char PKG_NAME_ABILITY_MANAGER[] = "ohos.abilitymanagerservice";
constexpr const char PKG_NAME_DMS[] = "ohos.distributedschedule";
constexpr const char EVENT_KEY_ORG_PKG[] = "ORG_PKG";
constexpr const char EVENT_KEY_HOST_PKG[] = "HOST_PKG";
constexpr const char EVENT_KEY_FUNC[] = "FUNC";
constexpr const char EVENT_KEY_BIZ_SCENE[] = "BIZ_SCENE";
constexpr const char EVENT_KEY_BIZ_STAGE[] = "BIZ_STAGE";
constexpr const char EVENT_KEY_STAGE_RES[] = "STAGE_RES";
constexpr const char EVENT_KEY_ERROR_CODE[] = "ERROR_CODE";

void DmsReporter::ReportRegisterSessionListener(bool isSuccess, int32_t errCode)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[AppContinue]Report register session listener");
    int32_t ret = HiSysEventWrite(
        APP_CONTINUE_DOMAIN, APP_CONTINUE_EVENT_NAME,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        EVENT_KEY_ORG_PKG, PKG_NAME_SCB,
        EVENT_KEY_FUNC, "RegisterSessionListener",
        EVENT_KEY_BIZ_SCENE, static_cast<int32_t>(BizScene::REGISTER),
        EVENT_KEY_BIZ_STAGE, static_cast<int32_t>(BizStage::REGISTER),
        EVENT_KEY_STAGE_RES, isSuccess ? static_cast<int32_t>(StageRes::SUCCESS) :
            static_cast<int32_t>(StageRes::FAILED),
        EVENT_KEY_ERROR_CODE, errCode,
        EVENT_KEY_HOST_PKG, PKG_NAME_ABILITY_MANAGER);
    if (ret != 0) {
        TLOGE(WmsLogTag::WMS_LIFE, "Write HiSysEvent error, ret:%{public}d", ret);
    }
}

void DmsReporter::ReportQuerySessionInfo(bool isSuccess, int32_t errCode)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[AppContinue]Report query session info");
    int32_t ret = HiSysEventWrite(
        APP_CONTINUE_DOMAIN, APP_CONTINUE_EVENT_NAME,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        EVENT_KEY_ORG_PKG, PKG_NAME_SCB,
        EVENT_KEY_FUNC, "GetSessionInfoByContinueSessionId",
        EVENT_KEY_BIZ_SCENE, static_cast<int32_t>(BizScene::QUERY_MISSION_INFO),
        EVENT_KEY_BIZ_STAGE, static_cast<int32_t>(BizStage::QUERY_MISSION_INFO),
        EVENT_KEY_STAGE_RES, isSuccess ? static_cast<int32_t>(StageRes::SUCCESS) :
            static_cast<int32_t>(StageRes::FAILED),
        EVENT_KEY_ERROR_CODE, errCode,
        EVENT_KEY_HOST_PKG, PKG_NAME_DMS);
    if (ret != 0) {
        TLOGE(WmsLogTag::WMS_LIFE, "Write HiSysEvent error, ret:%{public}d", ret);
    }
}

void DmsReporter::ReportContinueApp(bool isSuccess, int32_t errCode)
{
    TLOGI(WmsLogTag::WMS_LIFE, "[AppContinue]Report continue app pending session activation");
    int32_t ret = HiSysEventWrite(
        APP_CONTINUE_DOMAIN, APP_CONTINUE_EVENT_NAME,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        EVENT_KEY_ORG_PKG, PKG_NAME_SCB,
        EVENT_KEY_FUNC, "PendingSessionActivation",
        EVENT_KEY_BIZ_SCENE, static_cast<int32_t>(BizScene::CONTINUE_APP),
        EVENT_KEY_BIZ_STAGE, static_cast<int32_t>(BizStage::CONTINUE_APP),
        EVENT_KEY_STAGE_RES, isSuccess ? static_cast<int32_t>(StageRes::SUCCESS) :
            static_cast<int32_t>(StageRes::FAILED),
        EVENT_KEY_ERROR_CODE, errCode,
        EVENT_KEY_HOST_PKG, PKG_NAME_ABILITY_MANAGER);
    if (ret != 0) {
        TLOGE(WmsLogTag::WMS_LIFE, "Write HiSysEvent error, ret:%{public}d", ret);
    }
}

} // namespace Rosen
} // namespace OHOS
