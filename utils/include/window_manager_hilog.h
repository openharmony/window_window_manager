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
#include <cinttypes>
#include <functional>
#include <future>
#include <string>
#include <sstream>

#include "hilog/log.h"
namespace OHOS {
namespace Rosen {
static constexpr unsigned int HILOG_DOMAIN_WINDOW = 0xD004200;
static constexpr unsigned int HILOG_DOMAIN_DISPLAY = 0xD004201;
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

class InnerFunctionTracer {
public:
    using HilogFunc = std::function<int(const char *)>;

public:
    InnerFunctionTracer(HilogFunc logfn, const char* tag, LogLevel level)
        : logfn_ { logfn }, tag_ { tag }, level_ { level }
    {
        if (HiLogIsLoggable(OHOS::Rosen::HILOG_DOMAIN_WINDOW, tag_, level_)) {
            if (logfn_ != nullptr) {
                logfn_("in %{public}s, enter");
            }
        }
    }
    ~InnerFunctionTracer()
    {
        if (HiLogIsLoggable(OHOS::Rosen::HILOG_DOMAIN_WINDOW, tag_, level_)) {
            if (logfn_ != nullptr) {
                logfn_("in %{public}s, leave");
            }
        }
    }
private:
    HilogFunc logfn_ { nullptr };
    const char* tag_ { nullptr };
    LogLevel level_ { LOG_LEVEL_MIN };
};

#define CALL_DEBUG_ENTER        OHOS::Rosen::InnerFunctionTracer ___innerFuncTracer_Debug___    \
    { std::bind(&OHOS::HiviewDFX::HiLog::Debug, LABEL, std::placeholders::_1, __FUNCTION__), LABEL.tag, LOG_DEBUG }

#define CALL_INFO_TRACE         OHOS::Rosen::InnerFunctionTracer ___innerFuncTracer_Info___     \
    { std::bind(&OHOS::HiviewDFX::HiLog::Info, LABEL, std::placeholders::_1, __FUNCTION__), LABEL.tag, LOG_INFO }

#define CALL_TEST_DEBUG         OHOS::Rosen::InnerFunctionTracer ___innerFuncTracer_Info___     \
    { std::bind(&OHOS::HiviewDFX::HiLog::Info, LABEL, std::placeholders::_1,     \
    (test_info_ == nullptr ? "TestBody" : test_info_->name())), LABEL.tag, LOG_DEBUG }

} // namespace Rosen
}
#endif // FRAMEWORKS_WM_INCLUDE_WINDOW_MANAGER_HILOG_H