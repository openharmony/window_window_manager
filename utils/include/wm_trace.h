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

#ifndef OHOS_WM_INCLUDE_WM_TRACE_H
#define OHOS_WM_INCLUDE_WM_TRACE_H

#include <cstdarg>
#include <cstdio>

#include "noncopyable.h"

#define WM_SCOPED_TRACE(fmt, ...) WmScopedTrace wmScopedTrace(fmt, ##__VA_ARGS__)
#ifdef WM_DEBUG
#define WM_DEBUG_SCOPED_TRACE(fmt, ...) WmScopedTrace wmScopedTrace(fmt, ##__VA_ARGS__)
#else
#define WM_DEBUG_SCOPED_TRACE(fmt, ...)
#endif

#define WM_FUNCTION_TRACE() WM_SCOPED_TRACE(__func__)
#define WM_SCOPED_TRACE_BEGIN(fmt, ...) WmTraceBeginWithArgs(fmt,  ##__VA_ARGS__)
#define WM_SCOPED_TRACE_END() WmTraceEnd()

namespace OHOS {
namespace Rosen {
bool WmTraceEnabled();
void WmTraceBegin(const char* name);
bool WmTraceBeginWithArgs(const char* format, ...) __attribute__((__format__(printf, 1, 2)));
bool WmTraceBeginWithArgv(const char* format, va_list args);
void WmTraceEnd();

class WmScopedTrace final {
public:
    explicit WmScopedTrace(const char* format, ...) __attribute__((__format__(printf, 2, 3)));
    ~WmScopedTrace();

    WM_DISALLOW_COPY_AND_MOVE(WmScopedTrace);

private:
    bool traceEnabled_ { false };
};
} // namespace OHOS
}
#endif // OHOS_WM_INCLUDE_WM_TRACE_H