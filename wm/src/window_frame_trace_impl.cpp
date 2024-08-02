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

#include "window_frame_trace.h"
#include <unistd.h>
#ifdef FRAME_TRACE_ENABLE
#include "frame_trace.h"
#endif

namespace FRAME_TRACE {

WindowFrameTraceImpl* WindowFrameTraceImpl::GetInstance()
{
    static WindowFrameTraceImpl ftWindow;
    return &ftWindow;
}
#ifdef FRAME_TRACE_ENABLE
bool WindowFrameTraceImpl::AccessFrameTrace()
{
    return true;
}

void WindowFrameTraceImpl::VsyncStartFrameTrace()
{
    if (!AccessFrameTrace()) {
        return;
    }
    if (FrameAwareTraceEnable(intervalName_)) {
        if (handleUI_ == nullptr) {
            handleUI_ = CreateTraceTag(intervalName_);
        }
        if (handleUI_ != nullptr) {
            EnableTraceForThread(handleUI_);
            StartFrameTrace(handleUI_);
        }
    }
}
           
void WindowFrameTraceImpl::VsyncStopFrameTrace()
{
    if (!AccessFrameTrace()) {
        return;
    }
    if (FrameAwareTraceEnable(intervalName_)) {
        if (handleUI_ != nullptr) {
            StopFrameTrace(handleUI_);
        }
    }
}
#endif
} // namespace FRAME_TRACE
