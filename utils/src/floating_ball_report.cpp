/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "floating_ball_report.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
const std::string PNAMEID = "SCB_FLOATING_BALL";
const std::string PVERSION = "1.0";
}
WM_IMPLEMENT_SINGLE_INSTANCE(FloatingballReporter)

constexpr char EVENT_KEY_TEMPLATE_TYPE[] = "TEMPLATE_TYPE";
constexpr char EVENT_KEY_PNAMEID[] = "PNAMEID";
constexpr char EVENT_KEY_PVERSION[] = "PVERSION";
constexpr char EVENT_KEY_PACKAGE_NAME[] = "PACKAGE_NAME";
constexpr char EVENT_KEY_ERROR_REASON[] = "ERROR_REASON";

constexpr char EVENT_KEY_START_FLOATING_BALL[] = "START_FLOATING_BALL";
constexpr char EVENT_KEY_UPDATE_FLOATING_BALL[] = "UPDATE_FLOATING_BALL";
constexpr char EVENT_KEY_REMOVE_FLOATING_BALL[] = "REMOVE_FLOATING_BALL";
constexpr char EVENT_KEY_RESTORE_FLOATING_BALL_WINDOW[] = "RESTORE_FLOATING_BALL_WINDOW";

void FloatingballReporter::SetCurrentPackageName(const std::string& packageName)
{
    std::lock_guard<std::mutex> lock(packageNameMutex_);
    packageName_ = packageName;
}

std::string FloatingballReporter::GetPackageName() const
{
    std::lock_guard<std::mutex> lock(packageNameMutex_);
    return packageName_;
}

void FloatingballReporter::ReportFbStart(const uint32_t& templateType, const std::string& errorReason)
{
    TLOGI(WmsLogTag::WMS_SYSTEM, "Report start floating ball window");
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::MULTIWINDOW_UE, EVENT_KEY_START_FLOATING_BALL,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        EVENT_KEY_PNAMEID, PNAMEID,
        EVENT_KEY_PVERSION, PVERSION,
        EVENT_KEY_PACKAGE_NAME, GetPackageName(),
        EVENT_KEY_TEMPLATE_TYPE, templateType,
        EVENT_KEY_ERROR_REASON, errorReason);
    if (ret != 0) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "Write HiSysEvent error, errCode:%{public}d", ret);
    }
}

void FloatingballReporter::ReportFbUpdateContent(const uint32_t& templateType, const std::string &errorReason)
{
    TLOGI(WmsLogTag::WMS_SYSTEM, "Report update floating ball window content");
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::MULTIWINDOW_UE, EVENT_KEY_UPDATE_FLOATING_BALL,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        EVENT_KEY_PNAMEID, PNAMEID,
        EVENT_KEY_PVERSION, PVERSION,
        EVENT_KEY_PACKAGE_NAME, GetPackageName(),
        EVENT_KEY_TEMPLATE_TYPE, templateType,
        EVENT_KEY_ERROR_REASON, errorReason);
    if (ret != 0) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "Write HiSysEvent error, errCode:%{public}d", ret);
    }
}

void FloatingballReporter::ReportFbRemove(const uint32_t& templateType, const std::string& errorReason)
{
    TLOGI(WmsLogTag::WMS_SYSTEM, "Report remove floating ball window");
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::MULTIWINDOW_UE, EVENT_KEY_REMOVE_FLOATING_BALL,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        EVENT_KEY_PNAMEID, PNAMEID,
        EVENT_KEY_PVERSION, PVERSION,
        EVENT_KEY_PACKAGE_NAME, GetPackageName(),
        EVENT_KEY_TEMPLATE_TYPE, templateType,
        EVENT_KEY_ERROR_REASON, errorReason);
    if (ret != 0) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "Write HiSysEvent error, errCode:%{public}d", ret);
    }
}

void FloatingballReporter::ReportFbRestoreMainWindow(const uint32_t& templateType, const std::string& errorReason)
{
    TLOGI(WmsLogTag::WMS_SYSTEM, "Report restore main window to app, templateType:%{public}d", templateType);
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::MULTIWINDOW_UE, EVENT_KEY_RESTORE_FLOATING_BALL_WINDOW,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        EVENT_KEY_PNAMEID, PNAMEID,
        EVENT_KEY_PVERSION, PVERSION,
        EVENT_KEY_PACKAGE_NAME, GetPackageName(),
        EVENT_KEY_TEMPLATE_TYPE, templateType,
        EVENT_KEY_ERROR_REASON, errorReason);
    if (ret != 0) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "Write HiSysEvent error, errCode:%{public}d", ret);
    }
}

} // namespace Rosen
} // namespace OHOS
