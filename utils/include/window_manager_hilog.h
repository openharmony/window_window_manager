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
} // namespace OHOS
}
#endif // FRAMEWORKS_WM_INCLUDE_WINDOW_MANAGER_HILOG_H
