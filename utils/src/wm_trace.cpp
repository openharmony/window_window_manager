/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "wm_trace.h"

#include <bytrace.h>

#ifndef WINDOWS_PLATFORM
#include <securec.h>
#endif

namespace OHOS {
namespace Rosen {
namespace {
const size_t MAX_STRING_SIZE = 128;
}

bool WmTraceEnabled()
{
    return true;
}

void WmTraceBegin(const char* name)
{
    if (name == nullptr) {
        return;
    }
    std::string nameStr(name);
    // TODO：need BYTRACE_TAG_WINDOW_MANAGER
    StartTrace(BYTRACE_TAG_GRAPHIC_AGP, nameStr);
}

void WmTraceEnd()
{
    if (WmTraceEnabled()) {
        // TODO：need BYTRACE_TAG_WINDOW_MANAGER
        FinishTrace(BYTRACE_TAG_GRAPHIC_AGP);
    }
}

bool WmTraceBeginWithArgv(const char* format, va_list args)
{
    char name[MAX_STRING_SIZE] = { 0 };
    if (vsnprintf_s(name, sizeof(name), sizeof(name) - 1, format, args) < 0) {
        return false;
    }
    WmTraceBegin(name);
    return true;
}

bool WmTraceBeginWithArgs(const char* format, ...)
{
    if (WmTraceEnabled()) {
        va_list args;
        va_start(args, format);
        bool retVal = WmTraceBeginWithArgv(format, args);
        va_end(args);
        return retVal;
    }
    return false;
}

WmScopedTrace::WmScopedTrace(const char* format, ...) : traceEnabled_(WmTraceEnabled())
{
    if (traceEnabled_) {
        va_list args;
        va_start(args, format);
        traceEnabled_ = WmTraceBeginWithArgv(format, args);
        va_end(args);
    }
}

WmScopedTrace::~WmScopedTrace()
{
    if (traceEnabled_) {
        WmTraceEnd();
    }
}
} // namespace Rosen
} // namespace OHOS
