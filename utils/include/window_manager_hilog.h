/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_WM_INCLUDE_WINDOW_MANAGER_HILOG_H
#define OHOS_WM_INCLUDE_WINDOW_MANAGER_HILOG_H

#include "hilog/log.h"
#include <unordered_map>
namespace OHOS {
namespace Rosen {
static constexpr unsigned int HILOG_DOMAIN_WINDOW = 0xD004200;
static constexpr unsigned int HILOG_DOMAIN_DISPLAY = 0xD004201;
static constexpr unsigned int HILOG_DOMAIN_DMS_DM = 0xD004280;
static constexpr unsigned int HILOG_DOMAIN_DMS_DM_LITE = 0xD004281;
static constexpr unsigned int HILOG_DOMAIN_DMS_DM_SERVER = 0xD004282;
static constexpr unsigned int HILOG_DOMAIN_DMS_SCREEN_KITS = 0xD004283;
static constexpr unsigned int HILOG_DOMAIN_DMS_SCREEN_CLIENT = 0xD004284;
static constexpr unsigned int HILOG_DOMAIN_DMS_SCREEN_SESSION = 0xD004285;
static constexpr unsigned int HILOG_DOMAIN_DMS_SCREEN_SESSION_MANAGER = 0xD004286;
static constexpr unsigned int HILOG_DOMAIN_DMS_DISPLAY_RUNTIME = 0xD004287;
static constexpr unsigned int HILOG_DOMAIN_DMS_SCREEN_RUNTIME = 0xD004288;
static constexpr unsigned int HILOG_DOMAIN_DMS_SCREEN_SHOT = 0xD004289;

static constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowManager"};

#define WLOG_F(...)  (void)OHOS::HiviewDFX::HiLog::Fatal(LOG_LABEL, __VA_ARGS__)
#define WLOG_E(...)  (void)OHOS::HiviewDFX::HiLog::Error(LOG_LABEL, __VA_ARGS__)
#define WLOG_W(...)  (void)OHOS::HiviewDFX::HiLog::Warn(LOG_LABEL, __VA_ARGS__)
#define WLOG_I(...)  (void)OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, __VA_ARGS__)
#define WLOG_D(...)  (void)OHOS::HiviewDFX::HiLog::Debug(LOG_LABEL, __VA_ARGS__)

#define _W_DFUNC HiviewDFX::HiLog::Debug
#define _W_IFUNC HiviewDFX::HiLog::Info
#define _W_WFUNC HiviewDFX::HiLog::Warn
#define _W_EFUNC HiviewDFX::HiLog::Error

#define _W_CPRINTF(func, fmt, ...) func(LABEL, "<%{public}d>" fmt, __LINE__, ##__VA_ARGS__)

#define WLOGD(fmt, ...) _W_CPRINTF(_W_DFUNC, fmt, ##__VA_ARGS__)
#define WLOGI(fmt, ...) _W_CPRINTF(_W_IFUNC, fmt, ##__VA_ARGS__)
#define WLOGW(fmt, ...) _W_CPRINTF(_W_WFUNC, fmt, ##__VA_ARGS__)
#define WLOGE(fmt, ...) _W_CPRINTF(_W_EFUNC, fmt, ##__VA_ARGS__)

#define _W_FUNC __func__

#define WLOGFD(fmt, ...) WLOGD("%{public}s: " fmt, _W_FUNC, ##__VA_ARGS__)
#define WLOGFI(fmt, ...) WLOGI("%{public}s: " fmt, _W_FUNC, ##__VA_ARGS__)
#define WLOGFW(fmt, ...) WLOGW("%{public}s: " fmt, _W_FUNC, ##__VA_ARGS__)
#define WLOGFE(fmt, ...) WLOGE("%{public}s: " fmt, _W_FUNC, ##__VA_ARGS__)

enum class WmsLogTag : uint8_t {
    DEFAULT = 0,               // C04200
    DMS,                       // C04201
    WMS_MAIN,                  // C04202
    WMS_SUB,                   // C04203
    WMS_SCB,                   // C04204
    WMS_DIALOG,                // C04205
    WMS_SYSTEM,                // C04206
    WMS_FOCUS,                 // C04207
    WMS_LAYOUT,                // C04208
    WMS_IMMS,                  // C04209
    WMS_LIFE,                  // C0420A
    WMS_INPUT,                 // C0420B
    WMS_EVENT,                 // C0420C

    END = 256,                 // Last one, do not use
};

const std::unordered_map<WmsLogTag, const char *> DOMAIN_CONTENTS_MAP = {
    { WmsLogTag::DEFAULT, "WMS" },
    { WmsLogTag::DMS, "DMS" },
    { WmsLogTag::WMS_MAIN, "WMSMain" },
    { WmsLogTag::WMS_SUB, "WMSSub" },
    { WmsLogTag::WMS_SCB, "WMSScb" },
    { WmsLogTag::WMS_DIALOG, "WMSDialog" },
    { WmsLogTag::WMS_SYSTEM, "WMSSystem" },
    { WmsLogTag::WMS_FOCUS, "WMSFocus" },
    { WmsLogTag::WMS_LAYOUT, "WMSLayout" },
    { WmsLogTag::WMS_IMMS, "WMSImms" },
    { WmsLogTag::WMS_LIFE, "WMSLife" },
    { WmsLogTag::WMS_INPUT, "WMSInput" },
    { WmsLogTag::WMS_EVENT, "WMSEvent" },
};

#define WMS_FILE_NAME (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#define FMT_PREFIX "[%{public}s] %{public}s<%{public}d>: "

#define PRINT_TLOG(level, tag, ...)                                                                     \
    do {                                                                                                \
        uint32_t hilogDomain = HILOG_DOMAIN_WINDOW + static_cast<uint32_t>(tag);                        \
        const char *domainContent = DOMAIN_CONTENTS_MAP.count(tag) ? DOMAIN_CONTENTS_MAP.at(tag) : "";  \
        HILOG_IMPL(LOG_CORE, level, hilogDomain, domainContent, ##__VA_ARGS__);                         \
    } while (0)

#define TLOGD(tag, fmt, ...) PRINT_TLOG(LOG_DEBUG, tag, FMT_PREFIX fmt, WMS_FILE_NAME, _W_FUNC, __LINE__, ##__VA_ARGS__)
#define TLOGI(tag, fmt, ...) PRINT_TLOG(LOG_INFO, tag, FMT_PREFIX fmt, WMS_FILE_NAME, _W_FUNC, __LINE__, ##__VA_ARGS__)
#define TLOGW(tag, fmt, ...) PRINT_TLOG(LOG_WARN, tag, FMT_PREFIX fmt, WMS_FILE_NAME, _W_FUNC, __LINE__, ##__VA_ARGS__)
#define TLOGE(tag, fmt, ...) PRINT_TLOG(LOG_ERROR, tag, FMT_PREFIX fmt, WMS_FILE_NAME, _W_FUNC, __LINE__, ##__VA_ARGS__)

} // namespace OHOS
}
#endif // FRAMEWORKS_WM_INCLUDE_WINDOW_MANAGER_HILOG_H
