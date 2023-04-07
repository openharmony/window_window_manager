/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_RESSCHED_REPORT_H
#define OHOS_ROSEN_RESSCHED_REPORT_H

#ifdef RESOURCE_SCHEDULE_SERVICE_ENABLE
#include "event_handler.h"
#include "res_sched_client.h"
#endif
#include "window_helper.h"
#include "wm_common.h"
#include "wm_single_instance.h"

namespace OHOS {
namespace Rosen {
#ifdef RESOURCE_SCHEDULE_SERVICE_ENABLE
namespace {
    constexpr int64_t PERF_TIME_OUT = 200;
    constexpr int64_t SLIDE_PERF_TIME_OUT = 1000;
    constexpr int32_t PERF_CLICK_NORMAL_CODE = 9;
    constexpr int32_t PERF_DRAG_CODE = 31;
    constexpr int32_t PERF_MOVE_CODE = 32;
    constexpr int32_t PERF_SLIDE_CODE = 11;
    constexpr int32_t PERF_STATUS_BAR_DRAG_CODE = 37;
    const std::string TASK_NAME = "SlideOff";
}

/**
 * @brief Slide event status
 */
enum SlideEventStatus : int64_t {
    SLIDE_EVENT_OFF = 0,
    SLIDE_EVENT_ON = 1,
    SLIDE_NORMAL_BEGIN = 3,
    SLIDE_NORMAL_END = 4,
};
#endif

class ResSchedReport {
    WM_DECLARE_SINGLE_INSTANCE(ResSchedReport);
    public:
    void StopPerfIfNeed()
    {
#ifdef RESOURCE_SCHEDULE_SERVICE_ENABLE
        if (windowDragBoost_) {
            ClosePerf(PERF_DRAG_CODE);
            windowDragBoost_ = false;
        }
        if (windowMovingBoost_) {
            ClosePerf(PERF_MOVE_CODE);
            windowMovingBoost_ = false;
        }
#endif
    }

    void TrigClick()
    {
#ifdef RESOURCE_SCHEDULE_SERVICE_ENABLE
        std::unordered_map<std::string, std::string> mapPayload;
        // 2 means click event.
        OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(PERF_CLICK_NORMAL_CODE, 2, mapPayload);
#endif
    }

    void RequestPerfIfNeed(WindowSizeChangeReason reason, WindowType type, WindowMode mode)
    {
#ifdef RESOURCE_SCHEDULE_SERVICE_ENABLE
        if (WindowHelper::IsMainFloatingWindow(type, mode) || WindowHelper::IsSplitWindowMode(mode)) {
            switch (reason) {
                case WindowSizeChangeReason::DRAG_END: {
                    if (windowDragBoost_) {
                        ClosePerf(PERF_DRAG_CODE);
                        windowDragBoost_ = false;
                    }
                    break;
                }
                case WindowSizeChangeReason::DRAG_START:
                    [[fallthrough]];
                case WindowSizeChangeReason::DRAG: {
                    RequestPerf(PERF_DRAG_CODE, PERF_TIME_OUT);
                    windowDragBoost_ = true;
                    break;
                }
                case WindowSizeChangeReason::MOVE: {
                    RequestPerf(PERF_MOVE_CODE, PERF_TIME_OUT);
                    windowMovingBoost_ = true;
                    break;
                }
                default: {
                    // doing nothing
                }
            }
        }
#endif
    }

    void TrigSlide(WindowType type, bool isOn)
    {
#ifdef RESOURCE_SCHEDULE_SERVICE_ENABLE
        if (type == WindowType::WINDOW_TYPE_STATUS_BAR) {
            StatusBarDrag(isOn);
        }
        if (type == WindowType::WINDOW_TYPE_DESKTOP) {
            Slide(isOn);
        }
#endif
    }
private:
#ifdef RESOURCE_SCHEDULE_SERVICE_ENABLE
    void StatusBarDrag(bool isOn)
    {
        static auto lastRequestPerfTime = std::chrono::steady_clock::now();
        static auto eventRunner = AppExecFwk::EventRunner::GetMainEventRunner();
        static auto handler = std::make_shared<AppExecFwk::EventHandler>(eventRunner);
        static auto task = []() {
            std::unordered_map<std::string, std::string> mapPayload;
            OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(PERF_STATUS_BAR_DRAG_CODE,
                1, mapPayload);
        };
        auto current = std::chrono::steady_clock::now();
        bool isTimeOut = std::chrono::duration_cast<std::chrono::milliseconds>(current - lastRequestPerfTime).
            count() > SLIDE_PERF_TIME_OUT;
        if (isTimeOut && isOn) {
            handler->RemoveTask(TASK_NAME);
            lastRequestPerfTime = current;
            std::unordered_map<std::string, std::string> mapPayload;
            OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(PERF_STATUS_BAR_DRAG_CODE,
                0, mapPayload);
        }
        if (!isOn) {
            std::unordered_map<std::string, std::string> mapPayload;
            OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(PERF_STATUS_BAR_DRAG_CODE,
                0, mapPayload);
            // 990 is the animation duration.
            handler->PostTask(task, TASK_NAME, 990, AppExecFwk::EventQueue::Priority::HIGH);
        }
    }

    void Slide(bool isOn)
    {
        static auto lastRequestPerfTime = std::chrono::steady_clock::now();
        auto current = std::chrono::steady_clock::now();
        bool isTimeOut = std::chrono::duration_cast<std::chrono::milliseconds>(current - lastRequestPerfTime).
            count() > SLIDE_PERF_TIME_OUT;
        if (isTimeOut && isOn) {
            lastRequestPerfTime = current;
            std::unordered_map<std::string, std::string> mapPayload;
            OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(PERF_SLIDE_CODE,
                SlideEventStatus::SLIDE_EVENT_ON, mapPayload);
        }
        if (!isOn) {
            std::unordered_map<std::string, std::string> mapPayload;
            OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(PERF_SLIDE_CODE,
                SlideEventStatus::SLIDE_EVENT_OFF, mapPayload);
        }
    }

    void RequestPerf(int32_t code, int64_t timeOut)
    {
        auto currentTime = std::chrono::steady_clock::now();
        bool isTimeOut = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastRequestPerfTime_).
            count() > timeOut;
        if (isTimeOut) {
            std::unordered_map<std::string, std::string> mapPayload;
            // 0 means doing action.
            OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(code, 0, mapPayload);
            lastRequestPerfTime_ = currentTime;
        }
    }

    void ClosePerf(int32_t code)
    {
        std::unordered_map<std::string, std::string> mapPayload;
        // 1 means stop action.
        OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(code, 1, mapPayload);
    }

    std::chrono::steady_clock::time_point lastRequestPerfTime_ = std::chrono::steady_clock::now();
    bool windowMovingBoost_ = false;
    bool windowDragBoost_ = false;
#endif
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_RESSCHED_REPORT_H
