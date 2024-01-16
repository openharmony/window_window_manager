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

#include <map>
#include <hisysevent.h>
#include "pip_report.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "PictureInPictureReport"};
    const std::string PNAMEID = "windowManager";
    const std::string PVERSION = "1.0";
}
WM_IMPLEMENT_SINGLE_INSTANCE(PiPReporter)

static std::map<std::string, int32_t> CONTROL_ACTION_MAP = {
    {"playbackStateChanged", 0},
    {"nextVideo", 1},
    {"previousVideo", 2},
    {"hangUp", 3},
    {"microphoneStateChanged", 4},
    {"cameraStateChanged", 5},
    {"videoStateChanged", 6}
};
constexpr char EVENT_KEY_SOURCE[] = "SOURCE";
constexpr char EVENT_KEY_TEMPLATE_TYPE[] = "TEMPLATE_TYPE";
constexpr char EVENT_KEY_PNAMEID[] = "PNAMEID";
constexpr char EVENT_KEY_PVERSION[] = "PVERSION";
constexpr char EVENT_KEY_START_PACKAGE_NAME[] = "START_PACKAGE_NAME";
constexpr char EVENT_KEY_STOP_PACKAGE_NAME[] = "STOP_PACKAGE_NAME";
constexpr char EVENT_KEY_OPERATION_PACKAGE_NAME[] = "OPERATION_PACKAGE_NAME";
constexpr char EVENT_KEY_OPERATION_CODE[] = "OPERATION_CODE";
constexpr char EVENT_KEY_OPERATION_ERROR_REASON[] = "OPERATION_ERROR_REASON";
constexpr char EVENT_KEY_ACTION_EVENT[] = "ACTION_EVENT";
constexpr char EVENT_KEY_SCALE_LEVEL[] = "SCALE_LEVEL";
constexpr char EVENT_KEY_WINDOW_WIDTH[] = "WINDOW_WIDTH";
constexpr char EVENT_KEY_WINDOW_HEIGHT[] = "WINDOW_HEIGHT";

void PiPReporter::SetCurrentPackageName(const std::string &packageName)
{
    packageName_ = packageName;
}

void PiPReporter::ReportPiPStartWindow(int32_t source, int32_t templateType,
                                       int32_t isSuccess, const std::string& errorReason)
{
    WLOGI("Report start pip widow");
    if (source == 0) {
        WLOGI("need not report start pip widow");
        return;
    }
    std::string eventName = "START_PIP";
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::MULTIWINDOW_UE, eventName,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        EVENT_KEY_PNAMEID, PNAMEID,
        EVENT_KEY_PVERSION, PVERSION,
        EVENT_KEY_SOURCE, source,
        EVENT_KEY_TEMPLATE_TYPE, templateType,
        EVENT_KEY_START_PACKAGE_NAME, packageName_,
        EVENT_KEY_OPERATION_CODE, isSuccess,
        EVENT_KEY_OPERATION_ERROR_REASON, errorReason);
    if (ret != 0) {
        WLOGFE("Write HiSysEvent error, ret:%{public}d", ret);
    }
}

void PiPReporter::ReportPiPStopWindow(int32_t source, int32_t templateType,
                                      int32_t isSuccess, const std::string& errorReason)
{
    WLOGI("Report stop pip widow");
    if (source == 0) {
        WLOGI("need not report stop pip widow");
        return;
    }
    std::string eventName = "STOP_PIP";
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::MULTIWINDOW_UE, eventName,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        EVENT_KEY_PNAMEID, PNAMEID,
        EVENT_KEY_PVERSION, PVERSION,
        EVENT_KEY_SOURCE, source,
        EVENT_KEY_TEMPLATE_TYPE, templateType,
        EVENT_KEY_STOP_PACKAGE_NAME, packageName_,
        EVENT_KEY_OPERATION_CODE, isSuccess,
        EVENT_KEY_OPERATION_ERROR_REASON, errorReason);
    if (ret != 0) {
        WLOGFE("Write HiSysEvent error, ret:%{public}d", ret);
    }
}

void PiPReporter::ReportPiPActionEvent(int32_t templateType, const std::string &actionEvent)
{
    WLOGI("Report pip widow action event");
    std::string eventName = "CONTROL_PANNEL_ACTION_EVENT";
    int32_t currentAction = CONTROL_ACTION_MAP[actionEvent];
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::MULTIWINDOW_UE, eventName,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        EVENT_KEY_PNAMEID, PNAMEID,
        EVENT_KEY_PVERSION, PVERSION,
        EVENT_KEY_TEMPLATE_TYPE, templateType,
        EVENT_KEY_ACTION_EVENT, currentAction,
        EVENT_KEY_OPERATION_PACKAGE_NAME, packageName_);
    if (ret != 0) {
        WLOGFE("Write HiSysEvent error, ret:%{public}d", ret);
    }
}

void PiPReporter::ReportPiPResize(int32_t scaleLevel, int32_t windowWidth, int32_t windowHeight)
{
    WLOGI("Report pip widow resize");
    std::string eventName = "RESIZE_PIP_SIZE";
    int32_t currentScaleLevel = scaleLevel + 1;
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::MULTIWINDOW_UE, eventName,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        EVENT_KEY_PNAMEID, PNAMEID,
        EVENT_KEY_PVERSION, PVERSION,
        EVENT_KEY_SCALE_LEVEL, currentScaleLevel,
        EVENT_KEY_WINDOW_WIDTH, windowWidth,
        EVENT_KEY_WINDOW_HEIGHT, windowHeight,
        EVENT_KEY_OPERATION_PACKAGE_NAME, packageName_);
    if (ret != 0) {
        WLOGFE("Write HiSysEvent error, ret:%{public}d", ret);
    }
}

void PiPReporter::ReportPiPRatio(int32_t windowWidth, int32_t windowHeight)
{
    WLOGI("Report pip widow ratio");
    std::string eventName = "UPDATE_PIP_RATIO";
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::MULTIWINDOW_UE, eventName,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        EVENT_KEY_PNAMEID, PNAMEID,
        EVENT_KEY_PVERSION, PVERSION,
        EVENT_KEY_WINDOW_WIDTH, windowWidth,
        EVENT_KEY_WINDOW_HEIGHT, windowHeight,
        EVENT_KEY_OPERATION_PACKAGE_NAME, packageName_);
    if (ret != 0) {
        WLOGFE("Write HiSysEvent error, ret:%{public}d", ret);
    }
}

void PiPReporter::ReportPiPRestore()
{
    WLOGI("Report pip widow restore");
    std::string eventName = "RESOTRE_PIP";
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::MULTIWINDOW_UE, eventName,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        EVENT_KEY_PNAMEID, PNAMEID,
        EVENT_KEY_PVERSION, PVERSION,
        EVENT_KEY_OPERATION_PACKAGE_NAME, packageName_);
    if (ret != 0) {
        WLOGFE("Write HiSysEvent error, ret:%{public}d", ret);
    }
}

void PiPReporter::ReportPiPMove()
{
    WLOGI("Report pip widow move");
    std::string eventName = "MOVE_PIP";
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::MULTIWINDOW_UE, eventName,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        EVENT_KEY_PNAMEID, PNAMEID,
        EVENT_KEY_PVERSION, PVERSION,
        EVENT_KEY_OPERATION_PACKAGE_NAME, packageName_);
    if (ret != 0) {
        WLOGFE("Write HiSysEvent error, ret:%{public}d", ret);
    }
}
} // namespace Rosen
} // namespace OHOS
